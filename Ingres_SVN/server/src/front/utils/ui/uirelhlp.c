# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h> 
# include	<st.h> 
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <ui.h>
# include <uigdata.h>
/**
** Name:	uirelhlp.sc -	Get Relation Information for Help Routine.
**
** History:
**	Revision 6.4  1991/05/05  wong
**	Replaced 'version' test in 'IIUIrel_help()' with 'catalog_level' test.
**	20-mar-1991 (kathryn)
**		Added IIUI64rel_help() - New query to get help information for 
**		versions 6.4 and later.  The column "key_is_compressed" was 
**		added to iitables in release 6.4, and the value of this column 
**		is needed by the TM help facility.
**	10-May-1990 (fredv)
**		Added NO_OPTIM hint for ris_us5; otherwise, tm will SEGV on
**		"help table XXX".
**	27-jul-1992 (rdrane)
**		Add 6.5 support for owner.tablename, delimited identifiers,
**		and synonyms.  Tagged IIUI64rel_help() as being static to this
**		module, and added static module IIUI65rel_help();
**      06-mar-1996 (stial01)
**              Added IIUI66rel_help
**	17-jul-1996 (ramra01)
**		Added relversion and reltotwid for 66rel_help.
**	29-jan-1997 (hanch04)
**		Changed UI_LEVEL_66 to UI_LEVEL_800
**	27-Mar-1997 (jenjo02)
**	    Table priority project:
**	    Added reltcpri (table cache priority).
**      02-Sep-1997 (carsu07)
**		Changed the format to 1/1/1970 from the old format which was
**		1-1-1970. The old format caused a problem when II_DATE_FORMAT 
**		was set to SWEDEN or FINLAND.  It is not possible to choose
**		from the list of date formats the format n-m-n (dmy).
**		Instead the format n-n-n (ymd) is always chosen and this   
**		results in an error, stating that 1970 is not a valid day.
**		(See common/adf/adu/adutabdate.roc for the table of 
**		SWEDEN/FINLAND valid date formats). (Bug #59756)
**      23-Sep-1997 (kosma01)
**              Added no optim hint for rs4_us5
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	16-aug-2001 (toumi01)
**	    speculative i64_aix NO_OPTIM change for beta xlc_r - FIXME !!!
**	2-Jun-2004 (schka24)
**	    r3 additions for partitioned tables.
**    25-Oct-2005 (hanje04)
**        Add prototype for IIUI66rel_help() to prevent compiler
**        errors with GCC 4.0 due to conflict with implicit declaration.
**	30-May-2006 (jenjo02)
**	    Backed out Ingres2007 change to pick up "clustered" table 
**	    structure - no longer necessary.
**/
/*
NO_OPTIM=ris_us5 rs4_us5 i64_aix
*/
char	*IIUIscl_StdCatLevel();
bool	IIUIdct_use_iitables();
static	DB_STATUS	IIUI64rel_help();
static	DB_STATUS	IIUI65rel_help();
static	DB_STATUS	IIUI66rel_help();
static	DB_STATUS	IIUIr3rel_help();
/*{
** Name:    IIUIrel_help() - get relation info for one relation.
**
** Description:
**	This function fills a a statically defined UI_HELP_INFO structure 
**	for one relation and returns a pointer to that static structure.  
**	This static structure is reused upon next call to the routine, so 
**	all information must be copied into local data structures in order 
**	for it to remain after another call to IIUIrel_help().  The user
**	passes in the full name of the relation (no wildcards allowed).
**	IIUIrel_help does all dis-ambiguation nessesary to differenciate
**	between user owned and DBA owned relations.  If there is no relation
**	found, then the pointer to the UI_HELP_INFO structure will be
**	NULL;
**	the following is the description of the UI_HELP_INFO structure
**	char	table_name[FE_MAXNAME+1];
**	char	table_owner[FE_MAXNAME+1];
**	char	create_date[UI_DATESIZE+1];
**	char	alter_date[UI_DATESIZE+1];
**	char	table_type[2];
**	char	table_subtype[2]; 
**	char	table_version[9];
**	char	system_use[2];
**	char	table_stats[2];
**	char	table_indexes[2];
**	char	is_readonly[2];
**	i4	num_rows;
**	char	storage_structure[16];
**	char	is_compressed[2];
**	char	duplicate_rows[2];
**	char	unique_rule[2];
**	i4	number_pages;
**	i4	overflow_pages;
**	char	expire_date[UI_DATESIZE + 1]; 
**	char	table_integrities[2]; 
**	char	table_permits[2];
**	char	all_to_all[2]; 
**	char	ret_to_all[2]; 
**	i4	row_width;
**	char	is_journalled[2];
**	char	view_base[2]; 
**	char	multi_locations[2];
**	char	location_name[FE_MAXNAME + 1]; 
**	char	key_is_compressed[2];		* version 6.4 and later *
**      i4      page_size;                
**	i4	rel_version;
**	i4	rel_totwid;
**	i2	cache_pri;
**	u_i2	phys_partitions;		* New for r3 *
**	i2	partition_dimensions;
** UI_HELP_INFO;
**
** Inputs:
**	relname		Qualification string to be used as a query restriction
**			on the namestrings returned.  Any single value string
**			which identifies only one relation may be used.
**			This generally means that the qualification should be
**			fully specified, having no wild cards.
**	relowner	Name of the owner of the relation you want the
**			attributes of.  This is also expected to be in
**			normalized form.  However, it may also be specified
**			as NULL or an empty string for pre-6.5 compatibility
**			or when the true owner is unknown or unimportant.
** Outputs:
**	UI_HELP_INFO	**help_ptr	
**			*help_ptr will point to the static structure
**			containing the requested relation information
**			if the relation was found, otherwise *help_ptr will
**			be NULL.
**			
** Returns:
**	{STATUS}	OK			successful.
**			other			DBMS error returned
** History:
**	12-mar-90 (teresal)
**		Modified so expire date will be converted into the correct
**		century. (Fix for Bug 8442)
**	20-mar-1991 (kathryn)
**		If running on DBMS release 6.4 or greater, call IIUI64rel_help()
**		to perform query. This new query includes "key_is_compressed" 
**		which was not available prior to 6.4
**	05-may-1991 (jhw) Replaced 'version' test with 'catalog_level' test.
**		Bug #36993.
**	17-may-1991 (kathryn)
**		Correct query against iiphysical_tables, which attempted to
**		select non-existent columns.
**	27-jul-1992 (rdrane)
**		Add 6.5 support for owner.tablename, delimited identifiers,
**		and synonyms.  Change parameterization to include owner.
**		Indicate that both relname and relowner are expected to be
**		normalized, and that owner may be NULL or an empty string
**		(relowner only used if 6.5).
**      06-mar-1996 (stial01)
**              Call IIUI66rel_help for 6.6 catalogs
**	27-Mar-1997 (jenjo02)
**	    Table priority project:
**	    Added reltcpri (table cache priority).
**	02-Sep-1997 (carsu07)
**		Changed the format to 1/1/1970 from the old format which was	
**		1-1-1970. The old format caused a problem when II_DATE_FORMAT 
**		was set to SWEDEN or FINLAND.  It is not possible to choose
**		from the list of date formats the format n-m-n (dmy).      
**		Instead the format n-n-n (ymd) is always chosen and this   
**		results in an error when the date is 1-1-1970 because 1970
**		is interpreted as a day.  To avoid this, the date format was
**		changed. (See common/adf/adu/adutabdate.roc for the table of 
**		SWEDEN/FINLAND valid date formats). (Bug #59756)
**      08-Feb-2000 (hanal04) Bug 99787 Problem INGSRV 1075.
**              Remove use of _date() and _time() functions in the 
**              computation of SAVE UNTIL (expires on) dates. Now use
**              gmt_timestamp(). Change as per natjo01 change 276883.
*/
  static UI_HELP_INFO help_info[2];
