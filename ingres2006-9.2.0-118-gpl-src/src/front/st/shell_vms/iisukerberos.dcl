$!
$!  Copyright (c) 2002, 2007 Ingres Corporation
$!
$!  Name: iisukerberos -- setup script for Ingres Kerberos Driver
$!  Usage:
$!	iisukerberos
$!
$!  Description:
$!	This script should only be called by the Ingres installation
$!	utility.  It sets up the Ingres Kerberos Driver.
$!
$!  Exit Status:
$!	0	setup procedure completed.
$!	1	setup procedure did not complete.
$!
$!! History:
$!!	08-Jan-2002 (loera01)
$!!		Created.
$!!      28-Mar-2002 (loera01)
$!!              Ported to VMS.
$!!      21-Nov-2002 (loera01)
$!!              Use "search" instead of "grep" for purging Kerberos lines
$!!              from config.dat.
$!!	14-dec-2004 (abbjo03)
$!!	    Replace history comments.
$!!   17-may-2006 (abbjo03)
$!!       Remove double quotes from II_CONFIG process level definition.
$!!   22-Aug-2006 (Fei Ge) Bug 116528
$!!       Removed reference to ca.com.
$!!   02-Jan-2006 (loera01) Bug 117418 
$!!       Added reference to iisulock.
$!!   03-Aug-2007 (Ralph Loen) SIR 118898
$!!       Use iinethost to obtain the default domain name.
$!!   10-Aug-2007 (Ralph Loen) SIR 118898
$!!      Remove iigenres items, as they are now added from all.crs.
$!!      Prompting for domain is no longer necessary, since the
$!!      iinethost utility should enter the correct host name into
$!!      config.dat.  Iisukerberos now prompts for three main
$!!      configurations.
$!!  20-Aug-2007 (Ralph Loen) SIR 118898
$!!      Removed infinite loop due to invalid conditional for
$!!      evaluation of the client "remove" option.
$!!      
$!
$!  PROGRAM = iisukerberos
$!
$!  DEST = utility
$!----------------------------------------------------------------------------

$ on control_c then goto EXIT_FAIL
$ on control_y then goto EXIT_FAIL
$ set noon
$ !set noverify
$!
$! Define symbols for programs called locally.
$!
$ iigenres      := $II_system:[ingres.utility]iigenres.exe
$ iigetres      := $II_system:[ingres.utility]iigetres.exe
$ iigetsyi      := @ii_system:[ingres.utility]iigetsyi.com
$ iiingloc      := $II_system:[ingres.utility]iiingloc.exe
$ iipmhost      := $II_system:[ingres.utility]iipmhost.exe
$ iinethost     := $II_system:[ingres.utility]iinethost.exe
$ iimaninf      := $II_system:[ingres.utility]iimaninf.exe
$ iiresutl      := $II_system:[ingres.utility]iiresutl.exe
$ iisetres      := $II_system:[ingres.utility]iisetres.exe
$ iivalres      := $II_system:[ingres.utility]iivalres.exe
$ iisulock      := $II_system:[ingres.utility]iisulock.exe
$ iisulocked         = 0                             !Do we hold the setup lock?
$ saved_message      = f$environment( "MESSAGE" )    ! save message settings
$ set_message_off    = "set message/nofacility/noidentification/noseverity/notext"
$ set_message_on     = "set message''saved_message'"
$ SS$_NORMAL	     = 1
$ SS$_ABORT	     = 44
$ status	     = SS$_NORMAL
$ node_name	     = f$getsyi("NODENAME")
$ tmpfile            = "ii_system:[ingres]iisukerberos.''node_name'_tmp" !Temp file
$!
$! Define ii_config for the installation configuration files
$!
$ define/nolog/process II_CONFIG    II_SYSTEM:[ingres.files]
$ say		:= type sys$input
$ echo		:= write sys$output 
$ user_name     = f$trnlnm( "II_SUINGRES_USER","LNM$JOB" ) !Installation owner
$ user_uic	= f$user() !For RUN command (detached processes)
$ iinethost 'tmpfile
$ open iinethostout 'tmpfile
$ read iinethostout full_host_name
$ close iinethostout
$ delete 'tmpfile;*
$!
$ goto START
$!
$! Exit module.
$!
$!
$ EXIT_FAIL:
$ status = SS$_ABORT
$ EXIT_OK:
$!
$ on control_c then goto EXIT_FAIL
$ on control_y then goto EXIT_FAIL
$ if iisulocked  .and. f$search("ii_system:[ingres.files]config.lck;*").nes."" then -
        delete/noconf/nolog ii_system:[ingres.files]config.lck;*
