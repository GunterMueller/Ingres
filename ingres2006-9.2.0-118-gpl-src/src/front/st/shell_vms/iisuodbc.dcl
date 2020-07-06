$!----------------------------------------------------------------------------
$!
$!  Copyright (c) 2004, 2007 Ingres Corporation
$!
$!  Name:
$!	iisuodbc -- set-up script for ODBC.
$!
$!  Usage:
$!	iisuodbc
$!
$!  Description:
$!	This script is called by VMSINSTAL to set up the Ingres/EDBC
$!      ODBC configuration files. 
$!	It assumes that II_SYSTEM  has been set in the logical name table 
$!	(SYSTEM or GROUP) which is to be used to configure the installation 
$!	or in the JOB logical table.
$!
$!! History:
$!!	29-jan-2004 (loera01)
$!!	    Created.
$!!	30-jun-2004 (loera01)
$!!         Added ODBC trace library.
$!!	14-dec-2004 (abbjo03)
$!!	    Replace history comments.
$!!	18-apr-2005 (abbjo03)
$!!	    Fix reference to "JDBC".
$!!     05-may-2005 (loera01)
$!!         Revised naming of read-only ODBC driver and added iisetres for
$!!         the ODBC CLI.  Added support for "-batch" parameter.
$!!     31-may-2005 (loera01) Bug 114601
$!!         When prompting for read-only, check for both "n" and "N" replies.
$!!	17-may-2006 (abbjo03)
$!!	    Remove double quotes from II_CONFIG process level definition.
$!!     12-jun-2007 (loera01) Bug 118495
$!!         Added references to II_ODBC_LIB and II_ODBC_CLILIB in config.dat.
$!----------------------------------------------------------------------------
$!
$ on control_c then goto EXIT_FAIL
$ on control_y then goto EXIT_FAIL
$ set noon
$ set noverify
$!
$! Define symbols for programs called locally.
$!
$ iigenres	:= $ii_system:[ingres.utility]iigenres.exe
$ iigetres	:= $ii_system:[ingres.utility]iigetres.exe
$ iigetsyi	:= @ii_system:[ingres.utility]iigetsyi.com
$ iiingloc      := $ii_system:[ingres.utility]iiingloc.exe
$ iipmhost      := $ii_system:[ingres.utility]iipmhost.exe
$ iimaninf	:= $ii_system:[ingres.utility]iimaninf.exe
$ iiresutl	:= $ii_system:[ingres.utility]iiresutl.exe
$ iisetres	:= $ii_system:[ingres.utility]iisetres.exe
$ iisulock	:= $ii_system:[ingres.utility]iisulock.exe
$ iivalres	:= $ii_system:[ingres.utility]iivalres.exe
$ iiodbcinst	:= $ii_system:[ingres.utility]iiodbcinst.exe
$!
$! Other variables, macros and temporary files
$!
$ ii_compatlib_def   = f$trnlnm("ii_compatlib","LNM$JOB") ! Current values
$ ii_libqlib_def     = f$trnlnm("ii_libqlib","LNM$JOB")   !  for shared
$ ii_framelib_def    = f$trnlnm("ii_framelib","LNM$JOB")  !   library definitions
$ iisulocked         = 0                             !Do we hold the setup lock?
$ saved_message      = f$environment( "MESSAGE" )    ! save message settings
$ set_message_off    = "set message/nofacility/noidentification/noseverity/notext"
$ set_message_on     = "set message''saved_message'"
$ SS$_NORMAL	     = 1
$ SS$_ABORT	     = 44
$ status	     = SS$_NORMAL
$ node_name	     = f$getsyi("NODENAME")
$ tmpfile            = "ii_system:[ingres]iisuodbc.''node_name'_tmp" !Temp file
$!
$! Define ii_config for the installation configuration files
$!
$ define/nolog/process II_CONFIG    II_SYSTEM:[ingres.files]
$!
$  say		:= type sys$input
$  echo		:= write sys$output 
$!
$ clustered	= f$getsyi( "CLUSTER_MEMBER" ) !Is this node clustered?
$ user_uic	= f$user() !For RUN command (detached processes)
$ user_name= f$getjpi("","USERNAME") !User running this procedure
$!
$ ivp_name= f$getjpi("","USERNAME")
$ ivp_user = f$edit( ivp_name, "TRIM,LOWERCASE" )
$!
$ ing_user = f$edit( user_name, "TRIM,LOWERCASE" ) !Ingres version of username
$!
$ set noon
$!
$! Get compressed release ID.
$!
$ on error then goto EXIT_FAIL
$ iimaninf -version=odbc -output='tmpfile
$ set noon
$!
$ open iioutput 'tmpfile
$ read iioutput version_string
$ close iioutput
$ delete/noconfirm 'tmpfile;*
$ release_id = f$edit( version_string, "collapse, trim" )
$!
$ STRIPPER_1:
$!
$ offset = f$locate( ".", release_id )
$ length = f$length( release_id )
$ if( offset .eq. length ) then goto STRIPPER_2
$ sub1 = f$extract( 0, offset, release_id )
$ sub2 = f$extract( offset + 1, length - offset - 1, release_id )
$ release_id = sub1 + sub2
$ goto STRIPPER_1
$!
$ STRIPPER_2:
$!
$ offset = f$locate( "/", release_id )
$ length = f$length( release_id )
$ if( offset .eq. length ) then goto STRIPPER_3
$ sub1 = f$extract( 0, offset, release_id )
$ sub2 = f$extract( offset + 1, length - offset - 1, release_id )
$ release_id = sub1 + sub2
$ goto STRIPPER_2
$!
$ STRIPPER_3:
$!
$ offset = f$locate( "(", release_id )
$ length = f$length( release_id )
$ if( offset .eq. length ) then goto STRIPPER_4
$ sub1 = f$extract( 0, offset, release_id )
$ sub2 = f$extract( offset + 1, length - offset - 1, release_id )
$ release_id = sub1 + sub2
$ goto STRIPPER_3
$!
$ STRIPPER_4:
$!
$ offset = f$locate( ")", release_id )
$ length = f$length( release_id )
$ if( offset .eq. length ) then goto STRIPPERS_DONE
$ sub1 = f$extract( 0, offset, release_id )
$ sub2 = f$extract( offset + 1, length - offset - 1, release_id )
$ release_id = sub1 + sub2
$ goto STRIPPER_1
$!
$ STRIPPERS_DONE:
$ if p1 .eqs. "-BATCH"
$ then
$     iisetres ii.'node_name.odbc.module_name "ODBCFELIB.EXE"
$     iisetres ii.'node_name.odbc.cli_module_name "ODBCCLIFELIB.EXE"
$     iisetres ii.'node_name.lnm.ii_odbc_lib "II_SYSTEM:[INGRES.LIBRARY]ODBCFELIB.EXE"
$     iisetres ii.'node_name.lnm.ii_odbc_clilib "II_SYSTEM:[INGRES.LIBRARY]ODBCCLIFELIB.EXE"
$     iisetres ii.'node_name.odbc.trace_module_name "ODBCTRACEFELIB.EXE"
$     iiodbcinst -batch
$     exit 1
$ endif
$!
$ set_message_on
$    say 
 
