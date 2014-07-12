import os
import sys
from prj import Module


def get_module_files():
    for file_path in get_platform_file_paths(sys.platform):
        file_desc = {'input': file_path, 'output': os.path.basename(file_path)}
        if os.path.splitext(file_path)[1].lower() == '.c':
            file_desc['type'] = 'c'
        yield file_desc


def get_platform_file_paths(platform):
    platform_directory = os.path.join(os.path.dirname(__file__), platform)
    if os.path.isdir(platform_directory):
        return [os.path.join(platform, p) for p in os.listdir(platform_directory)
                if os.path.splitext(p)[1].lower() in ('.c', '.h')]
    else:
        return ()


class HostPlatformModule(Module):
    """Adds source files to the build process that are host-platform specific and required on the given host platform
    to make a build succeed.

    If this Python module is in directory foo, it picks up all source files in the directory foo/<platform>.
    <platform> is the value of the Python expression "sys.platform".
    The source files are returned such that the build system copies them straight into the build directory.
    They then reside in the build directory itself, not a platform-specific subdirectory.

    In the case of building for the posix target platform on the win32 host platform, this mixes in the ucontext.c/h
    files because neither MinGW nor Cygwin gcc come with implementations of this user-space context-switching
    functionality.

    """
    files = tuple(get_module_files())

module = HostPlatformModule()