$ say 

Kerberos configuration complete.  Be sure the GSS API library, 
$ echo "SYS$LIBRARY:GSS$RTL32.EXE, is installed on ''full_host_name'."
say

You may use the cbf utility to fine-tune Kerberos security options.

$ exit
$ START:
$ IF "''user_name'".EQS.""
$ THEN
$    isa_uic = f$file_attributes( "II_SYSTEM:[000000]INGRES.DIR", "UIC")
$    if isa_uic .nes. user_uic
$    then
$       say

     ---------------------------------------------------------
    | This setup procedure must be run from the Ingres System |
    | Administrator account.                                  |
     ---------------------------------------------------------
$
$ goto EXIT_FAIL
$ endif
$ user_name= f$getjpi("","USERNAME") !User running this procedure
$ ENDIF
$ ivp_name= f$getjpi("","USERNAME")
$ ivp_user = f$edit( ivp_name, "TRIM,LOWERCASE" )
$ ing_user = f$edit( user_name, "TRIM,LOWERCASE" ) !Ingres version of username
$ if ( "''p1'" .eqs. "-RMPKG" )
$ then
$ copy ii_config:config.dat ii_config:config.tmp
$ on error then copy ii_config:config.tmp -
      ii_config:config.dat
$ pipe search ii_config:config.dat "gcf.mech.kerberos"/match=nor, -
       "kerberos_driver"/match=nor >  ii_config:config.new
$ if (f$search("ii_config:config.new") .nes. "") 
$ then 
$     copy ii_config:config.new ii_config:config.dat
$ else
$     say
    There was a problem rewriting the configuration file 
    II_SYSTEM:[INGRES.FILES]CONFIG.DAT.  Please check that the
    file exists, is readable, and that the logical II_SYSTEM is
    defined.
$ goto EXIT_FAIL
$ endif
$ set noon
$ del/nol ii_config:config.tmp;*,config.new;*
$ say

  Ingres Kerberos Driver has been removed.

$ goto EXIT_OK
$ else
$ echo "Setting up the Ingres Kerberos Driver..."
$!
$! Create ii_system:[ingres.files]config.dat if non-existent. 
$!
$ if( f$search( "ii_system:[ingres.files]config.dat" ) .eqs. "" )
$ then
$    copy nl: ii_system:[ingres.files]config.dat -
	/prot=(s:rwed,o:rwed,g:rwed,w:re)
$ endif
$ set file /version_limit=20 ii_system:[ingres.files]config.dat
$ purge ii_system:[ingres.files]config.dat
$!
$! Lock the configuration file. 
$!
$ on error then goto EXIT_FAIL
$ iisulock "Ingres Kerberos setup"
$ iisulocked = 1
$ set noon
$!
$! Get compressed release ID.
$!
$ on error then goto EXIT_FAIL
$ iimaninf -version=basic -output='tmpfile
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
$! Run IIJOBDEF.COM if it exists. 
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
$ endif
$ set_message_on
$!
$ CONFIRM_SERVER_SETUP:
$    say 
 
This procedure will set up the following version of Ingres Kerberos Driver:
 
$    echo "	''version_string'"
$    say
 
to run on local node:
 
$    echo "	''full_host_name'"
$    say 
 
$ inquire/nopunc answer "Do you wish to continue this setup procedure? [y] "
$ if answer .eqs. "Y" .or. answer .eqs. "" then goto GENRES 
$ if answer .nes. "Y" .and. answer .nes. "N" 
$ then
$   echo "Entry must be 'y' or 'n' "
$   inquire junk "Hit RETURN continue"
$   goto CONFIRM_SERVER_SETUP
$ endif
$ if( .not. answer ) then goto EXIT_FAIL
$ GENRES:
$! 
$! Generate default configuration resources in case iisukerberos was
$! previously called with the "-rmpkg" qualifier.
$!
echo ""
echo "Generating default configuration..."
$ on error then goto GENRES_FAILED	
$ iigenres 'node_name ii_config:net.rfm 
$ goto GENRES_OK
$ GENRES_FAILED:
$ say
An error occurred while generating the default configuration.


$ goto EXIT_FAIL
$ GENRES_OK:
$ say

