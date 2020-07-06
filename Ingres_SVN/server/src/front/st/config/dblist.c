# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
**
**  Name: dblist.sc -- contains function which controls the DBMS server
**	database access list form of the INGRES Configuration Utility.
**
**  History:
**	14-dec-92 (tyler)
**		Created.
**	21-jun-93 (tyler)
**		Switched to new interface for multiple context PM
**		functions.
**	23-jul-93 (tyler)
**		Changed multiple context PM interfaces again. 
**	19-oct-93 (tyler)
**		Change type fo global variable "host" to (char *).
**	01-dec-93 (tyler)
**		Fixed bad calls to IIUGerr().
**	22-feb-94 (tyler)
**		Fixed BUG 59434: assign Help menu item to PF2.
**	30-nov-94 (nanpr01)
**		Fixed BUG 60404: Change parameter name for db_list to
**		database_list.
**	30-nov-94 (nanpr01)
**		Fixed BUG 65806: Initialized db_count to BIG_ENOUGH
** 		so that STgetwords work correctly.
**	30-nov-94 (nanpr01)
**		Fixed BUG 60371: Bound frskeys with the different CBF
** 		actions. If users want it they can do so by defining
**		frskey11 - frskey19 map.
**	27-nov-95 (nick)
**		VMS no longer requires a different format for database_list
**		as it isn't passed on the command line but obtained via PM.
**		Fixes #72814.
**	10-oct-1996 (canor01)
**		Replace hard-coded 'ii' prefixes with SystemCfgPrefix.
**	19-dec-1996 (reijo01)
**		Initialize ii_system_name and ii_installation_name in 
**		form.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# include	<compat.h>
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
# include	<uigdata.h>
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
/*
** global variables
*/
  GLOBALREF char *host;
  GLOBALREF char *ii_system;
  GLOBALREF char *ii_installation;
  GLOBALREF char *ii_system_name;
  GLOBALREF char *ii_installation_name;
GLOBALREF LOCATION	config_file;
GLOBALREF PM_CONTEXT	*config_data;
/*
** local forms
*/
  static char *dbms_dblist = ERx( "dbms_dblist" );
