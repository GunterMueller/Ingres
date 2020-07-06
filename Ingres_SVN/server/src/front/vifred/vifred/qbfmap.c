# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<cv.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	"decls.h"
#include	<ug.h>
#include	<uf.h>
# include <ooclass.h>
# include <oocat.h>
# include <uigdata.h>
#include	<ui.h>
#include	<stdprmpt.h>
#include	"ervf.h"
#include	"vfhlp.h"
/**
** Name:	qbfmap.qsc -	VIFRED QBFNames Module.
**
**	Routines for handling QBFNAMES
**
** History:
**	Modified 10/28/83 (nml) -NMpathIng and LO routines changed to
**				 NMloc call so temp files are created
**				 in ING_TEMP or local directory.
**	01/29/85 (drh)	Modified to add call to allocQbf
**	02/04/85 (drh)	Added call to vfdmpoff - integration of VMS bug fixes.
**	2/6/85 (peterk) - added (from frs path) distinguished error
**		message in updating system catalogs.
**		- removed retrieval of user info (done in initdb()
**		if single-process vifred or qbfnamMain() if a process
**		module.
**	06/27/85 (gac)	Replaced sleeped messages by FEmsg.
**	10/20/86 (KY)  -- Changed CH.h to CM.h.
**	05/11/87 (dkh) - Changed for generic catalogs.
**	05/18/87 (dkh) - Updated for OOid*() changes.
**	05/21/87 (dkh) - Integrated VMS changes.
**	05/21/87 (dkh) - Added display of detail info on a qbfname.
**	05/30/87 (dkh) - Changed transaction processing.
**	07/22/87 (dkh) - Fixed jup bug 399.
**	08/04/87 (dkh) - Changed to use compiled forms in a file.
**	08/14/87 (dkh) - ER changes.
**	09/21/87 (dkh) - Fixed jup bug 1075.
**	10/02/87 (dkh) - Help file changes.
**	10/17/87 (dkh) - Changed to not use single letter range vars.
**	10/17/87 (dkh) - Fixed jup bug 1073.
**	10/25/87 (dkh) - Error message cleanup.
**	11/25/87 (dkh) - Fixed jup bug 1411.
**	03-dec-87 (bruceb)
**		Changed to a qsc file; include .qsh files.
**	09-dec-87 (danielt)
**		SQLized the qualification statement in the call to
**			OOcatTableLoad().
**	12/19/87 (dkh) - Including .qsh instead of .qh headers.
**	21-dec-87 (sylviap)
**		Changed the QUEL statements to SQL.
**	22-feb-88 (sylviap)
**		Changed the transaction code to handle gateways.  Took out other
**		transaction code in vfsvedqf and vfsvqfname, since will be
**		handled by OOdetail and OOsaveSequence.
**	03/03/88 (dkh) - Changes to save routines called by OO.
**	29-mar-88 (sylviap)
**		Added commit work before calls to OOsaveSequence and OOdetail.
**	11-may-88 (sylviap)
**		Calls to OOdetail pass the title of a frame through a
**		  message buffer.
**		Calls IIUGlbo_lowerBEobject for creating a qbfname with a
**		  table.
**		Turned off GO to qbf if DG.
**		Added NULL (set-up) function call to OOsaveSequence parameter
**	  	  list.
**		Added SQL COMMIT after OO and FE calls to release the
**		  locks on the system catalogs.
**		Took out commit work before calls to OOsaveSequence and
**		  OOdetail.
**	17-aug-88 (sylviap)
**		Changed CVlower to IIUlbo_lowerBEobject to accommodate mixed
**		case tables names. (#3133)
**	11/01/88 (dkh) - Performance changes.
**	03-nov-88 (sylviap)
**		Added a new parameter, vfqbfsav.hlp, a customized help screen
**		for QBFNames, to OOsaveSequence and vfqbfdet.hlp to OOdetail.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**	11/30/88 (tom) - if CMS don't allow calling QBF from vifred.
**	08-dec-88 (sylviap)
**		Took out error message if DG, then cannot call QBF from vifred.
**	11-dec-1988 (danielt)
**		removed all commits and changed to use new transaction routines.
**	04/19/89 (dkh) - Put in generic error changes.
**	18-may-89 (bruceb)	Fix for bug 5989.
**		Save formname in more permanent memory on entry to fdqbfmap().
**		Then, other calls on OOcatCheck() won't overwrite the name.
**	28-jun-89 (bruceb)
**		Use vfsave and vfdetail instead of iisave and iidetail.
**	27-nov-89 (bruceb)
**		Find/top/bottom no longer visible menuitems.
**	30-nov-89 (bruceb)
**		Used standard prompt for destroy-confirmation.
**	12/12/89 (dkh) - Preliminary support for owner.table.
**	12/22/89 (dkh) - Completed support for owner.table.
**	01/24/90 (dkh) - Moved include of er.h ahead of uf.h due to changes
**			 in uf.h.
**	28-feb-90 (bruceb)
**		Added FALSE as new last parameter to call on OOsaveSequence().
**		This simply says to retain the current (old) behavior.
**	18-apr-90 (bruceb)
**		Lint cleanup; removed 'formid' parameter to fdqbfmap.
**	29-aug-1990 (Joe)
**	    Changed to put DBMS info in UIDBDATA structure returned
**	    by IIUIdbdata.
**	08/15/91 (dkh) - Fixed bug 39081.  Added correct return values
**			 to all exit points for qfaddrow().
**	08-jun-92 (leighb) DeskTop Porting Change:
**		Changed 'errno' to 'errnum' cuz 'errno' is a macro in WIN/NT.
**	09/07/92 (dkh) - Updated with 6.5 call interface to FErelexists.
**	09/20/92 (dkh) - Added support for owner.table.
**	03/01/93 (dkh) - Fixed bug 49947.  Added delim id check for table
**			 name without owner before requoting it.
**	06/19/93 (dkh) - Added additional checks to make sure that user
**			 did not enter a table name that begins or ends
**			 with a period that is not part of a delimited id.
**	26-jan-94 (connie) Bug #57718
**		Call FErelexists to validate the table to be used for 
**		creating QbfName
**      15-jan-1996 (toumi01; from 1.1 axp_osf port)
**              Added kchin's changes (from 6.4) for axp_osf
**              10/15/93 (kchin)
**              Cast argument to PTR when calling IIOOidObject(), this
**              is due to the change of datatype of variable parameters
**              in IIOOidObject() routine itself.  The change is made in
**              qfaddrow().
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      15-Apr-2003 (bonro01)
**              Added prototype for OOsaveSequence() to prevent core dump.
**	21-jan-03 (hayke02)
**	    Remove local prototyping of IIUFccConfirmChoice() - this is now
**	    done in fe.h.
*/
  typedef struct qbf_name {
    char *qfname;
    char *tb_jdname;
    char *formname;
    char *relowner;
    char *own_tbl;
    i4 type;
    i2 relown_null;
  } QF_NAME;
  static QF_NAME qstruct = {0};
