# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
/*
NO_DBLOPTIM = su4_us5 su4_u42 dr6_us5
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<qr.h>
# include	<id.h>
# include <ui.h>
# include	<ug.h>
# include	<st.h>
# include	<er.h>
# include	<cm.h>
# include <uigdata.h>
# include	"erqr.h"
# include	"qrhelp.h"
/**
** Name:	qrwldcrd.qsc -	Get lists of object names and owners
**
** Description:
**	This file contains routines that get a list of object names and
**	their owner, for different classifications of objects.  All entries
**	into the name lists are made through these routines.
**	The routines are:
**
**	IIQRono_ObjectNmOwner	Get list of all tables, views and indices
**	IIQRtno_TableNmOwner	Get list of tables
**	IIQRvno_ViewNmOwner	Get list of views
**	IIQRino_IndexNmOwner	Get list of indexes
**	IIQRlno_LinkNmOwner	Get list of registered objects
**	IIQRprn_ProcNmowner	Get list of procedures
**	IIQRrno_RuleNmOwner	Get list of rules
**	IIQRsno_SequenceNmOwner	Get list of sequences
**	IIQRsno_SynonymNmOwner	Get list of synonyms
**	IIQRcno_ColumnNmOwner	Get list of columns
**	IIQReno_EventNmOwner	Get list of Events
**	IIQRdno_DatabaseNmOwner Get list of Databases
**
**	Local routines:
**
**	IIQRtvi_TblViewIndex	Get list of one of tables, views or indexes
**	loop_init		Set up vars prior to start of get list routine.
**				This includes changing '*'s to '%'s for SQL.
**				The original name is also saved for use in an
**				error message if no objects are found to match
**				the string.
**	loop_proc		Process one list item (name & object)
**	loop_end		Process last list item (and starting in
**				6.5, eliminate table-like list items 
**				user doesn't have access to)
**	qrsetobjowner		
**	qrshosysobjs
**	qrreplc
**	qrxpandobj
**
**	This file has been written with the understanding that help on a 'type'
**	will return help on both native objects of that type, and on links to
**	that type.  That is, 'HELP TABLE name' (where name is a link to a table)
**	will not complain that 'name' is not a table, but will instead retrieve
**	the standard help information for a table.
**
**	No special care is taken to ensure that the user has not specified
**	a table name with both a '*' and one or more of '%' and '_', both
**	of which are SQL pattern matching characters with '_' also being a
**	legitimate character in INGRES names.
**
** History:
**	26-aug-88 (bruceb)	Written.
**	31-oct-88 (bruceb)
**		Look for registered objects instead of just links.  Still
**		pertains only to tables, views and indexes.
**	10-may-89 (kathryn) - Bug #5382
**		Changed select to repeated select.
**	18-may-89 (teresal)
**		Added help rule.
**	28-dec-89 (teresal)
**		Added help synonym, help comment column.
**	22-mar-91 (kathryn)
**		Added IIQReno_EventNmOwner for help permit on event.
**	18-feb-92 (sandyd/mikes)
**		Changed iiqrcatprefix definition to be static array instead of
**		a global pointer.  (It was causing W4GLUTIL shared image to
**		become unshareable due to address data.)
**	22-apr-92 (Kevinm)
**		Integrated change made by Data General for 6.4 port to 
**		DG/UX 5.4.  Make function declarations consistent with their
**		definititions.
**	14-jul-92 (rogerl)
**		6.5 maint.
**		Make all additions to name list be made via these routines -
**		this allows permits checking to be done by a join in the
**		'expanders'.  Queries are expanded to work on owner name
**		as well (for fips specified objects) - the permits stuff
**		still holds.  move initialization of iiqrcatprefix
**		into 'loop_init()', and intro of iiqrsystabown in order to
**		get info about system tables if we really want it,
**		also, took system_use check out of queries.
**		loop_init also is now doing the decomposition of
**		the object as well.  loop_end reinitializes statics.
**		Add GLOBALREF to synonym_yes, set layers above.  Change
**		ui.h include to ui.qsh.
**	9-oct-92 (rogerl)
**		Fix incorrect prototype, bug in qrreplc().
**	23-oct-92 (daver)
**		Modified loop_end for 6.5 to eliminate those list
**		members the current user doesn't have access to. 
**	12-nov-92 (rogerl)
**		Silence compiler warning by including FUNC_EXTERN for
**		IIQRfnp_FirstNodePtr()
**	25-nov-92 (daver)
**		Only eliminate list members which are tables, views, indexes
**	19-may-93 (rogerl)
**		Bugs (51830 and 51831) - missed matching identifiers of 
**		mixed case or 31/32 characters.  Restore ability to get help
**		on objects that have embedded quotes.  Delete qrstripendc().
**	21-jun-93 (rogerl)
**		Need to pass help type all the way down to wildcard expanders
**		to support security_alarm restrictions.  Make restriction
**		enforcement in loop end more general.
**	18-oct-93 (rogerl)
**		Use UI_FE_CAT_ID_65 instead of local literal.  It is properly
**		cased in loop_init().
**	10-dec-93 (robf)
**              Add IIQRdno_DatabaseNmOwner
**	08-mar-94 (geri)
**		Changed to use the normalized name of the object to
**		address bug 57466. This allows help commands to work
**		correctly when matching objects across gateways.
**	12-May-95 (rcs)
**		Added NO_DBLOPTIM for su4_us5 to
**		remove core dump on table help
**      13-Jul-95 (ramra01)
**              Tune the Zap node in loop_end to leave the non existant
**              objects for proper error msg display in help (b66258)
**	19-apr-1996 (canor01)
**		If we're zapping a node in loop_end, retain the
**		pointer to the previous node.
**	06-mar-1998 (kinpa04)
**		Added us4_u42 to NO_DBLOPTIM because the optimizer corrupts
**		the address of name, passed to qrreplc(), to be one byte.
**	19-Feb-1999 (ahaal01)
**		Added dr6_us5 to the NO_DBLOPTIM statement to remove core dump
**		on ICL platform when table help is used in an ISQL session.
**	26-Feb-1999 (ahaal01)
**		Added dr6_us5 to the NO_DBLOPTIM statement to remove core dump
**		on ICL platform when table help is used in an ISQL session.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	17-apr-02 (inkdo01)
**	    Added 'help sequence'.
**	23-may-2002 (abbjo03)
**	    In IIQRono_ObjectNmOwner, skip indexes and views for help
**	    constraint/integrity.
**	22-jul-2004 (wanfr01)
**	    Made qrsetobjowner non-static so it can be called from qrisrighttype
**	20-Dec-2004 (kodse01)
**	    BUG 113657
**	    Added "help permit on sequence".
**/
/* # define's */
/* GLOBALREF's */
GLOBALREF bool synonym_yes;
/* GLOBALDEF's */
  static char iiqrcatprefix[ sizeof( ERx("ii%") ) ];
  static char iiqrsystabown[ sizeof( UI_FE_CAT_ID_65 ) ];
			/* iiqrsystabown to be set to '$ingres' if
			** want to see system objects, else
			** to an empty string
			*/
/* extern's */
FUNC_EXTERN	VOID	qrprintf();
FUNC_EXTERN	VOID	qrtrimwhite( char *str );
FUNC_EXTERN	STATUS	qrtblowner();
FUNC_EXTERN	IIQRlist *	IIQRfnp_FirstNodePtr();
static		VOID	IIQRtvi_TblViewIndex();
static		VOID	loop_init();
static		STATUS	loop_proc();
static		VOID	loop_end();
static		VOID	qrinitncbpart( char *name, bool *isdlm );
static		VOID	qrinitncbnames( FE_RSLV_NAME *ncb );
static		VOID	qrinitncb( FE_RSLV_NAME *ncb, char **pattern );
static		VOID	qrdoperms( char **owner, char **object, char *owner_wld,
			    char *object_wld,  FE_RSLV_NAME *objectcb );
static		VOID	qrxpandobj( char **owner, char **object,
			    char *owner_wld, char *object_wld,
			    FE_RSLV_NAME *objectcb );
		VOID	qrsetobjowner( UIDBDATA *uidbdata, char **suser,
			    char **sdba, FE_RSLV_NAME *objectcb );
static		VOID	qrshosysobjs( char *pattern );
static		i4	qrreplc( char oc, char nc, char *objstr );
static		VOID	loop_end( QRB *qrb, i4  help_type );
/*
** startlist is set in loop_init to point to the end of the name list.
** endlist is set in loop_end to point to the current end of the name list.
** If the two addresses are the same, than no names were added to the list
** and the pattern specified by the user in the help command was
** non-productive.
*/
static		IIQRlist	*startlist;
static		IIQRlist	*endlist;
static		FE_RSLV_NAME 	objectcb =
				{ NULL, NULL, NULL,
				    (bool) FALSE, (bool) FALSE,
				    (bool) FALSE, (bool) FALSE,
				    (bool) FALSE
				};
