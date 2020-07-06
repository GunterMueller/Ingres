# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	QDESCFRM.QSC  -	This module is called to correlate the
**			qdef's columns with the forms fields.
**			It is only used in the case where a
**			qbfname is the qdefname and the VIFRED
**			defined form must then be matched with
**			the attributes of the qdef.
**
**	Routines:
**	     mqfrmqdesc()
**	     mqjpartchk()- check placement of M/D joins on VIFRED form
**
**	Written: 2/21/84 (nml)
**
**	History:
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh
**		10/13/90 (emerson)
**			Exempt logical key attributes from edits
**			involving forms (bug 8593).
**
**	Copyright (c) 2004 Ingres Corporation
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include "mqbf.h"
# include	<er.h>
# include	"erqf.h"
mqfrmqdesc(qdef)
MQQDEF	*qdef;	/* current qdef */
  {
    char *fname;
    char fldnm[FE_MAXNAME+1];
    char cname[FE_MAXNAME+1];
    i4 istable;
    i2 ftype;
    i2 flen;
    i4 fnum;
    char fmode[10];
	ATTRIBINFO	**atts;		/* ptr to set of attributes */
	i4		numatts;
	ATTRIBINFO	*catt;		/* ptr to current attribute */
	ATTRIBINFO	*jatt;
	i4		j;
	i4		i;
	i4		numtbls;	/* number of table fields */
	bool		found;
	bool		badform;
	bool		fldmtched[MQ_MAXATTS];
	bool		fldokay;
	fname = mq_frame;
	numtbls = 0;
	badform = FALSE;
	atts = qdef->mqatts;		/* ptr to set of attributes */
	numatts = qdef->mqnumatts;
	/* first do some general form checking */
/* # line 71 "qdescfrm.qsc" */	/* formdata */
    {
      if (IIdispfrm(fname,(char *)"Names") == 0) goto IIfdE1;
      while (IIfnames() != 0) {
        {
/* # line 73 "qdescfrm.qsc" */	/* inquire_frs */
          {
            if (IIiqset(2,0,ERx(""),(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&istable,28,(char *)0,0);
              IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fldnm,19,(char *)0,0);
            } /* IIiqset */
          }
/* # line 74 "qdescfrm.qsc" */	/* host code */
		if (istable)
		{
			numtbls++;
			STcopy(fldnm, mq_tbl);
			if (numtbls > 1)
			{
				FDerror(MORETBLS, 0 , ERx(""));
				badform = TRUE;
/* # line 82 "qdescfrm.qsc" */	/* enddata */
          {
            if (1) goto IIfdE1;
          }
/* # line 83 "qdescfrm.qsc" */	/* host code */
			}
		}
        }
      } /* IIfnames */
IIfdE1:;
      IIendfrm();
    }
/* # line 87 "qdescfrm.qsc" */	/* host code */
	if (badform)
	{
		mq_forms = FALSE;
		mq_frame[0] = '\0';
		return(FALSE);
	}
	if (onetbl)
	{
		if (numtbls == 1)
		{
			tblfield = 1;
/* # line 98 "qdescfrm.qsc" */	/* putform */
    {
      if (IIfsetio((char *)"mqbftbls") != 0) {
        IIputfldio((char *)"tablefield",(short *)0,1,32,0,ERget(F_QF0010_YES));
      } /* IIfsetio */
    }
/* # line 99 "qdescfrm.qsc" */	/* host code */
		}
		else if (numtbls == 0)
		{
			tblfield = 0;
/* # line 103 "qdescfrm.qsc" */	/* putform */
    {
      if (IIfsetio((char *)"mqbftbls") != 0) {
        IIputfldio((char *)"tablefield",(short *)0,1,32,0,ERget(F_QF0011_NO));
      } /* IIfsetio */
    }
/* # line 104 "qdescfrm.qsc" */	/* host code */
		}
	}
	else if ((numtbls == 1) && (!tblfield))
	{
		tblfield = 1;
/* # line 109 "qdescfrm.qsc" */	/* putform */
    {
      if (IIfsetio((char *)"mqbftbls") != 0) {
        IIputfldio((char *)"tablefield",(short *)0,1,32,0,ERget(F_QF0010_YES));
      } /* IIfsetio */
    }
/* # line 110 "qdescfrm.qsc" */	/* host code */
	}
	if ((numtbls == 0) && (tblfield))
	{
		tblfield = 0;
/* # line 114 "qdescfrm.qsc" */	/* putform */
    {
      if (IIfsetio((char *)"mqbftbls") != 0) {
        IIputfldio((char *)"tablefield",(short *)0,1,32,0,ERget(F_QF0011_NO));
      } /* IIfsetio */
    }
/* # line 115 "qdescfrm.qsc" */	/* host code */
	}
	/*
	** initialize array of matched fields
	*/
	for (i = 0; i < numatts; i++)
	{
		fldmtched[i] = 0;
	}
/* # line 124 "qdescfrm.qsc" */	/* formdata */
    {
      if (IIdispfrm(fname,(char *)"Names") == 0) goto IIfdE2;
      while (IIfnames() != 0) {
        {
/* # line 126 "qdescfrm.qsc" */	/* inquire_frs */
          {
            if (IIiqset(2,0,ERx(""),(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fldnm,19,(char *)0,0);
              IIiqfsio((short *)0,1,30,4,&istable,28,(char *)0,0);
              IIiqfsio((short *)0,1,30,2,&ftype,42,(char *)0,0);
              IIiqfsio((short *)0,1,30,4,&fnum,23,(char *)0,0);
              IIiqfsio((short *)0,1,30,2,&flen,24,(char *)0,0);
              IIiqfsio((short *)0,1,32,9,fmode,21,(char *)0,0);
            } /* IIiqset */
          }
/* # line 131 "qdescfrm.qsc" */	/* host code */
		if (istable)
		{
/* # line 133 "qdescfrm.qsc" */	/* tabledata */
          {
            while (IItdata() != 0) {
              {
/* # line 135 "qdescfrm.qsc" */	/* inquire_frs */
                {
                  if (IIiqset(4,0,ERx(""),ERx(""),(char *)0) != 0) {
                    IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,cname,19,(char *)0
                    ,0);
                    IIiqfsio((short *)0,1,30,2,&ftype,42,(char *)0,0);
                    IIiqfsio((short *)0,1,30,2,&flen,24,(char *)0,0);
                    IIiqfsio((short *)0,1,32,9,fmode,21,(char *)0,0);
                  } /* IIiqset */
                }
/* # line 139 "qdescfrm.qsc" */	/* host code */
				found = FALSE;
				for (i = 0; i<numatts; i++)
				{
					catt = atts[i];
					if (STcompare(catt->formfield, cname) == 0)
					{
						found = TRUE;
						if ((mq_typechk(catt, cname, flen, ftype, fmode, fnum, istable)
							!= TRUE))
						{
							badform = TRUE;
/* # line 150 "qdescfrm.qsc" */	/* endloop */
                {
                  if (1) goto IItbE1;
                }
/* # line 151 "qdescfrm.qsc" */	/* host code */
						}
						fldmtched[i] = 1;
						break;
					}
				}
              }
            } /* IItdata */
IItbE1:
            IItdatend();
          }
/* # line 157 "qdescfrm.qsc" */	/* host code */
		}
		else
		{
			found = FALSE;
			for (i=0; i<numatts; i++)
			{
				catt = atts[i];
				if (STcompare(catt->formfield, fldnm) == 0)
				{
					found = TRUE;
					if ((mq_typechk(catt, fldnm, flen, ftype, fmode, fnum, istable) != TRUE))
					{
						badform = TRUE;
/* # line 170 "qdescfrm.qsc" */	/* enddata */
          {
            if (1) goto IIfdE2;
          }
/* # line 171 "qdescfrm.qsc" */	/* host code */
					}
					fldmtched[i] = 1;
					break;
				}
			}
		}
        }
      } /* IIfnames */
IIfdE2:;
      IIendfrm();
    }
/* # line 178 "qdescfrm.qsc" */	/* host code */
	if (badform)
		return(FALSE);
	for (i = 0; i<numatts; i++)
	{
		if (!(fldmtched[i]) && atts[i]->lkeyinfo == 0)
		{
# ifdef DEBUG
			if (mq_debug)
			{
				SIfprintf(mqoutf, ERx("MQFRMQDESC: unmatched field is %s\n"), atts[i]->formfield);
				SIfprintf(mqoutf, ERx("with intbl=%d and jpart=%d\n"), atts[i]->intbl, atts[i]->jpart);
			}
# endif
			if ((atts[i]->jpart) == -1)
			{
				atts[i]->formfield[0] = EOS;
			}
			else
			{
				fldokay = FALSE;
				catt = atts[i];
				for (j=0; j<numatts; j++)
				{
					jatt = atts[j];
					if (((STcompare(jatt->formfield, catt->formfield)) == 0) && (i != j))
					{
						if (fldmtched[j])
						{
							fldmtched[i] = 1;
							fldokay = TRUE;
							catt->intbl = jatt->intbl;
							break;
						}
					}
				}
				if (!fldokay)
				{
					/* join fields must be displayed */
					FDerror(NOMATCH, 1, atts[i]->col);
					return(FALSE);
				}
			}
		}
	}
# ifdef lint
	found = found;		/* use variable to keep lint happy */
# endif
	return(TRUE);
  }
