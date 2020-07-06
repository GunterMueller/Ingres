$!----------------------------------------------------------------------------
$!
$!  Copyright (c) 2006 Ingres Corporation.  All Rights Reserved.
$!
$!  Name:
$!	iisuoracle -- set-up script for Ingres II Enterprise Access to Oracle
$!
$!  Usage:
$!	iisuoracle
$!
$!  Description:
$!	This script is called by VMSINSTAL to set up Ingres II Enterprise
$!	Access to Oracle.
$!	It assumes that II_SYSTEM has been set in the logical name table 
$!	(SYSTEM or GROUP) which is to be used to configure the installation.
$!      The logicals set up for Oracle gateway will be at either LOGICAL or
$!      sYSTEM level, depending upon the level of Ingres installation.
$!
$!!  History:
$!!      19-may-1999 (chash01)
$!!          edit to make setting database alias correct.
$!!	21-may-1999 (kinte01)
$!!	    Before checking version change the location of the manifest
$!!	    directory to ensure we are pointing to the gateway release.dat
$!!	    This is only necessary in 2.0 as the gateways will be included
$!!	    as part of 2.5
$!!      03-jun-1999 (chash01)
$!!          make multiple database entries work correctly.  Add message
$!!          to remind user create INGRES user in Oracle and define ORA_ROOT
$!!          if Oracle is installed at GROUP level.
$!!	06-jul-1999 (kinte01)
$!!	    rename oracle GW shared executable if installing the same version
$!!	    for the second time (could be upgrading from one beta to another
$!!	    beta or from the beta to GA version) (98247)
$!!      24-mar-2000 (devjo01)
$!!          Make gen of scratch file names safe for Clustered Ingres.
$!!      11-oct-2000 (chash01)
$!!          multiple versions of oracle gateway installation.
$!!      01-nov-2000 (chash01)
$!!          change strategy.  Do not delete other executables so that
$!!          user can upgrade to other oracle version easier later.
$!!      13-nov-2000 (chash01)
$!!          add more message to inform users what to do before running
$!!         iigwcatora. 
$!!	17-apr-2001 (kinte01)
$!!	    No longer define ii_manifest_dir (104584)
$!!	08-may-2001 (kinte01)
$!!	    For VAX VMS the only oracle version is 732
$!!      10-may-2001 (chash01)
$!!          Add inquiry for BLOB data handling; oracle_home and oracle_sid
$!!          are now ora_root and ora_sid (consistent with Oracle logical.)
$!!          and need not be provided by user if ingsysdef is first executed.
$!!          ora_sid can be determined by getting the db entry from config.dat.
$!!	21-may-2001 (kinte01)
$!!	    If a value is not specified for Oracle Root, the setup process
$!!	    is terminated however instead of immediately exiting the
$!!	    finishing touches are attempted which should not be done.
$!!	    Clean up spelling and prompt handling
$!!      11-jun-2001 (chash01)
$!!          add oracle 817 for Alpha vms
$!!	13-jun-2001 (kinte01)
$!!	    Get the ii_installation value from the config.dat file and not
$!!	    the local environment (104979)
$!!      18-Jun-2001 (horda03)
$!!          Only allow Ingres System Administrator of the VMSINSTAL script
$!!          to invoke the IVP scripts.
$!!      01-aug-2001 (chash01)
$!!          bug 105426
$!!          copy executable and re-establish share image even if user choose
$!!          NO to continue installation.  This accomplishes the purpose of 
$!!          product update without entire reinstallation.
$!!          Save oracle version in config.dat
$!!	14-dec-2004 (abbjo03)
$!!	    Replace history comments.
$!----------------------------------------------------------------------------
$!
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
$ iipmhost      := $ii_system:[ingres.utility]iipmhost.exe
$ iimaninf	:= $ii_system:[ingres.utility]iimaninf.exe
$ iiremres	:= $ii_system:[ingres.utility]iiremres.exe
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
$ tmpfile            = "ii_system:[ingres]iisuoracle.''node_name'_tmp" !Temp file
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
$!
$ echo "Setting up Ingres II Enterprise Access to Oracle"
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
$ iisulock "Ingres II Enterprise Access to Oracle setup"
$ iisulocked = 1
$ set noon
$!
$!!!!!! Get compressed release ID.
$!
$ on error then goto EXIT_FAIL
$ iimaninf -version=oracle -output='tmpfile
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
$! Check if Ingres Gateway to Oracle is already set up.
$!
$ set_message_off
$ sysmodel = f$getsyi("hw_model") 
$ iigetres ii.'node_name.config.oracle.'release_id ora_setup_status
$ ora_setup_status = f$trnlnm( "ora_setup_status" )
$ deassign "ora_setup_status"
$ iigetres ii.'node_name.gateway.oracle.version  ora_version
$ ora_version = f$trnlnm( "ora_version" )
$ deassign "ora_version"
$ set_message_on
$ if( ( ora_setup_status .nes. "" ) .and. -
     ( ora_setup_status .eqs. "COMPLETE" ) )
