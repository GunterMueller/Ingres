# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<cm.h>
#include	<ug.h>
#include	<erug.h>
#include	<flddesc.h>
# include "mqbf.h"
# include "mqsymbol.h"
#include	"erqf.h"
/**
** Name:	rules.qc -
**
** Description:
**	This file contains the routine mq_rules that displays the update
**	semantics form and semantics information.
**
** History:
**	Revision 6.4
**	6/20/90 (martym) Changed to accept 'y', 'ye', 'yes', 'n', 'no'  
**			 as input and always display 'yes', 'no', and 
**			 display popup when user selects ListChoices.
**	10/01/90 (emerson)
**		Changed fname declaration from uninitialized pointer
**		to unitialized array.
**	10/01/90 (emerson)
**		Replaced literal strings "yes" and "no" by strings
**		retrieved via ERget from internationalizable strings
**		defined in $HDR2/erug.msg.
**	10/11/90 (emerson)
**		Don't show logical keys in the Update Information table
**		(bug 8593).
**
**	Revision 6.1  
**	88/09  wong
**		Merged duplicate messages.
**	08-mar-89 (kenl)
**		Doubled the length of tblcol.
**
**	Revision 6.0
**	07-dec-87 (kenl)
**		Turned module into a .QSC file in order to incorporate
**		the new header file: mqbf.qsh
**	10-dec-87 (bruceb)
**		Changed buffer length specifications for those form fields
**		increased in length for dg.
**
**	Revision 2.0  83/07  nadene
**	written 7/28/83 (nml)
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
#define	_HelpExpl		F_FE0100_ExplFrameHelp
#define	_EndExpl		F_FE0102_ExplEnd
static 		VOID	updrules();
FUNC_EXTERN 	bool	IIUGyn();
i4
mq_rules(qdef, changed)
register MQQDEF	*qdef;
i4		*changed;
  {
    i4 chgd;
	i4	ss = 0;
    char ans[10];
	STATUS	ynstat;
	char	*choices;
	char 	*help_scr;
	char 	*help_file;
	i4	i;
    char fname[FE_MAXNAME + 1];
/* # line 89 "rules.qsc" */	/* inittable */
    {
      if (IItbinit(_mqbfops,_mquops,(char *)"u") != 0) {
        IIthidecol((char *)"aindx",(char *)"i4");
        IItfill();
      } /* IItbinit */
    }