DB_STATUS
IIUIrel_help(relname,relowner,help_ptr)
  char *relname;
  char *relowner;
  UI_HELP_INFO **help_ptr;
{
  UIDBDATA *uidbdata;
  i4 errnum, i = 0;
  i4 save_date[2];
  float ftemp;
	uidbdata = IIUIdbdata();
	/* Init page size in case level < UI_LEVEL_800 */
	help_info[0].page_size = help_info[1].page_size = 2048;
	help_info[0].rel_version = help_info[1].rel_version = 0;
	help_info[0].rel_totwid = help_info[1].rel_totwid = 0;
	help_info[0].cache_pri = help_info[1].cache_pri = 0;
	help_info[0].phys_partitions = help_info[1].phys_partitions = 0;
	help_info[0].partition_dimensions = help_info[1].partition_dimensions = 0;
	if  (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_900) >= 0)
	{ /* r3 or later standard catalogs */
	    if  ((errnum = IIUIr3rel_help(relname,relowner,&i,save_date,
					  uidbdata)) != OK)
		return(errnum);
	}
	else if  (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_800) >= 0)
	{ /* 6.6 or later standard catalogs */
	    if  ((errnum = IIUI66rel_help(relname,relowner,&i,save_date,
					  uidbdata)) != OK)
		return(errnum);
	}
	else if  (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_65) == 0)
	{ /* 6.5 or later standard catalogs */
	    if  ((errnum = IIUI65rel_help(relname,relowner,&i,save_date,
					  uidbdata)) != OK)
		return(errnum);
	}
	else if (STcompare(IIUIscl_StdCatLevel(), UI_LEVEL_64) == 0)
	{ /* 6.4 standard catalogs */
	    if (errnum = IIUI64rel_help(relname,&i,save_date,uidbdata) != OK)
		return(errnum);
	}
	else
	{
/* # line 240 "uirelhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"uirelhlp1",6499,21434);
      IIputdomio((short *)0,1,32,0,relname);
      IIputdomio((short *)0,1,32,0,uidbdata->suser);
      IIputdomio((short *)0,1,32,0,uidbdata->sdba);
      IIputdomio((short *)0,1,32,0,uidbdata->catowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"uirelhlp1",6499,21434);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, create_date, table_type, table_subtyp\
e, table_version, system_use, table_stats, table_indexes, is_readonly,\
 num_rows, storage_structure, is_compressed, duplicate_rows, unique_ru\
le, number_pages, overflow_pages, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"table_integrities, table_permits, all_to_all, ret_to_all, row_width, \
is_journalled, multi_locations, view_base, location_name, expire_date \
from iitables where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,uidbdata->catowner);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"uirelhlp1",6499,21434);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,help_info[i].table_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,help_info[i].table_owner);
      IIgetdomio((short *)0,1,32,UI_DATESIZE+1-1,help_info[i].create_date);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_type);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_subtype);
      IIgetdomio((short *)0,1,32,8,help_info[i].table_version);
      IIgetdomio((short *)0,1,32,1,help_info[i].system_use);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_stats);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_indexes);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_readonly);
      IIgetdomio((short *)0,1,30,4,&help_info[i].num_rows);
      IIgetdomio((short *)0,1,32,16,help_info[i].storage_structure);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_compressed);
      IIgetdomio((short *)0,1,32,1,help_info[i].duplicate_rows);
      IIgetdomio((short *)0,1,32,1,help_info[i].unique_rule);
      IIgetdomio((short *)0,1,30,4,&help_info[i].number_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].overflow_pages);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_integrities);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_permits);
      IIgetdomio((short *)0,1,32,1,help_info[i].all_to_all);
      IIgetdomio((short *)0,1,32,1,help_info[i].ret_to_all);
      IIgetdomio((short *)0,1,30,4,&help_info[i].row_width);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_journalled);
      IIgetdomio((short *)0,1,32,1,help_info[i].multi_locations);
      IIgetdomio((short *)0,1,32,1,help_info[i].view_base);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,help_info[i].location_name);
      IIgetdomio((short *)0,1,30,4,&save_date[i]);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 306 "uirelhlp.sc" */	/* host code */
		STtrmwhite(help_info[i].table_name);
		STtrmwhite(help_info[i].table_owner);
		STcopy(help_info[i].create_date, help_info[i].alter_date);
		i++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 311 "uirelhlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 312 "uirelhlp.sc" */	/* host code */
	if ( errnum != OK )
		return errnum;
	if ( !IIUIdct_use_iitables() )
	{
/* # line 316 "uirelhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"uirelhlp2",6500,18556);
      IIputdomio((short *)0,1,32,0,relname);
      IIputdomio((short *)0,1,32,0,uidbdata->suser);
      IIputdomio((short *)0,1,32,0,uidbdata->sdba);
      IIputdomio((short *)0,1,32,0,uidbdata->catowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"uirelhlp2",6500,18556);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_stats, table_indexes, is_readonly, num_rows, storage_str\
ucture, is_compressed, duplicate_rows, unique_rule, number_pages, over\
flow_pages, row_width from iiphysical_tables where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and((table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->suser);
        IIwritio(0,(short *)0,1,32,0,(char *)")or(table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)")or(table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,uidbdata->catowner);
        IIwritio(0,(short *)0,1,32,0,(char *)"))");
        IIexDefine(0,(char *)"uirelhlp2",6500,18556);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,help_info[i].table_stats);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_indexes);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_readonly);
      IIgetdomio((short *)0,1,30,4,&help_info[i].num_rows);
      IIgetdomio((short *)0,1,32,16,help_info[i].storage_structure);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_compressed);
      IIgetdomio((short *)0,1,32,1,help_info[i].duplicate_rows);
      IIgetdomio((short *)0,1,32,1,help_info[i].unique_rule);
      IIgetdomio((short *)0,1,30,4,&help_info[i].number_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].overflow_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].row_width);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 349 "uirelhlp.sc" */	/* host code */
			STtrmwhite(help_info[i].table_name);
			STtrmwhite(help_info[i].table_owner);
			STcopy(help_info[i].create_date, help_info[i].alter_date);
			i++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 354 "uirelhlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 355 "uirelhlp.sc" */	/* host code */
		if ( errnum != OK )
			return errnum;
	}
	}
	if ( i == 2 && STequal(uidbdata->suser, help_info[1].table_owner) )
	{
		if (save_date[1] > 0)
		{
			/* this is a terrible hack, and eventually a new
			** FE routine should be written to call ADF to
			** convert a _bintime into a gmt date, but this
			** code will ONLY be executed if a user wants
			** detailed information about a table with
			** an expiration date.  I am assuming that
			** this is a relatively rare occurance, and that
			** the increase in performance for the usual
			** case will balance out the performance hit
			** for the case with a table with an expire date
			**
			** Note there is a bug in _date() such that it doesn't
			** return the correct century (if >= 2000) _date can't
			** be changed (it's already published) so another
			** hack has been implemented here to convert to
			** the correct century. Assumptions: timestamps are
			** never older than 1970 and this hack only works until
			** 2069. (Bug fix 8442)
			*/
                        /* Now use gmt_timestamp for this */
/* # line 383 "uirelhlp.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select gmt_timestamp(");
    IIputdomio((short *)0,1,30,4,&save_date[1]);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,UI_DATESIZE + 1-1,help_info[1].expire_date);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 385 "uirelhlp.sc" */	/* host code */
		}
		else
		{
			help_info[1].expire_date[0] = EOS;
		}
		*help_ptr = &(help_info[1]);
	}
	else if (i == 0)
	{
		*help_ptr = (UI_HELP_INFO *) NULL;
	}
	else
	{
		if (save_date[0] > 0)
		{
			/* this is a terrible hack, and eventually a new
			** FE routine should be written to call ADF to
			** convert a _bintime into a gmt date, but this
			** code will ONLY be executed if a user wants
			** detailed information about a table with
			** an expiration date.  I am assuming that
			** this is a relatively rare occurance, and that
			** the increase in performance for the usual
			** case will balance out the performance hit
			** for the case with a table with an expire date
			**
			** Note there is a bug in _date() such that it doesn't
			** return the correct century (if >= 2000) _date can't
			** be changed (it's already published) so another
			** hack has been implemented here to convert to
			** the correct century. Assumptions: timestamps are
			** never older than 1970 and this hack only works until
			** 2069. (Bug fix 8442)
			*/
                        /* Now use gmt_timestamp to get the date+time */
/* # line 420 "uirelhlp.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select gmt_timestamp(");
    IIputdomio((short *)0,1,30,4,&save_date[0]);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,UI_DATESIZE + 1-1,help_info[0].expire_date);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 422 "uirelhlp.sc" */	/* host code */
		}
		else
		{
			help_info[0].expire_date[0] = EOS;
		}
		*help_ptr = &(help_info[0]);
	}
	return OK;
}
/*
** Name:  IIUI64rel_help - Query for relation help info - vers 6.4.
**
** Description:
**	Fills the  statically defined UI_HELP_INFO structure for one relation.
**
** Inputs:
**      relname         Qualification string to be used as a query restriction
**                      on the namestrings returned.  Any single value string
**                      which identifies only one relation may be used.
**                      This generally means that the qualification should be
**                      fully specified, having no wild cards.
**	ind:		Index of help_info structure to be filled.
**	uidbdata:	Current user/dba info.
**
** Ouputs:
**	save_date[]:	Expire_date of relation as retrieved from iitables.
**	ind:		Index of help_info structure is incremented.
**
**	Returns:
**		OK	successful.
**		other	DBMS error returned.
**
** Side Effects:
**	UI_HELP_INFO: 	Help_info structure contains the requested relation 
**			information.
**
** History:
**	20-mar-1991 (kathryn)
**		Written.
**	27-jul-1992 (rdrane)
**		Tag routine as being static.
*/
static
DB_STATUS
IIUI64rel_help(relname,ind,save_date,uidbdata)
  char *relname;
  i4 *ind;
  i4 save_date[];
  UIDBDATA *uidbdata;
{
  i4 errnum, i;
	i = *ind;
/* # line 478 "uirelhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"uirelhlp3",6501,16634);
      IIputdomio((short *)0,1,32,0,relname);
      IIputdomio((short *)0,1,32,0,uidbdata->suser);
      IIputdomio((short *)0,1,32,0,uidbdata->sdba);
      IIputdomio((short *)0,1,32,0,uidbdata->catowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"uirelhlp3",6501,16634);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, create_date, table_type, table_subtyp\
e, table_version, system_use, table_stats, table_indexes, is_readonly,\
 num_rows, storage_structure, is_compressed, duplicate_rows, unique_ru\
le, number_pages, overflow_pages, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"table_integrities, table_permits, all_to_all, ret_to_all, row_width, \
is_journalled, multi_locations, view_base, location_name, expire_date,\
 key_is_compressed from iitables where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,uidbdata->catowner);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"uirelhlp3",6501,16634);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,help_info[i].table_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,help_info[i].table_owner);
      IIgetdomio((short *)0,1,32,UI_DATESIZE+1-1,help_info[i].create_date);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_type);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_subtype);
      IIgetdomio((short *)0,1,32,8,help_info[i].table_version);
      IIgetdomio((short *)0,1,32,1,help_info[i].system_use);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_stats);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_indexes);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_readonly);
      IIgetdomio((short *)0,1,30,4,&help_info[i].num_rows);
      IIgetdomio((short *)0,1,32,16,help_info[i].storage_structure);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_compressed);
      IIgetdomio((short *)0,1,32,1,help_info[i].duplicate_rows);
      IIgetdomio((short *)0,1,32,1,help_info[i].unique_rule);
      IIgetdomio((short *)0,1,30,4,&help_info[i].number_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].overflow_pages);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_integrities);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_permits);
      IIgetdomio((short *)0,1,32,1,help_info[i].all_to_all);
      IIgetdomio((short *)0,1,32,1,help_info[i].ret_to_all);
      IIgetdomio((short *)0,1,30,4,&help_info[i].row_width);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_journalled);
      IIgetdomio((short *)0,1,32,1,help_info[i].multi_locations);
      IIgetdomio((short *)0,1,32,1,help_info[i].view_base);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,help_info[i].location_name);
      IIgetdomio((short *)0,1,30,4,&save_date[i]);
      IIgetdomio((short *)0,1,32,1,help_info[i].key_is_compressed);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 546 "uirelhlp.sc" */	/* host code */
		STtrmwhite(help_info[i].table_name);
		STtrmwhite(help_info[i].table_owner);
		STcopy(help_info[i].create_date, help_info[i].alter_date);
		i++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
/* # line 551 "uirelhlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 552 "uirelhlp.sc" */	/* host code */
	if ( errnum != OK )
		return errnum;
	if ( !IIUIdct_use_iitables() )
	{
/* # line 556 "uirelhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"uirelhlp4",6502,19829);
      IIputdomio((short *)0,1,32,0,relname);
      IIputdomio((short *)0,1,32,0,uidbdata->suser);
      IIputdomio((short *)0,1,32,0,uidbdata->sdba);
      IIputdomio((short *)0,1,32,0,uidbdata->catowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"uirelhlp4",6502,19829);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_stats, table_indexes, is_readonly, num_rows, storage_str\
ucture, is_compressed, duplicate_rows, unique_rule, number_pages, over\
flow_pages, row_width, key_is_compressed from iiphysical_tables where \
table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and((table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->suser);
        IIwritio(0,(short *)0,1,32,0,(char *)")or(table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)")or(table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,uidbdata->catowner);
        IIwritio(0,(short *)0,1,32,0,(char *)"))");
        IIexDefine(0,(char *)"uirelhlp4",6502,19829);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,help_info[i].table_stats);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_indexes);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_readonly);
      IIgetdomio((short *)0,1,30,4,&help_info[i].num_rows);
      IIgetdomio((short *)0,1,32,16,help_info[i].storage_structure);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_compressed);
      IIgetdomio((short *)0,1,32,1,help_info[i].duplicate_rows);
      IIgetdomio((short *)0,1,32,1,help_info[i].unique_rule);
      IIgetdomio((short *)0,1,30,4,&help_info[i].number_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].overflow_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].row_width);
      IIgetdomio((short *)0,1,32,1,help_info[i].key_is_compressed);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 591 "uirelhlp.sc" */	/* host code */
			STtrmwhite(help_info[i].table_name);
			STtrmwhite(help_info[i].table_owner);
			STcopy(help_info[i].create_date, help_info[i].alter_date);
			i++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
/* # line 596 "uirelhlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 597 "uirelhlp.sc" */	/* host code */
		if ( errnum != OK )
			return errnum;
	}
	*ind = i;
	return OK;
}
/*
** Name:  IIUI65rel_help - Query for relation help info - vers 6.5 and later.
**
** Description:
**	Fills the  statically defined UI_HELP_INFO structure for one relation.
**	If the specified relation is a synonym, then the information returned
**	including the table name and owner will refect that of the base table.
**
** Inputs:
**      relname         Name of the relation for which information is to be
**                      returned.  This is expected to be normalized and to
**	 		not contain any wildcards.  It may be a synonym.
**	relowner	Name of the owner of the relation for which information
**			is to be returned.  This is also expected to be in
**			normalized form.  However, it may also be specified
**			as NULL or an empty string when the true owner is
**			unknown or unimportant.
**	ind:		Index of help_info structure to be filled.
**	uidbdata:	Current user/dba info.
**
** Ouputs:
**	save_date[]:	Expire_date of relation as retrieved from iitables.
**	ind:		Index of help_info structure is incremented.
**
**	Returns:
**		OK	successful.
**		other	DBMS error returned.
**
** Side Effects:
**	UI_HELP_INFO: 	Help_info structure contains the requested relation 
**			information.
**
** History:
**	27-jul-1992 (rdrane)
**		Created from IIUI64rel_help().  Invokes FE_resolve() to handle
**		any synonyms.  STtrmwhite() can return an empty string if all
**		whitespace - need to detect this and force the result to be
**		one space.  Note that information for only one relation
**		will be returned.
**	21-apr-1994 (connie) Bug #49807
**		If the relowner is NULL, the query result from iitables
**		should be the same as initial index value and should not
**		fail out in this case, so > 1 is used instead of != 1.
*/
static
DB_STATUS
IIUI65rel_help(relname,relowner,ind,save_date,uidbdata)
char		*relname;
char		*relowner;
  i4 *ind;
  i4 save_date[];
  UIDBDATA *uidbdata;
{
  i4 errnum, i;
  char relname_buf[(FE_MAXNAME + 1)];
  char relowner_buf[(FE_MAXNAME + 1)];
FE_RSLV_NAME	w_frn;
	i = *ind;	/* Save initial value of help_info index	*/
	/*
	** Effect any synonym resolution, and determine any true owner.
	*/
	w_frn.owner_dest = &relowner_buf[0];
	w_frn.name_dest = &relname_buf[0];
	if  (!FE_resolve(&w_frn,relname,relowner))
	{
	    /*
	    ** No relations meet qualification
	    */
	    return(OK);
	}
/* # line 677 "uirelhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"uirelhlp5",6503,23287);
      IIputdomio((short *)0,1,32,0,relname_buf);
      IIputdomio((short *)0,1,32,0,relowner_buf);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"uirelhlp5",6503,23287);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, create_date, table_type, table_subtyp\
e, table_version, system_use, table_stats, table_indexes, is_readonly,\
 num_rows, storage_structure, is_compressed, duplicate_rows, unique_ru\
le, number_pages, overflow_pages, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"table_integrities, table_permits, all_to_all, ret_to_all, row_width, \
is_journalled, multi_locations, view_base, location_name, expire_date,\
 key_is_compressed from iitables where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relname_buf);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,relowner_buf);
        IIexDefine(0,(char *)"uirelhlp5",6503,23287);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,help_info[i].table_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,help_info[i].table_owner);
      IIgetdomio((short *)0,1,32,UI_DATESIZE+1-1,help_info[i].create_date);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_type);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_subtype);
      IIgetdomio((short *)0,1,32,8,help_info[i].table_version);
      IIgetdomio((short *)0,1,32,1,help_info[i].system_use);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_stats);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_indexes);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_readonly);
      IIgetdomio((short *)0,1,30,4,&help_info[i].num_rows);
      IIgetdomio((short *)0,1,32,16,help_info[i].storage_structure);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_compressed);
      IIgetdomio((short *)0,1,32,1,help_info[i].duplicate_rows);
      IIgetdomio((short *)0,1,32,1,help_info[i].unique_rule);
      IIgetdomio((short *)0,1,30,4,&help_info[i].number_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].overflow_pages);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_integrities);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_permits);
      IIgetdomio((short *)0,1,32,1,help_info[i].all_to_all);
      IIgetdomio((short *)0,1,32,1,help_info[i].ret_to_all);
      IIgetdomio((short *)0,1,30,4,&help_info[i].row_width);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_journalled);
      IIgetdomio((short *)0,1,32,1,help_info[i].multi_locations);
      IIgetdomio((short *)0,1,32,1,help_info[i].view_base);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,help_info[i].location_name);
      IIgetdomio((short *)0,1,30,4,&save_date[i]);
      IIgetdomio((short *)0,1,32,1,help_info[i].key_is_compressed);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 742 "uirelhlp.sc" */	/* host code */
		if  (STtrmwhite(help_info[i].table_name) == 0)
		{
		    STcopy(ERx(" "),help_info[i].table_name);
		}
		if  (STtrmwhite(help_info[i].table_owner) == 0)
		{
		    STcopy(ERx(" "),help_info[i].table_owner);
		}
		STcopy(help_info[i].create_date, help_info[i].alter_date);
		i++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
/* # line 753 "uirelhlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 754 "uirelhlp.sc" */	/* host code */
	if ( errnum != OK )
	{
	    return(errnum);
	}
	if  ((i - *ind) > 1)	/* Should never find more than one! */
	{
	    return(FAIL);
	}
	if ( !IIUIdct_use_iitables() )
	{
/* # line 764 "uirelhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"uirelhlp6",6504,20537);
      IIputdomio((short *)0,1,32,0,relname_buf);
      IIputdomio((short *)0,1,32,0,relowner_buf);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"uirelhlp6",6504,20537);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_stats, table_indexes, is_readonly, num_rows, storage_str\
ucture, is_compressed, duplicate_rows, unique_rule, number_pages, over\
flow_pages, row_width, key_is_compressed from iiphysical_tables where \
table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relname_buf);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,relowner_buf);
        IIexDefine(0,(char *)"uirelhlp6",6504,20537);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,help_info[i].table_stats);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_indexes);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_readonly);
      IIgetdomio((short *)0,1,30,4,&help_info[i].num_rows);
      IIgetdomio((short *)0,1,32,16,help_info[i].storage_structure);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_compressed);
      IIgetdomio((short *)0,1,32,1,help_info[i].duplicate_rows);
      IIgetdomio((short *)0,1,32,1,help_info[i].unique_rule);
      IIgetdomio((short *)0,1,30,4,&help_info[i].number_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].overflow_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].row_width);
      IIgetdomio((short *)0,1,32,1,help_info[i].key_is_compressed);
      if (IIerrtest() != 0) goto IIrtB6;
/* # line 797 "uirelhlp.sc" */	/* host code */
			if  (STtrmwhite(help_info[i].table_name) == 0)
			{
			    STcopy(ERx(" "),help_info[i].table_name);
			}
			if  (STtrmwhite(help_info[i].table_owner) == 0)
			{
			    STcopy(ERx(" "),help_info[i].table_owner);
			}
			STcopy(help_info[i].create_date,
			       help_info[i].alter_date);
			i++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
/* # line 809 "uirelhlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 810 "uirelhlp.sc" */	/* host code */
		if  (errnum != OK )
		{
		    return(errnum);
		}
		if  ((i - *ind) > 1)	/* Should never find more than one! */
		{
		    return(FAIL);
		}
	}
	*ind = i;
	return(OK);
}
/*
** Name:  IIUI66rel_help - Query for relation help info - vers 6.6 and later.
**
** Description:
**	Fills the  statically defined UI_HELP_INFO structure for one relation.
**	If the specified relation is a synonym, then the information returned
**	including the table name and owner will refect that of the base table.
**
** Inputs:
**      relname         Name of the relation for which information is to be
**                      returned.  This is expected to be normalized and to
**	 		not contain any wildcards.  It may be a synonym.
**	relowner	Name of the owner of the relation for which information
**			is to be returned.  This is also expected to be in
**			normalized form.  However, it may also be specified
**			as NULL or an empty string when the true owner is
**			unknown or unimportant.
**	ind:		Index of help_info structure to be filled.
**	uidbdata:	Current user/dba info.
**
** Ouputs:
**	save_date[]:	Expire_date of relation as retrieved from iitables.
**	ind:		Index of help_info structure is incremented.
**
**	Returns:
**		OK	successful.
**		other	DBMS error returned.
**
** Side Effects:
**	UI_HELP_INFO: 	Help_info structure contains the requested relation 
**			information.
**
** History:
**      06-mar-1996 (stial01)
**              Created from IIUI65rel_help().
**	27-Mar-1997 (jenjo02)
**	    Table priority project:
**	    Added reltcpri (table cache priority).
*/
static
DB_STATUS
IIUI66rel_help(relname,relowner,ind,save_date,uidbdata)
char		*relname;
char		*relowner;
  i4 *ind;
  i4 save_date[];
  UIDBDATA *uidbdata;
{
  i4 errnum, i;
  char relname_buf[(FE_MAXNAME + 1)];
  char relowner_buf[(FE_MAXNAME + 1)];
FE_RSLV_NAME	w_frn;
	i = *ind;	/* Save initial value of help_info index	*/
	/*
	** Effect any synonym resolution, and determine any true owner.
	*/
	w_frn.owner_dest = &relowner_buf[0];
	w_frn.name_dest = &relname_buf[0];
	if  (!FE_resolve(&w_frn,relname,relowner))
	{
	    /*
	    ** No relations meet qualification
	    */
	    return(OK);
	}
/* # line 891 "uirelhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"uirelhlp7",6505,13750);
      IIputdomio((short *)0,1,32,0,relname_buf);
      IIputdomio((short *)0,1,32,0,relowner_buf);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"uirelhlp7",6505,13750);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, create_date, table_type, table_subtyp\
e, table_version, system_use, table_stats, table_indexes, is_readonly,\
 num_rows, storage_structure, is_compressed, duplicate_rows, unique_ru\
le, number_pages, overflow_pages, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"table_integrities, table_permits, all_to_all, ret_to_all, row_width, \
is_journalled, multi_locations, view_base, location_name, expire_date,\
 key_is_compressed, table_pagesize, table_relversion, table_reltotwid,\
 table_reltcpri from iitables where table_name");
        IIwritio(0,(short *)0,1,32,0,(char *)"=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relname_buf);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,relowner_buf);
        IIexDefine(0,(char *)"uirelhlp7",6505,13750);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE7;
IIrtB7:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,help_info[i].table_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,help_info[i].table_owner);
      IIgetdomio((short *)0,1,32,UI_DATESIZE+1-1,help_info[i].create_date);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_type);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_subtype);
      IIgetdomio((short *)0,1,32,8,help_info[i].table_version);
      IIgetdomio((short *)0,1,32,1,help_info[i].system_use);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_stats);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_indexes);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_readonly);
      IIgetdomio((short *)0,1,30,4,&help_info[i].num_rows);
      IIgetdomio((short *)0,1,32,16,help_info[i].storage_structure);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_compressed);
      IIgetdomio((short *)0,1,32,1,help_info[i].duplicate_rows);
      IIgetdomio((short *)0,1,32,1,help_info[i].unique_rule);
      IIgetdomio((short *)0,1,30,4,&help_info[i].number_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].overflow_pages);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_integrities);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_permits);
      IIgetdomio((short *)0,1,32,1,help_info[i].all_to_all);
      IIgetdomio((short *)0,1,32,1,help_info[i].ret_to_all);
      IIgetdomio((short *)0,1,30,4,&help_info[i].row_width);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_journalled);
      IIgetdomio((short *)0,1,32,1,help_info[i].multi_locations);
      IIgetdomio((short *)0,1,32,1,help_info[i].view_base);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,help_info[i].location_name);
      IIgetdomio((short *)0,1,30,4,&save_date[i]);
      IIgetdomio((short *)0,1,32,1,help_info[i].key_is_compressed);
      IIgetdomio((short *)0,1,30,4,&help_info[i].page_size);
      IIgetdomio((short *)0,1,30,4,&help_info[i].rel_version);
      IIgetdomio((short *)0,1,30,4,&help_info[i].rel_totwid);
      IIgetdomio((short *)0,1,30,2,&help_info[i].cache_pri);
      if (IIerrtest() != 0) goto IIrtB7;
/* # line 964 "uirelhlp.sc" */	/* host code */
		if  (STtrmwhite(help_info[i].table_name) == 0)
		{
		    STcopy(ERx(" "),help_info[i].table_name);
		}
		if  (STtrmwhite(help_info[i].table_owner) == 0)
		{
		    STcopy(ERx(" "),help_info[i].table_owner);
		}
		STcopy(help_info[i].create_date, help_info[i].alter_date);
		i++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE7:;
  }
