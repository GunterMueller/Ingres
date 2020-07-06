# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**Copyright (c) 2004 Ingres Corporation
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
#define		        DU_SQL_NONE     100	/* sqlca.sqlcode value for
						** 'no tuples found'
						*/
#include    <compat.h>
#include    <gl.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <qeaiproc.h> 
#include    <tr.h>
#include    <pc.h>
#include    <me.h>
#include    <si.h>
#include    <ex.h>
#include    <st.h>
#include    <ut.h>
#include    <cv.h>
#include    <lo.h>
#include    <er.h>
#include    <duf.h>
#include    <cs.h>
#include    <lk.h>
#include    <di.h>
#include    <nm.h>
#include    <dmf.h>
#include    <duerr.h>
#include    <duenv.h>
# include <dudbms.h>
#include    <lg.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dm0c.h>
#include    <duvfiles.h>
# include <upgradedb.h>
# include <dustar.h>
#include    <cm.h>
/**
** Name:	doview.qsc
**
** Description:
**	This file defines:
**
**		populate_secalarm - rebuild security alarms for 6.5 ES
**
** History:
**	16-dec-92 (billc) written.
**	11-mar-93 (jpk) modified for upgradedb
**	18-sep-1993 (jpk) wean from FElibs dependence.
**	20-nov-1993 (jpk) make TXTBUF member number agree with
**	initialization parameter number.  Add rebuilding
**	of procedures, rules, and integrities as well as
**	views and permits.
**	13-oct-93 (swm)
**	    Bug #56448
**	    Removed d_print() function because it accepts a variable number
**	    of variable-sized arguments, which is not portable. It could
**	    not become a varargs function as this practice is outlawed in
**	    main line code.
**	    Replaced invocations of the function with calls to SIprintf. This
**	    works because SIprintf is a varargs function.
**	29-Jan-1994 (jpk)
**	    Bug #58408.
**	    Made those SIprintfs depend on upg_dbg_out flag.
**	28-March-1994 (jpk)
**	    Bug 58586: upgrading iidbdb required fixing view and permits
**	    rebuilding for databases where dba is $ingres, the iidbdb
**	    being one such.
**	28-March-1994 (jpk)
**	    Incorporate rblumer fixes for bugs:
**	    	60810: drop DBEVENT not EVENT
**		60812: work around server limit of 255 DDL/cursor
**		80813: parse quel query text more robustly
**		60814: exclude 6.5 permits, integrities from upgrade
**		60816: be $ingres before dropping iitmp_ tables
**      03-feb-1995 (chech02)
**          Bug# 66464, 66467, 64589, 64590
**          Added commit  statements at the beginning of each function.	
**      27-mar-1995 (chech02) 67532
**          Cant access views in stardb. Added star_populate_views() to
**          populate user views in stardb.
**      29-mar-1995 (chech02) 67789    
**          In Release 6.4 create permit insert was allowed on aggregate views 
**          that is not permitted in 1.1. Modified exec_txt() to test for sql  
**          return code of -34000 when attempting to create a permit.
**      24-may-1995 (lawst01) 68771
**          When insert permit is specified on a table for a user
**          'copy_into' permission should also be granted.
**	13-sep-1996 (canor01)
**	    Remove STATIC on prototype for chkinqerr().  This makes it
**	    static for Solaris (even without a static on the function
**	    definition).
**	02-oct-96 (mcgem01)
**	    Add include for duvfiles.h.
**      21-apr-1999 (hanch04)
**          replace STindex with STchr
**      28-apr-1999 (hanch04)
**          Replaced STlcopy with STncpy
**	21-may-1999 (hanch04)
**	    Replace STbcompare with STcasecmp,STncasecmp,STcmp,STncmp
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      09-jan-2002 (stial01)
**          Deleted code, functionality replaced by calling unloaddb (SIR 106745
[@history_template@]...
*/
/* GLOBALDEFs */
/* externs */
/*
** Definition of all global variables owned by this file.
*/
GLOBALREF DU_UPGRADE_CB	upgr_cb ;	/* upgradedb control block */
/* statics */
STATIC STATUS	exec_txt(DB_LANG dml, char *buf);
STATUS		setauth(char *owner);
static STATUS 	populate_alarms_for_user( char *alarm_owner);
GLOBALREF	bool		upg_dbg_out;
/*
** Test driver
**      MKMFIN Hints
**
PROGRAM =       doview
NEEDLIBS =      SQLCALIB GNLIB UFLIB RUNSYSLIB RUNTIMELIB FDLIB FTLIB FEDSLIB \
                UILIB LIBQSYSLIB LIBQLIB UGLIB FMTLIB AFELIB \
                LIBQGCALIB CUFLIB GCFLIB ADFLIB \
                COMPATLIB MALLOCLIB
UNDEFS =        II_copyright
*/
static char     Last_owner[DB_MAXNAME + 1] = ERx("");
/*{
** Name:	setauth: set session authorization auth_id.
**
** Inputs:
**	owner	auth id to set session authorization to.
**		Pass NULL to reset session authorization to "unknown".
**
** Outputs:
**	none.
**
**	Returns:
**		OK on success, FAIL (or other status) on failure.
**
** History:
**	June 1993 (jpk) written
**	November 1993 (jpk) make sure to commit before setting
**	authorization.  Also after, why not.
**	19-jul-94 (robf)
**         Do regular COMMIT instead of creating a string, then execute
**         immediate.
**	8-Sep-2004 (schka24)
**	    Init "last owner" by passing NULL, e.g. on (re)connect.
*/
STATUS
setauth(char *owner)
{
    if (owner != NULL && *owner != EOS && !STequal(Last_owner, owner))
    {
	char nbuf[((2 * DB_MAXNAME) + 2 + 1)];
	char obuf[512];
	STncpy( Last_owner, owner, sizeof(Last_owner) - 1);
	Last_owner[ sizeof(Last_owner) - 1 ] = EOS;
	/* Do we need to denormalize the name? */
	/*
	** XXX I hope not.  This requires FElibs, and seems silly.
	** 6.4 owners shouldn't have funky names.  But for the
	** idempotency (we aren't currently trying to exclude 6.5
	** views, so they may have 6.5 owners), is this really
	** necessary?
	*/
/*	if (IIUGdlm_ChkdlmBEobject(owner, NULL, TRUE))*/
/*	{*/
/*	    IIUGrqd_Requote_dlm(owner, nbuf);*/
/*	    owner = nbuf;*/
/*	}*/
/* # line 182 "doview.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 183 "doview.sc" */	/* host code */
	STprintf(obuf, "SET SESSION AUTHORIZATION %s", owner);
	if (exec_txt(DB_SQL, obuf) != OK)
	    return (FAIL);
/* # line 186 "doview.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 187 "doview.sc" */	/* host code */
    }
    if (owner == NULL)
	Last_owner[0] = EOS;
    return (OK);
}
STATIC STATUS
exec_txt(dml, txt_buf)
DB_LANG	dml;
  char *txt_buf;
{
    if (txt_buf == NULL || *txt_buf == EOS)
    {
	/* Nothing to exec. */
	return (OK);
    }
#ifdef T_DEBUG
    SIprintf("]==> %s\\p\\g\n%s%s\\p\\g\n\n\n",
	(dml == DB_SQL) ? "\\sql\nselect count(*) from iirelation"
		: "\\quel\nretrieve (c = count(iirelation.all))",
	txt_buf,
	(dml == DB_SQL) ? ";" : "");
    return (OK);
#else
    if (dml == DB_SQL)
    {
	if (upg_dbg_out)
	{
	    SIprintf("]==> executing SQL <%s>\n\n", txt_buf);
	}
/* # line 218 "doview.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(txt_buf);
    IIsyncup((char *)0,0);
  }
/* # line 219 "doview.sc" */	/* host code */
	if (upg_dbg_out)
		check_sql(sqlca.sqlcode);
	if (sqlca.sqlcode == -50)
	{
	    return(OK);
	}
	if (sqlca.sqlcode == -34000)
	{
	  show_sql_error(sqlca.sqlcode,txt_buf);
	  return (OK);
        }
    }
    else
    {
	do_quel(txt_buf);
    }
    return (chkinqerr());
#endif
}
/*
**
** History:
**	25 March 1994 (jpk) fix bug 60813 (another rblumer fix)
**	When recreating objects with queries (views) parse
**	quel query text more robustly.  A line feed in the
**	query text does not always delimit a separate quel
**	statement.  Look for "r" following it to signify a
**	range statement.  If "r" not found, then the line feed does
**	NOT delimit a statement; don't execute it separately.
*/
do_quel(str)
  char *str;
{
    char	*ptr;
    while ((ptr = STchr(str, '\n')) != NULL)
    {
	*ptr=EOS;
	if (upg_dbg_out)
	{
	    SIprintf("]==> executing first quel <%s>\n\n", str);
	}
/* # line 282 "doview.qsc" */	/* iistatement */
  {
    IIwritio(1,(short *)0,1,32,0,str);
    IIsyncup((char *)0,0);
  }
/* # line 283 "doview.qsc" */	/* host code */
	str = ptr;
	CMnext(str);
	/* if next character is not an 'r', we've hit the end
	** of the list of range statements, so break out and execute
	** rest of statement.
	** Since statement itself could contain line-feeds (\n),
	** can't just continue until out of line feeds
	*/
	if (*str != 'r')
	    break;
    }
    if (upg_dbg_out)
    {
	SIprintf("]==> executing nth quel <%s>\n\n", str);
    }
/* # line 298 "doview.qsc" */	/* iistatement */
  {
    IIwritio(1,(short *)0,1,32,0,str);
    IIsyncup((char *)0,0);
  }
/* # line 299 "doview.qsc" */	/* host code */
}
/*
** Name: check_sql - check sql status code
**
** Description:
**	Check an sql return status and print diagnostic
**
** Inputs:
** 	code - and sqlca.sqlcode
**
** History:
**     xxx-xx-xx (xxx)
**         Created
**     19-jul-94 (robf)
**         Added history, always print an error if present to minimize
**         silent failures in conversion.
*/
check_sql(code)
int	code;
{
  char err_msg[500];
	if (upg_dbg_out && code!=0)
	{
	    SIprintf("]==> ]=> sqlcode %d\n\n", code);
	}
	if (code < 0 || code > 1024)
	{
/* # line 316 "doview.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,499,err_msg,63);
  }
/* # line 317 "doview.sc" */	/* host code */
	    if (upg_dbg_out)
	    {
		SIprintf("]==> error text: %s\n\n", err_msg);
	    }
	    else
	    {
		SIprintf("%s\n",err_msg);
	    }
	}
}
/*{
** Name:    chkinqerr -- clone of FEinqerr() -- FE bad for DUF stuff.
**
** Description:
**	Returns the DBMS status of the most recent query.
**
** Returns:
**	{nat}  The DBMS status of the most recent query.
**
*/
i4
chkinqerr()
{
  i4 errnum;
/* # line 343 "doview.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 344 "doview.sc" */	/* host code */
    return errnum;
}  
/*
** Name: populate_seclarm
**
** Description:
**     Fill the iisecalarm table when converting from 65 to 65ES.
**     roles.
**
**     In 6.5 security alarms were stored in iiprotect as fake permits,
**     whereas in 6.5 ES they became distinct items, stored in iisecalarm
**     This routine extracts alarm permits from iiprotect and formats them
**     appropriately into iisecalarm. Any tables with alarms are also updated
**     to have their relstat2 bit marked (TCB_ALARM) so the 6.5 ES DBMS
**     knows they have alarms.
**
** Inputs:
**      none
** 
** Outputs:
**      none
**
** History:
**    13-jul-94 (robf)
**         Created
*/
STATUS
populate_secalarm(char *dbname_curr)
{
  char alarm_owner[DB_MAXNAME+1];
  i4 owner_count = 0;
  char **owner_list = NULL;
  char **list_ptr = NULL;
    int		i;
    STATUS	stat = OK;
/* # line 381 "doview.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 382 "doview.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(distinct(probjowner))from iiprotect p where mod((p.proop\
set/128), (2))=1");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&owner_count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 386 "doview.sc" */	/* host code */
    if (upg_dbg_out)
    {
	SIprintf("]==> There are %d alarm owners in the database\n\n",
	    owner_count );
    }
    if (owner_count == 0)
    {
	/* Nothing to do */
	return OK;
    }
    list_ptr = owner_list = (char **)
	MEreqmem(0, owner_count * sizeof(char *), FALSE, &stat);
    if (stat != OK)
	goto badness;
/* # line 400 "doview.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct trim(probjowner)from iiprotect p where mod((p.proopse\
t/128), (2))=1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,alarm_owner);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 405 "doview.sc" */	/* host code */
	*list_ptr = (char *)
	    MEreqmem(0, STlength(alarm_owner) + 1, FALSE, &stat);
	if (stat != OK)
	    goto badness;
	STcopy(alarm_owner, *list_ptr++);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
    /* debugging, verify list is OK */
    if (upg_dbg_out)
    {
	for (i = 0; i < owner_count; i++)
	{
	    SIprintf("]==> ==> %s\n\n", owner_list[i]);
	}
    }
    for (i = 0; i < owner_count; i++)
    {
	if (setauth(owner_list[i]) != OK)	/* set session owner */
	    return (FAIL);
						/* rebuild alarms */
	if (populate_alarms_for_user(owner_list[i]) != OK)	
	    return (FAIL);
    }
    MEfree(owner_list);
    /*setauth("$dba"); */
    setauth("$ingres");
badness:
/* # line 431 "doview.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 432 "doview.sc" */	/* host code */
    return (stat);
}
/*
** Name: populate_alarms_for_user
**
** Description:
**	Recreate the security  alarms for a single user
**
** Inputs:
**	Current user name
**
** Outputs:
**	none
** 
** History:
**	14-jul-94 (robf)
**	    Created
*/
static STATUS
populate_alarms_for_user( char *alarm_owner)
{
  char *sa_owner;
  char saqtxt[256];
	bool err=FALSE;
     sa_owner=alarm_owner;
    if (upg_dbg_out)
    {
	    SIprintf("]==> Converting alarms owned by '%s'\n", sa_owner);
    }
    /*
    ** This assumes that in 6.5 alarms are short and always fit into
    ** a single text segment
    */
/* # line 477 "doview.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"sacur",6244,11646);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select trim(q.txt)from iiqrytext q, iiprotect p where mod((p.proopset\
/128), (2))=1 and q.mode=19 and p.proqryid1=q.txtid1 and p.proqryid2=q\
.txtid2 and p.probjowner=");
    IIputdomio((short *)0,1,32,0,sa_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by 1");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"sacur",6244,11646);
  }
/* # line 478 "doview.sc" */	/* host code */
    check_sql(sqlca.sqlcode);
    for(;;)
    {
/* # line 481 "doview.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"sacur",6244,11646,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,255,saqtxt);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 482 "doview.sc" */	/* host code */
	check_sql(sqlca.sqlcode);
	if (sqlca.sqlcode == DU_SQL_NONE)
	{       
	    break;
	}
	if (sqlca.sqlcode < 0)
	{
		err=TRUE;
		break;
	}
	if(exec_txt(DB_SQL, saqtxt)!=OK)
	{
    	    if (upg_dbg_out)
    	    {
	    	SIprintf("]==> Error converting alarm\n");
		err=TRUE;
		break;
	    }
	}
    }
/* # line 502 "doview.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"sacur",6244,11646);
  }
/* # line 503 "doview.sc" */	/* host code */
    /*
    ** Now cleanup old values if all went well so far
    */
    if (err==FALSE)
    {
        if (upg_dbg_out)
	    	SIprintf("]==> Cleaning up iiqrytext\n");
/* # line 511 "doview.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiqrytext q where exists(select * from iiprotect p where \
mod((p.proopset/128), (2))=1 and p.proqryid1=q.txtid1 and p.proqryid2=\
q.txtid2 and p.probjowner=");
    IIputdomio((short *)0,1,32,0,sa_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)")and q.mode=19");
    IIsyncup((char *)0,0);
  }
/* # line 520 "doview.sc" */	/* host code */
	check_sql(sqlca.sqlcode);
	if(sqlca.sqlcode<0)
		err=TRUE;
    }
    if (err==FALSE)
    {
        if (upg_dbg_out)
	    	SIprintf("]==> Cleaning up iiprotect\n");
/* # line 528 "doview.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from iiprotect p where mod((p.proopset/128), (2))=1 and p.prob\
jowner=");
    IIputdomio((short *)0,1,32,0,sa_owner);
    IIsyncup((char *)0,0);
  }
/* # line 531 "doview.sc" */	/* host code */
	check_sql(sqlca.sqlcode);
	if(sqlca.sqlcode<0)
		err=TRUE;
    }
    if(err)
    {
/* # line 537 "doview.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 538 "doview.sc" */	/* host code */
	    return FAIL;
    }
    else
    {
/* # line 542 "doview.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 543 "doview.sc" */	/* host code */
	    return OK;
    }
}
/*{
** Name: populate_iischema() - add implicit schema defs to 6.4 databases
**
** Description:
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** History:
**	November 1993 (jpk)
**	     written
**	15-jul-94 (robf)
**           moved from upgradedb.qsc and essentially rewritten due to
**	     mainline bug 64590. Problem is a schema authorization must
**           be created by a user with the same name, so we need to
**           follow the view/grant/alarm methodolgy and build a list
**           of schemas and change user authorization as necessary to
**           create the schemas. 
**	3-Sep-2004 (schka24)
**	    Fix (silent) error converting iidbdb since the only schema
**	    it's likely to have is $ingres;  which is placed into iischema
**	    when we create tables.
*/
VOID
populate_iischema()
{
  char owner[DB_MAXNAME+1];
  i4 owner_count = 0;
  char **owner_list = NULL;
  char **list_ptr = NULL;
  char stmt[140];
    int		i;
    STATUS	stat = OK;
/* # line 584 "doview.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 585 "doview.sc" */	/* host code */
    du_talk(DU_MODESENS, &upgr_cb, I_DU00EF_UPG_SCHEMA, 0);
/* # line 586 "doview.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop xxsch_owner");
    IIsyncup((char *)0,0);
  }
/* # line 587 "doview.sc" */	/* host code */
/*    EXEC SQL DECLARE GLOBAL TEMPORARY TABLE SESSION.sch_owner (owner) */
/* # line 588 "doview.sc" */	/* create table */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table xxsch_owner(owner)as select distinct relowner from iirel\
ation union select distinct synonym_owner from iisynonym union select \
dbp_owner from iiprocedure union select distinct event_owner from iiev\
ent");
    IIsyncup((char *)0,0);
  }
