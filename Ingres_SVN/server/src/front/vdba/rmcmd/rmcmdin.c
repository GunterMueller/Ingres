# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/********************************************************************
**
**  Copyright (C) 1995, 2005 Ingres Corporation
**
**
**    Project : Ingres Visual DBA
**
**    Source : rmcmdin.sc
**    manages remote commands input
**
**
** History:
**
**	23-Sep-95 (johna)
**		Added MING hints and moved the 'main' to the start of the 
**		start of the line so that mkming would recognise it.
**
**      04-Jan-96 (spepa01)
**          	Add CL headers so that STxcompare()/STlength() can be used.
**	25-Jan-96 (boama01)
**		Added VMS-specific and CL includes; first cut at CL-izing code;
**		see "*CL*" line markings.  Also improved error reporting.
**  25-Mar-1998 (noifr01)
**      -Added usage of dbevents to speed up rmcmd  
**    05-oct-1998 (kinte01)
**	  For VMS only, if a user has the SERVER CONTROL he can setup the
**	  RMCMD objects & start/stop the RMCMD server. 
**	  The above changes meant that all SQL queries referencing 
**	  ingres.<whatever> had to be rewritten in dynamic sql to be
**	  the user setting up RMCMD. For UNIX & NT this will always be
**	  ingres. For VMS this may or may not be ingres but will be the
**	  Ingres System Admin User.
**  02-Jan-1999 (noifr01)
**    register also the rmcmdcmdend dbevent, in order to be notified
**    immediatly of the completion of the remote command execution.
**    this avoids the rmcmdin process to persist up to 60 seconds 
**    after the command has terminated, and even more if a new remote command
**    has been requested within the interval.
**  02-Feb-99 (noifr01)
**     specify the ingres server class when connecting, in order to support 
**     the case where the default server class is not ingres
**  29-nov-1999 (somsa01) 
**	For the TNG group, connect to iidbdb as the 'ingres' user. This
**	change is only built into Black Box Ingres.
**  11-sep-2000 (somsa01)
**	TNG_EDITION has been superseded by the use of embed_user.
**  11-oct-2000 (mcgem01)
**      The Ingres II SDK can be installed and run as any user, so it needs
**      to mimic the Embedded Ingres version.
**  03-Jan-2001 (noifr01)
**   (SIR #103625) now have rmcmd work against imadb rather than iidbdb
**   also removed unused variables that resulted in a build warning
**  22-Oct-2003 (schph01)
**   (SIR #111153) now have rmcmd run under any user id
**  23-feb-2005 (stial01)
**     Explicitly set isolation level after connect
**	20-sep-2005 (abbjo03)
**	    Initialize ires for a successful return.
*/
/*
PROGRAM =       rmcmdin
NEEDLIBS =      RUNSYSLIB RUNTIMELIB FDLIB FTLIB FEDSLIB \
                UILIB LIBQSYSLIB LIBQLIB UGLIB FMTLIB AFELIB \
                LIBQGCALIB CUFLIB GCFLIB ADFLIB SQLCALIB \
                COMPATLIB 
*/
/* 
	Note that MALLOCLIB should be added when this code has been CL'ized
*/
#include <stdio.h>
#include <ctype.h>
#ifdef VMS
#include <unixio.h>
#include <signal.h>
#endif
#ifdef WIN32
#include <windows.h>
#endif
/** Make sure SQL areas are included before compat.h to prevent GLOBALDEF
 ** conversion of their 'extern' definitions on VMS:
 **/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
/* *CL* -- New includes for CL routines -- */
#include <compat.h>
#include <cv.h>
#include <er.h>
#include <gl.h>
#include <lo.h>
#include <pm.h>
/* *CL* in future, use: #include <si.h> */
#include <st.h>
#ifdef VMS
#include <gc.h>
#include <iicommon.h>
#include <mu.h>
#include <pm.h>
#include <gca.h>
#endif
#include "dba.h"
#include "rmcmd.h"
/* TO BE FINISHED check ingres user and other issues */
#ifdef VMS
int
#else
void 
#endif
main(int argc, char* argv [])
{
    /* TO BE FINISHED: handle all errors; */
  char buf[RMCMDLINELEN];
  char buf1[100];
  char buf2[100];
  int ordno;
  int hdl;
  int istatus;
  int InputLine;
  int ires = 0;
  char stmt[2048];
    setvbuf(stdin, buf1,_IONBF,0);	/* *CL* do not use if converted */
    setvbuf(stdout,buf2,_IONBF,0);	/* *CL* do not use if converted */
   if (argc!=2)
#ifdef VMS
      return (200 + argc);
#else
      return;
#endif
/* *CL* converted:   hdl = atoi (argv [1]); */
   CVan( argv[1], &hdl );
/* # line 147 "rmcmdin.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqUser((char *)"$ingres");
    IIsqConnect(0,(char *)"imadb/ingres",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 149 "rmcmdin.sc" */	/* host code */
   if (sqlca.sqlcode !=0)
#ifdef VMS
     return (102);
#else
     return;
#endif
/* # line 156 "rmcmdin.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"set session isolation level serializable");
    IIsyncup((char *)0,0);
  }
/* # line 158 "rmcmdin.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select user_id, session_id from remotecmd where handle=");
    IIputdomio((short *)0,1,30,4,&hdl);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,99,buf1);
      IIgetdomio((short *)0,1,32,99,buf2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 162 "rmcmdin.sc" */	/* host code */
   if (sqlca.sqlcode < 0) 
#ifdef VMS
      {
      ires = 104;
      goto end;
      }
#else
      goto end;
#endif
/* # line 171 "rmcmdin.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 173 "rmcmdin.sc" */	/* host code */
   STprintf(stmt,ERx("register dbevent rmcmdnewinputline"));
/* # line 174 "rmcmdin.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 176 "rmcmdin.sc" */	/* host code */
   if (sqlca.sqlcode < 0) 
#ifdef VMS
      {
      ires = 104;
      goto end;
      }
#else
      goto end;
#endif
   STprintf(stmt,ERx("register dbevent rmcmdcmdend"));
/* # line 187 "rmcmdin.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(stmt);
    IIsyncup((char *)0,0);
  }
/* # line 189 "rmcmdin.sc" */	/* host code */
   if (sqlca.sqlcode < 0) 
#ifdef VMS
      {
      ires = 104;
      goto end;
      }
#else
      goto end;
#endif
/* # line 199 "rmcmdin.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 202 "rmcmdin.sc" */	/* host code */
   InputLine = -1;
   while (1) {
/* # line 205 "rmcmdin.sc" */	/* get dbevent */
  {
    IIsqInit(&sqlca);
    IILQesEvStat(0,60);
  }
/* # line 206 "rmcmdin.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 207 "rmcmdin.sc" */	/* host code */
	  /*  sleep (5); */
      ordno = -1;
/* # line 210 "rmcmdin.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select orderno, instring from remotecmdin where handle=");
    IIputdomio((short *)0,1,30,4,&hdl);
    IIwritio(0,(short *)0,1,32,0,(char *)"and orderno>");
    IIputdomio((short *)0,1,30,4,&InputLine);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by orderno");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&ordno);
      IIgetdomio((short *)0,1,32,RMCMDLINELEN-1,buf);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 217 "rmcmdin.sc" */	/* host code */
    if (ordno>=0) {
        InputLine++;
        if (InputLine!=ordno){
            /* Old code was: printf("internal error\n");
            Should not write error message thru utility-piped stdout! instead: */
	        ires = 106;
	        goto end;
	    }
        printf("%s\n",buf);       /* output to stdout */
    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
    if (sqlca.sqlcode < 0) 
#ifdef VMS
      {
      ires = 105;
      goto end;
      }
#else
      goto end;
#endif
/* # line 237 "rmcmdin.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 238 "rmcmdin.sc" */	/* host code */
      /* checks if command terminated */
      istatus = -1;
/* # line 240 "rmcmdin.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select status from remotecmd where handle=");
    IIputdomio((short *)0,1,30,4,&hdl);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&istatus);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 244 "rmcmdin.sc" */	/* host code */
   if (sqlca.sqlcode < 0 || sqlca.sqlcode == 100) 
#ifdef VMS
      {
      ires = 107;
      goto end;
      }
#else
      goto end;
#endif
/* # line 253 "rmcmdin.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 254 "rmcmdin.sc" */	/* host code */
      if (istatus==RMCMDSTATUS_FINISHED)
        break;
   }
   /* termination */
end:
/* # line 259 "rmcmdin.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 260 "rmcmdin.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
  }
/* # line 261 "rmcmdin.sc" */	/* host code */
#ifdef VMS
   close( 1 );
   return (ires);
#endif
}
