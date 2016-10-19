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

CORE_DIR="$(dirname "${0}")"
PY_VERSIONS="$(for V in 3.3 3.4 3.5 3.6 3.7 3.8 3.9; do ! python${V} --version > /dev/null 2>&1 || echo -n "${V} "; done)"

while getopts c:p: OPT
do
        case ${OPT} in
        c)  CORE_DIR="${OPTARG}";;
        p)  PY_VERSIONS="${OPTARG}";;
        \?) echo ${USAGE}; exit 1;;
        esac
done
shift `expr ${OPTIND} - 1`

TEST_PACKAGES="$(find "${CORE_DIR}/packages" -name '*.prx' -and -not -name '*config-only*.prx' -and -not -name gatria-system-simple.prx | sed -r 's/.*packages\///; s/.prx//; s/\//./g')"

export PATH="${PATH}:${HOME}/local/bin"
set +eu
. ./setenv
set -eu

for PY_VER in ${PY_VERSIONS}
do
    set -x
    python${PY_VER} x.py test licenses
    python${PY_VER} x.py test provenance
    python${PY_VER} x.py test style
    python${PY_VER} x.py test x
    python${PY_VER} x.py test pystache
    python${PY_VER} x.py test prj
    python${PY_VER} x.py build packages
    for PKG in ${TEST_PACKAGES}
    do
        python${PY_VER} "${CORE_DIR}/prj/app/prj.py" build ${PKG}
    done
    for PKG in ${TEST_PACKAGES}
    do
        PREFIX="${PKG%%.*}"
        if test "${PREFIX}" = "stub"
        then
            python${PY_VER} "${CORE_DIR}/prj/app/prj.py" analyze ${PKG}
        fi
    done
    if test -e pm/tasks/CbC0b6-message_queue_unit_tests; then python${PY_VER} x.py test units; fi
    python${PY_VER} x.py test systems
    python${PY_VER} x.py build prj
    TMPDIR="/tmp" xvfb-run -a -s "-screen 0 640x480x16" python${PY_VER} x.py build docs
    python${PY_VER} x.py build partials
    python${PY_VER} x.py build release
    python${PY_VER} x.py test release
    set +x
done
