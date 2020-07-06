# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**Copyright (c) 1987, 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<lo.h>
# include	<me.h>
# include	<si.h>
# include	<er.h>
# include	<uf.h>
# include	<ifid.h>
# include	<iltypes.h>
# include	<ilops.h>
# include	"ilargs.h"
# include	"il.h"
/**
** Name:	iltief.qsc	-   Routines to edit iltab at runtime
**
** Description:
**
**	IIITiteIlTabEdit	A debugging/test mode routine to
**				allow editing the iltab.
**
** History:
**	20-oct-1988 (Joe)
**		First Written
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* # define's */
/* GLOBALDEF's */
/* extern's */
GLOBALREF IL_OPTAB	IIILtab[];
FUNC_EXTERN i4		IIOgeGenericExec();
/* static's */
/*{
** Name:	IIITiteIlTabEdit	- Routine to allow editing of iltab.
**
** Description:
**	This routine displays a table field which shows the list of
**	il operators contained in the iltab.  The user can "edit" the
**	iltab by turning on and off different operators.  For example
**	if the user wants the interpreter to run, but to ignore RESUME
**	statements, he just has to mark the table field rows for the RESUME
**	operators.  This routine is intended to be used in test mode.
**
**	It displays the form IIITiefIltabEditForm.  This form contains
**	a table field whose rows represent the il operators in iltab.
**	The row contains the statement the il operator implements, and
**	a mark column that can contain an X or a blank.  If it contains
**	an X, then that il operator will be executed by the interpreter
**	during this session.  If it contains a blank, then the interpreter
**	will effectively ignore this operator.
**
**	The main work of this routine is to update the ilfunc element
**	of the iltab table.  As you probably know, each il operator has
**	an entry in iltab.  The entry includes a field call ilfunc which
**	is the function in the interpreter which executes the particular
**	il operator.
**
**	In this routine, if the user marks a row of the table field as
**	being off, then the ilfunc element for the il operator represented
**	by the table field row is set to IIOgeGenericExec.  This routine
**	is essentially a noop routine.  If the user sets the row to on,
**	then the correct function for the il operator is put back into iltab.
**
** Inputs:
**
** Outputs:
**	Returns:
**
** Side Effects:
**	Allocates some memory to hold a copy of the iltab at the
**	point the routine was first invoked.  This allows it to
**	save the function pointers for the operators in case they need
**	to be restored later.
**
** History:
**	20-oct-1988 (Joe)
**		First Written
*/
static bool	run_before = FALSE;
static i4	(**original_il_funcs)();
  VOID
  IIITiteIlTabEdit()
  {
    register IL_OPTAB	*ilp;
    register i4	(**fp)();
    i4 il_token;
    char il_on[2];
    char *il_name;
    if (!run_before)
    {
	LOCATION	*loc;
	i4		num_ops;
	if ((loc = IIUFlcfLocateForm()) == NULL)
	{
	    SIfprintf(stderr, "\r\nIIUFlcfLocateForm returns NULL\n");
	    return;
	}
	if (IIUFgtfGetForm(loc, "IIITiefIltabEditForm") != OK)
	{
	    SIfprintf(stderr, "\r\nCan't get form IIITiefIltabEditForm\n");
	    return;
	}
	run_before = TRUE;
	num_ops = 0;
	for (ilp = IIILtab; ilp->il_name != NULL; ilp++)
	    num_ops++;
	original_il_funcs = (i4 (**)())MEreqmem(0, 
		    (u_i2) num_ops*(sizeof(i4 (*)())), TRUE, (STATUS *) NULL);
	for (ilp = IIILtab, fp = original_il_funcs;
	     ilp->il_name != NULL; ilp++, fp++)
	{
	    *fp = ilp->il_func;
	}
    }
/* # line 133 "iltief.qsc" */	/* display */
    {
      if (IIdispfrm((char *)"IIITiefIltabEditForm",(char *)"f") == 0) goto 
      IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 136 "iltief.qsc" */	/* inittable */
        {
          if (IItbinit((char *)"IIITiefIltabEditForm",(char *)"iltab",(char *)"\
r") != 0) {
            IIthidecol((char *)"token",(char *)"i4");
            IItfill();
          } /* IItbinit */
        }
/* # line 140 "iltief.qsc" */	/* host code */
	il_on[1] = EOS;
	for (ilp = IIILtab, fp = original_il_funcs;
	     ilp->il_name != NULL; ilp++, fp++)
	{
	    il_name = ilp->il_name;
	    il_token = ilp->il_token;
	    /*
	    ** Cases:
	    ** 1) Function in table is not the same as original function
	    **	   then put blank in table field.
	    ** 2) Original function in table is IIOgeGenericExec.
	    **	   then put '=' in table field.
	    ** 3) Function in table is the original function
	    **	   then put 'X' in table field.
	    */
	    il_on[0] = ilp->il_func != *fp ? ' ' :
				(*fp == IIOgeGenericExec ? '=' : 'X');
/* # line 157 "iltief.qsc" */	/* loadtable */
        {
          if (IItbact((char *)"IIITiefIltabEditForm",(char *)"iltab",1) != 0) 
          {
            IItcoputio((char *)"name",(short *)0,1,32,0,il_name);
            IItcoputio((char *)"is_on",(short *)0,1,32,0,il_on);
            IItcoputio((char *)"token",(short *)0,1,30,4,&il_token);
            IITBceColEnd();
          } /* IItbact */
        }
/* # line 163 "iltief.qsc" */	/* host code */
	}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 167 "iltief.qsc" */	/* host code */
	FEtabfnd("iiitiefiltabeditform", "iltab");
          }
        } else if (IIretval() == 2) {
          {
/* # line 171 "iltief.qsc" */	/* getrow */
            {
              if (IItbsetio(2,(char *)"IIITiefIltabEditForm",(char *)
"iltab",-2) != 0) {
                IItcogetio((short *)0,1,32,1,il_on,(char *)"is_on");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 175 "iltief.qsc" */	/* host code */
	if (il_on[0] == '=')
/* # line 176 "iltief.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 177 "iltief.qsc" */	/* host code */
	il_on[0] = il_on[0] != 'X' ? 'X' : ' ';
/* # line 178 "iltief.qsc" */	/* putrow */
            {
              if (IItbsetio(3,(char *)"IIITiefIltabEditForm",(char *)
"iltab",-2) != 0) {
                IItcoputio((char *)"is_on",(short *)0,1,32,0,il_on);
                IITBceColEnd();
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 185 "iltief.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
          }
        } else if (IIretval() == 4) {
          {
            i4 changed;
/* # line 191 "iltief.qsc" */	/* unloadtable */
            {
              if (IItbact((char *)"IIITiefIltabEditForm",(char *)"ilta\
b",0) == 0) goto IItbE1;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,32,1,il_on,(char *)"is_on");
                IItcogetio((short *)0,1,30,4,&il_token,(char *)"token");
                IItcogetio((short *)0,1,30,4,&changed,(char *)"_STATE");
                IITBceColEnd();
                {
/* # line 198 "iltief.qsc" */	/* host code */
		if (changed == 3)
		{
		    IIILtab[il_token].il_func = (il_on[0] != 'X' ?
						   IIOgeGenericExec :
						   original_il_funcs[il_token]);
		}
                }
              } /* IItunload */
IItbE1:
              IItunend();
            }
/* # line 205 "iltief.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
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
      if (IInmuact((char *)"Find",(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact((char *)"Switch",(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact((char *)"Forget",(char *)0,2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact((char *)"Save",(char *)0,2,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(8,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
    {
    }
  }