/* # line 602 "doview.sc" */	/* host code */
     check_sql(sqlca.sqlcode);
/* # line 604 "doview.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from xxsch_owner where owner in(select schema_name from iische\
ma)");
    IIsyncup((char *)0,0);
  }
/* # line 605 "doview.sc" */	/* host code */
     check_sql(sqlca.sqlcode);
/* # line 606 "doview.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select count(*)from xxsch_ow\
ner");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&owner_count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 609 "doview.sc" */	/* host code */
    check_sql(sqlca.sqlcode);
    if (owner_count == 0)
    {
	if (upg_dbg_out)
	    SIprintf("]==> No schemas to add\n");
/* # line 614 "doview.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop xxsch_owner");
    IIsyncup((char *)0,0);
  }
/* # line 615 "doview.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 616 "doview.sc" */	/* host code */
	return;
    }
    list_ptr = owner_list = (char **)
	MEreqmem(0, owner_count * sizeof(char *), FALSE, &stat);
    if (stat != OK)
	goto badness;
	/* tables and views */
/* # line 623 "doview.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"select owner from xxsch_owne\
r");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,owner);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 627 "doview.sc" */	/* host code */
	*list_ptr = (char *)
	    MEreqmem(0, STlength(owner) + 1, FALSE, &stat);
	if (stat != OK)
	    goto badness;
	STcopy(owner, *list_ptr++);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 633 "doview.sc" */	/* drop */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop xxsch_owner");
    IIsyncup((char *)0,0);
  }
