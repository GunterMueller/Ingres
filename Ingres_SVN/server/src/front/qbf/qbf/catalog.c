# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>		/* 6-x_PC_80x86 */
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ui.h>
#include	"erqf.h"
# include "mqbf.h"
# include "mqsymbol.h"
#include	<oodefine.h>
/**
**	CATALOG.QSC  -  This module contains the routines to
**		       display the MQBF Catalogs and perform
**		       various tasks on stored query definitions.
**
**	Routines:
**	    mqcheck()
**	    mqcats()
**
**	History:
**	Written: 11/10/83 (nml)
**		10/20/86 (KY)  -- Changed CH.h to CM.h.
**		11/18/86 (KY)  -- changed STindex,STrindex for 
**				Double Bytes characters.
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh.  Replaced QUEL
**			transaction statements with SQL statements.  Replaced
**			QUEL DELETE on table ii_joindefs with SQL DELETE.
**		29-jan-1988 (peter)
**			Use repeat delete.
**		11-feb-1988 (peter)
**			Add parameter to mq_tbl_load to avoid refreshing
**			the catalog unless needed.
**		18-feb-1988 (peter)
**			Fix bad syntax for scroll command, discovered in
**			/06 testing.
**		22-apr-88 (kenl)
**			Added COMMIT TRANSACTION.
**		30-aug-1988 (danielt)
**			Changed to use new (QUEL style) transaction routines.
**		09-nov-88 (marian)
**			Modified column names for extended catalogs to allow 
**			them to work with gateways.
**		09-nov-88 (sylviap)
**			Added a NULL parameter when calling OOdetail.
**	09/12/90 (dkh) - Removed menuitems "Top", "Bottom" and "Find"
**			 per FRC decision.  Also put in explanation
**			 for the corresponding frskeys.
**		29-mar-1994 (rdrane)
**			Check for mq_noload (expert mode) when in DESTROY
**			option.  b58515
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
extern	char	Lastname[];
FUNC_EXTERN	STATUS do_go();
FUNC_EXTERN	mq_reinit();
FUNC_EXTERN	STATUS do_nothing();
FUNC_EXTERN	STATUS do_destroy();
FUNC_EXTERN	STATUS mqwritetodb();
GLOBALDEF MENUPARAM	Mq_menup[] = {
	{ "Go", 4,	do_go},
	{ NULL, 0,	NULL},
};
/*	Forward Declaration */
static	VOID	mqmvintbl();
cat_reload()
{
	/*
	**	This routine is the help the overlay loader
	**	reload the catalogs after calling mq_tables
	*/
}
mqnoldcat()
{
	OOcatEmpty(OC_JOINDEF, _iicatalog, _cattable);
}
/*{
** Name:	mqcats
**
** History:
**	Jan. - Jul. 87 (peterk) - re-worked to utilize 6.0 OO
**		and new catalog frame designs.
**	11-feb-1988 (peter)
**		Add parameter to mq_tbl_load, called lastsaved.  This
**		will be filled in by the underlying routine by the
**		name of the last joindef saved, if one was saved.  If
**		none were saved, it will be blank or the NULL string.
**		Only if one were saved do we need to refresh the catalogs --
**		otherwise they will be up to date (and a refresh is expensive).
**		Also, make sure to position the cursor on the last
**		saved joindef name (this is bug 2016).
**	05-dec-89 (kenl)
**		Replaced parameter lastsaved with the global mq_lastsaved.
**		This was required by the new calling sequences needed for
**		support of the -J flag.
*/
bool
mqcats(qdef)
MQQDEF	*qdef;
  {
    char *enditem;
	bool	retval = TRUE;
/* # line 135 "catalog.qsc" */	/* clear */
    {
      IIclrscr();
    }
/* # line 137 "catalog.qsc" */	/* host code */
	enditem = (mq_qflag && !mq_name)? ERx(""): ERget(FE_End);
/* # line 139 "catalog.qsc" */	/* display */
    {
      if (IIdispfrm(_iicatalog,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 142 "catalog.qsc" */	/* putform */
        {
          if (IIfsetio(_iicatalog) != 0) {
            IIputfldio((char *)"title",(short *)0,1,32,0,
            ERget(F_QF001F_QBF__JoinDefs_Catalog));
          } /* IIfsetio */
        }
/* # line 145 "catalog.qsc" */	/* host code */
	    if (mq_noload)
	    {
		mqnoldcat();
	    }
	    else
	    {
/* # line 151 "catalog.qsc" */	/* message */
        {
          IImessage(ERget(S_QF0023_Retrieving_JoinDefs_c));
        }
/* # line 152 "catalog.qsc" */	/* host code */
		mqcatinit(ERx("*"));
	    }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 157 "catalog.qsc" */	/* host code */
	    mqqdefinit(qdef);
	    mq_lastsaved[0] = EOS;
	    IIQFjl_JoindefListpick(qdef, _mqnewjoin);
	    if (mq_noload)
	    {
		mqnoldcat();
	    }
	    else 
	    {
		if (STlength(mq_lastsaved) > 0)
		{	/* Something was saved.  Refresh catalogs */
		mqcatinit(ERx("*"));
		    mqmvintbl(_iicatalog, _cattable, _name, mq_lastsaved); 
	    }
	    }
          }
        } else if (IIretval() == 2) {
          {
/* # line 176 "catalog.qsc" */	/* host code */
	    _VOID_ IIUIbeginXaction();
	    if (OOcatDestroy(OC_JOINDEF, _iicatalog, _cattable, &qdef->mqcr,
		mq_noload) == OK)
	    {
		if (do_destroy(qdef->mqcr.id, qdef) == OK)
		{
		    _VOID_ IIUIendXaction();
		    if (mq_noload)
		    {
			mqnoldcat();
		    }
		    else
		    {
			mqcatinit(ERx("*"));
		    }
		}
		else
		{
		    _VOID_ IIUIabortXaction();
		}
	    }
	    else
	    {
		_VOID_ IIUIabortXaction();
	    }
          }
        } else if (IIretval() == 3) {
          {
/* # line 204 "catalog.qsc" */	/* host code */
	    mqqdefinit(qdef);
	    if (OOcatCheck(OC_JOINDEF, _iicatalog, _cattable, &qdef->mqcr)
		== OK)
	    {
		do_edit(qdef->mqcr.id, qdef);
		if (mq_noload)
		{
		    mqnoldcat();
		}
		else 
		{
		    if (STlength(mq_lastsaved) > 0)
		    {	/* Something was saved.  Refresh catalogs */
		    mqcatinit(ERx("*"));
		        mqmvintbl(_iicatalog, _cattable, _name, mq_lastsaved); 
		}
	    }
	    }
          }
        } else if (IIretval() == 4) {
          {
/* # line 227 "catalog.qsc" */	/* host code */
	    OOcatRename(OC_JOINDEF, _iicatalog, _cattable, &qdef->mqcr,
		mq_noload);
          }
        } else if (IIretval() == 5) {
          {
/* # line 232 "catalog.qsc" */	/* host code */
	    mqqdefinit(qdef);
	    if (OOdetail(OC_JOINDEF, _iicatalog, _cattable,
		&qdef->mqcr, _iidetail, ERget(S_QF0025_QBF___JoinDef_Details),
		qdef, NULL, mq_reinit, do_nothing, do_destroy,
		do_nothing, Mq_menup, NULL))
	    {
		/* if any info changed, re-retrieve catalog */
		if (mq_noload)
		{
		    mqnoldcat();
		}
		else
		{
		    mqcatinit(ERx("*"));
		    mqmvintbl(_iicatalog, _cattable, _name, qdef->mqcr.name); 
		}
	    }
          }
        } else if (IIretval() == 6) {
          {
/* # line 253 "catalog.qsc" */	/* host code */
	    mqqdefinit(qdef);
	    if (OOcatCheck(OC_JOINDEF, _iicatalog, _cattable, &qdef->mqcr)==OK)
	    {
		do_go(qdef->mqcr.id, qdef);
		if (mq_noload)
		{
		    mqnoldcat();
		}
		}
          }
        } else if (IIretval() == 7) {
          {
/* # line 266 "catalog.qsc" */	/* host code */
	    FEtabfnd(_iicatalog, _cattable);
          }
        } else if (IIretval() == 8) {
          {
/* # line 270 "catalog.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_iicatalog,_cattable,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 9) {
          {
/* # line 274 "catalog.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_iicatalog,_cattable,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 10) {
          {
/* # line 278 "catalog.qsc" */	/* host code */
	    FEhelp(ERx("qcatlg.hlp"),ERget(F_QF001F_QBF__JoinDefs_Catalog));
          }
        } else if (IIretval() == 11) {
          {
/* # line 282 "catalog.qsc" */	/* host code */
	    if (enditem[0] != 'E')
	    {
		retval = FALSE;
	    }
/* # line 286 "catalog.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 12) {
          {
/* # line 290 "catalog.qsc" */	/* host code */
	    retval = FALSE;
/* # line 291 "catalog.qsc" */	/* breakdisplay */
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
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      goto IIfdE1;
IImuI1:
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Create),(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Destroy),(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Edit),(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Rename),(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_MoreInfo),(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Go),(char *)0,2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,9) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),(char *)0,2,2,10) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,10) == 0) goto IIfdE1;
      if (IInmuact(enditem,(char *)0,2,2,11) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,11) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Quit),(char *)0,2,2,12) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,0,2,12) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 294 "catalog.qsc" */	/* host code */
	return(retval);
  }
