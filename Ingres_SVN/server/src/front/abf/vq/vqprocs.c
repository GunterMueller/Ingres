# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<cm.h>
# include	<cv.h>
# include	<st.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <abclass.h>
# include	<adf.h>
# include	<ft.h>
# include	<fmt.h>
# include	<frame.h>
# include	<runtime.h>
# include <oocat.h>
# include	<ooclass.h>
# include	<stdprmpt.h>
# include	<afe.h>
# include	<ug.h>
# include	<ui.h>
# include	<uf.h>
# include	<metafrm.h>
# include	"vqescinf.h"
# include	"ervq.h"
/**
** Name:	vqlocals.qc -	User interface for local variable handling
**
** Description:
**	This file has routines to display existing local procedures for the
**	current frame, allows the user to create, delete, rename and edit
**	local procedures and saves the procedures back into the database.
**
**	This file defines:
**
**	IIVQlprocs		Displays and edits local variables
**	_create_edit_proc	FRS code for create/edit popup
**	_check_ing_type		Determines if datatype is derived or DBDV
**	_fe_name_check		Checks char string for a valid INGRES name
**	_save_proc		Saves local procedure
**	_check_unique		Checks table field for already existing name
**	loc_alloc		String allocation routine
**
** History:
**	11-aug-92 (blaise)
**		Initial version
**	21-apr-93 (blaise)
**		Added error message for invalid return types, and code to
**		treat "none" as a valid return type. (bug #49719)
**	24-may-93 (blaise)
**		S_VQ00D7_No_Procs... has been renamed to S_VQ00D8... due to
**		error number clash with 64 codeline.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**    25-Oct-2005 (hanje04)
**        Add prototypes for _check_unique(), _save_proc(), _fe_name_check(),
**	  _check_ing_type(), find_source(), rename_source() & edit_source()
**	  to prevent compiler errors with GCC 4.0 due to conflict with
**	  implicit declaration.
**	11-sep-2006 (gupsh01)
**	    Added support for ANSI date/time types.
**/
# define _yes	1
# define _no	0
# define _unset -1
# define _localp_prmpt		(ERget(F_VQ0145_localp_prmpt))
# define STGETWORDS_MAX		53
# define MAX_DATATYPE_STRING	105
/* GLOBALDEF's */
GLOBALREF APPL *IIVQappl; /* argument of frame flow entry, the current
			   application we are editing in abf */
GLOBALREF char *IIabExename;	/* Executable name */
/* extern's */
FUNC_EXTERN VOID	IIVQgeGlobalEdit();
FUNC_EXTERN STATUS	IIAMmpMetaPopulate();
FUNC_EXTERN ADF_CB	*FEadfcb();
/* static's */
static PTR	loc_alloc();
static i4 _create_edit_proc();
static i4 edit_source();
static i4 rename_source();
static i4 find_source();
static STATUS _check_ing_type();
static STATUS _fe_name_check();
static i4 _save_proc();
static STATUS _check_unique();
static READONLY
  char vqprocs_form[]= ERx("vqprocs"), vqprocs_table[]= ERx("procs"), 
  vqcreate_form[]= ERx("vqproccreate"), _return_type[]= ERx("return_type");
  static char *_n_a;
