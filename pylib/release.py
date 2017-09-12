#
# eChronos Real-Time Operating System
# Copyright (c) 2017, Commonwealth Scientific and Industrial Research
# Organisation (CSIRO) ABN 41 687 119 230.
#
# All rights reserved. CSIRO is willing to grant you a licence to the eChronos
# real-time operating system under the terms of the CSIRO_BSD_MIT license. See
# the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
#
# @TAG(CSIRO_BSD_MIT)
#

import io
import os
import shutil
import logging
import inspect
import tarfile
import subprocess
import functools
from glob import glob
from contextlib import contextmanager
from .utils import chdir, tempdir, BASE_TIME, top_path, base_to_top_paths, find_path, Git
from .utils import walk
from .cmdline import subcmd, Arg
from .docs import is_release_doc_file, is_nonrelease_doc_file


class _ReleaseMeta(type):
    """A pretty-printing meta-class for the Release class."""
    def __str__(cls):
        return '{}-{}'.format(cls.release_name, cls.version)


class Release(metaclass=_ReleaseMeta):
    """The Release base class is used by the release configuration."""
    packages = []
    platforms = []
    version = None
    product_name = None
    release_name = None
    enabled = False
    license = None
    top_level_license = None
    extra_files = []


class Package:
    """Represents a customer visible package.

    This is currently used mainly for release management.

    """
    @staticmethod
    def create_from_disk(topdir):
        """Return a dictionary that contains a Package instance for each package on disk in a 'package' directory.

        The dictionary keys are the package names.
        The dictionary values are the package instances.

        """
        pkgs = {}
        for pkg_parent_dir in base_to_top_paths(topdir, 'packages'):
            pkg_names = os.listdir(pkg_parent_dir)
            for pkg_name in pkg_names:
                pkg_path = os.path.join(pkg_parent_dir, pkg_name)
                if os.path.isdir(pkg_path):
                    if pkg_name in pkgs:
                        logging.warning('Overriding package %s with package %s', pkgs[pkg_name].path, pkg_path)
                    pkgs[pkg_name] = Package(pkg_path)
        return pkgs

    def __init__(self, path):
        assert os.path.isdir(path), 'Path {} is not a directory'.format(path)
        self.path = path
        self.name = os.path.basename(self.path)


class _ReleasePackage:
    """Represents a Package instance that is refined for a specific release configuration.

    Configuring a Package instance for release results in additional properties of a package, relevant when creating
    release files.

    """
    def __init__(self, package, release_configuration):
        self._pkg = package
        self._rls_cfg = release_configuration

    def get_name(self):
        return self._pkg.name

    def get_path(self):
        return self._pkg.path

    def get_files(self):
        def flt(file_path):
            return '__pycache__' in file_path.lower()
        return walk(self.get_path(), flt)

    def get_archive_name(self):
        return '{}-{}'.format(self._pkg.name, self._rls_cfg.release_name)

    def get_path_in_archive(self):
        return 'share/packages/{}'.format(self._pkg.name)

    def get_license(self):
        return self._rls_cfg.license

    def get_doc_license(self):
        if hasattr(self._rls_cfg, 'doc_license'):
            return self._rls_cfg.doc_license
        return self._rls_cfg.license


@contextmanager
def _tarfile_open(name, mode, **kwargs):
    assert mode.startswith('w')
    with tarfile.open(name, mode, **kwargs) as file_obj:
        try:
            yield file_obj
        except:
            os.unlink(name)
            raise


class _FileWithLicense:
    """_FileWithLicense provides a read-only file-like object that automatically includes license text when reading
    from the underlying file object.

    The _FileWithLicense object takes ownership of the underlying file object.
    The original file object should not be used after passing it to the _FileWithLicense object.

    """
    def __init__(self, file_obj, lic, old_xml_prologue_len, old_license_len):
        self._file_obj = file_obj
        self._read_license = True

        if old_xml_prologue_len:
            file_obj.read(old_xml_prologue_len)

        if old_license_len:
            file_obj.read(old_license_len)

        if lic:
            self._read_license = False
            self._license_io = io.BytesIO(lic)

    def read(self, size):
        assert size > 0
        data = b''
        if not self._read_license:
            data = self._license_io.read(size)
            if len(data) < size:
                self._read_license = True
                size -= len(data)

        if self._read_license:
            data += self._file_obj.read(size)

        return data

    def close(self):
        self._file_obj.close()

    def __enter__(self):
        return self

    def __exit__(self, typ, value, traceback):
        self.close()


