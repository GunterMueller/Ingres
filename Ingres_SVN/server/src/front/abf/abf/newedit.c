# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<adf.h>
# include <abclass.h>
# include <oocatlog.h>
# include <oocat.h>
#include	<oodefine.h>
# include "abclinfo.h"
#include	"abcatrow.h"
#include	"erab.h"
#include	<erar.h>
/**
** Name:	newedit.qc -	ABF Application Select New Component Object
**					for Editing SubMenu.
** Description:
**	Contains the routine that implements the "NewEdit" submenu.  Defines:
**
**	iiabNxtEdit()	select new component for editing submenu.
**
** History:
**	Revision 6.2  89/02  wong
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*{
** Name:	iiabNxtEdit() -	Select New Component for Editing SubMenu.
**
** Description:
**
** Inputs:
**	appl	{APPL *}  The application object.
**	cat	{OO_CATALOG *}  The visual objects catalog.
**
** Returns:
**	{APPL_COMP *}  The application component object selected for editing.
**
** History:
**	02/89 (jhw)  Written.
*/
APPL_COMP	*iiabGetComp();
static APPL_COMP	*_scan();
#define _CreateExpl	F_AB0015_CreateObj_expl
#define _FrameExpl	F_AB0070_Frame_expl
#define _ProcedureExpl	F_AB0071_Procedure_expl
#define _HelpExpl	F_FE0100_ExplFrameHelp
#define _EndExpl	F_FE0102_ExplEnd
APPL_COMP *
iiabNxtEdit ( appl, cat )
APPL		*appl;
OO_CATALOG	*cat;
{
	for (;;)
/* # line 75 "newedit.qsc" */	/* submenu */
  {
    goto IImuI1;
IIfdB1:
    IIrunmu(0);
    if (IIretval() == 1) {
      {
/* # line 78 "newedit.qsc" */	/* host code */
		OO_OBJECT	*obj;
		OOID		id = OC_UNDEFINED;
		STATUS	IIABcreateComp();
		if ( IIABcreateComp( appl, &id, cat ) == OK  &&
				(obj = OOp(id)) != NULL )
		{
			return (APPL_COMP *)obj;
		}
      }
    } else if (IIretval() == 2) {
      {
/* # line 92 "newedit.qsc" */	/* host code */
		char	buf[2*FE_MAXNAME];
		_VOID_ abinpname( ERget(F_AB0073_Frame_name),
					buf, sizeof(buf)-1
		);
		if ( STtrmwhite(buf) > 0 )
		{
			if ( FEchkname(buf) != OK )
			{
				i4 len;
				len = FE_MAXNAME;
				IIUGerr( E_AB0131_BadFrame, UG_ERR_ERROR,
						2, buf, &len
				);
			}
			else
			{
				CVlower(buf);
				return _scan( appl, cat, buf, OC_APPLFRAME );
			}
		}
		continue;
      }
    } else if (IIretval() == 3) {
      {
/* # line 120 "newedit.qsc" */	/* host code */
		char	buf[2*FE_MAXNAME];
		_VOID_ abinpname( ERget(F_AB0074_Procedure_name),
					buf, sizeof(buf)-1
		);
		if ( STtrmwhite(buf) > 0 )
		{
			if ( FEchkname(buf) != OK )
			{
				i4 len;
				len = FE_MAXNAME;
				IIUGerr( E_AB0133_BadProcedure, UG_ERR_ERROR,
						2, buf, &len
				);
			}
			else
			{
				CVlower(buf);
				return _scan( appl, cat, buf, OC_OSLPROC );
			}
		}
		continue;
      }
    } else if (IIretval() == 4) {
      {
/* # line 148 "newedit.qsc" */	/* host code */
		FEhelp(ERx("abnwedit.hlp"), ERget(F_AB0072_NewEdit_help));
		continue;
      }
    } else if (IIretval() == 5) {
      {
/* # line 154 "newedit.qsc" */	/* host code */
		return NULL;
      }
    } else {
      if (1) goto IIfdE1;
    } /* IIretval */
    goto IIfdE1;
IImuI1:
    IImuonly();
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Create),ERget(_CreateExpl),2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Frame),ERget(_FrameExpl),2,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Procedure),ERget(_ProcedureExpl),2,2,3) == 0) goto 
    IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),ERget(_EndExpl),2,2,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdB1;