static bool	first_call = TRUE;
static bool	save_vars = FALSE;
static bool	save_source = FALSE;
static ADF_CB	*adf_cb;
static READONLY
	DB_DT_ID	_types[] = { DB_INT_TYPE, DB_FLT_TYPE, DB_DEC_TYPE,
				     DB_CHA_TYPE, DB_VCH_TYPE, DB_TXT_TYPE,
				     DB_CHR_TYPE, DB_DTE_TYPE, DB_MNY_TYPE,
				     DB_ADTE_TYPE, DB_TMWO_TYPE, DB_TMW_TYPE,
				     DB_TME_TYPE, DB_TSWO_TYPE, DB_TSW_TYPE,
				     DB_TSTMP_TYPE, DB_INDS_TYPE, DB_INYM_TYPE,
				     DB_NODT
};
/*{
** Name:	IIVQlprocs() -  Visual Query Frame Interface to Local Variables.
**
** Description:
**	This routine implements the visual query frame local variable frame,
**	which allows local variables to be created, edited, deleted and
**	renamed for the input visual query frame.
**
** Inputs:
**	OO_OBJECT *apobj;	- ptr to user frame to define/edit variables
**
** History:
**	11-aug-92 (blaise)
**		Initial Version
*/
VOID
  IIVQlprocs(apobj)
  OO_OBJECT *apobj;
  {
	METAFRAME	*pmf;
	MFVAR		**frame_vars;
	i4		vartotal;
	i4		lproccount = 0;
	i4		lvarcount = 0;
	i4		i;
	TAGID		tag;
    char fe_name[FE_MAXNAME + 1];
    char db_temp[MAX_DATATYPE_STRING + 1];
    char remarks[OOSHORTREMSIZE+1];
	ADF_CB	*FEadfcb();
	/* only allow local variable editing for standard Visual Query frames */
	switch ( (*apobj).class )
	{
	  case OC_MUFRAME:
	  case OC_APPFRAME:
	  case OC_UPDFRAME:
	  case OC_BRWFRAME:
		break;
	  case OC_HLPROC:
	  case OC_QBFFRAME:
	  case OC_OSLFRAME:
	  case OC_RWFRAME:
	  case OC_OSLPROC:
	  case OC_DBPROC:
	  case OC_GRFRAME:
	  case OC_GBFFRAME:
	  default:
		IIUGerr(E_VQ014B_No_Procedures_Allowed, UG_ERR_ERROR, 0);
		return;
	} /* end switch */
	save_vars = FALSE;
	pmf = ((USER_FRAME *)apobj)->mf;
	/* initialize forms used for local procedures */
	if (first_call)
	{
		first_call = FALSE;
		IIARiaForm(vqprocs_form);
		IIARiaForm(vqcreate_form);
/* # line 194 "vqprocs.qsc" */	/* inittable */
    {
      if (IItbinit(vqprocs_form,vqprocs_table,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 195 "vqprocs.qsc" */	/* host code */
		_n_a = ERget(F_VQ010D_n_a);
	}
	if (IIAMmpMetaPopulate(pmf, MF_P_FLD) != OK)
	{
		return;
	}
	frame_vars = pmf->vars;
	tag = (pmf->ptag)[MF_PT_FLD];
	vartotal = pmf->numvars;
/* # line 207 "vqprocs.qsc" */	/* display */
    {
      if (IIdispfrm(vqprocs_form,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 210 "vqprocs.qsc" */	/* host code */
		register i4	i;
        char titl[80];
		IIUGfmt(titl, sizeof(titl), ERget(F_VQ014E_EditLocalProcs), 
			1, IIabExename);
/* # line 215 "vqprocs.qsc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio(_name,(short *)0,1,32,0,apobj->name);
            IIputfldio(_title,(short *)0,1,32,0,titl);
          } /* IIfsetio */
        }
/* # line 216 "vqprocs.qsc" */	/* host code */
		/* load up the main tablefield on the form */
		for ( i = 0 ; i < vartotal ; ++i )
        {
			register MFVAR	*cproc;
          char *pname;
          char *ptype;
          char *pcomment;
			cproc = frame_vars[i];
			/* Local variables come first in the metaframe */
			if (cproc->vartype == VAR_LVAR)
			{
				lvarcount++;
				continue;
			}
			/* else (VAR_LPROC) */
			pname = cproc->name;
			ptype = cproc->dtype;
			pcomment = cproc->comment;
			lproccount++;
/* # line 237 "vqprocs.qsc" */	/* loadtable */
          {
            if (IItbact(vqprocs_form,vqprocs_table,1) != 0) {
              IItcoputio(_name,(short *)0,1,32,0,pname);
              IItcoputio(_return_type,(short *)0,1,32,0,ptype);
              IItcoputio(_short_remark,(short *)0,1,32,0,pcomment);
              IITBceColEnd();
            } /* IItbact */
          }
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 248 "vqprocs.qsc" */	/* host code */
		_create_edit_proc(pmf, TRUE, &lproccount);
          }
        } else if (IIretval() == 2) {
          {
/* # line 253 "vqprocs.qsc" */	/* host code */
		if (lproccount == 0)
		{
			IIUGerr(S_VQ00D8_No_Procs_in_Table, UG_ERR_ERROR, 0);
		}
		else
		{
/* # line 259 "vqprocs.qsc" */	/* getrow */
            {
              if (IItbsetio(2,vqprocs_form,vqprocs_table,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,fe_name,_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 261 "vqprocs.qsc" */	/* host code */
			/*
			** if there's any source code for this procedure, the
			** user must destroy it before the procedure
			** declaration can be destroyed.
			*/
			if (find_source(pmf, fe_name) >= 0)
			{
				IIUGerr(E_VQ00E6_Source_Exists, UG_ERR_ERROR,
									0);
			}
			else	/* destroy the procedure */
			{
				if (IIUFccConfirmChoice (CONF_DESTROY, fe_name,
					ERget(F_VQ014F_Local_Procedure),
					ERget(F_VQ0136_Confirm_LV_Destroy),
					ERx("vqlvdest.hlp")) == CONFCH_YES)
				{
/* # line 278 "vqprocs.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,vqprocs_form,vqprocs_table,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 279 "vqprocs.qsc" */	/* host code */
					--lproccount;
					save_vars = TRUE;
				}
			}
	    	}
          }
        } else if (IIretval() == 3) {
          {
/* # line 289 "vqprocs.qsc" */	/* host code */
		if (lproccount == 0)
		{
			IIUGerr(S_VQ00D8_No_Procs_in_Table, UG_ERR_ERROR, 0);
/* # line 292 "vqprocs.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 293 "vqprocs.qsc" */	/* host code */
		}
		_create_edit_proc(pmf, FALSE, &lproccount);
          }
        } else if (IIretval() == 4) {
          {
/* # line 299 "vqprocs.qsc" */	/* host code */
	    if (lproccount == 0)
	    {
		IIUGerr(S_VQ00D8_No_Procs_in_Table, UG_ERR_ERROR, 0);
	    }
	    else
	    {
		IIUFask( ERget(S_VQ0090_Enter_new_name_Prompt),
					FALSE, db_temp, 0);
		if (db_temp[0] != EOS)
            {
              i4 item;
              char *strptr;
              char db_type[MAX_DATATYPE_STRING + 1];
              char db_temp_copy[MAX_DATATYPE_STRING + 1];
			CVlower(db_temp);
			STcopy(db_temp, db_temp_copy);
			/*
			** If there isn't a "." in the name, check the name
			** is valid
			*/
			if ((strptr = STindex(db_temp_copy, ERx("."), 0)) ==
									NULL)
			{
				if ((_fe_name_check(db_temp) != OK)
					|| (_check_unique(pmf, db_temp) != OK))
				{
/* # line 327 "vqprocs.qsc" */	/* resume */
              {
                if (1) goto IIfdB1;
              }
/* # line 328 "vqprocs.qsc" */	/* host code */
				}
			}
			else
			/*
			** Check each half of the name (separated by the ".")
			** individually. Bug #34835.
			*/
			{
				*strptr = EOS;
				CMnext(strptr);
				if ((_fe_name_check(db_temp_copy) != OK)
					|| (_fe_name_check(strptr) != OK)
					|| (_check_unique(pmf, db_temp) != OK))
				{
/* # line 342 "vqprocs.qsc" */	/* resume */
              {
                if (1) goto IIfdB1;
              }
/* # line 343 "vqprocs.qsc" */	/* host code */
				}
			}
/* # line 346 "vqprocs.qsc" */	/* getrow */
              {
                if (IItbsetio(2,vqprocs_form,vqprocs_table,-2) != 0) {
                  IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,fe_name,_name);
                  IItcogetio((short *)0,1,32,MAX_DATATYPE_STRING + 1-1,db_type
                  ,_return_type);
                  IItcogetio((short *)0,1,32,OOSHORTREMSIZE+1-1,remarks,
                  _short_remark);
                  IITBceColEnd();
                } /* IItbsetio */
              }
/* # line 352 "vqprocs.qsc" */	/* deleterow */
              {
                if (IItbsetio(4,vqprocs_form,vqprocs_table,-2) != 0) {
                  if (IItdelrow(0) != 0) {
                  } /* IItdelrow */
                } /* IItbsetio */
              }
/* # line 353 "vqprocs.qsc" */	/* host code */
			item = FEtbsins(vqprocs_form, vqprocs_table, db_temp,
				_name, TRUE);
/* # line 355 "vqprocs.qsc" */	/* insertrow */
              {
                if (IItbsetio(5,vqprocs_form,vqprocs_table,item) != 0) {
                  if (IItinsert() != 0) {
                    IItcoputio(_name,(short *)0,1,32,0,db_temp);
                    IItcoputio(_return_type,(short *)0,1,32,0,db_type);
                    IItcoputio(_short_remark,(short *)0,1,32,0,remarks);
                    IITBceColEnd();
                  } /* IItinsert */
                } /* IItbsetio */
              }
/* # line 360 "vqprocs.qsc" */	/* host code */
			save_vars = TRUE;
			/*
			** Now change the name of the variable in the MFESC
			** structure of the metaframe, where the source code
			** is stored.
			*/
			rename_source(pmf, fe_name, db_temp);
            }
	    }
          }
        } else if (IIretval() == 5) {
          {
/* # line 376 "vqprocs.qsc" */	/* host code */
		IIVQlocals(apobj);
          }
        } else if (IIretval() == 6) {
          {
/* # line 382 "vqprocs.qsc" */	/* host code */
		IIVQgeGlobalEdit(IIVQappl, FALSE);
          }
        } else if (IIretval() == 7) {
          {
/* # line 387 "vqprocs.qsc" */	/* host code */
		FEtabfnd(vqprocs_form, vqprocs_table);
          }
        } else if (IIretval() == 8) {
          {
/* # line 392 "vqprocs.qsc" */	/* scroll */
            {
              if (IItbsetio(1,vqprocs_form,vqprocs_table,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 9) {
          {
/* # line 397 "vqprocs.qsc" */	/* scroll */
            {
              if (IItbsetio(1,vqprocs_form,vqprocs_table,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 10) {
          {
/* # line 405 "vqprocs.qsc" */	/* host code */
		FEhelp(ERx("vqprocs.hlp"), _localp_prmpt);
          }
        } else if (IIretval() == 11) {
          {
/* # line 411 "vqprocs.qsc" */	/* host code */
	    if (save_vars)
            {
              i4 state;
		/*
		** copy local procedures back into the metaframe, after the
		** local variable entries (which obviously haven't changed)
		*/
		vartotal = lvarcount;
/* # line 420 "vqprocs.qsc" */	/* unloadtable */
              {
                if (IItbact(vqprocs_form,vqprocs_table,0) == 0) goto IItbE1;
                while (IItunload() != 0) {
                  IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,fe_name,_name);
                  IItcogetio((short *)0,1,32,MAX_DATATYPE_STRING + 1-1,db_temp
                  ,_return_type);
                  IItcogetio((short *)0,1,32,OOSHORTREMSIZE+1-1,remarks,
                  _short_remark);
                  IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
                  IITBceColEnd();
                  {
/* # line 427 "vqprocs.qsc" */	/* host code */
		    if (state != stDELETE)
		    {
			register MFVAR	*cproc;
			cproc = (MFVAR *) loc_alloc(tag, sizeof(MFVAR),NULL);
			cproc->name = loc_alloc(tag,0,fe_name);
			cproc->vartype = VAR_LPROC;
			cproc->dtype = loc_alloc(tag,0,db_temp);
			cproc->comment = loc_alloc(tag,0,remarks);
			frame_vars[vartotal++] = cproc;
		    }
                  }
                } /* IItunload */
IItbE1:
                IItunend();
              }
/* # line 439 "vqprocs.qsc" */	/* host code */
		/* set the metaframe state flag to indicate that a regen
		   is required because the local vars have changed. */
		pmf->state |= MFST_DOGEN;
		/* update the disk with the change. */
		IIAMufqUserFrameQuick(pmf->apobj);
		pmf->numvars = vartotal;
		/* update the disk with the local variables */
		IIAMmuMetaUpdate(pmf, MF_P_FLD);
            }
	    /* If there's source code which needs saving, update the disk */
	    if (save_source)
	    {
		IIAMmuMetaUpdate(pmf, MF_P_ESC);
	    }
	    IIAMmfMetaFree(pmf, MF_P_FLD);
	    IIAMmfMetaFree(pmf, MF_P_ESC);
/* # line 459 "vqprocs.qsc" */	/* clear */
            {
              IIfldclear(vqprocs_table);
            }
/* # line 460 "vqprocs.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_Create),ERget(F_VQ0146_CreateLP_expl),2,2,1) == 0
      ) goto IIfdE1;
      if (IInmuact(ERget(FE_Destroy),ERget(F_VQ0147_DeleteLP_expl),2,2,2) == 0
      ) goto IIfdE1;
      if (IInmuact(ERget(FE_Edit),ERget(F_VQ0148_EditLP_expl),2,2,3) == 0) 
      goto IIfdE1;
      if (IInmuact(ERget(FE_Rename),ERget(F_VQ010C_RenameLV_expl),2,2,4) == 0
      ) goto IIfdE1;
      if (IInmuact(ERget(F_VQ0155_LocalVars),ERget(F_VQ0156_Locals_expl),2,2,5)
       == 0) goto IIfdE1;
      if (IInmuact(ERget(F_VQ010F_Globals),ERget(F_VQ0110_Globals_expl),2,2,6)
       == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,9) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,10) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,10) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,11) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,11) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
/* # line 465 "vqprocs.qsc" */	/* host code */
/*{
** Name:	_create_edit_proc	- create/edit a local procedure
**
** Description:
**	This routine handles the popup for creating/editing local procedures.
**
** Inputs:
**	METAFRAME *mf;		- the metaframe
**	bool create_op;		- TRUE to create, FALSE to edit a variable
**	i4 *lproccount		- ptr to i4  indicating no. of local procedures
**
** Outputs:
**	lproccount is set to the updated number of local procedures
**
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	11-aug-92 (blaise)
**		Initial Version
*/
static
_create_edit_proc(mf, create_op, lproccount)
METAFRAME	*mf;
bool		create_op;
i4		*lproccount;
{
  char var_name[2 * FE_MAXNAME + 1];
  char titl[80];
  char proc_name[MAX_DATATYPE_STRING + 1];
  char old_type[MAX_DATATYPE_STRING + 1];
  char old_remark[OOSHORTREMSIZE+1];
/* # line 503 "vqprocs.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(vqcreate_form,(char *)"f") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 506 "vqprocs.qsc" */	/* clear */
      {
        IIclrflds();
      }
/* # line 507 "vqprocs.qsc" */	/* host code */
		if (create_op)
		{
			_VOID_ IIUGfmt(titl, sizeof(titl), 
				ERget(F_VQ014C_CreateProc), 1, IIabExename);
/* # line 512 "vqprocs.qsc" */	/* putform */
      {
        if (IIfsetio(vqcreate_form) != 0) {
          IIputfldio((char *)"title",(short *)0,1,32,0,titl);
        } /* IIfsetio */
      }
/* # line 513 "vqprocs.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,vqcreate_form,(char *)0, (char *)0) != 0) {
          IIstfsio(53,_name,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 514 "vqprocs.qsc" */	/* host code */
		}
		else
		{
        		_VOID_ IIUGfmt(titl, sizeof(titl),
				ERget(F_VQ014D_EditProc), 1, IIabExename);
/* # line 519 "vqprocs.qsc" */	/* putform */
      {
        if (IIfsetio(vqcreate_form) != 0) {
          IIputfldio((char *)"title",(short *)0,1,32,0,titl);
        } /* IIfsetio */
      }
/* # line 520 "vqprocs.qsc" */	/* getrow */
      {
        if (IItbsetio(2,vqprocs_form,vqprocs_table,-2) != 0) {
          IItcogetio((short *)0,1,32,MAX_DATATYPE_STRING + 1-1,proc_name,_name);
          IItcogetio((short *)0,1,32,MAX_DATATYPE_STRING + 1-1,old_type,
          _return_type);
          IItcogetio((short *)0,1,32,OOSHORTREMSIZE+1-1,old_remark,
          _short_remark);
          IITBceColEnd();
        } /* IItbsetio */
      }
/* # line 526 "vqprocs.qsc" */	/* putform */
      {
        if (IIfsetio(vqcreate_form) != 0) {
          IIputfldio(_name,(short *)0,1,32,0,proc_name);
          IIputfldio(_return_type,(short *)0,1,32,0,old_type);
          IIputfldio(_short_remark,(short *)0,1,32,0,old_remark);
        } /* IIfsetio */
      }
/* # line 531 "vqprocs.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,vqcreate_form,(char *)0, (char *)0) != 0) {
          IIstfsio(53,_name,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        } /* IIiqset */
      }
/* # line 533 "vqprocs.qsc" */	/* host code */
		}
    }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 538 "vqprocs.qsc" */	/* host code */
		char	*strptr;
		char	var_copy[2 * FE_MAXNAME + 1];
/* # line 541 "vqprocs.qsc" */	/* getform */
          {
            if (IIfsetio(vqcreate_form) != 0) {
              IIgetfldio((short *)0,1,32,2 * FE_MAXNAME + 1-1,var_name,_name);
            } /* IIfsetio */
          }
/* # line 542 "vqprocs.qsc" */	/* host code */
		STcopy(var_name, var_copy);
		if ((strptr = STindex(var_copy, ERx("."), 0)) == NULL)
		{
			if (_fe_name_check(var_name) == OK)
			{
/* # line 547 "vqprocs.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB2;
          }
/* # line 548 "vqprocs.qsc" */	/* host code */
			}
		}
		else
		{
			*strptr = EOS;
			CMnext(strptr);
			if ((_fe_name_check(var_copy) == OK)
					&& (_fe_name_check(strptr) == OK))
			{
/* # line 557 "vqprocs.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB2;
          }
/* # line 558 "vqprocs.qsc" */	/* host code */
			}
		}
/* # line 560 "vqprocs.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 565 "vqprocs.qsc" */	/* host code */
		if (_check_ing_type() == OK)
		{
/* # line 567 "vqprocs.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB2;
          }
/* # line 568 "vqprocs.qsc" */	/* host code */
		}
/* # line 569 "vqprocs.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 575 "vqprocs.qsc" */	/* getform */
          {
            if (IIfsetio(vqcreate_form) != 0) {
              IIgetfldio((short *)0,1,32,MAX_DATATYPE_STRING + 1-1,proc_name,
              _name);
            } /* IIfsetio */
          }
/* # line 576 "vqprocs.qsc" */	/* host code */
		if (proc_name[0] == EOS)
		{
			IIUGerr(E_VQ00E4_No_Proc_Name, UG_ERR_ERROR, 0);
/* # line 579 "vqprocs.qsc" */	/* resume */
          {
            IIresfld((char *)"procname");
            if (1) goto IIfdB2;
          }
/* # line 580 "vqprocs.qsc" */	/* host code */
		}
		/*
		** Validate the return type and check the uniqueness of the
		** procedure name before editing the source code
		*/
/* # line 586 "vqprocs.qsc" */	/* validate */
          {
            if (IIvalfld(_return_type) == 0) {
              goto IIfdB2;
            } /* IIvalfld */
          }
/* # line 587 "vqprocs.qsc" */	/* host code */
		if (create_op)
		{
			/* check to see if the name already exists */
/* # line 590 "vqprocs.qsc" */	/* getform */
          {
            if (IIfsetio(vqcreate_form) != 0) {
              IIgetfldio((short *)0,1,32,2 * FE_MAXNAME + 1-1,var_name,_name);
            } /* IIfsetio */
          }
/* # line 591 "vqprocs.qsc" */	/* host code */
			if (_check_unique(mf, var_name) != OK)
			{
/* # line 593 "vqprocs.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 594 "vqprocs.qsc" */	/* host code */
			}
		}
		/* Edit the procedure's source code */
		edit_source(mf, proc_name);
		/* Now save the procedure and return */
		_save_proc();
		if (create_op)
		{
			++(*lproccount);
		}
		save_vars = TRUE;
/* # line 607 "vqprocs.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 612 "vqprocs.qsc" */	/* validate */
          {
            if (IIvalfld(_return_type) == 0) {
              goto IIfdB2;
            } /* IIvalfld */
          }
/* # line 614 "vqprocs.qsc" */	/* host code */
		if (create_op)
		{
			/* check to see if the name already exists */
/* # line 617 "vqprocs.qsc" */	/* getform */
          {
            if (IIfsetio(vqcreate_form) != 0) {
              IIgetfldio((short *)0,1,32,2 * FE_MAXNAME + 1-1,var_name,_name);
            } /* IIfsetio */
          }
/* # line 618 "vqprocs.qsc" */	/* host code */
			if (_check_unique(mf, var_name) != OK)
			{
/* # line 620 "vqprocs.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 621 "vqprocs.qsc" */	/* host code */
			}
		}
		_save_proc();
		if (create_op)
		{
			++(*lproccount);
		}
		save_vars = TRUE;
/* # line 629 "vqprocs.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 636 "vqprocs.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 6) {
        {
          char fname[FE_MAXNAME+1];
/* # line 644 "vqprocs.qsc" */	/* host code */
		FLD_VALUE	fv;
		fv._form = vqcreate_form;
		fv._field = fname;
		fv._column = NULL;
/* # line 650 "vqprocs.qsc" */	/* inquire_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fname,22,vqcreate_form,0);
            } /* IIiqset */
          }
/* # line 651 "vqprocs.qsc" */	/* host code */
		if (STequal(fname, _return_type))
		{
			IIUFhlpType( _types, sizeof(_types)/sizeof(_types[0]),
					TRUE, &fv
			);
		}
		else
		{
			IIUFfldHelp(&fv);
		}
/* # line 661 "vqprocs.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB2;
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 669 "vqprocs.qsc" */	/* host code */
		if (create_op)
		{
			FEhelp(ERx("vqlpcre.hlp"), ERget(F_VQ014A_localp_create));
		}
		else
		{
			FEhelp(ERx("vqlpedit.hlp"), _localp_prmpt);
		}
        }
      } else {
        if (1) goto IIfdE2;
      } /* IIretval */
    } /* IIrunform */
IIfdF2:
    if (IIchkfrm() == 0) goto IIfdB2;
    goto IIfdE2;
IImuI2:
    if (IIFRafActFld(_name,0,1) == 0) goto IIfdE2;
    if (IIFRafActFld(_return_type,0,2) == 0) goto IIfdE2;
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_EditSource),ERget(F_VQ0154_EditSource_expl),2,2,3) 
    == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_OK),(char *)0,2,2,4) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,4) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0109_ExplForget),0,0,5) == 0) 
    goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,0,5) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,6) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,6) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,7) == 0) 
    goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,0,7) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 678 "vqprocs.qsc" */	/* host code */
}
/*{
** Name:	edit_source -	edit the source code of a local procedure.
**
** Description:
**	This routine edits the source code of a local procedure. The source
**	code is stored in the MFESC structure of the metaframe.
**
** Inputs:
**	METAFRAME *mf	- the metaframe
**	char *proc_name	- the name of the procedure
**
** Outputs:
**
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**	Updates the metaframe; allocates memory for the source code.
**
** History:
**	31-aug-92 (blaise)
**		Initial Version
*/
static
edit_source(mf, proc_name)
METAFRAME	*mf;
char		*proc_name;
{
	ESCINFO	escinfo;
	/*
	** The escape code editing routines will update the system catalogs
	** for us, so we don't need to do it when saving local procedures.
	*/
	save_source = FALSE;
	/* Load the escape code, if need be */
	if ((mf->popmask & MF_P_ESC) == 0)
		if (IIAMmpMetaPopulate(mf, MF_P_ESC) != OK)
			return FAIL;
	escinfo.metaframe = mf;
	escinfo.esctype = ESC_LOC_PROC;
	escinfo.item = proc_name;
	escinfo.line = 1;
	escinfo.flags = 0;
	escinfo.frmtag = 0;
	IIVQeecEditEscapeCode(&escinfo);
}
/*{
** Name:	rename_source -	change the name of a local procedure in the
**				MFESC structure of the metaframe.
**
** Description:
**	This routine changes the name of a local procedure in the MFESC
**	structure of the metaframe, where the source code is stored. It does
**	not change the name in the source code itself; that is up to the user.
**
** Inputs:
**	METAFRAME *mf	- the metaframe
**	char *old_name	- the original name of the procedure
**	char *new_name	- the new name of the procedure
**
** Outputs:
**
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**	Updates the metaframe; allocates memory for the new name.
**
** History:
**	31-aug-92 (blaise)
**		Initial Version
*/
static
rename_source(mf, old_name, new_name)
METAFRAME	*mf;
char		*old_name;
char		*new_name;
{
	i4	src_position;
	MFESC	**escs;
	TAGID	tag = (mf->ptag)[MF_PT_ESC];
	/* Is there any source code for this procedure? */
	if ((src_position = find_source(mf, old_name)) >= 0)
	{
		/* Warn the user that the source code needs to be edited */
		IIUGerr(E_VQ00E5_Update_Source_Warning, UG_ERR_ERROR, 0);
		/* rename the procedure in the MFESC array */
		escs = mf->escs;
		escs[src_position]->item = loc_alloc(tag, 0, new_name);
		save_source = TRUE;
	}
}
/*{
** Name:	find_source - check existence of local procedure source code
**
** Description:
**	This routine takes the name of a local procedure and checks whether
**	there is any source code for that procedure in the MFESC structure of
**	the metaframe.
**
** Inputs:
**	METAFRAME *mf	- the metaframe
**	char *proc_name	- the name of the procedure
**
** Outputs:
**
** Returns:
**	i4 - postition of the relevant source code in the MFESC array; -1
**	if no source code is found.
**
** History:
**	31-aug-92 (blaise)
**		Initial Version.
*/
static i4
find_source(mf, proc_name)
METAFRAME	*mf;
char		*proc_name;
{
	i4	i;
	i4	position = -1;
	bool	is_src = FALSE;
	MFESC   **escs;
	/* Load the escape code, if need be */
	if ((mf->popmask & MF_P_ESC) == 0)
		IIAMmpMetaPopulate(mf, MF_P_ESC);
	escs = mf->escs;
	/*
	** Go through the MFESC array to see whether there's any source code
	** for this procedure.
	*/
	for (i = 0; i < mf->numescs; i++)
	{
		if (STequal(escs[i]->item, proc_name))
		{
			position = i;
			break;
		}
	}
	return (position);
}
/*{
** Name:	_check_ing_type	- validate user entered datatype
**
** Description:
**	This routine determines if the datatype entered by the user is
**	either a valid INGRES DB_DATA_VALUE datatype (float, integer, etc.)
**	or a derived datatype (class).
**
** Inputs:
**	i4 *is_ingres		- ptr to i4  for type of datatype (derived,etc)
**
** Outputs:
**	is_ingres is set to indicate type of datatype (derived, standard, etc.)
**
**	Returns:
**		STATUS	- OK if datatype is valid, FAIL otherwise
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	11-aug-92 (blaise)
**		Initial Version
**	16-oct-2006 (gupsh01)
**		Added check for date type.
**	10-Jul-2007 (kibro01) b118702
**	    Remove IIAMdateAlias since its functionality is in adi_tyid
*/
static STATUS
_check_ing_type()
{
	i4	len;
  char db_type[MAX_DATATYPE_STRING + 1];
	char	*none;
	DB_USER_TYPE	utype;
	DB_DATA_VALUE	dbv;
  i4 changed;
        char            dtemp[11];
        char            *dp = dtemp;
/* # line 882 "vqprocs.qsc" */	/* inquire_frs */
  {
    if (IIiqset(2,0,vqcreate_form,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&changed,20,_return_type,0);
    } /* IIiqset */
  }
/* # line 883 "vqprocs.qsc" */	/* host code */
	if (changed == 0)
	{
		return(OK);
	}
/* # line 888 "vqprocs.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,vqcreate_form,(char *)0, (char *)0) != 0) {
      IIstfsio(20,_return_type,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 889 "vqprocs.qsc" */	/* getform */
  {
    if (IIfsetio(vqcreate_form) != 0) {
      IIgetfldio((short *)0,1,32,MAX_DATATYPE_STRING + 1-1,db_type,
      _return_type);
    } /* IIfsetio */
  }
/* # line 890 "vqprocs.qsc" */	/* host code */
	if ((len = STzapblank(db_type, db_type)) < DB_TYPE_SIZE)
	{
		if (len == 0)
		{
			return(FAIL);
		}
		adf_cb = FEadfcb();
		none = ERget(F_VQ0169_none);
		if (STbcompare(db_type, 0, none, 0, TRUE) == 0)
		{
			dbv.db_datatype = DB_NODT;
			return(OK);
		}
		STlcopy(db_type, utype.dbut_name, DB_TYPE_SIZE);
		if (afe_tychk(adf_cb, &utype, &dbv) == 0)
		{
/* # line 907 "vqprocs.qsc" */	/* putform */
  {
    if (IIfsetio(vqcreate_form) != 0) {
      IIputfldio(_return_type,(short *)0,1,32,0,db_type);
    } /* IIfsetio */
  }
/* # line 908 "vqprocs.qsc" */	/* host code */
			return(OK);
		}
	}
	IIUGerr(E_VQ00F4_Bad_Return_Type, UG_ERR_ERROR, 1, db_type);
	return(FAIL);
}
/*{
** Name:	_fe_name_check	- check char string for valid FE object name
**
** Description:
**	This routine determines if the passed in character string points to
**	a valid name for a frontend object.
**
** Inputs:
**	char *fe_name		- ptr to character string for FE name
**
** Outputs:
**	Returns:
**		STATUS	- OK for valid name, FAIL otherwise
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	11-aug-92 (blaise)
**		Initial Version
*/
static STATUS
_fe_name_check(fe_name)
char	*fe_name;
{
	if (STtrmwhite(fe_name) > 0 && FEchkname(fe_name) != OK)
	{
		IIUGerr(E_UG000E_BadName, UG_ERR_ERROR, 1, fe_name);
		return(FAIL);
	}
	return(OK);
}
/*{
** Name:	_save_proc	- update main table field with local procedure
**
** Description:
**	This routine updates the table field on the main Local Procedures
**	form after a procedure has been created or edited.
**
** Inputs:
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	11-aug-92 (blaise)
**		Initial Version
*/
static
_save_proc()
{
  char fname[FE_MAXNAME + 1];
  char rtype[MAX_DATATYPE_STRING + 1];
  char remarks[OOSHORTREMSIZE+1];
  i4 item;
/* # line 982 "vqprocs.qsc" */	/* getform */
  {
    if (IIfsetio(vqcreate_form) != 0) {
      IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,fname,_name);
      IIgetfldio((short *)0,1,32,MAX_DATATYPE_STRING + 1-1,rtype,_return_type);
      IIgetfldio((short *)0,1,32,OOSHORTREMSIZE+1-1,remarks,_short_remark);
    } /* IIfsetio */
  }
