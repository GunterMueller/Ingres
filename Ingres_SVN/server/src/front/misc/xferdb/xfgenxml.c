# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include       <lo.h>
# include	<me.h>
# include	<cm.h>		 
# include	<st.h>
# include	<er.h>
# include       <si.h>
# include       <lo.h>
# include	<gl.h>
# include	<nm.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<adf.h>
# include	<afe.h>
# include	<ui.h>
# include	<uigdata.h>
# include	<ug.h>
# include	<lc.h>
# include <xf.h>
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include	"erxf.h"
/*
# include <xf.qsh>
*/
#define NULL_DATE_TYPE  -3
#define NULL_CHAR_TYPE  -20
#define NULL_VCHR_TYPE  -21
#define NULL_INT_TYPE   -30
#define NULL_FLT_TYPE   -31
#define NULL_DEC_TYPE   -10
#define NULL_MNY_TYPE   -5
#define NULL_LVCH_TYPE  -22
#define NULL_BYTE_TYPE  -23
#define NULL_VBYTE_TYPE -24
#define NULL_LBYTE_TYPE -25
#define NULL_OBJ_TYPE   -45
IISQLDA *ptr_TableSqlda=NULL;    /* Pointer to the SQLDA containing the table */
IISQLDA *ptrorig_TableSqlda=NULL;    /* Pointer to the SQLDA containing the table */
                                 /* details.                                  */
IISQLDA *ptr_ColSqlda=NULL;      /* Pointer to the SQLDA accessing iicolumns  */
IISQLDA *ptr_TabSqlda=NULL;      /* Pointer to the SQLDA accessing iitables   */
IISQLDA *ptr_KeySqlda=NULL;      /* Pointer to the SQLDA accessing            */
                                 /* ii_key_columns                            */
IISQLDA *ptr_StatSqlda=NULL;     /* Pointer to the SQLDA accessing iistats    */
IISQLDA *ptr_IndexSqlda=NULL;    /* Pointer to the SQLDA accessing iitables to*/
                                 /* get the index name.                       */
IISQLDA *ptr_IndexColSqlda=NULL; /* Pointer to the SQLDA accessing            */
                                 /* iiindex_columns                           */
