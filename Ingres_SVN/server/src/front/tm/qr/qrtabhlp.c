# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 1988, 2005 Ingres Corporation
**
**
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<adf.h>
# include	<afe.h>
# include	<cm.h>
# include	<qr.h>
# include	<ui.h>
# include	<ug.h>
# include	<st.h>
# include	<cv.h>
# include	<er.h>
# include	<uigdata.h>
# include	"erqr.h"
# include	"qrhelp.h"
# include "qrcol.h"
/* NO_OPTIM=hp3_us5 */
/**
** Name:	qrtabhlp.qsc -	process help command for tabular output.
**
** Description:
**	Help command routines for those objects for which the output
**	will be somewhat tabular.  Currently this provides help on tables
**	and on indexes.  Routines are:
**	This file executes the help table statment, called
**	via the dohelp routine. This file contains the routines:
**
**	IIQRthp_TableHelp	does the help table tablename case
**	IIQRihp_IndexHelp	does the help index indexname case
**
** History:
**	29-sep-88 (bruceb)
**		Much stolen from qrtblhlp.qsc.  Modified to handle just
**		tables, and added code for just indexes.
**	03-oct-88 (kathryn)	bug 3366
**		STtrmwhite from iistats.column_name so later comparison
**		to iicolumns.column_name will work and print opt stats.
**	12-oct-88 (bruceb)
**		Only display permit and integrity information if the DBA.
**		Step one on route to greater security.
**	18-nov-88 (bruceb)
**		Use new utility routine to obtain table_information.
**		Continue to use iicolumns to obtain the per-column info.
**	01-dec-88 (bruceb)
**		Changed IIQRthp code to accept being called without knowing
**		the name of the object owner.  (For performance, not calling
**		IIQRtbo_TableOwner before this routine for 'help table X'.)
**
**		NOTE: IIQRihp code not changed since 'help index' is less
**		frequent a request; if that change is desired, would involve
**		knowledge of storage location of index information in IIQRmdh
**		routine, and change to index-owner routine.
**	14-dec-88 (bruceb)
**		Additional performance changes.  First time into the 'help
**		table' code, obtain most of the fast ER strings, assign
**		to local statics, and avoid ERget thereafter.
**	19-may-89 (teresal)
**		Squeezed out an extra space to display the new Terminator
**		data type name "object_key" in the column information area.  
**		Increased the data type display from 9 to 10 characters while
**		decreasing the data length display from 7 to 6 characters.  
**		This will have little visible effect.
**      26-may-89 (teresal)
**              Quick fix to display logical keys by using the internal data
**              type column in iicolumns for dbms's that allow logical keys.
**              When there is time, this should be rewritten to use
**              dynamic SQL.
**	25-jul-89 (teresal)
**		Bug #6304 fix.  Changed IIQRthp so Secondary indexes column 
**		names displayed under "Keyed On" will be in the correct order, 
**		i.e., displayed in key sequence order.
**		to be displayed in the correct key sequence order
**	17-aug-89 (teresal)
**		Made a change to distinguish logical keys and UDT from 'normal'
**		datatypes.  Only logical keys and UDT's will display the 
**		internal datatype and length from iicolumns.
**	29-aug-90 (kathryn)
**		Integration of Porting changes into mainline. Turn off
**		optimization for hp3_us5 (griffin 02-apr-90).
**	25-jul-91 (johnr)
**		Removed NO_OPTIM hp3_us5 which is no longer needed.
**	7-jul-19922 (rogerl)
**		Want secondary indices to display if its a system table too.
**		STtrmwhite changed to qrtrimwhite.
**	9-dec-1992 (rogerl)
**		Change qrprintf's involving object/owner names to qrdiprintf()
**		Which will ensure display of a delimited ident is quoted OK.
**	21-apr-93 (dwilson)
**		Turn off optimization for hp3_us5.  This will enable 
**		this to compile without the global optimizer running
**		out of memory.
**      11-may-94 (huffman)
**              Bug 62407
**              in IIQRpci_PrintColumnInfo, there is a call to qrprintf
**              passing a floating point.  'qrprintf' really is
**              doing variable argument list, and floating point
**              is not being successfully passed through to STprintf
**              This is a nasty problem, because the correct answer
**              is to modify qrprintf to accept variable arg list, and
**              VAX and AXP does not implement it in the same way.
**      08-mar-1995 (newca01) Bug 59235 
**              Added code to display "n/a" for defaults attribute for 
**              nullable columns - like the QBF (tables utility) display
**	06-may-96 (chech02)
**		Cast function arguments to i4  in qrprintf(%d) calls.
**	03-jun-96 (chech02)
**		Cast constant to i4  in qrprintf("*.s") calls.
**      06-mar-1996 (stial01)
**              Added page size to help table
**	21-may-1996 (shero03)
**	    Added "hidata" compression
**	17-jul-1996 (ramra01)
**	        Added relversion and reltotwid for help table info as
**		part of alter table add/drop column.
**	25-Mar-1997 (jenjo02)
**	    Table priority project:
**	    Added table's cache priority to help table.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	2-Jun-2004 (schka24)
**	    Various changes for help on partitioned tables.
**	25-Jun-2004 (schka24)
**	    More changes to add partitioning scheme to help table.
**	16-Mar-2005 (kodse01)
**	    BUG 113504
**	    Changed the display of permissions in help table.
**	19-jul-2005 (devjo01)
**	    Bug 114878 - Histogram avg repeat count > 99,999,999.9 overflows
**	    HELP TABLE display field.
**	10-May-2006 (jenjo02)
**	    Add "clustered " to structure type display.
**	30-May-2006 (jenjo02)
**	    Deleted special "clustered" stuff; "structure" will now show
**	    if clustered.
**/
/* # define's */
/* GLOBALDEF's */
/* extern's */
FUNC_EXTERN	VOID		qrprintf();
FUNC_EXTERN	VOID		qrtrimwhite( char *str );
FUNC_EXTERN	STATUS		IIQRgtl_GmtToLocal();
FUNC_EXTERN	VOID		qrdiprintf( u_i4 nth, QRB *qrb, char *fmt,
				... );
