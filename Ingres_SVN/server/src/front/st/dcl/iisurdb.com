$!----------------------------------------------------------------------------
$!
$!  Copyright (c) 2006 Ingres Corporation.  All Rights Reserved.
$!
$!  Name:
$!	iisurdb -- set-up script for Ingres Gateway to RDB
$!
$!  Usage:
$!	iisurdb
$!
$!  Description:
$!	This script is called by VMSINSTAL to set up Ingres Gateway to RDB.
$!	It assumes that II_SYSTEM has been set in the logical name table 
$!	(SYSTEM or GROUP) which is to be used to configure the installation 
$!	or in the JOB logical table.
$!
$!!  History:
$!!      18-jun-1999 (kinte01)
$!!          created
$!!      24-mar-2000 (devjo01)
$!!          Make gen of scratch file names safe for Clustered Ingres.
$!!      18-Jun-2001 (horda03)
$!!          Only allow Ingres System Administrator of the VMSINSTAL script
$!!          to invoke the IVP scripts.
$!!      08-feb-02 (chash01)
$!!          don't set ii_manifest_dir logical
$!!      15-may-02 (chash01)
$!!          rdb gw doesn't need user name and password.  Don't ask for it.
$!!	14-dec-2004 (abbjo03)
$!!	    Replace history comments.
$!----------------------------------------------------------------------------
$!
$ on control_c then goto EXIT_FAIL
$ on control_y then goto EXIT_FAIL
$ set noon
$! set noverify
$!
$! Define symbols for programs called locally.
$!
$ iigenres	:= $ii_system:[ingres.utility]iigenres.exe
$ iigetres	:= $ii_system:[ingres.utility]iigetres.exe
$ iigetsyi	:= @ii_system:[ingres.utility]iigetsyi.com
$ iipmhost      := $ii_system:[ingres.utility]iipmhost.exe
$ iimaninf	:= $ii_system:[ingres.utility]iimaninf.exe
$ iiresutl	:= $ii_system:[ingres.utility]iiresutl.exe
$ iisetres	:= $ii_system:[ingres.utility]iisetres.exe
$ iisulock	:= $ii_system:[ingres.utility]iisulock.exe
$ iivalres	:= $ii_system:[ingres.utility]iivalres.exe
$ ingstart	:= $ii_system:[ingres.utility]ingstart.exe
$ ingstop	:= @ii_system:[ingres.utility]iisdall.com
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
$!
$! Make temporary definitions for shared library logicals to 
$! be the unconcealed path of the library directory within ii_system
$! 
$ define/nolog/job ii_compatlib 'f$parse("ii_system:[ingres.library]clfelib.exe",,,,"NO_CONCEAL")
$ define/nolog/job ii_libqlib   'f$parse("ii_system:[ingres.library]libqfelib.exe",,,,"NO_CONCEAL")
$ define/nolog/job ii_framelib  'f$parse("ii_system:[ingres.library]framefelib.exe",,,,"NO_CONCEAL")
$ if f$search("ii_system:[ingres]iistartup.del") .nes. "" then @ii_system:[ingres]iistartup.del
$ set_message_off
$ if( f$search( "ii_system:[ingres.files]config.dat" ) .nes. "" )
$ then
$    set_message_on
$    @ii_system:[ingres.utility]iishare delete
$ endif
$!
$!!!!!! Define ii_config for the installation configuration files
$!
$ define/nolog/process ii_config    "ii_system:[ingres.files]"
$!
$  say		:= type sys$input
$  echo		:= write sys$output 
$!
$ clustered	= f$getsyi( "CLUSTER_MEMBER" ) !Is this node clustered?
$ user_uic	= f$user() !For RUN command (detached processes)
$ user_name     = f$trnlnm( "II_SUINGRES_USER","LNM$JOB" ) !Installation owner
$ IF "''user_name'".EQS.""
$ THEN
$    isa_uic = f$file_attributes( "II_SYSTEM:[000000]INGRES.DIR", "UIC")
$
$    if isa_uic .nes. user_uic
$    then
$       type sys$input

     ---------------------------------------------------------
    | This setup procedure must be run from the Ingres System |
    | Administrator account.                                  |
     ---------------------------------------------------------
$
$      goto EXIT_FAIL
$    endif
$
$    user_name= f$getjpi("","USERNAME") !User running this procedure
$ ENDIF
$ ivp_name= f$getjpi("","USERNAME")
$ ivp_user = f$edit( ivp_name, "TRIM,LOWERCASE" )
$!
$ ing_user = f$edit( user_name, "TRIM,LOWERCASE" ) !Ingres version of username
$
$!
$ echo "Setting up Ingres Gateway to RDB"
$!
$!!!!!! Create ii_system:[ingres.files]config.dat if non-existent. 
$!
$ if( f$search( "ii_system:[ingres.files]config.dat" ) .eqs. "" )
$ then
$    copy nl: ii_system:[ingres.files]config.dat -
	/prot=(s:rwed,o:rwed,g:rwed,w:re)
