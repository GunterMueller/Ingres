@echo off
setlocal
REM
REM  Name: buildrel.bat
REM
REM  Description:
REM	Similiar to UNIX's buildrel, it basically builds a cdimage from
REM	scratch.
REM
REM  History:
REM	16-jun-2001 (somsa01)
REM	    Created.
REM	05-sep-2001 (penga03)
REM	    Copy ingwrap.exe, and run msiupd.exe. 
REM	02-oct-2001 (somsa01)
REM	    Updated for InstallShield Developer 7.0 .
REM	28-oct-2001 (somsa01)
REM	    The .ism files need to be writeable for the command-line
REM	    build process to succeed.
REM	08-nov-2001 (somsa01)
REM	    Made changes to follow the new CA branding.
REM	08-nov-2001 (somsa01)
REM	    Removed building IngresIIDBMSNetEsqlCobol.ism.
REM	09-nov-2001 (somsa01)
REM	    Added 64-bit Windows builds.
REM	08-dec-2001 (somsa01)
REM	    Corrected name of Readme.
REM	15-feb-2002 (penga03)
REM	    Made changes to build Ingres SDK.
REM	21-mar-2002 (penga03)
REM	    Copy Apache HTTP Server and DBupdate for Ingres SDK.
REM	27-mar-2002 (somsa01)
REM	    Adjusted the copying of the final MSI to handle the special
REM	    trademark symbol in the name.
REM	28-may-2002 (penga03)
REM	    Made changes to build DoubleByte Ingres or SingleByte Ingres 
REM	    depending on whether or not DOUBLEBYTE is set to ON.
REM	12-feb-2003 (penga03)
REM	    Unzip ca licensing files from command line. And rename 
REM	    silent.bat to silent.exe.
REM	24-feb-2003 (penga03)
REM	    Corrected the syntax error while renaming silent.bat to 
REM	    silent.exe.
REM	11-mar-2003 (somsa01)
REM	    For now, use the 32-bit version of ODBC in the 64-bit build.
REM	    ALso, updated licensing with version 1.53, and copy multiple
REM	    READMEs into the CD image area.
REM	20-may-2003 (penga03)
REM	    Added Ingres Net to SDK. Also, took away the comments on 
REM	    OpenROAD.
REM	02-oct-2003 (somsa01)
REM	    Ported to NT_AMD64.
REM	26-jan-2004 (penga03)
REM	    Added a new feature Ingres .NET Data Provider.
REM	06-feb-2004 (somsa01)
REM	    Updated to use the .NET 2003 compiler.
REM	19-feb-2004 (penga03)
REM	    Added building MSRedists.ism.
REM	20-feb-2004 (somsa01)
REM	    Make sure the .ism files are writeable for ISCmdBld.
REM	02-mar-2004 (penga03)
REM	    Copy Ingres merge modules and msiupd.exe to 
REM	    %ING_SRC%\mergemodules.
REM	22-apr-2004 (penga03)
REM	    Unzip Microsoft .NET Framework package to
REM	    %ING_SRC%\cdimage\dotnetfx.
REM	03-jun-2004 (penga03)
REM	    Added a command-line option "-a" so that IsCmdBld ONLY builds
REM	    the MSI tables.
REM	09-jun-2004 (penga03)
REM	    Added error checking for IsCmdBld.
REM	24-Jun-2004 (drivi01)
REM	    Removed licensing.
REM	06-jul-2004 (penga03)
REM	    Use msidepcheck.exe, instead of IsCmdBld.exe to check out all
REM	    missing files and save them in %ing_src%\buildrel_report.txt.
REM	26-jul-2004 (penga03)
REM	    Removed all references to "Advantage".
REM	10-aug-2004 (somsa01)
REM	    We now use the 64-bit version of ODBC in the 64-bit build.
REM	02-sep-2004 (somsa01)
REM	    Updated for Open Source.
REM	14-sep-2004 (Penga03)
REM	    Replaced "Ingres Enterprise Edition" with "Ingres".
REM	14-sep-2004 (Penga03)
REM	    Corrected some errors introduced by the last change.
REM	07-oct-2004 (somsa01)
REM	    Updated with some new variables, and new locations for Open
REM	    Source.
REM	11-oct-2004 (Penga03)
REM	    Clean up buildrel_report.txt before auditing.
REM	15-Oct-2004 (drivi01)
REM	    Removed script for building merge module dlls and
REM	    pre-installer b/c it's being build by jam with the
REM	    rest of ingres code.
REM	12-Jan-2005 (drivi01)
REM	    Added calls to mkpreinstallAPI.bat and mkthinclient.bat
REM	    to copy directories that will be available to customers.
REM	21-Jan-2005 (drivi01)
REM	    Moved mkpreinstallAPI.bat and mkthinclient.bat calls to
REM	    buildrel execute instead of buildrel -a.
REM	28-Jan-2005 (drivi01)
REM	    Modified buildrel -a to print out a version number and
REM	    a build number from version.rel file.
REM	21-Mar-2005 (drivi01)
REM	    Modified buildrel to run mkembedextras.bat to create
REM	    embedded extras directory during roll of cdimage.
REM	30-Mar-2005 (drivi01)
REM	    Fixed spelling errors in call to mkembedextras.
REM	15-Apr-2005 (drivi01)
REM	    Modified buildrel to run mkpdbdir.bat to create
REM	    pdbs directory during creation of cdimage.
REM	25-May-2005 (drivi01)
REM	    Added copying of LICENSE file from ING_SRC to
REM	    cdimage directory.
REM	08-Aug-2005 (drivi01)
REM	    Added errorlevel check for bat files
REM	    executions.
REM	21-Sep-2005 (drivi01)
REM	    Updated this script to flag zero-length files and
REM	    to run audit on buildrel as well as buildrel -a.
REM	19-Jan-2006 (drivi01)
REM	    SIR 115615
REM	    Added new merge module project to the build proccess.
REM	23-Jan-2006 (drivi01)
REM	    Expanded this script to build limited cdimages. 
REM	    Limitted cdimage is an image without documentation
REM	    or i18n files.  During these changes -lmtd flag
REM	    was introduced which sets internal variable 
REM	    II_RELEASE_LMTD to on and results in IngresIIi18n
REM	    and IngresIIDoc modules being left out of build 
REM	    proccess, as well as forces use of IngresII_DBL_LMTD
REM	    project instead of IngresII_DBL.ism.
REM	1-Feb-2006 (drivi01)
REM	    SIR 155688
REM	    Made the changes to copy two licenses on the cdimage.
REM	    One license should a flat text file copied from %ING_SRC%
REM	    Second license file should be identical except in rtf
REM	    format and be copied from 
REM	    %ING_SRC%\front\st\enterprise_is_win\resource to 
REM	    License.rtf.  
REM	    buildrel is modified to copy gpl license by default, if
REM	    image requires another license then it should be copied
REM	    by hand to replace LICENSE and Licenter.rtf files on the 
REM	    image.
REM	06-Mar-2006 (drivi01)
REM	    Updated script to understand flag -p which will
REM	    print all files being shipped to %ING_BUILD%\picklist.dat
REM	09-Mar-2006 (drivi01)
REM	    buildrel should check for new version of adobe.
REM	21-Mar-2006 (drivi01)
REM	    Replaced calls to awk with calls to %AWK_CMD% to ensure
REM	    this script works with cygwin.
REM	22-Mar-2006 (drivi01)
REM	    Added creation of build.timestamp on first successful run of
REM	    of buildrel and only if it isn't already in ING_BUILD.
REM	13-Jun-2006 (drivi01)
REM	    Added another flag -s which is used in combination with flag
REM	    -p and prints out sptial object binary to picklist in 
REM	    II_SYSTEM\ingres
REM	20-Oct-2006 (drivi01)
REM	    Added a check for .NET Framework 2.0 in 
REM	    ING_ROOT\Framework2\dotnetfx.exe
REM	27-Oct-2006 (drivi01)
REM	    Pull out IngresIIi18n.ism module out and put new 
REM	    module IngresIIDBMSDotNETDataProvider.ism containing demo.
REM	16-Nov-2006 (drivi01)
REM	    SIR 116599
REM	    Modified the structure of the cdimage as well as added
REM	    two more instalelrs to the roll of cdimage. setup.exe is 
REM	    still in the root directory, however data cab, and msi 
REM	    as well as redistributables were moved into files directory.
REM	    documentation and dotnet directories contain the images 
REM	    for the other two installers, these two directories are
REM	    optional.  Added flags to buildrel:
REM	    -doc will omit documentation image from ingres cdimage
REM	    -dotnet will omit .NET Data Provider image from ingres cdimage
REM	    -doconly will build documentation only image
REM	    -dotnetonly will build .NET Data Provider only image
REM	    -? or -help will display usage
REM	    .NET Framework 2.0 was added to the redistributable list.
REM	20-Nov-2006 (drivi01)
REM	    .NET Data Provider module needs _isconfig.xml file that
REM	    is built into project directory.  Temporarily define
REM	    project directory and then remove it.
REM	20-Nov-2006 (drivi01)
REM	    Add building IngresIIi18n.ism back, even though it doesn't contain
REM	    any files we can't remove features without performing a major
REM	    upgrade.
REM	06-Dec-2006 (drivi01)
REM	    For option -lmtd change "echo DONE" statement to "goto DONE".
REM	    The option is disabled and should exit buildrel script.
REM	03-Jan-2007 (drivi01)
REM	    Take out check for adobe, we do not redistribute adobe anymore. Installer
REM	    will redirect user to adobe website if adobe is not installed.
REM	07-Jul-2007 (drivi01)
REM	    Remove any references to Framework packages. Framework packages
REM	    will no longer be included in the image.
REM	

