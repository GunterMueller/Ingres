# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name:	impdmf.sc
**
** This file contains all of the routines needed to support the "DMF" 
** Subcomponent of the IMA based IMP lookalike tools.
**
## History:
##
##	08-Jul-95	(johna)
##		Incorporated from earlier DMF tools
##
##	15-mar-2002	(mdf150302)
##		The existing ima_dmf_cache_stats doesn't contain the necessary
##		columns for this routine to work.
##		This routine now looks at a different table imp_dmf_cache_stats.
##
##		Now caters for different page sizes.
*/
/*
##      10-Apr-02       (mdf100402)
##              Expand domain to handle looking at a different DMF.
##              Ideas for further enhancements.
##              -  Shared cache handling.
##
##      24-Apr-02       (mdf240402)
##              Commit after calling imp_complete_vnode
##
##      02-Jul-2002 (fanra01)
##          Sir 108159
##          Added to distribution in the sig directory.
##
##      04-Sep-03       (mdf040903)
##              Commit after selecting data
##              Add extra error handling.
##              Added extra dmf cache information onto form
##      06-Apr-04    (srisu02) 
##              Sir 108159
##              Integrating changes made by flomi02  
##      05-Oct-04    (srisu02) 
##              Removed fatal error message and instead added a popup message 
##              whenever the cache information was unavailable in the DMF when
##              the user selected the 'Internals' option
##      05-Oct-04    (srisu02) 
##              Removed unnecessary comments, corrected spelling mistakes 
##		in the code comments
##
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "imp.h"
#define ERRLEN 100
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "impcommon.h"
void getDmfCache();
# include "imp_dmf.h"
  extern int imp_dmf_cache_stats;
  extern int inFormsMode;
  extern int imaSession;
  extern int iidbdbSession;
  extern int userSession;
  extern char *currentVnode;
/*
** Name:	impdmfsum.sc
**
** Display the DMF cache statistics summary
**
*/
  extern int timeVal;
  char timebuf[20];
