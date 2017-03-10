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

set -eu

USAGE="Run all regression tests available in the repository. \
-c CORE_DIR: the directory containing the repository contents; defaults to the parent directory of this script. \
-p PYTHON_VERSIONS: list of versions of Python the script shall test; version numbers are separated by blanks (e.g., '3.3 3.4'); defaults to all Python versions >= 3.3."
CORE_DIR="$(dirname "${0}")"
PY_VERSIONS="$(for V in 3.3 3.4 3.5 3.6 3.7 3.8 3.9; do ! python${V} --version > /dev/null 2>&1 || printf "${V} "; done)"

while getopts c:p: OPT
do
        case ${OPT} in
        c)  CORE_DIR="${OPTARG}";;
        p)  PY_VERSIONS="${OPTARG}";;
        \?) echo ${USAGE}; exit 1;;
        esac
done
shift `expr ${OPTIND} - 1`

TEST_PACKAGES="$(find "${CORE_DIR}/packages" -name '*.prx' -and -not -name '*config-only*.prx' -and -not -name gatria-system-simple.prx | sed 's/.*packages\///; s/.prx//; s/\//./g')"

export PATH="${PATH}:${HOME}/local/bin:${PWD}/tools/x86_64-unknown-linux-gnu/bin:${PWD}/${CORE_DIR}/tools/x86_64-unknown-linux-gnu/bin"

gcc --version
gdb --version

FAILED_TESTS=0

run_test () {
    #
    # Execute all function parameters (${@}) as a single command and print messages to make test output easily understandable
    #
    echo ""
    echo "#################### ${@} ####################"

    # do not cause this script to exit with an error if the test command fails
    set +e
    ${@}
    EC=${?}
    set -e
    if [ ${EC} -eq 0 ]
    then
        echo "-------------------- PASS --------------------"
    else
        echo "!!!!!!!!!!!!!!!!!!!! FAIL: ${@} -> ${EC} !!!!!!!!!!!!!!!!!!!!"
        FAILED_TESTS=$((${FAILED_TESTS} + 1))
    fi
    echo ""
}

test_build_test_systems () {
    local PASSES FAILS PKG
    PASSES=0
    FAILS=0
    for PKG in ${TEST_PACKAGES}
    do
        python${PY_VER} "${CORE_DIR}/prj/app/prj.py" build ${PKG} && PASSES=$((${PASSES}+1)) || FAILS=$((${FAILS}+1))
    done
    [ ${PASSES} -gt 0 ] && [ ${FAILS} -eq 0 ]
}

test_analyze_test_systems () {
    local PASSES FAILS PKG
    PASSES=0
    FAILS=0
    for PKG in ${TEST_PACKAGES}
    do
        PREFIX="${PKG%%.*}"
        if test "${PREFIX}" = "stub"
        then
            python${PY_VER} "${CORE_DIR}/prj/app/prj.py" analyze ${PKG} && PASSES=$((${PASSES}+1)) || FAILS=$((${FAILS}+1))
        fi
    done
    [ ${PASSES} -gt 0 ] && [ ${FAILS} -eq 0 ]
}

for PY_VER in ${PY_VERSIONS}
do
    run_test python${PY_VER} x.py test licenses
    run_test python${PY_VER} x.py test provenance
    run_test python${PY_VER} x.py test style
    run_test python${PY_VER} x.py test x
    run_test python${PY_VER} x.py test pystache
    run_test python${PY_VER} x.py test prj
    run_test python${PY_VER} x.py build packages
    run_test test_build_test_systems
    run_test test_analyze_test_systems
    if test -e pm/tasks/CbC0b6-message_queue_unit_tests; then run_test python${PY_VER} x.py test units; fi
    run_test python${PY_VER} x.py test systems
    run_test python${PY_VER} x.py build prj
    run_test eval "TMPDIR=/tmp xvfb-run -a -s '-screen 0 640x480x16' python${PY_VER} x.py build docs"
    run_test python${PY_VER} x.py build partials
    run_test python${PY_VER} x.py build release
    run_test python${PY_VER} x.py test release
done

# make the script exit with a non-zero exit code (indicating a test failure) if the number of failed tests is greater than 0
echo ""
if [ ${FAILED_TESTS} -eq 0 ]; then
    echo "SUMMARY: all tests passed"
else
    echo "SUMMARY: ${FAILED_TESTS} test categories failed"
    exit 1
fi
