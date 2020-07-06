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
# include	<oosymbol.h>
# include	<stdprmpt.h>
# include	<afe.h>
# include	<ug.h>
# include	<ui.h>
# include	<uf.h>
# include	<metafrm.h>
# include	"ervq.h"
/*
NO_OPTIM = nc4_us5
*/
/**
** Name:	vqlocals.qc -	User interface for local variable handling
**
** Description:
**	This file has routines to display existing local variables for the
**	current frame, allows the user to create, delete, rename and edit
**	local variables	and saves the variables back into the database.
**
**	This file defines:
**
**	IIVQlocals		Displays and edits local variables
**	_create_edit_var	FRS code for create/edit popup
**	_check_ing_type		Determines if datatype is derived or DBDV
**	_extract_var_info	Break apart datatype field for display in popup
**	_fe_name_check		Checks char string for a valid INGRES name
**	_save_var		Sticks local variable in main table field
**	_set_disp_only		Set display mode for nullable/isarray fields
**	_check_unique		Checks table field for already existing name
**	loc_alloc		String allocation routine
**
** History:
**	14-oct-89 (kenl) - created
**	27-feb-90 (kenl)  Added IIUFhlpType() to handle ListChoices help for
*8		datatype selection.
**	03/28/91 (dkh) - Replaced old inquire_frs syntax with new.
**	29-jul-92 (blaise)
**		Small change to description above: changed function names to
**		match what's really here.
**	08/24/92 (gautam) - fixed problem causing stack corruption
**		in vision and vigraph.
**	09/16/92 (deannaw)
**		Backed out previous fix by Gautam.  Stack corruption problems
**		corrected when new "refresher" compiler installed on AIX port.
**	17-sep-92 (blaise)
**		Reinstated some history backed out in previous change.
**	25-aug-92 (blaise)
**	    Made changes to deal with the fact that the vars structure can now
**	    contain local procedures as well as local variables.
**	22-apr-93 (blaise)
**	    Give an error if the user enters a datatype which isn't an ingres
**	    datatype or an existing record. Also check for a valid front end
**	    datatype; this will give an error for long varchar, which isn't
**	    supported in the front ends. (bug #51167, 51170)
**	9-jul-93 (blaise)
**	    Don't give error for variables of type TYPE OF TABLE, etc.
**	    (bug #52053)
**	15-nov-93 (robf)
**          Add security label (DB_SEC_TYPE) to list of types
**	09-July-1998 (thaal01) Bug 88637
**	    Creating local variables within Vision causes SIGSEGV 
**	    (sometimes) when saving new local variables. This is
**	    X-Int of 436428 from oping12 	
**	28-oct-1998 (matbe01)
**	    Added NO_OPTIM for NCR (nc4_us5) to avoid SIGSEGV that
**	    results when displaying local variables.
**    25-Oct-2005 (hanje04)
**        Add prototypes for _create_edit_var(), _extract_var_info(),
**	  _check_ing_type(), _fe_name_check(), _save_var(), _set_disp_only() &
**	  _check_unique() to prevent compiler errors with GCC 4.0 due to
**	  conflict with implicit declaration.
**	11-sep-2006 (gupsh01)
**	    Added support for ANSI date/time types.
**/
# define _yes	1
# define _no	0
# define _unset -1
# define _localv_prmpt		(ERget(F_VQ0108_localv_prmpt))
# define STGETWORDS_MAX		53
# define MAX_DATATYPE_STRING	105
/* GLOBALDEF's */
GLOBALREF APPL *IIVQappl; /* argument of frame flow entry, the current
			   application we are editing in abf */
GLOBALREF char *IIabExename;	/* Executable name */
/* extern's */
FUNC_EXTERN VOID IIVQgeGlobalEdit();
FUNC_EXTERN STATUS IIAMmpMetaPopulate();
FUNC_EXTERN bool IIAFfedatatype();
/* static's */
static PTR	loc_alloc();
static i4 _create_edit_var();
static i4 _extract_var_info();
static STATUS _check_ing_type();
static STATUS _fe_name_check();
static i4 _save_var();
static i4 _set_disp_only();
static STATUS _check_unique();
static READONLY
  char vqlocals_form[]= ERx("vqlocals"), vqlocals_table[]= ERx("locals"), 
  vqcreate_form[]= ERx("vqlocalcreate"), _data_type[]= ERx("data_type"), 
  _nullable[]= ERx("nullable"), _isarray[]= ERx("isarray");
  static char *_n_a;
