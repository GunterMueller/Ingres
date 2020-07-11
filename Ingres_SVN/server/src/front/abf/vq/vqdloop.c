# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/* 
**	Copyright (c) 2004 Ingres Corporation  
*/
#include	<compat.h> 
# include	<cv.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h> 
#include 	<er.h>
#include 	<st.h>
#include 	<cm.h>
#include 	<adf.h>
#include 	<ft.h>
#include 	<fmt.h>
#include 	<frame.h>
#include 	<uf.h>
#include 	<ug.h>
# include <ui.h>
/*
#include        <ui.h>
	..so ming knows about the dependency
*/
#include 	<ooclass.h>
#include 	<abclass.h>
#include 	<stdprmpt.h>
# include <metafrm.h>
/*
#include	<metafrm.h>
  ..so ming knows about the dependancy 
*/
#include        <uigdata.h>
#include	"tabinfo.h"
#include	"ervq.h"
#include	"vqloc.h"
/**
** Name:	vqdloop -	visual query display loop
**
** Description:
**	Process the main display loop of the visual query.
**
**	This file defines:
**		IIVQvdlVqDisplayLoop - run the vq display loop
**		IIVQptPrimaryTab - find the primary table in a section
**		IIVQcvqCopyVQ - find the primary table in a section
** 		IIVQcsqCheckSeq	- check for presence of a sequence field
**
** History:
**	03/16/89  (tom) - created
**	12/19/89 (dkh) - VMS shared lib changes.  No more direct access
**			 to "frterminfo" - must now use FTterminfo().
**	9-sept-92 (blaise)
**	    Replaced IIUGlbo_lowerBEobject() with IIUGdbo_dlmBEobject() to
**	    handle delimited ids, and added new argument to FErelexists().
**	14-jul-93 (blaise)
**	    Add local procedures to the list of things you can edit.
**	    (Bug #50717)
**	10-sep-93 (kchin)
**		Changed type of nat_frame_ptr in IIVQvdlVqDisplayLoop() 
**		to PTR, since it is holding pointer. Also changed the cast 
**		of vq to PTR when assigns it to nat_frame_ptr. Cast to i4
**		will result in truncation of 64-bit address.
**	15-sep-93 (connie) Bug #54047
**	    Use FE_fullresolve in place of IIUGdbo_dlmBEobject and
**	    FErelexists so that owner qualification of table names and
**	    delimiter identifiers are handled properly
**      11-apr-95 (harpa06)
**              Send the converted table name "table_part" to the
**              IIVQantAddNewTable() function which adds a table to the VISION
**              query instead of the table name the user entered in so the FIPS
**              installation is happy. (Bug #67951)
**	20-apr-95 (harpa06)
**		Reimplemented change for #67951. This change fixed 68228, 68233,
**		68232, 68241
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	21-jan-03 (hayke02)
**	    Remove local prototyping of IIUFccConfirmChoice() - this is now
**	    done in fe.h.
**/
/* # define's */
/* GLOBALDEF's */
/* extern's */
  GLOBALREF char *IIVQform;
FUNC_EXTERN FRAME *IIVQvmfVqMakeFrame(); /* vq dynamic frame creation */
FUNC_EXTERN bool *IIVQvlfVqLoadFrame(); /* vq load frame with info */
FUNC_EXTERN PTR IIVQ_VAlloc(); /* vq allocation */
FUNC_EXTERN bool IIVQantAddNewTable(); /* add a new table to vq */
FUNC_EXTERN  bool IIVQlftLoadFrameTable(); /* load frame table */
FUNC_EXTERN VOID IIVQmdjMakeDefaultJoin(); 
FUNC_EXTERN VOID IIARiaForm();
FUNC_EXTERN VOID IIdelfrm();
FUNC_EXTERN bool IIVQask();
FUNC_EXTERN i4  IIFDlpListPick();
FUNC_EXTERN char *IITUtpTblPik();   /* Tables Utility ListPick */
FUNC_EXTERN VOID IITUtbiTableinfo();	/* Tables Utility Examine-a-Table */
FUNC_EXTERN VOID IIVQgeGlobalEdit();	/* Global edit */
FUNC_EXTERN VOID IIVQegfEditGenForm();
FUNC_EXTERN VOID IIVQegcEditGenCode();
FUNC_EXTERN i4   IIVQptPrimaryTab();
FUNC_EXTERN bool IIVQjcJoinCheck(); 
FUNC_EXTERN bool IIVQjwJoinWarn(); 
FUNC_EXTERN i4   IIVQgfbGetFrameBehavior();
FUNC_EXTERN VOID FTterminfo();
FUNC_EXTERN i4  IIVQdcvDisplayCompView();
FUNC_EXTERN bool IIVQdceDisplayCompErrs();
FUNC_EXTERN i4  IIVQcfCurFld();
FUNC_EXTERN VOID IIVQfmFormMode();
FUNC_EXTERN STATUS IIVQvqcVQCheck();
FUNC_EXTERN bool IIVQcsqCheckSeq(); 
FUNC_EXTERN char *IIVQgivGetIncVal();
FUNC_EXTERN bool IIVQivsInitVQStrings();
GLOBALREF char **IIar_Dbname;
GLOBALREF char *IIVQtfname_fmt; /* format string to form a table */
GLOBALREF bool IIabAbf;         /* flag to say that we have ABF capability */
GLOBALREF char *IIabExename;
  GLOBALREF char IIVQcol_col[4];
  GLOBALREF char IIVQdir_col[4];
  GLOBALREF char IIVQexp_col[4];
  GLOBALREF char IIVQinc_col[4];
  GLOBALREF char IIVQord_col[4];
  GLOBALREF char IIVQsrt_col[4];
  GLOBALREF i4 IIVQjtab;
  GLOBALREF i4 IIVQjcol;
  GLOBALREF char *IIVQ_d_text;
  GLOBALREF char *IIVQ_a_text;
  GLOBALREF char *IIVQ_y_text;
  GLOBALREF char *IIVQ_n_text;
  GLOBALREF char *IIVQ_v_text;
  GLOBALREF char IIVQcfnCurFldName[20];
/* static's */
static i4  _state;		/* change state of the visual query */
  static char *newlup_form = ERx("newlup");
  static char *_shell;
# define LOC static
LOC STATUS vq_SaveIf();
LOC STATUS vq_SaveEdit();
LOC i4   vq_RunLoop(); 
LOC i4   vq_KeyCapture(); 
LOC bool vq_FldAct(); 
LOC bool vq_ColAct(); 
LOC bool vq_NumAct(); 
LOC bool vq_DirAct(); 
LOC VOID vq_Reorder(); 
LOC VOID vq_ColChanged(); 
LOC VOID vq_InsLup(); 
LOC i4   vq_Edit(); 
LOC i4   vq_ED2(); 
LOC bool vq_AddLup(); 
LOC VOID vq_DelLup(); 
LOC VOID vq_DelJoin(); 
LOC VOID vq_ZapJoin(); 
LOC VOID vq_SetJoin(); 
LOC VOID vq_PutJoin(); 
LOC bool vq_CheckJoin(); 
LOC STATUS vq_Validate();
#define _vqed_prmpt	ERget(F_VQ00A3_vqed_prmpt)
/* language independent strings, and variables */
  static char *_nullstring = ERx("");
/* edit option info table...
   this array supplies information about the various 
   edit options used to construct a popup menu which 
   displays the options.  (vqloc.h)
   NOTE: this array doesn't use the flags member */
static EDINFO vq_edinfo[] = 
{	/* message id		type	    flags */
	{ F_VQ00AB_eop_genfrm,	EOP_GENFRM, ~0 },
	{ F_VQ00B3_eop_frbeha,	EOP_FRBEHA, ~0 },
	{ F_VQ00AE_eop_locvar,	EOP_LOCVAR, ~0 },
	{ F_VQ0151_eop_locprc,	EOP_LOCPRC, ~0 },
	{ F_VQ00AF_eop_globob,	EOP_GLOBOB, ~0 },
	{ F_VQ00B0_eop_esccod,	EOP_ESCCOD, ~0 },
	{ F_VQ00B1_eop_srcfil,	EOP_SRCFIL, ~0 },
	{ F_VQ00B2_eop_errors,	EOP_ERRORS, ~0 },
	{ 0 }	/* must be null terminated */ 
};
/*}
** Name:	VQED	- vq edit information passing structure
**
** Description:
**	This structure is used to pass information thru a 
**	call to the list pick code to a handler function.
**
** History:
**	10/03/89 (tom) - created
*/
typedef struct
{
	METAFRAME *mf;		/* pointer to the metaframe */
	i4 *typetab;		/* edit type translation */
} VQED;
/*{
** Name:	IIVQvdlVqDisplayLoop	- Vq dislay loop 
**
** Description:
**	Run the main display loop for the Visual Query processing.
**	The child function vq_RunLoop actually runs the display
**	loop.. and will return to us TRUE as long as it wishes
**	us to recreate the frame and continue to run the loop.
**	When it wishes to quit it returns FALSE.
**
**
** Inputs:
**	METAFRAME *mf;	- metaframe to be edited
**	bool new;	- flag to say if the frame is new.
**
** Outputs:
**	Returns:
**		bool	- TRUE if we want to be recalled
**			  FALSE if we are really done 
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/16/89  (tom) - created
*/
i4
IIVQvdlVqDisplayLoop(mf, new)
METAFRAME *mf;
bool new;
  {
	i4 tab;
    i4 row;
	FRAME *vq;
    PTR nat_frame_ptr;
	i4 stat;
	tab = -1;
	row = 0;
	/* if we have a new vq.. then we must save now, or else if the
	   user cancels we won't save anything. */
	if (new)
	{
		_state = MFST_DOFORM | MFST_DOGEN;
		_VOID_ vq_SaveIf(mf, VQ_SAVE);
	}
	else
	{
		_state = 0;
	}
#ifndef DGC_AOS
#ifndef CMS
#ifdef VMS
	_shell = ERget(FE_Spawn);
#else
	_shell = ERget(FE_Shell);
#endif
#else
	_shell = ERget(FE_CMSSubset);
#endif
#else
	_shell = ERget(FE_CLI);
#endif
	for ( ; ; )	
	{
		/* dynamically create the vq frame */
		vq = IIVQvmfVqMakeFrame(mf, FALSE, 0);
		/* add the form which was created */
		nat_frame_ptr = (PTR) vq;
/* # line 297 "vqdloop.qsc" */	/* addform */
    {
      IIaddform(nat_frame_ptr);
    }
/* # line 299 "vqdloop.qsc" */	/* host code */
		/* run the display loop til it says it's done */
		if ( (stat = vq_RunLoop(mf, tab, row)) & VQ_DONE)
		{
			IIdelfrm(IIVQform);
			break;
		}
		/* find out the field and row so when we re-run the
		   display loop we will put the user back on the right
		   table.column.  Note: this does not place them
		   back on the associtated fields if that is what they
		   were positioned on.. they always just land on the table */
		/* get the current field, and set IIVQcfnCurFldName */
		tab = IIVQcfCurFld();
		if (STlength(IIVQcfnCurFldName) <= 4)
		{
/* # line 317 "vqdloop.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,IIVQform,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&row,29,IIVQcfnCurFldName,0);
      } /* IIiqset */
    }
/* # line 319 "vqdloop.qsc" */	/* host code */
		}
		else
		{
			row = -1;
		}
		/* the user may have deleted the bottom table
		   so we have to fold back to the next to last */
		if (tab >= mf->numtabs) 
		{
			tab = mf->numtabs - 1;
			row = -1;
		}
		/* if the user deleted a table then the old row
		   may not really exist in the table that we are now on */
		if (row >= mf->tabs[tab]->numcols + 1)
		{
			row = -1;
		}
		IIdelfrm(IIVQform);
	}
	/* save the vq info if the user wanted to save, and anything changed */
	_VOID_ vq_SaveIf(mf, stat);
	return stat;
  }