/* # line 987 "vqprocs.qsc" */	/* host code */
	item = FEtbsins(vqprocs_form, vqprocs_table, fname, _name, TRUE);
/* # line 988 "vqprocs.qsc" */	/* insertrow */
  {
    if (IItbsetio(5,vqprocs_form,vqprocs_table,item) != 0) {
      if (IItinsert() != 0) {
        IItcoputio(_name,(short *)0,1,32,0,fname);
        IItcoputio(_return_type,(short *)0,1,32,0,rtype);
        IItcoputio(_short_remark,(short *)0,1,32,0,remarks);
        IITBceColEnd();
      } /* IItinsert */
    } /* IItbsetio */
  }
/* # line 993 "vqprocs.qsc" */	/* host code */
}
/*{
** Name:	_check_unique	- check main table field for new variable name
**
** Description:
**	This routine checks the table field on the main Local Variables form
**	to see if the variable that was created or renamed already exists
**	as a local variable for this frame.
**
** Inputs:
**	char *new_name		- ptr to char string containing new name
**
** Outputs:
**	Returns:
**		STATUS	- OK if new name is acceptable, FAIL otherwise
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	11-aug-92 (blaise)
**		Initial Version
*/
static STATUS
_check_unique(mf, new_name)
METAFRAME	*mf;
char		*new_name;
{
	MFVAR	*mf_var;
  char existing_name[FE_MAXNAME + 1];
  i4 name_state;
	i4	i;
	STATUS	stat = OK;
	/*
	** First check the table field of local procedures to see if the
	** name already exists.
	*/
/* # line 1035 "vqprocs.qsc" */	/* unloadtable */
  {
    if (IItbact(vqprocs_form,vqprocs_table,0) == 0) goto IItbE2;
    while (IItunload() != 0) {
      IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,existing_name,_name);
      IItcogetio((short *)0,1,30,4,&name_state,(char *)"_STATE");
      IITBceColEnd();
      {
/* # line 1040 "vqprocs.qsc" */	/* host code */
		if (name_state != stDELETE && STequal(new_name, existing_name))
		{
			stat = FAIL;
/* # line 1043 "vqprocs.qsc" */	/* endloop */
        {
          if (1) goto IItbE2;
        }
/* # line 1044 "vqprocs.qsc" */	/* host code */
		}
      }
    } /* IItunload */
IItbE2:
    IItunend();
  }