static bool	first_call = TRUE;
static bool	save_vars = FALSE;
static ADF_CB	*adf_cb;
static READONLY
	DB_DT_ID	_types[] = { DB_INT_TYPE, DB_FLT_TYPE, DB_DEC_TYPE,
				     DB_CHA_TYPE, DB_VCH_TYPE, DB_TXT_TYPE,
				     DB_CHR_TYPE, DB_DTE_TYPE, DB_MNY_TYPE,
				     DB_ADTE_TYPE, DB_TMWO_TYPE, DB_TMW_TYPE,
				     DB_TME_TYPE, DB_TSWO_TYPE, DB_TSW_TYPE,
				     DB_TSTMP_TYPE, DB_INDS_TYPE, DB_INYM_TYPE
};
/*{
** Name:	IIVQlocals() -  Visual Query Frame Interface to Local Variables.
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
**	14-oct-89  (kenl)  - initial version
**	20-dec-90 (blaise)
**	    Stopped "not a legal ingres name" error from occurring when a
**	    local variable is renamed and the new name contains a ".".
**	    Bug #34835.
**      23-feb-93 (connie) Bug #43375
**          Prevent the user from creating more than MF_MAXVAR local variables
**          per frame with an error message
*/
VOID
  IIVQlocals(apobj)
  OO_OBJECT *apobj;
  {
	METAFRAME	*pmf;
	MFVAR		**frame_vars;
	MFVAR		**frame_procs;
	MFVAR		*fp;
	TAGID		tag;
	TAGID		fp_tag;
	i4		vartotal;
	i4		lvarcount = 0;
	i4		lproccount = 0;
	i4		i;
	i4		j;
	i4		ix;
	i4 		jx;
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
		IIUGerr(E_VQ008C_No_Locals_Allowed, UG_ERR_ERROR, 0);
		return;
	} /* end switch */
	save_vars = FALSE;
	pmf = ((USER_FRAME *)apobj)->mf;
	adf_cb = FEadfcb();
	/* initialize forms used for local variables */
	if (first_call)
	{
		first_call = FALSE;
		IIARiaForm(vqlocals_form);
		IIARiaForm(vqcreate_form);
/* # line 232 "vqlocals.qsc" */	/* inittable */
    {
      if (IItbinit(vqlocals_form,vqlocals_table,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 233 "vqlocals.qsc" */	/* host code */
		_n_a = ERget(F_VQ010D_n_a);
	}
	if (IIAMmpMetaPopulate(pmf, MF_P_FLD) != OK)
	{
		return;
	}
	frame_vars = pmf->vars;
	tag = (pmf->ptag)[MF_PT_FLD];
	vartotal = pmf->numvars;
	/*
	** Allocate memory for frame_procs, a temporary array to hold the 
	** local procedures in the metaframe.
	*/
	fp_tag = FEgettag();
	frame_procs = (MFVAR **) loc_alloc(fp_tag, sizeof(MFVAR *) * MF_MAXVARS,
			NULL);
/* # line 253 "vqlocals.qsc" */	/* display */
    {
      if (IIdispfrm(vqlocals_form,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 256 "vqlocals.qsc" */	/* host code */
		register i4	i;
        char titl[80];
		IIUGfmt(titl, sizeof(titl), ERget(F_VQ0078_EditLocalVars), 
			1, IIabExename);
/* # line 261 "vqlocals.qsc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio(_name,(short *)0,1,32,0,apobj->name);
            IIputfldio(_title,(short *)0,1,32,0,titl);
          } /* IIfsetio */
        }
/* # line 262 "vqlocals.qsc" */	/* host code */
		/* load up the main tablefield on the form */
		for ( i = 0 ; i < vartotal ; ++i )
        {
			register MFVAR	*cvar;
          char *vname;
          char *vtype;
          char *vcomment;
			cvar = frame_vars[i];
			/*
			** local variables come first in the metaframe.
			*/
			if (cvar->vartype == VAR_LVAR)
          {
				vname = cvar->name;
				vtype = cvar->dtype;
				vcomment = cvar->comment;
				lvarcount++;
/* # line 281 "vqlocals.qsc" */	/* loadtable */
            {
              if (IItbact(vqlocals_form,vqlocals_table,1) != 0) {
                IItcoputio(_name,(short *)0,1,32,0,vname);
                IItcoputio(_data_type,(short *)0,1,32,0,vtype);
                IItcoputio(_short_remark,(short *)0,1,32,0,vcomment);
                IITBceColEnd();
              } /* IItbact */
            }
          }
/* # line 287 "vqlocals.qsc" */	/* host code */
			else
			/* Local procedure. Copy into the temporary array. */
			{
				fp = (MFVAR *) loc_alloc(fp_tag, sizeof(MFVAR),
					NULL);
				frame_procs[lproccount++] = fp;
				fp->name = loc_alloc(fp_tag,0,cvar->name);
				fp->dtype = loc_alloc(fp_tag,0,cvar->dtype);
				fp->comment = loc_alloc(fp_tag,0,cvar->comment);
			}
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 303 "vqlocals.qsc" */	/* host code */
		if (lvarcount >= MF_MAXVARS )
		{
			i4	maxlocal;
			maxlocal = MF_MAXVARS;
			IIUGerr( S_VQ00D7_Max_Local_Var, UG_ERR_ERROR, 2, 
				(PTR)&maxlocal, (PTR)&maxlocal );
		}
		else
			_create_edit_var(pmf, TRUE, &lvarcount);
          }
        } else if (IIretval() == 2) {
          {
/* # line 316 "vqlocals.qsc" */	/* host code */
		if (lvarcount == 0)
		{
			IIUGerr(S_VQ008E_None_in_table, UG_ERR_ERROR, 0);
		}
		else
		{
/* # line 322 "vqlocals.qsc" */	/* getrow */
            {
              if (IItbsetio(2,vqlocals_form,vqlocals_table,-2) != 0) {
                IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,fe_name,_name);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 323 "vqlocals.qsc" */	/* host code */
			if (IIUFccConfirmChoice (CONF_DESTROY, fe_name,
				ERget(F_VQ0137_Local_Variable),
				ERget(F_VQ0136_Confirm_LV_Destroy),
				ERx("vqlvdest.hlp")) == CONFCH_YES)
			{
/* # line 328 "vqlocals.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,vqlocals_form,vqlocals_table,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 329 "vqlocals.qsc" */	/* host code */
				--lvarcount;
				save_vars = TRUE;
			}
	    	}
          }
        } else if (IIretval() == 3) {
          {
/* # line 338 "vqlocals.qsc" */	/* host code */
		if (lvarcount == 0)
		{
			IIUGerr(S_VQ008E_None_in_table, UG_ERR_ERROR, 0);
/* # line 341 "vqlocals.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 342 "vqlocals.qsc" */	/* host code */
		}
		_create_edit_var(pmf, FALSE, &lvarcount);
          }
        } else if (IIretval() == 4) {
          {
/* # line 348 "vqlocals.qsc" */	/* host code */
	    if (lvarcount == 0)
	    {
		IIUGerr(S_VQ008E_None_in_table, UG_ERR_ERROR, 0);
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
/* # line 376 "vqlocals.qsc" */	/* resume */
              {
                if (1) goto IIfdB1;
              }
/* # line 377 "vqlocals.qsc" */	/* host code */
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
/* # line 391 "vqlocals.qsc" */	/* resume */
              {
                if (1) goto IIfdB1;
              }
/* # line 392 "vqlocals.qsc" */	/* host code */
				}
			}
/* # line 395 "vqlocals.qsc" */	/* getrow */
              {
                if (IItbsetio(2,vqlocals_form,vqlocals_table,-2) != 0) {
                  IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,fe_name,_name);
                  IItcogetio((short *)0,1,32,MAX_DATATYPE_STRING + 1-1,db_type
                  ,_data_type);
                  IItcogetio((short *)0,1,32,OOSHORTREMSIZE+1-1,remarks,
                  _short_remark);
                  IITBceColEnd();
                } /* IItbsetio */
              }
/* # line 401 "vqlocals.qsc" */	/* deleterow */
              {
                if (IItbsetio(4,vqlocals_form,vqlocals_table,-2) != 0) {
                  if (IItdelrow(0) != 0) {
                  } /* IItdelrow */
                } /* IItbsetio */
              }
/* # line 402 "vqlocals.qsc" */	/* host code */
			item = FEtbsins(vqlocals_form, vqlocals_table, db_temp,
				_name, TRUE);
/* # line 404 "vqlocals.qsc" */	/* insertrow */
              {
                if (IItbsetio(5,vqlocals_form,vqlocals_table,item) != 0) {
                  if (IItinsert() != 0) {
                    IItcoputio(_name,(short *)0,1,32,0,db_temp);
                    IItcoputio(_data_type,(short *)0,1,32,0,db_type);
                    IItcoputio(_short_remark,(short *)0,1,32,0,remarks);
                    IITBceColEnd();
                  } /* IItinsert */
                } /* IItbsetio */
              }
/* # line 409 "vqlocals.qsc" */	/* host code */
			save_vars = TRUE;
            }
	    }
          }
        } else if (IIretval() == 5) {
          {
/* # line 417 "vqlocals.qsc" */	/* host code */
		IIVQlprocs(apobj);
          }
        } else if (IIretval() == 6) {
          {
/* # line 423 "vqlocals.qsc" */	/* host code */
		IIVQgeGlobalEdit(IIVQappl, FALSE);
          }
        } else if (IIretval() == 7) {
          {
/* # line 428 "vqlocals.qsc" */	/* host code */
		FEtabfnd(vqlocals_form, vqlocals_table);
          }
        } else if (IIretval() == 8) {
          {
/* # line 433 "vqlocals.qsc" */	/* scroll */
            {
              if (IItbsetio(1,vqlocals_form,vqlocals_table,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 9) {
          {
/* # line 438 "vqlocals.qsc" */	/* scroll */
            {
              if (IItbsetio(1,vqlocals_form,vqlocals_table,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 10) {
          {
/* # line 446 "vqlocals.qsc" */	/* host code */
		FEhelp(ERx("vqlocal.hlp"), _localv_prmpt);
          }
        } else if (IIretval() == 11) {
          {
/* # line 452 "vqlocals.qsc" */	/* host code */
	    if (save_vars)
            {
              i4 state;
		i = 0;
/* # line 457 "vqlocals.qsc" */	/* unloadtable */
              {
                if (IItbact(vqlocals_form,vqlocals_table,0) == 0) goto IItbE1;
                while (IItunload() != 0) {
                  IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,fe_name,_name);
                  IItcogetio((short *)0,1,32,MAX_DATATYPE_STRING + 1-1,db_temp
                  ,_data_type);
                  IItcogetio((short *)0,1,32,OOSHORTREMSIZE+1-1,remarks,
                  _short_remark);
                  IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
                  IITBceColEnd();
                  {
/* # line 464 "vqlocals.qsc" */	/* host code */
		    if (state != stDELETE)
		    {
			register MFVAR	*cvar;
			cvar = (MFVAR *) loc_alloc(tag, sizeof(MFVAR),NULL);
			cvar->name = loc_alloc(tag,0,fe_name);
			cvar->vartype = VAR_LVAR;
			cvar->dtype = loc_alloc(tag,0,db_temp);
			cvar->comment = loc_alloc(tag,0,remarks);
			frame_vars[i++] = cvar;
		    }
                  }
                } /* IItunload */
IItbE1:
                IItunend();
              }
/* # line 476 "vqlocals.qsc" */	/* host code */
 		lvarcount = i;
		vartotal = lvarcount + lproccount;
		/*
		** Now copy the local procedures back into the metaframe and
		** free memory allocated for frame_procs[]
		*/
		ix = 0 ;
		jx = lvarcount ;
		while ( ix <  lproccount )
		{
			register MFVAR	*cvar;
			cvar = (MFVAR *) loc_alloc(tag, sizeof(MFVAR),NULL);
			cvar->name = loc_alloc(tag, 0, frame_procs[ix]->name);
			cvar->vartype = VAR_LPROC;
			cvar->dtype = loc_alloc(tag, 0, frame_procs[ix]->dtype);
			cvar->comment = loc_alloc(tag, 0,
						frame_procs[ix]->comment);
			frame_vars[jx] = cvar;
			ix++ ; jx++ ;
		}
		if (fp_tag != 0)
			FEfree(fp_tag);
		/* set the metaframe state flag to indicate that a regen
		   is required because the local vars have changed. */
		pmf->state |= MFST_DOGEN;
		/* update the disk with the change. */
		IIAMufqUserFrameQuick(pmf->apobj);
		pmf->numvars = vartotal;
		/* update the disk with the local variables */
		IIAMmuMetaUpdate(pmf, MF_P_FLD);
            }
	    IIAMmfMetaFree(pmf, MF_P_FLD);
/* # line 514 "vqlocals.qsc" */	/* clear */
            {
              IIfldclear(vqlocals_table);
            }
/* # line 515 "vqlocals.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_Create),ERget(F_VQ0109_CreateLV_expl),2,2,1) == 0
      ) goto IIfdE1;
      if (IInmuact(ERget(FE_Destroy),ERget(F_VQ010A_DeleteLV_expl),2,2,2) == 0
      ) goto IIfdE1;
      if (IInmuact(ERget(FE_Edit),ERget(F_VQ010B_EditLV_expl),2,2,3) == 0) 
      goto IIfdE1;
      if (IInmuact(ERget(FE_Rename),ERget(F_VQ010C_RenameLV_expl),2,2,4) == 0
      ) goto IIfdE1;
      if (IInmuact(ERget(F_VQ0157_LocalProcs),ERget(F_VQ0158_Procs_expl),2,2,5)
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
/* # line 520 "vqlocals.qsc" */	/* host code */
/*{
** Name:	_create_edit_var	- create/edit a local variable
**
** Description:
**	This routine handles the popup for creating/editing local variables.
**
** Inputs:
**	bool create_op;		- TRUE to create, FALSE to edit a variable
**	i4 *lvarcount		- ptr to i4  indicating no. of local variables
**
** Outputs:
**	lvarcount is set to the updated number of local variables
**
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	01-dec-89  (kenl)  - initial version
**	31-jul-91 (blaise)
**		Remove activation of frskey3; it wasn't listed as a menu item
**		but if hit inadvertently would end without any opportunity
**		for saving changes. Bug #37647.
*/
static
_create_edit_var(mf, create_op, lvarcount)
METAFRAME	*mf;
bool		create_op;
i4		*lvarcount;
{
	i4	is_ingres;
  char var_name[2 * FE_MAXNAME + 1];
/* # line 557 "vqlocals.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(vqcreate_form,(char *)"f") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 560 "vqlocals.qsc" */	/* clear */
      {
        IIclrflds();
      }
/* # line 561 "vqlocals.qsc" */	/* putform */
      {
        if (IIfsetio(vqcreate_form) != 0) {
          IIputfldio(_isarray,(short *)0,1,32,0,_n_a);
          IIputfldio(_nullable,(short *)0,1,32,0,_n_a);
        } /* IIfsetio */
      }
/* # line 562 "vqlocals.qsc" */	/* host code */
		if (create_op)
		{
      char titl[80];
			_VOID_ IIUGfmt(titl, sizeof(titl), 
				ERget(F_VQ0075_CreateLocal), 1, IIabExename);
/* # line 568 "vqlocals.qsc" */	/* putform */
      {
        if (IIfsetio(vqcreate_form) != 0) {
          IIputfldio((char *)"title",(short *)0,1,32,0,titl);
        } /* IIfsetio */
      }
/* # line 569 "vqlocals.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,vqcreate_form,(char *)0, (char *)0) != 0) {
          IIstfsio(53,_name,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 570 "vqlocals.qsc" */	/* host code */
			/* make the n/a fields display only */
			_set_disp_only(_yes, _yes);
			is_ingres = _unset;
		}
		else
		{
			_extract_var_info(&is_ingres);
		}
    }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 582 "vqlocals.qsc" */	/* host code */
		char	*strptr;
		char	var_copy[2 * FE_MAXNAME + 1];
/* # line 585 "vqlocals.qsc" */	/* getform */
          {
            if (IIfsetio(vqcreate_form) != 0) {
              IIgetfldio((short *)0,1,32,2 * FE_MAXNAME + 1-1,var_name,_name);
            } /* IIfsetio */
          }
/* # line 586 "vqlocals.qsc" */	/* host code */
		STcopy(var_name, var_copy);
		if ((strptr = STindex(var_copy, ERx("."), 0)) == NULL)
		{
			if (_fe_name_check(var_name) == OK)
			{
/* # line 591 "vqlocals.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB2;
          }
/* # line 592 "vqlocals.qsc" */	/* host code */
			}
		}
		else
		{
			*strptr = EOS;
			CMnext(strptr);
			if ((_fe_name_check(var_copy) == OK)
					&& (_fe_name_check(strptr) == OK))
			{
/* # line 601 "vqlocals.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB2;
          }
/* # line 602 "vqlocals.qsc" */	/* host code */
			}
		}
/* # line 604 "vqlocals.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 609 "vqlocals.qsc" */	/* host code */
		if (_check_ing_type(&is_ingres) == OK)
		{
/* # line 611 "vqlocals.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB2;
          }
/* # line 612 "vqlocals.qsc" */	/* host code */
		}
/* # line 613 "vqlocals.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 618 "vqlocals.qsc" */	/* validate */
          {
            if (IIvalfld(_data_type) == 0) {
              goto IIfdB2;
            } /* IIvalfld */
          }
/* # line 620 "vqlocals.qsc" */	/* host code */
		if (create_op)
		{
			/* check to see if the name already exists */
/* # line 623 "vqlocals.qsc" */	/* getform */
          {
            if (IIfsetio(vqcreate_form) != 0) {
              IIgetfldio((short *)0,1,32,2 * FE_MAXNAME + 1-1,var_name,_name);
            } /* IIfsetio */
          }
/* # line 624 "vqlocals.qsc" */	/* host code */
			if (_check_unique(mf, var_name) != OK)
			{
/* # line 626 "vqlocals.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 627 "vqlocals.qsc" */	/* host code */
			}
		}
		_save_var(is_ingres);
		if (create_op)
		{
			++(*lvarcount);
		}
		save_vars = TRUE;
/* # line 635 "vqlocals.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 642 "vqlocals.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 5) {
        {
          char fname[FE_MAXNAME+1];
/* # line 650 "vqlocals.qsc" */	/* host code */
		FLD_VALUE	fv;
		fv._form = vqcreate_form;
		fv._field = fname;
		fv._column = NULL;
/* # line 656 "vqlocals.qsc" */	/* inquire_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fname,22,vqcreate_form,0);
            } /* IIiqset */
          }
/* # line 657 "vqlocals.qsc" */	/* host code */
		if (STequal(fname, _data_type))
		{
			IIUFhlpType( _types, sizeof(_types)/sizeof(_types[0]),
					TRUE, &fv
			);
		}
		else
		{
			IIUFfldHelp(&fv);
		}
/* # line 667 "vqlocals.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB2;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 675 "vqlocals.qsc" */	/* host code */
		if (create_op)
		{
			FEhelp(ERx("vqlvcre.hlp"), ERget(F_VQ010E_localv_create));
		}
		else
		{
			FEhelp(ERx("vqlvedit.hlp"), _localv_prmpt);
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
    if (IIFRafActFld(_data_type,0,2) == 0) goto IIfdE2;
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_OK),(char *)0,2,2,3) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,3) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0109_ExplForget),0,0,4) == 0) 
    goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,0,4) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,5) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,5) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,6) == 0) 
    goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,0,6) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 684 "vqlocals.qsc" */	/* host code */
}
/*{
** Name:	_extract_var_info	- break apart datatype field
**
** Description:
**	The datatype, as the user sees it, on the main Local Variable
**	table field needs to be broken up into several pieces before it is
**	displayed on the create/edit popup.  This routine does that pulling
**	apart.
**
** Inputs:
**	i4 *is_ingres		- ptr to i4  for type of datatype (derived,etc)
**
** Outputs:
**	is_ingres is set to indicate type of datatype (derived, standard, etc.)
**
** History:
**	01-dec-89  (kenl)  - initial version
*/
static
_extract_var_info(is_ingres)
i4	*is_ingres;
{
  char new_type[MAX_DATATYPE_STRING + 1];
  char old_type[MAX_DATATYPE_STRING + 1];
  char *vtype;
  char *null_yorn;
  char *array_yorn;
  char old_remark[OOSHORTREMSIZE+1];
  char fe_name[FE_MAXNAME + 1];
	i4		count;
	i4		start;
	char		*wordarray[STGETWORDS_MAX];
	register i4	i;
  char titl[80];
	_VOID_ IIUGfmt(titl, sizeof(titl), ERget(F_VQ0076_EditLocal), 
			1, IIabExename);
/* # line 726 "vqlocals.qsc" */	/* putform */
  {
    if (IIfsetio(vqcreate_form) != 0) {
      IIputfldio((char *)"title",(short *)0,1,32,0,titl);
    } /* IIfsetio */
  }
/* # line 727 "vqlocals.qsc" */	/* host code */
	null_yorn = array_yorn = _n_a;
/* # line 728 "vqlocals.qsc" */	/* getrow */
  {
    if (IItbsetio(2,vqlocals_form,vqlocals_table,-2) != 0) {
      IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,fe_name,_name);
      IItcogetio((short *)0,1,32,MAX_DATATYPE_STRING + 1-1,old_type,_data_type);
      IItcogetio((short *)0,1,32,OOSHORTREMSIZE+1-1,old_remark,_short_remark);
      IITBceColEnd();
    } /* IItbsetio */
  }