/*{
** Name:	vq_SaveIf	- Save the VQ info if necessary
**
** Description:
**	Save the visual query info if necessary.
**
** Inputs:
**	METAFRAME *mf;	- metaframe pointer
**	i4 stat;	- status indicator
**
** Outputs:
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**	11/21/89  (tom) - created
*/
LOC STATUS 
vq_SaveIf(mf, stat)
METAFRAME *mf;
i4  stat;
{
	STATUS retval = OK;
	/* update the visual query information only if we set the update flag */
	if ((stat & VQ_SAVE) && _state)
	{
		/* put our message of our intentions */
		IIUGmsg(ERget(S_VQ0040_SaveVQ), FALSE, 0);
		/* set the metaframe state flags */
		mf->state |= _state;
		/* validate the vq, (also sets the error into mf->state if
		   there is an error */
		retval = vq_Validate(mf);
		/* update the state flags */
		IIAMufqUserFrameQuick(mf->apobj);
		/* write the vq info to the dbms */
		IIAMmuMetaUpdate(mf, mf->updmask |= MF_P_VQ);
		/* reset the state flag */
		_state = 0;
	}
	return (retval);
}
/*{
** Name:	vq_SaveEdit	- warn about need to save
**
** Description:
**	Warn the user that his requested edit operation necessitates
**	that the frame be saved, and compiled.
**
** Inputs:
**	METAFRAME *mf;	- metaframe pointer
**	i4 stat;	- status indicator
**
** Outputs:
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**	11/21/89  (tom) - created
*/
LOC STATUS 
vq_SaveEdit(mf, edit_obj)
METAFRAME *mf;
ER_MSGID edit_obj;
{
	STATUS retval = OK;
	/* test if we need to update */
	if (_state)
	{
		/* now do the save */
		retval = vq_SaveIf(mf, VQ_SAVE);
		/* if there were errors then say that we can't edit */
		if (retval != OK)
		{
			IIVQer1(E_VQ007E_No_Edit, ERget(edit_obj));
		}
	}
	return (retval);
}
/*{
** Name:	vq_RunLoop	- Vq run dislay loop 
**
** Description:
**	Run the main display loop for the Visual Query processing.
**
** Inputs:
**	none
**
** Outputs:
**	Returns:
**		bool	- TRUE if we want to be recalled
**			  FALSE if we are really done 
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/16/89  (tom) - created
*/
LOC i4
vq_RunLoop(mf, tab, row)
register METAFRAME *mf;
i4  tab;
  i4 row;
  {
	register i4  i;
	register i4  num;
	i4 stat;
	i4 curfld;
    char fld[MF_MAXTABS][5];
    char fname[20];
	/* form the field names so we can activate on them */
	for (i = 0; i < mf->numtabs; i++)
		STprintf(fld[i], IIVQtfname_fmt, i);
	/* null out the non applicable names */
	for ( ; i < MF_MAXTABS; i++)
		fld[i][0] = EOS;
/* # line 490 "vqdloop.qsc" */	/* display */
    {
      if (IIdispfrm(IIVQform,(char *)"u") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 499 "vqdloop.qsc" */	/* host code */
		/* load the current column and field values to the form */
		IIVQvlfVqLoadFrame(mf);
		/*
		   if we are just re-entering because we had to
		   regenerate the vq in the middle of a join insertion 
		   then resume the join insertion 
			-1 means that we have started the join sequence
			   but have not yet set our first value.
			-2 means that we are not in a join sequence 
		*/
		if (IIVQjtab >= -1)
			goto join_insert;
		/* resume to a particular row of a particular table */ 
		if (tab >= 0)
		{
			STprintf(fname, IIVQtfname_fmt, tab);
			if (row >= 0)
			{
/* # line 520 "vqdloop.qsc" */	/* scroll */
        {
          if (IItbsetio(1,IIVQform,fname,-3) != 0) {
            IItbsmode((char *)"to");
            if (IItscroll(0,row) != 0) {
            } /* IItscroll */
          } /* IItbsetio */
        }
/* # line 521 "vqdloop.qsc" */	/* host code */
			}
/* # line 522 "vqdloop.qsc" */	/* resume */
        {
          IIresfld(fname);
          if (1) goto IIfdB1;
        }
/* # line 523 "vqdloop.qsc" */	/* host code */
		}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 535 "vqdloop.qsc" */	/* host code */
		num = 0;
   col_activate:
		if (vq_ColAct(mf, num) == FALSE)
		{
/* # line 539 "vqdloop.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 540 "vqdloop.qsc" */	/* host code */
		}
/* # line 541 "vqdloop.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 545 "vqdloop.qsc" */	/* host code */
		num = 1; goto col_activate;
          }
        } else if (IIretval() == 3) {
          {
/* # line 549 "vqdloop.qsc" */	/* host code */
		num = 2; goto col_activate;
          }
        } else if (IIretval() == 4) {
          {
/* # line 553 "vqdloop.qsc" */	/* host code */
		num = 3; goto col_activate;
          }
        } else if (IIretval() == 5) {
          {
/* # line 557 "vqdloop.qsc" */	/* host code */
		num = 4; goto col_activate;
          }
        } else if (IIretval() == 6) {
          {
/* # line 561 "vqdloop.qsc" */	/* host code */
		num = 5; goto col_activate;
          }
        } else if (IIretval() == 7) {
          {
/* # line 565 "vqdloop.qsc" */	/* host code */
		num = 6; goto col_activate;
          }
        } else if (IIretval() == 8) {
          {
/* # line 569 "vqdloop.qsc" */	/* host code */
		num = 7; goto col_activate;
          }
        } else if (IIretval() == 9) {
          {
/* # line 573 "vqdloop.qsc" */	/* host code */
		num = 8; goto col_activate;
          }
        } else if (IIretval() == 10) {
          {
/* # line 577 "vqdloop.qsc" */	/* host code */
		num = 9; goto col_activate;
          }
        } else if (IIretval() == 11) {
          {
/* # line 586 "vqdloop.qsc" */	/* host code */
		if (vq_FldAct(mf) == FALSE)
		{
/* # line 588 "vqdloop.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 589 "vqdloop.qsc" */	/* host code */
		}
/* # line 590 "vqdloop.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 12) {
          {
/* # line 600 "vqdloop.qsc" */	/* host code */
		vq_InsLup(mf);
	    repaint_vq:	/* common point to effect repainting */
		stat = VQ_CONT;
/* # line 604 "vqdloop.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 13) {
          {
/* # line 609 "vqdloop.qsc" */	/* host code */
		vq_DelLup(mf);
		goto repaint_vq;
          }
        } else if (IIretval() == 14) {
          {
/* # line 615 "vqdloop.qsc" */	/* host code */
		if (vq_Edit(mf)	== VQ_QUIT)
		{
			stat = VQ_DONE | VQ_SAVE | VQ_QUIT;
/* # line 618 "vqdloop.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 619 "vqdloop.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 15) {
          {
/* # line 624 "vqdloop.qsc" */	/* host code */
		/* see if there are any tables to join */
		if (mf->numtabs < 2) 
		{
			IIVQer1(E_VQ0059_No_Table_Join, 
				ERget(F_VQ002A_AddJoin));
/* # line 629 "vqdloop.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 630 "vqdloop.qsc" */	/* host code */
		}
		/* have we exceeded the maximum number of joins */
		if (mf->numjoins >= MF_MAXJOINS)
		{
			i4 njoins = MF_MAXJOINS;
			IIVQer1(E_VQ0049_Max_Joins, &njoins);
/* # line 637 "vqdloop.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 638 "vqdloop.qsc" */	/* host code */
		}
		/* avoid the resume logic below */
		tab = -1;
		/* we reenter here from above when
		   we are in the middle of a join insertion sequence */
	    join_insert:
		/* make the form displayonly */
		IIVQfmFormMode(mf, TRUE);
/* # line 649 "vqdloop.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE2;
              goto IImuI2;
IIfdI2:;
              if (IIfsetio((char *)0) == 0) goto IIfdE2;
              {
/* # line 652 "vqdloop.qsc" */	/* host code */
			/* resume to a particular row of a 
		   	   particular table */ 
			if (tab >= 0)
			{
				STprintf(fname, IIVQtfname_fmt, tab);
				if (row >= 0)
				{
/* # line 660 "vqdloop.qsc" */	/* scroll */
                {
                  if (IItbsetio(1,IIVQform,fname,-3) != 0) {
                    IItbsmode((char *)"to");
                    if (IItscroll(0,row) != 0) {
                    } /* IItscroll */
                  } /* IItbsetio */
                }
/* # line 661 "vqdloop.qsc" */	/* host code */
				}
/* # line 662 "vqdloop.qsc" */	/* resume */
                {
                  IIresfld(fname);
                  if (1) goto IIfdB2;
                }
/* # line 663 "vqdloop.qsc" */	/* host code */
			}
              }
IIfdB2:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
/* # line 668 "vqdloop.qsc" */	/* host code */
			vq_SetJoin(mf, TRUE);	
/* # line 669 "vqdloop.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE2;
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 674 "vqdloop.qsc" */	/* host code */
			vq_SetJoin(mf, FALSE);	
/* # line 675 "vqdloop.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE2;
                    }
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 680 "vqdloop.qsc" */	/* host code */
			i = IIVQcfCurFld() + 1;
			if (i >= mf->numtabs)
			{
				i = 0;
			}
			STprintf(fname, IIVQtfname_fmt, i);
/* # line 686 "vqdloop.qsc" */	/* resume */
                    {
                      IIresfld(fname);
                      if (1) goto IIfdB2;
                    }
                  }
                } else if (IIretval() == 4) {
                  {
/* # line 691 "vqdloop.qsc" */	/* host code */
			if (IIVQjtab < 0)
			{
				/* if not yet set a join give one help */
				FEhelp(ERx("vqjoin0.hlp"), 
					ERget(F_VQ00B9_add_join));
			}
			else
			{
				/* else use a different help file */
				FEhelp(ERx("vqjoin1.hlp"), 
					ERget(F_VQ00B9_add_join));
			}
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
              if (IInmuact(ERget(F_VQ0025_JoinCol),ERget(F_VQ005D_JoinColExpl)
              ,2,2,1) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_Cancel),ERget(F_VQ005E_CancelExpl),2,2,2) 
              == 0) goto IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(9,(char *)0,2,2,2) == 0) goto IIfdE2;
              if (IInmuact(ERget(F_VQ002D_NextTable),
              ERget(F_VQ0061_NextTableExpl),2,2,3) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_Help),ERget(F_VQ005F_HelpExpl),2,2,4) == 0
              ) goto IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE2;
              if (IIendmu() == 0) goto IIfdE2;
              goto IIfdI2;