set SUFFIX=
if "%CPU%" == "IA64" set SUFFIX=64
if "%CPU%" == "AMD64" set SUFFIX=64

if not x%II_RELEASE_DIR%==x goto CHECK1
echo You must set II_RELEASE_DIR !!
goto DONE

:CHECK1
if not x%II_SYSTEM%==x goto CHECK2
echo You must set II_SYSTEM !!
goto DONE

:CHECK2
if not x%ING_DOC%==x goto CHECK3
echo You must set ING_DOC !!
goto DONE

:CHECK3
if not x%ING_SRC%==x goto CHECK35
echo You must set ING_SRC !!
goto DONE

:CHECK35
if not x%ING_BUILD%==x goto CHECK36
echo You must set ING_BUILD !!
goto DONE

:CHECK36
if "%1" == "-p" goto LISTALL
goto CHECK6

:CHECK6
if exist %ING_SRC%\LICENSE.gpl goto CHECK7
echo You must have at least GPL license in %ING_SRC% !!

:CHECK7
if exist %ING_SRC%\front\st\enterprise_is_win\resource\GPL.rtf goto CHECK8
echo You must have rtf formatted copy of a license in %ING_SRC%\front\st\enterprise_is_win\resource directory !!


:CHECK8
if "%1" == "-lmtd" goto CONT0
if "%2" == "-lmtd" goto CONT0
if "%1" == "-doc" goto CONT0_1
if "%2" == "-doc" goto CONT0_1
if "%1" == "-dotnet" goto CONT0_2
if "%2" == "-dotnet" goto CONT0_2
if "%1" == "-doconly" goto CONT0_3
if "%1" == "-dotnetonly" goto CONT0_4
if "%1" == "-?" goto CONT0_9
if "%1" == "-help"  goto CONT0_9
goto CONT1

