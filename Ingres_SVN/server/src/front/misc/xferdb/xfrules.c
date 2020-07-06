# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
*/
# include       <compat.h>
# include       <gl.h>
# include       <sl.h>
# include       <iicommon.h>
# include       <st.h>
# include       <er.h>
# include       <si.h>
# include       <lo.h>
# include       <fe.h>
# include       <ug.h>
# include       <adf.h>
# include       <afe.h>
# include <xf.h>
# include       "erxf.h"
/*
** Fool MING, which doesn't know about EXEC SQL INCLUDE
# include <xf.qsh>
*/
/**
** Name:        xfrules.sc - write statements to handle rules, events,
**                      and some miscellany.
**
** Description:
**      This file defines:
**
**      xfrules         write statement to create rules.
**      xfevents        write statement to create events and associated
**                      GRANTs.
**	xfsequences	write statement to create sequences.
**      xfsynonyms      write stmt to create synonyms.
**
** History:
**      16-may-89 (marian)
**              written to support terminator RULES.
**      05-mar-1990 (mgw)
**              Changed #include <erxf.h> to #include "erxf.h" since this is
**              a local file and some platforms need to make the destinction.
**      04-may-90 (billc)
**              Major rewrite.  Convert to SQL.
**      04-mar-91 (billc)
**              Add support for events.
**      14-aug-91 (billc)
**              fix 39229: forgot to change "event" to "dbevent" for LRC change.
**      27-jul-1992 (billc)
**              Rename from .qsc to .sc suffix.
**      25-oct-1994 (sarjo01) Bug 63542
**              xfsynonyms(): synonym name was being output in single
**              quotes (?). Changed code to use xfwrite_id() to
**              see if correct delimiters are needed.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	29-jan-03 (inkdo01)
**	    Add support for sequences.
**/
/* # define's */
/* GLOBALDEF's */
GLOBALREF bool With_sequences;
GLOBALREF i4   Objcount;
GLOBALREF PTR  Obj_list;
/* extern's */
/* static's */
/*{
** Name:        xfrules - write statement to create rules.
**
** Description:
**      Write CREATE RULE statements for all rules.
**
** Inputs:
**
** Outputs:
**
**      Returns:
**
** History:
**      16-may-89 (marian) written.
*/
static void write_rule(
    char        *rule_name,
    char        *rule_owner,
    char        *text_segment,
    i4          text_sequence,
    TXT_HANDLE  **tfd,
    i4          *rcount);
void
xfrules()
{
  char text_segment[XF_RULELINE + 1];
  char rule_name[FE_MAXNAME + 1];
  char rule_owner[FE_MAXNAME + 1];
  i4 text_sequence;
    i4     rcount = 0;
    TXT_HANDLE    *tfd = NULL;
    if (With_65_catalogs)
    {
/* # line 113 "xfrules.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select r.rule_name, r.rule_owner, r.text_segment, r.text_sequence fro\
m iirules r where(r.rule_owner=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"or ''=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)
")and r.system_use<>'G' order by r.rule_owner, r.rule_name, r.text_seq\
uence");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,rule_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,rule_owner);
      IIgetdomio((short *)0,1,32,XF_RULELINE + 1-1,text_segment);
      IIgetdomio((short *)0,1,30,4,&text_sequence);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 121 "xfrules.sc" */	/* host code */
        {
            write_rule(rule_name, rule_owner, text_segment, text_sequence,
                                    &tfd, &rcount);
        }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
    }
    else
    {
/* # line 129 "xfrules.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select r.rule_name, r.rule_owner, r.text_segment, r.text_sequence fro\
m iirules r where(r.rule_owner=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"or ''=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)
")order by r.rule_owner, r.rule_name, r.text_sequence");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,rule_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,rule_owner);
      IIgetdomio((short *)0,1,32,XF_RULELINE + 1-1,text_segment);
      IIgetdomio((short *)0,1,30,4,&text_sequence);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 136 "xfrules.sc" */	/* host code */
        {
            write_rule(rule_name, rule_owner, text_segment, text_sequence,
                                    &tfd, &rcount);
        }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
    }
    if (tfd != NULL)
        xfclose(tfd);
    xf_found_msg(ERx("R"), rcount);
}
static void
write_rule( char        *rule_name,
    char        *rule_owner,
    char        *text_segment,
    i4          text_sequence,
    TXT_HANDLE  **tfd,
    i4          *rcount)
{
    if (*tfd == NULL)
    {
        /*
        ** First time in loop.  Write a comment indicating that
        ** we're writing rules now.
        */
        xfwritehdr(RULES_COMMENT);
        *tfd = xfreopen(Xf_in, TH_IS_BUFFERED);
        /* Set language for rules (rules are supported only in sql). */
        xfsetlang(*tfd, DB_SQL);
    }
    (void) STtrmwhite(rule_name);
    xfread_id(rule_owner);
    if (text_sequence == 1)
    {
        /* We have a new rule. */
        if (*rcount > 0)
            xfflush(*tfd);
        /* Does user id have to be reset? */
        xfsetauth(*tfd, rule_owner);
        (*rcount)++;
    }
    xfwrite(*tfd, text_segment);
}
/*{
** Name:        xfevents - write statements to create events and their
**                      associated GRANTs.
**
** Description:
**      Write CREATE DBEVENT statements for all events, interleaved with
**      GRANT statements.
**
** Inputs:
**      output_flags
**
** Outputs:
**
**      Returns:
**              none.
**
**      Exceptions:
**              none.
**
** Side Effects:
**      none.
**
** History:
**      04-mar-91 (billc) written.
**	9-Sep-2004 (schka24)
**	    permit-only output for upgradedb.
*/
static void write_dbevent(
    char        *e_name,
    char        *e_owner,
    char        *permit_grantor,
    char        *p_text,
    i2          permit_number,
    i4          text_sequence,
    TXT_HANDLE  **tfd,
    i4          *ecount,
    i4		output_flags);
