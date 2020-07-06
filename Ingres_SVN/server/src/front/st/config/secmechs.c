# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  Copyright (c) 2004 Ingres Corporation
**
**  Name: secmechs.sc -- contains function which controls the Security
**	mechanism configuration form of the INGRES configuration utility.
**
**  History:
**	26-Jul-98 (gordy)
**	    Created.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# include	<compat.h>
# include       <cv.h>
# include	<si.h>
# include	<ci.h>
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
# include	<stdprmpt.h>
# include	<pc.h>
# include	<pm.h>
# include	<pmutil.h>
# include	<util.h>
# include	<simacro.h>
# include	"cr.h"
# include	"config.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
  GLOBALREF char *host;
  GLOBALREF char *ii_system;
  GLOBALREF char *ii_system_name;
  GLOBALREF char *ii_installation;
  GLOBALREF char *ii_installation_name;
GLOBALREF PM_CONTEXT	*config_data;
GLOBALREF LOCATION	change_log_file;
GLOBALREF PM_CONTEXT	*units_data;
  static char *sec_mech = ERx("security_mechanisms");
static bool	mechanism_form( char * );
#define	MECH_MAX	10
/*
** Name: security_mech_form
**
** Description:
**	Generates a listpick popup window of mechanisms and permits
**	user to select a mechanism for configuration.  Displays
**	mechanism configuration form for selected mechanism.
**
** Input:
**	None.
**
** Output:
**	None.
**
** Returns:
**	bool		TRUE if configuration changed, FALSE otherwise.
**
** History:
**	26-Jul-98 (gordy)
**	    Created.
**	 1-Sep-98 (gordy)
**	    Initialize mech_cnt to 0.
*/
bool
security_mech_form( VOID )
{
    PM_SCAN_REC	state;
    STATUS	status;
    char	title[ BIG_ENOUGH ];
    char	*regexp, expbuf[ BIG_ENOUGH ];
    char	*mech_list, *mechs[ MECH_MAX ];
    char	*name, *value;
    char	*ele, *prev = NULL;
    i4		mech, mech_cnt = 0;
    i4		len = 0;
    bool	changed = FALSE;
    /*
    ** Build list of mechanism for PM config symbols.
    */
    STprintf( expbuf, ERx("%s.%s.gcf.mech.%%.%%"), SystemCfgPrefix, host );
    regexp = PMmExpToRegExp( config_data, expbuf );
    for( 
	 status = PMmScan( config_data, regexp, &state, NULL, &name, &value );
	 status == OK;
	 status = PMmScan( config_data, NULL, &state, NULL, &name, &value )
       )
    {
	ele = PMmGetElem( config_data, 4, name );
	if ( ! prev  ||  STcompare( ele, prev ) )
	{
	    len += STlength( ele ) + 1;
	    mechs[ mech_cnt ] = STalloc( ele );
	    prev = mechs[ mech_cnt ];
	    if ( ++mech_cnt >= MECH_MAX )  break;
	}
    }
    mech_list = (char *)MEreqmem( 0, len + 1, FALSE, NULL );
    for( mech = 0, mech_list[0] = EOS; mech < mech_cnt; mech++ )
    {
	STcat( mech_list, mechs[ mech ] );
	STcat( mech_list, "\n" );
    }
    /*
    ** Prompt for mechanism using list pick pop-up.
    */
    STcopy( ERget( S_ST0456_select_mechanism ), title );
    mech = IIFDlpListPick( title, mech_list,
			   MECH_MAX, -1, -1, NULL, NULL, NULL, NULL );
    if ( mech >= 0 )  changed = mechanism_form( mechs[ mech ] );
    /*
    ** Free memory.
    */
    for( mech = 0; mech < mech_cnt; mech++ )  MEfree( mechs[ mech ] );
    MEfree( mech_list );
    return( changed );
}
/*
** Name: mechanism_form
**
** Description:
**	Drives the mechanism configuration form for the requested
**	mechanism.
**
** Input:
**	mech_name	Name of mechanism to configure.
**
** Output:
**	None.
**
** Returns:
**	bool		TRUE if configuration changed, FALSE otherwise.
**
** History:
**	26-Jul-98 (gordy)
**	    Created.
**      16-Jul-2007 (Ralph Loen) Bug 118746
**          Replaced generic help page of the mechanisms with specific
**          mechanism help pages. 
*/
static bool
mechanism_form( char *mech_name )
{
  char *name, *value;
  char title[ BIG_ENOUGH ];
  char in_buf[ BIG_ENOUGH ];
  char name_val[ BIG_ENOUGH ];
  char description[ BIG_ENOUGH ];
  char temp_buf[ MAX_LOC + 1 ];
  char *my_name = mech_name;
    LOCATION	*IIUFlcfLocateForm();
    STATUS	IIUFgtfGetForm();
    PM_SCAN_REC	state;
    STATUS	status;
    bool	changed = FALSE;
    char	expbuf[ BIG_ENOUGH ];
    char	*p, *regexp;
    i4		i, len;
    bool        help_status;
    STcopy( ERget( S_ST0454_configure_mechanism ), title );
    if ( IIUFgtfGetForm( IIUFlcfLocateForm(), sec_mech ) != OK )
	IIUGerr( S_ST031A_form_not_found, UG_ERR_FATAL, 1, sec_mech );
    /* center description (62 characters wide) */
    STcopy( ERget( S_ST0455_mechanism_parameter ), temp_buf );
    len = (62 - STlength( temp_buf )) / 2;
    for( i = 0, p = description; i < len; i++, CMnext( p ) )
	STcopy( ERx( " " ), p );	
    STcopy( temp_buf, p );
/* # line 217 "secmechs.sc" */	/* inittable */
  {
    if (IItbinit(sec_mech,(char *)"params",(char *)"r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 218 "secmechs.sc" */	/* display */
  {
    if (IIdispfrm(sec_mech,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    IIputfldio((char *)"title",(short *)0,1,32,0,title);
    IIputfldio((char *)"host",(short *)0,1,32,0,host);
    IIputfldio((char *)"ii_installation",(short *)0,1,32,0,ii_installation);
    IIputfldio((char *)"ii_installation_name",(short *)0,1,32,0,
    ii_installation_name);
    IIputfldio((char *)"ii_system",(short *)0,1,32,0,ii_system);
    IIputfldio((char *)"ii_system_name",(short *)0,1,32,0,ii_system_name);
    IIputfldio((char *)"description",(short *)0,1,32,0,description);
    IIputfldio((char *)"def_name",(short *)0,1,32,0,my_name);
/* # line 234 "secmechs.sc" */	/* host code */
	STprintf( expbuf, ERx( "%s.%s.gcf.mech.%s.%%" ), SystemCfgPrefix, 
		  host, my_name );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( 
	     status = PMmScan(config_data, regexp, &state, NULL, &name, &value);
	     status == OK; 
	     status = PMmScan( config_data, NULL, &state, NULL, &name, &value ) 
	   )
	{
    char *units;
	    if ( PMmGet( units_data, name, &units ) != OK )  units = ERx("");
	    name = PMmGetElem( config_data, 5, name );
/* # line 252 "secmechs.sc" */	/* loadtable */
    {
      if (IItbact(sec_mech,(char *)"params",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,name);
        IItcoputio((char *)"value",(short *)0,1,32,0,value);
        IItcoputio((char *)"units",(short *)0,1,32,0,units);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 254 "secmechs.sc" */	/* host code */
	}
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 261 "secmechs.sc" */	/* getrow */
        {
          if (IItbsetio(2,sec_mech,(char *)"params",-2) != 0) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,name_val,(char *)"\
name");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 263 "secmechs.sc" */	/* host code */
	STprintf( temp_buf, ERx( "%s.%s.gcf.mech.%s.%s" ), 
		  SystemCfgPrefix, host, my_name, name_val );
/* # line 266 "secmechs.sc" */	/* prompt */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IIprmptio(0,ERget( S_ST0328_value_prompt ),(short *)0,1,32,
          BIG_ENOUGH-1,in_buf);
        }
/* # line 269 "secmechs.sc" */	/* host code */
	if ( set_resource( temp_buf, in_buf ) )
	{
	    changed = TRUE;
/* # line 272 "secmechs.sc" */	/* putrow */
        {
          if (IItbsetio(3,sec_mech,(char *)"params",-2) != 0) {
            IItcoputio((char *)"value",(short *)0,1,32,0,in_buf);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 273 "secmechs.sc" */	/* host code */
	}
      } else if (IIretval() == 2) {
	if ( browse_file( &change_log_file,
			  ERget( S_ST0347_change_log_title ), TRUE ) != OK )
	{
/* # line 281 "secmechs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget(S_ST0348_no_change_log));
        }
/* # line 282 "secmechs.sc" */	/* host code */
	}
      } else if (IIretval() == 3) {
/* # line 288 "secmechs.sc" */	/* getrow */
        {
          if (IItbsetio(2,sec_mech,(char *)"params",-2) != 0) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,name_val,(char *)"\
name");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 290 "secmechs.sc" */	/* host code */
    if ( ! STcompare( my_name, "ingres" ) )
    {
        if ( ! STcompare ( name_val, "enabled" ) )
            help_status = get_help ( ERx("gcf.mech.ingres.enabled") ); 
        else if ( ! STcompare ( name_val, "expiration_time" ) )
            help_status = get_help ( ERx("gcf.mech.ingres.expiration_time") ); 
        else if ( ! STcompare ( name_val, "expirations_required" ) )
            help_status = 
                 get_help ( ERx("gcf.mech.ingres.expirations_required") ); 
    }
    else if ( ! STcompare( my_name, "kerberos" ) )
    {
        if ( ! STcompare ( name_val, "enabled" ) )
            help_status = get_help ( ERx("gcf.mech.kerberos.enabled") ); 
        else if ( ! STcompare ( name_val, "domain" ) )
            help_status = get_help ( ERx("gcf.mech.kerberos.domain") ); 
        else if ( ! STcompare ( name_val, "entry" ) )
            help_status = get_help ( ERx("gcf.mech.kerberos.entry") ); 
        else if ( ! STcompare ( name_val, "module" ) )
            help_status = get_help ( ERx("gcf.mech.kerberos.module") ); 
        else if ( ! STcompare ( name_val, "delegation" ) )
            help_status = get_help ( ERx("gcf.mech.kerberos.delegation") ); 
    }
    else if ( ! STcompare( my_name, "null" ) )
    {
        if ( ! STcompare ( name_val, "enabled" ) )
            help_status = get_help ( ERx("gcf.mech.null.enabled") ); 
    }
    else if ( ! STcompare( my_name, "system" ) )
    {
        if ( ! STcompare ( name_val, "enabled" ) )
            help_status = get_help ( ERx("gcf.mech.system.enabled") ); 
    }
    else 
        help_status = get_help( ERx("gcf.mech") );
        if ( !help_status ) 
	{
/* # line 328 "secmechs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget(S_ST0346_no_help_available));
        }
/* # line 330 "secmechs.sc" */	/* host code */
	}
      } else if (IIretval() == 4) {
/* # line 335 "secmechs.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
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
    if (IInmuact(ERget( FE_Edit ),(char *)0,2,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(12,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget( S_ST035F_change_log_menu ),(char *)0,2,2,2) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(18,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget( FE_Help ),(char *)0,2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact(ERget( FE_End ),(char *)0,2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 338 "secmechs.sc" */	/* host code */
    return( changed );
}
