#!/usr/bin/env python3.3
"""
Overview
---------
`x.py` is the main *project management script* for the RTOS project.
As a *project magement script* it should handle any actions related to working on the project, such as building
artifacts for release, project management related tasks such as creating reviews, and similar task.
Any project management related task should be added as a subcommand to `x.py`, rather than adding another script.

Released Files
---------------

One of the main tasks of `x.py` is to create the releasable artifacts (i.e.: things that will be shipped to users).

### `prj` release information

prj will be distributed in source format for now as the customer likes it that way, and also because of the
impracticality of embedding python3 into a distributable .exe .
The enduser will need to install Python 3.3.
The tool can be embedded (not installed) into a project tree (i.e.: used inplace).

### Package release information

Numerous *packages* will be release to the end user.

One or more RTOS packages will be released.
These include:
* The RTOS core
* The RTOS optional-modules
* Test Suite
* Documentation (PDF)

Additionally one or more build modules will be released.
These include:
* Module
* Documentation

### Built files

The following output files will be produced by `x.py`.

* release/prj-<version>.zip
* release/<rtos-foo>-<version>.zip
* release/<build-name>-<version>.zip

Additional Requirements
-----------------------

This `x.py` tool shouldn't leave old files around (like every other build tool on the planet.)
So, if `x.py` is building version 3 of a given release, it should ensure old releases are not still in the `releases`
directory.

"""
import sys
import os

externals = ['pep8', 'nose', 'ice']
from pylib.utils import BASE_DIR

sys.path = [os.path.join(BASE_DIR, 'external_tools', e) for e in externals] + sys.path
sys.path.insert(0, os.path.join(BASE_DIR, 'prj/app/pystache'))
if __name__ == '__main__':
    sys.modules['x'] = sys.modules['__main__']

### Check that the correct Python is being used.
correct = None
if sys.platform == 'darwin':
    correct = os.path.abspath(os.path.join(BASE_DIR, 'tools/x86_64-apple-darwin/bin/python3.3'))
elif sys.platform.startswith('linux'):
    correct = os.path.abspath(os.path.join(BASE_DIR, 'tools/x86_64-unknown-linux-gnu/bin/python3.3'))

if correct is not None and sys.executable != correct:
    print("x.py expects to be executed using {} (not {}).".format(correct, sys.executable), file=sys.stderr)
    sys.exit(1)

import argparse
import calendar
import glob
import inspect
import io
import ice
import logging
import signal
import shutil
import subprocess
import tarfile
import zipfile
from contextlib import contextmanager
from glob import glob

from pylib.tasks import new_review, new_task, tasks, integrate, Git
from pylib.tests import prj_test, x_test, pystache_test, rtos_test, check_pep8
from pylib.utils import base_path, top_path, base_to_top_paths, chdir, tempdir
from pylib.components import Component, ArchitectureComponent, Architecture, RtosSkeleton

# Set up a specific logger with our desired output level
logger = logging.getLogger()
logger.setLevel(logging.INFO)


BASE_TIME = calendar.timegm((2013, 1, 1, 0, 0, 0, 0, 0, 0))


# topdir is the rtos repository directory in which the user invoked the x tool.
# If the x tool is invoked from a client repository through a wrapper, topdir contains the directory of that client
# repository.
# If the user directly invokes x tool of the RTOS core, topdir is the directory of this file.
# topdir defaults to the core directory.
# It may be modified by an appropriate invocation of main().
topdir = os.path.normpath(os.path.dirname(__file__))


SIG_NAMES = dict((k, v) for v, k in signal.__dict__.items() if v.startswith('SIG'))


def show_exit(exit_code):
    sig_num = exit_code & 0xff
    exit_status = exit_code >> 8
    if sig_num == 0:
        return "exit: {}".format(exit_status)
    else:
        return "signal: {}".format(SIG_NAMES.get(sig_num, 'Unknown signal {}'.format(sig_num)))


def get_host_platform_name():
    if sys.platform == 'darwin':
        return 'x86_64-apple-darwin'
    elif sys.platform == 'linux':
        return 'x86_64-unknown-linux-gnu'
    elif sys.platform == 'win32':
        return 'win32'
    else:
        raise RuntimeError('Unsupported platform {}'.format(sys.platform))


_executable_extension = None


def get_executable_extension():
    global _executable_extension
    if _executable_extension is None:
        _executable_extension = {'darwin': '',
                                 'linux': '',
                                 'win32': '.exe',
                                 }[sys.platform]
    return _executable_extension