void
xfevents(i4 output_flags)
{
  char e_name[FE_MAXNAME + 1];
  char e_owner[FE_MAXNAME + 1];
  char permit_grantor[FE_MAXNAME + 1];
  char p_text[XF_PERMITLINE + 1];
  i4 text_sequence;
  i2 permit_number;
  i2 permit_depth;
    i4          ecount = 0;
    TXT_HANDLE  *tfd = NULL;
    if (With_65_catalogs)
    {
        /* permit_grantor, permit_depth debut in 6.5 */
/* # line 247 "xfrules.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select e.event_owner, e.event_name, '',  -1, 0, 0, '' from iievents e\
 where(e.event_owner=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"or ''=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)
")union select p.object_owner, p.object_name, p.permit_grantor, p.perm\
it_depth, p.permit_number, p.text_sequence, p.text_segment from iiperm\
its p where(p.object_owner=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"or ''=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)
")and p.object_type='E' order by 1, 2, 4, 5, 6");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,e_owner);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,e_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,permit_grantor);
      IIgetdomio((short *)0,1,30,2,&permit_depth);
      IIgetdomio((short *)0,1,30,2,&permit_number);
      IIgetdomio((short *)0,1,30,4,&text_sequence);
      IIgetdomio((short *)0,1,32,XF_PERMITLINE + 1-1,p_text);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 262 "xfrules.sc" */	/* host code */
        {
            write_dbevent(e_name, e_owner, permit_grantor, p_text,
                        permit_number, text_sequence, &tfd, &ecount,
			output_flags);
        }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
    }
    else
    {
        /* 6.4 and earlier catalogs. */
/* # line 273 "xfrules.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select e.event_owner, e.event_name, '', 0, 0, '' from iievents e wher\
e(e.event_owner=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"or ''=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)
")union select p.object_owner, p.object_name, p.object_owner, p.permit\
_number, p.text_sequence, p.text_segment from iipermits p where(p.obje\
ct_owner=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"or ''=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)
")and p.object_type='E' order by 1, 2, 4, 5");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,e_owner);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,e_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,permit_grantor);
      IIgetdomio((short *)0,1,30,2,&permit_number);
      IIgetdomio((short *)0,1,30,4,&text_sequence);
      IIgetdomio((short *)0,1,32,XF_PERMITLINE + 1-1,p_text);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 287 "xfrules.sc" */	/* host code */
        {
            write_dbevent(e_name, e_owner, permit_grantor, p_text,
                        permit_number, text_sequence, &tfd, &ecount,
			output_flags);
        }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
    }
    if (tfd != NULL)
        xfclose(tfd);
    xf_found_msg(ERx("E"), ecount);
}
static void
write_dbevent(
    char        *e_name,
    char        *e_owner,
    char        *permit_grantor,
    char        *p_text,
    i2          permit_number,
    i4          text_sequence,
    TXT_HANDLE  **tfd,
    i4          *ecount,
    i4		output_flags)
{
    if (*tfd == NULL)
    {
        /* First time in loop.  Write an informative comment.  */
        xfwritehdr(EVENTS_COMMENT);
        *tfd = xfreopen(Xf_in, TH_IS_BUFFERED);
        /* set language for events (events only in sql) */
        xfsetlang(*tfd, DB_SQL);
    }
    if (permit_number <= 0)
    {
        /* This is "CREATE DBEVENT" stuff.  No text. */
	/* No print if just permits-only */
	if (!output_flags
	  || (output_flags & XF_EVENT_ONLY)
	  || (output_flags & XF_PRINT_TOTAL) )
	{
	    if (*ecount > 0)
		xfflush(*tfd);
	    /* Does user id have to be reset? */
	    xfread_id(e_owner);
	    xfsetauth(*tfd, e_owner);
	    /* fix 39229: forgot to change "event" to "dbevent" (LRC change) */
	    xfwrite(*tfd, ERx("create dbevent "));
	    xfread_id(e_name);
	    xfwrite_id(*tfd, e_name);
	    (*ecount)++;
	}
    }
    else
    {
        /* This is permission text */
        /* if new permit statement */
        if (text_sequence == 1)
        {
            /* flush the previous statement. */
            xfflush(*tfd);
            /* Does grantor's user id need to be reset? */
            (void) STtrmwhite(permit_grantor);
            xfsetauth(*tfd, permit_grantor);
        }
        xfwrite(*tfd, p_text);
    }
}
/*{
** Name:        xfsequences - write statement to create sequences.
**
** Description:
**      Write CREATE SEQUENCE statements for all sequences.
**
** Inputs:
**
** Outputs:
**
**      Returns:
**
** History:
**      29-jan-03 (inkdo01)
**	    Written.
**	30-july-04 (inkdo01)
**	    Change so copy out of sequence sets start value to current 
**	    next value.
**      29-Jan-2008 (hanal04) Bug 119827
**          The fix for Bug 117574 reintroduced Bug 114150. If we were
**          given a table list on the command line we should only dump
**          sequences that are required to create the specified tables.
**      19-Jan-2008 (hanal04) Bug 119945
**          E_AD2093 error when column_default_val contains float values
**          not associated with sequences. Exclused non-sequence values by
**          checking for "next value for%". 
*/
void
xfsequences()
{
  char seqname[FE_MAXNAME + 1];
  char seqowner[FE_MAXNAME + 1];
  char seqtype[8];
  int seqlen, seqprec, seqcache;
  char seqstart[31], seqincr[31], seqmin[31], seqmax[31];
  char seqstartf[2], seqincrf[2], seqminf[2], seqmaxf[2];
  char seqrestartf[2], seqcachef[2], seqcyclef[2], seqorderf[2];
  char *obj_name;
    char	buf[256];
    i4		scount = 0;
    i4		seqtlen;
    bool	first = TRUE;
    TXT_HANDLE	*tfd = NULL;
    DB_STATUS   status;
    char        *entry_key;
    if (With_sequences && With_20_catalogs)
    {
/* # line 420 "xfrules.sc" */	/* declare */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"declare global temporary table session.seq_list as select seq_name, s\
eq_owner, data_type, seq_length, seq_precision, trim(char(next_value))\
as next_value, trim(char(increment_value))as increment_value, trim(cha\
r(min_value))as min_value, trim(char(max_value");
    IIwritio(0,(short *)0,1,32,0,(char *)
"))as max_value, cache_size, start_flag, incr_flag, min_flag, max_flag\
, restart_flag, cache_flag, cycle_flag, order_flag from iisequences wh\
ere(1=0)on commit preserve rows with NORECOVERY");
    IIsyncup((char *)0,0);
  }