IIfdE2:;
              IIendnest();
            }
/* # line 705 "vqdloop.qsc" */	/* host code */
		/* reset normal vq form mode */
		IIVQfmFormMode(mf, FALSE);
		goto repaint_vq;
          }
        } else if (IIretval() == 16) {
          {
/* # line 713 "vqdloop.qsc" */	/* host code */
		vq_DelJoin(mf);
		goto repaint_vq;
          }
        } else if (IIretval() == 17) {
          {
/* # line 719 "vqdloop.qsc" */	/* host code */
		i = IIVQcfCurFld() + 1;
		if (i >= mf->numtabs)
		{
			i = 0;
		}
		STprintf(fname, IIVQtfname_fmt, i);
/* # line 725 "vqdloop.qsc" */	/* resume */
            {
              IIresfld(fname);
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 18) {
          {
/* # line 730 "vqdloop.qsc" */	/* host code */
		MFCOL *col;
		i4 tabidx;
            i4 colidx;
            i4 tbl;
		char  table_name[FE_MAXTABNAME+1];
		/* get table and column indices of user's current position */
		tabidx = IIVQcfCurFld();
/* # line 739 "vqdloop.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,IIVQform,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&tbl,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 740 "vqdloop.qsc" */	/* host code */
		if (tbl == 1)	/* cursor in table field */
		{
		    /* set which column name to scroll to in Examine-a-Table */
/* # line 743 "vqdloop.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,IIVQform,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&colidx,29,(char *)0,0);
              } /* IIiqset */
            }
/* # line 744 "vqdloop.qsc" */	/* host code */
		    colidx--;       /* zero based */
		    col = mf->tabs[tabidx]->cols[colidx];
		}
		else
		{
		    col = NULL;
		}
		/* Can't pass METAFRAME version of table name to Tables/Examine
		   cause Tables writes to it if user selects NewTable. */
		STcopy (mf->tabs[tabidx]->name, table_name);
		/* call Tables Utility Examine-a-Table */
		IITUtbiTableinfo(table_name,
				NULL,		/* should be table owner */
				IIabExename,
	    			((col == NULL) ? (char *)NULL : col->name));
          }
        } else if (IIretval() == 19) {
          {
/* # line 766 "vqdloop.qsc" */	/* host code */
		for (;;)
/* # line 767 "vqdloop.qsc" */	/* submenu */
            {
              goto IImuI3;
IIfdB3:
              IIrunmu(0);
              if (IIretval() == 1) {
                {
/* # line 771 "vqdloop.qsc" */	/* host code */
			IIIMnaive(*IIar_Dbname, "", TRUE, FALSE);
			break;
                }
              } else if (IIretval() == 2) {
                {
/* # line 777 "vqdloop.qsc" */	/* call */
                  {
                    IIutsys(0,(char *)"system",(char *)0);
                    IIutsys(1,(char *)"command",(char *)"");
                    IIutsys(2,(char *)0,(char *)0);
                  }
/* # line 778 "vqdloop.qsc" */	/* host code */
			break;
                }
              } else if (IIretval() == 3) {
                {
/* # line 784 "vqdloop.qsc" */	/* host code */
			FEhelp(ERx("vqutilty.hlp"), 
				ERget(F_VQ0063_UtilitiesExpl));
                }
              } else if (IIretval() == 4) {
                {
/* # line 791 "vqdloop.qsc" */	/* host code */
			break;
                }
              } else {
                if (1) goto IIfdE3;
              } /* IIretval */
              goto IIfdE3;
IImuI3:
              IImuonly();
              if (IIinitmu() == 0) goto IIfdE3;
              if (IInmuact(ERget(F_VQ0069_IngMenu),ERget(F_VQ006A_IngMenuExpl)
              ,2,2,1) == 0) goto IIfdE3;
              if (IInmuact(_shell,ERget(F_FE010A_ExplShell),2,2,2) == 0) goto 
              IIfdE3;
              if (IInmuact(ERget(FE_Help),ERget(F_VQ006C_HelpUtilExpl),2,2,3) 
              == 0) goto IIfdE3;
              IIFRInternal(0);
              if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE3;
              if (IInmuact(ERget(FE_End),ERget(F_VQ006B_EndUtilExpl),2,2,4) ==
               0) goto IIfdE3;
              IIFRInternal(0);
              if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE3;
              if (IIendmu() == 0) goto IIfdE3;
              goto IIfdB3;
IIfdE3:;
            }
          }
        } else if (IIretval() == 20) {
          {
/* # line 797 "vqdloop.qsc" */	/* host code */
		/* give user the compressed view of the tables */
		curfld = IIVQdcvDisplayCompView(mf, IIVQcfCurFld());
		/* form field name */
		STprintf(fname, IIVQtfname_fmt, curfld);
		/* resume to the field that the user was last on in
		   the compressed view. */
/* # line 805 "vqdloop.qsc" */	/* resume */
            {
              IIresfld(fname);
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 21) {
          {
/* # line 811 "vqdloop.qsc" */	/* host code */
		FEhelp(ERx("vqmain.hlp"), ERget(F_VQ00BC_vq_prmpt));
          }
        } else if (IIretval() == 22) {
          {
/* # line 816 "vqdloop.qsc" */	/* host code */
		stat = VQ_DONE;
/* # line 817 "vqdloop.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 23) {
          {
/* # line 822 "vqdloop.qsc" */	/* host code */
		stat = VQ_DONE | VQ_SAVE;
/* # line 823 "vqdloop.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 24) {
          {
/* # line 828 "vqdloop.qsc" */	/* host code */
		if (_state)	/* if there were changes */
		{
			switch (IIUFccConfirmChoice(CONF_QUIT, mf->apobj->name, 
				ERget(FE_Frame), ERget(F_VQ008C_QuitvqTitle), 
				ERx("vqquitvq.hlp")))
			{
			case CONFCH_YES:
				stat = VQ_DONE | VQ_SAVE | VQ_QUIT;
				break;
			case CONFCH_NO:
				stat = VQ_DONE | VQ_QUIT;
				break;
			case CONFCH_CANCEL:
/* # line 843 "vqdloop.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 844 "vqdloop.qsc" */	/* host code */
			}
		}
		else
		{
			stat = VQ_DONE | VQ_QUIT;
		}
/* # line 850 "vqdloop.qsc" */	/* breakdisplay */
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
      if (IITBcaClmAct(fld[0],(char *)"all",0,1) == 0) goto IIfdE1;
      if (IITBcaClmAct(fld[1],(char *)"all",0,2) == 0) goto IIfdE1;
      if (IITBcaClmAct(fld[2],(char *)"all",0,3) == 0) goto IIfdE1;
      if (IITBcaClmAct(fld[3],(char *)"all",0,4) == 0) goto IIfdE1;
      if (IITBcaClmAct(fld[4],(char *)"all",0,5) == 0) goto IIfdE1;
      if (IITBcaClmAct(fld[5],(char *)"all",0,6) == 0) goto IIfdE1;
      if (IITBcaClmAct(fld[6],(char *)"all",0,7) == 0) goto IIfdE1;
      if (IITBcaClmAct(fld[7],(char *)"all",0,8) == 0) goto IIfdE1;
      if (IITBcaClmAct(fld[8],(char *)"all",0,9) == 0) goto IIfdE1;
      if (IITBcaClmAct(fld[9],(char *)"all",0,10) == 0) goto IIfdE1;
      if (IIFRafActFld((char *)"all",0,11) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(F_VQ0028_AddTable),ERget(F_VQ0059_AddTableExpl),2,2,
      12) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_VQ0029_DelTable),ERget(F_VQ005A_DelTableExpl),2,2,
      13) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Edit),ERget(F_VQ005B_EditExpl),2,2,14) == 0) goto 
      IIfdE1;
      if (IInmuact(ERget(F_VQ002A_AddJoin),ERget(F_VQ005C_AddJoinExpl),2,2,15)
       == 0) goto IIfdE1;
      if (IInmuact(ERget(F_VQ002B_DelJoin),ERget(F_VQ0060_DelJoinExpl),2,2,16)
       == 0) goto IIfdE1;
      if (IInmuact(ERget(F_VQ002D_NextTable),ERget(F_VQ0061_NextTableExpl),2,2
      ,17) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_VQ002C_TableDef),ERget(F_VQ0062_TableDefExpl),2,2,
      18) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Utilities),ERget(F_VQ0063_UtilitiesExpl),2,2,19) 
      == 0) goto IIfdE1;
      if (IInmuact(ERget(F_VQ0027_Compress),ERget(F_VQ0064_CompressExpl),2,2,
      20) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_VQ0065_HelpExpl),2,2,21) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,21) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Cancel),ERget(F_VQ0066_CancelExpl),2,2,22) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,2,2,22) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_VQ0067_EndExpl),2,2,23) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,23) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Quit),ERget(F_VQ0068_QuitExpl),2,2,24) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,2,2,24) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 854 "vqdloop.qsc" */	/* host code */
	return (stat);
  }