def prj_build(args):
    host = get_host_platform_name()
    if sys.platform == 'darwin':
        extras = ['-framework', 'CoreFoundation', '-lz']
    elif sys.platform == 'linux':
        extras = ['-lz', '-lm', '-lpthread', '-lrt', '-ldl', '-lcrypt', '-lutil']
    elif sys.platform == 'win32':
        pass
    else:
        print("Building prj currently unsupported on {}".format(sys.platform))
        return 1

    prj_build_path = top_path(args.topdir, 'prj_build_{}'.format(host))
    os.makedirs(prj_build_path, exist_ok=True)

    if sys.platform == 'win32':
        prj_build_win32(prj_build_path)
    else:
        with chdir(prj_build_path):
            ice.create_lib('prj', '../prj/app', main='prj')
            ice.create_lib('prjlib', '../prj/app/lib')
            ice.create_lib('pystache', '../prj/app/pystache',
                           excluded=['setup', 'pystache.tests', 'pystache.commands'])
            ice.create_lib('ply', '../prj/app/ply', excluded=['setup'])
            ice.create_stdlib()
            ice.create_app(['stdlib', 'prj', 'prjlib', 'pystache', 'ply'])

            cmd = ['gcc', '*.c', '-o', 'prj', '-I../tools/include/python3.3m/',
                   '-I../tools/{}/include/python3.3m/'.format(host),
                   '-L../tools/{}/lib/python3.3/config-3.3m'.format(host),
                   '-lpython3.3m']
            cmd += extras

            cmd = ' '.join(cmd)
            r = os.system(cmd)
            if r != 0:
                print("Error building {}. cmd={}. ".format(show_exit(r), cmd))


def prj_build_win32(output_dir):
    """Create a distributable version of prj.py.

    We currently do not have the infrastructure in place to statically compile and link prj.py and its dependencies
    against the complete python interpreter.

    However, it is still desirable to create only a single resource that can stand alone given an installed python
    interpreter.
    Therefore, collect prj and its dependencies in a zip file that is executable by the python interpreter.

    """
    with zipfile.ZipFile(os.path.join(output_dir, 'prj'), mode='w') as zip:
        top = os.path.abspath(base_path('prj', 'app'))
        top_len = len(top)
        for dir_path, dir_names, file_names in os.walk(top):
            archive_dir_path = os.path.relpath(dir_path, top)
            for file_name in file_names:
                file_path = os.path.join(dir_path, file_name)
                if dir_path == top and file_name == 'prj.py':
                    # The python interpreter expects to be informed about the main file in the zip file by naming it
                    # __main__.py
                    archive_file_path = os.path.join(archive_dir_path, '__main__.py')
                else:
                    archive_file_path = os.path.join(archive_dir_path, file_name)
                zip.write(file_path, archive_file_path)
    with open(os.path.join(output_dir, 'prj.bat'), 'w') as f:
        f.write('@ECHO OFF\npython %~dp0\\prj')


def build(args):
    # Generate RTOSes
    for rtos_name, arch_names in configurations.items():
        generate_rtos_module(skeletons[rtos_name], [architectures[arch] for arch in arch_names])


def generate_rtos_module(skeleton, architectures):
    """Generate RTOS modules for several architectures from a given skeleton."""
    for arch in architectures:
        rtos_module = skeleton.create_configured_module(arch)
        rtos_module.generate()


@contextmanager
def tarfile_open(name, mode, **kwargs):
    assert mode.startswith('w')
    with tarfile.open(name, mode, **kwargs) as f:
        try:
            yield f
        except:
            os.unlink(name)
            raise


class FileWithLicense:
    """FileWithLicense provides a read-only file-like object that automatically includes license text when reading
    from the underlying file object.

    The FileWithLicense object takes ownership of the underlying file object.
    The original file object should not be used after passing it to the FileWithLicense object.

    """
    def __init__(self, f, lic, xml_mode):
        XML_PROLOG = b'<?xml version="1.0" encoding="UTF-8" ?>\n'
        self._f = f
        self._read_license = True

        if xml_mode:
            lic = XML_PROLOG + lic
            file_header = f.read(len(XML_PROLOG))
            if file_header != XML_PROLOG:
                raise Exception("XML File: '{}' does not contain expected prolog: {} expected {}".
                                format(f.name, file_header, XML_PROLOG))

        if len(lic) > 0:
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
            data += self._f.read(size)

        return data

    def close(self):
        self._f.close()

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()


