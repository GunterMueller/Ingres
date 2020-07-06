# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>		/* 6-x_PC_80x86 */
#include	<si.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<ui.h>
#include	<uigdata.h>
#include	<qg.h>
# include <mqtypes.h>
#include	<oodefine.h>
# include "mqsymbol.h"
#include	"mqglobs.h"
#include	"mqstates.h"
#include	"erqf.h"
FUNC_EXTERN VOID    mq_unrslv();
/**
** Name:	qbfnames.qsc -	QBFNames Catalog Frame for QBF.
**
** Description:
**	This file defines:
**
**	qqbfnames(qdef)
**
** History:
**	7-aug-1987  (danielt) Message changes, including removal of
**		static char message arrays.
**	13-aug-1987 (danielt) FEmsg() calls changed to IIUGerr()
**	07-dec-87 (kenl)
**		Turned module into a .QSC file in order to incorporate
**		the new header file: mqbf.qsh, mqsymbol.qsh, and oosymbol.qsh.
**		Changed QUEL RETRIEVE to SQL SELECT.
**	29-jan-1988 (peter)
**		Performance changes.  Get rid of RANGE and use REPEAT.
**	11-feb-1988 (peter)
**		Add parameter to mq_tbl_load.
**	20-jul-88 (kenl)
**		Bug fix 2962.  Removed ooid from error message E_QF0081.
**	21-jul-88 (sylviap)
**		Added SQL COMMIT After OOidCheck to release lock on system 
**		  catalogs.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**      09-nov-88 (sylviap)
**              Added a NULL parameter when calling OOdetail.
**	25-jul-89 (kenl) - Fixed bug 6402
**	4/16/90 (martym) - Fixed JUPBUG bug #4063.
**	09/12/90 (dkh) - Removed menuitems "Top", "Bottom" and "Find" per
**			 FRC decision.
**	18-jan-93 (daver)
**		Removed paramter to mq_tbl_load, nuked in 1989.
**      23-mar-93 (sylviap)
**              Added a qbf specific help table parameter when calling OOdetail.
**		(40943)
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
nam_reload()
{
	/*
	**	This reloads qbfnames after calling tables
	**	for the overlay linker.
	*/
}
static VOID	_initCat();
static STATUS	_go();
static STATUS	_display();
static MENUPARAM	_Menu[] = {
	{ NULL, 4,	_go},
	{ NULL, 0,	NULL},
};
/*
**	QQBFNAMES displays the catalog of QBFNames.
**
**	History:
**		10-11-85 (gac)	-- written.
**		09-30-86 (sandyd) Saturn changes.  Replaced retrieve of username
**			with STcopy() of global UserName.  Also changed range
**			variables to use full table names.
**		11/18/86 (KY)  -- changed STindex,STrindex for Double Bytes characters.
*/
#define _HelpExpl	F_FE0100_ExplFrameHelp
#define _EndExpl	F_FE0102_ExplEnd
#define _FindExpl	F_FE0104_ExplFind
#define _TopExpl	F_FE0105_ExplTop
#define _BottomExpl	F_FE0106_ExplBottom
#define _MoreInfoExpl	F_FE0107_ExplMoreInfo
bool
qqbfnames ( qdef )
register MQQDEF	*qdef;
  {
    char *enditem;
	i4		retval = TRUE;
	OO_CATREC	qbfn_cr;
	char		msg_buf[100];
    i4 rownum = 1;
	enditem = (mq_forms && !mq_name) ? (char *)NULL : ERget(FE_End);
/* # line 120 "qbfnames.qsc" */	/* display */
    {
      if (IIdispfrm(_iicatalog,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 123 "qbfnames.qsc" */	/* putform */
        {
          if (IIfsetio(_iicatalog) != 0) {
            IIputfldio((char *)"title",(short *)0,1,32,0,
            ERget(F_QF000C_QBF___QBFNames_Catalo));
          } /* IIfsetio */
        }
/* # line 126 "qbfnames.qsc" */	/* host code */
		if (mq_noload)
		{
			OOcatEmpty(OC_QBFNAME, _iicatalog, _cattable);
		}
		else
		{
/* # line 132 "qbfnames.qsc" */	/* message */
        {
          IImessage(ERget(S_QF007E_Retrieving_QBFNames_c));
        }
/* # line 133 "qbfnames.qsc" */	/* host code */
			_initCat(ERx("*"));
		}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 138 "qbfnames.qsc" */	/* host code */
		if ( _Menu[0].mpstring == NULL )
			_Menu[0].mpstring = ERget(FE_Go);
		STcopy(ERget(S_QF007D_QBF___QBFNames_Detail), msg_buf);
		if ( OOdetail(OC_QBFNAME, _iicatalog, _cattable, &qbfn_cr,
				_iidetail,
				msg_buf, (PTR)qdef,
				_display, (STATUS (*)())NULL,
				(STATUS (*)())NULL, (STATUS (*)())NULL,
				(STATUS (*)())NULL,
				_Menu, ERx("qmorinfo.hlp")) != OK )
		{
			_initCat(ERx("*"));
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 154 "qbfnames.qsc" */	/* host code */
		if ( OOcatCheck(OC_QBFNAME, _iicatalog, _cattable, &qbfn_cr)
				== OK )
		{
/* # line 157 "qbfnames.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_iicatalog,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&rownum,29,_cattable,0);
              } /* IIiqset */
            }
/* # line 159 "qbfnames.qsc" */	/* host code */
			mq_lastsaved [0] = EOS;
			_go(qbfn_cr.id, qdef);
			if (mq_lastsaved [0] != EOS)
			{
				/*
				** If we have saved a JoinDef, redraw the 
				** QBFNAMES catalog, because OO has written 
				** over it:
				*/
				_initCat(ERx("*"));
/* # line 169 "qbfnames.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_iicatalog,_cattable,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,rownum) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 171 "qbfnames.qsc" */	/* host code */
			}
		}
          }
        } else if (IIretval() == 3) {
          {
            i4 qcount;
/* # line 178 "qbfnames.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_iicatalog,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&qcount,31,_cattable,0);
              } /* IIiqset */
            }
/* # line 179 "qbfnames.qsc" */	/* host code */
		if ( qcount == 0 )
			IIUGerr( E_QF007F_There_are_no_QBFNames, UG_ERR_ERROR,
					0
			);
		else
			FEtabfnd(_iicatalog, _cattable);
          }
        } else if (IIretval() == 4) {
          {
/* # line 188 "qbfnames.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_iicatalog,_cattable,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 192 "qbfnames.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_iicatalog,_cattable,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 196 "qbfnames.qsc" */	/* host code */
		FEhelp(ERx("qqbfname.hlp"), ERget(S_QF0080_QBFNames_Catalog));
          }
        } else if (IIretval() == 7) {
          {
/* # line 200 "qbfnames.qsc" */	/* host code */
		if ( enditem == NULL )
		{
			retval = FALSE;
		}
/* # line 204 "qbfnames.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 8) {
          {
/* # line 208 "qbfnames.qsc" */	/* host code */
		retval = FALSE;
/* # line 209 "qbfnames.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_MoreInfo),ERget(_MoreInfoExpl),2,2,1) == 0) goto 
      IIfdE1;
      if (IInmuact(ERget(FE_Go),(char *)0,2,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(_FindExpl),2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(_TopExpl),2,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(_BottomExpl),2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,6) == 0) goto IIfdE1;
      if (IInmuact(enditem,ERget(_EndExpl),2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,7) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Quit),(char *)0,2,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,2,2,8) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 213 "qbfnames.qsc" */	/* host code */
	return retval;
  }
static VOID
_initCat ( qual )
char	*qual;
{
	OOcatTableLoad( OC_QBFNAME, _iicatalog, _cattable,
		FALSE, qual, (char *)NULL, (char *)NULL
	);
}
/*
** Name:	_go() -	QBFNames Go function.
**
** Description:
**	The application-specific function implementing Go on 
**	QBFName objects from QBF.
**
** Inputs:
**	OOID	qbfnid;		// object id of QBFName
**	MQQDEF	*qdef;		// parent qdef to set up and
**				   pass to qbf "Go" function.
**
** Returns:
**	STATUS
**
** Side Effects:
**	Runs QBF Query phase.
**
** History:
**	6/87 (peterk) - completed initial version for 6.0
**	29-jan-1988 (peter)
**		Get rid of RANGE and use REPEAT.
**	11-aug-1993 (rdrane)
**		Include the table owner when displaying or otherwise accessing
**		tables associated with QBFNames, and make sure that it is
**		propagated to the OO_CATREC structure of the qdef.  Also,
**		pass owner through IIOOidCheck(), and note that it takes five
**		parameters, not four!  This addresses all/part of b54048,
**		b51773, b51756, and b52570.
**		Eliminate the STtrmwhite() instances - these are all
**		varchar fields!
**	24-nov-1993 (rdrane)
**		Correct parameterization of IIOOidCheck() - always pass a
**		NULL pointer for the owner.  ii_qbfnames appears to store the
**		owner as NULL, so we wind up asking IIOOidCheck() for an
**		object owned by '', which never exists ...  This corrects
**		bug 56654.
**	13-sep-1995 (abowler)
**		Removed usage of ifnull function - its not OpenSQL so fails
**		through gateways (B70931).
*/
static STATUS
_go ( qbfnid, qdef )
  i4 qbfnid;
MQQDEF	*qdef;
{
  char tfrid[FE_MAXNAME+1];
  char ttblname[FE_MAXNAME+1];
  char ttblown[FE_MAXNAME+1];
  i4 qtypeval;
  i4 rowcnt;
  i2 owner_null;
/* # line 360 "qbfnames.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qbfnames1",7170,14323);
      IIputdomio((short *)0,1,30,4,&qbfnid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"qbfnames1",7170,14323);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select frname, relname, relowner, qbftype from ii_qbfnames where obje\
ct_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&qbfnid);
        IIexDefine(0,(char *)"qbfnames1",7170,14323);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,tfrid);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,ttblname);
      IIgetdomio(&owner_null,1,32,FE_MAXNAME+1-1,ttblown);
      IIgetdomio((short *)0,1,30,4,&qtypeval);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 364 "qbfnames.sc" */	/* host code */
	if (owner_null == -1)
	    ttblown[0] = '\0';
/* # line 366 "qbfnames.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&rowcnt,8);
  }
/* # line 367 "qbfnames.sc" */	/* host code */
	if ( rowcnt != 1 )
	{
		IIUGerr( E_QF0081_QBFName___d__not_foun, UG_ERR_ERROR,
			1, (PTR)_ii_qbfnames
		);
	}
	else
	{
		STcopy(ttblname, qdef->mqcr.name);
		STcopy(ttblown, qdef->mqcr.owner);
		if ( ! (onetbl = qtypeval == 0) )
		{ /* qbfname target is a joindef */
			OOID	joindefClass = OC_JOINDEF;
			/* retrieve JoinDef object info */
			qdef->mqcr.id = UNKNOWN;
			if  (IIOOidCheck(&joindefClass,&qdef->mqcr.id,
					 ttblname,NULL,(i4)0) != OK )
			{
				IIUGerr( E_QF0082_JoinDef___s__not_foun,
					UG_ERR_ERROR, 1, (PTR)ttblname
				);
				goto out;
			}
		}
		STcopy(tfrid, mq_frame);
		mq_forms = TRUE;
		mq_going = TRUE;
		mq_tbl_load(qdef, !onetbl, mqQBFNAMES);
		mq_going = FALSE;
	 }
out:
	if (mq_noload)
	{
		OOcatEmpty(OC_QBFNAME, _iicatalog, _cattable);
	}
	return OK;
}
/*
** Name:	_display() -	qbfname-specific display routine
**
** Description:
**	The "application-specific" display function for the Details
**	frame from QBF QBFNames catalog.  Shows form and target
**	associated with QBFName in display-only extra fields on
**	Details form.
**
** Inputs:
**	OOID	qbfnid;		// object id of QBFName
**
** Returns:
**	STATUS
**
** Side Effects:
**	Updates Catalog Details frame display.
**
** History:
**	7/7/87 (peterk) - written
**	29-jan-1988 (peter)
**		Get rid of RANGE and use REPEAT.
**	12-aug-1993 (rdrane)
**		Display the associated table name w/ owner as required.
**		Be careful of the owner, since ii_qbfnames defines that
**		column as NULLable!  This addresses all/part of
**		b54048, b51773, b51756, and b52570.
**	13-sep-1995 (abowler)
**		Removed usage of ifnull function - its not OpenSQL so fails
**		through gateways (B70931).
*/
static STATUS
  _display( qbfnid )
  OOID qbfnid;
  {
  char tfrid[FE_MAXNAME+1];
  char ttblname[(FE_MAXNAME + 1)];
  char ttblown[(FE_MAXTABNAME + 1)];
  char buf[FE_MAXTABNAME];
  i4 qtypeval;
  i4 rowcnt;
  i2 owner_null;
/* # line 451 "qbfnames.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qbfnames2",7171,11255);
      IIputdomio((short *)0,1,30,4,&qbfnid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"qbfnames2",7171,11255);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select frname, relname, relowner, qbftype from ii_qbfnames where obje\
ct_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&qbfnid);
        IIexDefine(0,(char *)"qbfnames2",7171,11255);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,tfrid);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,ttblname);
      IIgetdomio(&owner_null,1,32,FE_MAXNAME+1-1,ttblown);
      IIgetdomio((short *)0,1,30,4,&qtypeval);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 455 "qbfnames.sc" */	/* host code */
	if (owner_null == -1)
		ttblown[0] = '\0';
/* # line 457 "qbfnames.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&rowcnt,8);
  }
/* # line 458 "qbfnames.sc" */	/* host code */
	if ( rowcnt != 1 )
	{
		IIUGerr( E_QF0083_QBFName___d__not_foun, UG_ERR_ERROR,
				1, (PTR) &qbfnid, (PTR) _ii_qbfnames
		);
		return FAIL;
	}
	/*
	** Construct the table name as owner.tablename as required.
	*/
	mq_unrslv(&ttblown[0],&ttblname[0],&buf[0]);
	/* put QBFName-specific info onto current form */
/* # line 407 "qbfnames.qsc" */	/* putform */
    {
      if (IIfsetio(_iidetail) != 0) {
        IIputfldio((char *)"xfield1",(short *)0,1,32,0,
        ERget(F_QF000F_Form_name_));
        IIputfldio((char *)"xfield2",(short *)0,1,32,0,tfrid);
        IIputfldio((char *)"xfield3",(short *)0,1,32,0,
        ERget(qtypeval == 0 ? F_QF000D_Table_name_
					: F_QF000E_JoinDef_name_));
        IIputfldio((char *)"xfield4",(short *)0,1,32,0,buf);
      } /* IIfsetio */
    }
/* # line 414 "qbfnames.qsc" */	/* host code */
	return OK;
  }