$ then
$    say 

The latest version of Ingres II Enterprise Access to Oracle 
has already been set up to run on local
$    echo "node ''node_name'."
$    echo ""
$    inquire/punct to_cont "Do you want to continue? (answer YES or NO)"
$    if (to_cont .eqs. "NO")
$    then
$      if ( ora_version .eqs. "" )
$      then
$        if sysmodel .lt. 1024
$        then
$          ora_version = 732
$          iisetres ii.'node_name.gateway.oracle.version  'ora_version
$        else
$          say

Your installation does not have an Oracle version number, you MUST provide one.
$   inquire ora_version " Enter version of oracle, current choices are 732, 805, 816, 817"
$          if ( ora_version .nes. "" )
$          then
$              iisetres ii.'node_name.gateway.oracle.version  'ora_version
$          else
$              say

Insufficient information to update gateway executables, Installation process
exits. Please obtain your Oracle version number and rerun IISUORACLE.COM.
$              goto EXIT_OK
$          endif
$        endif
$      endif
$!
$! we still need to copy updated executables and shared images 
$! to their repective locations.  Therefore, instead of going to
$! EXIT_OK, we go to FINISH_TOUCH. Note that 'oraver' will be 
$! used in FINISH_TOUCH
$!
$      oraver = ora_version
$      say
The installation process now copies new gateway executables
$      goto FINISH_TOUCH
$    endif
$ endif
$!
$ say 

Generating default configuration...

$       on error then goto IIGENRES_SERVER_FAILED
$       iigenres 'node_name ii_system:[ingres.files]oracle.rfm 
$       set noon
$       iisetres ii.'node_name.config.oracle.'release_id "DEFAULTS"
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
$! ask users how they want blob being handled.
$!
$ BLOB_STORAGE:
$       say

This release of Ingres II  Enterprise Access to Oracle can handle large 
objects. There are two ways to store intermediate large objects, in 
memory or in a temporary file, before storing into an Oracle database.

$ inquire memfile "Large Object intermediate storage mechanism (MEMORY or FILE)"
$ if ((f$edit(memfile,"UPCASE") .eqs. "MEMORY") .or. -
     (f$edit(memfile,"UPCASE") .eqs. "FILE")) 
$ then      
$    iisetres ii.'node_name.gateway.oracle.blob_data  'memfile
$ else
$   goto BLOB_STORAGE
$ endif
$!
$! now ask input from user about database name, sid, user, password
$!
$ say

Please enter values for oracle root, database name, sid, username, 
and password.

Enter Carriage Return to terminate at any time.  The value for oracle root 
(ora_root) should point to the Oracle installation that will be accessed.