mq_typechk(catt, fldnm, flen, ftype, fmode, fnum, istable)
ATTRIBINFO	*catt;
char		*fldnm;
i2		flen;		/*!!!!! NOTE: i2 parameter !!!!!*/
i2		ftype;		/*!!!!! NOTE: i2 parameter !!!!!*/
char		*fmode;
i4		fnum;
i4		istable;
{
	DB_DATA_VALUE	fld_dbv;
	bool		cancoerce;
	/* NOTE:
	** This is where MQBF checks the data type of
	** the field with that of the attribute.
	*/
	fld_dbv.db_length = flen;
	fld_dbv.db_datatype = ftype;
	if (afe_cancoerce(FEadfcb(), &catt->dbv, &fld_dbv, &cancoerce)
	    != OK)
	{
	    FDerror(COERCE_CHK_ERR, 0, ERx(""));
	    return FALSE;
	}
	if (cancoerce == FALSE)
	{
	    FDerror(TYPEMATCH, 2, fldnm, catt->col);
	    return FALSE;
	}
	/* now do other necessary checking */
	if (istable)
	{
		catt->intbl = 1;
		if (((STcompare(fmode, ERx("QUERY"))) == 0) || ((STcompare(fmode, ERx("READ"))) == 0))
		{
			catt->opupdate = 0;
# ifdef DEBUG
			if (mq_debug)
				SIfprintf(mqoutf, ERx("QUERY/DISPLAY only column: %s\n"), catt->formfield);
# endif
		}
	}
	else
	{
		catt->intbl = 0;
		if (((STcompare(fmode, ERx("QUERY"))) == 0) || ((STcompare(fmode, ERx("READ"))) == 0))
		{
			catt->opupdate = 0;
# ifdef DEBUG
			if (mq_debug)
				SIfprintf(mqoutf, ERx("QUERY/DISPLAY only field: %s\n"), catt->formfield);
# endif
		}
		if (fnum < 0)
			catt->opupdate = 0;
	}
	return(TRUE);
}
mqjpartchk(qdef)
MQQDEF	*qdef;
{
	ATTRIBINFO	**atts = qdef->mqatts;
	i4		numatts = qdef->mqnumatts;
	register JOININFO	*join;
	register i4		j;
	i4	att1;
	i4	att2;
	char	*jtype;
	char	*rng1;
	char	*col1;
	char	*rng2;
	char	*col2;
	for (j=0; j<qdef->mqnumjoins; j++)
	{
		join = qdef->mqjoins[j];
		jtype = join->jcode;
		if (((STcompare(jtype, ERx("MD"))) == 0)
			|| ((STcompare(jtype, ERx("DM"))) == 0))
		{
			rng1 = join->rv1;
			col1 = join->col1;
			rng2 = join->rv2;
			col2 = join->col2;
			att1 = mqfindatt(qdef, rng1, col1);
			att2 = mqfindatt(qdef, rng2, col2);
			if (atts[att1]->intbl && atts[att1]->lkeyinfo == 0)
			{
				FDerror(MDJINTBL, 1, col1);
				atts[att1]->intbl = 0;
				atts[att2]->intbl = 0;
				mq_forms = FALSE;
				mq_frame[0] = '\0';
				return(FALSE);
			}
		}
	}
	return(TRUE);
}
