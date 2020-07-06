@echo off
REM
REM  Name: buildenv.bat
REM
REM  Description:
REM	Similar to UNIX's buildenv, this batch file sets up a build
REM	environment. It should be run within a Command Prompt whereby the
REM	compiler environment is set up beforehand, as well as the UNIX tools.
REM
REM  History:
REM	08-jul-2004 (somsa01)
REM	    Created.
REM	02-Aug-2004 (kodse01)
REM	    Modified to support Cygwin.
REM	02-Sep-2004 (somsa01)
REM	    More updates for Open Source.
REM	23-sep-2004 (somsa01)
REM	    Added vdba into the PATH. Also, added making of VERS file.
REM	06-oct-2004 (somsa01)
REM	    ing_tools is now just "tools", located in ING_ROOT. Also, added
REM	    new variables used by buildrel.
REM	14-oct-2004 (somsa01)
REM	    Updated Xerces look and feel, to match binary drop.
REM	19-oct-2004 (somsa01)
REM	    Make sure we set DOUBLEBYTE to ON.
REM	29-oct-2004 (somsa01)
REM	    Updated name of dotnet directory.
REM	29-oct-2004 (somsa01)
REM	    Place the MKSLOC path in front of the existing path.
REM	31-oct-2004 (somsa01)
REM	    Replace VISUALC setting in Jamdefs.int_w32 if it is not the
REM	    expected location.
REM     20-apr-2005 (loera01) SIR 114358
REM         Added check for KRB5HDR.
REM	26-May-2005 (drivi01)
REM	     Slightly modified creation of ING_TOOLS directories.
REM	03-Jan-2007 (drivi01)
REM	     Added VS2005 and VS2005SDK variables to point to Visual Studio 2005 and SDK.
REM	24-Jan-2007 (drivi01)
REM	     Added another variable to be set called MT that points to MS Manifest Tool.
REM     04-Jan-2008 (clach04)
REM          Added check for mt.exe (%MT%) existence, if it is not there unset MT
REM          This is needed for build systems that do not have Visual Studio 2005
REM          installed (e.g. do not want a Vista or .NET Data Provider built).
REM	22-Feb-2008 (smeke01)
REM	     Don't prompt for II_SYSTEM, DEBUG, XERCES or KRB5HDR if these are 
REM	     already set in the environment. Handle MT better - put out an 
REM	     informative message if mt.exe not found. Allow caller to specify 
REM	     Cygwin use, so that bldenv doesn't have to check for it.
REM      7-Mar-2008 (hanal04) Bug 119979
REM          Double quote if exist of %MT%, otherwise we get a failure
REM          when the executable does exist.
REM	12-Mar-2008 (drivi01)
REM	     Add MSREDIST and CAZIPXP variables to the script.
REM     14-Mar-2008 (hanal04)
REM          When an invalid directory is entered for MSREDIST and CAZIPXP
REM          we go into a loop reporting that the entered directory does not
REM          exist. The problem also exists for other variables.
REM

:TRUNK_LOOP
if "%ING_ROOT%"=="" SET /P ING_ROOT=Root Ingres location (default is %CD:\src\tools\port\jam=%): 
if "%ING_ROOT%"=="" SET ING_ROOT=%CD:\src\tools\port\jam=%
if not exist %ING_ROOT% echo Directory %ING_ROOT% does not exist& SET ING_ROOT=& goto TRUNK_LOOP

SET ING_SRC=%ING_ROOT%\src
if not exist %ING_SRC%\bin mkdir %ING_SRC%\bin
if not exist %ING_SRC%\lib mkdir %ING_SRC%\lib

if "%II_SYSTEM%"=="" SET /P II_SYSTEM=Root build location (default is %ING_ROOT%\build): 
if "%II_SYSTEM%"=="" SET II_SYSTEM=%ING_ROOT%\build
if not exist %II_SYSTEM% mkdir %II_SYSTEM%
SET ING_BUILD=%II_SYSTEM%\ingres

:DEBUG_SET
if "%DEBUG%"=="" SET /P DEBUG=DEBUG build status (ON or OFF):
if "%DEBUG%"=="OFF" SET DEBUG=& goto ASK_PSDK
if "%DEBUG%"=="ON" goto ASK_PSDK
echo Invalid setting for DEBUG.
goto DEBUG_SET

:ASK_PSDK
if "%USE_PSDK%"=="" SET /P USE_PSDK=Do you want to use Platform SDK? (Yes or No) (default is No):
if "%USE_PSDK%"=="" SET USE_PSDK=No
if "%USE_PSDK%"=="Yes" goto PSDK_LOOP
if "%USE_PSDK%"=="No" goto MSVC_LOOP