static		char		ownerbuf[ FE_MAXNAME + 1 + 1 ];
static		char		objectbuf[ FE_MAXNAME + 1 + 1 ];
static		char		orig_name[ FE_UNRML_MAXNAME ];
static		bool		oname_sz_too_long = FALSE;
static		char		*oname_ptr;
static		char		nilstr = EOS;
static		char		*quotc = ERx( "\"" );
char				*synonym = NULL;
/*
** extended_name is 2 bytes longer than orig_name because it will also
** contain a trailing ' %' required when querying against the disgusting
** char data type used for table names.
*/
static		char	extended_name[ FE_MAXNAME + 1 + 1 ];
static	bool	usable_object = (bool) FALSE;
static	char	name_save[ FE_MAXNAME + 1 + 1 ];
static	char	own_save[ FE_MAXNAME + 1 + 1 ];
static	char	syn_save[ FE_MAXNAME + 1 + 1 ];
/*{
** Name:	IIQRono_ObjectNmOwner - get list of appropriate tables, views
**				      and indices
**
** Description:
**	This routine adds to the name list all the entries in the iitables
**	catalog (tables, views and indices) that are visible to the user
**	and whose names match the specified pattern.
**
** Inputs:
**	qrb	query runner control block
**	name1	specification of names to retrieve (contains wildcard)
**	dummy	Not used
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	Will add any names retrieved to the names list for later 'help'
**	processing.
**
** History:
**	25-aug-88 (bruceb)	Written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	14-jul-92 (rogerl)	Add iiqrsystabown
**	    Add wildcarding on owner as well as object.
**	08-mar-94 (geri)
**	    Changed to use the normalized name of the object to
**	    address bug 57466.
**	23-may-2002 (abbjo03)
**	    Skip indexes and views for help constraint/integrity.
*/
VOID
IIQRono_ObjectNmOwner(qrb, name1, dummy, help_type)
QRB	*qrb;
  char *name1;
char	*dummy;
i4	help_type;
{
  char cur1_tblname[FE_MAXNAME + 1];
  char cur1_ownname[FE_MAXNAME + 1];
  char dummy_mem[FE_MAXNAME + 1];
  char cur1_system[9];
  char tbl_type[2];
  char *ename1;
  char *nname1;
  char *suser;
  char *sdba;
  UIDBDATA *uidbdata;
  char *suser_wld;
  char *name1_wld;
    STATUS	stat = OK;
    loop_init(name1, &ename1);
    nname1 = objectcb.name_dest;
    uidbdata = IIUIdbdata();
	/* determine whether to search for tables owned by only
	** (uidbdata->suser and uidbdata->sdba) or the user specified
	** owner (which might be wildcarded, incidently)
	**
	** if objectcb->owner_dest != EOS, then use it as suser.
	** else, use uidbdata
	*/
    qrsetobjowner( uidbdata, &suser, &sdba, &objectcb );
	/* set vars which'll determine whether to expand wilds (use LIKES) */
    qrxpandobj( &suser_wld, &name1_wld, suser, name1, &objectcb );
    if (ename1 != NULL)
    {
	if ( synonym_yes )
	{
			/* pick up synonyms as well, dummy_men and redundant
			** table name in iitables query are so union can
			** be used
			*/
	/* bug #5382 - Changed to Repeated select */
/* # line 318 "qrwldcrd.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrwldcrd1",7481,1780);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,suser_wld);
      IIputdomio((short *)0,1,32,0,sdba);
      IIputdomio((short *)0,1,32,0,iiqrsystabown);
      IIputdomio((short *)0,1,32,0,iiqrcatprefix);
      IIputdomio((short *)0,1,32,0,name1_wld);
      IIputdomio((short *)0,1,32,0,ename1);
      IIputdomio((short *)0,1,32,0,ename1);
      IIputdomio((short *)0,1,32,0,nname1);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,suser_wld);
      IIputdomio((short *)0,1,32,0,sdba);
      IIputdomio((short *)0,1,32,0,iiqrsystabown);
      IIputdomio((short *)0,1,32,0,iiqrcatprefix);
      IIputdomio((short *)0,1,32,0,name1_wld);
      IIputdomio((short *)0,1,32,0,ename1);
      IIputdomio((short *)0,1,32,0,ename1);
      IIputdomio((short *)0,1,32,0,nname1);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrwldcrd1",7481,1780);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select synonym_name, synonym_owner, table_name, 'S' from iisynonyms w\
here(synonym_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or synonym_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,suser_wld);
        IIwritio(0,(short *)0,1,32,0,(char *)"or synonym_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or synonym_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,iiqrsystabown);
        IIwritio(0,(short *)0,1,32,0,(char *)")and synonym_name not li\
ke");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,iiqrcatprefix);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(synonym_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,name1_wld);
        IIwritio(0,(short *)0,1,32,0,(char *)"or synonym_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,ename1);
        IIwritio(0,(short *)0,1,32,0,(char *)"or synonym_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,ename1);
        IIwritio(0,(short *)0,1,32,0,(char *)"or synonym_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,nname1);
        IIwritio(0,(short *)0,1,32,0,(char *)
")union(select table_name, table_owner, table_name, table_type from ii\
tables where(table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $10=");
        IIputdomio((short *)0,1,32,0,suser_wld);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $11=");
        IIputdomio((short *)0,1,32,0,sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $12=");
        IIputdomio((short *)0,1,32,0,iiqrsystabown);
        IIwritio(0,(short *)0,1,32,0,(char *)")and table_name not like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $13=");
        IIputdomio((short *)0,1,32,0,iiqrcatprefix);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $14=");
        IIputdomio((short *)0,1,32,0,name1_wld);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $15=");
        IIputdomio((short *)0,1,32,0,ename1);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $16=");
        IIputdomio((short *)0,1,32,0,ename1);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $17=");
        IIputdomio((short *)0,1,32,0,nname1);
        IIwritio(0,(short *)0,1,32,0,(char *)"))order by table_name");
        IIexDefine(0,(char *)"qrwldcrd1",7481,1780);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur1_tblname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur1_ownname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,dummy_mem);
      IIgetdomio((short *)0,1,32,1,tbl_type);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 346 "qrwldcrd.sc" */	/* host code */
	    if (!((help_type == HELP_CONSTRAINT || help_type == HELP_INTEGRITY)
		&& (*tbl_type == 'V' || *tbl_type == 'I')))
	    {
		if ((stat = loop_proc(qrb, cur1_tblname, cur1_ownname,
			(bool)TRUE, dummy_mem)) != OK)
/* # line 351 "qrwldcrd.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
/* # line 352 "qrwldcrd.sc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	}
	else /* synonym_yes */
	{
	    /* bug #5382 - Changed to Repeated select */
/* # line 358 "qrwldcrd.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrwldcrd2",7482,13434);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,suser_wld);
      IIputdomio((short *)0,1,32,0,sdba);
      IIputdomio((short *)0,1,32,0,iiqrsystabown);
      IIputdomio((short *)0,1,32,0,iiqrcatprefix);
      IIputdomio((short *)0,1,32,0,name1_wld);
      IIputdomio((short *)0,1,32,0,ename1);
      IIputdomio((short *)0,1,32,0,ename1);
      IIputdomio((short *)0,1,32,0,nname1);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrwldcrd2",7482,13434);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, table_type from iitables where(table_\
owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,suser_wld);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,iiqrsystabown);
        IIwritio(0,(short *)0,1,32,0,(char *)")and table_name not like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,iiqrcatprefix);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,name1_wld);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,ename1);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,ename1);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,nname1);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by table_name");
        IIexDefine(0,(char *)"qrwldcrd2",7482,13434);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur1_tblname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur1_ownname);
      IIgetdomio((short *)0,1,32,1,tbl_type);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 372 "qrwldcrd.sc" */	/* host code */
		if (!((help_type == HELP_CONSTRAINT || help_type ==
		    HELP_INTEGRITY) && (*tbl_type == 'V' || *tbl_type == 'I')))
		{
		    if ((stat = loop_proc(qrb, cur1_tblname, cur1_ownname,
			    (bool)TRUE, cur1_tblname)) != OK)
/* # line 377 "qrwldcrd.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
      }
/* # line 378 "qrwldcrd.sc" */	/* host code */
		}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
	}
    }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(F_QR0044_ret_err_iitables));
    }
    else if (stat == OK)
    {
	/* TRUE means throw out those TABLE-like object we can't access */
	loop_end(qrb, help_type);
    }
}
/*{
** Name:	IIQRtno_TableNmOwner - get list of tables
**
** Description:
**	This routine adds to the name list all the tables in the iitables
**	catalog that are visible to the user and whose names match the
**	specified pattern.
**
** Inputs:
**	qrb	query runner control block
**	name	specification of names to retrieve (contains wildcard)
**	dummy	Not used
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	Will add any names retrieved to the names list for later 'help'
**	processing.
**
** History:
**	25-aug-88 (bruceb)	Written.
*/
VOID
IIQRtno_TableNmOwner(qrb, name, dummy, help_type)
QRB	*qrb;
char	*name;
char	*dummy; 
i4	help_type;
{
    IIQRtvi_TblViewIndex(qrb, name, 'T', help_type);
}
/*{
** Name:	IIQRvno_ViewNmOwner - get list of views
**
** Description:
**	This routine adds to the name list all the views in the iitables
**	catalog that are visible to the user and whose names match the
**	specified pattern.
**
** Inputs:
**	qrb	query runner control block
**	name	specification of names to retrieve (contains wildcard)
**	dummy	Not used
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	Will add any names retrieved to the names list for later 'help'
**	processing.
**
** History:
**	25-aug-88 (bruceb)	Written.
*/
VOID
IIQRvno_ViewNmOwner(qrb, name, dummy, help_type)
QRB	*qrb;
char	*name;
char	*dummy;
i4	help_type;
{
    IIQRtvi_TblViewIndex(qrb, name, 'V', help_type);
}
/*{
** Name:	IIQRino_IndexNmOwner - get list of indexes
**
** Description:
**	This routine adds to the name list all the indexes in the iitables
**	catalog that are visible to the user and whose names match the
**	specified pattern.
**
**	Using iitables if that's where indices are stored, else using
**	iiindexes.
**
** Inputs:
**	qrb	query runner control block
**	name5	specification of names to retrieve (contains wildcard)
**	dummy	Not used
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	Will add any names retrieved to the names list for later 'help'
**	processing.
**
** History:
**	25-aug-88 (bruceb)	Written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	14-jul-92 (rogerl)	Add iiqrsystabown
**	    Add wildcarding on owner as well as object.
**	08-mar-94 (geri)
**	    Changed to use the normalized name of the object
**	    to fix bug 57466.
*/
VOID
IIQRino_IndexNmOwner(qrb, name5, dummy, help_type)
QRB	*qrb;
  char *name5;