/* # line 733 "vqlocals.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,vqcreate_form,(char *)0, (char *)0) != 0) {
      IIstfsio(53,_name,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 734 "vqlocals.qsc" */	/* host code */
	count = STGETWORDS_MAX;
	_VOID_ STgetwords(old_type, &count, wordarray);
	start = 0;
	if (STequal(wordarray[0], ERx("ARRAY")))
	{
		array_yorn = ERget(F_UG0002_Yes2);
		start = 2;
		*is_ingres = _no;
	}
	else if ((count >= 3) && (STequal(wordarray[2], ERx("NULL"))))
	{
		null_yorn = (STequal(wordarray[1], ERx("WITH"))) ?
			ERget(F_UG0002_Yes2) : ERget(F_UG0007_No2);
		count = 1;
		*is_ingres = _yes;
	}
	else
	{
		*is_ingres = _no;
		array_yorn = ERget(F_UG0007_No2);
	}
	for (i = start, vtype = new_type; i < count; i++)
	{
		vtype += STlcopy (wordarray[i], vtype, 255);
		*vtype++ = ' ';
	}
	*vtype = EOS;
	if (*is_ingres == _yes)
	{
		_set_disp_only(_yes, _no);
	}
	else
	{
		_set_disp_only(_no, _yes);
	}