/*{
** Name:	mqcatinit
**
** History:
**	30-sep-1986 (sandyd)
**		Changed range variable to use full table name.
*/
mqcatinit(qual)
char	*qual;
{
	OOcatTableLoad(OC_JOINDEF, _iicatalog, _cattable, FALSE, qual, Lastname,
		NULL);
}
mqnoqdefs()
{
	IIUGerr(E_QF0026_No_JoinDefs_found, 0, 0);
}
/*
**	do_nothing - stub so a null function can be passed
*/
STATUS
do_nothing(id, qdef)
OOID	id;
MQQDEF	*qdef;
{
    return OK;
}
STATUS
do_go(id, qdef)
OOID	id;
MQQDEF	*qdef;
{
	mq_qflag = TRUE;
	mq_going = TRUE;
	/* Lastsaved set to NULL, as no changes will be made */
	mq_tbl_load(qdef, TRUE, mqCATALOGS);
	mq_lastsaved[0] = EOS;
	mq_qflag = FALSE;
	mq_going = FALSE;
	return OK;
}
do_edit(id, qdef)
OOID	id;
MQQDEF	*qdef;
{
	mqLast(qdef->mqcr.name);
	mq_qflag = TRUE;
	mq_lastsaved[0] = EOS;
	mq_tbl_load(qdef, TRUE, mqCATALOGS);
	mq_qflag = FALSE;
}
STATUS
do_destroy(id, qdef)
  OOID id;