/* # line 432 "xfrules.sc" */	/* host code */
        if (Objcount && Obj_list)
        {
            status = IIUGhsHtabScan(Obj_list, FALSE, &entry_key, &obj_name);
            while(status)
            {
/* # line 438 "xfrules.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into session.seq_list select seq_name, seq_owner, data_type, s\
eq_length, seq_precision, trim(char(next_value)), trim(char(increment_\
value)), trim(char(min_value)), trim(char(max_value)), cache_size, sta\
rt_flag, incr_flag, min_flag, max_flag, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"restart_flag, cache_flag, cycle_flag, order_flag from iisequences, ii\
columns where((seq_owner=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"or ''=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)")and(table_name=");
    IIputdomio((short *)0,1,32,0,obj_name);
    IIwritio(0,(short *)0,1,32,0,(char *)
")and(column_default_val like 'next value for%')and(seq_name=substring\
(column_default_val from(locate(column_default_val, '.') +2)for(length\
(column_default_val) -locate(column_default_val, '.') -2))))");
    IIsyncup((char *)0,0);
  }
/* # line 453 "xfrules.sc" */	/* host code */
                status = IIUGhsHtabScan(Obj_list, TRUE, &entry_key, &obj_name);
            }
        }
        else
        {
/* # line 458 "xfrules.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into session.seq_list select seq_name, seq_owner, data_type, s\
eq_length, seq_precision, trim(char(next_value)), trim(char(increment_\
value)), trim(char(min_value)), trim(char(max_value)), cache_size, sta\
rt_flag, incr_flag, min_flag, max_flag, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"restart_flag, cache_flag, cycle_flag, order_flag from iisequences whe\
re(seq_owner=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"or ''=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 466 "xfrules.sc" */	/* host code */
        }
        /* SELECT DISTINCT as Objlist may have generated duplicates */