class _LicenseOpener:
    """The license opener provides a single 'open' method, that can be used instead of the built-in 'open' function.

    This open will return a file-like object that modifies the underlying file to include an appropriate license
    header.

    The 'license' is passed to the object during construction.

    """
    LICENSE_TAG = '@TAG(CSIRO_BSD_MIT)'
    LICENSE_DOC_TAG = '@TAG(CSIRO_BSD_MIT)'
    BUILD_ARTIFACT_FILETYPES = ['.pyc']
    LICENSE_EXEMPTED_FILETYPES = ['.pdf', '.svg', '.png', '.txt', '.gdbout']

    class UnknownFiletypeException(Exception):
        """Raised when the given file type is unknown."""

    # pylint: disable=too-many-arguments
    def __init__(self, lic, doc_license, top_dir, allow_unknown_filetypes=False, filename=None):
        self.license = lic
        self.doc_license = doc_license
        self.top_dir = top_dir
        self.allow_unknown_filetypes = allow_unknown_filetypes
        self.filename = filename
        self.xml_prologue = '<?xml version="1.0" encoding="UTF-8" ?>'

    def _consume_xml_prologue(self, file_obj):
        xml_prologue_len = len(self.xml_prologue)

        file_header = file_obj.read(xml_prologue_len)
        if file_header != self.xml_prologue.encode('utf8'):
            raise Exception("XML File: '{}' does not contain expected prologue: {} expected {}".
                            format(file_obj.name, file_header, self.xml_prologue.encode('utf8')))

        # Files in repositories are guaranteed to have LF, but generated code may have OS-specific line endings
        if file_obj.peek(1).startswith(b'\n'):
            file_obj.read(1)
            xml_prologue_len += 1
        else:
            line_sep = file_obj.read(len(os.linesep))
            if line_sep == os.linesep.encode('utf8'):
                xml_prologue_len += len(os.linesep)
            else:
                raise Exception("XML File: '{}' prologue does not end with a valid line separator: {}".
                                format(file_obj.name, line_sep))

        return xml_prologue_len

    @staticmethod
    def license_sentinel(ext):
        result = None
        if ext in ['.c', '.h', '.ld', '.s']:
            result = _LicenseOpener.LICENSE_TAG + '\n */\n'
        elif ext in ['.py', '.gdb', '.sh', '.yml']:
            result = _LicenseOpener.LICENSE_TAG + '\n#\n'
        elif ext in ['.prx', '.xml', '.prj']:
            result = _LicenseOpener.LICENSE_TAG + '\n-->\n'
        elif ext in ['.asm']:
            result = _LicenseOpener.LICENSE_TAG + '\n;\n'
        elif ext in ['.md', '.markdown', '.html']:
            result = _LicenseOpener.LICENSE_DOC_TAG + '\n-->\n'
        elif ext in ['.css']:
            return _LicenseOpener.LICENSE_DOC_TAG + '\n */\n'
        elif ext in ['.bat']:
            return _LicenseOpener.LICENSE_TAG + '\r\nREM\r\n'
        elif ext in _LicenseOpener.LICENSE_EXEMPTED_FILETYPES or ext in _LicenseOpener.BUILD_ARTIFACT_FILETYPES:
            result = None
        else:
            raise _LicenseOpener.UnknownFiletypeException('Unexpected ext: {}'.format(ext))

        return result

    @staticmethod
    def _format_lic(lic, start, perline, emptyline, end):
        return start + os.linesep + \
            os.linesep.join([perline + line if line else emptyline for line in lic.splitlines()]) + \
            os.linesep + end + os.linesep

    def _get_lic(self, filename):
        lic = None
        old_xml_prologue_len = 0
        old_license_len = 0
        ext = os.path.splitext(filename)[1]
        is_xml = False

        if ext in ['.c', '.h', '.ld', '.s', '.css']:
            lic = self._format_lic(self.license, '/*', ' * ', ' *', ' */')
        elif ext in ['.py', '.gdb', '.yml']:
            lic = self._format_lic(self.license, '#', '# ', '#', '#')
        elif ext in ['.prx', '.xml', '.prj']:
            lic = self._format_lic(self.license, '<!--', '', '', '-->')
            is_xml = True
        elif ext in ['.asm']:
            lic = self._format_lic(self.license, ';', '; ', ';', ';')
        elif ext in ['.md', '.markdown']:
            lic = self._format_lic(self.doc_license, '<!--', '', '', '-->')
        elif ext in ['.html']:
            lic = self._format_lic(self.doc_license, '<!--', '', '', '-->')
        elif ext in ['.bat']:
            lic = self._format_lic(self.doc_license, 'REM', 'REM ', 'REM', 'REM')
        elif ext not in self.LICENSE_EXEMPTED_FILETYPES and not self.allow_unknown_filetypes:
            raise Exception('Unexpected ext: {}, for file {}'.format(ext, filename))

        if lic is None:
            lic = ''
        else:
            with open(filename, 'rb') as file_obj:
                # Count the length of the XML prologue in the input file and standardize its line ending for output
                if is_xml:
                    old_xml_prologue_len = self._consume_xml_prologue(file_obj)
                    lic = self.xml_prologue + os.linesep + lic

                # If the LICENSE license is present in the original source file, count its length for deletion
                license_sentinel = self.license_sentinel(ext)
                assert license_sentinel is not None
                old_lic_str, sentinel_found, _ = file_obj.peek().decode('utf8').partition(license_sentinel)
                if sentinel_found:
                    old_license_len = len(old_lic_str + sentinel_found)

        lic = lic.encode('utf8')

        return lic, old_xml_prologue_len, old_license_len

    def open(self, filename, mode):
        assert mode == 'rb'

        file_obj = open(filename, mode)
        lic, old_xml_prologue_len, old_license_len = self._get_lic(filename)
        return _FileWithLicense(file_obj, lic, old_xml_prologue_len, old_license_len)

    def tar_info_filter(self, tarinfo):
        # exclude all documentation files except the final PDF
        if is_nonrelease_doc_file(tarinfo.name):
            return None

        if tarinfo.isreg():
            if self.filename is not None:
                # This is used for releasing extra files potentially from outside the 'packages' dir of the repo.
                # A LicenseOpener object must be instantiated per-file to specify the filename in this manner.
                filename = self.filename
            else:
                # Infer the location of the original file in the 'packages' directory, from its destination path under
                # 'share/packages' in the release archive.
                assert tarinfo.name.startswith('share/packages')
                filename = find_path(tarinfo.name.replace('share/packages', 'packages', 1), self.top_dir)

            lic, old_xml_prologue_len, old_license_len = self._get_lic(filename)

            # lic includes the XML prologue string, if there is one.
            tarinfo.size += len(lic)

            if old_xml_prologue_len:
                tarinfo.size -= old_xml_prologue_len

            if old_license_len:
                tarinfo.size -= old_license_len

        return _tar_info_filter(tarinfo)