/*{
** Name:	vq_FldAct	- field activation function
**
** Description:
**	This function finds out what field it was that activated,
**	and processes the necessary changes to the metaframe 
**	structure.
**
** Inputs:
**	METAFRAME *mf;	- pointer to the metaframe structure
**	
** Outputs:
**	Returns:
**		none
**	Exceptions:
**
** Side Effects:
**
** History:
**	08/09/89	- tom created
*/
LOC bool 
vq_FldAct(mf)
register METAFRAME *mf;
{
	i4 idx;
  i4 changed;
	i4 flag;
	MFTAB *tab;
  char value[25];
	/* set index and also sets IIVQcfnCurFldName */
	idx = IIVQcfCurFld();
/* # line 891 "vqdloop.qsc" */	/* inquire_frs */
  {
    if (IIiqset(2,0,IIVQform,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&changed,20,IIVQcfnCurFldName,0);
    } /* IIiqset */
  }
/* # line 896 "vqdloop.qsc" */	/* host code */
	/* if the field has not changed, then return */
	if (changed == 0)
	{
		return (TRUE);
	}
	tab = mf->tabs[idx];
	/* the last character (4th) of the name of the field
	   is an indicator of which flag is to be used, 
	   see vqmakfrm.qsc */
	switch(IIVQcfnCurFldName[4])
	{
	case 'q':
		flag = TAB_QUALLUP;
		break;
	case 'i':
		flag = TAB_INSFLG;
		break;
	case 'd':
		flag = TAB_DELFLG;
		break;
	}
/* # line 922 "vqdloop.qsc" */	/* getform */
  {
    if (IIfsetio(IIVQform) != 0) {
      IIgetfldio((short *)0,1,32,24,value,IIVQcfnCurFldName);
    } /* IIfsetio */
  }
/* # line 925 "vqdloop.qsc" */	/* host code */
	if (value[0] == IIVQ_y_text[0])
	{
		if (!(tab->flags & flag))
		{
			tab->flags |= flag;
			_state |= MFST_DOGEN;
		}
	}
	else if (value[0] == IIVQ_n_text[0])
	{
		if (tab->flags & flag)
		{
			tab->flags &= ~flag;
			_state |= MFST_DOGEN;
		}
	}
	else 
	{
		IIVQer2(E_VQ006B_Yes_No_Field, IIVQ_y_text, IIVQ_n_text);
		if (tab->flags & flag)
		{
/* # line 946 "vqdloop.qsc" */	/* putform */
  {
    if (IIfsetio(IIVQform) != 0) {
      IIputfldio(IIVQcfnCurFldName,(short *)0,1,32,0,IIVQ_y_text);
    } /* IIfsetio */
  }
/* # line 947 "vqdloop.qsc" */	/* host code */
		}
		else
		{
/* # line 950 "vqdloop.qsc" */	/* putform */
  {
    if (IIfsetio(IIVQform) != 0) {
      IIputfldio(IIVQcfnCurFldName,(short *)0,1,32,0,IIVQ_n_text);
    } /* IIfsetio */
  }
/* # line 951 "vqdloop.qsc" */	/* host code */
		}
		return (FALSE);
	}
	return (TRUE);
}
/*{
** Name:	vq_SetJoin	- set join 
**
** Description:
**	This function is called when the user says that he wants to
**	set a join column. This is called once each, for each end
**	of the join. It is also called to kill an in progress join
**	sequence.
**	This fucntion inquires on the form system as to the
**	table and column the user is refering to. This information
**	is then posted into global variables which are read
**	by the vq frame making code.
**
** Inputs:
**	METAFRAME *mf;	- pointer to the metaframe structure
**	bool state;	- state of the call:
**				FALSE - means to cancel the join
**				TRUE - means to process it 
** Outputs:
**	Returns:
**		none
**	Exceptions:
**
** Side Effects:
**
** History:
**	05/15/89	- tom created
*/
LOC VOID
vq_SetJoin(mf, state)
register METAFRAME *mf;
  bool state;
  {
	register i4  i;
	MFCOL *col;
	MFTAB *tab;
	i4 tabidx;
    i4 colidx;
	bool is_lookup;
	if (state == FALSE) 
	{
		IIVQjtab = -2; 		/* say we are no longer in join */
		return;
	}
	else if (IIVQjtab == -2)	/* if we weren't in join before */
	{
		IIVQjtab = -1;		/* now we are */
	}
	/* get the table and column indices of the user's current position */
	tabidx = IIVQcfCurFld();
/* # line 1010 "vqdloop.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,IIVQform,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&colidx,29,(char *)0,0);
      } /* IIiqset */
    }
/* # line 1011 "vqdloop.qsc" */	/* host code */
	colidx--;	/* zero based */
	tab = mf->tabs[tabidx];
	col = tab->cols[colidx];
	/* if it's a lookup table, 
	   and it has a join already.. it's an error */
	if (tab->usage == TAB_LOOKUP)
	{
		is_lookup = TRUE;
		/* scan the joins to see if the table has been mentioned */ 
		for (i = 0; i < mf->numjoins; i++)
		{
			if (tabidx == mf->joins[i]->tab_2)
			{
				IIVQer2(E_VQ004A_2nd_LupJoin,
					tab->name,
					ERget(FE_Cancel));
				return;
			}
		}
	}
	/* mustn't join again to columns that are already 
	   subordinate join fields */
	else if (col->flags & COL_SUBJOIN)
	{
		IIVQer1(E_VQ004B_Already_Joined,
			ERget(FE_Cancel));
		return;
	}
	else
	{
		is_lookup = FALSE;
	}
	/* if first join table is already set.. then we complete the join */ 
	if (IIVQjtab >= 0)
	{
		/* if user is trying to join to the same table.. */
		if (tabidx == IIVQjtab)
		{
			IIVQer2(E_VQ004C_Same_Table,
				tab->name,
				ERget(FE_Cancel));
			return;
		}
		/* switch on the or'ed result of whether the two
		   specified tables to the joins are lookup */ 
		switch ( (is_lookup << 1) 
		       | (mf->tabs[IIVQjtab]->usage == TAB_LOOKUP)
		       )
		{
		case 0:		/* neither are lookups */
			break;
		case 1:
		case 2:		/* one and only one table is a lookup */
			/* they must be in the same section */ 
			if (mf->tabs[IIVQjtab]->tabsect 
				!= tab->tabsect)
			{
				IIVQer3(E_VQ004D_Dif_Section,
					tab->name,
					mf->tabs[IIVQjtab]->name,
					ERget(FE_Cancel));
				return;
			}
			/* if there is another join proceeding from
			   either end of the proposed join.. then we
			   must check that the user is not trying
			   to join a particular column to 2 lookup
			   tables */
			if (vq_CheckJoin(mf, IIVQjtab, IIVQjcol, tabidx) 
					== FALSE)
			{
				IIVQer2(E_VQ004E_Dbl_Lupjoin,
					tab->name,
					ERget(FE_Cancel));
				return;
			}
			if (vq_CheckJoin(mf, tabidx, colidx, IIVQjtab) 
					== FALSE)
			{
				IIVQer2(E_VQ004E_Dbl_Lupjoin,
					mf->tabs[IIVQjtab]->name,
					ERget(FE_Cancel));
				return;
			}
			break;
		case 3:		/* both are lookups */
			IIVQer3(E_VQ004F_Both_Lups,
				tab->name,
				mf->tabs[IIVQjtab]->name,
				ERget(FE_Cancel));
			return;
		}
		/* ok..  we have a good join.. so put into the join list */
		/* check for type incompatibility */
		if (IIVQjcJoinCheck(col, mf->tabs[IIVQjtab]->cols[IIVQjcol])
			== FALSE)
		{
			IIVQer4(E_VQ007A_Join_Type,
				tab->name, col->name,
				mf->tabs[IIVQjtab]->name,
				mf->tabs[IIVQjtab]->cols[IIVQjcol]->name);
			return;
		}
		/* warn user if there are potential problems */
		_VOID_ IIVQjwJoinWarn(
			tab->name, mf->tabs[IIVQjtab]->name,
			col, mf->tabs[IIVQjtab]->cols[IIVQjcol]);
		/* make tabidx,colidx be the lower indexed table */
		if (tabidx > IIVQjtab)
		{
			i = tabidx;		/* swap them */
			tabidx = IIVQjtab;
			IIVQjtab = i;
			i = colidx;
			colidx = IIVQjcol;
			IIVQjcol = i;
		}
		/* put join into tables */
		vq_PutJoin(mf, tabidx, colidx, IIVQjtab, IIVQjcol);
		/* indicate we are done by reseting the join table to -2 */
		IIVQjtab = -2;
		/* anytime we change joins we have to regen code,
		   also the form cause of special highlighting on join fields */
		_state |= MFST_DOGEN;
	}
	/* the user is just starting the join */
	else
	{
		IIVQjtab = tabidx;
		IIVQjcol = colidx;	
	}
  }
/*{
** Name:	vq_PutJoin - add the join to the metaframe
**
** Description:
**	Put a new join into the metaframe.
**
** Inputs:
**	METAFRAME *mf;	- ptr to the metaframe structure
**	i4 t1;	- index of table one (the higher up table)
**	i4 c1;	- index of column in table one
**	i4 t2;	- index of table two 
**	i4 c2;	- index of column in table two 
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
** Side Effects:
**		allocates memory
**
** History:
**	07/04/89 (tom) - written
*/
LOC VOID 
vq_PutJoin(mf, t1, c1, t2, c2)
METAFRAME *mf;
register i4  t1;
i4  c1;
i4  t2;
i4  c2;
{
	register i4  i;
	register MFJOIN *j;
	MFCOL *col;
	i4 n;
	/* find where this join sorts into the list */
	for (i = 0; i < mf->numjoins; i++)
	{
		j = mf->joins[i];
		if (t1 < j->tab_1)
			break;
		if (t1 == j->tab_1 && c1 < j->col_1)
			break;
	}
	/* allocate a new join struct on the vq tag */
	j = (MFJOIN*)IIVQ_VAlloc(sizeof(MFJOIN), (char*)NULL);
	/* open the list up */
	for (n = i, i = mf->numjoins; i > n; i--)
	{
		mf->joins[i] = mf->joins[i - 1];
	}
	j->tab_1 = t1;
	j->col_1 = c1; 
	j->tab_2 = t2; 
	j->col_2 = c2; 
	mf->joins[n] = j; 
	mf->numjoins++; 
	/* set the column flags with the join information */
	col = mf->tabs[t1]->cols[c1];
	if (i = (mf->tabs[t2]->usage == TAB_LOOKUP))
	{
		/* lookup table joins require the column
		   on the major table to be displayed */
		col->flags |= COL_LUPJOIN;
		vq_ColChanged(col, IIVQ_y_text);
	}
	else
	{
		col->flags |= COL_DETJOIN;
	}
	col = mf->tabs[t2]->cols[c2];
	col->flags |= COL_SUBJOIN;
	/* subordinate join field columns may not be used nor may it create
	   a variable */
	vq_ColChanged(col, IIVQ_n_text);
	/* set the join type based on the table sections */
	if (i /* if it's a lookup */)
	{
		if (mf->tabs[t1]->tabsect == TAB_MASTER)
		{
			j->type = JOIN_MASLUP; /* master->lookup */
		}
		else
		{
			j->type = JOIN_DETLUP; /* detail->lookup */
		}
	}
	else
	{
		j->type = JOIN_MASDET; /* master->detail */
	}
}
/*{
** Name:	vq_CheckJoin - check join for validity 
**
** Description:
**	If tab2 of the join is a lookup table then
**	this function checks that the other end is not already
**	joined to a lookup table.
**
** Inputs:
**	METAFRAME *mf;	- ptr to the metaframe structure
**	i4 tab1;	- index of table one
**	i4 col1;	- index of column in table one
**	i4 tab2;	- index of table two 
**
** Outputs:
**	Returns:
**		bool - FALSE for error else TRUE
**
**	Exceptions:
** Side Effects:
**
** History:
**	06/02/89 (tom) - written
*/
LOC bool
vq_CheckJoin(mf, tab1, col1, tab2)
register METAFRAME *mf;
register i4  tab1;
register i4  col1;
register i4  tab2;
{
	return
	(	mf->tabs[tab2]->usage == TAB_LOOKUP
	&&	(mf->tabs[tab1]->cols[col1]->flags & COL_LUPJOIN)
	) == FALSE;
}
/*{
** Name:	vq_ColAct	- process a column activation from vq
**
** Description:
**	This routine is called whenever a column activation occurs
**	on the visual query frame.
**	Interactions take place with the form system to ensure
**	that the change was valid.
**	Also if there was a change.. then the METFRAME is posted
**	and marked as changed. This avoids having to unload
**	all of the tables each time we have to recreate the form
**	(or even at exit).
**
** Inputs:
**	METAFRAME *mf;	- metaframe structure
**	i4 tabnum;	- index into table array
**
** Outputs:
**	Returns:
**		bool	TRUE - resume next,
**			FALSE - resume current.. cause an error 
**				occured.
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/25/89 (tom) - created
*/
LOC bool
vq_ColAct(mf, tabnum)
METAFRAME *mf;
i4  tabnum;
  {
	MFTAB *tab;
	MFCOL *col;
	bool ret;
    i4 changed;
    i4 rowno;
    char *p;
    char colname[20];
    char value[250];
	/* sets IIVQcfnCurFldName */
	_VOID_ IIVQcfCurFld();
/* # line 1343 "vqdloop.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,IIVQform,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&rowno,29,IIVQcfnCurFldName,0);
        IIiqfsio((short *)0,1,32,19,colname,34,IIVQcfnCurFldName,0);
      } /* IIiqset */
    }