IIfdE1:;
  }
/* # line 156 "newedit.qsc" */	/* host code */
	/* end sub menu */
}
# define	stNEW		1
# define	stUNCHANGED	2
# define	stCHANGE	3
/*
** Name:	_scan() -	Scan Visual Object Catalog for Component
**					Frame or Procedure.
** Description:
**	Scans the rows in the visual object catalog table field looking for
**	a specific component by name.  If found the table field will be
**	scrolled to that row and the component will be returned.
**
** Inputs:
**	appl	{APPL *}  The application object.
**	cat	{OO_CATALOG *}  The visual objects catalog.
**	name	{char *}  The component name.
**	class	{OOID}  The component class.
**
** Returns:
**	{APPL_COMP *}  The component, or NULL if not found.
**
** History:
**	02/89 (jhw)  Written.
*/
static APPL_COMP *
  _scan( appl, cat, name, class )
APPL		*appl;
  OO_CATALOG *cat;
char		*name;
OOID		class;
  {
	bool		frame;
	bool		found;
    i4 row_no;
    i4 status;
    OO_CATREC row;
	frame = ( class >= OC_APPLFRAME && class < OC_APPLFRAME + 99 );
	found = FALSE;
/* # line 199 "newedit.qsc" */	/* unloadtable */
    {
      if (IItbact(cat->c_form,cat->c_tfield,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,FE_MAXNAME+1-1,row.name,_name);
        IItcogetio((short *)0,1,30,4,&row.class,_class);
        IItcogetio((short *)0,1,30,4,&row_no,(char *)"_RECORD");
        IItcogetio((short *)0,1,30,4,&status,(char *)"_STATE");
        IITBceColEnd();
        {
/* # line 204 "newedit.qsc" */	/* host code */
		if ( ( status == stUNCHANGED || status == stCHANGE ||
				status == stNEW )  &&  STequal(row.name, name) )
		{
			found = TRUE;
			if ( ( frame && row.class >= OC_APPLFRAME &&
					row.class < OC_APPLFRAME + 99 ) ||
				( !frame && ( row.class == OC_OSLPROC ||
						row.class == OC_HLPROC ||
						row.class == OC_DBPROC ) ) )
			{
/* # line 214 "newedit.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 215 "newedit.qsc" */	/* host code */
			}
		}
		else if ( found )
		{
			found = FALSE;
/* # line 220 "newedit.qsc" */	/* endloop */
          {
            if (1) goto IItbE1;
          }
/* # line 221 "newedit.qsc" */	/* host code */
		}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 224 "newedit.qsc" */	/* host code */
	if ( !found )
	{
		IIUGerr( E_AB0137_UnknownObject, UG_ERR_ERROR,
			2, ERget(frame ? F_AR0005_frame : F_AR0006_procedure),
			name
		);
		return NULL;
	}
	else
	{ /* got it */
/* # line 234 "newedit.qsc" */	/* scroll */
    {
      if (IItbsetio(1,cat->c_form,cat->c_tfield,-3) != 0) {
        IItbsmode((char *)"to");
        if (IItscroll(0,row_no) != 0) {
        } /* IItscroll */
      } /* IItbsetio */
    }
/* # line 235 "newedit.qsc" */	/* host code */
		return ( _send(catCheck, cat)( cat, &row ) != OK )
				? NULL : iiabGetComp( appl, row.id );
	}
	/*NOT REACHED*/
  }
