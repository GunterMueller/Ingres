# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<cv.h>		/* 6-x_PC_80x86 */
#include	<st.h>
#include	<cm.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
# include "mqbf.h"
#include	<help.h>
#include	"erqf.h"
/**
** Name:	sort.qc - 
**
** History:
**		7/10/85 (gac)	Written.
**		1/30/86 (peter) Changed call to help.
**		10/20/86 (KY)  -- Changed CH.h to CM.h.
**		13-jul-87 (bab) Changed memory allocation to use [FM]Ereqmem.
**		13-aug-87 (danielt) changed FEmsg() calls to IIUGerr() 
**			for error messages.
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh.  Modified code so that
**			SQL sort parameters are set up as opposed to QUEL.
**		22-apr-88 (kenl)
**			Added code to handle the SQL ORDER BY clause.  Also
**			fixed bugs with QBF sort option.
**		15-jun-88 (kenl)	bug 12297
**			The field 'value' in mqsetsort() must be defined
**			for the maximum of TEXT characters allowed.
**			Otherwise CMS will walk all over the stack.
**      	09/26/90 (emerson) Don't display logical keys (bug 8593).
**      	02/28/91 (emerson) Corrected typo: "=" s.b. "==" in "if".
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**    25-Oct-2005 (hanje04)
**        Add prototypes for getvalue() & initsort() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
*/
/* local prototypes */
static STATUS getvalue();
static i4 initsort();
#define	_HelpExpl		F_FE0100_ExplFrameHelp
#define	_EndExpl		F_FE0102_ExplEnd
typedef struct srt
{
	ATTRIBINFO	*att;	/* attribute at this sort component */
	i1		type;	/* 0 if master, 1 if detail */
	char		dir;	/* 'a' or 'd' */
} MQSORT;
  extern char mq_frame[];
  extern char mq_tbl[];
