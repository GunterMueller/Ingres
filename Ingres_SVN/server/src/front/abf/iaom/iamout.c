# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1986, 2004 Ingres Corporation
*/
#include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
#include	<er.h>
#include	<nm.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
# include <ooclass.h>
#include	"iamstd.h"
/*
** debugging requires UNIX/MSDOS environment.  oo_encode opens this file
*/
/**
** Name:	iamout.sc -	DB Output Module.
**
** Description:
**	Contains routines used to write an interpreted frame object as an
**	encoded object to the "ii_encodings" table with suitable buffering.
**	The way this works:
**
**	Caller calls 'db_initbuf()' to set up buffer, available length,
**	ID and object fields for the write.  'db_initbuf()' initializes
**	local status, and opens a transaction.	The buffer address passed
**	in should contain enough storage for the passed in length plus an
**	EOS.  Must be <= ESTRINGSIZE to work correctly.
**
**	'db_initbuf()' also removes any old object from the database.  This is
**	done inside the transaction so that a failed write won't affect an
**	old object.
**
**	Caller then writes chunks of the encoding string by calling
**	'db_printf()' or 'db_puts()'.  These routines transfer bytes to the
**	buffer, writing tuples to the "ii_encodings" table when the buffer
**	fills up.  (The former performs a formatted write.)  The records are
**	written with appropriate ascending sequence numbers.
**
**	Caller calls 'db_endbuf()'.  This writes the remaining string in
**	the buffer to the "ii_encodings" table, and closes the transaction.
**
**	'db_endbuf()', 'db_printf()' and 'db_puts()' return status.  On the
**	first failure, the transaction will have been aborted.  Caller should
**	stop trying to write this particular object.
**
**	We do this by NOT deleting old ii_encoding records, but updating
**	existing rows until we fail, then doing inserts, finally removing
**	any remaining old records at the end.  The reason for doing it
**	this way is to avoid wasting pages.  Because this table has large
**	tuples, the savings are apparently significant.
**
** History:
**	Revision 6.0  86/08  bobm
**	Initial revision.
**	06/30/88 (dkh) - Fixed linking problems on VMS by removing
**			 include of oodefine.h and defining ESTRINGSIZE.
**	18-aug-88 (kenl)
**		Changed sql begin transaction to COMMIT WORK.
**	12-sep-88 (kenl)
**		Changed EXEC SQL COMMITS to appropriate IIUI...Xaction calls.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**
**	Revision 6.5
**	19-jun-92 (davel)
**		Added comments to the effect that db_printf can only handle
**		i4s, based on analogous changes made in W4GL.
**	08-mar-93 (davel)
**		Add temporary workaround for server bug 49944 - do not try
**		to update ii_encodings.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	13-May-2005 (kodse01)
**	    replace %ld with %d, %lo with %o  etc. for old nat and 
**	    long nat values.
*/
/*
**  The following define for ESTRINGSIZE is borrowed from oodefine.h.
**  The include of oodefine.h was removed since it contains references
**  to table field routines that VMS can not find when linking OSL
**  and OSLSQL.  This is a temporary solution.
*/
# define	ESTRINGSIZE	1990
extern bool	Xact;
  static OOID Id = -1, Object = -1;
  static i4 Seq = 0;
  static bool NeedDel;
  static char *Bufr = NULL;