char	*dummy;
i4	help_type;
{
  char cur5_indxname[FE_MAXNAME + 1];
  char cur5_ownname[FE_MAXNAME + 1];
  char *ename5;
  char *nname5;
  UIDBDATA *uidbdata;
  char *suser;
  char *sdba;
    STATUS	stat = OK;
    uidbdata = IIUIdbdata();
    if (IIQRisl_IndexStorageLoc() == USE_IITABLES)
    {
	IIQRtvi_TblViewIndex(qrb, name5, 'I', help_type);
    }
    else
    {
	/*
	** Only use iiindexes name space if on a gateway
	*/
	loop_init(name5, &ename5);
    	nname5 = objectcb.name_dest;
	qrsetobjowner( uidbdata, &suser, &sdba, &objectcb );
	if (ename5 != NULL)
	{
	    /* bug #5382 - Changed to Repeated select */
/* # line 531 "qrwldcrd.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrwldcrd3",7483,13938);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,sdba);
      IIputdomio((short *)0,1,32,0,iiqrsystabown);
      IIputdomio((short *)0,1,32,0,iiqrcatprefix);
      IIputdomio((short *)0,1,32,0,nname5);
      IIputdomio((short *)0,1,32,0,ename5);
      IIputdomio((short *)0,1,32,0,ename5);
      IIputdomio((short *)0,1,32,0,nname5);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrwldcrd3",7483,13938);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select index_name, index_owner from iiindexes where(index_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or index_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or index_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or index_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,iiqrsystabown);
        IIwritio(0,(short *)0,1,32,0,(char *)")and index_name not like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,iiqrcatprefix);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(index_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,nname5);
        IIwritio(0,(short *)0,1,32,0,(char *)"or index_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,ename5);
        IIwritio(0,(short *)0,1,32,0,(char *)"or index_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,ename5);
        IIwritio(0,(short *)0,1,32,0,(char *)"or index_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,nname5);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by index_name");
        IIexDefine(0,(char *)"qrwldcrd3",7483,13938);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur5_indxname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur5_ownname);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 545 "qrwldcrd.sc" */	/* host code */
		if ((stat = loop_proc(qrb, cur5_indxname, cur5_ownname,
		    (bool)(TRUE), cur5_indxname)) != OK)
		{
/* # line 548 "qrwldcrd.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE3;
      }
/* # line 549 "qrwldcrd.sc" */	/* host code */
		}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
	}
	if (FEinqerr())
	{
	    qrprintf(qrb, ERget(F_QR0011_ret_err_iiindexes));
	}
	else if (stat == OK)
	{
	    /* TRUE means throw out those TABLE-like object we can't access */
	    loop_end(qrb, help_type);
	}
    }
}
/*{
** Name:	IIQRtvi_TblViewIndex - get list of tables or views or indexes
**
** Description:
**	This routine adds to the name list all the tables (or views or indexes)
**	in the iitables catalog that are visible to the user and whose names
**	match the specified pattern.
**
** Inputs:
**	qrb	query runner control block
**	name2	specification of names to retrieve (contains wildcard)
**	otype	object type (either 'T' or 'V' or 'I')
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	Will add any names retrieved to the names list for later 'help'
**	processing.
**
** History:
**	25-aug-88 (bruceb)	Written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	14-jul-92 (rogerl)	Add iiqrsystabown
**	    Add wildcarding on owner as well as object.
**	08-mar-94 (geri)
**	    Changed to use the normalized name of the object
**	    to fix bug 57466.
*/
static VOID
IIQRtvi_TblViewIndex(qrb, name2, otype, help_type)
QRB	*qrb;
  char *name2;
