# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 1992, 2004 Ingres Corporation
**
**
**
**  Name: startup.sc -- contains functions associated with the startup
**	configuration form of the INGRES configuration utility.
**
**  History:
**	14-dec-92 (tyler)
**		Created.
**	16-feb-92 (tyler)
**		Changed "startup" to "ingstart" in PM function calls.
**	21-jun-93 (tyler)
**		Switched to new interface for multiple context PM
**		functions.
**	23-jul-93 (tyler)
**		Removed embedded strings.  Added ChangeLog menu item
**		which allows the user to browse the change log.  Fixed
**		bug which broke renaming/deletion/creation of DBMS
**		cache resources.  Whitespace is now stripped from
**		definition names input by the user.  Added 'Preferences'
**		menu item.
**	04-aug-93 (tyler)
**		Support name server configuration.  Reverse positions
**		of Preferences and ChangeLog menu choices.
**	19-oct-93 (tyler)
**		The "Startup Count" column now reflects the current
**		configuration of the transaction logs.  Cleaned up some
**		unnecessary code.  Fixed bug introduced in previous
**		integration which caused the copy_rename_nuke() to
**		crash.  Added "Security" menu item.  Changed type of
**		global variable "host" to (char *).  Changed "dmf" to
**		"dbms" in dbms cache resources names.  Changed
**		ii.$.dbms.$.$.config.connect to
**		ii.$.dbms.$.$.config.dmf_connect.
**	01-dec-93 (tyler)
**		Fixed bad calls to IIUGerr().
**	05-jan-94 (tyler)
**		Disable 'Security' menu on clients.
**	3-feb-94 (robf)
**              Update security handling, add frskey2 for Quit
**	22-feb-94 (tyler)
**		Fixed BUGS 58145, 58910, and 59101 -  all related to
**		handling of invalid configuration definition names.
**		Fixed BUG 54460: assign PF4 to Quit.
**	16-nov-94 (shust01)
**              fixed bug from ICL that has do with error message in CBF
**              when adding a number of servers, and the row is outside the
**              the range of the form.
**	30-nov-94 (nanpr01)
**              fixed bug # 60371. Added frskey mappings so that users 
**              can have uniform keyboard mapping for diff. CBF actions.
**	26-jun-95 (sarjo01)
**		Use NT_launch_cmd for running rcpstat so no console
**		windows created.
**	11-jan-96 (tutto01)
**		Removed use of NT_launch_cmd.  No longer needed.
**	07-Feb-96 (rajus01)
**		Added support for Protocol Bridge Server.
**	22-feb-96 (harpa06)
**		Added entry for Internet Communication.
**	06-jun-1996 (thaju02)
**		Added scan_priv_cache_parm() for variable page size support,
**		to copy/rename cache parameters when duplicating dbms server.
**      24-sep-96 (i4jo01)
**              Make Internet Communication entry available on client      
**              installations.
**	10-oct-1996 (canor01)
**		Replace hard-coded 'ii' prefixes with SystemCfgPrefix.
**      01-nov-1996 (hanch04)
**            Prevent changing startup count for Internet Communication 
**	      and name server
**      20-nov-1996 (canor01)
**		Fix typo in change from 10-oct-96 that left a STprintf()
**		with a trailing percent sign.
**      12-dec-1996 (reijo01)
**              Use SystemExecName( ing or jas) in strings %sstart
**              to get generic startup executable.
**      20-dec-1996 (reijo01)
**              Use global variables ii_system_name and ii_installation_name
**              to initialize forms.
**      26-dec-1996 (harpa06)
**              Removed ICE from CBF. ICE now has it's own configuration
**		program.
**      09-jan-1997 (funka01)
**              Use GLOBREF ingres_menu for setup of form.                   
**              Added Locking system parameters for Jasmine.
**	03-mar-96 (harpa06)
**		Added static ICE parameters back to CBF. They shouldn't have
**		been removed in the first place.
**	22-oct-97 (nanpr01)
**		bug 86531 : The intent of allowing _ was to have it embedded
**		between characters not to use it by itself or as a leading
**		or trailing name. This is now enforced in cbf.
**	27-apr-1998 (rigka01)
**		bug 90660 - add remote command to cbf on Windows NT
**	24-mar-98 (mcgem01)
**		Add configuration of the Informix, Sybase, Oracle, SQL Server
**		and ODBC gateways.
**	27-apr-1998 (rigka01)
**		bug 90660 - add remote command to cbf on Windows NT
**      03-jun-98 (mcgem01)
**              Remove the Duplicate configuration option for DBMS server
**              when running the Workgroup Edition of Ingres II.
**	03-jul-98 (ainjo01)
**		Reorder the components in the startup frame, so that the 
**		DBMS components are grouped, and then followed by Net server
**		and the gateways.   Also, make the rmcmd configuration
**		generic, since it applies across all platforms.
**	03-jun-98 (mcgem01)
**		Remove the Duplicate configuration option for DBMS server
**		when running the Workgroup Edition of Ingres II.
**	21-jul-98 (nicph02)
**		Bug 91358: Recovery server was not configurable anymore
**		from CBF!!! Cleaned up some source code.
**	26-Jul-98 (gordy)
**		Added component for GCF/GCS security.
**	26-Aug-1998 (hanch04)
**		Finish making rmcmd generic.
**      22-Sep-98 (fanra01)
**              Add ice server configuration.
**	19-nov-1998 (mcgem01)
**		It is now possible to set the startup count for ICE and the
**		gateways so that these can be switched off when not required.
**		Like rmcmd, these components have a maximum startup count of 1.
**		The history of CBF changes and the preferences menus should 
**		always be available, for some reason rmcmd was switching 
**		these off.  
**	04-dec-1998 (mcgem01)
**		The naming convention used for the gateways has been 
**		modified so that it's now ii.<machine>.gateway.oracle.blah
**		instead of ii.<machine>.oracle.*.blah.   CBF had to be 
**		modified slightly to accomodate this.
**	10-aug-1999 (kinte01)
**		Add configuration information for the RMS (98235) & 
**		RDB (98243) gateways
**	22-Feb-00 (rajus01)
**		Support JDBC Server configuration.
**      29-jun-2000 (hanje04)
**              Exempted Linux (int_lnx) from WORKGROUP exclusion of 
**              multi server startup
**	16-aug-2000 (somsa01)
**		Added ibm_lnx to above change.
**      17-jan-2002 (loera01) SIR 106858
**              Always display the Security menu.  Both clients and servers 
**              now configure security options.
**      21-Feb-2002 (bolke01)
**              Moved the open of the change log below the test for 
**              a valid name.  The returns from this section caused
**              a crash of CBF if no value was specified twice!
**              (bug 102964 )
**      14-jun-2002 (mofja02) 
**              Added support for DB2 UDB.
**      23-jan-2003 (chash01) integrate bug109055 fix
**              modify code to allow RMS configuration to have menu items 
**              (Cache) and (Derived) by setting cbf_srv_name (NULL, dbms, or
**              rms) and by replacing certain hard coded "dbms" with owner.
**	27-Feb-2003 (wansh01)
**		Added support for DAS server. 
**	08-Aug-2003 (drivi01)
**		Disabled the functions associated with keys that are mapped to
**		inactive menu items when menu items are not displayed on the
**		menu bar.
**	07-Apr-2004 (vansa02)
**		Bug:112041
**		While duplicating, cbf_srv_name is initialized to "dbms" if the
**		component type is DBMS server.cbf_srv_name is already is initial
**		ized to "dbms" during configuring if the component type is DBMS
**		server. This bug araised when DBMS server is duplicated and then
**		configured as cbf_srv_name remains NULL while duplicating.
**	13-apr-2004 (abbjo03)
**	    When returning from updating a Transaction Log, we cannot use a
**	    calculated (dataset) row number with putrow, which wants a
**	    displayed row number (or the current row by default.
**	05-Jun-2004 (hanje04)
**	    Remove stray ++row which was left in after previous change removed
**	    all other trace of it.
**      11-Jun-2004 (hanch04)
**          Removed reference to CI for the open source release.
**	14-jun-2004 (somsa01)
**	    Cleaned up code for Open Source.
**	16-Aug-2004 (rajus01) Bug # 112832, Problem #INGNET 146
**	    Ingres Bridge server duplicates only the inbound_limit parameter 
**	    when duplicate menu command option is chosen from CBF main screen.
**	18-feb-2005 (devjo01)
**	    Replace S_ST0333_private & S_ST0332_shared with hard coded
**	    english strings, as config.dat parameters should not be
**	    localized.   Correct compiler warnings.
**      13-feb-2006 (loera01) Bug 115533
**          In copy_rename_nuke(), added missing code required to create a
**          named DAS server.  After writing the DAS entries, a warning popup
**          advises the user to re-configure the startup address.
**       6-Nov-2006 (hanal04) SIR 117044
**          Add int.rpl for Intel Rpath Linux build.
**	12-Feb-2007 (bonro01)
**	    Remove JDBC package.
*/
# include	<compat.h>
# include	<si.h>
# include	<ci.h>
# include	<cv.h>
# include	<st.h>
# include	<me.h>
# include	<lo.h>
# include	<ex.h>
# include	<er.h>
# include	<cm.h>
# include	<nm.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ug.h>
# include	<ui.h>
# include	<te.h>
# include	<erst.h>
# include	<uigdata.h>
# include	<stdprmpt.h>
# include	<pc.h>
# include       <cs.h>
# include       <cx.h>
# include	<pm.h>
# include	<pmutil.h>
# include	<util.h>
# include	<simacro.h>
# include	"cr.h"
# include	"config.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
/*
** global variables
*/
  GLOBALREF char *host;
  GLOBALREF char *ii_system;
  GLOBALREF char *ii_system_name;
  GLOBALREF char *ii_installation_name;
  GLOBALREF char *ii_installation;
GLOBALREF char          *node;
GLOBALREF char          *local_host;
GLOBALREF bool		server;
GLOBALREF PM_CONTEXT	*config_data;
GLOBALREF LOCATION	change_log_file;
GLOBALREF FILE		*change_log_fp;
GLOBALREF char		*change_log_buf;
GLOBALREF bool		ingres_menu;
GLOBALREF char          *cbf_srv_name;
/*
** local form name declarations
*/
  static char *startup_config = ERx( "startup_config" );
/*
** external functions
*/
FUNC_EXTERN void generic_independent_form( char *, char *, char *, char * );
FUNC_EXTERN u_i2 transaction_logs_form( char * );
void scan_priv_cache_parm(char *, char *, char *, bool);
/*
** copy_rename_nuke() copies, renames or destroys a component definition.
*/
static void
copy_rename_nuke( char *new_name, bool keep_old )
{
  char comp_type[ BIG_ENOUGH ];
  char comp_name[ BIG_ENOUGH ];
  char *new = new_name;
  i4 old_row = -1;
  char *gcd_warnmsg = "Duplicate DAS server has the same network listen addresses as the default DAS server.  Please select the 'Configure->Protocols' menu to configure a unique address"
  ;
	PM_SCAN_REC state;
	STATUS status, insert_status;
	char resource[ BIG_ENOUGH ];
	char expbuf[ BIG_ENOUGH ];
	char *regexp, *last, *name, *value;
	char *owner, *instance, *param;
	bool is_dbms = FALSE;
	bool is_rms = FALSE;
	bool is_net = FALSE;
	bool is_bridge = FALSE;
	bool is_das  = FALSE;
	bool private_cache = FALSE;
	char *cp;
	/* strip whitespace */
	if( new_name != NULL )
	{
		bool	first;
		char	lastchar;
		STzapblank( new_name, new_name );
		/* abort if no input */
		if( *new_name == EOS )
			return;
		/* abort if non-alphanumeric input (underbars are OK
		** only if they are in the middle) */
		first = 1; 
		for( cp = new_name; *cp != EOS; CMnext( cp ) )
		{
			if( !CMalpha( cp ) && !CMdigit( cp ) && *cp != '_' )
			{
/* # line 318 "startup.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(ERget( S_ST0419_invalid_name ));
  }
/* # line 321 "startup.sc" */	/* host code */
				return;
			}
			if ((first) && (*cp == '_' ))
			{
/* # line 325 "startup.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(ERget( S_ST0419_invalid_name ));
  }
/* # line 328 "startup.sc" */	/* host code */
				return;
			}
			if (first)
			  first = 0;
			lastchar = *cp;
		}
		if (lastchar == '_')
		{
/* # line 336 "startup.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(ERget( S_ST0419_invalid_name ));
  }
/* # line 339 "startup.sc" */	/* host code */
			return;
		}
	}
	/* open change log file for appending */
	if( SIfopen( &change_log_file , ERx( "a" ), SI_TXT, SI_MAX_TXT_REC,
		&change_log_fp ) != OK )
	{
		EXdelete();
		unlock_config_data();
		IIUGerr( S_ST0323_unable_to_open, UG_ERR_FATAL, 1,
			change_log_buf );
	}
	SIfprintf( change_log_fp, ERx( "\n" ) );
/* # line 355 "startup.sc" */	/* getrow */
  {
    if (IItbsetio(2,(char *)"startup_config",(char *)"components",-2) != 0) {
      IItcogetio((short *)0,1,32,BIG_ENOUGH-1,comp_type,(char *)"type");
      IItcogetio((short *)0,1,32,BIG_ENOUGH-1,comp_name,(char *)"name");
      IITBceColEnd();
    } /* IItbsetio */
  }
/* # line 360 "startup.sc" */	/* host code */
	if( STcompare( comp_name,
		ERget( S_ST030F_default_settings ) ) == 0
	)
		instance = ERx( "*" );
	else
		instance = comp_name;
	if( STcompare( comp_type , ERget( S_ST0307_dbms_server ) )
		== 0 )
	{
		is_dbms = TRUE;	
		owner = ERx( "dbms" );
		cbf_srv_name = ERx ( "dbms" );
	}
	else if( STcompare( comp_type ,
		ERget( S_ST0308_net_server ) ) == 0 )
	{
		is_net = TRUE;
		owner = ERx( "gcc" );
	}
	else if( STcompare( comp_type ,
		ERget( S_ST0690_das_server) ) == 0 )
	{
		is_das = TRUE;
		owner = ERx( "gcd" );
	}
	else if( STcompare( comp_type ,
		ERget( S_ST052B_bridge_server) ) == 0 )
	{
		is_bridge = TRUE;
		owner = ERx( "gcb" );
	}
        else if( STcompare( comp_type ,
                ERget( S_ST055C_oracle_gateway) ) == 0 )
        {
                owner = ERx( "gateway" );
		instance = ERx( "oracle" );
        }
        else if( STcompare( comp_type ,
                ERget( S_ST055F_informix_gateway) ) == 0 )
        {
                owner = ERx( "gateway" );
		instance = ERx( "informix" );
        }
        else if( STcompare( comp_type ,
                ERget( S_ST0563_sybase_gateway) ) == 0 )
        {
                owner = ERx( "gateway" );
		instance = ERx( "sybase" );
        }
        else if( STcompare( comp_type ,
                ERget( S_ST0566_mssql_gateway) ) == 0 )
        {
                owner = ERx( "gateway" );
		instance = ERx( "mssql" );
        }
        else if( STcompare( comp_type ,
                ERget( S_ST0569_odbc_gateway) ) == 0 )
        {
                owner = ERx( "gateway" );
                instance = ERx( "odbc" );
        }
        else if( STcompare( comp_type ,
                ERget( S_ST0644_rms_gateway) ) == 0 )
        {
                is_rms = TRUE;
                owner = ERx( "rms" );
		cbf_srv_name = ERx ( "rms" );
        }
        else if( STcompare( comp_type ,
                ERget( S_ST0649_rdb_gateway) ) == 0 )
        {
                owner = ERx( "gateway" );
		instance = ERx( "rdb" );
        }
        else if( STcompare( comp_type ,
                ERget( S_ST0677_db2udb_gateway) ) == 0 )
        {
                owner = ERx( "gateway" );
		instance = ERx( "db2udb" );
        }
	else if( STcompare( comp_type ,
		ERget( S_ST0309_star_server ) ) == 0
	)
		owner = ERx( "star" );
	STprintf( expbuf, ERx( "%s.%s.%s.%s.%%" ), 
		  SystemCfgPrefix, host, owner, instance );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status = PMmScan(
		config_data, NULL, &state, last, &name, &value ) )
	{
		/* set 'last' to handle deletion during scan */
		last = name;
		/* extract parameter name */
		param = PMmGetElem( config_data, 4, name );
		/* destroy old resource if 'keep_old' not TRUE */ 
		if( !keep_old && PMmDelete( config_data, name ) == OK )
		{
			SIfprintf( change_log_fp, ERx( "DELETE %s\n" ),
				name );
			SIflush( change_log_fp );
		}
		/* insert resource if 'new' not NULL */ 
		if( new != NULL )
		{
			/* compose duplicate resource name */
			STprintf( resource, ERx( "%s.%s.%s.%s.%s" ), 
				  SystemCfgPrefix, host, owner, new, param );
			if( PMmInsert( config_data, resource, value ) == OK )
			{
				SIfprintf( change_log_fp,
					ERx( "CREATE %s: (%s)\n" ),
					resource, value );
				SIflush( change_log_fp );
			}
		}
		/* check for private DBMS cache */
		if( ( is_dbms || is_rms )&& STcompare( ERx( "cache_sharing" ),
			param ) == 0 && !ValueIsBoolTrue( value )
		)
			private_cache = TRUE;
	}
	/* scan Net protocol resources */
	if( is_net || is_bridge || is_das )
	{
		char *protocol;
		STprintf( expbuf, ERx( "%s.%s.%s.%s.%%.%%" ), 
			  SystemCfgPrefix, host, owner, instance );
		regexp = PMmExpToRegExp( config_data, expbuf );
		for( status = PMmScan( config_data, regexp, &state,
			NULL, &name, &value ); status == OK; status =
			PMmScan( config_data, NULL, &state, last, &name,
			&value ) )
		{
			/* set 'last' to handle deletion during scan */
			last = name;
			/* extract protocol name */
			protocol = PMmGetElem( config_data, 4, name );
			/* extract parameter name */
			param = PMmGetElem( config_data, 5, name );
			/* destroy old resource if 'keep_old' not TRUE */ 
			if( !keep_old && PMmDelete( config_data, name )
				== OK )
			{
				SIfprintf( change_log_fp,
					ERx( "DELETE %s\n" ),
					name );
				SIflush( change_log_fp );
			}
			/* insert resource if 'new' not NULL */ 
			if( new != NULL )
			{
				/* compose duplicate resource name */
				if( is_net )
				    STprintf( resource,
					  ERx( "%s.%s.gcc.%s.%s.%s" ),
					  SystemCfgPrefix, host, new, 
					  protocol, param );
                                 else if( is_das )
                                     STprintf( resource,
                                           ERx( "%s.%s.gcd.%s.%s.%s" ),
                                           SystemCfgPrefix, host, new,
                                           protocol, param );
				else if( is_bridge )
				    STprintf( resource,
					  ERx( "%s.%s.gcb.%s.%s.%s" ),
					  SystemCfgPrefix, host, new, 
					  protocol, param );
                                if( PMmInsert( config_data, resource,
                                       value ) == OK )
				{
					SIfprintf( change_log_fp,
						ERx( "CREATE %s: (%s)\n" ),
						resource, value );
					SIflush( change_log_fp );
				}
			} /* if( new != NULL ) */
		} /* for( status = PMmScan( config_data, regexp, &state... */
	} /* if( is_das || is_net || is_bridge ) */
	/* scan private DBMS cache resources */
	if( private_cache )
	{
		scan_priv_cache_parm(instance, new, NULL, keep_old);
		scan_priv_cache_parm(instance, new, ERx("cache"), keep_old);
	 	if(!ingres_menu) 
		scan_priv_cache_parm(instance, new, ERx("p2k"), keep_old);
		scan_priv_cache_parm(instance, new, ERx("p4k"), keep_old);
		scan_priv_cache_parm(instance, new, ERx("p8k"), keep_old);
		scan_priv_cache_parm(instance, new, ERx("p16k"), keep_old);
		scan_priv_cache_parm(instance, new, ERx("p32k"), keep_old);
		scan_priv_cache_parm(instance, new, ERx("p64k"), keep_old);
		/* compose hidden dmf connect resource name */
		STprintf( resource,
 			  ERx( "%s.%s.%s.private.%s.config.dmf_connect" ),
			  SystemCfgPrefix, host, owner, instance );
	}
	else
	{
		/* compose hidden dmf connect resource name */
		STprintf( resource,
			  ERx( "%s.%s.%s.shared.%s.config.dmf_connect" ),
			  SystemCfgPrefix, host, owner, instance );
	}
	/* delete hidden dmf connect resource */
	(void) PMmDelete( config_data, resource );
	if( !keep_old )
	{
		/* get current row number */
/* # line 599 "startup.sc" */	/* inquire_frs */
  {
    if (IIiqset(3,0,(char *)"startup_config",(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&old_row,29,(char *)"components",0);
    } /* IIiqset */
  }
/* # line 602 "startup.sc" */	/* host code */
		--old_row;
		/* compose name of existing startup resource name */
		STprintf( resource, ERx( "%s.%s.%sstart.%s.%s" ), 
			  SystemCfgPrefix, host, SystemExecName, instance,
			  owner );	
		/* delete it */
		if( PMmDelete( config_data, resource ) == OK )
		{
			SIfprintf( change_log_fp, ERx( "DELETE %s\n" ),
				resource );
			SIflush( change_log_fp );
		}
		/* delete tablefield row */
/* # line 618 "startup.sc" */	/* deleterow */
  {
    if (IItbsetio(4,(char *)"startup_config",(char *)"components",-2) != 0) {
      if (IItdelrow(0) != 0) {
      } /* IItdelrow */
    } /* IItbsetio */
  }
/* # line 619 "startup.sc" */	/* host code */
	}
	if( new != NULL )
	{
		/* compose duplicate startup resource */
		STprintf( resource, ERx( "%s.%s.%sstart.%s.%s" ), 
			  SystemCfgPrefix, host, SystemExecName, new, owner );	
		/* insert it and update tablefield if necessary */
                if( ( insert_status = PMmInsert( config_data, resource,
                    is_das ? ERx( "1" ) : ERx( "0" ) ) ) == OK )
		{	
                        if (is_das)
                            SIfprintf( change_log_fp, ERx( "CREATE %s: (1)\n" ),
                                resource );
                        else
                            SIfprintf( change_log_fp, ERx( "CREATE %s: (0)\n" ),
                                resource );
			SIflush( change_log_fp );
			/* old_row > -1 if old row was deleted */
			if( old_row >= 0 )
			{
			    if (is_das)
/* # line 643 "startup.sc" */	/* insertrow */
  {
    if (IItbsetio(5,(char *)"startup_config",(char *)"components",old_row) != 
    0) {
      if (IItinsert() != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,comp_type);
        IItcoputio((char *)"name",(short *)0,1,32,0,new);
        IItcoputio((char *)"enabled",(short *)0,1,32,0,ERx( "1" ));
        IITBceColEnd();
      } /* IItinsert */
    } /* IItbsetio */
  }
/* # line 650 "startup.sc" */	/* host code */
                             else
/* # line 651 "startup.sc" */	/* insertrow */
  {
    if (IItbsetio(5,(char *)"startup_config",(char *)"components",old_row) != 
    0) {
      if (IItinsert() != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,comp_type);
        IItcoputio((char *)"name",(short *)0,1,32,0,new);
        IItcoputio((char *)"enabled",(short *)0,1,32,0,ERx( "0" ));
        IITBceColEnd();
      } /* IItinsert */
    } /* IItbsetio */
  }
/* # line 658 "startup.sc" */	/* host code */
			}
			else
			{
                            if (is_das)
/* # line 662 "startup.sc" */	/* insertrow */
  {
    if (IItbsetio(5,(char *)"startup_config",(char *)"components",-2) != 0) {
      if (IItinsert() != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,comp_type);
        IItcoputio((char *)"name",(short *)0,1,32,0,new);
        IItcoputio((char *)"enabled",(short *)0,1,32,0,ERx( "1" ));
        IITBceColEnd();
      } /* IItinsert */
    } /* IItbsetio */
  }
/* # line 667 "startup.sc" */	/* host code */
                             else
/* # line 668 "startup.sc" */	/* insertrow */
  {
    if (IItbsetio(5,(char *)"startup_config",(char *)"components",-2) != 0) {
      if (IItinsert() != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,comp_type);
        IItcoputio((char *)"name",(short *)0,1,32,0,new);
        IItcoputio((char *)"enabled",(short *)0,1,32,0,ERx( "0" ));
        IITBceColEnd();
      } /* IItinsert */
    } /* IItbsetio */
  }
/* # line 673 "startup.sc" */	/* host code */
			}
		}
                if (is_das && insert_status == OK)