/* # line 1348 "vqdloop.qsc" */	/* inquire_frs */
    {
      if (IIiqset(5,0,IIVQform,IIVQcfnCurFldName,(char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&changed,20,colname,0);
      } /* IIiqset */
    }
/* # line 1353 "vqdloop.qsc" */	/* host code */
	ret = TRUE; /* assume all is ok, and we want to just resume next */
	/* if the column has not changed, then return */
	if (changed == 0)
		return (TRUE);
	/* the column has changed so process various fields as necessary */
	rowno--;	/* make row number 0 based for indexing */
	/* set pointers to the current table and column */
	tab = mf->tabs[tabnum];
	col = tab->cols[rowno];
	/* if it's the field inclusion field.. and it's an append table
	   then we must set up the indicator field appropriatley */
	if (STequal(colname, IIVQinc_col))
	{
/* # line 1372 "vqdloop.qsc" */	/* getrow */
    {
      if (IItbsetio(2,IIVQform,IIVQcfnCurFldName,-2) != 0) {
        IItcogetio((short *)0,1,32,249,value,IIVQinc_col);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1374 "vqdloop.qsc" */	/* host code */
		if (value[0] == IIVQ_y_text[0])
		{
			/* if it's a subordinate join field, then 
			   the user isn't allowed to display it */
			if (col->flags & COL_SUBJOIN)
			{
				IIVQer0(E_VQ0051_Bad_Star);
/* # line 1381 "vqdloop.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,IIVQcfnCurFldName,-2) != 0) {
        IItcoputio(IIVQinc_col,(short *)0,1,32,0,IIVQ_n_text);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1383 "vqdloop.qsc" */	/* host code */
			}
			else	/* the column has changed */
			{
				vq_ColChanged(col, IIVQ_y_text);
				/* reload the frame table */
				(VOID) IIVQlftLoadFrameTable(tab, tabnum);
			}
		}
		else if (value[0] == IIVQ_n_text[0] || 
			 value[0] == IIVQ_v_text[0])
		{
			/* if this is superior join field then we must
			   display and the user must not unstar */
			if (col->flags & COL_LUPJOIN)
			{
				IIVQer0(E_VQ0052_Bad_NoStar);
/* # line 1400 "vqdloop.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,IIVQcfnCurFldName,-2) != 0) {
        IItcoputio(IIVQinc_col,(short *)0,1,32,0,IIVQ_y_text);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1402 "vqdloop.qsc" */	/* host code */
			}
			else
			{
				vq_ColChanged(col, value);
				/* reload the frame table */
				(VOID) IIVQlftLoadFrameTable(tab, tabnum);
			}
		}
		else	/* user made a mistake, so give error and fix it */
		{
			IIVQer0(E_VQ0050_Bad_Inc);
			p = IIVQgivGetIncVal(col);
/* # line 1415 "vqdloop.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,IIVQcfnCurFldName,-2) != 0) {
        IItcoputio(IIVQinc_col,(short *)0,1,32,0,p);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1417 "vqdloop.qsc" */	/* host code */
		}
	}
	/* the sort column for display and update tables */
	else if (STequal(colname, IIVQsrt_col))
	{
		ret = vq_NumAct(tab, tabnum, col, 
			IIVQcfnCurFldName, IIVQsrt_col);
	}
	/* the order column for lookup tables */
	else if (STequal(colname, IIVQord_col))
	{
		/* must be a lookup table */
		ret = vq_NumAct(tab, tabnum, col, 
			IIVQcfnCurFldName, IIVQord_col);
	}
	/* the sort direction column for update/browse tables */
	else if (STequal(colname, IIVQdir_col))
	{
		ret = vq_DirAct(col, IIVQcfnCurFldName);
	}
	/* the info column containing various restrictions and expressions */
	else if (STequal(colname, IIVQexp_col))
	{
/* # line 1440 "vqdloop.qsc" */	/* getrow */
    {
      if (IItbsetio(2,IIVQform,IIVQcfnCurFldName,-2) != 0) {
        IItcogetio((short *)0,1,32,249,value,IIVQexp_col);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1442 "vqdloop.qsc" */	/* host code */
		/* if it's a lookup table colum that isn't ordered and
		   the user tried to enter into the title field. */
		if (	value[0] != EOS
		   &&	tab->usage == TAB_LOOKUP 
		   &&	col->corder == 0
		   )
		{
			IIVQer0(E_VQ0053_Order_First);
/* # line 1450 "vqdloop.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,IIVQcfnCurFldName,-2) != 0) {
        IItcoputio(IIVQexp_col,(short *)0,1,32,0,_nullstring);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1452 "vqdloop.qsc" */	/* host code */
		}
		/* check if it's not equal to the last value */
		else if (!STequal(value, col->info))
		{
			/* if it's an append table, and it's in a tf */
			if (  tab->usage == TAB_APPEND
			   && (  tab->tabsect >= TAB_DETAIL
			      || (mf->mode & MFMODE) == MF_MASTAB
			      )
			   )
			{
				/* ..and the column is to be used or to become
				   a variable then we don't support default 
				   values */
				if (col->flags & (COL_USED|COL_VARIABLE))
				{
					IIVQer2(E_VQ00CD_No_Def_Vals, 
						tab->name, col->name);
/* # line 1470 "vqdloop.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,IIVQcfnCurFldName,-2) != 0) {
        IItcoputio(IIVQexp_col,(short *)0,1,32,0,_nullstring);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1472 "vqdloop.qsc" */	/* host code */
					return (ret);
				}
				/* if it's subordinate join.. then we don't
				   support assign.. cause we are assigning
				   from the value of the master table join col*/
				if (col->flags & COL_SUBJOIN)
				{
					IIVQer0(E_VQ00CF_No_Asgn);
/* # line 1481 "vqdloop.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,IIVQcfnCurFldName,-2) != 0) {
        IItcoputio(IIVQexp_col,(short *)0,1,32,0,_nullstring);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1483 "vqdloop.qsc" */	/* host code */
					return (ret);
				}
			}
			/* if there is a value, and the table is not a lookup, 
			   and we are dealing with a subordinate join column..
			   then give an error message, cause a restriction 
			   doesn't make sense */
			else if (  tab->usage != TAB_LOOKUP 
				&& (col->flags & COL_SUBJOIN)
				)
			{
				IIVQer0(E_VQ00D0_No_Restrict);
/* # line 1496 "vqdloop.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,IIVQcfnCurFldName,-2) != 0) {
        IItcoputio(IIVQexp_col,(short *)0,1,32,0,_nullstring);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1498 "vqdloop.qsc" */	/* host code */
				return (ret);
			}
			col->info = (char*) IIVQ_VAlloc(0, value);
			if (IIVQcsqCheckSeq(mf, tab, col) == FALSE)
			{
/* # line 1504 "vqdloop.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,IIVQcfnCurFldName,-2) != 0) {
        IItcoputio(IIVQexp_col,(short *)0,1,32,0,_nullstring);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1506 "vqdloop.qsc" */	/* host code */
				col->info = _nullstring;
				ret = FALSE;
			}
			_state |= MFST_DOGEN;
		}
	}
	return (ret);
  }
/*{
** Name:	IIVQcsqCheckSeq	- check for presence of a sequence field
**
** Description:
**	The user just changed the value in an info field.. and we 
**	must check if they have specified a 'sequence' field, and
**	if it is legal.
**	
** Inputs:
**	METAFRAME *mf;	- ptr to metaframe struct 
**	MFTAB *tab;	- ptr to relevent table structure
**	MFCOL *col;	- ptr to relevent column structure
**
** Outputs:
**	Returns:
**		TRUE if user entry is legal
**		FALSE otherwise
**	Exceptions:
**
** Side Effects:
**
** History:
**	08/29/89 (tom) - created
**	07/18/89 (Mike S) - Made public
*/
bool 
IIVQcsqCheckSeq(mf, tab, col)
register METAFRAME *mf;
register MFTAB *tab;
register MFCOL *col;
{
	static char *_seqtxt;
	static i4  _seqlen = 0;
	register i4  i;
	/* if it's not an append table in the master section, return */
	if ( !(  tab->usage == TAB_APPEND 
	      && (  tab->tabsect == TAB_MASTER
	         && ! ( (mf->mode & MFMODE) == MF_MASTAB )
		 )
	      )
	   )
	{
		return (TRUE);
	} 
	/* initially turn the sequence bit off for this column */
	col->flags &= ~COL_SEQUENCE;
	/* if not yet initialized.. read message file. */
	if (_seqlen == 0)
	{
		_seqtxt = ERget(F_VQ0105_Seq_Text);
		_seqlen = STlength(_seqtxt);
	}
	/* see if the user typed in the word "sequenced" */
	if (  STlength(col->info) != _seqlen 
	   || STbcompare(_seqtxt, _seqlen, col->info, _seqlen, TRUE) != 0
	   )
	{
		return (TRUE);
	}
	/* if it's not an i4 type.. then we give an error..
	   note: if it's negative.. means nulls are allowed, and they
	   need 1 byte added to the length for null indicator.. */
	if (  abs(col->type.db_datatype) != DB_INT_TYPE
	   || col->type.db_length != 4 + (col->type.db_datatype < 0)
	   )
	{
		IIVQer3(E_VQ0072_Seq_3, col->name, tab->name, _seqtxt);
		return (FALSE);
	}
	/* make sure that there is only one column of the table 
	   which is sequenced */
	for (i = 0; i < tab->numcols; i++)
	{
		if (tab->cols[i]->flags & COL_SEQUENCE)
		{
			IIVQerr(E_VQ0071_Seq_2, 4, tab->cols[i]->name,
				tab->name, col->name, _seqtxt);
			return (FALSE);
		}
	}
	col->flags |= COL_SEQUENCE;
	return (TRUE);
}
/*{
** Name:	vq_NumAct	- process activation on numeric column
**
** Description:
**	The user just activated on a numeric sort or order column.
**	A column in a table field must be a number or spaces.. 
**	if it isn't.. blank out the column and return false.
**	If it's either spaces, or a valid number.. then
**	we renumber the rest of the column's numbers.
**	
**
** Inputs:
**	METAFRAME *mf;	- ptr to metaframe structure
**	MFTAB *tab;	- ptr to relevent table structure
**	i4 tabnum;	- table index 
**	MFCOL *col;	- ptr to relevent column structure
**	char *fname;	- name of the table field
**	char *cname;	- name of the table field column
**
** Outputs:
**	Returns:
**		bool	- TRUE if the user's input was good
**			  else FALSE
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/08/89 (tom) - created
**	06/11/06 (kiria01) b117042 Conform CMxxxxx macro calls to relaxed bool form
*/
LOC bool
vq_NumAct(tab, tabnum, col, fname, cname)
register MFTAB *tab;
i4  tabnum;
MFCOL *col;
  char *fname;
  char *cname;
  {
	register i4  i;
	register i4  old;
	i4 val;
	i4 maxval;
	char *p;
	bool digit_found;
    char value[20];
/* # line 1657 "vqdloop.qsc" */	/* getrow */
    {
      if (IItbsetio(2,IIVQform,fname,-2) != 0) {
        IItcogetio((short *)0,1,32,19,value,cname);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1659 "vqdloop.qsc" */	/* host code */
	digit_found = FALSE;
	/* scan string, must contain nothing but spaces or digits,
	   note that because it is only 2 chars wide.. it can't contain
	   invalid numbers such as "2 6" */
	for (p = value; *p; p++)
	{
		if (CMdigit(p))
		{
			digit_found = TRUE;
		}	
		else if (!CMspace(p))
		{
			/* if not then we blank the field and return error */
			digit_found = FALSE;
			value[0] = EOS;
/* # line 1675 "vqdloop.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,fname,-2) != 0) {
        IItcoputio(cname,(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1676 "vqdloop.qsc" */	/* host code */
			return (FALSE);
		}
	}
	if (digit_found)
	{
		CVan(value, &val);
	}
	else
	{
		val = 0;
	}
	old = col->corder;
	/* find current max value for the table field */
	for (maxval = i = 0; i < tab->numcols; i++)
	{
		if (tab->cols[i]->corder > maxval)
			maxval = tab->cols[i]->corder;
	}
	/* if new posting is higher than the highest, minimize it */
	if (val > maxval)
	{
		val = maxval;	/* initially limit to maxval */ 
		/* if there was no old value.. and we were more than maxval.. 
		   then bump by one to make the new one be the new top */
		if (old == 0)
		{
			val++;
		}
	}
	/* if user entered same value into the column, we're done */
	if (val == old)
	{
		return (TRUE);
	}
	/* if there is a value, and the table is not a lookup, 
	   we are dealing with a subordinate join column..
	   then give an error message */
	if (  val != 0  
	   && tab->usage != TAB_LOOKUP 
	   && (col->flags & COL_SUBJOIN))
	{
		IIVQer0(E_VQ00D1_No_Sort);
/* # line 1726 "vqdloop.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,IIVQcfnCurFldName,-2) != 0) {
        IItcoputio(cname,(short *)0,1,32,0,_nullstring);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1728 "vqdloop.qsc" */	/* host code */
		return (TRUE);
	}
	/******
	  there are 4 conditions which dicate different reordering:
	     - inserting a brand new ordered column (old == 0)
	     - 0'ing out an previously ordered col (val == 0)
	     - we are overwriting with a larger number (val > old) 
	     - we are overwriting with a smaller number (else..)
	******/
	if (old == 0)
	{
		/* if the col was't previously ordered, then we 
		   will default the column to ascending */
		col->flags &= ~COL_DESCEND;
		vq_Reorder(tab, maxval, val, -1);
	}
	else if (val == 0)
	{
		vq_Reorder(tab, old + 1, maxval, 1);
	}
	else if (val > old)
	{
		vq_Reorder(tab, old + 1, val, 1);
	}
	else
	{
		vq_Reorder(tab, old - 1, val, -1);
	}
	col->corder = val;
	_state |= MFST_DOGEN;
	/* reload the frame table */
	(VOID) IIVQlftLoadFrameTable(tab, tabnum);
	return (TRUE);
  }
/*{
** Name:	vq_DirAct	- process activation on direction field 
**
** Description:
** 	Processes activations on the a/d ascending/descending column
**	associated with sorting.
**
** Inputs:
**	MFCOL *col;	- ptr to relevent column structure
**	char *fname;	- name of the table field
**
** Outputs:
**	Returns:
**		TRUE
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/08/89 (tom) - created
*/
LOC bool
vq_DirAct(col, f_name)
MFCOL *col;
  char *f_name;
  {
    char value[20];
/* # line 1798 "vqdloop.qsc" */	/* getrow */
    {
      if (IItbsetio(2,IIVQform,f_name,-2) != 0) {
        IItcogetio((short *)0,1,32,19,value,IIVQdir_col);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1800 "vqdloop.qsc" */	/* host code */
	/* if the column isn't ordered.. then give error message,
	   the column order must be specified first */
	if (col->corder == 0 && value[0] != EOS)
	{
		IIVQer0(E_VQ00D2_Order_First);
/* # line 1805 "vqdloop.qsc" */	/* putrow */
    {
      if (IItbsetio(3,IIVQform,IIVQcfnCurFldName,-2) != 0) {
        IItcoputio(IIVQdir_col,(short *)0,1,32,0,_nullstring);
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 1807 "vqdloop.qsc" */	/* host code */
		return (FALSE);	/* resume on the current field */
	}
	/* check for valid value */
	if (value[0] == IIVQ_a_text[0])
	{
		if (col->flags & COL_DESCEND)
		{
			_state |= MFST_DOGEN;
			col->flags &= ~COL_DESCEND;
		}
	}
	else if (value[0] == IIVQ_d_text[0])
	{
		if ((col->flags & COL_DESCEND) == 0)
		{
			_state |= MFST_DOGEN;
			col->flags |= COL_DESCEND;
		}
	}
	return (TRUE);
  }
/*{
** Name:	vq_Reorder	- redo the column's sort/order field
**
** Description:
**	The user is changing the ordering of the columns..
**	This routine goes through the column structures and
**	adjusts the existing corder member.
**
** Inputs:
**	MFTAB *tab;	- ptr to relevent table structure
**	i4 start;	- starting point for adjustment
**	i4 end;	- ending point for adjustment
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
**	04/08/89 (tom) - created
*/
LOC VOID
vq_Reorder(tab, start, end, dir) 
register MFTAB *tab;
i4  start;
i4  end; 
i4  dir;
  {
	register i4  i;
	register i4  n;
	if (dir < 0)
	{
		/* loop backwards through the values incrementing */
		for (n = start; n >= end ; n--)
		{
			for (i = 0; i < tab->numcols; i++)
			{
				if (n == tab->cols[i]->corder)
				{
					tab->cols[i]->corder = n + 1;
					break;
				}
			}
		}
	}
	else
	{
		/* loop forwards through the values decrementing */
		for (n = start; n <= end; n++)
		{
			for (i = 0; i < tab->numcols; i++)
			{
				if (n == tab->cols[i]->corder)
				{
					tab->cols[i]->corder = n - 1;
					break;
				}
			}
		}
	}
  }
/*{
** Name:	vq_ColChanged	- Column's include flag has changed
**
** Description:
**	Mark the column as changed, also mark the metaframe structure.
**
** Inputs:
**	MFCOL *col;	- ptr to column
**	char *state;	- 'use' state of the column
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
**	05/27/89 (tom) - created
*/
LOC VOID
vq_ColChanged(col, state)
register MFCOL *col;
char *state;
{
	char *old_state = IIVQgivGetIncVal(col);
	/* if the old state is the same as the requested state then return */
	if ( *old_state == *state)
		return;
	/* Make any needed changes */
	if (*state == *IIVQ_y_text)
	{
		/* 
		** We're now displaying a previously undisplayed column. This 
		** requires formgen and codegen 
		*/
		col->flags |= COL_USED;
		col->flags &= ~COL_VARIABLE;
		_state |= MFST_DOFORM | MFST_DOGEN;
	}
	else if (*state == *IIVQ_v_text)
	{
		/* The column now becomes a variable.  This requires codegen */
		col->flags |= COL_VARIABLE;
		col->flags &= ~COL_USED;
		_state |= MFST_DOGEN;
		/* If the column was displayed, this requires formgen */
		if (*old_state == *IIVQ_y_text)
			_state |= MFST_DOFORM;
	}
	else 
	{
		/* The column is not used at all */
		col->flags &= ~(COL_USED|COL_VARIABLE);
		_state |= MFST_DOGEN;
		/* If the column was displayed, this requires formgen */
		if (*old_state == *IIVQ_y_text)
			_state |= MFST_DOFORM;
	}
}
/*{
** Name:	vq_Edit	- access editable components from the VQ  
**
** Description:
**	This routine supervises the display of a popup list of 
**	editable components from the visual query.
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		i4	VQ_xxxx  values which may indicate
**			that quiting is desired.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
*/
LOC i4  
vq_Edit(mf)
METAFRAME *mf;
{
	i4 type;
	register i4  i;
	i4 num;
	i4 typetab[VQMAXEOPS];
	VQED ed;
	char buf[500];
	type = IIVQotObjectType(mf->apobj);
	buf[0] = EOS;
	/* loop through the table for this edit menu and 
	   construct the menu to be presented to the user */
	for (num = i = 0; num < VQMAXEOPS && vq_edinfo[i].type != 0; i++)
	{
		switch (vq_edinfo[i].type)
		{
		 /* only allow edit of code if they have the capability */
		case EOP_SRCFIL:
			if (IIabAbf == FALSE)
			{
				continue;
			}
			break;
		/* frame behavior is only relevent to update/browse frames */
		case EOP_FRBEHA:
			if (type != MF_UPDATE && type != MF_BROWSE)
			{
				continue;
			}
			break;
		}
		if (num) /* if it's not the first item put in seperator */
		{
			STcat(buf, ERx("\n"));
		}
		STcat(buf, ERget(vq_edinfo[i].mitem));
		typetab[num++] = vq_edinfo[i].type;
	}
	/* set up the information passing structure */
	ed.mf = mf;
	ed.typetab = typetab;
	/* let the user choose the edit option */
	return (IIFDlpListPick(_vqed_prmpt, buf, num, 
		2, 2, _vqed_prmpt, ERx("vqedit.hlp"),
		vq_ED2, (PTR) &ed));
}
/*{
** Name:	vq_ED2 - handler function to handle editing various objects 
**
** Description:
**	This is a handler of the list pick code, which has
**	allowed the user to select from a list of options
**	of things that can be edited from the visual query.
**
** Inputs:
**	PTR dat;	- really a pointer to edit info struct
**      i4  choice;     - the user's choice
**      bool *resume;   - ptr to caller's resume flag
**
** Outputs:
**	Returns:
**		void
**
** Side Effects:
**
** History:
**	09/29/89 (tom) - created
**	10/90 (Mike S) - Refresh application data before editing errors
*/
LOC i4  
vq_ED2(dat, choice, resume)
PTR dat;
i4  choice;
bool *resume;
  {
	VQED *ed = (VQED*)dat;
	METAFRAME *mf = ed->mf;
	USER_FRAME *uf = (USER_FRAME*)mf->apobj; 
	i4 stat = VQ_DONE;
	*resume = FALSE;
	/* did the user actually select something */
	if (choice >= 0)
	{
		switch (ed->typetab[choice])
		{
		case EOP_GENFRM:
			if (vq_SaveEdit(mf, F_VQ00AB_eop_genfrm) == OK)
			{
				IIVQegfEditGenForm((OO_OBJECT*)uf);
			}
			break;
		case EOP_FRBEHA:
			_state |= IIVQgfbGetFrameBehavior(mf);
			break;
		case EOP_LOCVAR:
			IIVQlocals((OO_OBJECT*)uf);
			break;
		case EOP_LOCPRC:
			IIVQlprocs((OO_OBJECT*)uf);
			break;
		case EOP_GLOBOB:
			IIVQgeGlobalEdit(uf->appl, FALSE);
			break;
		case EOP_ESCCOD:
			if (vq_SaveEdit(mf, F_VQ00B0_eop_esccod) == OK)
			{
				IIVQefeEditFrameEscapes(mf);
			}
			break;
		case EOP_SRCFIL:
			if (vq_SaveEdit(mf, F_VQ00B1_eop_srcfil) == OK)
			{
				IIVQegcEditGenCode((OO_OBJECT*)uf);
			}
			break;
		case EOP_ERRORS:
			if (IIAMapFetch(uf->appl, OC_UNDEFINED, TRUE) == OK)
			{
				_VOID_ IIVQdceDisplayCompErrs(uf->appl, uf, 
							      FALSE);
			}
			break;
		}
	}
	return (stat);
  }
/*{
** Name:	vq_InsLup	- insert lookup table 
**
** Description:
**	Supervise the Insert of a lookup table below the current table.
**
** Inputs:
**	METAFRAME *mf;	- ptr to metaframe
**
** Outputs:
**	Returns:
**		void
**
** Side Effects:
**		reports errors
**
** History:
**	04/20/89 (tom) - created
*/
LOC VOID
vq_InsLup(mf)
METAFRAME *mf;
  {
	static bool first = TRUE;
    char tab_name[FE_MAXTABNAME + 1];
	i4 tmp = MF_MAXTABS;
	/* see if we have the maximum tables already */
	if (mf->numtabs >= MF_MAXTABS)
	{
		tmp = MF_MAXTABS;
		IIVQer1(E_VQ0054_Max_Tables, &tmp);
		return;
	}
	/* if it is the first time for this call.. */
	if (first)
	{
		IIARiaForm(newlup_form);
		first = FALSE;
	}
/* # line 2180 "vqdloop.qsc" */	/* display */
    {
      if (IIdispfrm(newlup_form,(char *)"f") == 0) goto IIfdE4;
      goto IImuI4;
IIfdI4:;
      if (IIfsetio((char *)0) == 0) goto IIfdE4;
      {
      }
IIfdB4:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 2187 "vqdloop.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXTABNAME + 1-1,tab_name,
                (char *)"tabname");
              } /* IIfsetio */
            }
/* # line 2189 "vqdloop.qsc" */	/* host code */
		if (tab_name[0] == EOS)	/* if it's a null name */
		{
			IIVQer2(E_VQ005A_No_Table_Name,
				ERget(FE_Cancel),
				ERget(FE_Lookup));
/* # line 2194 "vqdloop.qsc" */	/* resume */
            {
              if (1) goto IIfdB4;
            }
/* # line 2195 "vqdloop.qsc" */	/* host code */
		}
		if (vq_AddLup(mf, tab_name) == FALSE)
		{
			IIVQer3(E_VQ0048_No_Table,
				tab_name,
				ERget(FE_Lookup),
				ERget(FE_Cancel));
/* # line 2203 "vqdloop.qsc" */	/* resume */
            {
              if (1) goto IIfdB4;
            }
/* # line 2204 "vqdloop.qsc" */	/* host code */
		}
/* # line 2205 "vqdloop.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE4;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 2210 "vqdloop.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE4;
            }
          }
        } else if (IIretval() == 3) {
          {
            char tblname[FE_MAXTABNAME+1];
            char *p_tbl;
/* # line 2218 "vqdloop.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXTABNAME+1-1,tblname,(char *)"\
tabname");
              } /* IIfsetio */
            }
/* # line 2220 "vqdloop.qsc" */	/* host code */
                /* If "tblname" is empty string, then IITUtpTblPik treats
		** it as a wildcard, otherwise it is used as a literal
		** string. If "tblname" contains pattern match characters,
		** then an SQL LIKE clause will be used.
                */
                p_tbl = IITUtpTblPik(tblname, IIabExename);
                if ((p_tbl != NULL) && (*p_tbl != EOS))
                {
                    /* user Selected a row. */
/* # line 2230 "vqdloop.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio((char *)"tabname",(short *)0,1,32,0,p_tbl);
              } /* IIfsetio */
            }
/* # line 2231 "vqdloop.qsc" */	/* host code */
                }
          }
        } else if (IIretval() == 4) {
          {
/* # line 2236 "vqdloop.qsc" */	/* host code */
		FEhelp(ERx("vqinslup.hlp"), ERget(F_VQ00BB_inslup_tit)); 
          }
        } else {
          if (1) goto IIfdE4;
        } /* IIretval */
      } /* IIrunform */
IIfdF4:
      if (IIchkfrm() == 0) goto IIfdB4;
      if (IIfsetio((char *)0) == 0) goto IIfdE4;
      goto IIfdE4;
IImuI4:
      if (IIinitmu() == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_OK),ERget(F_VQ006F_OKExpl),2,2,1) == 0) goto IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_Cancel),(char *)0,2,2,2) == 0) goto IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(9,ERget(F_VQ0070_CancelExpl),2,2,2) == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_Lookup),(char *)0,2,2,3) == 0) goto IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(10,ERget(F_VQ0071_LookupExpl),2,2,3) == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_Help),ERget(F_VQ0072_HelpExpl),2,2,4) == 0) goto 
      IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE4;
      if (IIendmu() == 0) goto IIfdE4;
      goto IIfdI4;
