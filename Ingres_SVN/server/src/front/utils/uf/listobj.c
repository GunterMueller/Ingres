# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<lo.h>
# include	<st.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ft.h>
# include	<fmt.h>
# include	<adf.h>
# include	<frame.h>
# include	<afe.h>
# include	<uf.h>
# include	<ug.h>
# include	"eruf.h"
# include <ooclass.h>
# include <uigdata.h>
/**
** Name:	listobjs.qc - Call ListPick on lists of objects.
**
** Description:
**	This file contains routines to display a list of a specified
**	type of object from which the user can select.
**
**	IIUFopObjPik	Allow user to select from a list of names.
**
**	This file contains code that modifies internal frame and field
**	data structures to change the displayed size of those objects
**	each time they are displayed.  This code is here only by special
**	dispensation; it must be kept in sync with frs code.
**
** History:
**	26-oct-89 (bruceb)
**		Written (much swiped from Vifred's IIVFlfListForms routine.)
**	25-jul-90 (bruceb)
**		FDsetparse() back to original value after adding form.
**	23-apr-96 (chech02)
**		fixed compiler complaint for windows 3.1 port.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# define	MAX_TFROWS	10	/* Max rows displayed in list tblfld. */
FUNC_EXTERN	bool	FDsetparse();
#ifndef WIN16
LOCATION	*IIUFlcfLocateForm();
#endif
i4		IIFRrlpRunListPick();
FIELD		*FDfndfld();
FRAME		*IIfindfrm();
/* uflistform is the one form used for displaying lists of 'object' names. */
static READONLY
  char list_form[]= ERx("uflistform"), list[]= ERx("list");
  static char choice[FE_MAXNAME + 1];
