# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h> 
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h> 
# include	<er.h> 
# include <ui.h>
# include <uigdata.h>
/**
** Name:	ferelacc.qc - Relation info access a la cursor.
**
** Description:
**	Presents a generalized cursor-like implementation for access to 
**	relation info.  Since this is based on an EQUEL retrieve loop rather 
**	than a cursor, only one database access may be active at a time.
**
**	This file defines:
**	FErel_access	Allows access to a relation through OPEN, FETCH, and
**			CLOSE operations.
**
** History:
**	29-mar-1986 (rdesmond)
**		first created for Jupiter release.
**	18-jan-1988 (danielt)
**		SQLized for DG project
**	18-aug-1988 (peter)
**		Update catalog column names for 6.1.
**	10-nov-1988 (danielt)
**		took out seperate select loops based on
**		whether details are wanted.  The main concern
**		here is keeping down the number of repeated
**		queries in the DBMS shared query cache (for
**		INGRES)
**	10-oct-91 (seg)
**		Changed "errno" to "errnum" to avoid naming conflict with
**		c runtime library
**	16-jul-1992 (rdrane)
**		Add 6.5 support for owner.tablename, delimited identifiers,
**		and synonyms.
**	19-feb-1996 (pchang)
**		Add dbmsinfo('group') to the qualification of permit_user for
**		the iiaccess catalog in FErel_access() so that 'help' also 
**		displays tables for which the user has been granted access via
**	06-mar-1996 (harpa06)
**		Changed UI_LEVEL_65 to UI_LEVEL_66 for VPS project.
**	29-jan-1997 (hanch04)
**		Changed UI_LEVEL_66 back to UI_LEVEL_65.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*{
** Name:	FErel_access - cursor-like access to any relation.
**
** Description:
**	This function presents a cursor-like abstraction for access relation
**	information.  It is called in one of three modes: OPEN, FETCH
**	or CLOSE.  When called in OPEN mode, a query must be specified.
**	The query parameters are relation name, param target list, param
**	address list, qualification and sort field(s).  When called in
**	OPEN mode, the first tuple is returned.  For each call in FETCH
**	mode, the next tuple is returned or, if the end of the set has been 
**	reached, a flag to that effect.  Note that the end-of-set flag
**	can be set returned from the OPEN operation.
**
** Inputs:
**	mode		The mode in which the function is to operate:
**				{MODE_OPEN | MODE_FETCH | MODE_CLOSE }
**
**	relname   	Qualification string to be used as a query restriction
**			on the namestrings returned.  Any single value string,
**			which will be compared with the relation name may be
**			The matching templates "%", "_" and "\[...\]" may
**			be used for partial match specification (backslash
**			is always the escape character).  Use "%" to indicate
**			that all names are desired.
**
**			The qualification string may not contain an owner
**			specification, and must be otherwise normalized.
**			Ignored for modes other than MODE_OPEN.
**
**	relowner	Qualification string to be used as a query restriction
**			on the owner of the namestrings returned.  Passed
**			through to FErel_access as is.  It may be NULL or an
**			empty string, but must not contain SQL wildcards
**			and must be normalized.  Ignored for pre-6.5 databases
**			and modes other than MODE_OPEN.
**
**	relsource	pointer to an allocated FE_REL_SOURCE structure in
**			which to return the results.
**
**	cats_wanted	Boolean to indicate whether catalogs should be
**			included in the active set.
**
**	status		Pointer to allocated status of query, also set before 
**			return; this field is significant only to this 
**			procedure and should not be interpreted external to it:
**				{ STATUS_CLOSED | STATUS_OPEN | STATUS_DONE }
** Outputs:
**	status		Pointer to allocated status of query, also set before 
**			return; this field is significant only to this 
**			procedure and should not be interpreted external to it:
**				{ STATUS_CLOSED | STATUS_OPEN | STATUS_DONE }
** Returns:
**	OK			successful
**	E_UI0002_EOS		no more tuples in active set
**	E_UI0001_BLOCK_CLOSED	query block is not open (from status)
**	other			dbms error
**
** History:
**	29-mar-1986 rdesmond)
**		first created for Jupiter release.
**	28-jun-1987 (danielt) changed pointer args for recursive
**		call to FErel_access from 0's to NULLs (cast to correct type)
**	08-oct-1987 (rdesmond)
**		removed call to FErel_access to close for open call.
**	02-dec-1987 (rdesmond)
**		rewrote using repeat retrieve statements.
**	05-jan-1988 (rdesmond)
**		took off @'s from IIUIuser and IIUIdba in repeat retrieves;
**		removed EQUEL range statement.
**	18-jan-1988 (danielt)
**		SQLized for DG project
**	2-mar-1988 (danielt)
**		changed IIUIuser and IIUIdba to IIUIsuser and IIUIsdba
**	18-aug-1988 (peter)
**		Update catalog names for 6.1.
**	10-oct-91 (seg)
**		Changed "errno" to "errnum" to avoid naming conflict with
**		c runtime library
**	16-jul-1992 (rdrane)
**		Add 6.5 support for owner.tablename, delimited identifiers,
**		and synonyms.  Extensively reworked the entire routine.
**		Added parameter to optionally specify owner when MODE_OPEN and
**		6.5 database.  Removed the dets_wanted parameter since it was
**		unused and the interface is changing anyway.
**		STtrmwhite() can return an empty string if all whitespace -
**		need to detect this and force the result to be one space.
**		Include mode in the SQL variables so we can get around the
**		ANSI compiler warnings.
**	23-apr-1993 (rdrane)
**		Correct 6.5 level SELECT's so that catalogs are included
**		(cats_wanted == TRUE) only if their names match the supplied
**		pattern (they were being included unconditionally).  Exclude
**		data tables (begin w/ ii but have systenm_use of 'U').  This
**		should be a temporary fix ...
**	22-jul-1993 (rdrane)
**		Effect case translation of UI_FE_CAT_ID_65 prior to use
**		for FIPS support.
**	14-oct-1993 (rdrane)
**		Correct the usage of system_use in the SELECT's so Gateways
**		that do not provide this information will still return the
**		correct list of tables.
**	15-dec-1993 (rdrane)
**		Add PUBLIC to the permit_user qualifications for iiaccess,
**      23-nov-1994 (chech02) bug#64468
**              Add escape '\' in where clause for pre-65 catalogs also.
**		since Gateways return this instead of ' ' (space) for public
**		access.  Note that SQL92 says that an authorization ID of
**		PUBLIC is not valid, and will not be confused with the
**		FIPS case-sensitive delimited ID "public".
**      10-apr-1995 (harpa06)
**		Bug #68007 - Correct the usage of system_use in the SELECT so 
**		that primary keys and references are not visible by the user.
**	19-feb-1996 (pchang)
**		Add dbmsinfo('group') to the qualification of permit_user for
**		the iiaccess catalog so that 'help' also displays tables for
**		which the user has been granted access via the user's group 
**		(B73528).
**	06-mar-1996 (harpa06)
**		Changed UI_LEVEL_65 to UI_LEVEL_66.
*/
DB_STATUS
FErel_access(mode, relname, relowner, relsource, cats_wanted, status)
  i4 mode;
  char *relname;
  char *relowner;
  FE_REL_SOURCE *relsource;
	bool		cats_wanted;
	i4		*status;
{
	i4		sql_wild;
  i4 errnum;
  bool dummy;
  char *relowner_ptr;
  UIDBDATA *uidbdata;
  char pttrn_name[(FE_MAXNAME + 2 + 1)];
  char ii_pattern_string[4];
  char r65_cat_id[FE_MAXNAME];
/*
** This function must break into and out of the retrieve loop in an 
** unorthodox manner; however, it works.  Unfortunately, the ANSI "C"
** compiler flags each return out of the SELECT LOOP as
**		warning: end-of-loop code not reached
** So, make them conditional on a dummy variable which we'll always set
** TRUE and fool the compiler. Sigh!
*/
    dummy = TRUE;
    uidbdata = IIUIdbdata();
    switch(mode)
    {
	case MODE_FETCH:
		if (*status == STATUS_DONE)
		{
	    		/* No more records */
	    		return(E_UI0002_EOS);
		}
		if (*status == STATUS_CLOSED)
		{
	    		/* illegal request */
	    		return(E_UI0001_BLOCK_CLOSED);
		}
		/*
		** In order to optimize the listpick/help query, we took a look
		** at what the thing was actually returning to its clients. For
		** historical reasons, things like create and alter dates and
		** table version were returned; however, none of this routine's
		** callers actually USE that for 6.5. So, we took it out of the
		** 6.5 query (when you're specifically getting all the tables)
		** to make listpicks and help\g (and the opening frame of
		** tables utility) run fast. This means we need a second entry
		** point, since the query is different; we goto _65fetchdets to
		** get the EXEC SQL END that has the same number of columns
		** returned as the 65 EXEC SQL BEGIN's.
		*/
		if  (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_65) < 0)
		{
   			goto fetchdets;
		}
		else
		{
			goto _65fetchdets;
		}
	case MODE_CLOSE:
		if (*status == STATUS_DONE)
		{
			return (OK);
		}
		else
		{
			goto close;
		}
		break;
	case MODE_OPEN:
		/* All INGRES catalog table names are REGULAR identifiers! */
		STcopy(ERx("ii%"),&ii_pattern_string[0]);
		IIUGdbo_dlmBEobject(&ii_pattern_string[0],FALSE);
		/*
		**	This is the only way to ensure matches on equals and
		**	when relname contains an '_' wildcard as the last
		**	character.  The LIKE operator behaves strangely with
		**	blank-padded char fields, which is how the columns
		**	in the FE catalogs are defined.  
		*/
		STprintf(&pttrn_name[0],"%s %%",relname);
		if  (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_65) < 0)
		{
		    if  (cats_wanted)
		    {
				/* Pre 6.5 WITH ii catalogs */
/* # line 278 "ferelacc.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ferelacc1",6504,18620);
      IIputdomio((short *)0,1,32,0,relname);
      IIputdomio((short *)0,1,32,0,pttrn_name);
      IIputdomio((short *)0,1,32,0,uidbdata->suser);
      IIputdomio((short *)0,1,32,0,uidbdata->sdba);
      IIputdomio((short *)0,1,32,0,uidbdata->catowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"ferelacc1",6504,18620);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, create_date, alter_date, table_type, \
table_version, system_use from iitables where(table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relname);
        IIwritio(0,(short *)0,1,32,0,(char *)
"escape '\\' or table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,pttrn_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"escape '\\')and(table_ow\
ner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,uidbdata->sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,uidbdata->catowner);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by table_name");
        IIexDefine(0,(char *)"ferelacc1",6504,18620);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,relsource->table_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,relsource->table_owner);
      IIgetdomio((short *)0,1,32,25,relsource->create_date);
      IIgetdomio((short *)0,1,32,25,relsource->alter_date);
      IIgetdomio((short *)0,1,32,8,relsource->table_type);
      IIgetdomio((short *)0,1,32,8,relsource->version);
      IIgetdomio((short *)0,1,32,1,relsource->system_use);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 304 "ferelacc.sc" */	/* host code */
					*status = STATUS_OPEN;
/* # line 305 "ferelacc.sc" */	/* inquire_ingres */
      {
        IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
      }
/* # line 307 "ferelacc.sc" */	/* host code */
					if (errnum == OK)
					{
						STtrmwhite(
							relsource->table_name);
						STtrmwhite(
							relsource->table_owner);
						STtrmwhite(
							relsource->create_date);
						STtrmwhite(
							relsource->alter_date);
						STtrmwhite(
							relsource->table_type);
						STtrmwhite(relsource->version);
						STtrmwhite(
							relsource->system_use);
					}
					if  (dummy)
					{
						return(errnum);
					}
			close:
				/*
				** any endretrieve will work, but it must be
				** in a retrieve loop to pass the (dreaded!!)
				** equel preprocessor.  This is the closest
				** retrieve loop so here it is.
				*/
/* # line 334 "ferelacc.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
/* # line 335 "ferelacc.sc" */	/* host code */
			fetchdets:
					;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
				break;
		    }
		    else
		    {
				/* Pre 6.5 WITHOUT ii catalogs */
/* # line 343 "ferelacc.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ferelacc2",6505,17470);
      IIputdomio((short *)0,1,32,0,relname);
      IIputdomio((short *)0,1,32,0,pttrn_name);
      IIputdomio((short *)0,1,32,0,ii_pattern_string);
      IIputdomio((short *)0,1,32,0,uidbdata->suser);
      IIputdomio((short *)0,1,32,0,uidbdata->sdba);
      IIputdomio((short *)0,1,32,0,uidbdata->catowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"ferelacc2",6505,17470);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, create_date, alter_date, table_type, \
table_version, system_use from iitables where(table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relname);
        IIwritio(0,(short *)0,1,32,0,(char *)
"escape '\\' or table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,pttrn_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
"escape '\\')and table_name not like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,ii_pattern_string);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and system_use<>'S' and(table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,uidbdata->suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,uidbdata->sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,uidbdata->catowner);
        IIwritio(0,(short *)0,1,32,0,(char *)")order by table_name");
        IIexDefine(0,(char *)"ferelacc2",6505,17470);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,relsource->table_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,relsource->table_owner);
      IIgetdomio((short *)0,1,32,25,relsource->create_date);
      IIgetdomio((short *)0,1,32,25,relsource->alter_date);
      IIgetdomio((short *)0,1,32,8,relsource->table_type);
      IIgetdomio((short *)0,1,32,8,relsource->version);
      IIgetdomio((short *)0,1,32,1,relsource->system_use);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 371 "ferelacc.sc" */	/* host code */
					*status = STATUS_OPEN;
/* # line 372 "ferelacc.sc" */	/* inquire_ingres */
      {
        IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
      }
/* # line 374 "ferelacc.sc" */	/* host code */
					if (errnum == OK)
					{
						STtrmwhite(
							relsource->table_name);
						STtrmwhite(
							relsource->table_owner);
						STtrmwhite(
							relsource->create_date);
						STtrmwhite(
							relsource->alter_date);
						STtrmwhite(
							relsource->table_type);
						STtrmwhite(relsource->version);
						STtrmwhite(
							relsource->system_use);
					}
					if  (dummy)
					{
						return(errnum);
					}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
			}
		}
		else  /* Version is 6.5 or greater */
		{
			/*
			** Catalog owner is ALWAYS a regular identifier.
			*/
			STcopy(UI_FE_CAT_ID_65,&r65_cat_id[0]);
			IIUGdbo_dlmBEobject(&r65_cat_id[0],FALSE);
			if  ((relowner == NULL) || (*relowner == EOS))
			{
				relowner_ptr = uidbdata->suser;
			}
			else
			{
				relowner_ptr = relowner;
			}
			/*
			** If wildcards are present, we have to trust that the
			** caller has specified the identifier exactly!
			*/
			if  (cats_wanted)
			{
				/* 6.5 or greater WITH ii catalogs */
/* # line 419 "ferelacc.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ferelacc3",6506,16483);
      IIputdomio((short *)0,1,32,0,relowner_ptr);
      IIputdomio((short *)0,1,32,0,r65_cat_id);
      IIputdomio((short *)0,1,32,0,relname);
      IIputdomio((short *)0,1,32,0,pttrn_name);
      IIputdomio((short *)0,1,32,0,relname);
      IIputdomio((short *)0,1,32,0,pttrn_name);
      IIputdomio((short *)0,1,32,0,relowner_ptr);
      IIputdomio((short *)0,1,32,0,relowner_ptr);
      IIputdomio((short *)0,1,32,0,relname);
      IIputdomio((short *)0,1,32,0,pttrn_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"ferelacc3",6506,16483);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, table_type, system_use from iitables \
where(table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relowner_ptr);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,r65_cat_id);
        IIwritio(0,(short *)0,1,32,0,(char *)")and(table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,relname);
        IIwritio(0,(short *)0,1,32,0,(char *)
"escape '\\' or table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,pttrn_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
"escape '\\')union select table_name, table_owner, table_type, system_\
use from iiaccess where(table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,relname);
        IIwritio(0,(short *)0,1,32,0,(char *)
"escape '\\' or table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,pttrn_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"escape '\\')and(permit_u\
ser=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,relowner_ptr);
        IIwritio(0,(short *)0,1,32,0,(char *)
"or permit_user=' ' or permit_user='PUBLIC')union select isyn.synonym_\
name as table_name, isyn.synonym_owner as table_owner, 'S' as table_ty\
pe, itbl.system_use from iisynonyms isyn, iitables itbl where isyn.tab\
le_name=itbl.table_name and isyn.table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)
"itbl.table_owner and isyn.synonym_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,relowner_ptr);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(isyn.synonym_name li\
ke");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,relname);
        IIwritio(0,(short *)0,1,32,0,(char *)
"escape '\\' or isyn.synonym_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,32,0,pttrn_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
"escape '\\')order by table_name, table_owner");
        IIexDefine(0,(char *)"ferelacc3",6506,16483);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,relsource->table_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,relsource->table_owner);
      IIgetdomio((short *)0,1,32,8,relsource->table_type);
      IIgetdomio((short *)0,1,32,1,relsource->system_use);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 465 "ferelacc.sc" */	/* host code */
					*status = STATUS_OPEN;
/* # line 466 "ferelacc.sc" */	/* inquire_ingres */
      {
        IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
      }
/* # line 468 "ferelacc.sc" */	/* host code */
					if (errnum == OK)
					{
						if  (STtrmwhite(
							relsource->table_name)
									 == 0)
						{
							STcopy(ERx(" "),
							relsource->table_name);
						}
						if  (STtrmwhite(
							relsource->table_owner)
									 == 0)
						{
							STcopy(ERx(" "),
							relsource->table_owner);
						}
						STtrmwhite(
							relsource->table_type);
						STtrmwhite(
							relsource->system_use);
					}
					if  (dummy)
					{
						return(errnum);
					}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
			}
			else if ((relname[0] == '%')&&(STlength(relname) == 1))
			{
/* # line 498 "ferelacc.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ferelacc4",6507,19969);
      IIputdomio((short *)0,1,32,0,relowner_ptr);
      IIputdomio((short *)0,1,32,0,ii_pattern_string);
      IIputdomio((short *)0,1,32,0,relowner_ptr);
      IIputdomio((short *)0,1,32,0,ii_pattern_string);
      IIputdomio((short *)0,1,32,0,relowner_ptr);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"ferelacc4",6507,19969);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, table_type, system_use from iitables \
where table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relowner_ptr);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and system_use<>'S' and system_use<>'G' and table_name not like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,ii_pattern_string);
        IIwritio(0,(short *)0,1,32,0,(char *)
"union select table_name, table_owner, table_type, system_use from iia\
ccess where(permit_user=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,relowner_ptr);
        IIwritio(0,(short *)0,1,32,0,(char *)
"or permit_user=' ' or permit_user='PUBLIC' or permit_user=dbmsinfo('g\
roup'))and system_use<>'S' and table_name not like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,ii_pattern_string);
        IIwritio(0,(short *)0,1,32,0,(char *)
"union select isyn.synonym_name as table_name, isyn.synonym_owner as t\
able_owner, 'S' as table_type, itbl.system_use from iisynonyms isyn, i\
itables itbl where isyn.table_name=itbl.table_name and isyn.table_owne\
r=itbl.table_owner and synonym_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,relowner_ptr);
        IIwritio(0,(short *)0,1,32,0,(char *)
"order by table_name, table_owner");
        IIexDefine(0,(char *)"ferelacc4",6507,19969);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,relsource->table_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,relsource->table_owner);
      IIgetdomio((short *)0,1,32,8,relsource->table_type);
      IIgetdomio((short *)0,1,32,1,relsource->system_use);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 555 "ferelacc.sc" */	/* host code */
					*status = STATUS_OPEN;
/* # line 556 "ferelacc.sc" */	/* inquire_ingres */
      {
        IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
      }
/* # line 558 "ferelacc.sc" */	/* host code */
					if (errnum == OK)
					{
						if  (STtrmwhite(
							relsource->table_name)
									 == 0)
						{
							STcopy(ERx(" "),
							relsource->table_name);
						}
						if  (STtrmwhite(
							relsource->table_owner)
									 == 0)
						{
							STcopy(ERx(" "),
							relsource->table_owner);
						}
						STtrmwhite(
							relsource->table_type);
						STtrmwhite(
							relsource->system_use);
					}
					if (dummy)
					{
						return(errnum);
					}
			_65fetchdets:
					/*
					** We can remove this when BLOBs
					** extension tables are no longer
					** categorized as system_use 'U'
					** (we say "!= 'S'" since Gateways may
					** return system_use = ' ').
					*/
					if  ((STbcompare(relsource->table_name,
							 2,
							 ERx("ii"),
							 2,TRUE) == 0) &&
					      (*relsource->system_use != 'S'))
					{
						continue;
					} 
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
			}
			/*
			** end else-if all wild card query (help\g and listpick)
			*/
			else
			{
				/* 
				** 6.5 or greater WITHOUT ii catalogs
				*/
/* # line 609 "ferelacc.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ferelacc5",6508,23386);
      IIputdomio((short *)0,1,32,0,relowner_ptr);
      IIputdomio((short *)0,1,32,0,relname);
      IIputdomio((short *)0,1,32,0,pttrn_name);
      IIputdomio((short *)0,1,32,0,ii_pattern_string);
      IIputdomio((short *)0,1,32,0,relname);
      IIputdomio((short *)0,1,32,0,pttrn_name);
      IIputdomio((short *)0,1,32,0,relowner_ptr);
      IIputdomio((short *)0,1,32,0,relowner_ptr);
      IIputdomio((short *)0,1,32,0,relname);
      IIputdomio((short *)0,1,32,0,pttrn_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"ferelacc5",6508,23386);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, table_type, system_use from iitables \
where table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relowner_ptr);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and system_use<>'S' and(table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,relname);
        IIwritio(0,(short *)0,1,32,0,(char *)
"escape '\\' or table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,pttrn_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
"escape '\\')and table_name not like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,ii_pattern_string);
        IIwritio(0,(short *)0,1,32,0,(char *)
"union select table_name, table_owner, table_type, system_use from iia\
ccess where(table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,relname);
        IIwritio(0,(short *)0,1,32,0,(char *)
"escape '\\' or table_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,pttrn_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"escape '\\')and(permit_u\
ser=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,relowner_ptr);
        IIwritio(0,(short *)0,1,32,0,(char *)
"or permit_user=' ' or permit_user='PUBLIC')union select isyn.synonym_\
name as table_name, isyn.synonym_owner as table_owner, 'S' as table_ty\
pe, system_use from iisynonyms isyn, iitables itbl where isyn.table_na\
me=itbl.table_name and isyn.table_owner=itbl.");
        IIwritio(0,(short *)0,1,32,0,(char *)
"table_owner and synonym_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,relowner_ptr);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(isyn.synonym_name li\
ke");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,relname);
        IIwritio(0,(short *)0,1,32,0,(char *)
"escape '\\' or isyn.synonym_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,32,0,pttrn_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
"escape '\\')order by table_name, table_owner");
        IIexDefine(0,(char *)"ferelacc5",6508,23386);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,relsource->table_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,relsource->table_owner);
      IIgetdomio((short *)0,1,32,8,relsource->table_type);
      IIgetdomio((short *)0,1,32,1,relsource->system_use);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 664 "ferelacc.sc" */	/* host code */
					*status = STATUS_OPEN;
/* # line 665 "ferelacc.sc" */	/* inquire_ingres */
      {
        IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
      }
/* # line 667 "ferelacc.sc" */	/* host code */
					if (errnum == OK)
					{
						if  (STtrmwhite(
							relsource->table_name)
									 == 0)
						{
							STcopy(ERx(" "),
							relsource->table_name);
						}
						if  (STtrmwhite(
							relsource->table_owner)
									 == 0)
						{
							STcopy(ERx(" "),
							relsource->table_owner);
						}
						STtrmwhite(
							relsource->table_type);
						STtrmwhite(
							relsource->system_use);
					}
					if  (dummy)
					{
						return(errnum);
					}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
			}
			/* end */
			break;
		}
		break;
	default:
		/*
		** unknown operation - let it fall out and force STATUS_DONE
		*/
		break;
	}
	if (mode == MODE_CLOSE)
	{
		*status = STATUS_CLOSED;
		return(OK);
	}
    *status = STATUS_DONE;
    /* No more records */
    return(E_UI0002_EOS);
}