char	otype;
i4	help_type;
{
  char cur2_tblname[FE_MAXNAME + 1];
  char cur2_ownname[FE_MAXNAME + 1];
  char cur2_tbltype[9];
  char *ename2;
  char *nname2;
  UIDBDATA *uidbdata;
  char *suser;
  char *sdba;
    STATUS	stat = OK;
    loop_init(name2, &ename2);
    nname2 = objectcb.name_dest;
    uidbdata = IIUIdbdata();
    qrsetobjowner( uidbdata, &suser, &sdba, &objectcb );
    if (ename2 != NULL)
    {
	/* bug #5382 - Changed to Repeated select */
/* # line 623 "qrwldcrd.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrwldcrd4",7484,11120);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,sdba);
      IIputdomio((short *)0,1,32,0,iiqrsystabown);
      IIputdomio((short *)0,1,32,0,iiqrcatprefix);
      IIputdomio((short *)0,1,32,0,nname2);
      IIputdomio((short *)0,1,32,0,nname2);
      IIputdomio((short *)0,1,32,0,ename2);
      IIputdomio((short *)0,1,32,0,ename2);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrwldcrd4",7484,11120);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, table_type from iitables where(table_\
owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,iiqrsystabown);
        IIwritio(0,(short *)0,1,32,0,(char *)")and table_name not like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,iiqrcatprefix);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,nname2);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,nname2);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,ename2);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,ename2);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by table_name");
        IIexDefine(0,(char *)"qrwldcrd4",7484,11120);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur2_tblname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur2_ownname);
      IIgetdomio((short *)0,1,32,8,cur2_tbltype);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 637 "qrwldcrd.sc" */	/* host code */
	    if ((stat = loop_proc(qrb, cur2_tblname, cur2_ownname,
		(bool)(cur2_tbltype[0] == otype), cur2_tblname)) != OK)
	    {
/* # line 640 "qrwldcrd.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE4;
      }
/* # line 641 "qrwldcrd.sc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
    }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(F_QR0044_ret_err_iitables));
    }
    else if (stat == OK)
    {
        /* TRUE means throw out those TABLE-like object we can't access */
	loop_end(qrb, help_type);
    }
}
/*{
** Name:	IIQRlno_LinkNmOwner - get list of registered objects
**
** Description:
**	This routine adds to the name list all the registered objects in the
**	iiregistratioos catalog visible to the user and whose names match the
**	specified pattern.
**
** Inputs:
**	qrb	query runner control block
**	name3	specification of names to retrieve (contains wildcard)
**	dummy	Not used
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	Will add any names retrieved to the names list for later 'help'
**	processing.
**
** History:
**	25-aug-88 (bruceb)	Written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	14-jul-92 (rogerl)	Add iiqrsystabown
**	    Add wildcarding on owner as well as object.
**	14-jul-92 (rogerl)	Query iiregistrations instead of iitables,
**	    to get registered procedures as well.
**	08-mar-94 (geri)
**	    Changed to use the normalized name of the object to
**	    address bug 57466.
*/
VOID
IIQRlno_LinkNmOwner(qrb, name3, dummy, help_type)
QRB	*qrb;
  char *name3;
char	*dummy;
i4	help_type;
{
  char cur3_tblname[FE_MAXNAME + 1];
  char cur3_ownname[FE_MAXNAME + 1];
  char cur3_tblstype[9];
  char *ename3;
  char *nname3;
  UIDBDATA *uidbdata;
  char *suser;
  char *sdba;
    STATUS	stat = OK;
    loop_init(name3, &ename3);
    nname3 = objectcb.name_dest;
    uidbdata = IIUIdbdata();
    qrsetobjowner( uidbdata, &suser, &sdba, &objectcb );
    if (ename3 != NULL)
    {
	/* bug #5382 - Changed to Repeated select */
/* # line 716 "qrwldcrd.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrwldcrd5",7485,24945);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,sdba);
      IIputdomio((short *)0,1,32,0,iiqrsystabown);
      IIputdomio((short *)0,1,32,0,iiqrcatprefix);
      IIputdomio((short *)0,1,32,0,nname3);
      IIputdomio((short *)0,1,32,0,ename3);
      IIputdomio((short *)0,1,32,0,ename3);
      IIputdomio((short *)0,1,32,0,nname3);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrwldcrd5",7485,24945);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select object_name, object_owner, object_subtype from iiregistrations\
 where(object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or object_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,iiqrsystabown);
        IIwritio(0,(short *)0,1,32,0,(char *)")and object_name not lik\
e");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,iiqrcatprefix);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(object_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,nname3);
        IIwritio(0,(short *)0,1,32,0,(char *)"or object_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,ename3);
        IIwritio(0,(short *)0,1,32,0,(char *)"or object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,ename3);
        IIwritio(0,(short *)0,1,32,0,(char *)"or object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,nname3);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by object_name");
        IIexDefine(0,(char *)"qrwldcrd5",7485,24945);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur3_tblname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur3_ownname);
      IIgetdomio((short *)0,1,32,8,cur3_tblstype);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 730 "qrwldcrd.sc" */	/* host code */
	    if ((stat = loop_proc(qrb, cur3_tblname, cur3_ownname,
		(bool)((cur3_tblstype[0] == 'L')
		|| (cur3_tblstype[0] == 'I')),cur3_tblname))
		    != OK)
	    {
/* # line 735 "qrwldcrd.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE5;
      }
/* # line 736 "qrwldcrd.sc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
    }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(F_QR0044_ret_err_iitables));
    }
    else if (stat == OK)
    {
	/* FALSE means these aren't table names, don't check accessability */
	loop_end(qrb, help_type);
    }
}
/*{
** Name:	IIQRprn_ProcNmowner - get list of procedures
**
** Description:
**	This routine adds to the name list all the procedures in the
**	iiprocedures catalog that are visible to the user and whose names
**	match the specified pattern.
**
** Inputs:
**	qrb	query runner control block
**	name4	specification of names to retrieve (contains wildcard)
**	dummy	Not used
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	Will add any names retrieved to the names list for later 'help'
**	processing.
**
** History:
**	06-oct-88 (bruceb)	Written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	14-jul-92 (rogerl)	Add iiqrsystabown
**	    Add wildcarding on owner as well as object.
**	08-mar-94 (geri)
**	    Changed to use the normalized name of the object
**	    to fix bug 57466.
*/
VOID
IIQRprn_ProcNmowner(qrb, name4, dummy, help_type)
QRB	*qrb;
  char *name4;
char	*dummy;
i4	help_type;
{
  char cur4_procname[FE_MAXNAME + 1];
  char cur4_ownname[FE_MAXNAME + 1];
  char *ename4;
  char *nname4;
  char *suser;
  char *sdba;
  UIDBDATA *uidbdata;
    STATUS	stat = OK;
    loop_init(name4, &ename4);
    nname4 = objectcb.name_dest;
    uidbdata = IIUIdbdata();
    qrsetobjowner( uidbdata, &suser, &sdba, &objectcb );
    if (ename4 != NULL)
    {
	/* bug #5382 - Changed to Repeated select */
/* # line 808 "qrwldcrd.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrwldcrd6",7486,28406);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,sdba);
      IIputdomio((short *)0,1,32,0,iiqrsystabown);
      IIputdomio((short *)0,1,32,0,iiqrcatprefix);
      IIputdomio((short *)0,1,32,0,nname4);
      IIputdomio((short *)0,1,32,0,ename4);
      IIputdomio((short *)0,1,32,0,ename4);
      IIputdomio((short *)0,1,32,0,nname4);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrwldcrd6",7486,28406);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select procedure_name, procedure_owner from iiprocedures where(proced\
ure_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or procedure_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or procedure_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or procedure_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,iiqrsystabown);
        IIwritio(0,(short *)0,1,32,0,(char *)")and procedure_name not \
like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,iiqrcatprefix);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(procedure_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,nname4);
        IIwritio(0,(short *)0,1,32,0,(char *)"or procedure_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,ename4);
        IIwritio(0,(short *)0,1,32,0,(char *)"or procedure_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,ename4);
        IIwritio(0,(short *)0,1,32,0,(char *)"or procedure_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,nname4);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by procedure_name");
        IIexDefine(0,(char *)"qrwldcrd6",7486,28406);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur4_procname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur4_ownname);
      if (IIerrtest() != 0) goto IIrtB6;
/* # line 822 "qrwldcrd.sc" */	/* host code */
	    if ((stat = loop_proc(qrb, cur4_procname, cur4_ownname,
		(bool)(TRUE),cur4_procname)) != OK)
	    {
/* # line 825 "qrwldcrd.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE6;
      }
/* # line 826 "qrwldcrd.sc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
    }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR001C_err_ret_iiproc));
    }
    else if (stat == OK)
    {
	/* FALSE means these aren't table names, don't check accessability */
	loop_end(qrb, help_type);
    }
}
/*{
** Name:	IIQRrno_RuleNmOwner - get list of rules
**
** Description:
**	This routine adds to the name list all the rules in the
**	iirules catalog that are visible to the user and whose names
**	match the specified pattern.
**
** Inputs:
**	qrb	query runner control block
**	name6	specification of names to retrieve (contains wildcard)
**	dummy	Not used
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	Will add any names retrieved to the names list for later 'help'
**	processing.
**
** History:
**	18-may-89 (teresal)	Written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	14-jul-92 (rogerl)	Add qrcatprefix; iiqrsystabown
**	    Add wildcarding on owner as well as object.
**	08-mar-94 (geri)
**	    Changed to use the normalized name of the object to
**	    address bug 57466.
*/
VOID
IIQRrno_RuleNmOwner(qrb, name6, dummy, help_type)
QRB	*qrb;
  char *name6;
char 	*dummy;
i4	help_type;
{
  char cur6_rulename[FE_MAXNAME + 1];
  char cur6_ownname[FE_MAXNAME + 1];
  char *ename6;
  char *nname6;
  UIDBDATA *uidbdata;
  char *suser;
  char *sdba;
    STATUS	stat = OK;
    loop_init(name6, &ename6);
    nname6 = objectcb.name_dest;
    uidbdata = IIUIdbdata();
    qrsetobjowner( uidbdata, &suser, &sdba, &objectcb );
    if (ename6 != NULL)
    {
	/* bug #5382 - Changed to Repeated select */
/* # line 898 "qrwldcrd.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrwldcrd7",7487,31861);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,sdba);
      IIputdomio((short *)0,1,32,0,iiqrsystabown);
      IIputdomio((short *)0,1,32,0,nname6);
      IIputdomio((short *)0,1,32,0,ename6);
      IIputdomio((short *)0,1,32,0,ename6);
      IIputdomio((short *)0,1,32,0,nname6);
      IIputdomio((short *)0,1,32,0,iiqrcatprefix);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrwldcrd7",7487,31861);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select rule_name, rule_owner from iirules where(rule_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or rule_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or rule_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or rule_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,iiqrsystabown);
        IIwritio(0,(short *)0,1,32,0,(char *)")and(rule_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,nname6);
        IIwritio(0,(short *)0,1,32,0,(char *)"or rule_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,ename6);
        IIwritio(0,(short *)0,1,32,0,(char *)"or rule_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,ename6);
        IIwritio(0,(short *)0,1,32,0,(char *)"or rule_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,nname6);
        IIwritio(0,(short *)0,1,32,0,(char *)")and rule_name not like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,iiqrcatprefix);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by rule_name");
        IIexDefine(0,(char *)"qrwldcrd7",7487,31861);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE7;
IIrtB7:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur6_rulename);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur6_ownname);
      if (IIerrtest() != 0) goto IIrtB7;
/* # line 912 "qrwldcrd.sc" */	/* host code */
	    if ((stat = loop_proc(qrb, cur6_rulename, cur6_ownname,
		(bool)(TRUE),cur6_rulename)) != OK)
	    {
/* # line 915 "qrwldcrd.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE7;
      }
/* # line 916 "qrwldcrd.sc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE7:;
  }
    }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR002A_err_ret_iirul));
    }
    else if (stat == OK)
    {
	/* FALSE means these aren't table names, don't check accessability */
	loop_end(qrb, help_type);
    }
}
/*{
** Name:	IIQRsno_SequenceNmOwner - get list of sequences
**
** Description:
**	This routine adds to the name list all the sequences in the
**	iisequences catalog that are visible to the user and whose names
**	match the specified pattern.
**
** Inputs:
**	qrb	query runner control block
**	name6	specification of names to retrieve (contains wildcard)
**	dummy	Not used
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	Will add any names retrieved to the names list for later 'help'
**	processing.
**
** History:
**	17-apr-02 (inkdo01)
**	    Written.
*	29-jan-03 (inkdo01)
**	    Changed to accomodate compound column names in iisequences.
*/
VOID
IIQRsno_SequenceNmOwner(qrb, name6, dummy, help_type)
QRB	*qrb;
  char *name6;
char 	*dummy;
i4	help_type;
{
  char cur6_seqname[FE_MAXNAME + 1];
  char cur6_ownname[FE_MAXNAME + 1];
  char *ename6;
  char *nname6;
  UIDBDATA *uidbdata;
  char *suser;
  char *sdba;
    STATUS	stat = OK;
    loop_init(name6, &ename6);
    nname6 = objectcb.name_dest;
    uidbdata = IIUIdbdata();
    qrsetobjowner( uidbdata, &suser, &sdba, &objectcb );
    if (ename6 != NULL)
    {
/* # line 982 "qrwldcrd.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrwldcrd8",7488,28909);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,sdba);
      IIputdomio((short *)0,1,32,0,iiqrsystabown);
      IIputdomio((short *)0,1,32,0,nname6);
      IIputdomio((short *)0,1,32,0,ename6);
      IIputdomio((short *)0,1,32,0,ename6);
      IIputdomio((short *)0,1,32,0,nname6);
      IIputdomio((short *)0,1,32,0,iiqrcatprefix);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrwldcrd8",7488,28909);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select seq_name, seq_owner from iisequences where(seq_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or seq_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or seq_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or seq_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,iiqrsystabown);
        IIwritio(0,(short *)0,1,32,0,(char *)")and(seq_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,nname6);
        IIwritio(0,(short *)0,1,32,0,(char *)"or seq_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,ename6);
        IIwritio(0,(short *)0,1,32,0,(char *)"or seq_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,ename6);
        IIwritio(0,(short *)0,1,32,0,(char *)"or seq_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,nname6);
        IIwritio(0,(short *)0,1,32,0,(char *)")and seq_name not like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,iiqrcatprefix);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by seq_name");
        IIexDefine(0,(char *)"qrwldcrd8",7488,28909);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE8;
IIrtB8:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur6_seqname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur6_ownname);
      if (IIerrtest() != 0) goto IIrtB8;
/* # line 996 "qrwldcrd.sc" */	/* host code */
	    if ((stat = loop_proc(qrb, cur6_seqname, cur6_ownname,
		(bool)(TRUE),cur6_seqname)) != OK)
	    {
/* # line 999 "qrwldcrd.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE8;
      }
/* # line 1000 "qrwldcrd.sc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE8:;
  }
    }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR0063_err_ret_iiseq));
    }
    else if (stat == OK)
    {
	/* FALSE means these aren't table names, don't check accessability */
	loop_end(qrb, help_type);
    }
}
/*{
** Name:	IIQRsno_SynonymNmOwner - get list of synonyms
**
** Description:
**	This routine adds to the name list all the synonyms in the
**	iisynonyms catalog that are owned by the user and whose names
**	match the specified pattern.
**
** Inputs:
**	qrb	query runner control block
**	name7	specification of names to retrieve (contains wildcard)
**	Dummy	Not used
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	Will add any names retrieved to the names list for later 'help'
**	processing.
**
** History:
**	28-dec-89 (teresal)	Written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	14-jul-92 (rogerl)	Add qrcatprefix; iiqrsystabown
**				Add a several more restrictions to
**				query to make it like all the rest.
**	    Add wildcarding on owner as well as object.
**	08-mar-94 (geri)
**	    Changed to use the normalized name of the object to
**	    address bug 57466.
*/
VOID
IIQRsno_SynonymNmOwner(qrb, name7, dummy, help_type)
QRB	*qrb;
  char *name7;
char	*dummy;
i4	help_type;
{
  char cur7_synname[FE_MAXNAME + 1];
  char cur7_ownname[FE_MAXNAME + 1];
  char *ename7;
  char *nname7;
  UIDBDATA *uidbdata;
  char *suser;
  char *sdba;
    STATUS	stat = OK;
    loop_init(name7, &ename7);
    nname7 = objectcb.name_dest;
    uidbdata = IIUIdbdata();
    qrsetobjowner( uidbdata, &suser, &sdba, &objectcb );
    if (ename7 != NULL)
    {
/* # line 1073 "qrwldcrd.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrwldcrd9",7489,21998);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,sdba);
      IIputdomio((short *)0,1,32,0,iiqrsystabown);
      IIputdomio((short *)0,1,32,0,iiqrcatprefix);
      IIputdomio((short *)0,1,32,0,nname7);
      IIputdomio((short *)0,1,32,0,nname7);
      IIputdomio((short *)0,1,32,0,ename7);
      IIputdomio((short *)0,1,32,0,ename7);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrwldcrd9",7489,21998);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select synonym_name, synonym_owner from iisynonyms where(synonym_owne\
r=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or synonym_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or synonym_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or synonym_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,iiqrsystabown);
        IIwritio(0,(short *)0,1,32,0,(char *)")and synonym_name not li\
ke");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,iiqrcatprefix);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(synonym_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,nname7);
        IIwritio(0,(short *)0,1,32,0,(char *)"or synonym_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,nname7);
        IIwritio(0,(short *)0,1,32,0,(char *)"or synonym_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,ename7);
        IIwritio(0,(short *)0,1,32,0,(char *)"or synonym_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,ename7);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by synonym_name");
        IIexDefine(0,(char *)"qrwldcrd9",7489,21998);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE9;
IIrtB9:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur7_synname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur7_ownname);
      if (IIerrtest() != 0) goto IIrtB9;
/* # line 1087 "qrwldcrd.sc" */	/* host code */
	    if ((stat = loop_proc(qrb, cur7_synname, cur7_ownname,
		(bool)(TRUE),cur7_synname)) != OK)
	    {
/* # line 1090 "qrwldcrd.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE9;
      }
/* # line 1091 "qrwldcrd.sc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE9:;
  }
    }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR0035_err_ret), ERx("iisynonyms"));
    }
    else if (stat == OK)
    {
	/* 
	** The query only includes those synonyms owned by the current
	** user; therefore, we don't throw out objects we don't have
	** access to even though synonyms are "table-like" objects,
	** because all we'll show the user are the ones the user owns!
	*/
	loop_end(qrb, help_type);
    }
}
/*{
** Name:	IIQRcno_ColumnNmOwner - get list of columns
**
** Description:
**	This routine adds to the name list all the columns in the 
**	iicolumns catalog that can be seen by the user and whose names
**	match the specified pattern. This routine is used for the help comment
**	column command.
**
** Inputs:
**	qrb	query runner control block
**	name8	specification of column names to retrieve (contains wildcard)
**	objname	The primary object name (i.e., the table, view or index name)
**		associated with the given column name(s).
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	Will add any names retrieved to the names list for later 'help'
**	processing.
**
** History:
**	02-jan-89 (teresal)	Written.
**	14-jul-92 (rogerl)	Add qrcatprefix; iiqrsystabown
**				Rip out owner retrieve (qrtblower), add call
**				to UIdbdata().
**	    Add wildcarding on owner as well as object.
**	05-oct-93 (rogerl)	If primary identifier is delimited, may
**				have to strip quotes.  Apply qrinitncbpart().
**				(54549)
**	08-mar-94 (geri)
**	    Changed to use the normalized name of the object to
**	    address bug 57466.
**	17-mar-94 (geri)
**	    Allowed processing of compound identifiers for both the
**	    column and table arguments (bug 60244).
*/
VOID
IIQRcno_ColumnNmOwner(qrb, name8, objname, help_type)
QRB	*qrb;
  char *name8;
  char *objname;
i4	help_type;
{
  char cur8_name[FE_MAXNAME + 1];
  char cur8_ownname[FE_MAXNAME + 1];
  char *ename8;
  char *nname8;
  char nobjname[FE_MAXNAME + 1];
  char tuser[FE_MAXNAME + 1];
  i4 colseq;
  char *suser;
  char *sdba;
  UIDBDATA *uidbdata;
    STATUS	stat = OK;
    bool	dmy;
    /*
    ** Process the table name argument (especially in case it
    ** is an owner.table argument (bug 60244). The name and
    ** owner arguments will be used in the query to iitables.
    */
    loop_init(objname, &ename8);
    /* 
    ** Now prepare for the query to the iicolumns table as usual,
    ** but replace the owner arg with the table owner (bug 60244).
    */
    if (ename8 != NULL)
    {
        STcopy(objectcb.name_dest, nobjname);
        STcopy(objectcb.owner_dest, tuser);
        loop_init(name8, &ename8);
        nname8 = objectcb.name_dest;
        STcopy(tuser, objectcb.owner_dest);
        STcopy(nobjname, objname);
        uidbdata = IIUIdbdata();
        qrsetobjowner( uidbdata, &suser, &sdba, &objectcb );
    }
    if (ename8 != NULL)
    {
/* # line 1195 "qrwldcrd.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrwldcrd10",7490,8940);
      IIputdomio((short *)0,1,32,0,nobjname);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,sdba);
      IIputdomio((short *)0,1,32,0,iiqrsystabown);
      IIputdomio((short *)0,1,32,0,iiqrcatprefix);
      IIputdomio((short *)0,1,32,0,nname8);
      IIputdomio((short *)0,1,32,0,ename8);
      IIputdomio((short *)0,1,32,0,ename8);
      IIputdomio((short *)0,1,32,0,nname8);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrwldcrd10",7490,8940);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select column_name, table_owner, column_sequence from iicolumns where\
(table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,nobjname);
        IIwritio(0,(short *)0,1,32,0,(char *)")and(table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,iiqrsystabown);
        IIwritio(0,(short *)0,1,32,0,(char *)")and table_name not like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,iiqrcatprefix);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(column_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,nname8);
        IIwritio(0,(short *)0,1,32,0,(char *)"or column_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,ename8);
        IIwritio(0,(short *)0,1,32,0,(char *)"or column_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,ename8);
        IIwritio(0,(short *)0,1,32,0,(char *)"or column_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,32,0,nname8);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by column_sequenc\
e");
        IIexDefine(0,(char *)"qrwldcrd10",7490,8940);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE10;
IIrtB10:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur8_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur8_ownname);
      IIgetdomio((short *)0,1,30,4,&colseq);
      if (IIerrtest() != 0) goto IIrtB10;
/* # line 1210 "qrwldcrd.sc" */	/* host code */
	    if ((stat = loop_proc(qrb, cur8_name, cur8_ownname,
		(bool)(TRUE),cur8_name)) != OK)
	    {
/* # line 1213 "qrwldcrd.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE10;
      }
/* # line 1214 "qrwldcrd.sc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE10:;
  }
    }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR0035_err_ret), ERx("iicolumns"));
    }
    else if (stat == OK)
    {
	/* FALSE means these aren't table names, don't check accessability */
	loop_end(qrb, help_type);
    }
}
/*{
** Name:	IIQReno_EventNmOwner - get list of events
**
** Description:
**	This routine adds to the name list all the events in the
**	iievents catalog that are visible to the user and whose names
**	match the specified pattern.
**
** Inputs:
**	qrb	query runner control block
**	name9	specification of names to retrieve (contains wildcard)
**	dummy	Not used
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	Will add any names retrieved to the names list for later 'help'
**	processing.
**
** History:
**	ss-mar-91 (kathryn)	Written.
**	14-jul-92 (rogerl)	Add qrcatprefix; iiqrsystabown
**	    Add wildcarding on owner as well as object.
**	08-mar-94 (geri)
**	    Changed to use the normalized name of the object to
**	    address bug 57466.
*/
VOID
IIQReno_EventNmOwner(qrb, name9, dummy, help_type)
QRB	*qrb;
  char *name9;
char	*dummy;
i4	help_type;
{
  char cur9_evtname[FE_MAXNAME + 1];
  char cur9_ownname[FE_MAXNAME + 1];
  char *ename9;
  char *nname9;
  UIDBDATA *uidbdata;
  char *suser;
  char *sdba;
    STATUS	stat = OK;
    loop_init(name9, &ename9);
    nname9 = objectcb.name_dest;
    uidbdata = IIUIdbdata();
    qrsetobjowner( uidbdata, &suser, &sdba, &objectcb );
    if (ename9 != NULL)
    {
	/* bug #5382 - Changed to Repeated select */
/* # line 1283 "qrwldcrd.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrwldcrd11",7491,12273);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,sdba);
      IIputdomio((short *)0,1,32,0,iiqrsystabown);
      IIputdomio((short *)0,1,32,0,iiqrcatprefix);
      IIputdomio((short *)0,1,32,0,nname9);
      IIputdomio((short *)0,1,32,0,ename9);
      IIputdomio((short *)0,1,32,0,ename9);
      IIputdomio((short *)0,1,32,0,nname9);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrwldcrd11",7491,12273);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select event_name, event_owner from iievents where(event_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or event_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or event_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or event_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,iiqrsystabown);
        IIwritio(0,(short *)0,1,32,0,(char *)")and event_owner not lik\
e");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,iiqrcatprefix);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(event_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,nname9);
        IIwritio(0,(short *)0,1,32,0,(char *)"or event_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,ename9);
        IIwritio(0,(short *)0,1,32,0,(char *)"or event_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,ename9);
        IIwritio(0,(short *)0,1,32,0,(char *)"or event_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,nname9);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by event_name");
        IIexDefine(0,(char *)"qrwldcrd11",7491,12273);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE11;
IIrtB11:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur9_evtname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur9_ownname);
      if (IIerrtest() != 0) goto IIrtB11;
/* # line 1297 "qrwldcrd.sc" */	/* host code */
	    if ((stat = loop_proc(qrb, cur9_evtname, cur9_ownname,
		(bool)(TRUE),cur9_evtname)) != OK)
	    {
/* # line 1300 "qrwldcrd.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE11;
      }
/* # line 1301 "qrwldcrd.sc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE11:;
  }
    }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR0042_err_ret_iievt));
    }
    else if (stat == OK)
    {
	/* FALSE means these aren't table names, don't check accessability */
	loop_end(qrb, help_type);
    }
}
/*{
** Name:	IIQRond_DatabaseNmOwner - get list of databases
**
** Description:
**	This routine adds to the name list all the databases in the
**	iidatabase_info catalog that are visible to the user and whose names
**	match the specified pattern.
**
** Inputs:
**	qrb	query runner control block
**	name10	specification of names to retrieve (contains wildcard)
**	dummy	Not used
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	Will add any names retrieved to the names list for later 'help'
**	processing.
**
** History:
**	10-dec-93 (robf) 	Written
*/
VOID
IIQRond_DatabaseNmOwner(qrb, name10, dummy, help_type)
QRB	*qrb;
  char *name10;
char	*dummy;
i4	help_type;
{
  char cur10_dbname[FE_MAXNAME + 1];
  char cur10_ownname[FE_MAXNAME + 1];
  char *ename10;
  UIDBDATA *uidbdata;
  char *suser;
  char *sdba;
    STATUS	stat = OK;
    loop_init(name10, &ename10);
    uidbdata = IIUIdbdata();
    qrsetobjowner( uidbdata, &suser, &sdba, &objectcb );
    if (ename10 != NULL)
    {
/* # line 1362 "qrwldcrd.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrwldcrd12",7492,23658);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,suser);
      IIputdomio((short *)0,1,32,0,sdba);
      IIputdomio((short *)0,1,32,0,iiqrsystabown);
      IIputdomio((short *)0,1,32,0,iiqrcatprefix);
      IIputdomio((short *)0,1,32,0,name10);
      IIputdomio((short *)0,1,32,0,ename10);
      IIputdomio((short *)0,1,32,0,ename10);
      IIputdomio((short *)0,1,32,0,name10);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrwldcrd12",7492,23658);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select database_name, database_owner from iidatabase_info where(datab\
ase_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or database_owner like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or database_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or database_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,iiqrsystabown);
        IIwritio(0,(short *)0,1,32,0,(char *)")and database_owner not \
like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,iiqrcatprefix);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(database_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,name10);
        IIwritio(0,(short *)0,1,32,0,(char *)"or database_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,ename10);
        IIwritio(0,(short *)0,1,32,0,(char *)"or database_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,ename10);
        IIwritio(0,(short *)0,1,32,0,(char *)"or database_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,name10);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by database_name");
        IIexDefine(0,(char *)"qrwldcrd12",7492,23658);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE12;
IIrtB12:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur10_dbname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,cur10_ownname);
      if (IIerrtest() != 0) goto IIrtB12;
/* # line 1376 "qrwldcrd.sc" */	/* host code */
	    if ((stat = loop_proc(qrb, cur10_dbname, cur10_ownname,
		(bool)(TRUE),cur10_dbname)) != OK)
	    {
/* # line 1379 "qrwldcrd.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE12;
      }
/* # line 1380 "qrwldcrd.sc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE12:;
  }
    }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(F_QR00A4_Error_iidatabase_info));
    }
    else if (stat == OK)
    {
	/* FALSE means these aren't database names, don't check accessability */
	loop_end(qrb, help_type);
    }
}
/*{
** Name:	loop_init_obj - extern declaration for loop_init
**
** Description:	
**	If a routine in another file needs to call loop_init,
**	they need access to the objectcb variable statically defined
**	in this file.  
**
**	This routine calls loop_init and returns the pointer to objectcb.
**
** Inputs:
**	pattern		name containing pattern matching characters
**
** Outputs:
**	pattern		name with all '*'s replaced by SQL '%'s
**	epattern	the above extended by a trailing ' %'
**
** History:
**	22-jul-2004 (wanfr01)
**	   Routine created for bug 111105; INGSRV2556
*/
PTR
loop_init_obj(pattern, epattern)
char	*pattern;
char	**epattern;
{
   loop_init(pattern,epattern);
   return(&objectcb); 
}
/*{
** Name:	loop_init - Set up vars prior to start of get list routine
**
** Description:
**	This routine initializes globals used in showing only one of user
**	and DBA owned objects of the same name.  Additionally, the 'ii%'
**	string is IIUBlbo'd.  Finally, the '*'s in the name are replaced
**	by '%'s.
**
**	Additionally, decompose pattern into owner and table (or just table)
**	using FE_decompose()
**	Run IIUGdlm_ChkdlmBEobject() on each piece, and use option to strip
**	non-embedded quotes out, in preparation for using in query.
**
** Inputs:
**	pattern		name containing pattern matching characters
**
** Outputs:
**	pattern		name with all '*'s replaced by SQL '%'s
**	epattern	the above extended by a trailing ' %'
**	obj_owner	if fips specified, will contain owner spec
**	obj_name	will contain obj_name
**	
**
**	Returns:
**		VOID
**
** Side Effects:
**	None
**
** History:
**	31-aug-88 (bruceb)	Written.
**	14-jul-92 (rogerl)	If no wildcards in object, invalidate dissallow
**				of system tables ( want to give help on
**				explicitly named system relats (( only  wilds
**				don't ever match system names )) ).
**				If the object is specified using o.t format,
**				break it up, strip any delimiters, init
**				objectcb.
**	19-may-93 (rogerl)	Don't extend the name if it is 31 or 32 chars;
**			apply lBEo to the original pattern (51830, 51831)
**	21-may-93 (rogerl)	Even if name fails, need to init name control
**				block since it's dereferenced later.
**	09-mar-94 (geri)
**	    bug 58238: check for tablename > 32 characters without 
**	    inadvertently including owner specification in the check.
*/
static VOID
loop_init(pattern, epattern)
char	*pattern;
char	**epattern;
{
    i4		len;
    char	*beg_pattern;
    bool	extendit = TRUE;
    usable_object = TRUE;
    name_save[0] = EOS;
    own_save[0] = EOS;
    syn_save[0] = EOS;
    STcopy(pattern, orig_name);	/* save the user specified pattern */
    /* initialize name control block */
    qrinitncb( &objectcb, &pattern );
    /* parse possibly  -delimited id, -owner.schema specified */
    FE_decompose( &objectcb );
    /* need to know whether or not to run the 'like' stuff on the
    ** object components now (don't want to run 'LIKE' clauses if
    ** the component is a delimited id, because any quoted wildcard
    ** characters will NOT be expanded)
    **
    ** since we also want to strip the delimiter characters (so a
    ** string sent as "rogerl" will match rogerl, even though it doesn't
    ** need to be quoted)
    */
    /* set up the names in the control block */
    qrinitncbnames( &objectcb );
    /* only use extended name stuff on the object name */
    beg_pattern = objectcb.name_dest;
    len = STlength(beg_pattern);
    oname_sz_too_long = (len > FE_MAXNAME);
    /* yech.  check to make sure name isn't FE_MAXNAME minus one or two,
    ** 'cause if it is, don't want to extend the name (extension is
    ** so queries against the fixed length object name field will match)
    */
    if ( oname_sz_too_long )
    {
	if ( len == ( FE_MAXNAME + 2 )
	      && beg_pattern[ 0 ] == ERx( '"' )
	      && beg_pattern[ FE_MAXNAME + 1 ] == ERx( '"' ) )
	{
	    oname_sz_too_long = FALSE;
	    extendit = FALSE;
	}
	else if ( len == ( FE_MAXNAME + 1 )
	      && beg_pattern[ 0 ] == ERx( '"' )
	      && beg_pattern[ FE_MAXNAME ] == ERx( '"' ) )
	{
	    oname_sz_too_long = FALSE;
	    extendit = FALSE;
	}
    }	
    else if ( len == 32 || len == 31 )
    {
	extendit = FALSE;
    }
    /*
    ** If pattern specified is too long, than monitor will automatically
    ** reject it; needn't do anything else here.
    */
    if (oname_sz_too_long)
    {
	oname_ptr = pattern;
	*epattern = NULL;	/* indicate no work performed */
    }
    else
    {
	/* only extend if it is less than 31 characters long */
	if ( extendit )
	    _VOID_ STpolycat(2, beg_pattern, ERx(" %"), extended_name);
	else
	    STcopy( beg_pattern, extended_name );
	*epattern = extended_name;
	/* set vars determing to add system tables to name list */
	/* 	edits globals */
	qrshosysobjs( pattern );
	IIUGlbo_lowerBEobject(iiqrcatprefix);
	IIUGlbo_lowerBEobject(iiqrsystabown);
	startlist = IIQRlnp_LastNodePtr();
    }
}
/*{
** Name:	loop_proc - Process one list item (name & object)
**
** Description:
**	This routine processes one object name and owner pair.  This
**	processing involves only permanently saving (creating name list
**	nodes) those objects actually visible to users.  Thus, if both
**	user and DBA own an object X, only the user object will ultimately
**	be saved.  This is accomplished by only saving away an object
**	after the successive object is examined--thereby gaining time to
**	determine whether object names conflict, and eliminating display
**	of the DBA object.  (This assumes that the routines driving this
**	procedure are retrieving from the database in object_name sorted
**	order.)
**
**	One other constraint imposed on the saving of name list nodes is
**	whether the object type is 'good'.  This is used by the calling
**	routines to work around yet another name space issue.  The issue
**	is that if the user typed 'help view *' and a DBA owned view called
**	X and a user owned index called X both exist, the user name space
**	takes precedence, but object X shouldn't be shown since the user's X
**	is an index, not a view.  All objects need to be stored to determine
**	name precedence, but only those objects whose precedence AND object
**	type are both correct are permanently saved.  good_type is used to
**	tell this routine whether the current object name/owner pair is
**	to be used for permanent storage, or for name precedence processing
**	only.
**
** Inputs:
**	qrb		query runner control block
**	object_name	name of the object
**	object_owner	owner of the object
**	good_type	whether this pair is for storage, or processing only
**
** Outputs:
**
**	Returns:
**		STATUS	FAIL if unable to save away a name list node, else OK
**
** Side Effects:
**	None
**
** History:
**	31-aug-88 (bruceb)	Written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	23-sep-92 (rogerl)
**	    If object_name != syname, this is a synonym and mark as such
**	    so in qrloghlp we can generate some help. 
*/
static STATUS
loop_proc(qrb, object_name, object_owner, good_type, syname)
QRB	*qrb;
char	*object_name;
char	*object_owner;
bool	good_type;
char 	*syname;
{
    bool	saveit = FALSE;
    qrtrimwhite( object_name );
    qrtrimwhite( object_owner );
    qrtrimwhite( syname );
    if (STcompare(object_name, name_save) != 0)
    {
	/*
	** If the current item is different from the previous item,
	** then process the previous item.
	**
	** If the first time through (name_save[0] = EOS), or if the
	** saved values aren't usable (used for name precedence processing
	** only), don't permanently process the last item.
	*/
	if ((name_save[0] != EOS) && (usable_object))
	{
	    if (IIQRaln_AddListNode(qrb, name_save, own_save, synonym, 0) != OK)
		return(FAIL);
	}
	saveit = TRUE; /* save current (new) table info */
	usable_object = good_type; /* indicate whether this pair will be used*/
    }
    else if (STcompare(object_owner, IIUIdbdata()->suser) == 0)
    {
	saveit = TRUE; /* duplicate name - save if user is owner */
	usable_object = good_type; /* indicate whether this pair will be used*/
    }
    if (saveit)
    {
	STcopy(object_name, name_save);
	STcopy(object_owner, own_save);
		/* if this is a synonym, save the synonym name too */
	if ( STcompare( object_name, syname ) != 0 )
	{
	    STcopy(syname, syn_save);
	    synonym = syn_save;
	}
	else
	{
	    synonym = NULL;
	}
    }
    return(OK);
}
/*{
** Name:	loop_end - Process last list item (name & object)
**
** Description:
**	This routine processes the last object name and owner pair.  This
**	processing takes the form of creating a name list node containing
**	already saved information.  If no name/owner has been saved, then
**	save a node stating either that the specified pattern was too
**	long to bother with, or that no objects matching that pattern were
**	found.
**
** Inputs:
**	qrb		query runner control block
**	help_type	help type
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**	As of 6.5, modifies the name list to remove objects the current
**	user can't access.
**
** History:
**	31-aug-88 (bruceb)	Written.
**	15-jul-92 (rogerl)	
**		Exit init statics objectcb and {owner,object}bufs
**	23-oct-92 (daver)
**		Added code to scan the name list, removing items that
**		the current user doesn't have access to. This allows
**		help *\g to only display those DBA objects the user has 
**		access to, and help *.* to display on those objects in
**		the database the user has access to. Done to support
**		the new 6.5 name space.
**	24-nov-92 (daver)
**		Added throw_em_out param to loop end; we only want to
**		toss those tables/indexes/views off the name list the
**		user doesn't have access to. Other objects (procedures,
**		links, events) won't show up as an accessable table
**		name.
**	27-may-93 (rogerl)
**		Check accessibility for HELP_SECURITY_ALARM in a slightly
**		different fashion.
**	12-aug-93 (rogerl)
**		Add more types that ignore 'accessibility' criteria.  If these
**		match the user string, put them in the list.
**      13-Jul-95 (ramra01)
**              Tune the Zap node in loop_end to leave the non existant
**              objects for proper error msg display in help (b66258)
**	19-apr-1996 (canor01)
**		If we're zapping a node in loop_end, retain the
**		pointer to the previous node.
*/
static VOID
loop_end(
QRB	*qrb,
i4	help_type
) {
    /* if a usable name exists to process */
    if ((name_save[0] != EOS) && (usable_object))
	IIQRaln_AddListNode(qrb, name_save, own_save, synonym,0);
    if (oname_sz_too_long)
    {
	/*
	** "+" matches define of TOO_LONG ('+') in qrhelp.h
	*/
	IIQRaln_AddListNode(qrb, oname_ptr, ERx("+"), synonym,0);
    }
    else
    {
	endlist = IIQRlnp_LastNodePtr();
	if (startlist == endlist)
	{
	    /*
	    ** "*" matches define of NONE_FOUND ('*') in qrhelp.h
	    */
	    IIQRaln_AddListNode(qrb, orig_name, ERx("*"), synonym, 1);
	}
	else switch (help_type)
	{
		/* a few types we don't want to subject to any the
		** 'accessability' discrimination
		*/
	    case HELP_INTEGRITY:
	    case HELP_PERMIT:
	    case HELP_PROCEDURE:
	    case HELP_LINK:
	    case HELP_RULE:
	    case HELP_SEQUENCE:
	    case HELP_SYNONYM :
	    case HELP_COMMENT_TBL:
	    case HELP_COMMENT_COL:
	    case HELP_PERM_EVENT:
	    case HELP_PERM_PROC:
	    case HELP_PERM_VIEW:
	    case HELP_PERM_IDX:
	    case HELP_CONSTRAINT:
	    case HELP_ALARM_DB:
	    case HELP_ALARM_INST:
	    case HELP_PERM_SEQ:
	    break;
		/* all others do the 'minimally accessible' check to start
		*/
	    default:
	    {
	    /* 
	    ** If the list is complete and happy (in a pre-6.5 kind of way), 
	    ** lets now run through, and check to see if we can access 
	    ** each table in the list (to implement the new 6.5 namespace,
	    ** where we care if we can access the objects we display in help).
	    ** If we can't access the object, zap the node from the list. 
	    ** 
	    ** So, we want to:
	    **
	    ** find the head of the list
	    ** for each node in the list
	    ** 	call resolve_table to check if we can access the object name
	    ** 	if resolve_table tells us we can access it
	    **		if its our first node, update the head of the list
	    **		leave the node on the name list
	    **		(move the pointers along the list)
	    **	otherwise
	    **		zap it from the name list by having the
	    **		previous node point to the next node on the
	    **		list, passing it up. Free up the memory taken
	    **		by the current node
	    ** if at the end of all this we never found a valid node, reset
	    ** the list pointers, and add a node which means "NONE FOUND"
	    */ 	
		IIQRlist		*current;
		IIQRlist		*previous;
		IIQRlist		*zap_this;
		char		*obj_name;
		char		*obj_owner;
		bool		first_found;
		FE_RSLV_NAME	resolved_name;
		resolved_name.name_dest = NULL;		/* for FE_resolve */
		resolved_name.owner_dest = NULL;	/* for FE_resolve */
		previous = NULL;
		current = IIQRfnp_FirstNodePtr(); 
		first_found = FALSE;
		while (current != NULL)
		{
			IIQRgno_GetNameOwner(current, &obj_name, &obj_owner);
			if (/* TRUE ||*/ FE_resolve(&resolved_name, obj_name, obj_owner))
			{
			    /* 
			    ** name found. keep it in the list. as we now 
			    ** move down the list, our current node becomes 
			    ** the "previous" one 
			    */
			    previous = current;
			    /*
			    ** If its the first good name we found, set the
			    ** flag, and see if its the current first name in
			    ** our list. If not, we'll need to reset the head
			    ** of the list to our first good name found.
			    */
			    if (!first_found)
			    {
				first_found = TRUE;
				if (previous != IIQRfnp_FirstNodePtr())
				{
				    IIQRnfn_NewFirstNode(previous);
				}
			    }
			    current = current->next;
			}
			else /* bad name, remove from list */
			{
			    /*
			    ** point to the node to zap; then have the 
			    ** node behind the offensive one skip the 
			    ** zap-ee by pointing to the next node in 
			    ** the list. move our current pointer down 
			    ** the line, free up the memory, and off we go!
			    */ 
			    zap_this = current;
                            if (!first_found && zap_this->exist == 1)
                            {
                                first_found = TRUE;
				/* 
				** keep current node as previous only
				** when we're keeping it
				*/
			        previous = current;
                                if (previous != IIQRfnp_FirstNodePtr())
                                {
                                    IIQRnfn_NewFirstNode(previous);
                                }
                            }
			    if (first_found && zap_this->exist != 1)
			    {
				/*
				** we have a previous node only if we've
				** found one first!! So only move this
				** pointer along if we have, indeed, found one
				*/
				previous->next = current->next;
			    }
			    current = current->next;
			    if (zap_this->exist != 1)
			    {
				/*
				** if this is the first node ptr, and
				** we are about to free it, replace it
				** with the next on the list
				*/
                                if (zap_this == IIQRfnp_FirstNodePtr())
                                {
                                    IIQRnfn_NewFirstNode(current);
                                }
			        IIQRznd_ZapNode(zap_this);
			    }
			} /* if FEresolve */
		} /* while loop */
		if (!first_found)
		{
		    /*
		    ** If we never found a good node, then set the name list to
		    ** null, then add a node that indicates we didn't find
		    ** anything accessable. Each individual bad node has already
		    ** been freed up, so all we have to do is reset the pointers
		    */
		    IIQRrnl_ResetNameList();
		    IIQRaln_AddListNode(qrb, orig_name, ERx("*"), (char *)NULL,0);
		}
		/* if there are still some nodes in the list, we might want
		** to discriminate further if the help type warrants
		*/
		if ( first_found )
		{
		    switch( help_type )
		    {
			/* the existing * and *.* namespace definition has just
			** been applied to the set of tables.  For security
			** alarm we also want to apply security_priv criteria.
			** For more information, see LRC items:
			** From pholman Fri Apr 16 18:02:42 1993
			** Subject: Revised HELP SECURITY_AUDIT LRC
			*/
		    case HELP_SECURITY_ALARM:
		    {
  char has_priv[2]= { ERx(' '), ERx(' ') };
			IIQRlist	*current = IIQRfnp_FirstNodePtr(); 
			char		*obj_name;
			char		*obj_owner;
			char		user_name[ FE_MAXNAME ];
			char		*puser_name = user_name;
/* # line 1906 "qrwldcrd.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select dbmsinfo('security_pr\
iv')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,has_priv);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1908 "qrwldcrd.sc" */	/* host code */
			/* dbmsinfo('security_priv') is based upon REAL user
			** not any '-u' user (sears user).  the real user thus
			** must be used to determine whether we'll generate
			** security_label information
			*/
			IDname( &puser_name );
			do	 /* already know there is at least one node */
			{
			    IIQRgno_GetNameOwner(current, &obj_name,
			    		&obj_owner);
				   /* if no security priv and not owner, replace
				   ** this node owner with a 'special' code
				   ** interpreted by qrmlthlp (from qrhelp.h)
				   */
			    if ( STcompare( puser_name, obj_owner ) != 0 )
				if ( CMcmpnocase( has_priv, ERx( "N" ) ) == 0 )
				    *(current->owner) = ERx('#');
			    current = current->next;
			} while ( current != NULL );
		    }
		    break;
		    default:
			break;
		    }
		}
	    }
	}
    } /* else oz_name_too_long */
    *(objectcb.name) = EOS;
    *ownerbuf = EOS;
    *objectbuf = EOS;
}
/*{
** Name:	qrreplc	-	string replace char
**
** Description:
**		replace all occurances of oc with nc in objstr
**		(only for single byte oc and nc, but works with
**		variable byte char strings)
**
** Inputs:
**	oc	old character
**	nc	new character 
**	objstr	object string
**
** Outputs:
**
**	Returns:
**		i4	number of characters replaced
**
** Side Effects:
**	replaces chars in string
**
** History:
**	15-jul-92 (rogerl)	abstracted
*/
static i4
qrreplc( char oc, char nc, char *objstr )
{
    char *c;
    i4  count = 0;
    while ( ( c = STindex( objstr, &oc, 0 ) ) != NULL )
    {
	CMcpychar( &nc, c );
	count ++;
    }
    return ( count );
}
/* the following three techniques could be changed to use 
** a 'where' that is created using sprintf
*/
/*{
** Name:	qrshosysobjs	-	edit global patterns which'll
**					determine (in queries) whether
**					system tables are shown or not
**
** Description:
**		Initialize iiqrsystabown and iiqrcatprefix
**
** Inputs:
**		Pattern
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**		Initialize iiqrsystabown and iiqrcatprefix
**
** History:
**	15-jul-92 (rogerl)	written
*/
static VOID
qrshosysobjs( char *pattern )
{
    if ( STindex( pattern, ERx( "*" ), 0 ) != NULL )
    {
	STcopy( ERx( "ii%" ), iiqrcatprefix );
	*iiqrsystabown = EOS;  /* don't match */
    }
    else
    {
	STcopy( UI_FE_CAT_ID_65, iiqrsystabown );
	*iiqrcatprefix = EOS;  /* don't match */
    }
    return;
}
/*{
** Name:	qrsetobjowner	-	edit global patterns which'll
**					determine (in queries) whose
**					tables go into name list
**
** Description:
**		Initialize suser and sdba, depending upon objectcb->owner_dest
**		(if owner was given, as in owner.table; or iiuidbdata vals
**		if not (for current user))
**
** Inputs:
**	uidbdata	- ptr to UIDBdata
**	objectcb	- ptr to object control block
**	
** Outputs:
**	suser		- session user
**	sdba		- session dba
**
** Returns:
**		VOID
**
** Side Effects:
**
** History:
**	15-jul-92 (rogerl)	written
*/
VOID
qrsetobjowner( UIDBDATA *uidbdata, char **suser, char **sdba,
    FE_RSLV_NAME *objectcb )
{
    if ( *( objectcb->owner_dest ) == EOS )
    {
	*suser = uidbdata->suser;
	*sdba = uidbdata->sdba;
    }
    else
    {
	*suser = objectcb->owner_dest;
	*sdba = objectcb->owner_dest;
	    /* don't want real dba in query in this case, so just stick
	    ** in owner (redundant, but hey it works)
	    */
    }
    return;
}
/*{
** Name:	qrxpandobj	-	edit global patterns which'll
**					determine (in queries) whether
**					or not we expand wildcards in the
**					object/owner. 
**
** Description:
**		Initialize owner and object, depending upon objectcb->owner_dlm
**		and objectcb->name_dlm.  If the name and or owner is delimited,
**		initialize the 'owner'(object) to "", which'll prevent it from
**		using a LIKE in the query.  Else, init the fields to the
**		real suser/sdba
**
** Inputs:
**		owner_wld
**		object_wld
**		objectcb
** Outputs:
**		init owner, object to either EOS or to {owner,object}_wld
**		(owner and object 'wld' are merely the strings representing
**		the object that the user typed)
**
** Returns:
**		VOID
**
** Side Effects:
**		init owner, object to either EOS or to {owner,object}_wld
**
** History:
**	15-jul-92 (rogerl)	written
*/
/* call after qrsetobjowner */
static VOID
qrxpandobj( char **owner, char **object, char *owner_wld,
    char *object_wld,  FE_RSLV_NAME *objectcb )
{
    if ( objectcb->owner_dlm )
	*owner = &nilstr;		 /* don't want to expand owner name */
    else
	*owner = owner_wld;
    if ( objectcb->name_dlm )
	*object = &nilstr;		 /* don't want to expand object name */
    else
	*object = object_wld;
    return;
}
/*{
** Name:	qrdoperms	-	edit global patterns which'll
**					determine (in queries) whether
**					or not to check permissions on the
**					object owner for the set owner
**
** Description:
**
** Inputs:
**
** Outputs:
**
** Returns:
**		VOID
**
** Side Effects:
**
** History:
**	15-jul-92 (rogerl)	written
*/
/* call after qrsetobjowner */
static VOID
qrdoperms( char **owner, char **object, char *owner_wld,
    char *object_wld,  FE_RSLV_NAME *objectcb )
{
    return;
}
/*{
** Name:	qrinitncb	-	
**
** Description:
**	Initializse name control block to static buffer 
**	and name to input pattern, is_nrml to false (always user typed)
**
** Side Effect:
**	Side effect - assigns ownerbuf and objectbuf globals for use
**
** Inputs:
**	FE_RSRV_NAME *	ncb
**
** History:
**	15-jul-92 (rogerl)	written
*/
static VOID
qrinitncb( FE_RSLV_NAME *ncb, char **pattern )
{
	ncb->is_nrml = FALSE;		/* always a user typed string */
	ncb->owner_dest = ownerbuf;
	ncb->name_dest = objectbuf;
	ncb->name = *pattern;
	return;
}
/*{
** Name:	qrinitncbnames	-	calls qrinitncbpart for the name
**					and the owner fields of ncb
**
** Description:
**
** Inputs:
**	FE_RSRV_NAME *	ncb	- initialized with qrinitncb
**
** Side Effects:
**	has qrinitncbpart side effects on owner and object fields of ncb
**
** History:
**	15-jul-92 (rogerl)	written
*/
static VOID
qrinitncbnames( FE_RSLV_NAME *ncb )
{
	qrinitncbpart( ncb->name_dest, &(ncb->name_dlm) );
	qrinitncbpart( ncb->owner_dest, &(ncb->owner_dlm) );
	return;
}
/*{
** Name:	qrinitncbpart	-	sets name descriptor 
**					substitutes * with native wildcard
**
** Inputs:
**	char * name	- name
**	bool * isdlm	- name delimited then TRUE else FALSE
**
** Side Effect:
**	Substitutes '*' with native wildcard if it isn't delimited id
**	strips quotes from name
**
** History:
**	15-jul-92 (rogerl)	written
**	20-may-93 (rogerl)
*/
static VOID
qrinitncbpart( char *name, bool *isdlm )
{
		/* anything that begins with a '"' is regarded
		** as delimited here, otherwise not
		*/
	if ( CMcmpcase( name, ERx( "\"" ) ) == 0 )
	    *isdlm = TRUE;
	else
	    *isdlm = FALSE;
		/* ignore possibly illegal object names;
		** all we really care about is the translation
		** of the wildcards into the sql wild character so
		** it'll work in the ensuing LIKES; and stripping
		** off quotes.
		*/
		/* if user didn't delimit - case and translate wilds */
		/* else, strip out the 'quoter' quotes (it's data) */
	if ( *isdlm == FALSE )
	{
	    IIUGlbo_lowerBEobject(name);
	    (VOID) qrreplc( ERx( '*' ), ERx( '%' ), name );
	}
	else
	{
	    IIUGdlm_ChkdlmBEobject( name, name, FALSE );
	}
	return;
}