static bool	lf_alloced = FALSE;
static i4	maxy_form = 0;	/* Max length of the list_form form. */
static i4	maxy_tbl = 0;	/* Max length of the list table field. */
static FRAME	*frm;
static TBLFLD	*tbl;
static FLDHDR	*tbl_hdr;
/*{
** Name:	allocLF	- Allocate the list_form form.
**
** Description:
**	Make the list_form form runnable.  Also set up pointers to the
**	internal form structures to enable the form and table field to
**	resize as required when displaying smaller amounts of data than
**	the amount defined for the form.
**
** Side Effects:
**	Defines several static variables.
**
** History:
**	26-oct-89 (bruceb)	Written.
*/
static VOID
allocLF()
{
	FIELD	*fd;
	bool	dummy;
	bool	oval;
	if (!lf_alloced)
	{
	    oval = FDsetparse(FALSE);
	    if (IIUFgtfGetForm(IIUFlcfLocateForm(), list_form))
	    {
		IIUGerr( E_UF0026_BadForm, UG_ERR_FATAL, 1, (PTR)list_form );
	    }
	    frm = IIfindfrm(list_form);
	    maxy_form = frm->frmaxy;
	    fd = FDfndfld(frm, list, &dummy);
	    tbl = fd->fld_var.fltblfld;
	    tbl_hdr = &(tbl->tfhdr);
	    maxy_tbl = tbl_hdr->fhmaxy;
	    lf_alloced = TRUE;
	    _VOID_ FDsetparse(oval);
	}
}
/*{
** Name:	initTF	- Initialize the list table field.
**
** Description:
**	Initialize the list table field with data after setting the
**	size of the form and table field to match the amount of data.
**
** Inputs:
**	num_names	Number of rows to display in the table field.
**	datalist	Names to load; num_names of them.
**
** Outputs:
**
**	Returns:
**		VOID
**
**	Exceptions:
**		None.
**
** Side Effects:
**	Loads num_names data rows into the table field.
**
** History:
**	26-oct-89 (bruceb)	Written.
*/
static VOID
initTF(num_names, datalist)
i4	num_names;
  char datalist[][FE_MAXNAME + 1];
{
	i4	size_dif = (MAX_TFROWS - num_names);
	i4	i;
	frm->frmaxy = maxy_form - size_dif;
	tbl_hdr->fhmaxy = maxy_tbl - size_dif;
	tbl->tfrows = num_names;
/* # line 158 "listobj.qsc" */	/* inittable */
  {
    if (IItbinit(list_form,list,(char *)"r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 160 "listobj.qsc" */	/* host code */
	for (i = 0; i < num_names; i++)
	{
/* # line 162 "listobj.qsc" */	/* loadtable */
  {
    if (IItbact(list_form,list,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,datalist[i]);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 163 "listobj.qsc" */	/* host code */
	}
}
/*{
** Name:	IIUFopObjPik	- Allow user to select from a list of names.
**
** Description:
**	Generate list of the specified type of object.  Then call ListPick
**	on that list, allowing for user selection from the list.
**
**	The allowed object classes are those stored in ii_objects.
**
**	List contains those objects accessible to the user.  This includes
**	those objects owned by the user, and those owned by the DBA that
**	don't conflict with (the name of) objects owned by the user.
**	If FIPS on front end objects becomes a reality, then the query needs
**	to change accordingly.
**
** Inputs:
**	htitle		Help screen title -- NULL if no help menuitem desired.
**	hfile		Help file name.  Searched for in the 'helpfile'
**			directory.
**	objclass	Object type to list.  (e.g.  OC_FORM)
**	objtype		Name of object type (plural, e.g. 'forms') for use in
**			error message if none are available to list.
**	fmsg		Message to display while loading the form.
**			(e.g.  "Selecting form names. . .")
**	ftitle		Title for the list-form.  (e.g.  "Form Name")
**
** Outputs:
**
**	Returns:
**		Pointer to name of object selected.  NULL or EOS on
**		'Cancel' or if no such objects found.
**
**	Exceptions:
**		None.
**
** Side Effects:
**	None.
**
** History:
**	26-oct-89 (bruceb)	Written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
char	*
IIUFopObjPik(htitle, hfile, objclass, objtype, fmsg, ftitle)
char	*htitle;
char	*hfile;
  OOID objclass;
char	*objtype;
  char *fmsg;
  char *ftitle;
{
  char *objname;
  char owner[FE_MAXNAME + 1];
  UIDBDATA *uidbdata;
  char *loadname;
	char	*names[2];
	char	*nameptr;
	char	buf1[FE_MAXNAME + 1];
	char	buf2[FE_MAXNAME + 1];
	i4	objcount;
	i4	userfirst;
	i4	state;
  i4 row;
	bool	need_to_init = TRUE;
	char	nmlist[MAX_TFROWS][FE_MAXNAME + 1];
/* # line 239 "listobj.qsc" */	/* message */
  {
    IImessage(fmsg);
  }
/* # line 241 "listobj.qsc" */	/* host code */
	allocLF();
/* # line 242 "listobj.qsc" */	/* putform */
  {
    if (IIfsetio(list_form) != 0) {
      IIputfldio((char *)"title",(short *)0,1,32,0,ftitle);
    } /* IIfsetio */
  }
/* # line 244 "listobj.qsc" */	/* host code */
	names[0] = buf1;
	names[1] = buf2;
	objcount = 0;
	uidbdata = IIUIdbdata();
	userfirst = (STcompare(uidbdata->user, uidbdata->dba) <= 0 ? 1 : 0);
	state = 0;
	objname = names[0];
	IIUIcnb_CatNolockBegin(ERx("ii_objects"));
/* # line 254 "listobj.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"listobj1",6325,12797);
      IIputdomio((short *)0,1,30,4,&objclass);
      IIputdomio((short *)0,1,32,0,uidbdata->user);
      IIputdomio((short *)0,1,32,0,uidbdata->dba);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"listobj1",6325,12797);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select object_name, object_owner from ii_objects where object_class=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objclass);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->user);
        IIwritio(0,(short *)0,1,32,0,(char *)"or object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->dba);
        IIwritio(0,(short *)0,1,32,0,(char *)
")order by object_name, object_owner");
        IIexDefine(0,(char *)"listobj1",6325,12797);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,0,objname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,owner);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 262 "listobj.sc" */	/* host code */
	    if (state == 0)
	    {
		objname = names[1];
		state++;
	    }
	    else
	    {
		if (STequal(names[0], names[1]))
		{
		    if (userfirst)
		    {
			loadname = names[0];
		    }
		    else
		    {
			loadname = names[1];
		    }
		    objname = names[0];
		    state = 0;
		}
		else
		{
		    loadname = nameptr = objname = names[0];
		    names[0] = names[1];
		    names[1] = nameptr;
		}
		if (objcount < MAX_TFROWS)
		{
		    STcopy(loadname, nmlist[objcount]);
		}
		else
		{
		    if (need_to_init)
		    {
			initTF(MAX_TFROWS, nmlist);
			need_to_init = FALSE;
		    }
/* # line 299 "listobj.qsc" */	/* loadtable */
  {
    if (IItbact(list_form,list,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,loadname);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 300 "listobj.qsc" */	/* host code */
		}
		objcount++;
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	IIUIcne_CatNolockEnd(ERx("ii_objects"));
	if (state)
	{
	    loadname = names[0];
	    if (objcount < MAX_TFROWS)
	    {
		STcopy(loadname, nmlist[objcount]);
	    }
	    else
	    {
		if (need_to_init)
		{
		    initTF(MAX_TFROWS, nmlist);
		    need_to_init = FALSE;
		}
/* # line 319 "listobj.qsc" */	/* loadtable */
  {
    if (IItbact(list_form,list,1) != 0) {
      IItcoputio((char *)"name",(short *)0,1,32,0,loadname);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 320 "listobj.qsc" */	/* host code */
	    }
	    objcount++;
	}
	/*
	**  If user has no objects (of the requested type:  forms, reports, ...)
	**  that can be accessed, display error message and return.
	*/
	if (!objcount)
	{
	    IIUGerr(E_UF0025_No_such_objects, UG_ERR_ERROR, 1, objtype);
	    return(NULL);
	}
	if (need_to_init)
	{
	    initTF(objcount, nmlist);
	}
	row = IIFRrlpRunListPick(list_form, list, -1, -1,
	    htitle, hfile, NULL, NULL);
	if (row < 0)
	{
	    choice[0] = EOS;	/* User chose to Cancel. */
	}
	else
	{
	    row++;	/* Adjust value returned by ListPick to Scroll value. */
/* # line 349 "listobj.qsc" */	/* scroll */
  {
    if (IItbsetio(1,list_form,list,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,row) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 350 "listobj.qsc" */	/* getrow */
  {
    if (IItbsetio(2,list_form,list,-2) != 0) {
      IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,choice,(char *)"name");
      IITBceColEnd();
    } /* IItbsetio */
  }
/* # line 351 "listobj.qsc" */	/* host code */
	}
/* # line 353 "listobj.qsc" */	/* inittable */
  {
    if (IItbinit(list_form,list,(char *)"r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 355 "listobj.qsc" */	/* host code */
	return(choice);
}