def _tar_info_filter(tarinfo, execute_permission=False):
    tarinfo.uname = '_default_user_'
    tarinfo.gname = '_default_group_'
    tarinfo.mtime = BASE_TIME
    tarinfo.uid = 1000
    tarinfo.gid = 1000
    # Directories automatically have the execute permission bit set which needs to be preserved.
    # However, the default permission are to permissive for the group and other users, which needs to be reset.
    if tarinfo.mode & 0o100 or execute_permission:
        tarinfo.mode = 0o700
    else:
        tarinfo.mode = 0o600
    return tarinfo


def _tar_add_data(tarfile_obj, arcname, data, ti_filter=None):
    """Directly add data to a tarfile.

    tarfile_obj is a tarfile.TarFile object.
    arcname is the name the data will have in the archive.
    data is the raw data (which should be of type 'bytes').
    fi_filter filters the created TarInfo object. (In a similar manner to the tarfile.TarFile.add() method.

    """
    tar_info = tarfile.TarInfo(arcname)
    tar_info.size = len(data)
    if ti_filter:
        tar_info = ti_filter(tar_info)
    tarfile_obj.addfile(tar_info, io.BytesIO(data))


# pylint: disable=too-many-arguments
def _tar_gz_with_license(output, dir_path, file_paths, prefix, lic, doc_license, allow_unknown_filetypes):

    """Create a tar.gz file named `output` from a list of file paths relative to a directory path.

    Any appropriate files have the specified license attached.

    When creating the tar.gz a standard set of meta-data will be used to help ensure things are consistent.

    """
    opener = _LicenseOpener(lic, doc_license, os.getcwd(), allow_unknown_filetypes)
    try:
        with tarfile.open(output, 'w:gz', format=tarfile.GNU_FORMAT) as tarfile_obj:
            tarfile.bltn_open = opener.open
            with chdir(dir_path):
                for file_path in file_paths:
                    if os.path.isabs(file_path):
                        file_path = os.path.relpath(file_path, dir_path)
                    tarfile_obj.add(file_path, _arc_path_join(prefix, file_path), filter=opener.tar_info_filter)
    finally:
        tarfile.bltn_open = open