IIfdE4:;
      IIendfrm();
    }
  }
/* # line 2242 "vqdloop.qsc" */	/* host code */
/*{
** Name:	vq_AddLup	- add table of given name to the visual query
**
** Description:
**	We now have a name for the table to add.. so look it up
**	and add it. This means reading the attribute definitions from
**	the catalogs.. setting up a MFTAB and MFCOL structures.. and
**	inserting them into the metaframe's table at the correct index.
**	Errors are reported by the caller.
**
** Inputs:
**	METAFRAME *mf;	- ptr to metaframe
**	char name;	- ptr to the name to be added 
**
** Outputs:
**	Returns:
**		bool	- TRUE means the table was added,
**			  FALSE means that the table doesn't exist
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/20/89 (tom) - created
**      04/11/95 (harpa06)
**		Send the converted table name "table_part" to the 
**		IIVQantAddNewTable() function which adds a table to the VISION
**		query instead of the table name the user entered in so the FIPS
**		installation is happy. (Bug #67951)
**      20-apr-95 (harpa06)
**              Reimplemented change for #67951. This change fixed 68228, 68233,
**              68232, 68241
*/
LOC bool
vq_AddLup(mf, name)
METAFRAME *mf;
char *name;
{
	i4 idx;
	i4 sect; 
	MFTAB *tab;
	register MFCOL *col;
	register i4  i;
	/* for 6.5 ["]owner["].["]tablename["] support */
	FE_RSLV_NAME    resolved_name;
	char    	owner_part[FE_MAXNAME + 1];
	char    	table_part[FE_MAXNAME + 1];
        char    	table_to_use[FE_MAXNAME *2 + 1];
	/* generic FE function to get current user */
	/* get the index of the current table */
	idx = IIVQcfCurFld();
	resolved_name.name = name;
	resolved_name.owner_dest = owner_part;
	resolved_name.name_dest = table_part;
	resolved_name.is_nrml = FALSE;
	if ( FE_fullresolve(&resolved_name) == FALSE )
	{
		/* error is reported by caller */
		return (FALSE);
	}
	/* add the table to the visual query */
/* Bug #67951, 68232, 68241 
**      68228, 68233
*/
	if (owner_part==EOS)
		STcopy(table_part,table_to_use);
	else
		STpolycat(3,owner_part,ERx("."),table_part,table_to_use);
        if (IIVQantAddNewTable(mf, table_to_use, idx + 1, TAB_LOOKUP,
                        sect = mf->tabs[idx]->tabsect) == FALSE)
	{
		/* error is reported by caller */
		return (FALSE);
	}
	_state = MFST_DOGEN | MFST_DOFORM;
	/* insert one default join */
	IIVQmdjMakeDefaultJoin(mf, IIVQptPrimaryTab(mf, sect), idx + 1);
	/* now we set up a default column to be present in the lookup,
	   if there is no default join column.. then use first column */
	tab = mf->tabs[idx + 1];
        /* Bug #67951  - Since we might have just passed in "owner.tablename" 
        ** into IIVQantAddNewTable(), this will be displayed on the user's 
	** display. We don't want this if the current user owns "tablename". 
	** Therefore, we must change "owner.tablename" to just "tablename" for 
	** user display purposes.
	*/
	if STequal(owner_part, IIUIdbdata()->suser)
		STcopy(table_part,tab->name);
	for (i = tab->numcols; i--; )
	{
		col = tab->cols[i];
		if (i == 0 || (col->flags & COL_SUBJOIN))
		{
			/* we just need to set corder.. the info member 
			   will be setup when the table is displayed */
			col->corder = 1;
			break;
		}
	}
	return (TRUE);
}
/*{
** Name:	IIVQmdjMakeDefaultJoin - find default joins 
**
** Description:
**	Find the first default join between to tables with given
**	indices.
**
** Inputs:
**	METAFRAME *mf;	- ptr to metaframe
**	i4 primary;	- primary table
**	i4 secondary;	- secondary table 
**
** Outputs:
**	Returns:
**		i4	- index of the primary table
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	07/04/89 (tom) - written
**	2/91 (Mike S) - Init VQ strings
*/
VOID
IIVQmdjMakeDefaultJoin(mf, primary, secondary)
METAFRAME *mf;
i4  primary;
i4  secondary;
{
	register MFTAB *tab1;
	register i4  j;
	register char *match;
	register MFTAB *tab2;
	register i4  i;
	bool found = FALSE;
	(VOID) IIVQivsInitVQStrings();
	tab1 = mf->tabs[primary];
	tab2 = mf->tabs[secondary];
	/* outer loop is the 2ndary table */
	for (i = 0; i < tab2->numcols; i++)
	{
		match = tab2->cols[i]->name;
		for (j = 0; j < tab1->numcols; j++)
		{
			if (STequal(match, tab1->cols[j]->name)) 
			{
				/* don't allow joining 2 lookups to the
				   same column of the primary table */
				if (vq_CheckJoin(mf, primary, j, secondary)
						== FALSE)
				{
					continue;	
				} 
				/* check for type incompatibility */
				if (IIVQjcJoinCheck(
					tab1->cols[j], tab2->cols[i]) == FALSE)
				{
					continue;
				}
				/* check for possibility of warnings */
				if (IIVQjwJoinWarn((char*)NULL, (char*)NULL,
					tab1->cols[j], tab2->cols[i]) == FALSE)
				{
					continue;
				}
				/* if there are too many joins already */
				if (mf->numjoins >= MF_MAXJOINS)
				{
					/* !!! should give error */
					return;
				}
				/* put join into tables */
				vq_PutJoin(mf, primary, j, secondary, i);
				found = TRUE;
				/* IF the secondary table is a lookup
				   OR both tables are really the same */
				if (	tab2->usage == TAB_LOOKUP
				   || 	STequal(tab2->name, tab1->name)
				   )
				{
					/* quit after the first join is found */
					return;
				}
			}
		}
	}
	/* if no join was found.. */
	if (!found)
	{
		/* if it's the master detail tables */
		if (tab2->usage != TAB_LOOKUP)
		{
			IIVQer1(E_VQ005B_No_MD_Join, ERget(F_VQ002A_AddJoin));
		}
		else	/* else.. it's a new lookup table */
		{
			IIVQer2(E_VQ005C_No_Lup_Join, ERget(F_VQ002A_AddJoin),
				tab1->name);
		}
	}
}
/*{
** Name:	IIVQptPrimaryTab - return primary table in given section 
**
** Description:
**	Given a section.. return the index into the MFTAB array for
**	the primary table in the section. This is always the first table
**	found with a matching section.
**
** Inputs:
**	METAFRAME *mf;	- ptr to metaframe
**	i4 sect;	- desired section
**
** Outputs:
**	Returns:
**		i4	- index of the primary table
**			  -1 if there is no such section
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	07/04/89 (tom) - written
*/
i4
IIVQptPrimaryTab(mf, sect)
register METAFRAME *mf;
register i4  sect;
{
	register i4  i;
	for (i = 0; i < mf->numtabs; i++)
	{
		if (sect == mf->tabs[i]->tabsect)
			return (i);
	}
	return (-1);	/* none found */
}
/*{
** Name:	vq_DelLup - delete a lookup table
**
** Description:
**	Delete the current table from the visual query.
**
** Inputs:
**	METAFRAME *mf;	- ptr to metaframe
**
** Outputs:
**	Returns:
**
**
** History:
**	04/20/89 (tom) - created
*/
LOC VOID
vq_DelLup(mf)
METAFRAME *mf;
{
	i4 idx;
	register i4  i;
	register MFTAB **tabs;
	register MFJOIN *j;
	idx = IIVQcfCurFld();
	if (mf->tabs[idx]->usage != TAB_LOOKUP)
	{
		IIVQer1(E_VQ0058_Del_Not_Lup, mf->tabs[idx]->name);
		return;
	}
	if (IIUFccConfirmChoice(CONF_DELETE, mf->tabs[idx]->name, 
		ERget(FE_Table), ERget(F_VQ008A_DelltTitle), 
		ERx("vqdellt.hlp")) == CONFCH_NO)
	{
		return;
	}
	/* go through the join array and adjust for the deletion */
	for (i = 0; i < mf->numjoins; i++)
	{
		j = mf->joins[i];
		/* if the join is to our deleted table, then delete the join */
		if (j->tab_1 == idx || j->tab_2 == idx)
		{
			/* remove the join from the join table */
			vq_ZapJoin(mf, j, i);
			/* effectivley do this one again because the table 
			   has been closed up due to the deletion */
			i--;
		}
		else  /* otherwise decrement the join table indexes */
		{
			if (j->tab_1 > idx)
				j->tab_1--;
			if (j->tab_2 > idx)
				j->tab_2--;
		}
	}
	/* close up the table array */
	tabs = mf->tabs;
	for (i = idx; i < mf->numtabs; i++)
		tabs[i] = tabs[i + 1]; 
	mf->numtabs--;
	/* mark the VQ as needing code and form gen */
	_state |= MFST_DOGEN | MFST_DOFORM;
}
/*{
** Name:	vq_DelJoin - delete a join from vq
**
** Description:
**	Delete a join from the visual query
**
** Inputs:
**	METAFRAME *mf;	- ptr to metaframe
**
** Outputs:
**	Returns:
**
**
** History:
**	04/20/89 (tom) - created
*/
LOC VOID
vq_DelJoin(mf)
METAFRAME *mf;
  {
	i4 idx;
    i4 colidx;
	i4 flags;
	register i4  i;
	register MFJOIN *j;
	idx = IIVQcfCurFld();
	/* test if user is at least in the table field and not
	   positioned on one of the supporting field */
	if (CMalpha(&IIVQcfnCurFldName[4]))
	{
		IIVQer0(E_VQ0057_Del_Join_Err);
		return;
	}
/* # line 2627 "vqdloop.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,IIVQform,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&colidx,29,(char *)0,0);
      } /* IIiqset */
    }