:CONT0
set II_RELEASE_LMTD=ON
echo This option has been disabled!
goto DONE

:CONT0_1
set II_RELEASE_NODOC=ON
goto CONT1

:CONT0_2
set II_RELEASE_NODOTNET=ON
goto CONT1

:CONT0_3
set II_RELEASE_DOC=ON
if "%II_RELEASE_DOC%" == "ON" goto DOC_IMAGE
goto CONT1

:CONT0_4
set II_RELEASE_DOTNET=ON
if "%II_RELEASE_DOTNET%" == "ON" goto DOTNET_IMAGE
goto CONT1

:CONT0_9
echo "USAGE: buildrel [-a| -p|-doc|-dotnet|-doconly|-dotnetonly]"
goto DONE

:CONT1
echo.
"%MKSLOC%\echo" "Version: \c" 
cat %II_SYSTEM%/ingres/version.rel
echo Release directory: %II_RELEASE_DIR%
echo CD image directory: %II_CDIMAGE_DIR%
echo Merge Modules directory: %II_MM_DIR%
echo II_RELEASE_LMTD=%II_RELEASE_LMTD%
echo.

:AUDIT
echo Auditing Merge Modules...
rm -f %ING_SRC%\buildrel_report.txt
call gd enterprise_is_win
@echo off
rm -f buildrel_report.txt
if "%II_RELEASE_LMTD%" == "ON" rm -rf %II_RELEASE_DIR%\IngresIIi18n %II_RELEASE_DIR%\IngresIIDoc %II_RELEASE_DIR%\IngresII\*
msidepcheck IngresIIDBMS%SUFFIX%.ism 
msidepcheck IngresIIDBMSIce%SUFFIX%.ism 
if "%DOUBLEBYTE%" == "ON" goto AUDIT_DBL1
msidepcheck IngresIIDBMSNet%SUFFIX%.ism 
goto AUDIT_CONT_1
:AUDIT_DBL1
msidepcheck IngresIIDBMSNet_DBL.ism 
:AUDIT_CONT_1
msidepcheck IngresIIDBMSNetIce%SUFFIX%.ism 
msidepcheck IngresIIDBMSNetTools%SUFFIX%.ism 
msidepcheck IngresIIDBMSNetVision%SUFFIX%.ism 
msidepcheck IngresIIDBMSReplicator%SUFFIX%.ism 
msidepcheck IngresIIDBMSTools%SUFFIX%.ism 
msidepcheck IngresIIDBMSVdba%SUFFIX%.ism 
msidepcheck IngresIIDBMSVision%SUFFIX%.ism 
if NOT "%II_RELEASE_LMTD%" == "ON" msidepcheck IngresIIDoc%SUFFIX%.ism
if NOT "%II_RELEASE_LMTD%" == "ON" msidepcheck IngresIIi18n%SUFFIX%.ism 
msidepcheck IngresIIEsqlC%SUFFIX%.ism 
msidepcheck IngresIIEsqlCEsqlCobol%SUFFIX%.ism 
msidepcheck IngresIIIce%SUFFIX%.ism 
msidepcheck IngresIIJdbc%SUFFIX%.ism 
msidepcheck IngresIIODBC%SUFFIX%.ism 
msidepcheck IngresIIVdba%SUFFIX%.ism 
msidepcheck IngresIINet%SUFFIX%.ism 
msidepcheck IngresIINetTools%SUFFIX%.ism 
msidepcheck IngresIIDotNETDataProvider.ism 
msidepcheck IngresIIDBMSDotNETDataProvider.ism
msidepcheck MSRedists.ism 
if "%EVALBUILD%" == "ON" goto AUDIT_SDK1
msidepcheck IngresIIEsqlCobol%SUFFIX%.ism 
msidepcheck IngresIIEsqlFortran%SUFFIX%.ism 
msidepcheck IngresIIReplicator%SUFFIX%.ism 
msidepcheck IngresIIStar%SUFFIX%.ism 
msidepcheck IngresIITools%SUFFIX%.ism 
msidepcheck IngresIIToolsVision%SUFFIX%.ism 
msidepcheck IngresIIVision%SUFFIX%.ism 
if NOT "%II_RELEASE_LMTD%" == "ON" msidepcheck IngresII_Doc.ism
msidepcheck IngresII_DotNETDataProvider.ism
@echo Auditing Ingres setup package...
if "%DOUBLEBYTE%" == "ON" goto AUDIT_DBL2
msidepcheck IngresII%SUFFIX%.ism 
goto AUDIT_CONT_2
:AUDIT_DBL2
if "%II_RELEASE_LMTD%" == "ON" goto AUDIT_CONT_LMTD
msidepcheck IngresII_DBL.ism 
goto AUDIT_CONT_2
:AUDIT_SDK1
msidepcheck IngresSDK.ism 

