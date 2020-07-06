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
#include	<ug.h>
#include	<ui.h>
#include	<ooclass.h>
# include <oocat.h>
#include	<oodefine.h>
#include	<oosymbol.h>
#include	"eroo.h"
/**
** Name:	oolremfr.qsc -	Object Long Remark Edit Frame.
**
** Description:
**	Contains the routine used to display and edit a long remark for an
**	object.  Defines:
**
**	IIOOedLongRemark()	edit/view object long remark frame.
**	IIOOckLongRemark()	get long remark from edit/view form.
**
** History:
**	Revision 6.4  90/04  wong
**	Initial revision.
**      06-dec-93 (smc)
**	    Bug #58882
**          Commented lint pointer truncation warning.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
char	*iiooStrAlloc();
/*{
** Name:	IIOOedLongRemark() -	Edit/View Object Long Remark Frame.
**
** Description:
**	Displays Long Remark Edit/View Frame for an object.  The long remark
**	for the object will be fetched from the DB if has not already.  A
**	long remark will be non-NULL, but may be empty.
**
** Input params:
**	self	{OO_OBJECT *}  The object.
**	edit	{bool}  Whether editing or viewing.
**
** Side Effects:
**	Long remark for an object may be retrieved and set.
**
** History:
**	04/90 (jhw) Written.
*/
static OOID	_curID = 0;
static READONLY
  char _Form[]= ERx("iilongremarks");
#define _HelpExpl	F_FE0100_ExplFrameHelp
#define _CancelExpl	F_FE0102_ExplEnd
VOID
IIOOedLongRemark ( self, edit )
register OO_OBJECT	*self;
bool			edit;
  {
    char *_mode;
    i4 change;
	if ( _curID == 0 )
	{
		char	*forms[2];
		forms[0] = _Form;
		forms[1] = NULL;
		if ( IIUFaddForms(forms) != OK )
			return;
	}
        /* lint truncation warning if size of ptr > OOID, but code valid */
	if ( edit && (bool)OOsndSelf(self, _authorized) )
	{ /* can update */
		_mode = ERx("update");
	}
	else
	{ /* no update, read only */
		_mode = ERx("read");
	}
/* # line 101 "oolremfr.qsc" */	/* display */
    {
      if (IIdispfrm(_Form,_mode) == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 104 "oolremfr.qsc" */	/* host code */
		/* fetch long remarks and display them */
		if ( self->long_remark == NULL )
		{
			if ( self->ooid <= OC_UNDEFINED )
/* # line 108 "oolremfr.qsc" */	/* clear */
        {
          IIfldclear(_long_remark);
        }
/* # line 109 "oolremfr.qsc" */	/* host code */
			else
			{
				char	long_rem[OOLONGREMSIZE+1];
				if ( OOrd_seqText( self->ooid, long_rem,
						_ii_longremarks, _long_remark
					) != nil )
				{
					_VOID_ STtrmwhite(long_rem);
					self->long_remark =
						iiooStrAlloc(self, long_rem);
				}
				OOput_long_remark(_Form, self->long_remark);
			}
		}
		else if ( _curID != self->ooid )
		{
			OOput_long_remark(_Form, self->long_remark);
		}
		_curID = self->ooid;
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 133 "oolremfr.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&change,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 134 "oolremfr.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 141 "oolremfr.qsc" */	/* host code */
		/* restore */
		change = 0;
		OOput_long_remark(_Form, self->long_remark);
/* # line 144 "oolremfr.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 150 "oolremfr.qsc" */	/* host code */
		FEhelp( ERx("oolremfr.hlp"), ERget(S_OO0050_LRemHelpBanner));
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
      if (IInmuact(ERget(FE_Cancel),ERget(_CancelExpl),0,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,3) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 153 "oolremfr.qsc" */	/* host code */
	if ( change != 0 )
/* # line 154 "oolremfr.qsc" */	/* set_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(20,(char *)0,0,(short *)0,1,30,4,&change);
      } /* IIiqset */
    }
  }
/* # line 158 "oolremfr.qsc" */	/* host code */
/*
** Name:	IIOOckLongRemark() -	Get Long Remark from Edit/View Form.
**
** Description:
**	Checks whether the long remark was edited on the edit/view frame
**	and sets the long remark to the new value.
**
** Input:
**	self	{OO_OBJECT *}  The object.
**
** Returns:
**	{bool}  TRUE if the long remark was changed.
**
** History:
**	04/90 (jhw) Written.
*/
bool
IIOOckLongRemark ( self )
register OO_OBJECT	*self;
  {
    i4 change;
	char	long_rem[OOLONGREMSIZE+1];
	if ( self->long_remark == NULL || _curID != self->ooid )
		return FALSE;
	OOget_long_remark(_Form, long_rem);
	if ( STequal(self->long_remark, long_rem) )
		return FALSE;
	else
	{
		self->long_remark = iiooStrAlloc(self, long_rem);
		self->data.dirty = TRUE;
		return TRUE;
	}
	/*NOT REACHED*/
  }
