# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>
#include	<lo.h>
#include	<ds.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ft.h>
#include	<fmt.h>
#include	<adf.h>
#include	<frame.h>
#include	<ug.h>
#include	"fdfuncs.h"
#include	"erfd.h"
# include	<cv.h>
# include	<feds.h>
#include	<frmindex.h>
/**
** Name:	fdfrcrfl.c -	Get a Frame from a Form Index File.
**
** Description:
**	Contains a routine used to read a form from a Form Index file and
**	then initialize its FRS structure.  Called from 'IIUFgtfGetForm()'.
**	Defines:
**
**	IIFDfrcrfle()	get and initialize form from a form index file.
**
** History:
**	Revision 6.1  88/04/01  wong
**	Abstracted read into 'IIFDfiRead()' (in "frame/frmiread.c") along with
**	changes for Form Index file as a class.
**
**	Revision 6.0  87/01/20  Kobayashi
**	Initial version.
**	10/02/87 (dkh) - Changed FDbincrfle to IIFDrffReadFormFIle for check7.
**	12/01/87 (dkh) - Fixed to handle slots with no forms in them.
**	01/04/88 (dkh) - Changed "regname" to MAX_LOC + 1 from 50.
**	01/21/88 (dkh) - Fixed SUN compilation problems.
**	05/27/88 (dkh) - Changed to not increase size of form to width of
**			 terminal since this does not work for popups.
**	10/22/88 (dkh) - Added check to "commit" if not in a transaction.
**	02-nov-88 (bruceb)
**		Change type of arg in call on DSload.
**	09/06/90 (dkh) - Changed to use IIFDgfdGetFdDstab() instead of
**			 IIFDdstab directly.
**	04/16/91 (dkh) - Changed call to IIFDfiOpen() in order to support
**			 adapting to different formindex file header sizes.
**	10/14/92 (dkh) - Removed use of ArrayOf in favor of DSTARRAY.
**			 ArrayOf confused the alpha C compiler.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
FUNC_EXTERN	bool	FDbinfldcre();
FUNC_EXTERN	bool	IIFDggvGetGetlistVar();
/* Routine to template table. */
FUNC_EXTERN	DSTARRAY	*IIFDgfdGetFdDstab();
/*{
** Name:	IIFDfrcrfle() -	Get and Initialize Form From a Form Index File.
**
** Description:
**	Initializes the FRS structure for a form fetched from a Form Index file.
**	The Form Index file is kept open across invocations of this routine
**	unless a different Form Index file is to be searched.  (Then the old
**	one is closed and the new one is opened.)  The form is read from this
**	file (using 'IIFDfiRead()', which returns a reference to it) and then
**	relocated using the DS module.  Finally, it is initialized and a
**	reference to the FRS runtime structure is returned.
**
** Inputs:
**	frmid	{char *}  The name of the form to be initialized.
**	fileloc	{LOCATION *}  The location of the Form Index file.
**
** Returns:
**	{FRAME *}  The initialized FRS form structure for the fetched form.
**			   Otherwise, NULL on error.
**
** Errors (FATAL):
**	E_FD0004_FormRead		Error reading form from file.
**	E_FD0005_FormLoadErr	Error relocating form.
**
** Side Effects:
**	None.
**
** History:
**	20-Jan-1987 (Kobayashi) Create new for 5.0K.
**	08/14/87 (dkh) - ER changes.
**	04/88 (jhw) -- Moved functionality of old 'IIFDrffReadFormFile()'
**		directly into here while abstracting read into 'IIFDfiRead()'
**		along with changes for Form Index file as a class.
**	01/12/89 (wolf)- Add FT3270 ifdef
*/
static FORM_INDEX	*frmindex = NULL;	/* Form Index file */
FRAME *
IIFDfrcrfle ( frmid, fileloc )
char		*frmid;		/* name of form for which to look */
LOCATION	*fileloc;	/* location of form index file */
{
	FRAME	*frm;		/* frame reference */
	i4	frmsize;	/* frame size */
	SH_DESC	sh_desc;	/* shared data region descriptor */
	char	*locst;
	i4	retval;
  i4 was_inxaction;
	PTR	frmptr;
	/*
	** Check filename.  If filename is different
	** than current file name, open the file.
	*/
	LOtos( fileloc, &locst );
	if ( frmindex == NULL || !STequal(locst, frmindex->fname) )
	{ /* new Form Index file */
		if ( frmindex != NULL )
		{ /* Form Index already open, close it. */
			IIFDfiClose( frmindex );
		}
		if ( ( frmindex = IIFDfiOpen( fileloc,
		    FALSE, TRUE ) ) == NULL )
		{
			return NULL;
		}
	}
	if ( (frm = (FRAME *)IIFDfiRead( frmindex, frmid, &frmsize )) == NULL )
		return NULL;
	else if ( frmsize <= 0 )
	{
		IIUGerr( E_FD0004_FormRead, UG_ERR_FATAL, 1, (PTR)frmid );
		/*NOTREACHED*/
	}
	/*
	** Relocate pointers within FRAME structure
	*/
	sh_desc.sh_type = IS_MEM;
	sh_desc.sh_keep = NULL;
	sh_desc.size = frmsize;
	frmptr = (PTR)frm;
	/* DSload expects a pointer to a PTR--and needs it for Data General */
	if ( DSload( &sh_desc, &frmptr, IIFDgfdGetFdDstab()) != OK )
	{
		IIUGerr( E_FD0005_FormLoadErr, UG_ERR_FATAL, 1, (PTR)frmid );
		/*NOTREACHED*/
	}
	/* reassign address since DSload can change the value */
	frm = (FRAME *)frmptr;
	/*
	**  Code no longer increases the size of the form to the
	**  size of the terminal since this is not right for popups.
	*/
	/* set frame driver mode */
	frm->frmode = fdcmINSRT;
/* # line 162 "fdfrcrfl.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&was_inxaction,73);
  }
/* # line 163 "fdfrcrfl.sc" */	/* host code */
	/* create field structures for frame */
	retval = FDbinfldcre(frm, WIN);
	if (IIFDggvGetGetlistVar() && !was_inxaction)
	{
/* # line 167 "fdfrcrfl.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 168 "fdfrcrfl.sc" */	/* host code */
	}
	if ( !retval)
		return NULL;
	/*
	**  Call 'FTnewfrm()' to allocate FT specific structures
	**	if we really need it.
	*/
# ifdef FT3270
	/* Avoid potential exceptions freeing memory in FTnewibm() */
	frm->frscr = NULL;
# endif
	FTnewfrm( frm );
	STcopy( frmid, frm->frname );	/* set name */
	CVlower( frm->frname );
	return frm;
}