IISQLHDLR xdatahdlr_struct;	 /*  Data handler to retreive blobs */
typedef struct
{
    char	*arg;
    int		arg_int;
} HDLR_PARAM;
HDLR_PARAM xhdlr_arg;
int Get_HandlerLVCH();
int Get_HandlerLBYTE();
FILE *XMLfile=NULL;          	 /* File pointer                              */
int i_loc_no = 0;
bool printedRowTag = FALSE;
/**
** Name: xfgenxml.sc - support for bulkload of Ingres data to XML format.
**
** Description:
**	This file defines the following functions 
** xmldtd	    - Create a dtd information for the table. 
** xmldbinfo	    - Write a numerical value to the xml file.
** get_schema_nr    - Obtain the schema information from the database.
** get_table_nr	    - Obtain the table count from the database.
** get_version	    - Obtain the version for ingres.
** get_view_nr	    - Obtain the #of views in the database.
** xmlprintcols     - Print the column information for the table. 
** xmltabinfo 	    - Print the structure info for the table
** xmlprintresultset- Print the  resultsetinfo for the table
** xmlprintindex    - Print the index information for the table 
** xmlprintrows     - Print the row information for the table 
** xfxmlloc 	    - Add the additional locations for a table.
** print_docType    - Write a docType header to the xml file.
** xfspecialchars   - Handles writing the special characters, 
**		      <,>,',",& to xml files.
**		
** History:
**	29-aug-2000 (gupsh01) written.
**	06-jun-2001 (gupsh01) modified the xml to include the new dtd. 
**	16-oct-2001 (devjo01) Tru64 weirdness.  If STprintf is passed a
**		string literal as the fmt arg., compiler messes up
**		in passing the address, leading to incorrect output.
**		Also correct argument to LOtos in print_docType.
**		Adjusted procedure body indents to Ingres standard.
**	10-Oct-2001 (hanch04)
**	    Remove writevalue, writelongvalue, writedoublevalue,
**	   	replaced with STprintf,xfwrite.
**	    Move GLOBALDEF out to xfdata, Xf_xmlfile, Xf_xmlfile_data, 
**		Xf_xmlfile_sql, Xf_xml_dtd.
**	    Added blob handler functions, Get_HandlerLVCH, Get_HandlerLBYTE.
**	    Added precision and scale to xml output.
**	    Print the correct column width.
**	    Handle dates, money, decimal and long datatypes output to xml.
**	28-Oct-2001 (kinte01)
**	    Add include of me.h 
**	30-Nov-2001 (hanch04)
**	    When printing a float with STprintf, you must pass the decimal
**	    point character as a parameter.
**	30-Nov-2001 (hanch04)
**          Ensure field validate uses the number separator
**          specified by II_DECIMAL. Repeated resolution of
**          II_DECIMAL is avoided with the use of a static
**          variable. 
**	11-Dec-2001 (hanch04)
**	    Removed double quotes from being printed between is_key and 
**	    key_position.
**	    replaced xfwrite_id with xfwrite.  The names of tables and columns
**	    are already surround by double quotes.
**	19-Dec-2001 (hanch04)
**	    TEXT type columns should be treated like a VARCHAR when printed
**	    the columns length.
**      21-Dec-2001 (hanch04)
**          Added printing out unique_keys, persistent and compression 
**	    for indexes.
**	    Print out NULL for null value columns
**	10-Jan-2001 (hanch04)
**	    Make sure columns of specials types: date, money, blobs, etc
**	    can be nullable.
**	24-Jan-2002 (hanch04)
**	    Removed dt:type that was going to be used for blobs, but microsoft IE
**	    does not support it.
**	07-Feb-2002 (hweho01)
**          Fixed the memory corruption error by allocating sufficent 
**	    space to hold the dbms version data in get_version()   
**          routine.    
**	15-Dec-2003 (hanal04) Bug 111436 INGSRV2632
**          Renamed and rewrote xfspecialchars() to prevent genxml from
**          hanging. A buffer over-run was trashing QU structures and
**          generating a SIGSEGV in QUremove().
**	14-Jun-2004 (schka24)
**	    CM now provides safe way of getting charset name.
**      02-May-2005 (gilta01) bug 112138 INGCBT522
**          Added new flag to xfxmlwrite() which allows the caller to indicate
**          whether special characters should be expanded.
**	30-Aug-2005 (thaju02)
**	    Null terminate blob seg_buf.
**	28-Oct-2005 (bonro01)
**	    Specify i4 instead of long since a long might be 4 or 8 bytes
**	    depending on what platform this is compiled on.
**
**/
/* # define's */
#define 	IISQ_DTE_LEN 	25
/* Define the money output length as defined in adumoney.h */
# define	AD_5MNY_OUTLENGTH	20
/* GLOBALDEF's */
GLOBALREF       TXT_HANDLE      *Xf_xmlfile;
GLOBALREF       TXT_HANDLE      *Xf_xmlfile_data;
GLOBALREF       TXT_HANDLE      *Xf_xmlfile_sql;
GLOBALREF       TXT_HANDLE      *Xf_xml_dtd;
/* extern's */
GLOBALREF bool With_multi_locations;
FUNC_EXTERN ADF_CB	*FEadfcb();
/*{
** Name: xmldtd -   Create a dtd information for the table. 
**					 
**
** Description:
**        xmldtd creates the dtd for the table as per the information
**		  passed into the XF_TABINFO block.
**
** The following symbols are used, as per standards
**		* zero or more occurrences of that element.
**		+ one or more occurrance of that element.
**		? zero or one occurrence of that element.
**		
** Inputs:   NONE
**		internal_dtd :  Whether the DTD is internal.
**		title_doctype:  The name to be used in the doctype tag. 
**				By default it is IngresDoc.
** Outputs:
**
**	Returns:
**
** History:
**	29-aug-2000 (gupsh01) written.
**	06-jun-2001 (gupsh01) 
**	    Modified to include the internal dtd flag and the title_doctype.
**	08-jun-2001 (gupsh01)
**	    Fixed typo in xmldtd.
**	12-jun-2001 (gupsh01)
**	    Added the IngresDoc declaration inside the ingres dtd.
**	17-Sep-2001 (hanch04)
**	    Corrected spelling for is_foriegn_key to is_foreign_key
**	14-Nov-2001 (hanch04)
**	    Corrected spelling for is_foriegn_key to is_foreign_key
**	    again after my bad integration.
**	19-Mar-2002 (gupsh01)
**	    Added new attribute locale for meta_tables. This is to match up
**	    the generated dtd with the generic Ingres dtd.
**	09-May-2002 (gupsh02)
**	    Added LC_getStdLocale call to obtain the standard name
**	    for the locale, instead of generically setting it to 
**	    UTF-8.
**      28-MAY-2002 (gupsh01)
**          Modified error message E_XF0055_No_locale_found to
**          E_XF0055_No_encoding_found.
**	30-MAY-2002 (gupsh01)
**	    Added has_default attribute to meta_columns. The earlier 
**	    default_value now holds the actual value of the attribute.
*/
void 
xmldtd(bool internal_dtd, char *title_doctype) 
{
    char 		encoding[DB_MAXNAME];	
    CL_ERR_DESC		cl_err;
    if (LC_getStdLocale (0, encoding, &cl_err) != OK )
    {
      switch (cl_err.errnum)
      {
        case E_LC_LOCALE_NOT_FOUND:
          IIUGerr(E_XF0055_No_encoding_found, UG_ERR_ERROR, 0);
          break;
        case E_LC_CHARSET_NOT_SET:
          IIUGerr(E_XF0056_Charset_not_set, UG_ERR_ERROR, 0);
          PCexit (FAIL);
	case E_LC_FORMAT_INCORRECT:
          IIUGerr(E_XF0058_LC_Format_incorrect, UG_ERR_ERROR, 
				1 , &cl_err.moreinfo[0].data._i4);
          PCexit (FAIL);
        default:
          IIUGerr(E_XF0057_Unknown_LC_error, UG_ERR_ERROR, 0);
          PCexit (FAIL);
      }
    }
    xfwrite(Xf_xml_dtd , ERx("<?xml version=\'1.0\' ")); 
    xfwrite(Xf_xml_dtd, ERx(" encoding=\'"));
    xfwrite(Xf_xml_dtd, ERx(encoding));
    xfwrite(Xf_xml_dtd, ERx("\'?> \n"));
    if (internal_dtd)
    {
      xfwrite(Xf_xml_dtd , ERx("<!DOCTYPE "));	
      xfwrite(Xf_xml_dtd , ERx(title_doctype));	
      xfwrite(Xf_xml_dtd , ERx(" [\n"));	
    }
    xfwrite(Xf_xml_dtd , ERx("<!ELEMENT "));
    xfwrite(Xf_xml_dtd , ERx(title_doctype));
    xfwrite(Xf_xml_dtd , ERx(" (meta_ingres?, table*)>\n"));
    xfwrite(Xf_xml_dtd , ERx("<!ATTLIST "));
    xfwrite(Xf_xml_dtd , ERx(title_doctype));
    xfwrite(Xf_xml_dtd , ERx(" version CDATA  #REQUIRED >\n"));
    xfwrite(Xf_xml_dtd , ERx("<!ELEMENT meta_ingres (meta_table+)>\n"));
    xfwrite(Xf_xml_dtd , 
	ERx("<!ATTLIST meta_ingres \t class \t\t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t db_name \t CDATA \t #IMPLIED \n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t owner \t\t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t locale \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t schema_nr \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t table_nr \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t version \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t view_nr \t CDATA \t #REQUIRED>\n"));
    xfwrite(Xf_xml_dtd, 
	ERx("<!ELEMENT meta_table (meta_column+, meta_index*, table_location*)>\n"));
    xfwrite(Xf_xml_dtd , 
	ERx("<!ATTLIST meta_table \t table_name \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t table_owner \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t id \t\t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t column_nr \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t page_size \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t row_nr \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t locale \t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t table_type \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t allow_duplicates CDATA  #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t row_width \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t data_compression CDATA  #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t key_compression CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t journaling \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t unique_keys \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t table_structure CDATA \t #REQUIRED>\n"));
    xfwrite(Xf_xml_dtd ,
    ERx("<!ELEMENT meta_column EMPTY >\n"));
    xfwrite(Xf_xml_dtd ,
    ERx("<!ATTLIST meta_column \t column_name \t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t column_type \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t column_width \t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t allow_nulls \t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t has_default \t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t default_value \t CDATA \t #IMPLIED\n"));	
    xfwrite(Xf_xml_dtd , ERx("\t\t\t is_key \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t key_position \t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t is_foreign_key  CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t precision \t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t scale \t\t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t column_label \t CDATA \t #IMPLIED >\n"));
    /* write meta_index information */
    xfwrite(Xf_xml_dtd ,
	ERx("<!ELEMENT meta_index (index_column+)>\n"));
    xfwrite(Xf_xml_dtd ,
	ERx("<!ATTLIST meta_index \t index_name \t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t column_nr \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t page_size \t CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t index_structure CDATA \t #IMPLIED \n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t persistent \t CDATA \t #IMPLIED \n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t data_compression CDATA  #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t key_compression CDATA \t #REQUIRED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t unique_keys \t CDATA \t #IMPLIED >\n"));
    /* write index_column information */
    xfwrite(Xf_xml_dtd ,
	ERx("<!ELEMENT index_column (#PCDATA)>\n"));
    xfwrite(Xf_xml_dtd ,
	ERx("<!ATTLIST index_column \t column_name \t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t index_position  CDATA \t #IMPLIED >\n"));
    /* write location information 
    xfwrite(Xf_xml_dtd ,
	ERx("<!ELEMENT table_location (location+)>\n"));
    xfwrite(Xf_xml_dtd ,
	ERx("<!ELEMENT location (#PCDATA)>\n"));
    */
    xfwrite(Xf_xml_dtd ,
        ERx("<!ELEMENT table_location EMPTY>\n"));
    xfwrite(Xf_xml_dtd ,
        ERx("<!ATTLIST table_location  \t table_path \t CDATA \t #IMPLIED >\n"));
    xfwrite(Xf_xml_dtd ,
        ERx("<!ELEMENT table (resultset+)>\n")); 
    xfwrite(Xf_xml_dtd ,
        ERx("<!ATTLIST table  \t table_name \t CDATA \t #IMPLIED \n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t table_nr \t CDATA \t #REQUIRED >\n"));
    xfwrite(Xf_xml_dtd ,
        ERx("<!ELEMENT resultset (row*)>\n"));
    xfwrite(Xf_xml_dtd ,
        ERx("<!ATTLIST resultset \t row_start \t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t row_end \t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t subset_size \t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t row_count \t CDATA \t #IMPLIED >\n"));
    xfwrite(Xf_xml_dtd ,
        ERx("<!ELEMENT row (column+)>\n"));
    xfwrite(Xf_xml_dtd ,
        ERx("<!ELEMENT column (#PCDATA)>\n"));
    xfwrite(Xf_xml_dtd ,
        ERx("<!ATTLIST column \t column_name \t CDATA \t #IMPLIED\n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t is_null \t CDATA \t #IMPLIED \n"));
    xfwrite(Xf_xml_dtd , ERx("\t\t\t a-dtype NMTOKENS 'is_null boolean' >\n"));
    if (internal_dtd)
	xfwrite(Xf_xml_dtd ,ERx("]>\n"));
}
/*{
** Name: xmldbinfo - write a numerical value to the xml file.
**
** Description:
**
**      Routine to write out the value to a file
**
** Inputs:
**	tp	pointer to the XF_TABINFO structure.
**
** Output:
**      none.
**
** 21-dec-2000 (gupsh01)
**      Add.
*/
void
xmldbinfo(char *dbname, char *owner)
{
    char *ver;
    char	tbuf[256];
    /* db_name */
    xfwrite(Xf_out, ERx("\t\t db_name = \""));
    xfwrite(Xf_out, ERx(dbname));
    /* owner */
    xfwrite(Xf_out, ERx("\"\n\t\t owner = \""));
    xfwrite(Xf_out, ERx(owner));
    /* locale */
    xfwrite(Xf_out, ERx("\"\n\t\t locale = \""));
    CMget_charset_name(&tbuf[0]);
    xfwrite(Xf_out, tbuf);
    /* schema_nr */
    STprintf(tbuf, ERx("\"\n\t\t schema_nr = \"%ld\""),
        ERx(get_schema_nr()));
    xfwrite(Xf_out, tbuf );
    /* table_nr */
    STprintf(tbuf, ERx("\n\t\t table_nr = \"%d\""),
        ERx(get_table_nr()));
    xfwrite(Xf_out, tbuf );
    /* version */
    STprintf(tbuf, ERx("\n\t\t version = \"%d\""),
        ERx(get_version()));
    xfwrite(Xf_out, tbuf );
    /* view_nr */
    STprintf(tbuf, ERx("\n\t\t view_nr = \"%d\">\n"),
        ERx(get_view_nr()));
    xfwrite(Xf_out, tbuf );
}
/*{
** Name: get_schema_nr -   obtain the schema information from the database.
**
** Description:
**
** Inputs:   NONE
**
** Outputs:
**
**      Returns:
**
** History:
**      06-June-2001 (gupsh01) 
**          written.
*/
i4 get_schema_nr()
{
  i4 cnt;
{
/* # line 456 "xfgenxml.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"select count(*)from iischema");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
      if (IIerrtest() != 0) goto IIrtB1;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 460 "xfgenxml.sc" */	/* host code */
}
    return cnt;
}
/*{
** Name: get_table_nr -   obtain the table count from the database.
**
** Description:
**
** Inputs:   NONE
**
** Outputs:
**
**      Returns:
**
** History:
**      06-June-2001 (gupsh01) 
**          written.
*/
i4 get_table_nr()
{
  i4 cnt;
{
/* # line 485 "xfgenxml.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iitables where table_owner<>$ingres");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
      if (IIerrtest() != 0) goto IIrtB2;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 488 "xfgenxml.sc" */	/* host code */
}
    return cnt;
}
/*{
** Name: get_version -   obtain the version for ingres.
**
** Description:
**
** Inputs:   NONE
**
** Outputs:
**
**      Returns:
**
** History:
**      06-June-2001 (gupsh01)
**          written.
*/
char *get_version()
{
  static char buf[100]= {0};
    {
/* # line 513 "xfgenxml.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"select dbmsinfo('_version')");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,99,buf);
      if (IIerrtest() != 0) goto IIrtB3;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
/* # line 516 "xfgenxml.sc" */	/* host code */
    }
    return buf;
}
/*{
** Name: get_view_nr -   obtain the #of views in the database.
**
** Description:
**
** Inputs:   NONE
**
** Outputs:
**
**      Returns:
**
** History:
**      06-June-2001 (gupsh01)
**          written.
*/
i4 get_view_nr()
{
  i4 cnt;
{
/* # line 541 "xfgenxml.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiviews where table_owner<>$ingres");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
      if (IIerrtest() != 0) goto IIrtB4;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
/* # line 544 "xfgenxml.sc" */	/* host code */
}
    return cnt;
}
static	char lfmt[] = "%ld";
/*{
** Name: xmlprintcols -   Print the column information for the table. 
**
** Description:
**		Print the meta data information about the columns of the table and
** Inputs:
**		XF_TABINFO structure for the table.
** Outputs:
**
**	Returns:
**
** History:
**	30-aug-2000 (gupsh01) 
**	    written.
**	14-jun-2001 (gupsh01) 
**	    fix writing the key_position statement.
**	29-sep-2006 (gupsh01)
**	    Added support for ANSI date/time types.
*/
i4
xmlprintcols (XF_TABINFO *tp)
{
    XF_COLINFO	*cp;
    i4 length;
    i4 count = 0;
    char tbuf[256];
    for (cp = tp->column_list; cp != NULL; cp = cp->col_next)
    {
	count++;
	/* column_name */		
        xfwrite(Xf_out, ERx("<meta_column \t column_name=\""));
	xfwrite(Xf_out, ERx(cp->column_name));
        xfwrite(Xf_out, ERx("\""));
	/* column_type */
        xfwrite(Xf_out, ERx("\n\t\t column_type="));
	/*
	** there are 3 more types in the sql manual 
	** integer2, integer1, float8 
	** which are not covered here 
	*/
        if (cp->adf_type < 0 ) 
        {
	    length = cp->intern_length - 1;
        }
        else
        {
	    length = cp->intern_length;
        }
        switch (abs(cp->adf_type))
        {
        case DB_CHR_TYPE:
            xfwrite(Xf_out, ERx("\"C\""));
            break;
        case DB_CHA_TYPE:
            xfwrite(Xf_out, ERx("\"CHAR\""));
            break;
        case DB_TXT_TYPE:
            xfwrite(Xf_out, ERx("\"TEXT\""));
            break;
        case DB_VCH_TYPE:
            xfwrite(Xf_out, ERx("\"VARCHAR\""));
            break;
        case DB_LVCH_TYPE:
            xfwrite(Xf_out, ERx("\"LONG VARCHAR\""));
            break;
        case DB_INT_TYPE:
            xfwrite(Xf_out, ERx("\"INTEGER\""));
            break;
        case DB_FLT_TYPE:
            xfwrite(Xf_out, ERx("\"FLOAT\""));
            break;
        case DB_DEC_TYPE:
            xfwrite(Xf_out, ERx("\"DECIMAL\""));
            break;
        case DB_BIT_TYPE:
            xfwrite(Xf_out, ERx("\"BIT\""));
            break;
        case DB_VBIT_TYPE:
            xfwrite(Xf_out, ERx("\"BIT VARYING\""));
            break;
        case DB_LBIT_TYPE:
            xfwrite(Xf_out, ERx("\"LONG BIT\""));
            break;
        case DB_CPN_TYPE:
            xfwrite(Xf_out, ERx("\"COUPON\""));
            break;
        case DB_DTE_TYPE:
            xfwrite(Xf_out, ERx("\"INGRESDATE\""));
            break;
        case DB_ADTE_TYPE:
            xfwrite(Xf_out, ERx("\"ANSIDATE\""));
            break;
        case DB_TME_TYPE:
            xfwrite(Xf_out, ERx("\"TIME WITH LOCAL TIME ZONE\""));
            break;
        case DB_TMW_TYPE:
            xfwrite(Xf_out, ERx("\"TIME WITH TIME ZONE\""));
            break;
        case DB_TMWO_TYPE:
            xfwrite(Xf_out, ERx("\"TIME WITHOUT TIME ZONE\""));
            break;
        case DB_TSTMP_TYPE:
            xfwrite(Xf_out, ERx("\"TIMESTAMP WITH LOCAL TIME ZONE\""));
            break;
        case DB_TSW_TYPE:
            xfwrite(Xf_out, ERx("\"TIMESTAMP WITH TIME ZONE\""));
            break;
        case DB_TSWO_TYPE:
            xfwrite(Xf_out, ERx("\"TIMESTAMP WITHOUT TIME ZONE\""));
            break;
        case DB_INDS_TYPE:
            xfwrite(Xf_out, ERx("\"INTERVAL DAY TO SECOND\""));
            break;
        case DB_INYM_TYPE:
            xfwrite(Xf_out, ERx("\"INTERVAL YEAR TO MONTH\""));
            break;
        case DB_MNY_TYPE:
            xfwrite(Xf_out, ERx("\"MONEY\""));
            break;
        case DB_LOGKEY_TYPE:
            xfwrite(Xf_out, ERx("\"OBJECT_KEY\""));
            break;
        case DB_TABKEY_TYPE:
            xfwrite(Xf_out, ERx("\"TABLE_KEY\""));
            break;
       case DB_BYTE_TYPE:
            xfwrite(Xf_out, ERx("\"BYTE\""));
            break;
        case DB_VBYTE_TYPE:
            xfwrite(Xf_out, ERx("\"BYTE VARYING\""));
            break;
        case DB_LBYTE_TYPE:
            xfwrite(Xf_out, ERx("\"LONG BYTE\""));
            break;
	default:
	    xfwrite(Xf_out, ERx("\"UNKNOWN\""));
        }
        switch (abs(cp->adf_type))
        {
	case DB_DEC_TYPE:
	    STprintf(tbuf, ERx("\n\t\t precision=\"%ld\""),
    	        ERx( DB_P_DECODE_MACRO(cp->precision)));
	    xfwrite(Xf_out, tbuf );
	    STprintf(tbuf, ERx("\n\t\t scale=\"%ld\""),
    	        ERx( DB_S_DECODE_MACRO(cp->precision)));
	    xfwrite(Xf_out, tbuf );
            break;
	}
	/* COLUMn_width */
        switch (abs(cp->adf_type))
        {
        case DB_CHR_TYPE:
        case DB_CHA_TYPE:
        case DB_INT_TYPE:
        case DB_FLT_TYPE:
        case DB_DEC_TYPE:
        case DB_BIT_TYPE:
        case DB_CPN_TYPE:
        case DB_LOGKEY_TYPE:
        case DB_TABKEY_TYPE:
        case DB_BYTE_TYPE:
	    STprintf(tbuf, ERx("\n\t\t column_width=\"%d\""),
	        length);
	    xfwrite(Xf_out,tbuf ); 
            break;
        case DB_TXT_TYPE:
        case DB_VCH_TYPE:
        case DB_VBIT_TYPE:
        case DB_VBYTE_TYPE:
	    STprintf(tbuf, ERx("\n\t\t column_width=\"%d\""),
	        length - 2);
	    xfwrite(Xf_out,tbuf ); 
            break;
        case DB_LVCH_TYPE:
        case DB_MNY_TYPE:
        case DB_LBIT_TYPE:
        case DB_DTE_TYPE:
        case DB_LBYTE_TYPE:
	default:
            break;
        }
	/* allow_nulls */
	STprintf(tbuf, ERx("\n\t\t allow_nulls=\"%s\""),
    	    cp->nulls);
    	xfwrite(Xf_out, tbuf );
	/* has_default */
	STprintf(tbuf, ERx("\n\t\t has_default=\"%s\""),
    	    cp->defaults);
    	xfwrite(Xf_out, tbuf );
	/* default_value */
	if (STcompare(cp->defaults,"Y") == 0)
	{
	  STprintf(tbuf, ERx("\n\t\t default_value=\"%s\""),
    	    cp->default_value);
    	  xfwrite(Xf_out, tbuf );
	}
	/* key_column */
	if (cp->key_seq > 0)
	{
	    xfwrite(Xf_out, ERx("\n\t\t is_key=\"Y\""));
	}
	else
	{
	    xfwrite(Xf_out, ERx("\n\t\t is_key=\"N\""));
	}
	if (cp->key_seq > 0)
	{
	    STprintf(tbuf, ERx("\n\t\t key_position=\"%d\""),
	        cp->key_seq);
	    xfwrite(Xf_out, tbuf ); 
	}
	/* is foreign key */
	xfwrite(Xf_out, ERx("\n\t\t is_foreign_key=\" \""));
	/* column label exists */
	xfwrite(Xf_out, ERx("\n\t\t column_label=\" \""));
	xfwrite(Xf_out, ERx("/>\n"));		
    }
    return count;	
}
/*{
** Name: xmltabinfo -   Print the structure info for the table
**
**
** Description:
**              Get the information about the table and print it.
**
** Inputs:
**                      XF_TABINFO structure for the table.
** Outputs:
**
**      Returns:
**
** History:
**      22-dec-2000 (gupsh01) written.
**      06-jun-2001 (gupsh01) 
**	    Modified to the new dtd.
**      24-Aug-2004 (gilta01) bug 112138 INGCBT522
**          Table names should be printed in expanded XML format as they may
**          contain special characters.
*/
i4
xmltabinfo(XF_TABINFO *tp)
{
   /* column_nr */
   i4 cnt = 0;
   char tbuf[256];
   XF_COLINFO *cp;
   for (cp = tp->column_list; cp != NULL; cp = cp->col_next)
	cnt++;
   STprintf( tbuf, ERx("<meta_table \t table_name=\""));
   xfwrite(Xf_out, tbuf);
   xfxmlwrite((PTR)tp->name, STlength(tp->name), XML_EXPANDED);
   STprintf( tbuf, ERx("\"\n"));
   xfwrite(Xf_out, tbuf); 
   /* table_owner*/
   STprintf( tbuf, ERx("\t\t table_owner=\"%s\"\n"),
	tp->owner);
   xfwrite(Xf_out, tbuf);
   /* id 
   ** we assume that this is only the base table so we need only the 
   ** relid information. for index tables we require also the relidx 
   ** information
   */
   STprintf( tbuf, ERx("\t\t id=\"%d\"\n"),
	tp->table_reltid);
   xfwrite(Xf_out, tbuf);
   /* number of columns */
   STprintf( tbuf, ERx("\t\t column_nr=\"%d\"\n"),
	cnt);
   xfwrite(Xf_out, tbuf);
   /* page_size */
   STprintf( tbuf, ERx("\t\t page_size=\"%d\"\n"),
	tp->pagesize);
   xfwrite(Xf_out, tbuf);
   /* row_nr */
   STprintf( tbuf, ERx("\t\t row_nr=\"%d\"\n"),
	tp->num_rows);
   xfwrite(Xf_out, tbuf);
   /* table_type */
   STprintf( tbuf, ERx("\t\t table_type=\"%s\"\n"),
	tp->table_type);
   xfwrite(Xf_out, tbuf);
   /* allow_duplicates */
   STprintf( tbuf, ERx("\t\t allow_duplicates=\"%s\"\n"),
	tp->duplicates);
   xfwrite(Xf_out, tbuf);
   /* row_width */
   STprintf( tbuf, ERx("\t\t row_width=\"%d\"\n"),
	tp->row_width);
   xfwrite(Xf_out, tbuf);
   /* data_compression */
   STprintf( tbuf, ERx("\t\t data_compression=\"%s\"\n"),
	tp->is_data_comp);
   xfwrite(Xf_out, tbuf);
   /* key_compression */
   STprintf( tbuf, ERx("\t\t key_compression=\"%s\"\n"),
	tp->is_key_comp);
   xfwrite(Xf_out, tbuf);
   /* journaling */
   STprintf( tbuf, ERx("\t\t journaling=\"%s\"\n"),
	tp->journaled);
   xfwrite(Xf_out, tbuf);
   /* table_structure */
   STprintf( tbuf, ERx("\t\t table_structure=\"%s\"\n"),
	tp->storage);
   xfwrite(Xf_out, tbuf);
   /* is_unique*/
   STprintf( tbuf, ERx("\t\t unique_keys=\"%s\">\n"),
	tp->is_unique);
   xfwrite(Xf_out, tbuf);
   cnt = xmlprintcols(tp);
   /* print index metadata information */
   xmlprintindex(tp);
   /* location */
   STprintf( tbuf, ERx("<table_location  table_path=\"%s\"/>\n"),
	tp->location);
   xfwrite(Xf_out, tbuf);
   if (tp->otherlocs[0] == 'Y')
	xfxmlloc(Xf_out, tp->name, tp->owner);
   xfwrite(Xf_out, ERx("</meta_table>\n"));
   return cnt;
}
/*{
** Name: xmlprintresultset -   Print the  resultsetinfo for the table
**
**
** Description:
**              Get the information about the resultsets.
**
** Inputs:
**              XF_TABINFO structure for the table.
** Outputs:
**
**      Returns:
**
** History:
**      27-dec-2000 (gupsh01) written.
*/
void
xmlprintresultset(XF_TABINFO *tp)
{
    char tbuf[256];
    /* row_end */
    STprintf( tbuf, ERx("\t\t row_end=\"%d\"\n"),
	tp->num_rows);
    xfwrite(Xf_out, tbuf);
    /* subset_size */
    STprintf( tbuf, ERx("\t\t subset_size=\"%d\"\n"),
	tp->num_rows);
    xfwrite(Xf_out, tbuf);
    /* row_count */
    STprintf( tbuf, ERx("\t\t row_count=\"%d\">\n"),
	tp->num_rows);
    xfwrite(Xf_out, tbuf);
}
/*{
** Name: xmlprintindex -   Print the index information for the table 
**					 
**
** Description:
**		Print index information for the table 
** Inputs:
**
** Outputs:
**
**	Returns:
**
** History:
**	6-june-2001 (gupsh01) 
**	    written.
**	15-Apr-2005 (thaju02)
**	    Add tlist param to xffillindex.
*/
void
xmlprintindex(XF_TABINFO *tp)
{
    XF_TABINFO	*ilist;
    XF_TABINFO	*ip;
    XF_COLINFO	*cp;
    char tbuf[256];
    if ((ilist = xffillindex(0, tp)) == NULL)
	return;
    xfread_id(tp->name);
    xfread_id(tp->owner);
    /* 
    ** write information about the 
    ** index_name, index_column, and index_type 
    */
   for (ip = ilist; ip != NULL; ip = ip->tab_next)
   {
        xfread_id(ip->base_name);
        xfread_id(ip->base_owner);
	if (STequal(ip->base_name, tp->name) && 
		STequal(ip->base_owner, tp->owner))
	{
	i4 count = 0;
	STprintf( tbuf, ERx("<meta_index \t index_name=\"%s\"\n"),
	    ip->name);
	xfwrite(Xf_out, tbuf);
	/* index_column */
        for (cp = ip->column_list; cp != NULL; cp = cp->col_next)
	count++;
	/* key column count */
	STprintf( tbuf, ERx("\t\t column_nr=\"%d\"\n"),
	    count);
	xfwrite(Xf_out, tbuf);
        /* is_unique*/
        STprintf( tbuf, ERx("\t\t unique_keys=\"%s\"\n"),
	     ip->is_unique);
        xfwrite(Xf_out, tbuf);
        /* persistent */
        STprintf( tbuf, ERx("\t\t persistent=\"%s\"\n"),
	     ip->persistent);
        xfwrite(Xf_out, tbuf);
	/* data_compression */
   	STprintf( tbuf, ERx("\t\t data_compression=\"%s\"\n"),
	    ip->is_data_comp);
	xfwrite(Xf_out, tbuf);
	/* key_compression */
	STprintf( tbuf, ERx("\t\t key_compression=\"%s\"\n"),
	    ip->is_key_comp);
	xfwrite(Xf_out, tbuf);
        /* page_size */
        STprintf( tbuf, ERx("\t\t page_size=\"%d\"\n"),
	     ip->pagesize);
        xfwrite(Xf_out, tbuf);
	/* index_type */
        xfwrite(Xf_out, ERx("\t\t index_structure=\""));
        xfwrite(Xf_out, ERx(ip->storage));
	xfwrite(Xf_out, ERx("\">\n"));
        for (cp = ip->column_list; cp != NULL; cp = cp->col_next)
        {
            xfwrite(Xf_out, ERx("<index_column \t column_name=\""));	
	    xfwrite(Xf_out, ERx(cp->column_name));
	    STprintf( tbuf, ERx("\"\n\t\t index_position=\"%d\"/>\n"),
	        cp->key_seq);
	    xfwrite(Xf_out, tbuf);
        }
        xfwrite(Xf_out, ERx("</meta_index>\n"));
	}
    }
    xfifree(ilist);
}
/*{
** Name: xmlprintrows -   Print the row information for the table 
**					 
**
** Description:
**		Get table data and format each row and 
**		print the table data to the XML file.
** Inputs:
**
** Outputs:
**
**	Returns:
**
** History:
**	20-sep-2000 (gupsh01) 
**	    written.
**	29-may-2002 (gupsh01)
**	    modified to process column data based on the correct 
**	    datatype for the columns. 
**	10-jun-2003 (gupsh01)
**	    Added parsing for special characters in the string 
**	    written to XML file.
**	03-jul-2003 (gupsh01)
**	    Added Ampersand to the list of special characters 
**	    removed ?.
**      24-Aug-2004 (gilta01) bug 112138 INGCBT522
**          Added new flag to xfxmlwrite().
**	10-July-2005 (gupsh01)
**	    Byte type do not handle special characters correctly.
**	    Fixed byte data type to correctly escape special characters.
**	    (Bug 115026)
**	22-Aug-2005 (gupsh01)
**	    Refix Byte column case (Bug 115026). Incorrect parameters for
**	    the function call.
**	26-Aug-2005 (gupsh01)
**	    Fixed case for BigInt to print the Bigint data 
**	    correctly. 
**	26-Aug-2005 (gupsh01)
**	    Fixed case for Float4 and Float8 datatypes. 
**      29-Aug-2005 (hweho01)
**          Corrected the type cast for argument in xfxmlwrite() call. 
**	30-Aug-2005 (thaju02)
**	    Null terminate blob seg_buf.
**	07-Oct-2005 (gupsh01)
**	    Added support for printing small integers.
**	28-Oct-2005 (bonro01)
**	    Specify i4 instead of long since a long might be 4 or 8 bytes
**	    depending on what platform this is compiled on.
**	25-Aug-2006 (kibro01) bug 116454
**	    Cater for i1 (tinyint) values when producing xml of a database.
*/
void 
xmlprintrows( numcols, tp, dbname)
int numcols;
XF_TABINFO *tp;
char *dbname;
{
    typedef struct vch{
	short len;
	char  buf[8]; /* place holder for data */
	} VCH;
    VCH *vchp = NULL;
    i4  i=0;
    i4  j=0;
    char *mem;
    char col_name[FE_MAXNAME];
    char	nbuf[((2 * FE_MAXNAME) + 2 + 1)];
    char tbuff[256];
    ADF_CB *ladfcb;
    char   *cp;
    static char ii_decimal = NULLCHAR;
  char sql_statement[FE_MAXTABNAME + 32];
  char sql_dbname[FE_MAXDBNM];
  char sql_tablename[FE_MAXTABNAME];
    IISQLHDLR *datahdlr_struct;	 /*  Data handler to retreive blobs */
    HDLR_PARAM *hdlr_arg;
    char	loc_buffer[MAX_LOC + 1];
    TXT_HANDLE *Xf_row;
    XF_COLINFO *collist;
    i4  k = 0;
    i4	internal_length = 0;
    STcopy (dbname, sql_dbname);
    STcopy (tp->name, sql_tablename );
    ptr_TableSqlda = (IISQLDA *) XF_REQMEM(IISQDA_HEAD_SIZE + 
				(numcols * IISQDA_VAR_SIZE), TRUE);
    ptrorig_TableSqlda = (IISQLDA *) XF_REQMEM(IISQDA_HEAD_SIZE + 
				(numcols * IISQDA_VAR_SIZE), TRUE);
    if (ptr_TableSqlda == NULL || ptr_TableSqlda == NULL)
    {
	IIUGerr(E_XF0060_Out_of_memory, UG_ERR_FATAL, 0);
	    /* NOTREACHED */
    }
    ptr_TableSqlda->sqln = numcols;      
    STcopy("SELECT ", sql_statement);
    STcat(sql_statement,    "* ");
    STcat(sql_statement, "FROM ");
    if (IIUGdlm_ChkdlmBEobject(tp->name, NULL, TRUE))
    {
     	IIUGrqd_Requote_dlm(tp->name, nbuf);
	STcat(sql_statement, nbuf);
    }
    else
    {
	STcat(sql_statement, tp->name);
    }
/* # line 1169 "xfgenxml.sc" */	/* prepare */
  {
    IIsqInit(&sqlca);
    IIsqPrepare(0,(char *)"s2",(char *)0,0,sql_statement);
  }
/* # line 1170 "xfgenxml.sc" */	/* describe */
  {
    IIsqInit(&sqlca);
    IIsqDescribe(0,(char *)"s2",ptr_TableSqlda,0);
  }
/* # line 1171 "xfgenxml.sc" */	/* host code */
    MEcopy(ptr_TableSqlda, IISQDA_HEAD_SIZE + (numcols * IISQDA_VAR_SIZE),
	ptrorig_TableSqlda);
    for (i = 0; i < ptr_TableSqlda->sqld; i++)
    {
	bool nullsqltype = FALSE;
	if( ptr_TableSqlda->sqlvar[i].sqltype < 0 )
	{
            nullsqltype = TRUE;
	    ptr_TableSqlda->sqlvar[i].sqlind  = (short *)
		XF_REQMEM(sizeof(short), TRUE);
	}
	else
	{
	    ptr_TableSqlda->sqlvar[i].sqlind  = (short *)0;
	}
         switch (abs(ptr_TableSqlda->sqlvar[i].sqltype))
         {
            case IISQ_INT_TYPE:
	      ptr_TableSqlda->sqlvar[i].sqldata = 
		(char *) XF_REQMEM(ptr_TableSqlda->sqlvar[i].sqllen + 1, TRUE);
              break;
            case IISQ_FLT_TYPE:
	      /* obtain the internal length of a float column */ 
	      if (tp && tp->column_list)
                collist = tp->column_list;
              else 
	      {
                IIUGerr (E_XF0059_Colinfo_incorrect, UG_ERR_ERROR, 0);
                PCexit (FAIL);
              }
              for (k=0; k<i, collist; k++)
              {
		/* we are running the query select * from table, this 
		** will force the sql result column sequence to be obtained in 
		** the column sequence order and the sql result data type will  
		** be the same as the original column data type. Therefore we 
		** check these values to obtain the internal column length.
		*/ 
                if ((collist->col_seq - 1 == i) &&
                      (collist->adf_type ==  ptr_TableSqlda->sqlvar[i].sqltype))
                {
                  internal_length = collist->intern_length;
                  break;
                }    
                else
                  collist = collist->col_next;
              }
               if(internal_length == 0)
               {
                   IIUGerr (E_XF0059_Colinfo_incorrect, UG_ERR_ERROR, 0);
                   PCexit (FAIL);
               }
               else
                 if (collist->adf_type < 0 )
                   internal_length -= 1;
               if (internal_length == sizeof(float))
	       {
                  ptr_TableSqlda->sqlvar[i].sqllen  = sizeof(float);
	          ptr_TableSqlda->sqlvar[i].sqldata = 
		    (char *) XF_REQMEM(sizeof(float), TRUE);
	       }
	       else 
	       { 
                  ptr_TableSqlda->sqlvar[i].sqllen  = sizeof(double);
	          ptr_TableSqlda->sqlvar[i].sqldata = 
		    (char *) XF_REQMEM(sizeof(double), TRUE);
	       }
            break;
            case IISQ_DTE_TYPE:
                ptr_TableSqlda->sqlvar[i].sqllen  = IISQ_DTE_LEN;
                ptr_TableSqlda->sqlvar[i].sqldata = 
		    (char *) XF_REQMEM(IISQ_DTE_LEN + 1, TRUE);
                ptr_TableSqlda->sqlvar[i].sqltype = IISQ_CHA_TYPE;
		if( nullsqltype )
		{
		    ptr_TableSqlda->sqlvar[i].sqltype *= -1;
		}
            break;
            case IISQ_DEC_TYPE:
            case IISQ_MNY_TYPE:
                ptr_TableSqlda->sqlvar[i].sqldata = 
		    (char *) XF_REQMEM(sizeof(double), TRUE);
                ptr_TableSqlda->sqlvar[i].sqllen  = sizeof(double);
                ptr_TableSqlda->sqlvar[i].sqltype = IISQ_FLT_TYPE;
		if( nullsqltype )
		{
		    ptr_TableSqlda->sqlvar[i].sqltype *= -1;
		}
            break;
            case IISQ_CHA_TYPE:
            case IISQ_BYTE_TYPE:
                ptr_TableSqlda->sqlvar[i].sqldata = 
		    (char *) XF_REQMEM(ptr_TableSqlda->sqlvar[i].sqllen 
					+ sizeof(short) + 1, TRUE);
            break;
            case IISQ_VBYTE_TYPE:
            case IISQ_VCH_TYPE:
                ptr_TableSqlda->sqlvar[i].sqldata =
		    (char *) XF_REQMEM(ptr_TableSqlda->sqlvar[i].sqllen 
					+ sizeof(short) + 1, TRUE);
            break;
	    case IISQ_LBIT_TYPE:	
	    case IISQ_LBYTE_TYPE:
            case IISQ_OBJ_TYPE:
		hdlr_arg = 
		    (HDLR_PARAM *) XF_REQMEM( sizeof(HDLR_PARAM), TRUE);
    		datahdlr_struct = 
		    (IISQLHDLR *) XF_REQMEM(sizeof(IISQLHDLR), TRUE);
    		datahdlr_struct->sqlarg = (PTR)hdlr_arg;
		/*
		** For now just print out the bytes
    		** datahdlr_struct->sqlhdlr = Get_HandlerLBYTE;
		*/
    		datahdlr_struct->sqlhdlr = Get_HandlerLVCH;
		hdlr_arg->arg = 
		    (char *) XF_REQMEM(ptr_TableSqlda->sqlvar[i].sqlname.sqlnamel
		    + 1, TRUE);
		STlcopy (ptr_TableSqlda->sqlvar[i].sqlname.sqlnamec, 
		    hdlr_arg->arg,
	    	    ptr_TableSqlda->sqlvar[i].sqlname.sqlnamel);
		   hdlr_arg->arg[ptr_TableSqlda->sqlvar[i].sqlname.sqlnamel]='\0';
                ptr_TableSqlda->sqlvar[i].sqldata = 
		    (char *) datahdlr_struct;
                ptr_TableSqlda->sqlvar[i].sqltype = IISQ_HDLR_TYPE;
		if( nullsqltype )
		{
		    ptr_TableSqlda->sqlvar[i].sqltype *= -1;
		}
	    break;
            case IISQ_LVCH_TYPE:
		hdlr_arg = 
		    (HDLR_PARAM *) XF_REQMEM( sizeof(HDLR_PARAM), TRUE);
    		datahdlr_struct = 
		    (IISQLHDLR *) XF_REQMEM(sizeof(IISQLHDLR), TRUE);
    		datahdlr_struct->sqlarg = (PTR)hdlr_arg;
    		datahdlr_struct->sqlhdlr = Get_HandlerLVCH;
		hdlr_arg->arg = 
		    (char *) XF_REQMEM(ptr_TableSqlda->sqlvar[i].sqlname.sqlnamel
		    + 1, TRUE);
		STlcopy (ptr_TableSqlda->sqlvar[i].sqlname.sqlnamec, 
		    hdlr_arg->arg,
	    	    ptr_TableSqlda->sqlvar[i].sqlname.sqlnamel);
		   hdlr_arg->arg[ptr_TableSqlda->sqlvar[i].sqlname.sqlnamel]='\0';
                ptr_TableSqlda->sqlvar[i].sqldata = 
		    (char *) datahdlr_struct;
                ptr_TableSqlda->sqlvar[i].sqltype = IISQ_HDLR_TYPE;
		if( nullsqltype )
		{
		    ptr_TableSqlda->sqlvar[i].sqltype *= -1;
		}
	    break;
	    default :
                ptr_TableSqlda->sqlvar[i].sqldata = 
		    (char *) XF_REQMEM(ptr_TableSqlda->sqlvar[i].sqllen 
					+ sizeof(short) + 1, TRUE);
                ptr_TableSqlda->sqlvar[i].sqltype = IISQ_CHA_TYPE;
		if( nullsqltype )
		{
		    ptr_TableSqlda->sqlvar[i].sqltype *= -1;
		}
            break;
        }
	if ( ptr_TableSqlda->sqlvar[i].sqldata == NULL)
        {
	    IIUGerr(E_XF0060_Out_of_memory, UG_ERR_FATAL, 0);
	    /* NOTREACHED */
	}  
    }                                        
    ladfcb = FEadfcb();
    if (ii_decimal == NULLCHAR)
    {
	/* Set up value of II_DECIMAL */
	NMgtAt(ERx("II_DECIMAL"), &cp);
	cp = STalloc(cp);
	if ( cp != NULL && *cp != EOS )
	{
	    if (*(cp+1) != EOS || (*cp != '.' && *cp != ','))
	    {
		ii_decimal = ladfcb->adf_decimal.db_decimal;
	    }
	    else
	    {
		ii_decimal = cp[0];
	    }
	}
	else
	    ii_decimal = ladfcb->adf_decimal.db_decimal;
	MEfree(cp);
    }
/* # line 1373 "xfgenxml.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqMods(3);
    IIsqExImmed(sql_statement);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIcsDaGet(0,ptr_TableSqlda);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 1377 "xfgenxml.sc" */	/* host code */
    if( !printedRowTag )
    {
	printedRowTag = TRUE;
        xfwrite(Xf_out, "<row>\n");
    }
    for (i = 0; i < ptr_TableSqlda->sqld; i++)
    {
/*
        switch (abs(ptr_TableSqlda->sqlvar[i].sqltype))
        {
	    case IISQ_HDLR_TYPE:
	    case IISQ_LBYTE_TYPE:
	    case IISQ_LVCH_TYPE:
		continue;
                break;
	    default:
                break;
        }
*/
	STlcopy (ptr_TableSqlda->sqlvar[i].sqlname.sqlnamec, col_name, 
	    ptr_TableSqlda->sqlvar[i].sqlname.sqlnamel);
        col_name[ptr_TableSqlda->sqlvar[i].sqlname.sqlnamel]='\0';
	xfwrite(Xf_out, ERx("<column \t column_name=\""));
	xfwrite(Xf_out, col_name);
	if(( ptrorig_TableSqlda->sqlvar[i].sqltype < 0 ) &&
	   ( *ptr_TableSqlda->sqlvar[i].sqlind == -1 ))
	{
	    xfwrite(Xf_out, "\" is_null=\"true" );
	}
	xfwrite(Xf_out, ERx("\">"));
	if(( ptrorig_TableSqlda->sqlvar[i].sqltype < 0 ) &&
	   ( *ptr_TableSqlda->sqlvar[i].sqlind == -1 ))
	{
	    xfwrite(Xf_out, "]^NULL^[" );
	}
	else
	{
	    switch (abs(ptrorig_TableSqlda->sqlvar[i].sqltype))
	    {
            case IISQ_CHA_TYPE:
		xfxmlwrite(ptr_TableSqlda->sqlvar[i].sqldata,
			(i4)ptr_TableSqlda->sqlvar[i].sqllen, XML_CDATA);
                break;
            case IISQ_INT_TYPE:
	    {
	      if (ptr_TableSqlda->sqlvar[i].sqllen < 2)
		STprintf(tbuff, ERx("%d"),
		    *((i1 *)ptr_TableSqlda->sqlvar[i].sqldata));
	      else if (ptr_TableSqlda->sqlvar[i].sqllen < 4)
		STprintf(tbuff, ERx("%d"),
		    *((i2 *)ptr_TableSqlda->sqlvar[i].sqldata));
	      else if (ptr_TableSqlda->sqlvar[i].sqllen < 8)
    		  STprintf(tbuff, ERx("%ld"), 
		    *((i4 *)ptr_TableSqlda->sqlvar[i].sqldata));
		else
    		  STprintf(tbuff, ERx("%lld"), 
		    *((i8 *)ptr_TableSqlda->sqlvar[i].sqldata));
		xfwrite(Xf_out, tbuff);
	    }
                break;
            case IISQ_DEC_TYPE:
	    {
		char formatbuff[64];
		STprintf(formatbuff, ERx("%%%d.%dlf"),
		    DB_P_DECODE_MACRO(ptrorig_TableSqlda->sqlvar[i].sqllen) + 1,
		    DB_S_DECODE_MACRO(ptrorig_TableSqlda->sqlvar[i].sqllen));
		STprintf(tbuff, formatbuff,
		    *((f8 *)ptr_TableSqlda->sqlvar[i].sqldata), ii_decimal);
		xfwrite(Xf_out, tbuff);
	    }
                break;
            case IISQ_MNY_TYPE:
	    {
		DB_DATA_VALUE dv1;
		DB_DATA_VALUE rdv;
		char outbuf[AD_5MNY_OUTLENGTH + 1];
		f8 mny_cents = 0.0;
		/*
		** Take the double, multiply it by 100 to get cents
		** then convert the money to a string
		*/
		dv1.db_datatype = ptrorig_TableSqlda->sqlvar[i].sqltype;
		dv1.db_prec = ptrorig_TableSqlda->sqlvar[i].sqllen;
		dv1.db_length = 0;
		mny_cents = *((f8 *)ptr_TableSqlda->sqlvar[i].sqldata) * 100;
		dv1.db_data = (PTR)&mny_cents;
    		MEfill( AD_5MNY_OUTLENGTH + 1, (unsigned char) '\0',
		    (PTR) outbuf);
		rdv.db_data = outbuf;
		rdv.db_length = AD_5MNY_OUTLENGTH;
		adu_cpmnytostr(FEadfcb(), &dv1, &rdv);
		xfwrite(Xf_out, 
			((char *) rdv.db_data));
	    }
                break;
            case IISQ_FLT_TYPE:
	    {
                if (ptr_TableSqlda->sqlvar[i].sqllen < sizeof(double))
    		  STprintf(tbuff, ERx("%lg"),  
		    *((f4 *)ptr_TableSqlda->sqlvar[i].sqldata), ii_decimal);
		else
    		  STprintf(tbuff, ERx("%lg"),  
		    *((f8 *)ptr_TableSqlda->sqlvar[i].sqldata), ii_decimal);
		xfwrite(Xf_out, tbuff);
	    }
                break;
	    case IISQ_HDLR_TYPE:
	    case IISQ_LVCH_TYPE:
	    case IISQ_LBYTE_TYPE:
	    {
		char	seg_buf[2001];
		bool	done = FALSE;
		i4 	count = 0;
		STATUS  cl_stat = OK;
		/*
		** FIX ME - implement xlink
		*/
		/*
		** xfwrite(Xf_out, "http://" );
		*/
		STprintf( loc_buffer, "%s.row", col_name);
		/* open xml file */
		if ((Xf_row = xfopen(loc_buffer,TH_READONLY)) == NULL) 
		{
		    PCexit (FAIL);
		}
		while( !done )
		{
		    cl_stat = SIread( Xf_row->f_fd, 2000, &count, seg_buf );
		    seg_buf[count] = '\0';
		    xfxmlwrite(seg_buf, count, XML_EXPANDED);
		    if (cl_stat == ENDFILE) /* no records left */
		    {
			done = TRUE;
            		break;
		    }
		    if (cl_stat != OK) /* error */
		    {
			done = TRUE;
            		break;
		    }
		}
    		/* close files */
    		xfclose(Xf_row);
    		xfdelete(Xf_row);
	    }
                break;
	    case IISQ_VBYTE_TYPE:
	    case IISQ_VCH_TYPE:
		vchp = (VCH *) ptr_TableSqlda->sqlvar[i].sqldata;
		vchp->buf[vchp->len] = '\0';
		xfxmlwrite((char *)vchp->buf, (i4)vchp->len, XML_CDATA);
                break;
	    case IISQ_BYTE_TYPE:
		xfxmlwrite (((char *)ptr_TableSqlda->sqlvar[i].sqldata), 
		    ((i4)ptr_TableSqlda->sqlvar[i].sqllen), 
		    XML_CDATA);
                break;
	    case IISQ_DTE_TYPE:
	    default:
		xfwrite(Xf_out, 
		    ((char *)ptr_TableSqlda->sqlvar[i].sqldata));
                break;
            }
	}
	xfwrite(Xf_out, "</column>\n");
    }
    xfwrite(Xf_out, "</row>\n");
    printedRowTag = FALSE;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
    for (i = 0; i < ptr_TableSqlda->sqld; i++)
    {
	if( abs(ptr_TableSqlda->sqlvar[i].sqltype) ==  IISQ_HDLR_TYPE)
        {
	    datahdlr_struct = 
		(IISQLHDLR *) ptr_TableSqlda->sqlvar[i].sqldata;
	    hdlr_arg = 
		(HDLR_PARAM *)datahdlr_struct->sqlarg;
	    MEfree( (PTR)hdlr_arg );
	    MEfree( (PTR)datahdlr_struct );
	}
	else
	{
	    MEfree (ptr_TableSqlda->sqlvar[i].sqldata);
	}
    }
    MEfree ((PTR)ptr_TableSqlda);	
}
/* xfxmlwrite - Formats writing of special characters into xml file.
** Description:
** 	Parses the input string for special characters 
**	in XML which needs to be parsed and represented  
**	correctly in XML files eg  ", <, >, ', & etc.
**	These characters must either be written with  
**	special names, eg & will be written as &amp; 
**	or the string should be surrounded by <!CDATA[....]]>
**		
** Inputs:
**		instr  - Input string. 
**		len    - length of input string.
**              format - XML_CDATA or XML_EXPANDED
** Outputs:
**		None.
**
** Side Effects:
**		Calls xfwrite() to output either original input
**		string or modified string where necessary.
**
** Returns:
**
** History:
**      10-jun-2003 (gupsh01)
**          written.
**	03-jul-2003 (gupsh01)
**	    Added Ampersand to the list of special 
**	    characters removed ?.
**	15-Dec-2003 (hanal04) Bug 111436 INGSRV2632
**          Rewritten and renamed in order to resolve buffer over-run
**          and memory leaks seen in xfspecialchars().
**      24-Aug-2004 (gilta01) bug 112138 INGCBT522
**          Added new flag to xfxmlwrite(). XML_CDATA to encapsulate
**          text containing special characters, OR XML_EXPANDED
**          to expand special characters into XML format.
*/
void 
xfxmlwrite(char *instr, i4 len, i4 format)
{
  char beginquote[]  = "<![CDATA[";
  char endquote[]    = "]]>";
  char dblquote[]    = "&quot;";
  char ampersand[]   = "&amp;";
  char lessthan[]    = "&lt;";
  char greaterthan[] = "&gt;";
  char apos[]	     = "&apos;";
  char *cp, *result;
  i4   expansion = 0; 
  i4   i;
  bool found = FALSE;
  result = cp = instr;
  for(i=0;i<len;i++)
  {
      switch(*cp)
      {
          /* Some of the special characters are
          ** &, <, >, ', ".
          ** These should be surrounded by <![CDATA[....]]>
          */
	  case '&':
	      found = TRUE;
	      expansion += (STlength(ampersand) -1);
	      break;
	  case '<':
	      found = TRUE;
	      expansion += (STlength(lessthan) -1);
	      break;
	  case '>':
              found = TRUE;
	      expansion += (STlength(greaterthan) -1);
              break;
	  case '\'':
	      found = TRUE;
	      expansion += (STlength(apos) -1);
	      break;
	  case '"':
	      found = TRUE;
	      expansion += (STlength(dblquote) -1);
              break;
      }
      if(found && (format == XML_CDATA))
      {
          break;
      }
      cp++;
  }
  if(found)
  {
      if (format == XML_CDATA)
      {
          result = (char *) XF_REQMEM(len + STlength(beginquote) +
				    STlength(endquote) + 1, TRUE);
          if(result == NULL)
	  {
              IIUGerr(E_XF0060_Out_of_memory, UG_ERR_FATAL, 0);
	      return;
	  }
          STcopy (beginquote, result);
	  STcat (result, instr);
	  STcat (result, endquote);
      }
      else
      {
          char *ocp;
	  result = (char *) XF_REQMEM(len + expansion + 1, TRUE);
	  if(result == NULL)
	  {
	      IIUGerr(E_XF0060_Out_of_memory, UG_ERR_FATAL, 0);
	      return;
	  }
          cp = instr;
	  ocp = result;
          for(i=0;i<len;i++)
          {
              switch(*cp)
              {
                  case '&':
	              STcopy(ampersand, ocp);
	              ocp += STlength(ampersand);
                      break;
                  case '<':
		      STcopy(lessthan, ocp);
		      ocp += STlength(lessthan);
                      break;
                  case '>':
		      STcopy(greaterthan, ocp);
		      ocp += STlength(greaterthan);
                      break;
                  case '\'':
		      STcopy(apos, ocp);
		      ocp += STlength(apos);
                      break;
                  case '"':
                      STcopy(dblquote, ocp);
		      ocp += STlength(dblquote);
                      break;
                  default:
                      *ocp = *cp;
		      ocp++;
		      break;
              }
              cp++;
          }
      }
  }
  xfwrite(Xf_out, result);
  if(found)
  {
      MEfree((PTR)result); 
  }
  return;
}
/*{
** Name: xfxmlloc - Add the additional locations for a table.
**
** Description:
**	Retrieves location information from iimulti_locations if it
**	exists.  If iimulti_locations does not exist, return.
**	modified from xfaddloc
**	    use like the following
**		xfwrite(Xf_in, ip->location);
**		if (ip->otherlocs[0] == 'Y')
**	    xfxmlloc(ip->name, ip->owner);
**
** Inputs:
**	tdp 		This is the TXT_HANDLE where we are writing.
**	name		Name of the curent table.
**	owner		Owner of the current table.
**
** Outputs:
**	Returns:
**		none.
**
**	Exceptions:
**		none.
**
** Side Effects:
**	none.
** History:
**	21-dec-2000 (gupsh01) 
**	    created for genxml.
*/
void
xfxmlloc(tdp, name, owner)
TXT_HANDLE *tdp;
  char *name;
  char *owner;
{
  char add_loc[FE_MAXNAME + 1];
  i2 loc_seq;
    if ( !With_multi_locations)
	return;
    /* There may be additional locations for this table */
/* # line 1804 "xfgenxml.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"xfgenxml1",7155,22893);
      IIputdomio((short *)0,1,32,0,name);
      IIputdomio((short *)0,1,32,0,owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"xfgenxml1",7155,22893);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select m.location_name, m.loc_sequence from iimulti_locations m where\
 m.table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and m.table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by m.loc_sequence");
        IIexDefine(0,(char *)"xfgenxml1",7155,22893);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,add_loc);
      IIgetdomio((short *)0,1,30,2,&loc_seq);
      if (IIerrtest() != 0) goto IIrtB6;
/* # line 1811 "xfgenxml.sc" */	/* host code */
    {
	/* add the locations to the list */
	(void) STtrmwhite(add_loc);
	xfwrite (Xf_out, ERx("table_location table_path=\""));
	xfwrite (Xf_out, ERx(add_loc));
	xfwrite (Xf_out, ERx("\"/>\n"));
    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
}
/*{
** Name: print_docType - write a docType header to the xml file.
**
** Description:
**      Routine to write out the doctype value to xml  file
**
** Inputs:
**	dtd_filename  : The name of the dtd file.
**	referredloc   : Whether the dtd is at the referred location, ie
**			%II_SYSTEM/ingres/files.
**	title_doctype : The title type of the resultant xml document.
**			It is IngresDoc if none is provided. 
**
**      Returns:
**              none.
**      6-sep-2000 (gupsh01)
**        Added.
**	14-jun-2001 (gupsh01)
**	    modified loc_buffer to an array.
**	16-oct-2001 (devjo01)
**	    Added loc_ptr to accomidate both settings of 'referredloc'.
**      09-May-2002 (gupsh02)
**          Added LC_getStdLocale call to obtain the standard name
**          for the locale, instead of generically setting it to 
**          UTF-8.
**	28-MAY-2002 (gupsh01)
**	    Modified error message E_XF0055_No_locale_found to 
**	    E_XF0055_No_encoding_found. 
**	27-Jun-2002 (gupsh01)
**	    Added LOcopy for referred_dtd case. 
*/
void
print_docType(char *dtd_filename, bool referredloc, char *title_doctype)
{
    LOCATION		tmploc;
    LOCATION		dtdloc;
    char		*loc_ptr;
    char		encoding[DB_MAXNAME];
    char		dtdlocation[MAX_LOC + 1];
    CL_ERR_DESC		cl_err;
    if (referredloc)
    {
	NMloc(FILES, FILENAME, dtd_filename, &tmploc);
	LOcopy(&tmploc, dtdlocation, &dtdloc);
	LOtos(&dtdloc, &loc_ptr);
    }	
    else 
	loc_ptr = dtd_filename;
    if (LC_getStdLocale (NULL, encoding, &cl_err) != OK )
    { 
      switch (cl_err.errnum)
      {
        case E_LC_LOCALE_NOT_FOUND:
          IIUGerr(E_XF0055_No_encoding_found, UG_ERR_ERROR, 0);
	  break;
        case E_LC_CHARSET_NOT_SET:
          IIUGerr(E_XF0056_Charset_not_set, UG_ERR_ERROR, 0);
          PCexit (FAIL);
	case E_LC_FORMAT_INCORRECT:
         IIUGerr(E_XF0058_LC_Format_incorrect, UG_ERR_ERROR, 
			1 , &cl_err.moreinfo[0].data._i4);
          PCexit (FAIL);
        default:
	  IIUGerr(E_XF0057_Unknown_LC_error, UG_ERR_ERROR, 0);
	  PCexit (FAIL);
      }
    }
    xfwrite(Xf_out, ERx("<?xml version=\'1.0\' ")); 
    xfwrite(Xf_out, ERx("encoding=\'"));
    xfwrite(Xf_out, ERx(encoding));
    xfwrite(Xf_out, ERx("\'?>\n"));
    xfwrite(Xf_out, ERx("<!DOCTYPE "));	
    xfwrite(Xf_out, ERx(title_doctype));	
    xfwrite(Xf_out, ERx(" SYSTEM \""));	
    xfwrite(Xf_out, loc_ptr);	
/*
    xfwrite(Xf_out, ERx("\" version=\'1.0\' "));
*/
    xfwrite(Xf_out, ERx("\" > \n"));
}
/*{
** Name: get_colcnt - gives a count of number of columns in the table. 
**
** Description:
**	 gives a count of number of columns in the table. 
**
** Inputs:
**       tp               Table pointer.
**
**      Returns:
**              none.
**      12-jun-2001 (gupsh01)
**	   Added.
*/
i4
get_colcnt(XF_TABINFO *tp)
{
   i4 cnt = 0;
   XF_COLINFO *cp;
   if (tp)
   {
	for (cp = tp->column_list; cp != NULL; cp = cp->col_next)
	cnt++;
   } 
   return cnt;
}
/*{
** Name:	Get_Handler - retrieve the data segments for a blob
**
** Description:
**
** Inputs:
**	hdlr - .
**
** Outputs:
**	none.
**
**	Returns:
**
** History:
**	21-sep-2001 (hanch04)
**	   create to retrieve blob data segments
**	30-aug-2005 (thaju02)
**	   Null terminate seg_buf.
*/
int
Get_HandlerLVCH(HDLR_PARAM *hdlr)
{
  char seg_buf[2001];
  int seg_len;
  int data_end;
  int max_len;
    char 	*col_name;
    char	loc_buffer[MAX_LOC + 1];
    TXT_HANDLE *Xf_row;
    max_len = 2000;
    seg_buf[max_len] = '\0';
    data_end = 0;
    col_name = hdlr->arg;
    STprintf( loc_buffer, "%s.row", hdlr->arg);
    /* open xml file */
    if ((Xf_row = xfopen(loc_buffer,0)) == NULL) 
    {
        PCexit (FAIL);
    }
    while (data_end == 0)
    {
/* # line 1984 "xfgenxml.sc" */	/* get data */
  {
    IILQlgd_LoGetData(3,32,2001,seg_buf,max_len,&seg_len,&data_end);
  }
/* # line 1989 "xfgenxml.sc" */	/* host code */
	xfwrite(Xf_row, seg_buf);
   }   
    /* close files */
    xfclose(Xf_row);
   return(0);
}
/*{
** Name:	Get_Handler - retrieve the data segments for a blob
**
** Description:
**		This function will retrieve the data segments for a blob
**		of type long byte.  It will then encode it in 
**		base64 as defined in RFC 1341.
**
** Inputs:
**	hdlr - .
**
** Outputs:
**	none.
**
**	Returns:
**
** History:
**	21-sep-2001 (hanch04)
**	   create to retrieve blob data segments
**	   not used yet.
*/
int
Get_HandlerLBYTE(HDLR_PARAM *hdlr)
{
  char seg_buf[2000];
  int seg_len;
  int data_end;
  int max_len;
    TXT_HANDLE	*Xf_blobdata = NULL;
    char 	*col_name;
    int i = 0;
    int j = 0;
    int n = 0;
    unsigned char c;
    unsigned char dtable[256];
    unsigned char out_seg[2665];
    unsigned char igroup[3], ogroup[4];
    bool hiteos = FALSE;
    for(i= 0;i<9;i++)
    {
	dtable[i]= 'A'+i;
	dtable[i+9]= 'J'+i;
	dtable[26+i]= 'a'+i;
	dtable[26+i+9]= 'j'+i;
    }
    for(i= 0;i<8;i++)
    {
	dtable[i+18]= 'S'+i;
	dtable[26+i+18]= 's'+i;
    }
    for(i= 0;i<10;i++)
    {
	dtable[52+i]= '0'+i;
    }
    dtable[62]= '+';
    dtable[63]= '/';
    max_len = 1998;
    data_end = 0;
    col_name = hdlr->arg;
    if( !printedRowTag )
    {
	printedRowTag = TRUE;
        xfwrite(Xf_out, "<row>\n");
    }
    xfwrite(Xf_out, ERx("<column \t column_name=\""));
    xfwrite(Xf_out, col_name);
    xfwrite(Xf_out, ERx("\" dt:type=\"bin.base64\">"));
    while (data_end == 0)
    {
/* # line 2076 "xfgenxml.sc" */	/* get data */
  {
    IILQlgd_LoGetData(3,32,2001,seg_buf,max_len,&seg_len,&data_end);
  }
/* # line 2081 "xfgenxml.sc" */	/* host code */
	hiteos = FALSE;
        i = j = 0;
	while(!hiteos)
	{
	    igroup[0]= igroup[1]= igroup[2]= 0;
	    for(n= 0;n<3;n++)
	    {
		c= seg_buf[i++];
		if( i > seg_len )
		{
		    hiteos= TRUE;
	            break;
	        }
		igroup[n]= (unsigned char)c;
	    }
	    if(n> 0)
	    {
		ogroup[0]= dtable[igroup[0]>>2];
		ogroup[1]= dtable[((igroup[0]&3)<<4)|(igroup[1]>>4)];
		ogroup[2]= dtable[((igroup[1]&0xF)<<2)|(igroup[2]>>6)];
		ogroup[3]= dtable[igroup[2]&0x3F];
		if(n<3)
		{
		    ogroup[3]= '=';
		    if(n<2)
		    {
		        ogroup[2]= '=';
		    }
		}
		for(n= 0;n<4;n++)
		{
		    out_seg[j++] = ogroup[n];
		}
	    }
	}
        out_seg[j] = '\0';
	xfwrite(Xf_out, (PTR)out_seg);
   }
   xfwrite(Xf_out, "</column>\n");
   return(0);
}
