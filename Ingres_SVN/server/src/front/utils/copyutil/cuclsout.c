# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <ooclass.h>
# include	<oocat.h>
# include	<cu.h>
# include	<er.h>
# include	"ercu.h"
# include	<si.h>
/**
** Name:	cuclsout.c -    Copy a class out.
**
** Description:
**	Copies a user-defined class definition to a copyutil file.
**
**
** History:
**	1/80 (billc) First Written
**	01-May-97 (sarjo01)
**	    Bug 81866: If attr def's did not have a long remark, the
**	    attr would be skipped. Added UNION SELECT to account for this.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	13-May-2005 (kodse01)
**	    replace %ld with %d for old nat and long nat variables.
**/
/*{
** Name:	IICUclwCLassWrite	-  Copy a class's attributes to a file.
**
** Description:
**	Given a class id this copies the class's attribute 
**	definitions to a copyutil file.  (The routines name is arguably a
** 	misnomer.  The layout for the attributes
**	of the class tables is given in the CURECORD
**	for the attribute objects.
**
** Inputs:
**	ooid		The ooid of the class to copy out.
**	fp		The file the output to.
**
** Outputs:
**	Returns:
**		OK if succeeded.
**		otherwise a failure status.
**
** History:
**	1/80 (billc) First Written
**	01-May-97 (sarjo01)
**	    Bug 81866: If attr def's did not have a long remark, the
**	    attr would be skipped. Added UNION SELECT to account for this.
*/
STATUS
IICUclwCLassWrite(ooid, fp)
  OOID ooid;
FILE	*fp;
{
  char a_name[FE_MAXNAME + 1];
  i2 a_order;
  char a_typename[(FE_MAXNAME * 3) + 1];
  i4 a_type;
  i4 a_length;
  i4 a_scale;
  char a_shortrem[OOSHORTREMSIZE + 1];
  char a_longrem[OOLONGREMSIZE + 1];
/* # line 78 "cuclsout.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select o.object_name, c.class_order, c.type_name, c.adf_type, c.adf_l\
ength, c.adf_scale, o.short_remark, l.long_remark from ii_objects o, i\
i_abfclasses c, ii_longremarks l where c.class_id=");
    IIputdomio((short *)0,1,30,4,&ooid);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and l.object_id=c.catt_id and o.object_id=c.catt_id union select o.ob\
ject_name, c1.class_order, c1.type_name, c1.adf_type, c1.adf_length, c\
1.adf_scale, o.short_remark, ' ' from ii_objects o, ii_abfclasses c1 w\
here c1.class_id=");
    IIputdomio((short *)0,1,30,4,&ooid);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and o.object_id=c1.catt_id and not exists(select l.object_id from ii_\
longremarks l where l.object_id=c1.catt_id)order by 2 asc");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,a_name);
      IIgetdomio((short *)0,1,30,2,&a_order);
      IIgetdomio((short *)0,1,32,(FE_MAXNAME * 3) + 1-1,a_typename);
      IIgetdomio((short *)0,1,30,4,&a_type);
      IIgetdomio((short *)0,1,30,4,&a_length);
      IIgetdomio((short *)0,1,30,4,&a_scale);
      IIgetdomio((short *)0,1,32,OOSHORTREMSIZE + 1-1,a_shortrem);
      IIgetdomio((short *)0,1,32,OOLONGREMSIZE + 1-1,a_longrem);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 105 "cuclsout.sc" */	/* host code */
    	{
		/* Write out the header. */
		_VOID_ cu_wrobj(OC_RECMEM, 
				2, /* OC_RECORD level + 1 */
				a_name, a_shortrem, a_longrem, fp
			);
		/* Write out the detail line. */
	    	SIfprintf(fp, ERx("\t%d\t%d\t%s\t%d\t%d\n"),
			(i4) a_order,
			a_type,
			a_typename,
			a_length,
			a_scale
		    );
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	return FEinqerr();
}
