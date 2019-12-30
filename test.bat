@ECHO OFF

REM
REM eChronos Real-Time Operating System
REM Copyright (c) 2017, Commonwealth Scientific and Industrial Research
REM Organisation (CSIRO) ABN 41 687 119 230.
REM
REM All rights reserved. CSIRO is willing to grant you a licence to the eChronos
REM real-time operating system under the terms of the CSIRO_BSD_MIT license. See
REM the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
REM
REM @TAG(CSIRO_BSD_MIT)
REM

set PATH=C:\splint-3.1.2\bin;%PATH%
IF NOT DEFINED PYTHON set PYTHON=py -3
IF NOT DEFINED COREDIR set COREDIR=.

ECHO ON

FOR %%P in ("C:\cygwin64\usr\bin;C:\cygwin64\bin" "C:\msys64\usr\bin;C:\msys64\bin") DO (CALL :runtestswithpath %%P)
GOTO :eof


:runtestswithpath
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

SET OLDPATH=%PATH%
SET PATH=%~1;%PATH%
CALL :runtests
SET TESTRESULT=%ERRORLEVEL%
SET PATH=%OLDPATH%

EXIT /B %TESTRESULT%


:runtests
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%

%PYTHON% --version
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
git --version
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
gcc --version
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
gdb --version
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
REM %PYTHON% x.py test units
REM @IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
REM system tests on msys only work if invoked directly from an msys shell (instead of a batch file)
ECHO %PATH% | FINDSTR msys64 > NUL || %PYTHON% x.py test systems
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

%PYTHON% %COREDIR%\prj\app\prj.py gen stub.acamar
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py gen stub.acrux
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py gen stub.gatria
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py gen stub.kochab
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py gen stub.kraz
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py gen stub.phact
@IF %ERRORLEVEL% NEQ 0 EXIT /B %ERRORLEVEL%
%PYTHON% %COREDIR%\prj\app\prj.py gen stub.rigel
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

GOTO :eof
