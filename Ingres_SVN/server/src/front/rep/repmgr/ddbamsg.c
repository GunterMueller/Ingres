# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <stdarg.h>
# include <compat.h>
# include <st.h>
# include <er.h>
/**
** Name:	ddbamsg.sc - RepMgr (was DDBA) message routines
**
** Description:
**	Defines
**		ddba_messageit		- output a RepMgr message
**
** History:
**	16-dec-96 (joea)
**		Created based on ddbamsg.sc in replicator library.
**	19-nov-99 (abbjo03)
**		Remove unused functions ddba_messageit_text and _no_popup.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# define DISPLAY_MSG_LEVEL	3
/*{
** Name:	ddba_messageit - output a RepMgr message
**
** Description:
**	Outputs a message using FRS MESSAGE WITH STYLE = POPUP.  According to
**	the print level, a message will be output or ignored.
**
** Inputs:
**	message_level	- an indicator of the print level of the message
**	message_number	- the identifier of the message
**	params		- additional optional parameters to be displayed, using
**			  printf conventions
**
** Outputs:
**	none
**
** Returns:
**	none
*/
void
ddba_messageit(
i4	message_level,
i4	message_number, ...)
{
	va_list	ap;
  char dest_string[1000];
	char	message_text[1000];
	char	msg_prefix[20];
	char	timestamp[26];
	va_start(ap, message_number);
	if (message_level > DISPLAY_MSG_LEVEL)
	{
		va_end(ap);
		return;
	}
	build_messageit_string(msg_prefix, timestamp, message_text,
		message_number, message_level, ap);
	va_end(ap);
	STprintf(dest_string, ERx("%-7s-- %s -- %d -- %s"), msg_prefix,
		timestamp, message_number, message_text);
/* # line 78 "ddbamsg.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(dest_string);
  }
/* # line 79 "ddbamsg.sc" */	/* host code */
}
