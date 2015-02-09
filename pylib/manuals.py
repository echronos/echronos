import os
import shutil
import subprocess
import sys

from .utils import BASE_DIR, base_path, get_host_platform_name, get_executable_extension, find_path
from .components import build


def get_platform_tools_dir():
    return os.path.join(BASE_DIR, 'tools', get_host_platform_name())


class ExecutableNotAvailable(Exception):
    pass


def get_executable_from_repo_or_system(name):
    """Get the path of an executable, searching first in the repository's tool directory, then on the system.

    `name` is the base name of the executable without path components and without extensions.

    If no executable with the given name can be found, an ExecutableNotAvailable exception is raised.

    Example:
    - On Windows:
      get_executable_from_repo_or_system('pandoc') => '.\\tools\\win32\\bin\\pandoc.exe'
    - On Linux with pandoc installed in the system:
      get_executable_from_repo_or_system('pandoc') => '/usr/bin/pandoc'

    """
    path = os.path.join(get_platform_tools_dir(), 'bin', name + get_executable_extension())
    if not os.path.exists(path):
        path = shutil.which(name)
        if not path:
            raise ExecutableNotAvailable('Unable to find the executable "{}" in the repository or the system. \
This may be resolved by installing the executable on your system or it may indicate that the RTOS toolchain does not \
support your host platform.'.format(name))
    return path


def get_package_dirs(required_files=None):
    if required_files is None:
        required_files = set()

    for root, dirs, files in os.walk(os.path.join(BASE_DIR, 'packages')):
        if required_files.issubset(files):
            yield root


def get_doc_vars(markdown_file):
    doc_vars = {}
    for line in open(markdown_file).readlines():
        if line.startswith('<!-- %'):
            key, value = line.strip()[6:-4].split(' ', 1)
            doc_vars[key] = value
    return doc_vars


def build_manual(pkg_dir, top_dir, verbose=False):
    markdown_file = os.path.join(pkg_dir, 'documentation.markdown')
    pdf_file = os.path.join(pkg_dir, 'documentation.pdf')
    html_file = os.path.join(pkg_dir, 'documentation.html')

    doc_vars = get_doc_vars(markdown_file)
    if not doc_vars:
        print('Not generating manual for {} because documentation is incomplete'.format(pkg_dir))
        return

    css_abs_path = find_path(os.path.join('docs', 'manual_template', 'documentation_stylesheet.css'), top_dir)
    css_url = os.path.relpath(css_abs_path, pkg_dir).replace(os.path.sep, '/')

    pandoc_executable = get_executable_from_repo_or_system('pandoc')
    pandoc_cmd = [pandoc_executable,
                  '--write', 'html',
                  '--standalone',
                  '--template=' + os.path.abspath(base_path('docs', 'manual_template',
                                                            'documentation_template.html')),
                  '--css=' + css_url,
                  '--toc', '--toc-depth=2'] +\
                 ['-V{}={}'.format(key, value) for key, value in doc_vars.items()] +\
                 ['--output=' + html_file,
                  markdown_file]
    if verbose:
        print(pandoc_cmd)
    subprocess.check_call(pandoc_cmd)

    wkh_executable = get_executable_from_repo_or_system('wkhtmltopdf')
    wkh_cmd = [wkh_executable,
               '--outline-depth', '2',
               '--page-size', 'A4',
               '--margin-top', '20',
               '--margin-bottom', '25',
               '--margin-left', '20',
               '--margin-right', '20',
               '--header-spacing', '5',
               '--header-html', find_path(os.path.join('docs', 'manual_template', 'documentation_header.html'),
                                          top_dir),
               '--footer-spacing', '5',
               '--footer-html', find_path(os.path.join('docs', 'manual_template', 'documentation_footer.html'),
                                          top_dir),
               '--replace', 'docid', 'Document ID: {}'.format(doc_vars['docid']),
               html_file,
               pdf_file]
    if verbose:
        print(wkh_cmd)
    try:
        subprocess.check_call(wkh_cmd)
    except subprocess.CalledProcessError:
        if not sys.platform.startswith('win'):
            print('If wkhtmltopdf fails because it cannot connect to an X server, try installing xvfb and running \
your command as xvfb-run -a -s "-screen 0 640x480x16" ./x.py [...]')
        raise


def build_manuals(args):
    build(args)
    for pkg_dir in get_package_dirs(set(('documentation.markdown',))):
        build_manual(pkg_dir, args.topdir, args.verbose)
