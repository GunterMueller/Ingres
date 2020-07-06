# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include       <er.h>
# include       <me.h>
# include       <st.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <abclass.h>
# include <metafrm.h>
# include	"vqaclist.h"
#include        <erug.h>
#include        "ervq.h"
/**
** Name:	vqselcmp.qsc - Select application components
**
** Description:
**	A popup form which allows the user to choose a group of 
**	application components.
**	This file defines:
**
**	IIVQcacChooseApplComp	- Choose an application component
**	IIVQsvfSelVQFrames	- select VQ frames only
**
** History:
**	12/22/89 (Mike S)	Initail version
**	19-oct-1990 (mgw)
**		Fixed #include of local vqaclist.h to use "" instead of <>
**	01-may-91 (davel)
**		Fixed bug 37301 (do not activate on 'cancel' or 'help'
**		menuitems in IIVQcacChooseApplComp).
**	25-Aug-1993 (fredv)
**		Included <me.h>.
**	26-Aug-1993 (fredv)
**		Forgot to remove MEreqmem()'s declaration after I included
**		<me.h>.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* # define's */
/* GLOBALDEF's */
/* extern's */
FUNC_EXTERN STATUS      MEfree();
FUNC_EXTERN VOID        IIUGbmaBadMemoryAllocation();
FUNC_EXTERN VOID        IIUGerr();
FUNC_EXTERN VOID        IIUGmsg();
FUNC_EXTERN VOID        IIUGqsort();
FUNC_EXTERN bool        IIUGyn();
/* static's */
static READONLY char routine[] = ERx("IIVQcacChooseApplComp");
static  bool            init_done = FALSE;      /* Initialization flag */
static READONLY
  char _form[]= ERx("vqmarkfm"), _tf[]= ERx("vqmarkfm"), _name[]= ERx("name"), 
  _use[]= ERx("use"), _title[]= ERx("title");
static
  char *_yes, *_no;
static i4	cmp_comp();
/*{
** Name:	IIVQcacChooseApplComp   - Choose an application component
**
** Description:
**	Allow the user to choose a set of application components.
**
**	Two functions are specified: 
**
**	sel_rtn selects which components the user sees:
**
**		bool sel_rtn(appl, comp)
**		APPL *appl;		/* Application 
**		APPL_COMP *comp;	/* Component 
**
**		Returns:	TRUE to select it
**				FALSE otherwise
**
**	an example is IIVQsvfSelVQFrames, below.
**	ok_rtn is called if the user chooses "OK" from the display, and
**	at least one component was chosen.
**
**		STATUS ok_rtn(appl, comps, no_comps, data)
**		APPL *appl;		/* Application
**		AC_LIST *comps;		/* Component list 
**		i4	no_comps;	/* Size of list
**		PTR	data;		/* Passed-through data
**
**	If sel_rtn selects no components, IIVQcacChooseApplComp returns
**	with a status of OK.
**	
** Inputs:
**	appl		APPL * 		Application structure
**	ok_rtn		STATUS () 	Routine to call on "OK"
**	sel_rtn		bool () 	Routine to select components 
**	defuse		bool		Default components to "yes" or "no"
**	title		char *		Title for popup
**	helpfile	char *		Help file for display loop
**	banner		char *		Help display banner
**	data		PTR		Data to pass to ok_rtn
**
** Outputs:
**	any		bool *		Were any components selected by sel_rtn
**
**	Returns:
**		STATUS		OK if Cancel is chosen
**				OK if sel_rtn selects no components.	
**				return from ok_rtn otherwise
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	12/22/89 (Mike S)	Initail version
**	01-may-91 (davel)
**		Fixed bug 37301 (do not activate on 'cancel' or 'help'
**		menuitems).
*/
STATUS
IIVQcacChooseApplComp(appl, ok_rtn, sel_rtn, defuse, title, helpfile, banner, 
		      data, any)
APPL	*appl;
STATUS  (*ok_rtn)();   
bool	(*sel_rtn)();
bool	defuse;
  char *title;