def _mk_partial(pkg, topdir, allow_unknown_filetypes):
    file_path = top_path(topdir, 'release', 'partials', '{}.tar.gz'.format(pkg.get_archive_name()))
    src_prefix = 'share/packages/{}'.format(pkg.get_name())
    _tar_gz_with_license(file_path, pkg.get_path(), pkg.get_files(), src_prefix, pkg.get_license(),
                         pkg.get_doc_license(), allow_unknown_filetypes)


@subcmd(name='partials', cmd='build', help='Build partial release files',
        args=(Arg('--allow-unknown-filetypes', action='store_true'),))
def build_partials(args):
    os.makedirs(top_path(args.topdir, 'release', 'partials'), exist_ok=True)
    packages = Package.create_from_disk(args.topdir).values()
    for pkg in packages:
        for config in get_release_configs():
            release_package = _ReleasePackage(pkg, config)
            _mk_partial(release_package, args.topdir, args.allow_unknown_filetypes)
    return 0


# pylint: disable=too-many-locals
def build_single_release(config, topdir):
    """Build a release archive for a specific release configuration."""
    # for an unknown reason, tarfile.bltn_open is not reliably reset to the open() function in the extra files loop
    tarfile.bltn_open = open
    basename = '{}-{}-{}'.format(config.product_name, config.release_name, config.version)
    logging.info("Building %s", basename)
    tarfilename = top_path(topdir, 'release', '{}.tar.gz'.format(basename))
    with _tarfile_open(tarfilename, 'w:gz', format=tarfile.GNU_FORMAT) as tarfile_obj:
        for pkg in config.packages:
            release_file_path = top_path(topdir, 'release', 'partials', '{}-{}.tar.gz')
            with tarfile.open(release_file_path.format(pkg, config.release_name), 'r:gz') as in_f:
                for member in in_f.getmembers():
                    m_f = in_f.extractfile(member)
                    member.name = basename + '/' + member.name
                    if is_release_doc_file(member.name):
                        variant = os.path.basename(os.path.dirname(member.name)).replace('rtos-', '')
                        member.name = '{}/{}-{}-{}-{}.pdf'.format(basename, config.product_name, config.release_name,
                                                                  variant, config.version)
                    tarfile_obj.addfile(member, m_f)

        prj_build_dir = 'prj_build'
        for file_name in os.listdir(prj_build_dir):
            # mark all files except the zipped prj 'binary' as executable because prj cannot be executed itself
            file_filter = functools.partial(_tar_info_filter, execute_permission=not file_name.endswith('prj'))
            arcname = _arc_path_join(basename, 'bin', file_name)
            tarfile_obj.add(os.path.join(prj_build_dir, file_name), arcname=arcname, filter=file_filter)
        for prj_release_files_path in base_to_top_paths(topdir, os.path.join('prj', 'release_files')):
            for file_name in os.listdir(prj_release_files_path):
                if file_name != 'README.md':
                    file_path = os.path.join(prj_release_files_path, file_name)
                    archive_path = _arc_path_join(basename, file_name)
                    tarfile_obj.add(file_path, arcname=archive_path, filter=_tar_info_filter)

        if config.top_level_license is not None:
            _tar_add_data(tarfile_obj, _arc_path_join(basename, 'LICENSE'),
                          config.top_level_license.encode('utf8'),
                          _tar_info_filter)

        # Run license replacer on all extra files released
        dummy_pkg = _ReleasePackage(None, config)
        for arcname, filename in config.extra_files:
            file_path = find_path(filename, topdir)
            opener = _LicenseOpener(dummy_pkg.get_license(), dummy_pkg.get_doc_license(), topdir, filename=file_path)
            tarfile.bltn_open = opener.open
            tarfile_obj.add(file_path, arcname=_arc_path_join(basename, arcname), filter=opener.tar_info_filter)
            tarfile.bltn_open = open

        if 'TEAMCITY_VERSION' in os.environ:
            build_info = os.environ['BUILD_VCS_NUMBER']
        else:
            git = Git()
            build_info = git.branch_hash()
            if not git.working_dir_clean():
                build_info += "-unclean"
        build_info += '\n'
        _tar_add_data(tarfile_obj, _arc_path_join(basename, 'build_info'), build_info.encode('utf8'),
                      _tar_info_filter)

        _tar_add_data(tarfile_obj, _arc_path_join(basename, 'version_info'), config.version.encode('utf8'),
                      _tar_info_filter)