VOID		IIQRdci_DoColumnInfo();
VOID		IIQRpci_PrintColumnInfo();
VOID		IIQRthp_TableHelp();
VOID		IIQRihp_IndexHelp();
bool		IIQRget_is_secure();
static	VOID	set_those_wacky_strings();
struct QRSTAT_STRUCT
{
	bool	unique;
	char	col[FE_MAXNAME+1];
	float	avgcount;
};
/*
** Pointers to most of the fast ER strings used in 'help table'.
*/
static	bool	set_fast_strings = FALSE;
static	char	*s12_Name = NULL;
static	char	*s13_Owner = NULL;
static	char	*s14_Location = NULL;
static	char	*s15_Type = NULL;
static	char	*s16_system_catalog = NULL;
static	char	*s17_user_table = NULL;
static	char	*s18_secondary_index_on = NULL;
static	char	*s1C_Row_width = NULL;
static	char	*s1D_Number_of_rows = NULL;
static	char	*s1E_Storage_structure = NULL;
static	char	*s1F_compressed = NULL;
static	char	*s20_with_unique_keys = NULL;
static	char	*s21_Number_of_pages = NULL;
static	char	*s22_Overflow_data_pages = NULL;
static	char	*s23_Journaling = NULL;
static	char	*s24_index_enabled = NULL;
static	char	*s25_enabled = NULL;
static	char	*s26_at_next_checkpoint = NULL;
static	char	*s27_disabled = NULL;
static	char	*s29_Permissions = NULL;
static	char	*s2A_including_ALL_to_ALL = NULL;
static	char	*s2B_and_RETRIEVE_to_ALL = NULL;
static	char	*s2C_yes__including_RETRIE = NULL;
static	char	*s2D_yes_n = NULL;
static	char	*s2F_none_n = NULL;
static	char	*s30_Integrities = NULL;
static	char	*s32_Optimizer_statistics = NULL;
static	char	*s34_index_stats = NULL;
static	char	*s35_yes_see_stats_below = NULL;
static	char	*s36_Column_Information = NULL;
static	char	*s38_avg_count = NULL;
static	char	*s3A_per_value = NULL;
static	char	*s3B_yes_null = NULL;
static	char	*s3C_no_null = NULL;
static	char	*s3D_yes_default = NULL;
static	char	*s3E_no_default = NULL;
static  char    *s39_n_a_default = NULL;
static	char	*s3F_unique_stats = NULL;
static	char	*s41_Secondary_indices = NULL;
static	char	*s43_Secondary_index_none = NULL;
static	char	*s5A_unknown_ = NULL;
static	char	*s5B_topline_key = NULL;
static	char	*s5C_mainline_coltitle = NULL;
static	char	*s5D_mainline_dtype_name = NULL;
static	char	*s5E_mainline_dtype_len = NULL;
static	char	*s5F_mainline_nulls = NULL;
static	char	*s60_mainline_defaults = NULL;
static	char	*s61_mainline_seq = NULL;
static	char	*s62_index_name = NULL;
static	char	*s63_structure = NULL;
static	char	*s64_keyed_on = NULL;
static	char	*s65_and_SELECT_to_ALL = NULL;
static	char	*s66_yes__including_SELECT = NULL;
static	char	*s6E_secondary_index = NULL;
static	char	*s6F_Version = NULL;
static	char	*s70_Created = NULL;
static	char	*s71_Location = NULL;
static	char	*s72_Duplicate_Rows = NULL;
static	char	*s73_allowed = NULL;
static	char	*s74_not_allowed = NULL;
static	char	*s7E_Base_table = NULL;
static	char	*s7F_yes = NULL;
static	char	*s80_no = NULL;
static  char    *s98_objkey_sm = NULL;
static  char    *s99_objkeynosm = NULL;
static  char    *s9A_tblkey_sm = NULL;
static  char    *s9B_tblkeynosm = NULL;
static	char	*sA9_user_table_part = NULL;
static	char	*sAA_phys_parts = NULL;
static	char	*sAB_part_dims = NULL;
static	char	*sB0_part_def = NULL;
static  char    *s110_Compression = NULL;
static  char    *s111_data = NULL;
static  char    *s112_key = NULL;
static  char	*s113_user_synonym = NULL;
static  char    *s123_hidata = NULL;
static  char    *s10_access = NULL;
static  char    *s201_row = NULL;
static  char    *s202_table = NULL;
static  char    *s203_Page_size = NULL;
static	char	*s204_rel_version = NULL;
static  char    *s205_Tot_width = NULL;
static  char    *s206_cache_pri = NULL;
/*{
** Name:	IIQRthp_TableHelp - perform help table <table> for one table nm
**
** Description:
**	Produce output in response to 'help table tablename' command.
**
** Inputs:
**	qrb		ubiquitous qrb
**	t		table name
**	town		table owner
**
** Outputs:
**	none
**
** Side Effects:
**	puts the help information for a table into the qrb's output buffer
**
** History:
**	29-sep-88 (bruceb)
**		Mostly swiped from qrtblhelp().  Modified to handle just
**		tables and indexes.
**	14-mar-89 (kathryn)	Bug #1464
**		If language is SQL then print "vchar" not "text" for vchar
**		data types.
**	11-apr-89 (kathryn)	Bug #4473
**		Added BEGIN and END for the non-singleton sql select statements.
**		(checking for permits and integrities.)
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	15-mar-1991 (kathryn)
**	    Print Column Information for Indexes as well as tables.
**	21-mar-1991 (kathryn)
**	    Integrate Desktop Porting changes for " 6-x_PC_80x86 ".
**	    Added ifdef for HIGHC.
**	20-mar-1991 (kathryn)
**	    Add Compression information to HELP TABLE and HELP INDEX.
**	    Remove compression info from "Storage Structure" line
**	    Add new "Compression" line.  
**	7-jul-19922 (rogerl)
**	    Remove section that prohibits output of secondary indices
**	    if it's a system table.  Change params to IIUIrel_help() as
**	    per 6.5 library changes.  STtrmwhite to qrtrimwhite.
**	1-oct-1992 (rogerl)
**	    Need to use owner arg to IIUIrel_help
**	12-oct-1992 (rogerl)
**	    Add argument to E_QR0025_err_ret_help_info
**	11-mar-1993 (rogerl)
**	    qrtrimwhite() iname; bug 49808
**	22-jun-1999 (musro02)
**          Removed conditional code for __HIGHC__
**      13-feb-2000 (musro02)
**          On advice from Orlan Canon, 
**          Removed conditional code for __HIGHC__
**          He believes all HIGHC specific code in unnecessary, but I'll
**          only remove this piece because it causes a problem.
**          (specific symptom was that PUSHF4 does not exist on nc4_us5)
**	31-may-01 (inkdo01)
**	    Add code to check for composite histograms on secondary indexes
**	    when displaying "help table" statistics message.
**	2-Jun-2004 (schka24)
**	    Partitioned table help; fix messed up indentations.
**	25-Jun-2004 (schka24)
**	    Add partitioning details to partitioned table help.
*/
VOID
IIQRthp_TableHelp(qrb, t, town)
QRB	*qrb;
  char *t;
  char *town;
{
    UI_HELP_INFO	*help_struct;
    char	*ttype;
    char	*tstype;
    char	*crdate;
    char	*ststrct;
    char	*exdate;
  char basetname[FE_MAXNAME + 1];
  char locat[25];
  char stostruct[17];
  char compr[9];
  char uniq[9];
  f4 acount;
  i4 dummy;
  char iname[FE_MAXNAME + 1];
  char oldiname[FE_MAXNAME + 1];
  char colname[FE_MAXNAME + 1];
  char icolname[FE_MAXNAME + 1];
  i4 qrows;
  i4 seq;
  i4 kseq;
    struct	QRSTAT_STRUCT statarray[DB_GW2_MAX_COLS + 1];
    i4		i, maxstats;
    bool	stats_exist = FALSE;
    bool	firstime = TRUE;
    char	ststruct[17];
    char	*vwbase_value = NULL;
    char	*etype;
    char        compres_buf[20];
    if (IIUIrel_help(t, town, &help_struct) != OK)
    {
	qrdiprintf( QRARG1, qrb, ERget(E_QR0025_err_ret_help_info), t);
	return;
    }
    else
    {
	ttype = help_struct->table_type;
	/* No object found of name t */
	if (help_struct == NULL)
	{
	    if (qrb->step == HELP_TABLE)
	    {
		qrdiprintf( QRARG1, qrb, ERget(F_QR0008_Table_does_not_exist),
		    t );
	    }
	    else
	    {
		/*
		** This routine is called only for help on tables and
		** indexes.  For help on an index, IIUIrel_help() should
		** never return a NULL structure because IIQRmdh() already
		** checked that an index of this name exists.  Obtaining
		** a NULL pointer therefore indicates an error in
		** IIUIrel_help.
		*/
		qrdiprintf( QRARG1, qrb, ERget(E_QR0025_err_ret_help_info), t );
	    }
	    return;
	}
	/* If help table X was requested, but X isn't a table */
	if ((ttype[0] != 'T') && (qrb->step == HELP_TABLE))
	{
	    if (ttype[0] == 'V')
		etype = ERget(F_QR0003_view);
	    else
		etype = ERget(F_QR0004_index);
	    qrdiprintf( QRARG1, qrb, ERget(E_QR0021_not_a_table), t, etype );
	    return;
	}
    }
# ifdef	QRDEBUG
    dump_help_struct(help_struct);
# endif
    if (set_fast_strings == FALSE)
    {
	set_those_wacky_strings();
	set_fast_strings = TRUE;
    }
    crdate = help_struct->create_date;
    tstype = help_struct->table_subtype;
    exdate = help_struct->expire_date;
    ststrct = help_struct->storage_structure;
    town = help_struct->table_owner;
    qrdiprintf( QRARG1, qrb, s12_Name, t );
    qrdiprintf( QRARG1, qrb, s13_Owner, town );
    if (crdate[0] != ' ')
    {
	if (IIQRgtl_GmtToLocal(crdate, crdate) == OK)
	    qrprintf(qrb, s70_Created, crdate);
    }
    if (IIUIdcn_ingres())	/* if it's INGRES */
    {
	qrtrimwhite( help_struct->location_name );
	qrprintf(qrb, s14_Location, help_struct->location_name);
	if (help_struct->multi_locations[0] == 'Y')
	{
	    /* print out additional locations (if any) */
/* # line 425 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp1",7486,25319);
      IIputdomio((short *)0,1,32,0,t);
      IIputdomio((short *)0,1,32,0,town);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtabhlp1",7486,25319);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select location_name, loc_sequence from iimulti_locations where table\
_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,t);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,town);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by loc_sequence");
        IIexDefine(0,(char *)"qrtabhlp1",7486,25319);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,24,locat);
      IIgetdomio((short *)0,1,30,4,&seq);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 432 "qrtabhlp.sc" */	/* host code */
		qrtrimwhite( locat );
		qrprintf(qrb, s71_Location, locat);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	    if (FEinqerr())
	    {
		qraddstr(qrb, ERget(F_QR0069_ret_err_iimulti));
		return;
	    }
	}
	qrputc(qrb, '\n');
    }
    /*
    ** For object type, give strings like:
    **  "user table", "system catalog", "index on basetable",
    **  "link to index" or "import of table"
    */
    qraddstr(qrb, s15_Type);
    if (tstype[0] == 'L')
    {
	if (ttype[0] == ' ')	/* object type is unknown */
	{
	    qraddstr(qrb, ERget(F_QR006A_link));
	}
	else	/* "link to <object>" */
	{
	    qraddstr(qrb, ERget(F_QR006B_link_to));
	}
    }
    else if (tstype[0] == 'I')
    {
	if (ttype[0] == ' ')	/* object type is unknown */
	{
	    qraddstr(qrb, ERget(F_QR006C_import));
	}
	else	/* "import of <object>" */
	{
	    qraddstr(qrb, ERget(F_QR006D_import_of));
	}
    }
    /* Only print out 'system catalog' if a native object. */
    if (((help_struct->system_use[0] == 'S')
	    || (STbcompare(t, 2, ERx("ii"), 2, TRUE) == 0))
	&& (tstype[0] == 'N'))
    {
	qraddstr(qrb, s16_system_catalog);
    }
    else if (ttype[0] == 'T')
    {
	if (help_struct->phys_partitions > 0)
	    qraddstr(qrb, sA9_user_table_part);
	else
	    qraddstr(qrb, s17_user_table);
    }
    else if (ttype[0] == 'I')
    {
/* # line 487 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp2",7487,9724);
      IIputdomio((short *)0,1,32,0,t);
      IIputdomio((short *)0,1,32,0,town);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"qrtabhlp2",7487,9724);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select base_name from iiindexes where index_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,t);
        IIwritio(0,(short *)0,1,32,0,(char *)"and index_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,town);
        IIexDefine(0,(char *)"qrtabhlp2",7487,9724);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,basetname);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 492 "qrtabhlp.sc" */	/* host code */
	if (FEinqerr())
	{
	    qraddstr(qrb, ERget(F_QR0011_ret_err_iiindexes));
	    return;
	}
/* # line 497 "qrtabhlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&qrows,8);
  }
/* # line 498 "qrtabhlp.sc" */	/* host code */
	if (qrows == 0)
	{
	    qraddstr(qrb, s6E_secondary_index);
	}
	else
	{
	    qrdiprintf( QRARG1, qrb, s18_secondary_index_on, basetname );
	}
    }
    else if (ttype[0] == 'S')
    {
	/* routine now must service a list of nodes which may contain
	** synonyms
	*/
	qraddstr(qrb, s113_user_synonym);
    }
    if (exdate[0] != EOS)
    {
	if (IIQRgtl_GmtToLocal(exdate, exdate) == OK)
	    qrprintf(qrb, ERget(F_QR001B_Saved_until), exdate);
    }
    qrprintf(qrb, s6F_Version, help_struct->table_version);
    qrprintf(qrb, s203_Page_size, help_struct->page_size);
    qrprintf(qrb, s206_cache_pri, help_struct->cache_pri);
    qrprintf(qrb, s204_rel_version, help_struct->rel_version);
    if (help_struct->row_width != -1)
    {
	qrprintf(qrb, s205_Tot_width, help_struct->rel_totwid);
	qrprintf(qrb, s1C_Row_width, help_struct->row_width);
    }
    /* display physical attributes */
    if (help_struct->num_rows != -1)
    {
	qrprintf(qrb, s1D_Number_of_rows, help_struct->num_rows);
    }
    CVlower(ststrct);
    qrtrimwhite( ststrct );
    if (ststrct[0] != EOS)
    {
	compres_buf[0] = EOS;
	qraddstr(qrb, s1E_Storage_structure);
	if (help_struct->unique_rule[0] == 'U')
	{
	    qrprintf(qrb, s20_with_unique_keys, ststrct);
	}
	else
	{
	    qrprintf(qrb, ERx("%s\n"), ststrct);
	}
	qraddstr(qrb,s110_Compression);
	if ((help_struct->is_compressed[0] == 'Y') ||
	    (help_struct->is_compressed[0] == 'H'))
	{
	    if (help_struct->is_compressed[0] == 'Y')
	        STcat(compres_buf,s111_data);
	    else
		STcat(compres_buf,s123_hidata);
	    if (help_struct->key_is_compressed[0] == 'Y')
	    {
		STcat(compres_buf,ERx(", "));
		STcat(compres_buf,s112_key);
	    }
	    qrprintf(qrb, ERx("%s\n"),compres_buf);
	}
	else if (help_struct->key_is_compressed[0] == 'Y')
	    qrprintf(qrb, ERx("%s\n"),s112_key);
	else
	    qraddstr(qrb, s2F_none_n);
    }
    /*
    ** Print out 'Duplicate Rows:   [not] allowed'
    ** based on the following information.
    **   If the storage structure is 'heap' than it is always
    **     the case that duplicate rows are allowed.
    **   If some storage structure other than 'heap' is being
    **     used, and the storage structure keys are unique,
    **     than duplicate rows are obviously not allowed.
    **   If not 'heap', and not 'unique keys', than if duplicate
    **     rows information is specified, use that setting.
    */
    if (STcompare(ststrct, ERx("heap")) == 0)
    {
	qrprintf(qrb, s72_Duplicate_Rows, s73_allowed);
    }
    else if (help_struct->unique_rule[0] == 'U')
    {
	qrprintf(qrb, s72_Duplicate_Rows, s74_not_allowed);
    }
    else if (help_struct->duplicate_rows[0] == 'D')
    {
	qrprintf(qrb, s72_Duplicate_Rows, s73_allowed);
    }
    else if (help_struct->duplicate_rows[0] == 'U')
    {
	qrprintf(qrb, s72_Duplicate_Rows, s74_not_allowed);
    }
    if (help_struct->number_pages != -1)
    {
	qrprintf(qrb, s21_Number_of_pages, help_struct->number_pages);
    }
    if (help_struct->overflow_pages != -1)
    {
	qrprintf(qrb, s22_Overflow_data_pages, help_struct->overflow_pages);
    }
    if (help_struct->phys_partitions > 0)
    {
	qrprintf(qrb, sAA_phys_parts, help_struct->phys_partitions);
	qrprintf(qrb, sAB_part_dims, help_struct->partition_dimensions);
    }
    if (IIUIdcn_ingres())
    {
	qraddstr(qrb, s23_Journaling);
	if (ttype[0] == 'I')
	{
	    qraddstr(qrb, s24_index_enabled);
	}
	else if (help_struct->is_journalled[0] == 'Y')
	{
	    qraddstr(qrb, s25_enabled);
	}
	else if (help_struct->is_journalled[0] == 'C')
	{
	    qraddstr(qrb, s26_at_next_checkpoint);
	}
	else
	{
	    qraddstr(qrb, s27_disabled);
	}
	/*
	** Indicate if this table has a view defined on it.
	*/
	if (help_struct->view_base[0] == 'Y')
	{
	    vwbase_value =  s7F_yes;
	}
	else if (help_struct->view_base[0] == 'N')
	{
	    vwbase_value =  s80_no;
	}
	if (vwbase_value != NULL)
	{
	    qrprintf( qrb, s7E_Base_table, vwbase_value );
	}
	if (IIUIdbdata()->isdba)
	{
	    /*
	    ** Show the permissions and integrities on the table
	    ** if they exist and not running against a gateway
	    ** where the information is not available anyway.
	    ** Only show this information if the DBA.
	    */
	    qraddstr(qrb, s29_Permissions);
	    /*
	    ** if permits is NO, then that's that. otherwise,
	    ** check if the special permissions "ret-to-all" or
	    ** "all-to-all" are granted: if so, print the
	    ** appropriate meesage. otherwise, if permits is
	    ** blank, do a lookup on iipermits to see if there
	    ** are permissions granted on this table.
	    ** time consuming, but unavoidable.
	    **/
	    if (help_struct->table_permits[0] != 'N')
	    {
		if (help_struct->table_permits[0] == 'Y') 
		{
		    qraddstr(qrb, s2D_yes_n);
		}
		else	/* perms[0] = ' '; we have to PROBE iipermits*/
		{
		    /*
		    ** this code is copied from the probe below
		    ** so repeat queries can be used. at this point,
		    ** if repeat queries aren't used, the user
		    ** might as well go eat lunch when issuing a
		    ** help command.
		    */
/* # line 674 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp3",7488,13044);
      IIputdomio((short *)0,1,32,0,t);
      IIputdomio((short *)0,1,32,0,town);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtabhlp3",7488,13044);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select permit_number from iipermits where object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,t);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,town);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_type='T'");
        IIexDefine(0,(char *)"qrtabhlp3",7488,13044);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dummy);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 681 "qrtabhlp.sc" */	/* host code */
			/* Bug 4473 - may not be singleton select */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 683 "qrtabhlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&qrows,8);
  }
/* # line 684 "qrtabhlp.sc" */	/* host code */
		    if (qrows > 0)
			qraddstr(qrb, s2D_yes_n);
		    else
			qraddstr(qrb, s2F_none_n);
		}
	    }	/* end if perms != N */
	    else
	    {
		qraddstr(qrb, s2F_none_n);
	    }
	    /* may have to probe integrities too */
	    qraddstr(qrb, s30_Integrities);
	    if (help_struct->table_integrities[0] == 'Y')
	    {
		qraddstr(qrb, s2D_yes_n);
	    }
	    else if (help_struct->table_integrities[0] == 'N')
	    {
		qraddstr(qrb, s2F_none_n);
	    }
	    else
	    {
/* # line 706 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp4",7489,13023);
      IIputdomio((short *)0,1,32,0,t);
      IIputdomio((short *)0,1,32,0,town);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtabhlp4",7489,13023);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select integrity_number from iiintegrities where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,t);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,town);
        IIexDefine(0,(char *)"qrtabhlp4",7489,13023);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&dummy);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 712 "qrtabhlp.sc" */	/* host code */
			/* Bug 4473 - may not be singleton select */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
/* # line 714 "qrtabhlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&qrows,8);
  }
/* # line 715 "qrtabhlp.sc" */	/* host code */
		if (qrows > 0)
		    qraddstr(qrb, s2D_yes_n);
		else
		    qraddstr(qrb, s2F_none_n);
	    }
	}
    }	/* end of non-gateway code */
    /* optimizer code goes here */
    maxstats = 0;
    stats_exist = FALSE;
    if ((ttype[0] != 'I') && (help_struct->table_stats[0] != 'N'))
    {
/* # line 727 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp5",7490,12783);
      IIputdomio((short *)0,1,32,0,t);
      IIputdomio((short *)0,1,32,0,town);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtabhlp5",7490,12783);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select rept_factor, has_unique, column_name from iistats where table_\
name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,t);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,town);
        IIexDefine(0,(char *)"qrtabhlp5",7490,12783);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,31,4,&acount);
      IIgetdomio((short *)0,1,32,8,uniq);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,colname);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 733 "qrtabhlp.sc" */	/* host code */
	    qrtrimwhite( colname );
	    STcopy(colname, statarray[maxstats].col);
	    statarray[maxstats].unique = (uniq[0] == 'Y');
	    statarray[maxstats].avgcount = acount;
	    maxstats++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
	if (FEinqerr())
	{
	    qraddstr(qrb, ERget(F_QR0031_ret_err_iistats));
	    return;
	}
	stats_exist = (maxstats > 0);
    }	/* end if table_stats != N */
    if (!stats_exist && ttype[0] != 'I')
    {
	/* Check for composite histograms on secondary indexes. */
/* # line 749 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp6",7491,20729);
      IIputdomio((short *)0,1,32,0,t);
      IIputdomio((short *)0,1,32,0,town);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"qrtabhlp6",7491,20729);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iistats, iiindexes where base_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,t);
        IIwritio(0,(short *)0,1,32,0,(char *)"and base_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,town);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and table_name=index_name and table_owner=index_owner");
        IIexDefine(0,(char *)"qrtabhlp6",7491,20729);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&qrows);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 753 "qrtabhlp.sc" */	/* host code */
	if (qrows > 0)
	    stats_exist = TRUE;
    }
    qraddstr(qrb, s32_Optimizer_statistics);
    if (ttype[0] == 'I')
    {
	qraddstr(qrb, s34_index_stats);
    }
    else if (stats_exist)
    {
	qraddstr(qrb, s35_yes_see_stats_below);
    }
    else
    {
	qraddstr(qrb, s2F_none_n);
    }
    qrputc(qrb, '\n');
    if (ttype[0] == 'I')
	qraddstr(qrb, ERget(F_QR0085_Index_Col_Info));
    else
	qraddstr(qrb, s36_Column_Information);
    /*
    ** the size calculated immediately below is derived from the sizes
    ** used on the 'info_mainline' below.
    */
    qrprintf(qrb, ERx("%*s"),
	(i4)((FE_MAXNAME + 1) + (10+ 1) + (6 + 1) + (5 + 1) + (8 + 1) + 3), 
	s5B_topline_key);
    if (stats_exist)
	qraddstr(qrb, s38_avg_count);
    else
	qrputc(qrb, '\n');
    qrprintf(qrb, ERx("%-*s %-10s %6s %s %s %s"), (i4)FE_MAXNAME, 
	s5C_mainline_coltitle, s5D_mainline_dtype_name,
	s5E_mainline_dtype_len, s5F_mainline_nulls,
	s60_mainline_defaults, s61_mainline_seq);
    if (stats_exist)
	qraddstr(qrb, s3A_per_value);
    else
	qrputc(qrb, '\n');
    IIQRdci_DoColumnInfo(qrb, statarray, maxstats, t, town);
    if (FEinqerr())
    {
	    qraddstr(qrb, ERget(F_QR0040_ret_err_iicolumns));
	    return;
    }
    if ( ttype[0] != 'I' )
    {
	if (help_struct->table_indexes[0] != 'N')
	{
	    /*
	    ** just throw something illegal in the oldiname
	    ** so it doesn't match, and we think we're printing
	    ** the next index
	    */
	    STcopy(ERx("#*&-@%"), oldiname);
/* # line 809 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp7",7492,21489);
      IIputdomio((short *)0,1,32,0,t);
      IIputdomio((short *)0,1,32,0,town);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtabhlp7",7492,21489);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select ind.index_name, storage_structure, column_name, key_sequence f\
rom iiindexes ind, iiindex_columns col where col.index_name=ind.index_\
name and col.index_owner=ind.index_owner and ind.base_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,t);
        IIwritio(0,(short *)0,1,32,0,(char *)"and ind.base_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,town);
        IIwritio(0,(short *)0,1,32,0,(char *)
"order by index_name, storage_structure, key_sequence");
        IIexDefine(0,(char *)"qrtabhlp7",7492,21489);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,iname);
      IIgetdomio((short *)0,1,32,16,stostruct);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,icolname);
      IIgetdomio((short *)0,1,30,4,&kseq);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 820 "qrtabhlp.sc" */	/* host code */
		if (firstime)	/* print heading */
		{
		    qraddstr(qrb, s41_Secondary_indices);
		    qrprintf(qrb, ERx("%-*s %-*s %-s\n"),
			(i4)FE_MAXNAME, s62_index_name, (i4)10, s63_structure,
			s64_keyed_on);
		}
		qrtrimwhite( icolname );
		/*
		** If the current index name read is not the same
		** as the last index name read, then we have a
		** new index to display. this is because we have
		** a one to many mapping of index column names to
		** indexes, and we're sorting by index name.
		**
		** If the index name IS the same as the last one,
		** then we've got another column name corresponding
		** to our current index, so print the ", colname"
		** rather than the index, storage, and 1st colname.
		**
		** Initially, we fill in the oldiname field with
		** junk, so our first index retrieved will not
		** match with the non-existant "last" index.
		*/
		qrtrimwhite( iname );
		if (STcompare(oldiname, iname) != 0)
		{
		    /* hit return for the previous line on screen */
		    if (!firstime)
			qrputc(qrb, '\n');
		    CVlower(stostruct);
		    STcopy(stostruct, ststruct);
		    qrtrimwhite( ststruct );
		    if (ststruct[0] != EOS)
		    {
			stostruct[10] = EOS;/* for formatting */
			qrdiprintf( QRARG2 | QRARG4, qrb, ERx("%-*s %s %s"),
			    (i4)FE_MAXNAME, iname, stostruct, icolname);
		    }
		    else
		    {
			STcopy(s5A_unknown_, stostruct);
			stostruct[10] = EOS;	/* for formatting */
			qrdiprintf( QRARG2 | QRARG4, qrb, ERx("%-*s %s %s"),
			    (i4)FE_MAXNAME, iname, stostruct, icolname);
		    }
		    firstime = FALSE;
		}
		else
		{
		    qrdiprintf( QRARG1, qrb, ERx(", %s"), icolname);
		}
		STcopy(iname, oldiname);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
	    if (firstime)	/* then not really any indices */
	    {
		qraddstr(qrb, s43_Secondary_index_none);
	    }
	    else
	    {
		qrputc(qrb, '\n');
	    }
	}	/* end if there are secondary indices */
	else
	{
	    qraddstr(qrb, s43_Secondary_index_none);
	}
    }
    qrputc(qrb, '\n');
    /* If a partitioned table, dump out partitioning info */
    if (help_struct->phys_partitions > 0)
    {
	qraddstr(qrb, sB0_part_def);
	qrputc(qrb, '\n');
	qrputc(qrb, '\n');
	IIUIpd_PartitionDesc(help_struct->table_name,
		help_struct->table_owner, help_struct->partition_dimensions,
		FALSE, qraddstr, qrb);
	qrputc(qrb, '\n');
    }
}
/*{
** Name:	IIQRdci_DoColumnInfo - do the "Column Information" help shme
**
** Description:
**	Produce the Column Information help 
**
** Inputs:
**	qrb		ubiquitous qrb
**	statarray	stats info; NULL if called from help <object>
**	maxstats	max index into the stat array 
**
** Outputs:
**	none
**
** Side Effects:
**	puts the help column information into the qrb's output buffer
**
** History:
**	7-jan-1993 (daver)
**		First Written.
**	7-mar-1994 (geri)
**		Queries on column names > 18 characters are not
**		allowed on gateways, so don't do it (bug 57943).
*/
VOID
IIQRdci_DoColumnInfo(qrb, statarray, maxstats, tab_name, tab_owner)
QRB			*qrb;
struct QRSTAT_STRUCT	statarray[];
i4			maxstats;
  char *tab_name;
  char *tab_owner;
{
  QR_COL_INFO colinfo;
	colinfo.maxstats = maxstats;
	if  (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_65) >= 0)
	{
	    if (IIUIdbdata()->dmldef != UI_DML_GTWSQL)
	    {
/* # line 944 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp8",7493,19438);
      IIputdomio((short *)0,1,32,0,tab_name);
      IIputdomio((short *)0,1,32,0,tab_owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtabhlp8",7493,19438);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select column_name, column_datatype, column_length, column_nulls, col\
umn_defaults, key_sequence, column_sequence, column_system_maintained,\
 column_internal_datatype, column_internal_length, column_has_default,\
 column_default_val from iicolumns where ");
        IIwritio(0,(short *)0,1,32,0,(char *)"table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tab_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,tab_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by column_sequence");
        IIexDefine(0,(char *)"qrtabhlp8",7493,19438);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,colinfo.colname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,colinfo.datatype);
      IIgetdomio((short *)0,1,30,4,&colinfo.extern_len);
      IIgetdomio((short *)0,1,32,1,colinfo.nulls);
      IIgetdomio((short *)0,1,32,1,colinfo.defaults);
      IIgetdomio((short *)0,1,30,4,&colinfo.keyseq);
      IIgetdomio((short *)0,1,30,4,&colinfo.seq);
      IIgetdomio((short *)0,1,32,1,colinfo.sysmaint);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,colinfo.idatatype);
      IIgetdomio((short *)0,1,30,4,&colinfo.intern_len);
      IIgetdomio((short *)0,1,32,1,colinfo.has_default);
      IIgetdomio(&colinfo.nulli,1,32,1500,colinfo.def_value);
      if (IIerrtest() != 0) goto IIrtB6;
/* # line 965 "qrtabhlp.sc" */	/* host code */
		IIQRpci_PrintColumnInfo(qrb, &colinfo, statarray);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
	    }
	    else
	    {
	    	/* For gateways, don't query columnnames > 18 chars */
	    	colinfo.sysmaint[0] = EOS;
	    	colinfo.idatatype[0] = EOS;
	    	colinfo.intern_len = 0;
/* # line 975 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp9",7494,17638);
      IIputdomio((short *)0,1,32,0,tab_name);
      IIputdomio((short *)0,1,32,0,tab_owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtabhlp9",7494,17638);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select column_name, column_datatype, column_length, column_nulls, col\
umn_defaults, key_sequence, column_sequence, column_has_default, colum\
n_default_val from iicolumns where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tab_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,tab_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by column_sequence");
        IIexDefine(0,(char *)"qrtabhlp9",7494,17638);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE7;
IIrtB7:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,colinfo.colname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,colinfo.datatype);
      IIgetdomio((short *)0,1,30,4,&colinfo.extern_len);
      IIgetdomio((short *)0,1,32,1,colinfo.nulls);
      IIgetdomio((short *)0,1,32,1,colinfo.defaults);
      IIgetdomio((short *)0,1,30,4,&colinfo.keyseq);
      IIgetdomio((short *)0,1,30,4,&colinfo.seq);
      IIgetdomio((short *)0,1,32,1,colinfo.has_default);
      IIgetdomio(&colinfo.nulli,1,32,1500,colinfo.def_value);
      if (IIerrtest() != 0) goto IIrtB7;
/* # line 992 "qrtabhlp.sc" */	/* host code */
		IIQRpci_PrintColumnInfo(qrb, &colinfo, statarray);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE7:;
  }
	    }
	}
	/* should really be whatever version logical keys came in */
	else if (IIUIdcl_logkeys())
	{
	    /* zap elements corresponding to defaults */
	    colinfo.has_default[0] = EOS;
	    colinfo.def_value[0] = EOS;
	    colinfo.nulli = DB_EMB_NULL;
	    if (IIUIdbdata()->dmldef != UI_DML_GTWSQL)
	    {
/* # line 1006 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp10",7495,24802);
      IIputdomio((short *)0,1,32,0,tab_name);
      IIputdomio((short *)0,1,32,0,tab_owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtabhlp10",7495,24802);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select column_name, column_datatype, column_length, column_nulls, col\
umn_defaults, key_sequence, column_sequence, column_system_maintained,\
 column_internal_datatype, column_internal_length from iicolumns where\
 table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tab_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,tab_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by column_sequence");
        IIexDefine(0,(char *)"qrtabhlp10",7495,24802);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE8;
IIrtB8:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,colinfo.colname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,colinfo.datatype);
      IIgetdomio((short *)0,1,30,4,&colinfo.extern_len);
      IIgetdomio((short *)0,1,32,1,colinfo.nulls);
      IIgetdomio((short *)0,1,32,1,colinfo.defaults);
      IIgetdomio((short *)0,1,30,4,&colinfo.keyseq);
      IIgetdomio((short *)0,1,30,4,&colinfo.seq);
      IIgetdomio((short *)0,1,32,1,colinfo.sysmaint);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,colinfo.idatatype);
      IIgetdomio((short *)0,1,30,4,&colinfo.intern_len);
      if (IIerrtest() != 0) goto IIrtB8;
/* # line 1022 "qrtabhlp.sc" */	/* host code */
		IIQRpci_PrintColumnInfo(qrb, &colinfo, statarray);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE8:;
  }
	    }
	    else
	    {
	    	/* For gateways, don't query columnnames > 18 chars */
	    	colinfo.sysmaint[0] = EOS;
	    	colinfo.idatatype[0] = EOS;
	    	colinfo.intern_len = 0;
/* # line 1032 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp11",7496,8422);
      IIputdomio((short *)0,1,32,0,tab_name);
      IIputdomio((short *)0,1,32,0,tab_owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtabhlp11",7496,8422);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select column_name, column_datatype, column_length, column_nulls, col\
umn_defaults, key_sequence, column_sequence from iicolumns where table\
_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tab_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,tab_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by column_sequence");
        IIexDefine(0,(char *)"qrtabhlp11",7496,8422);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE9;
IIrtB9:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,colinfo.colname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,colinfo.datatype);
      IIgetdomio((short *)0,1,30,4,&colinfo.extern_len);
      IIgetdomio((short *)0,1,32,1,colinfo.nulls);
      IIgetdomio((short *)0,1,32,1,colinfo.defaults);
      IIgetdomio((short *)0,1,30,4,&colinfo.keyseq);
      IIgetdomio((short *)0,1,30,4,&colinfo.seq);
      if (IIerrtest() != 0) goto IIrtB9;
/* # line 1044 "qrtabhlp.sc" */	/* host code */
		IIQRpci_PrintColumnInfo(qrb, &colinfo, statarray);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE9:;
  }
	    }
	}	
	else
	{
	    /* zap elements corresponding to logical keys */
	    colinfo.sysmaint[0] = EOS;
	    colinfo.idatatype[0] = EOS;
	    colinfo.intern_len = 0;
	    /* zap elements corresponding to defaults */
	    colinfo.has_default[0] = EOS;
	    colinfo.def_value[0] = EOS;
	    colinfo.nulli = DB_EMB_NULL;
/* # line 1059 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp12",7497,8404);
      IIputdomio((short *)0,1,32,0,tab_name);
      IIputdomio((short *)0,1,32,0,tab_owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtabhlp12",7497,8404);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select column_name, column_datatype, column_length, column_nulls, col\
umn_defaults, key_sequence, column_sequence from iicolumns where table\
_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tab_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,tab_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by column_sequence");
        IIexDefine(0,(char *)"qrtabhlp12",7497,8404);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE10;
IIrtB10:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,colinfo.colname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,colinfo.datatype);
      IIgetdomio((short *)0,1,30,4,&colinfo.extern_len);
      IIgetdomio((short *)0,1,32,1,colinfo.nulls);
      IIgetdomio((short *)0,1,32,1,colinfo.defaults);
      IIgetdomio((short *)0,1,30,4,&colinfo.keyseq);
      IIgetdomio((short *)0,1,30,4,&colinfo.seq);
      if (IIerrtest() != 0) goto IIrtB10;
/* # line 1072 "qrtabhlp.sc" */	/* host code */
		    IIQRpci_PrintColumnInfo(qrb, &colinfo, statarray);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE10:;
  }
	} 	/* end else before 6.5 and not logical keys */
}
/*{
** Name:	IIQRihp_IndexHelp - perform help index <table> for one index nm
**
** Description:
**	Produce output in response to 'help index indexname' command.
**
** Inputs:
**	qrb		ubiquitous qrb
**	i		index name
**	iown		index owner
**
** Outputs:
**	none
**
** Side Effects:
**	puts the help information for an index into the qrb's output buffer
**
** History:
**	29-sep-88 (bruceb)
**		Written.  Calls IIQRthp_TableHelp if running against INGRES;
**		otherwise uses only the information in iiindexes and
**		iiindex_columns.
**	21-sep-89 (teresal)
**		Added "Type:" to be displayed before "secondary index on xxx".
**		Makes HELP index display more consistent with HELP object 
**		display.
*/
VOID
IIQRihp_IndexHelp(qrb, i, iown)
QRB	*qrb;
  char *i;
  char *iown;
{
  char cdate[26];
  char bsname[FE_MAXNAME + 1];
  char strstruct[17];
  char comprsd[9];
  char uniqrule[9];
  i4 kseq;
  char sortdir[9];
  char colmname[FE_MAXNAME + 1];
	char	*uniqstr = NULL;
	char	*direction = NULL;
	if (IIQRisl_IndexStorageLoc() == USE_IIINDEXES)
	{
/* # line 1125 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp13",7498,8427);
      IIputdomio((short *)0,1,32,0,i);
      IIputdomio((short *)0,1,32,0,iown);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"qrtabhlp13",7498,8427);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select create_date, base_name, storage_structure, is_compressed, uniq\
ue_rule from iiindexes where index_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,i);
        IIwritio(0,(short *)0,1,32,0,(char *)"and index_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,iown);
        IIexDefine(0,(char *)"qrtabhlp13",7498,8427);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,25,cdate);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,bsname);
      IIgetdomio((short *)0,1,32,16,strstruct);
      IIgetdomio((short *)0,1,32,8,comprsd);
      IIgetdomio((short *)0,1,32,8,uniqrule);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1131 "qrtabhlp.sc" */	/* host code */
	    if (FEinqerr())
	    {
		qraddstr(qrb, ERget(F_QR0011_ret_err_iiindexes));
		return;
	    }
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0012_Name), i );
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0013_Owner), iown);
	    if (cdate[0] != ' ')
	    {
		if (IIQRgtl_GmtToLocal(cdate, cdate) == OK)
		    qrprintf(qrb, ERget(F_QR0070_Created), cdate);
	    }
	    qraddstr(qrb, ERget(F_QR0015_Type));
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0018_secondary_index_on), bsname );
	    CVlower(strstruct);
	    qrtrimwhite( strstruct );
	    if (strstruct[0] != EOS)
	    {
		qraddstr(qrb, ERget(F_QR001E_Storage_structure));
		if (comprsd[0] == 'Y')
		{
		    qraddstr(qrb, ERget(F_QR001F_compressed));
		}
		qrprintf(qrb, ERx("%s\n"), strstruct);
	    }
	    if (uniqrule[0] == 'U')
	    {
		uniqstr = ERget(F_QR0074_not_allowed);
	    }
	    else if (uniqrule[0] == 'D')
	    {
		uniqstr = ERget(F_QR0073_allowed);
	    }
	    if (uniqstr != NULL)
	    {
		qrprintf(qrb, ERget(F_QR0084_Dup_index_keys), uniqstr);
	    }
	/* Generic column information */
	qrputc(qrb, '\n');
	qraddstr(qrb, ERget(F_QR0085_Index_Col_Info));
	qrprintf(qrb, ERx("%-*s %s\n"), (i4)FE_MAXNAME, 
	    ERget(F_QR005C_mainline_coltitle),
	    ERget(F_QR0086_Sort_Direction));
