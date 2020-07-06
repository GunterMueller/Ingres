# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 1988, 2005 Ingres Corporation
**
*/
#include	<compat.h>
#include	<gl.h>
#include	<me.h>
#include	<bt.h>
#include	<st.h>
#include	<sl.h>
#include	<iicommon.h>
#include	<dbdbms.h>
#include	<er.h>
#include	<lo.h>
#include	<pc.h>
#include	<si.h>
#include	<duerr.h>
#include	<duvfiles.h>
#include	<duve.h>
#include	<duustrings.h>
#include	<cs.h>
#include	<lg.h>
#include	<lk.h>
#include	<dm.h>
#include	<dmf.h>
#include	<dmp.h>
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include <duvevent.h>
# include <duveproc.h>
# include <duvepro.h>
# include <duveprv.h>
/**
**
**  Name: DUVECHK2.SC - Verifydb System Catalog checks for non-core dbms
**		       catalogs.
**
**  Description:
**      This module contains the ESQL to execute the tests/fixed on the system
**	catalogs specified by the Verifydb Product Specification for:
**	    iidbms_comments
**	    iidbdepends
**	    iidefault
**	    iievent
**	    iigw06_attribute
**	    iigw06_relation
**	    iihistogram
**	    iiintegrity
**	    iikey	
**	    iiprocedure
**	    iiprotect
**	    iiqrytext
**	    iirule
**	    iistatistics
**	    iisynonym
**	    iitree
**	    iixdbdepends
**	    iisecalarm
**  
**      duvechk2.sc uses the verifydb control block to obtain information
**	about the verifydb system catalog check environment and to access
**	cached information about the iirelation and iidevices catalogs.
**	If duvechk wishes to drop a user table from the DBMS, it marks the
**	table for drop by setting the drop_table flag in the cached relinfo
**	(information about iirelation).
**
**	    ckcomments - check / fix iidbms_comments system catalog
**	    ckdbdep - check / fix iidbdepends system catalog
**	    ckdefault - check /fix iidefault system catalog
**	    ckevent - check / fix iievent system catalog
**	    ckgw06att - check / fix iigw06_attribute catalog
**	    ckgw06rel - check / fix iigw06_relation calalog
**	    ckhist - check / fix iihistogram system catalog
**          ckintegs - check / fix iiintegrity system catalog
**	    ckkey - check / fix iikey system catalog
**	    ckproc - check /fix iiprocedure catalog
**	    ckprotups - check/fix iiprotect catalog
**	    ckqrytxt - check / fix iiqrytext system catalog
**	    ckrule - check / fix iirule system catalog
**	    ckstat - check / fix iistatistic system catalog
**	    cksynonym - check /fix iisynonym system catalog
**	    cktree - check / fix iitree system catalog
**	    ckxdbdep - check / fix iixdbdepends system catalog
**	    cksecalarm  - check / fix iisecalarm system catalog
**	    
**	Special ESQL include files are used to describe each dbms system
**	catalog. These include files are generated via the DCLGEN utilility
**	and require a dbms server to be running. The following commands create
**	the include files in the format required by verifydb:
**	    dclgen c iidbdb iidbms_commnt duvecmnt.sh cmnt_tbl
**	    dclgen c iidbdb iidbdepends duvedpnd.sh  dpnd_tbl
**	    dclgen c iidbdb iidefault duvedef.sh default_tbl
**	    dclgen c iidbdb iievent duvevent.sh evnt_tbl
**	    dclgen c iidbdb iihistogram duvehist.sh hist_tbl
**	    dclgen c iidbdb iiintegrity duveint.sh int_tbl
**	    dclgen.c iidbdb iikey duvekey.sh key_tbl 
**	    dclgen c iidbdb iiprocedure duveproc.sh proc_tbl
**	    dclgen c iidbdb iiprotect duvepro.sh pro_tbl
**	    dclgen c iidbdb iiqrytext duveqry.sh qry_tbl
**	    dclgen c iidbdb iirule duverule.sh rule_tbl
**	    dclgen c iidbdb iistatistics duvestat.sh stat_tbl
**	    dclgen c iidbdb iisynonym duvesyn.sh syn_tbl
**	    dclgen c iidbdb iitree duvetree.sh tree_tbl
**	    dclgen c iidbdb iixdbdepends duvexdp.sh xdpnd_tbl
**	    dclgen c iidbdb iixpriv duvexprv.sh xprv_tbl
**          dclgen c iidbdb iipriv duveprv.sh prv_tbl
**	    dclgen c iidbdb iixevent duvexev.sh xev_tbl
**	    dclgen c iidbdb iixprocedure duvexdbp.sh xdbp_tbl
**	    dclgen c iidbdb iisecalarm duvealm.sh alarm_tbl
**
** History:
**	25-Aug-88 (teg)
**	    initial creation
**      04-Apr-89 (neil)
**          Added checks for rule trees and text.  At this time this was only
**          added to suppress failures and not to actually test the
**          relationships between iitree, iiqrytext, iirelation and iirule.
**      03-Jan-89 (neil)
**          Added checks for event text, permits and trees.  As with rules
**          these checks were added to suppress failures and not to actually
**          test the relationships between the qrymod tables and iievent.
**      12-Jun-90 (teresa)
**          add logic to check iisynonym catalog
**      15-feb-91 (teresa)
**          fix bug 35940 (assumed there was only 1 tree entry per table id,
**          which is invalid assumption.  ex: define multiple QUEL permits on
**          the same table.)
**      06-jan-92 (rickh)
**          Constraints don't have query trees so please forgive
**          them.
**      26-apr-1993 (bryanp)
**          6.5 Cluster support:
**              Include <lg.h> to pick up logging system typedefs
**	16-jun-93 (anitap)
**	    Added new tests to cksynonmym, ckproc() and ckevent() for CREATE 
**	    SCHEMA project.
**	    Added new routines ckdefault() and ckkey() and tests to ckintegs()
**          and ckdbdep() for FIPS constraints.
**	26-jul-1993 (bryanp)
**	    Add includes of <lk.h> and <pc.h> in support of dmp.h. I would
**	    prefer that this file did not include dmp.h; maybe someday that
**	    will be changed.
**	30-jul-1993 (stephenb)
**	    Added routines ckgw06rel() and ckgw06att() for verifydb
**	    dbms_catalogs checks of iigw06_relation and iigw06_attribute.
**	 4-aug-93 (shailaja)
**	    Unnested comments.
**	8-aug-93 (ed)
**	    unnest dbms.h
**	18-sep-93 (teresa)
**	    Add routine ckcomments() and add new tests to ckdatabase().
**	10-oct-93 (anitap)
**	    Added new tests to ckrule() and ckproc() for FIPS
**	    constraints' project. Also added support to delete tuples from
**	    iiintegrity if corresponding tuples not present in iirule, 
**	    iiprocedure, iiindex in ckintegs().
**	    Added new routine create_cons() for FIPS constraints' project.
**	    Corrected spelling of message S_DU0320_DROP_FROM_IIINTERGRITY to
**	    S_DU0320_DROP_FROM_IIINTEGRITY in ckintegs().
**	17-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
**	20-dec-93 (robf)
**          Add cksecalarm() for security alarms.
**	01-jan-94 (anitap)
**	    performance enhancement for verifydb. Use repeat queries wherever 
**	    possible and use ANY instead of count. Use iidbdepends cache and
**	    iirelation cache wherever possible.
**	    If more than one referential constraint dependent on an unique
**	    constraint, use loop to create all the referential constraints
**	    after dropping the unique constraint. 
**	25-jan-94 (andre)
**	    removed duplicate definition of treeclean()
**	08-mar-94 (anitap)
**	    Rick has added another dbproc for referential constraint. Fix for
**	    the same in ckintegs().
**      03-feb-95 (chech02) 
**          After 'select count(*) into :cnt from table' stmt, We need to  
**          check cnt also.
**	 2-feb-1996 (nick)
**	    X-ref dependency check for events was querying 'iidbevent' ; this
**	    is a typo and should be 'iievent'.  The query condition was broken
**	    too ( though it never got that far ! ). #74424
**	05-apr-1996 (canor01)
**	    after freeing duve_cb->duvestat, set it to NULL to prevent it
**	    from being freed again later.
**	23-may-96 (nick)
**	    X-ref dependency check for synonyms broken. #76673
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	24-Jul-2002 (inifa01)
**	    Bug 106821.  In ckintegs() do not verify existence of an index in 
**	    Test 10 for constraint where base table is index.
**	09-Apr-2003 (inifa01) bug 110019 INGSRV 2204
**	    in ckkey() don't return error if dealing with referential constraint
**	    column.
**      03-may-2004 (stial01)
**          Select int4(tid) (since tid is now i8) (b112259)
**          (Temporary fix until we add support for i8 host variable)
**      04-may-2004 (stial01)
**          Fixed up references to tid host variables. (b112259, b112266)
**	29-Dec-2004 (schka24)
**	    Remove iixprotect.
**	19-jan-2005 (abbjo03)
**	    Change duvecb to GLOBALREF.
**/
/*
**  Forward and/or External typedef/struct references.
*/
	GLOBALREF DUVE_CB duvecb;
/*
**  Forward and/or External function references.
*/
FUNC_EXTERN DU_STATUS ckcomments();	  /* check/repair iidbms_comments */
FUNC_EXTERN DU_STATUS ckdbdep();          /* check/repair iidbdepends */
FUNC_EXTERN DU_STATUS ckdefault();	  /* check/repair iidefault */
FUNC_EXTERN DU_STATUS ckevent();          /* check/repair iievent */
FUNC_EXTERN DU_STATUS ckgw06att();	  /* check/repair iigw06_attribute */
FUNC_EXTERN DU_STATUS ckgw06rel();	  /* check/repair iigw06_relation */
FUNC_EXTERN DU_STATUS ckhist();           /* check/repair iihistogram */
FUNC_EXTERN DU_STATUS ckintegs();         /* check/repair iiintegrity */
FUNC_EXTERN DU_STATUS ckkey();		  /* check/repair iikey */
FUNC_EXTERN DU_STATUS ckproc();           /* check/repair iiprocedure */
FUNC_EXTERN DU_STATUS ckprotups();        /* check/repair iiprotect */
FUNC_EXTERN DU_STATUS ckqrytxt();         /* check iiqrytext */
FUNC_EXTERN DU_STATUS ckrule();           /* check/repair iirule */
FUNC_EXTERN DU_STATUS ckstat();           /* check/repair iistatistic */
FUNC_EXTERN DU_STATUS cksynonym();        /* check/repair iisynonym */
FUNC_EXTERN DU_STATUS cktree();           /* check/repair iitree */
FUNC_EXTERN DU_STATUS ckxdbdep();         /* check/repair iixdbdepends */
FUNC_EXTERN DU_STATUS duve_d_dpndfind();  /* find dependent object info element 
					  ** in IIDBDEPENDS cache */
FUNC_EXTERN DU_STATUS duve_i_dpndfind();  /* find independent object info
                                          ** element in IIDBDEPENDS cache */
FUNC_EXTERN void      duve_obj_type();    /* produce a string describing object
                                          ** type based on object type code */
FUNC_EXTERN DU_STATUS cksecalarm();       /* check/repair iisecalarm */
FUNC_EXTERN VOID treeclean();             /* clean up after drop of tree tuple*/
/* Statistics gathering/reporting utility routines for Debug mode */
FUNC_EXTERN VOID get_stats(i4 *bio, i4 *dio, i4 *ms);
FUNC_EXTERN VOID printstats(DUVE_CB *duve_cb, i4  test_level, char *id);
FUNC_EXTERN VOID init_stats(DUVE_CB *duve_cb, i4  test_level);
/* local functions */
static DU_STATUS create_cons();		  /* cache iintegrity info to drop and
					  ** add create constraint
					  ** for ckintegs()
					  */
static DU_STATUS alloc_mem();		  /* allocate memory and cache query 
					  ** text for creating constraint.
					  */
static VOID
duve_check_privs(
		 DB_TAB_ID	*objid, 
		 i4		privneeded, 
		 char		*privgrantee, 
		 u_i4		*attrmap, 
		 bool		*satisfied);
/*
**  LOCAL CONSTANTS
*/
#define        CMDBUFSIZE      512
/*{
** Name: ckcomments - Perform checks/fixes on iidbms_comments system catalog
**
** Description:
**      this routine performs system catalog tests on iidbms_comment
**	system catalog.  The specific tests are:
** (by comtabbase, comtabidx, comattid, text_sesquence in ascending order)
** 1.      Verify that the catalog receiving the comment really exists
** TEST:   find a tuple in iirelation where
**               iirelation.reltid = iidbms_comments.comtabbase and
**               iirelation.reltidx = iidbms_comments.comtabidx
** FIX:    delete the tuple from iidbms_comments
**
** 2.    Verify that the catalog receiving the comment knows there are comments
**       on it.
** TEST: for the iirelation tuple found above, verify that the TCB_COMMENT bit
**       of iirelation.relstat is set.
** FIX:  Set iirelation.relstat.TCB_COMMENT for this tuple.
**
** 3.    Verify internal integrity of the comment type
** TEST: If iidbms_comments.comattid is zero, verify that bit DBC_T_TABLE
**       is set and bit DBC_T_COLUMN is not set in iidbms_comments.comtype.
**       If iidbms_comments.comattid is nonzero, verify that bit DBC_T_COLUMN
**       is set and bit DBC_T_TABLE s not set in iidbms_comments.comtype.
** FIX:  Delete this tuple from iidbms_comments and clear TCB_COMMENT bit in
**       iirelation.relstat
**
** 4.    Verify that the attribute receiving the comment really exists if this
**       is a column comment (as oppossed to a table comment)
** TEST: if iidbms_comments.comattid is nonzero, verify that it is less than or
**       equal to iirelation.relatts.
** FIX:  Delete this tuple from iidbms_comments and clear TCB_COMMENT bit in
**       iirelation.relstat.
**
** 5.    If this comment is a long remark that spands more than 1 tuple, verify
**       that the sequence numbers are correct.
** TEST: Collect all tuples for this iidbms_comments.comtabbase,
**       iidbms_comments.comtabidx and iidbms_comments.comattid, ordering by
**       iidbms_comments.text_sequence.  Verify that text_sequence starts with
**       zero and ordinally increases (i.e., 0, then 1, then 2, etc).
** FIX:  Delete this tuple from iidbms_comments and clear TCB_COMMENT bit in
**
** Inputs:
**      duve_cb                         verifydb control block
**	    duverel                          cached iirelation information
**
** Outputs:
**      duve_cb                         verifydb control block
**	    duverel                         cached iirelation information
**		du_stat				relstat bits may be set/cleared
**	        du_tbldrop			flag indicating to drop table
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    Table or Column Comments may be dropped from database
**	    iirelation tuples may be modified
**
** History:
**      18-sep-93 (teresa)
**          initial creation
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
[@history_template@]...
*/
DU_STATUS
ckcomments(duve_cb)
DUVE_CB		*duve_cb;
{
# include <duvecmnt.h>
  u_i4 tid, tidx;
  u_i4 basetid, basetidx;
  i4 id;
  i4 relstat;
  i4 cnt;
    DB_TAB_ID	cmnt_id;
    i2		cmnt_att;
    i4		base = DUVE_DROP;
    i4	seq=0;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);
/* # line 382 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"cmnt_cursor",6232,1769);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select * from iidbms_comment order by comtabbase, comtabidx, comattid\
, text_sequence");
    IIcsQuery((char *)"cmnt_cursor",6232,1769);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 384 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iidbms_comments */
    for (cmnt_id.db_tab_base= -1,cmnt_id.db_tab_index= -1, cmnt_att = -1;;)
    {
	/* get tuple from table */
/* # line 388 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"cmnt_cursor",6232,1769,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(cmnt_tbl).comtabbase);
      IIcsGetio((short *)0,1,30,4,&(cmnt_tbl).comtabidx);
      IIcsGetio((short *)0,1,30,2,&(cmnt_tbl).comattid);
      IIcsGetio((short *)0,1,30,2,&(cmnt_tbl).comtype);
      IIcsGetio((short *)0,1,32,60,(cmnt_tbl).short_remark);
      IIcsGetio((short *)0,1,30,2,&(cmnt_tbl).text_sequence);
      IIcsGetio((short *)0,1,32,1600,(cmnt_tbl).long_remark);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 389 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in table */
	{
/* # line 391 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"cmnt_cursor",6232,1769);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 392 "duvechk2.sc" */	/* host code */
	    break;
	}
	if ( (cmnt_tbl.comtabbase != cmnt_id.db_tab_base) ||
	     (cmnt_tbl.comtabidx != cmnt_id.db_tab_index) ||
	     (cmnt_att != cmnt_tbl.comattid) )
	{	
	    /* test 1 - verify base table really exists */
	    cmnt_id.db_tab_base = cmnt_tbl.comtabbase;
	    cmnt_id.db_tab_index  = cmnt_tbl.comtabidx;
	    seq = 0;
	    base = duve_findreltid ( &cmnt_id, duve_cb);
	    if (base == DUVE_DROP)
		continue;
	    if (base == DU_INVALID)
	    {
		if (duve_banner( DUVE_IIDBMS_COMMENT, 1, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk( DUVE_MODESENS, duve_cb, S_DU169E_MISSING_COM_BASE, 4,
			   sizeof(cmnt_id.db_tab_base), &cmnt_id.db_tab_base,
			   sizeof(cmnt_id.db_tab_index), &cmnt_id.db_tab_index);
		if (duve_talk( DUVE_ASK, duve_cb, S_DU034F_DROP_IIDBMS_COMMENT,
			   4, sizeof(cmnt_tbl.comtabbase), &cmnt_tbl.comtabbase,
			   sizeof(cmnt_tbl.comtabidx), &cmnt_tbl.comtabidx)
		== DUVE_YES)
		{
/* # line 420 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbms_comment where comtabbase=");
    IIputdomio((short *)0,1,30,4,&cmnt_tbl.comtabbase);
    IIwritio(0,(short *)0,1,32,0,(char *)"and comtabidx=");
    IIputdomio((short *)0,1,30,4,&cmnt_tbl.comtabidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 422 "duvechk2.sc" */	/* host code */
		    duve_talk( DUVE_MODESENS, duve_cb,
			      S_DU039F_DROP_IIDBMS_COMMENT, 4,
			      sizeof(cmnt_tbl.comtabbase), &cmnt_tbl.comtabbase,
			      sizeof(cmnt_tbl.comtabidx), &cmnt_tbl.comtabidx);
		}
		continue;
	    }
	    /* test 2 - verify base knows it has comments */
	    if ( (duve_cb->duverel->rel[base].du_stat & TCB_COMMENT) == 0 )
	    {
		if (duve_banner( DUVE_IIDBMS_COMMENT, 2, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS)DUVE_BAD);
		duve_talk(DUVE_MODESENS, duve_cb, S_DU169F_NO_COMMENT_RELSTAT,4,
			  0,duve_cb->duverel->rel[base].du_tab,
			  0,duve_cb->duverel->rel[base].du_own);
		if ( duve_talk( DUVE_ASK, duve_cb, S_DU0240_SET_COMMENT, 0)
		    == DUVE_YES)
		{
		    relstat = duve_cb->duverel->rel[base].du_stat | TCB_COMMENT;
		    duve_cb->duverel->rel[base].du_stat = relstat;
		    basetid =  duve_cb->duverel->rel[base].du_id.db_tab_base;
		    basetidx =  duve_cb->duverel->rel[base].du_id.db_tab_index;
/* # line 447 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&relstat);
    IIwritio(0,(short *)0,1,32,0,(char *)"where reltid=");
    IIputdomio((short *)0,1,30,4,&basetid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&basetidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 449 "duvechk2.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS, duve_cb, S_DU0440_SET_COMMENT, 0);
		}
	    } /* end test 2 */
	    /* test 3 - verify internal integrity of the comment type */
	    if ( ( (cmnt_tbl.comattid )
		   &&
		   ( (cmnt_tbl.comtype & DBC_T_TABLE) ||
		     ((cmnt_tbl.comtype & DBC_T_COLUMN)==0)
		   )
		 )
		 ||
		 ( (cmnt_tbl.comattid==0 )
		    &&
		    ( ((cmnt_tbl.comtype & DBC_T_TABLE)==0) ||
		      (cmnt_tbl.comtype & DBC_T_COLUMN)
		    )
		 )
	       )	   
	    {
		if (duve_banner( DUVE_IIDBMS_COMMENT, 3, duve_cb) == DUVE_BAD)
		    return ( (DU_STATUS) DUVE_BAD);
		if (cmnt_tbl.comattid)
		    duve_talk(	DUVE_MODESENS, duve_cb,
				S_DU16A0_BAD_COL_COMMENT, 6,
				sizeof(cmnt_tbl.comattid), &cmnt_tbl.comattid,
				0,duve_cb->duverel->rel[base].du_tab,
				0,duve_cb->duverel->rel[base].du_own);
		else
		    duve_talk(	DUVE_MODESENS, duve_cb,
				S_DU16A1_BAD_TBL_COMMENT, 4,
				0,duve_cb->duverel->rel[base].du_tab,
				0,duve_cb->duverel->rel[base].du_own);
		if (duve_talk( DUVE_ASK, duve_cb, S_DU0241_DROP_THIS_COMMENT,0)
		== DUVE_YES)
		{
/* # line 488 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbms_comment where comtabbase=");
    IIputdomio((short *)0,1,30,4,&cmnt_tbl.comtabbase);
    IIwritio(0,(short *)0,1,32,0,(char *)"and comtabidx=");
    IIputdomio((short *)0,1,30,4,&cmnt_tbl.comtabidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and comattid=");
    IIputdomio((short *)0,1,30,2,&cmnt_tbl.comattid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 491 "duvechk2.sc" */	/* host code */
		    duve_talk( DUVE_MODESENS, duve_cb,
			      S_DU0441_DROP_THIS_COMMENT, 0);
		}
		continue;
	    } /* end test 3 */
	    /* test 4 - verify col id is valid if its a column comment */
	    if (duve_cb->duverel->rel[base].du_attno < cmnt_tbl.comattid)
	    {
		if (duve_banner( DUVE_IIDBMS_COMMENT, 4, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS)DUVE_BAD);
		if (cmnt_tbl.comattid)
		    duve_talk(	DUVE_MODESENS, duve_cb,
				S_DU16A0_BAD_COL_COMMENT, 6,
				sizeof(cmnt_tbl.comattid), &cmnt_tbl.comattid,
				0,duve_cb->duverel->rel[base].du_tab,
				0,duve_cb->duverel->rel[base].du_own);
		else
		    duve_talk(	DUVE_MODESENS, duve_cb,
				S_DU16A1_BAD_TBL_COMMENT, 4,
				0,duve_cb->duverel->rel[base].du_tab,
				0,duve_cb->duverel->rel[base].du_own);
		if (duve_talk( DUVE_ASK, duve_cb, S_DU0241_DROP_THIS_COMMENT, 0)
		== DUVE_YES)
		{
/* # line 519 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbms_comment where comtabbase=");
    IIputdomio((short *)0,1,30,4,&cmnt_tbl.comtabbase);
    IIwritio(0,(short *)0,1,32,0,(char *)"and comtabidx=");
    IIputdomio((short *)0,1,30,4,&cmnt_tbl.comtabidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and comattid=");
    IIputdomio((short *)0,1,30,2,&cmnt_tbl.comattid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 522 "duvechk2.sc" */	/* host code */
		    duve_talk( DUVE_MODESENS, duve_cb,
			      S_DU0441_DROP_THIS_COMMENT, 0);
		}
		continue;
	    } /* end test 4 */
	} /* endif this is the first time we've seen this col #, table id */
	/* test 5 - verify sequence number is correct */
	if ( cmnt_tbl.text_sequence == seq )
	    seq++;			/* all is well */
	else if (cmnt_tbl.text_sequence < seq)
	{
	    /* duplicate dbms comment sequence tuple */
	    if (duve_banner( DUVE_IIDBMS_COMMENT, 5, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU16A2_COMMENT_DUP_ERR, 8,
			0, duve_cb->duverel->rel[base].du_tab,
			0, duve_cb->duverel->rel[base].du_own,
			sizeof(cmnt_tbl.comattid), &cmnt_tbl.comattid,
			sizeof(seq), &seq);
	    if (duve_talk( DUVE_ASK, duve_cb, S_DU0241_DROP_THIS_COMMENT, 0)
	    == DUVE_YES)
	    {
/* # line 547 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbms_comment where comtabbase=");
    IIputdomio((short *)0,1,30,4,&cmnt_tbl.comtabbase);
    IIwritio(0,(short *)0,1,32,0,(char *)"and comtabidx=");
    IIputdomio((short *)0,1,30,4,&cmnt_tbl.comtabidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 549 "duvechk2.sc" */	/* host code */
		duve_talk( DUVE_MODESENS, duve_cb,S_DU0441_DROP_THIS_COMMENT,0);
	    }
	    else
		seq = cmnt_tbl.text_sequence + 1;
	}
	else
	{
	    /* missing dbms comments sequence tuple */
	    if (duve_banner( DUVE_IIDBMS_COMMENT, 5, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU16A3_COMMENT_SEQ_ERR, 8,
			0, duve_cb->duverel->rel[base].du_tab,
			0, duve_cb->duverel->rel[base].du_own,
			sizeof(cmnt_tbl.comattid), &cmnt_tbl.comattid,
			sizeof(seq), &seq);
	    if (duve_talk( DUVE_ASK, duve_cb, S_DU0241_DROP_THIS_COMMENT, 0)
	    == DUVE_YES)
	    {
/* # line 568 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iidbms_comment where comtabbase=");
    IIputdomio((short *)0,1,30,4,&cmnt_tbl.comtabbase);
    IIwritio(0,(short *)0,1,32,0,(char *)"and comtabidx=");
    IIputdomio((short *)0,1,30,4,&cmnt_tbl.comtabidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 570 "duvechk2.sc" */	/* host code */
		duve_talk( DUVE_MODESENS, duve_cb,S_DU0441_DROP_THIS_COMMENT,0);
	    }
	    else
		seq = cmnt_tbl.text_sequence + 1;
	}
    } /* end loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckcomments");
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckdbdep - Perform system catalog checks/fixes on iidbdepends catalog
**
** Description:
**      this routine performs system catalog tests on iidbdpends 
**	system catalog, caching information that will be useful to tests
**	on iixdbdepends to enhance performance by reducing disk IOs.
**
**      It allocates the memory for the cache based on the number of tuples
**	currently in the iidbdepends table.  (This memory is deallocated by
**	duve_close when closing the database.) 
**
** Inputs:
**      duve_cb                         verifydb control block
**	    duverel                          cached iirelation information
**
** Outputs:
**      duve_cb                         verifydb control block
**	    duverel                         cached iirelation information
**	        du_tbldrop			flag indicating to drop table
**	    duvedpnds			    cached iidbdepends information
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    views/permits may be dropped from database
**	    iirelation tuples may be modified
**
** History:
**      05-sep-88 (teg)
**          initial creation
**	31-oct-90 (teresa)
**	    change to stop doing iidbdepends checks if either deid or inid
**	    do not really exist.
**	2-nov-90 (teresa)
**	    fix bug where test 57 set TCB_VBASE instead of TCB_VIEW.
**	06-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**	16-jun-93 (anitap)
**	    Added test for FIPS constraints project. Added switch statement
**	    to include dependent objects like rules and procedures.
**	14-jul-93 (teresa)
**	    Fixed FIPS constraints tests for procedures and rules.  Cannot
**	    select count(deid1) from iirule and iiprocedure catalogs, as those
**	    catalogs do not contain column deid1.
**	06-dec-93 (andre)
**	    IIDBDEPENDS cache will contain both dependent and independent 
**	    object info (with independent object info list hanging off a 
**	    dependent object info element); before checking for existence of a 
**	    given dependent or independent object (by querying a catalog), we 
**	    will check whether the description of the object has been added to 
**	    the IIDBDEPENDS cache 
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
**	15-dec-93 (andre)
**	    iidbdepends cache will represent even those tuples which may have 
**	    been deleted because the independent (or dependent) object mentioned
**	    in them does not exist.
**	11-jan-94 (andre)
**	    added code to handle unexpected dependent/independent object types
**	 2-feb-1996 (nick)
**	    query to check if a reference db event exists was hosed ( in two
**	    places ). #74424
**	23-may-96 (nick)
**	    The query to check synonyms was hosed also ... #76673
**      07-Jun-2005 (inifa01) b111211 INGSRV2580
**          Verifydb reports error; S_DU1619_NO_VIEW iirelation indicates that
**          there is a view defined for table a (owner ingres), but none exists,
**          when view(s) on a table are dropped.
**          Were making DMT_ALTER call on DMT_C_VIEW, to turn on iirelation.relstat
**          TCB_VBASE bit, indicating view is defined over table(s).  TCB_VBASE
**          not tested anywhere in 2.6 and not turned off when view is dropped.
**          qeu_cview() call to dmt_alter() to set bit has been removed ,
**          removeing references to TCB_VBASE here.
[@history_template@]...
*/
DU_STATUS
ckdbdep(duve_cb)
DUVE_CB		*duve_cb;
{
    /*
    ** independent and dependent may contain the following values:
    **  DUVE_BAD_TYPE	- tuple contained unexpected dependent/independent 
    **			  object type;  this means that either ckdbdpnds() needs
    **			  to be updated or that a catalog has been corrupted
    **	DUVE_DROP	- object is marked to be dropped
    **	DU_INVALID	- object does not exist
    **	DU_NOTKNOWN	- "dependent" will be set to this value if dependent
    **			  object D exists and is not marked for destruction and 
    **			  either D's description was found in the dependent 
    **			  object info list or D is not a view or index 
    **			  "independent" will be set to this value if the 
    **			  independent object I exists and is not marked for 
    **			  destruction and the dependent object is not a view and
    **			  either I's description was found in the independent 
    **			  object info list or I is not a table, view, or index
    **  non-negative #	- offset into duverel for a table/view/index
    */
    i4		independent;
    i4		dependent;
    u_i4		size;
    STATUS		mem_stat;
    bool		tuple_deleted;
# include <duvedpnd.h>
  u_i4 tid, tidx;
  u_i4 basetid, basetidx;
  i4 type, id;
  i4 relstat;
  char obj_name[DB_MAXNAME + 1];
  char sch_name[DB_MAXNAME + 1];
  i4 cnt;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);
/* # line 712 "duvechk2.sc" */	/* host code */
    /* 
    ** allocate memory to hold IIDBDEPENDS cache
    */
/* # line 715 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(inid1)from iidbdepends");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 716 "duvechk2.sc" */	/* host code */
    if ( (sqlca.sqlcode == 100) || (cnt == 0))
    {	
	/* its unreasonable to have an empty iidbdepends table, as the system
	** catalogs include some views 
	*/
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU5022_EMPTY_IIDBDEPENDS, 0);
	return ( (DU_STATUS) DUVE_BAD);
    }
    size = cnt * sizeof(DU_I_DPNDS);
    duve_cb->duvedpnds.duve_indep = 
	(DU_I_DPNDS *) MEreqmem((u_i4) 0, size, TRUE, &mem_stat);
    if (mem_stat != OK || duve_cb->duvedpnds.duve_indep == NULL)
    {
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	return ( (DU_STATUS) DUVE_BAD);
    }
    duve_cb->duvedpnds.duve_indep_cnt = 0;
    /* 
    ** number of dependent objects described by iidbdepends will most likely 
    ** be lower than the total number of rows - to avoid allocating excessive 
    ** amounts of memory, we will separately compute number of distinct 
    ** dependent objects described in IIDBDEPENDS
    */
/* # line 742 "duvechk2.sc" */	/* declare */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"declare global temporary table session.dbdep as select distinct deid1\
, deid2, dtype, qid from iidbdepends on commit preserve rows with nore\
covery");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 746 "duvechk2.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&cnt,8);
  }
/* # line 748 "duvechk2.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop session.dbdep");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 750 "duvechk2.sc" */	/* host code */
    size = cnt * sizeof(DU_D_DPNDS);
    duve_cb->duvedpnds.duve_dep = 
	(DU_D_DPNDS *) MEreqmem((u_i4) 0, size, TRUE, &mem_stat);
    if (mem_stat != OK || duve_cb->duvedpnds.duve_dep == NULL)
    {
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	return ( (DU_STATUS) DUVE_BAD);
    }
    duve_cb->duvedpnds.duve_dep_cnt = 0;
/* # line 762 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"dpnd_cursor",6233,6645);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iidbdepends");
    IIcsQuery((char *)"dpnd_cursor",6233,6645);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 764 "duvechk2.sc" */	/* host code */
    /*
    ** (as of 12/06/93) IIDBDEPENDS is expected to describe the following types 
    ** of dependencies:
    **   - view depends on underlying objects
    **   - dbproc depends on underlying objects (tables/views, dbevents, other 
    **     dbprocs, synonyms)
    **   - permit depends on a table (non-GRANT compat. perms)
    **   - permit depends on a privilege descriptor contained in IIPRIV
    **   - rules created to enforce a constraint depend on that constraint
    **   - dbprocs created to enforce a constraint depend on that 
    **	   constraint
    **   - indices created to enforce a constraint depend on that constraint
    **   - REFERENCES constraints depend on UNIQUE constraints on the 
    **	   <referencing columns>
    **   - REFERENCES constraints depend on REFERENCES privilege on 
    **	   <referenced columns> (if the owner of the <referenced table> is
    **	   different from that of the <referencing table>)
    */
    /* loop for each tuple in iidbdepends */
    for (;;)  
    {
	DU_D_DPNDS	*dep;
	DU_I_DPNDS	*indep;
	DU_STATUS	dep_offset;
	DU_STATUS	indep_offset;
	/* get tuple from table */
/* # line 792 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"dpnd_cursor",6233,6645,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(dpnd_tbl).inid1);
      IIcsGetio((short *)0,1,30,4,&(dpnd_tbl).inid2);
      IIcsGetio((short *)0,1,30,4,&(dpnd_tbl).itype);
      IIcsGetio((short *)0,1,30,4,&(dpnd_tbl).i_qid);
      IIcsGetio((short *)0,1,30,4,&(dpnd_tbl).qid);
      IIcsGetio((short *)0,1,30,4,&(dpnd_tbl).deid1);
      IIcsGetio((short *)0,1,30,4,&(dpnd_tbl).deid2);
      IIcsGetio((short *)0,1,30,4,&(dpnd_tbl).dtype);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 793 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in table */
	{
/* # line 795 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"dpnd_cursor",6233,6645);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 796 "duvechk2.sc" */	/* host code */
	    break;
	}
	tuple_deleted = FALSE;
	/* 
	** check whether we have already created a dependent object info 
	** element for this object;  if so, we will not populate another 
	** dependent object info element and can skip checking whether it 
	** exists, but will have to populate a new independent object info 
	** element and link it into the list associated with this dependent 
	** object; otherwise, we will create new dependent and independent 
	** object info elements and link them together + we will have to check 
	** whether the dependent object exists
	*/
	dep_offset = duve_d_dpndfind(dpnd_tbl.deid1, dpnd_tbl.deid2, 
	    dpnd_tbl.dtype, dpnd_tbl.qid, duve_cb);
	/*
	** if a dependent object info element for this object is yet to be 
	** created, we will check if the independent object info list contains 
	** its description;  we will NOT do it for views and indices since 
	** duverel contains descriptions of all views and indices and we might 
	** as well try to look it up there; for all other objects, a binary 
	** search through independent object info list may be far more 
	** efficient than a query
	*/
	if (   dep_offset == DU_INVALID
	    && dpnd_tbl.dtype != DB_VIEW
	    && dpnd_tbl.dtype != DB_INDEX)
	{
	    indep_offset = duve_i_dpndfind(dpnd_tbl.deid1, dpnd_tbl.deid2, 
		dpnd_tbl.dtype, dpnd_tbl.qid, duve_cb, (bool) FALSE);
	}
	else
	{
	    /* 
	    ** we haven't even tried to look for independent object - set 
	    ** indep_offset to DU_INVALID
	    */
	    indep_offset = DU_INVALID;
	}
	if (dep_offset != DU_INVALID)
	{
	    dep = duve_cb->duvedpnds.duve_dep + dep_offset;
	    /*
	    ** if this dependent object has been marked for destruction, 
	    **     set dependent to DUVE_DROP (this will result in us asking 
	    **     user whether he wants the tuple deleted, checking whether 
	    **	   the independent object exists and skipping remaining tests 
	    **	   on this tuple)
	    ** else if we have determined that the dependent object does not 
	    **         exist
	    **	   set dependent to DU_INVALID (this will result in an error 
	    **	   message and asking user whether he wants the tuple deleted)
	    ** else if the dependent object type was unexpected
	    **	   set dependent to DUVE_BAD_TYPE (this will result in an error
	    **	   message and asking user whether he wants the tuple deleted)
	    ** otherwise
	    **	   set "dependent" to DU_NOTKNOWN to indicate that the 
	    **	   description of dependent object was found in IIDBDEPENDS 
	    **	   cache; we will link the new independent object info element 
	    **	   (once it is populated) into the list associated with this 
	    **	   dependent object; 
	    */
	    if (dep->du_dflags & DU_DEP_OBJ_WILL_BE_DROPPED)
	    {
		dependent = DUVE_DROP;
	    }
	    else if (dep->du_dflags & DU_NONEXISTENT_DEP_OBJ)
	    {
		dependent = DU_INVALID;
	    }
	    else if (dep->du_dflags & DU_UNEXPECTED_DEP_TYPE)
	    {
		dependent = DUVE_BAD_TYPE;
	    }
	    else
	    {
		/*
		** set dependent to DU_NOTKNOWN to indicate that we looked up
		** a description of dependent object in IIDBDEPENDS cache
		*/
		dependent = DU_NOTKNOWN;
	    }
	}
	else
	{
	    /* populate a new dependent object info element */
	    dep = 
		duve_cb->duvedpnds.duve_dep + duve_cb->duvedpnds.duve_dep_cnt++;
	    dep->du_deid.db_tab_base  = dpnd_tbl.deid1;
	    dep->du_deid.db_tab_index = dpnd_tbl.deid2;
	    dep->du_dtype             = dpnd_tbl.dtype;
	    dep->du_dqid              = dpnd_tbl.qid;
	    dep->du_dflags 	      =	0;
	    if (indep_offset != DU_INVALID)
	    {
	        DU_I_DPNDS	    *existing_indep;
	        /*
	        ** if the existing independent object info element indicates 
		**        that this dependent object has been marked for 
		**        destruction, 
	        **     set dependent to DUVE_DROP (this will result in us asking
	        **     user whether he wants the tuple deleted, checking whether
	        **     the independent object exists and skipping remaining 
		**     tests on this tuple)
	        ** else if the existing independent object info element 
		**        indicates that this dependent object does not exist
	        **     set dependent to DU_INVALID (this will result in an 
		**     error message and asking user whether he wants the 
		**     tuple deleted)
	        ** otherwise
		**     if the dependent object type is one of those which we 
		**             expect
	        **         set "dependent" to DU_NOTKNOWN to indicate that the 
	        **         description of dependent object was found in 
		**	   IIDBDEPENDS cache; we will link the new independent 
		**	   object info element (once it is populated) into the 
		**	   list associated with this dependent object; 
		**     else
	        **	   set dependent to DUVE_BAD_TYPE (this will result in 
		**         an error message and asking user whether he wants 
		**	   the tuple deleted)
	        */
	        existing_indep = duve_cb->duvedpnds.duve_indep + indep_offset;
	        if (existing_indep->du_iflags & DU_INDEP_OBJ_WILL_BE_DROPPED)
	        {
		    dependent = DUVE_DROP;
	        }
	        else if (existing_indep->du_iflags & DU_NONEXISTENT_INDEP_OBJ)
	        {
		    dependent = DU_INVALID;
	        }
		else
		{
		    switch (existing_indep->du_itype)
		    {
			case DB_VIEW:
			case DB_INDEX:
			case DB_DBP:
			case DB_RULE:
			case DB_PROT:
			case DB_CONS:
			    /*
			    ** set dependent to DU_NOTKNOWN to indicate that we
			    ** looked up a description of dependent object in 
			    ** IIDBDEPENDS cache
			    */
			    dependent = DU_NOTKNOWN;
			    break;
			default:
			    dependent = DUVE_BAD_TYPE;
			    break;
		    }
		}
	    }
	    else
	    {
	        /* test 1 - verify dependent object really exists */
	        switch(dpnd_tbl.dtype)
                {
                    case DB_VIEW:
                    case DB_INDEX:
                        dependent = duve_findreltid(&dep->du_deid, duve_cb);
                        break;
                    case DB_DBP:
/* # line 972 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk21",6234,4470);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid1);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid2);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk21",6234,4470);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(dbp_id)from iiprocedure where dbp_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and dbp_idx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid2);
        IIexDefine(0,(char *)"duvechk21",6234,4470);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 978 "duvechk2.sc" */	/* host code */
		       dependent = (cnt == NULL) ? DU_INVALID : DU_NOTKNOWN;
                       break;
                    case DB_RULE:
/* # line 982 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk22",6235,8166);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid1);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid2);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk22",6235,8166);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(rule_id1)from iirule where rule_id1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and rule_id2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid2);
        IIexDefine(0,(char *)"duvechk22",6235,8166);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 988 "duvechk2.sc" */	/* host code */
		       dependent = (cnt == NULL) ? DU_INVALID : DU_NOTKNOWN;
	               break;
		    case DB_PROT:
/* # line 992 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk23",6236,5607);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid1);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid2);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.qid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk23",6236,5607);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(1)from iiprotect where protabbase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and protabidx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid2);
        IIwritio(0,(short *)0,1,32,0,(char *)"and propermid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.qid);
        IIexDefine(0,(char *)"duvechk23",6236,5607);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 999 "duvechk2.sc" */	/* host code */
		        dependent = (cnt == NULL) ? DU_INVALID : DU_NOTKNOWN;
		        break;
		    case DB_CONS:
/* # line 1003 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk24",6237,27637);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid1);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid2);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.qid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk24",6237,27637);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(1)from iiintegrity where inttabbase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid2);
        IIwritio(0,(short *)0,1,32,0,(char *)"and intnumber=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.qid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and consflags!=0");
        IIexDefine(0,(char *)"duvechk24",6237,27637);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1011 "duvechk2.sc" */	/* host code */
		        dependent = (cnt == NULL) ? DU_INVALID : DU_NOTKNOWN;
		        break;
	            default:
		         /* 
		         ** this means that either ckdbdep() code is out of 
			 ** date or the catalog got corrupted - we'll give 
			 ** ourselves a benefit of a doubt and blaim it on the 
			 ** catalog
		         */
		         dependent = DUVE_BAD_TYPE;
		         break;
                }
	    }
	}
        if (dependent == DUVE_DROP)
        {
	    /* 
	    ** dependent object has been marked for destruction - mark the 
	    ** dependent object info element accordingly
	    */
	    dep->du_dflags |= DU_DEP_OBJ_WILL_BE_DROPPED;
	}
        else if (dependent == DU_INVALID)
        {
	    char	obj_type[50];
	    i4		type_len;
	    /* 
	    ** dependent object does not exist - mark the dependent object info
	    ** element accordingly
	    */
	    dep->du_dflags |= DU_NONEXISTENT_DEP_OBJ;
            if (duve_banner( DUVE_IIDBDEPENDS, 1, duve_cb) == DUVE_BAD) 
	        return ( (DU_STATUS) DUVE_BAD);
	    duve_obj_type((i4) dpnd_tbl.dtype, obj_type, &type_len);
            duve_talk(DUVE_MODESENS, duve_cb, S_DU16C0_NONEXISTENT_DEP_OBJ, 10,
		       type_len, 	       obj_type,
		       sizeof(dpnd_tbl.deid1), &dpnd_tbl.deid1,
	               sizeof(dpnd_tbl.deid2), &dpnd_tbl.deid2,
		       sizeof(dpnd_tbl.dtype), &dpnd_tbl.dtype,
		       sizeof(dpnd_tbl.qid),   &dpnd_tbl.qid);
            if (duve_talk(DUVE_ASK, duve_cb, 
	                   S_DU0322_DROP_IIDBDEPENDS_TUPLE, 8, 
	                   sizeof(dpnd_tbl.inid1), &dpnd_tbl.inid1,
	                   sizeof(dpnd_tbl.inid2), &dpnd_tbl.inid2,
	                   sizeof(dpnd_tbl.deid1), &dpnd_tbl.deid1,
	                   sizeof(dpnd_tbl.deid2), &dpnd_tbl.deid2)
                == DUVE_YES)
            {
/* # line 1065 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iidbdepends",(char *)"dpnd_cursor",6233,6645);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1066 "duvechk2.sc" */	/* host code */
	        duve_talk( DUVE_MODESENS, duve_cb, 
		        S_DU0372_DROP_IIDBDEPENDS_TUPLE, 8, 
		        sizeof(dpnd_tbl.inid1), &dpnd_tbl.inid1,
		        sizeof(dpnd_tbl.inid2), &dpnd_tbl.inid2,
		        sizeof(dpnd_tbl.deid1), &dpnd_tbl.deid1,
		        sizeof(dpnd_tbl.deid2), &dpnd_tbl.deid2);
		/* remember that we have already deleted the current tuple */
		tuple_deleted = TRUE;
            }
        }
        else if (dependent == DUVE_BAD_TYPE)
        {
	    /* 
	    ** dependent object type found in the tuple was unexpected - mark 
	    ** the dependent object info element accordingly
	    */
	    dep->du_dflags |= DU_UNEXPECTED_DEP_TYPE;
            if (duve_banner( DUVE_IIDBDEPENDS, 1, duve_cb) == DUVE_BAD) 
	        return ( (DU_STATUS) DUVE_BAD);
            duve_talk(DUVE_MODESENS, duve_cb, 
		S_DU16C2_UNEXPECTED_DEP_OBJ_TYPE, 8,
		sizeof(dpnd_tbl.dtype), &dpnd_tbl.dtype,
		sizeof(dpnd_tbl.deid1), &dpnd_tbl.deid1,
	        sizeof(dpnd_tbl.deid2), &dpnd_tbl.deid2,
		sizeof(dpnd_tbl.qid),   &dpnd_tbl.qid);
            if (duve_talk(DUVE_ASK, duve_cb, 
	                   S_DU0322_DROP_IIDBDEPENDS_TUPLE, 8, 
	                   sizeof(dpnd_tbl.inid1), &dpnd_tbl.inid1,
	                   sizeof(dpnd_tbl.inid2), &dpnd_tbl.inid2,
	                   sizeof(dpnd_tbl.deid1), &dpnd_tbl.deid1,
	                   sizeof(dpnd_tbl.deid2), &dpnd_tbl.deid2)
                == DUVE_YES)
            {
/* # line 1102 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iidbdepends",(char *)"dpnd_cursor",6233,6645);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1103 "duvechk2.sc" */	/* host code */
	        duve_talk( DUVE_MODESENS, duve_cb, 
		        S_DU0372_DROP_IIDBDEPENDS_TUPLE, 8, 
		        sizeof(dpnd_tbl.inid1), &dpnd_tbl.inid1,
		        sizeof(dpnd_tbl.inid2), &dpnd_tbl.inid2,
		        sizeof(dpnd_tbl.deid1), &dpnd_tbl.deid1,
		        sizeof(dpnd_tbl.deid2), &dpnd_tbl.deid2);
		/* remember that we have already deleted the current tuple */
		tuple_deleted = TRUE;
            }
        }
	/*
	** populate the next independent element (note that we will not 
	** increament duve_indep_cnt until we are done looking up a description
	** of independent object in the independent object info list - this 
	** ensures that we don't find this new element while searching for an 
	** existing element)
	*/
	indep = 
	    duve_cb->duvedpnds.duve_indep + duve_cb->duvedpnds.duve_indep_cnt;
	indep->du_inid.db_tab_base  = dpnd_tbl.inid1;
	indep->du_inid.db_tab_index = dpnd_tbl.inid2;
	indep->du_itype             = dpnd_tbl.itype;
	indep->du_iqid              = dpnd_tbl.i_qid;
	indep->du_iflags	    = 0;
	/* 
	** if we had to populate a new dependent object info element, link new 
	** dependent and independent object info elements; otherwise, link new 
	** independent element into a list associated with the existing 
	** dependent element
	*/
	if (dep_offset == DU_INVALID)
	{
	    dep->du_indep = indep;
	    indep->du_iflags |= DU_LAST_INDEP;
	    indep->du_next.du_dep = dep;
	}
	else
	{
	    indep->du_next.du_inext = dep->du_indep;
	    dep->du_indep = indep;
	}
	/* test 2 - verify independent object really exists */
	/*
	** if the dependent object is a view, we will look up independent object
	** description in duverel because test 3 expects "independent" to be an 
	** offset into IIRELATION cache;
	** otherwise, we will attempt to locate existing description of the 
	** independent object in the independent object info list
	** if that fails we will try to locate existing description of the
	** independent object in the dependent object info list unless the
	** independent object is a table, view, or index in which case a binary
	** search of duverel will be much more efficient
	*/
	if (dpnd_tbl.dtype == DB_VIEW)   
	{
	    indep_offset = dep_offset = DU_INVALID;
	}
	else
	{
	    /*
	    ** First try to look up the object description in the independent
	    ** object info list 
	    */
	    indep_offset = duve_i_dpndfind(dpnd_tbl.inid1, dpnd_tbl.inid2,
		dpnd_tbl.itype, dpnd_tbl.i_qid, duve_cb, (bool) FALSE);
	    if (indep_offset != DU_INVALID)
	    {
		DU_I_DPNDS          *existing_indep;
		/*
                ** if the existing independent object info element indicates
                **    that this independent object has been marked for
                **    destruction,
                **        set independent to DUVE_DROP (this will result in us
                **        skipping remaining tests on this tuple)
                ** else if the existing independent object info element
                **         indicates that this independent object does not exist
                **    set independent to DU_INVALID (this will result in an
                **    error message and asking user whether he wants the
                **    tuple deleted)
	        ** else if the independent object type was unexpected
	        **     set independent to DUVE_BAD_TYPE (this will result in an
		**     error message and asking user whether he wants the tuple
		**     deleted)
		** otherwise
		**    set independent to DU_NOTKNOWN to indicate that we  
		**    looked up a description of independent object in 
		**    IIDBDEPENDS cache
		*/
                existing_indep = duve_cb->duvedpnds.duve_indep + indep_offset;
                if (existing_indep->du_iflags & DU_INDEP_OBJ_WILL_BE_DROPPED)
                {
                    independent = DUVE_DROP;
                }
                else if (existing_indep->du_iflags & DU_NONEXISTENT_INDEP_OBJ)
                {
                    independent = DU_INVALID;
                }
	        else if (existing_indep->du_iflags & DU_UNEXPECTED_INDEP_TYPE)
	        {
		    independent = DUVE_BAD_TYPE;
	        }
	        else
	        {
		    /*
		    ** set independent to DU_NOTKNOWN to indicate that we 
		    ** looked up a description of independent object in 
		    ** IIDBDEPENDS cache
		    */
		    independent = DU_NOTKNOWN;
	        }
		/* 
		** set dep_offset to DU_INVALID to indicate that we haven't 
		** even tried to look up description of this independent object 
		** in the dependent object info list
		*/
		dep_offset = DU_INVALID;
	    }
	    else if (   dpnd_tbl.itype == DB_TABLE
	             || dpnd_tbl.itype == DB_VIEW
	             || dpnd_tbl.itype == DB_INDEX)
	    {
		/* 
		** binary search of duverel is more efficient than a scan of the
		** dependent object info list - don't bother calling 
		** duve_d_dpndfind() if independent object is a table, view, or 
		** index
		*/
		dep_offset = DU_INVALID;
	    }
	    else
	    {
		dep_offset = duve_d_dpndfind(dpnd_tbl.inid1, dpnd_tbl.inid2,
		    dpnd_tbl.itype, dpnd_tbl.i_qid, duve_cb);
		if (dep_offset != DU_INVALID)
		{
		    DU_D_DPNDS		*existing_dep;
                    /*
                    ** if the existing dependent object info element indicates
                    **    that this independent object has been marked for
                    **    destruction,
                    **        set independent to DUVE_DROP (this will result in
		    **	      us skipping remaining tests on this tuple)
                    ** else if the existing dependent object info element
                    **    indicates that this independent object does not exist
                    **        set independent to DU_INVALID (this will result in
		    **	      an error message and asking user whether he wants 
		    **	      the tuple deleted)
		    ** otherwise
		    **     if the independent object type is one of those which 
		    **             we expect
	            **         set "independent" to DU_NOTKNOWN to indicate that
		    **	       the description of independent object was found 
		    **         in IIDBDEPENDS cache
		    **     else
	            **	       set independent to DUVE_BAD_TYPE (this will 
		    **	       result in an error message and asking user 
		    **         whether he wants the tuple deleted)
                    */
                    existing_dep = duve_cb->duvedpnds.duve_dep + dep_offset;
                    if (existing_dep->du_dflags & DU_DEP_OBJ_WILL_BE_DROPPED)
                    {
                        independent = DUVE_DROP;
                    }
                    else if (existing_dep->du_dflags & DU_NONEXISTENT_DEP_OBJ)
                    {
                        independent = DU_INVALID;
                    }
	            else
	            {
			switch (existing_dep->du_dtype)
			{
			    case DB_TABLE:
			    case DB_VIEW:
			    case DB_INDEX:
			    case DB_EVENT:
			    case DB_SYNONYM:
			    case DB_PRIV_DESCR:
			    case DB_DBP:
			    case DB_CONS:
		                /*
		                ** set independent to DU_NOTKNOWN to indicate 
				** that we looked up a description of 
				** independent object in IIDBDEPENDS cache
		                */
		                independent = DU_NOTKNOWN;
				break;
			    default:
				independent = DUVE_BAD_TYPE;
				break;
			}
	            }
		}
	    }
	}
	/*
	** if we haven't found a description of the independent object in the 
	** IIDBDEPENDS cache, try to look elsewhere
	*/
	if (dep_offset == DU_INVALID && indep_offset == DU_INVALID)
	{
            switch(dpnd_tbl.itype)
            {
	        case DB_TABLE:
                case DB_VIEW:
                case DB_INDEX:
                    independent = duve_findreltid(&indep->du_inid, duve_cb);
                    break;
	        case DB_EVENT:
/* # line 1329 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk25",6238,2274);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid1);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid2);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk25",6238,2274);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(1)from iievent where event_idbase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and event_idx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid2);
        IIexDefine(0,(char *)"duvechk25",6238,2274);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1335 "duvechk2.sc" */	/* host code */
		    independent = (cnt == NULL) ? DU_INVALID : DU_NOTKNOWN;
		    break;
	        case DB_SYNONYM:
/* # line 1339 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk26",6239,2008);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid1);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid2);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk26",6239,2008);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(1)from iisynonym where synid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and synidx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid2);
        IIexDefine(0,(char *)"duvechk26",6239,2008);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1345 "duvechk2.sc" */	/* host code */
		    independent = (cnt == NULL) ? DU_INVALID : DU_NOTKNOWN;
		    break;
	        case DB_PRIV_DESCR:
/* # line 1349 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk27",6240,7516);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid1);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid2);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.i_qid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk27",6240,7516);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(1)from iipriv where d_obj_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and d_obj_idx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid2);
        IIwritio(0,(short *)0,1,32,0,(char *)"and d_priv_number=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.i_qid);
        IIexDefine(0,(char *)"duvechk27",6240,7516);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1356 "duvechk2.sc" */	/* host code */
		    independent = (cnt == NULL) ? DU_INVALID : DU_NOTKNOWN;
		    break;
                case DB_DBP:
/* # line 1360 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk28",6241,1006);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid1);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid2);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk28",6241,1006);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(1)from iiprocedure where dbp_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and dbp_idx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid2);
        IIexDefine(0,(char *)"duvechk28",6241,1006);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1366 "duvechk2.sc" */	/* host code */
		    independent = (cnt == NULL) ? DU_INVALID : DU_NOTKNOWN;
                    break;
	        case DB_CONS:
/* # line 1370 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk29",6242,492);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid1);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid2);
      IIputdomio((short *)0,1,30,4,&dpnd_tbl.i_qid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk29",6242,492);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(1)from iiintegrity where inttabbase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.inid2);
        IIwritio(0,(short *)0,1,32,0,(char *)"and intnumber=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&dpnd_tbl.i_qid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and consflags!=0");
        IIexDefine(0,(char *)"duvechk29",6242,492);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1378 "duvechk2.sc" */	/* host code */
		    independent = (cnt == NULL) ? DU_INVALID : DU_NOTKNOWN;
		    break;
	        default:
		    /* 
		    ** this means that either ckdbdep() code is out of 
		    ** date or the catalog got corrupted - we'll give 
		    ** ourselves a benefit of a doubt and blaim it on the 
		    ** catalog
		    */
		    independent = DUVE_BAD_TYPE;
		    break;
            }
	}
	/* 
	** we have populated the new independent object info element, but have 
	** not incremented duve_cb->duvedpnds.duve_indep_cnt to make sure that 
	** duve_i_dpndfind() does not accidently "find" it; now, however, is a 
	** good time to update duve_cb->duvedpnds.duve_indep_cnt
	*/
	duve_cb->duvedpnds.duve_indep_cnt++;
	if (independent == DUVE_DROP)
	{
	    /*
	    ** independent object has been marked for destruction - mark the
	    ** independent object info element accordingly + propagate this 
	    ** information into the dependent object info element
	    */
	    indep->du_iflags |= DU_INDEP_OBJ_WILL_BE_DROPPED;
	    dep->du_dflags |= DU_MISSING_INDEP_OBJ;
	}
	else if (independent == DU_INVALID)
	{
	    char	obj_type[50];
	    i4		type_len;
	    /*
	    ** independent object does not exist - mark the independent object 
	    ** info element accordingly + propagate this information into the
	    ** dependent object info element
	    */
	    indep->du_iflags |= DU_NONEXISTENT_INDEP_OBJ;
	    dep->du_dflags |= DU_MISSING_INDEP_OBJ;
	    if (duve_banner( DUVE_IIDBDEPENDS, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_obj_type((i4) dpnd_tbl.itype, obj_type, &type_len);
            duve_talk(DUVE_MODESENS, duve_cb, S_DU16C1_NONEXISTENT_INDEP_OBJ, 
		       10,
		       type_len, 	       obj_type,
		       sizeof(dpnd_tbl.inid1), &dpnd_tbl.inid1,
	               sizeof(dpnd_tbl.inid2), &dpnd_tbl.inid2,
		       sizeof(dpnd_tbl.itype), &dpnd_tbl.itype,
		       sizeof(dpnd_tbl.i_qid), &dpnd_tbl.i_qid);
	    /* 
	    ** delete tuple unless it was already deleted because the dependent 
	    ** object does not exist
	    */
	    if (!tuple_deleted)
	    {
	        if (duve_talk( DUVE_ASK, duve_cb, 
			S_DU0322_DROP_IIDBDEPENDS_TUPLE, 8, 
			sizeof(dpnd_tbl.inid1), &dpnd_tbl.inid1,
		        sizeof(dpnd_tbl.inid2), &dpnd_tbl.inid2,
		        sizeof(dpnd_tbl.deid1), &dpnd_tbl.deid1,
		        sizeof(dpnd_tbl.deid2), &dpnd_tbl.deid2)
	        == DUVE_YES)
	        {
/* # line 1453 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iidbdepends",(char *)"dpnd_cursor",6233,6645);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1454 "duvechk2.sc" */	/* host code */
		    duve_talk( DUVE_MODESENS, duve_cb, 
			    S_DU0372_DROP_IIDBDEPENDS_TUPLE, 8, 
			    sizeof(dpnd_tbl.inid1), &dpnd_tbl.inid1,
			    sizeof(dpnd_tbl.inid2), &dpnd_tbl.inid2,
			    sizeof(dpnd_tbl.deid1), &dpnd_tbl.deid1,
			    sizeof(dpnd_tbl.deid2), &dpnd_tbl.deid2);
		    /* remember that we deleted the current tuple */
		    tuple_deleted = TRUE;
	        }
	    }
	}
	else if (independent == DUVE_BAD_TYPE)
	{
	    /*
	    ** independent object type found in this tuple was unexpected - mark
	    ** the independent object info element accordingly + propagate this
	    ** information into the dependent object info element
	    */
	    indep->du_iflags |= DU_UNEXPECTED_INDEP_TYPE;
	    dep->du_dflags |= DU_MISSING_INDEP_OBJ;
	    if (duve_banner( DUVE_IIDBDEPENDS, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
            duve_talk(DUVE_MODESENS, duve_cb, 
		S_DU16C3_UNEXPECTED_IND_OBJ_TYPE, 8,
		   sizeof(dpnd_tbl.itype), &dpnd_tbl.itype,
		   sizeof(dpnd_tbl.inid1), &dpnd_tbl.inid1,
	           sizeof(dpnd_tbl.inid2), &dpnd_tbl.inid2,
		   sizeof(dpnd_tbl.i_qid), &dpnd_tbl.i_qid);
	    /* 
	    ** delete tuple unless it was already deleted because the dependent 
	    ** object does not exist or was of unexpected type
	    */
	    if (!tuple_deleted)
	    {
	        if (duve_talk( DUVE_ASK, duve_cb, 
			S_DU0322_DROP_IIDBDEPENDS_TUPLE, 8, 
			sizeof(dpnd_tbl.inid1), &dpnd_tbl.inid1,
		        sizeof(dpnd_tbl.inid2), &dpnd_tbl.inid2,
		        sizeof(dpnd_tbl.deid1), &dpnd_tbl.deid1,
		        sizeof(dpnd_tbl.deid2), &dpnd_tbl.deid2)
	        == DUVE_YES)
	        {
/* # line 1501 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iidbdepends",(char *)"dpnd_cursor",6233,6645);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1502 "duvechk2.sc" */	/* host code */
		    duve_talk( DUVE_MODESENS, duve_cb, 
			    S_DU0372_DROP_IIDBDEPENDS_TUPLE, 8, 
			    sizeof(dpnd_tbl.inid1), &dpnd_tbl.inid1,
			    sizeof(dpnd_tbl.inid2), &dpnd_tbl.inid2,
			    sizeof(dpnd_tbl.deid1), &dpnd_tbl.deid1,
			    sizeof(dpnd_tbl.deid2), &dpnd_tbl.deid2);
		    /* remember that we deleted the current tuple */
		    tuple_deleted = TRUE;
	        }
	    }
	}
	/* 
	** skip the remaining tests if the dependent object does not exist, 
	** is slated to be dropped or is of unexpected type
	*/
	if (dep->du_dflags & 
		(  DU_DEP_OBJ_WILL_BE_DROPPED 
		 | DU_NONEXISTENT_DEP_OBJ
		 | DU_UNEXPECTED_DEP_TYPE))
	{
	    /* 
	    ** in the past we used to skip test 2 as well, but since we decided 
	    ** that IIDBDEPENDS will represent all IIDBDEPENDS tuples, I wanted
	    ** to perform test 2 in case the independent object did not exist
	    ** or was marked for destruction
	    */
	    continue;
	}
	/*
	** if the independent object does not exist, is marked for destruction,
	** or is of unexpected type, we will skip the remaining tests.  
	** If the dependent object is a view or an index, set du_tbldrop in 
	** duverel entry corresponding to it to DUVE_DROP to indicate that it 
	** should be dropped; otherwise add an entry to the "fixit" list 
	** describing the object to be dropped (given the time, I would modify 
	** code marking tables, views, and indices for destruction to also add 
	** the description to the "fixit" list and have that list be the only 
	** source of information about objects to be dropped - as it is I will 
	** settle for using duverel as a source of info about 
	** tables/views/indices to be dropped and the "fixit" list as a source 
	** of info on all other objects to be dropped)
	*/
	if (dep->du_dflags & DU_MISSING_INDEP_OBJ)
	{
	    /* 
	    ** if we got here, dependent object exists and has not yet been
	    ** marked for destruction
	    */
	    if (   dpnd_tbl.dtype == DB_VIEW 
		|| dpnd_tbl.dtype == DB_INDEX
		|| dpnd_tbl.dtype == DB_PROT)
	    {
		i4		tbl_offset;
	        /*
	        ** note that if IIDBDEPENDS indicates that a permit depends on a
	        ** privilege descriptor and the privilege descriptor is no 
		** longer there, rather than trying to drop the permit and 
		** launch the full-blown REVOKE processing, we will attempt to 
		** drop the object (a view) on which the permit was defined
	        */
		/*
		** if we looked up its description in the IIDBDEPENDS cache,
		** look it up in IIRELATION cache first
		*/
		if (dependent == DU_NOTKNOWN)
		{
		    tbl_offset = duve_findreltid(&dep->du_deid, duve_cb);
		}
		else
		{
		    tbl_offset = dependent;
		}
		if (tbl_offset == DU_INVALID || tbl_offset == DUVE_DROP)
		{
		    /* 
		    ** tbl_offset may get set to DU_INVALID if a view on which a
		    ** permit was defined no longer exists - this condition 
		    ** will be checked in ckprot();
		    ** tbl_offset may get set to DUVE_DROP if a view on which a
		    ** permit was defined was has been marked for destruction
		    */
		    continue;
		}
		if (dpnd_tbl.dtype == DB_PROT)
		{
		     duve_talk(DUVE_MODESENS, duve_cb, 
			 S_DU16C5_MISSING_PERM_INDEP_OBJ, 6,
			 sizeof(dpnd_tbl.qid), &dpnd_tbl.qid,
			 0, duve_cb->duverel->rel[tbl_offset].du_own,
			 0, duve_cb->duverel->rel[tbl_offset].du_tab);
		}
		else
		{
		    char    obj_type[50];
		    i4      type_len;
		    duve_obj_type((i4) dpnd_tbl.dtype, obj_type, &type_len);
		    duve_talk(DUVE_MODESENS, duve_cb,
			S_DU16C4_MISSING_INDEP_OBJ, 6,
			type_len, obj_type,
			0, duve_cb->duverel->rel[tbl_offset].du_own,
			0, duve_cb->duverel->rel[tbl_offset].du_tab);
		}
		if (duve_talk( DUVE_IO, duve_cb, S_DU0302_DROP_TABLE, 4,
			0, duve_cb->duverel->rel[tbl_offset].du_tab,
			0, duve_cb->duverel->rel[tbl_offset].du_own)
		    == DUVE_YES)
		{
		    idxchk(duve_cb, dpnd_tbl.deid1, dpnd_tbl.deid2);
		    duve_cb->duverel->rel[tbl_offset].du_tbldrop = DUVE_DROP;
		    duve_talk( DUVE_MODESENS, duve_cb,
		        S_DU0352_DROP_TABLE, 4,
		        0, duve_cb->duverel->rel[tbl_offset].du_tab,
		        0, duve_cb->duverel->rel[tbl_offset].du_own);
		    dep->du_dflags |= DU_DEP_OBJ_WILL_BE_DROPPED;
		}
	    }
	    else if (dpnd_tbl.dtype == DB_RULE) 
	    { 
		i4         tbl_offset;
		DB_TAB_ID	rule_tab_id;
/* # line 1634 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_name, rule_owner, rule_tabbase, rule_tabidx from iirule w\
here rule_id1=");
    IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_id2=");
    IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,obj_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,sch_name);
      IIgetdomio((short *)0,1,30,4,&tid);
      IIgetdomio((short *)0,1,30,4,&tidx);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1641 "duvechk2.sc" */	/* host code */
		rule_tab_id.db_tab_base = tid;
		rule_tab_id.db_tab_index = tidx;
		tbl_offset = duve_findreltid(&rule_tab_id, duve_cb);
		if (tbl_offset == DU_INVALID || tbl_offset == DUVE_DROP)
		{
		    /* 
		    ** tbl_offset may get set to DU_INVALID if a table on which
		    ** a rule was defined no longer exists - this condition 
		    ** will be checked in ckrule();
		    ** tbl_offset may get set to DUVE_DROP if a table on which a
		    ** rule was defined was has been marked for destruction
		    */
		    continue;
		}
		duve_talk(DUVE_MODESENS, duve_cb,
		    S_DU16C4_MISSING_INDEP_OBJ, 6,
		    sizeof("RULE") - 1, "RULE", 0, sch_name, 0, obj_name);
		if (duve_talk(DUVE_IO, duve_cb, S_DU0345_DROP_FROM_IIRULE, 4,
			0, obj_name, 0, sch_name)
		    == DUVE_YES)
		{
		    duve_talk(DUVE_MODESENS, duve_cb,
		        S_DU0395_DROP_FROM_IIRULE, 4,
		        0, obj_name, 0, sch_name);
		    dep->du_dflags |= DU_DEP_OBJ_WILL_BE_DROPPED;
		}
	    }
	    else if (dpnd_tbl.dtype == DB_DBP)
	    { 
/* # line 1675 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbp_name, dbp_owner from iiprocedure where dbp_id=");
    IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dbp_idx=");
    IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,obj_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,sch_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1682 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb,
		    S_DU16C4_MISSING_INDEP_OBJ, 6,
		    sizeof("PROCEDURE") - 1, "PROCEDURE", 
		    0, sch_name, 0, obj_name);
		if (duve_talk(DUVE_IO, duve_cb, S_DU0251_MARK_DBP_DORMANT, 0)
		    == DUVE_YES)
		{
		    duve_talk(DUVE_MODESENS, duve_cb, 
			S_DU0451_MARKED_DBP_DORMANT, 0);
		    dep->du_dflags |= DU_DEP_OBJ_WILL_BE_DROPPED;
		}
	    }
	    else if (dpnd_tbl.dtype == DB_CONS)
	    { 
		i4         tbl_offset;
		DB_TAB_ID	cons_tab_id;
/* # line 1701 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select consname from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&dpnd_tbl.deid2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and intnumber=");
    IIputdomio((short *)0,1,30,4,&dpnd_tbl.qid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and consflags!=0");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,obj_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1709 "duvechk2.sc" */	/* host code */
		cons_tab_id.db_tab_base = dpnd_tbl.deid1;
		cons_tab_id.db_tab_index = dpnd_tbl.deid2;
		tbl_offset = duve_findreltid(&cons_tab_id, duve_cb);
		if (tbl_offset == DU_INVALID || tbl_offset == DUVE_DROP)
		{
		    /* 
		    ** tbl_offset may get set to DU_INVALID if a table on which
		    ** a constraint was defined no longer exists - this 
		    ** condition will be checked in ckinteg();
		    ** tbl_offset may get set to DUVE_DROP if a table on which a
		    ** constraint was defined was has been marked for 
		    ** destruction
		    */
		    continue;
		}
		duve_talk(DUVE_MODESENS, duve_cb,
		    S_DU16C4_MISSING_INDEP_OBJ, 6,
		    sizeof("CONSTRAINT") - 1, "CONSTRAINT", 
		    0, duve_cb->duverel->rel[tbl_offset].du_own, 
		    0, obj_name);
		if (duve_talk(DUVE_IO, duve_cb, S_DU0247_DROP_CONSTRAINT, 4,
			0, duve_cb->duverel->rel[tbl_offset].du_own,
			0, obj_name)
		    == DUVE_YES)
		{
		    duve_talk(DUVE_MODESENS, duve_cb,
		        S_DU0447_DROP_CONSTRAINT, 4,
		        0, duve_cb->duverel->rel[tbl_offset].du_own,
			0, obj_name);
		    dep->du_dflags |= DU_DEP_OBJ_WILL_BE_DROPPED;
		}
	    }
	    if (   dep->du_dflags & DU_DEP_OBJ_WILL_BE_DROPPED
	        && dpnd_tbl.dtype != DB_VIEW 
		&& dpnd_tbl.dtype != DB_INDEX
		&& dpnd_tbl.dtype != DB_PROT)
	    {
		DUVE_DROP_OBJ		*drop_obj;
		/* 
		** user decided to drop the object - add its description to the 
		** "fixit" list (we don't do it if the independent object was 
		** a view, index, or permit because in those cases we use the 
		** iirelation cache to store that information)
		*/
	        drop_obj = (DUVE_DROP_OBJ *) MEreqmem((u_i4) 0, 
		    sizeof(DUVE_DROP_OBJ), TRUE, &mem_stat);
	        if (mem_stat != OK || drop_obj == NULL)
	        {
		    duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
		    return ( (DU_STATUS) DUVE_BAD);
	        }
		drop_obj->duve_obj_id.db_tab_base  = dpnd_tbl.deid1;
		drop_obj->duve_obj_id.db_tab_index = dpnd_tbl.deid2;
		drop_obj->duve_obj_type            = dpnd_tbl.dtype;
		drop_obj->duve_secondary_id	   = dpnd_tbl.qid;
		drop_obj->duve_drop_flags	   = 0;
		/* link this description into the existing list */
		drop_obj->duve_next = duve_cb->duvefixit.duve_objs_to_drop;
		duve_cb->duvefixit.duve_objs_to_drop = drop_obj;
	    }
	    continue;
	}
	/* 
	** a couple of tests if the tuple described dependence of a view on some
	** object used in its definition
	*/
	if (dpnd_tbl.dtype == DB_VIEW)
	{
	    /* test 3 - verify independent table knows its a view base.
	    ** Removed this test.  TCB_VBASE is no longer set when a 
	    ** view is created.  b111211
	    */
	    /* 
	    ** test 4 - verify dependent view is really a view - this test need
	    ** to be run only when we first populate a dependent object info 
	    ** element 
	    */
	    if (   dependent != DU_NOTKNOWN 
	        && (duve_cb->duverel->rel[dependent].du_stat & TCB_VIEW) == 0)
	    {
	        if (duve_banner( DUVE_IIDBDEPENDS, 4, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS)DUVE_BAD);
	        duve_talk(DUVE_MODESENS, duve_cb, S_DU1643_DEPENDENT_NOT_VIEW, 
		    4,
		    0,duve_cb->duverel->rel[dependent].du_tab,
		    0,duve_cb->duverel->rel[dependent].du_own);
	        if (duve_talk( DUVE_IO, duve_cb, S_DU031D_SET_VIEW, 0) 
			== DUVE_YES)
	        {
		    relstat = 
			duve_cb->duverel->rel[dependent].du_stat | TCB_VIEW;
		    basetid = dpnd_tbl.deid1;
		    basetidx = dpnd_tbl.deid2;
		    duve_cb->duverel->rel[dependent].du_stat = relstat;
/* # line 1817 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&relstat);
    IIwritio(0,(short *)0,1,32,0,(char *)"where reltid=");
    IIputdomio((short *)0,1,30,4,&basetid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&basetidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1819 "duvechk2.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS, duve_cb,S_DU036D_SET_VIEW, 0);
		}
	    } /* end test 4 */
	} /* end tests run if the dependent object is a view */
	/* test 5 - verify there is an entry for this table in index table */
	id =	dpnd_tbl.qid;
	type =	dpnd_tbl.dtype;
	tid =	dpnd_tbl.deid1;
	tidx =	dpnd_tbl.deid2;
/* # line 1831 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk210",6243,29026);
      IIputdomio((short *)0,1,30,4,&tid);
      IIputdomio((short *)0,1,30,4,&tidx);
      IIputdomio((short *)0,1,30,4,&type);
      IIputdomio((short *)0,1,30,4,&id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk210",6243,29026);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(deid1)from iixdbdepends where deid1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&tid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and deid2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&tidx);
        IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&type);
        IIwritio(0,(short *)0,1,32,0,(char *)"and qid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"duvechk210",6243,29026);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1834 "duvechk2.sc" */	/* host code */
	if (cnt==0)
	{  /* The entry for iidbdepends is missing from iixdbdepends */
	    if (duve_banner( DUVE_IIDBDEPENDS, 5, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS)DUVE_BAD);
	    duve_talk( DUVE_MODESENS, duve_cb, S_DU1644_MISSING_IIXDBDEPENDS, 8,
		       0, duve_cb->duverel->rel[independent].du_tab,
		       0, duve_cb->duverel->rel[independent].du_own,
		       0, duve_cb->duverel->rel[dependent].du_tab,
		       0, duve_cb->duverel->rel[dependent].du_own);
	    if ( duve_talk( DUVE_ASK, duve_cb, S_DU0324_CREATE_IIXDBDEPENDS, 0)
	       == DUVE_YES )
	    {
		tid  = dpnd_tbl.deid1;
		tidx = dpnd_tbl.deid2;
		type = dpnd_tbl.dtype;
		id   = dpnd_tbl.qid;
		/* Note although tid is i8, tidp is still i4 */
/* # line 1852 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select int4(tid)from iidbdepends where deid1=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and deid2=");
    IIputdomio((short *)0,1,30,4,&tidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dtype=");
    IIputdomio((short *)0,1,30,4,&type);
    IIwritio(0,(short *)0,1,32,0,(char *)"and qid=");
    IIputdomio((short *)0,1,30,4,&id);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1855 "duvechk2.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iixdbdepends(deid1, deid2, dtype, qid, tid)values(");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&tidx);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&type);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&id);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&cnt);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1858 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU0374_CREATE_IIXDBDEPENDS,
			  0);
	    }
	} /* end test 5 */
    } /* end loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckdbdep"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckxevent - run verifydb check on iixevent dbms catalog
**
** Description:
**	This function will ensure that IIXEVENT is in sync with IIEVENT.  This 
**	will be accomplished in two steps:
**	- first we will open a cursor to read tuples in iixevent for which 
**	  there are no corresponding tuples in iievent; if one or more such
**	  tuples are found, we will ask for permission to drop and recreate the 
**	  index
**	- then we will open a cursor on iievent to read tuples for which there 
**	  is no corresponding iixevent tuple and insert appropriate iixevent 
**	  tuples
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**	none.
**
** Returns:
**    	DUVE_YES
**      DUVE_BAD
**
** Exceptions:
**	none
**
** Side Effects:
**	tuples may be inserted into / deleted from iixevent
**
** History:
**      19-jan-94 (andre)
**	    written
[@history_template@]...
*/
DU_STATUS
ckxevent(
	DUVE_CB		*duve_cb)
{
# include <duvexev.h>
  u_i4 tid_ptr;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 1930 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"xev_tst_curs_1",6244,28775);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select * from iixevent xev where not exists(select 1 from iievent ev \
where xev.tidp=ev.tid and xev.event_idbase=ev.event_idbase and xev.eve\
nt_idx=ev.event_idx)");
    IIcsQuery((char *)"xev_tst_curs_1",6244,28775);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1932 "duvechk2.sc" */	/* host code */
    /*
    ** test 1: for every IIXEVENT tuple for which there is no corresponding 
    ** IIEVENT tuple ask user whether we may delete the offending IIXEVENT
    ** tuple
    */
    for (;;)
    {
/* # line 1939 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"xev_tst_curs_1",6244,28775,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(xev_tbl).event_idbase);
      IIcsGetio((short *)0,1,30,4,&(xev_tbl).event_idx);
      IIcsGetio((short *)0,1,30,4,&(xev_tbl).tidp);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1940 "duvechk2.sc" */	/* host code */
        if (sqlca.sqlcode == 100) /* then no more tuples to read */
        {
/* # line 1942 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"xev_tst_curs_1",6244,28775);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1943 "duvechk2.sc" */	/* host code */
	    break;
        }
	/* there is no tuple in IIEVENT corresponding to this IIXEVENT tuple */
        if (duve_banner(DUVE_IIXEVENT, 1, duve_cb) == DUVE_BAD) 
	    return ( (DU_STATUS)DUVE_BAD);
        duve_talk( DUVE_MODESENS, duve_cb, S_DU16C9_EXTRA_IIXEVENT, 6,
            sizeof(xev_tbl.event_idbase), &xev_tbl.event_idbase,
	    sizeof(xev_tbl.event_idx), &xev_tbl.event_idx,
	    sizeof(xev_tbl.tidp), &xev_tbl.tidp);
	if (duve_talk( DUVE_ASK, duve_cb, S_DU024A_DELETE_IIXEVENT, 0) 
	    == DUVE_YES)
	{
/* # line 1957 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iixevent",(char *)"xev_tst_curs_1",6244,28775);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1959 "duvechk2.sc" */	/* host code */
	    duve_talk(DUVE_MODESENS,duve_cb, S_DU044A_DELETED_IIXEVENT, 0);
	    break;
	}
    }
    /* end test 1 */
/* # line 1981 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"xev_tst_curs_2",6245,29020);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select ev.*, ev.tid from iievent ev where not exists(select 1 from ii\
xevent xev where xev.tidp=ev.tid and xev.event_idbase=ev.event_idbase \
and xev.event_idx=ev.event_idx and event_name=event_name)");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"xev_tst_curs_2",6245,29020);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1983 "duvechk2.sc" */	/* host code */
    /*
    ** test 2: for every IIEVENT tuple for which there is no corresponding 
    ** IIXEVENT tuple ask user whether we may insert a missing tuple into 
    ** IIXEVENT
    */
    for (;;)
    {
/* # line 1990 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"xev_tst_curs_2",6245,29020,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,(evnt_tbl).event_name);
      IIcsGetio((short *)0,1,32,32,(evnt_tbl).event_owner);
      IIcsGetio((short *)0,1,32,25,(evnt_tbl).event_create);
      IIcsGetio((short *)0,1,30,4,&(evnt_tbl).event_type);
      IIcsGetio((short *)0,1,30,4,&(evnt_tbl).event_idbase);
      IIcsGetio((short *)0,1,30,4,&(evnt_tbl).event_idx);
      IIcsGetio((short *)0,1,30,4,&(evnt_tbl).event_qryid1);
      IIcsGetio((short *)0,1,30,4,&(evnt_tbl).event_qryid2);
      IIcsGetio((short *)0,1,32,8,(evnt_tbl).event_free);
      IIcsGetio((short *)0,1,30,4,&tid_ptr);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1991 "duvechk2.sc" */	/* host code */
        if (sqlca.sqlcode == 100) /* then no more tuples to read */
        {
/* # line 1993 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"xev_tst_curs_2",6245,29020);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 1994 "duvechk2.sc" */	/* host code */
	    break;
        }
	/* there is no tuple in IIXEVENT corresponding to this IIEVENT tuple */
        if (duve_banner(DUVE_IIXEVENT, 2, duve_cb) == DUVE_BAD) 
	    return ( (DU_STATUS)DUVE_BAD);
        duve_talk( DUVE_MODESENS, duve_cb, S_DU16CA_MISSING_IIXEVENT, 6,
	    sizeof(evnt_tbl.event_idbase), &evnt_tbl.event_idbase,
	    sizeof(evnt_tbl.event_idx), &evnt_tbl.event_idx,
	    sizeof(tid_ptr), &tid_ptr);
	if (duve_talk( DUVE_ASK, duve_cb, S_DU024B_INSERT_IIXEVENT_TUPLE, 0) 
	    == DUVE_YES)
	{
/* # line 2008 "duvechk2.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iixevent(event_idbase, event_idx, tidp)values(");
    IIputdomio((short *)0,1,30,4,&evnt_tbl.event_idbase);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&evnt_tbl.event_idx);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&tid_ptr);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2012 "duvechk2.sc" */	/* host code */
	    duve_talk(DUVE_MODESENS, duve_cb, 
		S_DU044B_INSERTED_IIXEVENT_TUPLE, 0);
	}
    }
    /* end test 2 */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckxevent"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckevent - run verifydb tests to check system catalog iievent.
**
** Description:
**      This routine opens a cursor to walk iievent one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iievent system catalog.
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    tuples may be dropped from table iievent.
**
** History:
**      27-may-1993 (teresa)
**          initial creation
**	16-jun-93 (anitap)
**	    Added test for CREATE SCHEMA project.
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
**	20-dec-93 (anitap)
**	    Use repeat query in test 3 and ANY instead of COUNT to improve
**	    performance. Also cache the schema names to be created.
**	07-jan-94 (teresa)
**	    if there are no tuples in iievent, then dont open the cursor.
**	28-jan-94 (andre)
**	    if the dbevent's query text is missing (test #1), rather than 
**	    deleting the IIEVENT tuple, add a record to the "fix it" list 
**	    reminding us to drop description of the dbevent and mark dormant 
**	    any dbprocs that depend on it.
[@history_template@]...
*/
DU_STATUS
ckevent(duve_cb)
DUVE_CB            *duve_cb;
{
    DB_TAB_ID		event_id;
    DU_STATUS		base;
    u_i4		size;
    STATUS		mem_stat;
  i4 cnt;
  i2 mode;
  char schname[DB_MAXNAME + 1];
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 2092 "duvechk2.sc" */	/* host code */
    /* allocate memory to hold add commands */	
/* # line 2093 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(event_name)from iievent");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2095 "duvechk2.sc" */	/* host code */
    duve_cb->duve_ecnt = 0;
    if (cnt == 0)
	return DUVE_YES;
    else	
        size = cnt * sizeof(DU_EVENT);
    duve_cb->duveeves = (DUVE_EVENT *) MEreqmem( (u_i4)0, size, TRUE, 
			&mem_stat);
    if ( (mem_stat != OK) || (duve_cb->duveeves == 0))
    {
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
        return( (DU_STATUS) DUVE_BAD);
    }	
/* # line 2113 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"event_cursor",6246,23259);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iievent");
    IIcsQuery((char *)"event_cursor",6246,23259);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2115 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iievent */
    for (;;)  
    {
	/* get tuple from iievent */
/* # line 2119 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"event_cursor",6246,23259,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,(evnt_tbl).event_name);
      IIcsGetio((short *)0,1,32,32,(evnt_tbl).event_owner);
      IIcsGetio((short *)0,1,32,25,(evnt_tbl).event_create);
      IIcsGetio((short *)0,1,30,4,&(evnt_tbl).event_type);
      IIcsGetio((short *)0,1,30,4,&(evnt_tbl).event_idbase);
      IIcsGetio((short *)0,1,30,4,&(evnt_tbl).event_idx);
      IIcsGetio((short *)0,1,30,4,&(evnt_tbl).event_qryid1);
      IIcsGetio((short *)0,1,30,4,&(evnt_tbl).event_qryid2);
      IIcsGetio((short *)0,1,32,8,(evnt_tbl).event_free);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2120 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in iievent */
	{
/* # line 2122 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"event_cursor",6246,23259);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2123 "duvechk2.sc" */	/* host code */
	    break;
	}
	event_id.db_tab_base = evnt_tbl.event_idbase;
	event_id.db_tab_index = evnt_tbl.event_idx;
	/* test 1 - verify query text used to define event exists */
	mode = DB_EVENT;
/* # line 2132 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk211",6247,20846);
      IIputdomio((short *)0,1,30,4,&evnt_tbl.event_qryid1);
      IIputdomio((short *)0,1,30,4,&evnt_tbl.event_qryid2);
      IIputdomio((short *)0,1,30,2,&mode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk211",6247,20846);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(txtid1)from iiqrytext where txtid1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&evnt_tbl.event_qryid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&evnt_tbl.event_qryid2);
        IIwritio(0,(short *)0,1,32,0,(char *)"and mode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&mode);
        IIexDefine(0,(char *)"duvechk211",6247,20846);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2136 "duvechk2.sc" */	/* host code */
	if (cnt == 0)
	{
	    /* missing query text that defined dbevent */
            if (duve_banner( DUVE_IIEVENT, 1, duve_cb)
	        == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU167E_NO_TXT_FOR_EVENT, 4,
			0, evnt_tbl.event_name, 0, evnt_tbl.event_owner);
	    if ( duve_talk(DUVE_IO, duve_cb, S_DU0343_DROP_EVENT, 0)
	    == DUVE_YES)
	    {
		DUVE_DROP_OBJ	*drop_obj;
		drop_obj = (DUVE_DROP_OBJ *) MEreqmem((u_i4) 0, 
		    sizeof(DUVE_DROP_OBJ), TRUE, &mem_stat);
		if (mem_stat != OK || drop_obj == NULL)
		{
		    duve_talk (DUVE_ALWAYS, duve_cb, 
			E_DU3400_BAD_MEM_ALLOC, 0);
		    return ((DU_STATUS) DUVE_BAD);
		}
		/*
		** add a record to the "fix it" list reminding us to drop this 
		** dbevent 
		*/
		drop_obj->duve_obj_id.db_tab_base  = evnt_tbl.event_idbase;
		drop_obj->duve_obj_id.db_tab_index = evnt_tbl.event_idx;
		drop_obj->duve_obj_type            = DB_EVENT;
		drop_obj->duve_secondary_id	   = 0;
		drop_obj->duve_drop_flags	   = 0;
		/* link this description into the existing list */
		drop_obj->duve_next = 
		    duve_cb->duvefixit.duve_objs_to_drop;
		duve_cb->duvefixit.duve_objs_to_drop = drop_obj;
		duve_talk(DUVE_MODESENS, duve_cb, S_DU0393_DROP_EVENT, 0);
	    }
	    continue;
	} /* endif event definition does not exist */	    
	/* test 2 -- verify event identifier is unique */
	base = duve_findreltid ( &event_id, duve_cb);
	if (base != DU_INVALID)
	{
	    /* this id is in iirelation, so drop from iievent */
	    if (duve_banner( DUVE_IIEVENT, 2, duve_cb)
	    == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU167F_EVENTID_NOT_UNIQUE, 8,
			0, evnt_tbl.event_name, 0, evnt_tbl.event_owner,
			sizeof(evnt_tbl.event_idbase), &evnt_tbl.event_idbase,
			sizeof(evnt_tbl.event_idx), &evnt_tbl.event_idx);
	    if ( duve_talk(DUVE_IO, duve_cb, S_DU0343_DROP_EVENT, 0)
	    == DUVE_YES)
	    {
/* # line 2197 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&evnt_tbl.event_qryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&evnt_tbl.event_qryid2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2199 "duvechk2.sc" */	/* host code */
		/*
		** FIX_ME_ANDRE - instead of deleting the tuple, we need to
		** either add an entry to a drop'rm list or find all independent
		** descriptors referring to this dbevent and mark them
		** accordingly
		*/
/* # line 2205 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iievent",(char *)"event_cursor",6246,23259);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2206 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU0393_DROP_EVENT, 0);
	    }
	    continue;
	} /* endif identifier is not unique */
	/* test 3 -- verify event belongs to a schema */
        /* first, take 32 char event owner and force into
        ** string DB_MAXNAME char long.
        */
        MEcopy( (PTR)evnt_tbl.event_owner, DB_MAXNAME, (PTR)schname);
        schname[DB_MAXNAME] ='\0';
        (void) STtrmwhite(schname);
        /* now search for iischema tuple for owner match */
/* # line 2222 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk212",6248,18782);
      IIputdomio((short *)0,1,32,0,schname);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk212",6248,18782);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(schema_id)from iischema where schema_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,schname);
        IIexDefine(0,(char *)"duvechk212",6248,18782);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2225 "duvechk2.sc" */	/* host code */
        if (cnt == 0)
        {
           /* corresponding entry missing in iischema */
           if (duve_banner(DUVE_IIEVENT, 3, duve_cb)
                   == DUVE_BAD)
              return ( (DU_STATUS) DUVE_BAD);
           duve_talk (DUVE_MODESENS, duve_cb, S_DU168D_SCHEMA_MISSING, 2,
                       0, evnt_tbl.event_owner);
	   if ( duve_talk(DUVE_IO, duve_cb, S_DU0245_CREATE_SCHEMA, 0)
			== DUVE_YES)
	   {	
              /* cache info about this tuple */
              MEcopy( (PTR)evnt_tbl.event_owner, DB_MAXNAME,
           	(PTR) duve_cb->duveeves->events[duve_cb->duve_ecnt].du_schname);
	      duve_cb->duve_ecnt++;
              duve_talk(DUVE_MODESENS, duve_cb, S_DU0445_CREATE_SCHEMA, 0);
	   }
	   continue;
        } /* end test 3 */
    }  /* end loop for each tuple in table */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckevent"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckgw06att - Perform verifydb checks/repairs on iigw06_attribute
**
** Description:
**	This routine opens a cursor on iigw06_attribute and walks the table,
**	one tuple at a time, performing verifydb checks and repairs on it.
**
**	NOTE: This routine should be run before ckgw06rel() since it may
**	remove tuples that subsequently make a row in iigw06_relation invalid
**	e.g. a table no longer has any attributes. ckgwo6rel() will check for
**	this and delete the row. See also note in ckgw06rel().
**
** Inputs:
**	duve_cb			verifydb control block
**
** Outputs:
**	duve_cb			verifydb control block
**
**	Returns:
**	    DUVE_BAD
**	    DUVE_YES
**	Exceptions:
**	    None.
**
** Side effects:
**	May delete tuples in iigw06_attribute, and iiattribute after prompting
**	user for confirmation.
**
** History:
**	30-jul-93 (stephenb)
**	    Initial creation
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
[@history_template@]
*/
DU_STATUS
ckgw06att (DUVE_CB *duve_cb)
{
# include <duvegwat.h>
  i4 cnt = 0;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);
/* # line 2314 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"gwatt_cursor",6249,29919);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iigw06_attribu\
te");
    IIcsQuery((char *)"gwatt_cursor",6249,29919);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2316 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iigw06_attribute */
    for (;;)
    {
	/* get tuple from iigw06_attribute */
/* # line 2320 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"gwatt_cursor",6249,29919,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(gwatt_tbl).reltid);
      IIcsGetio((short *)0,1,30,4,&(gwatt_tbl).reltidx);
      IIcsGetio((short *)0,1,30,2,&(gwatt_tbl).attid);
      IIcsGetio((short *)0,1,32,31,(gwatt_tbl).attname);
      IIcsGetio((short *)0,1,30,2,&(gwatt_tbl).auditid);
      IIcsGetio((short *)0,1,32,31,(gwatt_tbl).auditname);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2321 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* no more tuples */
	{
/* # line 2323 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"gwatt_cursor",6249,29919);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2324 "duvechk2.sc" */	/* host code */
	    break;
	}
	/*
	** Test 1 -- Find a tuple in iigw06_relation.
	*/
/* # line 2329 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk213",6250,26448);
      IIputdomio((short *)0,1,30,4,&gwatt_tbl.reltid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk213",6250,26448);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(reltid)from iigw06_relation where reltid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&gwatt_tbl.reltid);
        IIexDefine(0,(char *)"duvechk213",6250,26448);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2331 "duvechk2.sc" */	/* host code */
	if (cnt == 0) /* no iigw06_relation */
	{
	    if (duve_banner (DUVE_IIGW06_ATTRIBUTE, 1, duve_cb)
	    == DUVE_BAD)
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU169A_MISSING_GW06REL, 2,
		0, gwatt_tbl.attname);
	    if (duve_talk (DUVE_IO, duve_cb, S_DU034E_DROP_IIGW06_ATTRIBUTE, 0)
	    == DUVE_YES)
	    {
		/*
		** Fisrt delete row in iiattribute, iigw06_attribute
		*/
/* # line 2345 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&gwatt_tbl.reltid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attid=");
    IIputdomio((short *)0,1,30,2,&gwatt_tbl.attid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2348 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iigw06_attribute",(char *)"gwatt_cursor",6249,29919);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2349 "duvechk2.sc" */	/* host code */
		duve_talk (DUVE_MODESENS, duve_cb, 
		    S_DU039E_DROP_IIGW06_ATTRIBUTE, 0);
	    }
	    continue;
	}
	/*
	** Test 2 -- Find a tuple in iiattribute.
	*/
/* # line 2357 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk214",6251,28495);
      IIputdomio((short *)0,1,30,4,&gwatt_tbl.reltid);
      IIputdomio((short *)0,1,30,2,&gwatt_tbl.attid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk214",6251,28495);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(attrelid)from iiattribute where attrelid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&gwatt_tbl.reltid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and attid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,2,&gwatt_tbl.attid);
        IIexDefine(0,(char *)"duvechk214",6251,28495);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2360 "duvechk2.sc" */	/* host code */
	if (cnt == 0) /* no iiattribute */
	{
	    if (duve_banner (DUVE_IIGW06_ATTRIBUTE, 2, duve_cb)
	    == DUVE_BAD)
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU169B_MISSING_IIATTRIBUTE,
		2, 0, gwatt_tbl.attname);
	    if (duve_talk (DUVE_IO, duve_cb, S_DU034E_DROP_IIGW06_ATTRIBUTE, 0)
	    == DUVE_YES)
	    {
		/*
		** delete row from iigw06_attribute
		*/
/* # line 2374 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iigw06_attribute",(char *)"gwatt_cursor",6249,29919);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2375 "duvechk2.sc" */	/* host code */
		duve_talk (DUVE_MODESENS, duve_cb, 
		    S_DU039E_DROP_IIGW06_ATTRIBUTE, 0);
	    }
	    continue;
	}
	/*
	** Test 3 -- check that reltidx=0 (indexes are not supported)
	*/
	if (gwatt_tbl.reltidx != 0)
	{
	    if (duve_banner (DUVE_IIGW06_ATTRIBUTE, 3, duve_cb) == DUVE_BAD)
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk (DUVE_MODESENS, duve_cb, S_DU169C_NONZERO_GW06ATTIDX,
		2, 0, gwatt_tbl.attname);
	    if (duve_talk (DUVE_IO, duve_cb, S_DU034E_DROP_IIGW06_ATTRIBUTE, 0)
	    == DUVE_YES)
	    {
		/*
		** first delete from iiattribute then iigw06_attribute.
		*/
/* # line 2396 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&gwatt_tbl.reltid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&gwatt_tbl.reltidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attid=");
    IIputdomio((short *)0,1,30,2,&gwatt_tbl.attid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2400 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iigw06_attribute",(char *)"gwatt_cursor",6249,29919);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2401 "duvechk2.sc" */	/* host code */
		duve_talk ( DUVE_MODESENS, duve_cb, 
		    S_DU039E_DROP_IIGW06_ATTRIBUTE, 0);
	    }
	} /* end of test 3 */
    } /* end of loop for each row in iigw06_attribute */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckgw06att"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckgw06rel - Perform verifydb checks/repairs on iigw06_relation
** 
** Description:
**	This routine opens a cursor on iigw06_relation and walks the table,
**	one tuple at a time, performing verifydb checks and repairs on it.
**
**	NOTE: the tables iigw06_relation and iigw06_attribute are fully
**	dependent on eachother and on iirelation and iiattribute, i.e. a
**	row in iigw06_relation must have a corresponding row in iirelation
**	and a row in iigw06_attribute must have a corresponding row in
**	iiattribute and in iigw06_relation and in iirelation (by inference)
**	so removing rows from any of these tables means that we need to perform
**	cascading deletes to maintain consistency, we can't always rely on
**	DROP REGISTRATION since the kind of corruption we are checking for
**	may prevent this statement from working properly, so we have to perform
**	all the deletes by hand. Currently tables are joined only on
**	reltid, since reltidx is always zero (there is a check for this)
**	reltid provides a unique key, this should be changed if the
**	code is ever updated to support indexes.
**
** Inputs:
**	duve_cb				verifydb control block
**
** Outputs:
**	duve_cb				verifydb control block
**
**	Returns:
**	    DUVE_BAD
**	    DUVE_YES
**	Exceptions:
**	    None
**
** Side Effects:
**	May delete rows in iirelation, iigw06_relation, iiattribute and 
**	iigw06_attribute after prompting for user confirmation.
**
** History
**	27-jul-93 (stephenb)
**	    Initial creation
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
[@history_template@]
*/
DU_STATUS
ckgw06rel(DUVE_CB *duve_cb)
{
    i4 ctr = 0;
# include <duvegwrl.h>
  i4 cnt = 0;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 2478 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"gwrel_cursor",6252,229);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iigw06_relatio\
n");
    IIcsQuery((char *)"gwrel_cursor",6252,229);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2480 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iigw06_relation */
    for (;;)
    {
	/* get tuple from iigw06_relation */
/* # line 2484 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"gwrel_cursor",6252,229,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(gwrel_tbl).reltid);
      IIcsGetio((short *)0,1,30,4,&(gwrel_tbl).reltidx);
      IIcsGetio((short *)0,1,32,255,(gwrel_tbl).audit_log);
      IIcsGetio((short *)0,1,30,4,&(gwrel_tbl).reg_date);
      IIcsGetio((short *)0,1,30,4,&(gwrel_tbl).flags);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2485 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* no more tuples */
	{
/* # line 2487 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"gwrel_cursor",6252,229);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2488 "duvechk2.sc" */	/* host code */
	    break;
	}
	/*
	** Test 1 -- Verify that a correspondig tuple exists in iirelation
	*/
	/* traverse the array containg iirelation tuples looking for a 
	** value that matches, break out when found
	*/
	for (ctr = 0; ctr < duve_cb->duve_cnt; ctr++)
	{
	    if (duve_cb->duverel->rel[ctr].du_id.db_tab_base 
		== gwrel_tbl.reltid) /* found a tuple that corresponds */
		break;
	}
	if (ctr == duve_cb->duve_cnt) /* no value found */
	{
	    if (duve_banner( DUVE_IIGW06_RELATION, 1, duve_cb) == DUVE_BAD)
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1697_MISSING_RELTUP, 2,
		0, gwrel_tbl.audit_log);
	    if ( duve_talk(DUVE_IO, duve_cb, S_DU034D_DROP_IIGW06_RELATION, 0)
	    == DUVE_YES)
	    {
		/* 
		** first delete rows in iigw06_attribute and then
		** iiattribute and finaly the offendig row in iigw06_relation
		*/
/* # line 2516 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_attribute where reltid=");
    IIputdomio((short *)0,1,30,4,&gwrel_tbl.reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2518 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&gwrel_tbl.reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2520 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iigw06_relation",(char *)"gwrel_cursor",6252,229);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2521 "duvechk2.sc" */	/* host code */
		duve_talk (DUVE_MODESENS, duve_cb, 
		    S_DU039D_DROP_IIGW06_RELATION, 0);
	    }
	    continue;
	}
	/*
	** Test 2 -- Verify there is at least one corresponding attribute
	**           in iigw06_attribute.
	*/
/* # line 2530 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk215",6253,30428);
      IIputdomio((short *)0,1,30,4,&gwrel_tbl.reltid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk215",6253,30428);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(reltid)from iigw06_attribute where reltid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&gwrel_tbl.reltid);
        IIexDefine(0,(char *)"duvechk215",6253,30428);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2532 "duvechk2.sc" */	/* host code */
	if (cnt == 0) /* no iigw06_attribute */
	{
	    if (duve_banner (DUVE_IIGW06_RELATION, 2, duve_cb)
	    == DUVE_BAD)
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1698_MISSING_GW06ATT, 4,
		0, duve_cb->duverel->rel[ctr].du_tab, 0, gwrel_tbl.audit_log);
	    if (duve_talk( DUVE_IO, duve_cb, S_DU034D_DROP_IIGW06_RELATION, 0)
	    == DUVE_YES)
	    {
		/* 
		** First delete from iiattribute, then iirelation and finaly
		** from iigw06_relation
		*/
/* # line 2547 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&gwrel_tbl.reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2549 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&gwrel_tbl.reltid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2551 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iigw06_relation",(char *)"gwrel_cursor",6252,229);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2552 "duvechk2.sc" */	/* host code */
		duve_talk (DUVE_MODESENS, duve_cb, 
		    S_DU039D_DROP_IIGW06_RELATION, 0);
	    }
	    continue;
	}
	/*
	** Test 3 -- Verify that the value in reltidx=0 (we do not support
	** indexes in this gateway
	*/
	if (gwrel_tbl.reltidx != 0)
	{
	    if (duve_banner ( DUVE_IIGW06_RELATION, 3, duve_cb)
	    == DUVE_BAD)
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk (DUVE_MODESENS, duve_cb, S_DU1699_NONZERO_GW06IDX, 4,
		0, duve_cb->duverel->rel[ctr].du_tab, 0, gwrel_tbl.audit_log);
	    if (duve_talk( DUVE_IO, duve_cb, S_DU034D_DROP_IIGW06_RELATION, 0)
	    == DUVE_YES)
	    {
	    /*
	    ** first delete corresponding iigw06_attribue then iiattribute
	    ** then iirelation and finally iigw06_relation
	    */
/* # line 2576 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iigw06_attribute where reltid=");
    IIputdomio((short *)0,1,30,4,&gwrel_tbl.reltid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&gwrel_tbl.reltidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2579 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiattribute where attrelid=");
    IIputdomio((short *)0,1,30,4,&gwrel_tbl.reltid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and attrelidx=");
    IIputdomio((short *)0,1,30,4,&gwrel_tbl.reltidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2582 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirelation where reltid=");
    IIputdomio((short *)0,1,30,4,&gwrel_tbl.reltid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&gwrel_tbl.reltidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2585 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iigw06_relation",(char *)"gwrel_cursor",6252,229);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2586 "duvechk2.sc" */	/* host code */
		duve_talk (DUVE_MODESENS, duve_cb, 
		    S_DU039D_DROP_IIGW06_RELATION, 0);
	    }
	} /* end of test 3 */
    } /* end of loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckgw06rel"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckhist	- perform verifydb checks/repairs on iihistogram
**
** Description:
**      This routine opens a cursor on iihistogram and walks the table,
**	one tuple at a time, performing verifydb checks and repairs on it. 
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**	duve_cb				verifydb control block
**	    duvestat			cache of statistics entries
**		stats[]			statistics cache entry
**
**	Returns:
**	    DUVE_BAD
**	    DUVE_YES
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      nn-nn-nn (teresa)
**          Initial creation
**	15-jul-93 (teresa)
**	    Fix MEfree statement -- was passing address of duve_cb->duvedpnds
**	    instead of just duve_cb->duvedpnds.
**      18-oct-93 (teresa)
**          fix bug 50840 -- was releasing the wrong memory (duve_cb->duvedpnds
**          instead of duve_cb->duvestat).  This caused an AV on UNIX but
**          appeared to slip by on VMS.
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
**	05-apr-1996 (canor01)
**	    after freeing duve_cb->duvestat, set it to NULL to prevent it
**	    from being freed again later.
**
[@history_template@]...
*/
DU_STATUS
ckhist(duve_cb)
DUVE_CB            *duve_cb;
{
    DB_TAB_ID	   histid;
    DU_STATUS	   base,ptr;
    i4	   seq;
    i2		   hist_att;
# include <duvehist.h>
  u_i4 tid, tidx;
  i2 att;
  i2 cells;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 2665 "duvechk2.sc" */	/* host code */
    if ( duve_cb ->duve_scnt == 0)
    {
	/* there were no statistics, so there should be no tuples */
/* # line 2668 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select any(htabbase)from iihistogram");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&att);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2669 "duvechk2.sc" */	/* host code */
	if (att != 0)
	{
	    if (duve_banner( DUVE_IIHISTOGRAM, 1, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk (DUVE_MODESENS, duve_cb, S_DU1651_INVALID_IIHISTOGRAM, 0);
	    if (duve_talk (DUVE_ASK, duve_cb, S_DU032C_EMPTY_IIHISTOGRAM, 0)
	    == DUVE_YES)
	    {
/* # line 2677 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"delete from iihistogram");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2678 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb,S_DU037C_EMPTY_IIHISTOGRAM, 0);
	    }
	}
	return ( (DU_STATUS) DUVE_YES);
    } /* endif iistatistics is empty */
/* # line 2687 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"hist_cursor",6254,4199);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select * from iihistogram order by htabbase, htabindex, hatno, hseque\
nce");
    IIcsQuery((char *)"hist_cursor",6254,4199);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2689 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iihistogram */
    for (histid.db_tab_base= -1,histid.db_tab_index= -1,base= -1;;)
    {
	/* get tuple from table */
/* # line 2693 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"hist_cursor",6254,4199,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(hist_tbl).htabbase);
      IIcsGetio((short *)0,1,30,4,&(hist_tbl).htabindex);
      IIcsGetio((short *)0,1,30,2,&(hist_tbl).hatno);
      IIcsGetio((short *)0,1,30,2,&(hist_tbl).hsequence);
      IIcsGetio((short *)0,1,32,228,(hist_tbl).histogram);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2694 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in table */
	{
/* # line 2696 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"hist_cursor",6254,4199);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2697 "duvechk2.sc" */	/* host code */
	    break;
	}
	/* determine if this is the 1st tuple for this particular histogram
	** entry
	*/
	if ( (hist_tbl.htabbase != histid.db_tab_base) ||
	     (hist_tbl.htabindex != histid.db_tab_index) ||
	     (hist_att != hist_tbl.hatno) )
	{
	    histid.db_tab_base  = hist_tbl.htabbase;
	    histid.db_tab_index = hist_tbl.htabindex;
	    hist_att = hist_tbl.hatno;
	    base = duve_statfind ( &histid, hist_tbl.hatno, &base, duve_cb);
	    seq = 0;
	    /* test 1 - make sure that statistics entry exists that references
	    **		this histogram
	    */
	    if ( base == DU_INVALID)
	    {
		if (duve_banner( DUVE_IIHISTOGRAM, 1, duve_cb) == DUVE_BAD) 
			return ( (DU_STATUS)DUVE_BAD);
		duve_talk( DUVE_MODESENS, duve_cb, 
			    S_DU164A_NO_STATS_FOR_HISTOGRAM, 4,
			    sizeof(histid.db_tab_base), &histid.db_tab_base,
			    sizeof(histid.db_tab_index), &histid.db_tab_index);
		if ( duve_talk (DUVE_ASK, duve_cb, 
			    S_DU0329_DROP_FROM_IIHISTOGRAM, 6,
			    sizeof(histid.db_tab_base), &histid.db_tab_base,
			    sizeof(histid.db_tab_index), &histid.db_tab_index,
			    sizeof(hist_tbl.hatno), &hist_tbl.hatno)
		== DUVE_YES)
		{
/* # line 2735 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iihistogram where htabbase=");
    IIputdomio((short *)0,1,30,4,&hist_tbl.htabbase);
    IIwritio(0,(short *)0,1,32,0,(char *)"and htabindex=");
    IIputdomio((short *)0,1,30,4,&hist_tbl.htabindex);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2737 "duvechk2.sc" */	/* host code */
		    base = DUVE_DROP;
		    duve_talk(DUVE_MODESENS, duve_cb, S_DU0379_DROP_FROM_IIHISTOGRAM,
			    6, sizeof(histid.db_tab_base), &histid.db_tab_base,
			    sizeof(histid.db_tab_index), &histid.db_tab_index,
			    sizeof(hist_tbl.hatno), &hist_tbl.hatno);
		    continue;
		}
	    }	/* end test 1 */
	}   /* end 1st tuple in this histogram entry */
	/* if we are dropping the statistic tuple, dont bother
	** checking the histogram
	*/
	if ( base == DUVE_DROP)
	    continue;
	/* test 2 - verify sequence of multiple tuple histogram entry is valid
	*/
	ptr = duve_cb->duvestat->stats[base].du_rptr;
	if ( hist_tbl.hsequence == seq )
	    seq++;			/* all is well */
	else if (hist_tbl.hsequence < seq)
	{
	    /* duplicate histogram sequence tuple */
            if (duve_banner( DUVE_IIHISTOGRAM, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1652_HISTOGRAM_DUP_ERR, 8,
			0, duve_cb->duverel->rel[ptr].du_tab,
			0, duve_cb->duverel->rel[ptr].du_own,
			sizeof(hist_tbl.hatno), &hist_tbl.hatno,
			sizeof(hist_tbl.hsequence), &hist_tbl.hsequence);
	    if ( duve_talk (DUVE_ASK, duve_cb, S_DU0329_DROP_FROM_IIHISTOGRAM,
		    6,sizeof(histid.db_tab_base), &histid.db_tab_base,
		    sizeof(histid.db_tab_index), &histid.db_tab_index,
		    sizeof(hist_tbl.hatno), &hist_tbl.hatno)
	    == DUVE_YES)
	    {
	    	    duve_cb->duvestat->stats[base].du_hdrop = DUVE_DROP;
		    base = DUVE_DROP;
		    duve_talk(DUVE_MODESENS, duve_cb, 
			    S_DU0379_DROP_FROM_IIHISTOGRAM,
			    6, sizeof(histid.db_tab_base), &histid.db_tab_base,
			    sizeof(histid.db_tab_index), &histid.db_tab_index,
			    sizeof(hist_tbl.hatno), &hist_tbl.hatno);
		    continue;
	    }
	    else
		seq = hist_tbl.hsequence + 1;
	}
	else
	{
	    /* missing tree sequence tuple */
            if (duve_banner( DUVE_IIHISTOGRAM, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb,S_DU164B_HISTOGRAM_SEQUENCE_ERR, 
			8, 0, duve_cb->duverel->rel[ptr].du_tab,
			0, duve_cb->duverel->rel[ptr].du_own,
			sizeof(hist_tbl.hatno), &hist_tbl.hatno,
			sizeof(seq), &seq);
	    if ( duve_talk (DUVE_ASK, duve_cb, S_DU0329_DROP_FROM_IIHISTOGRAM,6,
		    sizeof(histid.db_tab_base), &histid.db_tab_base,
		    sizeof(histid.db_tab_index), &histid.db_tab_index,
		    sizeof(hist_tbl.hatno), &hist_tbl.hatno)
	    == DUVE_YES)
	    {
	    	    duve_cb->duvestat->stats[base].du_hdrop = DUVE_DROP;
		    base = DUVE_DROP;
		    duve_talk(DUVE_MODESENS, duve_cb, 
			    S_DU0379_DROP_FROM_IIHISTOGRAM,
			    6, sizeof(histid.db_tab_base), &histid.db_tab_base,
			    sizeof(histid.db_tab_index), &histid.db_tab_index,
		    sizeof(hist_tbl.hatno), &hist_tbl.hatno);
		    continue;
	    }
	    else
		seq = hist_tbl.hsequence + 1;
	}
    }  /* end loop for each tuple */
    /* now that each tuple in iistatistics and iihistogram has been examined,
    ** open a cursor and loop to see if any modification should be made to the
    ** table
    */
    for (base=0; base < duve_cb->duve_scnt; base++)
    {
	if (duve_cb->duvestat->stats[base].du_rptr == DUVE_DROP)
	    continue;
	else if (duve_cb->duvestat->stats[base].du_sdrop == DUVE_DROP)
	{   
	    tid = duve_cb->duvestat->stats[base].du_sid.db_tab_base;
	    tidx = duve_cb->duvestat->stats[base].du_sid.db_tab_index;
	    att = duve_cb->duvestat->stats[base].du_satno;
/* # line 2835 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iistatistics where stabbase=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and stabindex=");
    IIputdomio((short *)0,1,30,4,&tidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and satno=");
    IIputdomio((short *)0,1,30,2,&att);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2837 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iihistogram where htabbase=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and htabindex=");
    IIputdomio((short *)0,1,30,4,&tidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and hatno=");
    IIputdomio((short *)0,1,30,2,&att);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2839 "duvechk2.sc" */	/* host code */
	    continue;
	}
	else if (duve_cb->duvestat->stats[base].du_hdrop == DUVE_DROP)
	{
	    tid = duve_cb->duvestat->stats[base].du_sid.db_tab_base;
	    tidx = duve_cb->duvestat->stats[base].du_sid.db_tab_index;
	    att = duve_cb->duvestat->stats[base].du_satno;
/* # line 2846 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iistatistics set snumcells=0 where stabbase=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and stabindex=");
    IIputdomio((short *)0,1,30,4,&tidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and satno=");
    IIputdomio((short *)0,1,30,2,&att);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2848 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iihistogram where htabbase=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and htabindex=");
    IIputdomio((short *)0,1,30,4,&tidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and hatno=");
    IIputdomio((short *)0,1,30,2,&att);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2850 "duvechk2.sc" */	/* host code */
	    continue;
	}
	else if (duve_cb->duvestat->stats[base].du_numcel != 0)
	{
    	    tid = duve_cb->duvestat->stats[base].du_sid.db_tab_base;
	    tidx = duve_cb->duvestat->stats[base].du_sid.db_tab_index;
	    att = duve_cb->duvestat->stats[base].du_satno;
	    cells = duve_cb->duvestat->stats[base].du_numcel;
/* # line 2858 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iistatistics set snumcells=");
    IIputdomio((short *)0,1,30,2,&cells);
    IIwritio(0,(short *)0,1,32,0,(char *)"where stabbase=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and stabindex=");
    IIputdomio((short *)0,1,30,4,&tidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and satno=");
    IIputdomio((short *)0,1,30,2,&att);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2860 "duvechk2.sc" */	/* host code */
	    continue;
	}
    } /* end loop for each entry in iistatistics cache */
    MEfree ( (PTR) duve_cb->duvestat);
    duve_cb->duvestat = NULL;
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckhist"); 
    return ( (DU_STATUS) DUVE_YES );
}
/*{
** Name: ckintegs - run verifydb tests to check system catalog iiintegrity.
**
** Description:
**      This routine opens a cursor to walk iiintegrity one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iiintegrity system catalog.
**
** Inputs:
**      duve_cb                         verifydb control block
**	    duverel                          cached iirelation information
**
** Outputs:
**      duve_cb                         verifydb control block
**	    duverel                         cached iirelation information
**	        du_tbldrop			flag indicating to drop table
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    tuples may be dropped or entered from/into table iiintegrity
**	    tuples may be modified or dropped from iirelation
**	    tuples may be entered into iirule
**	    tuples may be entered into iiprocedure
**	    tuples may be entered into iiindex
**
** History:
**      29-Aug-1988 (teg)
**          initial creation
**	21-Feb-1989 (teg)
**	    Stopped caching qryid for integrity in relinfo cache.
**	6-jan-92 (rickh)
**	    Constraints don't have query trees so please forgive
**	    them.
**	06-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**	16-jun-93 (anitap)
**	    Added tests for FIPS constraints project.
**	10-oct-93 (anitap)
**	    If matching iikey, iirule, iiprocedure, iiindex tuples not 
**	    present for the constraint, delete the constraint and create a SQL
**	    file to recreate the constraint.
**	    Added call to new function to drop the constraint and create SQL 
**	    file to create the constraint.
**	    Corrected spelling of message S_DU0320_DROP_FROM_IIINTERGRITY to
**	    S_DU0320_DROP_FROM_IIINTEGRITY.
**	10-dec-93 (teresa)
**	    Changed test 3 to query iitree on its keys so the query does not
**	    need to be a scan.  This is part of an effort to improve performance
**	    for verifydb.  Also added performance statistics reporting.
**	20-dec-93 (anitap)
**	    Cache the integrity tuples for which constraint needs to be dropped
**	    and recreated. Use iidbdepends cache in test 8 to check if 
**	    rule present for CHECK constraint. Also use iidbdepends cache in
**	    test 9 to check for procedures for referential constarints.
**	    Added new test 6 to check if the unique constraint is present
**	    for the referential constraint. If not, we continue with the next
**	    iiintegrity tuple.
**	    Do not call duve_obj_type() as we know the independent object type.
**	21-dec-93 (andre,teresa)
**	    Fix error handling for S_DU16A8_IIDBDEPENDS_MISSING, which did a
**	    query of two parameters into a single esqlc variable and used the
**	    wrong parameters to the error message.
**	07-dec-94 (teresa)
**	    remove erronous abort if iiintegrity is empty.  Do not open cursor
**	    if there are no tuples in iiintegrity.
**	08-mar-94 (anitap)
**	    Rick has added another dbproc for referential constraint. Fix for
**	    the same.
**	24-Jul-2002 (inifa01
**	    Bug 106821.  Do not verify existence of an index in Test 10 for 
**	    constraint where 'index = base table structure'. 
**	12-Sep-2004 (schka24)
**	    Another base table structure bug, test 7 this time.
*/
DU_STATUS
ckintegs(duve_cb)
DUVE_CB            *duve_cb;
{
    DB_INTEGRITY	iiintegrity;	
    DB_TAB_ID		int_id;
    DU_STATUS		base;
    DU_STATUS		status;
    i4	 	integrity = DU_SQL_OK;
    u_i4		size;
    STATUS              mem_int;
    i4		len;
    DU_STATUS 	        dep_offset;
    i4			dtype;
    i4 		ind_offset;
    i4		ind_type = DB_CONS; 
# include <duveint.h>
  i4 cnt = 0;
  u_i4 tid, tidx;
  u_i4 basetid, basetidx;
  i4 id1, id2;
  char name[DB_MAXNAME + 1];
  char owner[DB_MAXNAME + 1];
  i4 relstat;
  i2 mode;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 2986 "duvechk2.sc" */	/* host code */
    /* allocate memory to hold add/drop commands */
/* # line 2987 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(inttabbase)from iiintegrity");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 2989 "duvechk2.sc" */	/* host code */
    if (cnt == 0)
	return DUVE_YES;
    else	
        size = cnt * sizeof(DU_INTEGRITY);
    duve_cb->duveint = (DUVE_INTEGRITY *) MEreqmem( (u_i4)0, size, TRUE,
                               &mem_int);
    duve_cb->duve_intcnt = 0;
    if ( (mem_int != OK) || (duve_cb->duveint == 0) )
    {
        duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	return ( (DU_STATUS) DUVE_BAD);
    }
/* # line 3007 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"int_cursor",6255,8018);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iiintegrity");
    IIcsQuery((char *)"int_cursor",6255,8018);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3009 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iiintegrity */
    for (;;)  
    {
	/* get tuple from iiintegrity */
/* # line 3013 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"int_cursor",6255,8018,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(int_tbl).inttabbase);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).inttabidx);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intqryid1);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intqryid2);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).inttreeid1);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).inttreeid2);
      IIcsGetio((short *)0,1,30,2,&(int_tbl).intresvar);
      IIcsGetio((short *)0,1,30,2,&(int_tbl).intnumber);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intseq);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset1);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset2);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset3);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset4);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset5);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset6);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset7);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset8);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset9);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomseta);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset11);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset12);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset13);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset14);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset15);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset16);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset17);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset18);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset19);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset20);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset21);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset22);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset23);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset24);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset25);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset26);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset27);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset28);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset29);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset30);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset31);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset32);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).intdomset33);
      IIcsGetio((short *)0,1,32,32,(int_tbl).consname);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).consid1);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).consid2);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).consschema_id1);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).consschema_id2);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).consflags);
      IIcsGetio((short *)0,1,30,4,&(int_tbl).cons_create_date);
      IIcsGetio((short *)0,1,30,2,&(int_tbl).consdelrule);
      IIcsGetio((short *)0,1,30,2,&(int_tbl).consupdrule);
      IIcsGetio((short *)0,1,32,32,(int_tbl).intreserve);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3014 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in iiintegrity */
	{
/* # line 3016 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"int_cursor",6255,8018);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3017 "duvechk2.sc" */	/* host code */
	    break;
	}
	/* Put entry from iiintegrity table in iiintegrity
	** structure */
	iiintegrity.dbi_tabid.db_tab_base = int_tbl.inttabbase;
	iiintegrity.dbi_tabid.db_tab_index = int_tbl.inttabidx;
	iiintegrity.dbi_txtid.db_qry_high_time = int_tbl.intqryid1;
	iiintegrity.dbi_txtid.db_qry_low_time = int_tbl.intqryid2;
     	iiintegrity.dbi_tree.db_tre_high_time = int_tbl.inttreeid1;
	iiintegrity.dbi_tree.db_tre_low_time = int_tbl.inttreeid2;
	iiintegrity.dbi_resvar = int_tbl.intresvar;
	iiintegrity.dbi_number = int_tbl.intnumber;
	iiintegrity.dbi_seq = int_tbl.intseq;
	iiintegrity.dbi_cons_id.db_tab_base = int_tbl.consid1;
	iiintegrity.dbi_cons_id.db_tab_index = int_tbl.consid2;
	iiintegrity.dbi_consschema.db_tab_base = int_tbl.consschema_id1;
	iiintegrity.dbi_consschema.db_tab_index = int_tbl.consschema_id2;
	iiintegrity.dbi_consflags = int_tbl.consflags;
	len = STtrmwhite(int_tbl.consname);
	STcopy(int_tbl.consname, iiintegrity.dbi_consname.db_constraint_name);
	iiintegrity.dbi_consname.db_constraint_name[len] = '\0';
	int_id.db_tab_base = int_tbl.inttabbase;
	int_id.db_tab_index = int_tbl.inttabidx;
	tid = int_tbl.inttabbase;
	tidx = int_tbl.inttabidx;
	/* test 1 -- verify table receiving integrity exists */
	base = duve_findreltid ( &int_id, duve_cb);
	if (base == DU_INVALID)
	{
	    /* this tuple is not in iirelation, so drop from iiintegrity */
            if (duve_banner( DUVE_IIINTEGRITY, 1, duve_cb)
	    == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1634_NO_BASE_FOR_INTEG, 4,
			sizeof(tid), &tid, sizeof(tidx), &tidx);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0320_DROP_FROM_IIINTEGRITY,
		 4, sizeof(tid), &tid, sizeof(tidx), &tidx)
	    == DUVE_YES)
	    {
/* # line 3064 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iiintegrity",(char *)"int_cursor",6255,8018);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3065 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS,duve_cb,S_DU0370_DROP_FROM_IIINTEGRITY, 
			  4, sizeof(tid), &tid, sizeof(tidx), &tidx);
	    }
	    continue;
	} /* endif base table not found */
	else 
	{
	    if ( base == DUVE_DROP)
		continue;	/* if table is marked for drop, stop now */
	}
	/* 
	**  test 2 - verify base table indicates integrities are defined on it
	*/
	if ( (duve_cb->duverel->rel[base].du_stat & TCB_INTEGS) == 0 )
	{
	    /* the relstat in iirelation needs to be modified */
            if (duve_banner( DUVE_IIINTEGRITY, 2, duve_cb)
	    == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1632_NO_INTEGS_RELSTAT, 4,
			0, duve_cb->duverel->rel[base].du_tab,
			0, duve_cb->duverel->rel[base].du_own);
	    if ( duve_talk( DUVE_ASK, duve_cb, S_DU031F_SET_INTEGS, 0)
	    == DUVE_YES)
	    {
		basetid =  duve_cb->duverel->rel[base].du_id.db_tab_base;
		basetidx =  duve_cb->duverel->rel[base].du_id.db_tab_index;
		relstat = duve_cb->duverel->rel[base].du_stat | TCB_INTEGS;
		duve_cb->duverel->rel[base].du_stat = relstat;
/* # line 3096 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&relstat);
    IIwritio(0,(short *)0,1,32,0,(char *)"where reltid=");
    IIputdomio((short *)0,1,30,4,&basetid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&basetidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3098 "duvechk2.sc" */	/* host code */
		if (sqlca.sqlcode == 0)
		    duve_talk(DUVE_MODESENS, duve_cb, S_DU036F_SET_INTEGS, 0);
	    }  /* endif fix relstat */
	} /* endif relstat doesnt indicate integrities */
	/* test 3 -- verify internal representation of integrity exists */
	/* integrities should have trees unless they are constraints! */
	if ( ( int_tbl.consflags & ( CONS_UNIQUE | CONS_CHECK | CONS_REF ) )
		== 0 )
	{
	    mode = DB_INTG;
/* # line 3112 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk216",6256,17364);
      IIputdomio((short *)0,1,30,4,&tid);
      IIputdomio((short *)0,1,30,4,&tidx);
      IIputdomio((short *)0,1,30,2,&mode);
      IIputdomio((short *)0,1,30,4,&int_tbl.inttreeid1);
      IIputdomio((short *)0,1,30,4,&int_tbl.inttreeid2);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk216",6256,17364);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(treetabbase)from iitree where treetabbase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&tid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&tidx);
        IIwritio(0,(short *)0,1,32,0,(char *)"and treemode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&mode);
        IIwritio(0,(short *)0,1,32,0,(char *)"and treeid1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&int_tbl.inttreeid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&int_tbl.inttreeid2);
        IIexDefine(0,(char *)"duvechk216",6256,17364);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3117 "duvechk2.sc" */	/* host code */
	    if (cnt == 0)
	    {
	        /* missing internal representation of integrity */
                if (duve_banner( DUVE_IIINTEGRITY, 3, duve_cb)
	        == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
	        duve_talk ( DUVE_MODESENS, duve_cb, S_DU1635_NO_TREE_FOR_INTEG, 8,
			    sizeof(tid), &tid, sizeof(tidx), &tidx,
			    0, duve_cb->duverel->rel[base].du_tab,
			    0, duve_cb->duverel->rel[base].du_own);
	        if ( duve_talk(DUVE_ASK, duve_cb, S_DU0320_DROP_FROM_IIINTEGRITY,
			   4, sizeof(tid), &tid, sizeof(tidx), &tidx)
	        == DUVE_YES)
	        {
/* # line 3133 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iiintegrity",(char *)"int_cursor",6255,8018);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3134 "duvechk2.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS,duve_cb,S_DU0370_DROP_FROM_IIINTEGRITY, 
			  4, sizeof(tid), &tid, sizeof(tidx), &tidx);
		    continue;
	        }
	    }
	}	/* end if not a constraint */
	/* test 4 - verify query text used to define integrity exists */
	mode = DB_INTG;
/* # line 3144 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk217",6257,17887);
      IIputdomio((short *)0,1,30,4,&int_tbl.intqryid1);
      IIputdomio((short *)0,1,30,4,&int_tbl.intqryid2);
      IIputdomio((short *)0,1,30,2,&mode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk217",6257,17887);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(txtid1)from iiqrytext where txtid1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&int_tbl.intqryid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&int_tbl.intqryid2);
        IIwritio(0,(short *)0,1,32,0,(char *)"and mode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&mode);
        IIexDefine(0,(char *)"duvechk217",6257,17887);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3147 "duvechk2.sc" */	/* host code */
	if (cnt == 0)
	{
	    /* missing query text that defined integrity */
            if (duve_banner( DUVE_IIINTEGRITY, 4, duve_cb)
	        == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1636_NO_INTEG_QRYTEXT, 8,
			sizeof(tid), &tid, sizeof(tidx), &tidx,
			0, duve_cb->duverel->rel[base].du_tab,
			0, duve_cb->duverel->rel[base].du_own);
	}
	/* test 5 - verify that the integrity which is a constraint has
        ** enough tuples in iikey.
        */
        if (int_tbl.consflags == CONS_UNIQUE)
	{
/* # line 3166 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk218",6258,19808);
      IIputdomio((short *)0,1,30,4,&int_tbl.consid1);
      IIputdomio((short *)0,1,30,4,&int_tbl.consid2);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk218",6258,19808);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(key_attid)from iikey where key_consid1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&int_tbl.consid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and key_consid2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&int_tbl.consid2);
        IIexDefine(0,(char *)"duvechk218",6258,19808);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3170 "duvechk2.sc" */	/* host code */
           if (cnt < 1)
           {
              if (duve_banner(DUVE_IIINTEGRITY, 5,  duve_cb)
               		== DUVE_BAD)
                 return ((DU_STATUS) DUVE_BAD);
              duve_talk(DUVE_MODESENS, duve_cb,
                S_DU1690_MISSING_KEYTUP, 2, 0, int_tbl.consname);
	       if ( duve_talk(DUVE_ASK, duve_cb, S_DU0320_DROP_FROM_IIINTEGRITY,
			   4, sizeof(tid), &tid, sizeof(tidx), &tidx)
	        == DUVE_YES)
	      {	
   	         /* call routine which drops and creates SQL file to 
	         ** create the constraint */
	         duve_cb->duveint->integrities[duve_cb->duve_intcnt].du_dcons =
                               DUVE_DROP;
 	         if ((status = create_cons(duve_cb, &iiintegrity, base)) 
			!= DUVE_YES)
	            return(status);
                 continue;
	      }	
           } /* endif corresponding tuple not in iikey */
	} /* end test 5 */
	/* Test 6 - Check that for referential constraint, we have the
	** the corresponding UNIQUE constraint.
	*/
	if (int_tbl.consflags == CONS_REF)
	{
	   DU_STATUS	depend_offset;
	   DU_D_DPNDS	*depend;
	   i4		dep_type = DB_CONS;
	   depend_offset = duve_d_dpndfind(int_tbl.inttabbase,
				int_tbl.inttabidx, dep_type,
				int_tbl.intnumber, duve_cb);
	   if (depend_offset != DU_INVALID)
	   {
	      depend = duve_cb->duvedpnds.duve_dep + depend_offset;
	      if (depend->du_dflags & DU_MISSING_INDEP_OBJ)
	      {
	         char	obj_type[50];
	         i4	type_len;
            	 if (duve_banner(DUVE_IIINTEGRITY, 6, duve_cb)
                       == DUVE_BAD)
                    return ((DU_STATUS) DUVE_BAD);
		 STcopy("CONSTRAINT", obj_type);
		 type_len = sizeof("CONSTRAINT") - 1;
                 duve_talk(DUVE_MODESENS, duve_cb, 
			S_DU16C1_NONEXISTENT_INDEP_OBJ, 
		       	10,
		       	type_len, 	       obj_type,
		       	sizeof(int_tbl.inttabbase), &int_tbl.inttabbase,
	               	sizeof(int_tbl.inttabidx), &int_tbl.inttabidx,
			sizeof(dep_type), &dep_type,
		       	sizeof(int_tbl.intnumber), &int_tbl.intnumber);
	        continue;
	      } /* endif unique constraint missing */
	  } /* end if depend_offset != DU_INVALID */
	} /* end test 6 */
        /* Test 7 - check that we have enough tuples in iidbdepends 
        ** for the constraint, i.e., 1 for unique or check and 8/9 for
        ** referential (the index for referential constraint is user droppable)
	** FIXME eliminate this check for UNIQUE because the CONS_BASEIX
	** flag is not reliable - it's not set prior to late 2.6 patches,
	** and there's no convenient way I could think of to get
	** upgradedb to set it without a lot of overhead or work.
        */ 
        if ( /* ** removed (int_tbl.consflags == CONS_UNIQUE) || ** */
	  (int_tbl.consflags == CONS_CHECK) || 
	  (int_tbl.consflags == CONS_REF))
        {	
          integrity = DU_SQL_OK; 
	  ind_offset = DU_INVALID; 
          dtype = DU_INVALID;
          dep_offset = duve_d_cnt(ind_offset, int_tbl.inttabbase, 
					int_tbl.inttabidx,
					ind_type, int_tbl.intnumber,
					dtype, duve_cb);
          switch (int_tbl.consflags)
          {
	     case CONS_UNIQUE:
	     case CONS_CHECK:
	       	    if (dep_offset < 1)
	       	    {
		  	integrity = DU_INVALID;
	       	    }
	       	    break;
	     case CONS_REF:
			if (dep_offset < 8)
			{
			   integrity = DU_INVALID;
			}
			break;
	     default:
			break;
         }
         if (integrity == DU_INVALID)
         {
            if (duve_banner(DUVE_IIINTEGRITY, 7, duve_cb)
                       == DUVE_BAD)
               return ((DU_STATUS) DUVE_BAD);
            duve_talk(DUVE_MODESENS, duve_cb, S_DU16A8_IIDBDEPENDS_MISSING, 4,
			0, duve_cb->duverel->rel[base].du_tab, 0, 
			duve_cb->duverel->rel[base].du_own);
            if ( duve_talk(DUVE_ASK, duve_cb, S_DU0320_DROP_FROM_IIINTEGRITY,
			   4, sizeof(tid), &tid, sizeof(tidx), &tidx)
	        == DUVE_YES)
	    {
	       /* call routine to drop and create the SQL file to 
	       ** recreate constraint 
	       */ 
               duve_cb->duveint->integrities[duve_cb->duve_intcnt].du_dcons =
                               DUVE_DROP;
	       if ((status = create_cons(duve_cb, &iiintegrity, base)) 
			!= DUVE_YES)
		  return(status);
	       continue;
	    } /* endif drop constraint */
         } /* endif enough iidbdepends tuples not present */
     } /* end test 7 */
     /* test 8 - verify that there are enough rule tuples for
     ** the constraint i.e., 1 for check and 4 for referential.
     */
     integrity = DU_SQL_OK;
     switch (int_tbl.consflags)
     {
	    case CONS_CHECK:	
		{	
			bool		first = FALSE;
    			DU_STATUS	indep_offset;
    			DU_I_DPNDS	*indep;
			indep_offset = duve_i_dpndfind(int_tbl.inttabbase,
					int_tbl.inttabidx, ind_type, 
					int_tbl.intnumber, duve_cb, first);
			if (indep_offset != DU_INVALID)
			{
			   indep = duve_cb->duvedpnds.duve_indep 
					+ indep_offset; 
			   /* use iidbdepends cache to check if rule(dependent
			   ** object) present for the check constraint.
			   */
			   if (indep->du_next.du_dep->du_dflags &
					DU_NONEXISTENT_INDEP_OBJ)
	      		      integrity = DU_INVALID;
			} /* endif indep_offset != DU_INVALID */
			break;	
		 }
	    case CONS_REF:	
		 {	
			dtype = DB_RULE;
		        ind_offset = DU_INVALID;
			dep_offset = duve_d_cnt(ind_offset,
				int_tbl.inttabbase, int_tbl.inttabidx,
				ind_type, int_tbl.intnumber,
				dtype, duve_cb);
			if (dep_offset < 4)	
	                   integrity = DU_INVALID;
			break;
	      	  }
	      default:
			break;	
	}
	if (integrity == DU_INVALID)
	{
           if (duve_banner(DUVE_IIINTEGRITY, 8, duve_cb)
                       == DUVE_BAD)
               return ((DU_STATUS) DUVE_BAD);
           duve_talk(DUVE_MODESENS, duve_cb,
                        S_DU1691_MISSING_RULETUP, 2, 0, int_tbl.consname);
           if ( duve_talk(DUVE_ASK, duve_cb, S_DU0320_DROP_FROM_IIINTEGRITY,
			   4, sizeof(tid), &tid, sizeof(tidx), &tidx)
	        == DUVE_YES)
	   {
   	      /* call routine which drops and creates the SQL file to create
	      ** SQL file to create the constraint 
	      */
              duve_cb->duveint->integrities[duve_cb->duve_intcnt].du_dcons =
                               DUVE_DROP;
	      if ((status = create_cons(duve_cb, &iiintegrity, base)) 
			!= DUVE_YES)
	         return(status);
	      continue;
	   } /* endif drop constraint */
        } /* end if corresponding tuple not present in iirule */
        /* Test 9 - check that we have enough tuples in iiprocedure
        ** for the constraint - i.e., 4 for referential.
        */
        if (int_tbl.consflags == CONS_REF)
        {
	   dtype = DB_DBP;
	   ind_offset = DU_INVALID;
	   dep_offset = duve_d_cnt(ind_offset,
				int_tbl.inttabbase, int_tbl.inttabidx,
				ind_type, int_tbl.intnumber,
				dtype, duve_cb);
           if (dep_offset < 4)
           {
              if (duve_banner(DUVE_IIINTEGRITY, 9, duve_cb)
                       == DUVE_BAD)
                 return ((DU_STATUS) DUVE_BAD);
              duve_talk(DUVE_MODESENS, duve_cb,
                       S_DU1692_MISSING_DBPROCTUP, 2, 0, int_tbl.consname);
              if (duve_talk (DUVE_ASK, duve_cb, S_DU0320_DROP_FROM_IIINTEGRITY,
		 	4, sizeof(tid), &tid, sizeof(tidx), &tidx)
			== DUVE_YES)
	      {	
                 duve_cb->duveint->integrities[duve_cb->duve_intcnt].du_dcons =
                               DUVE_DROP;
	         if ((status = create_cons(duve_cb, &iiintegrity, base)) 
			!= DUVE_YES)
	         return(status);
	         continue;
	      } /* endif drop constraint */
           } /* endif corresponding tuple not present in iiprocedure */
       } /* endif constraint type = CONS_REF */
       /* Test 10 - check that we have enough tuples in iiindex
       ** for the constraint i.e., 1 for unique.
       ** We do not check the index for referential as the user can drop the
       ** index, also we do not check for index if base table is index.		
	** FIXME removed this test because CONS_BASEIX is not set
	** reliably for older (ie pre-late-2.6-patch) databases.
       */
#if 0
       if ( (int_tbl.consflags & (CONS_UNIQUE | CONS_PRIMARY)) &&
	    !(int_tbl.consflags & CONS_BASEIX) )
       {
/* # line 3452 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk219",6259,28382);
      IIputdomio((short *)0,1,30,4,&int_tbl.inttabbase);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk219",6259,28382);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(indexid)from iiindex where baseid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&int_tbl.inttabbase);
        IIexDefine(0,(char *)"duvechk219",6259,28382);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3456 "duvechk2.sc" */	/* host code */
          if (cnt != 1)
          {
            if (duve_banner(DUVE_IIINTEGRITY, 10, duve_cb)
                        == DUVE_BAD)
                 return ((DU_STATUS) DUVE_BAD);
               duve_talk(DUVE_MODESENS, duve_cb,
                        S_DU1693_MISSING_INDEXTUP, 2, 0,
                                int_tbl.consname);
               if (duve_talk (DUVE_ASK, duve_cb, S_DU0320_DROP_FROM_IIINTEGRITY,
		 	4, sizeof(tid), &tid, sizeof(tidx), &tidx)
			== DUVE_YES)
	       {	
	          /* if the index is missing for the unique constraint,
	          ** we cannot drop the constraint because DMF will complain.
	          ** This is because there is a corresponding entry in
		  ** iirelation for the index. So we need to hand delete the 
		  ** corresponding tuples in iikey, iiintegrity.
	          ** The corresponding iidbdepends tuple would have been deleted
	          ** by ckdbdep(). The corresponding tuple in iirelation is 
		  ** deleted by ckrel() and the corresponding tuples in 
		  ** iiattribute by ckatt(). 
	          */ 
	          if ((status = create_cons(duve_cb, &iiintegrity, base)) 
			!= DUVE_YES)
	             return(status);
/* # line 3486 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iikey where key_consid1=");
    IIputdomio((short *)0,1,30,4,&int_tbl.consid1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3488 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&int_tbl.inttabbase);
    IIwritio(0,(short *)0,1,32,0,(char *)"and consid1=");
    IIputdomio((short *)0,1,30,4,&int_tbl.consid1);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3490 "duvechk2.sc" */	/* host code */
	          continue;     
	      } /* endif drop constraint */
         } /* endif corresponding tuple not present in iiindex */  
      } /* endif CONS_UNIQUE */
#endif
    }  /* end loop for each tuple in table */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckintegs"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckxproc - run verifydb check on iixprocedure dbms catalog
**
** Description:
**	This function will ensure that IIXPROCEDURE is in sync with IIPROCEDURE.
**	This will be accomplished in two steps:
**	- first we will open a cursor to read tuples in iixprocedure for which 
**	  there are no corresponding tuples in iiprocedure; if one or more such
**	  tuples are found, we will ask for permission to drop and recreate the 
**	  index
**	- then we will open a cursor on iiprocedure to read tuples for which 
**	  there is no corresponding iixprocedure tuple and insert appropriate 
**	  iixprocedure tuples
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**	none.
**
** Returns:
**    	DUVE_YES
**      DUVE_BAD
**
** Exceptions:
**	none
**
** Side Effects:
**	tuples may be inserted into / deleted from iixprocedure
**
** History:
**      19-jan-94 (andre)
**	    written
[@history_template@]...
*/
DU_STATUS
ckxproc(
	DUVE_CB		*duve_cb)
{
# include <duvexdbp.h>
  u_i4 tid_ptr;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 3563 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"xdbp_tst_curs_1",6260,21088);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select * from iixprocedure xdbp where not exists(select 1 from iiproc\
edure dbp where xdbp.tidp=dbp.tid and xdbp.dbp_id=dbp.dbp_id and xdbp.\
dbp_idx=dbp.dbp_idx)");
    IIcsQuery((char *)"xdbp_tst_curs_1",6260,21088);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3565 "duvechk2.sc" */	/* host code */
    /*
    ** test 1: for every IIXPROCEDURE tuple for which there is no corresponding 
    ** IIPROCEDURE tuple ask user whether we may delete the offending tuple from
    ** IIXPROCEDURE
    */
    for (;;)
    {
/* # line 3572 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"xdbp_tst_curs_1",6260,21088,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(xdbp_tbl).dbp_id);
      IIcsGetio((short *)0,1,30,4,&(xdbp_tbl).dbp_idx);
      IIcsGetio((short *)0,1,30,4,&(xdbp_tbl).tidp);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3573 "duvechk2.sc" */	/* host code */
        if (sqlca.sqlcode == 100) /* then no more tuples to read */
        {
/* # line 3575 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"xdbp_tst_curs_1",6260,21088);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3576 "duvechk2.sc" */	/* host code */
	    break;
        }
	/* 
	** there is no tuple in IIPROCEDURE corresponding to this IIXPROCEDURE 
	** tuple 
	*/
        if (duve_banner(DUVE_IIXPROCEDURE, 1, duve_cb) == DUVE_BAD) 
	    return ( (DU_STATUS)DUVE_BAD);
        duve_talk( DUVE_MODESENS, duve_cb, S_DU16CB_EXTRA_IIXPROCEDURE, 6,
            sizeof(xdbp_tbl.dbp_id), &xdbp_tbl.dbp_id,
	    sizeof(xdbp_tbl.dbp_idx), &xdbp_tbl.dbp_idx,
	    sizeof(xdbp_tbl.tidp), &xdbp_tbl.tidp);
	if (duve_talk( DUVE_ASK, duve_cb, S_DU024C_DELETE_IIXPROC, 0) 
	    == DUVE_YES)
	{
/* # line 3593 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iixprocedure",(char *)"xdbp_tst_curs_1",6260,21088);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3595 "duvechk2.sc" */	/* host code */
	    duve_talk(DUVE_MODESENS,duve_cb, S_DU044C_DELETED_IIXPROC, 0);
	    break;
	}
    }
    /* end test 1 */
/* # line 3617 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"xdbp_tst_curs_2",6261,23635);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbp.*, dbp.tid from iiprocedure dbp where not exists(select 1 \
from iixprocedure xdbp where xdbp.tidp=dbp.tid and xdbp.dbp_id=dbp.dbp\
_id and xdbp.dbp_idx=dbp.dbp_idx and dbp_name=dbp_name)");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"xdbp_tst_curs_2",6261,23635);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3619 "duvechk2.sc" */	/* host code */
    /*
    ** test 2: for every IIPROCEDURE tuple for which there is no corresponding 
    ** IIXPROCEDURE tuple ask user whether we may insert a missing tuple into 
    ** IIXPROCEDURE
    */
    for (;;)
    {
/* # line 3626 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"xdbp_tst_curs_2",6261,23635,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,(proc_tbl).dbp_name);
      IIcsGetio((short *)0,1,32,32,(proc_tbl).dbp_owner);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_txtlen);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_txtid1);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_txtid2);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_mask1);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_mask2);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_id);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_idx);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_parameter_count);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_record_width);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_ubt_id);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_ubt_idx);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_create_date);
      IIcsGetio((short *)0,1,32,16,(proc_tbl).dbp_free);
      IIcsGetio((short *)0,1,30,4,&tid_ptr);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3627 "duvechk2.sc" */	/* host code */
        if (sqlca.sqlcode == 100) /* then no more tuples to read */
        {
/* # line 3629 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"xdbp_tst_curs_2",6261,23635);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3630 "duvechk2.sc" */	/* host code */
	    break;
        }
	/* 
	** there is no tuple in IIXPROCEDURE corresponding to this IIPROCEDURE 
	** tuple 
	*/
        if (duve_banner(DUVE_IIXPROCEDURE, 2, duve_cb) == DUVE_BAD) 
	    return ( (DU_STATUS)DUVE_BAD);
        duve_talk( DUVE_MODESENS, duve_cb, S_DU16CC_MISSING_IIXPROCEDURE, 6,
	    sizeof(proc_tbl.dbp_id), &proc_tbl.dbp_id,
	    sizeof(proc_tbl.dbp_idx), &proc_tbl.dbp_idx,
	    sizeof(tid_ptr), &tid_ptr);
	if (duve_talk( DUVE_ASK, duve_cb, S_DU024D_INSERT_IIXPROC_TUPLE, 0) 
	    == DUVE_YES)
	{
/* # line 3647 "duvechk2.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iixprocedure(dbp_id, dbp_idx, tidp)values(");
    IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_id);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_idx);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&tid_ptr);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3651 "duvechk2.sc" */	/* host code */
	    duve_talk(DUVE_MODESENS, duve_cb, 
		S_DU044D_INSERTED_IIXPROC_TUPLE, 0);
	}
    }
    /* end test 2 */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckxproc"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckproc - run verifydb tests to check system catalog iiprocedure.
**
** Description:
**      This routine opens a cursor to walk iiprocedure one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iiprocedure system catalog.
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      duve_cb                         verifydb control block
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    tuples may be dropped from table iiprocedure.
**
** History:
**      27-may-1993 (teresa)
**          initial creation
**	16-jun-93 (anitap)
**	    Added test for CREATE SCHEMA project.
**	10-oct-93 (anitap)
**	    Added test for FIPS constraints project. 
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
**	13-dec-93 (anitap)
**	    Corrected typos. Use ANY instead of COUNT to improve performance.
**	    Corrected test number in duve_banner() to 4.
**	21-jan-94 (andre)
**	    added code to implement newly defined tests 5-8
**	28-jan-94 (andre)
**	    if the dbproc text is missing (test #1), 
**	    dbp_id is not unique (test #2), or 
**	    IIDBDEPENDS tuple is missing for a system-generated dbproc (test #4)
**	    then rather than deleting IIPROCEDURE tuple we will add a record to
**	    the "fix it" list reminding us to drop this dbproc and mark dormant
**	    any dbprocs dependent on it.
[@history_template@]...
*/
DU_STATUS
ckproc(duve_cb)
DUVE_CB            *duve_cb;
{
    DB_TAB_ID		proc_id;
    DU_STATUS		base;
    DU_STATUS   	proc_dep_offset = DU_INVALID;
    u_i4		size;
    DUVE_DROP_OBJ	*drop_obj;
    STATUS		mem_stat;
  i4 cnt;
  i2 mode;
  char schname[DB_MAXNAME + 1];
  char *statement_buffer, buf[CMDBUFSIZE];
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 3738 "duvechk2.sc" */	/* host code */
    /* allocate memory to hold add commands */	
/* # line 3739 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(dbp_name)from iiprocedure");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3741 "duvechk2.sc" */	/* host code */
    if ( (sqlca.sqlcode == 100) || (cnt == 0))
    {
       /* its unreasonable to have an empty iiprocedure table, as procedures 
       ** "iierror" and "iiqef_alter_extension" should be created by createdb. 
       */
       duve_talk (DUVE_ALWAYS, duve_cb, E_DU503B_EMPTY_IIPROCEDURE, 0);
       return DUVE_BAD;
    }
    duve_cb->duve_prcnt = 0;
    size = cnt * sizeof(DU_PROCEDURE);
    duve_cb->duveprocs = (DUVE_PROCEDURE *) MEreqmem( (u_i4)0, size, TRUE, 
			&mem_stat);
    if ( (mem_stat != OK) || (duve_cb->duveprocs == 0))
    {
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
        return( (DU_STATUS) DUVE_BAD);
    }	
/* # line 3763 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"proc_cursor",6262,20954);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iiprocedure");
    IIcsQuery((char *)"proc_cursor",6262,20954);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3765 "duvechk2.sc" */	/* host code */
    mode = DB_DBP;
    /* loop for each tuple in iiprocedure */
    for (;;)  
    {
	/* get tuple from iiprocedure */
/* # line 3771 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"proc_cursor",6262,20954,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,(proc_tbl).dbp_name);
      IIcsGetio((short *)0,1,32,32,(proc_tbl).dbp_owner);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_txtlen);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_txtid1);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_txtid2);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_mask1);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_mask2);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_id);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_idx);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_parameter_count);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_record_width);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_ubt_id);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_ubt_idx);
      IIcsGetio((short *)0,1,30,4,&(proc_tbl).dbp_create_date);
      IIcsGetio((short *)0,1,32,16,(proc_tbl).dbp_free);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3772 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in iiprocedure */
	{
/* # line 3774 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"proc_cursor",6262,20954);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3775 "duvechk2.sc" */	/* host code */
	    break;
	}
	proc_id.db_tab_base = proc_tbl.dbp_id;
	proc_id.db_tab_index = proc_tbl.dbp_idx;
	STtrmwhite(proc_tbl.dbp_owner);
	STtrmwhite(proc_tbl.dbp_name);
	/* test 1 - verify query text used to define procedure exists */
/* # line 3786 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk220",6263,24925);
      IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_txtid1);
      IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_txtid2);
      IIputdomio((short *)0,1,30,2,&mode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk220",6263,24925);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(txtid1)from iiqrytext where txtid1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_txtid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_txtid2);
        IIwritio(0,(short *)0,1,32,0,(char *)"and mode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&mode);
        IIexDefine(0,(char *)"duvechk220",6263,24925);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3790 "duvechk2.sc" */	/* host code */
	if (cnt == 0)
	{
	    /* missing query text that defined procedure */
            if (duve_banner( DUVE_IIPROCEDURE, 1, duve_cb)
	        == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1680_NO_TXT_FOR_DBP, 4,
			0, proc_tbl.dbp_name, 0, proc_tbl.dbp_owner);
	    if ( duve_talk(DUVE_IO, duve_cb, S_DU0344_DROP_DBP, 0) == DUVE_YES)
	    {
		drop_obj = (DUVE_DROP_OBJ *) MEreqmem((u_i4) 0, 
		    sizeof(DUVE_DROP_OBJ), TRUE, &mem_stat);
		if (mem_stat != OK || drop_obj == NULL)
		{
		    duve_talk (DUVE_ALWAYS, duve_cb, 
			E_DU3400_BAD_MEM_ALLOC, 0);
		    return ((DU_STATUS) DUVE_BAD);
		}
		/*
		** add a record to the "fix it" list reminding us to drop this 
		** database procedure
		*/
		drop_obj->duve_obj_id.db_tab_base  = proc_tbl.dbp_id;
		drop_obj->duve_obj_id.db_tab_index = proc_tbl.dbp_idx;
		drop_obj->duve_obj_type            = DB_DBP;
		drop_obj->duve_secondary_id	   = 0;
		drop_obj->duve_drop_flags	   = DUVE_DBP_DROP;
		/* link this description into the existing list */
		drop_obj->duve_next = 
		    duve_cb->duvefixit.duve_objs_to_drop;
		duve_cb->duvefixit.duve_objs_to_drop = drop_obj;
		duve_talk(DUVE_MODESENS, duve_cb, S_DU0394_DROP_DBP, 0);
	    }
	    continue;
	} /* endif procedure definition does not exist */	    
	/* test 2 -- verify procedure identifier is unique */
	base = duve_findreltid ( &proc_id, duve_cb);
	if (base != DU_INVALID)
	{
	    /* this id is in iirelation, so drop from iiprocedure */
	    if (duve_banner( DUVE_IIPROCEDURE, 2, duve_cb)
	    == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1681_DBPID_NOT_UNIQUE, 8,
			0, proc_tbl.dbp_name, 0, proc_tbl.dbp_owner,
			sizeof(proc_tbl.dbp_id), &proc_tbl.dbp_id,
			sizeof(proc_tbl.dbp_idx), &proc_tbl.dbp_idx);
	    if ( duve_talk(DUVE_IO, duve_cb, S_DU0344_DROP_DBP, 0)
	    == DUVE_YES)
	    {
		drop_obj = (DUVE_DROP_OBJ *) MEreqmem((u_i4) 0, 
		    sizeof(DUVE_DROP_OBJ), TRUE, &mem_stat);
		if (mem_stat != OK || drop_obj == NULL)
		{
		    duve_talk (DUVE_ALWAYS, duve_cb, 
			E_DU3400_BAD_MEM_ALLOC, 0);
		    return ((DU_STATUS) DUVE_BAD);
		}
		/*
		** add a record to the "fix it" list reminding us to drop this 
		** database procedure
		*/
		drop_obj->duve_obj_id.db_tab_base  = proc_tbl.dbp_id;
		drop_obj->duve_obj_id.db_tab_index = proc_tbl.dbp_idx;
		drop_obj->duve_obj_type            = DB_DBP;
		drop_obj->duve_secondary_id	   = 0;
		drop_obj->duve_drop_flags	   = DUVE_DBP_DROP;
		/* link this description into the existing list */
		drop_obj->duve_next = 
		    duve_cb->duvefixit.duve_objs_to_drop;
		duve_cb->duvefixit.duve_objs_to_drop = drop_obj;
		duve_talk(DUVE_MODESENS, duve_cb, S_DU0394_DROP_DBP, 0);
	    }
	    continue;
	} /* endif identifier is not unique */
	/* test 3 -- verify procedure belongs to a schema */
        /* first, take 32 char procedure owner and force into
        ** string DB_MAXNAME char long.
        */
        MEcopy( (PTR)proc_tbl.dbp_owner, DB_MAXNAME, (PTR)schname);
        schname[DB_MAXNAME]='\0';
        (void) STtrmwhite(schname);
        /* now search for iischema tuple or owner match */
/* # line 3887 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk221",6264,22986);
      IIputdomio((short *)0,1,32,0,schname);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk221",6264,22986);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(schema_id)from iischema where schema_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,schname);
        IIexDefine(0,(char *)"duvechk221",6264,22986);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 3890 "duvechk2.sc" */	/* host code */
        if (cnt == 0)
        {
           /* corresponding entry missing in iischema */
           if (duve_banner(DUVE_IIPROCEDURE, 3, duve_cb)
                   == DUVE_BAD)
                return ( (DU_STATUS) DUVE_BAD);
           duve_talk (DUVE_MODESENS, duve_cb, S_DU168D_SCHEMA_MISSING, 2,
                       0, proc_tbl.dbp_owner);
	   if ( duve_talk(DUVE_IO, duve_cb, S_DU0245_CREATE_SCHEMA, 0)
			== DUVE_YES)
	   {	
 	      /* cache info about this tuple */
              MEcopy( (PTR)proc_tbl.dbp_owner, DB_MAXNAME,
                (PTR) duve_cb->duveprocs->procs[duve_cb->duve_prcnt].du_schname);
	      duve_cb->duve_prcnt++;
	      duve_talk(DUVE_MODESENS, duve_cb, S_DU0445_CREATE_SCHEMA, 0);
	   }
	  continue;
        } /* end test 3 */
	/* test 4 -- verify iidbdepends tuple present for procedure */
        /* Special check needed for procedures supporting
	** constraints.
	*/
	if ((proc_tbl.dbp_mask1 & DBP_CONS) != 0)
	{
	   /*
	   ** search IIDBDEPENDS cache for a dependent object info element
	   ** describing this procedure
	   */
	   proc_dep_offset = duve_d_dpndfind(proc_tbl.dbp_id, proc_tbl.dbp_idx,
				(i4) DB_DBP, (i4) 0, duve_cb);
	   if (proc_dep_offset == DU_INVALID) 
	   {  
	      /* missing dbdepends tuple for the procedure */
	      if (duve_banner( DUVE_IIPROCEDURE, 4, duve_cb)
                	== DUVE_BAD)
                 return ( (DU_STATUS) DUVE_BAD); 
	       duve_talk(DUVE_MODESENS, duve_cb, S_DU16A8_IIDBDEPENDS_MISSING, 4,
			0, proc_tbl.dbp_name, 0, proc_tbl.dbp_owner);
	      if ( duve_talk(DUVE_IO, duve_cb, S_DU0344_DROP_DBP, 0)
            	== DUVE_YES)
	      {
		 drop_obj = (DUVE_DROP_OBJ *) MEreqmem((u_i4) 0, 
		     sizeof(DUVE_DROP_OBJ), TRUE, &mem_stat);
		 if (mem_stat != OK || drop_obj == NULL)
		 {
		     duve_talk (DUVE_ALWAYS, duve_cb, 
			 E_DU3400_BAD_MEM_ALLOC, 0);
		     return ((DU_STATUS) DUVE_BAD);
		 }
		 /*
		 ** add a record to the "fix it" list reminding us to drop this 
		 ** database procedure
		 */
		 drop_obj->duve_obj_id.db_tab_base  = proc_tbl.dbp_id;
		 drop_obj->duve_obj_id.db_tab_index = proc_tbl.dbp_idx;
		 drop_obj->duve_obj_type            = DB_DBP;
		 drop_obj->duve_secondary_id	    = 0;
		 drop_obj->duve_drop_flags	    = DUVE_DBP_DROP;
		 /* link this description into the existing list */
		 drop_obj->duve_next = 
		     duve_cb->duvefixit.duve_objs_to_drop;
		 duve_cb->duvefixit.duve_objs_to_drop = drop_obj;
                 duve_talk(DUVE_MODESENS, duve_cb, S_DU0394_DROP_DBP, 0);
	      } 
	      continue;
	   } /* endif iidbdepends tuple not present for procedure */ 
	} /* end test 4 */
	else
	{
	    /* 
	    ** need to reset proc_dep_offset to DU_INVALID to indicate that we 
	    ** have not yet tried to look up the dbproc in the dependent object
	    ** info list
	    */
	    proc_dep_offset = DU_INVALID;
	}
	/*
	** tests 5 and 6: make sure that an independent object/privilege list 
	**		  for a dbproc exists iff the current record contains an
	**		  indication to that effect
	*/
	if (proc_dep_offset == DU_INVALID)
	{
	    proc_dep_offset = duve_d_dpndfind(proc_tbl.dbp_id, proc_tbl.dbp_idx,
		(i4) DB_DBP, (i4) 0, duve_cb);
	}
	if (proc_dep_offset == DU_INVALID)
	{
	    /* 
	    ** IIDBDEPENDS does not contain any records describing objects on 
	    ** which this dbproc depends - see if you can find any records
	    ** describing privileges on which this dbproc depends in IIPRIV
	    */
/* # line 4002 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk222",6265,12865);
      IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_id);
      IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_idx);
      IIputdomio((short *)0,1,30,2,&mode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk222",6265,12865);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(1)from iipriv where d_obj_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_id);
        IIwritio(0,(short *)0,1,32,0,(char *)"and d_obj_idx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_idx);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and d_priv_number=0 and d_obj_type=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&mode);
        IIexDefine(0,(char *)"duvechk222",6265,12865);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4009 "duvechk2.sc" */	/* host code */
	}
	else
	{
	    /*
	    ** init cnt to 0 so that we don't mistakenly assume that we found
	    ** IIPRIV tuples describing privileges on which this dbproc depends
	    */
	    cnt = 0;
	}
	if (   proc_tbl.dbp_mask1 & DB_DBP_INDEP_LIST
	    && proc_dep_offset == DU_INVALID
	    && cnt == 0)
	{
	    /*
	    ** IIPROCEDURE tuple indicates that there is a list of 
	    ** objects/privileges on which the dbproc depends, but none can be 
	    ** found.  Ask user for permission to mark this database procedure 
	    ** dormant
	    */
            if (duve_banner(DUVE_IIPROCEDURE, 5, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU16E5_MISSING_INDEP_LIST, 4,
		0, proc_tbl.dbp_owner,
		0, proc_tbl.dbp_name);
	    if (duve_talk(DUVE_IO, duve_cb, S_DU0251_MARK_DBP_DORMANT, 0)
		== DUVE_YES)
	    {
		/* 
		** user decided to allow us to mark the dbproc 
		** dormant - add its description to the "fixit" list 
		*/
		drop_obj = (DUVE_DROP_OBJ *) MEreqmem((u_i4) 0, 
		    sizeof(DUVE_DROP_OBJ), TRUE, &mem_stat);
		if (mem_stat != OK || drop_obj == NULL)
		{
		    duve_talk (DUVE_ALWAYS, duve_cb, 
			E_DU3400_BAD_MEM_ALLOC, 0);
		    return ((DU_STATUS) DUVE_BAD);
		}
		drop_obj->duve_obj_id.db_tab_base  = proc_tbl.dbp_id;
		drop_obj->duve_obj_id.db_tab_index = proc_tbl.dbp_idx;
		drop_obj->duve_obj_type            = DB_DBP;
		drop_obj->duve_secondary_id	   = 0;
		drop_obj->duve_drop_flags	   = 0;
		/* link this description into the existing list */
		drop_obj->duve_next = 
		    duve_cb->duvefixit.duve_objs_to_drop;
		duve_cb->duvefixit.duve_objs_to_drop = drop_obj;
		duve_talk(DUVE_MODESENS, duve_cb, 
		    S_DU0451_MARKED_DBP_DORMANT, 0);
	    }
	}
	else if (   ~proc_tbl.dbp_mask1 & DB_DBP_INDEP_LIST
		 && (proc_dep_offset != DU_INVALID || cnt != 0))
	{
	    /*
	    ** IIPROCEDURE tuple indicates that there is no list of 
	    ** objects/privileges on which the dbproc depends, but we found 
	    ** record(s) in IIDBDEPENDS and/or IIPRIV describing objects and/or
	    ** privileges on which this database procedure depends.  Ask user 
	    ** for permission to set DB_DBP_INDEP_LIST in IIPROCEDURE.dbp_mask1
	    */
            if (duve_banner(DUVE_IIPROCEDURE, 6, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU16E6_UNEXPECTED_INDEP_LIST, 4,
		0, proc_tbl.dbp_owner,
		0, proc_tbl.dbp_name);
	    if (duve_talk(DUVE_IO, duve_cb, S_DU0252_SET_DB_DBP_INDEP_LIST, 0)
		== DUVE_YES)
	    {
		proc_tbl.dbp_mask1 |= DB_DBP_INDEP_LIST;
/* # line 4091 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprocedure set dbp_mask1=");
    IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_mask1);
    IIcsERplace((char *)"proc_cursor",6262,20954);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4094 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb,
		    S_DU0452_SET_DB_DBP_INDEP_LIST, 0);
	    }
	}
	/* end of tests 5 and 6 */
	/*
	** test 7: if the IIPROCEDURE tuple contains a description of the 
	**	   dbproc's underlying base table, verify that it exists and 
	**	   is not a catalog.
	*/
	if (proc_tbl.dbp_ubt_id || proc_tbl.dbp_ubt_idx)
	{
	    DB_TAB_ID		ubt_id;
	    ubt_id.db_tab_base = proc_tbl.dbp_ubt_id;
	    ubt_id.db_tab_index = proc_tbl.dbp_ubt_idx;
	    base = duve_findreltid (&ubt_id, duve_cb);
	    if (base == DU_INVALID)
	    {
		/*
		** dbproc's underlying base table no longer exists
		*/
                if (duve_banner( DUVE_IIPROCEDURE, 7, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
	        duve_talk(DUVE_MODESENS, duve_cb, S_DU16E7_MISSING_UBT, 8,
		    sizeof(proc_tbl.dbp_ubt_id), &proc_tbl.dbp_ubt_id,
		    sizeof(proc_tbl.dbp_ubt_idx), &proc_tbl.dbp_ubt_idx,
		    0, proc_tbl.dbp_owner,
		    0, proc_tbl.dbp_name);
	        if (duve_talk(DUVE_IO, duve_cb, S_DU0251_MARK_DBP_DORMANT, 0)
		    == DUVE_YES)
	        {
		    /* 
		    ** user decided to allow us to mark the dbproc 
		    ** dormant - add its description to the "fixit" list 
		    */
		    drop_obj = (DUVE_DROP_OBJ *) MEreqmem((u_i4) 0, 
		        sizeof(DUVE_DROP_OBJ), TRUE, &mem_stat);
		    if (mem_stat != OK || drop_obj == NULL)
		    {
		        duve_talk (DUVE_ALWAYS, duve_cb, 
			    E_DU3400_BAD_MEM_ALLOC, 0);
		        return ((DU_STATUS) DUVE_BAD);
		    }
		    drop_obj->duve_obj_id.db_tab_base  = proc_tbl.dbp_id;
		    drop_obj->duve_obj_id.db_tab_index = proc_tbl.dbp_idx;
		    drop_obj->duve_obj_type            = DB_DBP;
		    drop_obj->duve_secondary_id	       = 0;
		    drop_obj->duve_drop_flags	       = 0;
		    /* link this description into the existing list */
		    drop_obj->duve_next = 
		        duve_cb->duvefixit.duve_objs_to_drop;
		    duve_cb->duvefixit.duve_objs_to_drop = drop_obj;
		    duve_talk(DUVE_MODESENS, duve_cb, 
		        S_DU0451_MARKED_DBP_DORMANT, 0);
	        }
	    }
	    else if (   base != DUVE_DROP
		     && duve_cb->duverel->rel[base].du_stat & TCB_CATALOG
		     && ~duve_cb->duverel->rel[base].du_stat & TCB_EXTCATALOG)
	    {
		/*
		** dbproc's underlying base table is a DBMS catalog
		*/
		if (duve_banner( DUVE_IIPROCEDURE, 7, duve_cb) == DUVE_BAD)
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk(DUVE_MODESENS, duve_cb, S_DU16E8_UBT_IS_A_CATALOG, 8,
		    sizeof(proc_tbl.dbp_ubt_id), &proc_tbl.dbp_ubt_id,
		    sizeof(proc_tbl.dbp_ubt_idx), &proc_tbl.dbp_ubt_idx,
		    0, proc_tbl.dbp_owner,
		    0, proc_tbl.dbp_name);
	        if (duve_talk(DUVE_IO, duve_cb, 
			S_DU0253_UBT_IS_A_DBMS_CATALOG, 0)
		    == DUVE_YES)
	        {
		    proc_tbl.dbp_ubt_id = proc_tbl.dbp_ubt_idx = 0;
/* # line 4186 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iiprocedure set dbp_ubt_id=");
    IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_ubt_id);
    IIwritio(0,(short *)0,1,32,0,(char *)", dbp_ubt_idx=");
    IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_ubt_idx);
    IIcsERplace((char *)"proc_cursor",6262,20954);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4191 "duvechk2.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS, duve_cb,
		        S_DU0453_CLEARED_DBP_UBT_ID, 0);
	        }
	    }
	}
	/* end of test 7 */
	/*
	** test 8: If the procedure is not marked grantable, make sure there 
	**	   are no permits were granted on it
	*/
	if (~proc_tbl.dbp_mask1 & DB_DBPGRANT_OK)
	{
/* # line 4205 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk223",6266,17366);
      IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_id);
      IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_idx);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk223",6266,17366);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(1)from iiprotect where protabbase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_id);
        IIwritio(0,(short *)0,1,32,0,(char *)"and protabidx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&proc_tbl.dbp_idx);
        IIexDefine(0,(char *)"duvechk223",6266,17366);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4211 "duvechk2.sc" */	/* host code */
	    if (cnt != 0)
	    {
		/*
		** dbproc is marked non-grantable, but there are still some 
		** permits defined on it
		*/
                if (duve_banner( DUVE_IIPROCEDURE, 8, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
	        duve_talk(DUVE_MODESENS, duve_cb, 
		    S_DU16E9_UNEXPECTED_DBP_PERMS, 4,
		    0, proc_tbl.dbp_owner,
		    0, proc_tbl.dbp_name);
	        if (duve_talk(DUVE_IO, duve_cb, S_DU0251_MARK_DBP_DORMANT, 0)
		    == DUVE_YES)
	        {
		    /* 
		    ** user decided to allow us to mark the dbproc 
		    ** dormant - add its description to the "fixit" list 
		    */
		    drop_obj = (DUVE_DROP_OBJ *) MEreqmem((u_i4) 0, 
		        sizeof(DUVE_DROP_OBJ), TRUE, &mem_stat);
		    if (mem_stat != OK || drop_obj == NULL)
		    {
		        duve_talk (DUVE_ALWAYS, duve_cb, 
			    E_DU3400_BAD_MEM_ALLOC, 0);
		        return ((DU_STATUS) DUVE_BAD);
		    }
		    drop_obj->duve_obj_id.db_tab_base  	= proc_tbl.dbp_id;
		    drop_obj->duve_obj_id.db_tab_index 	= proc_tbl.dbp_idx;
		    drop_obj->duve_obj_type            	= DB_DBP;
		    drop_obj->duve_secondary_id	   	= 0;
		    drop_obj->duve_drop_flags	        = 0;
		    /* link this description into the existing list */
		    drop_obj->duve_next = 
		        duve_cb->duvefixit.duve_objs_to_drop;
		    duve_cb->duvefixit.duve_objs_to_drop = drop_obj;
		    duve_talk(DUVE_MODESENS, duve_cb, 
		        S_DU0451_MARKED_DBP_DORMANT, 0);
	        }
	    }
	}
    }  /* end loop for each tuple in table */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckproc"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckprotups - run verifydb tests to check system catalog iiprotect.
**
** Description:
**      This routine opens a cursor to walk iiprotect one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iiprotect system catalog.
**
** Inputs:
**      duve_cb                         verifydb control block
**	    duverel                          cached iirelation information
**
** Outputs:
**      duve_cb                         verifydb control block
**	    duverel                         cached iirelation information
**	        du_tbldrop			flag indicating to drop table
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    tuples may be dropped from table iiprotect
**	    tuples may be modified in iirelation
**
** History:
**      29-Aug-1988 (teg)
**          initial creation
**	5-Dec-1988 (teg)
**	    removed test for iitree tuple for permit.
**	21-Feb-1989 (teg)
**	    stopped caching permit qry text id in relinfo
**	18-aug-89  (teg)
**	    fixed bug 7547 - stop assuming permits are only for tables, and also
**	    search iiprocedure for table ids before removing permit.
**	03-jan-89 (neil)
**	    Added checks for event permits.  This is only added to suppress
**	    failures and not to test relationships.  Made sure to include in
**	    fix for bug 7547.
**	02-feb-90 (teg)
**	    if dbp or even permission, then there is no iirelation tuple, so
**	    skip permit checks that test the relationship between iirelation
**	    and iiprotect.
**	06-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**	28-jun-93 (teresa)
**	    Handle special quel objects owned by DBA, which have entries in
**	    iiprotect with txtid1=0 and txtid2=0.
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
**	21-jan-94 (andre)
**	    Added tests 5-12    
**	21-mar-94 (robf)
**          Allow DB_COPY_INTO and DB_COPY_FROM as legal grants for tables
**      02-Jan-1997 (merja01)
**          Changed long to i4 to fix bug 55186 E_LQ000A Conversion error
**          while doing a verifydb on axp.osf.
**	15-mar-2002 (toumi01)
**	    add prodomset11-33 to support 1024 cols/row
**	12-Sep-2004 (schka24)
**	    Don't complain about sequence rows.
**	3-Mar-2008 (kibro01) b119427
**	    Don't suggest dropping the whole of iiprotect if a permit is
**	    found wrong for a database table - just note it (like with
**	    S_DU163D).
[@history_template@]...
*/
DU_STATUS
ckprotups(duve_cb)
DUVE_CB            *duve_cb;
{
    DB_TAB_ID		pro_id;
    DU_STATUS		base;
    long		valid_privs;
    char		*schname_p, *objname_p;
  i4 cnt;
  u_i4 tid, tidx;
  u_i4 basetid, basetidx;
  i4 relstat;
  i4 qtxt_status;
  i4 id1, id2;
  char obj_name[DB_MAXNAME + 1];
  char sch_name[DB_MAXNAME + 1];
  i2 mode;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 4370 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"pro_cursor",6267,27608);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iiprotect");
    IIcsQuery((char *)"pro_cursor",6267,27608);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4372 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iiprotect */
    for (;;)  
    {
	/* get tuple from iiprotect */
/* # line 4376 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"pro_cursor",6267,27608,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).protabbase);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).protabidx);
      IIcsGetio((short *)0,1,30,2,&(pro_tbl).propermid);
      IIcsGetio((short *)0,1,30,2,&(pro_tbl).proflags);
      IIcsGetio((short *)0,1,30,2,&(pro_tbl).protodbgn);
      IIcsGetio((short *)0,1,30,2,&(pro_tbl).protodend);
      IIcsGetio((short *)0,1,30,2,&(pro_tbl).prodowbgn);
      IIcsGetio((short *)0,1,30,2,&(pro_tbl).prodowend);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).proqryid1);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).proqryid2);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).protreeid1);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).protreeid2);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).procrtime1);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).procrtime2);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).proopctl);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).proopset);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).provalue);
      IIcsGetio((short *)0,1,30,2,&(pro_tbl).prodepth);
      IIcsGetio((short *)0,1,32,1,(pro_tbl).probjtype);
      IIcsGetio((short *)0,1,32,1,(pro_tbl).probjstat);
      IIcsGetio((short *)0,1,32,32,(pro_tbl).probjname);
      IIcsGetio((short *)0,1,32,32,(pro_tbl).probjowner);
      IIcsGetio((short *)0,1,32,32,(pro_tbl).prograntor);
      IIcsGetio((short *)0,1,32,32,(pro_tbl).prouser);
      IIcsGetio((short *)0,1,32,16,(pro_tbl).proterm);
      IIcsGetio((short *)0,1,30,2,&(pro_tbl).profill3);
      IIcsGetio((short *)0,1,30,2,&(pro_tbl).progtype);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).proseq);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset1);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset2);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset3);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset4);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset5);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset6);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset7);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset8);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset9);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomseta);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset11);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset12);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset13);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset14);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset15);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset16);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset17);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset18);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset19);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset20);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset21);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset22);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset23);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset24);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset25);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset26);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset27);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset28);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset29);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset30);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset31);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset32);
      IIcsGetio((short *)0,1,30,4,&(pro_tbl).prodomset33);
      IIcsGetio((short *)0,1,32,32,(pro_tbl).proreserve);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4377 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in iiprotect */
	{
/* # line 4379 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"pro_cursor",6267,27608);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4380 "duvechk2.sc" */	/* host code */
	    break;
	}
	pro_id.db_tab_base = pro_tbl.protabbase;
	pro_id.db_tab_index = pro_tbl.protabidx;
	tid = pro_tbl.protabbase;
	tidx = pro_tbl.protabidx;
	/*
	** test 1 -- verify that the type of object on which a permit was
	**	     created is one of those we expect + verify that the 
	**	     the object on which the permit was created exists
	*/
	switch (pro_tbl.probjtype[0])
	{
	    case DBOB_TABLE:
	    case DBOB_VIEW:
		base = duve_findreltid(&pro_id, duve_cb);
		if (base != DU_INVALID && base != DUVE_DROP)
		{
		    schname_p = duve_cb->duverel->rel[base].du_own;
		    objname_p = duve_cb->duverel->rel[base].du_tab;
		}
		break;
	    case DBOB_DBPROC:
/* # line 4408 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk224",6268,21081);
      IIputdomio((short *)0,1,30,4,&pro_tbl.protabbase);
      IIputdomio((short *)0,1,30,4,&pro_tbl.protabidx);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk224",6268,21081);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select dbp_name, dbp_owner from iiprocedure where dbp_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&pro_tbl.protabbase);
        IIwritio(0,(short *)0,1,32,0,(char *)"and dbp_idx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&pro_tbl.protabidx);
        IIexDefine(0,(char *)"duvechk224",6268,21081);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,obj_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,sch_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4414 "duvechk2.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&cnt,8);
  }
/* # line 4416 "duvechk2.sc" */	/* host code */
		base = (cnt == NULL) ? DU_INVALID : DU_SQL_OK;
		if (base != DU_INVALID)
		{
		    STtrmwhite(schname_p = sch_name);
		    STtrmwhite(objname_p = obj_name);
		}
		break;
	    case DBOB_EVENT:
/* # line 4426 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk225",6269,20822);
      IIputdomio((short *)0,1,30,4,&pro_tbl.protabbase);
      IIputdomio((short *)0,1,30,4,&pro_tbl.protabidx);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk225",6269,20822);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select event_name, event_owner from iievent where event_idbase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&pro_tbl.protabbase);
        IIwritio(0,(short *)0,1,32,0,(char *)"and event_idx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&pro_tbl.protabidx);
        IIexDefine(0,(char *)"duvechk225",6269,20822);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,obj_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,sch_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4432 "duvechk2.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&cnt,8);
  }
/* # line 4434 "duvechk2.sc" */	/* host code */
		base = (cnt == NULL) ? DU_INVALID : DU_SQL_OK;
		if (base != DU_INVALID)
		{
		    STtrmwhite(schname_p = sch_name);
		    STtrmwhite(objname_p = obj_name);
		}
		break;
	    case DBOB_SEQUENCE:
/* # line 4444 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk226",6270,20438);
      IIputdomio((short *)0,1,30,4,&pro_tbl.protabbase);
      IIputdomio((short *)0,1,30,4,&pro_tbl.protabidx);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk226",6270,20438);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select seq_name, seq_owner from iisequence where seq_idbase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&pro_tbl.protabbase);
        IIwritio(0,(short *)0,1,32,0,(char *)"and seq_idx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&pro_tbl.protabidx);
        IIexDefine(0,(char *)"duvechk226",6270,20438);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,obj_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,sch_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4450 "duvechk2.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&cnt,8);
  }
/* # line 4452 "duvechk2.sc" */	/* host code */
		base = (cnt == NULL) ? DU_INVALID : DU_SQL_OK;
		if (base != DU_INVALID)
		{
		    STtrmwhite(schname_p = sch_name);
		    STtrmwhite(objname_p = obj_name);
		}
		break;
	    default:
		base = DUVE_BAD_TYPE;
		break;
	}
	if (base == DU_INVALID)
	{
	    char	obj_type[50];
	    i4		type_len;
	    /* this tuple is not in iirelation, iiprocedure or iievent
	    ** so drop from iiprotect */
	    switch (pro_tbl.probjtype[0])
	    {
		case DBOB_TABLE:
		    mode = DB_TABLE; break;
		case DBOB_VIEW:
		    mode = DB_VIEW;  break;
		case DBOB_DBPROC:
		    mode = DB_DBP;   break;
		case DBOB_SEQUENCE:
		    mode = DB_SEQUENCE; break;
		default:
		    mode = DB_EVENT; break;
	    }
	    duve_obj_type((i4) mode, obj_type, &type_len);
	    if (duve_banner( DUVE_IIPROTECT, 1, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU16EB_NO_OBJECT_FOR_PERMIT, 8,
		type_len, obj_type,
		sizeof(pro_tbl.protabbase), &pro_tbl.protabbase, 
		sizeof(pro_tbl.protabidx), &pro_tbl.protabidx, 
		sizeof(pro_tbl.propermid), &pro_tbl.propermid);
	    if (duve_talk(DUVE_ASK, duve_cb,
			S_DU0321_DROP_FROM_IIPROTECT, 4,
			sizeof(tid), &tid, sizeof(tidx), &tidx)
	        == DUVE_YES)
	    {
/* # line 4505 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iiprotect",(char *)"pro_cursor",6267,27608);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4506 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS,duve_cb,
			  S_DU0371_DROP_FROM_IIPROTECT, 4,
			  sizeof(tid), &tid, sizeof(tidx), &tidx);
	    }
	    continue;
	} /* endif object not found */
	else if (base == DUVE_BAD_TYPE)
	{
	    /*
	    ** type of object on which a permit was defined is not one of those
	    ** we would expect to find
	    */
	    if (duve_banner( DUVE_IIPROTECT, 1, duve_cb) == DUVE_BAD)
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk(DUVE_MODESENS, duve_cb, 
		S_DU16EA_UNEXPECTED_PERMOBJ_TYPE, 8,
		0, pro_tbl.probjtype,
		sizeof(pro_tbl.protabbase), &pro_tbl.protabbase,
		sizeof(pro_tbl.protabidx), &pro_tbl.protabidx,
		sizeof(pro_tbl.propermid), &pro_tbl.propermid);
	    if (duve_talk(DUVE_ASK, duve_cb,
			S_DU0321_DROP_FROM_IIPROTECT, 4,
			sizeof(tid), &tid, sizeof(tidx), &tidx)
	        == DUVE_YES)
	    {
/* # line 4534 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iiprotect",(char *)"pro_cursor",6267,27608);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4535 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS,duve_cb,
			  S_DU0371_DROP_FROM_IIPROTECT, 4,
			  sizeof(tid), &tid, sizeof(tidx), &tidx);
	    }
	    continue;
	}
	else if ( base == DUVE_DROP)
	{
	    continue;	/* if table is marked for destruction, stop now */
	}
	/* 
	**  test 2 - if the permit was defined on a table or view, verify that
	**	     IIRELATION indicates that permits are defined on it
	*/
	if (   (   pro_tbl.probjtype[0] == DBOB_TABLE 
		|| pro_tbl.probjtype[0] == DBOB_VIEW)
	    && (duve_cb->duverel->rel[base].du_stat & TCB_PRTUPS) == 0 )
	{
	    /* the relstat in iirelation needs to be modified */
            if (duve_banner( DUVE_IIPROTECT, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1631_NO_PROTECT_RELSTAT, 4,
			0, duve_cb->duverel->rel[base].du_tab,
			0, duve_cb->duverel->rel[base].du_own);
	    if ( duve_talk( DUVE_ASK, duve_cb, S_DU031E_SET_PRTUPS, 0)
	    == DUVE_YES)
	    {
		basetid =  duve_cb->duverel->rel[base].du_id.db_tab_base;
		basetidx =  duve_cb->duverel->rel[base].du_id.db_tab_index;
		relstat = duve_cb->duverel->rel[base].du_stat | TCB_PRTUPS;
		duve_cb->duverel->rel[base].du_stat = relstat;
/* # line 4569 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&relstat);
    IIwritio(0,(short *)0,1,32,0,(char *)"where reltid=");
    IIputdomio((short *)0,1,30,4,&basetid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&basetidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4571 "duvechk2.sc" */	/* host code */
		if (sqlca.sqlcode == 0)
		    duve_talk(DUVE_MODESENS, duve_cb, S_DU036E_SET_PRTUPS, 0);
	    }  /* endif fix relstat */
	} /* endif relstat doesnt indicate permits */
/*
**	test 3 (reported error if there was not an entry in iitree for each
**	entry in iipermit) was removed because there will not be an iitree
**	entry unless there is a "where" clause.	  SQL has been modified to
**	stop using create permit and now uses GRANT (which does not permit a
**	where clause.  The usual case is that there will not be an iitree entry.
*/
	/*********************************************************/
	/* test 4 verify query text used to define permit exists */
	/*********************************************************/
	mode = DB_PROT;
	/* if both qryid1 and qryid2 are zero, this is a special DBA owned
	** quel object protect that PSF has put into iiprotect without any
	** qrytext.  If this special case, skip check #4
	**/
	if ( (pro_tbl.proqryid1==0) && (pro_tbl.proqryid2==0))
	{
	    cnt = 1; /* fake cnt to keep from checking this tuple */
	}
	else
	{
	    id1 = pro_tbl.proqryid1;
	    id2 = pro_tbl.proqryid2;
/* # line 4601 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk227",6271,26044);
      IIputdomio((short *)0,1,30,4,&id1);
      IIputdomio((short *)0,1,30,4,&id2);
      IIputdomio((short *)0,1,30,2,&mode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk227",6271,26044);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select status from iiqrytext where txtid1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&id2);
        IIwritio(0,(short *)0,1,32,0,(char *)"and mode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&mode);
        IIwritio(0,(short *)0,1,32,0,(char *)"and seq=0");
        IIexDefine(0,(char *)"duvechk227",6271,26044);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&qtxt_status);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4606 "duvechk2.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&cnt,8);
  }
/* # line 4607 "duvechk2.sc" */	/* host code */
	}
	if (cnt == 0) 
	{
	    /* missing query text that defined permit */
            if (duve_banner( DUVE_IIPROTECT, 4, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1639_NO_PERMIT_QRYTEXT, 8,
			sizeof(tid), &tid, sizeof(tidx), &tidx,
			0, duve_cb->duverel->rel[base].du_tab,
			0, duve_cb->duverel->rel[base].du_own);
	}
	/*
	** test 5: verify that name of the object and the schema to which it
	**	   belongs which are stored in IIPROTECT match those in the 
	**	   catalog describing the object itself
	*/
	/*
	** need to get rid of trailing blanks in pro_tbl.probjowner and
	** pro_tbl.probjname to make the following comparison meaningful
	*/
	STtrmwhite(pro_tbl.probjowner);
	STtrmwhite(pro_tbl.probjname);
	if (   STcompare(schname_p, pro_tbl.probjowner)
	    || STcompare(objname_p, pro_tbl.probjname))
	{
	    char	obj_type[50];
	    i4		type_len;
	    switch (pro_tbl.probjtype[0])
	    {
		case DBOB_TABLE:
		    mode = DB_TABLE; break;
		case DBOB_VIEW:
		    mode = DB_VIEW;  break;
		case DBOB_DBPROC:
		    mode = DB_DBP;   break;
		case DBOB_SEQUENCE:
		    mode = DB_SEQUENCE; break;
		default:
		    mode = DB_EVENT; break;
	    }
	    duve_obj_type((i4) mode, obj_type, &type_len);
	    /*
	    ** schema and/or object name stored in IIPROTECT do not match thoses
	    ** found in the catalog describing the object itself.
	    */
	    if (duve_banner(DUVE_IIPROTECT, 5, duve_cb) == DUVE_BAD)
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU16EC_NAME_MISMATCH, 10,
		type_len, obj_type,
		0, pro_tbl.probjowner,
		0, pro_tbl.probjname,
		0, schname_p,
		0, objname_p);
	    if (duve_talk( DUVE_ASK, duve_cb, S_DU0254_FIX_OBJNAME_IN_IIPROT, 0)
	        == DUVE_YES)
	    {
		STcopy(schname_p, pro_tbl.probjowner);
		STcopy(objname_p, pro_tbl.probjname);
/* # line 4673 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiprotect set probjow\
ner=");
    IIputdomio((short *)0,1,32,0,pro_tbl.probjowner);
    IIwritio(0,(short *)0,1,32,0,(char *)", probjname=");
    IIputdomio((short *)0,1,32,0,pro_tbl.probjname);
    IIcsERplace((char *)"pro_cursor",6267,27608);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4678 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, 
		    S_DU0454_FIXED_OBJNAME_IN_IIPROT, 0);
	    }
	}
	/* end of test 5 */
	/*
	** test 6 and 7: verify that the privileges specified in proopset and 
	**	         proopctl are valid for the type of object to which this
	**	  	 permit or security_alarm applies.
	*/
        switch (pro_tbl.probjtype[0])
        {
	    case DBOB_TABLE:
	    case DBOB_VIEW:
		/* 
		** bits that may be set depend on whether this tuple represents
		** a permit or a security_alarm
		*/
		if (pro_tbl.proopset & DB_ALARM)
		{
		    valid_privs =   DB_RETRIEVE | DB_REPLACE | DB_DELETE 
				  | DB_APPEND | DB_TEST | DB_AGGREGATE
				  | DB_ALARM | DB_ALMFAILURE | DB_ALMSUCCESS;
		}
		else
		{
		    valid_privs =   DB_RETRIEVE | DB_REPLACE | DB_DELETE
				  | DB_APPEND | DB_TEST | DB_AGGREGATE
				  | DB_COPY_INTO | DB_COPY_FROM  
				  | DB_GRANT_OPTION | DB_CAN_ACCESS; 
		    if (pro_tbl.probjtype[0] == DBOB_TABLE)
			valid_privs |= DB_REFERENCES;
		}
		break;
	    case DBOB_DBPROC:
	        valid_privs = DB_EXECUTE | DB_GRANT_OPTION;
		break;
	    case DBOB_SEQUENCE:
		valid_privs = DB_NEXT | DB_GRANT_OPTION;
		break;
	    default:
	        valid_privs = DB_EVREGISTER | DB_EVRAISE | DB_GRANT_OPTION; 
		break;
        }
	/*
	** pro_tbl.proopset and valid_privs must have more in common than
	** DB_GRANT_OPTION, DB_ALARM, DB_ALMFAILURE, or DB_ALMSUCCESS + 
	** pro_tbl.proopset must be a subset of valid_privs
	*/
	if (   !(  pro_tbl.proopset 
		 & valid_privs 
		 & ~(DB_GRANT_OPTION | DB_ALARM | DB_ALMFAILURE | DB_ALMSUCCESS)
		)
	    || (pro_tbl.proopset & ~valid_privs)
	   )
	{
	    char	obj_type[50];
	    i4		type_len;
	    /*
	    ** proopset specifies privilege which is illegal for the type of 
	    ** object to which this permit or security_alarm applies.
	    */
	    switch (pro_tbl.probjtype[0])
	    {
		case DBOB_TABLE:
		    mode = DB_TABLE; break;
		case DBOB_VIEW:
		    mode = DB_VIEW;  break;
		case DBOB_DBPROC:
		    mode = DB_DBP;   break;
		case DBOB_SEQUENCE:
		    mode = DB_SEQUENCE; break;
		default:
		    mode = DB_EVENT; break;
	    }
	    duve_obj_type((i4) mode, obj_type, &type_len);
	    if (duve_banner( DUVE_IIPROTECT, 6, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU16ED_INVALID_OPSET_FOR_OBJ, 8,
		sizeof(pro_tbl.proopset), &pro_tbl.proopset,
		type_len, obj_type,
		0, schname_p,
		0, objname_p);
	    if (duve_talk(DUVE_ASK, duve_cb,
			S_DU0321_DROP_FROM_IIPROTECT, 4,
			sizeof(tid), &tid, sizeof(tidx), &tidx)
	        == DUVE_YES)
	    {
/* # line 4781 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iiprotect",(char *)"pro_cursor",6267,27608);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4782 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS,duve_cb,
			  S_DU0371_DROP_FROM_IIPROTECT, 4,
			  sizeof(tid), &tid, sizeof(tidx), &tidx);
	        continue;
	    }
	}
	/*
	** pro_tbl.proopctl and valid_privs must have more in common than
	** DB_GRANT_OPTION, DB_ALARM, DB_ALMFAILURE, or DB_ALMSUCCESS + 
	** pro_tbl.proopctl must be a subset of valid_privs
	*/
	if (   !(  pro_tbl.proopctl 
		 & valid_privs 
		 & ~(DB_GRANT_OPTION | DB_ALARM | DB_ALMFAILURE | DB_ALMSUCCESS)
		)
	    || (pro_tbl.proopctl & ~valid_privs)
	   )
	{
	    char	obj_type[50];
	    i4		type_len;
	    /*
	    ** proopctl specifies privilege which is illegal for the type of 
	    ** object to which this permit or security_alarm applies.
	    */
	    switch (pro_tbl.probjtype[0])
	    {
		case DBOB_TABLE:
		    mode = DB_TABLE; break;
		case DBOB_VIEW:
		    mode = DB_VIEW;  break;
		case DBOB_DBPROC:
		    mode = DB_DBP;   break;
		case DBOB_SEQUENCE:
		    mode = DB_SEQUENCE; break;
		default:
		    mode = DB_EVENT; break;
	    }
	    duve_obj_type((i4) mode, obj_type, &type_len);
	    if (duve_banner( DUVE_IIPROTECT, 7, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU16EE_INVALID_OPCTL_FOR_OBJ, 8,
		sizeof(pro_tbl.proopctl), &pro_tbl.proopctl,
		type_len, obj_type,
		0, schname_p,
		0, objname_p);
	    if (duve_talk(DUVE_ASK, duve_cb,
			S_DU0321_DROP_FROM_IIPROTECT, 4,
			sizeof(tid), &tid, sizeof(tidx), &tidx)
	        == DUVE_YES)
	    {
/* # line 4841 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iiprotect",(char *)"pro_cursor",6267,27608);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4842 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS,duve_cb,
			  S_DU0371_DROP_FROM_IIPROTECT, 4,
			  sizeof(tid), &tid, sizeof(tidx), &tidx);
	        continue;
	    }
	}
	/* end of tests 6 and 7 */
	/*
	** tests 8 and 9: verify that info regarding the language used to 
	**		  specify the permit is correct.
	*/
	if (   ~pro_tbl.proopset & DB_ALARM
	    && (pro_tbl.proqryid1 != 0 || pro_tbl.proqryid2 != 0))
	{
	    /* this a permit other than an access permit */
	    if (~qtxt_status & DBQ_SQL && pro_tbl.proflags & DBP_SQL_PERM)
	    {
	        char		obj_type[50];
	        i4		type_len;
	        switch (pro_tbl.probjtype[0])
	        {
		    case DBOB_TABLE:
		        mode = DB_TABLE; break;
		    case DBOB_VIEW:
		        mode = DB_VIEW;  break;
		    case DBOB_DBPROC:
		        mode = DB_DBP;   break;
		    case DBOB_SEQUENCE:
			mode = DB_SEQUENCE; break;
		    default:
		        mode = DB_EVENT; break;
	        }
	        duve_obj_type((i4) mode, obj_type, &type_len);
	        /*
	        ** IIQRYTEXT indicates that the permit was created using QUEL, 
		** but IIPROTECT tuple indicates that it was created using 
		** SQL - assume that IIQRYTEXT knows best
	        */
	        if (duve_banner(DUVE_IIPROTECT, 8, duve_cb) == DUVE_BAD)
		    return ( (DU_STATUS) DUVE_BAD);
	        duve_talk(DUVE_MODESENS, duve_cb, 
		    S_DU16EF_QUEL_PERM_MARKED_AS_SQL, 8,
		    sizeof(pro_tbl.propermid), &pro_tbl.propermid,
		    type_len, obj_type,
		    0, schname_p,
		    0, objname_p);
	        if (duve_talk( DUVE_ASK, duve_cb, 
			S_DU0255_SET_PERM_LANG_TO_QUEL, 0)
	            == DUVE_YES)
	        {
		    pro_tbl.proflags &= ~DBP_SQL_PERM;
/* # line 4901 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiprotect set proflag\
s=");
    IIputdomio((short *)0,1,30,2,&pro_tbl.proflags);
    IIcsERplace((char *)"pro_cursor",6267,27608);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4905 "duvechk2.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS, duve_cb, 
		        S_DU0455_SET_PERM_LANG_TO_QUEL, 0);
	        }
	    }
	    else if (qtxt_status & DBQ_SQL && ~pro_tbl.proflags & DBP_SQL_PERM)
	    {
	        char		obj_type[50];
	        i4		type_len;
	        switch (pro_tbl.probjtype[0])
	        {
		    case DBOB_TABLE:
		        mode = DB_TABLE; break;
		    case DBOB_VIEW:
		        mode = DB_VIEW;  break;
		    case DBOB_DBPROC:
		        mode = DB_DBP;   break;
		    case DBOB_SEQUENCE:
			mode = DB_SEQUENCE; break;
		    default:
		        mode = DB_EVENT; break;
	        }
	        duve_obj_type((i4) mode, obj_type, &type_len);
	        /*
	        ** IIQRYTEXT indicates that the permit was created using SQL, 
		** but IIPROTECT tuple indicates that it was created using 
		** QUEL - assume that IIQRYTEXT knows best
	        */
	        if (duve_banner(DUVE_IIPROTECT, 9, duve_cb) == DUVE_BAD)
		    return ( (DU_STATUS) DUVE_BAD);
	        duve_talk(DUVE_MODESENS, duve_cb, 
		    S_DU16F0_SQL_PERM_MARKED_AS_QUEL, 8,
		    sizeof(pro_tbl.propermid), &pro_tbl.propermid,
		    type_len, obj_type,
		    0, schname_p,
		    0, objname_p);
	        if (duve_talk( DUVE_ASK, duve_cb, 
			S_DU0256_SET_PERM_LANG_TO_SQL, 0)
	            == DUVE_YES)
	        {
		    pro_tbl.proflags |= DBP_SQL_PERM;
/* # line 4950 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiprotect set proflag\
s=");
    IIputdomio((short *)0,1,30,2,&pro_tbl.proflags);
    IIcsERplace((char *)"pro_cursor",6267,27608);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 4954 "duvechk2.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS, duve_cb, 
		        S_DU0456_SET_PERM_LANG_TO_SQL, 0);
	        }
	    }
	}
	/* end of tests 8 and 9 */
	/*
	** test 10: if a description of a permit indicates that it was created 
	** 	    after 6.4, verify that it specifies exactly one privilege
	** test 11: if a description of a permit indicates that it was created 
	**	    in 6.4 or prior, warn the user that he should consider 
	**	    running UPGRADEDB over his database
	** test 12: if the grantor of a permit is not the owner of the object,
	**	    verify that he/she possesses GRANT OPTION FOR privilege 
	**	    being granted
	*/
	if (!(pro_tbl.proopset & (DB_ALARM | DB_CAN_ACCESS)))
	{
	    if (pro_tbl.proflags & DBP_65_PLUS_PERM)
	    {
		long		priv;
		/* 
		** we want to make sure that the permit conveys exactly one 
		** privilege - turn off extraneous bits whicxh may be set in 
		** the permit tuple
		*/
		priv =   pro_tbl.proopset 
		       & ~(DB_GRANT_OPTION | DB_TEST | DB_AGGREGATE);
		if (BTcount((char *) &priv, BITS_IN(priv)) > 1)
		{
	            char	obj_type[50];
	            i4		type_len;
	            switch (pro_tbl.probjtype[0])
	            {
		        case DBOB_TABLE:
		            mode = DB_TABLE; break;
		        case DBOB_VIEW:
		            mode = DB_VIEW;  break;
		        case DBOB_DBPROC:
		            mode = DB_DBP;   break;
			case DBOB_SEQUENCE:
			    mode = DB_SEQUENCE; break;
		        default:
		            mode = DB_EVENT; break;
	            }
	            duve_obj_type((i4) mode, obj_type, &type_len);
	            /*
	            ** IIQRYTEXT indicates that the permit was created 
		    ** after 6.4, but it conveys more than one privilege which 
		    ** would not happen in 6.5/1.0
	            */
	            if (duve_banner(DUVE_IIPROTECT, 10, duve_cb) == DUVE_BAD)
		        return ( (DU_STATUS) DUVE_BAD);
	            duve_talk(DUVE_MODESENS, duve_cb, S_DU16F1_MULT_PRIVS, 8,
		        sizeof(pro_tbl.propermid), &pro_tbl.propermid,
		        type_len, obj_type,
		        0, schname_p,
		        0, objname_p);
	            if (duve_talk( DUVE_ASK, duve_cb, 
			    S_DU0257_PERM_CREATED_BEFORE_10, 0)
	                == DUVE_YES)
	            {
		        pro_tbl.proflags &= ~DBP_65_PLUS_PERM;
/* # line 5026 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iiprotect set proflag\
s=");
    IIputdomio((short *)0,1,30,2,&pro_tbl.proflags);
    IIcsERplace((char *)"pro_cursor",6267,27608);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5030 "duvechk2.sc" */	/* host code */
		        duve_talk(DUVE_MODESENS, duve_cb, 
		            S_DU0457_PERM_CREATED_BEFORE_10, 0);
	            }
		}
	    }
	    /* end of test 10 */
	    /* test 11 */
	    if (~pro_tbl.proflags & DBP_65_PLUS_PERM)
	    {
	        char	obj_type[50];
	        i4	type_len;
	        switch (pro_tbl.probjtype[0])
	        {
		    case DBOB_TABLE:
		        mode = DB_TABLE; break;
		    case DBOB_VIEW:
		        mode = DB_VIEW;  break;
		    case DBOB_DBPROC:
		        mode = DB_DBP;   break;
		    case DBOB_SEQUENCE:
			mode = DB_SEQUENCE; break;
		    default:
		        mode = DB_EVENT; break;
	        }
	        duve_obj_type((i4) mode, obj_type, &type_len);
	        /*
	        ** IIPROTECT indicates that this permit was created prior to 6.5
	        */
	        if (duve_banner(DUVE_IIPROTECT, 11, duve_cb) == DUVE_BAD)
		    return ( (DU_STATUS) DUVE_BAD);
	        duve_talk(DUVE_MODESENS, duve_cb, S_DU16F2_PRE_10_PERMIT, 8,
		    sizeof(pro_tbl.propermid), &pro_tbl.propermid,
		    type_len, obj_type,
		    0, schname_p,
		    0, objname_p);
	    }
	    /* end of test 11 */
	    /* test 12 */
	    /*
	    ** need to trim trailing blanks from pro_tbl.prograntor if the 
	    ** following comparison is to work correctly
	    */
	    STtrmwhite(pro_tbl.prograntor);
	    if (STcompare(pro_tbl.prograntor, pro_tbl.probjowner))
	    {
	        u_i4		attr_map[DB_COL_WORDS];
	        STATUS          mem_stat;
	        bool		satisfied;
		/* 
		** grantor of the permit does not own the object itself - verify
		** that he possesses GRANT OPTION FOR privilege conveyed by the 
		** permit
		**
		** If he does not possess them, we will report this fact to the
		** user and ask for permission to deal with offending object(s)
		** as follows:  
		**   - if the permit was granted on a table, view, or dbevent,
		**     we will mark that object for destruction;
		**   - if the permit was granted on a dbproc, that dbproc will 
		**     be marked dormant
		*/
		attr_map[0] = pro_tbl.prodomset1;
		attr_map[1] = pro_tbl.prodomset2;
		attr_map[2] = pro_tbl.prodomset3;
		attr_map[3] = pro_tbl.prodomset4;
		attr_map[4] = pro_tbl.prodomset5;
		attr_map[5] = pro_tbl.prodomset6;
		attr_map[6] = pro_tbl.prodomset7;
		attr_map[7] = pro_tbl.prodomset8;
		attr_map[8] = pro_tbl.prodomset9;
		attr_map[9] = pro_tbl.prodomseta;
		attr_map[10] = pro_tbl.prodomset11;
		attr_map[11] = pro_tbl.prodomset12;
		attr_map[12] = pro_tbl.prodomset13;
		attr_map[13] = pro_tbl.prodomset14;
		attr_map[14] = pro_tbl.prodomset15;
		attr_map[15] = pro_tbl.prodomset16;
		attr_map[16] = pro_tbl.prodomset17;
		attr_map[17] = pro_tbl.prodomset18;
		attr_map[18] = pro_tbl.prodomset19;
		attr_map[19] = pro_tbl.prodomset20;
		attr_map[20] = pro_tbl.prodomset21;
		attr_map[21] = pro_tbl.prodomset22;
		attr_map[22] = pro_tbl.prodomset23;
		attr_map[23] = pro_tbl.prodomset24;
		attr_map[24] = pro_tbl.prodomset25;
		attr_map[25] = pro_tbl.prodomset26;
		attr_map[26] = pro_tbl.prodomset27;
		attr_map[27] = pro_tbl.prodomset28;
		attr_map[28] = pro_tbl.prodomset29;
		attr_map[29] = pro_tbl.prodomset30;
		attr_map[30] = pro_tbl.prodomset31;
		attr_map[31] = pro_tbl.prodomset32;
		attr_map[32] = pro_tbl.prodomset33;
		duve_check_privs(&pro_id, 
		    (i4) (pro_tbl.proopset | DB_GRANT_OPTION),
		    pro_tbl.prograntor, attr_map, &satisfied);
		if (!satisfied)
		{
	            char	obj_type[50];
	            i4		type_len;
		    /*
		    ** grantor of the permit lacks GRANT OPTION FOR privilege 
		    ** conveyed in this permit; if the permit was granted on 
		    ** table, view, or dbevent, that object will be marked for 
		    ** destruction; if the permit was granted on a dbproc, the 
		    ** dbproc will be marked dormant
		    */
	            switch (pro_tbl.probjtype[0])
	            {
		        case DBOB_TABLE:
		            mode = DB_TABLE; break;
		        case DBOB_VIEW:
		            mode = DB_VIEW;  break;
		        case DBOB_DBPROC:
		            mode = DB_DBP;   break;
			case DBOB_SEQUENCE:
			    mode = DB_SEQUENCE; break;
		        default:
		            mode = DB_EVENT; break;
	            }
	            duve_obj_type((i4) mode, obj_type, &type_len);
	            if (duve_banner(DUVE_IIPROTECT, 12, duve_cb) == DUVE_BAD)
		        return ( (DU_STATUS) DUVE_BAD);
	            duve_talk(DUVE_MODESENS, duve_cb, 
			S_DU16F3_LACKING_GRANT_OPTION, 10,
			0, pro_tbl.prograntor,
		        sizeof(pro_tbl.propermid), &pro_tbl.propermid,
		        type_len, obj_type,
		        0, schname_p,
		        0, objname_p);
		    if (mode == DB_TABLE || mode == DB_VIEW)
		    {
			char tmp_tabname[DB_MAXNAME*2 + 2];
			STprintf(tmp_tabname,ERx("%s.%s"),
				duve_cb->duverel->rel[base].du_own,
				duve_cb->duverel->rel[base].du_tab);
			duve_talk( DUVE_MODESENS, duve_cb, S_DU16F6_INVALID_IIPROTECT, 10,
				sizeof(pro_tbl.protabbase),&pro_tbl.protabbase,
				sizeof(pro_tbl.protabidx),&pro_tbl.protabidx,
				0, pro_tbl.prouser,
				sizeof(pro_tbl.proopset),&pro_tbl.proopset,
				0, tmp_tabname);
		    }
		    else if (mode == DB_DBP)
		    { 
			DUVE_DROP_OBJ		*drop_obj;
			/*
			** before asking user whether we may mark the dbproc 
			** dormant, check the last entry added to the "fix it" 
			** list - if it describes the current procedure, we 
			** will avoid issuing redundant messages
			*/
			if (   !(drop_obj = 
				     duve_cb->duvefixit.duve_objs_to_drop)
			    || drop_obj->duve_obj_id.db_tab_base   != 
				   pro_tbl.protabbase
			    || drop_obj->duve_obj_id.db_tab_index  != 
				   pro_tbl.protabidx
			    || drop_obj->duve_obj_type 		   != DB_DBP
			   )
			{
			    if (duve_talk(DUVE_IO, duve_cb, 
				    S_DU0251_MARK_DBP_DORMANT, 0)
				== DUVE_YES)
			    {
				/* 
				** user decided to allow us to mark the dbproc 
				** dormant - add its description to the "fixit" 
				** list 
				*/
				drop_obj = (DUVE_DROP_OBJ *) MEreqmem((u_i4) 0,
				    sizeof(DUVE_DROP_OBJ), TRUE, &mem_stat);
				if (mem_stat != OK || drop_obj == NULL)
				{
				    duve_talk (DUVE_ALWAYS, duve_cb, 
					E_DU3400_BAD_MEM_ALLOC, 0);
				    return ((DU_STATUS) DUVE_BAD);
				}
				drop_obj->duve_obj_id.db_tab_base  = 
				    pro_tbl.protabbase;
				drop_obj->duve_obj_id.db_tab_index = 
				    pro_tbl.protabidx;
				drop_obj->duve_obj_type            = DB_DBP;
				drop_obj->duve_secondary_id	   = 0;
				drop_obj->duve_drop_flags	   = 0;
				/* 
				** link this description into the existing list 
				*/
				drop_obj->duve_next = 
				    duve_cb->duvefixit.duve_objs_to_drop;
				duve_cb->duvefixit.duve_objs_to_drop = drop_obj;
				duve_talk(DUVE_MODESENS, duve_cb, 
				    S_DU0451_MARKED_DBP_DORMANT, 0);
			    }
			}
		    }
		    else if (mode == DB_EVENT)
		    { 
			DUVE_DROP_OBJ		*drop_obj;
			/*
			** before asking user whether we may drop the dbevent,
			** check the last entry added to the "fix it" list - if
			** it describes the current dbevent, we will avoid 
			** issuing redundant messages
			*/
			if (   !(drop_obj = 
				     duve_cb->duvefixit.duve_objs_to_drop)
			    || drop_obj->duve_obj_id.db_tab_base   != 
				   pro_tbl.protabbase
			    || drop_obj->duve_obj_id.db_tab_index  != 
				   pro_tbl.protabidx
			    || drop_obj->duve_obj_type 		   != DB_EVENT
			   )
			{
			    if (duve_talk(DUVE_IO, duve_cb, 
				    S_DU0258_DROP_DBEVENT, 0)
				== DUVE_YES)
			    {
				/* 
				** user decided to allow us to drop the 
				** dbevent - add its description to the "fixit"
				** list 
				*/
				drop_obj = (DUVE_DROP_OBJ *) MEreqmem((u_i4) 0,
				    sizeof(DUVE_DROP_OBJ), TRUE, &mem_stat);
				if (mem_stat != OK || drop_obj == NULL)
				{
				    duve_talk (DUVE_ALWAYS, duve_cb, 
					E_DU3400_BAD_MEM_ALLOC, 0);
				    return ((DU_STATUS) DUVE_BAD);
				}
				drop_obj->duve_obj_id.db_tab_base  = 
				    pro_tbl.protabbase;
				drop_obj->duve_obj_id.db_tab_index = 
				    pro_tbl.protabidx;
				drop_obj->duve_obj_type            = DB_EVENT;
				drop_obj->duve_secondary_id	   = 0;
				drop_obj->duve_drop_flags	   = 0;
				/* 
				** link this description into the existing list 
				*/
				drop_obj->duve_next = 
				    duve_cb->duvefixit.duve_objs_to_drop;
				duve_cb->duvefixit.duve_objs_to_drop = drop_obj;
				duve_talk(DUVE_MODESENS, duve_cb, 
				    S_DU0458_DROPPED_DBEVENT, 0);
			    }
			}
		    }
		}
	    }
	    /* end of test 12 */
	}
    }  /* end loop for each tuple in table */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckprotups"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckqrytxt - run verifydb checks on the iiqrytext dbms catalog
**
** Description:
**      This routine opens a cursor and examines each tuple in iiqrytext. 
**      It reports errors, but does not attempt to correct them (in any mode).
**	The logic is that the text used to create views, integrities, and
**	permits is valuable.  Even a partial qrytext entry may provide the
**	DBA with enough information to create a view, permit, integrity. 
**
** Inputs:
**      duve_cb                         verifydb control block
**	    duverel                          cached iirelation information
**
** Outputs:
**	none
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
** Side Effects:
**	    none
**
** History:
**      5-Sep-1988 (teg)
**          initial creation
**	20-Oct-1988 (teg)
**	    added check to test 49 for REGISTER, LINK and GATEWAY
**	21-Feb-1989 (teg)
**	    Fixed logic in tests 51 & 52 which used to assume information they
**	    needed was in the RELINFO cache.  This was not adequate because
**	    it had an invalid underlaying assumption that only one
**	    integrity or permit would be defined on a table.
**	04-Apr-1989 (neil)
**	    Added checks for rule text.  At this time this is only
**	    added to suppress failures and not to actually test the
**	    relationships between iiqrytext, and iirule.
**	03-Jan-89 (neil)
**	    Added checks for event text.  This is only added to suppress
**	    failures and not to test relationships.
**	12-Oct-90 (teresa)
**	    Pick Up GW change: 23-May-90 (linda)
**          Changed checks of q.mode in iiqrytext table to reflect changes of
**          constant names for "create...as link", "register...as link" and
**          "register...as import" statements.  *NOTE* when integrating STAR:
**          constant DB_REG_LINK used to be DB_REGISTER; it was changed to
**          reflect is real meaning which is "register...as link".
**	21-feb-91 (teresa)
**	    fix incomplete ifdeffed code for rules and events for test 50.
**	06-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**	27-may-93 (teresa)
**	    Added handling for DBPs (test 8)
**	01-nov-93 (andre)
**	    since we stopped using timestamps in attributes used to join 
**	    IIQRYTEXT/IITREE tuples to tuples in catalogs describing objects 
**	    with which they are affiliated, it is now possible to have a tuple 
**	    containing integrity text to have the same qryid as the tuple 
**	    containing permit text (for the same table).  This should not be a 
**	    problem because the two will have different "mode".  This test will
**	    be modified to NOT treat as error occurrences of text tuples with 
**	    identical qryid if they were created for DIFFERENT object types.
**	    Got rid of a couple of unused variables; folded IF statement into
**	    the switch statement to make things a tad more efficient
**	10-dec-93 (teresa)
**	    rewrite cursor as an outer join to improve performance as this has
**	    been determined to be a very costly portion of verifydb.  Rework
**	    switch statement significantly to handle outer join change.  This
**	    is part of an effort to improve performance for verifydb.  Also
**	    added logic to report peformance statistics
**	20-dec-93 (robf)
**          Add support for security alarms (DB_ALM)
[@history_template@]...
*/
DU_STATUS
ckqrytxt(duve_cb)
DUVE_CB            *duve_cb;
{
#define		IS_NULL		-1
	char		*test;
	i4		qid1,qid2;
	i4		mode;
	i4		msg,qseq;
        DU_STATUS	base;
# include <duveqry.h>
  i4 cnt;
  u_i8 tid8;
  i2 vmode;
  i2 null_i;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 5425 "duvechk2.sc" */	/* host code */
    /* make a union all join of all the objects for which query text may exist.
    ** note that this union view does NOT include views because views are
    ** already cached on the iirelation cache, so way waste the I/Os querying
    ** iirelation for them again...
    */
/* # line 5430 "duvechk2.sc" */	/* declare */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"declare global temporary table session.vdb_qry(id1, id2, vmode)as sel\
ect proqryid1, proqryid2, 19 from iiprotect union all select event_qry\
id1, event_qryid2, 33 from iievent union all select intqryid1, intqryi\
d2, 20 from iiintegrity union all select ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"dbp_txtid1, dbp_txtid2, 87 from iiprocedure union all select rule_qry\
id1, rule_qryid2, 110 from iirule on commit preserve rows with norecov\
ery");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5449 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"qry_cursor",6272,10068);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct q.tid, q.txtid1, q.txtid2, q.mode, q.seq, q.status, v\
.vmode from(iiqrytext q left join session.vdb_qry v on q.txtid1=v.id1 \
and q.txtid2=v.id2 and q.mode=v.vmode)order by q.txtid1, q.txtid2, q.m\
ode, q.seq");
    IIcsQuery((char *)"qry_cursor",6272,10068);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5451 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iiqrytext */
    for (qid1 = qid2 = mode = -1;;)  
    {
	/* get tuple from table */
/* # line 5455 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"qry_cursor",6272,10068,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,8,&tid8);
      IIcsGetio((short *)0,1,30,4,&qry_tbl.txtid1);
      IIcsGetio((short *)0,1,30,4,&qry_tbl.txtid2);
      IIcsGetio((short *)0,1,30,4,&qry_tbl.mode);
      IIcsGetio((short *)0,1,30,4,&qry_tbl.seq);
      IIcsGetio((short *)0,1,30,4,&qry_tbl.status);
      IIcsGetio(&null_i,1,30,2,&vmode);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5458 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in iiqrytext */
	{
/* # line 5460 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"qry_cursor",6272,10068);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5461 "duvechk2.sc" */	/* host code */
	    break;
	}
	/* determine if this is a new query text id */
	if (   (qry_tbl.txtid1 != qid1) 
	    || (qry_tbl.txtid2 != qid2) 
	    || (qry_tbl.mode != mode))
	{
        /* this is the first tuple for this query text entry  */
	    qid1 = qry_tbl.txtid1;
	    qid2 = qry_tbl.txtid2;
	    mode = qry_tbl.mode;
	    qseq = 0;
	    /* test 1 - assure text id is valid */
	    if (qid1 == 0  &&  qid2 == 0)
	    {
		if (duve_banner( DUVE_IIQRYTEXT, 1, duve_cb)
		== DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk(DUVE_MODESENS, duve_cb,S_DU163B_INVALID_QRYTEXT_ID,0);
	    }
	    /* tests 2, 3, 4, 5, 7, 8, 9
	    **  -- assure there is recepient for query text
	    */
	    if (qry_tbl.mode == DB_VIEW)
	    {
		base = duve_qidfind( qid1, qid2, duve_cb);
		if (base == DU_INVALID)
		{   
		    /* test 3 - view corresponding to qrytext does not exist */
		    if (duve_banner( DUVE_IIQRYTEXT, 3, duve_cb) == DUVE_BAD) 
			return ( (DU_STATUS) DUVE_BAD);
		    duve_talk ( DUVE_MODESENS, duve_cb, 
				S_DU163C_QRYTEXT_VIEW_MISSING, 4, 
				sizeof(qid1), &qid1, sizeof(qid2), &qid2);
		}
	    }
	    else if (null_i == (i2) IS_NULL)
	    {
		/* there exists a query text without a corresponding object,
		** so figure out which one and report it -- this works because
		** of the outer join -- if there is a corresponding tuple from
		** the various tables (iiprotect, iievent, iiintegrity,
		** iiprocedure, iirule) then vmode will have the same value as
		** qry_tbl.mode.   If there is no corresponding tuple in the
		** above list of catalogs, then vmode will have a value of NULL.
		** Note that views are NOT handled in this particular query
		** because relation information has already been cached, so we
		** look for views on the relation cache -- vmode will always
		** be null when qry_tbl.mode is DB_VIEW (17).
		*/
		/* NOTE: esqlc puts nulls into a separate indicator, null_i in
		**	 this case.  The value of null_i is set to -1 if there
		**	 is a null indicator and is not guaranteed (except that
		**	 it is NOT -1) if there is not a null indicator.  The
		**	 value of vmode is also not guarenteed from platform to
		**	 platform -- it would be nice if it were guarenteed to
		**	 be zero when there are nulls, or some nice known value.
		**	 Ideally, we'd like to test for
		**		if (qry_tbl.mode != vmode)
		**	 but we cannot do that because of possible FE
		**	 inconsistencies on some platforms, we instead test
		**	 for the null indicator
		*/
		switch (qry_tbl.mode){
		  case DB_PROT:
		    /* test 4 - permit corresponding to qrytext does not exist*/
		    if (duve_banner( DUVE_IIQRYTEXT, 4, duve_cb) == DUVE_BAD) 
			return ( (DU_STATUS) DUVE_BAD);
		    duve_talk ( DUVE_MODESENS, duve_cb, 
				S_DU163D_QRYTEXT_PERMIT_MISSING, 4, 
				sizeof(qid1), &qid1, sizeof(qid2), &qid2);
		    break;
		  case DB_INTG:
		    /* test 5-integrity corresponding to qrytext does not exist */
		    if (duve_banner( DUVE_IIQRYTEXT, 5, duve_cb) == DUVE_BAD) 
			return ( (DU_STATUS) DUVE_BAD);
		    duve_talk ( DUVE_MODESENS, duve_cb, 
				S_DU163E_QRYTEXT_INTEG_MISSING, 4, 
				sizeof(qid1), &qid1, sizeof(qid2), &qid2);
		    break;
		  case DB_DBP:
		    /* test 8 - DBP corresponding to qrytext does not exist */
		    if (duve_banner( DUVE_IIQRYTEXT,8, duve_cb) == DUVE_BAD) 
			return ( (DU_STATUS) DUVE_BAD);
		    duve_talk ( DUVE_MODESENS, duve_cb, 
				S_DU167C_QRYTEXT_DBP_MISSING, 4, 
				sizeof(qid1), &qid1, sizeof(qid2), &qid2);
		    break;
	          case DB_RULE:
		    if (duve_banner( DUVE_IIQRYTEXT, 9, duve_cb) == DUVE_BAD) 
			return ( (DU_STATUS) DUVE_BAD);
		    duve_talk ( DUVE_MODESENS, duve_cb, 
				S_DU1674_QRYTEXT_RULE_MISSING, 4, 
				sizeof(qid1), &qid1, sizeof(qid2), &qid2);
		    break;
	          case DB_ALM:
		    if (duve_banner( DUVE_IIQRYTEXT, 9, duve_cb) == DUVE_BAD) 
			return ( (DU_STATUS) DUVE_BAD);
		    duve_talk ( DUVE_MODESENS, duve_cb, 
			        S_DU16D0_QRYTEXT_ALARM_MISSING, 4, 
				sizeof(qid1), &qid1, sizeof(qid2), &qid2);
			break;
		 case DB_EVENT:
		    /* test 7 - event corresponding to qrytext does not exist */
		    if (duve_banner( DUVE_IIQRYTEXT, 7, duve_cb) == DUVE_BAD) 
			return ( (DU_STATUS) DUVE_BAD);
		    duve_talk ( DUVE_MODESENS, duve_cb, 
				S_DU1675_QRYTEXT_EVENT_MISSING, 4, 
				sizeof(qid1), &qid1, sizeof(qid2), &qid2);
		    break;
		 case DB_CRT_LINK:
		 case DB_REG_LINK:
		 case DB_REG_IMPORT:
		    /* looks like no checking is done for these objects. */
		    break;
		 default:
		    /* test 2 -- text mode is NOT recognizable to dbms */
		    if (duve_banner( DUVE_IIQRYTEXT, 2, duve_cb) == DUVE_BAD) 
			return ( (DU_STATUS) DUVE_BAD);
		    duve_talk(DUVE_MODESENS, duve_cb, S_DU163A_UNKNOWN_QRYMODE,
				6, sizeof(qid1), &qid1, sizeof(qid2), &qid2,
				   sizeof(qry_tbl.mode), &qry_tbl.mode);
		    break;
		} /* end case */
	    } /* endif the object of the query text does not exist */
	}  /* endif start of new text entry */
	/*
	**  the following tests are performed on all tuples that comprise a
	**  query text entry.
	*/
	/* test 2 -- assure text mode is recognizable to dbms */
	if ( (qry_tbl.mode != DB_VIEW) && (qry_tbl.mode != DB_PROT) &&
	     (qry_tbl.mode != DB_CRT_LINK) && (qry_tbl.mode != DB_REG_LINK) &&
	     (qry_tbl.mode != DB_REG_IMPORT) &&
	     (qry_tbl.mode != DB_ALM) &&
	     (qry_tbl.mode != DB_INTG) && (qry_tbl.mode != DB_DBP ) &&
	     (qry_tbl.mode != DB_RULE) && (qry_tbl.mode != DB_EVENT) )
	{
	    if (duve_banner( DUVE_IIQRYTEXT, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU163A_UNKNOWN_QRYMODE, 6,
			    sizeof(qid1), &qid1, sizeof(qid2), &qid2,
			    sizeof(qry_tbl.mode), &qry_tbl.mode);
	}
        /* test 6 -- check for sequence errors */
	if ( qry_tbl.seq == qseq )
	    qseq++;			/* all is well */
	else if (qry_tbl.seq > qseq)
	{
	    /* missing tree sequence tuple */
            if (duve_banner( DUVE_IIQRYTEXT, 6, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU164F_QRYTEXT_SEQUENCE_ERR, 
			6, sizeof(qid1), &qid1, sizeof(qid2), &qid2,
			sizeof(qseq), &qseq);
	    qseq = qry_tbl.seq + 1;
	}
	else
	{
	    /* duplicate tree sequence tuple */
            if (duve_banner( DUVE_IIQRYTEXT, 6, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1650_QRYTEXT_DUP_ERR, 6,
			sizeof(qid1), &qid1, sizeof(qid2), &qid2,
			sizeof(qry_tbl.seq), &qry_tbl.seq);
	    qseq = qry_tbl.seq + 1;
	}
    }  /* end loop for each tuple in table */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckqrytxt");
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckrule - run verifydb tests to check system catalog iirule.
**
** Description:
**      This routine opens a cursor to walk iirule one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iirule system catalog.
**
** Inputs:
**      duve_cb                         verifydb control block
**	    duverel                          cached iirelation information
**
** Outputs:
**      duve_cb                         verifydb control block
**	    duverel                         cached iirelation information
**	        du_tbldrop			flag indicating to drop table
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    tuples may be dropped from table iirule
**	    tuples may be modified in iirelation
**
** History:
**	27-May-93 (teresa)
**	    Initial creation
**	23-jun-93 (teresa)
**	    Fixed test 5 to look for all blanks as well as '\0'
**      10-oct-93 (anitap)
**          Added test to check if corresponding iidbdepends tuple
**          present for the rule tuple. If not present, delete the
**          corresponding rule tuple from iirule.
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
**	20-dec-93 (anitap)
**	    Use iidbdepends cache for test 6 to improve performance.
**	    Corrected test number to 6 in duve_banner().
[@history_template@]...
*/
DU_STATUS
ckrule(duve_cb)
DUVE_CB            *duve_cb;
{
    DB_TAB_ID		rule_id;
    DU_STATUS		base;
# include <duverule.h>
  i4 cnt;
  u_i4 basetid, basetidx;
  i4 relstat;
  i4 id1, id2;
  i2 mode;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);
/* # line 5714 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"rule_cursor",6273,15437);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iirule");
    IIcsQuery((char *)"rule_cursor",6273,15437);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5716 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iirule */
    for (;;)  
    {
	/* get tuple from iirule */
/* # line 5720 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"rule_cursor",6273,15437,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,(rule_tbl).rule_name);
      IIcsGetio((short *)0,1,32,32,(rule_tbl).rule_owner);
      IIcsGetio((short *)0,1,30,2,&(rule_tbl).rule_type);
      IIcsGetio((short *)0,1,30,2,&(rule_tbl).rule_flags);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_tabbase);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_tabidx);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_qryid1);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_qryid2);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_treeid1);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_treeid2);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_statement);
      IIcsGetio((short *)0,1,32,32,(rule_tbl).rule_dbp_name);
      IIcsGetio((short *)0,1,32,32,(rule_tbl).rule_dbp_owner);
      IIcsGetio((short *)0,1,32,25,(rule_tbl).rule_time_date);
      IIcsGetio((short *)0,1,32,25,(rule_tbl).rule_time_int);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col1);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col2);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col3);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col4);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col5);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col6);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col7);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col8);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col9);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_cola);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col11);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col12);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col13);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col14);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col15);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col16);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col17);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col18);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col19);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col20);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col21);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col22);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col23);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col24);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col25);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col26);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col27);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col28);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col29);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col30);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col31);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col32);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_col33);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_id1);
      IIcsGetio((short *)0,1,30,4,&(rule_tbl).rule_id2);
      IIcsGetio((short *)0,1,30,2,&(rule_tbl).rule_dbp_param);
      IIcsGetio((short *)0,1,30,2,&(rule_tbl).rule_pad);
      IIcsGetio((short *)0,1,32,8,(rule_tbl).rule_free);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5721 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in iirule */
	{
/* # line 5723 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"rule_cursor",6273,15437);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5724 "duvechk2.sc" */	/* host code */
	    break;
	}
	rule_id.db_tab_base = rule_tbl.rule_tabbase;
	rule_id.db_tab_index = rule_tbl.rule_tabidx;
	/* test 1 -- verify table receiving rule exists */
	base = duve_findreltid ( &rule_id, duve_cb);
	if ( (base == DU_INVALID) || ( base == DUVE_DROP) )
	{
	    /* this tuple is not in iirelation so drop from iirule */
	    if (duve_banner( DUVE_IIRULE, 1, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb,
			S_DU1682_NO_BASE_FOR_RULE, 4,
			0, rule_tbl.rule_name, 0, rule_tbl.rule_owner);
	    if ( duve_talk(DUVE_IO, duve_cb,
			S_DU0345_DROP_FROM_IIRULE, 4,
			0, rule_tbl.rule_name, 0, rule_tbl.rule_owner)
	    == DUVE_YES)
	    {
/* # line 5747 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iirule",(char *)"rule_cursor",6273,15437);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5748 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS,duve_cb,
			  S_DU0395_DROP_FROM_IIRULE, 4,
			  0, rule_tbl.rule_name, 0, rule_tbl.rule_owner);
	    }
	    continue;
	} /* endif base table not found or is marked for delete */
	/* 
	**  test 2 - verify base table indicates rules are defined on it
	*/
	if ( (duve_cb->duverel->rel[base].du_stat & TCB_RULE) == 0 )
	{
	    /* the relstat in iirelation needs to be modified */
            if (duve_banner( DUVE_IIRULE, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1683_NO_RULE_RELSTAT, 4,
			0, duve_cb->duverel->rel[base].du_tab,
			0, duve_cb->duverel->rel[base].du_own);
	    if ( duve_talk( DUVE_ASK, duve_cb, S_DU0346_SET_RULE, 0)
	    == DUVE_YES)
	    {
		basetid =  duve_cb->duverel->rel[base].du_id.db_tab_base;
		basetidx =  duve_cb->duverel->rel[base].du_id.db_tab_index;
		relstat = duve_cb->duverel->rel[base].du_stat | TCB_PRTUPS;
		duve_cb->duverel->rel[base].du_stat = relstat;
/* # line 5774 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&relstat);
    IIwritio(0,(short *)0,1,32,0,(char *)"where reltid=");
    IIputdomio((short *)0,1,30,4,&basetid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&basetidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5776 "duvechk2.sc" */	/* host code */
		if (sqlca.sqlcode == 0)
		    duve_talk(DUVE_MODESENS, duve_cb, S_DU0396_SET_RULE, 0);
	    }  /* endif fix relstat */
	} /* endif relstat doesnt indicate permits */
	/* test 3 verify query text used to define rule exists */
	mode = DB_RULE;
	id1 = rule_tbl.rule_qryid1;
	id2 = rule_tbl.rule_qryid2;
/* # line 5786 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk228",6274,28089);
      IIputdomio((short *)0,1,30,4,&id1);
      IIputdomio((short *)0,1,30,4,&id2);
      IIputdomio((short *)0,1,30,2,&mode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk228",6274,28089);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(txtid1)from iiqrytext where txtid1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&id2);
        IIwritio(0,(short *)0,1,32,0,(char *)"and mode=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&mode);
        IIexDefine(0,(char *)"duvechk228",6274,28089);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5788 "duvechk2.sc" */	/* host code */
	if (cnt == 0)
	{
	    /* missing query text that defined rule */
	    if (duve_banner( DUVE_IIRULE, 3, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1684_NO_RULE_QRYTEXT, 8,
			sizeof(id1), &id1, sizeof(id2), &id2,
			0, rule_tbl.rule_name, 0, rule_tbl.rule_owner);
	    if ( duve_talk(DUVE_IO, duve_cb,
			S_DU0345_DROP_FROM_IIRULE, 4,
			0, rule_tbl.rule_name, 0, rule_tbl.rule_owner)
	    == DUVE_YES)
	    {
/* # line 5803 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iirule",(char *)"rule_cursor",6273,15437);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5804 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS,duve_cb,
			  S_DU0395_DROP_FROM_IIRULE, 4,
  			  0, rule_tbl.rule_name, 0, rule_tbl.rule_owner);
	    }
	    continue;
	}
	/* test 4 -- verify referenced procedure really exists */
	if (rule_tbl.rule_dbp_name[0])
	{
/* # line 5814 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk229",6275,26167);
      IIputdomio((short *)0,1,32,0,rule_tbl.rule_dbp_name);
      IIputdomio((short *)0,1,32,0,rule_tbl.rule_dbp_owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk229",6275,26167);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(dbp_name)from iiprocedure where dbp_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,rule_tbl.rule_dbp_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and dbp_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,rule_tbl.rule_dbp_owner);
        IIexDefine(0,(char *)"duvechk229",6275,26167);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5817 "duvechk2.sc" */	/* host code */
	    if (cnt== 0)
	    {
		if (duve_banner( DUVE_IIRULE, 4, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk (DUVE_MODESENS, duve_cb,
			   S_DU1685_NO_DBP_FOR_RULE, 8,
			   0, rule_tbl.rule_dbp_name, 0,rule_tbl.rule_dbp_owner,
			   0, rule_tbl.rule_name, 0, rule_tbl.rule_owner);
		/* procedure does not exist, so drop rule */
		if ( duve_talk(DUVE_IO, duve_cb,
			    S_DU0345_DROP_FROM_IIRULE, 4,
			    0, rule_tbl.rule_name, 0, rule_tbl.rule_owner)
		== DUVE_YES)
		{
/* # line 5833 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iirule",(char *)"rule_cursor",6273,15437);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5834 "duvechk2.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS,duve_cb,
			      S_DU0395_DROP_FROM_IIRULE, 4,
			      0, rule_tbl.rule_name, 0, rule_tbl.rule_owner);
		}
		continue;
	    }  /* endif referenced procedure does not exist */
	}  
	/* test 5 - if timebased rule, verify that a time is specified */
	if (rule_tbl.rule_type == DBR_T_TIME)
	{
	    if ( ( (rule_tbl.rule_time_date[0] == '\0')	      &&
		   (rule_tbl.rule_time_int[0] == '\0')		 )  ||
		 ( (STcompare(rule_tbl.rule_time_date,
			      "                         ")==0) &&
		   (STcompare(rule_tbl.rule_time_int,
			      "                         ")==0)    )
	       )
	    {
		/* time interval and date both missing from a time based
		** rule so it will never fire */
		if (duve_banner( DUVE_IIRULE, 5, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk ( DUVE_MODESENS, duve_cb,
			    S_DU1686_NO_TIME_FOR_RULE, 4,
			    0, rule_tbl.rule_name, 0, rule_tbl.rule_owner);
		if ( duve_talk(DUVE_IO, duve_cb,
			    S_DU0345_DROP_FROM_IIRULE, 4,
			    0, rule_tbl.rule_name, 0, rule_tbl.rule_owner)
		== DUVE_YES)
		{
/* # line 5866 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iirule",(char *)"rule_cursor",6273,15437);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5867 "duvechk2.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS,duve_cb,
			      S_DU0395_DROP_FROM_IIRULE, 4,
			      0, rule_tbl.rule_name, 0, rule_tbl.rule_owner);
		}
		continue;
	    }
	} /* endif time/interval missing */
	/* test 6 -- verify iidbdepends tuple exists for the
        ** rule tuple.
        */
	/* check for WCO rule by checking rule_flags.
	** Rule for WCO does not have entry in iidbdepends.
	** So do not delete that rule.
	** Rules supporting constraints are system generated and are not
	** user droppable.
	** Rules created by users do not have corresponding tuples in
	** iidbdepends.
 	*/
	if (((rule_tbl.rule_flags & 
		DBR_F_CASCADED_CHECK_OPTION_RULE) == 0) &&
		((rule_tbl.rule_flags & DBR_F_NOT_DROPPABLE) != 0) && 
                ((rule_tbl.rule_flags &	DBR_F_SYSTEM_GENERATED) != 0))
	{
	   DU_STATUS 	rule_dep_offset;
	   rule_dep_offset = duve_d_dpndfind(rule_tbl.rule_id1,
				rule_tbl.rule_id2, (i4) DB_RULE, 
				(i4) 0, duve_cb);	
	   if (rule_dep_offset == DU_INVALID)
	   {
	       /* this tuple is not in iidbdepends so drop from iirule */
	       if (duve_banner( DUVE_IIRULE, 6, duve_cb) == DUVE_BAD)
                  return ( (DU_STATUS) DUVE_BAD);
	       duve_talk( DUVE_MODESENS, duve_cb, S_DU16A8_IIDBDEPENDS_MISSING,
		 4, 0, rule_tbl.rule_name, 0, rule_tbl.rule_owner);
	       if ( duve_talk(DUVE_IO, duve_cb,
		S_DU0345_DROP_FROM_IIRULE, 4,
                        0, rule_tbl.rule_name, 0, rule_tbl.rule_owner)
            		== DUVE_YES)
               {
/* # line 5914 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iirule",(char *)"rule_cursor",6273,15437);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 5915 "duvechk2.sc" */	/* host code */
                  duve_talk(DUVE_MODESENS, duve_cb,
                         S_DU0395_DROP_FROM_IIRULE, 4,
                         0, rule_tbl.rule_name, 0, rule_tbl.rule_owner);
               }
               continue;
	    } /* endif corresponding iidbdepends tuple does not exist */
	} /* end test 6 */
    }  /* end loop for each tuple in table */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckrule"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckstat	- run verifydb tests to check system catalog iistatistics
**	         
**
** Description:
**      This routine opens a cursor to walk iistatistics one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iistatistics system catalog.
**
**	It allocates memory to keep track of statistic and histogram info
**	in order to reduct the number of disk accesses required for checking
**	the catalogs.  It operates under the assumption that the catalog will
**	normally be healthy.  Thus it gathers info from iistatistics, then
**	test ckhist gathers info from iihistogram.  If any tuples are to be
**	deleted, this is cached, and deletion occurs after all tuples from
**	both tables are examined.  Function ckhist may cause iistatistics
**	tuples to be updated (snumcels modified) or to delete tuples
**	from iistatistics.  Function ckhist may also delete tuples from
**	iihistogram.
**
**	Since there are several error exist possible, and all of them exit
**	through duve_close, duve_close will be responsible for deallocating
**	any memory that was allocated by ckstat.
**
**
**
** Inputs:
**      duve_cb                         verifydb control block
**	    duverel                          cached iirelation information
**
** Outputs:
**      duve_cb                         verifydb control block
**	    duverel                         cached iirelation information
**	        du_tbldrop			flag indicating to drop table
**	    duvestat			    cached iistatistics information
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    tuples read from iistatistics may be modified
**	    tuples in iirelation may be modified
**	    view/prot/etc that tree defines may be dropped from database
**
** History:
**      29-aug-88   (teg)
**          initial creation
**	06-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
**	07-jan-94 (teresa)
**	    dont open cursor until we're sure there are tuples in iistatistics
**	26-jun-2000 (thaju02)
**	    Upon entry, initialize duve_cb->duve_scnt to zero. (b101942)
[@history_template@]...
*/
DU_STATUS
ckstat(duve_cb)
DUVE_CB            *duve_cb;
{
    DB_TAB_ID	   statid;
    i4	   repeat;
    DU_STATUS	   base;
    i2		   atno = -1;
    u_i4		size;
    STATUS		mem_stat;
# include <duvestat.h>
  i4 cnt;
  u_i4 tid, tidx;
  u_i4 basetid, basetidx;
  i2 satno;
  i4 relstat;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
    duve_cb -> duve_scnt = 0;	/* will be incremented before it is used */
/* # line 6024 "duvechk2.sc" */	/* host code */
    /* allocate memory for duvestat */
/* # line 6025 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(stabbase)from iistatistics");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6026 "duvechk2.sc" */	/* host code */
    if ( (sqlca.sqlcode == 100) || (cnt == NULL) )
    {	
	return ( (DU_STATUS) DUVE_NO);
    }
    size = cnt * sizeof(DU_STAT);
    duve_cb->duvestat = (DUVE_STAT *) MEreqmem( (u_i4)0, size, TRUE, &mem_stat);
    if ( (mem_stat != OK) || (duve_cb->duvestat == 0) )
    {
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	return ( (DU_STATUS) DUVE_BAD);
    }
/* # line 6040 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"stat_cursor",6276,28740);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select * from iistatistics order by stabbase, stabindex, satno");
    IIcsQuery((char *)"stat_cursor",6276,28740);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6042 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iistatistics */
    for (statid.db_tab_base= -1,statid.db_tab_index= -1;;
	 duve_cb->duve_scnt++, repeat++)
    {
	/* get tuple from table */
/* # line 6047 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"stat_cursor",6276,28740,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(stat_tbl).stabbase);
      IIcsGetio((short *)0,1,30,4,&(stat_tbl).stabindex);
      IIcsGetio((short *)0,1,31,4,&(stat_tbl).snunique);
      IIcsGetio((short *)0,1,31,4,&(stat_tbl).sreptfact);
      IIcsGetio((short *)0,1,31,4,&(stat_tbl).snull);
      IIcsGetio((short *)0,1,30,2,&(stat_tbl).satno);
      IIcsGetio((short *)0,1,30,2,&(stat_tbl).snumcells);
      IIcsGetio((short *)0,1,30,2,&(stat_tbl).sdomain);
      IIcsGetio((short *)0,1,30,2,&(stat_tbl).scomplete);
      IIcsGetio((short *)0,1,30,2,&(stat_tbl).sunique);
      IIcsGetio((short *)0,1,32,25,(stat_tbl).sdate);
      IIcsGetio((short *)0,1,32,8,(stat_tbl).sversion);
      IIcsGetio((short *)0,1,30,2,&(stat_tbl).shistlength);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6048 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in table */
	{
/* # line 6050 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"stat_cursor",6276,28740);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6051 "duvechk2.sc" */	/* host code */
	    break;
	}
	/* determine if this is the 1st tuple of statistics for this particular
	** iirelation entry -- of course, there can be multiple statistics on
	** a single table, one for each attribute 
	*/
	if ( (stat_tbl.stabbase != statid.db_tab_base) ||
	     (stat_tbl.stabindex != statid.db_tab_index) ||
	     (stat_tbl.satno != atno) )
	{
	    /* this is the first statistics tuple for this table */
	    statid.db_tab_base  = stat_tbl.stabbase;
	    statid.db_tab_index = stat_tbl.stabindex;
	    atno = stat_tbl.satno;
	    base = duve_findreltid ( &statid, duve_cb);
	    repeat = 0;
	    /* 
	    **  copy pertentent info to the cache 
	    */
	    duve_cb->duvestat->stats[duve_cb->duve_scnt].du_rptr = base;
	    duve_cb->duvestat->stats[duve_cb->duve_scnt].du_satno = stat_tbl.satno;
	    duve_cb->duvestat->stats[duve_cb->duve_scnt].du_sid.db_tab_base =
							    stat_tbl.stabbase;
	    duve_cb->duvestat->stats[duve_cb->duve_scnt].du_sid.db_tab_index =
							    stat_tbl.stabindex;
	}
	/* if table receiving statistics is to be dropped, then dont bother
	** checking statistics on it 
	*/
	if (base == DUVE_DROP)
	    continue;
	if (! repeat)
	{
	    /* test 1 - verify table receiving statistics really exists */
	    if (base == DU_INVALID)
	    {   
		if (duve_banner( DUVE_IISTATISTICS, 1, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk ( DUVE_MODESENS, duve_cb, S_DU164C_NO_BASE_FOR_STATS, 
			4, sizeof(stat_tbl.stabbase), &stat_tbl.stabbase,
			sizeof(stat_tbl.stabindex), &stat_tbl.stabindex);
		if ( duve_talk(DUVE_ASK, duve_cb, 
			S_DU0326_DROP_FROM_IISTATISTICS, 4,
			sizeof(stat_tbl.stabbase), &stat_tbl.stabbase,
			sizeof(stat_tbl.stabindex), &stat_tbl.stabindex)
		== DUVE_YES)
		{
		    /* indicate to delete when done with cursor */
		    duve_cb->duvestat->stats[duve_cb->duve_scnt].du_sdrop=
			DUVE_DROP;
		    duve_talk(DUVE_MODESENS, duve_cb, 
			S_DU0376_DROP_FROM_IISTATISTICS, 4,
			sizeof(stat_tbl.stabbase), &stat_tbl.stabbase,
			sizeof(stat_tbl.stabindex), &stat_tbl.stabindex);
		    base = DUVE_DROP;
		}
		continue;
	    }
	    /* test 2 - verify table recieving statistics is not index */
	    if (stat_tbl.stabindex != 0)
	    {
		if (duve_banner( DUVE_IISTATISTICS, 2, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk(DUVE_MODESENS, duve_cb, S_DU1649_STATS_ON_INDEX, 8,
			0, duve_cb->duverel->rel[base].du_tab,
			0, duve_cb->duverel->rel[base].du_own,
			sizeof(stat_tbl.stabbase), &stat_tbl.stabbase,
			sizeof(stat_tbl.stabindex), &stat_tbl.stabindex);
		if ( duve_talk( DUVE_ASK, duve_cb,
			S_DU0326_DROP_FROM_IISTATISTICS, 4,
			sizeof(stat_tbl.stabbase), &stat_tbl.stabbase,
			sizeof(stat_tbl.stabindex), &stat_tbl.stabindex)
		== DUVE_YES)
		{
		    /* indicate to delete when done with cursor */
		    duve_cb->duvestat->stats[duve_cb->duve_scnt].du_sdrop=
			DUVE_DROP;
		    duve_talk(DUVE_MODESENS, duve_cb, 
			S_DU0376_DROP_FROM_IISTATISTICS, 4,
			sizeof(stat_tbl.stabbase), &stat_tbl.stabbase,
			sizeof(stat_tbl.stabindex), &stat_tbl.stabindex);
		    base = DUVE_DROP;
		    continue;
		}
	    }
	    tid = statid.db_tab_base;
	    tidx= statid.db_tab_index;
	    /* test 3 - verify base table knows it has optimizer statistics */
	    if ( ( duve_cb->duverel->rel[base].du_stat & TCB_ZOPTSTAT) == 0)
	    {
		if (duve_banner( DUVE_IISTATISTICS, 3, duve_cb)
		== DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk ( DUVE_MODESENS, duve_cb, S_DU1647_NO_OPSTAT_IN_BASE,
			    4, 0, duve_cb->duverel->rel[base].du_tab,
			    0, duve_cb->duverel->rel[base].du_own);
		if ( duve_talk( DUVE_ASK, duve_cb, S_DU0327_SET_OPTSTAT, 0)
		== DUVE_YES)
		{
		    basetid =  duve_cb->duverel->rel[base].du_id.db_tab_base;
		    basetidx =  duve_cb->duverel->rel[base].du_id.db_tab_index;
		    relstat = duve_cb->duverel->rel[base].du_stat |TCB_ZOPTSTAT;
		    duve_cb->duverel->rel[base].du_stat = relstat;
/* # line 6163 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&relstat);
    IIwritio(0,(short *)0,1,32,0,(char *)"where reltid=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&tidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6165 "duvechk2.sc" */	/* host code */
		    if (sqlca.sqlcode == 0)
			duve_talk(DUVE_MODESENS, duve_cb,S_DU0377_SET_OPTSTAT,0);
		}
	    } /* end test 3 */
	} /* endif 1st statistic tuple for this table */
	/* test 4 -  verify statistic is for a valid column */
	if ( (stat_tbl.satno > duve_cb->duverel->rel[base].du_attno) ||
	     (stat_tbl.satno == 0) )
	{
		if (duve_banner( DUVE_IISTATISTICS, 4, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk (DUVE_MODESENS, duve_cb,
			   S_DU1646_INVALID_STATISTIC_COL, 8,
			   0, duve_cb->duverel->rel[base].du_tab,
			   0, duve_cb->duverel->rel[base].du_own,
			   sizeof(stat_tbl.satno), &stat_tbl.satno,
			   sizeof(duve_cb->duverel->rel[base].du_attno),
			   &duve_cb->duverel->rel[base].du_attno  );
		if (duve_talk(DUVE_ASK, duve_cb,
			S_DU0326_DROP_FROM_IISTATISTICS, 4,
			sizeof(stat_tbl.stabbase), &stat_tbl.stabbase,
			sizeof(stat_tbl.stabindex), &stat_tbl.stabindex)
		== DUVE_YES)
		{
		    /* indicate to delete when done with cursor */
		    duve_cb->duvestat->stats[duve_cb->duve_scnt].du_sdrop=
			DUVE_DROP;
		    duve_talk(DUVE_MODESENS, duve_cb, 
			S_DU0376_DROP_FROM_IISTATISTICS, 4,
			sizeof(stat_tbl.stabbase), &stat_tbl.stabbase,
			sizeof(stat_tbl.stabindex), &stat_tbl.stabindex);
		    base = DUVE_DROP;
		    continue;
		}
	}
	/* test 5 - verify there really is a histogram if one is expected */
/* # line 6205 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk230",6277,22216);
      IIputdomio((short *)0,1,30,4,&tid);
      IIputdomio((short *)0,1,30,4,&tidx);
      IIputdomio((short *)0,1,30,2,&stat_tbl.satno);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk230",6277,22216);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(hatno)from iihistogram where htabbase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&tid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and htabindex=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&tidx);
        IIwritio(0,(short *)0,1,32,0,(char *)"and hatno=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&stat_tbl.satno);
        IIexDefine(0,(char *)"duvechk230",6277,22216);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6208 "duvechk2.sc" */	/* host code */
	if ( (cnt == 0) && (stat_tbl.snumcells) )
	{
	    if (duve_banner( DUVE_IISTATISTICS, 5, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);	    
	    duve_talk(DUVE_MODESENS, duve_cb, S_DU1648_MISSING_HISTOGRAM,6,
		   0, duve_cb->duverel->rel[base].du_tab,
		   0, duve_cb->duverel->rel[base].du_own,
		   sizeof(stat_tbl.satno), &stat_tbl.satno);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU0328_FIX_SNUMCELLS, 10,
		   sizeof(stat_tbl.snumcells), &stat_tbl.snumcells,
		   sizeof(cnt), &cnt,
		   0, duve_cb->duverel->rel[base].du_tab,
		   0, duve_cb->duverel->rel[base].du_own,
		   sizeof(stat_tbl.satno), &stat_tbl.satno) == DUVE_YES)
	    {
	        /* clean snumcells */
/* # line 6224 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update iistatistics set snumcells=");
    IIputdomio((short *)0,1,30,4,&cnt);
    IIwritio(0,(short *)0,1,32,0,(char *)"where stabbase=");
    IIputdomio((short *)0,1,30,4,&stat_tbl.stabbase);
    IIwritio(0,(short *)0,1,32,0,(char *)"and stabindex=");
    IIputdomio((short *)0,1,30,4,&stat_tbl.stabindex);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6227 "duvechk2.sc" */	/* host code */
	        duve_talk(DUVE_MODESENS, duve_cb, S_DU0378_FIX_SNUMCELLS,10,
		   sizeof(stat_tbl.snumcells), &stat_tbl.snumcells,
		   sizeof(cnt), &cnt,
		   0, duve_cb->duverel->rel[base].du_tab,
		   0, duve_cb->duverel->rel[base].du_own,
		   sizeof(stat_tbl.satno), &stat_tbl.satno) ;
	    }
	}
    } /* end loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckstat"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: cksynonym - run verifydb tests to check system catalog iisynonym.
**
** Description:
**      This routine opens a cursor to walk iisynonym one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iisynonym system catalog.
**
**	TEST 1 : Verify that there are not any tables in iirelation with the
**		 same name and owner as the synonym
**	        TEST:   Select count(relid) from iirelation r, iisyonym s where
**			r.relid = s.synonym_name and
**			r.relowner = s.synonym_owner.
**			If count is non-zero, then we have detected error.
**	        FIX:    Delete the synonym tuple.
**
**	TEST 2 : Verify that there really is an iirelation object (table, view
**		 or index) that the synonym resolves to.
**	        TEST:   select count(relid) from iirelation r, iisynonym s where
**			r.reltid = s.syntabbase and r.reltidx = s.syntabidx.
**			If count is not exactly 1, then there is a problem.
**	        FIX:    Delete the synonym tuple.
**
**	TEST 3: Verify that there is a corresponding schema for the synonym
**              owner.
**              TEST:   Select repeat any(schema_id)
**			from iischema s, iisynonym sy
**                      where sy.synonym_owner = s.schema_name.
**
** Inputs:
**      duve_cb                         verifydb control block
**	    duverel                          cached iirelation information
**
** Outputs:
**	none
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    tuples may be dropped from table iisynonym
**	    tuples will be read from iischema	
**
** History:
**      12-jun-90 (teresa)
**          initial creation
**	06-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**      16-jun-93 (anitap)
**          Added test for CREATE SCHEMA project.
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
**	20-dec-93 (anitap)
**	    Cache schema information to create schemas for orphaned objects.
**	07-jan-94 (teresa)
**	    change logic to 1) exit if there are no tuples (since above chagne
**	    now counts tuples) and 2) don't open cursor until after memory
**	    is allocated.
[@history_template@]...
*/
DU_STATUS
cksynonym(duve_cb)
DUVE_CB            *duve_cb;
{
    DB_TAB_ID		int_id;
    DU_STATUS		base;
    char		tabid[DB_MAXNAME+1];
    u_i4		size;
    STATUS		mem_stat;
# include <duvesyn.h>
  i4 cnt;
  u_i4 tid, tidx;
  char tabowner[DB_MAXNAME+1];
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 6334 "duvechk2.sc" */	/* host code */
    /* allocate memory to hold add commands */	
/* # line 6335 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(synonym_name)from iisynonym");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6337 "duvechk2.sc" */	/* host code */
    duve_cb->duve_sycnt = 0;
    if (cnt == 0)
	return DUVE_NO;
    else
        size = cnt * sizeof(DU_SYNONYM);
    duve_cb->duvesyns = (DUVE_SYNONYM *) MEreqmem( (u_i4)0, size, TRUE, 
			&mem_stat);
    if ( (mem_stat != OK) || (duve_cb->duvesyns == 0))
    {
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
        return( (DU_STATUS) DUVE_BAD);
    }	
/* # line 6355 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"syn_cursor",6278,24771);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iisynonym");
    IIcsQuery((char *)"syn_cursor",6278,24771);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6357 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iisynonym */
    for (;;)  
    {
	/* get tuple from iisynonym */
/* # line 6361 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"syn_cursor",6278,24771,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,(syn_tbl).synonym_name);
      IIcsGetio((short *)0,1,32,32,(syn_tbl).synonym_owner);
      IIcsGetio((short *)0,1,30,4,&(syn_tbl).syntabbase);
      IIcsGetio((short *)0,1,30,4,&(syn_tbl).syntabidx);
      IIcsGetio((short *)0,1,30,4,&(syn_tbl).synid);
      IIcsGetio((short *)0,1,30,4,&(syn_tbl).synidx);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6362 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in iisynonym */
	{
/* # line 6364 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"syn_cursor",6278,24771);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6365 "duvechk2.sc" */	/* host code */
	    break;
	}
	/* test 1 -- verify synonym name is unique */
	/* first, take 32 char synonym name, synonym owner and force into string
	** DB_NAXNAME char long.
	**
	** FIXME -- if DB_MAXNAME is extended to exceed 32 char and the
	**	    iisynonym names are not likewise extended, this will cause
	**	    an AV here.  Of course, that is very unlikely to occur.
	*/
	MEcopy( (PTR)syn_tbl.synonym_name, DB_MAXNAME, (PTR)tabid);
	MEcopy( (PTR)syn_tbl.synonym_owner, DB_MAXNAME, (PTR)tabowner);
	tabid[DB_MAXNAME]='\0';
	tabowner[DB_MAXNAME]='\0';
	STtrmwhite(tabid);
	STtrmwhite(tabowner);
	/* now search cache of iirelation tuple info for name,owner match */
	base = duve_relidfind (tabid, tabowner, duve_cb);
	if (base != DU_INVALID)
	{
	    /* this name,owner pair is in iirelation, so drop from iisynonym */
            if (duve_banner( DUVE_IISYNONYM, 1, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU166F_NONUNIQUE_SYNONYM, 4,
			0, syn_tbl.synonym_name, 0, syn_tbl.synonym_owner);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU033C_DROP_IISYNONYM, 0)
	    == DUVE_YES)
	    {
/* # line 6397 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iisynonym",(char *)"syn_cursor",6278,24771);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6398 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU038C_DROP_IISYNONYM, 0);
		continue;
	    }
	} /* endif base table not found */
	/* test 2 -- verify table for synonym exists */
	int_id.db_tab_base = syn_tbl.syntabbase;
	int_id.db_tab_index = syn_tbl.syntabidx;
	base = duve_findreltid ( &int_id, duve_cb);
	if ( (base == DU_INVALID) || ( base == DUVE_DROP) )
	{
	    /* this tuple is not in iirelation, so drop from iisynonym */
            if (duve_banner( DUVE_IISYNONYM, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1670_INVALID_SYNONYM, 8,
			0, syn_tbl.synonym_name, 0, syn_tbl.synonym_owner,
			sizeof(syn_tbl.syntabbase), &syn_tbl.syntabbase,
			sizeof(syn_tbl.syntabidx), &syn_tbl.syntabidx);
	    if ( duve_talk(DUVE_ASK, duve_cb, S_DU033C_DROP_IISYNONYM, 0)
	    == DUVE_YES)
	    {
/* # line 6421 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iisynonym",(char *)"syn_cursor",6278,24771);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6422 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS, duve_cb, S_DU038C_DROP_IISYNONYM, 0);
	    }
	    continue;
	} /* endif base table not found */
	/* test 3 -- verify there is a corresponding schema in iischema */
        /* now search for iischema tuple for owner match */
/* # line 6430 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk231",6279,17985);
      IIputdomio((short *)0,1,32,0,tabowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk231",6279,17985);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(schema_id)from iischema where schema_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tabowner);
        IIexDefine(0,(char *)"duvechk231",6279,17985);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6434 "duvechk2.sc" */	/* host code */
        if (cnt == NULL)
        {
           /* corresponding entry missing in iischema */
	   if (duve_banner(DUVE_IISYNONYM, 3, duve_cb)
                  == DUVE_BAD)
                return ( (DU_STATUS) DUVE_BAD);
           duve_talk (DUVE_MODESENS, duve_cb, S_DU168D_SCHEMA_MISSING, 2,
                        0, syn_tbl.synonym_owner);
	   if ( duve_talk(DUVE_IO, duve_cb, S_DU0245_CREATE_SCHEMA, 0)
			== DUVE_YES)
	   {	
              /* cache info about this tuple */
              MEcopy( (PTR)syn_tbl.synonym_owner, DB_MAXNAME,
           	(PTR) duve_cb->duvesyns->syns[duve_cb->duve_sycnt].du_schname);
	      duve_cb->duve_sycnt++;
              duve_talk(DUVE_MODESENS, duve_cb, S_DU0445_CREATE_SCHEMA, 0);
	   }
	   continue;
        } /* end test 3 */
    }  /* end loop for each tuple in table */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "cksynonym"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: cktree	- run verifydb tests to check system catalog iitree
**
** Description:
**      This routine opens a cursor to walk iitree one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iitree system catalog.
**
**	It allocates memory to keep track of any iitree tuples that should be
**	deleted from the iitree table (identified by table id).  After it has
**	finished walking the table via cursor, it deletes any iitree tuples
**	marked for deletion.  If the deleted tree entry defines a view, it marks
**	that for deletion also.  cktree is a good housekeeper, it deallocates
**	the memory it was using once it is done with it.
**
**	Function cktree is able to efficiently handle tree entries that span
**	across more than 1 iitree tuple.  It does this by:
**	    1. keeping track of whether or not it has seen this tree's table
**		id before.  If so, it skips cross referencing it against the
**		cached iirelation table.
**	    2. Skipping all futher tests on the tree entry once it has been
**		marked for deletion.
**
** Inputs:
**      duve_cb                         verifydb control block
**	    duverel                          cached iirelation information
**
** Outputs:
**      duve_cb                         verifydb control block
**	    duverel                         cached iirelation information
**	        du_tbldrop			flag indicating to drop table
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    tuples read from iitree
**	    tuples in iirelation may be modified
**	    view/prot/etc that tree defines may be dropped from database
**
** History:
**      29-aug-88   (teg)
**          initial creation
**	21-feb-89   (teg)
**	    fix bug where verifydb unable to handle two different trees on the
**	    same table (where more than one permit defined for same table, or
**	    more than one integrity), etc.
**	    Also, stop caching information about stored procedures, as the
**	    trees for stored procedures are no longer put into iitree.
**	04-apr-89 (neil)
**	    Added checks for rule trees.  At this time this is only
**	    added to suppress failures and not to actually test the
**	    relationships between iitree, and iirule.
**	03-jan-89 (neil)
**	    Added checks for event trees.  At this time there are really no
**	    event trees (these are expected with event enhancements).  The
**	    checks were added to suppress failures and not to test.
**	26-oct-90 (teresa)
**	    Skip all other iitree checks if test 35 fails but the fix option
**	    is NOT selected.
**	30-oct-90 (teresa)
**	    fix bug where S_DU036C_DEL_TREE was reported with DUVE_ASK message
**	    mode instead of DUVE_MODESENS.
**	11-feb-91 (teresa)
**	    Implement ingres6302p change 340465
**	15-feb-91 (teresa)
**	    fix bug 35940 (assumed there was only 1 tree entry per table id,
**	    which is invalid assumption.  ex: define multiple QUEL permits on
**	    the same table.)
**	01-oct-91 (teresa)
**	    fixed compiler warning where duve_cb->duvetree was being cast to
**	    the wrong type.
**	06-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**	26-may-93 (teresa)
**	    Changed to handle events, procedures, rules not being in iirelation,
**	    which also implements new tests 6, 7, 8.
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
**	07-jan-94 (teresa)
**	    do NOT open cursor until after memory is allocated.
[@history_template@]...
*/
DU_STATUS
cktree (duve_cb)
DUVE_CB            *duve_cb;
{
    i4		dropcnt = 0;
    DU_STATUS		base;
    i4		seq;
    i4		diag_msgno,fix_msgno,done_msgno,bit_val;
    DUVE_TREE		tree;	  /* iitree.treetabbase,iitree.treetabidx */
    u_i4		size;
    STATUS		mem_stat;
    char		*name;
    char		*own;
    bool		badtree;
    i4			testid;
# include <duvetree.h>
  i4 cnt;
  u_i4 tid, tidx;
  i4 id1, id2;
  u_i4 basetid, basetidx;
  i4 relstat;
  char ownname[DB_MAXNAME + 1];
  char objname[DB_MAXNAME + 1];
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 6589 "duvechk2.sc" */	/* host code */
    /* allocate memory to hold drop commands */
/* # line 6590 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(treeseq)from iitree where treeseq=0");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6591 "duvechk2.sc" */	/* host code */
    if (sqlca.sqlcode == 100 || cnt ==0)
    {	
	/* its unreasonable to have an empty iitree table, as the system
	** catalogs include some views 
	*/
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU5021_EMPTY_IITREE, 0);
	return DUVE_BAD;
    }
    size = cnt * sizeof(tree);
    duve_cb->duvetree = (DUVE_TREE *) MEreqmem((u_i4)0, size, TRUE, &mem_stat);
    if ( (mem_stat != OK) || (duve_cb->duvetree == 0) )
    {
	duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	return ( (DU_STATUS) DUVE_BAD);
    }
    /* loop for each tuple in iitree */
/* # line 6612 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"tree_cursor",6280,25534);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select * from iitree order by treetabbase, treetabidx, treeid1, treei\
d2, treeseq");
    IIcsQuery((char *)"tree_cursor",6280,25534);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6614 "duvechk2.sc" */	/* host code */
    tree.tabid.db_tab_base = -1;
    tree.tabid.db_tab_index = -1;
    tree.treeid.db_tre_high_time = -1;
    tree.treeid.db_tre_low_time = -1;
    for (;;)  
    {
	/* get tuple from iitree */
/* # line 6621 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"tree_cursor",6280,25534,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(tree_tbl).treetabbase);
      IIcsGetio((short *)0,1,30,4,&(tree_tbl).treetabidx);
      IIcsGetio((short *)0,1,30,4,&(tree_tbl).treeid1);
      IIcsGetio((short *)0,1,30,4,&(tree_tbl).treeid2);
      IIcsGetio((short *)0,1,30,2,&(tree_tbl).treeseq);
      IIcsGetio((short *)0,1,30,2,&(tree_tbl).treemode);
      IIcsGetio((short *)0,1,30,2,&(tree_tbl).treevers);
      IIcsGetio((short *)0,1,32,1024,(tree_tbl).treetree);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6622 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in iitree */
	{
/* # line 6624 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"tree_cursor",6280,25534);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6625 "duvechk2.sc" */	/* host code */
	    break;
	}
	/* determine if this is the 1st tuple for this tree entry */
	if ( (tree_tbl.treetabbase != tree.tabid.db_tab_base) ||
	     (tree_tbl.treetabidx != tree.tabid.db_tab_index) ||
	     (tree_tbl.treeid1 != tree.treeid.db_tre_high_time) ||
	     (tree_tbl.treeid2 != tree.treeid.db_tre_low_time) )
	{
	    /* this is the first tuple for this tree */
	    tree.tabid.db_tab_base  = tree_tbl.treetabbase;
	    tree.tabid.db_tab_index = tree_tbl.treetabidx;
	    tree.treeid.db_tre_high_time = tree_tbl.treeid1;
	    tree.treeid.db_tre_low_time = tree_tbl.treeid2;
	    seq = 0;
	    badtree = FALSE;
	    /* test 1 - verify tree has corresponding iirelation entry
	    **	        (or iirule for rules or iievent for events,
	    **		which are tests 6 and 7)
	    */
	    if (tree.treemode == DB_EVENT)
	    {
		testid = 7;
/* # line 6649 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk232",6281,22844);
      IIputdomio((short *)0,1,30,4,&tree_tbl.treetabbase);
      IIputdomio((short *)0,1,30,4,&tree_tbl.treetabidx);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk232",6281,22844);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select event_name, event_owner from iievent where event_ibase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&tree_tbl.treetabbase);
        IIwritio(0,(short *)0,1,32,0,(char *)"and event_idx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&tree_tbl.treetabidx);
        IIexDefine(0,(char *)"duvechk232",6281,22844);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,objname);
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,ownname);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6654 "duvechk2.sc" */	/* host code */
		if (sqlca.sqlcode == 100)
		    badtree = TRUE;
		else
		{
		    name = objname;
		    own = ownname;
		}
	    }
	    else if (tree.treemode == DB_RULE)
	    {
		testid = 6;
/* # line 6665 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk233",6282,22463);
      IIputdomio((short *)0,1,30,4,&tree_tbl.treeid1);
      IIputdomio((short *)0,1,30,4,&tree_tbl.treeid2);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk233",6282,22463);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_name, rule_owner from iirule where rule_treeid1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&tree_tbl.treeid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and rule_treeid2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&tree_tbl.treeid2);
        IIexDefine(0,(char *)"duvechk233",6282,22463);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,objname);
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,ownname);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6670 "duvechk2.sc" */	/* host code */
		if (sqlca.sqlcode == 100)
		    badtree = TRUE;
		else
		{
		    name = objname;
		    own = ownname;
		}
	    }
	    else 
	    {
		testid = 1;
		base = duve_findreltid ( &tree.tabid, duve_cb);
		if (base == DU_INVALID)
		    badtree = TRUE;
		else
		{
		    name = duve_cb->duverel->rel[base].du_tab;
		    own = duve_cb->duverel->rel[base].du_own;
		}
	    }
	    if (badtree)
	    {   
		if (duve_banner( DUVE_IITREE, testid, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk ( DUVE_MODESENS, duve_cb, S_DU162F_NO_TREEBASE, 4,
			sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx);
		if ( duve_talk(DUVE_ASK, duve_cb, S_DU031C_DEL_TREE, 8,
			sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			sizeof(tree_tbl.treeid1), &tree_tbl.treeid1,
			sizeof(tree_tbl.treeid2), &tree_tbl.treeid2)
		== DUVE_YES)
		{
		    /* indicate to drop tree when done with cursor */
		    duve_cb->duvetree[dropcnt].tabid.db_tab_base =
					tree.tabid.db_tab_base;
		    duve_cb->duvetree[dropcnt].tabid.db_tab_index =
					tree.tabid.db_tab_index;
		    duve_cb->duvetree[dropcnt].treemode = tree.treemode;
		    duve_cb->duvetree[dropcnt].treeid.db_tre_high_time =
					tree.treeid.db_tre_high_time;
		    duve_cb->duvetree[dropcnt++].treeid.db_tre_low_time =
					tree.treeid.db_tre_low_time;
		    duve_talk(DUVE_MODESENS, duve_cb, S_DU036C_DEL_TREE, 8,
			sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			sizeof(tree_tbl.treeid1), &tree_tbl.treeid1,
			sizeof(tree_tbl.treeid2), &tree_tbl.treeid2);
		    continue;
		}
		continue; /* no sense doing other tests on this tree tuple */
	    } /* endif this tree missing corresponding entry from iirelation */
	}	    
	/* determine if this tree entry is marked for drop */
	if ( ( base == DUVE_DROP) ||
	     ((duve_cb->duvetree[dropcnt-1].tabid.db_tab_base ==
	       tree.tabid.db_tab_base) &&
	      (duve_cb->duvetree[dropcnt-1].tabid.db_tab_index ==
	       tree.tabid.db_tab_index) &&
	      (duve_cb->duvetree[dropcnt-1].treeid.db_tre_high_time ==
	       tree.treeid.db_tre_high_time) &&
	      (duve_cb->duvetree[dropcnt-1].treeid.db_tre_low_time ==
	       tree.treeid.db_tre_low_time)
	     )
	   )
	{
	    /* this tree entry is marked for deletion, so skip testing it */
	    continue;
	}
	/* test 2 -- verify tree mode is valid */
	if ( (tree_tbl.treemode != DB_VIEW) && 
	     (tree_tbl.treemode != DB_PROT) &&
	     (tree_tbl.treemode != DB_CRT_LINK) &&
	     (tree_tbl.treemode != DB_REG_LINK) && 
	     (tree_tbl.treemode != DB_REG_IMPORT) &&
	     (tree_tbl.treemode != DB_RULE) &&
	     (tree_tbl.treemode != DB_EVENT) &&
	     (tree_tbl.treemode != DB_INTG) )
	{
            if (duve_banner( DUVE_IITREE, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU162D_UNKNOWN_TREEMODE, 
			10,
			sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			0, name, 0, own,
			sizeof(tree_tbl.treemode), &tree_tbl.treemode);
	    if ( duve_talk( DUVE_ASK, duve_cb, S_DU031C_DEL_TREE, 8,
			    sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			    sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			    sizeof(tree_tbl.treeid1), &tree_tbl.treeid1,
			    sizeof(tree_tbl.treeid2), &tree_tbl.treeid2)
	    == DUVE_YES)
	    {
		duve_cb->duvetree[dropcnt].tabid.db_tab_base =
				    tree.tabid.db_tab_base;
		duve_cb->duvetree[dropcnt].tabid.db_tab_index =
				    tree.tabid.db_tab_index;
		duve_cb->duvetree[dropcnt].treemode = tree.treemode;
		duve_cb->duvetree[dropcnt].treeid.db_tre_high_time =
				    tree.treeid.db_tre_high_time;
		duve_cb->duvetree[dropcnt++].treeid.db_tre_low_time =
				    tree.treeid.db_tre_low_time;
		duve_talk(DUVE_MODESENS, duve_cb, S_DU036C_DEL_TREE, 8,
			sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			sizeof(tree_tbl.treeid1), &tree_tbl.treeid1,
			sizeof(tree_tbl.treeid2), &tree_tbl.treeid2);
		continue;		
	    }
	} /* endif bad tree mode */
	/* test 3 - verify tree id is valid */
	if ( (tree_tbl.treeid1 == 0) && (tree_tbl.treeid2 == 0) )
	{
	    /* invalid tree id */
            if (duve_banner( DUVE_IITREE, 3, duve_cb)
	    == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU162E_INVALID_TREEID, 8,
			sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			0, name, 0, own);
	    if ( duve_talk( DUVE_ASK, duve_cb, S_DU031C_DEL_TREE, 8,
			    sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			    sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			    sizeof(tree_tbl.treeid1), &tree_tbl.treeid1,
			    sizeof(tree_tbl.treeid2), &tree_tbl.treeid2)
	    == DUVE_YES)
	    {
		duve_cb->duvetree[dropcnt].tabid.db_tab_base =
				    tree.tabid.db_tab_base;
		duve_cb->duvetree[dropcnt].tabid.db_tab_index =
				    tree.tabid.db_tab_index;
		duve_cb->duvetree[dropcnt].treemode = tree.treemode;
		duve_cb->duvetree[dropcnt].treeid.db_tre_high_time =
				    tree.treeid.db_tre_high_time;
		duve_cb->duvetree[dropcnt++].treeid.db_tre_low_time =
				    tree.treeid.db_tre_low_time;
		duve_talk(DUVE_MODESENS, duve_cb, S_DU036C_DEL_TREE, 8,
			sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			sizeof(tree_tbl.treeid1), &tree_tbl.treeid1,
			sizeof(tree_tbl.treeid2), &tree_tbl.treeid2);
		continue;		
	    }
	} /* endif bad tree id */	    
	/*
	** test 4  - verify that iirelation tuple correctly reflects
	**	     tree mode (relstat)
	*/
	diag_msgno = 0;
	switch (tree_tbl.treemode)
	{
	case DB_VIEW:
	    if ( (duve_cb->duverel->rel[base].du_stat & TCB_VIEW) == 0 )
	    {
		diag_msgno = S_DU1630_NO_VIEW_RELSTAT;
		fix_msgno  = S_DU031D_SET_VIEW;
		done_msgno = S_DU036D_SET_VIEW;
		bit_val = TCB_VIEW;
	    }
	    break;
	case DB_PROT:
	    if ( (duve_cb->duverel->rel[base].du_stat & TCB_PRTUPS) == 0 )
	    {
		diag_msgno = S_DU1631_NO_PROTECT_RELSTAT;
		fix_msgno  = S_DU031E_SET_PRTUPS;
		done_msgno = S_DU036E_SET_PRTUPS;
		bit_val = TCB_PRTUPS;
	    }
	    break;
	case DB_INTG:
	    if ( (duve_cb->duverel->rel[base].du_stat & TCB_INTEGS) == 0 )
	    {
		diag_msgno = S_DU1632_NO_INTEGS_RELSTAT;
		fix_msgno  = S_DU031F_SET_INTEGS;
		done_msgno = S_DU036F_SET_INTEGS;
		bit_val = TCB_INTEGS;
	    }
	    break;
	case DB_DBP:
	    break;
	case DB_RULE:
	    if ( (duve_cb->duverel->rel[base].du_stat & TCB_RULE) == 0 )
	    {
		diag_msgno = S_DU1673_NO_RULES_RELSTAT;
		fix_msgno  = S_DU033E_SET_RULES;
		done_msgno = S_DU038E_SET_RULES;
		bit_val = TCB_RULE;
	    }
	    break;
	case DB_EVENT:
	    break;
	default:
	    break;
	}
	if (diag_msgno != 0)
	{
	    /* the relstat in iirelation needs to be modified to indicate mode*/
            if (duve_banner( DUVE_IITREE, 4, duve_cb)
	    == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, diag_msgno, 4,
			0, name, 0, own);
	    if ( duve_talk( DUVE_ASK, duve_cb, fix_msgno, 0)
	    == DUVE_YES)
	    {
		basetid = duve_cb->duverel->rel[base].du_id.db_tab_base;
		basetidx= duve_cb->duverel->rel[base].du_id.db_tab_index;
		relstat = duve_cb->duverel->rel[base].du_stat | bit_val;
		duve_cb->duverel->rel[base].du_stat = relstat;
/* # line 6889 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t=");
    IIputdomio((short *)0,1,30,4,&relstat);
    IIwritio(0,(short *)0,1,32,0,(char *)"where reltid=");
    IIputdomio((short *)0,1,30,4,&basetid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&basetidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6891 "duvechk2.sc" */	/* host code */
		if (sqlca.sqlcode == 0)
		    duve_talk(DUVE_MODESENS, duve_cb, done_msgno, 0);
	    }  /* endif fix relstat */
	} /* endif relstat doesnt reflect mode */
	/* test 5  -- verify sequence is correct for iitree entries that
	**	      span more than 1 tuple */
	if ( tree_tbl.treeseq == seq )
	    seq++;			/* all is well */
	else if (tree_tbl.treeseq < seq)
	{
	    /* duplicate tree sequence tuple */
            if (duve_banner( DUVE_IITREE, 5, duve_cb)
	    == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU164E_DUPLICATE_TREE_SEQ, 10,
			sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			0, name, 0, own,
			sizeof(tree_tbl.treeseq), &tree_tbl.treeseq);
	    if ( duve_talk (DUVE_ASK, duve_cb, S_DU031C_DEL_TREE, 8,
			    sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			    sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			    sizeof(tree_tbl.treeid1), &tree_tbl.treeid1,
			    sizeof(tree_tbl.treeid2), &tree_tbl.treeid2)
	    == DUVE_YES)
	    {
		duve_cb->duvetree[dropcnt].tabid.db_tab_base =
				    tree.tabid.db_tab_base;
		duve_cb->duvetree[dropcnt].tabid.db_tab_index =
				    tree.tabid.db_tab_index;
		duve_cb->duvetree[dropcnt].treemode = tree.treemode;
		duve_cb->duvetree[dropcnt].treeid.db_tre_high_time =
				    tree.treeid.db_tre_high_time;
		duve_cb->duvetree[dropcnt++].treeid.db_tre_low_time =
				    tree.treeid.db_tre_low_time;
		treeclean(base, &tree, tree_tbl.treemode, duve_cb);
		duve_talk(DUVE_MODESENS, duve_cb, S_DU036C_DEL_TREE, 8,
			sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			sizeof(tree_tbl.treeid1), &tree_tbl.treeid1,
			sizeof(tree_tbl.treeid2), &tree_tbl.treeid2);
		continue;
	    }
	    else
		seq = tree_tbl.treeseq + 1;
	}
	else
	{
	    /* missing tree sequence tuple */
            if (duve_banner( DUVE_IITREE, 5, duve_cb)
	    == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU1633_MISSING_TREE_SEQ, 10,
			sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			0, name, 0, own,
			sizeof(tree_tbl.treeseq), &tree_tbl.treeseq);
	    if ( duve_talk (DUVE_ASK, duve_cb, S_DU031C_DEL_TREE, 8,
			    sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			    sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			    sizeof(tree_tbl.treeid1), &tree_tbl.treeid1,
			    sizeof(tree_tbl.treeid2), &tree_tbl.treeid2)
	    == DUVE_YES)
	    {
		duve_cb->duvetree[dropcnt].tabid.db_tab_base =
				    tree.tabid.db_tab_base;
		duve_cb->duvetree[dropcnt].tabid.db_tab_index =
				    tree.tabid.db_tab_index;
		duve_cb->duvetree[dropcnt].treemode = tree.treemode;
		duve_cb->duvetree[dropcnt].treeid.db_tre_high_time =
				    tree.treeid.db_tre_high_time;
		duve_cb->duvetree[dropcnt++].treeid.db_tre_low_time =
				    tree.treeid.db_tre_low_time;
		treeclean(base, &tree, tree_tbl.treemode, duve_cb);
		duve_talk(DUVE_MODESENS, duve_cb, S_DU036C_DEL_TREE, 8,
			sizeof(tree_tbl.treetabbase), &tree_tbl.treetabbase,
			sizeof(tree_tbl.treetabidx), &tree_tbl.treetabidx,
			sizeof(tree_tbl.treeid1), &tree_tbl.treeid1,
			sizeof(tree_tbl.treeid2), &tree_tbl.treeid2);
		continue;
	    }
	    else
		seq = tree_tbl.treeseq + 1;
	}
    }	/* end loop for each iitree tuple */
    /* delete any iitree tuples marked for delete
    ** then deallocate the memory used by duve_cb->duvetree
    */
    while (dropcnt > 0)
    {
	/* drop from iitree */
	tid  = duve_cb->duvetree[--dropcnt].tabid.db_tab_base;
	tidx = duve_cb->duvetree[dropcnt].tabid.db_tab_index;
	id1 = duve_cb->duvetree[dropcnt].treeid.db_tre_high_time;
	id2 = duve_cb->duvetree[dropcnt].treeid.db_tre_low_time;
/* # line 6992 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iitree where treetabbase=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treetabidx=");
    IIputdomio((short *)0,1,30,4,&tidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and treeid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6994 "duvechk2.sc" */	/* host code */
	if (duve_cb->duvetree[dropcnt].treemode == DB_EVENT)
	{
	    /* Drop from iievent */
/* # line 6998 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iievent where event_ibase=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and event_idx=");
    IIputdomio((short *)0,1,30,4,&tidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 6999 "duvechk2.sc" */	/* host code */
    	}
	if (duve_cb->duvetree[dropcnt].treemode == DB_RULE)
	{
	    /* Drop from iirule */
/* # line 7004 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirule where rule_treeid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_treeid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7005 "duvechk2.sc" */	/* host code */
	}
	else
	{
	    /* if view, drop from iirelation */
	    base = duve_findreltid ( &tree.tabid, duve_cb);
	    if ( base >= 0 )
		if (duve_cb->duverel->rel[base].du_stat & TCB_VIEW)
		    duve_cb->duverel->rel[base].du_tbldrop = DUVE_DROP;
	}
    }
    /*
    ** deallocate duve_cb->duvetree cache and indicate in control block that
    ** it has been dropped.
    */
    MEfree ( (PTR) duve_cb->duvetree);
    duve_cb->duvetree = 0;
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "cktree"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckxdbdep - run verifydb check on iixdbdepends dbms catalog
**
** Description:
**      This routine opens a cursor and examines each tuple in iixdbdepends.
**	It checks the tuple against the cached iidbdepends table.  If it
**	cannot find a match with tuples in this table, it reports an error
**	and possible (mode dependent) deletes that tuple from iixdbdepends.
**
** Inputs:
**      duve_cb                         verifydb control block
**	    duvedpnds			    cached iidbdepends information
**
** Outputs:
**	none.
**
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
** Side Effects:
**	    tuples may be deleted from iixdbdepends
**
** History:
**      6-Sep-1988 (teg)
**          initial creation
**	06-May-93 (teresa)
**	    Changed interface to duve_banner() and renumbered verifydb checks.
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
[@history_template@]...
*/
DU_STATUS
ckxdbdep(duve_cb)
DUVE_CB            *duve_cb;
{
    DU_STATUS		base;
# include <duvexdp.h>
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 7084 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"xdpnd_cursor",6283,29868);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iixdbdepends");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"xdpnd_cursor",6283,29868);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7086 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iixdbdepends */
    for (;;)
    {
	/* get tuple from table */
/* # line 7090 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"xdpnd_cursor",6283,29868,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(xdpnd_tbl).deid1);
      IIcsGetio((short *)0,1,30,4,&(xdpnd_tbl).deid2);
      IIcsGetio((short *)0,1,30,4,&(xdpnd_tbl).dtype);
      IIcsGetio((short *)0,1,30,4,&(xdpnd_tbl).qid);
      IIcsGetio((short *)0,1,30,4,&(xdpnd_tbl).tidp);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7091 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in iixdbdepends */
	{
/* # line 7093 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"xdpnd_cursor",6283,29868);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7094 "duvechk2.sc" */	/* host code */
	    break;
	}
	/* test 1 - see if there is corresponding tuple in iidbdepends */
	base = duve_d_dpndfind( xdpnd_tbl.deid1, xdpnd_tbl.deid2, 
	    xdpnd_tbl.dtype, xdpnd_tbl.qid, duve_cb);
	if (base == DU_INVALID)
	{
	    if (duve_banner( DUVE_IIXDBDEPENDS, 1, duve_cb)
	    == DUVE_BAD) 
		return ( (DU_STATUS)DUVE_BAD);
	    duve_talk( DUVE_MODESENS, duve_cb, S_DU1645_MISSING_IIDBDEPENDS, 4,
		       sizeof(xdpnd_tbl.deid1), &xdpnd_tbl.deid1,
		       sizeof(xdpnd_tbl.deid2),	&xdpnd_tbl.deid2);
	    if (duve_talk( DUVE_ASK, duve_cb, S_DU0325_REDO_IIXDBDEPENDS, 0)
	    ==DUVE_YES)
	    {
/* # line 7111 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"xdpnd_cursor",6283,29868);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7112 "duvechk2.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop iixdbdepends");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7113 "duvechk2.sc" */	/* create index */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index iixdbdepends on iidbdepends(deid1, deid2, dtype, qid)wit\
h structure=btree");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7116 "duvechk2.sc" */	/* host code */
		duve_talk(DUVE_MODESENS,duve_cb, S_DU0375_REDO_IIXDBDEPENDS, 0);
	    }
	} /* end test 1 */
    } /* end loop for each tuple in table */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckxdbdep"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckxpriv - run verifydb check on iixpriv dbms catalog
**
** Description:
**	This function will ensure that IIXPRIV is in sync with IIPRIV.  This 
**	will be accomplished in two steps:
**	- first we will open a cursor to read tuples in iixpriv for 
**	  which there are no corresponding tuples in iipriv; if one or more such
**	  tuples are found, we will ask for permission to drop and recreate the 
**	  index
**	- then we will open a cursor on iipriv to read tuples for which there 
**	  is no corresponding iixpriv tuple and insert appropriate iixpriv 
**	  tuples
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**	none.
**
** Returns:
**    	DUVE_YES
**      DUVE_BAD
**
** Exceptions:
**	none
**
** Side Effects:
**	tuples may be inserted into / deleted from iixpriv
**
** History:
**      19-jan-94 (andre)
**	    written
[@history_template@]...
*/
DU_STATUS
ckxpriv(
	DUVE_CB		*duve_cb)
{
# include <duvexprv.h>
  u_i4 tid_ptr;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 7187 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"xpriv_tst_curs_1",6284,22579);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select * from iixpriv xpr where not exists(select 1 from iipriv pr wh\
ere xpr.tidp=pr.tid and xpr.i_obj_id=pr.i_obj_id and xpr.i_obj_idx=pr.\
i_obj_idx and xpr.i_priv=pr.i_priv and xpr.i_priv_grantee=pr.i_priv_gr\
antee)");
    IIcsQuery((char *)"xpriv_tst_curs_1",6284,22579);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7189 "duvechk2.sc" */	/* host code */
    /*
    ** test 1: for every IIXPRIV tuple for which there is no corresponding 
    ** IIPRIV tuple ask user whether we may delete the offending IIXPRIV tuple
    */
    for (;;)
    {
/* # line 7195 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"xpriv_tst_curs_1",6284,22579,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(xprv_tbl).i_obj_id);
      IIcsGetio((short *)0,1,30,4,&(xprv_tbl).i_obj_idx);
      IIcsGetio((short *)0,1,30,4,&(xprv_tbl).i_priv);
      IIcsGetio((short *)0,1,32,32,(xprv_tbl).i_priv_grantee);
      IIcsGetio((short *)0,1,30,4,&(xprv_tbl).tidp);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7196 "duvechk2.sc" */	/* host code */
        if (sqlca.sqlcode == 100) /* then no more tuples to read */
        {
/* # line 7198 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"xpriv_tst_curs_1",6284,22579);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7199 "duvechk2.sc" */	/* host code */
	    break;
        }
	/* there is no tuple in IIPRIV corresponding to this IIXPRIV tuple */
        if (duve_banner(DUVE_IIXPRIV, 1, duve_cb) == DUVE_BAD) 
	    return ( (DU_STATUS)DUVE_BAD);
        duve_talk( DUVE_MODESENS, duve_cb, S_DU16C7_EXTRA_IIXPRIV, 10,
            sizeof(xprv_tbl.i_obj_id), &xprv_tbl.i_obj_id,
	    sizeof(xprv_tbl.i_obj_idx), &xprv_tbl.i_obj_idx,
	    sizeof(xprv_tbl.i_priv), &xprv_tbl.i_priv,
	    0, xprv_tbl.i_priv_grantee,
	    sizeof(xprv_tbl.tidp), &xprv_tbl.tidp);
	if (duve_talk(DUVE_ASK, duve_cb, S_DU0248_DELETE_IIXPRIV, 0) 
	    == DUVE_YES)
	{
/* # line 7215 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iixpriv",(char *)"xpriv_tst_curs_1",6284,22579);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7217 "duvechk2.sc" */	/* host code */
	    duve_talk(DUVE_MODESENS,duve_cb, S_DU0448_DELETED_IIXPRIV, 0);
	    break;
	}
    }
    /* end test 1 */
/* # line 7241 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"xpriv_tst_curs_2",6285,10935);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select pr.*, pr.tid from iipriv pr where not exists(select 1 from iix\
priv xpr where xpr.tidp=pr.tid and xpr.i_obj_id=pr.i_obj_id and xpr.i_\
obj_idx=pr.i_obj_idx and xpr.i_priv=pr.i_priv and xpr.i_priv_grantee=p\
r.i_priv_grantee and prv_flags=prv_flags)");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"xpriv_tst_curs_2",6285,10935);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7243 "duvechk2.sc" */	/* host code */
    /*
    ** test 2: for every IIPRIV tuple for which there is no corresponding 
    ** IIXPRIV tuple ask user whether we may insert a missing tuple into IIXPRIV
    */
    for (;;)
    {
/* # line 7249 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"xpriv_tst_curs_2",6285,10935,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).d_obj_id);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).d_obj_idx);
      IIcsGetio((short *)0,1,30,2,&(prv_tbl).d_priv_number);
      IIcsGetio((short *)0,1,30,2,&(prv_tbl).d_obj_type);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_obj_id);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_obj_idx);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv);
      IIcsGetio((short *)0,1,32,32,(prv_tbl).i_priv_grantee);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).prv_flags);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map1);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map2);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map3);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map4);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map5);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map6);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map7);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map8);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map9);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_mapa);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map11);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map12);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map13);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map14);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map15);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map16);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map17);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map18);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map19);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map20);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map21);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map22);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map23);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map24);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map25);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map26);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map27);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map28);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map29);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map30);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map31);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map32);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map33);
      IIcsGetio((short *)0,1,30,4,&tid_ptr);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7250 "duvechk2.sc" */	/* host code */
        if (sqlca.sqlcode == 100) /* then no more tuples to read */
        {
/* # line 7252 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"xpriv_tst_curs_2",6285,10935);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7253 "duvechk2.sc" */	/* host code */
	    break;
        }
	/* there is no tuple in IIXPRIV corresponding to this IIPRIV tuple */
        if (duve_banner(DUVE_IIXPRIV, 2, duve_cb) == DUVE_BAD) 
	    return ( (DU_STATUS)DUVE_BAD);
	STtrmwhite(prv_tbl.i_priv_grantee);
        duve_talk( DUVE_MODESENS, duve_cb, S_DU16C8_MISSING_IIXPRIV, 10,
            sizeof(prv_tbl.i_obj_id), &prv_tbl.i_obj_id,
	    sizeof(prv_tbl.i_obj_idx), &prv_tbl.i_obj_idx,
	    sizeof(prv_tbl.i_priv), &prv_tbl.i_priv,
	    0, prv_tbl.i_priv_grantee,
	    sizeof(tid_ptr), &tid_ptr);
	if (duve_talk( DUVE_ASK, duve_cb, S_DU0249_INSERT_IIXPRIV_TUPLE, 0) 
	    == DUVE_YES)
	{
/* # line 7271 "duvechk2.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into iixpriv(i_obj_id, i_obj_idx, i_priv, i_priv_grantee, tidp\
)values(");
    IIputdomio((short *)0,1,30,4,&prv_tbl.i_obj_id);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&prv_tbl.i_obj_idx);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&prv_tbl.i_priv);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,prv_tbl.i_priv_grantee);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&tid_ptr);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7276 "duvechk2.sc" */	/* host code */
	    duve_talk(DUVE_MODESENS, duve_cb, 
		S_DU0449_INSERTED_IIXPRIV_TUPLE, 0);
	}
    }
    /* end test 2 */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckxpriv"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckpriv - Perform system catalog checks/fixes on iipriv catalog
**
** Description:
**      this routine performs system catalog tests on iipriv 
**	system catalog.
**
** Inputs:
**      duve_cb                         verifydb control block
**	    duverel                          cached iirelation information
**
** Outputs:
**      duve_cb                         verifydb control block
**	    duverel                         cached iirelation information
**	        du_tbldrop			flag indicating to drop table
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    iipriv tuples may get deletred, database objects may be marked for 
**	    destruction
**
** History:
**      19-jan-94 (andre)
**          written
**	9-Sep-2004 (schka24)
**	    1024 column expansion.
[@history_template@]...
*/
DU_STATUS
ckpriv(
       DUVE_CB		*duve_cb)
{
    DB_TAB_ID		obj_id;
    u_i4		priv_number, obj_type;
    DU_STATUS		view_offset;
    u_i4		attr_map[DB_COL_WORDS];
    DB_TAB_ID		indep_obj_id;
    i4			priv_needed;
    STATUS              mem_stat;
    bool		satisfied;
    bool		sel_perms_on_view;
    bool		perms_on_dbp;
  u_i4 tid, tidx;
  i4 qid;
  char obj_name[DB_MAXNAME + 1];
  char sch_name[DB_MAXNAME + 1];
  i4 cnt;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);
/* # line 7351 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"priv_cursor",6286,16555);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iipriv");
    IIcsQuery((char *)"priv_cursor",6286,16555);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7353 "duvechk2.sc" */	/* host code */
    /*
    ** IIPRIV contains privilege descriptors for objects of 4 types:
    **	- views,
    **	- database procedures,
    **	- permits on views, and
    **  - constraints
    **
    ** descriptors for views and dbprocs will be assigned descriptor number 0
    ** descriptors for views and constraints will descriptor number > 0
    */
    /*
    ** for views and dbprocs, there may be multiple records in IIPRIV describing
    ** privileges on which these objects depend. In order to avoid redundant 
    ** checks for existence of an object, we will save object id and type and 
    ** the descriptor number found in the last row.
    */
    obj_id.db_tab_base = obj_id.db_tab_index = 0L;
    /* loop for each tuple in iipriv */
    for (;;)  
    {
	/* get tuple from table */
/* # line 7376 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"priv_cursor",6286,16555,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).d_obj_id);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).d_obj_idx);
      IIcsGetio((short *)0,1,30,2,&(prv_tbl).d_priv_number);
      IIcsGetio((short *)0,1,30,2,&(prv_tbl).d_obj_type);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_obj_id);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_obj_idx);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv);
      IIcsGetio((short *)0,1,32,32,(prv_tbl).i_priv_grantee);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).prv_flags);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map1);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map2);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map3);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map4);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map5);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map6);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map7);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map8);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map9);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_mapa);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map11);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map12);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map13);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map14);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map15);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map16);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map17);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map18);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map19);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map20);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map21);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map22);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map23);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map24);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map25);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map26);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map27);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map28);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map29);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map30);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map31);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map32);
      IIcsGetio((short *)0,1,30,4,&(prv_tbl).i_priv_map33);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7377 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in table */
	{
/* # line 7379 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"priv_cursor",6286,16555);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7380 "duvechk2.sc" */	/* host code */
	    break;
	}
	STtrmwhite(prv_tbl.i_priv_grantee);
	/*
	** test 1: verify that the object for which this descriptor has been 
	** 	   created exists; 
	**
	** if the descriptor contains privileges on which a view or a dbproc 
	** depends, we will verify that the object exists; if it contains 
	** privileges on which a permit or a constraint depends, we will verify
	** that there is a tuple in IIDBDEPENDS describing dependence of an 
	** object of specified type on this descriptor; in all cases, we will 
	** compare object id and type and descriptor number with those saved 
	** in obj_id, obj_type and priv_number - if they are the same, we have 
	** already checked for existence of the object and there is hardly a 
	** reason to do it again.
	*/
	if (   obj_id.db_tab_base 	!= prv_tbl.d_obj_id
	    || obj_id.db_tab_index 	!= prv_tbl.d_obj_idx
	    || priv_number		!= prv_tbl.d_priv_number
	    || obj_type 		!= prv_tbl.d_obj_type)
	{
	    bool	bad_obj_type;
	    bool	found;
	    /*
	    ** save object id and type and descriptor number found in this 
	    ** record
	    */
	    obj_id.db_tab_base = prv_tbl.d_obj_id;
	    obj_id.db_tab_index = prv_tbl.d_obj_idx;
	    priv_number = prv_tbl.d_priv_number;
	    obj_type = prv_tbl.d_obj_type;
	    bad_obj_type = found = FALSE;
	    switch (obj_type)
	    {
		case DB_VIEW:
		    /*
		    ** look up the view description in the relation cache;
		    ** will complain only if the object was not found; there is 		    ** no reason to complain even if it is marked for 
		    ** destruction because this tuple will then get destroyed 
		    ** as a part of destruction of the view itself
		    */
		    if ((view_offset = duve_findreltid(&obj_id, duve_cb)) 
			== DUVE_DROP)
		    {
			/* 
			** view is marked for destruction - no point in running 
			** checks on this tuple
			*/
			continue;
		    }
		    found = (view_offset != DU_INVALID);
		    if (found)
		    {
  u_i4 sel;
  u_i4 sel_wgo;
			sel = DB_RETRIEVE | DB_TEST | DB_AGGREGATE;
			sel_wgo = sel | DB_GRANT_OPTION;
			/*
			** if the view exists, determine whether there are 
			** select permits on it - if so, when checking whether 
			** the user possesses privileges described by this 
			** record, we will check for SELECT WITH GRANT OPTION 
			** as opposed to just SELECT
			*/
/* # line 7456 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk234",6287,20011);
      IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_id);
      IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_idx);
      IIputdomio((short *)0,1,30,4,&sel);
      IIputdomio((short *)0,1,30,4,&sel_wgo);
      IIputdomio((short *)0,1,32,0,prv_tbl.i_priv_grantee);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk234",6287,20011);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(1)from iiprotect where protabbase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_id);
        IIwritio(0,(short *)0,1,32,0,(char *)"and protabidx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_idx);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(proopset=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&sel);
        IIwritio(0,(short *)0,1,32,0,(char *)"or proopset=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&sel_wgo);
        IIwritio(0,(short *)0,1,32,0,(char *)")and prograntor=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,prv_tbl.i_priv_grantee);
        IIexDefine(0,(char *)"duvechk234",6287,20011);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7463 "duvechk2.sc" */	/* host code */
			sel_perms_on_view = (cnt != 0);
		    }
		    break;
		case DB_DBP:
/* # line 7469 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk235",6288,21679);
      IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_id);
      IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_idx);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk235",6288,21679);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(dbp_id)from iiprocedure where dbp_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_id);
        IIwritio(0,(short *)0,1,32,0,(char *)"and dbp_idx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_idx);
        IIexDefine(0,(char *)"duvechk235",6288,21679);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7475 "duvechk2.sc" */	/* host code */
		    found = (cnt != 0);
		    if (found)
		    {
  u_i4 exec_priv;
  u_i4 exec_priv_wgo;
			exec_priv = DB_EXECUTE;
			exec_priv_wgo = exec_priv | DB_GRANT_OPTION;
			/*
			** if the dbproc exists, determine whether there are 
			** permits on it - if so, when checking whether 
			** the user possesses privileges described by this 
			** record, we will check for <priv> WITH GRANT OPTION 
			** as opposed to just <priv>
			*/
/* # line 7494 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk236",6289,21426);
      IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_id);
      IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_idx);
      IIputdomio((short *)0,1,30,4,&exec_priv);
      IIputdomio((short *)0,1,30,4,&exec_priv_wgo);
      IIputdomio((short *)0,1,32,0,prv_tbl.i_priv_grantee);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk236",6289,21426);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(1)from iiprotect where protabbase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_id);
        IIwritio(0,(short *)0,1,32,0,(char *)"and protabidx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_idx);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(proopset=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&exec_priv);
        IIwritio(0,(short *)0,1,32,0,(char *)"or proopset=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&exec_priv_wgo);
        IIwritio(0,(short *)0,1,32,0,(char *)")and prograntor=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,prv_tbl.i_priv_grantee);
        IIexDefine(0,(char *)"duvechk236",6289,21426);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7502 "duvechk2.sc" */	/* host code */
			perms_on_dbp = (cnt != 0);
		    }
		    break;
		case DB_PROT:
		case DB_CONS:
		    /*
		    ** make sure IIDBDEPENDS cache contains an entry describing 
		    ** dependence of an object of specified type on this 
		    ** privilege descriptor.
		    */
		    found = 
			(duve_d_cnt((i4) DU_INVALID, (i4) obj_id.db_tab_base,
			    (i4) obj_id.db_tab_index, (i4) DB_PRIV_DESCR,
			    (i4) priv_number, (i4) obj_type, duve_cb) > 0);
		    break;
		default:
		    bad_obj_type = TRUE;
		    break;
	    }
            if (bad_obj_type)
            {
	        /* 
	        ** type of object for which the privilege descriptor has been 
		** created is not one of those which we expected to find
		*/
                if (duve_banner(DUVE_IIPRIV, 1, duve_cb) == DUVE_BAD) 
	            return ( (DU_STATUS) DUVE_BAD);
                duve_talk(DUVE_MODESENS, duve_cb, 
		    S_DU16CF_BAD_OBJ_TYPE_IN_IIPRIV, 8,
		    sizeof(prv_tbl.d_obj_type), &prv_tbl.d_obj_type,
		    sizeof(prv_tbl.d_obj_id), &prv_tbl.d_obj_id,
	            sizeof(prv_tbl.d_obj_idx), &prv_tbl.d_obj_idx,
		    sizeof(prv_tbl.d_priv_number), &prv_tbl.d_priv_number);
                if (duve_talk(DUVE_ASK, duve_cb, 
	                S_DU0250_DELETE_IIPRIV_TUPLE, 8, 
	                sizeof(prv_tbl.d_obj_id), &prv_tbl.d_obj_id,
	                sizeof(prv_tbl.d_obj_idx), &prv_tbl.d_obj_idx,
	                sizeof(prv_tbl.d_priv_number), &prv_tbl.d_priv_number,
	                sizeof(prv_tbl.d_obj_type), &prv_tbl.d_obj_type)
                    == DUVE_YES)
                {
/* # line 7549 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iipriv",(char *)"priv_cursor",6286,16555);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7551 "duvechk2.sc" */	/* host code */
	            duve_talk( DUVE_MODESENS, duve_cb, 
		        S_DU0450_DELETED_IIPRIV_TUPLE, 8, 
	                sizeof(prv_tbl.d_obj_id), &prv_tbl.d_obj_id,
	                sizeof(prv_tbl.d_obj_idx), &prv_tbl.d_obj_idx,
	                sizeof(prv_tbl.d_priv_number), &prv_tbl.d_priv_number,
	                sizeof(prv_tbl.d_obj_type), &prv_tbl.d_obj_type);
                }
		continue;
            }
            else if (!found)
            {
	        char	obj_type_str[50];
	        i4	type_len;
		i4	error_number;
	        /* 
	        ** object for which the descriptor has been created does not 
		** exist 
	        */
                if (duve_banner(DUVE_IIPRIV, 1, duve_cb) == DUVE_BAD) 
	            return ((DU_STATUS) DUVE_BAD);
	        duve_obj_type((i4) prv_tbl.d_obj_type, obj_type_str, 
		    &type_len);
		if (obj_type == DB_VIEW || obj_type == DB_DBP)
		{
		    error_number = S_DU16E0_NONEXISTENT_OBJ;
		}
		else
		{
		    error_number = S_DU16E1_NO_OBJ_DEPENDS_ON_DESCR;
		}
                duve_talk(DUVE_MODESENS, duve_cb, error_number, 8,
		    type_len, 	       obj_type_str,
		    sizeof(prv_tbl.d_obj_id), &prv_tbl.d_obj_id,
	            sizeof(prv_tbl.d_obj_idx), &prv_tbl.d_obj_idx,
		    sizeof(prv_tbl.d_priv_number), &prv_tbl.d_priv_number);
                if (duve_talk(DUVE_ASK, duve_cb, 
	                S_DU0250_DELETE_IIPRIV_TUPLE, 8, 
	                sizeof(prv_tbl.d_obj_id), &prv_tbl.d_obj_id,
	                sizeof(prv_tbl.d_obj_idx), &prv_tbl.d_obj_idx,
	                sizeof(prv_tbl.d_priv_number), &prv_tbl.d_priv_number,
	                sizeof(prv_tbl.d_obj_type), &prv_tbl.d_obj_type)
                    == DUVE_YES)
                {
/* # line 7600 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iipriv",(char *)"priv_cursor",6286,16555);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7602 "duvechk2.sc" */	/* host code */
	            duve_talk( DUVE_MODESENS, duve_cb, 
		        S_DU0450_DELETED_IIPRIV_TUPLE, 8, 
	                sizeof(prv_tbl.d_obj_id), &prv_tbl.d_obj_id,
	                sizeof(prv_tbl.d_obj_idx), &prv_tbl.d_obj_idx,
	                sizeof(prv_tbl.d_priv_number), &prv_tbl.d_priv_number,
	                sizeof(prv_tbl.d_obj_type), &prv_tbl.d_obj_type);
                }
		continue;
            }
	}
	/* end of test 1 */
	/*
	** test 2: verify that the user who created the object dependent on 
	** 	   privilege(s) described by this privilege descriptor still 
	**	   possesses them;
	**
	** If he does not possess them, we will report this fact to the user and
	** ask for permission to deal with offending object(s) as follows:  
	**   - if the privilege descriptor described privileges on which a view 
	**     depends, we will mark that view for destruction;
	**   - if the privilege descriptor described privileges on which a
	**     dbproc depends, that dbproc will be marked dormant
	**   - if the privilege descriptor described privileges on which 
	**     permit(s) on a view depended, we will mark that view for 
	**     destruction
	**   - if the privilege descriptor described privileges on which
	**     a constraint depended, we will mark that constraint for 
	**     destruction
	*/
	indep_obj_id.db_tab_base = prv_tbl.i_obj_id;
	indep_obj_id.db_tab_index = prv_tbl.i_obj_idx;
	/* 
	** permits depend of GRANT OPTION FOR privilege; 
	**
	** if the current record describes privilege on which a view depends
	** and there are some permits defined on it, we will check for 
	** SELECT WGO rather than SELECT to avoid having to check whether the 
	** owner of a view possesses GRANT OPTION FOR all privileges on which a
	** view depends when performing IIPROTECT tests 
	**
	** similarly, if the current record describes privilege on which a 
	** dbproc depends and there are some permits defined on it, we will 
	** check for <priv> WGO rather than <priv> to avoid having to check 
	** whether the owner of a dbproc possesses GRANT OPTION FOR all 
	** privileges on which a dbproc depends when performing IIPROTECT tests
	*/
	if (   prv_tbl.d_obj_type == DB_PROT
	    || (prv_tbl.d_obj_type == DB_VIEW && sel_perms_on_view)
	    || (prv_tbl.d_obj_type == DB_DBP && perms_on_dbp)
	   )
	{
	    priv_needed = prv_tbl.i_priv | DB_GRANT_OPTION;
	}
	else
	{
	    priv_needed = prv_tbl.i_priv;
	}
	attr_map[0] = prv_tbl.i_priv_map1;
	attr_map[1] = prv_tbl.i_priv_map2;
	attr_map[2] = prv_tbl.i_priv_map3;
	attr_map[3] = prv_tbl.i_priv_map4;
	attr_map[4] = prv_tbl.i_priv_map5;
	attr_map[5] = prv_tbl.i_priv_map6;
	attr_map[6] = prv_tbl.i_priv_map7;
	attr_map[7] = prv_tbl.i_priv_map8;
	attr_map[8] = prv_tbl.i_priv_map9;
	attr_map[9] = prv_tbl.i_priv_mapa;
	attr_map[10] = prv_tbl.i_priv_map11;
	attr_map[11] = prv_tbl.i_priv_map12;
	attr_map[12] = prv_tbl.i_priv_map13;
	attr_map[13] = prv_tbl.i_priv_map14;
	attr_map[14] = prv_tbl.i_priv_map15;
	attr_map[15] = prv_tbl.i_priv_map16;
	attr_map[16] = prv_tbl.i_priv_map17;
	attr_map[17] = prv_tbl.i_priv_map18;
	attr_map[18] = prv_tbl.i_priv_map19;
	attr_map[19] = prv_tbl.i_priv_map20;
	attr_map[20] = prv_tbl.i_priv_map21;
	attr_map[21] = prv_tbl.i_priv_map22;
	attr_map[22] = prv_tbl.i_priv_map23;
	attr_map[23] = prv_tbl.i_priv_map24;
	attr_map[24] = prv_tbl.i_priv_map25;
	attr_map[25] = prv_tbl.i_priv_map26;
	attr_map[26] = prv_tbl.i_priv_map27;
	attr_map[27] = prv_tbl.i_priv_map28;
	attr_map[28] = prv_tbl.i_priv_map29;
	attr_map[29] = prv_tbl.i_priv_map30;
	attr_map[30] = prv_tbl.i_priv_map31;
	attr_map[31] = prv_tbl.i_priv_map32;
	attr_map[32] = prv_tbl.i_priv_map33;
	duve_check_privs(&indep_obj_id, priv_needed, 
	    prv_tbl.i_priv_grantee, attr_map, &satisfied);
	if (!satisfied)
	{
            if (duve_banner(DUVE_IIPRIV, 2, duve_cb) == DUVE_BAD) 
	        return ( (DU_STATUS) DUVE_BAD);
	    /*
	    ** user specified in this IIPRIV tuple lacks privileges described by
	    ** the tuple - object will be marked for destruction
	    */
	    if (prv_tbl.d_obj_type == DB_VIEW || prv_tbl.d_obj_type == DB_PROT)
	    {
	        /*
	        ** note that if IIPRIV indicates that permit(s) depend on a
	        ** privilege and the grantor of the permit no longer possesses 
		** that privilege, rather than trying to drop the permit and 
		** launch the full-blown REVOKE processing, we will attempt to 
		** drop the object (a view) on which the permit was defined
	        */
		/* if the descriptor represents a privilege on which a permit
		** depends, lookup the definition of the view with which this 
		** permit is affiliated
		*/
		if (prv_tbl.d_obj_type == DB_PROT)
		{
		    view_offset = duve_findreltid(&obj_id, duve_cb);
		}
		if (view_offset == DU_INVALID || view_offset == DUVE_DROP)
		{
		    /* 
		    ** view_offset may get set to DU_INVALID if a view on which 
		    ** a permit was defined no longer exists - this condition 
		    ** will be checked in ckprot();
		    ** view_offset may get set to DUVE_DROP if a view on which a
		    ** permit was defined was has been marked for destruction
		    */
		    continue;
		}
		if (prv_tbl.d_obj_type == DB_PROT)
		{
		    duve_talk(DUVE_MODESENS, duve_cb, 
			S_DU16E3_LACKING_PERM_INDEP_PRIV, 8,
			0, prv_tbl.i_priv_grantee,
			sizeof(prv_tbl.d_priv_number), &prv_tbl.d_priv_number,
			0, duve_cb->duverel->rel[view_offset].du_own,
			0, duve_cb->duverel->rel[view_offset].du_tab);
		}
		else
		{
		    duve_talk(DUVE_MODESENS, duve_cb,
			S_DU16E2_LACKING_INDEP_PRIV, 10,
			0, prv_tbl.i_priv_grantee,
			sizeof(prv_tbl.d_priv_number), &prv_tbl.d_priv_number,
			sizeof("VIEW") - 1, "VIEW",
			0, duve_cb->duverel->rel[view_offset].du_own,
			0, duve_cb->duverel->rel[view_offset].du_tab);
		}
		if (duve_talk( DUVE_IO, duve_cb, S_DU0302_DROP_TABLE, 4,
			0, duve_cb->duverel->rel[view_offset].du_tab,
			0, duve_cb->duverel->rel[view_offset].du_own)
		    == DUVE_YES)
		{
		    idxchk(duve_cb, prv_tbl.d_obj_id, prv_tbl.d_obj_idx);
		    duve_cb->duverel->rel[view_offset].du_tbldrop = DUVE_DROP;
		    duve_talk( DUVE_MODESENS, duve_cb,
		        S_DU0352_DROP_TABLE, 4,
		        0, duve_cb->duverel->rel[view_offset].du_tab,
		        0, duve_cb->duverel->rel[view_offset].du_own);
		}
	    }
	    else if (prv_tbl.d_obj_type == DB_DBP)
	    { 
		DUVE_DROP_OBJ		*drop_obj;
/* # line 7779 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbp_name, dbp_owner from iiprocedure where dbp_id=");
    IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_id);
    IIwritio(0,(short *)0,1,32,0,(char *)"and dbp_idx=");
    IIputdomio((short *)0,1,30,4,&prv_tbl.d_obj_idx);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,obj_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,sch_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7786 "duvechk2.sc" */	/* host code */
		STtrmwhite(obj_name);
		STtrmwhite(sch_name);
		duve_talk(DUVE_MODESENS, duve_cb,
		    S_DU16E2_LACKING_INDEP_PRIV, 10,
		    0, prv_tbl.i_priv_grantee,
		    sizeof(prv_tbl.d_priv_number), &prv_tbl.d_priv_number,
		    sizeof("PROCEDURE") - 1, "PROCEDURE",
		    0, sch_name,
		    0, obj_name);
		/*
		** before asking user whether we may mark the dbproc dormant,
		** check the last entry added to the "fix it" list - if it 
		** describes the current procedure, we will avoid issuing 
		** redundant messages
		*/
		if (   !(drop_obj = duve_cb->duvefixit.duve_objs_to_drop)
		    || drop_obj->duve_obj_id.db_tab_base   != prv_tbl.d_obj_id
		    || drop_obj->duve_obj_id.db_tab_index  != prv_tbl.d_obj_idx
		    || drop_obj->duve_obj_type 		   != prv_tbl.d_obj_type
		   )
		{
		    if (duve_talk(DUVE_IO, duve_cb, 
			    S_DU0251_MARK_DBP_DORMANT, 0)
		        == DUVE_YES)
		    {
			/* 
			** user decided to allow us to mark the dbproc 
			** dormant - add its description to the "fixit" list 
			*/
			drop_obj = (DUVE_DROP_OBJ *) MEreqmem((u_i4) 0, 
			    sizeof(DUVE_DROP_OBJ), TRUE, &mem_stat);
			if (mem_stat != OK || drop_obj == NULL)
			{
			    duve_talk (DUVE_ALWAYS, duve_cb, 
				E_DU3400_BAD_MEM_ALLOC, 0);
			    return ((DU_STATUS) DUVE_BAD);
			}
			drop_obj->duve_obj_id.db_tab_base  = prv_tbl.d_obj_id;
			drop_obj->duve_obj_id.db_tab_index = prv_tbl.d_obj_idx;
			drop_obj->duve_obj_type            = prv_tbl.d_obj_type;
			drop_obj->duve_secondary_id	   = 0;
			drop_obj->duve_drop_flags	   = 0;
			/* link this description into the existing list */
			drop_obj->duve_next = 
			    duve_cb->duvefixit.duve_objs_to_drop;
			duve_cb->duvefixit.duve_objs_to_drop = drop_obj;
		        duve_talk(DUVE_MODESENS, duve_cb, 
			    S_DU0451_MARKED_DBP_DORMANT, 0);
		    }
		}
	    }
	    else if (prv_tbl.d_obj_type == DB_CONS)
	    { 
	    	DU_STATUS	indep_offset;
		DU_STATUS       tbl_offset;
		DU_I_DPNDS      *indep;
		DU_D_DPNDS	*dep;
		tbl_offset = duve_findreltid(&obj_id, duve_cb);
		if (tbl_offset == DU_INVALID || tbl_offset == DUVE_DROP)
		{
		    /* 
		    ** tbl_offset may get set to DU_INVALID if a table on which
		    ** a constraint was defined no longer exists - this 
		    ** condition will be checked in ckinteg();
		    ** tbl_offset may get set to DUVE_DROP if a table on which a
		    ** constraint was defined was has been marked for 
		    ** destruction
		    */
		    continue;
		}
		duve_talk(DUVE_MODESENS, duve_cb, 
		    S_DU16E4_LACKING_CONS_INDEP_PRIV, 8,
		    0, prv_tbl.i_priv_grantee,
		    sizeof(prv_tbl.d_priv_number), &prv_tbl.d_priv_number,
		    0, duve_cb->duverel->rel[tbl_offset].du_own,
		    0, duve_cb->duverel->rel[tbl_offset].du_tab);
		/* 
		** find the first entry in the independent object info list of 
		** IIDBDEPENDS cache that refers to the current privilege 
		** descriptor
		*/
		indep_offset = duve_i_dpndfind((i4) obj_id.db_tab_base, 
		    (i4) obj_id.db_tab_index, (i4) DB_PRIV_DESCR, 
		    (i4) priv_number, duve_cb, (bool) TRUE);
		for (indep = duve_cb->duvedpnds.duve_indep + indep_offset;
		     indep_offset < duve_cb->duvedpnds.duve_indep_cnt;
		     indep++, indep_offset++)
		{
		    DU_I_DPNDS      *cur;
		    /*
		    ** if this independent object ino element does not describe 
		    ** our privilege descriptor, we are done
		    */
		    if (   obj_id.db_tab_base  	!= indep->du_inid.db_tab_base
			|| obj_id.db_tab_index 	!= indep->du_inid.db_tab_index
			|| indep->du_itype     	!= DB_PRIV_DESCR
			|| priv_number	       	!= indep->du_iqid)
		    {
			break;
		    }
		    /*
		    ** find the dependent object info element to which this 
		    ** independent object info element belongs
		    */
		    for (cur = indep;
			 ~cur->du_iflags & DU_LAST_INDEP;
			 cur = cur->du_next.du_inext)
		    ;
		    /*
		    ** cur now points at the last independent object info 
		    ** element in a ring associated with a given dependent 
		    ** object info element - cur->du_next.du_dep points at the 
		    ** dependent object info element 
		    ** (I would be very surprised if the dependent object type 
		    ** were not DB_CONS, but things may change, so let's check)
		    */
		    dep = cur->du_next.du_dep;
		    if (prv_tbl.d_obj_type == dep->du_dtype)
		    {
			DUVE_DROP_OBJ           *drop_obj;
			/*
			** before asking user whether we may drop this 
			** constraint, check the last entry added to the 
			** "fix it" list - if it describes the current 
			** constraint, we will avoid issuing redundant messages
			*/
			if (   !(drop_obj = 
				     duve_cb->duvefixit.duve_objs_to_drop)
			    || drop_obj->duve_obj_id.db_tab_base   != 
				   dep->du_deid.db_tab_base
			    || drop_obj->duve_obj_id.db_tab_index  != 
				   dep->du_deid.db_tab_index
			    || drop_obj->duve_obj_type 		   != 
				   dep->du_dtype
			    || drop_obj->duve_secondary_id         != 
				   dep->du_dqid
			   )
			{
			    tid = dep->du_deid.db_tab_base;
			    tidx = dep->du_deid.db_tab_index;
			    qid = dep->du_dqid;
/* # line 7945 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select consname from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&tidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and intnumber=");
    IIputdomio((short *)0,1,30,4,&qid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and consflags!=0");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME + 1-1,obj_name);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 7953 "duvechk2.sc" */	/* host code */
			    if (duve_talk(DUVE_IO, duve_cb, 
			            S_DU0247_DROP_CONSTRAINT, 4,
				    0, duve_cb->duverel->rel[tbl_offset].du_own,
				    0, obj_name)
				== DUVE_YES)
			    {
				/* 
				** user decided to drop the constraint - add its
				** description to the "fixit" list 
				*/
				drop_obj = (DUVE_DROP_OBJ *) MEreqmem((u_i4) 0,
				    sizeof(DUVE_DROP_OBJ), TRUE, &mem_stat);
				if (mem_stat != OK || drop_obj == NULL)
				{
				    duve_talk (DUVE_ALWAYS, duve_cb, 
					E_DU3400_BAD_MEM_ALLOC, 0);
				    return ((DU_STATUS) DUVE_BAD);
				}
				drop_obj->duve_obj_id.db_tab_base  = 
				    dep->du_deid.db_tab_base;
				drop_obj->duve_obj_id.db_tab_index = 
				    dep->du_deid.db_tab_index;
				drop_obj->duve_obj_type            = 
				    dep->du_dtype;
				drop_obj->duve_secondary_id	   = 
				    dep->du_dqid;
				drop_obj->duve_drop_flags	   = 0;
				/* 
				** link this description into the existing list 
				*/
				drop_obj->duve_next = 
				    duve_cb->duvefixit.duve_objs_to_drop;
				duve_cb->duvefixit.duve_objs_to_drop = drop_obj;
				duve_talk(DUVE_MODESENS, duve_cb,
				    S_DU0447_DROP_CONSTRAINT, 4,
				    0, duve_cb->duverel->rel[tbl_offset].du_own,
				    0, obj_name);
			    }
			}
		    }
		}
	    }
	}
	/* end of test 2 */
    } /* end loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckpriv"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: duve_check_privs - determine whether a specified user possesses 
**			    required privilege on the specified object
**
** Description:
**      this routine will scan iiprotect trying to determine whether a specified
**      user (or PUBLIC) possess required privilege on the specified object
**
** Inputs:
**	objid			id of the object
**	privneeded		required privilege
**	privgrantee		user who must possess the required privilege
**	attrmap			map of attributes of a table or view on which 
**				the user must possess a specified privilege
**
** Outputs:
**      satisfied		TRUE if the specified user (or PUBLIC) possess 
**				required privilege on the specified object; 
**				FALSE otherwise
** Returns:
**  	none
**
** Exceptions:
**    	none
**
** Side Effects:
**	none
**
** History:
**      20-jan-94 (andre)
**          written
**	9-Sep-2004 (schka24)
**	    1024 column expansion
**	28-Feb-2008 (kibro01) b119427
**	    The DBA doesn't have the GRANT option in the bitmask in iiprotect,
**	    but they always have an implicit GRANT option to anything.
[@history_template@]...
*/
static VOID
duve_check_privs( 
		 DB_TAB_ID	*objid, 
		 i4		privneeded, 
		 char		*privgrantee, 
		 u_i4		*attrmap, 
		 bool		*satisfied)
{
  u_i4 priv_tid;
  u_i4 priv_tidx;
  char *priv_grantee;
  i4 priv;
  i4 priv_wgo;
  i4 priv_wogo;
  i4 user_grantee;
  i4 public_grantee;
  u_i4 attr_word1;
  u_i4 attr_word2;
  u_i4 attr_word3;
  u_i4 attr_word4;
  u_i4 attr_word5;
  u_i4 attr_word6;
  u_i4 attr_word7;
  u_i4 attr_word8;
  u_i4 attr_word9;
  u_i4 attr_worda;
  u_i4 attr_word11;
  u_i4 attr_word12;
  u_i4 attr_word13;
  u_i4 attr_word14;
  u_i4 attr_word15;
  u_i4 attr_word16;
  u_i4 attr_word17;
  u_i4 attr_word18;
  u_i4 attr_word19;
  u_i4 attr_word20;
  u_i4 attr_word21;
  u_i4 attr_word22;
  u_i4 attr_word23;
  u_i4 attr_word24;
  u_i4 attr_word25;
  u_i4 attr_word26;
  u_i4 attr_word27;
  u_i4 attr_word28;
  u_i4 attr_word29;
  u_i4 attr_word30;
  u_i4 attr_word31;
  u_i4 attr_word32;
  u_i4 attr_word33;
  i4 cnt;
    *satisfied = FALSE;
    priv_tid = objid->db_tab_base;
    priv_tidx = objid->db_tab_index;
    priv_grantee = privgrantee;
    /* 
    ** if an object depends on [GRANT OPTION FOR] SELECT, we need to set 
    ** two more bits in priv_needed since a permit conveying SELECT 
    ** privilege has three bits set in iiprotect.proopset: DB_RETRIEVE, 
    ** DB_TEST, and DB_AGGREGATE
    */
    priv = (privneeded & DB_RETRIEVE) ? (privneeded | DB_TEST | DB_AGGREGATE)
				      : privneeded;
    priv_wgo = priv | DB_GRANT_OPTION;
    priv_wogo = priv & ~DB_GRANT_OPTION;
    user_grantee = DBGR_USER;
    public_grantee = DBGR_PUBLIC;
    /* 
    ** we need to make sure that the privilege applies to all columns 
    ** described in attrmap only if the privilege is on a table; otherwise we
    ** only need to ascertain that there is a tuple in IIPROTECT which grants
    ** specified privilege to the specified user (or public) on the specified 
    ** object
    */
    if (  privneeded 
	& (DB_RETRIEVE | DB_REPLACE | DB_DELETE | DB_APPEND | 
		DB_COPY_FROM | DB_COPY_INTO | DB_REFERENCES))
    {
        i4	i;
        u_i4	attrs_to_find[DB_COL_WORDS];
    	u_i4	*priv_attrs[DB_COL_WORDS];
	/* 
	** make a copy of users attribute map since we plan to clear some (and
	** hope to clear all) bits in it
	*/
	for (i = 0; i < DB_COL_WORDS; i++)
	    attrs_to_find[i] = attrmap[i];
	/* 
	** store addresses of attr_word1-attr_worda into priv_attrs to simplify
	** code determining whether the privilege has been satisfied
	*/
	priv_attrs[0] = &attr_word1;
	priv_attrs[1] = &attr_word2;
	priv_attrs[2] = &attr_word3;
	priv_attrs[3] = &attr_word4;
	priv_attrs[4] = &attr_word5;
	priv_attrs[5] = &attr_word6;
	priv_attrs[6] = &attr_word7;
	priv_attrs[7] = &attr_word8;
	priv_attrs[8] = &attr_word9;
	priv_attrs[9] = &attr_worda;
	priv_attrs[10] = &attr_word11;
	priv_attrs[11] = &attr_word12;
	priv_attrs[12] = &attr_word13;
	priv_attrs[13] = &attr_word14;
	priv_attrs[14] = &attr_word15;
	priv_attrs[15] = &attr_word16;
	priv_attrs[16] = &attr_word17;
	priv_attrs[17] = &attr_word18;
	priv_attrs[18] = &attr_word19;
	priv_attrs[19] = &attr_word20;
	priv_attrs[20] = &attr_word21;
	priv_attrs[21] = &attr_word22;
	priv_attrs[22] = &attr_word23;
	priv_attrs[23] = &attr_word24;
	priv_attrs[24] = &attr_word25;
	priv_attrs[25] = &attr_word26;
	priv_attrs[26] = &attr_word27;
	priv_attrs[27] = &attr_word28;
	priv_attrs[28] = &attr_word29;
	priv_attrs[29] = &attr_word30;
	priv_attrs[30] = &attr_word31;
	priv_attrs[31] = &attr_word32;
	priv_attrs[32] = &attr_word33;
/* # line 8203 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"check_priv_cursor",6290,24900);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select prodomset1, prodomset2, prodomset3, prodomset4, prodomset5, pr\
odomset6, prodomset7, prodomset8, prodomset9, prodomseta, prodomset11,\
 prodomset12, prodomset13, prodomset14, prodomset15, prodomset16, prod\
omset17, prodomset18, prodomset19, prodomset20");
    IIwritio(0,(short *)0,1,32,0,(char *)
", prodomset21, prodomset22, prodomset23, prodomset24, prodomset25, pr\
odomset26, prodomset27, prodomset28, prodomset29, prodomset30, prodoms\
et31, prodomset32, prodomset33 from iiprotect where protabbase=");
    IIputdomio((short *)0,1,30,4,&priv_tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and protabidx=");
    IIputdomio((short *)0,1,30,4,&priv_tidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and((progtype=");
    IIputdomio((short *)0,1,30,4,&user_grantee);
    IIwritio(0,(short *)0,1,32,0,(char *)"and prouser=");
    IIputdomio((short *)0,1,32,0,priv_grantee);
    IIwritio(0,(short *)0,1,32,0,(char *)")or progtype=");
    IIputdomio((short *)0,1,30,4,&public_grantee);
    IIwritio(0,(short *)0,1,32,0,(char *)")and prograntor!=");
    IIputdomio((short *)0,1,32,0,priv_grantee);
    IIwritio(0,(short *)0,1,32,0,(char *)"and(proopset=");
    IIputdomio((short *)0,1,30,4,&priv);
    IIwritio(0,(short *)0,1,32,0,(char *)"or proopset=");
    IIputdomio((short *)0,1,30,4,&priv_wgo);
    IIwritio(0,(short *)0,1,32,0,(char *)"or(proopset=");
    IIputdomio((short *)0,1,30,4,&priv_wogo);
    IIwritio(0,(short *)0,1,32,0,(char *)"and prouser=DBMSINFO('DBA'))\
)");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"check_priv_cursor",6290,24900);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8205 "duvechk2.sc" */	/* host code */
	/* 
	** examine all tuples conveying specified privilege to the specified 
	** user (or public) on the specified object
	*/
        for (; !*satisfied; )  
        {
	    /* get tuple from iiprotect */
/* # line 8212 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"check_priv_cursor",6290,24900,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&attr_word1);
      IIcsGetio((short *)0,1,30,4,&attr_word2);
      IIcsGetio((short *)0,1,30,4,&attr_word3);
      IIcsGetio((short *)0,1,30,4,&attr_word4);
      IIcsGetio((short *)0,1,30,4,&attr_word5);
      IIcsGetio((short *)0,1,30,4,&attr_word6);
      IIcsGetio((short *)0,1,30,4,&attr_word7);
      IIcsGetio((short *)0,1,30,4,&attr_word8);
      IIcsGetio((short *)0,1,30,4,&attr_word9);
      IIcsGetio((short *)0,1,30,4,&attr_worda);
      IIcsGetio((short *)0,1,30,4,&attr_word11);
      IIcsGetio((short *)0,1,30,4,&attr_word12);
      IIcsGetio((short *)0,1,30,4,&attr_word13);
      IIcsGetio((short *)0,1,30,4,&attr_word14);
      IIcsGetio((short *)0,1,30,4,&attr_word15);
      IIcsGetio((short *)0,1,30,4,&attr_word16);
      IIcsGetio((short *)0,1,30,4,&attr_word17);
      IIcsGetio((short *)0,1,30,4,&attr_word18);
      IIcsGetio((short *)0,1,30,4,&attr_word19);
      IIcsGetio((short *)0,1,30,4,&attr_word20);
      IIcsGetio((short *)0,1,30,4,&attr_word21);
      IIcsGetio((short *)0,1,30,4,&attr_word22);
      IIcsGetio((short *)0,1,30,4,&attr_word23);
      IIcsGetio((short *)0,1,30,4,&attr_word24);
      IIcsGetio((short *)0,1,30,4,&attr_word25);
      IIcsGetio((short *)0,1,30,4,&attr_word26);
      IIcsGetio((short *)0,1,30,4,&attr_word27);
      IIcsGetio((short *)0,1,30,4,&attr_word28);
      IIcsGetio((short *)0,1,30,4,&attr_word29);
      IIcsGetio((short *)0,1,30,4,&attr_word30);
      IIcsGetio((short *)0,1,30,4,&attr_word31);
      IIcsGetio((short *)0,1,30,4,&attr_word32);
      IIcsGetio((short *)0,1,30,4,&attr_word33);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8222 "duvechk2.sc" */	/* host code */
	    if (sqlca.sqlcode == 100) /* then no more tuples in table */
	    {
/* # line 8224 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"check_priv_cursor",6290,24900);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8225 "duvechk2.sc" */	/* host code */
	        break;
	    }
	    /* be optimistic */
	    *satisfied = TRUE;
	    for (i = 0; i < DB_COL_WORDS; i++)
	    {
		if (attrs_to_find[i] &= ~*priv_attrs[i])
		    *satisfied = FALSE;
	    }
	}
	if (*satisfied)
	{
	    /*
	    ** we exited the loop because we determined that the specified 
	    ** user possesses required privilege on the specified table or 
	    ** view - must remember to close the cursor
	    */
/* # line 8245 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"check_priv_cursor",6290,24900);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8246 "duvechk2.sc" */	/* host code */
	}
    }
    else
    {
/* # line 8250 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk237",6291,3882);
      IIputdomio((short *)0,1,30,4,&priv_tid);
      IIputdomio((short *)0,1,30,4,&priv_tidx);
      IIputdomio((short *)0,1,30,4,&user_grantee);
      IIputdomio((short *)0,1,32,0,priv_grantee);
      IIputdomio((short *)0,1,30,4,&public_grantee);
      IIputdomio((short *)0,1,30,4,&priv);
      IIputdomio((short *)0,1,30,4,&priv_wgo);
      IIputdomio((short *)0,1,30,4,&priv_wogo);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk237",6291,3882);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(protabbase)from iiprotect where protabbase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&priv_tid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and protabidx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&priv_tidx);
        IIwritio(0,(short *)0,1,32,0,(char *)"and((progtype=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&user_grantee);
        IIwritio(0,(short *)0,1,32,0,(char *)"and prouser=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,priv_grantee);
        IIwritio(0,(short *)0,1,32,0,(char *)")or progtype=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&public_grantee);
        IIwritio(0,(short *)0,1,32,0,(char *)")and(proopset=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&priv);
        IIwritio(0,(short *)0,1,32,0,(char *)"or proopset=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&priv_wgo);
        IIwritio(0,(short *)0,1,32,0,(char *)"or(proopset=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,30,4,&priv_wogo);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and prouser=DBMSINFO('DBA')))");
        IIexDefine(0,(char *)"duvechk237",6291,3882);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8259 "duvechk2.sc" */	/* host code */
	*satisfied = (cnt != 0);
    }
    return;
}
/*{
** Name: ckkey - run verifydb test #1 to check system catalog iikey
**
** Description:
**      This routine opens a cursor to walk iikey one tuple at a time.
**	It performs checks on each tuple.
**
**      TEST 1: Verify that for each constraint id, all columns in its key
**                are legal columns (for the table) and that all positions
**                (1-n) in the key have columns assigned to them.
**              TEST:   Select count(attid) from iikey k, iiattribute a,
**                             iirelation r
**                      where k.key_attid = a.attid and
**                             a.attrelid = r.reltid.
**              If count(attid) is zero, then we have detected error.
**
** Inputs:
**      duve_cb                         verifydb control block
**
** Outputs:
**      none.
**
**      Returns:
**          DUVE_YES
**          DUVE_BAD
**      Exceptions:
**          none
** Side Effects:
**          tuples read from iiattribute
**          tuples read from iirelation
**
** Histry:
**      18-nov-92 (anitap)
**          initial creation
**	04-nov-93 (anitap)
**	    Added null indicator variable 'keyind' to get rid of the following
**	    error: "E_LQ000E No null indicator supplied with program variable.
**	    Null data from column 3 can not be converted."
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
**	01-jan-94 (anitap)
**	    Performance enhancements. Use repeat query and ANY instead
**	    of COUNT.
**	09-april-2003 (inifa01) bug 110019 INGSRV 2204
**	    verifydb ... -odbms check on a referential constraint, where constraint 
**	    columns are not part of a key, returns error;
**	    S_DU168F_ILLEGAL_KEY Attribute with constraint id <ID> is not a legal
**	    column for the key. 
**	    Fix: Check if attkdom in iiattribute is not turned on b/cos we're dealing
**	    with a CONS_REF, then don't return error. 
[@history_template@]...
*/
DU_STATUS
ckkey (duve_cb)
DUVE_CB                 *duve_cb;
{
    DU_STATUS                base;
# include <duvekey.h>
  short keyind[4];
  i4 cnt;
  i4 cons_type;
  i4 id1, id2;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 8344 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"key_cursor",6292,14784);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iikey");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"key_cursor",6292,14784);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8346 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iikey */
    for (;;)
    {
        /* get tuple from table */
/* # line 8350 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"key_cursor",6292,14784,-1,-1) != 0) {
      IIcsGetio(&keyind[0],1,30,4,&(key_tbl).key_consid1);
      IIcsGetio(&keyind[1],1,30,4,&(key_tbl).key_consid2);
      IIcsGetio(&keyind[2],1,30,2,&(key_tbl).key_attid);
      IIcsGetio(&keyind[3],1,30,2,&(key_tbl).key_position);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8351 "duvechk2.sc" */	/* host code */
        if (sqlca.sqlcode == 100) /* then no more tuples in iikey */
        {
/* # line 8353 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"key_cursor",6292,14784);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8354 "duvechk2.sc" */	/* host code */
           break;
        }
        /* test 1 -- verify that for each constraint id, all columns in its
	**	     key are legal columns and that all positions (1-n)
	**	     in the key have columns assigned to them.
	*/
/* # line 8362 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk238",6293,14384);
      IIputdomio((short *)0,1,30,4,&key_tbl.key_consid1);
      IIputdomio((short *)0,1,30,4,&key_tbl.key_consid2);
      IIputdomio((short *)0,1,30,2,&key_tbl.key_position);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk238",6293,14384);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(attid)from iiattribute, iiintegrity where consid1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&key_tbl.key_consid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and consid2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&key_tbl.key_consid2);
        IIwritio(0,(short *)0,1,32,0,(char *)"and attkdom=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,2,&key_tbl.key_position);
        IIwritio(0,(short *)0,1,32,0,(char *)"and inttabbase=attrelid");
        IIexDefine(0,(char *)"duvechk238",6293,14384);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8370 "duvechk2.sc" */	/* host code */
        if (cnt == NULL)
        {
           /* The key tuple is not referenced in iiattribute
           */
           if (duve_banner(DUVE_IIKEY, 1, duve_cb)
                  == DUVE_BAD)
                return ( (DU_STATUS) DUVE_BAD);
	   /* (inifa01) could be no rows where returned b/cos 
	      attkdom == 0 != :key_tbl.key_position, which will be
	      the case for referential constraint columns not part
	      of a key eg CONS_REF created WITH NO INDEX.
	   */
/* # line 8383 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk239",6294,1834);
      IIputdomio((short *)0,1,30,4,&key_tbl.key_consid1);
      IIputdomio((short *)0,1,30,4,&key_tbl.key_consid2);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk239",6294,1834);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select consflags from iiintegrity where consid1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&key_tbl.key_consid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and consid2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&key_tbl.key_consid2);
        IIexDefine(0,(char *)"duvechk239",6294,1834);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cons_type);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8386 "duvechk2.sc" */	/* host code */
	   if (!(cons_type & CONS_REF))
               duve_talk (DUVE_MODESENS, duve_cb, S_DU168F_ILLEGAL_KEY, 2,
                        0, key_tbl.key_consid1);
           continue;
        } /* end test 1 */
    } /* end loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckkey"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: ckdefault - run verifydb test #1 to check system catalog iidefault
**
** Description:
**      This routine opens a cursor to walk iidefault one tuple at a time.
**      It performs checks and gives user the option to delete the default
**      tuple.
**
**      TEST 1: Verify tht each non-canonical default tuple is referenced
**               by some attribute.
**              TEST:   Select count(defid1) from iidefault , iiattribute a
**                      where d.defid1 = a.attdefid1 and
**                              d.defid2 = a.attdefid2.
**              if any(defid1) is zero, then we have detected error.
**      FIX:    Offer the user the option to delete the default tuple.
**
** Inputs:
**     duve_cb                         verifydb control block
**
** Outputs:
**      none.
**
**      Returns:
**          DUVE_YES
**          DUVE_BAD
**      Exceptions
**          none
** Side Effects:
**          tuples read from iiattribute
**          tuples may be deleted from iidefault
**
** History:
**     18-nov-92 (anitap)
**         initial creation.
**     25-oct-93 (anitap)
**	   Added indicator variable for default_tbl.
**	10-dec-93 (teresa)
**	    performance enchancement for verifydb.  Includes performance 
**	    statistics reporting, changing many queries to repeated queries,
**	    modifying existing queries to be more efficient (such as using
**	    key columns in the qualification).  Also added logic to suppress
**	    some non-significant difs during run-all testing, including a
**	    change to duve_log parameters).
**	20-dec-93 (anitap)
**	    Use repeat query and ANY instead of COUNT to improve performance.
**	    Added the two cases DB_DEF_ID_NEEDS_CONVERTING & 
**	    DB_DEF_ID_UNRECOGNIZED to test 3.
**	    
[@history_template@]..
*/
DU_STATUS
ckdefault (duve_cb)
DUVE_CB                 *duve_cb;
{
   DU_STATUS                base;
# include <duvedef.h>
  short defind[3];
  i4 cnt;
    /* set up to gather performance statistics */
    init_stats(duve_cb, (i4) DUVE_TEST_PERF);    
/* # line 8470 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"default_cursor",6295,14522);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iidefault");
    IIcsQuery((char *)"default_cursor",6295,14522);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8472 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iidefault */
    for (;;)
    {
	/* get tuple from table */
/* # line 8476 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"default_cursor",6295,14522,-1,-1) != 0) {
      IIcsGetio(&defind[0],1,30,4,&(default_tbl).defid1);
      IIcsGetio(&defind[1],1,30,4,&(default_tbl).defid2);
      IIcsGetio(&defind[2],1,32,1501,(default_tbl).defvalue);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8477 "duvechk2.sc" */	/* host code */
        if (sqlca.sqlcode == 100) /* then no more tuples in iidefault */
        {
/* # line 8479 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"default_cursor",6295,14522);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8480 "duvechk2.sc" */	/* host code */
          break;
        }
        /* test 1 -- verify each non-canonical default tuple is referenced
        ** by some attribute
        */
	/* check if canonical default */
	switch(default_tbl.defid1)
        {
            case DB_DEF_NOT_DEFAULT:
            case DB_DEF_NOT_SPECIFIED:
            case DB_DEF_UNKNOWN:
            case DB_DEF_ID_0:
            case DB_DEF_ID_BLANK:
            case DB_DEF_ID_TABKEY:
            case DB_DEF_ID_LOGKEY:
            case DB_DEF_ID_NULL:
            case DB_DEF_ID_USERNAME:
            case DB_DEF_ID_CURRENT_DATE:
            case DB_DEF_ID_CURRENT_TIMESTAMP:
            case DB_DEF_ID_CURRENT_TIME:
            case DB_DEF_ID_SESSION_USER:
            case DB_DEF_ID_SYSTEM_USER:
            case DB_DEF_ID_INITIAL_USER:
            case DB_DEF_ID_DBA:
            case DB_DEF_ID_NEEDS_CONVERTING:
            case DB_DEF_ID_UNRECOGNIZED:
            {
                /* "special" default - no need to check IIDEFAULT */
                break;
            }
            default:
            {
/* # line 8514 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk240",6296,14124);
      IIputdomio((short *)0,1,30,4,&default_tbl.defid1);
      IIputdomio((short *)0,1,30,4,&default_tbl.defid2);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk240",6296,14124);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select any(attdefid1)from iiattribute where attdefid1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&default_tbl.defid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and attdefid2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&default_tbl.defid2);
        IIexDefine(0,(char *)"duvechk240",6296,14124);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8519 "duvechk2.sc" */	/* host code */
	   	if (cnt == NULL)
	   	{
	      	    /* The default tuple is not referenced in iiattribute */
	      	    if (duve_banner(DUVE_IIDEFAULT, 1, duve_cb) == DUVE_BAD)
		      return ( (DU_STATUS) DUVE_BAD);
	      	   duve_talk (DUVE_MODESENS, duve_cb,
			S_DU168E_UNREFERENCED_DEFTUPLE, 4,
			0, default_tbl.defid1, 0, default_tbl.defid2);
	      	        if (duve_talk(DUVE_ASK, duve_cb, S_DU034B_DELETE_IIDEFAULT, 
			    4, 0, default_tbl.defid1, 0, default_tbl.defid2)
				    == DUVE_YES) 
	           {
/* # line 8533 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iidefault",(char *)"default_cursor",6295,14522);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8534 "duvechk2.sc" */	/* host code */
		      duve_talk(DUVE_MODESENS, duve_cb,
			    S_DU039B_DELETE_IIDEFAULT, 4,
			    0, default_tbl.defid1, 0, default_tbl.defid2);
	           }
	           continue;
	       } /* endif cnt = null */ 
	       break;
	} 
      } /* end test 1 */	
    } /* end loop for each tuple */
    /* report performance statistics */
    printstats(duve_cb, (i4) DUVE_TEST_PERF, "ckdefault"); 
    return ( (DU_STATUS) DUVE_YES);
}
/*{
** Name: treeclean - clean up integrities, permits, views for which th tree
**		     is dropped.
**	         
**
** Description:
**      This routine drops view, integrity, permit and rule tuples for
**	which the tree is dropped, as follows:
**	  case tree_table.treemode
**	    DB_VIEW - mark in duverel as dropped
**	    DB_PROT - delete tuple from iiprotect
**	    DB_RULE - delete tuple from iirule
**	    DB_INTG - delete tuple from iiintegrity
**
**
** Inputs:
**	base				offset into duverel for this entry
**					  (only used for views)
**	tree				table id and tree id
**	mode				tree type (from tree_tbl.treemode)
**      duve_cb                         verifydb control block
**	    duverel                          cached iirelation information
**
** Outputs:
**      duve_cb                         verifydb control block
**	    duverel                         cached iirelation information
**	        du_tbldrop			flag indicating to drop table
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    tuples may be dropped from iiprotect, iiintegrity or iirule. A
**	    view tuple in duverel cache may be marked for deletion
**
** History:
**      20-feb-91 (teresa)
**          initial creation for bug 35940
*/
VOID
treeclean( base, tree, mode, duve_cb)
DU_STATUS	   base;
DUVE_TREE	   *tree;
short		   mode;
DUVE_CB		   *duve_cb;
{
  u_i4 tid, tidx;
  u_i4 tree1, tree2;
/* # line 8606 "duvechk2.sc" */	/* host code */
    switch (mode)
    {
    case DB_VIEW:
	/* mark this view as dropped */
	duve_cb->duverel->rel[base].du_tbldrop = DUVE_DROP;
	break;
    case DB_PROT:
	tid = tree->tabid.db_tab_base;
	tidx = tree->tabid.db_tab_index;
	tree1 = tree->treeid.db_tre_high_time;
	tree2 = tree->treeid.db_tre_low_time;
/* # line 8617 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiprotect where protabbase=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and protabidx=");
    IIputdomio((short *)0,1,30,4,&tidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and protreeid1=");
    IIputdomio((short *)0,1,30,4,&tree1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and protreeid2=");
    IIputdomio((short *)0,1,30,4,&tree2);
    IIsyncup((char *)0,0);
  }
/* # line 8619 "duvechk2.sc" */	/* host code */
	break;
    case DB_INTG:
	tid = tree->tabid.db_tab_base;
	tidx = tree->tabid.db_tab_index;
	tree1 = tree->treeid.db_tre_high_time;
	tree2 = tree->treeid.db_tre_low_time;
/* # line 8625 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiintegrity where inttabbase=");
    IIputdomio((short *)0,1,30,4,&tid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
    IIputdomio((short *)0,1,30,4,&tidx);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttreeid1=");
    IIputdomio((short *)0,1,30,4,&tree1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and inttreeid2=");
    IIputdomio((short *)0,1,30,4,&tree2);
    IIsyncup((char *)0,0);
  }
/* # line 8627 "duvechk2.sc" */	/* host code */
	break;
    case DB_RULE:
	tree1 = tree->treeid.db_tre_high_time;
	tree2 = tree->treeid.db_tre_low_time;
/* # line 8632 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iirule where rule_treeid1=");
    IIputdomio((short *)0,1,30,4,&tree1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and rule_treeid2=");
    IIputdomio((short *)0,1,30,4,&tree2);
    IIsyncup((char *)0,0);
  }
/* # line 8633 "duvechk2.sc" */	/* host code */
	break;
    default:
	break;
    }
}
/*{
** Name: create_cons - drop constraint and recreate the same from the qrytext
**			in iiqrytext catalog.
**
** Description:
**	This routine drops UNIQUE/CHECK/REFERENTIAL constraint on a table and
**	creates a SQL file to create the same. The constraint is dropped 
**	because one of the following database objects may have been missing: 
**	rule, procedure, index, iidbdepends tuple.
**
** Inputs:
**	duve_cb			verifydb control block
**	iiintegrity		ptr to structure containing all attributes in 
**				iiintegrity tuple
**	base			offset into iirelation cache for the table
**				on which the constraint needs to be 
**				dropped/added
**
** Outputs:
**	duve_cb			verifydb control block
**	Returns:
**		DUVE_YES,
**		DUVE_NO,
**		DUVE_BAD
**	Exceptions:
**		none
**
** Side Effects:
**	old tuples in iirule/iiprocedure/iiindex/iiintegrity/iidbdepends
**	/iiqrytext may be dropped.	
**	new tuples may be entered into iirule/iiprocedure/iiindex/iiintegrity/
**	iidbdepends/iiqrytext at the end of verifydb.
**
** History:
**	01-nov-93 (anitap)
**	    initial creation.
**	05-jan-94 (anitap)
**	    use iidbdepends cache to count the number of referential constraints
**	    dependent on the unique constraint (which is being dropped).
**	    Also take care of the condition when more than one referential 
**	    constraint is defined on a table.
**	    Allocate memory dynamically for cases where the query text sstring
**	    spans more than one tuple in iiqrytext catalog.	
[@history_template@]...
*/
static DU_STATUS
create_cons (duve_cb, iiintegrity, base)
DUVE_CB		*duve_cb;
DB_INTEGRITY	*iiintegrity;
DU_STATUS	base;
{
  u_i4 qryid1, qryid2;
  u_i4 inum, rflag;
  u_i4 id1, id2;
        u_i4		tid, tidx, flag;	
        u_i4 		len;	
        char		cons[DB_MAXNAME + 1];
	DB_TAB_ID	ref_id;
	DU_STATUS	rbase;
	DU_STATUS	indep_offset, status;
	DU_I_DPNDS	*indep;
	DU_D_DPNDS	*dep;
	DB_QRY_ID	qyid;
	i4		i;
	bool		ref = FALSE;
	bool		first = TRUE;
        i4		ind_type = DB_CONS; 
/* # line 8714 "duvechk2.sc" */	/* host code */
	tid = iiintegrity->dbi_tabid.db_tab_base;
	tidx = iiintegrity->dbi_tabid.db_tab_index;
	inum = iiintegrity->dbi_number;
	flag = iiintegrity->dbi_consflags;
	qyid.db_qry_high_time = iiintegrity->dbi_txtid.db_qry_high_time;
	qyid.db_qry_low_time = iiintegrity->dbi_txtid.db_qry_low_time;
	STcopy(iiintegrity->dbi_consname.db_constraint_name, cons);
        len = STtrmwhite(cons);
	/* cache info about this tuple */
        MEcopy( (PTR)cons, DB_MAXNAME,
         (PTR) duve_cb->duveint->integrities[duve_cb->duve_intcnt].du_consname);
        MEcopy( (PTR)duve_cb->duverel->rel[base].du_own,
			DB_MAXNAME,           
	   (PTR)duve_cb->duveint->integrities[duve_cb->duve_intcnt].du_ownname);
        MEcopy( (PTR)duve_cb->duverel->rel[base].du_tab, DB_MAXNAME,
           (PTR)duve_cb->duveint->integrities[duve_cb->duve_intcnt].du_tabname);
	/* 
      	** drop constraint.
        */
      	duve_talk (DUVE_MODESENS, duve_cb,
			   S_DU0370_DROP_FROM_IIINTEGRITY,
		 4, sizeof(tid), &tid, sizeof(tidx), &tidx);
        if (duve_talk (DUVE_ASK, duve_cb, S_DU0246_CREATE_CONSTRAINT, 0)
			== DUVE_YES)
	{
           duve_cb->duveint->integrities[duve_cb->duve_intcnt].du_acons =
                               DUVE_ADD;
	   /* Get the query text for creating the constraint */
	   if ((status = alloc_mem(duve_cb, &qyid, 
	     &duve_cb->duveint->integrities[duve_cb->duve_intcnt].du_txtstring))
				!= DUVE_YES)
	      return(status);
	   if (flag == CONS_UNIQUE)
	   {
	      /* If UNIQUE constraint, we can have a referential constraint
	      ** depending on this constraint. Dropping the UNIQUE constraint
	      ** will also drop the REFERENTIAL constraint.
	      ** 
	      ** We want to get the query text for the referential constraint
	      ** before dropping the UNIQUE constraint.
	      ** We can do this by querying the IIDBDEPENDS catalog.
	      */
	      indep_offset = duve_i_dpndfind( tid, tidx,
				ind_type, inum, duve_cb, first);
	      if (indep_offset != DU_INVALID)
	      {
	         indep = duve_cb->duvedpnds.duve_indep + indep_offset;
	         for (i = 0, dep = indep->du_next.du_dep;
			i < duve_cb->duvedpnds.duve_dep_cnt;
			i++, dep++)
	         {
		     if (dep->du_dtype == DB_CONS
			 && dep->du_indep->du_inid.db_tab_base ==
				indep->du_inid.db_tab_base)
		     {
		        id1 = dep->du_deid.db_tab_base;
		        id2 = dep->du_deid.db_tab_index;
			inum = dep->du_dqid;
		        /* We have a referential constraint depending on this
	                ** unique constraint.
		        ** Get the query text for the referential constraint.
		        */
		        /* We have a referential constraint depending on this
                        ** unique constraint.
                        */
                        /* Get the table name and owner on which
                        ** referential constraint
                        ** needs to be defined.
                        */
	                ref_id.db_tab_base = id1;
		        ref_id.db_tab_index = id2;
                        rbase = duve_findreltid ( &ref_id, duve_cb);
	                /* We do not want to recreate the referential
		        ** constraint on the table if it has been marked
			** for drop.
		        */
	 	        if (rbase != DUVE_DROP ||
			     rbase != DU_INVALID)
		        {
	                   /* cache info about the constraint text,
			   ** table name and owner
                    	   */
                   duve_cb->duveint->integrities[duve_cb->duve_intcnt].du_rcons =
                                         DUVE_ADD;
                           MEcopy( (PTR)duve_cb->duverel->rel[rbase].du_own,
					DB_MAXNAME,
          (PTR)duve_cb->duveint->integrities[duve_cb->duve_intcnt].du_rownname);
                   MEcopy( (PTR)duve_cb->duverel->rel[rbase].du_tab, DB_MAXNAME,
          (PTR)duve_cb->duveint->integrities[duve_cb->duve_intcnt].du_rtabname);
		           rflag = CONS_REF;
/* # line 8828 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk241",6297,21038);
      IIputdomio((short *)0,1,30,4,&id1);
      IIputdomio((short *)0,1,30,4,&id2);
      IIputdomio((short *)0,1,30,4,&rflag);
      IIputdomio((short *)0,1,30,4,&inum);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk241",6297,21038);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select intqryid1, intqryid2 from iiintegrity where inttabbase=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and inttabidx=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&id2);
        IIwritio(0,(short *)0,1,32,0,(char *)"and consflags=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&rflag);
        IIwritio(0,(short *)0,1,32,0,(char *)"and intnumber=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&inum);
        IIexDefine(0,(char *)"duvechk241",6297,21038);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&qryid1);
      IIgetdomio((short *)0,1,30,4,&qryid2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8836 "duvechk2.sc" */	/* host code */
			   qyid.db_qry_high_time = qryid1;
			   qyid.db_qry_low_time = qryid2;
	   	           /* Get the query text for creating the 
		           ** referential constraint 
		           */
			   if ((status = alloc_mem(duve_cb, &qyid,
	  &duve_cb->duveint->integrities[duve_cb->duve_intcnt].du_rtxtstring))
				!= DUVE_YES)
			      return(status);
		} /* endif rbase != DU_INVALID or DUVE_DROP */
	        ref = TRUE; 
	        duve_cb->duve_intcnt++;
	      } /* endif referential constraint dependent on this unique cons*/	
	     } /* end of for loop */	
	    } /* endif indep_offset != DU_INVALID */
	   } /* endif consflags = CONS_UNIQUE */
	   if (ref == FALSE)
	      duve_cb->duve_intcnt++;
      	   duve_talk (DUVE_MODESENS, duve_cb,
			   S_DU0446_CREATE_CONSTRAINT, 0);
	} /* endif create constraint */
	return ((DU_STATUS)DUVE_YES);	
}
/*{
** Name: alloc_mem - allocate memory dynamically to cache the query text to 
**			create the constraint and cache the query text.
**
** Description:
** 	If the integrity tuple spans more than one tuple, we need to
** 	dynamically allocate the memory for caching the query text
** 	string.
**
** 	We need to find the len of the total query text. We do thiis
** 	by calculating the length of the last tuple and adding to the
** 	product of the sequence and 240, i.e., len(last tuple) 
** 	+ (seq * 240).
**
** 	We find the length of the last tuple by counting the number of
** 	iiqrytext tuples and using count - 1 to match with sequence
** 	number.
**
**	This routine is called to allocate memory and cache query text for
**	both unique constraint and for referential constraint which depends
**	on the unique constraint.
**
** Inputs:
**	duve_cb		verifydb control block
**	iiqrytext	structure containing
**			query text id of text which needs to be cached
**	string		pointer to query text string where the query text will
**			be cached.
**
** Outputs:
**	duve_cb		verifydb control block
**	Returns:
**		DUVE_YES,
**		DUVE_BAD
**	Exceptions:
**		none
**
** Side Effects:
**	tuples will be read from iiqrytext.
**
** History:
**	09-jan-94 (anitap)
**	    Created for FIPS' constraints project.
**	    Corrected memory corruption bug. Was allocating correct memory.
**	    But was trying to MEcopy a larger chunk of test string. 
**	    Also null terminate the text string.
[@history_template@]...
*/
static DU_STATUS
alloc_mem(duve_cb, quyid, string)
DUVE_CB		*duve_cb;
DB_QRY_ID	*quyid;	
char		**string;
{
  i4 cnt = 0;
  u_i4 queryid1;
  u_i4 queryid2;
  char text[240];
	STATUS          mem_text;
	u_i4           size;
	u_i4		length, str_len;
	char		*save_ptr;
	char		*txt_ptr;
	i4		i;
/* # line 8941 "duvechk2.sc" */	/* host code */
	queryid1 = quyid->db_qry_high_time;
	queryid2 = quyid->db_qry_low_time;
/* # line 8944 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(txtid1)from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&queryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&queryid2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8949 "duvechk2.sc" */	/* host code */
        cnt--;
	/* Get the length of the last text string */
/* # line 8952 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select txt from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&queryid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&queryid2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and seq=");
    IIputdomio((short *)0,1,30,4,&cnt);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,239,text);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8957 "duvechk2.sc" */	/* host code */
        size = STlength(text) + (cnt * 240);
	/* Assign memory */ 
        txt_ptr	= (char *) MEreqmem( (u_i4)0, size, TRUE, &mem_text);
	if (mem_text != OK || txt_ptr == NULL)
        {
           duve_talk (DUVE_ALWAYS, duve_cb, E_DU3400_BAD_MEM_ALLOC, 0);
	   return ( (DU_STATUS) DUVE_BAD);
    	}
	*string = txt_ptr;
   	save_ptr = txt_ptr;
        for (i = 0, length = 0; i <= cnt; i++)
	{ 	
/* # line 8974 "duvechk2.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"duvechk242",6298,27317);
      IIputdomio((short *)0,1,30,4,&queryid1);
      IIputdomio((short *)0,1,30,4,&queryid2);
      IIputdomio((short *)0,1,30,4,&cnt);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIsqMods(1);
        IIexDefine(1,(char *)"duvechk242",6298,27317);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select txt from iiqrytext where txtid1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&queryid1);
        IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&queryid2);
        IIwritio(0,(short *)0,1,32,0,(char *)"and seq=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&cnt);
        IIexDefine(0,(char *)"duvechk242",6298,27317);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,239,text);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 8980 "duvechk2.sc" */	/* host code */
            /* cache info about the constraint text
	    */
	    str_len = STlength(text);
            MEcopy( (PTR)text, str_len, (PTR)txt_ptr);
            length = length + STlength(text);
	    txt_ptr += length;
	 }
	 /* Null terminate the text string */
	 *txt_ptr = '\0';
	 /* Restore the pointer to the beginning of the text string */
	 txt_ptr = save_ptr;
	 return DUVE_YES;
}
/*{
** Name: DEBUG UTILITY ROUTINES:
**	    get_stats()	 - get _bio_cnt, _cpu_ms, _dio_cnt via dbms info
**	    init_stats() - Initialize gathering of performance statistics
**	    printstats() - Complete performanc statistics gather and report them
**
** Description:
**      These utilities are meant to allow verifydb (in debug mode only) to
**	gather and report statistics for elapsed CPU time, DIO and BIO count.
**	The sequence is as follows:
**	    call init_stats() to initialize and gather "before" statistics"
**	    call printstats() to gather "after" statistics, calculate actual
**		statistics as AFTER - BEFORE.  printstats() also writes the
**		statistics to the verifydb log file.
**	NOTE:
**	    routine get_stats() is a utility to support init_stats() and 
**	    printstats(), and should never be called except by those two 
**	    routines.
**
** Inputs:
**	duve_cb		    ptr to verifydb control block.
**	id		    ASCII identifier string (null terminated)
** Outputs:
**	duve_cb		    ptr to verifydb control block.
**
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    An entry is written to the verifydb log file.
**
** History:
**      10-dec-1993 (teresa)
**          initial creation
[@history_template@]...
*/
VOID
get_stats(i4 *bio, i4 *dio, i4 *ms)
{
  char bio_str[DB_MAXNAME+1], dio_str[DB_MAXNAME+1], ms_str[DB_MAXNAME+1];
/* # line 9048 "duvechk2.sc" */	/* host code */
    /* get _bio_cnt, _cpu_ms, _dio_cnt via dbms info */
/* # line 9049 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('_bio_cnt'), dbmsinfo('_dio_cnt'), dbmsinfo('_cpu_ms'\
)");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,bio_str);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,dio_str);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,ms_str);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 9051 "duvechk2.sc" */	/* host code */
    if (sqlca.sqlcode < 0)
    {
	*bio = 0;
	*dio = 0;
	*ms = 0;
    }
    else
    {
	/* convert ascii to integer */
	CVal (bio_str, bio);
	CVal (dio_str, dio);
	CVal (ms_str, ms);
    }
}
VOID
init_stats(DUVE_CB *duve_cb, i4  test_level)
{
    if (duve_cb->duve_debug)
    {
	DUVE_PERF   *perf_info;
	if (test_level == DUVE_TEST_PERF)
	    perf_info = &duve_cb->duve_test_perf_numbers;
	else
	    perf_info = &duve_cb->duve_qry_perf_numbers;
	perf_info->bio_after = 0;
	perf_info->dio_after = 0;
	perf_info->ms_after = 0;
	get_stats(&perf_info->bio_before, &perf_info->dio_before,
			&perf_info->ms_before);
    }
}
VOID
printstats(DUVE_CB *duve_cb, i4  test_level, char *id)
{
    i4 bio, dio, ms;
    char msg_buffer[500];
    if (duve_cb->duve_debug)
    {
	DUVE_PERF   *perf_info;
	if (test_level == DUVE_TEST_PERF)
	    perf_info = &duve_cb->duve_test_perf_numbers;
	else
	    perf_info = &duve_cb->duve_qry_perf_numbers;
	/* get "after" stats */
	get_stats(&perf_info->bio_after, &perf_info->dio_after,
	    &perf_info->ms_after);
	/* calculate delta of after minus before */
	bio = perf_info->bio_after - perf_info->bio_before;
	dio = perf_info->dio_after - perf_info->dio_before;
	ms = perf_info->ms_after - perf_info->ms_before;
	/* handle any esqlc failure */
	if ( (perf_info->bio_after == 0) || (perf_info->bio_before < 0) )
	    bio = 0;
	if ( (perf_info->dio_after == 0) || (perf_info->dio_before < 0) )
	    dio = 0;
	if ( (perf_info->ms_after == 0) || (perf_info->ms_before < 0) )
	    ms = 0;
	/* output a message to the verifydb log file */
	if (id)
	    STprintf( msg_buffer, "debug: %s -- CPU_MS: %d, BIO: %d, DIO: %d\n",
		      id, ms, bio, dio);
	else 
	    STprintf( msg_buffer,
		      "debug: %XXXX -- CPU_MS: %d, BIO: %d, DIO: %d\n",
		      ms, bio, dio);
	duve_log (duve_cb, 0, msg_buffer);
    } /* endif debug */
}
/*{
** Name: cksecalarm - run verifydb tests to check system catalog iisecalarm.
**
** Description:
**      This routine opens a cursor to walk iisecalarm one tuple at a time.
**      It performs checks and associated repairs (mode permitting) on each 
**      tuple to assure the consistency of the iisecalarm system catalog.
**
** Inputs:
**      duve_cb                         verifydb control block
**	    duverel                          cached iirelation information
**
** Outputs:
**      duve_cb                         verifydb control block
**	    duverel                         cached iirelation information
**	        du_tbldrop			flag indicating to drop table
**	Returns:
**	    DUVE_YES
**	    DUVE_BAD
**	Exceptions:
**	    none
**
** Side Effects:
**	    tuples may be dropped from table iisecalarm
**	    tuples may be modified in iirelation
**
** History:
**      20-dec-93  (robf)
**          initial creation
[@history_template@]...
*/
DU_STATUS
cksecalarm(duve_cb)
DUVE_CB            *duve_cb;
{
    DB_TAB_ID		alm_id;
    DU_STATUS		base;
# include <duvealm.h>
  i4 cnt;
  u_i4 tid, tidx;
  u_i4 basetid, basetidx;
  i4 relstat;
  i4 id1, id2;
  i2 mode;
	bool	found;
/* # line 9186 "duvechk2.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"alm_cursor",6299,12701);
    IIwritio(0,(short *)0,1,32,0,(char *)"select * from iisecalarm");
    IIcsQuery((char *)"alm_cursor",6299,12701);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 9188 "duvechk2.sc" */	/* host code */
    /* loop for each tuple in iisecalarm */
    for (;;)  
    {
	/* get tuple from iisecalarm */
/* # line 9192 "duvechk2.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"alm_cursor",6299,12701,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,32,(alarm_tbl).alarm_name);
      IIcsGetio((short *)0,1,30,4,&(alarm_tbl).alarm_num);
      IIcsGetio((short *)0,1,30,4,&(alarm_tbl).obj_id1);
      IIcsGetio((short *)0,1,30,4,&(alarm_tbl).obj_id2);
      IIcsGetio((short *)0,1,32,32,(alarm_tbl).obj_name);
      IIcsGetio((short *)0,1,32,1,(alarm_tbl).obj_type);
      IIcsGetio((short *)0,1,30,2,&(alarm_tbl).subj_type);
      IIcsGetio((short *)0,1,32,32,(alarm_tbl).subj_name);
      IIcsGetio((short *)0,1,30,2,&(alarm_tbl).alarm_flags);
      IIcsGetio((short *)0,1,30,4,&(alarm_tbl).alarm_txtid1);
      IIcsGetio((short *)0,1,30,4,&(alarm_tbl).alarm_txtid2);
      IIcsGetio((short *)0,1,30,4,&(alarm_tbl).alarm_opctl);
      IIcsGetio((short *)0,1,30,4,&(alarm_tbl).alarm_opset);
      IIcsGetio((short *)0,1,30,4,&(alarm_tbl).event_id1);
      IIcsGetio((short *)0,1,30,4,&(alarm_tbl).event_id2);
      IIcsGetio((short *)0,1,32,256,(alarm_tbl).event_text);
      IIcsGetio((short *)0,1,30,4,&(alarm_tbl).alarm_id1);
      IIcsGetio((short *)0,1,30,4,&(alarm_tbl).alarm_id2);
      IIcsGetio((short *)0,1,32,32,(alarm_tbl).alarm_reserve);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 9193 "duvechk2.sc" */	/* host code */
	if (sqlca.sqlcode == 100) /* then no more tuples in iisecalarm */
	{
/* # line 9195 "duvechk2.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"alm_cursor",6299,12701);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 9196 "duvechk2.sc" */	/* host code */
	    break;
	}
	alm_id.db_tab_base = alarm_tbl.obj_id1;
	alm_id.db_tab_index = alarm_tbl.obj_id2;
	tid = alarm_tbl.obj_id1;
	tidx = alarm_tbl.obj_id2;
	/* test 1 -- verify table receiving alarm exists */
	base = duve_findreltid ( &alm_id, duve_cb);
	if (base == DU_INVALID)
	{
	    /*
	    ** Not found, so check if its a database or installation
	    ** alarm.
	    */
	    found=FALSE;
	    if(alarm_tbl.alarm_flags&DBA_ALL_DBS)
		found=TRUE;
	    else if(*alarm_tbl.obj_type==DBOB_DATABASE)
	    {
/* # line 9217 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(name)from iidatabase where name=");
    IIputdomio((short *)0,1,32,0,alarm_tbl.obj_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 9222 "duvechk2.sc" */	/* host code */
		if(cnt)
			found=TRUE;
	    }
	    if (!found)
	    {
		/* this tuple is not in iirelation/iidatabase
		** so drop from iisecalarm */
		if (duve_banner( DUVE_IISECALARM, 1, duve_cb) == DUVE_BAD) 
		    return ( (DU_STATUS) DUVE_BAD);
		duve_talk ( DUVE_MODESENS, duve_cb,
			    S_DU16D1_NO_BASE_FOR_ALARM, 6,
			    sizeof(tid), &tid, sizeof(tidx), &tidx,
			    0, alarm_tbl.alarm_name);
	        if ( duve_talk( DUVE_ASK, duve_cb, S_DU16D2_DROP_FROM_IISECALARM, 2,
			0, alarm_tbl.alarm_name)
			== DUVE_YES)
		{
		    mode = DB_ALM;
/* # line 9242 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&alarm_tbl.alarm_txtid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&alarm_tbl.alarm_txtid2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and mode=");
    IIputdomio((short *)0,1,30,2,&mode);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 9247 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iisecalarm",(char *)"alm_cursor",6299,12701);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 9248 "duvechk2.sc" */	/* host code */
		    duve_talk(DUVE_MODESENS,duve_cb,
			      S_DU16D3_IISECALARM_DROP, 2,
			      0, alarm_tbl.alarm_name);
		}
	        continue;
	    }
	} /* endif base table not found */
	else 
	{
	    if ( base == DUVE_DROP)
		continue;	/* if table is marked for drop, stop now */
	}
	/* 
	**  test 2 - verify base table indicates alarms are defined on it
	*/
	if (*alarm_tbl.obj_type==DBOB_TABLE &&
	   (duve_cb->duverel->rel[base].du_stat2 & TCB_ALARM) == 0 )
	{
	    /* the relstat in iirelation needs to be modified */
            if (duve_banner( DUVE_IISECALARM, 2, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU16D4_NO_ALARM_RELSTAT, 4,
			0, duve_cb->duverel->rel[base].du_tab,
			0, duve_cb->duverel->rel[base].du_own);
	    if ( duve_talk( DUVE_ASK, duve_cb, S_DU16D5_SET_ALMTUPS, 0)
		    == DUVE_YES)
	    {
		basetid =  duve_cb->duverel->rel[base].du_id.db_tab_base;
		basetidx =  duve_cb->duverel->rel[base].du_id.db_tab_index;
		relstat = duve_cb->duverel->rel[base].du_stat2 | TCB_ALARM;
		duve_cb->duverel->rel[base].du_stat2 = relstat;
/* # line 9280 "duvechk2.sc" */	/* update */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"update iirelation set relsta\
t2=");
    IIputdomio((short *)0,1,30,4,&relstat);
    IIwritio(0,(short *)0,1,32,0,(char *)"where reltid=");
    IIputdomio((short *)0,1,30,4,&basetid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and reltidx=");
    IIputdomio((short *)0,1,30,4,&basetidx);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 9282 "duvechk2.sc" */	/* host code */
		if (sqlca.sqlcode == 0)
		    duve_talk(DUVE_MODESENS, duve_cb, S_DU16D6_ALMTUPS_SET, 0);
	    }  /* endif fix relstat */
	} /* endif relstat doesnt indicate alarms */
	/*********************************************************/
	/* test 3 verify query text used to define alarm exists */
	/*********************************************************/
	mode = DB_ALM;
	id1 = alarm_tbl.alarm_txtid1;
	id2 = alarm_tbl.alarm_txtid2;
/* # line 9294 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(txtid1)from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&id2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and mode=");
    IIputdomio((short *)0,1,30,2,&mode);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 9296 "duvechk2.sc" */	/* host code */
	if (cnt == 0) 
	{
	    /* mising query text that defined permit */
            if (duve_banner( DUVE_IISECALARM, 3, duve_cb) == DUVE_BAD) 
		return ( (DU_STATUS) DUVE_BAD);
	    duve_talk ( DUVE_MODESENS, duve_cb, S_DU16D7_NO_ALARM_QRYTEXT, 6,
			sizeof(id1), &id1, sizeof(id2), &id2,
			0, alarm_tbl.alarm_name);
	}
	/*********************************************************/
	/* test 4 if alarm fires dbevent make sure the event exists
	/*********************************************************/
	if(alarm_tbl.alarm_flags&DBA_DBEVENT)
	{
/* # line 9311 "duvechk2.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iievent where event_idbase=");
    IIputdomio((short *)0,1,30,4,&alarm_tbl.event_id1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and event_idx=");
    IIputdomio((short *)0,1,30,4,&alarm_tbl.event_id2);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 9316 "duvechk2.sc" */	/* host code */
	    if(!cnt)
	    {
	        /* the event is unknown */
                if (duve_banner( DUVE_IISECALARM, 4, duve_cb) == DUVE_BAD) 
			return ( (DU_STATUS) DUVE_BAD);
	         duve_talk ( DUVE_MODESENS, duve_cb, S_DU16D8_NO_ALARM_EVENT, 6,
			sizeof(alarm_tbl.event_id1), &alarm_tbl.event_id1,
			sizeof(alarm_tbl.event_id2), &alarm_tbl.event_id2,
			0, alarm_tbl.alarm_name);
	        if ( duve_talk( DUVE_ASK, duve_cb, S_DU16D2_DROP_FROM_IISECALARM, 2,
			0, alarm_tbl.alarm_name)
		    == DUVE_YES)
	        {
		    mode = DB_ALM;
/* # line 9332 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext where txtid1=");
    IIputdomio((short *)0,1,30,4,&alarm_tbl.alarm_txtid1);
    IIwritio(0,(short *)0,1,32,0,(char *)"and txtid2=");
    IIputdomio((short *)0,1,30,4,&alarm_tbl.alarm_txtid2);
    IIwritio(0,(short *)0,1,32,0,(char *)"and mode=");
    IIputdomio((short *)0,1,30,2,&mode);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 9336 "duvechk2.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIcsDelete((char *)"iisecalarm",(char *)"alm_cursor",6299,12701);
    if (sqlca.sqlcode < 0) 
      Clean_Up();
  }
/* # line 9337 "duvechk2.sc" */	/* host code */
		    if (sqlca.sqlcode == 0)
		        duve_talk(DUVE_MODESENS, duve_cb, 
				S_DU16D3_IISECALARM_DROP, 2,
				0, alarm_tbl.alarm_name
				);
	        }  
	    }
	}
    }  /* end loop for each tuple in iisecalarm */
    return ( (DU_STATUS) DUVE_YES);
}
