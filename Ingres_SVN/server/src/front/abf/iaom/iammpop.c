# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1989, 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<nm.h>
# include	<ex.h>
# include	<ol.h>
# include	<cm.h>
# include	<st.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ug.h>
# include	<ui.h>
# include <abclass.h>
# include <metafrm.h>
# include	<oodep.h>
# include	<ilerror.h>
# include	"iamstd.h"
# include	"iamtbl.h"
# include	"iamtok.h"
# include	"eram.h"
/**
** Name:	iammpop.c -	Metaframe populate.
**
** Description:
**
**	Routines to fill in parts of the METAFRAME structure from
**	auxiliary catalogs, and update them.  Defines:
**
**		IIAMmpMetaPopulate
**		IIAMmnaMetaNewArray
**		IIAMmuMetaUpdate
**		IIAMmfMetaFree
**
** History:
**	4/89 (bobm)	written.
**	17-nov-89 (kenl)
**		Changed mf_fpop() and mf_fupd() to handle dtype field in
**		MFVAR struct which was changed from a DB_DATA_VALUE to a *char.
**	03/09/91 (emerson)
**		Integrated DESKTOP porting changes.
**	22-jul-92 (blaise)
**		Added initializers to Escenc and Escdec corresponding to
**		new vproc member of CODETABLE structure.
**	11-aug-92 (blaise)
**		Added logic to mf_fpop() and mf_fupd() to handle the fact that
**		the MFVAR structure can now hold local procedures as well as
**		local variables.
**	18-may-93 (davel)
**		added argument to the iiamAddDep() calls.
**	15-nov-93 (connie) Bug #40333
**		Added IIAMmtdMenuTextDel() to remove the ii_menuargs entries
**		corresponding to a removed/destroyed frame
**	4-jan-1994 (mgw) Bug #58241
**		Implement owner.table for query frame creation.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**    25-Oct-2005 (hanje04)
**        Add prototype for mf_vqpjoins(), mf_vqptabs(), mf_vqujoins(),
**	  mf_vqutabs(), mf_vqucols() & bs_puts() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
**/
/* define's */
# define MAXVARWORDS	12	/* max no. of words in a variable type (size
					of array passed to STgetwords) */