static bool		qf_update = FALSE;
static bool		qbfcat_alloc = FALSE;
  static char *_vfdetail = ERx("vfdetail");
  static char *_vfsave = ERx("vfsave");
GLOBALREF	OO_CATREC	vfoocatrec;
FUNC_EXTERN	OO_OBJECT	*OOp();
FUNC_EXTERN	STATUS	vfsvdummy();
FUNC_EXTERN	bool	FEestabsp();
FUNC_EXTERN	VOID	vfcallqbf();
FUNC_EXTERN	VOID	IIVFasfAllocSaveFrm();
FUNC_EXTERN	void	IIVFgtmGetTblName();
FUNC_EXTERN	STATUS		qfaddrow();
STATUS OOsaveSequence( OOID *id, char *savename, char *catform,
	char *cattabl, char *saveform, char *title,
	PTR ptr, i4 (*display)(), i4 (*save)(),
	MENUPARAM *menup, STATUS (*setup)(), char *help_scr,
	bool bypass);
# define    START_TRANSACTION	    FALSE
# define    COMMIT_TRANSACTION	    TRUE
# define    ROLLBACK_ENTIRE_XACT    TRUE
# define    ALL_BACKENDS	    FALSE
# define	TABLE_ADD	0
# define	QDEF_ADD	1
# define	stNEW		1
# define	stUNCHANGED	2
# define	stCHANGED	3
static STATUS
vfsvedqf ( eid, aptr )
  OOID eid;
PTR	aptr;
{
	FE_RSLV_NAME	delim_id;
	char		normal[FE_MAXNAME + 1];
  char nformname[FE_MAXNAME + 1];
  char ntb_jdnm[FE_MAXTABNAME + 1];
  char nowner[FE_MAXNAME + 2 + 1];
  char nrelname[FE_MAXNAME + 2 + 1];
  i2 n_null_own = 0;
	STATUS	stat = OK;
	i4	errnum;
/* # line 225 "qbfmap.qsc" */	/* getform */
  {
    if (IIfsetio(_vfdetail) != 0) {
      IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,nformname,(char *)
"xfield4");
      IIgetfldio((short *)0,1,32,FE_MAXTABNAME + 1-1,ntb_jdnm,(char *)
"xfield2");
    } /* IIfsetio */
  }