int
processDmfSummary(server)
  GcnEntry *server;
{
  char server_name[SERVER_NAME_LENGTH];
  int page_size;
  int multicache;
  int cachechosen;
/* # line 105 "impdmf.sc" */	/* set_sql */
  {
    IILQssSetSqlio(46,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
  }
/* # line 107 "impdmf.sc" */	/* execute procedure */
  {
    IIsqInit(&sqlca);
    IILQpriProcInit(2,(char *)"imp_complete_vnode");
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
  }
/* # line 108 "impdmf.sc" */	/* host code */
/*
## mdf100402 
*/
/* # line 111 "impdmf.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 112 "impdmf.sc" */	/* host code */
/*
## mdf240402 
*/
	strcpy(server_name,currentVnode);    
/*
## mdf040903 
*/
	strcat(server_name,"::/@");
	strcat(server_name,server->serverAddress);
/* # line 122 "impdmf.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_dmf_cache_stats",(char *)"u") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
/* # line 125 "impdmf.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_dmf_cache_stats") != 0) {
        IIputfldio((char *)"server",(short *)0,1,32,0,server_name);
      } /* IIfsetio */
    }
/* # line 126 "impdmf.sc" */	/* host code */
		getDmfCache(server_name, &page_size, &multicache, &cachechosen);
                if (cachechosen == TRUE)
                {
                    /* Only show the SelectCache menuitem when relevant */
                    if (multicache == TRUE) 
                        {
/* # line 132 "impdmf.sc" */	/* set_frs */
    {
      if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
        IIstfsio(67,(char *)"SelectCache",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 133 "impdmf.sc" */	/* host code */
                        }
	            else
                        {
/* # line 136 "impdmf.sc" */	/* set_frs */
    {
      if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
        IIstfsio(67,(char *)"SelectCache",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 137 "impdmf.sc" */	/* host code */
                        }
/* # line 138 "impdmf.sc" */	/* putform */
    {
      if (IIfsetio((char *)"imp_dmf_cache_stats") != 0) {
        IIputfldio((char *)"page_size",(short *)0,1,30,4,&page_size);
      } /* IIfsetio */
    }
/* # line 139 "impdmf.sc" */	/* host code */
		    getDmfSummary(server_name);
		    displayDmfSummary();
/* # line 141 "impdmf.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&timeVal);
      } /* IIiqset */
    }
/* # line 142 "impdmf.sc" */	/* host code */
                }
                else
                {
/* # line 145 "impdmf.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 146 "impdmf.sc" */	/* set_sql */
    {
      IILQssSetSqlio(46,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    }
/* # line 147 "impdmf.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE1;
    }
/* # line 148 "impdmf.sc" */	/* host code */
                }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
		getDmfSummary(server_name);
		displayDmfSummary();
      } else if (IIretval() == 2) {
		clear_dmf();
		getDmfSummary(server_name);
		displayDmfSummary();
      } else if (IIretval() == 3) {
		getDmfSummary(server_name);
		displayDmfSummary();
      } else if (IIretval() == 4) {
		getDmfCache(server_name, &page_size, &multicache, &cachechosen);
                if (cachechosen == TRUE)
                {
/* # line 175 "impdmf.sc" */	/* putform */
        {
          if (IIfsetio((char *)"imp_dmf_cache_stats") != 0) {
            IIputfldio((char *)"page_size",(short *)0,1,30,4,&page_size);
          } /* IIfsetio */
        }
/* # line 176 "impdmf.sc" */	/* host code */
		    getDmfSummary(server_name);
		    displayDmfSummary();
                }
      } else if (IIretval() == 5) {
/* # line 183 "impdmf.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 184 "impdmf.sc" */	/* set_sql */
        {
          IILQssSetSqlio(46,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        }
/* # line 185 "impdmf.sc" */	/* breakdisplay */
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
    if (IInmuact((char *)"SelectCache",(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact((char *)"End",(char *)0,2,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 188 "impdmf.sc" */	/* host code */
	return(TRUE);
}
clear_dmf()
{
/* # line 193 "impdmf.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set trace point dm421");
    IIsyncup((char *)0,0);
  }
/* # line 194 "impdmf.sc" */	/* host code */
}
getDmfSummary(server_name)
  char *server_name;
{
    int i;
    int blks;
  int page_size;
/* # line 206 "impdmf.sc" */	/* getform */
  {
    if (IIfsetio((char *)"imp_dmf_cache_stats") != 0) {
      IIgetfldio((short *)0,1,30,4,&page_size,(char *)"page_size");
    } /* IIfsetio */
  }
/* # line 208 "impdmf.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"impdmf1",7546,20349);
      IIputdomio((short *)0,1,32,0,server_name);
      IIputdomio((short *)0,1,30,4,&page_size);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"impdmf1",7546,20349);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select * from imp_dmf_cache_stats where server=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,server_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and page_size=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&page_size);
        IIexDefine(0,(char *)"impdmf1",7546,20349);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,64,(dmfbuf).server);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).page_size);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).force_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).io_wait_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).group_buffer_read_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).group_buffer_write_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).fix_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).unfix_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).read_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).write_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).hit_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).dirty_unfix_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).pages_still_valid);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).pages_invalid);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).buffer_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).page_buffer_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).flimit);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).mlimit);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).wbstart);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).wbend);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).hash_bucket_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).group_buffer_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).group_buffer_size);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).cache_status);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).free_buffer_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).free_buffer_waiters);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).fixed_buffer_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).modified_buffer_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).free_group_buffer_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).fixed_group_buffer_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).modified_group_buffer_count);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).fc_wait);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).bm_gwait);
      IIgetdomio((short *)0,1,30,4,&(dmfbuf).bm_mwait);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 213 "impdmf.sc" */	/* host code */
    if (sqlerr_check() == NO_ROWS) {
        fatal("getDmfSummary: No information found for this cache");
    }
}
displayDmfSummary()
{
  char bar[100];
  int pct;
  float flt_pct;
  int blks;
  int i;
  int free;
  int dirty;
  int modified;
/* # line 232 "impdmf.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_dmf_cache_stats") != 0) {
      IIputfldio((char *)"server",(short *)0,1,32,0,dmfbuf.server);
      IIputfldio((char *)"page_buffer_count",(short *)0,1,30,4,&
      dmfbuf.page_buffer_count);
      IIputfldio((char *)"group_buffer_count",(short *)0,1,30,4,&
      dmfbuf.group_buffer_count);
      IIputfldio((char *)"group_buffer_size",(short *)0,1,30,4,&
      dmfbuf.group_buffer_size);
      IIputfldio((char *)"read_count",(short *)0,1,30,4,&dmfbuf.read_count);
      IIputfldio((char *)"group_buffer_read_count",(short *)0,1,30,4,&
      dmfbuf.group_buffer_read_count);
      IIputfldio((char *)"write_count",(short *)0,1,30,4,&dmfbuf.write_count);
      IIputfldio((char *)"group_buffer_write_count",(short *)0,1,30,4,&
      dmfbuf.group_buffer_write_count);
      IIputfldio((char *)"free_buffer_count",(short *)0,1,30,4,&
      dmfbuf.free_buffer_count);
      IIputfldio((char *)"cache_status",(short *)0,1,30,4,&dmfbuf.cache_status);
      IIputfldio((char *)"pages_still_valid",(short *)0,1,30,4,&
      dmfbuf.pages_still_valid);
      IIputfldio((char *)"pages_invalid",(short *)0,1,30,4,&
      dmfbuf.pages_invalid);
      IIputfldio((char *)"buffer_count",(short *)0,1,30,4,&dmfbuf.buffer_count);
      IIputfldio((char *)"free_buffer_waiters",(short *)0,1,30,4,&
      dmfbuf.free_buffer_waiters);
      IIputfldio((char *)"free_group_buffer_count",(short *)0,1,30,4,&
      dmfbuf.free_group_buffer_count);
      IIputfldio((char *)"fixed_buffer_count",(short *)0,1,30,4,&
      dmfbuf.fixed_buffer_count);
      IIputfldio((char *)"fixed_group_buffer_count",(short *)0,1,30,4,&
      dmfbuf.fixed_group_buffer_count);
      IIputfldio((char *)"modified_buffer_count",(short *)0,1,30,4,&
      dmfbuf.modified_buffer_count);
      IIputfldio((char *)"modified_group_buffer_count",(short *)0,1,30,4,&
      dmfbuf.modified_group_buffer_count);
      IIputfldio((char *)"mlimit",(short *)0,1,30,4,&dmfbuf.mlimit);
      IIputfldio((char *)"flimit",(short *)0,1,30,4,&dmfbuf.flimit);
      IIputfldio((char *)"wbstart",(short *)0,1,30,4,&dmfbuf.wbstart);
      IIputfldio((char *)"wbend",(short *)0,1,30,4,&dmfbuf.wbend);
      IIputfldio((char *)"fix_count",(short *)0,1,30,4,&dmfbuf.fix_count);
      IIputfldio((char *)"hit_count",(short *)0,1,30,4,&dmfbuf.hit_count);
      IIputfldio((char *)"unfix_count",(short *)0,1,30,4,&dmfbuf.unfix_count);
      IIputfldio((char *)"dirty_unfix_count",(short *)0,1,30,4,&
      dmfbuf.dirty_unfix_count);
      IIputfldio((char *)"io_wait_count",(short *)0,1,30,4,&
      dmfbuf.io_wait_count);
      IIputfldio((char *)"fc_wait",(short *)0,1,30,4,&dmfbuf.fc_wait);
      IIputfldio((char *)"bm_gwait",(short *)0,1,30,4,&dmfbuf.bm_gwait);
      IIputfldio((char *)"bm_mwait",(short *)0,1,30,4,&dmfbuf.bm_mwait);
      IIputfldio((char *)"force_count",(short *)0,1,30,4,&dmfbuf.force_count);
    } /* IIfsetio */
  }
/* # line 272 "impdmf.sc" */	/* host code */
	/* calculate the buffer stuff */
	if (dmfbuf.fix_count == 0) 
	    flt_pct = 0;
        else  
	    flt_pct = (100 * dmfbuf.hit_count) / dmfbuf.fix_count;
	pct = (int) flt_pct;
	blks = (pct * 45) / 100;
	for (i = 0; i < blks; i++) {
		bar[i] = '%';
	}
	bar[i] = '\0';
/* # line 284 "impdmf.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_dmf_cache_stats") != 0) {
      IIputfldio((char *)"hit_ratio_pct",(short *)0,1,31,4,&flt_pct);
      IIputfldio((char *)"hit_ratio",(short *)0,1,32,0,bar);
    } /* IIfsetio */
  }
/* # line 287 "impdmf.sc" */	/* host code */
	/* modified pages */
	pct = (100 * dmfbuf.modified_buffer_count) / dmfbuf.page_buffer_count;
	blks = (pct * 45) / 100;
	for ( i = 0;(i < blks) && (i < 45); i++) {
		bar[i] = 'D';
	}
	/* fixed but not modified */
	pct = (100 * (dmfbuf.page_buffer_count - (dmfbuf.free_buffer_count +
		dmfbuf.modified_buffer_count))) / dmfbuf.page_buffer_count;
	blks = (pct * 45) / 100;
	blks = blks + i;
	for (/* no start */; i < blks; i++) {
		bar[i] = 'u';
	}
	pct = (100 * dmfbuf.free_buffer_count) / dmfbuf.page_buffer_count;
	blks = (pct * 45) / 100;
	blks = blks + i;
	for (/* no start */; i < blks; i++) {
		bar[i] = 'f';
	}
	free = blks;
	bar[i] = '\0';
/* # line 310 "impdmf.sc" */	/* putform */
  {
    if (IIfsetio((char *)"imp_dmf_cache_stats") != 0) {
      IIputfldio((char *)"cache",(short *)0,1,32,0,bar);
    } /* IIfsetio */
  }
/* # line 312 "impdmf.sc" */	/* host code */
}
void getDmfCache(server_name , p_page_size , p_multicache, p_cachechosen)
  char *server_name;
  int *p_page_size;
  int *p_multicache;
  int *p_cachechosen;
{
  int cnt;
  int l_page_size;
  char l_msgbuf[ERRLEN];
/* # line 327 "impdmf.sc" */	/* display */
  {
    if (IIdispfrm((char *)"imp_dmfcache",(char *)"r") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
/* # line 330 "impdmf.sc" */	/* inittable */
    {
      if (IItbinit((char *)"imp_dmfcache",(char *)"cache",(char *)"f") != 0) 
      {
        IItfill();
      } /* IItbinit */
    }
/* # line 331 "impdmf.sc" */	/* select */
    {
      IIsqInit(&sqlca);
      IIwritio(0,(short *)0,1,32,0,(char *)
"select page_size from imp_dmf_cache_stats where server=");
      IIputdomio((short *)0,1,32,0,server_name);
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&l_page_size);
        if (IIerrtest() != 0) goto IIrtB2;
/* # line 336 "impdmf.sc" */	/* insertrow */
        {
          if (IItbsetio(5,(char *)"imp_dmfcache",(char *)"cache",-2) != 0) {
            if (IItinsert() != 0) {
              IItcoputio((char *)"page_size",(short *)0,1,30,4,&l_page_size);
              IITBceColEnd();
            } /* IItinsert */
          } /* IItbsetio */
        }
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE2:;
    }
/* # line 339 "impdmf.sc" */	/* host code */
        sqlerr_check();    
/* 
##mdf040903 
*/
/* # line 343 "impdmf.sc" */	/* inquire_sql */
    {
      IILQisInqSqlio((short *)0,1,30,4,&cnt,8);
    }
/* # line 344 "impdmf.sc" */	/* commit */
    {
      IIsqInit(&sqlca);
      IIxact(3);
    }
/* # line 345 "impdmf.sc" */	/* host code */
/*
##mdf040903
*/
        if (cnt == 0) {
/*** A popup error message is displayed when no cache information is found ***/ 
            sprintf(l_msgbuf,"No information found for this cache");
/* # line 351 "impdmf.sc" */	/* message */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IImessage(l_msgbuf);
    }
/* # line 352 "impdmf.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE2;
    }
/* # line 353 "impdmf.sc" */	/* host code */
                }                                   
        else if (cnt == 1) {                        
             *p_multicache = FALSE;
             *p_page_size = l_page_size;
             *p_cachechosen = TRUE;
/* # line 358 "impdmf.sc" */	/* breakdisplay */
    {
      if (1) goto IIfdE2;
    }
/* # line 359 "impdmf.sc" */	/* host code */
                }                                   
        else {                        
             *p_multicache = TRUE;
/* # line 362 "impdmf.sc" */	/* scroll */
    {
      if (IItbsetio(1,(char *)"imp_dmfcache",(char *)"cache",-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,1) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 363 "impdmf.sc" */	/* host code */
/*
##mdf040903
*/
                }                                   
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 371 "impdmf.sc" */	/* getrow */
        {
          if (IItbsetio(2,(char *)"imp_dmfcache",(char *)"cache",-2) != 0) {
            IItcogetio((short *)0,1,30,4,&l_page_size,(char *)"page_si\
ze");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 372 "impdmf.sc" */	/* host code */
        *p_page_size = l_page_size;
        *p_cachechosen = TRUE;
/* # line 374 "impdmf.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
        }
      } else if (IIretval() == 2) {
/* # line 379 "impdmf.sc" */	/* host code */
        *p_cachechosen = FALSE;
/* # line 380 "impdmf.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
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
    if (IInmuact((char *)"Select",(char *)0,2,2,1) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact((char *)"Cancel",(char *)0,2,2,2) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,2) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 383 "impdmf.sc" */	/* host code */
}