/* # line 1174 "qrtabhlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtabhlp14",7499,15571);
      IIputdomio((short *)0,1,32,0,i);
      IIputdomio((short *)0,1,32,0,iown);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtabhlp14",7499,15571);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select column_name, sort_direction, key_sequence from iiindex_columns\
 where index_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,i);
        IIwritio(0,(short *)0,1,32,0,(char *)"and index_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,iown);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by key_sequence");
        IIexDefine(0,(char *)"qrtabhlp14",7499,15571);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE11;
IIrtB11:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,colmname);
      IIgetdomio((short *)0,1,32,8,sortdir);
      IIgetdomio((short *)0,1,30,4,&kseq);
      if (IIerrtest() != 0) goto IIrtB11;
/* # line 1181 "qrtabhlp.sc" */	/* host code */
	    qrtrimwhite( colmname );
	    if (sortdir[0] == 'A')
	    {
		direction = ERget(F_QR0087_ascending);
	    }
	    else	/* std catalog def allows only for 'A' or 'D' */
	    {
		direction = ERget(F_QR0088_descending);
	    }
	    qrdiprintf( QRARG2, qrb, ERx("%-*s %s\n"), (i4)FE_MAXNAME, colmname,
		direction);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE11:;
  }
	}
	else        /* it's INGRES */
	{
	    IIQRthp_TableHelp(qrb, i, iown);
	}
	if (FEinqerr())
	{
	    qraddstr(qrb, ERget(E_QR001F_err_ret_iiindex_col));
	    return;
	}
	qrputc(qrb, '\n');
    }
