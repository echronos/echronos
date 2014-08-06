import os

from .utils import BASE_DIR, get_host_platform_name, get_executable_extension
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


def build_manual(pkg_dir, verbose=False):
    markdown_file = os.path.join(pkg_dir, 'documentation.markdown')
    pdf_file = os.path.join(pkg_dir, 'documentation.pdf')
    html_file = os.path.join(pkg_dir, 'documentation.html')

    pandoc_executable = get_executable_from_repo_or_system('pandoc')
    doc_vars = get_doc_vars(markdown_file)
    pandoc_vars = ' '.join(['-V{}="{}"'.format(key, value) for key, value in doc_vars.items()])
    pandoc_cmd = '{}\
                  --write html\
                  --standalone\
                  --template="{}"\
                  --css="documentation_stylesheet.css"\
                  --toc --toc-depth=2\
                  {}\
                  --output="{}"\
                  "{}"'
    pandoc_cmd = pandoc_cmd.format(pandoc_executable,
                                   # pandoc fails if the template path is relative, so make it absolute:
                                   os.path.abspath(os.path.join(pkg_dir, 'documentation_template.html')),
                                   pandoc_vars,
                                   html_file,
                                   markdown_file)
    if verbose:
        print(pandoc_cmd)
    subprocess.check_call(pandoc_cmd)

    wkh_executable = get_executable_from_repo_or_system('wkhtmltopdf')
    wkh_cmd = '{}\
               --outline-depth 2\
               --page-size A4\
               --margin-top 20\
               --margin-bottom 25\
               --margin-left 20\
               --margin-right 20\
               --header-spacing 5\
               --header-html "{}"\
               --footer-spacing 5\
               --footer-html "{}"\
               --replace docid "Document ID: {}"\
               "{}" "{}"'
    wkh_cmd = wkh_cmd.format(wkh_executable,
                             os.path.join(pkg_dir, 'documentation_header.html'),
                             os.path.join(pkg_dir, 'documentation_footer.html'),
                             doc_vars['docid'],
                             html_file,
                             pdf_file)
    if verbose:
        print(wkh_cmd)
    subprocess.check_call(wkh_cmd)


def build_manuals(args):
    build([])
    for pkg_dir in get_package_dirs(set(('documentation.markdown',))):
        build_manual(pkg_dir, args.verbose)