:AUDIT_CONT_LMTD
msidepcheck IngresII_DBL_LMTD.ism
goto AUDIT_CONT_2

:AUDIT_CONT_2
if exist buildrel_report.txt mv buildrel_report.txt %ING_SRC%\buildrel_report.txt
if exist %ING_SRC%\buildrel_report.txt echo There are some files missing or zero length:& echo.
if exist %ING_SRC%\buildrel_report.txt type %ING_SRC%\buildrel_report.txt& echo.
if exist %ING_SRC%\buildrel_report.txt echo A log of these missing files has been saved in %ING_SRC%\buildrel_report.txt.& goto DONE
if "%1" == "-a" goto CONT4

:INST64
devenv /useenv /rebuild Release64 /project enterprise enterprise.sln
goto CONT2

:INSTAMD64
devenv /useenv /rebuild ReleaseAMD64 /project enterprise enterprise.sln

:CONT2
echo.
echo Copying preinstall directory...
echo.
call mkpreinstallAPI
if errorlevel 1 goto ERROR2
echo.
echo Copying thinclient directory...
echo.
call mkthinclient
if errorlevel 1 goto ERROR2
echo.
echo.
echo Copying embedded extras directory...
echo.
call mkembedextras
if errorlevel 1 goto ERROR2
echo.
echo
echo Copying pdbs directory...
echo.
call mkpdbdir.bat
if errorlevel 1 goto ERROR2
echo.
echo Building Merge Modules...
call gd enterprise_is_win
call chmod 777 *.ism
ISCmdBld.exe -x -p IngresIIDBMS%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIDBMS
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIDBMSIce%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIDBMSIce
if errorlevel 1 goto ERROR
@echo.
if "%DOUBLEBYTE%" == "ON" goto DBL1
ISCmdBld.exe -x -p IngresIIDBMSNet%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIDBMSNet
if errorlevel 1 goto ERROR
@echo.
goto CONT2_1
:DBL1
ISCmdBld.exe -x -p IngresIIDBMSNet_DBL.ism -b %II_RELEASE_DIR%\IngresIIDBMSNet
if errorlevel 1 goto ERROR
@echo.
:CONT2_1
ISCmdBld.exe -x -p IngresIIDBMSNetIce%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIDBMSNetIce
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIDBMSNetTools%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIDBMSNetTools
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIDBMSNetVision%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIDBMSNetVision
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIDBMSReplicator%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIDBMSReplicator
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIDBMSTools%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIDBMSTools
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIDBMSVdba%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIDBMSVdba
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIDBMSVision%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIDBMSVision
if errorlevel 1 goto ERROR
@echo.
if NOT "%II_RELEASE_LMTD%" == "ON" ISCmdBld.exe -x -p IngresIIDoc%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIDoc
if errorlevel 1 goto ERROR
@echo
if NOT "%II_RELEASE_LMTD%" == "ON" ISCmdBld.exe -x -p IngresII_Doc.ism -b %II_RELEASE_DIR%\IngresII_Doc
if errorlevel 1 goto ERROR
@echo.
if NOT "%II_RELEASE_LMTD%" == "ON" ISCmdBld.exe -x -p IngresIIi18n%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIi18n
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIEsqlC%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIEsqlC
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIEsqlCEsqlCobol%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIEsqlCEsqlCobol
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIIce%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIIce
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIJdbc%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIJdbc
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIODBC%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIODBC
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIVdba%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIVdba
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIINet%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIINet
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIINetTools%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIINetTools
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIDotNETDataProvider.ism -b %II_RELEASE_DIR%\IngresIIDotNETDataProvider
if errorlevel 1 goto ERROR
@echo.
mkdir IngresII_DotNETDataProvider
ISCmdBld.exe -x -p IngresII_DotNETDataProvider.ism -b %II_RELEASE_DIR%\IngresII_DotNETDataProvider
rm -rf IngresII_DotNETDataProvider
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIDBMSDotNETDataProvider.ism -b %II_RELEASE_DIR%\IngresIIDBMSDotNETDataProvider
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p MSRedists.ism -b %II_RELEASE_DIR%\MSRedists
if errorlevel 1 goto ERROR
@echo.
if "%EVALBUILD%" == "ON" goto SDK1
ISCmdBld.exe -x -p IngresIIEsqlCobol%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIEsqlCobol
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIEsqlFortran%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIEsqlFortran
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIReplicator%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIReplicator
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIStar%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIStar
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIITools%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIITools
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIToolsVision%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIToolsVision
if errorlevel 1 goto ERROR
@echo.
ISCmdBld.exe -x -p IngresIIVision%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresIIVision
if errorlevel 1 goto ERROR
@echo.
@echo Building Ingres setup package...
if "%DOUBLEBYTE%" == "ON" goto DBL2
ISCmdBld.exe -x -p IngresII%SUFFIX%.ism -b %II_RELEASE_DIR%\IngresII
if errorlevel 1 goto ERROR
@echo.
goto CONT
:DBL2
if "%II_RELEASE_LMTD%" == "ON" goto DBL2_LMTD
ISCmdBld.exe -x -p IngresII_DBL.ism -b %II_RELEASE_DIR%\IngresII
if errorlevel 1 goto ERROR
@echo.
goto CONT
:SDK1
ISCmdBld.exe -x -p IngresSDK.ism -b %II_RELEASE_DIR%\IngresSDK
if errorlevel 1 goto ERROR
@echo.