/* # line 90 "rules.qsc" */	/* inittable */
    {
      if (IItbinit(_mqbfops,_mqdops,(char *)"u") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 91 "rules.qsc" */	/* display */
    {
      if (IIdispfrm(_mqbfops,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 94 "rules.qsc" */	/* host code */
		register i4	i;
		for ( i = 0 ; i < qdef->mqnumtabs ; ++i )
		{
			register RELINFO	*rel = qdef->mqtabs[i];
        char rv[FE_UNRML_MAXNAME+1];
        char rv_owner[FE_UNRML_MAXNAME+1];
			/*
			** Normalize (i.e., re-quote) a delimited identifier
			** name and owner, if necessary
			*/
			IIUGxri_id(rel->mqrangevar, rv);
			IIUGxri_id(rel->mqrelown, rv_owner);
/* # line 109 "rules.qsc" */	/* loadtable */
        {
          if (IItbact(_mqbfops,_mqdops,1) != 0) {
            IItcoputio((char *)"role",(short *)0,1,32,0,
            ERget( (rel->mqtype == 0)
					? F_QF0015_MASTER : F_QF0016_detail
				));
            IItcoputio((char *)"table",(short *)0,1,32,0,rv);
            IItcoputio((char *)"owner",(short *)0,1,32,0,rv_owner);
            IItcoputio((char *)"del",(short *)0,1,32,0,
            ERget( (rel->opdelete)
						? F_UG0002_Yes2 : F_UG0007_No2
				));
            IITBceColEnd();
          } /* IItbact */
        }
/* # line 119 "rules.qsc" */	/* host code */
		}
		for ( i = 0 ; i < qdef->mqnumatts ; ++i )
		{
		    register ATTRIBINFO	*att = qdef->mqatts[i];
		    if (att->jpart != -1 && att->lkeyinfo == 0)
		    {
			register i4		j;
			register RELINFO	*rel;
        i4 attindx;
        char tblcol[2 * FE_UNRML_MAXNAME + 1 + 1];
			for ( j = 0 ; j < qdef->mqnumtabs ; ++j )
			{
				rel = qdef->mqtabs[j];
				if ( STequal(att->rvar, rel->mqrangevar) )
				{
					break;
				}
			}
			attindx = i;
			/*
			** IIUIxrt_tbl() is used to rejoining owner.table
			** pairs, but can be used for table.column ones just
			** the same
			*/
			IIUIxrt_tbl(rel->mqrangevar, att->col, tblcol);
/* # line 148 "rules.qsc" */	/* loadtable */
        {
          if (IItbact(_mqbfops,_mquops,1) != 0) {
            IItcoputio((char *)"col",(short *)0,1,32,0,tblcol);
            IItcoputio((char *)"upd",(short *)0,1,32,0,
            ERget( (att->opupdate == 1)
						? F_UG0002_Yes2 : F_UG0007_No2
				));
            IItcoputio((char *)"aindx",(short *)0,1,30,4,&attindx);
            IITBceColEnd();
          } /* IItbact */
        }
/* # line 155 "rules.qsc" */	/* host code */
		    }
		}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 160 "rules.qsc" */	/* host code */
		if (qdef->mqnumtabs <= 1 || qdef->mqnumjoins == 0)
/* # line 161 "rules.qsc" */	/* validate */
            {
              if (IIvalfld(_mqdops) == 0) {
                goto IIfdB1;
              } /* IIvalfld */
            }
/* # line 162 "rules.qsc" */	/* host code */
		else
/* # line 163 "rules.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 165 "rules.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chgd,20,_mqbfops,0);
              } /* IIiqset */
            }
/* # line 166 "rules.qsc" */	/* host code */
		if ( chgd != 0 )
		{
			updrules(qdef);
		}
/* # line 171 "rules.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 172 "rules.qsc" */	/* host code */
		ss = 1;
/* # line 173 "rules.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 178 "rules.qsc" */	/* host code */
		chgd = 0;
/* # line 179 "rules.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 186 "rules.qsc" */	/* host code */
		/*
		IIUFfieldHelp();
		*/
/* # line 190 "rules.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_mqbfops,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,fname,19,(char *)0,0);
              } /* IIiqset */
            }
/* # line 192 "rules.qsc" */	/* host code */
		if (STequal(fname, _mquops))
		{
			help_scr = ERget(S_QF0162_EnableModOfJF);
			choices = ERget(F_QF0160_EnableModOfJF);
			help_file = ERx("qfmodjf.hlp");
		}
		else
		{
			help_scr = ERget(S_QF0163_DisableDelOfRows);
			choices = ERget(F_QF0161_DisableDelOfRows);
			help_file = ERx("qfdelrow.hlp");
		}
		i = IIFDlpListPick(NULL, choices, 0, 
				LPGR_FLOAT, LPGR_FLOAT, help_scr, help_file,
				NULL, NULL);
		if (i >= 0)
		{
			if (i == 0)
				STcopy(ERget(F_UG0002_Yes2), ans);
			else
				STcopy(ERget(F_UG0007_No2), ans);
			if (STequal(fname, _mquops))
			{
/* # line 216 "rules.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_mqbfops,fname,-2) != 0) {
                IItcoputio((char *)"upd",(short *)0,1,32,0,ans);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 217 "rules.qsc" */	/* host code */
			}
			else
			{
/* # line 220 "rules.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_mqbfops,fname,-2) != 0) {
                IItcoputio((char *)"del",(short *)0,1,32,0,ans);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 221 "rules.qsc" */	/* host code */
			}
		}
          }
        } else if (IIretval() == 4) {
          {
/* # line 228 "rules.qsc" */	/* host code */
		FEhelp( ERx("qrules.hlp"), ERget(F_QF0017_QBF_UpdateDelete) );
          }
        } else if (IIretval() == 5) {
          {
/* # line 233 "rules.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_mqbfops,_mquops,-2) != 0) {
                IItcogetio((short *)0,1,32,9,ans,(char *)"upd");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 235 "rules.qsc" */	/* host code */
		_VOID_ IIUGyn(ans, &ynstat);
		if (ynstat == E_UG0004_Yes_Response)
		{
			STcopy(ERget(F_UG0002_Yes2), ans);
/* # line 239 "rules.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_mqbfops,_mquops,-2) != 0) {
                IItcoputio((char *)"upd",(short *)0,1,32,0,ans);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 240 "rules.qsc" */	/* host code */
		}
		else
		if (ynstat == E_UG0005_No_Response)
		{
			STcopy(ERget(F_UG0007_No2), ans);
/* # line 245 "rules.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_mqbfops,_mquops,-2) != 0) {
                IItcoputio((char *)"upd",(short *)0,1,32,0,ans);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 246 "rules.qsc" */	/* host code */
		}
/* # line 248 "rules.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 253 "rules.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_mqbfops,_mqdops,-2) != 0) {
                IItcogetio((short *)0,1,32,9,ans,(char *)"del");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 255 "rules.qsc" */	/* host code */
		_VOID_ IIUGyn(ans, &ynstat);
		if (ynstat == E_UG0004_Yes_Response)
		{
			STcopy(ERget(F_UG0002_Yes2), ans);
/* # line 259 "rules.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_mqbfops,_mqdops,-2) != 0) {
                IItcoputio((char *)"del",(short *)0,1,32,0,ans);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 260 "rules.qsc" */	/* host code */
		}
		else
		if (ynstat == E_UG0005_No_Response)
		{
			STcopy(ERget(F_UG0007_No2), ans);
/* # line 265 "rules.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_mqbfops,_mqdops,-2) != 0) {
                IItcoputio((char *)"del",(short *)0,1,32,0,ans);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 266 "rules.qsc" */	/* host code */
		}
/* # line 268 "rules.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 7) {
          {
/* # line 273 "rules.qsc" */	/* host code */
		if (qdef->mqnumtabs <= 1 || qdef->mqnumjoins == 0)
/* # line 274 "rules.qsc" */	/* validate */
            {
              if (IIvalfld(_mqdops) == 0) {
                goto IIfdB1;
              } /* IIvalfld */
            }
/* # line 275 "rules.qsc" */	/* host code */
		else
/* # line 276 "rules.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 278 "rules.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&chgd,20,_mqbfops,0);
              } /* IIiqset */
            }
/* # line 279 "rules.qsc" */	/* host code */
		if ( chgd != 0 )
		{
			updrules(qdef);
		}
/* # line 284 "rules.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(F_QF0022_Joins),(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Forget),(char *)0,2,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,3) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE1;
      if (IITBcaClmAct(_mquops,(char *)"upd",0,5) == 0) goto IIfdE1;
      if (IITBcaClmAct(_mqdops,(char *)"del",0,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(_EndExpl),2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,7) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 287 "rules.qsc" */	/* host code */
	*changed = chgd;
	return ss;
  }
static VOID
updrules ( qdef )
register MQQDEF	*qdef;
  {
    char ans[5];
    i4 recnum;
    i4 recstat;
    i4 attindx;
	STATUS	ynstat;
/* # line 302 "rules.qsc" */	/* unloadtable */
    {
      if (IItbact(_mqbfops,_mqdops,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,4,ans,(char *)"del");
        IItcogetio((short *)0,1,30,4,&recnum,(char *)"_RECORD");
        IItcogetio((short *)0,1,30,4,&recstat,(char *)"_STATE");
        IITBceColEnd();
        {
/* # line 305 "rules.qsc" */	/* host code */
		if ( recstat == CHANGED )
		{
			qdef->mqtabs[recnum-1]->opdelete = IIUGyn(ans,&ynstat);
		}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 310 "rules.qsc" */	/* unloadtable */
    {
      if (IItbact(_mqbfops,_mquops,0) == 0) goto IItbE2;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,4,ans,(char *)"upd");
        IItcogetio((short *)0,1,30,4,&recstat,(char *)"_STATE");
        IItcogetio((short *)0,1,30,4,&attindx,(char *)"aindx");
        IITBceColEnd();
        {
/* # line 313 "rules.qsc" */	/* host code */
		if ( recstat == CHANGED )
		{
			qdef->mqatts[attindx]->opupdate = IIUGyn(ans, &ynstat);
		}
        }
      } /* IItunload */
IItbE2:
      IItunend();
    }
  }