This procedure will set up the following version of Ingres ODBC Server:
 
$    echo "	''version_string'"
$    say
 
to run on local node:
 
$    echo "	''node_name'"
$    say 
 

$!
$ CONFIRM_CONFIG_SETUP:
$!
$    inquire/nopunc answer "Do you wish to continue this setup procedure? (y/n) "
$    if( answer .eqs. "" ) then goto CONFIRM_CONFIG_SETUP
$    if( .not. answer ) then goto EXIT_FAIL
$ newpath = f$trnlnm("ODBCSYSINI")
$ if (newpath .eqs. "") 
$ then 
$    newpath = f$trnlnm("SYS$SHARE")
$ else
$    echo "ODBCSYSINI is defined.  Value is ''newpath'"
$ endif
$ ENTER_PATH:
$ inquire/nopunc path -
     "Enter the default ODBC configuration path [ ''newpath' ]: "
$ CONFIRM_PATH:
$ if path .eqs. "" then path = newpath
$ inquire/nopunc answer -
   "Is the path [ ''path' ] correct? (y/n): "
$ if( answer .eqs. "" ) then goto CONFIRM_PATH
$ if( .not. answer ) then goto ENTER_PATH
$!
$ if path .eqs. "" then path = newpath
$ if (newpath .nes. path) then pathFlag = "-p ''path' "
$ ENTER_READ_ONLY:
$ inquire/nopunc answer -
   "Is your ODBC driver read-only? (y/n) [n]: "
$ if( answer .eqs. "" .or. answer .eqs. "n" .or. answer .eqs. "N") 
$ then 
$    readOnlyFlag = ""
$    echo "Your ODBC driver will allow updates"
$ else
$    readOnlyFlag = "-r "
$    echo "Your ODBC driver will be read-only"
$ endif
$ CONFIRM_READ_ONLY:
$ inquire/nopunc answer -
    "Is this information correct? (y/n): "
$ if( answer .eqs. "" ) then goto CONFIRM_READ_ONLY
$ if( .not. answer ) then goto ENTER_READ_ONLY
$ define/user sys$input sys$command
$ iiodbcinst 'pathFlag' 'readOnlyFlag'
$ if (readOnlyFlag .eqs. "-r")
$ then
$ iisetres ii.'node_name.odbc.module_name "ODBCROFELIB.EXE"
$ iisetres ii.'node_name.lnm.ii_odbc_lib "II_SYSTEM:[INGRES.LIBRARY]ODBCROFELIB.EXE"
$ else
$ iisetres ii.'node_name.odbc.module_name "ODBCFELIB.EXE"
$ iisetres ii.'node_name.lnm.ii_odbc_lib "II_SYSTEM:[INGRES.LIBRARY]ODBCFELIB.EXE"
$ endif
$ iisetres ii.'node_name.odbc.cli_module_name "ODBCCLIFELIB.EXE"
$ iisetres ii.'node_name.lnm.ii_odbc_clilib "II_SYSTEM:[INGRES.LIBRARY]ODBCCLIFELIB.EXE"
$ iisetres ii.'node_name.odbc.trace_module_name "ODBCTRACEFELIB.EXE"
$!
$! Exit with success or error.
$!
$ status = SS$_NORMAL
$ goto exit_ok1
$!
$ EXIT_FAIL:
$ echo "ODBC configuration aborted."
$ status = SS$_ABORT
$exit_ok1:
$!
$ on control_c then goto EXIT_FAIL
$ on control_y then goto EXIT_FAIL
$ set on
$ exit 'status'