/*{
** Name:	IIqfSetSort() -		Query Sort Frame.
**
** Description:
**	It initially loads the form from the sort structure.
**	When the frame is ended, the frame is unloaded into
**	the sort structure.  If any fields have sequence numbers,
**	they are placed in the sort list for the query generator.
**
** Input:
**	qdef	{MQQDEF *}  query definition
**
** History:
**	7/10/85 (gac)	Written.
**	10-nov-1993 (rdrane)
**		Ensure that sort attribute and correlation name are
**		unnormalized as required when generating the ORDER BY clause.
**		This addresses bug 56732.
*/
VOID
IIqfSetSort ( qdef )
MQQDEF	*qdef;
{
	QDESC	*qry = qdef->mqqdesc;
	bool	blanked = FALSE;
	IIqbfval(TRUE);
/* # line 98 "sort.qsc" */	/* display */
  {
    if (IIdispfrm(mq_frame,(char *)"q") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 101 "sort.qsc" */	/* host code */
		i4	seq;
		if ( mq_tbl[0] != EOS )
		{
/* # line 105 "sort.qsc" */	/* inittable */
      {
        if (IItbinit(mq_frame,mq_tbl,(char *)"q") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 106 "sort.qsc" */	/* host code */
		}
		seq = initsort(qry, 1, 0, qdef);
		if (qry->qg_child != NULL)
		{
			_VOID_ initsort(qry->qg_child, seq, 1, qdef);
		}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 115 "sort.qsc" */	/* host code */
		blanked = TRUE;
/* # line 116 "sort.qsc" */	/* clear */
          {
            IIclrflds();
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 120 "sort.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 126 "sort.qsc" */	/* host code */
		FEfehelp(ERx("qsort.hlp"), ERget(F_QF0018_Sort_Form), H_QBFSRT);
        }
      } else if (IIretval() == 4) {
        {
          i4 changed;
/* # line 132 "sort.qsc" */	/* inquire_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&changed,20,mq_frame,0);
            } /* IIiqset */
          }
/* # line 133 "sort.qsc" */	/* host code */
		if (changed || blanked)
		{
		    register i4	i;
		    i4			highmaster;
		    i4			lowdetail;
		    i4			nmsl = 0;
		    i4			ndsl = 0;
          char *lowdetfield;
		    char		dir;
		    QRY_SPEC	*mastsl[10];
		    QRY_SPEC	*detsl[10];
		    char	sortbuf[2][2000];
		    char	*sortp[2];
		    MQSORT	sortarr[MQ_MAXCOLS+1];
		    sortbuf[mqIN_MASTER][0] = sortbuf[mqIN_DETAIL][0] = EOS;
		    sortp[mqIN_MASTER] = sortbuf[mqIN_MASTER];
		    sortp[mqIN_DETAIL] = sortbuf[mqIN_DETAIL];
		    for ( i = 0 ; i <= MQ_MAXCOLS ; ++i )
		    {
			sortarr[i].att = NULL;
		    }
		    highmaster = 0;
		    lowdetail = MQ_MAXCOLS+1;
		    for ( i = 0 ; i < qdef->mqnumatts ; ++i )
		    {
			register ATTRIBINFO	*catt = qdef->mqatts[i];
			i4			seq;
          char *lastfield;
			if ( catt->jpart == 0 || catt->formfield[0] == EOS
					      || catt->lkeyinfo != 0 )
			{
			    continue;
			}
			lastfield = catt->formfield;
			if ( getvalue((bool)catt->intbl, lastfield, &seq, &dir)
					!= OK )
			{
			    if (catt->intbl)
			    {
/* # line 178 "sort.qsc" */	/* resume */
          {
            IIrescol(mq_tbl,lastfield);
            if (1) goto IIfdB1;
          }
/* # line 179 "sort.qsc" */	/* host code */
			    }
			    else
			    {
/* # line 182 "sort.qsc" */	/* resume */
          {
            IIresfld(lastfield);
            if (1) goto IIfdB1;
          }
/* # line 183 "sort.qsc" */	/* host code */
			    }
			}
			if (sortarr[seq].att != NULL)
			{
			    IIUGerr(E_QF0091_DuplicateSortSeq, UG_ERR_ERROR, 0);
			    if (catt->intbl)
			    {
/* # line 190 "sort.qsc" */	/* resume */
          {
            IIrescol(mq_tbl,lastfield);
            if (1) goto IIfdB1;
          }
/* # line 191 "sort.qsc" */	/* host code */
			    }
			    else
			    {
/* # line 194 "sort.qsc" */	/* resume */
          {
            IIresfld(lastfield);
            if (1) goto IIfdB1;
          }
/* # line 195 "sort.qsc" */	/* host code */
			    }
			}
			if (seq == 0)
			{
			    continue;
			}
			sortarr[seq].att = catt;
			if ( catt->ttype == mqIN_MASTER )
			{
			    if (seq > highmaster)
			    {
				highmaster = seq;
			    }
			}
			else
			{
			    if (seq < lowdetail)
			    {
				lowdetail = seq;
				lowdetfield = catt->formfield;
			    }
			}
			sortarr[seq].type = catt->ttype;
			sortarr[seq].dir = dir;
		    } /* end for */
		    if (highmaster > lowdetail)
		    {
			IIUGerr(E_QF0092_DetailSortSeq, UG_ERR_ERROR, 0);
			if ( mq_tbl[0] != EOS )
			{
/* # line 225 "sort.qsc" */	/* resume */
          {
            IIrescol(mq_tbl,lowdetfield);
            if (1) goto IIfdB1;
          }
/* # line 226 "sort.qsc" */	/* host code */
			}
			else
			{
/* # line 229 "sort.qsc" */	/* resume */
          {
            IIresfld(lowdetfield);
            if (1) goto IIfdB1;
          }
/* # line 230 "sort.qsc" */	/* host code */
			}
		    }
		    /* qg_sort is set to NULL but it should never get used */
		    qry->qg_sort = NULL;
		    qry->qg_order = NULL;
		    if (qry->qg_child != NULL)
		    {
			qry->qg_child->qg_sort = NULL;
			qry->qg_child->qg_order = NULL;
		    }
		    for ( i = 1 ; i <= MQ_MAXCOLS ; ++i )
		    {
			if (sortarr[i].att != NULL)
			{
		    	    i4	typ, clen, rlen;
			    char    xri_col_buf[(FE_UNRML_MAXNAME + 1)];
			    char    xri_rvar_buf[(FE_UNRML_MAXNAME + 1)];
			    typ = sortarr[i].type;
			    IIUGxri_id(sortarr[i].att->col,&xri_col_buf[0]);
			    IIUGxri_id(sortarr[i].att->rvar,&xri_rvar_buf[0]);
			    clen = STlength(&xri_col_buf[0]);
			    rlen = STlength(&xri_rvar_buf[0]);
			    if (sortp[typ] - sortbuf[typ] + clen + rlen + 7
				> 1999)
			    {
				/* emerson 02/28/91: changed "=" to "==" */
				if (typ == mqIN_MASTER)
				    qs_text(qdef->mqtag, &nmsl, mastsl,
					    sortbuf[0]);
			        else
				    qs_text(qdef->mqtag, &ndsl, detsl,
					    sortbuf[1]);
				sortp[typ] = sortbuf[typ];
			    }
			    if (sortp[typ] > sortbuf[typ])
				*sortp[typ]++ = ',';
			    STcopy(&xri_rvar_buf[0], sortp[typ]);
			    sortp[typ] += rlen;
			    *sortp[typ]++ = '.';
			    STcopy(&xri_col_buf[0], sortp[typ]);
			    sortp[typ] += clen;
			    *sortp[typ]++ = ' ';
			    *sortp[typ]++ = sortarr[i].dir;
			    /*
			    **	This code is cute.  If the first letter for
			    **	the sort direction is 'D' then it must be
			    **	followed by an 'E' before the 'SC' is added.
			    **  This way we build up 'desc' or 'asc'.
			    */
			    if (sortarr[i].dir == 'd')
    			    {
				*sortp[typ]++ = 'e';
			    }
			    *sortp[typ]++ = 's';
			    *sortp[typ]++ = 'c';
			    *sortp[typ] = EOS;
			}
		    }
		    if (nmsl > 0 || sortp[0] > sortbuf[0])
		    {
			i4	mtot = 0;
			qs_text(qdef->mqtag, &nmsl, mastsl, sortbuf[0]);
			/* alloc one extra QRY_SPEC for the QS_END */
			qry->qg_order = (QRY_SPEC (*)[])FEreqmem((u_i4)qdef->mqtag,
			    (u_i4)((nmsl+1) * sizeof(QRY_SPEC)),
			    FALSE, (STATUS *)NULL);
			qs_add(&mtot, qry->qg_order, nmsl, mastsl);
			qs_end(qdef->mqtag, mtot, qry->qg_order);
		    }
		    if (ndsl > 0 || sortp[1] > sortbuf[1])
		    {
			i4	dtot = 0;
			qs_text(qdef->mqtag, &ndsl, detsl, sortbuf[1]);
			/* alloc one extra QRY_SPEC for the QS_END */
			qry->qg_child->qg_order =
			    (QRY_SPEC (*)[])FEreqmem((u_i4)qdef->mqtag,
			    (u_i4)((ndsl+1) * sizeof(QRY_SPEC)),
			    FALSE, (STATUS *)NULL);
			qs_add(&dtot, qry->qg_child->qg_order, ndsl, detsl);
			qs_end(qdef->mqtag, dtot, qry->qg_child->qg_order);
		    }
		}
/* # line 324 "sort.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 5) {
        {
          char fld[FE_MAXNAME+1];
/* # line 329 "sort.qsc" */	/* host code */
		i4 	seq;
		char	dir;
/* # line 332 "sort.qsc" */	/* inquire_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fld,22,mq_frame,0);
            } /* IIiqset */
          }
/* # line 333 "sort.qsc" */	/* host code */
		if ( getvalue(FALSE, fld, &seq, &dir) == OK )
		{
/* # line 335 "sort.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
/* # line 336 "sort.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 6) {
        {
          char fld[FE_MAXNAME+1];
/* # line 342 "sort.qsc" */	/* host code */
		i4 	seq;
		char	dir;
/* # line 345 "sort.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,mq_frame,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fld,34,mq_tbl,0);
            } /* IIiqset */
          }
/* # line 346 "sort.qsc" */	/* host code */
		if ( getvalue(TRUE, fld, &seq, &dir) == OK )
		{
/* # line 348 "sort.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
/* # line 349 "sort.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 7) {
        {
/* # line 353 "sort.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 8) {
        {
/* # line 357 "sort.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    goto IIfdE1;
IImuI1:
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Blank),(char *)0,0,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Forget),(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,3) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),(char *)0,2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"all",0,5) == 0) goto IIfdE1;
    if (IITBcaClmAct(mq_tbl,(char *)"all",0,6) == 0) goto IIfdE1;
    if (IIactscrl(mq_tbl,0,7) == 0) goto IIfdE1;
    if (IIactscrl(mq_tbl,1,8) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 360 "sort.qsc" */	/* host code */
	IIqbfval(FALSE);
}
static STATUS
  getvalue( intblfld, objname, sequence, direction )
bool	intblfld;
  char *objname;
i4	*sequence;
char	*direction;
  {
	register char	*v;
    i4 gnum;
    char value[DB_MAXTUP];
	if (intblfld)
	{
/* # line 377 "sort.qsc" */	/* inquire_frs */
    {
      if (IIiqset(4,0,mq_frame,mq_tbl,(char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&gnum,23,objname,0);
      } /* IIiqset */
    }
/* # line 379 "sort.qsc" */	/* host code */
		gnum -= 1;
		objname = mq_tbl;
	}
	else
	{
		gnum = 0;
	}
	IIqbfgetraw(mq_frame, objname, gnum, value);
	*sequence = 0;
	for ( v = value ; CMwhite(v) ; CMnext(v) )
		;
	if ( CMdigit(v) )
	{ /* sequence : direction */
		i4	num = 0;
		for (; CMdigit(v); v++)
		{
			num = 10*num + (*v - '0');
		}
		if ( num < 1 || num > MQ_MAXCOLS )
		{
			i4	tmp = MQ_MAXCOLS;
			IIUGerr(E_QF0093_MaxSortSeq, UG_ERR_ERROR, 1, (PTR)&tmp);
			return FAIL;
		}
		else
		{
			*sequence = num;
			for (; CMwhite(v); CMnext(v))
				;
			if (*v == ':')
			{
				for ( v += 1 ; CMwhite(v) ; CMnext(v) )
					;
			}
			CMtolower(v, v);
			switch (*v)
			{
			case '\0':
			case 'a':
				*direction = 'a';
				break;
			case 'd':
				*direction = 'd';
				break;
			default:
				IIUGerr(E_QF0094_ValidSortDir, UG_ERR_ERROR, 0);
				return FAIL;
			}
		}
	}
	else if ( *v != EOS )
	{
		i4	tmp = MQ_MAXCOLS;
		IIUGerr( E_QF0093_MaxSortSeq, UG_ERR_ERROR, 1, (PTR)&tmp );
		return FAIL;
	}
	return OK;
  }
static VOID	putvalue();
static i4
initsort(qry, seq, type, qdef)
register QDESC	*qry;
i4		seq;
i4		type;
MQQDEF		*qdef;
{
	if (qry->qg_order != NULL)
	{
		QRY_SPEC	*sl;
		for ( sl =(QRY_SPEC*)qry->qg_order; sl->qs_type != QS_END; ++sl)
		{
			register char	*p;
			char		name[FE_MAXNAME+1];
			char		rname[FE_MAXNAME+1];
			for ( p = sl->qs_var ; *p != EOS ; ++p )
			{
				register char	*n;
		    		register i4	i;
			    if (*p == ',')
			    {
				continue;
			    }
			    n = rname;
			    while (*p != '.')
			    {
				*n++ = *p++;
			    }
			    *n = EOS;
			    ++p;
			    n = name;
			    while (*p != ' ')
			    {
				*n++ = *p++;
			    }
			    *n = EOS;
			    for (i = 0; i < qdef->mqnumatts; i++)
			    {
				register ATTRIBINFO	*catt = qdef->mqatts[i];
				if (catt->ttype == type && catt->jpart != 0 &&
						catt->formfield[0] != EOS &&
						catt->lkeyinfo == 0)
				{
				    if ( STequal(catt->col, name) &&
					    STequal(catt->rvar, rname) )
				    {
					char	value[10];
			    		CVna(seq++, value);
			    		if (*++p == 'd')
						STcat(value, ERx("d"));
					putvalue((bool)catt->intbl,
						catt->formfield, value
					);
					break;
				    }
				}
			    }
			    while ( *p != ',' && *p != EOS )
			    {
				++p;
			    }
			    if (*p == EOS)
				break;
			}
		}
	}
	return seq;
}
static VOID
  putvalue( intblfld, objname, value )
bool	intblfld;
  char *objname;
char	*value;
  {
    i4 pnum;
	if (intblfld)
	{
/* # line 536 "sort.qsc" */	/* inquire_frs */
    {
      if (IIiqset(4,0,mq_frame,mq_tbl,(char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&pnum,23,objname,0);
      } /* IIiqset */
    }
/* # line 538 "sort.qsc" */	/* host code */
		pnum -= 1;
		objname = mq_tbl;
	}
	else
	{
		pnum = 0;
	}
	IIqbfputraw( mq_frame, objname, pnum, value );
  }