:PSDK_LOOP
if "%PSDK%"=="" SET INCLUDE=
if "%PSDK%"=="" SET LIB=
if "%PSDK%"=="" SET /P PSDK=Root location of Microsoft Platform SDK (default is C:\Program Files\Microsoft Platform SDK):
if "%PSDK%"=="" SET PSDK=C:\Program Files\Microsoft Platform SDK
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" SET FLAG=/X64
if "%PROCESSOR_ARCHITECTURE%"=="IA64" SET FLAG=/SRV64
if "%DEBUG%"=="" call "%PSDK%\SetEnv.bat" %FLAG% /RETAIL& goto OTHERS
if "%DEBUG%"=="ON" call "%PSDK%\SetEnv.bat" %FLAG% /DEBUG& goto OTHERS

:MSVC_LOOP
if "%MSVCDir%"=="" SET INCLUDE=
if "%MSVCDir%"=="" SET LIB=
if "%MSVCDir%"=="" SET /P MSVCLoc=Root location of the Microsoft Visual Studio .NET compiler (default is C:\Program Files\Microsoft Visual Studio .NET 2003): 
if "%MSVCLoc%"=="" SET MSVCLoc=C:\Program Files\Microsoft Visual Studio .NET 2003
if "%MSVCDir%"=="" call "%MSVCLoc%\Common7\Tools\vsvars32.bat"& set MSVCLoc=

:VS2005_LOOP
if "%VS2005%"=="" SET /P VS2005=Root location of the Microsoft Visual Studio 2005 compiler (default is C:\Program Files\Microsoft Visual Studio 8):
if "%VS2005%"=="" SET VS2005=C:\Program Files\Microsoft Visual Studio 8
if "%MT%"=="" SET MT=%VS2005%\SDK\v2.0\bin\mt.exe
if exist "%MT%" goto VS2005SDK_LOOP
echo No mt.exe found - unsetting MT environment variable.& SET MT=

:VS2005SDK_LOOP
if "%VS2005SDK%"=="" SET /P VS2005SDK=Root location of the Visual Studio 2005 SDK (default is C:\Program Files\Visual Studio 2005 SDK):
if "%VS2005SDK%"=="" SET VS2005SDK=C:\Program Files\Visual Studio 2005 SDK

:OTHERS
set MSVCLoc=
SET JAM_TOOLSET=VISUALC
SET ING_SRCRULES=%ING_SRC%\tools\port\jam\Jamrules

:XERCES_LOOP
if "%XERCESCROOT%"=="" SET /P XERCESCROOT=Root location of Xerces source (default is %ING_ROOT%\xerces-c_2_5_0-windows_nt-msvc_60):
if "%XERCESCROOT%"=="" SET XERCESCROOT=%ING_ROOT%\xerces-c_2_5_0-windows_nt-msvc_60
if not exist %XERCESCROOT% echo %XERCESCROOT% does not exist& SET XERCESCROOT=& goto XERCES_LOOP
SET XERCESLOC=%XERCESCROOT%\lib

:KERBEROS_LOOP
if "%KRB5HDR%"=="" SET /P KRB5HDR=Location of Kerberos header files (default is %ING_ROOT%\Kerberos5):
if "%KRB5HDR%"=="" SET KRB5HDR=%ING_ROOT%\Kerberos5
if not exist %KRB5HDR% echo %KRB5HDR% does not exist& SET KRB5HDR=& goto KERBEROS_LOOP

:MSREDIST_LOOP
if "%MSREDIST%"=="" SET /P MSREDIST=Location of Microsoft redistributables (default is %ING_ROOT%\partners\microsoft\7_1):
if "%MSREDIST%"=="" SET MSREDIST=%ING_ROOT%\partners\microsoft\7_1
if not exist %MSREDIST% echo %MSREDIST% does not exist& SET MSREDIST=& goto MSREDIST_LOOP

:CAZIPXP
if "%CAZIPXP%"=="" SET /P CAZIPXP=Location of cazipxp (default is %ING_ROOT%\partners\ca):
if "%CAZIPXP%"=="" SET CAZIPXP=%ING_ROOT%\partners\ca
if not exist %CAZIPXP% echo %CAZIPXP% does not exist& SET CAZIPXP=& goto CAZIPXP

:JDK_LOOP
REM if "%II_JDK_HOME%"=="" SET /P II_JDK_HOME=Root location of Java SDK: 
REM if "%II_JDK_HOME%"=="" goto JDK_LOOP
REM SET II_JRE_HOME=%II_JDK_HOME%\jre

REM Check if Cygwin is installed
if "%USE_CYGWIN%"=="TRUE" goto SHELL_SET
SET USE_CYGWIN=
which cygpath.exe > c:\nul 2>&1
if errorlevel 1 goto CHK_UNXLOC
SET USE_CYGWIN=TRUE