# define BUFLEN		255
/*
** NOTE:  This is the structure described by Escdec.  It MUST match.
*/
typedef struct
{
	i4	version;
	MFESC	*esc;
	i4	numescs;
} ESCDEC;
/*
** NOTE:  This is the structure described by Escenc.  It MUST match.
*/
typedef struct
{
	i4	version;
	MFESC	**esc;
	i4	numescs;
} ESCENC;
VOID vproc_err();
VOID vproc_noop();
static STATUS mf_vqpop();
static STATUS mf_vqupd();
static STATUS mf_epop();
static STATUS mf_eupd();
static STATUS mf_apop();
static STATUS mf_aupd();
static STATUS mf_fpop();
static STATUS mf_fupd();
static VOID mf_vqall();
static VOID mf_eall();
static VOID mf_aall();
static VOID mf_fall();
static PTR esc_alloc();
static i4  esc_dec();
static i4  esc_enc();
static char *bs_index();
static STATUS mf_vqpjoins();
static STATUS mf_vqptabs();
static STATUS mf_vqujoins();
static STATUS mf_vqutabs();
static STATUS mf_vqucols();
static STATUS bs_puts();
/*
** drive table for populating metaframe structure.
*/
static struct
{
	i4 bit;		/* which bit this entry corresponds to */
	STATUS (*pop)();	/* populate routine */
	STATUS (*upd)();	/* update routine */
	VOID (*arr)();		/* array alloc. routine */
	i4 tagidx;		/* index of tag */
} Utab[] =
{
	{ MF_P_VQ, mf_vqpop, mf_vqupd, mf_vqall, MF_PT_VQ },
	{ MF_P_ESC, mf_epop, mf_eupd, mf_eall, MF_PT_ESC },
	{ MF_P_ARG, mf_apop, mf_aupd, mf_aall, MF_PT_ARG },
	{ MF_P_FLD, mf_fpop, mf_fupd, mf_fall, MF_PT_FLD }
};
/*
** encode / decode handled through different tables so that in
** one instance we can directly decode an array of structures,
** and in the other go through an array of pointers.
*/
static CODETABLE Escenc [] =
{
	{AOO_i4, 0, NULL, NULL, NULL, vproc_err},
	{AOO_array, sizeof(MFESC *), NULL, NULL, esc_enc, NULL}
};
static CODETABLE Escdec [] =
{
	{AOO_i4, 0, NULL, NULL, NULL, vproc_noop},
	{AOO_array, sizeof(MFESC), esc_alloc, esc_dec, NULL, NULL}
};
/*
** allocation failure handling - generate exception.
** If str argument in loc_alloc() is non-NULL, a string allocation
** is desired, and size is irrelevent.
*/
static i4
ex_hdlr(ex_args)
EX_ARGS ex_args;
{
	if (ex_args.exarg_num == EXFEMEM)
		return (EXDECLARE);
	return (EXRESIGNAL);
}
static PTR
loc_alloc(tag,size,str)
u_i4	tag;
i4	size;
char	*str;
{
	PTR mem;
	if (str != NULL)
		size = STlength(str)+1;
	mem = FEreqmem((u_i4)tag, (u_i4)size, FALSE, (STATUS *)NULL);
	if (mem == NULL)
	{
		FEfree(tag);
		EXsignal(EXFEMEM,1, "iiamMeta()");
	}
	if (str != NULL)
		STcopy(str,(char *) mem);
	return (mem);
}
/*{
** Name:	IIAMmpMetaPopulate() - populate metaframe structure.
**
** Description:
**	Populate portions of the metaframe structure which are read from
**	auxiliary catalogs.  Updates mask of metaframe structure showing
**	which bits are populated.
**
** Inputs:
**	mf - metaframe structure which is to be populated.
**	which - bit mask of which parts to populate.
**
** Returns:
**	OK - filled in structure.
**		or failure status.
**
** History:
**	4/89 (bobm)	written
*/
STATUS
IIAMmpMetaPopulate(mf,which)
METAFRAME *mf;
u_i4 which;
{
	register i4	i;
	STATUS		rc;
	EX_CONTEXT	ex;
	i4		tagidx;
	if (EXdeclare(ex_hdlr, &ex) != OK)
	{
		EXdelete();
		IIUGerr(E_AM0029_MetaPopErr, UG_ERR_ERROR, 1,
			mf->apobj->name);
		return (FAIL);
	}
	for (i=0; i < sizeof(Utab)/sizeof(Utab[0]); ++i)
	{
		if (i >= MF_P_NUM)
		{
			EXsignal( EXFEBUG, 1,
			       ERx("IIAMmpMetaPopulate(): drive table too long")
			);
			/*NOT REACHED*/
		}
		if ((which & Utab[i].bit) == 0)
			continue;
		tagidx = Utab[i].tagidx;
		/*
		** use FEfree, not IIUGfree - we will reuse this
		** tag number.
		*/
		if ((mf->ptag)[tagidx] != 0)
			FEfree((mf->ptag)[tagidx]);
		else
			(mf->ptag)[tagidx] = FEgettag();
		(*(Utab[i].arr))(mf,(mf->ptag)[tagidx]);
		if ((rc = (*(Utab[i].pop))(mf,(mf->ptag)[tagidx])) != OK)
		{
			EXdelete();
			IIUGerr(E_AM0029_MetaPopErr, UG_ERR_ERROR, 1,
				mf->apobj->name);
			return (rc);
		}
		mf->popmask |= Utab[i].bit;
		mf->updmask &= ~(Utab[i].bit);
	}
	EXdelete();
	return (OK);
}
/*{
** Name:	IIAMmnaMetaNewArray() - allocate arrays in metaframe
**
** Description:
**	Allocate arrays in metaframe structure.  This operation results in
**	array allocations tagged as for "Populate", but does not actually
**	query the DB.  This is used upon creation of a new metaframe, to
**	allocate arrays which may be filled in, and then freed through the
**	same mechanism as existing frames.
**
** Inputs:
**	mf - metaframe structure which is to be allocated.
**	which - bit mask of which parts to allocate.
**
** Returns:
**	OK - filled in structure.
**		or failure status.
**
** History:
**	4/90 (bobm)	written
*/
STATUS
IIAMmnaMetaNewArray(mf,which)
METAFRAME *mf;
u_i4 which;
{
	register i4	i;
	STATUS		rc;
	EX_CONTEXT	ex;
	i4		tagidx;
	if (EXdeclare(ex_hdlr, &ex) != OK)
	{
		EXdelete();
		return (FAIL);
	}
	for (i=0; i < sizeof(Utab)/sizeof(Utab[0]); ++i)
	{
		if (i >= MF_P_NUM)
		{
			EXsignal( EXFEBUG, 1,
			       ERx("IIAMmpMetaPopulate(): drive table too long")
			);
			/*NOT REACHED*/
		}
		if ((which & Utab[i].bit) == 0)
			continue;
		tagidx = Utab[i].tagidx;
		/*
		** use FEfree, not IIUGfree - we will reuse this
		** tag number.
		*/
		if ((mf->ptag)[tagidx] != 0)
			FEfree((mf->ptag)[tagidx]);
		else
			(mf->ptag)[tagidx] = FEgettag();
		(*(Utab[i].arr))(mf,(mf->ptag)[tagidx]);
		mf->popmask |= Utab[i].bit;
		mf->updmask &= ~(Utab[i].bit);
	}
	EXdelete();
	return (OK);
}
/*{
** Name:	IIAMmuMetaUpdate() - Update metaframe structure.
**
** Description:
**	Update auxiliary catalog portions corresponding to a metaframe
**	structure.  Clears bits of update mask in metaframe structure.
**
** Inputs:
**	mf - metaframe structure which catalogs are to be updated from.
**	which - bit mask of which parts to update.
**
** Returns:
**	OK - updated catalogs.
**		or failure status.
**
** History:
**	4/89 (bobm)	written
*/
STATUS
IIAMmuMetaUpdate(mf,which)
METAFRAME *mf;
u_i4 which;
{
	i4 i;
	STATUS rc;
	IIUIbeginXaction();
	rc = OK;
	for (i=0; i < sizeof(Utab)/sizeof(Utab[0]); ++i)
	{
		if ((which & Utab[i].bit) == 0)
			continue;
		iiuicw1_CatWriteOn();
		rc = (*(Utab[i].upd))(mf, mf->apobj->ooid);
		iiuicw0_CatWriteOff();
		if (rc != OK )
			break;
		mf->updmask &= ~(Utab[i].bit);
	}
	if (rc == OK)
		IIUIendXaction();
	else
		IIUIabortXaction();
	return (rc);
}
/*{
** Name:	IIAMmfMetaFree() - Free portions of metaframe structure.
**
** Description:
**	Free portions of metaframe structure corresponding to auxiliary
**	catalogs.  Updates mask of populated portions to show that they
**	are no longer populated since their memory has been freed.
**
** Inputs:
**	mf - metaframe structure which is to have portions freed
**	which - bit mask of which parts to free.
**
** Returns:
**	OK - updated catalogs.
**		or failure status.
**
** History:
**	4/89 (bobm)	written
*/
STATUS
IIAMmfMetaFree(mf,which)
METAFRAME *mf;
u_i4 which;
{
	register i4	i;
	STATUS		rc;
	i4		tagidx;
	for (i=0; i < sizeof(Utab)/sizeof(Utab[0]); ++i)
	{
		if ((which & Utab[i].bit) == 0)
			continue;
		/* if the tag is 0.. then assume caller has been managing 
		   his own memory */
		if ( (mf->ptag)[i] != 0 )
		{
			tagidx = Utab[i].tagidx;
			IIUGtagFree((mf->ptag)[tagidx]);
			(mf->ptag)[tagidx] = 0;
		}
		mf->popmask &= ~(Utab[i].bit);
		mf->updmask &= ~(Utab[i].bit);
	}
	return (OK);
}
/*{
** Name - IIAMwmdWriteMenuDep
**
** Description:
**	Write the menu dependencies for a metaframe.
**	Optionally clear the old ones first.  This is an option
**	because when an entire user frame is written the operation
**	would be an unneccesary DB access.  If the frame-flow diagram
**	needs to update only the menu dependencies, it may do so
**	most efficiently by making this call with clear = TRUE, and
**	not updating the actual user frame.
**
**	The clear flag is also overloaded to mean that cat write on/off
**	has to be called, thereby matching the usage given above without
**	requiring callers to set it.
**
** Inputs:
**	mf - metaframe.
**	clear - TRUE if old ones to be removed first
**
** Returns:
**	STATUS
**
** History:
**	5/89 (bobm)	written
*/
STATUS
IIAMwmdWriteMenuDep(mf,clear)
METAFRAME *mf;
bool clear;
{
	register MFMENU **menu;
	STATUS		rval = OK;
	OOID	id;
	i4	i;
	OO_OBJECT *objptr = (OO_OBJECT *)mf->apobj;
	id = objptr->ooid;
	if (clear)
	{
		iiuicw1_CatWriteOn();
		rval = IIAMdpDeleteDeps(objptr, OC_DTMREF, OC_DTMREF,OC_DTMREF);
	}
	menu = mf->menus;
	for (i=0; rval == OK && i < mf->nummenus; ++i,++menu)
	{
		register APPL_COMP *apobj = (APPL_COMP *)((*menu)->apobj);
		rval = iiamAddDep(objptr, apobj->appl->ooid, apobj->name,
			apobj->class, OC_DTMREF, (char *)NULL, (*menu)->text,i);
	}
	if (clear)
		iiuicw0_CatWriteOff();
	return rval;
}
/*{
** Name - IIAMmtcMenuTextChange
**
** Description:
**	Change the menu text for a metaframe.  We can do this via a few
**	updates, rather than deleting and adding a mess of records.
**
** Inputs:
**	id - frame id.
**	old - old menu text.
**	new - new menu text.
**
** Returns:
**	STATUS
**
** History:
**	4/90 (bobm)	written
*/
STATUS
IIAMmtcMenuTextChange(id,old,new)
  i4 id;
  char *old;
  char *new;
{
	STATUS rval;
	iiuicw1_CatWriteOn();
	IIUIbeginXaction();
/* # line 546 "iammpop.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop1",6607,18139);
      IIputdomio((short *)0,1,32,0,new);
      IIputdomio((short *)0,1,32,0,old);
      IIputdomio((short *)0,1,30,4,&id);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop1",6607,18139);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_abfdependencies set abf_linkname=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,new);
        IIwritio(0,(short *)0,1,32,0,(char *)"where abf_linkname=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,old);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"iammpop1",6607,18139);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 550 "iammpop.sc" */	/* host code */
	rval = FEinqerr();
	if (rval == OK)
	{
/* # line 554 "iammpop.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop2",6608,19706);
      IIputdomio((short *)0,1,32,0,new);
      IIputdomio((short *)0,1,32,0,old);
      IIputdomio((short *)0,1,30,4,&id);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop2",6608,19706);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_menuargs set mu_text=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,new);
        IIwritio(0,(short *)0,1,32,0,(char *)"where mu_text=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,old);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"iammpop2",6608,19706);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 558 "iammpop.sc" */	/* host code */
		rval = FEinqerr();
	}
	if (rval != OK)
		IIUIabortXaction();
	else
		IIUIendXaction();
	iiuicw0_CatWriteOff();
	return rval;
}
/*{
** Name - IIAMmtdMenuTextDel
**
** Description:
**	For a metaframe which is removed or destroyed, remove from
**	ii_menuargs the entries which correspond to the given 
**	parent id & menu text.
**
** Inputs:
**	id - parent frame id.
**	mnuitm - old menu text.
**
** Returns:
**	STATUS
**
** History:
**	15-nov-93 (connie)	written
*/
STATUS
IIAMmtdMenuTextDel(id,mnuitm)
  i4 id;
  char *mnuitm;
{
	STATUS rval;
	iiuicw1_CatWriteOn();
	IIUIbeginXaction();
/* # line 604 "iammpop.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop3",6609,17403);
      IIputdomio((short *)0,1,30,4,&id);
      IIputdomio((short *)0,1,32,0,mnuitm);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop3",6609,17403);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_menuargs where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIwritio(0,(short *)0,1,32,0,(char *)"and mu_text=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,mnuitm);
        IIexDefine(0,(char *)"iammpop3",6609,17403);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 606 "iammpop.sc" */	/* host code */
	rval = FEinqerr();
	if (rval != OK)
		IIUIabortXaction();
	else
		IIUIendXaction();
	iiuicw0_CatWriteOff();
	return rval;
}
static VOID
mf_vqall(mf, tag)
METAFRAME *mf;
u_i4 tag;
{
	mf->tabs = (MFTAB **) loc_alloc(tag, sizeof(MFTAB *)*MF_MAXTABS, NULL);
	mf->joins = (MFJOIN **)
			loc_alloc(tag, sizeof(MFJOIN *)*MF_MAXJOINS, NULL);
}
static STATUS
mf_vqpop(mf,tag)
METAFRAME *mf;
u_i4 tag;
{
	STATUS rc;
	if ((rc = mf_vqpjoins(mf,tag)) == OK)
		rc = mf_vqptabs(mf,tag);
	return (rc);
}
static STATUS
mf_vqpjoins(mf,tag)
METAFRAME *mf;
u_i4 tag;
{
  OOID ooid;
  i4 jtype, vqseq;
  i4 jtab1, jtab2, jcol1, jcol2;
	register MFJOIN *jptr;
	ooid = (mf->apobj)->ooid;
	mf->numjoins = 0;
/* # line 657 "iammpop.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop4",6610,17368);
      IIputdomio((short *)0,1,30,4,&ooid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop4",6610,17368);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select join_type, join_tab1, join_tab2, join_col1, join_col2, vq_seq \
from ii_vqjoins where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&ooid);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by vq_seq");
        IIexDefine(0,(char *)"iammpop4",6610,17368);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&jtype);
      IIgetdomio((short *)0,1,30,4,&jtab1);
      IIgetdomio((short *)0,1,30,4,&jtab2);
      IIgetdomio((short *)0,1,30,4,&jcol1);
      IIgetdomio((short *)0,1,30,4,&jcol2);
      IIgetdomio((short *)0,1,30,4,&vqseq);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 665 "iammpop.sc" */	/* host code */
	{
		if (mf->numjoins >= MF_MAXJOINS)
		{
/* # line 668 "iammpop.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
/* # line 669 "iammpop.sc" */	/* host code */
		}
		jptr = (MFJOIN *) loc_alloc(tag,sizeof(MFJOIN),NULL);
		(mf->joins)[mf->numjoins++] = jptr;
		jptr->type = jtype;
		jptr->tab_1 = jtab1;
		jptr->tab_2 = jtab2;
		jptr->col_1 = jcol1;
		jptr->col_2 = jcol2;
		++jptr;
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	return ( FEinqerr() != OK ) ? ILE_FAIL : OK;
}
static STATUS
mf_vqptabs(mf,tag)
METAFRAME *mf;
u_i4	tag;
{
  OOID ooid;
  char tabname[FE_MAXNAME+1];
  char ownname[FE_MAXNAME+1];
  char owndottab[FE_MAXTABNAME+1];
  char colname[FE_MAXNAME+1];
  char refname[FE_MAXNAME+1];
  char colinfo[241];
  i4 tabsect, tabuse, tabflags;
  i4 adftype;
  i4 adflength;
  i4 adfscale;
  i4 colflags, colsort;
  i4 vqmode;
  i4 skey1, skey2;
	register MFTAB *tptr;
	register MFCOL *cptr;
	i4 curtab;
	ooid = (mf->apobj)->ooid;
	curtab = -1;
	mf->numtabs = 0;
/* # line 715 "iammpop.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop5",6611,17911);
      IIputdomio((short *)0,1,30,4,&ooid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop5",6611,17911);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select tab_name, tab_owner, tab_section, tab_usage, tab_flags, col_na\
me, ref_name, adf_type, adf_length, adf_scale, col_flags, col_sortorde\
r, col_info, vq_mode, ii_vqtables.vq_seq, ii_vqtabcols.vq_seq from ii_\
vqtables, ii_vqtabcols where ii_vqtables.");
        IIwritio(0,(short *)0,1,32,0,(char *)"object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&ooid);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and ii_vqtables.object_id=ii_vqtabcols.object_id and ii_vqtables.vq_s\
eq=ii_vqtabcols.tvq_seq order by ii_vqtables.vq_seq, ii_vqtabcols.vq_s\
eq");
        IIexDefine(0,(char *)"iammpop5",6611,17911);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,tabname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,ownname);
      IIgetdomio((short *)0,1,30,4,&tabsect);
      IIgetdomio((short *)0,1,30,4,&tabuse);
      IIgetdomio((short *)0,1,30,4,&tabflags);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,colname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,refname);
      IIgetdomio((short *)0,1,30,4,&adftype);
      IIgetdomio((short *)0,1,30,4,&adflength);
      IIgetdomio((short *)0,1,30,4,&adfscale);
      IIgetdomio((short *)0,1,30,4,&colflags);
      IIgetdomio((short *)0,1,30,4,&colsort);
      IIgetdomio((short *)0,1,32,240,colinfo);
      IIgetdomio((short *)0,1,30,4,&vqmode);
      IIgetdomio((short *)0,1,30,4,&skey1);
      IIgetdomio((short *)0,1,30,4,&skey2);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 732 "iammpop.sc" */	/* host code */
	{
		if (mf->numtabs == 0)
		{
			/* 
			** User qualification for masters in a tablefield wasn't
			** supported in 6.3/03, but the bit indicating that 
			** it's disabled wasn't set.  For backward compatibility
			** with VQ's created in 6.3/03, we'll treat the database
			** MF_NOQUAL bit as reversed from the in-memory copy
			** of the bit, if we have masters in a tablefield.
			** This reversal needs to happen twice: here, where
			** we read the database, and in mf_vqutabs, where we
			** write it.
			**
			** Also, such frames implicitly have MF_NONEXT set.
			*/
			mf->mode = vqmode;
			if ((vqmode & MFMODE) == MF_MASTAB)
			{
				mf->mode ^= MF_NOQUAL;
				mf->mode |= MF_NONEXT;
			}
			/* Fix to bug 82370 : Make sure that the MF_NOQUAL
			   bit is NOT set for Append frame
			   Note : No call to IIVQotObjectType for getting the 
			   frame type since it won't build with shared lib.
			   */
                        if (mf->apobj->class == OC_APPFRAME &&
                            (mf->mode & MF_NOQUAL))
			  mf->mode ^= MF_NOQUAL;
		}
		if (skey1 != curtab)
		{
			curtab = skey1;
			if (mf->numtabs >= MF_MAXTABS)
			{
/* # line 771 "iammpop.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
      }
/* # line 772 "iammpop.sc" */	/* host code */
			}
			tptr = (MFTAB *) loc_alloc(tag,sizeof(MFTAB),NULL);
			mf->tabs[mf->numtabs++] = tptr;
			IIUIxrt_tbl(ownname, tabname, owndottab);
			tptr->name = (char *) loc_alloc(tag,0,owndottab);
			tptr->tabsect = tabsect;
			tptr->usage = tabuse;
			tptr->flags = tabflags;
			tptr->numcols = 0;
			tptr->cols = (MFCOL **) loc_alloc(tag,
					sizeof(MFCOL *)*DB_GW2_MAX_COLS,NULL);
		}
		if (tptr->numcols < DB_GW2_MAX_COLS)
		{
			cptr = (MFCOL *) loc_alloc(tag,sizeof(MFCOL),NULL);
			(tptr->cols)[tptr->numcols++] = cptr;
			cptr->name = (char *) loc_alloc(tag,0,colname);
			cptr->alias = (char *) loc_alloc(tag,0,refname);
			cptr->type.db_datatype = adftype;
			cptr->type.db_length = adflength;
			cptr->type.db_prec = adfscale;
			cptr->flags = colflags;
			cptr->corder = colsort;
			cptr->info = (char *) loc_alloc(tag,0,colinfo);
		}
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
	if ( FEinqerr() != OK )
		return (ILE_FAIL);
	return (OK);
}
static STATUS
mf_vqupd(mf,id)
METAFRAME	*mf;
OOID		id;
{
	STATUS rc;
	if ((rc = mf_vqujoins(mf,id)) == OK)
		rc = mf_vqutabs(mf,id);
	return (rc);
}
static STATUS
mf_vqujoins(mf,id)
  METAFRAME *mf;
  OOID id;
{
  i4 i;
  register MFJOIN *mfj;
/* # line 831 "iammpop.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop6",6612,22008);
      IIputdomio((short *)0,1,30,4,&id);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop6",6612,22008);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_vqjoins where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"iammpop6",6612,22008);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 833 "iammpop.sc" */	/* host code */
	if ( FEinqerr() != OK )
		return (ILE_FAIL);
	for ( i = mf->numjoins ; --i >= 0 ; )
	{
		mfj = (mf->joins)[i];
/* # line 840 "iammpop.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop7",6613,23255);
      IIputdomio((short *)0,1,30,4,&id);
      IIputdomio((short *)0,1,30,4,&i);
      IIputdomio((short *)0,1,30,4,&mfj->type);
      IIputdomio((short *)0,1,30,4,&mfj->tab_1);
      IIputdomio((short *)0,1,30,4,&mfj->tab_2);
      IIputdomio((short *)0,1,30,4,&mfj->col_1);
      IIputdomio((short *)0,1,30,4,&mfj->col_2);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop7",6613,23255);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_vqjoins(object_id, vq_seq, join_type, join_tab1, join_\
tab2, join_col1, join_col2)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&mfj->type);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&mfj->tab_1);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&mfj->tab_2);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&mfj->col_1);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&mfj->col_2);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iammpop7",6613,23255);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 852 "iammpop.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return (ILE_FAIL);
	}
	return (OK);
}
static STATUS
mf_vqutabs(mf,id)
  METAFRAME *mf;
  OOID id;
{
	STATUS rc;
  i4 i;
  register MFTAB *mft;
  i4 vqmode;
  char tmpname[FE_MAXNAME+1];
  char tmpown[FE_MAXNAME+1];
/* # line 875 "iammpop.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop8",6614,23287);
      IIputdomio((short *)0,1,30,4,&id);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop8",6614,23287);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_vqtables where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"iammpop8",6614,23287);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 877 "iammpop.sc" */	/* host code */
	if ( FEinqerr() != OK )
		return (ILE_FAIL);
/* # line 880 "iammpop.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop9",6615,18404);
      IIputdomio((short *)0,1,30,4,&id);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop9",6615,18404);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_vqtabcols where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"iammpop9",6615,18404);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 882 "iammpop.sc" */	/* host code */
	if ( FEinqerr() != OK )
		return (ILE_FAIL);
	for ( i = mf->numtabs ; --i >= 0 ; )
	{
		FE_RSLV_NAME tmprslv;
		tmpname[0] = EOS;
		tmpown[0] = EOS;
		mft = (mf->tabs)[i];
		tmprslv.name = mft->name;
		tmprslv.is_nrml = TRUE;
		tmprslv.owner_dest = tmpown;
		tmprslv.name_dest = tmpname;
		FE_decompose(&tmprslv);
		/* 
		** See note in mf_vqptabs above about masters in a tablefield 
		** and user qualifiaction processing.
		*/
		vqmode = mf->mode;
		if ((vqmode & MFMODE) == MF_MASTAB)
			vqmode ^= MF_NOQUAL;
/* # line 908 "iammpop.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop10",6616,10994);
      IIputdomio((short *)0,1,30,4,&id);
      IIputdomio((short *)0,1,30,4,&i);
      IIputdomio((short *)0,1,30,4,&vqmode);
      IIputdomio((short *)0,1,32,0,tmpname);
      IIputdomio((short *)0,1,30,4,&mft->tabsect);
      IIputdomio((short *)0,1,30,4,&mft->usage);
      IIputdomio((short *)0,1,30,4,&mft->flags);
      IIputdomio((short *)0,1,32,0,tmpown);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop10",6616,10994);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_vqtables(object_id, vq_seq, vq_mode, tab_name, tab_sec\
tion, tab_usage, tab_flags, tab_owner)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&vqmode);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,tmpname);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&mft->tabsect);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&mft->usage);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&mft->flags);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,tmpown);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iammpop10",6616,10994);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 922 "iammpop.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return (ILE_FAIL);
		rc = mf_vqucols(mft,i,id);
		if (rc != OK)
			return (rc);
	}
	return (OK);
}
static STATUS
mf_vqucols(tab,tseq,id)
  MFTAB *tab;
  i4 tseq;
  OOID id;
{
  i4 i;
  i4 dtype;
  i4 dlength;
  i4 dscale;
  register MFCOL *mfc;
	for ( i = tab->numcols ; --i >= 0 ; )
	{
		mfc = (tab->cols)[i];
		dtype = mfc->type.db_datatype;
		dlength = mfc->type.db_length;
		dscale = mfc->type.db_prec;
/* # line 957 "iammpop.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop11",6617,11249);
      IIputdomio((short *)0,1,30,4,&id);
      IIputdomio((short *)0,1,30,4,&i);
      IIputdomio((short *)0,1,30,4,&tseq);
      IIputdomio((short *)0,1,32,0,mfc->name);
      IIputdomio((short *)0,1,32,0,mfc->alias);
      IIputdomio((short *)0,1,30,4,&dtype);
      IIputdomio((short *)0,1,30,4,&dlength);
      IIputdomio((short *)0,1,30,4,&dscale);
      IIputdomio((short *)0,1,30,4,&mfc->flags);
      IIputdomio((short *)0,1,30,4,&mfc->corder);
      IIputdomio((short *)0,1,32,0,mfc->info);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop11",6617,11249);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_vqtabcols(object_id, vq_seq, tvq_seq, col_name, ref_na\
me, adf_type, adf_length, adf_scale, col_flags, col_sortorder, col_inf\
o)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&tseq);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,mfc->name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,mfc->alias);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&dtype);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&dlength);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,30,4,&dscale);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,30,4,&mfc->flags);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,30,4,&mfc->corder);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $10=");
        IIputdomio((short *)0,1,32,0,mfc->info);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iammpop11",6617,11249);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 972 "iammpop.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return (ILE_FAIL);
	}
	return (OK);
}
static VOID
mf_aall(mf,tag)
METAFRAME *mf;
u_i4 tag;
{
	i4 i;
	for (i = 0;  i < mf->nummenus ; ++i)
		((mf->menus)[i])->args = (MFCFARG **)
			 loc_alloc(tag, sizeof(MFCFARG *)*MF_MAXARGS, NULL);
}
static STATUS
mf_apop(mf,tag)
METAFRAME *mf;
u_i4 tag;
{
  OOID ooid;
  char a_mtext[FE_MAXNAME+1];
  char a_fld[2*FE_MAXNAME+2];
  char a_col[FE_MAXNAME+1];
  char a_exp[241];
  i4 a_idx;
	i4 i;
	MFCFARG	*cfarg;
	MFMENU	*cmenu;
	ooid = (mf->apobj)->ooid;
	for ( i = mf->nummenus ; --i >= 0 ; )
	{
		cmenu = (mf->menus)[i];
		cmenu->numargs = 0;
	}
	cmenu = NULL;
/* # line 1020 "iammpop.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop12",6618,24274);
      IIputdomio((short *)0,1,30,4,&ooid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop12",6618,24274);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select mu_text, mu_field, mu_column, mu_expr, mu_seq from ii_menuargs\
 where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&ooid);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by mu_text, mu_seq");
        IIexDefine(0,(char *)"iammpop12",6618,24274);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,a_mtext);
      IIgetdomio((short *)0,1,32,2*FE_MAXNAME+2-1,a_fld);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,a_col);
      IIgetdomio((short *)0,1,32,240,a_exp);
      IIgetdomio((short *)0,1,30,4,&a_idx);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 1028 "iammpop.sc" */	/* host code */
	{
		if ( cmenu == NULL || ! STequal(cmenu->text,a_mtext))
		{
			cmenu = NULL;
			for ( i = mf->nummenus ; --i >= 0 ; )
			{
				if (STequal(((mf->menus)[i])->text,a_mtext))
				{
					cmenu = (mf->menus)[i];
					cmenu->numargs = 0;
					cmenu->args =
						(MFCFARG **) loc_alloc(tag,
						MF_MAXARGS*sizeof(MFCFARG *),
						NULL);
					break;
				}
			}
		}
		if (cmenu != NULL && cmenu->numargs < MF_MAXARGS)
		{
			cfarg = (MFCFARG *) loc_alloc(tag,sizeof(MFCFARG),NULL);
			(cmenu->args)[cmenu->numargs++] = cfarg;
			if (a_col[0] != EOS)
				STprintf(a_fld+STlength(a_fld),
					 ERx(".%s"), a_col);
			cfarg->var = loc_alloc(tag,0,a_fld);
			cfarg->expr = loc_alloc(tag,0,a_exp);
		}
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
	if ( FEinqerr() != OK )
		return (ILE_FAIL);
	return (OK);
}
static STATUS
mf_aupd(mf,id)
METAFRAME *mf;
  OOID id;
{
	register i4	i;
  MFMENU *menu;
  i4 j;
  MFCFARG *arg;
  char *col;
  char *dot;
/* # line 1081 "iammpop.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop13",6619,20706);
      IIputdomio((short *)0,1,30,4,&id);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop13",6619,20706);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_menuargs where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"iammpop13",6619,20706);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 1083 "iammpop.sc" */	/* host code */
	if ( FEinqerr() != OK )
		return (ILE_FAIL);
	for (i=0; i < mf->nummenus; ++i)
	{
		menu = (mf->menus)[i];
		for (j=0; j < menu->numargs; ++j)
		{
			arg = (menu->args)[j];
			dot = STindex(arg->var, ".", 0);
			if (dot != NULL)
			{
				*dot = EOS;
				col = dot+1;
			}
			else
				col = ERx("");
/* # line 1101 "iammpop.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop14",6620,28369);
      IIputdomio((short *)0,1,30,4,&id);
      IIputdomio((short *)0,1,32,0,menu->text);
      IIputdomio((short *)0,1,30,4,&j);
      IIputdomio((short *)0,1,32,0,arg->var);
      IIputdomio((short *)0,1,32,0,col);
      IIputdomio((short *)0,1,32,0,arg->expr);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop14",6620,28369);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_menuargs(object_id, mu_text, mu_seq, mu_field, mu_colu\
mn, mu_expr)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,menu->text);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&j);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,arg->var);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,col);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,arg->expr);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iammpop14",6620,28369);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 1112 "iammpop.sc" */	/* host code */
			if ( FEinqerr() != OK )
				return (ILE_FAIL);
			if (dot != NULL)
				*dot = '.';
		}
	}
	return (OK);
}
static VOID
mf_fall(mf,tag)
METAFRAME *mf;
u_i4 tag;
{
	mf->vars = (MFVAR **) loc_alloc(tag, sizeof(MFVAR *)*MF_MAXVARS, NULL);
}
static STATUS
mf_fpop(mf,tag)
METAFRAME *mf;
u_i4 tag;
{
  OOID ooid;
  char fv_fld[2*FE_MAXNAME+2];
  i2 fv_idx;
  char fv_col[FE_MAXNAME+1];
  char fv_datatype[106];
  char fv_desc[241];
	i4	vtype;
	i4	start = 0;
	i4	i;
	i4	num_words;
	char	*wordarray[MAXVARWORDS];
	char	*dtype_ptr;
	register MFVAR	*var;
	ooid = (mf->apobj)->ooid;
	mf->numvars = 0;
/* # line 1157 "iammpop.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop15",6621,30702);
      IIputdomio((short *)0,1,30,4,&ooid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop15",6621,30702);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select var_field, var_column, var_comment, var_datatype, fr_seq from \
ii_framevars where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&ooid);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by fr_seq");
        IIexDefine(0,(char *)"iammpop15",6621,30702);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,2*FE_MAXNAME+2-1,fv_fld);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,fv_col);
      IIgetdomio((short *)0,1,32,240,fv_desc);
      IIgetdomio((short *)0,1,32,105,fv_datatype);
      IIgetdomio((short *)0,1,30,2,&fv_idx);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 1166 "iammpop.sc" */	/* host code */
	{
		if (mf->numvars >= MF_MAXVARS)
		{
/* # line 1169 "iammpop.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE4;
      }
/* # line 1170 "iammpop.sc" */	/* host code */
		}
		var = (MFVAR *) loc_alloc(tag,sizeof(MFVAR),NULL);
		(mf->vars)[mf->numvars++] = var;
		if (fv_col[0] != EOS)
			STprintf(fv_fld+STlength(fv_fld),ERx(".%s"),fv_col);
		/* is this entry a local variable or a local procedure? */
		num_words = MAXVARWORDS;
		STgetwords(fv_datatype, &num_words, wordarray);
		if (STequal(wordarray[0], ERx("PROCEDURE")))
		{
			/* local procedure */
			vtype = VAR_LPROC;
			start = 2;
		}
		else
		{
			/* local variable */
			vtype = VAR_LVAR;
		}
		for (i = start, dtype_ptr = fv_datatype; i < num_words; i++)
		{
			dtype_ptr += STlcopy(wordarray[i], dtype_ptr,
						STlength(wordarray[i]));
			*dtype_ptr++ = ' ';
		}
		*dtype_ptr = EOS;
		var->name = loc_alloc(tag,0,fv_fld);
		var->vartype = vtype;
		var->dtype = loc_alloc(tag,0,fv_datatype);
		var->comment = loc_alloc(tag,0,fv_desc);
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
	if ( FEinqerr() != OK )
		return (ILE_FAIL);
	return (OK);
}
static STATUS
mf_fupd(mf,id)
METAFRAME *mf;
  OOID id;
{
	char *dot;
  i4 i;
  register MFVAR *var;
  char *col_p;
  char datatype[BUFLEN];
/* # line 1227 "iammpop.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop16",6622,29631);
      IIputdomio((short *)0,1,30,4,&id);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop16",6622,29631);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_framevars where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"iammpop16",6622,29631);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 1229 "iammpop.sc" */	/* host code */
	if ( FEinqerr() != OK )
		return (ILE_FAIL);
	for ( i = mf->numvars ; --i >= 0 ; )
	{
		var = (mf->vars)[i];
		if (var->vartype == VAR_LPROC)
		{
			/* local procedure */
			STcopy ("PROCEDURE RETURNING ", datatype);
			STcat(datatype, var->dtype);
		}
		else
		{
			/* local variable */
			STcopy (var->dtype, datatype);
		}
		dot = STindex(var->name, ".", 0);
		if (dot != NULL)
		{
			*dot = EOS;
			col_p = dot+1;
		}
		else
			col_p = ERx("");
/* # line 1257 "iammpop.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iammpop17",6623,29165);
      IIputdomio((short *)0,1,30,4,&id);
      IIputdomio((short *)0,1,32,0,var->name);
      IIputdomio((short *)0,1,32,0,col_p);
      IIputdomio((short *)0,1,32,0,datatype);
      IIputdomio((short *)0,1,32,0,var->comment);
      IIputdomio((short *)0,1,30,4,&i);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iammpop17",6623,29165);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_framevars(object_id, var_field, var_column, var_dataty\
pe, var_comment, fr_seq)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,var->name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,col_p);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,datatype);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,var->comment);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iammpop17",6623,29165);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 1268 "iammpop.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return (ILE_FAIL);
		if (dot != NULL)
			*dot = '.';
	}
	return (OK);
}
static VOID
mf_eall(mf,tag)
METAFRAME *mf;
u_i4 tag;
{
	mf->escs = (MFESC **) loc_alloc(tag, sizeof(MFESC *)*MF_MAXESCS, NULL);
}
static STATUS
mf_epop(mf,tag)
register METAFRAME *mf;
u_i4 tag;
{
	STATUS rc;
	ESCDEC estr;
	rc = oo_decode(Escdec, sizeof(Escdec)/sizeof(Escdec[0]),
						(mf->apobj)->ooid, tag, &estr);
	if (rc == ILE_FIDBAD)
	{
		mf->numescs = 0;
		rc = OK;
	}
	else if ( rc == OK )
	{
		register i4	i;
		mf->numescs = estr.numescs;
		for ( i = estr.numescs ; --i >= 0 ; )
			(mf->escs)[i] = estr.esc + i;
	}
	return rc;
}
static STATUS
mf_eupd(mf,id)
METAFRAME *mf;
OOID	id;
{
	ESCENC estr;
	STATUS rc;
	estr.version = 0;
	estr.numescs = mf->numescs;
	estr.esc = mf->escs;
	rc = oo_encode(Escenc,
		sizeof(Escenc)/sizeof(Escenc[0]), id, (PTR) &estr);
	return (rc);
}
static PTR
esc_alloc ( tag, nbytes )
u_i4	tag;
i4	nbytes;
{
	return (loc_alloc(tag, nbytes, NULL));
}
static i4
esc_dec ( addr_ref, num, state )
MFESC		**addr_ref;
i4		num;
register i4	*state;
{
	register MFESC	*addr = *addr_ref;
	TOKEN tok;
	/* if first call, initialize state */
	if (num != 0)
		*state = 0;
	for (;;)
	{
		_VOID_ tok_next(&tok);
		switch (tok.type)
		{
		case TOK_eos:
			*addr_ref = addr;
			return(1);
		case TOK_end:
			if ( *state != 0 )
				return(-1);
			return(0);
		default:
			break;
		}
		switch ( *state )
		{
		case 0:
			switch(tok.type)
			{
			case TOK_i2:
				addr->type = tok.dat.i2val;
				break;
			case TOK_i4:
				addr->type = tok.dat.i4val;
				break;
			default:
				return (-1);
			}
			break;
		case 1:
			if (tok.type != TOK_str)
				return(-1);
			addr->item = tok.dat.str;
			break;
		case 2:
			if (tok.type != TOK_str)
				return(-1);
			addr->text = tok.dat.str;
			++addr;
			break;
		}
		*state = ( *state + 1 ) % 3;
	} /* end for ever */
}
/*
** esc_enc is passed an array of POINTERS, not an array of actual structures
*/
static i4
esc_enc(addr,num)
register MFESC	**addr;
register i4	num;
{
	for ( ; --num >= 0 ; ++addr )
	{
		if (db_printf("i%d",(*addr)->type) != OK)
			return (FAIL);
		if (bs_puts((*addr)->item) != OK)
			return (FAIL);
		if (bs_puts((*addr)->text) != OK)
			return (FAIL);
	}
	return OK;
}
/*
** NOTE: these routines were snitched from iameprc.  We don't surface
**	the symbols in that file and use them to avoid unwanted modules
**	being linked.  We should pull these two routines out to a separate
**	source for common use.  They have been modified here to place
**	the '"' explicitly on the string rather than this being done
**	by the caller, so they aren't quite identical, either.
**
** put out string with backslash treatment.  Must preserve string.
** backslashes themselves, and doublequotes are backslashed.
** bs_index saves making 2 STindex calls.
*/
static char *
bs_index(s)
register char	*s;
{
	if (s != NULL)
		for ( ; *s != EOS ; ++s )
			if (*s == '"' || *s == '\\')
				return s;
	return (NULL);
}
static STATUS
bs_puts(s)
char *s;
{
	register char	*out;
	char		hold;
	if (db_puts(ERx("\"")) != OK)
		return (FAIL);
	for (out = bs_index(s); out != NULL; out = bs_index(s = out+1))
	{
		hold = *out;
		*out = '\0';
		if (db_puts(s) != OK)
			return (FAIL);
		if (db_printf(ERx("%c%c"),'\\',hold) != OK)
			return (FAIL);
		*out = hold;
	}
	return ( db_puts(s) != OK || db_puts(ERx("\"")) != OK ) ? FAIL : OK;
}