$ endif
$ set file /version_limit=20 ii_system:[ingres.files]config.dat
$ purge ii_system:[ingres.files]config.dat
$!
$!!!!!! Lock the configuration file. 
$!
$ on error then goto EXIT_FAIL
$ iisulock "Ingres Gateway to RDB setup"
$ iisulocked = 1
$ set noon
$!
$!!!!!! Get compressed release ID.
$!
$ on error then goto EXIT_FAIL
$! define II_MANIFEST_DIR II_SYSTEM:[ingres.install.gateways]
$ iimaninf -version=rdb -output=sys$scratch:iioutput.tmp
$! deassign II_MANIFEST_DIR
$ set noon
$!
$ open iioutput sys$scratch:iioutput.tmp
$ read iioutput version_string
$ close iioutput
$ delete/noconfirm sys$scratch:iioutput.tmp;*
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
$!
$!!!!!! Run IIJOBDEF.COM if it exists. 
$!
$ set_message_off
$ if( f$search( "ii_system:[ingres]iijobdef.com" ) .nes. "" )
$ then
$    set_message_on
$    say

Loading existing configuration...
$    ! Save settings thay may be lost
$    !
$    @ii_system:[ingres]iijobdef
$    !
$    ! Restore modified settings used by setup scripts
$    define/nolog/job ii_compatlib 'f$parse("ii_system:[ingres.library]clfelib.exe",,,,"NO_CONCEAL")
$    define/nolog/job ii_libqlib   'f$parse("ii_system:[ingres.library]libqfelib.exe",,,,"NO_CONCEAL")
$    define/nolog/job ii_framelib  'f$parse("ii_system:[ingres.library]framefelib.exe",,,,"NO_CONCEAL")
$ endif
$ set_message_on
$!
$! Check if Ingres Gateway to RDB is already set up.
$!
$ set_message_off
$ iigetres ii.'node_name.config.rdb.'release_id rdb_setup_status
$ rdb_setup_status = f$trnlnm( "rdb_setup_status" )
$ deassign "rdb_setup_status"
$ set_message_on
$ if( ( rdb_setup_status .nes. "" ) .and. -
     ( rdb_setup_status .eqs. "COMPLETE" ) )
$ then
$    say 

The latest version of Ingres Gateway to RDB 
has already been set up to run on
local
$    echo "node ''node_name'."
$    echo ""
$    ii_installation = f$trnlnm("II_INSTALLATION")
$!
$    if( ii_installation .nes. "" )
$    then
$       copy ii_system:[ingres.bin]iigwrdb.exe -
            ii_system:[ingres.bin]iigwrdb'ii_installation.exe
$    endif
$    goto EXIT_OK
$ endif
$!
$ say 

Generating default configuration...

$       on error then goto IIGENRES_SERVER_FAILED
$       iigenres 'node_name ii_system:[ingres.files]rdb.rfm 
$       set noon
$       iisetres ii.'node_name.config.rdb.'release_id "DEFAULTS"
$       goto IIGENRES_SERVER_DONE
$!
$       IIGENRES_SERVER_FAILED:
$!
$       say 
An error occurred while generating the default configuration.

$       goto EXIT_FAIL
$!
$       IIGENRES_SERVER_DONE:
$       say 
 
Default configuration generated.

$!
$! now ask input from user about database name, rdb/cdd file/path name,
$! user, password.  User and password is not currently used, but ask for
$! anyway
$!
$        say

Please enter database name, and Rdb location.  Enter Carriage Return
 in inquiry to terminate at any time.

  Database name: An Ingres style alias of the rdb database you want
                 to connect to. Example: employee_db
   
  Location     : This can be either an Rdb database file name such as
                 II_SYSTEM:[INGRES.FILES]employee/RDB

                 or a CDDplus path name, such as 
                 cdd$top.employee/CDD

                 Note the "/RDB" and "/CDD" at the end of the examples.
                 These must be provided by user when location is requested.
                 They are used as markers to determine whether connect to
                 Rdb through Rdb filename or CDD pathname.