$ inquire oraroot " Enter location of ora_root, example: $8$dkb200:[oracle.]"
$ if (oraroot .eqs."") then goto SETUP_END
$ iisetres ii.'node_name.lnm.ora_root 'oraroot
$!
$ if sysmodel .lt. 1024
$ then
$   oraver = 732
$ else
$   inquire oraver " Enter version of oracle, current choices are 732, 805, 816, 817"
$ endif
$ if (oraver .eqs. "") then goto SETUP_END
$!
$! if oracle version (ora_version) from config.dat is different from
$! that given by user (oraver), we need to delete all entries in config.dat
$! that are related to the old oracle version.
$!
$ if (oraver .nes. ora_version)
$ then
$    if (ora_version .eqs. "732")
$    then
$      iiremres ii.'node_name.lnm.ORA_COREV732
$      iiremres ii.'node_name.lnm.ORA_RDBMSV732
$      iiremres ii.'node_name.lnm.ORA_UPISHRV732
$      iiremres ii.'node_name.lnm.ORA_SQLLIBSHRV732
$      iiremres ii.'node_name.lnm.ORA_SRV2V732
$!
$!     extras for alpha version
$!
$      if (sysmodel .ge. 1024)
$      then
$        iiremres ii.'node_name.lnm.ORA_CORE_64V732
$        iiremres ii.'node_name.lnm.ORA_RDBMS_64V732
$        iiremres ii.'node_name.lnm.ORA_UPISHR_64V732
$      endif
$    else
$      iiremres ii.'node_name.lnm.ORACLIENT64_V'ora_version
$      iiremres ii.'node_name.lnm.ORACLIENT_V'ora_version
$    endif
$!
$! do we also need to remove database alias?
$! Do it here Can we remove multiple lines using
$! wildcard (*)?
$!
$!    iiremres ii.'node_name.gateway.oracle.gwalias.*
$ endif 
$
$! 
$! store version number into config.dat
$!
$ iisetres ii.'node_name.gateway.oracle.version  'oraver
$!
$! Now define version specific logicals.
$!
$  oracle_root = oraroot - "]"
$ if (oraver .eqs. "732")
$ then
$  if f$getsyi("hw_model") .lt. 1024
$  then
$    symval = oracle_root + "RDBMS]COREV732.EXE"
$    iisetres ii.'node_name.lnm.ORA_COREV732  'symval
$    symval = oracle_root + "RDBMS]ORACLEV732.EXE"
$    iisetres ii.'node_name.lnm.ORA_RDBMSV732  'symval
$    symval = oracle_root + "RDBMS]UPISHRV732.EXE"
$    iisetres ii.'node_name.lnm.ORA_UPISHRV732  'symval
$  else
$    symval = oracle_root + "UTIL]COREV732.EXE"
$    iisetres ii.'node_name.lnm.ORA_COREV732  'symval
$    symval = oracle_root + "UTIL]ORACLEV732.EXE"
$    iisetres ii.'node_name.lnm.ORA_RDBMSV732  'symval
$    symval = oracle_root + "UTIL]UPISHRV732.EXE"
$    iisetres ii.'node_name.lnm.ORA_UPISHRV732  'symval
$!
$    symval = oracle_root + "RDBMS]COREV732_64.EXE"
$    iisetres ii.'node_name.lnm.ORA_CORE_64V732  'symval
$    symval = oracle_root + "RDBMS]ORACLEV732_64.EXE"
$    iisetres ii.'node_name.lnm.ORA_RDBMS_64V732  'symval
$    symval = oracle_root + "RDBMS]UPISHRV732_64.EXE"
$    iisetres ii.'node_name.lnm.ORA_UPISHR_64V732  'symval
$  endif
$  symval = oracle_root + "UTIL]SQLLIBSHRV732.EXE"
$  iisetres ii.'node_name.lnm.ORA_SQLLIBSHRV732  'symval
$  symval = oracle_root + "RDBMS]SRV2.EXE"
$  iisetres ii.'node_name.lnm.ORA_SRV2V732  'symval
$ endif
$!
$ if (oraver .eqs. "805")
$ then
$    symval = oracle_root + "RDBMS]ORACLIENT64_V805.EXE"
$    iisetres ii.'node_name.lnm.ORACLIENT64_V805  'symval
$    symval = oracle_root + "UTIL]ORACLIENT_V805.EXE"
$    iisetres ii.'node_name.lnm.ORACLIENT_V805  'symval
$ endif  
$!
$ if (oraver .eqs. "816")
$ then
$    symval = oracle_root + "RDBMS]ORACLIENT64_V816.EXE"
$    iisetres ii.'node_name.lnm.ORACLIENT64_V816  'symval
$    symval = oracle_root + "UTIL]ORACLIENT_V816.EXE"
$    iisetres ii.'node_name.lnm.ORACLIENT_V816  'symval
$ endif  
$ if (oraver .eqs. "817")
$ then
$    symval = oracle_root + "RDBMS]ORACLIENT64_V817.EXE"
$    iisetres ii.'node_name.lnm.ORACLIENT64_V817  'symval
$    symval = oracle_root + "UTIL]ORACLIENT_V817.EXE"
$    iisetres ii.'node_name.lnm.ORACLIENT_V817  'symval
$ endif  
$ GWDBLOOP:
$     inquire gwdbname " Enter Oracle database name alias"
$     if (gwdbname .eqs. "") then goto SETUP_END
$     inquire gwsid " Enter Oracle database SID"
$     if (gwsid .eqs. "") then goto SETUP_END
$     inquire gwuser " Enter user name for connection to Oracle"
$     if (gwuser .eqs. "") then goto SETUP_END
$     inquire gwpwd " Enter password for the user"
$     if (gwpwd .eqs."") then goto SETUP_END
$     gwcat = gwsid + "," + gwuser + "," + gwpwd +"," + "," + oraroot
$     iisetres ii.'node_name.gateway.oracle.gwalias.'gwdbname  'gwcat
$     inquire notdone " Enter another database ? [y/n]"
$     if (notdone .eqs. "Y") then goto GWDBLOOP
$     goto FINISH_TOUCH       
$ SETUP_END:
$ say