/* # line 634 "doview.sc" */	/* host code */
    check_sql(sqlca.sqlcode);
/* # line 635 "doview.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 636 "doview.sc" */	/* host code */
    /* debugging, verify list is OK */
    if (upg_dbg_out)
    {
	for (i = 0; i < owner_count; i++)
	{
	    SIprintf("]==> ==> %s\n\n", owner_list[i]);
	}
    }
    for (i = 0; i < owner_count; i++)
    {
	if (setauth(owner_list[i]) != OK)	/* set session owner */
		break;
	STprintf(stmt, "create schema authorization %s", owner_list[i]);
	if(exec_txt(DB_SQL, stmt)!=OK)
	{
	    if (upg_dbg_out)
	    {
		SIprintf("]==> Error creating schema\n");
		break;
	    }
	}
/* # line 657 "doview.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 658 "doview.sc" */	/* host code */
    }
    MEfree(owner_list);
    /* setauth("$dba"); */
    setauth("$ingres");
    return;
badness:
/* # line 664 "doview.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 665 "doview.sc" */	/* host code */
    if (upg_dbg_out)
	    SIprintf("]==> Error in populate_iischema\n");
    /* setauth("$dba"); */
    setauth("$ingres");
    return ;
}
show_sql_error(code,qrytxt)
 int code;
 char *qrytxt;
{
  char err_msg[500];
char lbuf[25];
        SIprintf ("]==> -- %s --\n\n", qrytxt);
	SIprintf ("]==> ]=> sqlcode %d\n\n", code);
/* # line 681 "doview.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,499,err_msg,63);
  }
/* # line 682 "doview.sc" */	/* host code */
        SIprintf("]==> error text: %s\n\n", err_msg);
        du_log( upgr_cb.du_skiplog, &upgr_cb.du_log, qrytxt);
	STprintf(&lbuf, "\nsqlcode = %d\n", code);
	du_log( upgr_cb.du_skiplog, &upgr_cb.du_log, &lbuf);
        du_log( upgr_cb.du_skiplog, &upgr_cb.du_log, &err_msg);
}