/* # line 676 "startup.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(gcd_warnmsg);
  }
/* # line 677 "startup.sc" */	/* host code */
	}
	/* close change log after appending */
	(void) SIclose( change_log_fp );
	/* write config.dat */
	if( PMmWrite( config_data, NULL ) != OK )
		IIUGerr( S_ST0318_fatal_error, UG_ERR_FATAL, 1,
			ERget( S_ST0314_bad_config_write ) );
}
STATUS do_csinstall(void);
STATUS call_rcpstat( char *arguments );
void do_ipcclean(void);
void
startup_config_form( void )
{
  char *name, *value;
  char comp_type[ BIG_ENOUGH ];
  char comp_name[ BIG_ENOUGH ];
  char enabled_val[ BIG_ENOUGH ];
  char in_buf[ BIG_ENOUGH ];
  char *enabled1;
  char *enabled2;
  i4 activated = 0;
  i4 curr_row;
  i4 max_row;
  char temp_buf[ BIG_ENOUGH ];
  char log_buf[ BIG_ENOUGH ];
	LOCATION *IIUFlcfLocateForm();
	STATUS IIUFgtfGetForm();
	PM_SCAN_REC state;
	STATUS status;
	char *regexp;
	char expbuf[ BIG_ENOUGH ];
	char pm_id[ BIG_ENOUGH ];
	bool use_secure;
	if( IIUFgtfGetForm( IIUFlcfLocateForm(), startup_config ) != OK )
	{
		FEendforms();
		EXdelete();
		F_ERROR( "\nbad news: form \"%s\" not found.\n\n",
			(char *) startup_config );
	}
/* # line 728 "startup.sc" */	/* inittable */
  {
    if (IItbinit((char *)"startup_config",(char *)"components",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 730 "startup.sc" */	/* display */
  {
    if (IIdispfrm((char *)"startup_config",(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    IIputfldio((char *)"host",(short *)0,1,32,0,host);
    IIputfldio((char *)"ii_installation",(short *)0,1,32,0,ii_installation);
    IIputfldio((char *)"ii_installation_name",(short *)0,1,32,0,
    ii_installation_name);
    IIputfldio((char *)"ii_system",(short *)0,1,32,0,ii_system);
    IIputfldio((char *)"ii_system_name",(short *)0,1,32,0,ii_system_name);
/* # line 741 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0300_name_server ));
        IItcoputio((char *)"name",(short *)0,1,32,0,
        ERget( S_ST030F_default_settings ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,ERx( "1" ));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 747 "startup.sc" */	/* host code */
	if(!ingres_menu) {
/* # line 748 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0304_locking_system ));
        IItcoputio((char *)"name",(short *)0,1,32,0,
        ERget( S_ST030F_default_settings ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,ERx( "1" ));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 753 "startup.sc" */	/* host code */
	}
	if( server )
	{
		/* scan DBMS definitions */
		STprintf( expbuf, ERx( "%s.%s.%sstart.%%.dbms" ), 
			  SystemCfgPrefix, host, SystemExecName );
		regexp = PMmExpToRegExp( config_data, expbuf );
		for( status = PMmScan( config_data, regexp, &state, NULL,
			&name, &value ); status == OK; status =
			PMmScan( config_data, NULL, &state, NULL, &name,
			&value ) )
		{
			name = PMmGetElem( config_data, 3, name );
			if( STcompare( name, ERx( "*" ) ) == 0 )
				name = ERget( S_ST030F_default_settings );
/* # line 770 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0307_dbms_server ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 775 "startup.sc" */	/* host code */
		}	
		/* examine transaction logs */
		{
                    STATUS      csinstall;
/* # line 781 "startup.sc" */	/* message */
    {
      IImessage(ERget( S_ST038A_examining_logs ));
    }
/* # line 783 "startup.sc" */	/* host code */
                    csinstall = do_csinstall();
                    if ( OK == call_rcpstat( ERx( "-exist -silent" ) ) &&
                         OK == call_rcpstat( ERx( "-enable -silent" ) ) )
                    {
                            enabled1 = ERx( "1" );
                    }
                    else
                            enabled1 = ERx( "0" );
                    if ( OK == call_rcpstat( ERx( "-exist -dual -silent" ) ) &&
                         OK == call_rcpstat( ERx( "-enable -dual -silent" ) ) )
                    {
                            enabled2 = ERx( "1" );
                    }
                    else
                            enabled2 = ERx( "0" );
                    if(OK == csinstall)
                    {
                        do_ipcclean();
                    }
                }
	}
	/* disable "Security" menu item if security auditing is not setup */ 
	STprintf( temp_buf, ERx( "%s.$.c2.%%" ), SystemCfgPrefix );
	regexp = PMmExpToRegExp( config_data, temp_buf );
	if( !server || 
		PMmScan( config_data, regexp, &state, NULL, &name, &value )
		!= OK )
	{
		use_secure=FALSE;
	}
	else
		use_secure=TRUE;
	/* check for ICE definition */
  	STprintf( expbuf, ERx( "%s.%s.%sstart.%%.icesvr" ), SystemCfgPrefix,
            host,SystemExecName );
  	regexp = PMmExpToRegExp( config_data, expbuf );
  	if ( PMmScan( config_data, regexp, &state, NULL, &name, &value )
  		== OK )
  	{
            for( status = PMmScan( config_data, regexp, &state, NULL,
                &name, &value ); status == OK; status =
                PMmScan( config_data, NULL, &state, NULL, &name,
                &value ) )
            {
                name = PMmGetElem( config_data, 3, name );
                if( STcompare( name, ERx( "*" ) ) == 0 )
                    name = ERget( S_ST030F_default_settings );
/* # line 838 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0578_ice_server ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 843 "startup.sc" */	/* host code */
            }
  	}
	if( server )
	{
		/* scan Star definitions */
		STprintf( expbuf, ERx( "%s.%s.%sstart.%%.star" ), 
			  SystemCfgPrefix, host, SystemExecName );
		regexp = PMmExpToRegExp( config_data, expbuf );
		for( status = PMmScan( config_data, regexp, &state, NULL,
			&name, &value ); status == OK; status =
			PMmScan( config_data, NULL, &state, NULL, &name,
			&value ) )
		{
			name = PMmGetElem( config_data, 3, name );
			if( STcompare( name, ERx( "*" ) ) == 0 )
				name = ERget( S_ST030F_default_settings );
/* # line 861 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0309_star_server ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 866 "startup.sc" */	/* host code */
		}
/* # line 868 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0304_locking_system ));
        IItcoputio((char *)"name",(short *)0,1,32,0,
        ERget( S_ST030F_default_settings ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,ERx( "1" ));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 874 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0303_logging_system ));
        IItcoputio((char *)"name",(short *)0,1,32,0,
        ERget( S_ST030F_default_settings ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,ERx( "1" ));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 880 "startup.sc" */	/* host code */
		STprintf( log_buf, "%s_LOG_FILE", SystemVarPrefix );
/* # line 881 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0305_transaction_log ));
        IItcoputio((char *)"name",(short *)0,1,32,0,log_buf);
        IItcoputio((char *)"enabled",(short *)0,1,32,0,enabled1);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 887 "startup.sc" */	/* host code */
		STprintf( temp_buf, "%s_DUAL_LOG", SystemVarPrefix );
/* # line 888 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0305_transaction_log ));
        IItcoputio((char *)"name",(short *)0,1,32,0,temp_buf);
        IItcoputio((char *)"enabled",(short *)0,1,32,0,enabled2);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 894 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0301_recovery_server ));
        IItcoputio((char *)"name",(short *)0,1,32,0,
        ERget( S_ST030F_default_settings ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,ERx( "1" ));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 900 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0302_archiver_process ));
        IItcoputio((char *)"name",(short *)0,1,32,0,
        ERget( S_ST030A_not_configurable ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,ERx( "1" ));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 906 "startup.sc" */	/* host code */
                /* scan Remote Command definitions */
                STprintf( expbuf, ERx( "%s.%s.%sstart.%%.rmcmd" ),
                          SystemCfgPrefix, host, SystemExecName );
                regexp = PMmExpToRegExp( config_data, expbuf );
                status=PMmScan( config_data, regexp, &state, NULL,
                        &name, &value);
                if (status)
/* # line 913 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,ERget( S_ST02FE_rmcmd ));
        IItcoputio((char *)"name",(short *)0,1,32,0,
        ERget( S_ST030A_not_configurable ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,ERx( "0" ));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 918 "startup.sc" */	/* host code */
                else
                {
/* # line 920 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,ERget( S_ST02FE_rmcmd ));
        IItcoputio((char *)"name",(short *)0,1,32,0,
        ERget( S_ST030A_not_configurable ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 925 "startup.sc" */	/* host code */
                }
	}
/* # line 929 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST034E_security_menu ));
        IItcoputio((char *)"name",(short *)0,1,32,0,
        ERget( S_ST030F_default_settings ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,ERx( "1" ));
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 935 "startup.sc" */	/* host code */
	/* scan Net definitions */
	STprintf( expbuf, ERx( "%s.%s.%sstart.%%.gcc" ), 
		  SystemCfgPrefix, host, SystemExecName );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
		name = PMmGetElem( config_data, 3, name );
		if( STcompare( name, ERx( "*" ) ) == 0 )
			name = ERget( S_ST030F_default_settings );
/* # line 948 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0308_net_server ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 953 "startup.sc" */	/* host code */
	}
	/* scan das server definitions */
	STprintf( expbuf, ERx( "%s.%s.%sstart.%%.gcd" ), 
		  SystemCfgPrefix, host, SystemExecName );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
		name = PMmGetElem( config_data, 3, name );
		if( STcompare( name, ERx( "*" ) ) == 0 )
			name = ERget( S_ST030F_default_settings );
/* # line 968 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0690_das_server ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 973 "startup.sc" */	/* host code */
	}
	/* scan Bridge definitions */
	STprintf( expbuf, ERx( "%s.%s.%sstart.%%.gcb" ), 
		  SystemCfgPrefix, host, SystemExecName );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
		name = PMmGetElem( config_data, 3, name );
		if( STcompare( name, ERx( "*" ) ) == 0 )
			name = ERget( S_ST030F_default_settings );
/* # line 988 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST052B_bridge_server ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 993 "startup.sc" */	/* host code */
	}	
	/* 
	** Scan Oracle Gateway definitions 
	*/
	STprintf( expbuf, ERx( "%s.%s.%sstart.%%.oracle" ), 
		  SystemCfgPrefix, host, SystemExecName );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
		name = PMmGetElem( config_data, 3, name );
		if( STcompare( name, ERx( "*" ) ) == 0 )
			name = ERget( S_ST030F_default_settings );
/* # line 1010 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST055C_oracle_gateway ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1015 "startup.sc" */	/* host code */
	}	
	/* 
	** Scan Informix Gateway definitions 
	*/
	STprintf( expbuf, ERx( "%s.%s.%sstart.%%.informix" ), 
		  SystemCfgPrefix, host, SystemExecName );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
		name = PMmGetElem( config_data, 3, name );
		if( STcompare( name, ERx( "*" ) ) == 0 )
			name = ERget( S_ST030F_default_settings );
/* # line 1032 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST055F_informix_gateway ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1037 "startup.sc" */	/* host code */
	}	
	/* 
	** Scan Sybase Gateway definitions 
	*/
	STprintf( expbuf, ERx( "%s.%s.%sstart.%%.sybase" ), 
		  SystemCfgPrefix, host, SystemExecName );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
		name = PMmGetElem( config_data, 3, name );
		if( STcompare( name, ERx( "*" ) ) == 0 )
			name = ERget( S_ST030F_default_settings );
/* # line 1054 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0563_sybase_gateway ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1059 "startup.sc" */	/* host code */
	}	
	/* 
	** Scan SQL Server Gateway definitions 
	*/
	STprintf( expbuf, ERx( "%s.%s.%sstart.%%.mssql" ), 
		  SystemCfgPrefix, host, SystemExecName );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
		name = PMmGetElem( config_data, 3, name );
		if( STcompare( name, ERx( "*" ) ) == 0 )
			name = ERget( S_ST030F_default_settings );
/* # line 1076 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0566_mssql_gateway ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1081 "startup.sc" */	/* host code */
	}	
	/* 
	** Scan ODBC Gateway definitions 
	*/
	STprintf( expbuf, ERx( "%s.%s.%sstart.%%.odbc" ), 
		  SystemCfgPrefix, host, SystemExecName );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
		name = PMmGetElem( config_data, 3, name );
		if( STcompare( name, ERx( "*" ) ) == 0 )
			name = ERget( S_ST030F_default_settings );
/* # line 1098 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0569_odbc_gateway ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1103 "startup.sc" */	/* host code */
	}	
	/* scan RMS definitions */
	STprintf( expbuf, ERx( "%s.%s.%sstart.%%.rms" ), 
		SystemCfgPrefix, host, SystemExecName );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
		name = PMmGetElem( config_data, 3, name );
		if( STcompare( name, ERx( "*" ) ) == 0 )
			name = ERget( S_ST030F_default_settings );