/* # line 975 "uirelhlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 976 "uirelhlp.sc" */	/* host code */
	if ( errnum != OK )
	{
	    return(errnum);
	}
	if  ((i - *ind) > 1)	/* Should never find more than one! */
	{
	    return(FAIL);
	}
	if ( !IIUIdct_use_iitables() )
	{
/* # line 986 "uirelhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"uirelhlp8",6506,15090);
      IIputdomio((short *)0,1,32,0,relname_buf);
      IIputdomio((short *)0,1,32,0,relowner_buf);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"uirelhlp8",6506,15090);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_stats, table_indexes, is_readonly, num_rows, storage_str\
ucture, is_compressed, duplicate_rows, unique_rule, number_pages, over\
flow_pages, row_width, key_is_compressed from iiphysical_tables where \
table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relname_buf);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,relowner_buf);
        IIexDefine(0,(char *)"uirelhlp8",6506,15090);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE8;
IIrtB8:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,help_info[i].table_stats);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_indexes);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_readonly);
      IIgetdomio((short *)0,1,30,4,&help_info[i].num_rows);
      IIgetdomio((short *)0,1,32,16,help_info[i].storage_structure);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_compressed);
      IIgetdomio((short *)0,1,32,1,help_info[i].duplicate_rows);
      IIgetdomio((short *)0,1,32,1,help_info[i].unique_rule);
      IIgetdomio((short *)0,1,30,4,&help_info[i].number_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].overflow_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].row_width);
      IIgetdomio((short *)0,1,32,1,help_info[i].key_is_compressed);
      if (IIerrtest() != 0) goto IIrtB8;
/* # line 1019 "uirelhlp.sc" */	/* host code */
			if  (STtrmwhite(help_info[i].table_name) == 0)
			{
			    STcopy(ERx(" "),help_info[i].table_name);
			}
			if  (STtrmwhite(help_info[i].table_owner) == 0)
			{
			    STcopy(ERx(" "),help_info[i].table_owner);
			}
			STcopy(help_info[i].create_date,
			       help_info[i].alter_date);
			i++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE8:;
  }
