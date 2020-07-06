# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
**
**  Name: netprots.sc -- contains function which controls the Net server
**	protocol configuration form of the INGRES configuration utility.
**
**  History:
**	14-dec-92 (tyler)
**		Created.
**	21-jun-93 (tyler)
**		Switched to new interface for multiple context PM
**		functions.
**	23-jul-93 (tyler)
**		Changed multiple context PM interfaces again. 
**	04-aug-93 (tyler)
**		Removed Restore menu item.
**	19-oct-93 (tyler)
**		Changed type of global variable "host" to (char *).
**	22-nov-93 (tyler)
**		Implemented SIRS for the Net group.  Removed dead
**		code.  Listen address changes are now include in the
**		configuration change log and this form now supports
**		the ChangeLog menu item.
**	01-dec-93 (tyler)
**		Fixed bad calls to IIUGerr().
**	22-feb-94 (tyler)
**		Fixed BUG 59434: asssign Help menu item to PF2.
**	30-Nov-94 (nanpr01)
**		Fixed BUG 60371: Added frskey mappings so that users can
**		have uniform key board mapping from screen to screen.
**		define frskey11-frskey19
**	07-Feb-96 (rajus01)
**		Added bridge_protocols_form() function for Protocol Bridge.
**	14-Feb-96 (rajus01)
**		Added '==>Vnode' field and additional processing to 
**		bridge_protocols_form.
**	10-oct-1996 (canor01)
**		Replace hard-coded 'ii' prefixes with SystemCfgPrefix.
**	20-dec-1996 (reijo01)
**		Initialize bridge_protocols form with globals 
**		ii_system_name and ii_installation_name
**	21-aug-97 (mcgem01)
**		Correct order of parameters when writing out the port
**		identifier.
**	 1-Oct-98 (gordy)
**		Generalized the net protocols form so that it can also
**		be used for installation registry protocols.
**      17-aug-2001 (wansh01) 
**              Deleted edit_vnode option. Added create vnode and
**              destroy vnode options for bridge configuration. 
**              Add add_bridge_vnode() and protocol_listpick() to
**              handle the create vnode option. 
**      26-Feb-2003 (wansh01) 
**              Added protocol configuration for DAS server. It used
** 		the generialized net protocols form.
**	07-04-2003 (wansh01)
**		Added gcd protocols help support. 
**	04-Oct-2004 (hweho01)
**              Added uf.h include file, the correct prototype
**              declarations for IIUFgtfGetForm and IIUFlcfLocateForm 
**              are needed.
*/
# include <compat.h>
# include <si.h>
# include <ci.h>
# include <st.h>
# include <me.h>
# include <lo.h>
# include <ex.h>
# include <er.h>
# include <cm.h>
# include <nm.h>
# include <gl.h>
# include <sl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <ui.h>
# include <te.h>
# include <erst.h>
# include <uigdata.h>
# include <stdprmpt.h>
# include <pc.h>
# include <pm.h>
# include <pmutil.h>
# include <util.h>
# include <simacro.h>
# include "cr.h"
# include "config.h"
# include <uf.h>
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
/*
** global variables
*/
  GLOBALREF char *host;
  GLOBALREF char *ii_system;
  GLOBALREF char *ii_system_name;
  GLOBALREF char *ii_installation;
  GLOBALREF char *ii_installation_name;
GLOBALREF LOCATION	config_file;
GLOBALREF PM_CONTEXT	*config_data;
GLOBALREF PM_CONTEXT	*units_data;
GLOBALREF LOCATION	change_log_file;
static bool add_bridge_vnode(char *, char *, char *, char *);
static char *protocol_listpick(char *);
static bool     pm_initialized = FALSE;
/*
** local forms
*/
  static char *net_protocols = ERx( "net_protocols" );
  static char *bridge_protocols = ERx( "bridge_protocols" );
  static char *bridge_addvnode = ERx( "bridge_addvnode" );