/* # line 469 "xfrules.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"xfrules1",7149,19260);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"xfrules1",7149,19260);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct seq_name, seq_owner, data_type, seq_length, seq_preci\
sion, next_value, increment_value, min_value, max_value, cache_size, s\
tart_flag, incr_flag, min_flag, max_flag, restart_flag, cache_flag, cy\
cle_flag, order_flag from session.seq_list ");
        IIwritio(0,(short *)0,1,32,0,(char *)"order by seq_owner, seq_\
name");
        IIexDefine(0,(char *)"xfrules1",7149,19260);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,seqname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,seqowner);
      IIgetdomio((short *)0,1,32,7,seqtype);
      IIgetdomio((short *)0,1,30,4,&seqlen);
      IIgetdomio((short *)0,1,30,4,&seqprec);
      IIgetdomio((short *)0,1,32,30,seqstart);
      IIgetdomio((short *)0,1,32,30,seqincr);
      IIgetdomio((short *)0,1,32,30,seqmin);
      IIgetdomio((short *)0,1,32,30,seqmax);
      IIgetdomio((short *)0,1,30,4,&seqcache);
      IIgetdomio((short *)0,1,32,1,seqstartf);
      IIgetdomio((short *)0,1,32,1,seqincrf);
      IIgetdomio((short *)0,1,32,1,seqminf);
      IIgetdomio((short *)0,1,32,1,seqmaxf);
      IIgetdomio((short *)0,1,32,1,seqrestartf);
      IIgetdomio((short *)0,1,32,1,seqcachef);
      IIgetdomio((short *)0,1,32,1,seqcyclef);
      IIgetdomio((short *)0,1,32,1,seqorderf);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 482 "xfrules.sc" */	/* host code */
        {
	    if (first)
	    {
		first = FALSE;
		xfwritehdr(SEQUENCES_COMMENT);
        	tfd = xfreopen(Xf_in, TH_IS_BUFFERED);
        	/* Set language for sequences (sequences are supported 
		** only in sql). */
        	xfsetlang(tfd, DB_SQL);
	    }
            /* Write the bits and pieces of the sequence definition. */
	    xfread_id(seqname);
	    xfread_id(seqowner);
	    if (scount > 0)
		xfflush(tfd);
            /* Does user id have to be reset? */
            xfsetauth(tfd, seqowner);
            scount++;
	    xfwrite(tfd, ERx("create sequence "));
	    xfwrite_id(tfd, seqname);
	    /* Explicitly write data type. */
	    if (seqtype[0] == 'i')
		STprintf(buf, " as integer\n");
	    else STprintf(buf, " as decimal(%d)\n", seqprec);
	    xfwrite(tfd, buf);
	    /* Start and increment values. */
	    STprintf(buf, "    start with %s increment by %s\n",
		seqstart, seqincr);
	    xfwrite(tfd, buf);
	    /* Min and max values. */
	    STprintf(buf, "    minvalue %s maxvalue %s\n",
		seqmin, seqmax);
	    xfwrite(tfd, buf);
	    /* Cache (or nocache) and the other flags. */
	    STprintf(buf, "    ");
	    if (seqcachef[0] == 'N')
		STprintf(&buf[4], "no cache ");
	    else STprintf(&buf[4], "cache %d ", seqcache);
	    seqtlen = STlength(buf);
	    if (seqcyclef[0] != ' ')
		STprintf(&buf[seqtlen-1], " %s cycle ",
		    (seqcyclef[0] == 'N') ? ERx("no") : ERx(" "));
	    seqtlen = STlength(buf);
	    if (seqorderf[0] != ' ')
		STprintf(&buf[seqtlen-1], " %s order ",
		    (seqorderf[0] == 'N') ? ERx("no") : ERx(" "));
	    xfwrite(tfd, buf);
        }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