MQQDEF	*qdef;
{
	iiuicw1_CatWriteOn();
/* # line 416 "catalog.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"catalog1",7190,7289);
      IIputdomio((short *)0,1,30,4,&id);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"catalog1",7190,7289);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_joindefs where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"catalog1",7190,7289);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 417 "catalog.sc" */	/* host code */
	iiuicw0_CatWriteOff();
	if (FEinqerr())
	{
		return FAIL;
	}
	return OK;
}
/**
** Name:	mqmvintbl	Move cursor to position in table
**
** Description:
**	Move the row cursor to the position in the table field shown 
**	that corresponds to a name.  This searches the table field
**	column for a match, and then scrolls the form to that 
**	row.  If it is not found, it scrolls to the top, without
**	error message.
**
**	The current field must be the table field.  The match will
**	be an exact match check. 
**
** Parameters:
**	form		name of form.
**	tab		name of table field.
**	col		name of column to search.  (must be character type,
**			less than 100 characters in length)
**	match		string to match.  
**
** History:
**	11-feb-1988 (peter)
**		Written.
**	18-feb-1988 (peter)
**		Fix bad syntax for scroll command, discovered in
**		/06 testing.
*/
static
VOID
mqmvintbl(form, tab, col, match)
  char *form;
  char *tab;
  char *col;
char	*match;
  {
    i4 currec, matchrec, state;
    char cval[101];
	matchrec = 1;
/* # line 418 "catalog.qsc" */	/* unloadtable */
    {
      if (IItbact(form,tab,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,100,cval,col);
        IItcogetio((short *)0,1,30,4,&currec,(char *)"_RECORD");
        IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
        IITBceColEnd();
        {
/* # line 421 "catalog.qsc" */	/* host code */
		if ((state != 1) && (state != 2) && (state != 3))
		{	/* Row is no good */
			continue;
		}
		if (STcompare(cval, match) == 0)
		{	/* This is the match */
			matchrec = currec;
/* # line 428 "catalog.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 429 "catalog.qsc" */	/* host code */
		}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 432 "catalog.qsc" */	/* scroll */
    {
      if (IItbsetio(1,form,tab,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,matchrec) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 433 "catalog.qsc" */	/* host code */
	return;
  }
