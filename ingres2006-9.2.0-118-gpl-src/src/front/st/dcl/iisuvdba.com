$!----------------------------------------------------------------------------
$!
$!  Copyright (c) 2004 Ingres Corporation
$!
$!  Name:
$!	iisuvdba -- set up the Ingres VDBA Database
$!
$!  Usage:
$!      iisuvdba
$!
$!  Description:
$!      This script is called by VMSINSTAL to set up the Ingres VDBA
$!	database..
$!      It assumes that II_SYSTEM has been set in the logical name table 
$!      (SYSTEM or GROUP) which is to be used to configure the installation
$!	or in the JOB logical table.
$!
$!!  History:
$!!	19-Nov-1997 (kinte01)
$!!		Created.
$!!	04-Dec-1997 (kinte01)
$!!		Changed the name of the sql file that populates the imadb 
$!!		database
$!!	17-Dec-1997 (kinte01)
$!!		Ensured that the user symbols are defined before populating 
$!!		the ima database
$!!	20-apr-98 (mcgem01)
$!!		Product name change to Ingres.
$!!	28-may-1998 (kinte01)
$!!	    Modified the command files placed in ingres.utility so that
$!!	    they no longer include history comments.
$!!	04-sep-1998 (kinte01)
$!!	    remove setup of imadb
$!!      24-mar-2000 (devjo01)
$!!          Make gen of scratch file names safe for Clustered Ingres.
$!!      18-Jun-2001 (horda03)
$!!          Only allow Ingres System Administrator of the VMSINSTAL script
$!!          to invoke the IVP scripts.
$!!	25-oct-2001 (kinte01)
$!!	    Add symbol definition for iiingloc as it is utilized in the
$!!	    all.crs file
$!!	05-nov-2003 (kinte01)
$!!	    Remove iiruncsp as there is no separate DMFCSP process with 
$!!	    the Portable Clustered Ingres project
$!!	14-dec-2004 (abbjo03)
$!!	    Replace history comments.
$!----------------------------------------------------------------------------
$!
$ on control_c then goto EXIT_FAIL
$ on control_y then goto EXIT_FAIL
$ set noon
$ set noverify
$!
$! Define symbols for programs called locally.
$!
$ createdb	:= $ii_system:[ingres.bin]createdb.exe
$ iigenres	:= $ii_system:[ingres.utility]iigenres.exe
$ iigetres	:= $ii_system:[ingres.utility]iigetres.exe
$ iigetsyi	:= @ii_system:[ingres.utility]iigetsyi.com
$ iiingloc      := $ii_system:[ingres.utility]iiingloc.exe
$ iipmhost      := $ii_system:[ingres.utility]iipmhost.exe
$ iimaninf	:= $ii_system:[ingres.utility]iimaninf.exe
$ iimklog	:= $ii_system:[ingres.utility]iimklog.exe
$ iircpseg	:= @ii_system:[ingres.utility]iircpseg.com
$ iiresutl	:= $ii_system:[ingres.utility]iiresutl.exe
$ iirunacp	:= @ii_system:[ingres.utility]iirunacp.com
$ iirundbms	:= $ii_system:[ingres.utility]iirundbms.exe
$ iirungcc	:= @ii_system:[ingres.utility]iirungcc.com
$ iirungcn	:= @ii_system:[ingres.utility]iirungcn.com
$ iishowres	:= $ii_system:[ingres.bin]iishowres.exe
$ iisetres	:= $ii_system:[ingres.utility]iisetres.exe
$ iisulock	:= $ii_system:[ingres.utility]iisulock.exe
$ iisyschk	:= @ii_system:[ingres.utility]iisyschk.com
$ iivalres	:= $ii_system:[ingres.utility]iivalres.exe
$ ingstart	:= $ii_system:[ingres.utility]ingstart.exe
$ ingstop	:= @ii_system:[ingres.utility]iisdall.com
$ rcpconfig	:= $ii_system:[ingres.bin]rcpconfig.exe
$ rcpstat	:= $ii_system:[ingres.bin]rcpstat.exe
$ upgradedb	:= $ii_system:[ingres.bin]upgradedb.exe
$!
$! Other variables, macros and temporary files
$!
$ iisulocked         = 0				  ! Do we hold the setup lock?
$ ii_compatlib_def   = f$trnlnm("ii_compatlib","LNM$JOB") ! Current values
$ ii_libqlib_def     = f$trnlnm("ii_libqlib","LNM$JOB")   ! for shared
$ ii_framelib_def    = f$trnlnm("ii_framelib","LNM$JOB")  ! library definitions
$ saved_message      = f$environment( "MESSAGE" )
$ set_message_off    = "set message/nofacility/noidentification/noseverity/notext"
$ set_message_on     = "set message''saved_message'"
$ SS$_NORMAL         = 1
$ SS$_ABORT          = 44
$ status             = SS$_NORMAL
$ node_name	     = f$getsyi("NODENAME")
$ tmpfile            = "ii_system:[ingres]iisuvdba.''node_name'_tmp" !Temp file
$
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
$
$!
$  say		:= type sys$input
$  echo		:= write sys$output 
$!
$! Make temporary definitions for shared library logicals to 
$! be the unconcealed path of the library directory within ii_system
$! 
$ define/nolog/job ii_compatlib 'f$parse("ii_system:[ingres.library]clfelib.exe",,,,"NO_CONCEAL")
$ define/nolog/job ii_libqlib   'f$parse("ii_system:[ingres.library]libqfelib.exe",,,,"NO_CONCEAL")
$ define/nolog/job ii_framelib  'f$parse("ii_system:[ingres.library]framefelib.exe",,,,"NO_CONCEAL")
$ if f$search("ii_system:[ingres]iistartup.del") .nes. "" then @ii_system:[ingres]iistartup.del
$!
$! Define ii_config for the installation configuration files
$!
$ define/nolog/process ii_config    "ii_system:[ingres.files]"
$ set_message_on
$!
$! Look up II_DATABASE.
$!
$ iigetres ii.*.lnm.ii_database ii_database_value
$ set_message_off
$ ii_database = f$trnlnm( "ii_database_value" )
$ deassign "ii_database_value"
$ if( ii_database .eqs. "" ) then -
     ii_database = f$trnlnm( "II_DATABASE", "LNM$JOB" )
