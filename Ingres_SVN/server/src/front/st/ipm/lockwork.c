# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
/*
** lock_work.qc
**
** This file contains the utility routines for getting lock information.
** When new locktypes are added to INGRES, they must be added to these
** routines.
**
** This file defines
**    trans_lock_type() - given the locktype shorthand, returns
**	full lock type name
**    trns_lnm_to_lktyp() - given the full lock type name, returns the
**	integer representation
**    displktype() - displays a popup which lists the locktypes
**
** History
**	-/-/-		tomt	written
**	3/8/89 		tomt	created lockname popup help displktype()
**	10/1/89		tomt	integrated 6.3 changes
**	09/21/90	jpk	change "Forget" to "Cancel"
**      5/26/93 (vijay)
**              include pc.h
**	26-Aug-1993 (fredv)
**		Included <st.h>.
**	2-Feb-1994 (fredv)
**		Include <cs.h> so that CS_SID will be defined.
**      12/8/94       liibi01  Cross integration from 6.4. removed some
**                             unintentional trigraphs
**      21-Apr-1995 (nick) Bug #67888
**              Added lock type LK_CKP_TXN
**	9/25/95 (nick)
**		Remove another trigraph.
**	11/1/95 (nick)
**		Add support for LK_AUDIT and LK_ROW. #72320
**	22-nov-1996 (dilma04)
**		Row-level locking project:
**		Add support for LK_PH_PAGE and LK_VAL_LOCK.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	05-Mar-2002 (jenjo02)
**	    Added LK_SEQUENCE
*/
/* includes and globals */
#include <compat.h>
#include <st.h>
#include <dbms.h>
#include <fe.h>
#include <pc.h>
#include <cs.h>
#include <lk.h>
#include "dba.h"
/*
** Forward and External References
*/
i4  trans_lock_type();
i4  trns_lnm_to_lktyp();
PTR displktype();
  static char Fdisplktype[]= "fdisplktype";