/* # line 1031 "uirelhlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 1032 "uirelhlp.sc" */	/* host code */
		if  (errnum != OK )
		{
		    return(errnum);
		}
		if  ((i - *ind) > 1)	/* Should never find more than one! */
		{
		    return(FAIL);
		}
	}
	if (help_info[i].page_size == 0)
	    help_info[i].page_size = 2048;
	*ind = i;
	return(OK);
}
/*
** Name:  IIUIr3rel_help - Query for relation help info - release 3 and later.
**
** Description:
**	Fills the  statically defined UI_HELP_INFO structure for one relation.
**	If the specified relation is a synonym, then the information returned
**	including the table name and owner will refect that of the base table.
**
**	This clone is for release 3 catalogs which include partitioning info.
**	It would be slicker to come up with some kind of dynamic query,
**	but then it couldn't be REPEATED.
**
** Inputs:
**      relname         Name of the relation for which information is to be
**                      returned.  This is expected to be normalized and to
**	 		not contain any wildcards.  It may be a synonym.
**	relowner	Name of the owner of the relation for which information
**			is to be returned.  This is also expected to be in
**			normalized form.  However, it may also be specified
**			as NULL or an empty string when the true owner is
**			unknown or unimportant.
**	ind:		Index of help_info structure to be filled.
**	uidbdata:	Current user/dba info.
**
** Ouputs:
**	save_date[]:	Expire_date of relation as retrieved from iitables.
**	ind:		Index of help_info structure is incremented.
**
**	Returns:
**		OK	successful.
**		other	DBMS error returned.
**
** Side Effects:
**	UI_HELP_INFO: 	Help_info structure contains the requested relation 
**			information.
**
** History:
**	2-Jun-2004 (schka24)
**	    Yet another clone, for new r3 columns.
*/
static
DB_STATUS
IIUIr3rel_help(relname,relowner,ind,save_date,uidbdata)
char		*relname;
char		*relowner;
  i4 *ind;
  i4 save_date[];
  UIDBDATA *uidbdata;
{
  i4 errnum, i;
  char relname_buf[(FE_MAXNAME + 1)];
  char relowner_buf[(FE_MAXNAME + 1)];
FE_RSLV_NAME	w_frn;
	i = *ind;	/* Save initial value of help_info index	*/
	/*
	** Effect any synonym resolution, and determine any true owner.
	*/
	w_frn.owner_dest = &relowner_buf[0];
	w_frn.name_dest = &relname_buf[0];
	if  (!FE_resolve(&w_frn,relname,relowner))
	{
	    /*
	    ** No relations meet qualification
	    */
	    return(OK);
	}
/* # line 1116 "uirelhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"uirelhlp9",6507,8373);
      IIputdomio((short *)0,1,32,0,relname_buf);
      IIputdomio((short *)0,1,32,0,relowner_buf);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"uirelhlp9",6507,8373);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner, create_date, table_type, table_subtyp\
e, table_version, system_use, table_stats, table_indexes, is_readonly,\
 num_rows, storage_structure, is_compressed, duplicate_rows, unique_ru\
le, number_pages, overflow_pages, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"table_integrities, table_permits, all_to_all, ret_to_all, row_width, \
is_journalled, multi_locations, view_base, location_name, expire_date,\
 key_is_compressed, table_pagesize, table_relversion, table_reltotwid,\
 table_reltcpri, phys_partitions, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"partition_dimensions from iitables where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relname_buf);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,relowner_buf);
        IIexDefine(0,(char *)"uirelhlp9",6507,8373);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE9;
IIrtB9:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,help_info[i].table_name);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,help_info[i].table_owner);
      IIgetdomio((short *)0,1,32,UI_DATESIZE+1-1,help_info[i].create_date);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_type);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_subtype);
      IIgetdomio((short *)0,1,32,8,help_info[i].table_version);
      IIgetdomio((short *)0,1,32,1,help_info[i].system_use);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_stats);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_indexes);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_readonly);
      IIgetdomio((short *)0,1,30,4,&help_info[i].num_rows);
      IIgetdomio((short *)0,1,32,16,help_info[i].storage_structure);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_compressed);
      IIgetdomio((short *)0,1,32,1,help_info[i].duplicate_rows);
      IIgetdomio((short *)0,1,32,1,help_info[i].unique_rule);
      IIgetdomio((short *)0,1,30,4,&help_info[i].number_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].overflow_pages);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_integrities);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_permits);
      IIgetdomio((short *)0,1,32,1,help_info[i].all_to_all);
      IIgetdomio((short *)0,1,32,1,help_info[i].ret_to_all);
      IIgetdomio((short *)0,1,30,4,&help_info[i].row_width);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_journalled);
      IIgetdomio((short *)0,1,32,1,help_info[i].multi_locations);
      IIgetdomio((short *)0,1,32,1,help_info[i].view_base);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,help_info[i].location_name);
      IIgetdomio((short *)0,1,30,4,&save_date[i]);
      IIgetdomio((short *)0,1,32,1,help_info[i].key_is_compressed);
      IIgetdomio((short *)0,1,30,4,&help_info[i].page_size);
      IIgetdomio((short *)0,1,30,4,&help_info[i].rel_version);
      IIgetdomio((short *)0,1,30,4,&help_info[i].rel_totwid);
      IIgetdomio((short *)0,1,30,2,&help_info[i].cache_pri);
      IIgetdomio((short *)0,1,30,2,&help_info[i].phys_partitions);
      IIgetdomio((short *)0,1,30,2,&help_info[i].partition_dimensions);
      if (IIerrtest() != 0) goto IIrtB9;
/* # line 1193 "uirelhlp.sc" */	/* host code */
		if  (STtrmwhite(help_info[i].table_name) == 0)
		{
		    STcopy(ERx(" "),help_info[i].table_name);
		}
		if  (STtrmwhite(help_info[i].table_owner) == 0)
		{
		    STcopy(ERx(" "),help_info[i].table_owner);
		}
		STcopy(help_info[i].create_date, help_info[i].alter_date);
		i++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE9:;
  }
