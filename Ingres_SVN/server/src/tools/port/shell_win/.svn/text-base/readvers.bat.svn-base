REM @echo off
REM 
REM  readvers.bat
REM 
REM  History:
REM 	13-jul-95 (tutto01)
REM	    Created.
REM	13-dec-1999 (somsa01)
REM	    Set all variables that are in the VERS file.
REM 	14-Jun-2004 (drivi01)
REM	    Modified the script to work with new Unix Tools.


REM Locate the VERS file
REM
set VERSDIR=%ING_SRC%\tools\port\conf
if exist %VERSDIR%\VERS goto VERSFILE

echo.
echo There is no VERS file!!!!!!
echo.
goto EXIT



:VERSFILE

REM Transform the VERS file into an executable batch file.
REM Perhaps the VERS file itself should just be a batch file.
REM Do this for now so that VERS files looks just like other systems.
REM
%AWK_CMD% "{print \"set \" $1 $2 $3}" %VERSDIR%\VERS > %VERSDIR%\VERS.bat
call %VERSDIR%\VERS.bat


:EXIT
