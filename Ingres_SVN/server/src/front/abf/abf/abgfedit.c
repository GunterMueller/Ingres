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
#include	<fdesc.h>
#include	<abfrts.h>
# include <abclass.h>
# include <oocat.h>
# include "abclinfo.h"
#include	"erab.h"
/**
** Name:	abgfedit.qc -	ABF Graph Frame Component Edit Module.
**
** Description:
**	Contains the definition of the component class information structure
**	for the graph frame component object.  Defines:
**
**	iiABgraphFrame	ABF graph frame component class edit information.
**
** History:
**	Revision 6.4
**	03/22/91 (emerson)
**		Fix interoperability bug 36589:
**		Change all calls to abexeprog to remove the 3rd parameter
**		so that images generated in 6.3/02 will continue to work.
**		(Generated C code may contain calls to abexeprog).
**		This parameter was introduced in 6.3/03/00, but all calls
**		to abexeprog specified it as TRUE.  See abfrt/abrtexe.c
**		for further details.
**
**	11/16/92 (donc)
**		Modified COMP_CLASS_INFO initialization to accomodate fix for bug 44162.
**
**	Revision 6.2  89/02  wong
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
STATUS	iiabPrint();
STATUS	iiabVerifyType();
static STATUS	_test();
static STATUS	_edit();
static VOID	_display();
static STATUS	_save();
/*:
** Name:	iiABgraphFrame -	ABF Graph Frame Component Class Edit
**						Information.
** Description:
**	Defines additional class information for the Graph frame component
**	objects.
*/
#define _NewEditExpl	F_AB0052_NewEdit_expl
#define _GoExpl		F_AB0053_Go_expl
#define _EditExpl	F_AB0059_EditGr_expl
#define _EmptyExpl	F_AB005E_Empty_expl
#define _PrintExpl	F_AB005F_Print_expl
static READONLY
  char _graph[]= ERx("graph");
GLOBALDEF READONLY COMP_CLASS_INFO iiABgraphFrame = {
	OC_GRFRAME,
	ERx("abfgraphd"), ERx("abfgrafd.hlp"), ERx("abfgrami.hlp"), 1,
	ER_OK,
	F_AB0043_AGFDet_title,
	0,	NULL,	/* no types legal */
	{_graph,	NULL},	NULL,	NULL,	NULL, NULL, 	/* Fields */
	/*{iiabNxtEdit,	AB_NewEdit,	_NewEditExpl,	0},	/* NextEdit */
	{_test,		FE_Go,		_GoExpl,	4}, /* Go, FRSkey4 */
	{_edit,		FE_Edit,	_EditExpl,	0},	/* Edit */
	{NULL,		0,		_EmptyExpl,	0},	/* Compile */
	{NULL,		0,		_EmptyExpl,	0},	/* FormEdit */
	{iiabPrint,	FE_Print,	_PrintExpl,	0},	/* Print */
	_display, _save, NULL, iiabVerifyType
};
/*
** Name:	_edit() -	Edit Graph Frame Component Graph Object
**					from a Form.
** Description:
**	Edits the graph object as specified on the component definition form
**	for a graph frame component object.
**
** Inputs:
**	app	{APPL *}  The application object.
**	form	{char *}  The component definition form name.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	02/89 (jhw)  Written.
*/
static STATUS
  _edit( app, dummy, form )
register APPL	*app;
OO_OBJECT *dummy;
  char *form;
  {
    char graph[FE_MAXNAME+1];
/* # line 119 "abgfedit.qsc" */	/* getform */
    {
      if (IIfsetio(form) != 0) {
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,graph,_graph);
      } /* IIfsetio */
    }
/* # line 121 "abgfedit.qsc" */	/* host code */
	abexeprog(ERx("vigraph"), ERx("graph = %S"), 1, graph);
  }
/*
** Name:	_display() -	Display a Graph Frame Component Definition.
**
** Description:
**	Displays the attributes of a graph frame component on its component
**	definition form.
**
** Inputs:
**	frm	{QBF_FRAME *}  The graph frame component object.
**
** History:
**	02/89 (jhw)  Written.
*/
static VOID
  _display( frm )
  register GRAPH_FRAME *frm;
  {
    char *formname = iiABgraphFrame.form;
/* # line 143 "abgfedit.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIputfldio(_graph,(short *)0,1,32,0,frm->graph);
        IIputfldio(_comline,(short *)0,1,32,0,frm->cmd_flags);
      } /* IIfsetio */
    }
  }
/* # line 148 "abgfedit.qsc" */	/* host code */
/*
** Name:	_save() -	Save Graph Frame Component Definition from
**					the Form.
** Description:
**	Saves the attributes for a graph frame component object from the
**	component definition form for this class.  This checks if any values
**	changed before modifying the object's attributes and only then writing
**	the object to the database.
**
**	The values that can be changed are the graph object name, the optional
**	output file, and the optional Graph Writer command line.
**
** Input:
**	app	{APPL *}  The application object.
**	frm	{GRAPH_FRAME *}  The graph frame component object.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	02/89 (jhw)  Written.
**	05/90 (jhw)  Added long remark check for save.
*/
static STATUS
  _save( app, frm )