/*
** routine trns_lnm_to_lktyp()
**
** This routine will return the lock type for a given name
**
** Input: lock_name - the full name for a locktype
**
** Returns: the lock type, else 0 if a bad lock name was passed.
**
**    22-nov-1996 (dilma04)
**            Row-level locking project:
**            Add support for LK_PH_PAGE and LK_VAL_LOCK.
*/
i4
trns_lnm_to_lktyp(name)
PTR	name;
{
    /*
    **  Determine lock type
    */
    CVlower(name);	/* lower case */
    if (!STcompare(name, LN_DATABASE))
    {
	return(LK_DATABASE);
    }
    else if (!STcompare(name, LN_TABLE))
    {
	return(LK_TABLE);
    }
    else if (!STcompare(name, LN_PAGE))
    {
	return(LK_PAGE);
    }
    else if (!STcompare(name, LN_EXTEND_FILE))
    {
	return(LK_EXTEND_FILE);
    }
    else if (!STcompare(name, LN_BM_PAGE))
    {
	return(LK_BM_PAGE);
    }
    else if (!STcompare(name, LN_PH_PAGE))
    {
        return(LK_PH_PAGE);
    }
    else if (!STcompare(name, LN_CREATE_TABLE))
    {
	return(LK_CREATE_TABLE);
    }
    else if (!STcompare(name, LN_OWNER_ID))
    {
	return(LK_OWNER_ID);
    }
    else if (!STcompare(name, LN_CONFIG))
    {
	return(LK_CONFIG);
    }
    else if (!STcompare(name, LN_DB_TEMP_ID))
    {
	return(LK_DB_TEMP_ID);
    }
    else if (!STcompare(name, LN_SV_DATABASE))
    {
	return(LK_SV_DATABASE);
    }
    else if (!STcompare(name, LN_SV_TABLE))
    {
	return(LK_SV_TABLE);
    }
    else if (!STcompare(name, LN_SS_EVENT))
    {
	return(LK_SS_EVENT);
    }
    else if (!STcompare(name, LN_TBL_CONTROL))
    {
	return(LK_TBL_CONTROL);
    }
    else if (!STcompare(name, LN_JOURNAL)) 
    {
	return(LK_JOURNAL);
    }
    else if (!STcompare(name, LN_OPEN_DB))
    {
	return(LK_OPEN_DB);
    }
    else if (!STcompare(name, LN_CONTROL))
    {
	return(LK_CONTROL);
    }
    else if (!STcompare(name, LN_CKP_CLUSTER))
    {
	return(LK_CKP_CLUSTER);
    }
    else if (!STcompare(name, LN_CKP_DB))
    {
	return(LK_CKP_DB);
    }
    else if (!STcompare(name, LN_BM_LOCK))
    {
	return(LK_BM_LOCK);
    }
    else if (!STcompare(name, LN_BM_DATABASE))
    {
	return(LK_BM_DATABASE);
    }
    else if (!STcompare(name, LN_BM_TABLE))
    {
	return(LK_BM_TABLE);
    }
    else if (!STcompare(name, LN_EVENT))
    {
        return(LK_EVCONNECT);
    }
    else if (!STcompare(name, LN_CKP_TXN))
    {
	return(LK_CKP_TXN);
    }
    else if (!STcompare(name, LN_AUDIT))
    {
	return(LK_AUDIT);
    }
    else if (!STcompare(name, LN_ROW))
    {
	return(LK_ROW);
    }
    else if (!STcompare(name, LN_VALUE))
    {
        return(LK_VAL_LOCK);
    }
    else if (!STcompare(name, LN_SEQUENCE))
    {
        return(LK_SEQUENCE);
    }
    return(0);
}
/*
** routine trans_lock_type()
**
** This routine will return the full name for a given locktype.
**
** Input: lock_type - the shorthand representation for a locktype
**
** Output: name - the full name for a given locktype
**
** Returns: FAIL or OK
**
** Side Effects: buffer to write name into must be long enough
*/
i4
trans_lock_type(lock_type, name)
i4  lock_type;
PTR name;
{
    /*
    **  Determine lock type name
    */
    switch (lock_type) {
    case LK_DATABASE:
	STcopy(LN_DATABASE, name);
	break;
    case LK_TABLE:
	STcopy(LN_TABLE, name);
	break;
    case LK_PAGE:
	STcopy(LN_PAGE, name);
	break;
    case LK_EXTEND_FILE:
	STcopy(LN_EXTEND_FILE, name);
	break;
    case LK_BM_PAGE:
	STcopy(LN_BM_PAGE, name);
	break;
    case LK_PH_PAGE:
        STcopy(LN_PH_PAGE, name);
        break;
    case LK_CREATE_TABLE:
	STcopy(LN_CREATE_TABLE, name);
	break;
    case LK_OWNER_ID:
	STcopy(LN_OWNER_ID, name);
	break;
    case LK_CONFIG:
	STcopy(LN_CONFIG, name);
	break;
    case LK_DB_TEMP_ID:
	STcopy(LN_DB_TEMP_ID, name);
	break;
    case LK_SV_DATABASE:
	STcopy(LN_SV_DATABASE, name);
	break;
    case LK_SV_TABLE:
	STcopy(LN_SV_TABLE, name);
	break;
    case LK_SS_EVENT:
	STcopy(LN_SS_EVENT, name);
	break;
    case LK_TBL_CONTROL:
	STcopy(LN_TBL_CONTROL, name);
	break;
    case LK_JOURNAL:
	STcopy(LN_JOURNAL, name);
	break;
    case LK_OPEN_DB:
	STcopy(LN_OPEN_DB, name);
	break;
    case LK_CONTROL:
	STcopy(LN_CONTROL, name);
	break;
    case LK_CKP_CLUSTER:
	STcopy(LN_CKP_CLUSTER, name);
	break;
    case LK_CKP_DB:
	STcopy(LN_CKP_DB, name);
	break;
    case LK_BM_LOCK:
	STcopy(LN_BM_LOCK, name);
	break;
    case LK_BM_DATABASE:
	STcopy(LN_BM_DATABASE, name);
	break;
    case LK_BM_TABLE:
	STcopy(LN_BM_TABLE, name);
	break;
    case LK_EVCONNECT:
        STcopy(LN_EVENT, name);
        break;
    case LK_CKP_TXN:
	STcopy(LN_CKP_TXN, name);
	break;
    case LK_AUDIT:
	STcopy(LN_AUDIT, name);
	break;
    case LK_ROW:
	STcopy(LN_ROW, name);
	break;
    case LK_VAL_LOCK:
        STcopy(LN_VALUE, name);
        break;
    case LK_SEQUENCE:
        STcopy(LN_SEQUENCE, name);
        break;
    default:
        STprintf(name, "?? =%d", lock_type); /* return what they sent us */
        return(FAIL);
    }
    return(OK);
}
/*
**  displktype() - this routine displays
**	a form which contains a list of lock names.
**
**  History
**	3/8/89  tomt  Created
**      4/24/95 nick  Added LK_CKP_TXN
**      11/1/95 nick  Added LK_AUDIT and LK_ROW
**      22-nov-1996   dilma04 Added LK_PH_PAGE for row level loking project.
**
*/
PTR displktype()
{
  static char lock_name[14];
  char *lock_ptr;
/* # line 366 "lockwork.qsc" */	/* display */
  {
    if (IIdispfrm(Fdisplktype,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 369 "lockwork.qsc" */	/* inittable */
      {
        if (IItbinit(Fdisplktype,(char *)"locklist",(char *)"r") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 371 "lockwork.qsc" */	/* host code */
	lock_ptr = LN_DATABASE;
/* # line 372 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 373 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_TABLE;
/* # line 374 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 375 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_PAGE;
/* # line 376 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 377 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_EXTEND_FILE;
/* # line 378 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 379 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_BM_PAGE;
/* # line 380 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 381 "lockwork.qsc" */	/* host code */
        lock_ptr =  LN_PH_PAGE;
/* # line 382 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 383 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_CREATE_TABLE;
/* # line 384 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 385 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_OWNER_ID;
/* # line 386 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 387 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_CONFIG;
/* # line 388 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 389 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_DB_TEMP_ID;
/* # line 390 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 391 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_SV_DATABASE;
/* # line 392 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 393 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_SV_TABLE;
/* # line 394 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 395 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_SS_EVENT;
/* # line 396 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 397 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_TBL_CONTROL;
/* # line 398 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 399 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_JOURNAL;
/* # line 400 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 401 "lockwork.qsc" */	/* host code */
	lock_ptr =  LN_OPEN_DB;
/* # line 402 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 403 "lockwork.qsc" */	/* host code */
	lock_ptr = LN_CKP_DB;
/* # line 404 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 405 "lockwork.qsc" */	/* host code */
	lock_ptr = LN_CKP_CLUSTER;
/* # line 406 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 407 "lockwork.qsc" */	/* host code */
	lock_ptr = LN_BM_LOCK;
/* # line 408 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 409 "lockwork.qsc" */	/* host code */
	lock_ptr = LN_BM_DATABASE;
/* # line 410 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 411 "lockwork.qsc" */	/* host code */
	lock_ptr = LN_BM_TABLE;
/* # line 412 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 413 "lockwork.qsc" */	/* host code */
	lock_ptr = LN_CONTROL;
/* # line 414 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 415 "lockwork.qsc" */	/* host code */
	lock_ptr = LN_EVENT;
/* # line 416 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 417 "lockwork.qsc" */	/* host code */
	lock_ptr = LN_CKP_TXN;
/* # line 418 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 419 "lockwork.qsc" */	/* host code */
	lock_ptr = LN_AUDIT;
/* # line 420 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 421 "lockwork.qsc" */	/* host code */
	lock_ptr = LN_ROW;
/* # line 422 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 423 "lockwork.qsc" */	/* host code */
        lock_ptr = LN_VALUE;
/* # line 424 "lockwork.qsc" */	/* loadtable */
      {
        if (IItbact(Fdisplktype,(char *)"locklist",1) != 0) {
          IItcoputio((char *)"lock_name",(short *)0,1,32,0,lock_ptr);
          IITBceColEnd();
        } /* IItbact */
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 430 "lockwork.qsc" */	/* host code */
	my_help("ipmdsplt.hlp", "Lock Type Name");
        }
      } else if (IIretval() == 2) {
        {
/* # line 434 "lockwork.qsc" */	/* getrow */
          {
            if (IItbsetio(2,Fdisplktype,(char *)"locklist",-2) != 0) {
              IItcogetio((short *)0,1,32,13,lock_name,(char *)"lock_na\
me");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 435 "lockwork.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 439 "lockwork.qsc" */	/* host code */
	lock_name[0] = EOS;	/* return null */	
/* # line 440 "lockwork.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
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
    if (IInmuact((char *)"Help",(char *)0,2,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact((char *)"Select",(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact((char *)"Cancel",(char *)0,2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 443 "lockwork.qsc" */	/* host code */
    return(&lock_name[0]);
}