/* # line 1204 "uirelhlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 1205 "uirelhlp.sc" */	/* host code */
	if ( errnum != OK )
	{
	    return(errnum);
	}
	if  ((i - *ind) > 1)	/* Should never find more than one! */
	{
	    return(FAIL);
	}
	if ( !IIUIdct_use_iitables() )
	{
/* # line 1215 "uirelhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"uirelhlp10",6508,31344);
      IIputdomio((short *)0,1,32,0,relname_buf);
      IIputdomio((short *)0,1,32,0,relowner_buf);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"uirelhlp10",6508,31344);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_stats, table_indexes, is_readonly, num_rows, storage_str\
ucture, is_compressed, duplicate_rows, unique_rule, number_pages, over\
flow_pages, row_width, key_is_compressed from iiphysical_tables where \
table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relname_buf);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,relowner_buf);
        IIexDefine(0,(char *)"uirelhlp10",6508,31344);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE10;
IIrtB10:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,help_info[i].table_stats);
      IIgetdomio((short *)0,1,32,1,help_info[i].table_indexes);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_readonly);
      IIgetdomio((short *)0,1,30,4,&help_info[i].num_rows);
      IIgetdomio((short *)0,1,32,16,help_info[i].storage_structure);
      IIgetdomio((short *)0,1,32,1,help_info[i].is_compressed);
      IIgetdomio((short *)0,1,32,1,help_info[i].duplicate_rows);
      IIgetdomio((short *)0,1,32,1,help_info[i].unique_rule);
      IIgetdomio((short *)0,1,30,4,&help_info[i].number_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].overflow_pages);
      IIgetdomio((short *)0,1,30,4,&help_info[i].row_width);
      IIgetdomio((short *)0,1,32,1,help_info[i].key_is_compressed);
      if (IIerrtest() != 0) goto IIrtB10;
/* # line 1248 "uirelhlp.sc" */	/* host code */
			if  (STtrmwhite(help_info[i].table_name) == 0)
			{
			    STcopy(ERx(" "),help_info[i].table_name);
			}
			if  (STtrmwhite(help_info[i].table_owner) == 0)
			{
			    STcopy(ERx(" "),help_info[i].table_owner);
			}
			STcopy(help_info[i].create_date,
			       help_info[i].alter_date);
			i++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE10:;
  }
/* # line 1260 "uirelhlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 1261 "uirelhlp.sc" */	/* host code */
		if  (errnum != OK )
		{
		    return(errnum);
		}
		if  ((i - *ind) > 1)	/* Should never find more than one! */
		{
		    return(FAIL);
		}
	}
	/* Should never be true for an r3 database but I'll leave it */
	if (help_info[i].page_size == 0)
	    help_info[i].page_size = 2048;
	*ind = i;
	return(OK);
}
