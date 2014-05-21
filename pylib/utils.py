import os


def follow_link(l):
    """Return the underlying file from a symbolic link.

    This will (recursively) follow links until a non-symbolic link is found.
    No cycle checks are performed by this function.

    """
    if not os.path.islink(l):
        return l
    p = os.readlink(l)
    if os.path.isabs(p):
        return p
    return follow_link(os.path.join(os.path.dirname(l), p))


BASE_DIR = os.path.normpath(os.path.join(os.path.dirname(follow_link(__file__)), ".."))
