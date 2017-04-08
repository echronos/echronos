#!/bin/sh
#
# eChronos Real-Time Operating System
# Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, version 3, provided that these additional
# terms apply under section 7:
#
#   No right, title or interest in or to any trade mark, service mark, logo or
#   trade name of of National ICT Australia Limited, ABN 62 102 206 173
#   ("NICTA") or its licensors is granted. Modified versions of the Program
#   must be plainly marked as such, and must not be distributed using
#   "eChronos" as a trade mark or product name, or misrepresented as being the
#   original Program.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# @TAG(NICTA_AGPL)
#

set -u
set -e
set -x

GDB_VER=7.12
USAGE="Set up the prerequisites and dependencies of running regression tests via test.sh on a Ubuntu Linux host.\
-g GDB_VER: specify the version of GDB to install from source with PowerPC support; defaults to ${GDB_VER}"
TMPDIR="${TMPDIR:-/tmp}"

while getopts g: OPT
do
        case ${OPT} in
        g)  GDB_VER="${OPTARG}";;
        \?) echo ${USAGE}; exit 1;;
        esac
done
shift `expr ${OPTIND} - 1`

# python3 (>3.2): x.py
# splint: prj.py analyze
# gcc-powerpc-linux-gnu: prj.py build machine-qemu-ppce500.example.acamar-config-demo
# gcc-arm-none-eabi: required for building ARM systems
# gcc, gdb, qemu-system-ppc: x.py test systems
# texinfo: required for installing gdb from source
# xvfb pandoc wkhtmltopdf: required for building documentation
# python3.6: currently not available in default Travis CI environment
sudo add-apt-repository -y ppa:jonathonf/python-3.6
sudo apt-get -qq update
sudo apt-get -qq install -y python3 splint gcc gdb gcc-arm-none-eabi gcc-powerpc-linux-gnu qemu-system-ppc texinfo xvfb pandoc wkhtmltopdf
which python${PY_VER} || sudo apt-get install -y python${PY_VER}

# gdb-arm-none-eabi: required for testing ARM systems.
# Note: ARM systems are not currently regression tested however this file should serve as a rough
# guide to creating a basic RTOS development system, gdb with ARM support is required for this.
# The diversion is required due to an Ubuntu package bug that has been patched but may be present in some systems.
# See: https://bugs.launchpad.net/ubuntu/+source/gdb-arm-none-eabi/+bug/1267680
sudo dpkg-divert --package gdb --divert /usr/share/man/man1/arm-none-eabi-gdbserver.1.gz --rename /usr/share/man/man1/gdbserver.1.gz
sudo dpkg-divert --package gdb --divert /usr/share/man/man1/arm-none-eabi-gdb.1.gz --rename /usr/share/man/man1/gdb.1.gz
sudo apt-get -qq install -y gdb-arm-none-eabi

# install GDB with PowerPC support from source; required by x.py test systems
# unpack gdb tar ball to home directory to prevent tests below from discovering and failing on unrelated files
if ! test -e "${HOME}/local/bin/powerpc-linux-gdb"
then
    DIR="gdb-${GDB_VER}"
    FILE="${DIR}.tar.xz"
    URL="https://mirror.aarnet.edu.au/pub/gnu/gdb/${FILE}"

    cd "${TMPDIR}"
    wget -q "${URL}" || curl -o "${FILE}" "${URL}"
    tar xaf "${FILE}"
    cd "${DIR}"
    ./configure --target=powerpc-linux --prefix="${HOME}/local"
    make -s > make.log 2>&1 || { cat make.log; false; }
    make -s install > make.log 2>&1 || { cat make.log; false; }
fi

# The x tests depend on the master branch to exist in the git repository.
# Travis clones the git repository in such a way that the master branch is not available.
# Make it available:
cd "${TRAVIS_BUILD_DIR}"
if ! grep -e "fetch.*master" .git/config && ! grep -e "heads/\*" .git/config
then
    git config --add remote.origin.fetch "+refs/heads/master:refs/remotes/origin/master"
    git fetch --depth=1
    git branch --track master origin/master
fi
cd -
