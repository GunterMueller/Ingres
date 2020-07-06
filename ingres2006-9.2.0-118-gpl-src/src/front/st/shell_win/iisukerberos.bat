@echo off   
setlocal
REM
@REM  Copyright (c) 2007 Ingres Corporation
REM
REM  Name: iisukerberos -- setup script for Ingres Kerberos Driver
REM  Usage:
REM       iisukerberos
REM
REM  Description:
REM       This script sets up the Ingres Kerberos Driver.
REM
REM History:
REM      30-Jul-2007 (Ralph Loen) SIR 118898
REM          Created.
REM      12-Aug-2007 (Ralph Loen) SIR 118898
REM          Removed iigenres, since Kerberos info was moved to all.crs.
REM          Prompting for domain is no longer necessary, since the
REM          iinethost utility should enter the correct host name into
REM          config.dat.  Iisukerberos now prompts for three main
REM          configurations.
REM      15-Aug-2007 (Ralph Loen) SIR 118898
REM          Wrong label referenced for removal of client configuration.
REM          Should be RMCLIENT1, not RMCLIENT2.
REM

if "%INSTALL_DEBUG%" == "DEBUG" echo on

set CONFIG_HOST=%COMPUTERNAME%
call ipset IINETHOST iinethost

echo This procedure will configure Kerberos for the Ingres installation
echo located at:
echo.
echo             %II_SYSTEM%
echo.
echo to run on local host:
echo.
echo             %IINETHOST%
echo.
:PROMPT0
set /p YorN=Do you want to continue this setup procedure? (y/n) [y]:
if "%YorN%" == "" goto MAINDISP
if "%YorN%" == "y" goto MAINDISP
if "%YorN%" == "Y" goto MAINDISP
if "%YorN%" == "n" goto END
if "%YorN%" == "N" goto END
echo Please enter 'y' or 'n'.
pause
goto PROMPT0
:MAINDISP
set OPTION=0
echo.
echo Basic Kerberos Configuration Options
echo.
echo 1.  Client Kerberos configuration
echo 2.  Name Server Kerberos configuration
echo 3.  Server-level Kerberos configuration
echo 0.  Exit
echo.
set /p OPTION=select from [0 - 3] above [0]:
if "%OPTION%" == "" goto END
if "%OPTION%" == "0" goto END
if "%OPTION%" == "1" goto CLIENT
if "%OPTION%" == "2" goto NS
if "%OPTION%" == "3" goto SERVER
echo Entry must be in range 0 to 3.
pause
goto MAINDISP
:CLIENT
echo Client configuration enables this installation to use Kerberos to authenticate
echo against a remote installation that has been configured to use Kerberos for
authentication.  This is the mimimum level of Kerberos authentication.
:PROMPT1
echo.
echo Select 'a' to add client-level Kerberos authentication,
set /p AorR='r' to remove client-level Kerberos authentication:
if "%AorR%" == "a" goto ADDCLIENT1
if "%AorR%" == "A" goto ADDCLIENT1
if "%AorR%" == "r" goto RMCLIENT1
if "%AorR%" == "R" goto RMCLIENT1
echo Entry must be 'a' or 'r'
pause
goto PROMPT1
:ADDCLIENT1
iisetres ii.%CONFIG_HOST%.gcf.mechanisms "kerberos"
echo.
echo Client Kerberos configuration added.
echo.
echo You must add the "authentication_mechanism" attribute in netutil or
echo VDBA for each remote node you wish to authenticate using Kerberos.  The
echo "authentication_mechanism" attribute must be set to "kerberos".  There
echo is no need to define users or passwords for vnodes using Kerberos
echo authentication.
echo.
pause
goto MAINDISP
:RMCLIENT1
iisetres ii.%CONFIG_HOST%.gcf.mechanisms ""
echo.
echo Client Kerberos configuration removed.
pause
goto MAINDISP
:NS
echo Name Server Kerberos authentication allows the local Name Server to
echo authenticate using Kerberos.
echo.
:PROMPT2
echo.
echo Select 'a' to add Kerberos authentication for the local Name Server,
set /p AorR='r' to remove Name Server Kerberos authentication:
if "%AorR%" == "a" goto ADDCLIENT2
if "%AorR%" == "A" goto ADDCLIENT2
if "%AorR%" == "r" goto RMCLIENT2
if "%AorR%" == "R" goto RMCLIENT2
echo Entry must be 'a' or 'r'
pause
goto PROMPT2
:ADDCLIENT2
iisetres ii.%CONFIG_HOST%.gcn.mechanisms "kerberos"
iisetres ii.%CONFIG_HOST%.gcn.remote_mechanism "kerberos"
echo.
echo Kerberos authentication has been added to the Name Server 
echo on %IINETHOST%.
echo.
pause
goto MAINDISP
:RMCLIENT2
iisetres ii.%CONFIG_HOST%.gcn.mechanisms ""
iisetres ii.%CONFIG_HOST%.gcn.remote_mechanism "none"
echo.
echo Kerberos authentication has been removed from the Name Server 
echo on %IINETHOST%.
pause
goto MAINDISP
:SERVER
echo.
echo Server-level Kerberos authentication forces all local servers, such as the
echo DBMS Server and the Comm Server, to authenticate against the Name Server
echo using Kerberos.
echo.
:PROMPT3
echo.
echo Select 'a' to add Kerberos authentication for the local Name Server,
set /p AorR='r' to remove Name Server Kerberos authentication:
if "%AorR%" == "a" goto ADDCLIENT3
if "%AorR%" == "A" goto ADDCLIENT3
if "%AorR%" == "r" goto RMCLIENT3
if "%AorR%" == "R" goto RMCLIENT3
echo Entry must be 'a' or 'r'
pause
goto PROMPT3
:ADDCLIENT3
iisetres ii.%CONFIG_HOST%.gcf.user_mechanism "kerberos"
echo.
echo Server-level authentication has been added.
echo.
pause
goto MAINDISP
:RMCLIENT3
iisetres ii.%CONFIG_HOST%.gcf.user_mechanism "default"
echo.
echo Server-level authentication has been removed.
goto MAINDISP
pause
echo.
:END
echo Kerberos configuration complete.  Be sure the GSS API library, 
echo GSSAPI32.DLL, resides in your execution path, as defined by the 
echo environment variable PATH.
echo.
echo You may use the cbf utility to fine-tune Kerberos security options.
endlocal