/* # line 537 "xfrules.sc" */	/* drop */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop session.seq_list");
    IIsyncup((char *)0,0);
  }
/* # line 538 "xfrules.sc" */	/* host code */
    }
    if (tfd != NULL)
        xfclose(tfd);
    xf_found_msg(ERx("Q"), scount);
}
/*{
** Name:        xfsynonyms - write statements to create synonyms.
**
** Description:
**      Write CREATE SYNONYM statements for all synonyms
**
** Inputs:
**      none.
**
** Outputs:
**
**      Returns:
**              none.
**
**      Exceptions:
**              none.
**
** Side Effects:
**      none.
**
** History:
**      04-mar-91 (billc) written.
**      25-oct-94 (sarjo01) Bug 63542
**                removed use of single quotes in create synonym stmt;
**                use xfwrite_id() to handle delimited identifiers.
**	16-nov-2000 (gupsh01)
**	    Added the synonym_name to the order by sequence, in order to
**	    list the synonyms alphabetically.
*/
void
xfsynonyms()
{
  char s_name[FE_MAXNAME + 1];
  char s_owner[FE_MAXNAME + 1];
  char t_name[FE_MAXNAME + 1];
  char t_owner[FE_MAXNAME + 1];
    i4          scount = 0;
    char        tmpbuf[1028];
/* # line 588 "xfrules.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select s.synonym_owner, s.synonym_name, s.table_owner, s.table_name f\
rom iisynonyms s where(s.synonym_owner=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"or ''=");
    IIputdomio((short *)0,1,32,0,Owner);
    IIwritio(0,(short *)0,1,32,0,(char *)
")order by s.synonym_owner, s.synonym_name");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,s_owner);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,s_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,t_owner);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,t_name);
      if (IIerrtest() != 0) goto IIrtB6;
/* # line 595 "xfrules.sc" */	/* host code */
    {
        if (scount == 0)
        {
            /* First time in loop.  Write an informative comment.  */
            xfwritehdr(SYNONYMS_COMMENT);
            /* set language for synonyms (synonyms only in sql) */
            xfsetlang(Xf_in, DB_SQL);
        }
        xfread_id(s_name);
        xfread_id(s_owner);
        xfread_id(t_name);
        xfread_id(t_owner);
        /* Does user id have to be reset? */
        xfsetauth(Xf_in, s_owner);
/*
        STprintf(tmpbuf, ERx("create synonym %s for '%s'.%s%s"),
                s_name, t_owner, t_name, GO_STMT);
        xfwrite(Xf_in, tmpbuf);
*/
        xfwrite(Xf_in, "create synonym ");
        xfwrite_id(Xf_in, s_name);
        xfwrite(Xf_in, " for ");
        xfwrite_id(Xf_in, t_owner);
        xfwrite(Xf_in, ".");
        xfwrite_id(Xf_in, t_name);
        xfwrite(Xf_in, GO_STMT);
        scount++;
    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
    if (scount > 0)
        xf_found_msg(ERx("S"), scount);
}
