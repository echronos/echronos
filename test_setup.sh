#!/bin/sh
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

# if there is no local master branch, create it because some tests depend on it
if ! git rev-parse --verify master > /dev/null 2>&1
then
    git config remote.origin.fetch "+refs/heads/*:refs/remotes/origin/*"
    git fetch --depth=1 origin master
    git branch --track master origin/master
fi

# build-essential: installing gdb from source
# python3 (>3.2): x.py
# splint: prj.py analyze
# gcc-powerpc-linux-gnu: prj.py build machine-qemu-ppce500.example.acamar-config-demo
# gcc-arm-none-eabi: required for building ARM systems
# gcc, gdb, qemu-system-ppc: x.py test systems
# texinfo: required for installing gdb from source
# xvfb pandoc wkhtmltopdf: required for building documentation
# python3.5, python3.6: currently not available in default Travis CI environment
sudo add-apt-repository -y ppa:jonathonf/python-3.5
sudo add-apt-repository -y ppa:jonathonf/python-3.6
sudo apt-get -qq update
sudo apt-get -qq install -y build-essential python3 splint gcc gdb gcc-arm-none-eabi gcc-powerpc-linux-gnu qemu-system-ppc texinfo xvfb pandoc wkhtmltopdf
which python${PY_VER} || sudo apt-get install -y python${PY_VER}

# gdb-arm-none-eabi: required for testing ARM systems.
# The diversion is required due to an Ubuntu package bug that has been patched but may be present in some systems.
# See: https://bugs.launchpad.net/ubuntu/+source/gdb-arm-none-eabi/+bug/1267680
sudo dpkg-divert --package gdb --divert /usr/share/man/man1/arm-none-eabi-gdbserver.1.gz --rename /usr/share/man/man1/gdbserver.1.gz
sudo dpkg-divert --package gdb --divert /usr/share/man/man1/arm-none-eabi-gdb.1.gz --rename /usr/share/man/man1/gdb.1.gz
sudo apt-get -qq install -y gdb-arm-none-eabi

# libpixman-1-0, libglib2.0-0: required by qemu-system-arm
sudo apt-get -qq install -y libpixman-1-0 libglib2.0-0
# qemu-system-arm: required for testing ARM systems.
wget 'https://github.com/echronos/qemu/releases/download/v2.11.0-rc2/qemu-system-arm_2.11.0-rc2_trusty.deb'
sudo dpkg -i qemu-system-arm_2.11.0-rc2_trusty.deb
rm qemu-system-arm_2.11.0-rc2_trusty.deb # so license check doesn't fail later

# workaround for https://github.com/travis-ci/travis-ci/issues/8363
python${PY_VER} --version || pyenv global system ${PY_VER}

# If not available, install the Python package manager pip.
# Currently, this is necessary for both Python 3.4 and Python 3.6
if ! python${PY_VER} -m pip --version
then
    wget 'https://bootstrap.pypa.io/get-pip.py'
    python${PY_VER} get-pip.py --user
    rm get-pip.py # necessary so that license and pylint tests do not pick this up as a file belonging to the project
    python${PY_VER} -m pip --version
fi

python${PY_VER} -m pip install --user pylint

# install GDB with PowerPC support from source; required by x.py test systems
# unpack gdb tar ball to home directory to prevent tests below from discovering and failing on unrelated files
if ! test -e "${HOME}/local/bin/powerpc-linux-gdb"
then
    DIR="gdb-${GDB_VER}"
    FILE="${DIR}.tar.xz"
    URL="https://mirror.aarnet.edu.au/pub/gnu/gdb/${FILE}"

    cd "${TMPDIR}"
    [ -e "${FILE}" ] || wget -q "${URL}" || curl -o "${FILE}" "${URL}"
    [ ! -d "${DIR}" ] || rm -fr "${DIR}"
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