static char	*Current = NULL;	/* current storage pointer in buffer */
static i4	Bufr_len = 0;		/* length of Bufr (-1 for terminator) */
static i4	Avail = 0;		/* characters available at Current */
static STATUS blat_row();
VOID
db_initbuf ( addr, buf_len, id_field, obj_field )
char	*addr;
i4	buf_len;
OOID	id_field;
OOID	obj_field;
{
	char *opt;
	Seq = 0;
	Id = id_field;
	Object = obj_field;
	Avail = Bufr_len = min(buf_len, ESTRINGSIZE);
	Current = Bufr = addr;
	NeedDel = TRUE;
#ifdef AOMDTRACE
	aomprintf(ERx("\nOUTPUT: init X%lx %d %d %d\n>"),(i4) Bufr, Bufr_len,
					Id, Object);
#endif
	/* temporary workaround for bug 49944 */
	NMgtAt(ERx("II_4GL_WORKAROUND_B49944"), &opt);
	if (opt != NULL && *opt != EOS)
	{
		iiuicw1_CatWriteOn();
/* # line 138 "iamout.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamout1",6606,16571);
      IIputdomio((short *)0,1,30,4,&Object);
      IIputdomio((short *)0,1,30,4,&Seq);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamout1",6606,16571);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_encodings where encode_object=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&Object);
        IIwritio(0,(short *)0,1,32,0,(char *)"and encode_sequence>=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&Seq);
        IIexDefine(0,(char *)"iamout1",6606,16571);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 141 "iamout.sc" */	/* host code */
		iiuicw0_CatWriteOff();
		NeedDel = FALSE;
	}
}
STATUS
db_puts ( str )
char *str;
{
	i4 len;
#ifdef AOMDTRACE
	static i4  outcount = 0;
#endif
	/* take care of NULL string */
	if ( str == NULL || *str == EOS )
		return OK;
	/*
	** if string fits in remaining buffer, copy and return.
	** Zero length string winds up doing nothing.
	*/
	if ( (len = STlength(str)) <= Avail )
	{
#ifdef AOMDTRACE
		if ( (outcount += len) > 72 )
		{
			aomprintf(ERx("\n>%s"),str);
			outcount = 0;
		}
		else
			aomprintf(ERx("%s"),str);
#endif
		STcopy( str, Current );
		Avail -= len;
		Current += len;
		return OK;
	}
	/*
	** copy first part of string to remaining buffer, and dump
	*/
	STlcopy( str, Current, (i2)Avail );
	Current[Avail] = EOS;		/* make SURE */
#ifdef AOMDTRACE
	aomprintf( "\nOUTPUT: split string\n>%s\nOUTPUT: write full buffer\n>",
			Current
	);
	outcount = 0;
#endif
	len = Avail;
	if (blat_row() != OK)
		return FAIL;
	/* output the rest of the string */
	return db_puts(str + len);
}
/*
** db_printf will print up to 6 i4s, using the format template
** (in the form required by SIfprintf) specified as the first argument.
** Note that the only conversion specifications allowed in this template are
** %d, %o, %u, and %x (but you can put modifiers between the '%' and 
** conversion specifier 'd', 'o', 'u', or 'x').
** The total number of bytes printed must not exceed 79.
*/
STATUS
db_printf ( fs, a, b, c, d, e, f )
char	*fs;
i4	a,b,c,d,e,f;
{
	char bufr[80];
	return db_puts( STprintf(bufr,fs,a,b,c,d,e,f) );
}
STATUS
db_endbuf ()
{
#ifdef AOMDTRACE
	aomprintf(ERx("\nOUTPUT: write remaining %d bytes\n>"),
							Bufr_len - Avail);
#endif
	if ( Avail < Bufr_len )
	{
		if ( blat_row() != OK )
			return FAIL;
	}
	if (NeedDel)
	{
		iiuicw1_CatWriteOn();
/* # line 236 "iamout.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamout2",6607,17148);
      IIputdomio((short *)0,1,30,4,&Object);
      IIputdomio((short *)0,1,30,4,&Seq);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamout2",6607,17148);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_encodings where encode_object=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&Object);
        IIwritio(0,(short *)0,1,32,0,(char *)"and encode_sequence>=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&Seq);
        IIexDefine(0,(char *)"iamout2",6607,17148);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 239 "iamout.sc" */	/* host code */
		iiuicw0_CatWriteOff();
	}
	if ( Xact )
		IIUIendXaction();
	return OK;
}
/*
** flush the output buffer to ii_encodings, start new buffer
*/
static STATUS
blat_row ()
{
	iiuicw1_CatWriteOn();
	if (NeedDel)
	{
/* # line 258 "iamout.sc" */	/* repeat update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamout3",6608,16635);
      IIputdomio((short *)0,1,30,4,&Id);
      IIputdomio((short *)0,1,32,0,Bufr);
      IIputdomio((short *)0,1,30,4,&Seq);
      IIputdomio((short *)0,1,30,4,&Object);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamout3",6608,16635);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_encodings set object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&Id);
        IIwritio(0,(short *)0,1,32,0,(char *)", encode_estring=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,Bufr);
        IIwritio(0,(short *)0,1,32,0,(char *)"where encode_sequence=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&Seq);
        IIwritio(0,(short *)0,1,32,0,(char *)"and encode_object=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&Object);
        IIexDefine(0,(char *)"iamout3",6608,16635);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 263 "iamout.sc" */	/* host code */
		if (FEinqerr() != OK)
		{
			abort_it();
			return FAIL;
		}
		/* if we didn't update, start doing inserts */
		if (FEinqrows() == 0)
			NeedDel = FALSE;
	}
	if (! NeedDel)
	{
/* # line 276 "iamout.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamout4",6609,18088);
      IIputdomio((short *)0,1,30,4,&Id);
      IIputdomio((short *)0,1,30,4,&Object);
      IIputdomio((short *)0,1,30,4,&Seq);
      IIputdomio((short *)0,1,32,0,Bufr);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamout4",6609,18088);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_encodings(object_id, encode_object, encode_sequence, e\
ncode_estring)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&Id);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&Object);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&Seq);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,Bufr);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamout4",6609,18088);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 281 "iamout.sc" */	/* host code */
		if ( FEinqerr() != OK )
		{
			abort_it();
			return FAIL;
		}
	}
	iiuicw0_CatWriteOff();
	Avail = Bufr_len;
	Current = Bufr;
	Bufr[0] = EOS;
	++Seq;
	return OK;
}