char	*helpfile;
char	*banner;
PTR	data;
bool	*any;
{
	i4		no_comps;	/* Number of components selected */
	STATUS		status;		/* Final status */
  i4 tfrec;
	APPL_COMP       *comp;
	AC_LIST		*aclist;
	AC_LIST		*acptr;
        /* Initialize, if need be */
        if (!init_done)
        {
                IIARiaForm(_form);
                _yes = ERget(F_UG0001_Yes1);
                _no = ERget(F_UG0006_No1);
                init_done = TRUE;
        }
	/* Collect and sort the components */
	no_comps = 0;
        for (comp = (APPL_COMP *)appl->comps; comp != NULL; comp = comp->_next)
        {
		if ((*sel_rtn)(appl, comp))
			no_comps++;
        }
        /* Return if there are none */
        if (no_comps == 0)
	{
		*any = FALSE;
                return (OK);
	}
	*any = TRUE;
        /* Create an array for them */
        aclist = (AC_LIST *)
                        MEreqmem( (u_i4)0, no_comps * sizeof(AC_LIST),
                                  FALSE, NULL);
        if (aclist == NULL)
                IIUGbmaBadMemoryAllocation(routine);
        /* Second pass, put them into the array */
        acptr = aclist;
        for (comp = (APPL_COMP *)appl->comps; comp != NULL; comp = comp->_next)
	{
		if ((*sel_rtn)(appl, comp))
		{
			acptr->comp = comp;
			acptr->used = defuse;
			acptr++;
		}
	}
	/* Now sort them alphabetically */
	IIUGqsort((char *)aclist, no_comps, sizeof(AC_LIST), cmp_comp);
	/* Allow the user to choose which he wants */
/* # line 202 "vqselcmp.qsc" */	/* display */
  {
    if (IIdispfrm(_form,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
      char *use;
      char *name;
      i4 i;
/* # line 209 "vqselcmp.qsc" */	/* putform */
      {
        if (IIfsetio(_form) != 0) {
          IIputfldio(_title,(short *)0,1,32,0,title);
        } /* IIfsetio */
      }
/* # line 211 "vqselcmp.qsc" */	/* host code */
                /* Initialize and load the tablefield */
/* # line 212 "vqselcmp.qsc" */	/* inittable */
      {
        if (IItbinit(_form,_tf,(char *)"u") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 213 "vqselcmp.qsc" */	/* host code */
                for (i = 0; i < no_comps; i++)
                {
                        use = aclist[i].used ? _yes : _no;
                        name = aclist[i].comp->name;
/* # line 217 "vqselcmp.qsc" */	/* loadtable */
      {
        if (IItbact(_form,_tf,1) != 0) {
          IItcoputio(_name,(short *)0,1,32,0,name);
          IItcoputio(_use,(short *)0,1,32,0,use);
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 219 "vqselcmp.qsc" */	/* host code */
                }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
          char used[5];
          i4 comp_no;
/* # line 228 "vqselcmp.qsc" */	/* host code */
                i4   no_used = 0;
                /* Go ahead and do it, using the currently marked comps */
/* # line 231 "vqselcmp.qsc" */	/* unloadtable */
          {
            if (IItbact(_form,_tf,0) == 0) goto IItbE1;
            while (IItunload() != 0) {
              IItcogetio((short *)0,1,32,4,used,_use);
              IItcogetio((short *)0,1,30,4,&comp_no,(char *)"_RECORD");
              IITBceColEnd();
              {
/* # line 233 "vqselcmp.qsc" */	/* host code */
                        if (aclist[comp_no-1].used = 
						IIUGyn(used,(STATUS *)NULL))
                        	no_used++; 
              }
            } /* IItunload */
IItbE1:
            IItunend();
          }
/* # line 238 "vqselcmp.qsc" */	/* host code */
                /* If no comps were marked, complain */
                if (no_used == 0)
                {
                        IIUGerr(E_VQ009D_Nothing_marked, 0, 0);
                }
                else
                {
                       	status = (*ok_rtn)(appl, aclist, no_comps, data);
/* # line 246 "vqselcmp.qsc" */	/* clear */
          {
            IIfldclear(_tf);
          }
/* # line 247 "vqselcmp.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 248 "vqselcmp.qsc" */	/* host code */
                }
        }
      } else if (IIretval() == 2) {
        {
/* # line 255 "vqselcmp.qsc" */	/* host code */
                /* Don't do it */
		status = OK;
/* # line 257 "vqselcmp.qsc" */	/* clear */
          {
            IIfldclear(_tf);
          }
/* # line 258 "vqselcmp.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 265 "vqselcmp.qsc" */	/* host code */
                FEhelp(helpfile, banner);
        }
      } else if (IIretval() == 4) {
        {
          char use[5];
/* # line 272 "vqselcmp.qsc" */	/* getrow */
          {
            if (IItbsetio(2,_form,_tf,-2) != 0) {
              IItcogetio((short *)0,1,32,4,use,_use);
              IItcogetio((short *)0,1,30,4,&tfrec,(char *)"_RECORD");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 273 "vqselcmp.qsc" */	/* host code */
                if ((STcompare(use, _yes) == 0) || STcompare(use, _no) == 0)
                {
/* # line 275 "vqselcmp.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
/* # line 276 "vqselcmp.qsc" */	/* host code */
                }
                else
                {
                        IIUGerr(E_VQ009F_Bad_mark, 0, 0);
/* # line 280 "vqselcmp.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 281 "vqselcmp.qsc" */	/* host code */
                }
        }
      } else if (IIretval() == 5) {
        {
/* # line 286 "vqselcmp.qsc" */	/* host code */
                /* Avoid out-of-data message */
                if (1 != tfrec)
                {
/* # line 289 "vqselcmp.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_tf,-3) != 0) {
              IItbsmode((char *)"down");
              if (IItscroll(0,-3) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 290 "vqselcmp.qsc" */	/* host code */
                }
        }
      } else if (IIretval() == 6) {
        {
/* # line 295 "vqselcmp.qsc" */	/* host code */
                /* Avoid out-of-data message */
          i4 tfrows;
/* # line 298 "vqselcmp.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&tfrows,32,_tf,0);
            } /* IIiqset */
          }
/* # line 300 "vqselcmp.qsc" */	/* host code */
                if (tfrows != tfrec)
                {
/* # line 302 "vqselcmp.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_tf,-3) != 0) {
              IItbsmode((char *)"up");
              if (IItscroll(0,-3) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 303 "vqselcmp.qsc" */	/* host code */
                }
        }
      } else if (IIretval() == 7) {
        {
/* # line 308 "vqselcmp.qsc" */	/* host code */
                /* top */
/* # line 309 "vqselcmp.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_tf,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 8) {
        {
/* # line 314 "vqselcmp.qsc" */	/* host code */
                /* bottom */
/* # line 315 "vqselcmp.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_tf,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,-1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
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
    if (IInmuact(ERget(FE_OK),ERget(F_VQ012E_UseMarkedComps),2,1,1) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,1,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),2,0,2) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,0,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),2,0,3) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,0,3) == 0) goto IIfdE1;
    if (IITBcaClmAct(_tf,_use,0,4) == 0) goto IIfdE1;
    if (IIactscrl(_tf,1,5) == 0) goto IIfdE1;
    if (IIactscrl(_tf,0,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,7) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,8) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 318 "vqselcmp.qsc" */	/* host code */
	MEfree((PTR)aclist);
	return (status);
}
/*
**      cmp_comp
**
**      Compare two components alphabetically
*/
static i4
cmp_comp(comp1, comp2)
char *comp1;
char *comp2;
{
        return (STcompare(((AC_LIST *)comp1)->comp->name, 
			  ((AC_LIST *)comp2)->comp->name));
}
/*{
** Name:	IIVQsvfSelVQFrames -- selection routine to delect VQ frames
**
** Description:
**	Return TRUE only for frames which contain a VQ
**
** Inputs:
**	appl	APPL *		Application
**	comp	APPL_COMP * 	Component
**	
**
** Outputs:
**	none
**
**	Returns:
**		TRUE	if the component contains a visual query
**		FALSE 	otherwise
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	12/22/89 (Mike S)	Initial version
*/
bool
IIVQsvfSelVQFrames(appl, comp)	
APPL	*appl;
APPL_COMP	*comp;
{
	switch (comp->class)
	{
	    case OC_APPFRAME:
	    case OC_UPDFRAME:
	    case OC_BRWFRAME:
		return (TRUE);
	    default:
		return (FALSE);
	}
}