/* # line 2628 "vqdloop.qsc" */	/* host code */
	colidx--;	/* zero based */
	flags = mf->tabs[idx]->cols[colidx]->flags;
	/* if there are 2 joins on the column it's an error .. */ 
	if (	(flags & COL_DETJOIN)
	   &&	(flags & COL_LUPJOIN)
	   )
	{
		IIVQer0(E_VQ0055_Del_Join_2);
		return;
	}
	/*  if no joins on the column.. it's an error */
	if ( 	(flags & COL_DETJOIN) == 0
	   &&	(flags & COL_LUPJOIN) == 0
	   &&	(flags & COL_SUBJOIN) == 0
	   )
	{
		IIVQer0(E_VQ0057_Del_Join_Err);
		return;
	}
	if (IIUFccConfirmChoice(CONF_DELETE, _nullstring, 
		ERget(F_VQ00C4_join_text), ERget(F_VQ008B_DeljoTitle), 
		ERx("vqdeljo.hlp")) == CONFCH_NO)
	{
		return;
	}
	/* go through the join array and adjust for the deletion */
	for (i = 0; i < mf->numjoins; i++)
	{
		j = mf->joins[i];
		/* when we find the join.. then zap it */
		if (  (j->tab_1 == idx && j->col_1 == colidx)
		   || (j->tab_2 == idx && j->col_2 == colidx)
		   )
		{
			vq_ZapJoin(mf, j, i);
		}
	}
  }