:DBL2_LMTD
ISCmdBld.exe -x -p IngresII_DBL_LMTD.ism -b %II_RELEASE_DIR%\IngresII
if errorlevel 1 goto ERROR
@echo.
goto CONT

:CONT
call chmod 444 *.ism
@echo off
echo Creating mergemodules...
if not exist %II_MM_DIR% mkdir %II_MM_DIR%
rm -rf %II_MM_DIR%\*
cp -p -v %II_RELEASE_DIR%\*\build\release\DiskImages\Disk1\*.msm %II_MM_DIR%
cp -p -v %ING_BUILD%\bin\msiupd.exe %II_MM_DIR%
echo Creating cdimage...
if not exist %II_CDIMAGE_DIR% mkdir %II_CDIMAGE_DIR%
rm -rf %II_CDIMAGE_DIR%\*
mkdir %II_CDIMAGE_DIR%\files
mkdir %II_CDIMAGE_DIR%\files\msredist
cp -p -v %II_RELEASE_DIR%\MSRedists\build\release\DiskImages\Disk1\*.* %II_CDIMAGE_DIR%\files\msredist
if "%EVALBUILD%" == "ON" goto SDK2
cp -p -v %II_RELEASE_DIR%\IngresII\build\release\DiskImages\Disk1\*.* %II_CDIMAGE_DIR%\files
rm -rf %II_CDIMAGE_DIR%\files\setup.*
if "%CPU%" == "IA64" cp -p -v %ING_SRC%\front\st\enterprise_preinst\Release64\setup.exe %II_CDIMAGE_DIR%& goto CONT5
if "%CPU%" == "AMD64" cp -p -v %ING_SRC%\front\st\enterprise_preinst\ReleaseAMD64\setup.exe %II_CDIMAGE_DIR%& goto CONT5
cp -p -v %ING_SRC%\front\st\enterprise_preinst_win\Release\setup.exe %II_CDIMAGE_DIR%
:CONT5
cp -p -v %ING_SRC%\front\st\enterprise_preinst_win\AUTORUN.INF %II_CDIMAGE_DIR%
cp -p -v %ING_SRC%\front\st\enterprise_preinst_win\sample.rsp %II_CDIMAGE_DIR%
cp -p -v %II_SYSTEM%\readme.html %II_CDIMAGE_DIR%
cp -p -v %II_SYSTEM%\readme_int_w32.html %II_CDIMAGE_DIR%
cp -p -v %ING_SRC%\LICENSE.gpl %II_CDIMAGE_DIR%\LICENSE
REM cp -p -v %ING_SRC%\front\st\enterprise_is_win\resource\GPL.rtf %II_CDIMAGE_DIR%\License.rtf