/* # line 227 "qbfmap.qsc" */	/* host code */
	CVlower(nformname);
	/*
	**  Need to handle delimited identifiers.
	*/
	if (qstruct.type == TABLE_ADD)
	{
		delim_id.is_nrml = FALSE;
		delim_id.owner_dest = nowner;
		delim_id.name_dest = nrelname;
		delim_id.name = ntb_jdnm;
		FE_decompose(&delim_id);
		/*
		**  Check to make sure user did not enter a name that
		**  simply begins or ends in a period outside of a delimited
		**  identifier.  If the name
		**  begins with a period, FE_decompose will have set
		**  the owner_spec member of FE_RESLV_NAME but that
		**  owner_dest contains an empty string.  If the name
		**  ends in a period, then the name_dest member will
		**  contain an empty string.
		*/
		if ((delim_id.owner_spec && delim_id.owner_dest[0] == EOS) ||
			delim_id.name_dest[0] == EOS)
		{
			IIUGerr(E_VF0171_BAD_TBL_NAME, UG_ERR_ERROR,
				1, ntb_jdnm);
			return(FAIL);
		}
		if (nowner[0] != EOS)
		{
			if (IIUGdlm_ChkdlmBEobject(nowner, normal,
				(bool) FALSE) == UI_BOGUS_ID)
			{
				IIUGerr(E_VF0170_BAD_OWNER_NAME, UG_ERR_ERROR,
					1, nowner);
				return(FAIL);
			}
			STcopy(normal, nowner);
		}
		if (IIUGdlm_ChkdlmBEobject(nrelname, normal, (bool) FALSE) ==
			UI_BOGUS_ID)
		{
			IIUGerr(E_VF0171_BAD_TBL_NAME, UG_ERR_ERROR,
				1, nrelname);
			return(FAIL);
		}
		STcopy(normal, nrelname);
	}
	else
	{
		STcopy(ntb_jdnm, nrelname);
		*nowner = EOS;
	}
	if (nowner[0] == EOS)
	{
		n_null_own = -1;
	}
	if (! STequal(nformname, qstruct.formname) ||
		!STequal(nrelname, qstruct.tb_jdname) ||
		!STequal(nowner, qstruct.relowner) ||
		qstruct.relown_null != n_null_own)
	{
		iiuicw1_CatWriteOn();
/* # line 280 "qbfmap.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"update ii_qbfnames set frnam\
e=");
    IIputdomio((short *)0,1,32,0,nformname);
    IIwritio(0,(short *)0,1,32,0,(char *)", relname=");
    IIputdomio((short *)0,1,32,0,nrelname);
    IIwritio(0,(short *)0,1,32,0,(char *)", relowner=");
    IIputdomio(&n_null_own,1,32,0,nowner);
    IIwritio(0,(short *)0,1,32,0,(char *)"where object_id=");
    IIputdomio((short *)0,1,30,4,&eid);
    IIsyncup((char *)0,0);
  }
/* # line 285 "qbfmap.sc" */	/* host code */
		if ((errnum = FEinqerr()) || IIUIdeadlock(errnum))
			stat = FAIL;
		iiuicw0_CatWriteOff();
		if ( stat == OK )
		{
			STcopy(nformname, qstruct.formname);
			STcopy(nrelname, qstruct.tb_jdname);
			STcopy(nowner, qstruct.relowner);
			qstruct.relown_null = n_null_own;
		}
	}
	return stat;
}
static STATUS
vfedqfdisp ( rid, rptr )
  OOID rid;
PTR	rptr;
  {
  char qfformname[FE_MAXNAME+1];
  char qftjname[FE_MAXNAME + 2 + 1];
  char qfrelown[FE_MAXNAME + 2 + 1];
  i4 qftype;
  i2 qf_null_own;
	i4		count;
	char		quoted[FE_UNRML_MAXNAME + 1];
	*qfrelown = EOS;
	count = 0;
/* # line 316 "qbfmap.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select frname, relname, qbftype, relowner from ii_qbfnames where obje\
ct_id=");
    IIputdomio((short *)0,1,30,4,&rid);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,qfformname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 2 + 1-1,qftjname);
      IIgetdomio((short *)0,1,30,4,&qftype);
      IIgetdomio(&qf_null_own,1,32,FE_MAXNAME + 2 + 1-1,qfrelown);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 321 "qbfmap.sc" */	/* host code */
		count++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if ( count == 0  || count > 1 )
	{
		IIUGerr( count == 0 ? E_VF0085_Could_not_get_detaile
				    : E_VF0086_Too_many_entries_foun,
				UG_ERR_ERROR, 1, (OOp(rid))->name
		);
		return FAIL;
	}
	STcopy(qfformname, qstruct.formname);
	STcopy(qftjname, qstruct.tb_jdname);
	STcopy(qfrelown, qstruct.relowner);
	qstruct.relown_null = qf_null_own;
	qstruct.type = qftype;
	if (qf_update)
	{ /* make Form and Query Object fields updatable */
/* # line 367 "qbfmap.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,_vfdetail,(char *)0, (char *)0) != 0) {
        IIstfsio(53,(char *)"xfield2",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
        IIstfsio(53,(char *)"xfield4",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 371 "qbfmap.qsc" */	/* host code */
	}
	if (qf_null_own == -1)
	{
		if (qf_update && qftype == TABLE_ADD)
		{
			/*
			**  If doing an update for a table based qbfname,
			**  we need to quote the owner/table name.
			*/
			if (IIUGdlm_ChkdlmBEobject(qftjname, NULL,
				TRUE) == UI_DELIM_ID)
			{
				IIUGrqd_Requote_dlm(qftjname, qstruct.own_tbl);
			}
			else
			{
				STcopy(qftjname, qstruct.own_tbl);
			}
		}
		else
		{
			STcopy(qftjname, qstruct.own_tbl);
		}
	}
	else
	{
		if (qf_update)
		{
			if (qftype == TABLE_ADD)
			{
				/*
				**  If doing an update for a table based qbfname,
				**  we need to quote the owner/table name.
				*/
				if  (IIUGdlm_ChkdlmBEobject(qfrelown, NULL,
					TRUE) == UI_DELIM_ID)
				{
					IIUGrqd_Requote_dlm(qfrelown,
						qstruct.own_tbl);
				}
				else
				{
					STcopy(qfrelown, qstruct.own_tbl);
				}
				if  (IIUGdlm_ChkdlmBEobject(qftjname, NULL,
					TRUE) == UI_DELIM_ID)
				{
					IIUGrqd_Requote_dlm(qftjname,
						quoted);
				}
				else
				{
					STcopy(qftjname, quoted);
				}
				/*
				IIUGrqd_Requote_dlm(qfrelown, qstruct.own_tbl);
				*/
				STcat(qstruct.own_tbl, ERx("."));
				/*
				IIUGrqd_Requote_dlm(qftjname, quoted);
				*/
				STcat(qstruct.own_tbl, quoted);
			}
			else
			{
				/*
				**  We don't support owner.joindef so
				**  just copy the name.
				*/
				STcopy(qftjname, qstruct.own_tbl);
			}
		}
		else
		{
			if (qftype == TABLE_ADD)
			{
				STcopy(qfrelown, qstruct.own_tbl);
				STcat(qstruct.own_tbl, ERx("."));
				STcat(qstruct.own_tbl, qftjname);
			}
			else
			{
				STcopy(qftjname, qstruct.own_tbl);
			}
		}
	}
/* # line 461 "qbfmap.qsc" */	/* putform */
    {
      if (IIfsetio(_vfdetail) != 0) {
        IIputfldio((char *)"xfield3",(short *)0,1,32,0,ERget(F_VF0091_FORM));
        IIputfldio((char *)"xfield4",(short *)0,1,32,0,qstruct.formname);
        IIputfldio((char *)"xfield1",(short *)0,1,32,0,
        ERget( qftype == TABLE_ADD	
				? F_VF0090_TABLE : F_VF0089_JOINDEF
		));
        IIputfldio((char *)"xfield2",(short *)0,1,32,0,qstruct.own_tbl);
      } /* IIfsetio */
    }
/* # line 469 "qbfmap.qsc" */	/* host code */
	return OK;
  }