# ifdef	QRDEBUG
/*{
** Name:	dump_help_struct	- display contents of the help structure
**
** Description:
**	This debug routine prints out the contents of the help structure
**	filled in by the IIUIrel_help routine.
**
** Inputs:
**	help_struct     pointer to the help structure
**
** Outputs:
**	none
**
** Side Effects:
**
** History:
**	02-nov-88 (bruceb)      written.
*/
dump_help_struct(help_struct)
UI_HELP_INFO    *help_struct;
{
	printf("table_name is %s\n", help_struct->table_name);
	printf("table_owner is %s\n", help_struct->table_owner);
	printf("create_date is %s\n", help_struct->create_date);
	printf("alter_date is %s\n", help_struct->alter_date);
	printf("table_type is %s\n", help_struct->table_type);
	printf("table_subtype is %s\n", help_struct->table_subtype);
	printf("table_version is %s\n", help_struct->table_version);
	printf("system_use is %s\n", help_struct->system_use);
	printf("table_stats is %s\n", help_struct->table_stats);
	printf("table_indexes is %s\n", help_struct->table_indexes);
	printf("is_readonly is %s\n", help_struct->is_readonly);
	printf("num_rows is %d\n", help_struct->num_rows);
	printf("storage_structure is %s\n", help_struct->storage_structure);
	printf("is_compressed is %s\n", help_struct->is_compressed);
	printf("duplicate_rows is %s\n", help_struct->duplicate_rows);
	printf("unique_rule is %s\n", help_struct->unique_rule);
	printf("number_pages is %d\n", help_struct->number_pages);
	printf("overflow_pages is %d\n", help_struct->overflow_pages);
	printf("expire_date is %d\n", help_struct->expire_date);
	printf("table_integrities is %s\n", help_struct->table_integrities);
	printf("table_permits is %s\n", help_struct->table_permits);
	printf("all_to_all is %s\n", help_struct->all_to_all);
	printf("ret_to_all is %s\n", help_struct->ret_to_all);
	printf("row_width is %d\n", help_struct->row_width);
	printf("is_journalled is %s\n", help_struct->is_journalled);
	printf("multi_locations is %s\n", help_struct->multi_locations);
	printf("view_base is %s\n", help_struct->view_base);
	printf("location_name is %s\n", help_struct->location_name);
}
# endif	/* QRDEBUG */
VOID
IIQRpci_PrintColumnInfo(qrb, col_info, statarray)
QRB		*qrb;
QR_COL_INFO	*col_info;
struct QRSTAT_STRUCT	statarray[];
{
  char nullc[11];
	i4	i;
	if (set_fast_strings == FALSE)
	{
		set_those_wacky_strings();
		set_fast_strings = TRUE;
	}
	/*
	** If putfunc 'flush' function has been set, then call it
	** for each line of output.  This prevents a table with
	** many columns from generating large amounts of data before
	** getting chance to flush the buffer.
	*/
	if (qrb->putfunc != NULL)
	{
		(*qrb->putfunc)(qrb);
        }
	qrtrimwhite( col_info->colname );
	/*  
	** Logical key processing. If logical keys are not supported,
	** we'll get back EOS in the datatype. Only perform this check if 
	** we're actually supporting logical keys
	*/
	if (col_info->idatatype[0] != EOS)
	{
		/*
		** Assume this is a logical key or a user defined datatype if
		** the column datatype is different than the internal datatype.
		** Display the internal column length and internal datatype for
		** logical keys and UDTs.
		*/
		if (STbcompare(col_info->datatype, 0, 
					col_info->idatatype, 0, TRUE) != 0)
		{
		    col_info->extern_len =  col_info->intern_len;
		    STcopy(col_info->idatatype, col_info->datatype);
		}
	        col_info->datatype[10] = EOS;		/* truncate it here */
	        CVlower(col_info->datatype);
		/*
		** Combine logical key data type descriptions
		** with system_maintained info.
		*/
		if (STbcompare(col_info->datatype, 10, 
					ERx("object_key"), 10, TRUE) == 0)
		{
		    if (col_info->sysmaint[0] == 'Y')
			STcopy(s98_objkey_sm, col_info->datatype);
		    else
			STcopy(s99_objkeynosm, col_info->datatype);
		}
		else if (STbcompare(col_info->datatype, 
				10, ERx("table_key "), 10, TRUE) == 0)
		{
		    if (col_info->sysmaint[0] == 'Y')
			STcopy(s9A_tblkey_sm, col_info->datatype);
	            else
			STcopy(s9B_tblkeynosm, col_info->datatype);
		}
	} /* end if logical key support */
	else
	{
	        col_info->datatype[10] = EOS;		/* truncate it here */
	        CVlower(col_info->datatype);
	}
        /* Bug #1464	-If SQL print vchar not text for data type */
        if ((qrb->lang == DB_SQL) && 
		(STcompare(col_info->datatype,ERx("text      ")) == 0))
        {
		STcopy (ERx("vchar     "),col_info->datatype);
        }
	if (col_info->nulls[0] == 'Y')
		STcopy(s3B_yes_null, nullc);
	else
		STcopy(s3C_no_null, nullc);
	/* See if we're doing old defaults or 6.5 defaults */
	if (col_info->has_default[0] == EOS)
	{
		/* B59235 - 08-mar-1995 newca01 - added code to make 
		** sql help display like the QBF (tables utility display
		** for pre 6.5 tables - which includes STAR 
		*/ 
		if (col_info->nulls[0] == 'Y')
		{ 
			STcopy(s39_n_a_default, col_info->def_type);
		}
		else
		{
		/* Old, pre 6.5 defaults */
		if (col_info->defaults[0] == 'Y')
			STcopy(s3D_yes_default, col_info->def_type);
		else
			STcopy(s3E_no_default, col_info->def_type);
		}
	}
	else
	{
		/* New, spiffy defaults for 6.5 and on */
		IIUIgdt_GetDefaultType(col_info->def_value,
			col_info->nulli,
			col_info->has_default,
			col_info->nulls, 
			col_info->def_type);
	}
	/*
	** if the datatype is an ADT (like date or money), the
        ** length will be 0. to print this is bizarre, so just
        ** display blanks under the "length" heading if the
        ** type has a length of 0
        */
        if (col_info->extern_len == 0)
	    qrdiprintf( QRARG2, qrb, ERx("%-*s %s %6s %s %s "),
		(i4)FE_MAXNAME, col_info->colname, col_info->datatype, 
		ERx(" "), nullc, col_info->def_type);
        else
	    qrdiprintf( QRARG2, qrb, ERx("%-*s %s %6d %s %s "),
		(i4)FE_MAXNAME, col_info->colname, col_info->datatype, 
		col_info->extern_len, nullc, col_info->def_type);
        /*
        ** Print the 'Key Seq' value for the column.  Only print it if
        ** non-zero.
        */
        if (col_info->keyseq == 0)
        {
	    qraddstr(qrb, ERx("    "));
        }
        else
        {
	    qrprintf(qrb, ERx("%3d "), col_info->keyseq);
        }
	/*
	** If stats exist, then flip through the stat array.
	** If it has unique values, print "unique";
	** Otherwise print the avg count per value.
	** Once we find the column in the stat array, quit the loop.
	**
	**     If actual column didn't have stats on it, the
	**     carriage return at the end of the loop will, in effect,
	**     print a blank for this column of help.
	*/
	if (col_info->maxstats > 0 && statarray != NULL )
	{
		    for (i = 0; i < col_info->maxstats; i++)
		    {
		        if (STcompare(statarray[i].col,
						col_info->colname) == 0)
		        {
			    if (statarray[i].unique)
			    {
			        qraddstr(qrb, s3F_unique_stats);
			    }
			    else
			    {
/*
**              Bug 62407
**              in IIQRpci_PrintColumnInfo, there is a call to qrprintf
**              passing a floating point.  'qrprintf' really is
**              doing variable argument list, and floating point
**              is not being successfully passed through to STprintf
**              This is a nasty problem, because the correct answer
**              is to modify qrprintf to accept variable arg list, and
**              VAX and AXP does not implement it in the same way.
**
**              The best solution would be to fix qrprintf, so that 
**              would take argument list.  That may be the best solution
**              but would require much more time to implement, and so
**              far this bug only exposes itself when doing 'help table'
**              in sql.
*/
                               char fbuffer[20];
			       if ( statarray[i].avgcount <= 99999999.9 )
			       {
				    STprintf(fbuffer, "%10.1f", 
					statarray[i].avgcount,
					qrb->adfscb->adf_decimal.db_decimal);
			       }
			       else
			       {
				    STprintf(fbuffer, "%10ld", 
					(i4)statarray[i].avgcount);
			       }
                               qraddstr(qrb, fbuffer);
			    }
			    break;
		        }
		    }	/* end for i to maxstats */
	}	/* end if stats_exist */
	qrputc(qrb, '\n');
}
static VOID
set_those_wacky_strings()
{
	s12_Name = ERget(F_QR0012_Name);
	s13_Owner = ERget(F_QR0013_Owner);
	s14_Location = ERget(F_QR0014_Location);
	s15_Type = ERget(F_QR0015_Type);
	s16_system_catalog = ERget(F_QR0016_system_catalog);
	s17_user_table = ERget(F_QR0017_user_table);
	s18_secondary_index_on = ERget(F_QR0018_secondary_index_on);
	s1C_Row_width = ERget(F_QR001C_Row_width);
	s1D_Number_of_rows = ERget(F_QR001D_Number_of_rows);
	s1E_Storage_structure = ERget(F_QR001E_Storage_structure);
	s1F_compressed = ERget(F_QR001F_compressed);
	s20_with_unique_keys = ERget(F_QR0020_with_unique_keys);
	s21_Number_of_pages = ERget(F_QR0021_Number_of_pages);
	s22_Overflow_data_pages = ERget(F_QR0022_Overflow_data_pages);
	s23_Journaling = ERget(F_QR0023_Journaling);
	s24_index_enabled = ERget(F_QR0024_index_enabled);
	s25_enabled = ERget(F_QR0025_enabled);
	s26_at_next_checkpoint = ERget(F_QR0026_at_next_checkpoint);
	s27_disabled = ERget(F_QR0027_disabled);
	s29_Permissions = ERget(F_QR0029_Permissions);
	s2A_including_ALL_to_ALL = ERget(F_QR002A_including_ALL_to_ALL);
	s2B_and_RETRIEVE_to_ALL = ERget(F_QR002B_and_RETRIEVE_to_ALL);
	s2C_yes__including_RETRIE = ERget(F_QR002C_yes__including_RETRIE);
	s2D_yes_n = ERget(F_QR002D_yes_n);
	s2F_none_n = ERget(F_QR002F_none_n);
	s30_Integrities = ERget(F_QR0030_Integrities);
	s32_Optimizer_statistics = ERget(F_QR0032_Optimizer_statistics);
	s34_index_stats = ERget(F_QR0034_index_stats);
	s35_yes_see_stats_below = ERget(F_QR0035_yes_see_stats_below);
	s36_Column_Information = ERget(F_QR0036_Column_Information);
	s38_avg_count = ERget(F_QR0038_avg_count);
	s3A_per_value = ERget(F_QR003A_per_value);
	s3B_yes_null = ERget(F_QR003B_yes_null);
	s3C_no_null = ERget(F_QR003C_no_null);
	s3D_yes_default = ERget(F_QR003D_yes_default);
	s3E_no_default = ERget(F_QR003E_no_default);
	s39_n_a_default = ERget(F_QR0039_n_a_default); 
	s3F_unique_stats = ERget(F_QR003F_unique_stats);
	s41_Secondary_indices = ERget(F_QR0041_Secondary_indices);
	s43_Secondary_index_none = ERget(F_QR0043_Secondary_index_none);
	s5A_unknown_ = ERget(F_QR005A_unknown_);
	s5B_topline_key = ERget(F_QR005B_topline_key);
	s5C_mainline_coltitle = ERget(F_QR005C_mainline_coltitle);
	s5D_mainline_dtype_name = ERget(F_QR005D_mainline_dtype_name);
	s5E_mainline_dtype_len = ERget(F_QR005E_mainline_dtype_len);
	s5F_mainline_nulls = ERget(F_QR005F_mainline_nulls);
	s60_mainline_defaults = ERget(F_QR0060_mainline_defaults);
	s61_mainline_seq = ERget(F_QR0061_mainline_seq);
	s62_index_name = ERget(F_QR0062_index_name);
	s63_structure = ERget(F_QR0063_structure);
	s64_keyed_on = ERget(F_QR0064_keyed_on);
	s65_and_SELECT_to_ALL = ERget(F_QR0065_and_SELECT_to_ALL);
	s66_yes__including_SELECT = ERget(F_QR0066_yes__including_SELECT);
	s6E_secondary_index = ERget(F_QR006E_secondary_index);
	s6F_Version = ERget(F_QR006F_Version);
	s70_Created = ERget(F_QR0070_Created);
	s71_Location = ERget(F_QR0071_Location);
	s72_Duplicate_Rows = ERget(F_QR0072_Duplicate_Rows);
	s73_allowed = ERget(F_QR0073_allowed);
	s74_not_allowed = ERget(F_QR0074_not_allowed);
	s7E_Base_table = ERget(F_QR007E_Base_table);
	s7F_yes = ERget(F_QR007F_yes);
	s80_no = ERget(F_QR0080_no);
	s98_objkey_sm = ERget(F_QR0098_objkey_sm);
	s99_objkeynosm = ERget(F_QR0099_objkeynosm);
	s9A_tblkey_sm = ERget(F_QR009A_tblkey_sm);
	s9B_tblkeynosm = ERget(F_QR009B_tblkeynosm);
	sA9_user_table_part = ERget(F_QR00A9_user_table_part);
	sAA_phys_parts = ERget(F_QR00AA_phys_parts);
	sAB_part_dims = ERget(F_QR00AB_part_dims);
	sB0_part_def = ERget(F_QR00B0_part_def);
	s110_Compression = ERget(F_QR0110_Compression);
	s111_data = ERget(F_QR0111_data);
	s112_key = ERget(F_QR0112_key);
	s113_user_synonym = ERget(F_QR011A_user_synonym);
	s123_hidata = ERget(F_QR0123_hidata);
	s10_access = ERget(F_QR010A_Access_Label);
	s201_row = ERget(F_QR010C_row);
	s202_table = ERget(F_QR010D_table);
	s203_Page_size = ERget(F_QR00A5_Page_size);
	s204_rel_version = ERget(F_QR00A6_rel_version);
	s205_Tot_width = ERget(F_QR00A7_rel_totwid);
	s206_cache_pri = ERget(F_QR00A8_rel_cachepri);
}