you have terminated the process of creating database alias entries in
CONFIG.DAT.  You may run the utility GWALIAS to continue the this 
process at later time.

$ goto EXIT_OK
$ FINISH_TOUCH:
$!  
$! Now we copy excutables and install shared images
$!
$    iigetres ii.'node_name.lnm.ii_installation installid
$    ii_installation = f$trnlnm ("installid")
$    deassign "installid"
$    ii_installation = f$trnlnm("II_INSTALLATION")
$    copy ii_system:[ingres.bin]iigwora_'oraver.exe -
            ii_system:[ingres.bin]iigwora'ii_installation.exe
$    copy ii_system:[ingres.utility]iigwcatora_'oraver.exe -
            ii_system:[ingres.utility]iigwcatora.exe
$!
$!   After determining II_INSTALLATION, establish names of
$!   share libraries and thir logicals.
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
$ iisetres ii.'node_name.config.oracle.'release_id "COMPLETE"
$!
$ say

Ingres II Enterprise Access to Oracle has been successfully set up in this 
installation.

The user "ingres" must be created with all privileges in the Oracle database 
before using the Ingres II Enterprise Access to Oracle for accessing data in 
the Oracle database.

You can now use the "ingstart" command to start the Ingres II Enterprise 
Access to Oracle installation.  All necessary Oracle logicals utilized by 
Ingres II Enterprise Access to Oracle will be defined. 

The utility "iigwcatora" must be executed to create the necessary Enterprise 
Access system catalogs in the Oracle database.  The logicals ORACLE_HOME and 
ORA_SID need to be defined in order to execute "iigwcatora".   If the account 
to administer and manage the Ingres II installation is different from that of 
the Oracle installation, be sure to run ora_root:[util]orauser.com before 
running IIGWCATORA.

Refer to the Ingres II Enterprise Access Installation and Operations Guide 
for more information about starting and using Ingres II Enterprise Access to 
Oracle.


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
$ if f$search( "''tmpfile'" ) then -
	delete/noconfirm 'tmpfile;* 
$ if iisulocked  .and. f$search("ii_system:[ingres.files]config.lck;*").nes."" then -
        delete/noconfirm ii_system:[ingres.files]config.lck;*
$!
$ set on
$ exit 'status'