:DOC_IMAGE
echo.
echo Creating documentation image...
echo.
if "%II_RELEASE_NODOC%" == "ON" goto DOTNET_IMAGE
if "%II_RELEASE_DOC%" == "ON" cd %ING_SRC%\front\st\enterprise_is_win
if "%II_RELEASE_DOC%" == "ON" chmod 777 IngresII_Doc.ism 
if "%II_RELEASE_DOC%" == "ON" ISCmdBld.exe -x -p IngresII_Doc.ism -b %II_RELEASE_DIR%\IngresII_Doc
if "%II_RELEASE_DOC%" == "ON" chmod 444 IngresII_Doc.ism 
if not exist %II_CDIMAGE_DIR%\files\documentation mkdir %II_CDIMAGE_DIR%\files\documentation
rm -rf %II_CDIMAGE_DIR%\files\documentation\*
cp -p -v %II_RELEASE_DIR%\IngresII_Doc\build\release\DiskImages\Disk1\*.* %II_CDIMAGE_DIR%\files\documentation
rm -rf %II_CDIMAGE_DIR%\files\documentation\setup.*
cp -p -v %II_SYSTEM%\ingres\bin\SetupDriver.exe %II_CDIMAGE_DIR%\files\documentation\setup.exe
msiupd "%II_CDIMAGE_DIR%\files\documentation\Ingres 2006 Documentation.msi"
if "%II_RELEASE_DOC%" == "ON" goto DONE