Default configuration generated.

$ MAINDISP:
$ say
Basic Kerberos Configuration Options

1.  Client Kerberos configuration
2.  Name Server Kerberos configuration
3.  Server-level Kerberos configuration
0.  Exit

$ status = SS$_NORMAL
$ inquire/nopunc answer "Select from [0 - 3] above [0]: "
$ if answer .eqs. "" .or. answer .eqs. "0" then goto EXIT_OK
$ if answer .eqs. "1" then gosub CLIENT_CONFIG
$ if answer .eqs. "2" then gosub NS_CONFIG
$ if answer .eqs. "3" then gosub SERVER_CONFIG
$ goto MAINDISP
$ CLIENT_CONFIG:
$!
$ say
Client configuration enables this installation to use Kerberos to authenticate
against a remote installation that has been configured to use Kerberos for
authentication.  This is the mimimum level of Kerberos authentication.

$ PROMPT1:
$ echo "Select 'a' to add client-level Kerberos authentication,"
$ inquire/nopunc resp "Select 'r' to remove client-level Kerberos authentication: "
$ if resp .eqs. "A" 
$ then 
$   iisetres ii.'node_name.gcf.mechanisms "kerberos"
$ say

Client Kerberos configuration has been added.

You must add the "authentication_mechanism" attribute in netutil for
each remote node you wish to authenticate using Kerberos.  The
"authentication_mechanism" attribute must be set to "kerberos".  There
is no need to define users or passwords for vnodes using Kerberos
authentication.

$ endif
$ if resp .eqs. "R" 
$ then 
$   iisetres ii.'node_name.gcf.mechanisms ""
$   echo ""
$   echo "Client Kerberos authentication has been removed."
$   echo ""
$ endif
$ if resp .nes. "A" .and. resp .nes. "R" 
$ then
$ echo "Entry must be 'a' or 'r' "
$ inquire junk "Hit RETURN continue"
$ goto PROMPT1
$ endif
$ inquire junk "Hit RETURN continue"
$ return
$ NS_CONFIG:
$ say

Name Server Kerberos authentication allows the local Name Server to
authenticate using Kerberos.

$ PROMPT2:
$ echo "Select 'a' to add Name Server Kerberos authentication,"
$ inquire/nopunc resp "Select 'r' to remove Name Server Kerberos authentication: "
$ if resp .eqs. "A" 
$ then 
$   iisetres ii.'node_name.gcn.mechanisms "kerberos"
$   iisetres ii.'node_name.gcn.remote_mechanism "kerberos"
$   echo ""
$   echo "Kerberos authentication has been added to the Name Server on ''full_host_name'"
$   echo ""
$ endif
$ if resp .eqs. "R" 
$ then 
$   iisetres ii.'node_name.gcn.mechanisms ""
$   iisetres ii.'node_name.gcn.remote_mechanism "none"
$   echo ""
$   echo "Kerberos authentication has been removed from the Name Server on ''full_host_name'"
$   echo ""
$ endif
$ if resp .nes. "A" .and. resp .nes. "R" 
$ then
$ echo "Entry must be 'a' or 'r' "
$ inquire junk "Hit RETURN continue"
$ goto PROMPT2
$ endif
$ inquire junk "Hit RETURN continue"
$ return
$ SERVER_CONFIG:
$ say

Server-level Kerberos authentication forces all local servers, such as the
DBMS Server and the Comm Server, to authenticate against the Name Server
using Kerberos.

$ PROMPT3:
$ echo "Select 'a' to add server-level Kerberos authentication,"
$ inquire/nopunc resp "Select 'r' to remove server-level Kerberos authentication: "
$ if resp .eqs. "A" 
$ then 
$   iisetres ii.'node_name.gcf.user_mechanism "kerberos"
$   echo ""
$   echo "Server-level Kerberos authentication has been added."
$   echo ""
$ endif
$ if resp .eqs. "R" 
$ then 
$   iisetres ii.'node_name.gcf.user_mechanism "default"
$   echo ""
$   echo "Server-level Kerberos authentication has been removed."
$   echo ""
$ endif
$ if resp .nes. "A" .and. resp .nes. "R" 
$ then
$ echo "Entry must be 'a' or 'r' "
$ inquire junk "Hit RETURN continue"
$ goto PROMPT3
$ endif
$ inquire junk "Hit RETURN continue"
$ return
