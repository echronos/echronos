@ECHO OFF

REM
REM eChronos Real-Time Operating System
REM Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
REM
REM This program is free software: you can redistribute it and/or modify
REM it under the terms of the GNU Affero General Public License as published by
REM the Free Software Foundation, version 3, provided that these additional
REM terms apply under section 7:
REM
REM   No right, title or interest in or to any trade mark, service mark, logo or
REM   trade name of of National ICT Australia Limited, ABN 62 102 206 173
REM   ("NICTA") or its licensors is granted. Modified versions of the Program
REM   must be plainly marked as such, and must not be distributed using
REM   "eChronos" as a trade mark or product name, or misrepresented as being the
REM   original Program.
REM
REM This program is distributed in the hope that it will be useful,
REM but WITHOUT ANY WARRANTY; without even the implied warranty of
REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM GNU Affero General Public License for more details.
REM
REM You should have received a copy of the GNU Affero General Public License
REM along with this program.  If not, see <http://www.gnu.org/licenses/>.
REM
REM @TAG(NICTA_AGPL)
REM

set PATH=C:\splint-3.1.1\bin;%PATH%
IF NOT DEFINED PYTHON set PYTHON="py -3"
IF NOT DEFINED COREDIR set COREDIR=.

ECHO ON

%PYTHON% --version
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
git --version
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
gcc --version
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
splint --help
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

%PYTHON% x.py test licenses
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% x.py test provenance
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% x.py test x
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% x.py test style
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% x.py test pystache
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% x.py test prj
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
REM Unit tests are known to not work on Windows
%PYTHON% x.py test systems
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% x.py build prj
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% x.py build packages
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

%PYTHON% %COREDIR%\prj\app\prj.py build stub.acamar
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py build stub.acrux
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py build stub.gatria
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py build stub.kochab
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py build stub.kraz
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py build stub.phact
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py build stub.rigel
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

%PYTHON% %COREDIR%\prj\app\prj.py analyze stub.acamar
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py analyze stub.acrux
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py analyze stub.gatria
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py analyze stub.kochab
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py analyze stub.kraz
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py analyze stub.phact
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py analyze stub.rigel
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

%PYTHON% x.py build partials
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% x.py build docs
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% x.py build release
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