/* # line 1047 "vqprocs.qsc" */	/* host code */
	/*
	** Now check the names of existing local variables in the metaframe,
	** since these don't appear in the table field.
	*/
	if (stat == OK)
	{
		for (i = 0; i < mf->numvars; i++)
		{
			mf_var = mf->vars[i];
			if (mf_var->vartype != VAR_LPROC &&
				STequal(mf_var->name, new_name))
			stat = FAIL;
		}
	}
	if (stat == FAIL)
	{
		IIUGerr(E_VQ0150_duplicate_proc, UG_ERR_ERROR, 1, new_name);
	}
	return stat;
}
/*{
** Name:	loc_alloc	- allocate memory for a string variable
**
** Description:
**	This routine allocates memory for a string variable.  If the
**	character pointer passed in is NULL, the string allocated will
**	be 'size' in length, otherwise it will be the length of 'str' plus
**	one and 'str' will be copied to the allocated memory.
**
** Inputs:
**	TAGID tag;		- tag to use for allocated memory
**	i4 size		- size of allocated memory if 'str' is NULL
**	char *str		- ptr to char string to copy
**
** Returns:
**	{PTR}	- pointer to allocated memory which is a copy of
**			  'str' if 'str' was not NULL
** History:
**	11-aug-92 (blaise)
**		Initial Version
*/
static PTR
loc_alloc(tag,size,str)
TAGID   tag;
i4	size;
char    *str;
{
	PTR mem;
	if (str != NULL)
		size = STlength(str)+1;
	mem = FEreqmem((u_i4)tag, (u_i4)size, FALSE, (STATUS *)NULL);
	if (mem == NULL)
	{
		return (NULL);
	}
	if (str != NULL)
		STcopy(str,(char *) mem);
	return (mem);
}