static STATUS
vfsvdispqf()
  {
/* # line 475 "qbfmap.qsc" */	/* putform */
    {
      if (IIfsetio(_vfsave) != 0) {
        IIputfldio((char *)"xfield3",(short *)0,1,32,0,ERget(F_VF0091_FORM));
        IIputfldio((char *)"xfield4",(short *)0,1,32,0,qstruct.formname);
        IIputfldio((char *)"xfield1",(short *)0,1,32,0,
        ERget( qstruct.type == TABLE_ADD	
				? F_VF0090_TABLE : F_VF0089_JOINDEF
		));
        IIputfldio((char *)"xfield2",(short *)0,1,32,0,qstruct.own_tbl);
      } /* IIfsetio */
    }
/* # line 483 "qbfmap.qsc" */	/* host code */
	return OK;
  }
allocQbf()
{
	if (!qbfcat_alloc)
	{
		FDsetparse(FALSE);
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), ERx("vfqbfcat")) != OK)
		{
			syserr(ERget(S_VF008A_Can_not_access_form));
		}
		qbfcat_alloc = TRUE;
		FDsetparse(TRUE);
	}
}
i4
fdqbfmap(fname)
char	*fname;
{
  OOID qnid;
	i4			retcode = FALSE;
	i4			tries;
	i4			completed;
  char tqbfname[FE_MAXNAME + 1];
  char qual[300];
  char *_vfqbfcat = ERx("vfqbfcat");
	OOID			classid;
	char			*classname;
  char qfformname[FE_MAXNAME + 1];
  char qftjname[FE_MAXNAME + 2 + 1];
  char qfrelowner[FE_MAXNAME + 2 + 1];
  char qfown_tbl[FE_MAXTABNAME + 1];
  char msgbuf[100];
	i4			errnum;
  char formname[FE_MAXNAME + 1];
	char			*help_subject;
	vfdmpoff();
/* # line 531 "qbfmap.qsc" */	/* message */
  {
    IImessage(ERget(S_VF008B_Retrieving_catalogs));
  }
/* # line 532 "qbfmap.qsc" */	/* host code */
	allocQbf();
	IIVFasfAllocSaveFrm();
	STcopy(fname, formname);
	STprintf(qual,
ERx("ii_qbfnames.frname = '%s' and ii_objects.object_id = ii_qbfnames.object_id"),
	    formname);
/* # line 540 "qbfmap.qsc" */	/* display */
  {
    if (IIdispfrm(_vfqbfcat,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 543 "qbfmap.qsc" */	/* host code */
		STprintf(msgbuf, ERget(S_VF008C_VIFRED_QBFNames_Cat), formname);
/* # line 544 "qbfmap.qsc" */	/* putform */
      {
        if (IIfsetio(_vfqbfcat) != 0) {
          IIputfldio((char *)"title",(short *)0,1,32,0,msgbuf);
        } /* IIfsetio */
      }
/* # line 545 "qbfmap.qsc" */	/* host code */
		OOcatTableLoad(OC_QBFNAME, _vfqbfcat, _cattable, (bool) FALSE,
			ERx("*"), NULL, qual);
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
          char bufr[132];
/* # line 553 "qbfmap.qsc" */	/* host code */
		do
		{
/* # line 555 "qbfmap.qsc" */	/* prompt */
          {
            IIprmptio(0,ERget(S_VF008D_Name_to_use_with_QBF),(short *)0,1,32,
            131,bufr);
          }
/* # line 556 "qbfmap.qsc" */	/* host code */
			if ( STtrmwhite(bufr) <= 0 )
			{
/* # line 558 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 559 "qbfmap.qsc" */	/* host code */
			}
			bufr[FE_MAXNAME] = EOS;
		} while (FEchkname(bufr) != OK);
		for (;;)
		{
/* # line 565 "qbfmap.qsc" */	/* submenu */
          {
            goto IImuI2;
IIfdB2:
            IIrunmu(0);
            if (IIretval() == 1) {
              {
/* # line 568 "qbfmap.qsc" */	/* host code */
				if (qfaddrow(bufr, formname, TABLE_ADD) != OK)
				{
					break;
				}
				OOcatTableLoad(OC_QBFNAME, _vfqbfcat, _cattable,
					(bool) FALSE, ERx("*"), bufr, qual);
				break;
              }
            } else if (IIretval() == 2) {
              {
/* # line 578 "qbfmap.qsc" */	/* host code */
				if (qfaddrow(bufr, formname, QDEF_ADD) != OK)
				{
					break;
				}
				OOcatTableLoad(OC_QBFNAME, _vfqbfcat, _cattable,
					(bool) FALSE, ERx("*"), bufr, qual);
				break;
              }
            } else if (IIretval() == 3) {
              {
/* # line 588 "qbfmap.qsc" */	/* host code */
				FEhelp(VFH_QNSMU,
					ERget(S_VF008E_Create_QBFName_Submen));
				continue;
              }
            } else if (IIretval() == 4) {
              {
/* # line 594 "qbfmap.qsc" */	/* host code */
				break;
              }
            } else {
              if (1) goto IIfdE2;
            } /* IIretval */
            goto IIfdE2;
IImuI2:
            IImuonly();
            if (IIinitmu() == 0) goto IIfdE2;
            if (IInmuact(ERget(FE_Table),(char *)0,2,2,1) == 0) goto IIfdE2;
            if (IInmuact(ERget(FE_JoinDef),(char *)0,2,2,2) == 0) goto IIfdE2;
            if (IInmuact(ERget(FE_Help),(char *)0,2,2,3) == 0) goto IIfdE2;
            IIFRInternal(0);
            if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE2;
            if (IInmuact(ERget(FE_End),(char *)0,2,2,4) == 0) goto IIfdE2;
            IIFRInternal(0);
            if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE2;
            if (IIendmu() == 0) goto IIfdE2;
            goto IIfdB2;
IIfdE2:;
          }
/* # line 596 "qbfmap.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 2) {
        {
/* # line 600 "qbfmap.qsc" */	/* host code */
# ifdef DGC_AOS
		/*
		** Until INGRES supports a readonly mode, DG will use the
		** following code to avoid ugly error messages that occur
		** when trying to write to the database when we are only
		** connected for readonly access.
		*/
		if (IIUIcro_CheckReadOnly())
		{
			IIUGerr(E_UI0016_ReadOnly_err, 0, 0);
/* # line 610 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 611 "qbfmap.qsc" */	/* host code */
		}
# endif
		/*
		**  Fix for BUG 5400. (dkh)
		*/
		if (OOcatCheck(OC_QBFNAME, _vfqbfcat, _cattable,
			&vfoocatrec) != OK)
		{
/* # line 620 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 621 "qbfmap.qsc" */	/* host code */
		}
		STcopy(vfoocatrec.name, tqbfname);
		qnid = vfoocatrec.id;
		classname = (OOp(OC_QBFNAME))->name;
		if ( !STequal(vfoocatrec.owner, IIUIdbdata()->user) )
		{
			IIUGmsg(ERget(S_VF008F_You_do_not_own), TRUE,
				2, classname, tqbfname);
/* # line 630 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 631 "qbfmap.qsc" */	/* host code */
		}
		help_subject = STalloc(ERget(S_VF0159_Destroy_QBFName));
		if (IIUFccConfirmChoice(CONF_DESTROY, tqbfname, classname,
		    help_subject, VFH_DSTQBF) != CONFCH_YES)
		{
/* # line 637 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 638 "qbfmap.qsc" */	/* host code */
		}
		IIUGmsg(ERget(S_VF0091_Destroying), FALSE,
			2, classname, tqbfname);
		iiuicw1_CatWriteOn();
		completed = FALSE;
		for (tries = 0; tries < MAXDLOCK_TRIES; tries++)
		{
			/*
			**  Commit all work thus far.  If delete fails, then
			**  can rollback all work up to this point, but not
			**  including any transactions before this.
			*/
			_VOID_ IIUIbeginXaction();
/* # line 689 "qbfmap.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_qbfnames where object_id=");
    IIputdomio((short *)0,1,30,4,&qnid);
    IIsyncup((char *)0,0);
  }
/* # line 690 "qbfmap.sc" */	/* host code */
			if ((errnum = FEinqerr()))
			{
				IIUIabortXaction();
				if (IIUIdeadlock(errnum))
				{
					continue;
				}
				iiuicw0_CatWriteOff();
/* # line 669 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 670 "qbfmap.qsc" */	/* host code */
			}
			IIUIendXaction();
			completed = TRUE;
			break;
		}
		iiuicw0_CatWriteOff();
		if (!completed)
		{
			IIUGmsg(ERget(S_VF0092_Deadlock_has_occurred),
				TRUE, 0);
/* # line 682 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 683 "qbfmap.qsc" */	/* host code */
		}
		iiuicw1_CatWriteOn();
		classid = OC_QBFNAME;
		if ( IIOOidDestroy(&classid, &(vfoocatrec.id), vfoocatrec.name,
					vfoocatrec.owner) != OK )
		{
			IIUGerr(E_VF0114_destroy_err, UG_ERR_ERROR,
				2, OOsnd(classid, _name), vfoocatrec.name);
		}
		else
		{
/* # line 696 "qbfmap.qsc" */	/* deleterow */
          {
            if (IItbsetio(4,_vfqbfcat,_cattable,-2) != 0) {
              if (IItdelrow(0) != 0) {
              } /* IItdelrow */
            } /* IItbsetio */
          }
/* # line 697 "qbfmap.qsc" */	/* host code */
		}
		iiuicw0_CatWriteOff();
        }
      } else if (IIretval() == 3) {
        {
/* # line 703 "qbfmap.qsc" */	/* host code */
		if (OOcatCheck(OC_QBFNAME, _vfqbfcat, _cattable,
			&vfoocatrec) != OK)
		{
/* # line 706 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 707 "qbfmap.qsc" */	/* host code */
		}
		if ( !STequal(vfoocatrec.owner, IIUIdbdata()->user) )
		{
			IIUGmsg(ERget(S_VF0093_You_are_not_the_owner),
				TRUE, 1, vfoocatrec.name);
/* # line 712 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 713 "qbfmap.qsc" */	/* host code */
		}
		qstruct.formname = qfformname;
		qstruct.tb_jdname = qftjname;
		qstruct.relowner = qfrelowner;
		qstruct.own_tbl = qfown_tbl;
		qf_update = TRUE;
		STcopy(ERget(S_VF0094_Editing_a_qbfname), msgbuf);
		/*
		**  Important assumption - OOdetail() always sets fields
		**  "xfield1" and "xfield3" to DISPLAY ONLY everytime
		**  it is called.  And before call to routine vfedqfdisp().
		*/
		_VOID_ OOdetail(OC_QBFNAME, _vfqbfcat, _cattable, &vfoocatrec,
			_vfdetail, msgbuf,
			NULL, vfedqfdisp, NULL, NULL, NULL, vfsvedqf,
			NULL, ERx("vfqbfdet.hlp")
		);
		qf_update = FALSE;
/* # line 736 "qbfmap.qsc" */	/* putform */
          {
            if (IIfsetio(_vfdetail) != 0) {
              IIputfldio((char *)"xfield1",(short *)0,1,32,0,_iiOOempty);
              IIputfldio((char *)"xfield2",(short *)0,1,32,0,_iiOOempty);
              IIputfldio((char *)"xfield3",(short *)0,1,32,0,_iiOOempty);
              IIputfldio((char *)"xfield4",(short *)0,1,32,0,_iiOOempty);
            } /* IIfsetio */
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 742 "qbfmap.qsc" */	/* host code */
# ifdef DGC_AOS
		/*
		** Until INGRES supports a readonly mode, DG will use the
		** following code to avoid ugly error messages that occur
		** when trying to write to the database when we are only
		** connected for readonly access.
		*/
		if (IIUIcro_CheckReadOnly())
		{
			IIUGerr(E_UI0016_ReadOnly_err, 0, 0);
/* # line 752 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 753 "qbfmap.qsc" */	/* host code */
		}
# endif
		if (OOcatCheck(OC_QBFNAME, _vfqbfcat, _cattable,
			&vfoocatrec) != OK)
		{
/* # line 759 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 760 "qbfmap.qsc" */	/* host code */
		}
		if (OOcatRename(OC_QBFNAME, _vfqbfcat, _cattable,
			&vfoocatrec, FALSE) != OK)
		{
/* # line 767 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 768 "qbfmap.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 5) {
        {
/* # line 774 "qbfmap.qsc" */	/* host code */
		if (OOcatCheck(OC_QBFNAME, _vfqbfcat, _cattable,
			&vfoocatrec) != OK)
		{
/* # line 777 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 778 "qbfmap.qsc" */	/* host code */
		}
		qstruct.formname = qfformname;
		qstruct.tb_jdname = qftjname;
		qstruct.relowner = qfrelowner;
		qstruct.own_tbl = qfown_tbl;
		qf_update = FALSE;
		STcopy(ERget(S_VF0095_Detail_on_a_qbfname), msgbuf);
		_VOID_ OOdetail(OC_QBFNAME, _vfqbfcat, _cattable, &vfoocatrec,
			_vfdetail, msgbuf,
			NULL, vfedqfdisp, NULL, NULL, NULL, vfsvdummy,
			NULL, ERx("vfqbfdet.hlp")
		);
/* # line 794 "qbfmap.qsc" */	/* putform */
          {
            if (IIfsetio(_vfdetail) != 0) {
              IIputfldio((char *)"xfield1",(short *)0,1,32,0,_iiOOempty);
              IIputfldio((char *)"xfield2",(short *)0,1,32,0,_iiOOempty);
              IIputfldio((char *)"xfield3",(short *)0,1,32,0,_iiOOempty);
              IIputfldio((char *)"xfield4",(short *)0,1,32,0,_iiOOempty);
            } /* IIfsetio */
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 800 "qbfmap.qsc" */	/* host code */
		char		*go_qflag = ERx("-f");
		char		*uname = _iiOOempty;
		char		go_param[200];
		char		go_args[200];
          char prbuf[200];
          char go_table[FE_MAXTABNAME + 1];
		FE_RSLV_NAME	delim_id;
		char		table[FE_UNRML_MAXNAME + 1];
		char		owner[FE_UNRML_MAXNAME + 1];
		char		normal[FE_UNRML_MAXNAME + 1];
		/*
		**  If CMS, do not execute QBF from vifred.
		*/
# ifdef CMS
		IIUGmsg(ERget(S_VF011A_Not_implemen), (bool) TRUE, 0);
/* # line 816 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 817 "qbfmap.qsc" */	/* host code */
# endif
		if (OOcatCheck(OC_QBFNAME, _vfqbfcat, _cattable, &vfoocatrec) !=
			OK)
		{
			IIUGfmt(prbuf, sizeof(prbuf),
				ERget(S_VF0028_Table_to_use_with_for),
				1, vfoocatrec.name);
/* # line 824 "qbfmap.qsc" */	/* prompt */
          {
            IIprmptio(0,prbuf,(short *)0,1,32,FE_MAXTABNAME + 1-1,go_table);
          }
/* # line 825 "qbfmap.qsc" */	/* host code */
			if (go_table[0] == EOS)
			{
/* # line 827 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 828 "qbfmap.qsc" */	/* host code */
			}
			/*
			**  Run user entered name through delim
			**  checking code so we can determine
			**  if the table exists.
			**
			**  Note that we leave what the user entered
			**  alone so we can use it directly as a parameter
			**  when calling QBF.  This way, we don't have
			**  to requote as the user should have done it
			**  on entry.
			*/
			delim_id.is_nrml = FALSE;
			delim_id.owner_dest = owner;
			delim_id.name_dest = table;
			delim_id.name = go_table;
			FE_decompose(&delim_id);
			/*
			**  Check to make sure user did not enter a name that
			**  simply begins or ends in a period outside of a
			**  delimited identifier.  If the name
			**  begins with a period, FE_decompose will have set
			**  the owner_spec member of FE_RESLV_NAME but that
			**  owner_dest contains an empty string.  If the name
			**  ends in a period, then the name_dest member will
			**  contain an empty string.
			*/
			if ((delim_id.owner_spec &&
				delim_id.owner_dest[0] == EOS) ||
				delim_id.name_dest[0] == EOS)
			{
				IIUGerr(E_VF0171_BAD_TBL_NAME, UG_ERR_ERROR,
					1, go_table);
/* # line 864 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 865 "qbfmap.qsc" */	/* host code */
			}
			if (IIUGdlm_ChkdlmBEobject(table, normal,
				(bool) FALSE) == UI_BOGUS_ID)
			{
				IIUGerr(E_VF0171_BAD_TBL_NAME,
					UG_ERR_ERROR, 1, table);
/* # line 872 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 873 "qbfmap.qsc" */	/* host code */
			}
			STcopy(normal, table);
			if (owner[0] != EOS)
			{
				if (IIUGdlm_ChkdlmBEobject(owner,
					normal, (bool) FALSE) == UI_BOGUS_ID)
				{
					IIUGerr(E_VF0170_BAD_OWNER_NAME,
						UG_ERR_ERROR, 1, owner);
/* # line 882 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 883 "qbfmap.qsc" */	/* host code */
				}
				STcopy(normal, owner);
			}
			if (FErelexists(table, owner) != OK)
			{
				IIUGerr(E_VF0029_Table_does_not,
					UG_ERR_ERROR, 1, go_table);
/* # line 891 "qbfmap.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 892 "qbfmap.qsc" */	/* host code */
			}
			go_qflag = ERx("-F");
		}
		else
		{
			go_table[0] = EOS;
		}
		/*
		**  No testing flags passed for now.
		*/
		STprintf(go_param,
			ERx("database = %%S, equel = %%S, flags = %%S"));
		if (vfuname != NULL)
		{
			uname = vfuname;
		}
		STprintf(go_args, ERx("%s %s %s %s %s"), uname,
			ERx("-mall"), go_qflag, vfoocatrec.name, go_table);
		vfcallqbf(go_param, go_args);
        }
      } else if (IIretval() == 7) {
        {
/* # line 916 "qbfmap.qsc" */	/* host code */
		/* Implement the Find command. */
		FEtabfnd(_vfqbfcat, _cattable);
        }
      } else if (IIretval() == 8) {
        {
/* # line 921 "qbfmap.qsc" */	/* host code */
		/* Implement the Top command. */
/* # line 922 "qbfmap.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_vfqbfcat,_cattable,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 9) {
        {
/* # line 926 "qbfmap.qsc" */	/* host code */
		/* Implement the Bottom command. */
/* # line 927 "qbfmap.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_vfqbfcat,_cattable,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,-1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 10) {
        {
/* # line 931 "qbfmap.qsc" */	/* host code */
		FEhelp(VFH_QNCAT, ERget(S_VF0096_QBFName_Catalogs));
        }
      } else if (IIretval() == 11) {
        {
/* # line 935 "qbfmap.qsc" */	/* host code */
		retcode = FALSE;
/* # line 936 "qbfmap.qsc" */	/* breakdisplay */
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
    if (IInfrskact(1,(char *)0,2,2,10) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),(char *)0,2,2,11) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,11) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 938 "qbfmap.qsc" */	/* host code */
	return (retcode);
}
STATUS
vfsvqfname ( qid, aptr )
  OOID qid;
PTR	aptr;
{
	STATUS	stat = OK;
	iiuicw1_CatWriteOn();
/* # line 1090 "qbfmap.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_qbfnames(object_id, frname, relname, relowner, qbftype\
)values(");
    IIputdomio((short *)0,1,30,4,&qid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,qstruct.formname);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,qstruct.tb_jdname);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio(&qstruct.relown_null,1,32,0,qstruct.relowner);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&qstruct.type);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 1095 "qbfmap.sc" */	/* host code */
	stat = FEinqerr();
	iiuicw0_CatWriteOff();
	return stat;
}
STATUS
qfaddrow(sqbfname, frmname, type)
  char *sqbfname;
  char *frmname;
i4	type;
{
	char		*classname;
	OOID		addid = OC_UNDEFINED;
	OOID		qfsvid = OC_UNDEFINED;
	OOID		vfid_qbfn = OC_QBFNAME;
	char		title[33]; /* field is c32 */
	char		stblname[FE_MAXNAME + 2 + 1];
	char		srelowner[FE_MAXNAME + 2 + 1];
  char buf[FE_PROMPTSIZE + 1];
	i2		rel_null_own = 0;
	FE_RSLV_NAME	delim_id;
	char		normal[FE_MAXNAME + 1];
	classname = (OOp(OC_QBFNAME))->name;
	/*
	**  If an entry already exists, then just return;
	*/
	if (IIOOidCheck(&vfid_qbfn, &addid, sqbfname, IIUIdbdata()->user) == OK)
	{
		IIUGmsg(ERget(S_VF0099_already_exist), TRUE, 2,
			classname, sqbfname);
		return(FAIL);
	}
	buf[0] = EOS;
	for (;;)
	{
		if (type == TABLE_ADD)
		{
			IIVFgtmGetTblName(buf, ERget(F_VF00BD_ENTER_TNAME),
				QN_TBL_PROMPT);
		}
		else
		{
			IIVFgtmGetTblName(buf, ERget(F_VF00BE_ENTER_JNAME),
				QN_JDF_PROMPT);
		}
		if ( *buf == EOS )
			return(FAIL);
		if (type == TABLE_ADD)
		{
			delim_id.is_nrml = FALSE;
			delim_id.owner_dest = srelowner;
			delim_id.name_dest = stblname;
			delim_id.name = buf;
			FE_decompose(&delim_id);
			/*
			**  Check to make sure user did not enter a name that
			**  simply begins or ends in a period outside of a
			**  delimited identifier.  If the name
			**  begins with a period, FE_decompose will have set
			**  the owner_spec member of FE_RESLV_NAME but that
			**  owner_dest contains an empty string.  If the name
			**  ends in a period, then the name_dest member will
			**  contain an empty string.
			*/
			if ((delim_id.owner_spec &&
				delim_id.owner_dest[0] == EOS) ||
				delim_id.name_dest[0] == EOS)
			{
				IIUGerr(E_VF0171_BAD_TBL_NAME, UG_ERR_ERROR,
					1, buf);
				continue;
			}
			if (IIUGdlm_ChkdlmBEobject(stblname, normal,
				(bool) FALSE) == UI_BOGUS_ID)
			{
				IIUGerr(E_VF0171_BAD_TBL_NAME, UG_ERR_ERROR, 1,
					stblname);
				continue;
			}
			STcopy(normal, stblname);
			if (srelowner[0] != EOS)
			{
				if (IIUGdlm_ChkdlmBEobject(srelowner, normal,
					(bool) FALSE) == UI_BOGUS_ID)
				{
					IIUGerr(E_VF0170_BAD_OWNER_NAME,
						UG_ERR_ERROR, 1, srelowner);
					continue;
				}
				STcopy(normal, srelowner);
			}
			if (srelowner[0] == EOS)
			{
				rel_null_own = -1;
			}
			if (FErelexists(stblname, srelowner) != OK)
			{
				IIUGerr(E_VF0102_Relation_does_not, 
					UG_ERR_ERROR, 2, buf, dbname);
				return (FAIL);
			}
			break;
		}
		else
		{
			if (FEchkname(buf) == OK)
			{
				rel_null_own = -1;
				STlcopy(buf, stblname, FE_MAXNAME);
				break;
			}
			else
			{
				IIUGerr(E_VF0176_BAD_JDEF_NAME, UG_ERR_ERROR,
					1, buf);
			}
		}
	}
	CVlower( sqbfname );
	/*
	**  If table, then can be mixed case, but if a joindef, then
	**  must be lower cased.
	*/
	if (type != TABLE_ADD)
	{
		CVlower(stblname);
	}
	qstruct.qfname = sqbfname;
	qstruct.tb_jdname = stblname;
	qstruct.formname = frmname;
	qstruct.relowner = srelowner;
	qstruct.own_tbl = buf;
	qstruct.relown_null = rel_null_own;
	qstruct.type = type;
	if ( IIOOidObject(OC_QBFNAME, &qfsvid, sqbfname, (PTR)0, IIUIdbdata()->user,
			(PTR)1, (PTR)_iiOOempty, (char *)NULL, (char *)NULL, (PTR)_iiOOempty) != OK )
	{
		IIUGerr(E_VF009E_Error_occurred_create, UG_ERR_ERROR, 0);
		return(FAIL);
	}
	STcopy(ERget(S_VF009F_Saving_a_qbfname), title);
	return OOsaveSequence( &qfsvid, sqbfname, ERx("vfqbfcat"),
		_cattable, _vfsave, title, (PTR) &qstruct, vfsvdispqf,
		vfsvqfname, NULL, NULL, ERx("vfqbfsav.hlp"), (bool)FALSE
	);
}
