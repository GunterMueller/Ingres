# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/********************************************************************
**
**  Copyright (C) 1995,2003 Ingres Corporation
**
**    Project : CA/OpenIngres Visual DBA
**
**    Source : rmcmdgen.sc
**    Creates tables/views/procedures for remote commands management
**
**
** History:
**
**	23-Sep-95 (johna)
**		Added MING hints and moved the 'main' to the start of the 
**		start of the line so that mkming would recognise it.
**  17-Oct-95 (Emmanuel Blattes)
**    Tables columns are all not null with default
**      21-Nov-95 (hanch04)
**              Added dest to utility
**
**	04-Jan-96 (spepa01)
**	    	Add CL headers so that STxcompare()/STlength() can be used.
**	25-Jan-96 (boama01)
**		Added VMS-specific and CL includes; first cut at CL-izing code;
**		see "*CL*" line markings.  Also improved error reporting.
**  03-Apr-97 (rodjo04)
**      Added space to create procedure ingres.launchremotecmd
**      in the value clause as the back end will get confused if 
**      II_DECIMAL is set to ','.
**  25-Mar-98 (noifr01)  
**      -Added creation of dbevents for speed up of rmcmd,
**       modified the generated procedures to include raise dbevents
**      -simplified error management by usage of exec sql whenever sqlerror... 
**    05-oct-1998 (kinte01)
**	  For VMS only, if a user has the SERVER CONTROL he can setup the
**	  RMCMD objects & start/stop the RMCMD server. 
**	  The above changes meant that all SQL queries referencing 
**	  ingres.<whatever> had to be rewritten in dynamic sql to be
**	  the user setting up RMCMD. For UNIX & NT this will always be
**	  ingres. For VMS this may or may not be ingres but will be the
**	  Ingres System Admin User.
**  02-Feb-99 (noifr01)
**     specify the ingres server class when connecting, in order to support 
**     the case where the default server class is not ingres
**  04-May-2000 (noifr01)
**     (bug 101430) added a column in the remotecmdout table, and in
**     remotecmdoutview, indicating whether the corresponding line, 
**     returned to the VDBA client, is to be terminated by a carriage return
**  05-May 2000 (noifr01)
**     (SIR 101439) create the remotecmdout table with nojournaling
**     (also fixed timestamp error in the comment for the previous change)
**  11-sep-2000 (somsa01)
**     For embedded Ingres, connect to iidbdb as the 'ingres' user.
**  11-oct-2000 (mcgem01)
**      The Ingres II SDK can be installed and run as any user, so it needs
**      to mimic the Embedded Ingres version.
**  03-Jan-2001 (noifr01)
**   (SIR #103625) now have rmcmd work against imadb rather than iidbdb
**  26-Apr-2001 (noifr01)
**   (RMCMD counterpart of SIR 103299) increased max lenght of command lines
**   to be executed, and renamed the corresponding column in the remotecmd 
**   table in order to ensure rmcmd under 2.6 doesn't work on tables that
**   wouldn't have been updated (otherwise the commands to be executed could
**   get truncated )
**  27-Apr-2001 (horda03)
**     Allow +u parameter. Required on VMS as this utility may be
**     called during an Install of Ingres by a user which isn't the
**     Ingres administrator. (Bug 100039)
**  25-aug-2001 (somsa01)
**	Corrected previous cross integration.
**  31-jul-2003 (somsa01)
**	Increased size of session_id to 32 to allow for 64-bit session id's.
**  08-oct-2003 (somsa01)
**	Increased size of sessionid to 32 to allow for 64-bit session id's.
**  22-Oct-2003 (schph01)
**  (SIR #111153) now have rmcmd run under any user id
**  05-Jan-2004 (schph01)
**  (corrective fix for SIR #111153) replaced LPTSTR with char * for building
**   on any platform.
**  12-Oct-2004 (noifr01)
**     (bug 113216) cleaned up query in launchremotecmd procedure
**  23-feb-2005 (stial01)
**     Explicitly set isolation level after connect
*********************************************************************/
/*
PROGRAM =       rmcmdgen
NEEDLIBS =      RUNSYSLIB RUNTIMELIB FDLIB FTLIB FEDSLIB \
		UILIB LIBQSYSLIB LIBQLIB UGLIB FMTLIB AFELIB \
		LIBQGCALIB CUFLIB GCFLIB ADFLIB SQLCALIB \
		COMPATLIB
DEST =          utility
*/
/*
	Note that MALLOCLIB should be added when this code has been CLized
*/
/* #include <time.h> (unnecessary) */
/* #include <stdio.h> *CL* repl. w. CL functions */
#include <ctype.h>
/* #include <string.h> *CL* repl. w. CL functions */
/** Make sure SQL areas are included before compat.h to prevent GLOBALDEF
 ** conversion of their 'extern' definitions on VMS:
 **/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
/* *CL* -- New includes for CL routines -- */
#include <compat.h>
#include <er.h>
#include <gl.h>
#include <lo.h>
#include <pm.h>
#include <si.h>
#include <st.h>
#include <id.h>
#include <pc.h>
#include "dba.h"
#include "rmcmd.h"
#ifdef VMS
#include <gc.h>
#include <iicommon.h>
#include <mu.h>
#include <gca.h>
#endif /*VMS*/
#ifndef VMS
#include <gc.h>
#include <iicommon.h>
#include <mu.h>
#include <gca.h>
static STATUS chk_priv( char *user_name, char *priv_name )
{
        char    pmsym[128], *value, *valueptr ;
        char    *strbuf = 0;
        int     priv_len;
        /* 
        ** privileges entries are of the form
        **   ii.<host>.privileges.<user>:   SERVER_CONTROL, NET_ADMIN ...
        */
        STpolycat(2, "$.$.privileges.user.", user_name, pmsym);
        /* check to see if entry for given user */
        /* Assumes PMinit() and PMload() have already been called */
        if( PMget(pmsym, &value) != OK )
            return E_GC003F_PM_NOPERM;
        valueptr = value ;
        priv_len = STlength(priv_name) ;
        /*
        ** STindex the PM value string and compare each individual string
        ** with priv_name
        */
        while ( *valueptr != EOS && (strbuf=STindex(valueptr, "," , 0)))
        {
            if (!STscompare(valueptr, priv_len, priv_name, priv_len))
                return OK ;
            /* skip blank characters after the ','*/
            valueptr = STskipblank(strbuf+1, 10); 
        }
        /* we're now at the last or only (or NULL) word in the string */
        if ( *valueptr != EOS  && 
              !STscompare(valueptr, priv_len, priv_name, priv_len))
                return OK ;
        return E_GC003F_PM_NOPERM;
}
#endif
#ifdef VMS
int main (int argc, char * argv[])
#else
main()
#endif
{
  int ires;
  char **TabTemp;
  char *user = 0;
  int iNum,iMAX ;
  char szName[MAX_USER_NAME_LEN];
  char * TabGranteRmcmd[]={ "grant select,insert,update,delete on remotecmdinview to %s with grant option",
                            "grant select,insert,update,delete on remotecmdoutview to %s with grant option",
                            "grant select,insert,update,delete on remotecmdview to %s with grant option",
                            "grant execute on procedure launchremotecmd to %s with grant option",
                            "grant execute on procedure sendrmcmdinput to %s with grant option",
                            "grant register,raise on dbevent rmcmdcmdend to %s with grant option",
                            "grant register,raise on dbevent rmcmdnewcmd to %s with grant option",
                            "grant register,raise on dbevent rmcmdnewinputline to %s with grant option",
                            "grant register,raise on dbevent rmcmdnewoutputline to %s with grant option",
                            "grant register,raise on dbevent rmcmdstp to %s with grant option"
                           };
  char stmt[2048];
/*********** Retrieve the user executing this process ******************/
  user = szName;
  IDname(&user);
/*********** Check user privileges *************************************/
  PMinit();
  if( PMload( NULL, (PM_ERR_FUNC *)NULL ) != OK)
  {
    SIeqinit();
    SIprintf( ERx("Read error in config.dat.\n") );
    PCexit(FAIL);
  }
  PMsetDefault( 0, ERx( "ii" ) );
  PMsetDefault( 1, PMhost() );
  if (chk_priv( user, GCA_PRIV_SERVER_CONTROL ) != OK)
  {
     SIeqinit();
     SIprintf( ERx("User '%s' has insufficient privileges for generating the RMCMD objects.\n"),user );
     PCexit(FAIL);
  }
/* # line 234 "rmcmdgen.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqUser((char *)"$ingres");
    IIsqConnect(0,(char *)"imadb/ingres",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 235 "rmcmdgen.sc" */	/* host code */
  if (sqlca.sqlcode < 0)  {
    SIeqinit();
    SIprintf( ERx("Could not connect to system catalog:\n") );
    SIprintf( ERx("%*s\n"), sqlca.sqlerrm.sqlerrml,
                            sqlca.sqlerrm.sqlerrmc );
    PCexit(FAIL);
  }
/* # line 243 "rmcmdgen.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session isolation level serializable");
    IIsyncup((char *)0,0);
  }
/* # line 245 "rmcmdgen.sc" */	/* host code */
  ires =RES_ERR;
/* # line 248 "rmcmdgen.sc" */	/* host code */
  /*************** create dbevents for synchronization speedup **/
  STprintf(stmt,ERx("create dbevent rmcmdnewcmd"));
/* # line 251 "rmcmdgen.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 252 "rmcmdgen.sc" */	/* host code */
  STprintf(stmt,ERx("create dbevent rmcmdnewoutputline"));
/* # line 253 "rmcmdgen.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 254 "rmcmdgen.sc" */	/* host code */
  STprintf(stmt,ERx("create dbevent rmcmdnewinputline"));
/* # line 255 "rmcmdgen.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 256 "rmcmdgen.sc" */	/* host code */
  STprintf(stmt,ERx("create dbevent rmcmdcmdend"));
/* # line 257 "rmcmdgen.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 258 "rmcmdgen.sc" */	/* host code */
  STprintf(stmt,ERx("create dbevent rmcmdstp"));
/* # line 259 "rmcmdgen.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 261 "rmcmdgen.sc" */	/* host code */
  /************ create remotecmd table *************************/
/* # line 263 "rmcmdgen.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table remotecmd(handle int unique not null with default, statu\
s int not null with default, user_id varchar(32)not null with default,\
 session_id varchar(32)not null with default, cmd1 varchar(800)not nul\
l with default)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 271 "rmcmdgen.sc" */	/* host code */
  /************ create remotecmdview view **********************/
  STpolycat (7, "create view remotecmdview",
                " as select handle, status, cmd1",
                " from ",
                "$ingres",
                ".remotecmd ",
                " where user_id = dbmsinfo('session_user') ",
                " and session_id= dbmsinfo('session_id')", stmt );
/* # line 281 "rmcmdgen.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 283 "rmcmdgen.sc" */	/* host code */
  /************ create remotecmdout table *************************/
/* # line 285 "rmcmdgen.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table remotecmdout(handle int not null with default, orderno i\
nt not null with default, user_id varchar(32)not null with default, se\
ssion_id varchar(32)not null with default, outstring varchar(256)not n\
ull with default, nocr char(8)not null with ");
    IIwritio(0,(short *)0,1,32,0,(char *)"default)with nojournaling");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 294 "rmcmdgen.sc" */	/* host code */
  /************ create remotecmdoutview view **********************/
  STpolycat (7, "create view remotecmdoutview",
                " as select handle, orderno, outstring, nocr",
                " from ",
                "$ingres",
                ".remotecmdout",
                " where user_id = dbmsinfo('session_user')",
                " and   session_id= dbmsinfo('session_id')", stmt);
/* # line 304 "rmcmdgen.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 306 "rmcmdgen.sc" */	/* host code */
  /************ create remotecmdin table *************************/
/* # line 308 "rmcmdgen.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table remotecmdin(handle int not null with default, orderno in\
t not null with default, user_id varchar(32)not null with default, ses\
sion_id varchar(32)not null with default, instring varchar(256)not nul\
l with default)");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 315 "rmcmdgen.sc" */	/* host code */
  /************ create remotecmdinview view **********************/
  STpolycat(7, "create view remotecmdinview ", 
               " as select handle, orderno, instring",
               " from ",
               "$ingres",
               ".remotecmdin ",
               " where user_id = dbmsinfo('session_user')",
               " and   session_id= dbmsinfo('session_id')", stmt);
/* # line 325 "rmcmdgen.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 327 "rmcmdgen.sc" */	/* host code */
  /************ create launchremotecmd procedure *********/
  STpolycat(29, "create procedure ", 
               "$ingres", 
               ".launchremotecmd (rmcmd varchar(800))",
               " as", 
               " declare ",
               " hdl integer not null;", 
               " begin ",
               " hdl=-1; ",
               " select handle into :hdl ",
               " from remotecmd ",
               " where handle=(select max(handle) from remotecmd);",
               " if iierrornumber<>0 ",
               "   then hdl=-1; rollback; return :hdl; ",
               " endif; ",
               " hdl=hdl+1; ",
               " insert into remotecmd ",
               " values(:hdl, 0 , dbmsinfo('session_user'),dbmsinfo('session_id'),:rmcmd); ",
               " if iierrornumber<>0 ",
               "   then hdl=-1; rollback; return :hdl; ",
               " endif; ",
               " raise dbevent ",
               "$ingres",
               ".rmcmdnewcmd; ",
               " if iierrornumber<>0 ",
               "   then hdl=-1; rollback; return :hdl; ",
               " endif; ",
               " commit; ",
               " return :hdl;",
               " end ", stmt);
/* # line 359 "rmcmdgen.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 361 "rmcmdgen.sc" */	/* host code */
  /************ create sendrmcmdinputprocedure procedure *********/
  STpolycat (44, "create procedure ",
                "$ingres",
                ".sendrmcmdinput (inputhandle int, ",
                " inputstring varchar(256)) ",
                " as ",
                " declare ",
                " hdl integer not null; ",
                " username varchar(32) not null; ",
                " sessionid char(32) not null; ",
                " begin ",
                " hdl=-1; ",
                " username=''; ",
                " sessionid=''; ",
                " select orderno,user_id,session_id ",
                "   into :hdl,:username,:sessionid ",
                "   from remotecmdin ",
                "   where handle = :inputhandle ",
                "   order by orderno desc; ",
                " if iierrornumber<>0 ",
                "   then hdl=-1; rollback; return :hdl; ",
                " endif; ",
                " if hdl>=0 and ",
                "   not(:sessionid=dbmsinfo('session_id') and ",
                "   :username=dbmsinfo('session_user')) ",
                "   then return -1; ",
                " endif; ",
                " hdl=hdl+1; ",
                " insert into remotecmdin ",
                "   values(:inputhandle,:hdl, ",
                "   dbmsinfo('session_user'), ",
                "   dbmsinfo('session_id'), ", 
                "   :inputstring); ",
                " if iierrornumber<>0 ",
                "   then hdl=-1; rollback; return :hdl; ",
                " endif; ",
                " raise dbevent ",
                "$ingres",
                ".rmcmdnewinputline; ",
                " if iierrornumber<>0 ",
                "    then hdl=-1; rollback; return :hdl; ",
                " endif; ",
                " commit; ",
                " return :hdl; ",
                " end ", stmt );
/* # line 408 "rmcmdgen.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 410 "rmcmdgen.sc" */	/* host code */
/*********** Now Grant the user for the rmcmd objects ******************/
  TabTemp = TabGranteRmcmd;
  iMAX = sizeof(TabGranteRmcmd)/sizeof(TabGranteRmcmd[0]);
  for (iNum = 0; iNum < iMAX; iNum++)
  {
      STprintf( stmt,TabGranteRmcmd[iNum],szName);
/* # line 416 "rmcmdgen.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 417 "rmcmdgen.sc" */	/* host code */
  }
  ires =RES_SUCCESS;
end1:
  if (ires==RES_SUCCESS)
/* # line 423 "rmcmdgen.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 424 "rmcmdgen.sc" */	/* host code */
  else {
     SIeqinit();
     SIprintf( ERx("Create rmcmd objects failed:\n") );
     SIprintf( ERx("%*s\n"), sqlca.sqlerrm.sqlerrml,
                             sqlca.sqlerrm.sqlerrmc );
/* # line 429 "rmcmdgen.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 430 "rmcmdgen.sc" */	/* host code */
  }
/* # line 431 "rmcmdgen.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      goto end1;
  }
/* # line 433 "rmcmdgen.sc" */	/* host code */
  PCexit( (ires == RES_SUCCESS) ? OK : FAIL);
}