APPL			*app;
register GRAPH_FRAME	*frm;
  {
    register char *formname = iiABgraphFrame.form;
    i4 gr_change;
    i4 cmd_change;
    i4 srem_change;
	STATUS	IIAMwrGraphFrame();
/* # line 184 "abgfedit.qsc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&gr_change,20,_graph,0);
        IIiqfsio((short *)0,1,30,4,&cmd_change,20,_comline,0);
        IIiqfsio((short *)0,1,30,4,&srem_change,20,_short_remark,0);
      } /* IIiqset */
    }
/* # line 190 "abgfedit.qsc" */	/* host code */
	/* Fields should have been validated already. */
	if ( gr_change )
    {
      char graph[FE_MAXNAME+1];
/* # line 196 "abgfedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,graph,_graph);
        } /* IIfsetio */
      }
/* # line 197 "abgfedit.qsc" */	/* host code */
		/* Graph is a required DB reference */
		if ( !STequal(frm->graph, graph) )
		{
			frm->graph = FEtsalloc( frm->data.tag, graph );
			frm->data.dirty = TRUE;
		}
    }
	if ( cmd_change )
    {
      char cmdline[48+1];
/* # line 209 "abgfedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,48+1-1,cmdline,_comline);
        } /* IIfsetio */
      }
/* # line 210 "abgfedit.qsc" */	/* host code */
		if ( !STequal(frm->cmd_flags, cmdline) )
		{
			frm->cmd_flags = FEtsalloc( frm->data.tag, cmdline );
			frm->data.dirty = TRUE;
		}
    }
	if ( srem_change != 0 )
    {
		char	*cp;
      char short_remark[OOSHORTREMSIZE+1];
/* # line 222 "abgfedit.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
          IIstfsio(20,_short_remark,0,(short *)0,1,30,sizeof(0),
          (void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 223 "abgfedit.qsc" */	/* getform */
      {
        if (IIfsetio(formname) != 0) {
          IIgetfldio((short *)0,1,32,OOSHORTREMSIZE+1-1,short_remark,
          _short_remark);
        } /* IIfsetio */
      }
/* # line 224 "abgfedit.qsc" */	/* host code */
		if ( ! STequal(frm->short_remark, short_remark)
			&& (cp = FEtsalloc(frm->data.tag, short_remark))
				!= NULL )
		{
			frm->short_remark = cp;
			frm->data.dirty = TRUE;
		}
    }
	_VOID_ IIOOckLongRemark((OO_OBJECT *)frm);
	return frm->data.dirty ? iiabSvComp((APPL_COMP *)frm, IIAMwrGraphFrame)
				: OK;
  }
/*
** Name:	_test() -	Test Graph Frame Component Object.
**
** Description:
**	Runs the graph frame for testing purposes.  This simply creates a
**	run-time frame structure from the frame definition and then runs it.
**
** Input:
**	app	{APPL *}  The application object.
**	fclass	{COMP_CLASS_INFO *}  The graph frame component class edit
**					information.
**	frm	{GRAPH_FRAME *}  The graph frame component object.
**
** Returns:
**	{STATUS}  OK, if no errors.
**
** History:
**	02/89 (jhw)  Written.
*/
static STATUS
  _test( app, fclass, frm )
APPL		*app;
COMP_CLASS_INFO	*fclass;
GRAPH_FRAME	*frm;
  {
    char *form = iiABgraphFrame.form;
    char graph[FE_MAXNAME+1];
    char output[48+1];
    char comline[ABCOMSIZE+1];
	ABRTSFRM	rtsfrm;
	ABRTSVAR	rtsvar;
	ABRTSFO		rtsform;
	/*
	** Build an ABRTSFRM structure and pass it
	** to the run-time routine 'IIARvgfGraph()'.
	*/
/* # line 277 "abgfedit.qsc" */	/* getform */
    {
      if (IIfsetio(form) != 0) {
        IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,graph,_graph);
        IIgetfldio((short *)0,1,32,ABCOMSIZE+1-1,comline,_comline);
      } /* IIfsetio */
    }
/* # line 281 "abgfedit.qsc" */	/* host code */
	rtsfrm.abrfrname = frm->name;
	rtsfrm.abrfrtype = OC_GRFRAME;
	rtsfrm.abrfrvar = &rtsvar;
	STcopy(graph, rtsvar.abrfrvgbf.abrgfname);
	STcopy(comline, rtsvar.abrfrvgbf.abrgfcomline);
	rtsfrm.abrform = NULL;
	IIARvgfGraph(&rtsfrm, (ABRTSPRM *)NULL);
	return OK;
  }