bool
dbms_dblist_form( char *dbms_id, char *db_list_in, char **db_list_out )
{
  char *def_name;
  char in_buf[ BIG_ENOUGH ];
	LOCATION *IIUFlcfLocateForm();
	STATUS IIUFgtfGetForm();
	bool changed = FALSE;
	if( IIUFgtfGetForm( IIUFlcfLocateForm(), dbms_dblist ) != OK )
	{
		IIUGerr( S_ST031A_form_not_found, UG_ERR_FATAL, 1,
			dbms_dblist );
	}
	if( STcompare( dbms_id, ERx( "*" ) ) == 0 )
		def_name = ERget( S_ST030F_default_settings );
	else
		def_name = dbms_id;
/* # line 124 "dblist.sc" */	/* inittable */
  {
    if (IItbinit((char *)"dbms_dblist",(char *)"databases",(char *)"r") != 0) 
    {
      IItfill();
    } /* IItbinit */
  }
/* # line 126 "dblist.sc" */	/* display */
  {
    if (IIdispfrm((char *)"dbms_dblist",(char *)"f") == 0) goto IIfdE1;
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
/* # line 138 "dblist.sc" */	/* host code */
	{
    char *db_names[ BIG_ENOUGH ];
		char *db_list_copy, *p;
		i4 db_count = BIG_ENOUGH, i;
		/* make copy of db_list for parsing */
		db_list_copy  = STalloc( db_list_in );
# ifdef VMS
		/* 
		** on VMS, convert non-whitespace to whitespace.
		** nick> this is only here now to convert any
		** 'old' parenthesised entries to the current
		** format ; it can be removed at some stage.
		*/
		for (p = db_list_copy; *p != EOS; p++)
		{
		    if (*p == '(' || *p == ',' || *p == ')')
		    {
		    	*p = ' ';
			changed = TRUE;
		    }
		}
# endif /* VMS */
		/* parse whitespace delimited words from db_list_copy */
		STgetwords( db_list_copy, &db_count, db_names );
		/* load tablefield */
		for( i = 0; i < db_count; i++ )
		{
/* # line 172 "dblist.sc" */	/* loadtable */
    {
      if (IItbact((char *)"dbms_dblist",(char *)"databases",1) != 0) {
        IItcoputio((char *)"name",(short *)0,1,32,0,db_names[ i ]);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 175 "dblist.sc" */	/* host code */
		}
		MEfree( db_list_copy );
	}
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
		/* prompt for new database name */
/* # line 183 "dblist.sc" */	/* prompt */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IIprmptio(0,ERget( S_ST031C_add_db_prompt ),(short *)0,1,32,
          BIG_ENOUGH-1,in_buf);
        }
/* # line 188 "dblist.sc" */	/* insertrow */
        {
          if (IItbsetio(5,(char *)"dbms_dblist",(char *)"databases",0) != 0) 
          {
            if (IItinsert() != 0) {
              IItcoputio((char *)"name",(short *)0,1,32,0,in_buf);
              IITBceColEnd();
            } /* IItinsert */
          } /* IItbsetio */
        }
/* # line 191 "dblist.sc" */	/* host code */
		changed = TRUE;
      } else if (IIretval() == 2) {
        i4 rows;
		/* don't try to deleterow if tablefield empty */
/* # line 201 "dblist.sc" */	/* inquire_frs */
        {
          if (IIiqset(3,0,(char *)"dbms_dblist",(char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&rows,32,(char *)"databases",0);
          } /* IIiqset */
        }
/* # line 204 "dblist.sc" */	/* host code */
		if( rows < 1 ) 
/* # line 205 "dblist.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 207 "dblist.sc" */	/* deleterow */
        {
          if (IItbsetio(4,(char *)"dbms_dblist",(char *)"databases",-2) != 0) 
          {
            if (IItdelrow(0) != 0) {
            } /* IItdelrow */
          } /* IItbsetio */
        }
/* # line 208 "dblist.sc" */	/* host code */
		changed = TRUE;
      } else if (IIretval() == 3) {
		if( !get_help( ERx( "dbms.database_list" ) ) )
/* # line 214 "dblist.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"No help available.");
        }
      } else if (IIretval() == 4) {
        i4 state;
        char db_name[ BIG_ENOUGH ];
        char db_list_buf[ BIG_ENOUGH ];
/* # line 225 "dblist.sc" */	/* host code */
		char resource[ BIG_ENOUGH ];
		bool init = TRUE;
		if( changed )
		{
			STcopy( ERx( "" ), db_list_buf );
			/* re-construct db_list resource */
/* # line 232 "dblist.sc" */	/* unloadtable */
        {
          if (IItbact((char *)"dbms_dblist",(char *)"databases",0) == 0) goto 
          IItbE1;
          while (IItunload() != 0) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,db_name,(char *)"n\
ame");
            IItcogetio((short *)0,1,30,4,&state,(char *)"_state");
            IITBceColEnd();
/* # line 238 "dblist.sc" */	/* host code */
				/* skip deleted rows */
				if( state == 4 )
					continue; /* hope this is OK */
				if( !init )
					STcat( db_list_buf, ERx( " " ) );
				else
					init = FALSE;
				STcat( db_list_buf, db_name );
          } /* IItunload */
IItbE1:
          IItunend();
        }
			/* set the new db_list resource */
			STprintf( resource, ERx( "%s.%s.dbms.%s.database_list"),
				  SystemCfgPrefix, host, dbms_id );
			PMmDelete( config_data, resource );
			PMmInsert( config_data, resource, db_list_buf );
			/* write config.dat */
			if( PMmWrite( config_data, &config_file ) != OK )
			{
				IIUGerr( S_ST0314_bad_config_write, 0, 0 ); 
				return( FALSE );
			}
			/*
			** duplicate and return the modified db_list -
			** this will cause a minor memory leak.
			*/
			*db_list_out = STalloc( db_list_buf );
		}
/* # line 269 "dblist.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Add",(char *)0,2,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(11,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact((char *)"Remove",(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(14,(char *)0,2,2,2) == 0) goto IIfdE1;
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
/* # line 272 "dblist.sc" */	/* host code */
	return( changed );
}