:DOTNET_IMAGE
echo.
echo Creating Ingres .NET Data Provider image...
echo.
if "%II_RELEASE_NODOTNET%" == "ON" goto CONT3
if "%II_RELEASE_DOTNET%" == "ON" cd %ING_SRC%\front\st\enterprise_is_win
if "%II_RELEASE_DOTNET%" == "ON" chmod 777 IngresII_DotNETDataProvider.ism 
if "%II_RELEASE_DOTNET%" == "ON" ISCmdBld.exe -x -p IngresII_DotNETDataProvider.ism -b %II_RELEASE_DIR%\IngresII_DotNETDataProvider
if "%II_RELEASE_DOTNET%" == "ON" chmod 444 IngresII_DotNETDataProvider.ism 
if not exist %II_CDIMAGE_DIR%\files\dotnet mkdir %II_CDIMAGE_DIR%\files\dotnet
rm -rf %II_CDIMAGE_DIR%\files\dotnet\*
cp -p -v %II_RELEASE_DIR%\IngresII_DotNETDataProvider\build\release\DiskImages\Disk1\*.* %II_CDIMAGE_DIR%\files\dotnet
rm -rf %II_CDIMAGE_DIR%\files\dotnet\setup.*
cp -p -v %II_SYSTEM%\ingres\bin\SetupDriver.exe %II_CDIMAGE_DIR%\files\dotnet\setup.exe
msiupd "%II_CDIMAGE_DIR%\files\dotnet\Ingres .NET Data Provider 2.0.msi"
goto CONT3

:SDK2
xcopy /y "%II_RELEASE_DIR%\IngresSDK\build\release\DiskImages\Disk1\*.msi" "%II_CDIMAGE_DIR%\files\*.msi"
cp -p -v %II_RELEASE_DIR%\IngresSDK\build\release\DiskImages\Disk1\*.* %II_CDIMAGE_DIR%\files
cp -p -v %ING_SRC%\front\st\enterprise_preinst\AUTORUN.INF %II_CDIMAGE_DIR%
cp -p -v %II_SYSTEM%\readme_ingres_sdk.html %II_CDIMAGE_DIR%

:CONT3
if not exist %II_CDIMAGE_DIR%\files\dotnet mkdir %II_CDIMAGE_DIR%\files\dotnet
if "%II_RELEASE_DOTNET%" == "ON" goto DONE
cp -p -v %ING_BUILD%\bin\ingwrap.exe %II_CDIMAGE_DIR%\files\ingwrap.exe


if "%EVALBUILD%" == "ON" goto SDK3
cd /d %II_CDIMAGE_DIR%\files
ren Ingres.msi IngresII.msi
echo Updating MSI database...
msiupd %II_CDIMAGE_DIR%\files\IngresII.msi
goto CONT4

:SDK3
rm -rf %II_CDIMAGE_DIR%\*.msm
cp -p -v -r "%ING_SRC%\Forest And Trees RTL" "%II_CDIMAGE_DIR%\Forest And Trees RTL"
cp -p -v -r "%ING_SRC%\OpenROAD" "%II_CDIMAGE_DIR%\OpenROAD"
cp -p -v -r "%ING_SRC%\CleverPath Portal" "%II_CDIMAGE_DIR%\CleverPath Portal"
cp -p -v -r "%ING_SRC%\Apache HTTP Server" "%II_CDIMAGE_DIR%\Apache HTTP Server"
cp -p -v -r "%ING_SRC%\DBupdate" "%II_CDIMAGE_DIR%\DBupdate"
cd /d %II_CDIMAGE_DIR%
ren setup.exe install.exe
ren *.msi "Ingres SDK.msi"
echo Updating MSI database...
msiupd "%II_CDIMAGE_DIR%\Ingres SDK.msi"
goto CONT6