/* # line 771 "vqlocals.qsc" */	/* putform */
  {
    if (IIfsetio(vqcreate_form) != 0) {
      IIputfldio(_name,(short *)0,1,32,0,fe_name);
      IIputfldio(_data_type,(short *)0,1,32,0,new_type);
      IIputfldio(_nullable,(short *)0,1,32,0,null_yorn);
      IIputfldio(_isarray,(short *)0,1,32,0,array_yorn);
      IIputfldio(_short_remark,(short *)0,1,32,0,old_remark);
    } /* IIfsetio */
  }
/* # line 779 "vqlocals.qsc" */	/* host code */
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
**	01-dec-89  (kenl)  - initial version
**	16-oct-2006 (gupsh01)
**	      Fix date key word to refer to ingresdate or ansidate.
**	10-Jul-2007 (kibro01)
**	    Remove IIAMdateAlias since its functionality is in adi_tyid
*/
static STATUS
_check_ing_type(is_ingres)
i4	*is_ingres;
{
	i4	len;
  char db_type[MAX_DATATYPE_STRING + 1];
	char	tmpbuf[MAX_DATATYPE_STRING + 1];
	DB_USER_TYPE	utype;
	DB_DATA_VALUE	dbv;
  i4 changed;
  char *strptr;
	char		type_of[] = ERx("type of");
        char            dtemp[11];
        char            *dp = dtemp;
/* # line 827 "vqlocals.qsc" */	/* inquire_frs */
  {
    if (IIiqset(2,0,vqcreate_form,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&changed,20,_data_type,0);
    } /* IIiqset */
  }
/* # line 828 "vqlocals.qsc" */	/* host code */
	if (changed == 0)
	{
		return(OK);
	}
/* # line 833 "vqlocals.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,vqcreate_form,(char *)0, (char *)0) != 0) {
      IIstfsio(20,_data_type,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 834 "vqlocals.qsc" */	/* getform */
  {
    if (IIfsetio(vqcreate_form) != 0) {
      IIgetfldio((short *)0,1,32,MAX_DATATYPE_STRING + 1-1,db_type,_data_type);
    } /* IIfsetio */
  }
/* # line 836 "vqlocals.qsc" */	/* host code */
	/* Check whether this is this a valid ingres datatype */
	if ((len = STzapblank(db_type, tmpbuf)) < DB_TYPE_SIZE)
	{
		if (len == 0)
		{
			return(FAIL);
		}
		STlcopy(db_type, utype.dbut_name, DB_TYPE_SIZE);
		if (afe_tychk(adf_cb, &utype, &dbv) == OK &&
			IIAFfedatatype(&dbv) == TRUE)
		{
/* # line 847 "vqlocals.qsc" */	/* putform */
  {
    if (IIfsetio(vqcreate_form) != 0) {
      IIputfldio(_data_type,(short *)0,1,32,0,db_type);
    } /* IIfsetio */
  }
/* # line 848 "vqlocals.qsc" */	/* host code */
			_set_disp_only(_yes, _no);
			if (*is_ingres != _yes)
			{
/* # line 851 "vqlocals.qsc" */	/* putform */
  {
    if (IIfsetio(vqcreate_form) != 0) {
      IIputfldio(_isarray,(short *)0,1,32,0,_n_a);
      IIputfldio(_nullable,(short *)0,1,32,0,ERget(F_UG0007_No2));
    } /* IIfsetio */
  }
/* # line 855 "vqlocals.qsc" */	/* host code */
				*is_ingres = _yes;
			}
			return(OK);
		}
	}
	/*
	** We don't have a valid ingres datatype; check whether its type is
	** TYPE OF .... or an existing record type.
	*/
	if (STbcompare(db_type, 0, type_of, STlength(type_of), TRUE) != 0 &&
		OOsnd(OC_RECORD, _withName, db_type, (char*)NULL,
			IIVQappl->ooid) == 0)
	{
		/* Error - it isn't TYPE OF ... or an existing record type */
		IIUGerr(E_VQ00F5_Bad_Data_Type, UG_ERR_ERROR, 1, db_type);
		return(FAIL);
	}
	_set_disp_only(_no, _yes);
	if (*is_ingres != _no)
	{
/* # line 876 "vqlocals.qsc" */	/* putform */
  {
    if (IIfsetio(vqcreate_form) != 0) {
      IIputfldio(_nullable,(short *)0,1,32,0,_n_a);
      IIputfldio(_isarray,(short *)0,1,32,0,ERget(F_UG0007_No2));
    } /* IIfsetio */
  }
/* # line 880 "vqlocals.qsc" */	/* host code */
		*is_ingres = _no;
	}
	strptr = db_type;
	if (CMspace(strptr))
	{
		while (CMspace(strptr))
		{
			CMnext(strptr);
		}
/* # line 889 "vqlocals.qsc" */	/* putform */
  {
    if (IIfsetio(vqcreate_form) != 0) {
      IIputfldio(_data_type,(short *)0,1,32,0,strptr);
    } /* IIfsetio */
  }
/* # line 890 "vqlocals.qsc" */	/* host code */
	}
	return(OK);
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
**	01-dec-89  (kenl)  - initial version
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
** Name:	_save_var	- update main table field with local variable
**
** Description:
**	This routine updates the table field on the main Local Variables
**	form after a variable has been created or edited.
**
** Inputs:
**	i4 is_ingres		- indicates type of datatype (derived,etc)
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
**	01-dec-89  (kenl)  - initial version
*/
static
_save_var(is_ingres)
i4	is_ingres;
{
  char fname[FE_MAXNAME + 1];
  char dtype[MAX_DATATYPE_STRING + 1];
  char db_temp[MAX_DATATYPE_STRING + 1];
  char remarks[OOSHORTREMSIZE+1];
  char is_array[15+1];
  char is_nullable[15+1];
  i4 item;
/* # line 964 "vqlocals.qsc" */	/* getform */
  {
    if (IIfsetio(vqcreate_form) != 0) {
      IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,fname,_name);
      IIgetfldio((short *)0,1,32,MAX_DATATYPE_STRING + 1-1,dtype,_data_type);
      IIgetfldio((short *)0,1,32,OOSHORTREMSIZE+1-1,remarks,_short_remark);
    } /* IIfsetio */
  }
/* # line 968 "vqlocals.qsc" */	/* host code */
	STcopy(dtype, db_temp);
	if (is_ingres == _yes)
	{
/* # line 971 "vqlocals.qsc" */	/* getform */
  {
    if (IIfsetio(vqcreate_form) != 0) {
      IIgetfldio((short *)0,1,32,15+1-1,is_nullable,_nullable);
    } /* IIfsetio */
  }
/* # line 972 "vqlocals.qsc" */	/* host code */
		if (IIUGyn(is_nullable, (STATUS *)NULL))
		{
			STpolycat(2, dtype, ERx(" WITH NULL"), db_temp);
		}
		else
		{
			STpolycat(2, dtype, ERx(" NOT NULL"), db_temp);
		}
	}
	else
	{
/* # line 983 "vqlocals.qsc" */	/* getform */
  {
    if (IIfsetio(vqcreate_form) != 0) {
      IIgetfldio((short *)0,1,32,15+1-1,is_array,_isarray);
    } /* IIfsetio */
  }
/* # line 984 "vqlocals.qsc" */	/* host code */
		if (IIUGyn(is_array, (STATUS *)NULL))
		{
			STpolycat(2, ERx("ARRAY OF "), dtype, db_temp);
		}
	}
	item = FEtbsins(vqlocals_form, vqlocals_table, fname, _name, TRUE);
/* # line 991 "vqlocals.qsc" */	/* insertrow */
  {
    if (IItbsetio(5,vqlocals_form,vqlocals_table,item) != 0) {
      if (IItinsert() != 0) {
        IItcoputio(_name,(short *)0,1,32,0,fname);
        IItcoputio(_data_type,(short *)0,1,32,0,db_temp);
        IItcoputio(_short_remark,(short *)0,1,32,0,remarks);
        IITBceColEnd();
      } /* IItinsert */
    } /* IItbsetio */
  }
/* # line 996 "vqlocals.qsc" */	/* host code */
}
/*{
** Name:	_set_disp_only	- set displayonly attributes
**
** Description:
**	This routine sets the displayonly attributes for the fields
**	'isarray' and 'nullable' on the create/edit popup.
**
** Inputs:
**	i4 x;			- _yes (1) for displayonly, _no (0) otherwise
**	i4 y;			- _yes (1) for displayonly, _no (0) otherwise
**
** History:
**	01-dec-89  (kenl)  - initial version
*/
static
_set_disp_only(x, y)
  i4 x;
  i4 y;
{
/* # line 1019 "vqlocals.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,vqcreate_form,(char *)0, (char *)0) != 0) {
      IIstfsio(53,_isarray,0,(short *)0,1,30,4,&x);
      IIstfsio(53,_nullable,0,(short *)0,1,30,4,&y);
    } /* IIiqset */
  }