class LicenseOpener:
    """The license opener provides a single 'open' method, that can be used instead of the built-in 'open' function.

    This open will return a file-like object that modifies the underlying file to include an appropriate license
    header.

    The 'license' is passed to the object during construction.

    """
    def __init__(self, license):
        self.license = license

    def _get_lic(self, filename):
        lic = ''
        ext = os.path.splitext(filename)[1]
        is_xml = False

        if ext in ['.c', '.h', '.ld', '.s']:
            lic = '/*' + self.license + '*/\n'
        elif ext in ['.py']:
            lic = '"""' + self.license + '"""\n'
        elif ext in ['.prx']:
            lic = '<!--' + self.license + '-->\n'
            is_xml = True
        elif ext in ['.asm']:
            lic = "\n".join(['; ' + line for line in self.license.rsplit("\n")]) + "\n"

        lic = lic.encode('utf8')

        return lic, is_xml

    def open(self, filename, mode):
        assert mode == 'rb'

        f = open(filename, mode)
        lic, is_xml = self._get_lic(filename)
        return FileWithLicense(f, lic, is_xml)

    def tar_info_filter(self, tarinfo):
        if tarinfo.isreg():
            lic, _ = self._get_lic(tarinfo.name)
            tarinfo.size += len(lic)
        return tar_info_filter(tarinfo)


def tar_info_filter(tarinfo):
    tarinfo.uname = '_default_user_'
    tarinfo.gname = '_default_group_'
    tarinfo.mtime = BASE_TIME
    tarinfo.uid = 1000
    tarinfo.gid = 1000
    return tarinfo


def tar_add_data(tf, arcname, data, ti_filter=None):
    """Directly add data to a tarfile.

    tf is a tarfile.TarFile object.
    arcname is the name the data will have in the archive.
    data is the raw data (which should be of type 'bytes').
    fi_filter filters the created TarInfo object. (In a similar manner to the tarfile.TarFile.add() method.

    """
    ti = tarfile.TarInfo(arcname)
    ti.size = len(data)
    if ti_filter:
        ti = ti_filter(ti)
    tf.addfile(ti, io.BytesIO(data))


def tar_gz_with_license(output, tree, prefix, license):

    """Create a tar.gz file named `output` from a specified directory tree.

    Any appropriate files have the specified license attached.

    When creating the tar.gz a standard set of meta-data will be used to help ensure things are consistent.

    """
    lo = LicenseOpener(license)
    try:
        with tarfile.open(output, 'w:gz', format=tarfile.GNU_FORMAT) as tf:
            tarfile.bltn_open = lo.open
            with chdir(tree):
                for f in os.listdir('.'):
                    tf.add(f, arcname='{}/{}'.format(prefix, f), filter=lo.tar_info_filter)
    finally:
        tarfile.bltn_open = open


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
                if pkg_name in pkgs:
                    logging.warn('Overriding package {} with package {}'.format(pkgs[pkg_name].path, pkg_path))
                pkgs[pkg_name] = Package(pkg_path)
        return pkgs

    def __init__(self, path):
        assert os.path.isdir(path)
        self.path = path
        self.name = os.path.basename(self.path)


class ReleasePackage:
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

    def get_archive_name(self):
        return '{}-{}'.format(self._pkg.name, self._rls_cfg.release_name)

    def get_path_in_archive(self):
        return 'share/packages/{}'.format(self._pkg.name)

    def get_license(self):
        return self._rls_cfg.license


def mk_partial(pkg, topdir):
    fn = top_path(topdir, 'release', 'partials', '{}.tar.gz'.format(pkg.get_archive_name()))
    src_prefix = 'share/packages/{}'.format(pkg.get_name())
    tar_gz_with_license(fn, pkg.get_path(), src_prefix, pkg.get_license())


def build_partials(args):
    build([])
    os.makedirs(top_path(args.topdir, 'release', 'partials'),  exist_ok=True)
    packages = Package.create_from_disk(args.topdir).values()
    for pkg in packages:
        for config in get_release_configs():
            release_package = ReleasePackage(pkg, config)
            mk_partial(release_package, args.topdir)


def build_manual(pkg):
    manual_file = os.path.join(BASE_DIR, 'packages', pkg, '{}-manual'.format(pkg))
    if not os.path.exists(manual_file):
        print("Manual '{}' does not exist.".format(manual_file))
    else:
        print("Transforming manual '{}'".format(manual_file))


def build_manuals(args):
    build([])
    packages = os.listdir(os.path.join(BASE_DIR, 'packages'))
    for pkg in packages:
        build_manual(pkg)