$ set_message_on
$!
$ echo ""
$ echo "Setting up the Ingres VDBA..."
$!
$! Create ii_system:[ingres.files]config.dat if non-existent. 
$!
$ if( f$search( "ii_system:[ingres.files]config.dat" ) .eqs. "" )
$ then
$    copy nl: ii_system:[ingres.files]config.dat/prot=(s:rwed,o:rwed,g:rwed,w:re)
$ endif
$ set file /version_limit=20 ii_system:[ingres.files]config.dat
$ purge/nolog ii_system:[ingres.files]config.dat
$!
$! Lock the configuration file. 
$!
$ on error then goto EXIT_FAIL 
$ iisulock "Ingres VDBA setup"
$ iisulocked = 1
$ set noon
$!
$! Get compressed release ID.
$!
$ on error then goto EXIT_FAIL
$ iimaninf -version=dbms -output='tmpfile
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
$! Check if it has DBMS
$!
$ set_message_off
$ iigetres ii.'node_name.config.dbms.'release_id dbms_setup_status
$ dbms_setup_status = f$trnlnm( "dbms_setup_status" )
$ deassign "dbms_setup_status"
$ set_message_on
$ if (dbms_setup_status .eqs. "") 
$ then 
$    ! not a server not a net
$    say 

This installation of Ingres doesn't have a DBMS server configured.

Ingres VDBA can only be set up for server installations.
  
$    goto EXIT_FAIL
$ endif
$!
$! Check if Ingres VDBA is already set up.
$!
$ set_message_off
$ iigetres ii.'node_name.config.vdba.'release_id vdba_setup_status
$ vdba_setup_status = f$trnlnm( "vdba_setup_status" )
$ deassign "vdba_setup_status"
$ set_message_on
$ if( (vdba_setup_status .nes. "") .and. (vdba_setup_status .eqs. "COMPLETE") )
$ then
$    echo ""
$    echo "Ingres VDBA has already been set up on node ''node_name'."
$    echo ""
$    goto EXIT_OK
$ endif
$!
$! Check if DBMS is setup on this node 
$!
$ if( dbms_setup_status .nes. "COMPLETE" )
$ then
$    say 
 
The setup procedure for the following version of the Ingres DBMS:
 
$    echo "	''version_string'"
$    say
 
must be completed up before Ingres VDBA can be set up on this node:
 
$    echo "	''node_name'"
$    echo ""
$    goto EXIT_FAIL
$ endif
$!
$  say 
 
This procedure will set up the following version of Ingres VDBA:
 
$  echo "	''version_string'"
$  say
 
to run on node:
 
$  echo "	''node_name'"
$  echo ""
$! 
$ set noon
$ set_message_off
$   say 
                                  
Ingres VDBA has been successfully set up in this installation.

$   iisetres ii.'node_name.config.vdba.'release_id "COMPLETE"
$   goto EXIT_OK
$ EXIT_OK:
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
$!