bool
net_protocols_form( char *net_id, char *instance )
{
  char *def_name;
  char in_buf[ BIG_ENOUGH ];
  char *name, *value, *title;
  char expbuf[ BIG_ENOUGH ];
  char description[ BIG_ENOUGH ];
	STATUS status;
	PM_SCAN_REC state;
	bool changed = FALSE;
	char *regexp, *p;
	i4 len, i;
	char request[ BIG_ENOUGH ]; 
	char temp_buf[ BIG_ENOUGH ];
	if (IIUFgtfGetForm( IIUFlcfLocateForm(), net_protocols ) != OK)
 	 IIUGerr( S_ST031A_form_not_found, UG_ERR_FATAL, 1,
			net_protocols );
	if ( !STcompare( net_id, ERx( "gcb" ) ) )
	{
	   title = STalloc( ERget( S_ST041D_configure_bridge_prot));
	   STcopy( ERget( S_ST041E_bridge_protocols_parm),description);
	}
        else
	if ( !STcompare( net_id, ERx( "gcd" ) ) )
	{
	   title = STalloc( ERget( S_ST069C_configure_das_prot));
	   STcopy( ERget( S_ST069D_das_protocols_parm),description);
	}
	else
	{
	   title = STalloc( ERget( S_ST044C_configure_net_prot) );
	   if ( !STcompare( net_id, ERx( "gcc" ) ) )
	      STcopy ( ERget( S_ST044D_net_protocols_parm),description);
           else
	      STcopy ( ERget( S_ST044E_registry_proto_parm),description);
	}
        /* center description (62 characters wide) */
	STcopy( description, temp_buf );
	len = (62 - STlength( temp_buf )) / 2;
	for( i = 0, p = description; i < len; i++,
			CMnext( p ) )
 	    STcopy( ERx( " " ), p );
	STcopy( temp_buf, p );
	if ( ! instance )  
	    def_name = STalloc( net_id );
	else  if ( ! STcompare( instance, ERx( "*" ) ) )
	    def_name = STalloc( ERget( S_ST030F_default_settings ) );
	else
	    def_name = STalloc( instance );
/* # line 187 "netprots.sc" */	/* inittable */
  {
    if (IItbinit((char *)"net_protocols",(char *)"protocols",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 189 "netprots.sc" */	/* display */
  {
    if (IIdispfrm((char *)"net_protocols",(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    IIputfldio((char *)"host",(short *)0,1,32,0,host);
    IIputfldio((char *)"ii_installation",(short *)0,1,32,0,ii_installation);
    IIputfldio((char *)"ii_installation_name",(short *)0,1,32,0,
    ii_installation_name);
    IIputfldio((char *)"ii_system",(short *)0,1,32,0,ii_system);
    IIputfldio((char *)"ii_system_name",(short *)0,1,32,0,ii_system_name);
    IIputfldio((char *)"def_name",(short *)0,1,32,0,def_name);
    IIputfldio((char *)"title",(short *)0,1,32,0,title);
    IIputfldio((char *)"description",(short *)0,1,32,0,description);
/* # line 204 "netprots.sc" */	/* host code */
	/* construct expbuf for gcc port id scan */
	if ( ! instance )
	    STprintf( expbuf, ERx( "%s.%s.%s.%%.%%" ), 
		      SystemCfgPrefix, host, net_id );
	else
	    STprintf( expbuf, ERx( "%s.%s.%s.%s.%%.%%" ), 
		      SystemCfgPrefix, host, net_id, instance );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
    char *protocol;
    char *param;
    char *units;
		/* extract protocol name */
		protocol = PMmGetElem( config_data, instance ? 4 : 3, name ); 
		/* extract parameter name */
		param = PMmGetElem( config_data, instance ? 5 : 4, name );
		/* get units */
		if ( PMmGet( units_data, name, &units ) != OK )
		    units = ERx( "" );
/* # line 235 "netprots.sc" */	/* loadtable */
    {
      if (IItbact((char *)"net_protocols",(char *)"protocols",1) != 0) {
        IItcoputio((char *)"protocol",(short *)0,1,32,0,protocol);
        IItcoputio((char *)"name",(short *)0,1,32,0,param);
        IItcoputio((char *)"value",(short *)0,1,32,0,value);
        IItcoputio((char *)"units",(short *)0,1,32,0,units);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 241 "netprots.sc" */	/* host code */
	}
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        char protocol_id[ BIG_ENOUGH ];
        char parm[ BIG_ENOUGH ];
/* # line 252 "netprots.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"net_protocols",(char *)"protocols",-2) != 0
          ) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,protocol_id,(char *)
"protocol");
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,parm,(char *)"name");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 257 "netprots.sc" */	/* host code */
		if ( ! instance )
		    STprintf( request, ERx( "%s.%s.%s.%s.%s" ), 
			      SystemCfgPrefix, host, net_id, 
			      protocol_id, parm );
		else 
		    STprintf( request, ERx( "%s.%s.%s.%s.%s.%s" ), 
			      SystemCfgPrefix, host, net_id, 
			      instance, protocol_id, parm );
		if ( CRtype( CRidxFromPMsym( request ) ) == CR_BOOLEAN )
		{
/* # line 268 "netprots.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"net_protocols",(char *)"protocols",-2) != 0
          ) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,in_buf,(char *)"va\
lue");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 272 "netprots.sc" */	/* host code */
		    if ( ValueIsBoolTrue( in_buf ) )
			STcopy( ERx( "OFF" ), in_buf );
		    else
			STcopy( ERx( "ON" ), in_buf );
		}
		else
		{
/* # line 279 "netprots.sc" */	/* prompt */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IIprmptio(0,ERget( S_ST0328_value_prompt ),(short *)0,1,32,
          BIG_ENOUGH-1,in_buf);
        }
/* # line 283 "netprots.sc" */	/* host code */
		}
		if ( set_resource( request, in_buf ) )
		{
		    changed = TRUE;
/* # line 289 "netprots.sc" */	/* putrow */
        {
          if (IItbsetio(3,(char *)"net_protocols",(char *)"protocols",-2) != 0
          ) {
            IItcoputio((char *)"value",(short *)0,1,32,0,in_buf);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 292 "netprots.sc" */	/* host code */
		}
      } else if (IIretval() == 2) {
		if( browse_file( &change_log_file,
			ERget( S_ST0347_change_log_title ), TRUE ) != OK )
		{
/* # line 301 "netprots.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0348_no_change_log ));
        }
/* # line 303 "netprots.sc" */	/* host code */
		}
      } else if (IIretval() == 3) {
             if ( !STcompare( net_id, ERx( "gcb" ) ) )
	     {
		if( !get_help( ERx( "gcb.protocols" ) ) )
/* # line 311 "netprots.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"No help available.");
        }
/* # line 313 "netprots.sc" */	/* host code */
             }
	     else 
             if ( !STcompare( net_id, ERx( "gcd" ) ) )
	     {
		if( !get_help( ERx( "gcd.protocols" ) ) )
/* # line 318 "netprots.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"No help available.");
        }
/* # line 320 "netprots.sc" */	/* host code */
             }
	     else 
	     {
		if( !get_help( ERx( "gcc.protocols" ) ) )
/* # line 324 "netprots.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"No help available.");
        }
/* # line 326 "netprots.sc" */	/* host code */
             }
      } else if (IIretval() == 4) {
/* # line 331 "netprots.sc" */	/* breakdisplay */
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
/* # line 334 "netprots.sc" */	/* host code */
	MEfree( def_name );
	MEfree( title );
	return( changed );
}
bool
bridge_protocols_form( char *net_id )
{
  char *def_name;
  char in_buf[ BIG_ENOUGH ];
  char tmp_buf[ BIG_ENOUGH ];
  char *name, *value;
  char expbuf[ BIG_ENOUGH ];
  char protocol_id[ BIG_ENOUGH ], *protocol_status;
  char vnode_name[BIG_ENOUGH];
  char port_name[BIG_ENOUGH];
	LOCATION *IIUFlcfLocateForm();
	STATUS IIUFgtfGetForm();
	STATUS status;
	PM_SCAN_REC state;
	bool changed = FALSE;
	char *regexp;
	char request[ BIG_ENOUGH ]; 
	i4   protocols_rows = 0;    /* # of protocols in the protocol table  */
	if( IIUFgtfGetForm( IIUFlcfLocateForm(), bridge_protocols ) != OK )
	{
		IIUGerr( S_ST031A_form_not_found, UG_ERR_FATAL, 1,
			bridge_protocols );
	}
	if( STcompare( net_id, ERx( "*" ) ) == 0 )
		def_name = STalloc( ERget( S_ST030F_default_settings ) );
	else
		def_name = STalloc( net_id );
/* # line 373 "netprots.sc" */	/* inittable */
  {
    if (IItbinit((char *)"bridge_protocols",(char *)"protocols",(char *)"\
r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 375 "netprots.sc" */	/* display */
  {
    if (IIdispfrm((char *)"bridge_protocols",(char *)"f") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    IIputfldio((char *)"host",(short *)0,1,32,0,host);
    IIputfldio((char *)"ii_installation",(short *)0,1,32,0,ii_installation);
    IIputfldio((char *)"ii_installation_name",(short *)0,1,32,0,
    ii_installation_name);
    IIputfldio((char *)"ii_system",(short *)0,1,32,0,ii_system);
    IIputfldio((char *)"ii_system_name",(short *)0,1,32,0,ii_system_name);
    IIputfldio((char *)"def_name",(short *)0,1,32,0,def_name);
/* # line 389 "netprots.sc" */	/* host code */
	/* construct expbuf for gcb port id scan */
	STprintf( expbuf, ERx( "%s.%s.gcb.%s.%%.port.%%" ), 
		  SystemCfgPrefix, host, net_id );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
    char *vnode;
    char *protocol;
		/* extract protocol name */
		protocol = PMmGetElem( config_data, 4, name ); 
		/* extract vnode name */
		if( PMnumElem( name ) == 7 )
		   vnode = PMmGetElem( config_data, 6, name );
		/* get status of scanned protocol and vnode*/ 
		STprintf( request, ERx( "%s.%s.gcb.%s.%s.status.%s" ),
			  SystemCfgPrefix, host, net_id, protocol,vnode );
		if( PMmGet( config_data, request, &protocol_status )
			!= OK)	
                {
		    /* default to the status of scanned protocol */ 
		   STprintf( request, ERx( "%s.%s.gcb.%s.%s.status" ),
			  SystemCfgPrefix, host, net_id, protocol );
		   if( PMmGet( config_data, request, &protocol_status ))
			continue;
                }
/* # line 427 "netprots.sc" */	/* loadtable */
    {
      if (IItbact((char *)"bridge_protocols",(char *)"protocols",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,protocol);
        IItcoputio((char *)"port",(short *)0,1,32,0,value);
        IItcoputio((char *)"status",(short *)0,1,32,0,protocol_status);
        IItcoputio((char *)"vnode",(short *)0,1,32,0,vnode);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 433 "netprots.sc" */	/* host code */
                protocols_rows++;
	}
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
		char request[ BIG_ENOUGH ]; 
		/* get protocol id */
/* # line 443 "netprots.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"bridge_protocols",(char *)"protocol\
s",-2) != 0) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,protocol_id,(char *)
"name");
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,vnode_name,(char *)
"vnode");
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,in_buf,(char *)"st\
atus");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 450 "netprots.sc" */	/* host code */
	 	    if ( ValueIsBoolTrue( in_buf ) )
			STcopy( ERx( "OFF" ), in_buf );
		    else
			STcopy( ERx( "ON" ), in_buf );
		/* compose resource request for insertion */
		STprintf( request, ERx( "%s.%s.gcb.%s.%s.status.%s" ),
			  SystemCfgPrefix, host, net_id, protocol_id,
			  vnode_name);
		if( set_resource( request, in_buf ) )
		{
			/* update tablefield */
/* # line 463 "netprots.sc" */	/* putrow */
        {
          if (IItbsetio(3,(char *)"bridge_protocols",(char *)"protocol\
s",-2) != 0) {
            IItcoputio((char *)"status",(short *)0,1,32,0,in_buf);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 466 "netprots.sc" */	/* host code */
			changed = TRUE;
		}
      } else if (IIretval() == 2) {
		if (protocols_rows >0) 
		{
		/* get protocol id */
/* # line 475 "netprots.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"bridge_protocols",(char *)"protocol\
s",-2) != 0) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,protocol_id,(char *)
"name");
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,vnode_name,(char *)
"vnode");
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,port_name,(char *)
"port");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 481 "netprots.sc" */	/* host code */
		/* compose resource request for deletion */
		STprintf( request, ERx( "%s.%s.gcb.%s.%s.port.%s" ), 
			      SystemCfgPrefix, host,
			      net_id, protocol_id , vnode_name);
	        set_resource( request, NULL ); 
		STprintf( request, ERx( "%s.%s.gcb.%s.%s.status.%s" ), 
			      SystemCfgPrefix, host,
			      net_id, protocol_id , vnode_name);
	        set_resource( request, NULL ); 
		/* update tablefield */
/* # line 494 "netprots.sc" */	/* deleterow */
        {
          if (IItbsetio(4,(char *)"bridge_protocols",(char *)"protocol\
s",-2) != 0) {
            if (IItdelrow(0) != 0) {
            } /* IItdelrow */
          } /* IItbsetio */
        }
/* # line 496 "netprots.sc" */	/* host code */
		protocols_rows--;
		changed = TRUE;
		}
      } else if (IIretval() == 3) {
	     if (add_bridge_vnode(net_id,vnode_name,port_name,protocol_id))
	     {
	       /* compose resource request for insertion */
	       STprintf( request, ERx( "%s.%s.gcb.%s.%s.port.%s" ), 
		      SystemCfgPrefix, host, net_id, protocol_id, 
		      vnode_name );
	       set_resource( request, port_name  ); 
        	/*  default the status to ON  */ 	
	       STprintf( request, ERx( "%s.%s.gcb.%s.%s.status.%s" ), 
		      SystemCfgPrefix, host, net_id, protocol_id, 
		      vnode_name );
	       set_resource( request, ERx( "ON" ) ); 
		   /* insert the added nodes on top of the table */
/* # line 522 "netprots.sc" */	/* insertrow */
        {
          if (IItbsetio(5,(char *)"bridge_protocols",(char *)"protocol\
s",0) != 0) {
            if (IItinsert() != 0) {
              IItcoputio((char *)"port",(short *)0,1,32,0,port_name);
              IItcoputio((char *)"status",(short *)0,1,32,0,ERx( "ON" ));
              IItcoputio((char *)"name",(short *)0,1,32,0,protocol_id);
              IItcoputio((char *)"vnode",(short *)0,1,32,0,vnode_name);
              IITBceColEnd();
            } /* IItinsert */
          } /* IItbsetio */
        }
/* # line 529 "netprots.sc" */	/* host code */
	       protocols_rows++; 
	       changed = TRUE;
             }
      } else if (IIretval() == 4) {
		if( browse_file( &change_log_file,
			ERget( S_ST0347_change_log_title ), TRUE ) != OK )
		{
/* # line 539 "netprots.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0348_no_change_log ));
        }
/* # line 541 "netprots.sc" */	/* host code */
		}
      } else if (IIretval() == 5) {
		if( !get_help( ERx( "gcb.protocols" ) ) )
/* # line 547 "netprots.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"No help available.");
        }
      } else if (IIretval() == 6) {
/* # line 553 "netprots.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
        }
      } else if (IIretval() == 7) {
/* # line 559 "netprots.sc" */	/* host code */
	if (protocols_rows == 0 )
	{
/* # line 561 "netprots.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(S_ST041B_del_vnode_menu),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 562 "netprots.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(S_ST031E_edit_status_menu),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 563 "netprots.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(S_ST041A_add_vnode_menu),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 564 "netprots.sc" */	/* host code */
	}
	else
	{
/* # line 567 "netprots.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(S_ST041B_del_vnode_menu),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 568 "netprots.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(S_ST031E_edit_status_menu),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 569 "netprots.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(S_ST041A_add_vnode_menu),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 570 "netprots.sc" */	/* host code */
	}
/* # line 571 "netprots.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB2;
        }
      } else {
        if (1) goto IIfdE2;
      } /* IIretval */
    } /* IIrunform */
IIfdF2:
    if (IIchkfrm() == 0) goto IIfdB2;
    goto IIfdE2;
IImuI2:
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact(ERget( S_ST031E_edit_status_menu ),(char *)0,2,2,1) == 0) 
    goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(12,(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact(ERget( S_ST041B_del_vnode_menu ),(char *)0,2,2,2) == 0) goto 
    IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(13,(char *)0,2,2,2) == 0) goto IIfdE2;
    if (IInmuact(ERget(S_ST041A_add_vnode_menu ),(char *)0,2,2,3) == 0) goto 
    IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(19,(char *)0,2,2,3) == 0) goto IIfdE2;
    if (IInmuact(ERget( S_ST035F_change_log_menu ),(char *)0,2,2,4) == 0) 
    goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(18,(char *)0,2,2,4) == 0) goto IIfdE2;
    if (IInmuact(ERget( FE_Help ),(char *)0,2,2,5) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE2;
    if (IInmuact(ERget( FE_End ),(char *)0,2,2,6) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE2;
    if (IITBcaClmAct((char *)"protocols",(char *)"all",1,7) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 574 "netprots.sc" */	/* host code */
	MEfree( def_name );
	return( changed );
}
/*
**  add_bridge_vnode  
**  output:
**
**    protocol -- the protocol name.  
**    port  -- the listen address. Not edited.  
**    vnode -- the virtual node name.  Not edited.
**
**  Returns:
**
**    TRUE -- Add ok 
**    FALSE -- Add error/cancel
** History:
**	10-Aug-2001 (wansh01) 
**	   created 
*/
bool  add_bridge_vnode(char *net_id, char *addvnode, char *addport, char *addprotocol) 
{ 
    bool  rtn; 
    /* Make sure the sizes of these variables match form definitions */
  char fldname[64];
  char vnode[BIG_ENOUGH];
  char port[BIG_ENOUGH];
  char protocol[BIG_ENOUGH];
  bool changed;
    char msg[200];
    char                *pm_val;
    char                pmsym[128];
    if (IIUFgtfGetForm(IIUFlcfLocateForm(), bridge_addvnode) != OK)
        PCexit(FAIL);
    /* ...and display it.  Top of display loop. */
/* # line 619 "netprots.sc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(bridge_addvnode,(char *)"f") == 0) goto IIfdE3;
    goto IImuI3;
IIfdI3:;
    if (IIfsetio((char *)0) == 0) goto IIfdE3;
/* # line 624 "netprots.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(5,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(4,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(6,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(2,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(3,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 627 "netprots.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(41,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(39,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(40,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(36,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(37,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 631 "netprots.sc" */	/* set_frs */
    {
      if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
        IIstfsio(67,ERget(F_ST0001_list_choices),0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
IIfdB3:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 639 "netprots.sc" */	/* inquire_frs */
        {
          if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,1,&changed,20,bridge_addvnode,0);
          } /* IIiqset */
        }
/* # line 640 "netprots.sc" */	/* host code */
	if (!changed)
        { 
	    addvnode=addport=addprotocol=EOS;
	    rtn = FALSE;
/* # line 644 "netprots.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE3;
        }
/* # line 645 "netprots.sc" */	/* host code */
	}
/* # line 647 "netprots.sc" */	/* getform */
        {
          if (IIfsetio(bridge_addvnode) != 0) {
            IIgetfldio((short *)0,1,32,0,protocol,(char *)"protocol");
            IIgetfldio((short *)0,1,32,BIG_ENOUGH-1,port,(char *)"port");
            IIgetfldio((short *)0,1,32,0,vnode,(char *)"vnode");
          } /* IIfsetio */
        }
/* # line 654 "netprots.sc" */	/* host code */
	if (*protocol == EOS) 
	{
            IIUGerr(S_ST031F_edit_protocol_prompt,0,0);
/* # line 657 "netprots.sc" */	/* resume */
        {
          IIresfld((char *)"protocol");
          if (1) goto IIfdB3;
        }
/* # line 658 "netprots.sc" */	/* host code */
        } 
	else
	{
         if (!pm_initialized)
         {
	     PMinit();
	     if (PMload(NULL, (PM_ERR_FUNC *)NULL) == OK)
		 pm_initialized = TRUE;
         }
	 STprintf( pmsym, ERx( "%s.%s.gcb.%s.%s.status" ), 
		  SystemCfgPrefix, host, net_id, protocol );
         if ( PMget( pmsym, &pm_val ) != OK ) 
         {
            IIUGerr(S_ST031F_edit_protocol_prompt,0,0);
/* # line 673 "netprots.sc" */	/* resume */
        {
          IIresfld((char *)"protocol");
          if (1) goto IIfdB3;
        }
/* # line 674 "netprots.sc" */	/* host code */
	 }
        }
	if (*port == EOS)  
	{ 
	    IIUGerr(E_ST000E_listen_req,0,0);
/* # line 680 "netprots.sc" */	/* resume */
        {
          IIresfld((char *)"port");
          if (1) goto IIfdB3;
        }
/* # line 681 "netprots.sc" */	/* host code */
	} 
	if (*vnode == EOS)  
	{
            IIUGerr(S_ST0530_vnode_prompt,0,0);
/* # line 686 "netprots.sc" */	/* resume */
        {
          IIresfld((char *)"vnode");
          if (1) goto IIfdB3;
        }
/* # line 687 "netprots.sc" */	/* host code */
	} 
        STcopy(vnode, addvnode);
        STcopy(port, addport);
        STcopy(protocol,addprotocol);
        rtn = TRUE; 
/* # line 694 "netprots.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE3;
        }
      } else if (IIretval() == 2) {
/* # line 699 "netprots.sc" */	/* host code */
        rtn = FALSE;
/* # line 700 "netprots.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE3;
        }
      } else if (IIretval() == 3) {
        char *prot;
/* # line 711 "netprots.sc" */	/* host code */
	if (NULL != (prot = protocol_listpick( net_id )))
	{
/* # line 713 "netprots.sc" */	/* putform */
        {
          if (IIfsetio(bridge_addvnode) != 0) {
            IIputfldio((char *)"protocol",(short *)0,1,32,0,prot);
          } /* IIfsetio */
        }
/* # line 714 "netprots.sc" */	/* set_frs */
        {
          if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(20,bridge_addvnode,0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 715 "netprots.sc" */	/* set_frs */
        {
          if (IIiqset(2,0,bridge_addvnode,(char *)0, (char *)0) != 0) {
            IIstfsio(20,(char *)"protocol",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 716 "netprots.sc" */	/* redisplay */
        {
          IIredisp();
        }
/* # line 717 "netprots.sc" */	/* host code */
	}
      } else if (IIretval() == 4) {
	if( !get_help( ERx( "gcb.crtvnodes" ) ) )
/* # line 724 "netprots.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"No help available.");
        }
      } else if (IIretval() == 5) {
/* # line 730 "netprots.sc" */	/* inquire_frs */
        {
          if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,32,63,fldname,22,(char *)0,0);
          } /* IIiqset */
        }
/* # line 731 "netprots.sc" */	/* host code */
	if (STcompare(fldname, ERx("protocol")) == 0) 
	{
/* # line 733 "netprots.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(F_ST0001_list_choices),0,(short *)0,1,30,sizeof(
            1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 734 "netprots.sc" */	/* host code */
	}
	else
	{
/* # line 737 "netprots.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(F_ST0001_list_choices),0,(short *)0,1,30,sizeof(
            0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 738 "netprots.sc" */	/* host code */
	}
/* # line 739 "netprots.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB3;
        }
      } else {
        if (1) goto IIfdE3;
      } /* IIretval */
    } /* IIrunform */
IIfdF3:
    if (IIchkfrm() == 0) goto IIfdB3;
    if (IIfsetio((char *)0) == 0) goto IIfdE3;
    goto IIfdE3;
IImuI3:
    if (IIinitmu() == 0) goto IIfdE3;
    if (IInmuact(ERget(FE_Save),(char *)0,2,2,1) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE3;
    if (IInmuact(ERget(FE_Cancel),(char *)0,2,2,2) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,0,2) == 0) goto IIfdE3;
    if (IInmuact(ERget(F_ST0001_list_choices),(char *)0,2,2,3) == 0) goto 
    IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(13,(char *)0,2,0,3) == 0) goto IIfdE3;
    if (IInmuact(ERget(FE_Help),(char *)0,2,2,4) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE3;
    if (IIFRafActFld((char *)"all",1,5) == 0) goto IIfdE3;
    if (IIendmu() == 0) goto IIfdE3;
    goto IIfdI3;
IIfdE3:;
    IIendfrm();
  }
/* # line 744 "netprots.sc" */	/* host code */
    return rtn;
}
/*
**  protocol_listpick() -- Gets a ListChoices for possible protocols
**
**  Returns a character string that contains the chosen item out of a
**  a list of possible protocols.
**  use PMmscan to find protocols that are already defined in the
**  config.dat 
**  <prefix>.<host>.gcb.<net_id>.<protocols>.status
**
**  Inputs:
**
**    net_id  -- net_id so it can scan the config.dat  
**
**  Returns:
**
**    A Pointer to a character string representing a protocal.
**
*/
static char *
protocol_listpick(char * net_id)
{
    i4  lpx;
    static char lpstr[256];
    char *sp, *rtn;
    char *protocol;
    char *regexp;
    STATUS status;
    PM_SCAN_REC state;
    char *name, *value;
    char expbuf[ BIG_ENOUGH ];
    *lpstr = EOS;
	/* construct expbuf for gcb port id scan */
	STprintf( expbuf, ERx( "%s.%s.gcb.%s.%%.status" ), 
		  SystemCfgPrefix, host, net_id );
	regexp = PMmExpToRegExp( config_data, expbuf );
	for( status = PMmScan( config_data, regexp, &state, NULL,
		&name, &value ); status == OK; status =
		PMmScan( config_data, NULL, &state, NULL, &name,
		&value ) )
	{
		/* extract protocol name */
		protocol = PMmGetElem( config_data, 4, name ); 
                STcat(lpstr, protocol);
                STcat(lpstr, ERx("\n"));
        }
     if (*lpstr == EOS)
       return NULL;
    /* Display the ListChoices for selection */
    lpx = IIFDlpListPick( ERget(S_ST0041_protocol_prompt), lpstr, 
			  0, -1, -1, NULL, NULL, NULL, NULL );
    /* If nothing was picked just return an empty string */
    if (lpx < 0)
	return NULL;
    /* Find the selection in the delimitted character string */
    for (rtn = lpstr; lpx; lpx--)
    {
	rtn = STindex(rtn, ERx("\n"), 0);
	if (rtn == NULL)
	    return NULL;
	CMnext(rtn);
    }
    /* return the results of the string search */
    if (NULL != (sp = STindex(rtn, ERx("\n"), 0)))
	*sp = EOS;
    return rtn;
}