$ GWDBLOOP:
$     inquire gwdbname " Enter Ingres style database name"
$     if (gwdbname .eqs. "") then goto SETUP_END
$     inquire gwsid " Enter RDB database location"
$     if (gwsid .eqs. "") then goto SETUP_END
$     gwcat = gwsid + "," + ","
$     iisetres ii.'node_name.gateway.RDB.gwalias.'gwdbname  'gwcat
$     inquire gwdone " Enter another database ? [y/n]"
$     if (gwdone .eqs. "Y") then goto GWDBLOOP
$ 
$ SETUP_END:
$!  
$! Now we copy excutables and install shared images
$!
$    ii_installation = f$trnlnm("II_INSTALLATION")
$    if( ii_installation .nes. "" )
$    then
$       copy ii_system:[ingres.bin]iigwrdb.exe -
            ii_system:[ingres.bin]iigwrdb'ii_installation.exe
$    endif
$!
$!   After determing II_INSTALLATION, establish names of
$!   share libraries and their logicals.
$!
$    if( ii_installation .nes. "" )
$    then
$       echo ""
$       echo "II_INSTALLATION configured as ''ii_installation'."
$!
$       copy ii_system:[ingres.library]clfelib.exe -
                 ii_system:[ingres.library]clfelib'ii_installation.exe
$       iisetres ii.'node_name.lnm.ii_compatlib "ii_system:[ingres.library]clfelib''ii_installation'.exe"
$       copy ii_system:[ingres.library]framefelib.exe -
                 ii_system:[ingres.library]framefelib'ii_installation.exe
$       iisetres ii.'node_name.lnm.ii_framelib "ii_system:[ingres.library]framefelib''ii_installation'.exe" 
$       copy ii_system:[ingres.library]interpfelib.exe -
                 ii_system:[ingres.library]interpfelib'ii_installation.exe
$       iisetres ii.'node_name.lnm.ii_interplib "ii_system:[ingres.library]interpfelib''ii_installation'.exe" 
$       copy ii_system:[ingres.library]libqfelib.exe -
                 ii_system:[ingres.library]libqfelib'ii_installation.exe
$       iisetres ii.'node_name.lnm.ii_libqlib "ii_system:[ingres.library]libqfelib''ii_installation'.exe" 
$       copy ii_system:[ingres.library]apifelib.exe -
	         ii_system:[ingres.library]apifelib'ii_installation.exe
$       iisetres ii.'node_name.lnm.ii_apilib "ii_system:[ingres.library]apifelib''ii_installation'.exe"
$       define/nolog/job ii_compatlib ii_system:[ingres.library]clfelib'ii_installation.exe
$       define/nolog/job ii_libqlib ii_system:[ingres.library]libqfelib'ii_installation.exe
$       define/nolog/job ii_framelib ii_system:[ingres.library]framefelib'ii_installation.exe
$    else
$       ii_installation = "AA"
$!
$       define/nolog/job ii_compatlib ii_system:[ingres.library]clfelib.exe
$       define/nolog/job ii_libqlib ii_system:[ingres.library]libqfelib.exe
$       define/nolog/job ii_framelib ii_system:[ingres.library]framefelib.exe
$    endif
$    @ii_system:[ingres.utility]iishare add
$!
$! Make sure ingstart invokes image installation
$!
$ iisetres ii.'node_name.ingstart.*.begin "@ii_system:[ingres.utility]iishare"
$!
$ iisetres ii.'node_name.config.rdb.'release_id "COMPLETE"
$!
$ say

RDB gateway installation is complete.  Please make sure that the
user "ingres" is created with all privileges in the RDB database before
using RDB gateway to access data in the RDB database.

$!
$ EXIT_OK:
$ if f$search("ii_system:[ingres]iijobdef.com;*").nes."" then -
  delete/noconfirm ii_system:[ingres]iijobdef.com;*              
$ status = SS$_NORMAL
$ goto exit_ok1
$!
$ EXIT_FAIL:
$ status = SS$_ABORT
$exit_ok1:
$!
$ on control_c then goto EXIT_FAIL
$ on control_y then goto EXIT_FAIL
$!
$! Restore previous values of shared library definitions
$!
$ set_message_off
$ if "''ii_compatlib_def'".eqs."" 
$ then
$     deassign/job ii_compatlib
$ else
$     define/job/nolog ii_compatlib "''ii_compatlib_def'"
$ endif
$ if "''ii_libqlib_def'".eqs."" 
$ then
$     deassign/job ii_libqlib
$ else
$     define/job/nolog ii_libqlib "''ii_libqlib_def'"
$ endif
$ if "''ii_framelib_def'".eqs."" 
$ then
$     deassign/job ii_framelib
$ else
$     define/job/nolog ii_framelib "''ii_framelib_def'"
$ endif
$ set_message_on
$!
$ if iisulocked  .and. f$search("ii_system:[ingres.files]config.lck;*").nes."" then -
        delete/noconfirm ii_system:[ingres.files]config.lck;*
$!
$ set on
$ exit 'status'