/* # line 1118 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0644_rms_gateway ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1123 "startup.sc" */	/* host code */
	}
	/* 
	** Scan RDB Gateway definitions 
	*/
	STprintf( expbuf, ERx( "%s.%s.%sstart.%%.rdb" ), 
		  SystemCfgPrefix, host, SystemExecName );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
		name = PMmGetElem( config_data, 3, name );
		if( STcompare( name, ERx( "*" ) ) == 0 )
			name = ERget( S_ST030F_default_settings );
/* # line 1140 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0649_rdb_gateway ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1145 "startup.sc" */	/* host code */
	}	
	/* 
	** Scan DB2UDB Gateway definitions 
	*/
	STprintf( expbuf, ERx( "%s.%s.%sstart.%%.db2udb" ), 
		  SystemCfgPrefix, host, SystemExecName );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
		name = PMmGetElem( config_data, 3, name );
		if( STcompare( name, ERx( "*" ) ) == 0 )
			name = ERget( S_ST030F_default_settings );
/* # line 1162 "startup.sc" */	/* loadtable */
    {
      if (IItbact((char *)"startup_config",(char *)"components",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"type",(short *)0,1,32,0,
        ERget( S_ST0677_db2udb_gateway ));
        IItcoputio((char *)"enabled",(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1167 "startup.sc" */	/* host code */
	}	
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
		char *arg1, *arg2;
/* # line 1176 "startup.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"startup_config",(char *)"components",-2) !=
           0) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,comp_type,(char *)
"type");
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,comp_name,(char *)
"name");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1180 "startup.sc" */	/* host code */
		if( STcompare( comp_type, ERget( S_ST0305_transaction_log ) )
			== 0 )
		{
			u_i2 log_status = transaction_logs_form( comp_name ); 
			/*
			** The following assumes that the II_LOG_FILE and
			** II_DUAL_FILE entries are consecutive and in that
			** order, as loaded above.
			*/
/* # line 1190 "startup.sc" */	/* inquire_frs */
        {
          if (IIiqset(3,0,(char *)"startup_config",(char *)0, (char *)0) != 0
          ) {
            IIiqfsio((short *)0,1,30,4,&curr_row,29,(char *)0,0);
            IIiqfsio((short *)0,1,30,4,&max_row,30,(char *)0,0);
          } /* IIiqset */
        }
/* # line 1192 "startup.sc" */	/* host code */
			if (STcompare(comp_name, log_buf) == 0)
			{
			    enabled1 = (log_status & PRIMARY_LOG_ENABLED) ?
				ERx("1") : ERx("0");
/* # line 1196 "startup.sc" */	/* putrow */
        {
          if (IItbsetio(3,(char *)"startup_config",(char *)"components",-2) !=
           0) {
            IItcoputio((char *)"enabled",(short *)0,1,32,0,enabled1);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1198 "startup.sc" */	/* host code */
			    enabled2 = (log_status & SECONDARY_LOG_ENABLED) ?
				ERx("1") : ERx("0");
			    if (++curr_row <= max_row && curr_row > 0)
/* # line 1201 "startup.sc" */	/* putrow */
        {
          if (IItbsetio(3,(char *)"startup_config",(char *)"components",
          curr_row) != 0) {
            IItcoputio((char *)"enabled",(short *)0,1,32,0,enabled2);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1203 "startup.sc" */	/* host code */
			}
			else
			{
			    enabled2 = (log_status & SECONDARY_LOG_ENABLED) ?
				ERx("1") : ERx("0");
/* # line 1208 "startup.sc" */	/* putrow */
        {
          if (IItbsetio(3,(char *)"startup_config",(char *)"components",-2) !=
           0) {
            IItcoputio((char *)"enabled",(short *)0,1,32,0,enabled2);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1210 "startup.sc" */	/* host code */
			    enabled1 = (log_status & PRIMARY_LOG_ENABLED) ?
				ERx("1") : ERx("0");
			    if (--curr_row > 0 && curr_row <= max_row)
/* # line 1213 "startup.sc" */	/* putrow */
        {
          if (IItbsetio(3,(char *)"startup_config",(char *)"components",
          curr_row) != 0) {
            IItcoputio((char *)"enabled",(short *)0,1,32,0,enabled1);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1215 "startup.sc" */	/* host code */
			}
/* # line 1217 "startup.sc" */	/* redisplay */
        {
          IIredisp();
        }
/* # line 1219 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1220 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST0303_logging_system ) )
			== 0 )
		{
			arg1 = STalloc( ERget( S_ST0368_configure_logging ) );
			arg2 = STalloc( ERget( S_ST0369_logging_parameters ) );
			generic_independent_form( arg1, arg2,
				ERx( "rcp.log" ), NULL );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1230 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1231 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST0304_locking_system ) )
			== 0 )
		{
			arg1 = STalloc( ERget( S_ST036A_configure_locking ) );
			arg2 = STalloc( ERget( S_ST036B_locking_parameters ) );
			if (ingres_menu) 
			  generic_independent_form( arg1, arg2,
				ERx( "rcp.lock" ), NULL );
			else  
			  generic_independent_form( arg1, arg2,
				ERx( "lock"), NULL );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1245 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1246 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST0307_dbms_server ) )
			== 0 )
		{
			if( STcompare( comp_name,
				ERget( S_ST030F_default_settings ) ) == 0
			)
				STcopy( ERx( "*" ), comp_name );
			STprintf( pm_id, ERx( "dbms.%s" ), comp_name );
			arg1 = STalloc( ERget( S_ST0360_configure_dbms ) );
			arg2 = STalloc( ERget( S_ST0361_dbms_parameters ) );
                        cbf_srv_name = ERx( "dbms" );
			generic_independent_form( arg1, arg2, pm_id,
				comp_name );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1262 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1263 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST0308_net_server ) )
			== 0 )
		{
			if( STcompare( comp_name,
				ERget( S_ST030F_default_settings ) ) == 0
			)
				STcopy( ERx( "*" ), comp_name );
			STprintf( pm_id, ERx( "gcc.%s" ), comp_name );
			arg1 = STalloc( ERget( S_ST0364_configure_net ) );
			arg2 = STalloc( ERget( S_ST0365_net_parameters ) );
			generic_independent_form( arg1, arg2, pm_id,
				comp_name );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1278 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1279 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST0690_das_server ) )
			== 0 )
		{
			if( STcompare( comp_name,
				ERget( S_ST030F_default_settings ) ) == 0
			)
				STcopy( ERx( "*" ), comp_name );
			STprintf( pm_id, ERx( "gcd.%s" ), comp_name );
			arg1 = STalloc( ERget( S_ST0691_configure_das ) );
			arg2 = STalloc( ERget( S_ST0692_das_parameters ) );
			generic_independent_form( arg1, arg2, pm_id,
				comp_name );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1294 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1295 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST052B_bridge_server) )
			== 0 )
		{
			if( STcompare( comp_name,
				ERget( S_ST030F_default_settings ) ) == 0
			)
				STcopy( ERx( "*" ), comp_name );
			STprintf( pm_id, ERx( "gcb.%s" ), comp_name );
			arg1 = STalloc( ERget( S_ST0529_configure_bridge ) );
			arg2 = STalloc( ERget( S_ST052A_bridge_parameters ) );
			generic_independent_form( arg1, arg2, pm_id,
				comp_name );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1311 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1312 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST055C_oracle_gateway) )
			== 0 )
		{
			STcopy( ERx( "oracle" ), comp_name );
			STcopy( ERx( "gateway.oracle" ), pm_id );
			arg1 = STalloc( ERget( S_ST055A_configure_oracle ) );
			arg2 = STalloc( ERget( S_ST055B_oracle_parameters ) );
			generic_independent_form( arg1, arg2, pm_id,
				comp_name );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1324 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1325 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST055F_informix_gateway) )
			== 0 )
		{
			STcopy( ERx( "informix" ), comp_name );
			STprintf( pm_id, ERx( "gateway.%s" ), comp_name );
			arg1 = STalloc( ERget( S_ST055D_configure_informix ) );
			arg2 = STalloc( ERget( S_ST055E_informix_parameters ) );
			generic_independent_form( arg1, arg2, pm_id,
				comp_name );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1337 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1338 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST0563_sybase_gateway) )
			== 0 )
		{
			STcopy( ERx( "sybase" ), comp_name );
			STprintf( pm_id, ERx( "gateway.%s" ), comp_name );
			arg1 = STalloc( ERget( S_ST0561_configure_sybase ) );
			arg2 = STalloc( ERget( S_ST0562_sybase_parameters ) );
			generic_independent_form( arg1, arg2, pm_id,
				comp_name );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1350 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1351 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST0566_mssql_gateway) )
			== 0 )
		{
			STcopy( ERx( "mssql" ), comp_name );
			STprintf( pm_id, ERx( "gateway.%s" ), comp_name );
			arg1 = STalloc( ERget( S_ST0564_configure_mssql ) );
			arg2 = STalloc( ERget( S_ST0565_mssql_parameters ) );
			generic_independent_form( arg1, arg2, pm_id,
				comp_name );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1363 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1364 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST0569_odbc_gateway) )
			== 0 )
		{
			STcopy( ERx( "odbc" ), comp_name );
			STprintf( pm_id, ERx( "gateway.%s" ), comp_name );
			arg1 = STalloc( ERget( S_ST0567_configure_odbc ) );
			arg2 = STalloc( ERget( S_ST0568_odbc_parameters ) );
			generic_independent_form( arg1, arg2, pm_id,
				comp_name );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1376 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1377 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST0649_rdb_gateway) )
			== 0 )
		{
			STcopy( ERx( "rdb" ), comp_name );
			STcopy( ERx( "gateway.rdb" ), pm_id );
			arg1 = STalloc( ERget( S_ST0647_configure_rdb ) );
			arg2 = STalloc( ERget( S_ST0648_rdb_parameters ) );
			generic_independent_form( arg1, arg2, pm_id,
				comp_name );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1389 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1390 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST0644_rms_gateway ) )
			== 0 )
		{
			if( STcompare( comp_name,
				ERget( S_ST030F_default_settings ) ) == 0
			)
				STcopy( ERx( "*" ), comp_name );
			STprintf( pm_id, ERx( "rms.%s" ), comp_name );
			arg1 = STalloc( ERget( S_ST0642_configure_rms ) );
			arg2 = STalloc( ERget( S_ST0643_rms_parameters ) );
			cbf_srv_name = ERx( "rms" );
			generic_independent_form( arg1, arg2, pm_id,
				comp_name );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1406 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1407 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST0677_db2udb_gateway ) )
			== 0 )
		{
			STcopy( ERx( "db2udb" ), comp_name );
			STcopy( ERx( "gateway.db2udb" ), pm_id );
			arg1 = STalloc( ERget( S_ST0675_configure_db2udb ) );
			arg2 = STalloc( ERget( S_ST0676_db2udb_parameters ) );
			generic_independent_form( arg1, arg2, pm_id,
				comp_name );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1419 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1420 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST0309_star_server ) )
			== 0 )
		{
			if( STcompare( comp_name,
				ERget( S_ST030F_default_settings ) ) == 0
			)
				STcopy( ERx( "*" ), comp_name );
			STprintf( pm_id, ERx( "star.%s" ), comp_name );
			arg1 = STalloc( ERget( S_ST0366_configure_star ) );
			arg2 = STalloc( ERget( S_ST0367_star_parameters ) );
			generic_independent_form( arg1, arg2, pm_id,
				comp_name );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1435 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1436 "startup.sc" */	/* host code */
		}
                if( STcompare( comp_type, ERget( S_ST0301_recovery_server ))
                        == 0 )
                {
                        arg1 = STalloc( ERget( S_ST037E_configure_recovery ));
                        arg2 = STalloc( ERget( S_ST037F_recovery_parameters ));
                        generic_independent_form( arg1, arg2,
                                ERx( "recovery.*" ), NULL );
                        MEfree( arg1 );
                        MEfree( arg2 );
/* # line 1446 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1447 "startup.sc" */	/* host code */
                }
		if( STcompare( comp_type, ERget( S_ST0300_name_server ) )
			== 0 )
		{
			arg1 = STalloc( ERget(
				S_ST0375_configure_name_server ) );
			arg2 = STalloc( ERget( S_ST0376_iigcn_parameters ) );
			generic_independent_form( arg1, arg2,
				ERx( "gcn" ), NULL );
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1458 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1459 "startup.sc" */	/* host code */
		}
		if( STcompare( comp_type, ERget( S_ST034E_security_menu ) )
			== 0 )
		{
		    if(use_secure)
		    {
		    /*
		    ** Submenu: SecurityOptions Auditing End
		    */
/* # line 1468 "startup.sc" */	/* submenu */
        {
          goto IImuI2;
IIfdB2:
          IIrunmu(0);
          if (IIretval() == 1) {
/* # line 1471 "startup.sc" */	/* host code */
			arg1 = STalloc(ERget(S_ST0451_configure_auth_system));
			arg2 = STalloc(ERget(S_ST0452_auth_system_parameter));
			generic_independent_form(arg1, arg2, ERx("gcf"), NULL);
			MEfree( arg1 );
			MEfree( arg2 );
          } else if (IIretval() == 2) {
			arg1 = STalloc( ERget( S_ST037C_configure_sec_params ) );
			arg2 = STalloc( ERget( S_ST037A_security_params ) );
			generic_independent_form( arg1, arg2, ERx( "secure" ), NULL );
			MEfree( arg1 );
			MEfree( arg2 );
          } else if (IIretval() == 3) {
			arg1 = STalloc( ERget( S_ST0377_configure_security ) );
			arg2 = STalloc( ERget( S_ST0378_security_parameters ) );
			generic_independent_form( arg1, arg2, ERx("c2"), NULL ) ;
			MEfree( arg1 );
			MEfree( arg2 );
          } else if (IIretval() == 4) {
				/* Ends submenu */
				;
          } else {
            if (1) goto IIfdE2;
          } /* IIretval */
          goto IIfdE2;
IImuI2:
          IImuonly();
          if (IIinitmu() == 0) goto IIfdE2;
          if (IInmuact(ERget(S_ST0450_auth_system_menu),(char *)0,2,2,1) == 0
          ) goto IIfdE2;
          IIFRInternal(0);
          if (IInfrskact(15,(char *)0,2,2,1) == 0) goto IIfdE2;
          if (IInmuact(ERx("SecurityOptions"),(char *)0,2,2,2) == 0) goto 
          IIfdE2;
          IIFRInternal(0);
          if (IInfrskact(16,(char *)0,2,2,2) == 0) goto IIfdE2;
          if (IInmuact(ERget( S_ST037D_auditing_menu ),(char *)0,2,2,3) == 0) 
          goto IIfdE2;
          IIFRInternal(0);
          if (IInfrskact(17,(char *)0,2,2,3) == 0) goto IIfdE2;
          if (IInmuact(ERget( FE_End ),(char *)0,2,2,4) == 0) goto IIfdE2;
          IIFRInternal(0);
          if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE2;
          if (IIendmu() == 0) goto IIfdE2;
          goto IIfdB2;
IIfdE2:;
        }
/* # line 1499 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1500 "startup.sc" */	/* host code */
		    }
		    else
		    {
			arg1 = STalloc(ERget(S_ST0451_configure_auth_system));
			arg2 = STalloc(ERget(S_ST0452_auth_system_parameter));
			generic_independent_form(arg1, arg2, ERx("gcf"), NULL);
			MEfree( arg1 );
			MEfree( arg2 );
/* # line 1508 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1509 "startup.sc" */	/* host code */
		    }
		}
  		if( STcompare(comp_type, ERget(S_ST0578_ice_server))
  			== 0 )
  		{
			if( STcompare( comp_name,
				ERget( S_ST030F_default_settings ) ) == 0
			)
				STcopy( ERx( "*" ), comp_name );
			STprintf( pm_id, ERx( "icesvr.%s" ), comp_name );
  			arg1 = STalloc( ERget(
  				S_ST042F_config_internet_comm ) );
  			arg2 = STalloc( ERget( S_ST0430_internet_parameters ) );
  			generic_independent_form( arg1, arg2, pm_id,
                            comp_name);
  			MEfree( arg1 );
  			MEfree( arg2 );
/* # line 1528 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1529 "startup.sc" */	/* host code */
  		}
/* # line 1531 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"Sorry, unknown menuitem found. ");
        }
      } else if (IIretval() == 2) {
/* # line 1539 "startup.sc" */	/* inquire_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&activated,67,
            ERget( S_ST0355_edit_count_menu ),0);
          } /* IIiqset */
        }
/* # line 1542 "startup.sc" */	/* host code */
		if (activated == 0 )
		{
/* # line 1544 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1545 "startup.sc" */	/* host code */
		}
		else
			activated = 0;
		if ( ! STcompare( comp_type , ERget(S_ST0300_name_server ) ) )
/* # line 1550 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1552 "startup.sc" */	/* host code */
		if ( ! STcompare( comp_type , ERget(S_ST034E_security_menu) ) )
/* # line 1553 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1556 "startup.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"startup_config",(char *)"components",-2) !=
           0) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,comp_type,(char *)
"type");
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,comp_name,(char *)
"name");
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,enabled_val,(char *)
"enabled");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1562 "startup.sc" */	/* host code */
		/* prompt for new startup count */
/* # line 1563 "startup.sc" */	/* prompt */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IIprmptio(0,ERget( S_ST0310_start_count_prompt ),(short *)0,1,32,
          BIG_ENOUGH-1,in_buf);
        }
/* # line 1568 "startup.sc" */	/* host code */
		if( STcompare( comp_name,
			ERget( S_ST030F_default_settings ) ) == 0
		)
			STcopy( ERx( "*" ), comp_name );
		else if ( STcompare( comp_name,
			ERget( S_ST030A_not_configurable ) ) == 0
		) 
			STcopy( ERx( "*" ), comp_name );
		if( STcompare( comp_type ,
			ERget( S_ST0307_dbms_server ) ) == 0
		)
			STcopy( ERx( "dbms" ), comp_type );
		else if( STcompare( comp_type ,
			ERget( S_ST0308_net_server ) ) == 0
		)
			STcopy( ERx( "gcc" ), comp_type );
		else if( STcompare( comp_type ,
			ERget( S_ST0690_das_server ) ) == 0
		)
			STcopy( ERx( "gcd" ), comp_type );
		else if( STcompare( comp_type ,
			ERget( S_ST052B_bridge_server ) ) == 0
		)
			STcopy( ERx( "gcb" ), comp_type );
		else if( STcompare( comp_type ,
			ERget( S_ST0309_star_server ) ) == 0
		)
			STcopy( ERx( "star" ), comp_type );
		else if( STcompare( comp_type ,
			ERget( S_ST02FE_rmcmd ) ) == 0
		)
		{
			STcopy( ERx( "rmcmd" ), comp_type );
			if (STcompare(&in_buf[0],"1")!=0  && 
			    STcompare(&in_buf[0],"0")!=0)
			{
/* # line 1609 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0448_startupcnt_zero_one));
        }
/* # line 1612 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1613 "startup.sc" */	/* host code */
			}
		}
		else if (STcompare( comp_type, ERget(S_ST0578_ice_server))==0)
		{
			STcopy( ERx( "icesvr" ), comp_type );
			if (STcompare(&in_buf[0],"1")!=0  && 
			    STcompare(&in_buf[0],"0")!=0)
			{
/* # line 1621 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0448_startupcnt_zero_one));
        }
/* # line 1624 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1625 "startup.sc" */	/* host code */
			}
		}
		else if (STcompare( comp_type, 
				ERget( S_ST055C_oracle_gateway)) == 0)
		{
			STcopy( ERx( "oracle" ), comp_type );
			if (STcompare(&in_buf[0],"1")!=0  && 
			    STcompare(&in_buf[0],"0")!=0)
			{
/* # line 1634 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0448_startupcnt_zero_one));
        }
/* # line 1637 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1638 "startup.sc" */	/* host code */
			}
		}
		else if(STcompare( comp_type , 
				ERget( S_ST055F_informix_gateway )) == 0)
		{
			STcopy( ERx( "informix" ), comp_type );
			if (STcompare(&in_buf[0],"1")!=0  && 
			    STcompare(&in_buf[0],"0")!=0)
			{
/* # line 1647 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0448_startupcnt_zero_one));
        }
/* # line 1650 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1651 "startup.sc" */	/* host code */
			}
		}
		else if(STcompare( comp_type , 
				ERget( S_ST0563_sybase_gateway ) ) == 0)
		{
			STcopy( ERx( "sybase" ), comp_type );
			if (STcompare(&in_buf[0],"1")!=0  && 
			    STcompare(&in_buf[0],"0")!=0)
			{
/* # line 1660 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0448_startupcnt_zero_one));
        }
/* # line 1663 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1664 "startup.sc" */	/* host code */
			}
		}
		else if (STcompare( comp_type , 
				ERget( S_ST0566_mssql_gateway )) == 0)
		{
			STcopy( ERx( "mssql" ), comp_type );
			if (STcompare(&in_buf[0],"1")!=0  && 
			    STcompare(&in_buf[0],"0")!=0)
			{
/* # line 1673 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0448_startupcnt_zero_one));
        }
/* # line 1676 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1677 "startup.sc" */	/* host code */
			}
		}
		else if(STcompare( comp_type , 
				ERget( S_ST0569_odbc_gateway)) == 0)
		{
			STcopy( ERx( "odbc" ), comp_type );
			if (STcompare(&in_buf[0],"1")!=0  && 
			    STcompare(&in_buf[0],"0")!=0)
			{
/* # line 1686 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0448_startupcnt_zero_one));
        }
/* # line 1689 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1690 "startup.sc" */	/* host code */
			}
		}
		else if( STcompare( comp_type ,
			ERget( S_ST0677_db2udb_gateway ) ) == 0)
		{
			STcopy( ERx( "db2udb" ), comp_type );
                       if (STcompare(&in_buf[0],"1")!=0  &&
                            STcompare(&in_buf[0],"0")!=0)
                        {
/* # line 1699 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0448_startupcnt_zero_one));
        }
/* # line 1702 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1703 "startup.sc" */	/* host code */
                        }
                }
		else if( STcompare( comp_type ,
			ERget( S_ST0644_rms_gateway ) ) == 0)
		{
			STcopy( ERx( "rms" ), comp_type );
                       if (STcompare(&in_buf[0],"1")!=0  &&
                            STcompare(&in_buf[0],"0")!=0)
                        {
/* # line 1713 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0448_startupcnt_zero_one));
        }
/* # line 1716 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1717 "startup.sc" */	/* host code */
                        }
                }
		else if (STcompare( comp_type, 
				ERget( S_ST0649_rdb_gateway)) == 0)
		{
			STcopy( ERx( "rdb" ), comp_type );
			if (STcompare(&in_buf[0],"1")!=0  && 
			    STcompare(&in_buf[0],"0")!=0)
			{
/* # line 1727 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0448_startupcnt_zero_one));
        }
/* # line 1730 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1731 "startup.sc" */	/* host code */
			}
		}
		STprintf( temp_buf, ERx( "%s.%s.%sstart.%s.%s" ),
			  SystemCfgPrefix, host, SystemExecName, comp_name, 
			  comp_type );
		if( set_resource( temp_buf, in_buf ) )
		{
/* # line 1740 "startup.sc" */	/* putrow */
        {
          if (IItbsetio(3,(char *)"startup_config",(char *)"components",-2) !=
           0) {
            IItcoputio((char *)"enabled",(short *)0,1,32,0,in_buf);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1743 "startup.sc" */	/* host code */
		}
      } else if (IIretval() == 3) {
/* # line 1750 "startup.sc" */	/* inquire_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&activated,67,
            ERget( S_ST0351_duplicate_menu ),0);
          } /* IIiqset */
        }
/* # line 1753 "startup.sc" */	/* host code */
		if (activated == 0)
		{
/* # line 1755 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1756 "startup.sc" */	/* host code */
		}
		else
			activated = 0;
/* # line 1760 "startup.sc" */	/* prompt */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IIprmptio(0,ERget( S_ST032E_create_prompt ),(short *)0,1,32,
          BIG_ENOUGH-1,in_buf);
        }
/* # line 1765 "startup.sc" */	/* message */
        {
          IImessage(ERget( S_ST0341_duplicating ));
        }
/* # line 1767 "startup.sc" */	/* host code */
		copy_rename_nuke( in_buf, TRUE );
      } else if (IIretval() == 4) {
/* # line 1774 "startup.sc" */	/* inquire_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&activated,67,ERget( FE_Rename ),0);
          } /* IIiqset */
        }
/* # line 1777 "startup.sc" */	/* host code */
		if ( activated == 0 )
		{
/* # line 1779 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1780 "startup.sc" */	/* host code */
		}
		else 
			activated = 0;
/* # line 1784 "startup.sc" */	/* prompt */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IIprmptio(0,ERget( S_ST032F_rename_prompt ),(short *)0,1,32,
          BIG_ENOUGH-1,in_buf);
        }
/* # line 1789 "startup.sc" */	/* message */
        {
          IImessage(ERget( S_ST0342_renaming ));
        }
/* # line 1791 "startup.sc" */	/* host code */
		copy_rename_nuke( in_buf, FALSE );
      } else if (IIretval() == 5) {
		{
		i4 n;
/* # line 1800 "startup.sc" */	/* inquire_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&activated,67,ERget( FE_Destroy ),0);
          } /* IIiqset */
        }
/* # line 1803 "startup.sc" */	/* host code */
		if ( activated == 0 )
		{
/* # line 1805 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1806 "startup.sc" */	/* host code */
		}
		else	
			activated = 0;
/* # line 1810 "startup.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"startup_config",(char *)"components",-2) !=
           0) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,enabled_val,(char *)
"enabled");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1814 "startup.sc" */	/* host code */
		CVan( enabled_val, &n );
		if( n > 0 )
		{
/* # line 1817 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0374_startup_not_zero ));
        }
/* # line 1819 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1820 "startup.sc" */	/* host code */
		}
		if( CONFCH_YES != IIUFccConfirmChoice( CONF_GENERIC,
			NULL, NULL, NULL, NULL,
			S_ST0340_destroy_prompt,
			S_ST0384_destroy_component,
			S_ST0385_no_destroy_component,
			NULL, TRUE ) )
		{
/* # line 1829 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1830 "startup.sc" */	/* host code */
		}
/* # line 1832 "startup.sc" */	/* message */
        {
          IImessage(ERget( S_ST0343_destroying ));
        }
/* # line 1834 "startup.sc" */	/* host code */
		copy_rename_nuke( NULL, FALSE );
		}
      } else if (IIretval() == 6) {
		if( browse_file( &change_log_file,
			ERget( S_ST0347_change_log_title ), TRUE ) != OK )
		{
/* # line 1845 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0348_no_change_log ));
        }
/* # line 1847 "startup.sc" */	/* host code */
		}
      } else if (IIretval() == 7) {
		char *arg1, *arg2;
		arg1 = STalloc( ERget( S_ST0372_configure_preferences ) );
		arg2 = STalloc( ERget( S_ST0373_system_preferences ) );
		generic_independent_form( arg1, arg2, ERx( "prefs" ), NULL );
		MEfree( arg1 );
		MEfree( arg2 );
/* # line 1861 "startup.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
      } else if (IIretval() == 8) {
/* # line 1868 "startup.sc" */	/* host code */
		if( !get_help( ERx( "cbf.general" ) ) )
/* # line 1869 "startup.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"No help available.");
        }
      } else if (IIretval() == 9) {
/* # line 1876 "startup.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
      } else if (IIretval() == 10) {
/* # line 1881 "startup.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"startup_config",(char *)"components",-2) !=
           0) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,comp_type,(char *)
"type");
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,comp_name,(char *)
"name");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1885 "startup.sc" */	/* host code */
		if(
#if !defined(WORKGROUP_EDITION) || defined(int_lnx) || defined (int_rpl) \
                                || defined(ibm_lnx)
			STcompare( comp_type, ERget( S_ST0307_dbms_server ) )
			== 0 ||
#endif /* WORKGROUP_EDITION || int_lnx || int_rpl || ibm_lnx */
			STcompare( comp_type, ERget( S_ST0308_net_server ) )
			== 0 || 
			STcompare( comp_type, ERget( S_ST0690_das_server ) )
			== 0 || 
			STcompare( comp_type, ERget( S_ST0309_star_server ) )
			== 0 ||
			STcompare( comp_type, ERget( S_ST0644_rms_gateway ) )
			== 0 ||
			STcompare( comp_type, ERget( S_ST0677_db2udb_gateway ) )
			== 0 ||
			STcompare( comp_type, ERget( S_ST052B_bridge_server ) )
			== 0 )
		{
/* # line 1906 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0350_configure_menu ),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1910 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0355_edit_count_menu ),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1914 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0351_duplicate_menu ),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1918 "startup.sc" */	/* host code */
			if( STcompare( comp_name,
				ERget( S_ST030F_default_settings ) ) == 0 )
			{
/* # line 1921 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1924 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Rename ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1927 "startup.sc" */	/* host code */
			}
			else
			{
/* # line 1930 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1933 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Rename ),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1936 "startup.sc" */	/* host code */
			}
		}
		else if(
			STcompare( comp_type,
				ERget( S_ST02FE_rmcmd ) ) == 0 )
		{
/* # line 1942 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0350_configure_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1946 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0355_edit_count_menu ),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1950 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0351_duplicate_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1954 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1957 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Rename ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1960 "startup.sc" */	/* host code */
		}
		else if(
			STcompare( comp_type,
				ERget( S_ST0578_ice_server ) ) == 0 ||
			STcompare( comp_type,
				ERget( S_ST055C_oracle_gateway ) ) == 0 ||
			STcompare( comp_type,
				ERget( S_ST055F_informix_gateway ) ) == 0 ||
			STcompare( comp_type,
				ERget( S_ST0563_sybase_gateway ) ) == 0 ||
			STcompare( comp_type,
				ERget( S_ST0566_mssql_gateway ) ) == 0 ||
			STcompare( comp_type,
				ERget( S_ST0569_odbc_gateway ) ) == 0 ||
			STcompare( comp_type,
				ERget( S_ST0677_db2udb_gateway ) ) == 0 ||
			STcompare( comp_type,
				ERget( S_ST0649_rdb_gateway ) ) == 0 
		)
		{
/* # line 1980 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0350_configure_menu ),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1984 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0355_edit_count_menu ),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1988 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST034D_preferences_menu ),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1992 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST035F_change_log_menu ),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1996 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0351_duplicate_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2000 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2003 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Rename ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2006 "startup.sc" */	/* host code */
		}
		else if(
			STcompare( comp_type,
				ERget( S_ST0305_transaction_log ) ) == 0 )
		{
/* # line 2012 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0350_configure_menu ),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 2016 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0355_edit_count_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2020 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0351_duplicate_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2024 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2027 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Rename ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2030 "startup.sc" */	/* host code */
		}
		else if(
			STcompare( comp_type,
				ERget( S_ST0300_name_server ) ) == 0 ||
			STcompare( comp_type,
				ERget( S_ST0303_logging_system ) ) == 0 ||
			STcompare( comp_type,
                                ERget( S_ST0304_locking_system ) ) == 0 ||
                        STcompare( comp_type,
                                ERget( S_ST0301_recovery_server ) ) == 0 ||
                        STcompare( comp_type,
                                ERget( S_ST0305_transaction_log ) ) == 0 ||
                        STcompare( comp_type,
                                ERget( S_ST034E_security_menu ) ) == 0 )
		{
/* # line 2045 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0350_configure_menu ),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 2049 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0355_edit_count_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2053 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0351_duplicate_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2057 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2060 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Rename ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2063 "startup.sc" */	/* host code */
		}
		else if(
			STcompare( comp_type,
				ERget( S_ST0302_archiver_process ) ) == 0 )
		{
/* # line 2068 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0350_configure_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2072 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0355_edit_count_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2076 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0351_duplicate_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2080 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2083 "startup.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Rename ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2086 "startup.sc" */	/* host code */
		}
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    goto IIfdE1;
IImuI1:
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget( S_ST0350_configure_menu ),(char *)0,2,2,1) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(11,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget( S_ST0355_edit_count_menu ),(char *)0,2,2,2) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(12,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget( S_ST0351_duplicate_menu ),(char *)0,2,2,3) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(13,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact(ERget( FE_Rename ),(char *)0,2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(14,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact(ERget( FE_Destroy ),(char *)0,2,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(15,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact(ERget( S_ST035F_change_log_menu ),(char *)0,2,2,6) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(18,(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IInmuact(ERget( S_ST034D_preferences_menu ),(char *)0,2,2,7) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(19,(char *)0,2,2,7) == 0) goto IIfdE1;
    if (IInmuact(ERget( FE_Help ),(char *)0,2,2,8) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,8) == 0) goto IIfdE1;
    if (IInmuact(ERget( FE_Quit ),(char *)0,2,2,9) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(2,(char *)0,2,2,9) == 0) goto IIfdE1;
    if (IITBcaClmAct((char *)"components",(char *)"all",1,10) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
}
void
scan_priv_cache_parm(char *instance, char *new, char *page, 
	bool keep_old)
{
    PM_SCAN_REC	state;
    STATUS	status;
    char	*last, *name, *value, *param, *regexp;
    char	resource[BIG_ENOUGH];
    char	expbuf[BIG_ENOUGH];
    if (page == NULL)
   	STprintf( expbuf, ERx( "%s.%s.%s.private.%s.%%" ),
   		  SystemCfgPrefix, host, cbf_srv_name, instance );
    else
	STprintf( expbuf, ERx( "%s.%s.%s.private.%s.%s.%%" ),
		  SystemCfgPrefix, host, cbf_srv_name, instance, page);
    regexp = PMmExpToRegExp( config_data, expbuf );
    for( status = PMmScan( config_data, regexp, &state,
		NULL, &name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, last, &name, &value ) )
    {
	/* set 'last' to handle deletion during scan */
	last = name;
	/* extract parameter name */
	if (page == NULL)
	    param = PMmGetElem( config_data, 5, name );
	else
	    param = PMmGetElem( config_data, 6, name );
	/* destroy old resource if 'keep_old' not TRUE */ 
	if( !keep_old && PMmDelete( config_data, name ) == OK )
	{
	    SIfprintf( change_log_fp, ERx( "DELETE %s\n" ), name );
	    SIflush( change_log_fp );
	}
	/* insert resource if 'new' not NULL */ 
	if( new != NULL )
	{
	    /* compose duplicate resource name */
	    if (page == NULL)
	        STprintf( resource, ERx( "%s.%s.%s.private.%s.%s" ), 
			  SystemCfgPrefix, host, cbf_srv_name,
                          new, param );
	    else
		STprintf( resource, ERx( "%s.%s.%s.private.%s.%s.%s" ), 
			  SystemCfgPrefix, host, cbf_srv_name,
                          new, page, param);
	    if( PMmInsert( config_data, resource, value ) == OK )
	    {
		SIfprintf( change_log_fp, ERx( "CREATE %s: (%s)\n" ),
			resource, value );
		SIflush( change_log_fp );
	    }
	}
    }
}