:CHK_UNXLOC
which ls.exe > C:\nul 2>&1
if errorlevel 1 goto GET_UNXLOC
printf "SET MKSLOC=" > settmp.bat
if "%USE_CYGWIN%"=="" which ls.exe | sed "s:/:\\:g" | sed "s:\\ls.exe::g" >> settmp.bat& goto CALLSETTMP
which ls.exe | cygpath -am -f- | sed "s:/:\\:g" | sed "s:\\ls.exe::g" >> settmp.bat

:CALLSETTMP
call settmp.bat
rm settmp.bat
goto SHELL_SET

:GET_UNXLOC
SET /P MKSLOC=Root location of the UNIX tools: 
if not exist %MKSLOC%\ls.exe echo This location does not contain the appropriate set of UNIX tools.& goto GET_UNXLOC
SET PATH=%MKSLOC%;%PATH%

:SHELL_SET
if exist %MKSLOC%\sh.exe SET SHELL=%MKSLOC%\sh.exe& goto AWK_SET
if exist %MKSLOC%\zsh.exe SET SHELL=%MKSLOC%\zsh.exe

:AWK_SET
if exist %MKSLOC%\gawk.exe SET AWK_CMD=gawk& goto MISC
if exist %MKSLOC%\awk.exe SET AWK_CMD=awk

:MISC
SET DD_RSERVERS=%II_SYSTEM%\ingres\rep
SET INCLUDE=%II_SYSTEM%\ingres\files;%include%;%II_JDK_HOME%\include;%II_JDK_HOME%\include\win32
SET LIB=%II_SYSTEM%\ingres\lib;%lib%;%II_JDK_HOME%\lib
SET HOSTNAME=%COMPUTERNAME%
SET ING_DOC=%ING_SRC%\tools\techpub\pdf
SET DBCS=DOUBLEBYTE
SET DOUBLEBYTE=ON
SET II_RELEASE_DIR=%ING_ROOT%\release
SET II_CDIMAGE_DIR=%ING_ROOT%\cdimage
SET II_MM_DIR=%ING_ROOT%\mergemodules
if not exist %II_RELEASE_DIR% mkdir %II_RELEASE_DIR%
if not exist %II_CDIMAGE_DIR% mkdir %II_CDIMAGE_DIR%
if not exist %II_MM_DIR% mkdir %II_MM_DIR%

REM
REM Due to a hack in JAM to properly allow it to support embedded spaces
REM in the install path of the compiler, replace the entry in Jamdefs.int_w32
REM with the proper one.
REM
if "%MSVCDIR%"=="C:\Program Files\Microsoft Visual Studio .NET 2003\VC7" goto MISC2
if not exist Jamdefs.int_w32.orig cp Jamdefs.int_w32 Jamdefs.int_w32.orig
chmod 777 Jamdefs.int_w32
echo SET MSVCDIRLOC=%MSVCDIR%| sed -e s/\\/\\\//g >> settmp.bat
call settmp.bat
rm settmp.bat
sed -e "s/C:\/Program Files\/Microsoft Visual Studio .NET 2003\/VC7/%MSVCDIRLOC%/g" Jamdefs.int_w32.orig > Jamdefs.int_w32
chmod 444 Jamdefs.int_w32
SET MSVCDIRLOC=

:MISC2
SET CPU=%PROCESSOR_ARCHITECTURE%
if "%CPU%"=="x86" SET CPU=i386
if "%CPU%"=="i386" SET config_string=int_w32
if "%CPU%"=="IA64" SET config_string=i64_win
if "%CPU%"=="AMD64" SET config_string=a64_win

if exist %ING_SRC%\tools\port\conf\VERS goto VERS_EXIST
cp %ING_SRC%\tools\port\conf\VERS.%config_string% %ING_SRC%\tools\port\conf\VERS

:VERS_EXIST
SET TESTTOOLS=%ING_ROOT%\tools
if not exist %TESTTOOLS% mkdir %TESTTOOLS%\bin& mkdir %TESTTOOLS%\files& mkdir %TESTTOOLS%\man1
if not exist %TESTTOOLS%\bin mkdir %TESTTOOLS%\bin 
if not exist %TESTTOOLS%\files mkdir %TESTTOOLS%\files 
if not exist %TESTTOOLS%\man1 mkdir %TESTTOOLS%\man1

SET PATH=%ING_SRC%\bin;%II_SYSTEM%\ingres\bin;%II_SYSTEM%\ingres\utility;%II_SYSTEM%\ingres\vdba;%PATH%

title Ingres Build Window - ING_ROOT=%ING_ROOT%

REM
REM This stuff is for SEP
REM

set ING_TST=%ING_ROOT%\tst
set TECHDOC=
set ING_REL=2.5
set TST_OUTPUT=%II_SYSTEM%\output
set ING_TOOLS=%ING_ROOT%\tools
set TERM_INGRES=ibmpc
set TST_ROOT_OUTPUT=%TST_OUTPUT%

