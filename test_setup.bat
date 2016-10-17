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

REM If the master branch does not exist locally, create it because some tests rely on it
git rev-parse --verify master >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    git config remote.origin.fetch "+refs/heads/*:refs/remotes/origin/*" || EXIT /B %ERRORLEVEL%
    git fetch --depth=1 origin master || EXIT /B %ERRORLEVEL%
    git branch --track master origin/master || EXIT /B %ERRORLEVEL%
)

REM Install pylint because `x.py test style` depends on it
%PYTHON% -m pip install --user --upgrade pylint

IF NOT EXIST C:\splint-3.1.1 (
    powershell.exe -nologo -noprofile -command "& { Invoke-WebRequest 'http://www.splint.org/downloads/binaries/splint-3.1.1.win32.zip' -OutFile 'splint-3.1.1.win32.zip'; Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory('splint-3.1.1.win32.zip', 'C:\'); }" || EXIT /B %ERRORLEVEL%
    del splint-3.1.1.win32.zip || EXIT /B %ERRORLEVEL%
) ELSE (
    EXIT /B 0
)
