# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
# include <oocat.h>
/**
** Name:	oolngrem.qc -	Catalog Form Long Remark Utilities.
**
** Description:
**	Contains routines to get and put the long remarks onto a form.  The
**	name of the field should be "long_remark".  Defines:
**
**	OOput_long_remark()
**	OOget_long_remark()
**
** History:
**	Revision 6.2  89/01  wong
**	Abstracted from "obcatfor.qsc".  Also, fixed JupBug #???.
**/
/*{
** Name:	OOput_long_remark() - put long remark onto form
**
** Description:
**
** Input params:
**	char	*form;		form name
**	char	buffer[];	at least OOLONGREMSIZE+1 chars
**
** Side effect:
**	form display is updated;
**
** History:
**	01/89 (jhw) -- Must clear the field if no long remark exists
**		otherwise, a previous value may show up.
*/
  OOput_long_remark(form, buffer)
  char *form, *buffer;
  {
	if ( buffer != NULL )
/* # line 48 "oolngrem.qsc" */	/* putform */
    {
      if (IIfsetio(form) != 0) {
        IIputfldio(_long_remark,(short *)0,1,32,0,buffer);
      } /* IIfsetio */
    }
/* # line 49 "oolngrem.qsc" */	/* host code */
	else
/* # line 50 "oolngrem.qsc" */	/* clear */
    {
      IIfldclear(_long_remark);
    }
  }
/* # line 53 "oolngrem.qsc" */	/* host code */
/*{
** Name:	OOget_long_remark() - get long remark from form
**
** Description:
**
** Input params:
**	char	*form;		form name
**	char	buffer[];	at least OOLONGREMSIZE+1 chars
**
** Side effect:
**	buffer is filled in;
**
** History:
*/
  OOget_long_remark(form, buffer)
  char *form, buffer[OOLONGREMSIZE+1];
  {
/* # line 71 "oolngrem.qsc" */	/* getform */
    {
      if (IIfsetio(form) != 0) {
        IIgetfldio((short *)0,1,32,OOLONGREMSIZE+1-1,buffer,_long_remark);
      } /* IIfsetio */
    }
  }