class ReleaseMeta(type):
    """A pretty-printing meta-class for the Release class."""
    def __str__(cls):
        return '{}-{}'.format(cls.release_name, cls.version)


class Release(metaclass=ReleaseMeta):
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


def get_release_configs():
    """Return a list of release configs."""
    import release_cfg
    maybe_configs = [getattr(release_cfg, cfg) for cfg in dir(release_cfg)]
    configs = [cfg for cfg in maybe_configs if inspect.isclass(cfg) and issubclass(cfg, Release)]
    enabled_configs = [cfg for cfg in configs if cfg.enabled]
    return enabled_configs


def build_single_release(config, topdir):
    """Build a release archive for a specific release configuration."""
    basename = '{}-{}-{}'.format(config.product_name, config.release_name, config.version)
    logging.info("Building {}".format(basename))
    tarfilename = top_path(topdir, 'release', '{}.tar.gz'.format(basename))
    with tarfile_open(tarfilename, 'w:gz', format=tarfile.GNU_FORMAT) as tf:
        for pkg in config.packages:
            release_file_path = top_path(topdir, 'release', 'partials', '{}-{}.tar.gz')
            with tarfile.open(release_file_path.format(pkg, config.release_name), 'r:gz') as in_f:
                for m in in_f.getmembers():
                    m_f = in_f.extractfile(m)
                    m.name = basename + '/' + m.name
                    tf.addfile(m, m_f)
        for plat in config.platforms:
            arcname = '{}/{}/bin/prj'.format(basename, plat)
            tf.add('prj_build_{}/prj'.format(plat), arcname=arcname, filter=tar_info_filter)
        if config.top_level_license is not None:
            tar_add_data(tf, '{}/LICENSE'.format(basename),
                         config.top_level_license.encode('utf8'),
                         tar_info_filter)

        for arcname, filename in config.extra_files:
            tf.add(filename, arcname='{}/{}'.format(basename, arcname), filter=tar_info_filter)

        if 'TEAMCITY_VERSION' in os.environ:
            build_info = os.environ['BUILD_VCS_NUMBER']
        else:
            g = Git()
            build_info = g.branch_hash()
            if not g.working_dir_clean():
                build_info += "-unclean"
        build_info += '\n'
        tar_add_data(tf, '{}/build_info'.format(basename), build_info.encode('utf8'), tar_info_filter)


def build_release(args):
    """Implement the build-release command.

    Build release takes the various partial releases, and combines them in to a single tar file.

    Additionally, it takes the binary 'prj' files and adds it to the appropriate place in the release tar file.

    """
    for config in get_release_configs():
        try:
            build_single_release(config, args.topdir)
        except FileNotFoundError as e:
            logging.warning("Unable to build '{}'. File not found: '{}'".format(config, e.filename))


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
    with tarfile.open(rel_file, 'r:gz') as tf:
        for m in tf.getmembers():
            if m.gid != 1000:
                raise Exception("m.gid != 1000 {} -- {}".format(m.gid, m.name))
            if m.uid != 1000:
                raise Exception("m.uid != 1000 {} -- {}".format(m.uid, m.name))
            if m.mtime != BASE_TIME:
                raise Exception("m.gid != BASE_TIME({}) {} -- {}".format(m.mtime, BASE_TIME, m.name))

    platform = get_host_platform_name()

    with tempdir() as td:
        with chdir(td):
            assert shutil.which('tar')
            subprocess.check_call("tar xf {}".format(rel_file).split())
            release_dir = os.path.splitext(os.path.splitext(os.path.basename(archive))[0])[0]
            if not os.path.isdir(release_dir):
                raise RuntimeError("Release archive does not extract into top directory with the same name as the "
                                   "base name of the archive ({})".format(release_dir))
            with chdir(release_dir):
                cmd = "./{}/bin/prj".format(platform)
                try:
                    subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True)
                except subprocess.CalledProcessError as e:
                    if e.returncode != 1:
                        raise e
                pkgs = []
                pkg_root = './share/packages/'
                for root, _dir, files in os.walk(pkg_root):
                    for f in files:
                        if f.endswith('.prx'):
                            pkg = os.path.join(root, f)[len(pkg_root):-4].replace(os.sep, '.')
                            pkgs.append((pkg, os.path.join(root, f)))
                with open('project.prj', 'w') as f:
                    f.write(project_prj_template.format(pkg_root))
                for pkg, f in pkgs:
                    cmd = "./{}/bin/prj build {}".format(platform, pkg)
                    try:
                        subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True)
                    except subprocess.CalledProcessError as e:
                        err_str = None
                        for l in e.output.splitlines():
                            l = l.decode()
                            if l.startswith('ERROR'):
                                err_str = l
                                break
                        if err_str is None:
                            print(e.output)
                            raise e
                        elif 'missing or contains multiple Builder modules' in err_str:
                            pass
                        else:
                            print("Unexpected error:", err_str)
                            raise e


