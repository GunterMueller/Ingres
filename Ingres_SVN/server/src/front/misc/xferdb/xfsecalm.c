# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<st.h>
# include	<er.h>
# include       <si.h>
# include       <lo.h>
# include	<fe.h>
# include	<ug.h>
# include       <adf.h>
# include       <afe.h>
# include <xf.h>
# include	"erxf.h"
/*
** Fool MING, which doesn't know about EXEC SQL INCLUDE
# include <xf.qsh>
*/
/**
** Name:	xfsecalm.sc - write statements to create security_alarm.
**
** Description:
**	This file defines:
**
**	xfsecalarms	write statement to create security alarms 
**
** History:
**	17-nov-93 (robf)
**	   Created from xfpermit.sc
*/
/* # define's */
/* GLOBALDEF's */
/* extern's */
GLOBALREF bool	With_distrib;
GLOBALREF bool  With_security_alarms;
/* static's */
static void writealarm( TXT_HANDLE	**tfdp, 
	char	*object_name,
	i2	alarm_number,
	i4	text_sequence,
	char	*alarm_grantor,
	char	*text_segment);
/*{
** Name:	xfsecalarms - write statement to create security_alarm.
**
** Description:
**
** Inputs:
**
** Outputs:
**
**	Returns:
**
** History:
**	17-nov-93 (robf)
**         Created for Secure 2.0.
*/
void
xfsecalarms()
{
  char text_segment[XF_PERMITLINE + 1];
  char alarm_grantor[FE_MAXNAME + 1];
  char object_name[FE_MAXNAME + 1];
  char object_owner[FE_MAXNAME + 1];
  i2 alarm_number;
  i2 alarm_depth;
  i4 text_sequence;
    TXT_HANDLE		*tfd = NULL;
    if (!With_security_alarms)
	return;
/* # line 86 "xfsecalm.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select a.object_name, a.object_owner, a.security_number, a.text_seque\
nce, a.text_segment from iisecurity_alarms a where(a.object_owner=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"or ''=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)
")and a.object_type='T' order by a.object_owner, a.object_name, a.secu\
rity_number, a.text_sequence");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,object_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,object_owner);
      IIgetdomio((short *)0,1,30,2,&alarm_number);
      IIgetdomio((short *)0,1,30,4,&text_sequence);
      IIgetdomio((short *)0,1,32,XF_PERMITLINE + 1-1,text_segment);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 96 "xfsecalm.sc" */	/* host code */
    {
	    writealarm(&tfd, object_name, 
			alarm_number, text_sequence, 
			object_owner, text_segment);
    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
    if (tfd != NULL)
	xfclose(tfd);
}
/*{
** Name:	writealarm - write out security alarm information. 
**
** Description:
**
** Inputs:
**    tfdp		pointer to TXT_HANDLE pointer, we open this on the
**				first call.
**    object_name	name of the object.
**    alarm_number	the alarm number
**    text_sequence	sequence number of the text segment.
**    alarm_grantor	name of person granting the permission.
**    text_segment	the text of the permission.
**
** Outputs:
**    tfdp		pointer to TXT_HANDLE pointer, we open this on the
**				first call.
*/
static void
writealarm(
    TXT_HANDLE	**tfdp, 
    char	*object_name,
    i2		alarm_number,
    i4		text_sequence,
    char	*alarm_grantor,
    char	*text_segment
    )
{
    /* if new alarm statement */
    xfread_id(object_name);
    if (!xfselected(object_name))
	return;
    if (text_sequence == 1)
    {
	if (*tfdp == NULL)
	{
	    xfwritehdr(ALARMS_COMMENT);
	    *tfdp = xfreopen(Xf_in, TH_IS_BUFFERED);
	}
	else
	{
	    xfflush(*tfdp);
	}
	/* Change authorization id if permit issuer is someone new. */
	xfread_id(alarm_grantor);
	xfsetauth(*tfdp, alarm_grantor);
        xfsetlang(*tfdp, DB_SQL);
    }
    xfwrite(*tfdp, text_segment);
}