REM This variable should be set to the ingres user's password
SET QASETUSER_INGRESPW=

REM This variable controls how long SEP delays before diff'ing canons
SET SEP_DIFF_SLEEP=10

REM
REM The rest of this file should not be modified!!
REM

if "%ING_TOOLS%"=="" goto SETTOOLS2
set TOOLS_DIR=%ING_TOOLS%
set TST_SEP=%ING_TOOLS%\files
set PEDITOR=%ING_TOOLS%\bin\peditor.exe
set TST_TOOLS=%ING_TOOLS%\bin
goto CONTINUE

:SETTOOLS2
if "%ING_SRC%"=="" goto CONTINUE
set TST_SEP=%ING_SRC%\testtool\sep\files
set PEDITOR=%ING_SRC%\bin\peditor.exe

:CONTINUE
if not "%ING_TST%"=="" set SEPPARAM_SYSTEM=%OS%
if "%ING_EDIT%"=="" set ING_EDIT=%WINDIR%\system32\notepad.exe

if not "%TST_OUTPUT%"=="" goto CONTINUE2
if not "%ING_TST%"=="" set TST_OUTPUT=%ING_TST%\output

:CONTINUE2
if "%ING_TST%"=="" goto CONTINUE3
set TST_TESTENV=%ING_TST%
set TST_TESTOOLS=%ING_TST%\testtool
set TST_CFG=%ING_TST%\suites\handoffqa
set TST_SHELL=%ING_TST%\suites\bat
set TST_LISTEXEC=%ING_TST%\suites\handoffqa
set TST_DCL=%TST_SHELL%
set TST_INIT=%ING_TST%\basis\init
set path=%path%;%TST_SHELL%

:CONTINUE3
REM Regression directories
if "%ING_TST%"=="" goto CONTINUE4
set TST_FE=%ING_TST%\fe
set TST_BE=%ING_TST%\be
set TST_ACCESS=%ING_TST%\be\access
set TST_ACCNTL=%ING_TST%\be\accntl
set TST_DATATYPES=%ING_TST%\be\datatypes
set TST_QRYPROC=%ING_TST%\be\qryproc
set TST_GCF=%ING_TST%\gcf
set TST_NETTEST=%TST_GCF%\gcc

REM Stress Directories
set TST_STRESS=%ING_TST%\stress
set TST_POOL=%TST_STRESS%\pool
set TST_PROPHECY=%TST_STRESS%\prophecy
set TST_MAYO=%TST_STRESS%\mayo
set TST_FIDO=%TST_STRESS%\fido
set TST_REGRESSION=%TST_STRESS%\regression
set TST_GATEWAY=%TST_STRESS%\gateway
set TST_CALLT=%TST_STRESS%\callt
set TST_BUGS=%TST_STRESS%\bugs
set TST_ACTEST=%TST_STRESS%\actest
set TST_ITB=%TST_STRESS%\itb

:CONTINUE4
REM These are also needed.
set ING_BUILD=%II_SYSTEM%\ingres
set II_CONFIG=%II_SYSTEM%\ingres\files

set II_ABF_RUNOPT=nondynamic
if not exist %II_CONFIG%\termcap goto ELSE
set II_TERMCAP_FILE=%II_CONFIG%\termcap
goto CONTINUE5

:ELSE
set II_TERMCAP_FILE=

:CONTINUE5
REM if not "%ING_SRC%"=="" set path=%path%;%ING_SRC%\bin
if not "%ING_TOOLS%"=="" set path=%path%;%ING_TOOLS%\bin

REM Replicator
set TST_REP=%ING_TST%
set REP_TST=%TST_REP%

set lib=%lib%;%II_SYSTEM%\ingres\lib

REM Show what we have...
echo.
which cazipxp.exe > C:\nul 2>&1
if errorlevel 1 echo Warning: Please place cazipxp.exe in your path, or obtain it from the CA Open& echo          Source website.
if not exist %ING_SRC%\common\dotnet_win\specials\keypair.snk echo Warning: Please follow the README instructions to create the strong name key& echo          file, required in building the .NET Data Provider, located in& echo          %ING_SRC%\common\dotnet_win\specials.
which IsCmdBld.exe > C:\nul 2>&1
if errorlevel 1 echo Warning: The InstallShield Developer command-line packaging tool (IsCmdBld.exe)& echo          does not seem to be in the path.
echo.
echo 	Welcome to %COMPUTERNAME%/%OS% Ingres Build

echo.
echo II_SYSTEM    = %II_SYSTEM%
if not "%ING_SRC%"=="" echo ING_SRC      = %ING_SRC%
if not "%ING_TST%"=="" echo ING_TST      = %ING_TST%
if not "%ING_TOOLS%"=="" echo ING_TOOLS    = %ING_TOOLS%
echo.