# pylint: disable=too-many-locals
# pylint: disable=too-many-branches
def release_test_one(archive):
    """Test a single archive

    This is primarily a sanity check of the actual release file. Release
    files are only made after other testing has been successfully performed.

    Currently it simply does some sanitfy checking on the tar file to ensure it is consistent.

    In the future additional testing will be performed.

    """
    project_prj_template = """<?xml version="1.0" encoding="UTF-8" ?>
<project>
<search-path>{}</search-path>
</project>
"""

    rel_file = os.path.abspath(archive)
    with tarfile.open(rel_file, 'r:gz') as tarfile_obj:
        for member in tarfile_obj.getmembers():
            if member.gid != 1000:
                raise Exception("member.gid != 1000 {} -- {}".format(member.gid, member.name))
            if member.uid != 1000:
                raise Exception("member.uid != 1000 {} -- {}".format(member.uid, member.name))
            if member.mtime != BASE_TIME:
                raise Exception("member.gid != BASE_TIME({}) {} -- {}".format(member.mtime, BASE_TIME, member.name))

    with tempdir() as temp_dir_path:
        with chdir(temp_dir_path):
            assert shutil.which('tar')
            subprocess.check_call("tar xf {}".format(rel_file).split())
            release_dir = os.path.splitext(os.path.splitext(os.path.basename(archive))[0])[0]
            if not os.path.isdir(release_dir):
                raise RuntimeError("Release archive does not extract into top directory with the same name as the "
                                   "base name of the archive ({})".format(release_dir))
            with chdir(release_dir):
                prj_path = os.path.join("bin", "prj.sh")
                cmd = prj_path
                try:
                    subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True)
                except subprocess.CalledProcessError as exc:
                    if exc.returncode != 1:
                        raise
                pkgs = []
                pkg_root = './share/packages/'
                for root, _, files in os.walk(pkg_root):
                    for file_path in files:
                        if file_path.endswith('.prx'):
                            pkg = os.path.join(root, file_path)[len(pkg_root):-4].replace(os.sep, '.')
                            pkgs.append(pkg)
                with open('project.prj', 'w') as file_obj:
                    file_obj.write(project_prj_template.format(pkg_root))
                for pkg in pkgs:
                    cmd = "{} build {}".format(prj_path, pkg)
                    try:
                        subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True)
                    except subprocess.CalledProcessError as exc:
                        err_str = None
                        for line in exc.output.splitlines():
                            line = line.decode()
                            if line.startswith('ERROR'):
                                err_str = line
                                break
                        if err_str is None:
                            print(exc.output)
                            raise
                        elif 'missing or contains multiple Builder modules' in err_str:
                            pass
                        else:
                            print("Unexpected error:", err_str)
                            raise


@subcmd(name='release', cmd='test')
def test(args):
    """Implement the test-release command.

    This command is used to perform sanity checks and testing of the full release.

    """
    for rel in glob(top_path(args.topdir, 'release', '*.tar.gz')):
        release_test_one(rel)
    return 0


def get_release_configs():
    """Return a list of release configs."""
    import release_cfg
    maybe_configs = [getattr(release_cfg, cfg) for cfg in dir(release_cfg)]
    configs = [cfg for cfg in maybe_configs if inspect.isclass(cfg) and issubclass(cfg, release_cfg.Release)]
    enabled_configs = [cfg for cfg in configs if cfg.enabled]
    return enabled_configs


@subcmd(name='release', cmd='build')
def build(args):
    """Implement the build-release command.

    Build release takes the various partial releases, and combines them in to a single tar file.

    Additionally, it takes the binary 'prj' files and adds it to the appropriate place in the release tar file.

    """
    result = 0

    for config in get_release_configs():
        try:
            build_single_release(config, args.topdir)
        except FileNotFoundError as exc:
            logging.warning("Unable to build '%s'. File not found: '%s'", config, exc.filename)
            result = 1

    return result


def _arc_path_join(*elements):
    """Join path elements with '/' separators to construct a path string that works in release archives.
    Any tools that decompress release archive files need to handle such paths.
    Non-Windows tools generally do not handle a backslash character correctly as a path separator.
    However, forward-slash characters work as path separators on Windows and non-Windows platforms.
    Therefore, this function deliberately uses '/' forward slashes as path separators.
    This ensures that prj zip files created on any platform work on any other platform."""
    path = '/'.join(elements)
    path = path.replace('\\', '/')
    return path
