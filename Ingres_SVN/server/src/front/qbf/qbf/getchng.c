# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<me.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
# include "mqbf.h"
# include "mqsymbol.h"
#include	"erqf.h"
/**
** Name:	getchng.qc -
**
** Description:
**	This module is called to show the user the fields that will be
**	displayed for the current qdef. They can then delete any
**	of these (non-join) fields from the list so that those fields will
**	not be displayed.
**
**	Routines:
**	     mq_chgdispl()
**
** History:
**	Revision 6.4
**      	09/26/90 (emerson) Don't display logical keys
**	Revision 6.0  88/08/10  kenl
**		 07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new headers file: mqbf.qsh and mqsymbol.qsh.
**		02-may-88 (kenl)
**			Added code to handle the deletion of a key column from
**			a table.  Instead of actually deleting the column from
**			the ATTRIBINFO structure, the column's formfield is
**			set to EOS.  This column will still be used for
**			updates.
**		10-aug-88 (kenl)
**			Fixed bug #2963.  Undid 5.0 to 6.0 code change that
**			someone did to try to be efficient on using
**			INSERTROW when UNDOing a DELETE.  Changed DISPLAY
**			of _mqchange from UPDATE to READ.
**		09-sep-88 (jhw)  Fixed some international support problems.
**	12/31/88 (dkh) - Perf changes.
**      09/04/90 (emerson)
**			Removed unused code in getdisp that was computing
**                      column width (variables flen and clen were set
**                      but not referenced).
**      09/26/90 (emerson) - Don't display logical keys (bug 8593).
**
**	Revision 4.0  85/07/16  stevel
**		 07/16/85 (scl) added some FTspace support for CMS
**
**	Revision 2.0  83/12/22  nadene
**		Written: 12/22/83 (nml)
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	07-mar-2001 (somsa01)
**	    Changed maxcharwidth from 10000 to FE_FMTMAXCHARWIDTH.
*/
#define	_HelpExpl		F_FE0100_ExplFrameHelp
#define	_EndExpl		F_FE0102_ExplEnd
# define	MQ_MAXCHARWIDTH		72	/* max width before folding */
FUNC_EXTERN	STATUS	fmt_sdeffmt();
FUNC_EXTERN	ADF_CB	*FEadfcb();
static VOID	getdisp();
bool
mq_chgdispl ( qdef, changed )
register MQQDEF	*qdef;
i4		*changed;
  {
    char *delitem;
    char *undoitem;
	register RELINFO	**rels = qdef->mqtabs;
	register ATTRIBINFO	**atts = qdef->mqatts;
	register i4		numatts = qdef->mqnumatts;
	i4	relno = 0;
	i4	num_deleted = 0;
	/* Undo save variables */
    i4 delrow;
	i4		attnum;		/* deleted attribute number */
	bool		undodel;	/* for undo undo */
	ATTRIBINFO	*lastdel = NULL;/* deleted attribute */
	char		formcopy[FE_MAXNAME + 1];
	if (mq_forms)
		delitem =  undoitem = ERx("");
	else
	{
		delitem = ERget(FE_Delete);
		undoitem = ERget(FE_Undo);
	}
/* # line 111 "getchng.qsc" */	/* display */
    {
      if (IIdispfrm(_mqchange,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 114 "getchng.qsc" */	/* inittable */
        {
          if (IItbinit(_mqchange,_chgdisp,(char *)"r") != 0) {
            IItfill();
          } /* IItbinit */
        }
/* # line 116 "getchng.qsc" */	/* host code */
		getdisp(qdef, relno);
/* # line 117 "getchng.qsc" */	/* resume */
        {
          IIresfld(_chgdisp);
          if (1) goto IIfdB1;
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 122 "getchng.qsc" */	/* host code */
		if ( qdef->mqnumtabs == 1 )
		{
			IIUGerr(E_QF002A_There_is_only_one_tab, 0, 0);
		}
		else
		{
			if ( undodel && lastdel != NULL )
			{
				if (lastdel->keyinfo != mqISKEY)
				{
				    MEfree(lastdel);
				}
				lastdel = NULL;
			}
/* # line 136 "getchng.qsc" */	/* clear */
            {
              IIfldclear((char *)"table");
              IIfldclear(_chgdisp);
            }
/* # line 137 "getchng.qsc" */	/* host code */
			relno = (relno < qdef->mqnumtabs - 1) ? relno + 1 : 0;
			getdisp(qdef, relno);
		}
/* # line 140 "getchng.qsc" */	/* resume */
            {
              IIresfld(_chgdisp);
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
            i4 istable;
/* # line 147 "getchng.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_mqchange,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&istable,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 148 "getchng.qsc" */	/* host code */
		if ( !istable )
		{ /* not in table field */
			IIUGerr(E_QF002B_Place_cursor_over_nam, 0, 0);
		}
		else
            {
              i4 lrow;
/* # line 156 "getchng.qsc" */	/* inquire_frs */
              {
                if (IIiqset(3,0,ERx(""),(char *)0, (char *)0) != 0) {
                  IIiqfsio((short *)0,1,30,4,&lrow,31,(char *)0,0);
                } /* IIiqset */
              }
/* # line 157 "getchng.qsc" */	/* host code */
		    if ( lrow == 0 )
		    {
			/* no rows in tablefield */
			IIUGerr(E_QF002C_No_fields_to_delete_f, 0, 0);
/* # line 161 "getchng.qsc" */	/* resume */
              {
                IIresfld((char *)"table");
                if (1) goto IIfdB1;
              }
/* # line 162 "getchng.qsc" */	/* host code */
		    }
		    else if ( lrow == 1 && qdef->mqnumtabs == 1 )
		    {
			IIUGerr(E_QF002D_Can_t_delete_the_only, 0, 0);
/* # line 166 "getchng.qsc" */	/* resume */
              {
                IIresfld(_chgdisp);
                if (1) goto IIfdB1;
              }
/* # line 167 "getchng.qsc" */	/* host code */
		    }
		    else
              {
			register i4	d;
                i4 drow;
                char joinfld[4];
                char dff[FE_MAXNAME + 1];
                char dcol[FE_MAXNAME + 1];
/* # line 176 "getchng.qsc" */	/* getrow */
                {
                  if (IItbsetio(2,_mqchange,_chgdisp,-2) != 0) {
                    IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,dcol,(char *)"\
col");
                    IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,dff,(char *)"\
ff");
                    IItcogetio((short *)0,1,32,3,joinfld,(char *)
"joinfield");
                    IItcogetio((short *)0,1,30,4,&drow,(char *)"_RECOR\
D");
                    IITBceColEnd();
                  } /* IItbsetio */
                }
/* # line 181 "getchng.qsc" */	/* host code */
			STtrmwhite(dcol);
			if (*joinfld == '*')
			{
			    IIUGerr(E_QF002E_cant_delete_join_fld, 0, 1, dcol);
/* # line 185 "getchng.qsc" */	/* resume */
                {
                  IIresfld(_chgdisp);
                  if (1) goto IIfdB1;
                }
/* # line 186 "getchng.qsc" */	/* host code */
			}
			delrow = drow;
			STtrmwhite(dff);
			++num_deleted;
			for ( d = 0 ; d < numatts ; ++d )
			{
			    if ( STequal(dcol, atts[d]->col) &&
					STequal(dff, atts[d]->formfield) )
			    {
				    if ( undodel && lastdel != NULL )
				    {
					if (lastdel->keyinfo != mqISKEY)
					{
					    MEfree(lastdel);
					}
				    }
				    lastdel = atts[d];
				    if (lastdel->keyinfo != mqISKEY)
				    {
					register i4	c;
				        for ( c = d ; c < numatts - 1 ; ++c )
					    atts[c] = atts[c+1];
				        atts[c] = NULL;
				        --qdef->mqnumatts;
				    }
				    else
				    {
					STcopy(lastdel->formfield, formcopy);
					lastdel->formfield[0] = EOS;
				    }
				    attnum = d;
				    undodel = TRUE;
/* # line 221 "getchng.qsc" */	/* deleterow */
                {
                  if (IItbsetio(4,_mqchange,_chgdisp,-2) != 0) {
                    if (IItdelrow(0) != 0) {
                    } /* IItdelrow */
                  } /* IItbsetio */
                }
/* # line 222 "getchng.qsc" */	/* host code */
				    break;
			    }
			}
              }
            }
/* # line 227 "getchng.qsc" */	/* resume */
            {
              IIresfld(_chgdisp);
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 232 "getchng.qsc" */	/* host code */
		if (lastdel == NULL)
		{
			IIUGerr(E_QF002F_No_previous_delete_to, 0, 0);
/* # line 235 "getchng.qsc" */	/* resume */
            {
              IIresfld(_chgdisp);
              if (1) goto IIfdB1;
            }
/* # line 236 "getchng.qsc" */	/* host code */
		}
		if (undodel)
            {
              char *delcol;
              char *delff;
			delcol = lastdel->col;
			delff = lastdel->formfield;
			num_deleted--;
			if (lastdel->keyinfo != mqISKEY)
			{
			    register i4	c;
			    for ( c = numatts ; c > attnum ; --c )
				atts[c] = atts[c-1];
			    atts[attnum] = lastdel;
			    ++qdef->mqnumatts;
			}
			else
			{
			    STcopy(formcopy, lastdel->formfield);
			}
			/* bug 2963
			** Put back 5.0 style of code.
			*/
			if ( delrow - 1 == 0 )
			{
/* # line 264 "getchng.qsc" */	/* scroll */
              {
                if (IItbsetio(1,_mqchange,_chgdisp,-3) != 0) {
                  IItbsmode((char *)"to");
                  if (IItscroll(0,1) != 0) {
                  } /* IItscroll */
                } /* IItbsetio */
              }
/* # line 265 "getchng.qsc" */	/* insertrow */
              {
                if (IItbsetio(5,_mqchange,_chgdisp,0) != 0) {
                  if (IItinsert() != 0) {
                    IItcoputio((char *)"col",(short *)0,1,32,0,delcol);
                    IItcoputio((char *)"ff",(short *)0,1,32,0,delff);
                    IITBceColEnd();
                  } /* IItinsert */
                } /* IItbsetio */
              }
/* # line 267 "getchng.qsc" */	/* host code */
			}
			else
              {
                i4 irow = delrow - 1;
/* # line 272 "getchng.qsc" */	/* scroll */
                {
                  if (IItbsetio(1,_mqchange,_chgdisp,-3) != 0) {
                    IItbsmode((char *)"to");
                    if (IItscroll(0,irow) != 0) {
                    } /* IItscroll */
                  } /* IItbsetio */
                }
/* # line 273 "getchng.qsc" */	/* insertrow */
                {
                  if (IItbsetio(5,_mqchange,_chgdisp,-2) != 0) {
                    if (IItinsert() != 0) {
                      IItcoputio((char *)"col",(short *)0,1,32,0,delcol);
                      IItcoputio((char *)"ff",(short *)0,1,32,0,delff);
                      IITBceColEnd();
                    } /* IItinsert */
                  } /* IItbsetio */
                }
              }
/* # line 277 "getchng.qsc" */	/* host code */
			undodel = FALSE;
            }
		else
		{ /* undo `undo' */
			++num_deleted;
			if (lastdel->keyinfo != mqISKEY)
			{
			    register i4	c;
			    for ( c = attnum ; c < numatts - 1 ; ++c )
				atts[c] = atts[c+1];
			    atts[c] = NULL;
			    --qdef->mqnumatts;
		    	}
			else
			{
			    STcopy(lastdel->formfield, formcopy);
			    lastdel->formfield[0] = EOS;
			}
/* # line 296 "getchng.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_mqchange,_chgdisp,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,delrow) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 297 "getchng.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,_mqchange,_chgdisp,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 299 "getchng.qsc" */	/* host code */
			undodel = TRUE;
		}
          }
        } else if (IIretval() == 4) {
          {
/* # line 305 "getchng.qsc" */	/* host code */
		FEhelp(ERx("qchng.hlp"),ERget(S_QF0030_QBF_Change_Display_Fo));
          }
        } else if (IIretval() == 5) {
          {
/* # line 311 "getchng.qsc" */	/* host code */
		if ( undodel && lastdel != NULL )
		{
			if (lastdel->keyinfo != mqISKEY)
			{
			    MEfree(lastdel);
			}
			lastdel = NULL;
		}
/* # line 319 "getchng.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(F_QF0004_NextTable),(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(delitem,(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(undoitem,(char *)0,2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(_EndExpl),0,2,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,5) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 323 "getchng.qsc" */	/* host code */
	*changed = (num_deleted > 0);
	return TRUE;
  }
/*
** Name:	getdisp() - This routine puts a new table name,
**		    range variable and set of fields in
**		    the form (_mqchange).
**
*/
static VOID
getdisp(qdef, relno)
register MQQDEF	*qdef;
i4		relno;
  {
    char *rv;
	register i4	i;
	register i4	numatts = qdef->mqnumatts;
	ADF_CB		*ladfcb;
	ladfcb = FEadfcb();
	rv = qdef->mqtabs[relno]->mqrangevar;
/* # line 348 "getchng.qsc" */	/* putform */
    {
      if (IIfsetio(_mqchange) != 0) {
        IIputfldio((char *)"table",(short *)0,1,32,0,rv);
      } /* IIfsetio */
    }
/* # line 350 "getchng.qsc" */	/* host code */
	for ( i = 0 ; i < numatts ; ++i )
	{
		register ATTRIBINFO	*catt = qdef->mqatts[i];
		if ( catt->formfield[0] != EOS && STequal(rv, catt->rvar) &&
			catt->lkeyinfo == 0 && catt->jpart != 0 )
    {
      char *jfld;
			char	uformat[MAX_FMTSTR + 1];
			i4	x, y;
			i4	charwidth;
			charwidth = catt->intbl ?
					FE_FMTMAXCHARWIDTH : MQ_MAXCHARWIDTH;
			jfld = (catt->jpart == 1) ? ERx("*") : ERx("");
			if ( fmt_sdeffmt(ladfcb, &(catt->dbv), charwidth,
				FALSE, uformat, &x, &y) != OK )
			{
				IIUGerr(E_QF0031_Bad_datatype_format, 0, 0);
			}
			else
      {
        char *fldcol;
        char *ffld;
				fldcol = catt->col;
				ffld = catt->formfield;
/* # line 379 "getchng.qsc" */	/* loadtable */
        {
          if (IItbact(_mqchange,_chgdisp,1) != 0) {
            IItcoputio((char *)"col",(short *)0,1,32,0,fldcol);
            IItcoputio((char *)"ff",(short *)0,1,32,0,ffld);
            IItcoputio((char *)"joinfield",(short *)0,1,32,0,jfld);
            IITBceColEnd();
          } /* IItbact */
        }
      }
    }
/* # line 385 "getchng.qsc" */	/* host code */
	} /* end for */
  }
