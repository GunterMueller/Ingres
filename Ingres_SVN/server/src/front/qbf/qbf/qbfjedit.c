# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<st.h>
# include	<si.h>
# include	<tm.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include "mqbf.h"
# include "mqsymbol.h"
# include	<oosymbol.h>
# include	"erqf.h"
# include	<ui.h>
# include	<ug.h>
/**
** Name:	qbfjedit.qsc	- listpick related code for -J flag
**
** Description:
**	This file contains the QBF routines to handle the listpick menus
**	that were added to QBF to support the -J flag.  These routines are
**	also used when the user creates a new JoinDef.  At that point a
**	popup will appear asking the user whether he/she wants to start 
**	with a blank or a duplicate of another JoinDef.
**
**	This file defines:
**
**	IIQFjl_JoindefListpick
**	IIQFijf_InitJoinForms
**	IIQFcjCreateJoin
**	qf_j_dup
**
** History:
**	01-dec-89  (kenl)  - created
**	6/18/90 (martym)   - Wrapped ERx() around literal strings and 
**			     added help files.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**    25-Oct-2005 (hanje04)
**        Add prototype for qf_j_dup() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
*/
FUNC_EXTERN     char    *IIUFopObjPik();
GLOBALREF	OOID	mq_joinid;
i4  IIQFcjCreateJoin();
static i4 qf_j_dup();
# define _HelpExpl	F_FE0100_ExplFrameHelp
# define NO_JOIN	-1	/* "cancel" */
# define BLANK_JOIN	0
# define DUP_JOIN	1
/*{
** Name:	IIQFjl_JoindefListpick	- main listpick routine for QBF
**
** Description:
**	This is the main inteface for QBF with the ListPick routine.  This
**	routine is call by both the -J flag processing code and for the
**	JoinDef create option.  The difference between the two callings is
**	in the name of the form used for the listpick.
**
** Inputs:
**	MQQDEF *qdef		- ptr to query definition structure
**	char *list_form		- ptr to name of form to use for listpick
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
IIQFjl_JoindefListpick(qdef, list_form)
MQQDEF	*qdef;
char	*list_form;
{
  char join_def[FE_MAXNAME + 1];
	i4             cre_choice;     /* Type of JoinDef creation. */
	char            hlpbuf[132+1];
		/* loadtable with create options */
		IIQFijf_InitJoinForms(list_form);
		/* save JoinDef name in a global for later use */
		STcopy(qdef->mqcr.name, join_def);
		/* If using -J flag, put name of JoinDef on form */
		if (mq_editjoin)
		{
/* # line 106 "qbfjedit.qsc" */	/* putform */
  {
    if (IIfsetio(_mqcrjoin) != 0) {
      IIputfldio((char *)"form_name",(short *)0,1,32,0,join_def);
    } /* IIfsetio */
  }
/* # line 107 "qbfjedit.qsc" */	/* host code */
		}
		STcopy(ERx("JoinDef Creation"), hlpbuf);
		cre_choice = IIFRrlpRunListPick(list_form, _form_type, -1,
			-1, hlpbuf, ERx("qsource.hlp"), IIQFcjCreateJoin,
			(PTR)qdef);
}
/*{
** Name:	IIQFijf_InitJoinForms	- initialize form with create options
**
** Description:
**	This routine uses the form whose name was pass in and loads the
**	tablefield with the options available for creating a new JoinDef.
**
** Inputs:
**	char *formname		- ptr to name of form
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
IIQFijf_InitJoinForms(formname)
  char *formname;
{
	if (mqinitqcre() != TRUE)
	{
		/* couldn't initialize catalog form */
		FDerror(NOHELPFRMS, 0, ERx(""));
	}