/*{
** Name:	vq_ZapJoin	- delete one join, take care of column flags
**
** Description:
**	Delete the given join, and reset the column fields.
**
** Inputs:
**	METAFRAME *mf;	- metaframe to set flags in
**	MFJOIN	  *j;	- ptr to join to be deleted
**	i4	idx;	- index of join in the join array
**
** Outputs:
**	Returns:
**		none
**
**	Exceptions:
** Side Effects:
**
** History:
**	09/14/89 (tom) - created
*/
LOC VOID
vq_ZapJoin(mf, j, idx)
METAFRAME *mf;
MFJOIN *j;
register i4  idx;
{
	MFCOL *col;
	MFJOIN **joins;
	/* fix the flags of the subordinate join col */
	col = mf->tabs[j->tab_2]->cols[j->col_2];
	col->flags &= ~COL_SUBJOIN;
	/* fix the flags of the subordinate join col */
	col = mf->tabs[j->tab_1]->cols[j->col_1];
	/* depending on the table type of the subordinate
	   table.. we set the appropriate flag bit */
	if (mf->tabs[j->tab_2]->usage == TAB_LOOKUP)
	{
		col->flags &= ~COL_LUPJOIN;
	}
	else
	{
		col->flags &= ~COL_DETJOIN;
	}
	/* close up the table */
	joins = mf->joins;
	for (; idx < mf->numjoins; idx++)
	{
		joins[idx] = joins[idx + 1];
	}
	/* post that there is one less join */
	mf->numjoins--;
	/* any time the joins change we have to redo code,
	   also the form cause we generate special highlighting for 
	   join columns */
	_state |= MFST_DOFORM;
}
/*{
** Name:	vq_Validate	- validate the visual query
**
** Description:
**	Test the Visual query for validity.. if it is not valid
**	mark the state appropriately. Note that this function either
**	sets (incase of error) or unsets (incase of success) the flags
**	corresponding to the VQEERR condition.
**
** Inputs:
**	none
**
** Outputs:
**	Returns:
**		none
**
**	Exceptions:
** Side Effects:
**
** History:
**	11/06/89 (tom) -  created
*/
LOC STATUS 
vq_Validate(mf)
METAFRAME *mf;
{
	STATUS stat;
	char arg[FE_MAXTABNAME + 2];
	switch (stat = IIVQvqcVQCheck(mf, arg))
	{
	case OK:
		mf->state &= ~MFST_VQEERR;
		MFST_ER_SET_MACRO(mf, OK);
		return (stat);
	case VQ_NODCOL:
	case VQ_NOMCOL:
		IIVQer1(E_VQ007B_No_Cols, arg);
		break;
	case VQ_NOJOIN:
		IIVQer1(E_VQ007C_No_Join, arg);
		break;
	case VQ_NOMDJO:
		IIVQer0(E_VQ007D_No_MD_Join);
		break;
	case VQ_BADMDJ:
		IIVQer0(E_VQ007F_Bad_MD_Join);
		break;
	}
	mf->state |= MFST_VQEERR;
	MFST_ER_SET_MACRO(mf, stat);
	return (stat);
}