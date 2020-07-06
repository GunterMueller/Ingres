# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Monitor internal memory pools for the specified server
**
**
## History:
##	22-May-95 (johna)
##		created from a number of standalone programs.
##
##      15-mar-2002     (mdf150302)
##              Break display after IOMON selection.
##      02-Jul-2002 (fanra01)
##          Sir 108159
##          Added to distribution in the sig directory.
##      04-Sep-03       (mdf040903)                     
##              Commit after calling imp_complete_vnode 
##              Commit after calling imp_reset_domain
##      06-Apr-04    (srisu02) 
##              Sir 108159
##              Integrating changes made by flomi02  
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "imp.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "impcommon.h"
  extern int imp_server_diag;
  extern int inFormsMode;
  extern int imaSession;
  extern int iidbdbSession;
  extern int userSession;
  extern char *currentVnode;
int
impInternalMenu(server)
GcnEntry	*server;
{
  int i;
  char buf[60];
/* # line 54 "impinter.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_server_diag",(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
/* # line 58 "impinter.sc" */	/* repeated select */
    {
      IIsexec();
      while (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexExec(1,(char *)"impinter1",7544,7420);
        IIretinit((char *)0,0);
        if (IInexec() == 0) {
          IIsqInit(&sqlca);
          IIsqMods(1);
          IIexDefine(1,(char *)"impinter1",7544,7420);
          IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('ima_vnode')");
          IIexDefine(0,(char *)"impinter1",7544,7420);
          if (IIerrtest() == 0) {
            IIsexec();
          } /* IIerrtest */
        } /* IInexec */
      } /* IInexec */
      if (IInextget() != 0) {
        IIgetdomio((short *)0,1,32,59,buf);
      } /* IInextget */
      IIsqFlush((char *)0,0);
    }
/* # line 59 "impinter.sc" */	/* host code */
		sqlerr_check();
		if (strcmp(buf,currentVnode) != 0) {
/* # line 61 "impinter.sc" */	/* execute procedure */
    {
      IIsqInit(&sqlca);
      IILQpriProcInit(2,(char *)"imp_reset_domain");
      while (IILQprsProcStatus(0) != 0) {
      } /* IILQprsProcStatus */
    }
/* # line 62 "impinter.sc" */	/* execute procedure */
    {
      IIsqInit(&sqlca);
      IILQpriProcInit(2,(char *)"imp_set_domain");
      IILQprvProcValio((char *)"entry",0,(short *)0,1,32,0,currentVnode);
      while (IILQprsProcStatus(0) != 0) {
      } /* IILQprsProcStatus */
    }
/* # line 64 "impinter.sc" */	/* host code */
		} else {
/* # line 65 "impinter.sc" */	/* execute procedure */
    {
      IIsqInit(&sqlca);
      IILQpriProcInit(2,(char *)"imp_reset_domain");
      while (IILQprsProcStatus(0) != 0) {
      } /* IILQprsProcStatus */
    }
/* # line 66 "impinter.sc" */	/* host code */
		}
/* # line 67 "impinter.sc" */	/* commit */
    {
      IIsqInit(&sqlca);
      IIxact(3);
    }
/* # line 68 "impinter.sc" */	/* host code */
/*
##mdf040903
*/
		strcpy(buf,"GCA");
/* # line 73 "impinter.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"gca_field",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
/* # line 74 "impinter.sc" */	/* host code */
		strcpy(buf,"SCF");
/* # line 75 "impinter.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"scf_field",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
/* # line 76 "impinter.sc" */	/* host code */
		strcpy(buf,"PSF");
/* # line 77 "impinter.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"psf_field",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
/* # line 78 "impinter.sc" */	/* host code */
		strcpy(buf,"OPF");
/* # line 79 "impinter.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"opf_field",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
/* # line 80 "impinter.sc" */	/* host code */
		strcpy(buf,"QEF");
/* # line 81 "impinter.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"qef_field",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
/* # line 82 "impinter.sc" */	/* host code */
		strcpy(buf,"RDF");
/* # line 83 "impinter.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"rdf_field",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
/* # line 84 "impinter.sc" */	/* host code */
		strcpy(buf,"QSF");
/* # line 85 "impinter.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"qsf_field",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
/* # line 86 "impinter.sc" */	/* host code */
		strcpy(buf,"ADF");
/* # line 87 "impinter.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"adf_field",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
/* # line 88 "impinter.sc" */	/* host code */
		strcpy(buf,"SXF");
/* # line 89 "impinter.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"sxf_field",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
/* # line 90 "impinter.sc" */	/* host code */
		strcpy(buf,"GWF");
/* # line 91 "impinter.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"gwf_field",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
/* # line 92 "impinter.sc" */	/* host code */
		strcpy(buf,"DMF");
/* # line 93 "impinter.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"dmf_field",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
/* # line 94 "impinter.sc" */	/* host code */
		strcpy(buf,"DI (io)");
/* # line 95 "impinter.sc" */	/* putform */
    {
      if (IIfsetio((char *)0) != 0) {
        IIputfldio((char *)"di_field",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 101 "impinter.sc" */	/* inquire_frs */
        {
          if (IIiqset(2,0,(char *)"imp_server_diag",(char *)0, (char *)0) != 0
          ) {
            IIiqfsio((short *)0,1,32,59,buf,19,(char *)0,0);
          } /* IIiqset */
        }
/* # line 102 "impinter.sc" */	/* host code */
		if (strcmp(buf,"scf_field") == 0) {
			displayServerSessions(server);
		} else if (strcmp(buf,"qsf_field") == 0) {
			processQsfSummary(server);
		} else if (strcmp(buf,"rdf_field") == 0) {
/* debugging stuff
		strcpy(buf,server);
 exec frs message :buf with style = popup;
*/
			processRdfSummary(server);
		} else if (strcmp(buf,"di_field") == 0) {
/* # line 113 "impinter.sc" */	/* display */
        {
          if (IInestmu() == 0) goto IIfdE2;
          goto IImuI2;
IIfdI2:;
IIfdB2:
          while (IIrunnest() != 0) {
            if (IIretval() == 1) {
/* # line 117 "impinter.sc" */	/* host code */
					processIoMonitor(server);
/* # line 118 "impinter.sc" */	/* breakdisplay */
              {
                if (1) goto IIfdE2;
              }
/* # line 119 "impinter.sc" */	/* host code */
/*
##mdf150302
*/
            } else if (IIretval() == 2) {
					processSlaveInfo(server);
/* # line 127 "impinter.sc" */	/* breakdisplay */
              {
                if (1) goto IIfdE2;
              }
/* # line 128 "impinter.sc" */	/* host code */
/*
##mdf150302
*/
            } else if (IIretval() == 3) {
/* # line 135 "impinter.sc" */	/* breakdisplay */
              {
                if (1) goto IIfdE2;
              }
            } else {
              if (1) goto IIfdE2;
            } /* IIretval */
          } /* IIrunnest */
IIfdF2:
          if (IIchkfrm() == 0) goto IIfdB2;
          goto IIfdE2;
IImuI2:
          if (IIinitmu() == 0) goto IIfdE2;
          if (IInmuact((char *)"I/O by session",(char *)0,2,2,1) == 0) goto 
          IIfdE2;
          if (IInmuact((char *)"Slave Info",(char *)0,2,2,2) == 0) goto IIfdE2;
          if (IInmuact((char *)"End",(char *)0,2,2,3) == 0) goto IIfdE2;
          IIFRInternal(0);
          if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE2;
          if (IIendmu() == 0) goto IIfdE2;
          goto IIfdI2;
IIfdE2:;
          IIendnest();
        }
/* # line 138 "impinter.sc" */	/* host code */
		} else if (strcmp(buf,"dmf_field") == 0) {
			processDmfSummary(server);
		} else {
/* # line 141 "impinter.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"Sorry - no module written (yet)");
        }
/* # line 143 "impinter.sc" */	/* host code */
		}
      } else if (IIretval() == 2) {
/* # line 149 "impinter.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"Select",(char *)0,2,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 151 "impinter.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 152 "impinter.sc" */	/* host code */
/*
##mdf040903
*/
/* # line 155 "impinter.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"imp_complete_vnode");
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 156 "impinter.sc" */	/* host code */
	return(TRUE);
}
