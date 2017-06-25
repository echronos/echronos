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

C:\cygwin64\setup-x86_64.exe -q -P gdb

REM If the master branch does not exist locally, create it because some tests rely on it
git rev-parse --verify master >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    git config remote.origin.fetch "+refs/heads/*:refs/remotes/origin/*" || EXIT /B %ERRORLEVEL%
    git fetch --depth=1 origin master || EXIT /B %ERRORLEVEL%
    git branch --track master origin/master || EXIT /B %ERRORLEVEL%
)

REM Install pylint because `x.py test style` depends on it
%PYTHON% -m pip install --user --upgrade "pylint<2"

IF NOT EXIST C:\splint-3.1.2 (
    powershell.exe -nologo -noprofile -command "& { Invoke-WebRequest 'https://raw.githubusercontent.com/wiki/echronos/echronos/splint-3.1.2.win32.zip' -OutFile 'splint-3.1.2.win32.zip'; Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('splint-3.1.2.win32.zip', 'C:\'); }" || EXIT /B %ERRORLEVEL%
    del splint-3.1.2.win32.zip || EXIT /B %ERRORLEVEL%
) ELSE (
    EXIT /B 0
)

REM WinAVR cannot be installed from the default installer.
REM Therefore, unpack an archive of a local installation instead.
C: || EXIT /B %ERRORLEVEL%
cd \ || EXIT /B %ERRORLEVEL%
C:\cygwin64\bin\wget -q 'https://github.com/echronos/echronos/wiki/winavr.tar.xz' || EXIT /B %ERRORLEVEL%
C:\cygwin64\bin\xz --decompress winavr.tar.xz || EXIT /B %ERRORLEVEL%
C:\cygwin64\bin\tar xaf winavr.tar || EXIT /B %ERRORLEVEL%
cd %APPVEYOR_BUILD_FOLDER% || EXIT /B %ERRORLEVEL%
