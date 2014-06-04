import os

from .utils import BASE_DIR
from .components import build


def build_manual(pkg):
    manual_file = os.path.join(BASE_DIR, 'packages', pkg, '{}-manual'.format(pkg))
    if not os.path.exists(manual_file):
        print("Manual '{}' does not exist.".format(manual_file))
    else:
        print("Transforming manual '{}'".format(manual_file))


def build_manuals(args):
    build(args)
    packages = os.listdir(os.path.join(BASE_DIR, 'packages'))
    for pkg in packages:
        build_manual(pkg)
