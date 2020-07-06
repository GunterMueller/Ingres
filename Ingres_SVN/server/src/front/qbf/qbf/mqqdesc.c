# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	MQQDESC.QSC - This module contains routines to
**		      initialize and manipulate the QDESC
**		      structure.
**		      This structure contains all the info
**		      for a master/detail query to be run.
**		      Once initialized, MQBF calls special
**		      generate routines (shared by MQBF
**		      and OSL) to retrieve data.
**
**
**	Routines:
**	     mqinitqdesc()
**	     mqsetparam()
**	     mqsetwhere()
**
**	Written: 1/13/84 (nml)
**
**	History: 10-sept-85	(marian)	bug # 6381
**				places column in target list for append
**				even when field is in "read" mode - mqsetupd()
**		13-jul-87 (bab) Changed memory allocation to use [FM]Ereqmem.
**		05-aug-87 (marian)	bug 13021
**			Changed mqsetjoin() so it now checks the return
**			value from mqfindjoin() to determine if the join
**			was found.  If the join was not found, reinitilize
**			curra and have it search the remaining joins.
**		18-nov-87 (kenl) Removed QUEL related code.  Added SQL
**				functionality.  Added primary/alternate (they
**				better be unique) key handling mechanisms to
**				work in conjunction with TIDs.  Added
**				extensive (maybe too much?) documentation.
**		07-dec-87 (kenl)
**				Turned module into a .QSC file in order to 
**				incorporate the new header file: mqbf.qsh
**		19-feb-88 (kenl and marian)
**			Integrated fix for bugs 14072, 12969 from 5.0.
**			Set j1att->mdtype = j2att->mdtype so j1att
**			gets assigned correctly.  This value will be
**			used in update.qsc.  Change made in mqsetjoin.
**		22-apr-88 (kenl)
**			Added code to fix major JoinDef bugs.  mqsetjoin()
**			was not setting DB DATA VALUES properly.
**		10-aug-88 (kenl)
**			Modified routine mqsetwhere() so it will set up a
**			VALGNSV QRY_SPEC when join columns are nullable.
**			This fixes a bug when a master join column in NULL.
**		08-nov-88 (kenl)
**			Replaced STcat calls with calls to STlcopy.
**		23-nov-88 (kenl)
**			Added handling of allow_appends and altered FROM clause
**			construction so that the correlation name in not 
**			added if it is the same as the table name.  This
**			change originated from the IBM group (ham).
**		4/19/90 (martym)
**			Added code to make sure column names, range -
**			variables and table names that are SQL keywords 
**			are rejected.
**      	09/26/90 (emerson)
**			Don't display logical keys (bug 8593).
**      	10/05/90 (emerson)
**			Added some logic to allow user-maintained
**			logical keys to be specified on appends
**			*if* joined to a system-maintained logical key.
**			(The user key should be copied from the system key).
**			For this release, such appends are only supported
**			when adding details to an existing master (bug 8593).
**      	10/10/90 (emerson)
**			Replaced a bunch of logic in mqsetjoin
**			by using the new join_att field (bug 33795).
**		10/11/90 (emerson)
**			Replaced global variable allow_appends and allow_updates
**			by IIQFnkaNoKeyForAppends and IIQFnkuNoKeyForUpdates
**			(with opposite sense) because the names are misleading:
**			we now have other reasons for disallowing appends
**			(certain joins on logical keys), and we might have
**			other reasons for disallowing appends or updates
**			in the future.  Keeping separate flags allows us
**			to give specific error messages.  (bug 8593).
**		10/15/90 (emerson)
**			Added notes on logical keys.
**		10/25/90 (emerson)
**			Explicitly coerce logical key columns to char in the
**			"where" clause built by mqsetwhere to retrieve details;
**			an explicit coercion is required by the 6.3/02
**			back-end (bug 8593).
**		10/26/90 (emerson)
**			Changes to correctly implement appends of details
**			in a MD join on 2 user-maintained logical keys;
**			for logical key support (bug 8593).
**		03/09/91 (emerson)
**			Integrated DESKTOP porting changes.
**		17-aug-91 (leighb) DeskTop Porting Change:
**			Added __HIGHC__ pragams to break data segment that 
**			is greater than 64KB into 3 pieces.
**		19-feb-92 (leighb) DeskTop Porting Change:
**			IIUGbmaBadMemoryAllocation() was missing its argument.
**			Moved FUNC_EXTERN declarations outside of function
**			so that function prototypes will not be overridden.
**		6-apr-92 (fraser)
**			Changed arguments to IIUGbmaBadMemoryAllocation per
**			Emerson's suggestion.
**              01/13/92 (connie)
**                      Changed 'newrow' to 'iinewrow' which is less likely
**                      to conflict with the user table column name
**		18-jan-93 (daver)
**			6.5 owner.table and delimited id changes to always
**			use range variables, in case owner.table is
**			being used (and thus select a.col from daver.a a);
**			call IIUGxri_id() to re-quote incase column or
**			table name is a delimited id.
**	31-dec-1993 (rdrane)
**		Use OpenSQL level to determine 6.5 syntax features support,
**		and check dlm_Case capability directly to determine if
**		delimited identifiers are suppported.
**      22-jun-1999 (musro02)
**          Corrected pragma to #pragma
**      15-feb-2000 (musro02)
**          Corrected pragma to #pragma
**
**	Copyright (c) 2004 Ingres Corporation
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**    05-May-2004 (hanje04)
**            SIR 111507 - BIGINT Support
**            "tid" is now an 8 byte (i8) value and must be defined as such
**            when used in table field queries in both the query and the form.
**	14-Jul-2004 (schka24)
**	    Found some places where tid was hardcoded to 4 bytes, fix.
**    25-Oct-2005 (hanje04)
**        Add prototype for _buf_size() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
**      23-Oct-2006 (wonca01) BUG 116667
**          Check catalog level before setting the value of tid->db_length
**          to ensure backward compatibility when connecting to a server that does
**          not support BIGINT.
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include "mqbf.h"
# include	"mqqg.h"
# include	<me.h>
# include	<st.h>
# include	<er.h>
# include	<ui.h>
# include	<afe.h>
# include	"erqf.h"
/*
**  The following discourse on MQQDESC is divided into the following sections:
**
**	1.  Parameter/argument strings/lists which MQQDESC builds.
**	    a.	The SQL SELECT target strings - MQQDEF->mqqdesc->qg_tlist
**	    b.	Argument list for SQL SELECT - MQQDEF->mqqdesc->qg_argv
**	    c.  The SQL INSERT parameter strings - MQQDEF->mqtabs[]->mqaparam
**	    d.	Argument list for SQL INSERT - MQQDEF->mqtabs[]->mqaargv
**	    e.	The SQL UPDATE parameter strings - MQQDEF->mqtabs[]->mquparam
**	    f.	Argument list for SQL UPDATE - MQQDEF->mqtabs->mquargv
**	    g.	The SQL DELETE parameter strings - MQQDEF->mqtabs[]->mqdparam
**	    h.	Argument list for SQL DELETE - MQQDEF->mqtabs[]->mqdargv
**	    i.	The PUTFORM string for master - MQQDEF->mqmpform
**	    j.  Argument list for master PUTFORM - MQQDEF->mqmpargv
**	    k.	The PUTFORM string for detail - MQQDEF->mqdpform
**	    l.	Argument list for detail PUTFORM - MQQDEF->mqdpargv
**	    m.	The GETFORM string for master - MQQDEF->mqmgform
**	    n.	Argument list for master GETFORM - MQQDEF->mqmgargv
**	    o.	The GETFORM string for detail - MQQDEF->mqdgform
**	    p.	Argument list for detail GETFORM - MQQDEF->mqdgargv
**	    q.	Parameter string for FROM clause - MQQDEF->mqqdesc->qg_from     
**
**	2.  The pointers that are used within MQQDESC to keep track of
**	    the parameter strings and argument lists mentioned above.
**	    a.	dbdvoffset
**	    b.	dbdvarray
**	    c.	argnum
**	    d.	next_qargv
**	    e.	pargnum
**	    f.	largnum
**	    g.	aargnum
**	    h.	uargnum
**
**	3.  A cross reference table that equates all of the above parameter
**	    strings and arguments with their corresponding MQQDESC names
**	    and pointers/indexes.
**
**	4.  Comments on the handling of primary/alternate keys.
**
**	5.  Comments on the handling of logical keys.
**
**	SECTION 1
**
**	a. MQQDEF->mqdesc->qg_tlist
**	    This list is of the form:
**		"rvar1.col1,rvar1.col2, . . . rvarN.colN"
**
**	    A maximum of two target lists are ever built for a QBF query.  One
**	    for a master, one for a detail.  Each list may be composed of
**	    multiple tables and any number of columns per table.  Range
**	    variables (when supplied by the user) are used in these lists.  If
**	    range variables are not supplied, the table name is used.
**	    These lists are used by the query generator (QG) in order to perform
**	    a SELECT on the database.  
**
**	    IMPORTANT NOTE:  An SQL SELECT, for a JoinDef is the ONLY multiple
**	    table SQL statement performed on the user's tables.  All other
**	    operations (INSERT, DELETE, UPDATE) are done on a per table
**	    basis.
**
**	b. MQQDEF->mqqdesc->qg_argv
**	    qg_argv is a pointer to a structure of the following format
**	    (each pair of items is a QRY_ARGV structure):
**
**		DB_DATA_VALUE	*qv_dbval;   -- location of SELECTed value #1
**		char		*qv_name;    -- column name for value #1
**		   .
**		   .
**		DB_DATA_VALUE	*qv_dbval;   -- location of SELECTed value #N
**		char		*qv_name;    -- column name for value #N
**
**	    IMPORTANT NOTE: qg_argv IS NOT an array of pointers to structures 
**	    of type QRG_ARGV.  It points to the beginning of a list of items,
**	    each pair of which represents a QRY_ARGV.  NULL values terminate
**	    the list.
**
**	    For each attribute being SELECTed, the pointer to its column name
**	    is copied from the ATTRIBINFO structure to qv_name.  A pointer
**	    to a DB_DATA_VALUE is copied from the same structure to qv_dbval.
**
**	c. MQQDEF->mqtabs[]->mqaparam
**	    This parameter string is of the form:
**		"(col1,col2,...,coln) VALUES (?,?,...,?)"
**
**	    This string is used for SQL INSERTs on a per table basis.  This
**	    is used in conjunction with a dynamic SQL INSERT statement
**	    wherein an SQLDA is used to fill in the question marks with
**	    DB_DATA_VALUEs that point to the actual values to insert into
**	    the table.
**
**	d. MQQDEF->mqtabs[]->mqaargv
**	    This argument list is a set of pointers to DB_DATA_VALUEs to
**	    be used by the dynamic SQL INSERT.  Each pointer to a DB_DATA_VALUE
**	    will get copied into an SQLDA to be used as part of the dynamic
**	    statement.  The pointer to the DB_DATA_VALUE is copied from
**	    the ATTRIBINFO structure corresponding to the column to be
**	    inserted.
**
**	e. MQQDEF->mqtabs[]->mquparam
**	    This parameter string is of the form:
**		"col1=?,col2=?,...,coln=?"
**
**	    The string is used to perform dynamic SQL UPDATE statements.
**	    All columns that are updatable, based upon the form being
**	    used with the table/JoinDef, are included within the UPDATE
**	    statement.  If the table has keys, those columns are included
**	    as part of a WHERE clause even though they may not appear
**	    as one of the columns to update.  (see mqdparam(g) and
**	    mqdargv(h) below.)
**
**	f. MQQDEF->mqtabs[]->mquargv
**	    This list holds arguments for use with the dynamic SQL UPDATE.
**	    Unlike INSERT, the UPDATE statement requires a WHERE clause.
**	    There is only one argument list for both parts of the ESQL
**	    statement.  The DB_DATA_VALUES get filled in one-by-one in the same
**	    sequence as the question marks appear within the UPDATE
**	    statement.
**
**	g. MQQDEF->mqtabs[]->mqdparam
**	    This parameter string is of the form:
**		"col1=?,col2=?,...,coln=?"
**			or
**		"tid=?"  (INGRES backend)
**			or
**		a combination of the above
**
**	    This string is used to form the WHERE clause
**	    as part of the dynamic SQL DELETE and UPDATE statements.
**	    The question marks are filled in with the appropriate 
**	    DB_DATA_VALUES.  It is assumed that the WHERE
**	    clause used for an SQL UPDATE will always be identical to the
**	    WHERE clause used for an SQL DELETE (when accessing the same
**	    table, of course).  When keys are being used, if one of the
**	    keys contains a null value, the expression "key=?" will
**	    be replaced with "key IS NULL".  This is done at runtime by
**	    the routines in module QFSQL.QSC that are responsible for
**	    emitting dynamic SQL UPDATEs and DELETEs to the backend.
**
**	h. MQQDEF->mqtabs[]->mqdargv
**	    Argument list used for dynamic SQL DELETE statement.
**	    The argument list for DELETE uses the
**	    argument list used by the UPDATE statement.  In fact, the pointer
**	    mqdargv points directly into the middle of the argument list
**	    for the UPDATE statement, mquargv.  It points to those trailing
**	    arguments in the UPDATE list that represent the values to be
**	    used by the UPDATE WHERE clause.
**
**	i. MQQDEF->mqmpform
**	    This parameter string has the form:
**		"col1=%a,col2=%a,...,coln=%a"  (non-tablefield)
**			    or
**		"col1=%a,...,coln=%a,key1=%a,...,keyn=%a,iinewrow=%i2"
**
**	    There will be at most two PUTFORM strings, one for the master
**	    and one for the detail information.  A master for a SimpleFields
**	    display is quite straightforward, each item in the parameter
**	    string represents a displayed field.
**
**	    For a table field, however, additional information needs to
**	    be maintained within hidden columns for each row in the table.
**	    Unlike the the SimpleFields display which maintains primary key
**	    or TID information within the ATTRIBINFO structure for each
**	    column in the display (this is possible because we are only dealing
**	    with one row at a time), the TableFields display must keep track
**	    of the primary key or TID information on a per row basis.
**
**	    IMPORTANT NOTE:  When dealing with primary keys it is possible that
**	    the key WILL NOT appear as a column on the form but will still
**	    have to be maintained as a hidden column.  This also necessitates
**	    the column appearing in the initial SELECT statement so that it
**	    may be copied to the hidden column.  Please refer to the section
**	    below on primary keys for further information.
**
**	j. MQQDEF->mqmpargv
**	    The argument list for the master PUTFORM parameter string.  In
**	    olden days, this argument list would look very similay to the
**	    SELECT argument list.  With the addition of primary key handling,
**	    however, additional columns will be SELECTED from a table that
**	    will not appear on the display portion of a form yet will be stored
**	    in a hidden column for the proper updating and deleting of rows.
**
**	k. MQQDEF->mqdpform
**	    This is the parameter string used for PUTFORMS on the detail part
**	    of a form.  Refer to MQQDEF->mqmpform above (i) for more detail.
**
**	l. MQQDEF->MQdpargv
**	    This points to the argument list used for PUTFORMS on the detail
**	    part of a form.  Refer to MQQDEF->mqmpargv above (j) form more
**	    information.
**
**	m. MQQDEF->mqmgform
**	    This parameter string is of the form:
**		"%a=col1,...,%a=coln"  (non-TableField)
**				or
**		"%a=col1,...,%a=key1,...%i2=iinewrow,%i4=_RECORD,%i4=_STATE"
**
**	    There should be at most two GETFORM parameter strings within
**	    QBF at a time, one for the master information and one for the
**	    detail information.  The GETFORM string should be identical to the
**	    PUTFORM statement with the addition of the RECORD and STATE
**	    hidden columns.
**
**	n. MQQDEF->mqmgargv
**	    The argument list for the GETFORM from a master will parallel
**	    the argument list for PUTFORM except that the DB_DATA_VALUE pointed
**	    to will be at offset dbv in the ATTRIBINFO structure as opposed
**	    to offset rdbv.  In addition, there will be pointers to
**	    DB_DATA_VALUES that represent RECORD and STATE information.
**
**	o. MQQDEF->mqdgform
**	    This is the parameter string used for GETFORMs on the detail part
**	    of a form.  Refer to MQQDEF->mqmgform above (m) for more
**	    information.
**
**	p. MQQDEF->MQdgargv
**	    This points to the argument list used for GETFORMs on the detail
**	    part of a form.  Refer to MQQDEF->mqmpargv above (n) form more
**	    information.
**
**	q. MQQDEF->mqdesc->qg_from
**	    This list is of the form:
**		"table1 rvar1, ... ,tableN rvarN"
**
**	    Each table required for a table, JoinDef, or QBFName is included
**	    in the FROM clause.  In the default case, when the user does not
**	    supply an abbreviation for the table (range variable) both names
**	    will be the same. 
** 
** 	    With the advent of 6.5, table1 above can be represented as
** 	    owner.tablename. In this case, the rvar1 will simply be the
** 	    tablename without the "owner." prefix. For example, if the
** 	    real query's from list looks like "daver.foo, mgw.bar", the
** 	    qg_from list would look like "daver.foo foo, mgw.bar bar".
** 	    This DOES imply that if you want to use daver.foo and
** 	    mgw.foo, you HAVE to use an abbreviation (range variable)
** 	    for one of the two tables to disambiguate them.
**
**
**  SECTION 2
**
**	a. dbdvoffset - GLOBAL i4
**		and
**	b. dbdvarray[MQ_MAXATTS] - GLOBAL DB_DATA_VALUE
**	    MQQDESC allocates DB_DATA_VALUES at run time to build its
**	    SELECT argument list.  When a new DB_DATA_VALUE is required,
**	    MQQDESC takes the next available one from the array dbdvarray.
**	    When the procedure mqsetparam is invoked,
**	    it expects dbdvoffset to point to the next available
**	    DB_DATA_VALUE.  Therefore, the first time mqsetparam is invoked,
**	    dbdvoffset is set to zero.  At the end of its processing,
**	    mqsetparam resets dbdvoffset to point to the location immediately
**	    past (ie. next available) the one it just used.  When mqsetparam
**	    is invoked a second time (for the deatil processing of a 
**	    master/detail) it points to the proper location for the next
**	    available DBDV.
**
**	c. argnum - i4
**	    argnum keeps track of the number of arguments being allocated
**	    from the array dbdvarray for the current master or detail.
**	    It should ALWAYS be used relative to dbdvoffset when elements
**	    are being allocated from dbdvarray.
**
**	d. next_qargv - i4
**	    next_qargv keeps track of the number of arguments to be used in
**	    the actual SELECT for the master or detail.  The distinction 
**	    between next_qargv and argnum can be made as follows:  We may be
**	    adding an argument to the SELECT that does not require a new
**	    DB_DATA_VALUE to be allocated, such as a TID (in this case we
**	    increment next_qargv by one) or when setting up the PUTFORM
**	    and GETFORM argument lists we will need additional DB_DATA_VALUE
**	    locations to handle the hidden columns for primary keys (argnum
**	    will be incremented appropriately).
**
**	e. pargnum - i4
**	    pargnum keeps track of the arguments contained within the
**	    PUTFORM argument list.
**
**	f. largnum - i4
**	    largnum keeps track of the arguments contained within the 
**	    GETFORM argument list.  Please note that in the MQQDESC code,
**	    pargnum and largnum go hand in hand for a while.  In fact, it
**	    may be possible to use one instead of two variables but for the
**	    sake of my sanity, they will remain two.
**
**	g. aargnum - i4
**	    Used in the routine mqsetupd, aargnum keeps track of the number
**	    of arguments that are used in the argument list for INSERTs.
**
**	h. uargnum - i4
**	    Used in the routine mqsetupd, uargnum keeps track of the number
**	    of arguments that are used in the argument list for UPDATEs.
**
**
**  SECTION 3
**
**  QBF name			    Related
**  (in MQTYPES.QH   MQQDESC	    pointers/
**  or QG.H)	     names	    indexes		    Definition
**  --------------  ---------	    --------    -------------------------------
**  qg_tlist	    rparam			SELECT parameter string
**  qg_argv	    mq[12]qragv	    next_qargv	SELECT arguments
**  mqaparam	    aparm			INSERT parameter string
**  mqaargv	    aarg	    aargnum	INSERT arguments
**  mquparam	    uparm			UPDATE parameter string
**  mquargv	    uarg	    uargnum	UPDATE arguments
**  mqdparam	    wparm			DELETE (WHERE clause) string
**  mqdargv	    (uses uarg)			DELETE arguments
**  mq[md]pform	    pparam			Master/Detail PUTFORM string
**  mq[md]pargv	    mq[12]args	    pargnum	Master/Detail PUTFORM arguments
**  mq[md]gform	    gparam			Master/Detail GETFORM string
**  mq[md]gargv	    mq[md]uargs	    largnum	Master/Detail GETFORM arguments
**  qg_from	    fparam			FROM clause parameter string
**
**
**  SECTION 4
**
**  Comments on the handling of primary keys
**
**	    A new field has been added to the ATTRIBINFO structure called
**	    keyinfo.  Currently it can have one of two values: mqISKEY or
**	    mqISNOTKEY.  This field is initialized in the modules GETATTRIB
**	    and GETQDEF.  Throughout MQQDESC you will find statements that
**	    check this flag.
**
**	    If a field does not appear on a form it may still have to SELECTed
**	    from the database because it is a key.  In this case, the SELECT
**	    statement will reference the key, APPEND will NOT, UPDATE and 
**	    DELETE will reference it ONLY in the WHERE clause, and the PUTFORM
**	    and GETFORM strings and parameter lists will ONLY reference the
**	    column after columns that DO appear on the form (ie. as
**	    a hidden field, this is for TableFields only).  Needless
**	    to say, the above example indicates the need for a variety
**	    of pointers/indexes to keep track of things.
**
**	    For a key value to appear in a hidden field, it must be put there
**	    somehow.  Since we are not SELECTing directly into the hidden
**	    field (this could result in the same column appearing twice in
**	    the SELECT statement) there must be a method by which we know
**	    what SELECTed column needs to be copied to which hidden column.
**	    The array key_xref keeps track of this.  The variable next_xref
**	    indicates how many keys were cross-referenced.  The key_xref
**	    array works as follows:
**
**		If a TableField is being displayed, look for all keys that
**		belong to a row of the TableField.  By definition, all
**		keys appearing or not appearing in a row have already been
**		included as part the SELECT statement.  MQQDESC must determine
**		which argument, positionally, within the SELECT is a key.
**		That number, in addition to the position of the hidden column
**		argument within the PUTFORM is stored.  When the data is
**		SELECTed from the database, the key_xref array is used to
**		perform the copy from the SELECTed arguments to the
**		PUTFORM/GETFORM arguments (hidden columns).
**
**
**  SECTION 5
**
**  Comments on the handling of logical keys
**
**	    A new field has been added to the ATTRIBINFO structure called
**	    lkeyinfo.  It's non-zero for (and only for) logical keys.
**	    It indicates whether the logical key is a table key or an object key
**	    and whether it's system-maintained or user-maintained.
**
**	    Logical keys are never displayed on the form.
**	    They are never included in updates.
**	    They are never included in appends, except in the cases where:
**	    (1) a user-maintained logical key
**	    is joined to a system-maintained logical key.
**	    (2) a user-maintained logical key in a detail
**	    is joined to another user-maintained logical key in a master.
**
**	    In case (1), we'd like to derive the user-maintained
**	    logical key from the system-maintained key.
**	    In the case where a detail user key is be appended to an existing
**	    master system key via the update screen, we accomplish this
**	    by getting the detail user key for the append from the last master
**	    system key we retrieved.  (We copy the relevant DBV,
**	    *rdbv to dbv, after retrieving a new master).
**	    The other cases we'd like to support involve appending the system
**	    key just before appending the user key.  But currently, the back-end
**	    provides no feedback of the system key it generated internally
**	    on an append.  So for this release, we can't support these cases.
**	    We'll have to wait until the back-end supports feedback.
**	    The unsupported cases never get as far as attempting to execute
**	    a query; we flag them as errors or prevent the user from adding
**	    rows to the table field on the update or append screen.
**
**	    Case (2) is treated like case (1) is treated (in the subcase
**	    of a master-detail join).
**
**	    Note that when a user key is *not* joined to a system key
**	    and is *not* a detail user key joined to a master user key,
**	    we let appends proceed; if the key is nullable, it will be set
**	    to null; if the key has a default, it will be set to that.
**	    If the key is not nullable and has no default, the back-end
**	    will flag an error at run time.  If two nullable user keys
**	    are joined, appends are permitted from the joindef.
**	    The resulting records, however, have null keys; a retrieve
**	    from the joindef will not retrieve the records that were
**	    just appended.  This seems anomalous to me, but it's consistent
**	    with QBF's behavior for any join on a nullable column 
**	    (where the user doesn't enter any value for the column).
*/
/*
**  The two globals defined below are used to keep track of primary/alternate
**  keys.  The array key_xref provides a means by which QBF knows which
**  SELECTed columns from a database must be copied to hidden fields for
**  TableField processing (PUTFORM and GETFORM).  The first element within
**  each row of key_xref indicates, positionally, the column from the SELECT to
**  copy; the second element indicates, positionally, which hidden field the
**  column should be copied to.
**
**  The variable next_ref keeps a count of how many key fields have been
**  processed.
**
**  NOTE: A local variable, tbl_xref, is used within routine mqsetparam to
**  assist in the mapping of SELECTed columns to TableField columns.  Its
**  use is described in that routine.
*/
GLOBALDEF i4	key1_xref[MQ_MAXATTS][2];
GLOBALDEF i4	next1_xref;
# ifdef __HIGHC__
#pragma		Static_segment("MQQDESC1DATA");
# endif
GLOBALDEF i4	key2_xref[MQ_MAXATTS][2];
GLOBALDEF i4	next2_xref;
GLOBALDEF bool	IIQF_NoDetail = FALSE;
/*
**  The two globals defined below keep track of logical key
**  attributes in the master of a master-detail joindef.
**  The array IIQFmla_MLkeyInMDArray records pointers to all such attributes;
**  IIQFmlc_MLkeyInMDCount keeps track of how many such attributes have been
**  found.
*/
GLOBALDEF ATTRIBINFO	*IIQFmla_MLkeyInMDArray[MQ_MAXATTS];
GLOBALDEF i4	IIQFmlc_MLkeyInMDCount = 0;
static	mqsetwhere();
static i4 _buf_size();
mqinitqdesc(qdef)
MQQDEF	*qdef;
{
	QDESC	*set_qdesc;
	i4	setcode;
# ifdef DEBUG
	if (mq_debug)
	{
		SIfprintf( mqoutf, ERx("MQINITQDESC: at start of routine, msecs = %d\n"), TMcpu());
	}
# endif
	next1_xref = 0;
	next2_xref = 0;
	IIQFmlc_MLkeyInMDCount = 0;
	IIQF_NoDetail = FALSE;
	/*
	** First set up param strings for retrieve from database and
	** puts to database (ie-update/append).
	** Use mqsetparam() to do this.
	*/
	if (qdef_type == 1)
	{
		setcode = 2;
		if ((set_qdesc = mqsetparam(qdef, setcode)) == NULL)
		{
			/* problem setting up params in 1-1 case */
			return(FALSE);
		}
		qdef->mqqdesc = set_qdesc;
	}
	else
	{
		setcode = 0;
		if ((set_qdesc = mqsetparam(qdef, setcode)) == NULL)
		{
			/* problem setting up master params */
			return(FALSE);
		}
		qdef->mqqdesc = set_qdesc;
		setcode = 1;
		if ((set_qdesc = mqsetparam(qdef, setcode)) == NULL)
		{
			/* problem setting up detail params */
			return(FALSE);
		}
		if (set_qdesc->qg_tlist != (QRY_SPEC (*)[])NULL)
		{
			qdef->mqqdesc->qg_child = set_qdesc;
		}
		else
		{
			qdef_type = 1;
			IIQF_NoDetail = TRUE;
		}
	}
	/*
	** Now set up param strings for where clause(s).
	** Use mqsetwhere() to do this.
	*/
	if ((mqsetwhere(qdef)) != TRUE)
	{
		/* problem setting up where clauses. */
		return(FALSE);
	}
	if (qdef->mqqdesc->qg_where != NULL)
		mq_mwhereend = qs_count(qdef->mqqdesc->qg_where);
	if (qdef->mqqdesc->qg_child != NULL)
		mq_dwhereend = qs_count(qdef->mqqdesc->qg_child->qg_where);
# ifdef DEBUG
	if (mq_debug)
	{
		SIfprintf( mqoutf, ERx("MQINITQDESC: at end of routine, msecs = %d\n"), TMcpu());
		}
# endif
	return(TRUE);
}
/*
**  mqsetparam() - Routine to alloc space for the QDESC
**		   and set up the param strings.
**
*/
/*
**  By using keys instead of TIDs we must allocate additional space for the
**  arrays of pointers to DB DATA VALUES.  In the worst case situation, every
**  column for every table in a JoinDef may be a key field.  In this case,
**  the old limits would be pushed to twice the size that they used to be.
**
**  The limit for dbdvarray goes to four times its old value since we may
**  be allocating twice as many master and twice as many detail DB DATA
**  VALUES.
**
**  Please note that the limits for mq[12]qargv remain the same because even
**  with the use of keys, we will never be exceeding the specified limit
**  on a SELECT.  It's the need to have enough space to accommodate the 
**  hidden columns within a TableField display that have forced the other
**  limits up.
*/
GLOBALDEF PTR	mq1args[2 * MQ_MAXATTS];
GLOBALDEF PTR	mq2args[2 * MQ_MAXATTS];
GLOBALDEF PTR	mqmuargs[2 * MQ_MAXATTS];
GLOBALDEF PTR	mqduargs[2 * MQ_MAXATTS];
# ifdef __HIGHC__
#pragma		Static_segment("MQQDESC2DATA");
# endif
GLOBALDEF DB_DATA_VALUE dbdvarray[4 * MQ_MAXATTS];
GLOBALDEF i4	dbdvoffset;		/* index in dbdvarray */
GLOBALDEF QRY_ARGV	mq1qargv[MQ_MAXATTS];
GLOBALDEF QRY_ARGV	mq2qargv[MQ_MAXATTS];
GLOBALDEF char	*mqmrec = NULL;
GLOBALDEF char	*mqdrec = NULL;
GLOBALDEF char	*mqumrec = NULL;
GLOBALDEF char	*mqudrec = NULL;
GLOBALDEF i4	lmqmrec = 0;
GLOBALDEF i4	lmqdrec = 0;
GLOBALDEF i4	lmqumrec = 0;
GLOBALDEF i4	lmqudrec = 0;
static	char	*rparam = NULL;  		/* RETRIEVE param string */
static	char	*pparam = NULL;   		/* PUT param to form */
static	char	*gparam = NULL;   		/* GET param from form */
static	i4	lparams = 0;			/* length of parameter strings*/
static	i4	pnumatts = 0;			/* previous number of atts */
static	char	*uparm;
static	char	*aparm;
static	char	*wparm;
/*
**  The elements within tbl_xref form a one-to-one correspondence
**  with the elements within the mq[12]qargv structures.  The 
**  mq[12]qargv structures, used for the QG SELECT, indicate column
**  names but do not keep track of the table from which the column
**  came.  tbl_xref keeps track of table names so that when the
**  key cross reference table is built, key_xref, we will be sure
**  that we are using the right column (this gets around the problem
**  of two tables have the same column name).
*/
static	char		**tbl_xref = NULL;
FUNC_EXTERN i4		afe_pad();			 
FUNC_EXTERN ADF_CB	*FEadfcb();			 
FUNC_EXTERN bool	IIUGIsSQLKeyWord();		 
FUNC_EXTERN char	*IIQFRvToTabName();		 
QDESC	*
mqsetparam(qdef, code)
MQQDEF	*qdef;
i4	code;
{
	QDESC		*s_qdesc;
	ATTRIBINFO	*satt;
	RELINFO		*tabl;
	char		fparam[MQ_MAXRELS * ((2 * FE_MAXNAME) + 3)];
					        /* FROM param for use with SQL*/
	char		tbuf[FE_MAXNAME + 1];	/* buffer to build param tids*/
	char		kbuf[FE_MAXNAME + 1];	/* buffer to build param keys*/
	char		unrml_buf[FE_UNRML_MAXNAME+1]; /* unormalized ids */
	char		*rcomma;	/* pointer to end of RET param string*/
	char		*pcomma;	/* pointer to end of PUT param string*/
	char		*gcomma;	/* pointer to end of GET param string*/
	char		*tcomma;	/* pointer to end of tids param string*/
	char		*fcomma;	/* pointer to end of FROM param string*/
	char		*recptr;	/* pointer to retrieve buffer */
	char		*updptr;	/* pointer to update buffer */
	i4		argnum;		/* target list/dbdvarray offset */
	i4		next_qargv;	/* next argument in SELECT target list*/
	i4		largnum;	/* number of columns in getform */
	i4		pargnum;	/* number of columns in putform */
	i2		recl;
	i4		tnum;		/* table number for use in param tids */
	i4		i;
	i4		j;
	i4		off;
	i4		loc;
	i4		pad;
	i4		rval;
	ADF_CB		*adf_cb;
	DB_DATA_VALUE	*dbv;
	DB_DATA_VALUE	*rdbv;
	DB_DATA_VALUE	*kdbv;
	DB_USER_TYPE	user_type;
	/*
	**  As each table is processed in order to produce the UPDATE and
	**  INSERT parameter strings and argument lists, hidden columns may
	**  be created if TableFields are being used.  The hidden columns
	**  must point to DB_DATA_VALUES that reflect the original values of
	**  the keys that were loaded into the table.  first_key points to the
	**  first of possibly several keys that are being added to the putform
	**  and getform lists.  When the procedure mqsetupd is called to
	**  produce the UPDATE and INSERT lists it must know which keys need
	**  to be specified as part of a WHERE clause for DELETEs and UPDATEs.
	**  first_key points to the first argument within the lists mq[12]args
	**  that contain the DB_DATA_VALUES that will get referenced in the
	**  WHERE clause for DELETE and UPDATE statements.
	*/
	i4		first_key;
	i = (2 * qdef->mqnumatts * (FE_MAXNAME + 5)) + 40;
	if (i > lparams)
	{
	    lparams = (i < 1000) ? 1000: i;
	    if (rparam != NULL)
	    {
		MEfree(rparam);
		MEfree(pparam);
		MEfree(gparam);
	    	for (i = 0; i < pnumatts; i++)
		{
		    if (tbl_xref[i] != NULL)
		    {
			MEfree(tbl_xref[i]);
		    }
	    	}
		MEfree(tbl_xref);
		MEfree(wparm);
		MEfree(aparm);
		MEfree(uparm);
	    }
	    if ((rparam = (char *)MEreqmem((u_i4)0, (u_i4)lparams,
		TRUE, (STATUS *)NULL)) == NULL)
	    {
		IIUGbmaBadMemoryAllocation(ERx("mqqdesc.qsc"));  	
	    }
	    if ((pparam = (char *)MEreqmem((u_i4)0, (u_i4)lparams,
		TRUE, (STATUS *)NULL)) == NULL)
	    {
		IIUGbmaBadMemoryAllocation(ERx("mqqdesc.qsc"));  	
	    }
	    if ((gparam = (char *)MEreqmem((u_i4)0, (u_i4)lparams,
		TRUE, (STATUS *)NULL)) == NULL)
	    {
		IIUGbmaBadMemoryAllocation(ERx("mqqdesc.qsc"));  	
	    }
	    pnumatts = qdef->mqnumatts;
	    if ((tbl_xref = (char **)MEreqmem((u_i4)0,
		(u_i4)sizeof(char *) * pnumatts,
		TRUE, (STATUS *)NULL)) == NULL)
	    {
		IIUGbmaBadMemoryAllocation(ERx("mqqdesc.qsc"));  	
	    }
	    for (i = 0; i < pnumatts; i++)
	    {
		tbl_xref[i] = NULL;
	    }
	    if ((uparm = (char *)MEreqmem((u_i4)0, (u_i4)lparams,
		TRUE, (STATUS *)NULL)) == NULL)
	    {
		IIUGbmaBadMemoryAllocation(ERx("mqqdesc.qsc"));  	
	    }
	    if ((aparm = (char *)MEreqmem((u_i4)0, (u_i4)lparams,
		TRUE, (STATUS *)NULL)) == NULL)
	    {
		IIUGbmaBadMemoryAllocation(ERx("mqqdesc.qsc"));  	
	    }
	    if ((wparm = (char *)MEreqmem((u_i4)0, (u_i4)lparams,
		TRUE, (STATUS *)NULL)) == NULL)
	    {
		IIUGbmaBadMemoryAllocation(ERx("mqqdesc.qsc"));  	
	    }
	}
	else if (pnumatts < qdef->mqnumatts)
	{
	    if (tbl_xref != NULL)
	    {
	    	for (i = 0; i < pnumatts; i++)
		{
		    if (tbl_xref[i] != NULL)
		    {
			MEfree(tbl_xref[i]);
		    }
	    	}
	        MEfree(tbl_xref);
	    }
	    pnumatts = qdef->mqnumatts;
	    if ((tbl_xref = (char **)MEreqmem((u_i4)0,
		(u_i4)sizeof(char *) * pnumatts,
		TRUE, (STATUS *)NULL)) == NULL)
	    {
		IIUGbmaBadMemoryAllocation(ERx("mqqdesc.qsc"));  	
	    }
	    for (i = 0; i < pnumatts; i++)
	    {
		tbl_xref[i] = NULL;
	    }
	}
	rparam[0] = '\0';
	pparam[0] = '\0';
	gparam[0] = '\0';
	tbuf[0] = '\0';
	fparam[0] = '\0';
	mq_tids[0] = '\0';
	argnum = 0;
	pargnum = 0;
	recl = 0;
	tnum = 0;
	rcomma = rparam;
	pcomma = pparam;
	gcomma = gparam;
	tcomma = mq_tids;
	fcomma = fparam;
	if ((s_qdesc = (QDESC *)MEreqmem((u_i4)0, (u_i4)sizeof(QDESC), TRUE,
	    (STATUS *)NULL)) == NULL)
	{
		FDerror(ERRALLOC, 0, ERx(""));
		return(NULL);
	}
	s_qdesc->qg_child = 0;
	s_qdesc->qg_internal = 0;
	s_qdesc->qg_sort = NULL;
	s_qdesc->qg_dml = DB_SQL;
	if (code != 1)
	{
	    /* for master of M/D or 1-1 re-init dbdvoffset index */
	    dbdvoffset = 0;
	    if ((i = _buf_size(qdef, mqIN_MASTER)) > lmqmrec)
	    {
		lmqmrec = (i < 1000) ? 1000: i;
		if (mqmrec != NULL)
		{
			MEfree(mqmrec);
			MEfree(mqumrec);
		}
		if ((mqmrec = (char *)MEreqmem((u_i4)0, (u_i4)lmqmrec,
			TRUE, (STATUS *)NULL)) == NULL)
		{
		    IIUGbmaBadMemoryAllocation(ERx("mqqdesc.qsc"));  
		}
		if ((mqumrec = (char *)MEreqmem((u_i4)0, (u_i4)lmqmrec,
			TRUE, (STATUS *)NULL)) == NULL)
		{
		    IIUGbmaBadMemoryAllocation(ERx("mqqdesc.qsc"));  
		}
	    }
	    recptr = mqmrec;
	    updptr = mqumrec;
	}
	else
	{
	    /*
	    ** for detail part of master/detail just pick up
	    ** dbdvarray where master left it (at dbdvoffset).
	    */
	    if ((i = _buf_size(qdef, mqIN_DETAIL)) > lmqdrec)
	    {
		lmqdrec = (i < 1000) ? 1000: i;
		if (mqdrec != NULL)
		{
			MEfree(mqdrec);
			MEfree(mqudrec);
		}
		if ((mqdrec = (char *)MEreqmem((u_i4)0, (u_i4)lmqdrec,
			TRUE, (STATUS *)NULL)) == NULL)
		{
		    IIUGbmaBadMemoryAllocation(ERx("mqqdesc.qsc"));  
		}
		if ((mqudrec = (char *)MEreqmem((u_i4)0, (u_i4)lmqdrec,
			TRUE, (STATUS *)NULL)) == NULL)
		{
		    IIUGbmaBadMemoryAllocation(ERx("mqqdesc.qsc"));  
		}
	    }
	    recptr = mqdrec;
	    updptr = mqudrec;
	}
	adf_cb = FEadfcb();
	for (i = 0; i < qdef->mqnumatts; i++)
	{
		satt = qdef->mqatts[i];
		dbv = &satt->dbv;
		if (((satt->jpart == 0) || (satt->formfield[0] == EOS)) &&
			(satt->keyinfo == mqISNOTKEY))
			continue;
		/*
		** Check for SQL keywords in column names, range variables, 
		** and table names, but only pre-6.5. Starting with 6.5,
		** SQL keywords ARE allowed, as delimited identifiers!
		*/
		if  (STcompare(IIUIcsl_CommonSQLLevel(),UI_LEVEL_65) < 0)
		{
		    if (IIUGIsSQLKeyWord(satt->col))
		    {
			_VOID_ 
			IIUGerr(E_QF015D_ColIsSQLKeyWord, 0, 2, 
				    (PTR)IIQFRvToTabName(qdef, satt->rvar),
				    (PTR)satt->col);
			Qcode = 0;
			return(NULL);
		    }
		    if (IIUGIsSQLKeyWord(satt->rvar))
		    {
			_VOID_ 
			IIUGerr(E_QF015E_RvarIsSQLKeyWord, 0, 2, 
				    (PTR)IIQFRvToTabName(qdef, satt->rvar),
				    (PTR)satt->rvar);
			Qcode = 0;
			return(NULL);
		    }
		    if (IIUGIsSQLKeyWord(IIQFRvToTabName(qdef, satt->rvar)))
		    {
			_VOID_ 
			IIUGerr(E_QF015F_TabIsSQLKeyWord, 0, 2, 
				    (PTR)IIQFRvToTabName(qdef, satt->rvar),
				    (PTR)IIQFRvToTabName(qdef, satt->rvar));
			Qcode = 0;
			return(NULL);
		    }
		} /* end if pre-6.5 */
		if ((code == 2) || ((code == 0) && (satt->ttype ==0))
				|| ((code == 1) && (satt->ttype == 1)))
		{
		    /* first deal with padding for byte alignment*/
		    if ((rval = afe_pad(adf_cb, recptr,
			dbv->db_datatype, &pad)) != OK)
		    {
			FDerror(PADDING_ERROR, 0);
			return NULL;
		    }
		    updptr += pad;
		    recptr += pad;
		    recl += pad;
		    dbv->db_data = (PTR)updptr;
		    /*
		    ** The mq[12]args arrays point to the retrieve
		    ** buffer, recptr, via the dbdvarray of
		    ** DB_DATA_VALUES.	These mq[12]arrays are pointed
		    ** at by a QDESC structure.
		    ** This is different from the mq[md]uargs arrays.
		    ** They point to the update buffer, updptr, via the
		    ** DB_DATAVALUES in the ATTRIBINFO structure.
		    ** These ma[md]uargs are pointed at by a MQQDEF
		    ** structure, which also points at RELINFO and
		    ** ATTRIBINFO structures.
		    */
		    /* 
		    ** don't forget the db_prec for decimal!!
		    ** fixes bug 51508 (daver)
		    */
		    rdbv = satt->rdbv = &dbdvarray[argnum+dbdvoffset];
		    rdbv->db_datatype = dbv->db_datatype;
		    rdbv->db_length = dbv->db_length;
		    rdbv->db_prec = dbv->db_prec;
		    rdbv->db_data = (PTR)recptr;
		    /*  store DBDV pointers for QG SELECT processing */
		    if (code != 1)
		    {
			mq1qargv[argnum].qv_dbval = rdbv;
			mq1qargv[argnum].qv_name = satt->col;
		    }
		    else
		    {
			mq2qargv[argnum].qv_dbval = rdbv;
			mq2qargv[argnum].qv_name = satt->col;
		    }
		    /* save table name for later key cross reference checking */
		    tbl_xref[argnum] = STalloc(satt->rvar);
		    STcopy(satt->rvar, tbl_xref[argnum]);
		    argnum++;
		    recl += dbv->db_length;
		    recptr += dbv->db_length;
		    updptr += dbv->db_length;
		    IIUGxri_id(satt->rvar, unrml_buf);
                    rcomma += STlcopy(unrml_buf, rcomma, 255);
		    *rcomma++ = '.';
		    IIUGxri_id(satt->col, unrml_buf);
		    rcomma += STlcopy(unrml_buf, rcomma, 255);
		    *rcomma++ = ',';
		    /*
		    **	ALL keys must be part of the SELECT lists created
		    **	for QG.  Is some cases, the key will not appear on
		    **	the form itself.  The code below checks to see if the
		    **	current attribute being processed is on the form and
		    **	if it is, it is added to the appropriate putform and
		    **	getform lists.
		    */
		    if ((satt->jpart != 0) && (satt->formfield[0] != EOS)
					   && (satt->lkeyinfo == 0))
		    {
                        pcomma += STlcopy(satt->formfield, pcomma, 255);
			pcomma += STlcopy(ERx("=%a,"), pcomma, 255);
			gcomma += STlcopy(ERx("%a="), gcomma, 255);
			gcomma += STlcopy(satt->formfield, gcomma, 255);
                        *gcomma++ = ',';
			if (code != 1)
			{
			    mq1args[pargnum] = (PTR)rdbv;
			    mqmuargs[pargnum] = (PTR)dbv;
			}
			else
			{
			    mq2args[pargnum] = (PTR)rdbv;
			    mqduargs[pargnum] = (PTR)dbv;
			}
			pargnum++;
		    }
		}
	}
	/*
	** for joins not displayed on form, have their db_data point
	** to the same place in the record as the displayed part of
	** the join points to.
	*/
	mqsetjoin(qdef);
	next_qargv = argnum;
	largnum = pargnum;
	/* retrieve tid or keys too */
	if (!any_views && (!IIQFnkuNoKeyForUpdates || !IIQFnkaNoKeyForAppends))
	{
	    for (i = 0; i < qdef->mqnumtabs; i++)
	    {
		tabl = qdef->mqtabs[i];
		if ((code == 2) || ((code == 0) && (tabl->mqtype == 0))
			    	|| ((code == 1) && (tabl->mqtype == 1)))
		{
		    /*
		    **	Space will always be allocated for TIDs, even if
		    **	they are not required/used.  It was easier to do this
		    **	and it avoids the possibility of trying to resolve
		    **	null pointers in other QBF procedures.
		    */
		    tabl->mqmtid.db_datatype = DB_INT_TYPE;
                    if (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_902) >= 0)
		    	tabl->mqmtid.db_length = sizeof(i8);
                    else
		    	tabl->mqmtid.db_length = sizeof(i4);
		    if ((rval = afe_pad(adf_cb, recptr,
			DB_INT_TYPE, &pad)) != OK)
		    {
			FDerror(PADDING_ERROR, 0);
			return NULL;
		    }
		    recptr += pad;
		    updptr += pad;
		    recl += pad;
		    tabl->mqmtid.db_data = (PTR)recptr;
                    if (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_902) >= 0)
                    {
		        updptr += sizeof(i8);
		        recptr += sizeof(i8);
		        recl += sizeof(i8);
                    }
                    else
                    {
		        updptr += sizeof(i4);
		        recptr += sizeof(i4);
		        recl += sizeof(i4);
                    }
		    /*
		    **	For each table being processed, it is necessary to keep
		    **	track of where, if any, hidden key columns are placed.
		    **	first_key points to the next location within mq[12]args
		    **	at which a hidden key column will be pointed.  first_key
		    **	is passed as a parameter to mqsetupd.
		    */
		    first_key = pargnum;
		    if (tabl->keytype == mqUSETID)
		    {
		    	IIUGxri_id(tabl->mqrangevar, unrml_buf);
                        rcomma += STlcopy(unrml_buf, rcomma, 255);
			rcomma += STlcopy(ERx(".tid,"), rcomma, 255);
			if (code == 2 || code == 0)
			{
			    mq1qargv[next_qargv].qv_dbval = &tabl->mqmtid;
			    mq1args[pargnum] = (PTR)&tabl->mqmtid;
			    mq1qargv[next_qargv].qv_name = ERx("tid");
			}
			else if (code == 1)
			{
			    mq2qargv[next_qargv].qv_dbval = &tabl->mqmtid;
			    mq2args[pargnum] = (PTR)&tabl->mqmtid;
			    mq2qargv[next_qargv].qv_name = ERx("tid");
			}
			next_qargv++;
			pargnum++;
			if ((tblfield) && ((code == 2) ||
				((code == 1) && (tabl->mqtype == 1))))
			{
			    tnum++;
			    if (tnum > MQ_MAXHIDECOLS)
			    {
				i4	max_cols = MQ_MAXHIDECOLS;
			        IIUGerr(E_QF014A_max_hidecol_hit,
				    0, 1, &max_cols);
			        return NULL;
			    }
			    /* set up GETFORM and PUTFORM strings for TID */
                            STprintf(tbuf, ERx("tid%d"), tnum);
			    pcomma += STlcopy(tbuf, pcomma, 255);
			    pcomma += STlcopy(ERx("=%a,"), pcomma, 255);
			    gcomma += STlcopy(ERx("%a="), gcomma, 255);
                            gcomma += STlcopy(tbuf, gcomma, 255);
			    *gcomma++ = ',';
			    tcomma += STlcopy(tbuf, tcomma, 255);
                            if (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_902) >= 0)
			    	tcomma += STlcopy(ERx("=i8,"), tcomma, 255);
                            else
			    	tcomma += STlcopy(ERx("=i4,"), tcomma, 255);
			}
		    	if (code != 1)
			    mqmuargs[largnum++] = (PTR)&tabl->mqmtid;
			else
			    mqduargs[largnum++] = (PTR)&tabl->mqmtid;
		    }
		    else
		    {
			    /*
			    **	If the current attribute being processed belongs
			    **	to a table that requires a TID, then a hidden
			    **	column TID is created.  Otherwise, a hidden
			    **	field is created for EACH column required for
			    **	the key to the table.
			    */
				/*
				**  If a keyed table is used as part of a
				**  TableField, we must locate all columns
				**  that make up the key.  The attribute list
				**  will be scanned for attributes that belong
				**  to the current table being processed and
				**  that are flagged as being a key.
				*/
				for (j = 0; j < qdef->mqnumatts; j++)
				{
				    satt = qdef->mqatts[j];
				    if ((STcompare(tabl->mqrangevar,
					satt->rvar) == 0) &&
					(satt->keyinfo == mqISKEY))
				    {
					/*
					**  Now that a key column has been 
					**  located, we must find which
					**  argument within the QG SELECT list
					**  corresponds to this key field.
					**  This is necessary for cross-
					**  referencing columns read from the
					**  database to hidden columns.
					*/
					for (loc = 0; loc < argnum; loc++)
					{
					    if (code != 1)
					    {
    						if (STcompare(satt->col,
						    mq1qargv[loc].qv_name) == 0)
						{
						    if (STcompare(satt->rvar,
							tbl_xref[loc]) == 0)
							    break;
						}
					    }
					    else
					    {
						if (STcompare(satt->col,
						    mq2qargv[loc].qv_name) == 0)
						{
						    if (STcompare(satt->rvar,
							tbl_xref[loc]) == 0)
							    break;
						}
					    }
					}
					if (code != 1)
					{
					    key1_xref[next1_xref][0] = loc;
					    key1_xref[next1_xref][1] = pargnum;
					    next1_xref++;
					}
					else
					{
					    key2_xref[next2_xref][0] = loc;
					    key2_xref[next2_xref][1] = pargnum;
					    next2_xref++;
					}
					if (afe_tyoutput(adf_cb, satt->rdbv,
						&user_type) != OK)
					{
					    FEafeerr(adf_cb);
					    return(NULL);
					}
				    if ((tblfield) && ((code == 2) ||
					((code == 1) && (tabl->mqtype == 1))))
				    {
					/* add unique number to hidden field */
					tnum++;
			    		if (tnum > MQ_MAXHIDECOLS)
			    		{
					    i4	max_cols = MQ_MAXHIDECOLS;
			        	    IIUGerr(E_QF014A_max_hidecol_hit,
				    	    	0, 1, &max_cols);
			        	    return NULL;
			    		}
					STprintf(kbuf, ERx("ii_h%d"), tnum);
					/* add hidden field to putform */
                                        pcomma += STlcopy(kbuf, pcomma, 255);
					pcomma += STlcopy(ERx("=%a,"), pcomma,
						255);
					/* add hidden field to getform */
                                        gcomma += STlcopy(ERx("%a="), gcomma,
						255);
					gcomma += STlcopy(kbuf, gcomma, 255);
					*gcomma++ = ',';
					/*  save hidden fields for inittable */
                                        tcomma += STlcopy(kbuf, tcomma, 255);
					*tcomma++ = '=';
					tcomma += STlcopy(user_type.dbut_name,
						tcomma, 255);
                                        tcomma += STlcopy(ERx(" WITH NULL,"),
						tcomma, 255);
				    }
					/* grab a DBDV and initialize it */
					kdbv = &dbdvarray[dbdvoffset + argnum];
					argnum++;
					if (user_type.dbut_kind == DB_UT_NULL)
					{
					    kdbv->db_datatype = satt->rdbv->db_datatype;
					    kdbv->db_length = satt->rdbv->db_length;
					}
					else
					{
					    kdbv->db_datatype = -(satt->rdbv->db_datatype);
					    kdbv->db_length = satt->rdbv->db_length + 1;
					}
					/* 
					** copy over the prec too, just for fun!
					** should prevent further decimal bug, related to
					** bug 51508 (daver)
					*/
					kdbv->db_prec = satt->rdbv->db_prec;
					/* padding for byte alignment */
					if ((rval = afe_pad(adf_cb, recptr,
					    dbv->db_datatype, &pad)) != OK)
					{
					    FDerror(PADDING_ERROR, 0);
					    return NULL;
					}
					updptr += pad;
					recptr += pad;
					recl += pad;
					kdbv->db_data = (PTR)updptr;
					recl += kdbv->db_length;
					recptr += kdbv->db_length;
					updptr += kdbv->db_length;
					/* update [put|get]form arg lists */
					if (code != 1)
					{
					    mqmuargs[largnum++] = (PTR)kdbv;
					    mq1args[pargnum++] = (PTR)kdbv;
					}
					else
					{
					    mqduargs[largnum++] = (PTR)kdbv;
					    mq2args[pargnum++] = (PTR)kdbv;
					}
				    }
				}
		    }
		    /* now set up param strings for writes to db */
		    mqsetupd(tabl, qdef, code, first_key);
		}
	    }
	}
	if (code != 1)
	{
	    mq1qargv[next_qargv].qv_dbval = NULL;
	    mq1qargv[next_qargv].qv_name = NULL;
	}
	else
	{
	    mq2qargv[next_qargv].qv_dbval = NULL;
	    mq2qargv[next_qargv].qv_name = NULL;
	}
	/*
	**  If this is a MD joindef, then for each logical key in the master,
	**  we arrange to copy the logical key value from the "retrieve" DBV
	**  to the "update" DBV.  This will ensure that any details
	**  appended to this master will get the same logical key
	**  value for any detail user-maintained logical key that's
	**  joined to the master logical key.
	**  (The joined columns' "update" DBV's will have the same db_data).
	*/
	if (qdef_type == 0)
	{
		for (i = 0; i < qdef->mqnumatts; i++)
		{
			satt = qdef->mqatts[i];
			if ((satt->lkeyinfo != 0) &&
				(satt->ttype == mqIN_MASTER))
			{
				IIQFmla_MLkeyInMDArray[IIQFmlc_MLkeyInMDCount++]
					= satt;
			}
		}
	}
	/* getform needs to have _RECORD and _STATE info in param */
	if (tblfield && (!any_views) &&
		(!IIQFnkuNoKeyForUpdates || !IIQFnkaNoKeyForAppends))
	{
		if ((code != 1) && (qdef_type ==1))
		{
			/* add 'iinewrow' to list of hidden columns */
                        tcomma += STlcopy(ERx("iinewrow = i2"), tcomma, 255);
			/* add a hidden column for iinewrow flag*/
                        pcomma += STlcopy(ERx("iinewrow=%i2,"), pcomma, 255);
			gcomma += STlcopy(
				ERx("%i2=iinewrow,%i4=_RECORD,%i4=_STATE,"),
				gcomma, 255);
			mqmuargs[largnum++] = (PTR)(&(mqrowstat));
			mq1args[pargnum++] = (PTR)(&(mqrowstat));
			mqmuargs[largnum++] = (PTR)&mq_record;
			mqmuargs[largnum++] = (PTR)&mqrstate;
		}
		else if ((code == 1) && (qdef_type == 0))
		{
			/* add 'iinewrow' to list of hidden columns */
			tcomma += STlcopy(ERx("iinewrow = i2"), tcomma, 255);
			/* add a hidden column for iinewrow flag*/
                        pcomma += STlcopy(ERx("iinewrow=%i2,"), pcomma, 255);
			gcomma += STlcopy(
				ERx("%i2=iinewrow,%i4=_RECORD,%i4=_STATE,"),
				gcomma, 255);
			mqduargs[largnum++] = (PTR)&mqrowstat;
			mq2args[pargnum++] = (PTR)&mqrowstat;
			mqduargs[largnum++] = (PTR)&mq_record;
			mqduargs[largnum++] = (PTR)&mqrstate;
		}
	}
	dbdvoffset = argnum;
	/* Build the FROM list for QG.  This is required by QG. */
	for (i = 0; i < qdef->mqnumtabs; i++)
	{
	    tabl = qdef->mqtabs[i];
	    if ((code != 1) || ((code == 1) && (tabl->mqtype == 1)))
	    {
		/*
		** For 6.5 and beyond, it doesn't matter if we're using
		** a correlation name (mqrangevar) or not (we know if we are
		** if it differs from the table (mqrelid) name). Given the
		** 6.4 from clauses
		**
		**	FROM table1, table2	
		**	FROM table1 t1, table2 t2
		**
		** in 6.5 this will have to become
		**
		**	FROM owner1.table1 table1, owner2.table2 table2
		**	FROM owner1.table1 t1, owner2.table2 t2
		**
		** This is because we must always prefix the column name
		** with the the rangevar/tablename (i.e., t1.col1) in case
		** the column appears in more than one table in the query
		*/
		if  (IIUIdlmcase() != UI_UNDEFCASE)
		{
			/* 
			** Furthermore, since any or all of mqrelown
			** (owner), mqrelid (table name), or mqrangevar
			** (range variable) *could* be delimited
			** identifiers, we must now requote them before
			** sending to the database. 
			*/
			IIUGxri_id(tabl->mqrelown, unrml_buf);
		    	fcomma += STlcopy(unrml_buf, fcomma, 255);
			*fcomma++ = '.';
			IIUGxri_id(tabl->mqrelid, unrml_buf);
		    	fcomma += STlcopy(unrml_buf, fcomma, 255);
			*fcomma++ = ' ';
			IIUGxri_id(tabl->mqrangevar, unrml_buf);
			fcomma += STlcopy(unrml_buf, fcomma, 255);
			*fcomma++ = ',';
		}
		else
		{
			/* 
			** the pre-6.5 code fragment -- 6.4 and earlier
			** will not accept owner.tablename!!
			*/
			if (!STequal(tabl->mqrelid, tabl->mqrangevar))
			{
		    		fcomma += STlcopy(tabl->mqrelid, fcomma, 255);
		    		*fcomma++ = ' ';
			}
			fcomma += STlcopy(tabl->mqrangevar, fcomma, 255);
			*fcomma++ = ',';
		}
	    }
	}
	if (code != 1)
	{
	    fcomma += STlcopy(ERx("XPADX"), fcomma, 255);
	}
	if (rcomma != rparam)
	{
		*(--rcomma) = EOS;
	}
	if (pcomma != pparam)
	{
		*(--gcomma) = EOS;
		*(--pcomma) = EOS;
	}
        *tcomma = EOS;
        *(--fcomma) = EOS;
	/* now allocate space for the from QRY_SPECs */
	{
	    i4  n = 0;
	    s_qdesc->qg_from = (QRY_SPEC (*)[])FEreqmem((u_i4)qdef->mqtag,
		(u_i4)(2 * sizeof(QRY_SPEC)), FALSE, (STATUS *)NULL);
	    qs_text(qdef->mqtag, &n, s_qdesc->qg_from, fparam);
	    qs_end(qdef->mqtag, n, s_qdesc->qg_from);
	}
	/* now allocate space for the tlist QRY_SPECs */
	{
	    i4  n = 0;
	    if (rcomma != rparam)
	    {
	    	s_qdesc->qg_tlist = (QRY_SPEC (*)[])FEreqmem((u_i4)qdef->mqtag,
			(u_i4)(2 * sizeof(QRY_SPEC)), FALSE, (STATUS *)NULL);
	    	qs_text(qdef->mqtag, &n, s_qdesc->qg_tlist, rparam);
	    	qs_end(qdef->mqtag, n, s_qdesc->qg_tlist);
	    }
	    else
	    {
		s_qdesc->qg_tlist = (QRY_SPEC (*)[])NULL;
	    }
	}
	if (code != 1)
	{
		qdef->mqmpform = STalloc(pparam);
		qdef->mqmrecl = recl;
		qdef->mqmgform = STalloc(gparam);
		qdef->mqmgargv = (char **)mqmuargs;
		s_qdesc->qg_argv = (QRY_ARGV(*)[]) mq1qargv;
		qdef->mqmpargv = mq1args;
	}
	else
	{
		if (rcomma != rparam)
		{
			qdef->mqdpform = STalloc(pparam);
			qdef->mqdrecl = recl;
			qdef->mqdgform = STalloc(gparam);
			qdef->mqdgargv = (char **)mqduargs;
			s_qdesc->qg_argv = (QRY_ARGV(*)[]) mq2qargv;
			qdef->mqdpargv = mq2args;
		}
	}
	return(s_qdesc);
}
/*
** mqsetjoin() - Routine to set the db_data pointers
**		 up for the subordinate join parts.
*/
mqsetjoin(qdef)
MQQDEF	*qdef;
{
	ATTRIBINFO	*j1att, *j2att;
	i4		i;
	for (i = 0; i < qdef->mqnumatts; i++)
	{
		j1att = qdef->mqatts[i];
		if (j1att->jpart != 0)
		{
			continue;
		}
		j2att = j1att->join_att;
		j1att->dbv.db_data = j2att->dbv.db_data;
		j1att->dbv.db_length = j2att->dbv.db_length;
		j1att->dbv.db_datatype = j2att->dbv.db_datatype;
		j1att->dbv.db_prec = j2att->dbv.db_prec;
		j1att->rdbv = j2att->rdbv;
		/* bug 14072, 12969
		** make sure mdtype is set correctly
		** (esd 10/10/90) This statement shouldn't be necessary anymore,
		** but I leave it in out of paranoia.
		*/
		j1att->mdtype = j2att->mdtype;
	}
}
/*
** mqsetupd  - Routine to set up the param strings and arg
**	       vectors for a table for its param appends,
**	       and updates.  This routine is called once
**	       for each table in the qdef.
*/
mqsetupd(tbl, qdef, code, next_key)
RELINFO *tbl;
MQQDEF	*qdef;
i4	code;
i4	next_key;
{
	DB_DATA_VALUE	**uarg;
	DB_DATA_VALUE	**aarg;
	DB_DATA_VALUE	*dbtemp[MQ_MAXCOLS + 2];
	char		vparm[MQ_MAXCOLS * 3];
	char		*acomma;
	char		*ucomma;
	char            *wcomma;
	char		*wend;
	char		*vcomma;
	char		*avalmark;
	i4		aargnum, uargnum;
	i2		urecl;
	i4		acntr;
	i4		dbcount;
	i4		i;
	ATTRIBINFO	*attrib;
  char formname[FE_MAXNAME+1];
  char tablename[FE_MAXNAME+1];
  char fieldname[FE_MAXNAME+1];
  char fieldmode[10];
	uparm[0] = '\0';
	aparm[0] = '\0';
	wparm[0] = '\0';
	vparm[0] = '\0';
	aargnum = 0;
	uargnum = 0;
	urecl = 0;
	acomma = aparm;
	ucomma = uparm;
	vcomma = vparm;
	wcomma = wend = wparm;
	dbcount = 0;
	/* allocate space for argument vectors */
	if ((aarg = (DB_DATA_VALUE **)MEreqmem((u_i4)0,
	    (u_i4)((MQ_MAXCOLS + 2) * sizeof(DB_DATA_VALUE *)),
	    TRUE, (STATUS *)NULL)) == NULL)
	{
		/* don't we need an exception handler here? */
		FDerror(ERRALLOC, 0, ERx(""));
		return;
	}
	if ((uarg = (DB_DATA_VALUE **)MEreqmem((u_i4)0,
	    (u_i4)((MQ_MAXCOLS + 2) * sizeof(DB_DATA_VALUE *)),
	    TRUE, (STATUS *)NULL)) == NULL)
	{
		/* don't we need an exception handler here? */
		FDerror(ERRALLOC, 0, ERx(""));
		return;
	}
	STcopy(mq_frame, formname);
	STcopy(mq_tbl, tablename);
	*acomma++ = '(';
	*vcomma++ = '(';
	/*
	**  If TIDs are being used, set the WHERE clause for this table
	**  appropriately.  Also, store the pointer to the DBDV for the
	**  TID in dbtemp which will later be appended to the argument list
	**  for the WHERE clause in dynamic SQL UPDATEs and DELETEs.
	*/
	if (tbl->keytype == mqUSETID)
	{
	    wcomma += STlcopy(ERx("tid=?"), wcomma, 255);
	    wend = wcomma;
	    dbtemp[dbcount] = &tbl->mqmtid;
	    dbcount++;
	}
	/*
	**  Loop through all attributes in the QDEF;
	**  skip those that aren't in the specified table.
	*/
	for (acntr = 0; acntr < qdef->mqnumatts; acntr++)
	{
		char	col_unrml[FE_UNRML_MAXNAME+1]; /* unormalized ids */
		attrib = qdef->mqatts[acntr];
		IIUGxri_id(attrib->col, col_unrml);
		/* Don't include it if its an unsupported type! */
		if (!IIAFfedatatype(&attrib->dbv))
			continue;
		if ((STcompare(tbl->mqrangevar, attrib->rvar)) != 0)
		{
			continue;
		}
# ifdef DEBUG
		if (mq_debug)
		{
			SIfprintf(mqoutf, ERx("MQSETUPD: attrib->col is %s\n"), attrib->col);
			SIflush(mqoutf);
		}
# endif
		/*
		**	If this attribute is a key field, the column name must
		**	be added to the WHERE clause.
		*/	
		if (attrib->keyinfo == mqISKEY)
		{
			wcomma += STlcopy(col_unrml, wcomma, 255);
			wcomma += STlcopy(ERx("=? AND "), wcomma, 255);
			wend = wcomma - 5;
			/*
			**  If the column is part of a key, the DBDVs
			**  have already been set up in mqsetparam for each
			**  key.  These values will be temporarily copied to
			**  dbtemp before being appended to the UPDATE
			**  argument list.
			*/
			if (code != 1)
			{
			    dbtemp[dbcount] = (DB_DATA_VALUE *)mq1args[next_key];
			}
			else
			{
			    dbtemp[dbcount] = (DB_DATA_VALUE *)mq2args[next_key];
			}
			next_key++;
			dbcount++;
		}
		/*
		**  All that remains to be done with the current attribute
		**  is to see if it needs to participate in appends or updates.
		**  If the attribute has a null formfield (it's not on the form
		**  and it's not a logical key), it certainly doesn't need to.
		*/
		if (attrib->formfield[0] == EOS)
		{
			continue;
		}
		/*
		**  Get default "append value marker".
		*/
		avalmark = ERx("?");
		/*
		**  If the attribute is a logical key,
		**  it must not participate in updates
		**  (its opupdate has been set to 0).
		**
		**  Appends are more complicated:
		**
		**  If the attribute is a system-maintained logical key,
		**  it must not participate in appends.
		**
		**  If the attribute is a user-maintained logical key:
		**
		**  (1) If it's not a join column,
		**	it must not participate in appends.
		**
		**  (2) If it's joined only to other user-maintained logical
		**	keys, it must also not participate in appends.
		**	except in the case where it's a detail user key
		**	being appended to a master user key, in which case
		**	we want to copy the master user key to the detail
		**	user key.  We do this much as we do for case (3) below,
		**	except that we support adding details to a new master
		**	(we know the new master will have a null or default
		**	logical key).
		**
		**  (3) If it's joined to a system-maintained logical key
		**	(either directly or indirectly), then
		**	we set aarg[aargnum] to specify the "update" DBV,
		**	just as we do for other fields to be appended.
		**	This DBV's db_data will be shared with the DBV
		**	for the system-maintained logical key.
		**	Note that the user-maintained logical key's	
		**	join_att field will point to the system-maintained
		**	logical key, even if the join is indirect (by way of
		**	another user-maintained logical key).
		**
		**	If the join is MM or DD, appends will be disallowed
		**	(in this release, at least).
		**
		**	If the join is MD, with the system-maintained
		**	logical key the master (the reverse is disallowed),
		**	appends for the current (detail) table
		**	will, for this release, be allowed only by way of the
		**	update screen (adding details to an existing master).
		**	In this case, the user-maintained logical key
		**	for the detail record(s) we want to append is
		**	the system-maintained logical key for the last 
		**	master record we retrieved.  We copy this logical key
		**	value from the master's system key's "retrieve" DBV
		**	to its "update" DBV whenever we retrieve a new master.
		*/
		if (attrib->lkeyinfo & (mqTABLE_KEY | mqOBJECT_KEY))
		{
			if (attrib->ttype != mqIN_DETAIL)
			{
				continue;
			}
			if (attrib->join_att->ttype != mqIN_MASTER)
			{
				continue;
			}
			/*
			**  At this point, we know that attrib
			**  represents a detail joined to a master.
			**  Therefore, we can infer that:
			**  (1) attrib can't represent a system-maintained
			**      logical key (we disallow such joins), and
			**  (2) attrib->jpart == 0.
			*/
			if (attrib->lkeyinfo & mqTABLE_KEY)
			{
				avalmark = ERx("table_key(?)");
			}
			else
			{
				avalmark = ERx("object_key(?)");
			}
		}
		/*
		**  Get the attribute's formfield name and mode.
		*/
		STcopy(attrib->formfield, fieldname);
		if (attrib->jpart >= 0)
		{
			STcopy(ERx("FILL"), fieldmode);
		}
		else if (attrib->intbl)
		{
/* # line 1923 "mqqdesc.qsc" */	/* inquire_frs */
  {
    if (IIiqset(4,0,formname,tablename,(char *)0) != 0) {
      IIiqfsio((short *)0,1,32,9,fieldmode,21,fieldname,0);
    } /* IIiqset */
  }
/* # line 1925 "mqqdesc.qsc" */	/* host code */
		}
		else
		{
/* # line 1928 "mqqdesc.qsc" */	/* inquire_frs */
  {
    if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,32,9,fieldmode,21,fieldname,0);
    } /* IIiqset */
  }
/* # line 1930 "mqqdesc.qsc" */	/* host code */
		}
		/*
		**  ALWAYS place column in target string when doing APPEND
		**  -- BUG # 6381
		*/
		acomma += STlcopy(col_unrml, acomma, 255);
		*acomma++ = ',';
		vcomma += STlcopy(avalmark, vcomma, 255);
		*vcomma++ = ',';
		aarg[aargnum] = &attrib->dbv;
		aargnum++;
		/*
		**  If the rules allow updates, 
		**  and the field is in FILL mode,
		**  then arrange for the field to
		**  participate in updates.
		*/
		if (attrib->opupdate)
		{
			if (STcompare(fieldmode,ERx("FILL")) == 0)
			{
				ucomma += STlcopy(col_unrml, ucomma, 255);
				ucomma += STlcopy(ERx("=?,"), ucomma, 255);
				uarg[uargnum] = &attrib->dbv;
				uargnum++;
			}
		}
	}
	*(--vcomma) = ')';
	*(++vcomma) = EOS;
	--acomma;
	acomma += STlcopy(ERx(") VALUES "), acomma, 255);
	acomma += STlcopy(vparm, acomma, MQ_MAXATTS * 3);
	*acomma++ = EOS;
	*(--ucomma) = EOS;
	*wend = EOS;
	/* DELETE WHERE clause is identical to WHERE clause for UPDATEs */
	tbl->mqdargv = &uarg[uargnum];
	/*  Append DB DATA VALUE pointers to UPDATE argument list */
	for (i = 0; i < dbcount; i++)
	{
	    uarg[uargnum] = dbtemp[i];
	    uargnum++;
	}
	/* now allocate space for the param strings and arg vectors */
	tbl->mquparam = STalloc(uparm);
	tbl->mqaparam = STalloc(aparm);
	tbl->mqdparam = STalloc(wparm);
	tbl->mquargv = uarg;
	tbl->mqaargv = aarg;
	tbl->mqrecl = urecl;
}
/*
** mqsetwhere() - Routine to set up the where
**		  clauses/param strings for the
**		  query.
*/
static
mqsetwhere(qdef)
MQQDEF	*qdef;
{
	static DB_DATA_VALUE	*mqwargs[MQ_MAXJOINS];
	JOININFO	*ajoin;
	ATTRIBINFO	*att;
# ifndef PMFE
	QRY_SPEC	mwclause[MQ_MAXQRYSPEC];
	QRY_SPEC	dwclause[MQ_MAXQRYSPEC];
	QRY_SPEC	valclause[MQ_MAXQRYSPEC];
# endif
	QRY_SPEC	*clause;
	QRY_SPEC	(*qsval)[];
	DB_DATA_VALUE	*dbv;
	i4		nmw, ndw, nvw, *anxw;
	bool		firstmj;
	bool		firstdj;
	i4		attnum;
	i4		j;
	i4		mtot;
	MQQG_GEN	gen;
# ifdef PMFE
	/* Take the xxclause variables off the stack. */
	QRY_SPEC	*mwclause;
	QRY_SPEC	*dwclause;
	QRY_SPEC	*valclause;
	u_i4		memsize = sizeof(QRY_SPEC) * MQ_MAXQRYSPEC * 3;
	TAGID		memtag;
	QRY_SPEC	*memptr;
	/* Get the memory tag. */
	memtag = FEgettag ();
	/* Allocate the memory for mwclause, dwclause and valclause. */
	memptr = (QRY_SPEC *) FEreqmem ((u_i4)memtag,
			memsize, (bool)1, (STATUS *) NULL);
	/* Set the pointers for the xxclause variables. */
        mwclause = memptr;
        dwclause = memptr + MQ_MAXQRYSPEC;
        valclause = memptr + MQ_MAXQRYSPEC*2;
#endif
	/* initialize flags and param strings */
	firstmj = TRUE;
	firstdj = TRUE;
	nmw = ndw = nvw = 0;
	/* for each join, put it in the appropriate clause (master or detail)*/
	for (j = 0; j < qdef->mqnumjoins; j++)
	{
	    ajoin = qdef->mqjoins[j];
	    if ((STcompare(ajoin->jcode, ERx("MM"))) == 0)
	    {
		if (firstmj)
		    firstmj = FALSE;
		else
		    qs_text(qdef->mqtag, &nmw, mwclause, ERx(" and "));
		mqjoincat(qdef->mqtag, &nmw, mwclause, ajoin, 0);
	    }
	    else if ((STcompare(ajoin->jcode, ERx("DD"))) ==0)
	    {
		if (qdef_type == 1)
		{
		    if (firstmj)
			firstmj = FALSE;
		    else
			qs_text(qdef->mqtag, &nmw, mwclause, ERx(" and "));
		    mqjoincat(qdef->mqtag, &nmw, mwclause, ajoin, 0);
		}
		else
		{
		    if (firstdj)
			firstdj = FALSE;
		    else
			qs_text(qdef->mqtag, &ndw, dwclause, ERx(" and "));
		    mqjoincat(qdef->mqtag, &ndw, dwclause, ajoin, 0);
		}
	    }
	    else if (((STcompare(ajoin->jcode, ERx("MD")))==0) || ((STcompare(ajoin->jcode, ERx("DM")))==0))
	    {
		char	*master_rng;
		char	*master_col;
		char	detail_rng[FE_UNRML_MAXNAME+1];
		char	detail_col[FE_UNRML_MAXNAME+1];
		char	buf[FE_MAXNAME*2+8];
		if ((STcompare(ajoin->jcode, ERx("MD"))) == 0)
		{
		    master_rng = ajoin->rv1;
		    master_col = ajoin->col1;
		    IIUGxri_id(ajoin->rv2, detail_rng);
		    IIUGxri_id(ajoin->col2, detail_col);
		}
		else
		{
		    master_rng = ajoin->rv2;
		    master_col = ajoin->col2;
		    IIUGxri_id(ajoin->rv1, detail_rng);
		    IIUGxri_id(ajoin->col1, detail_col);
		}
		attnum = mqfindatt(qdef, master_rng, master_col);
		att = qdef->mqatts[attnum];
		dbv = att->rdbv;
		if (AFE_NULLABLE_MACRO(dbv->db_datatype))
		{
		    /*
		    **  If the datatype is nullable, this part of the join
		    **	clause will become part of a VALGEN (NO SAVE) QRY_SPEC.
		    **  No 'AND' text is need here.  It will get added when
		    **  IIQFbwv_buildVALGEN is called.
		    */
		    anxw = &nvw;
		    clause = valclause;
		}
		else
		{
		    if (firstdj)
		    {
		        firstdj = FALSE;
		    }
		    else
		    {
		        qs_text(qdef->mqtag, &ndw, dwclause, ERx(" and "));
		    }
		    anxw = &ndw;
		    clause = dwclause;
		}
		/* 
		** This is the place that the DETAIL table and column
		** names are added to the where clause. We needed to call
		** IIUGxri_id() to add the quotes, if necessary; this
		** was done when the variables were initialized...
		*/
		if (att->lkeyinfo != 0)
		{
		    STprintf(buf, ERx("char(%s.%s)="), detail_rng, detail_col);
		}
		else
		{
		    STprintf(buf, ERx("%s.%s="), detail_rng, detail_col);
		}
		qs_text(qdef->mqtag, anxw, clause, buf);
		qs_value(qdef->mqtag, anxw, clause, dbv);
	    }
	}
	/*
	** Add the generator clause for the where clause to
	** be built from the user's form input.
	** If there are clauses in the master add an "and"
	** first.
	*/
	if (nmw != 0)
	    qs_text(qdef->mqtag, &nmw, mwclause, ERx(" and "));
	gen.mqqg_where = MQQG_MASTER;
	gen.mqqg_qdef = qdef;
	qs_gen(qdef->mqtag, &nmw, mwclause, &gen);
	/* allocate space for master where clause QRY_SPEC ptrs */
	mtot = 0;
	qdef->mqqdesc->qg_where = (QRY_SPEC (*)[])FEreqmem((u_i4)qdef->mqtag,
	    (u_i4)((nmw+1) * sizeof(QRY_SPEC)), FALSE, (STATUS *)NULL);
	qs_add(&mtot, qdef->mqqdesc->qg_where, nmw, mwclause);
	qs_end(qdef->mqtag, mtot, qdef->mqqdesc->qg_where);
	if (qdef_type ==0)
	{
	    i4  dtot = 0;
	    i4  vtot = 0;
	    /*
	    ** Add the generator clause for the detail query.
	    */
	    if (nvw != 0)
	    {
	        qsval = (QRY_SPEC (*)[])FEreqmem((u_i4)qdef->mqtag,
		    (u_i4)((nvw+1) * sizeof(QRY_SPEC)), FALSE, (STATUS *)NULL);
	        qs_add(&vtot, qsval, nvw, valclause);
	        qs_end(qdef->mqtag, vtot, qsval);
	        if (ndw != 0)
		    qs_text(qdef->mqtag, &ndw, dwclause, ERx(" and "));
	        IIQFqsv_QSvalgen(qdef->mqtag, &ndw, dwclause, qsval);
	    }
	    if (ndw != 0)
		qs_text(qdef->mqtag, &ndw, dwclause, ERx(" and "));
	    gen.mqqg_where = MQQG_DETAIL;
	    gen.mqqg_qdef = qdef;
	    qs_gen(qdef->mqtag, &ndw, dwclause, &gen);
	    /* allocate space for detail where clause QRY_SPEC ptrs */
	    qdef->mqqdesc->qg_child->qg_where =
		(QRY_SPEC (*)[])FEreqmem((u_i4)qdef->mqtag,
		(u_i4)((ndw+1) * sizeof(QRY_SPEC)), FALSE, (STATUS *)NULL);
	    qs_add(&dtot, qdef->mqqdesc->qg_child->qg_where, ndw, dwclause);
	    qs_end(qdef->mqtag, dtot, qdef->mqqdesc->qg_child->qg_where);
	}
# ifdef PMFE
	FEfree (memtag);
#endif
	return(TRUE);
}
static i4
_buf_size(qdef, m_or_d)
MQQDEF	*qdef;
i4	m_or_d;
{
    ATTRIBINFO	    *satt;
    DB_DATA_VALUE   *dbv;
    i4		    i;
    i4		    bufsize;
    for (bufsize = 0, i = 0; i < qdef->mqnumatts; i++)
    {
	satt = qdef->mqatts[i];
	dbv = &satt->dbv;
	if (satt->ttype == m_or_d)
	{
	    if (satt->keyinfo == mqISKEY)
	    {
	        bufsize += 2 * (dbv->db_length + sizeof(ALIGN_RESTRICT));
	    }
	    else
	    {
	        bufsize += dbv->db_length;
	    }
	}
    }
    bufsize += (qdef->mqnumtabs * sizeof(i4))
	       + (qdef->mqnumatts * sizeof(ALIGN_RESTRICT));
    return (bufsize);
}