:CONT6
if not exist %II_SYSTEM%\ingres\temp mkdir %II_SYSTEM%\ingres\temp
ls %II_MM_DIR%|wc -l| %AWK_CMD% '{print "\n\n"$1" mergemodules created!!\n\n" }' 
goto CONT4

:LISTALL
rm %ING_BUILD%\picklist.dat
call gd enterprise_is_win
if "%2" == "-s" goto LISTSOL
goto LISTALL_1

:LISTSOL
msidepcheck -p IngresIISpat.ism 
goto CONT4

:LISTALL_1
msidepcheck -p IngresIIDBMS%SUFFIX%.ism 
msidepcheck -p IngresIIDBMSIce%SUFFIX%.ism 
if "%DOUBLEBYTE%" == "ON" goto LISTALL_DBL1
msidepcheck-p IngresIIDBMSNet%SUFFIX%.ism 
goto LISTALL_CONT_1
:LISTALL_DBL1
msidepcheck -p IngresIIDBMSNet_DBL.ism 
:LISTALL_CONT_1
msidepcheck -p IngresIIDBMSNetIce%SUFFIX%.ism 
msidepcheck -p IngresIIDBMSNetTools%SUFFIX%.ism 
msidepcheck -p IngresIIDBMSNetVision%SUFFIX%.ism 
msidepcheck -p IngresIIDBMSReplicator%SUFFIX%.ism 
msidepcheck -p IngresIIDBMSTools%SUFFIX%.ism 
msidepcheck -p IngresIIDBMSVdba%SUFFIX%.ism 
msidepcheck -p IngresIIDBMSVision%SUFFIX%.ism 
msidepcheck -p IngresIIDoc%SUFFIX%.ism 
msidepcheck -p IngresIIEsqlC%SUFFIX%.ism 
msidepcheck -p IngresIIEsqlCEsqlCobol%SUFFIX%.ism 
msidepcheck -p IngresIIIce%SUFFIX%.ism 
msidepcheck -p IngresIIJdbc%SUFFIX%.ism 
msidepcheck -p IngresIIODBC%SUFFIX%.ism 
msidepcheck -p IngresIIVdba%SUFFIX%.ism 
msidepcheck -p IngresIINet%SUFFIX%.ism 
msidepcheck -p IngresIINetTools%SUFFIX%.ism 
msidepcheck -p IngresIIDotNETDataProvider.ism 
msidepcheck -p MSRedists.ism 
if "%EVALBUILD%" == "ON" goto LISTALL_SDK1
msidepcheck -p IngresIIEsqlCobol%SUFFIX%.ism 
msidepcheck -p IngresIIEsqlFortran%SUFFIX%.ism 
msidepcheck -p IngresIIReplicator%SUFFIX%.ism 
msidepcheck -p IngresIIStar%SUFFIX%.ism 
msidepcheck -p IngresIITools%SUFFIX%.ism 
msidepcheck -p IngresIIToolsVision%SUFFIX%.ism 
msidepcheck -p IngresIIVision%SUFFIX%.ism 
if "%DOUBLEBYTE%" == "ON" goto LISTALL_DBL2
msidepcheck -p IngresII%SUFFIX%.ism 
goto CONT4
:LISTALL_DBL2
msidepcheck -p IngresII_DBL.ism 
goto CONT4
:LISTALL_SDK1
msidepcheck -p IngresSDK.ism 



:CONT4
if not exist %ING_BUILD%\build.timestamp touch  %ING_BUILD%\build.timestamp
echo DONE!!!
goto DONE


:ERROR
echo A build error occurs with ISCmdBld.exe!!!

:ERROR2
echo An error occured copying files for one of the build directories.

:DONE
endlocal