/* # line 146 "qbfjedit.qsc" */	/* inittable */
  {
    if (IItbinit(formname,_form_type,(char *)"r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 147 "qbfjedit.qsc" */	/* loadtable */
  {
    if (IItbact(formname,_form_type,1) != 0) {
      IItcoputio((char *)"type",(short *)0,1,32,0,ERx("Blank"));
      IItcoputio((char *)"description",(short *)0,1,32,0,
      ERx("Create a JoinDef starting with a blank"));
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 149 "qbfjedit.qsc" */	/* loadtable */
  {
    if (IItbact(formname,_form_type,1) != 0) {
      IItcoputio((char *)"type",(short *)0,1,32,0,ERx("Duplicate"));
      IItcoputio((char *)"description",(short *)0,1,32,0,
      ERx("Create a JoinDef by copying an existing JoinDef"));
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 151 "qbfjedit.qsc" */	/* host code */
}
/*{
** Name:	IIQFcjCreateJoin	- handling of create option selection
**
** Description:
**	This routine branches to the appropriate QBF routine to handle
**	the type of create option the user selected.  If 'blank' was selected
**	we just jump into QBF where we used to.  If 'duplicate' is picked
**	we need to put up a new form that will allow 'ListChoices' for a 
**	list of JoinDef names.
**
** Inputs:
**	PTR dataptr		- ptr to QDEF structure
**	i4 choice		- item user selected from listpick
**	bool *resume		- should we give the user another try?
**
** Outputs:
**	Returns:
**		i4 - return listpick selection number back to caller
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	01-dec-89  (kenl)  - new interface
*/
i4
IIQFcjCreateJoin(dataptr, choice, resume)
PTR     dataptr;        /* ptr to QDEF structure */
i4      choice;         /* Method of form creation. */
bool    *resume;        /* Another chance to select form creation method? */
{
	MQQDEF	*qdef = (MQQDEF *)dataptr;
	mq_lastsaved[0] = EOS;
	if (choice == BLANK_JOIN) /* Blank JoinDef to be created. */
	{
	    mq_tables(qdef, FALSE);
	}
	else if (choice == DUP_JOIN) /* JoinDef created from duplicate */
	{
	    if (qf_j_dup((MQQDEF *)qdef) == NO_JOIN)
	    {
		*resume = TRUE; /* Resume back to choice of creation method. */
	    }
	}
	return(choice);
}
/*{
** Name:	qf_j_dup	- create a duplicate JoinDef
**
** Description:
**	This routine handles the form whereby a user can create a JoinDef
**	based upon an existing JoinDef.
**
** Inputs:
**	MQQDEF *qdef		- ptr to query definition structure
**
** Outputs:
**	Returns:
**		i4 - returns DUP_JOIN is a duplicate JoinDef was 
**		      successfully operated upon, otherwise returns NO_JOIN
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	01-dec-89  (kenl)  - new interface
*/
static i4
qf_j_dup(qdef)
MQQDEF	*qdef;
{
	i4     retval = DUP_JOIN;
  char *jdefpik;
  char jname[FE_MAXNAME + 1];
	char    hlpbuf[132+1];
/* # line 236 "qbfjedit.qsc" */	/* display */
  {
    if (IIdispfrm(_mqjdup,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 239 "qbfjedit.qsc" */	/* getform */
          {
            if (IIfsetio(_mqjdup) != 0) {
              IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,jname,(char *)
"dup_name");
            } /* IIfsetio */
          }
/* # line 241 "qbfjedit.qsc" */	/* host code */
	    if (jname[0] == EOS)
	    {
		IIUGerr(E_QF0158_no_joindef_name, UG_ERR_ERROR, 0);
/* # line 244 "qbfjedit.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 245 "qbfjedit.qsc" */	/* host code */
	    }
	    /*
	    ** Copy the name the user entered into the main query definition
	    ** structure.  This will be used temporarily to load JoinDef
	    ** information but will be overwritten later by either blanks
	    ** (for a new user created JoinDef) or by the name that was
	    ** supplied along with the -J flag.
	    */
	    STcopy(jname, qdef->mqcr.name);
	    if (!mqgetqname(qdef, FALSE))
	    {
	        FDerror(NOQUERYD, 1, qdef->mqcr.name);
/* # line 258 "qbfjedit.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 259 "qbfjedit.qsc" */	/* host code */
	    }
	    OOcatRecord(qdef->mqcr.id, &qdef->mqcr);
	    /* This will overwrite the OOID later on to force a new JoinDef */
	    mq_joinid = -1;
	    /* if not editing a JoinDef, blank out the old name */
	    if (!mq_editjoin)
	    {
		*(qdef->mqcr.name) = EOS;
	    }
	    mq_tables(qdef, TRUE);
	    /*
	    ** If we are editing a JoinDef (-J) and qdef_type is set to -1
	    ** then something went wrong.  We don't know where but we do
	    ** know that QBF has already spit out an appropriate error.
	    */
	    if ((qdef_type == -1) && mq_editjoin)
	    {
		/* Error message has already been sent. */
/* # line 282 "qbfjedit.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 283 "qbfjedit.qsc" */	/* host code */
            }
/* # line 284 "qbfjedit.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 290 "qbfjedit.qsc" */	/* host code */
	    retval = NO_JOIN;
/* # line 291 "qbfjedit.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 297 "qbfjedit.qsc" */	/* host code */
	    STcopy(ERget(S_QF0159_JoinDefs), hlpbuf);
	    jdefpik = IIUFopObjPik(hlpbuf, ERx("qlpjd.hlp"), OC_JOINDEF,
		ERget(F_QF0028_JoinDefs),
		ERget(F_QF0026_Selecting_JDef_names),
		ERget(F_QF0027_JoinDef_Name));
	    if ((jdefpik != NULL) && (*jdefpik != EOS))
	    {
/* # line 304 "qbfjedit.qsc" */	/* putform */
          {
            if (IIfsetio((char *)"mqjdup") != 0) {
              IIputfldio((char *)"dup_name",(short *)0,1,32,0,jdefpik);
            } /* IIfsetio */
          }
/* # line 305 "qbfjedit.qsc" */	/* host code */
	    }
        }
      } else if (IIretval() == 4) {
        {
/* # line 311 "qbfjedit.qsc" */	/* host code */
	    STcopy(ERget(S_QF015A_JoinDef_from_JoinDef), hlpbuf);
	    FEhelp(ERx("qgtndup.hlp"), hlpbuf);
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
    if (IInmuact(ERget(FE_OK),(char *)0,2,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,2) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,0,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,3) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,4) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,0,4) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 315 "qbfjedit.qsc" */	/* host code */
	return(retval);
}