/* # line 1023 "vqlocals.qsc" */	/* host code */
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
**	01-dec-89  (kenl)  - initial version
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
	STATUS	rstat = OK;
	/*
	** First check the table field of local variables to see if the name
	** already exists.
	*/
/* # line 1065 "vqlocals.qsc" */	/* unloadtable */
  {
    if (IItbact(vqlocals_form,vqlocals_table,0) == 0) goto IItbE2;
    while (IItunload() != 0) {
      IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,existing_name,_name);
      IItcogetio((short *)0,1,30,4,&name_state,(char *)"_STATE");
      IITBceColEnd();
      {
/* # line 1070 "vqlocals.qsc" */	/* host code */
		if (name_state != stDELETE && STequal(new_name, existing_name))
		{
			rstat = FAIL;
/* # line 1073 "vqlocals.qsc" */	/* endloop */
        {
          if (1) goto IItbE2;
        }
/* # line 1074 "vqlocals.qsc" */	/* host code */
		}
      }
    } /* IItunload */
IItbE2:
    IItunend();
  }
/* # line 1077 "vqlocals.qsc" */	/* host code */
	/*
	** Now check the names of existing local variables in the metaframe,
	** since these don't appear in the table field.
	*/
	if (rstat == OK)
	{
		for (i = 0; i < mf->numvars; i++)
		{
			mf_var = mf->vars[i];
			if (mf_var->vartype != VAR_LVAR &&
				STequal(mf_var->name, new_name))
			rstat = FAIL;
		}
	}
	if (rstat == FAIL)
	{
		IIUGerr(E_VQ008D_duplicate_local, UG_ERR_ERROR, 1, new_name);
	}
	return(rstat);
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
**	01-dec-89  (kenl)  - initial version
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
