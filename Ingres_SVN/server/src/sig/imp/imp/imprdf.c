# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name:	imprdf.sc
**
** This file contains all of the routines needed to support the "RDF" 
** Subcomponent of the IMA based IMP lookalike tools.
**
## History:
##
##	07-Jul-95	(johna)
##		Created.
##
##	10-Apr-02	(mdf)
##		Expand domain to handle looking at a different RDF.
##
##      24-Apr-02       (mdf240402)
##              Commit after calling imp_complete_vnode
##
##      02-Jul-2002 (fanra01)
##          Sir 108159
##          Added to distribution in the sig directory.
##    08-Oct-2007 (hanje04)
##	SIR 114907
##	Remove declaration of timebuf as it's not referenced and causes
##	errors on Mac OSX
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "imp.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "impcommon.h"
  struct rdf_ {
    char server[65];
    long state_num;
    char state_string[61];
    long cache_size;
    long max_tables;
    long max_qtrees;
    long max_ldb_descs;
    long max_defaults;
  } rdf;
  extern int ima_rdf_cache_info;
  extern int inFormsMode;
  extern int imaSession;
  extern int iidbdbSession;
  extern int userSession;
  extern char *currentVnode;
/*
** Name:	imprdfsum.sc
**
** Display the RDF cache statistics summary
**
*/
  extern int timeVal;
int
processRdfSummary(server)
  GcnEntry *server;
{
  char server_name[SERVER_NAME_LENGTH];
/* # line 81 "imprdf.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"imp_complete_vnode");
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 84 "imprdf.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 88 "imprdf.sc" */	/* set_sql */
  {
    IILQssSetSqlio(46,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
  }
/* # line 89 "imprdf.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select dbmsinfo('IMA_VNODE')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,SERVER_NAME_LENGTH-1,server_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 90 "imprdf.sc" */	/* host code */
	strcat(server_name,"::/@");
	strcat(server_name,server->serverAddress);
/* # line 93 "imprdf.sc" */	/* display */
  {
    if (IIdispfrm((char *)"ima_rdf_cache_info",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
/* # line 96 "imprdf.sc" */	/* putform */
    {
      if (IIfsetio((char *)"ima_rdf_cache_info") != 0) {
        IIputfldio((char *)"server",(short *)0,1,32,0,server_name);
      } /* IIfsetio */
    }
/* # line 97 "imprdf.sc" */	/* host code */
		getRdfSummary(server_name);
		displayRdfSummary();
/* # line 99 "imprdf.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 105 "imprdf.sc" */	/* host code */
		getRdfSummary(server_name);
		displayRdfSummary();
      } else if (IIretval() == 2) {
		clear_rdf();
		getRdfSummary(server_name);
		displayRdfSummary();
      } else if (IIretval() == 3) {
		getRdfSummary(server_name);
		displayRdfSummary();
      } else if (IIretval() == 4) {
/* # line 124 "imprdf.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 125 "imprdf.sc" */	/* set_sql */
        {
          IILQssSetSqlio(46,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        }
/* # line 126 "imprdf.sc" */	/* breakdisplay */
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
    if (IIFRtoact(0,1) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact((char *)"Flush",(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"Refresh",(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 129 "imprdf.sc" */	/* host code */
	return(TRUE);
}
getRdfSummary(server_name)
  char *server_name;
{
	int i;
	int blks;
  char msgBuf[100];
/* # line 143 "imprdf.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"imprdf1",7545,3708);
      IIputdomio((short *)0,1,32,0,server_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"imprdf1",7545,3708);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select server, state_string, cache_size, max_tables, max_qtrees, max_\
ldb_descs, max_defaults from ima_rdf_cache_info where server=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,server_name);
        IIexDefine(0,(char *)"imprdf1",7545,3708);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,64,rdf.server);
      IIgetdomio((short *)0,1,32,60,rdf.state_string);
      IIgetdomio((short *)0,1,30,4,&rdf.cache_size);
      IIgetdomio((short *)0,1,30,4,&rdf.max_tables);
      IIgetdomio((short *)0,1,30,4,&rdf.max_qtrees);
      IIgetdomio((short *)0,1,30,4,&rdf.max_ldb_descs);
      IIgetdomio((short *)0,1,30,4,&rdf.max_defaults);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 164 "imprdf.sc" */	/* host code */
	/*sqlerr_check();*/
    if (sqlerr_check() == NO_ROWS) {      
        fatal("getRdfSummary: no rows");  
    }                                     
}
clear_rdf()
{
/* # line 173 "imprdf.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set trace point rd001");
    IIsyncup((char *)0,0);
  }
/* # line 174 "imprdf.sc" */	/* host code */
}
displayRdfSummary()
{
/* # line 178 "imprdf.sc" */	/* putform */
  {
    if (IIfsetio((char *)"ima_rdf_cache_info") != 0) {
      IIputfldio((char *)"server",(short *)0,1,32,0,rdf.server);
      IIputfldio((char *)"state_string",(short *)0,1,32,0,rdf.state_string);
      IIputfldio((char *)"cache_size",(short *)0,1,30,4,&rdf.cache_size);
      IIputfldio((char *)"max_tables",(short *)0,1,30,4,&rdf.max_tables);
      IIputfldio((char *)"max_qtrees",(short *)0,1,30,4,&rdf.max_qtrees);
      IIputfldio((char *)"max_ldb_descs",(short *)0,1,30,4,&rdf.max_ldb_descs);
      IIputfldio((char *)"max_defaults",(short *)0,1,30,4,&rdf.max_defaults);
    } /* IIfsetio */
  }
/* # line 186 "imprdf.sc" */	/* host code */
}
