@echo off
REM
REM	Name: mkpreinstallAPI.bat
REM
REM	Description:
REM	     This batch file creates preinstall directory and copies selected binaries 
REM	     there.
REM
REM	History:
REM	   12-Jan-2005 (drivi01)
REM		Created.
REM	   21-Jan-2005 (drivi01)
REM	        Removed error check from directory creation.
REM	   12-May-2006 (drivi01)
REM		Done was stripped out during integration by piccolo
REM		by mistake.


echo.
echo Copying binaries....
echo.
if not exist %ING_ROOT%\preinstall mkdir %ING_ROOT%\preinstall  

cp -f  %II_SYSTEM%\ingres\lib\iilibadf.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibadfdata.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibcompat.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibcompatdata.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibembed.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibembeddata.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibq.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibqdata.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibframe.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibframedata.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibgcf.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibgcfdata.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibapi.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibcuf.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\lib\iilibutil.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\bin\iigetver.dll  %ING_ROOT%\preinstall   
if ERRORLEVEL 1 goto EXIT
cp -f  %II_SYSTEM%\ingres\files\charsets\inst1252\desc.chx  %ING_ROOT%\preinstall\inst1252.chx   
if ERRORLEVEL 1 goto EXIT
goto DONE


:EXIT
echo ERROR: There was error copying one or more binaries to preinstall directory.
endlocal

:DONE