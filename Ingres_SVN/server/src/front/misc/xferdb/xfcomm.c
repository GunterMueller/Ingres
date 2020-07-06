# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<er.h>
# include	<fe.h>
# include	<st.h>
# include       <si.h>
# include       <lo.h>
# include	<ug.h>
# include        <adf.h>
# include        <afe.h>
# include <xf.h>
# include	"erxf.h"
/*
** Fool MING, which doesn't know about EXEC SQL INCLUDE
# include <xf.qsh>
*/
/**
** Name:	xfcomm.sc - write statements to create dbms comments.
**
** Description:
**	This file defines:
**
**	xfcomments		write statements to create comment.
**
** History:
**	24-aug-90 (billc) 
**		written.
**      27-jul-1992 (billc)
**              Rename from .qsc to .sc suffix.
**      24-nov-1999 (ashco01)
**              Reworked xfcomments() to accept pointer to XF_TABINFO
**              (which describes specific table) for which the 'COMMENT ON'
**              statement is to be generated.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*{
** Name:	xfcomments - write COMMENT ON statements.
**
** Description:
**	Probably overimplemented, since currently we don't support short 
**	comments.  Long comments are limited to 1600 characters, so 
**	there is only one comment per tuple.  This is a nice simplification.
**
** Inputs:
**      t       pointer to XF_TABINFO describing the table for which
**              to generate the 'COMMENT ON' statement.	
**
** Outputs:
**	writes to the reload scripts.
**
**	Returns:
**		none.
**
** History:
**	24-aug-90 (billc) written.
**      24-nov-1999 (ashco01)
**          Reworked to accept pointer to XF_TABINFO and generate 'COMMENT ON'
**          statment for specific table only. The 'COMMENT ON' statement is now
**          written as part of the TABLES section rather than in a COMMENTS 
**          section of the COPY.IN.
**       1-Apr-2003 (hanal04) Bug 109584 INGSRV 2102
**          No need for TH_BLANK_PAD padding hack now we have rewritten
**          F_CU format in f_colfmt().
**      31-May-2006 (smeke01) Bug 116153
**          Ensured that xfcomments looks for [table- & table-column-] 
**          comments on strictly 1 table by restricting the search 
**          qualification to a named table and a named table-owner.
**          Previously table-owner was optional, since the global
**          variable Owner could be empty, so > 1 table of the same
**          name could be processed by xfcomments, leading to bug
**          116153.
*/
void
xfcomments(t)
XF_TABINFO  *t;
{
  char obj_name[FE_MAXNAME + 1];
  char sub_obj_name[FE_MAXNAME + 1];
  char obj_owner[FE_MAXNAME + 1];
  char short_rem[XF_SHORTREMLINE + 1];
  char long_rem[XF_LONGREMLINE + 1];
  char obj_type[2];
  char *tabinfo_name;
  char *tabinfo_owner;
    i4	   com_count = 0;
    char    esc_buf[(XF_LONGREMLINE * 2) + 1];
    TXT_HANDLE    *tfd = NULL;
    /* Determine current table name */
    tabinfo_name = t->name;
    tabinfo_owner = t->owner;
/* # line 110 "xfcomm.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"xfcomm1",7157,19087);
      IIputdomio((short *)0,1,32,0,tabinfo_owner);
      IIputdomio((short *)0,1,32,0,tabinfo_name);
      IIputdomio((short *)0,1,32,0,tabinfo_owner);
      IIputdomio((short *)0,1,32,0,tabinfo_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"xfcomm1",7157,19087);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select c.object_owner, c.object_name, '', 'T', c.short_remark, c.long\
_remark from iidb_comments c where c.object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tabinfo_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and c.object_type='T' and c.object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,tabinfo_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
"union select sc.object_owner, sc.object_name, sc.subobject_name, 'C',\
 sc.short_remark, sc.long_remark from iidb_subcomments sc where sc.obj\
ect_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,tabinfo_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and sc.subobject_type='C' and sc.object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,tabinfo_name);
        IIexDefine(0,(char *)"xfcomm1",7157,19087);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,obj_owner);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,obj_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,sub_obj_name);
      IIgetdomio((short *)0,1,32,1,obj_type);
      IIgetdomio((short *)0,1,32,XF_SHORTREMLINE + 1-1,short_rem);
      IIgetdomio((short *)0,1,32,XF_LONGREMLINE + 1-1,long_rem);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 125 "xfcomm.sc" */	/* host code */
    {
	xfread_id(obj_name);
	xfread_id(obj_owner);
	if (obj_type[0] == 'C')
	    xfread_id(sub_obj_name);
	if (tfd == NULL)
	{
	    /* Open our text-buffer */
	    tfd = xfreopen(Xf_in, TH_IS_BUFFERED);
	    /* Make sure we're in SQL */
	    xfsetlang(tfd, DB_SQL);
	}
	/* Does user id have to be reset? */
	xfsetauth(tfd, obj_owner);
	xfwrite(tfd, ERx("comment on "));
	if (obj_type[0] == 'T')
	{
	    xfwrite(tfd, ERx("table "));
	    xfwrite_id(tfd, obj_name);
	}
	else
	{
	    xfwrite(tfd, ERx("column "));
	    xfwrite_id(tfd, obj_name);
	    xfwrite(tfd, ERx("."));
	    xfwrite_id(tfd, sub_obj_name);
	}
	xfwrite(tfd, ERx(" is '"));
	xfescape_quotes(long_rem, esc_buf);
        xfwrite(tfd, esc_buf);
	xfwrite(tfd, ERx("'"));
	/* 
	** Short remarks are not yet supported in 6.5, so this stuff may not
	** work.  Short-remarks in the catalogs are still poorly specified.
	*/
	if (STtrmwhite(short_rem) > 0)
	{
	    xfwrite(tfd, ERx("\n\twith short_remark='"));
	    xfescape_quotes(short_rem, esc_buf);
	    xfwrite(tfd, esc_buf);
	    xfwrite(tfd, ERx("'"));
	}
	xfflush(tfd);
	com_count++;
    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
    if (tfd != NULL)
	xfclose(tfd);
}