def release_test(args):
    """Implement the test-release command.

    This command is used to perform sanity checks and testing of the full release.

    """
    for rel in glob(top_path(args.topdir, 'release', '*.tar.gz')):
        release_test_one(rel)


class OverrideFunctor:
    def __init__(self, function, *args, **kwargs):
        self.function = function
        self.args = args
        self.kwargs = kwargs

    def __call__(self, *args, **kwargs):
        return self.function(*self.args, **self.kwargs)


CORE_ARCHITECTURES = {
    'posix': Architecture('posix', {}),
    'armv7m': Architecture('armv7m', {}),
}

CORE_SKELETONS = {
    'sched-rr-test': RtosSkeleton(
        'sched-rr-test',
        [Component('reentrant'),
         Component('sched', 'sched-rr', {'assume_runnable': False}),
         Component('sched-rr-test')]),
    'sched-prio-test': RtosSkeleton(
        'sched-prio-test',
        [Component('reentrant'),
         Component('sched', 'sched-prio', {'assume_runnable': False}),
         Component('sched-prio-test')]),
    'sched-prio-inherit-test': RtosSkeleton(
        'sched-prio-inherit-test',
        [Component('reentrant'),
         Component('sched', 'sched-prio-inherit', {'assume_runnable': False}),
         Component('sched-prio-inherit-test')]),
    'simple-mutex-test': RtosSkeleton(
        'simple-mutex-test',
        [Component('reentrant'),
         Component('mutex', 'simple-mutex'),
         Component('simple-mutex-test')]),
    'blocking-mutex-test': RtosSkeleton(
        'blocking-mutex-test',
        [Component('reentrant'),
         Component('mutex', 'blocking-mutex'),
         Component('blocking-mutex-test')]),
    'simple-semaphore-test': RtosSkeleton(
        'simple-semaphore-test',
        [Component('reentrant'),
         Component('semaphore', 'simple-semaphore'),
         Component('simple-semaphore-test')]),
    'acamar': RtosSkeleton(
        'acamar',
        [Component('reentrant'),
         Component('acamar'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('context_switch', 'context-switch'),
         Component('error'),
         Component('task'),
         ]),
    'gatria': RtosSkeleton(
        'gatria',
        [Component('reentrant'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('context_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': True}),
         Component('mutex', 'simple-mutex'),
         Component('error'),
         Component('task'),
         Component('gatria'),
         ]),
    'kraz': RtosSkeleton(
        'kraz',
        [Component('reentrant'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('ctxt_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': True}),
         Component('signal'),
         Component('mutex', 'simple-mutex'),
         Component('error'),
         Component('task'),
         Component('kraz'),
         ]),
    'acrux': RtosSkeleton(
        'acrux',
        [Component('reentrant'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('ctxt_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': False}),
         ArchitectureComponent('interrupt_event_arch', 'interrupt-event'),
         Component('interrupt_event', 'interrupt-event', {'timer_process': False, 'task_set': False}),
         Component('mutex', 'simple-mutex'),
         Component('error'),
         Component('task'),
         Component('acrux'),
         ]),
    'rigel': RtosSkeleton(
        'rigel',
        [Component('reentrant'),
         ArchitectureComponent('stack', 'stack'),
         ArchitectureComponent('ctxt_switch', 'context-switch'),
         Component('sched', 'sched-rr', {'assume_runnable': False}),
         Component('signal'),
         ArchitectureComponent('timer_arch', 'timer'),
         Component('timer'),
         ArchitectureComponent('interrupt_event_arch', 'interrupt-event'),
         Component('interrupt_event', 'interrupt-event', {'timer_process': True, 'task_set': True}),
         Component('mutex', 'blocking-mutex'),
         Component('profiling'),
         Component('message-queue'),
         Component('error'),
         Component('task'),
         Component('rigel'),
         ],
    ),
}


CORE_CONFIGURATIONS = {
    'sched-rr-test': ['posix'],
    'sched-prio-inherit-test': ['posix'],
    'simple-mutex-test': ['posix'],
    'blocking-mutex-test': ['posix'],
    'simple-semaphore-test': ['posix'],
    'sched-prio-test': ['posix'],
    'acamar': ['posix', 'armv7m'],
    'gatria': ['posix', 'armv7m'],
    'kraz': ['posix', 'armv7m'],
    'acrux': ['armv7m'],
    'rigel': ['armv7m'],
}


# client repositories may extend or override the following variables to control which configurations are available
architectures = CORE_ARCHITECTURES.copy()
skeletons = CORE_SKELETONS.copy()
configurations = CORE_CONFIGURATIONS.copy()


def main():
    """Application main entry point. Parse arguments, and call specified sub-command."""
    SUBCOMMAND_TABLE = {
        'prj-build': prj_build,
        'build': build,
        'test-release': release_test,
        'build-release': build_release,
        'build-partials': build_partials,
        'build-manuals': build_manuals,
        # Testing
        'check-pep8': check_pep8,
        'prj-test': prj_test,
        'pystache-test': pystache_test,
        'x-test': x_test,
        'rtos-test': rtos_test,
        # Tasks management
        'new-review': new_review,
        'new-task': new_task,
        'tasks': tasks,
        'integrate': integrate,
    }

    # create the top-level parser
    parser = argparse.ArgumentParser(prog='x.py')

    subparsers = parser.add_subparsers(title='subcommands', dest='command')

    # create the parser for the "prj.pep8" command
    subparsers.add_parser('tasks', help="List tasks")

    _parser = subparsers.add_parser('check-pep8', help='Run PEP8 on project Python files')
    _parser.add_argument('--teamcity', action='store_true',
                         help="Provide teamcity output for tests",
                         default=False)
    _parser.add_argument('--excludes', nargs='*',
                         help="Exclude directories from pep8 checks",
                         default=[])

    for component_name in ['prj', 'x', 'rtos']:
        _parser = subparsers.add_parser(component_name + '-test', help='Run {} unittests'.format(component_name))
        _parser.add_argument('tests', metavar='TEST', nargs='*',
                             help="Specific test", default=[])
        _parser.add_argument('--list', action='store_true',
                             help="List tests (don't execute)",
                             default=False)
        _parser.add_argument('--verbose', action='store_true',
                             help="Verbose output",
                             default=False)
        _parser.add_argument('--quiet', action='store_true',
                             help="Less output",
                             default=False)
    subparsers.add_parser('prj-build', help='Build prj')

    subparsers.add_parser('pystache-test', help='Test pystache')
    subparsers.add_parser('build-release', help='Build final release')
    subparsers.add_parser('test-release', help='Test final release')
    subparsers.add_parser('build-partials', help='Build partial release files')
    subparsers.add_parser('build-manuals', help='Build PDF manuals')
    subparsers.add_parser('build', help='Build all release files')
    _parser = subparsers.add_parser('new-review', help='Create a new review')
    _parser.add_argument('reviewers', metavar='REVIEWER', nargs='+',
                         help='Username of reviewer')

    _parser = subparsers.add_parser('new-task', help='Create a new task')
    _parser.add_argument('taskname', metavar='TASKNAME', help='Name of the new task')
    _parser.add_argument('--no-fetch', dest='fetch', action='store_false', default='true', help='Disable fetchign')

    # generate parsers and command table entries for generating RTOS variants
    for rtos_name, arch_names in configurations.items():
        SUBCOMMAND_TABLE[rtos_name + '-gen'] = OverrideFunctor(generate_rtos_module,
                                                               skeletons[rtos_name],
                                                               [architectures[arch] for arch in arch_names])
        subparsers.add_parser(rtos_name + '-gen', help="Generate {} RTOS".format(rtos_name))

    _parser = subparsers.add_parser('integrate', help='Integrate a completed development task/branch into the main \
upstream branch.')
    _parser.add_argument('--repo', help='Path of git repository to operate in. \
Defaults to current working directory.')
    _parser.add_argument('--name', help='Name of the task branch to integrate. \
Defaults to active branch in repository.')
    _parser.add_argument('--target', help='Name of branch to integrate task branch into. \
Defaults to "development".', default='development')
    _parser.add_argument('--archive', help='Prefix to add to task branch name when archiving it. \
Defaults to "archive".', default='archive')

    args = parser.parse_args()

    # Default to building
    if args.command is None:
        args.command = 'build'
    args.topdir = topdir

    return SUBCOMMAND_TABLE[args.command](args)


if __name__ == "__main__":
    sys.exit(main())
