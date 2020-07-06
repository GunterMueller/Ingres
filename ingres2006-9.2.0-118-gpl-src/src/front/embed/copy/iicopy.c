/*
** Copyright (c) 1986, 2005 Ingres Corporation
**
*/
 
# include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
# include	<lo.h>
# include	<si.h>
# include	<cm.h>
# include	<st.h>
# include	<ex.h>
# include	<nm.h>
# include	<er.h>
# include	<me.h>
# include	<gl.h>
# include	<sl.h>
# include       <mecl.h>
# include	<iicommon.h>
# include	<generr.h>
# include	<copy.h>
# include	<adf.h>
# include	<adp.h>
# include	<ade.h>
# include	<gca.h>
# include	<eqrun.h>
# include	<iicgca.h>
# include	<iirowdsc.h>
# include	<iisqlca.h>
# include	<iilibq.h>
# include	<iicopy.h>
# include	<erco.h>

#ifdef WIN16
# include	<sqlstate.h>
# include	<fe.h>
#endif /* WIN16 */

# define MAX_SEG_FROM_FILE MAXI2 /* Maximum segment size FROM file */
# define MAX_SEG_INTO_FILE 4096  /* Maximum segment size INTO file */

typedef struct {
	II_LBQ_CB	*libq_cb;
	FILE		*file_ptr;
	PTR		tupbuf;
	char		*ucv_buf;
	i4		ucv_len;
	i4		rowcount;
	DB_DT_ID	tuptype;
	i2		firstseglen;
	bool		iseof;
	bool		failure;
	char		*col_name;
	i4		col_num;
	bool		cp_program;
	FILE		*cp_dbg_ptr;
	i4		(*get_row)();
} LOBULKFROM_HDL;

typedef struct {
	II_LBQ_CB	*libq_cb;
	FILE		*file_ptr;
	PTR		tupbuf;
	char		*ucv_buf;
	i4		ucv_len;
	i4		rowcount;
	DB_DT_ID	tuptype;
	char		*col_name;
	bool		failure;
	bool		cp_program;
	FILE		*cp_dbg_ptr;
	i4		(*put_row)();
} LOBULKINTO_HDL;

/* 
**  Name: loc_alloc_i - a version of loc_alloc() that initializes the memory 
**
**  History:
**	05-jul-2000 (wanfr01)
**	    Written.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      04-jun-2002 (stial01)
**	    Change MAX_SEG_FILE from 4096 to MAXI2 (b107934)
**      06-jun-2002 (stial01)
**	    Backout 04-jun-2002 change
**      07-jun-2002 (stial01)
**	    Copy table FROM file, support segment size MAXI2
**      24-jan-2005 (stial01)
**          Fixes for binary copy LONG NVARCHAR (DB_LNVCHR_TYPE) (b113792)
*/
 
static STATUS
loc_alloc_i(n1,n2,p)
i4 n1,n2;
PTR *p;
{
      *p = MEreqmem(0,n1*n2,TRUE,NULL);

      if (*p == NULL)
              return(FAIL);
      return(OK);
}
 
static STATUS
loc_alloc(n1,n2,p)
i4 n1,n2;
PTR *p;
{
	*p = MEreqmem(0,n1*n2,FALSE,NULL);
 
	if (*p == NULL)
		return(FAIL);
	return(OK);
}
 
 
/*
+*  Name: iicopy.c - Runtime support for COPY command.  Contains routines
**	needed in Front End to execute COPY.
**
**  Defines:
**	IIcopy		- Drives COPY
**	IIcpinit	- Initialize for COPY processing
**	IIcptdescr	- Allocate and build GCA tuple descriptor.
**	IIcpfrom_init	- Initialize for next row during COPY FROM
**	IIcpinto_init	- Initialize for next row during COPY INTO
**	IIcptupformat	- Convert row to tuple format for COPY FROM
**	IIcprowformat	- Convert tuple to row format for COPY INTO
**	IIcpfopen	- Open copy file
**	IIcpgetrow	- Get next row during COPY FROM
**	IIcpputrow	- Put next row during COPY INTO
**	IIcpfclose	- Close copy file
**	IIcpgetch	- Char handler for IIcpgetrow
**	IIcpgetbytes	- Bytes handler for IIcpgetrow
**	IIcpdbwrite	- Write tuple to the DBMS during COPY FROM
**	IIcpdbread	- Read tuple from the DBMS during COPY INTO
**	IIcpdbflush	- Flush output to the DBMS
**	IIcpendcopy	- Do FE/DBMS handshaking at end of copy
**	IIcplgopen	- Open bad-row log file
**	IIcplgrow	- Write row to bad-row log file
**	IIcplgclose	- Close bad-row log file
**	IIcpcleanup	- Clean up memory, close files, etc. ...
**	IIcpabort	- Communicate with the DBMS to abort the COPY command.
**	IIcp_handler	- Handle interrupts during COPY
**
-*
**  History:
**	01-dec-1986 (rogerk)
**	    Written.
**	02-apr-1987 (ncg)
**	    Modified for new row descriptors.
**	24-apr-1987
**	    Add logging of bad rows, bug fixes.
**	09-jun-1987
**	    Write out varchar types with ascii len specifier.
**	25-aug-87 (puree)
**	    fix varchar, text, and variable length field
**	27-aug-87 (puree)
**	    fix exception handler for control-C
**	04-sep-87 (puree)
**	    change IIlocerr interface.
**	09-sep-87 (puree)
**	    change error message codes sent to IIlocerr.
**	10-sep-87 (puree)
**	    convert to GCF architecture.
**	17-nov-87 (puree)
**	    fixed IIcpendcopy for rollback option, fixed many places
**	    for better error handling.
**	24-feb-88 (puree)
**	    Modified IIcpinit to null terminate column names for better
**	    error reporting format.
**	04-aug-88 (neil)
**	    Added some consistency checks throughout the file (validating
**	    data sizes, etc).
**	11/11/88 (dkh/barabar) - Changed code in IIcpgetrow to synchronize
**	    "row" and "row_ptr" correctly when the buffer size is increased.
**	14-nov-88 (barbara)
**	    fixed bugs #3912, #3695 and #3314
**	12-dec-88 (neil)
**	    generic error processing changed all calls to IIlocerr plus some
**	    lint checks.
**	20-jan-89 (barbara)
**	    fixed bug #4370.  Also fixed COPY INTO bug where delimiter appeared
**	    prematurely in fixed field output.
**	21-jun-89 (barbara)
**	    fixed bug #6380.  dummy 'nul' format was not copying null[s]
**	    into file.
**	14-jul-89 (barbara)
**	    fixed bug #5798.  default null format was not being blank padded
**	    on COPY INTO using variable char or c format.
**	22-sep-89 (barbara)
**	    updated for decimal.
**	26-nov-89 (barbara)
**	    fixed bug 8886.  On COPY TO don't pad default null value unless
**	    column width is wider than length of default null value.
**	22-jan-90 (barbara)
**	    Fixed up calls to LOfroms.
**	24-jan-90 (fredp)
**	    Fixed alignment problems with DB_TEXT_STRING data types in 
**	    IIcprowformat() and IIcptupformat().
**	06-feb-90 (fredp)
**	    Fixed alignment problems for non-string datatypes too.
**	07-feb-90 (fredp)
**	    Fixed an alignment problem in IIcpinit() where adc_getempty().
**	    was called with unaligned DB_DATA_VALUEs.
**	14-feb-90 (fredp)
**	    Fix a bug introduced in the alignment fix of 06-feb-90.
**	15-feb-90 (fredp)
**	    Initialize align_buf in IIcptupformat().
**	26-feb-1990 (barbara)
**	    To enhance SandyH's libqgca fix to bug #7927, IICGC_COPY_MASK 
**	    is now turned off in IIcgc_read_results.  Removed code in this
**	    module that turned off IICGC_COPY_MASK flag.
**	05-apr-90 (teresal)
**	    Fix for bug 8725 so warning messages show correct row count for
**	    number of rows copied.  Also, added increment of row count when
**	    doing a COPY INTO and the number of badrows has exceeded max
**	    errors or else warning message's row count won't be accurate.
**	27-apr-1990 (barbara)
**	    All the clean up of byte alignment problems for varchar and text
**	    has exposed some bugs in the way we were copying nullable varchar
**	    and text formats.  Basically, these formats were getting copied
**	    into and read from file as "internal" data.  Fixed this.
**	03-may-1990 (barbara)
**	    IIcptupformat should only copy from aligned buf to tup buf if
**	    cp_convert is set.
**	18-jun-1990 (barbara)
**	    Changes for bug fix 21832.  LIBQ's new default behavior is to
**	    not quit on association failures.  Changes needed in IIcpdbread
**	    and IIcpendcopy.
**	24-jul-1990 (barbara)
**	    One more byte alignment problem in IIcptupformat.  When fieldlen
**	    is zero, adc_valchk was being called with possibly non-aligned data.
**	07-aug-1990 (mgw) Bug # 21432
**	    Report user error too in IIcptupformat if applicable.
**	22-may-90 (fredb)
**	    MPE/XL (hp9_mpe) specific change in IIcpinit to force the copy
**	    into file type to VAR.
**      02-oct-90 (teresal)
**          Modified IIcptupformat for bug fix 32562.  Give an error if
**          COPY FROM attempts to put NULL data into a non-nullable field
**          using the 'with null' option.
**	08-oct-90 (teresal)
**	    Bug fix 20335: fixed so a warning message will be displayed
**	    whenever character data is truncated when COPY FROM is done.
**      15-oct-90 (teresal) 
**	    Bug fix 20335: Took out this bug fix as this caused problems
**	    when 'value' in WITH NULL option was longer than the column's
**	    length.
**	19-oct-90 (teresal)
**	    Bug fix 20335: Put in a new bug fix to display a warning
**	    message when character data is truncated for variable length
**	    copy formats.  This fix handles a null value which is longer
**	    then the column's length by not truncating this value.
**	30-nov-90 (teresal)
**	    Bug fix 34405.  Fixed so COPY's ending error/warning messages
**	    will show the correct row count copied if rows with duplicate 
**	    keys were found. To comply with SQL semantics, a warning 
**	    message "COPY: Warning: %0c rows not copied because duplicate 
**	    key detected" will be displayed, for SQL, if the number 
**	    of rows added by the server is less than the number of rows 
**	    sent by COPY.  A requirement of the COPY dbms/fe interface 
**	    is: if the number of rows added by the server is less than 
**	    the number of rows sent by fe COPY, COPY can assume that 
**	    the rows were not added because duplicate key data was found.
**	26-dec-90 (teresal)
**	    Modified IIcpinit to display an error if a TEXT filetype
**	    is specified with a bulk copy.  Bug 6936.
**	04-jan-91 (teresal)
**	    Modified IIcpinit to only look for a filetype if VMS.  This
**	    fix was requested from ICL so filenames with commas can 
**	    be used.
**	10-jan-91 (teresal)
**	    Bug fix 35154.  COPY INTO with format "varchar(0)" was 
**	    causing an AV on UNIX because the row buffer allocated 
**	    wasn't big enough.  Modified IIcpinit to calculate
**	    "cp_row_length" correctly.
**	22-feb-91 (teresal)
**	    Bug fix 36059: avoid ERlookup error by calling IIdberr
**	    rather than IIlocerr to display formatted ADF error.
**	    Problem occurred when error message has >= 1 parameters.
**	29-mar-91 (barbara)
**	    Extended COPY FROM PROGRAM support to allow copying variable
**	    length rows.  Abstracted the IIcpgetrow code that "gets" a char
**	    (either from file or from user buffer).
**	09-apr-91 (barbara)
**	    Fixed bug (missing argument on call to IIcpgetbytes).
**	11-apr-91 (barbara)
**	    Fixed bug 36900.  IIcpgetbytes was complaining about EOF even
**	    in legitimate places. 
**	06-may-91 (teresal)
**	    Bug fix 37341 - avoid AV on unix when varchar(0) has a null string
**	    longer than the field.  This bug was fixed previously but broke 
**	    when bug fix 35154 was put in.  "cp_nullen" as sent from the 
**	    DBMS uses a 2 byte length specifier, we need to allocate 5 bytes 
**	    for writing out the null string to the file.
**	06-aug-1991 (teresal)
**	    Modify IIcpinit to accept a new copy map format (GCA Protocol 
**	    level 50). The 'with null' value is now sent as a DBV rather than 
**	    as a character data string.  This fixes bug 37564 where
**	    the null value wasn't being converted correctly over a
**	    hetnet connection.
**	29-nov-91 (seg)
**	    EXdeclare takes a (STATUS (*)()), not (EX (*)()).
**	    Can't dereference or do arithmetic on PTR
**	    Fixed bogus if statement in IIcpgetrow.
**	07-feb-1992 (teresal)
**	    Bug fix 41780: Modify IIcptupformat to validate DATE fields in
**	    the file for a COPY FROM when the copy format is
**	    a DATE type and tuple format is a DATE type. A DATE to DATE
**	    coersion doesn't validate that the date is in an internal 
**	    INGRES format, thus, COPY was causing invalid dates to get 
**	    into the database.
**	22-jul-1992 (sweeney)
**	    Switch off optimization for apl_us5 - compiler fails with:
**	    [Error #144]  Compiler failure, registers locked.
**	13-dec-1992 (teresal)
**	    For decimal changes, modify all references to the old global adf 
**	    control block and instead reference the new session specific 
**	    adf control block.
**	15-mar-1993 (mgw)
**	    Added handling for copying large objects INTO file. Also added
**	    limitted handling for copying large objects FROM file, but left
**	    that disabled since it doesn't work yet.
**	13-sep-1993 (dianeh)
**	    Removed NO_OPTIM setting for obsolete Apollo port (apl_us5).
**	23-sep-1993 (mgw)
**	    Add DB_VBYTE_TYPE cases to enable proper handling of BYTE
**	    Varying datatype.
**	01-oct-1993 (kathryn)
**	    First parameter to IILQlgd_LoGetData and IILQlpd_LoPutData
**	    (routines to get and put segments of large objects) has been
**	    changed to a bit mask.  Valid values are defined in eqrun.h.
**	17-feb-1994 (johnst)
**	    Bug #60013
**	    Integrated the (kchin) 6.4/03 fix for platforms with 64-bit 
**	    pointers (eg DEC alpha axp_osf):
**          "roll the GCA_COL_ATT element by element to avoid inclusion of 
**	    structure pads present on 64 bit pointer machines."
**	    Also, for earlier Bug #58882 change, corrected file merge problem 
**	    where the "#define NOT_ALIGNED..." line was not deleted in the 
**	    submission. Delete the line now.
**	20-feb-1995 (shero03)
**	    Bug #65565
**	    Support long User Data Types
**      12-sep-1995 (lawst01)
**          Bug #71149
**          If delimiter is omitted from format c0 field - default to '\n'
**          delimiter.
**	28-sep-1995 (kch)
**	    Add new argument (end_of_tuple) to function IICPcfgCheckForGaps().
**	    This change fixes bug 71595.
**	31-oct-95 (kch)
**	    In the function IIcopy(), switch off II_LO_END flag for ascii
**	    'copy ... from' in Large Object Info structure
**	    (IIlbqcb->ii_lq_lodata.ii_lo_flags). This allows multiple ascii
**	    'copy ... froms' in the same tm/itm session. This change fixes
**	    bug 72235.
**	07-nov-1995 (kch)
**	    Bug 72054: In the function IIcpinit(), the blob check has been
**	    changed from (IIDT_LONGTYPE_MACRO && IIDT_UDT_MACRO) to
**	    (IIDT_LONGTYPE_MACRO || IIDT_UDT_MACRO). The AND case could
**	    never be true; with the two macros ORed, an ascii 'copy .. into'
**	    will write the correct null blob values to the data file.
**	    Bug 72172: In the function IICPfbwFmtBlobWrite(), the null
**	    indicator variable, nulblob, is now reset to zero at the start of
**	    each domain in order to allow an ascii 'copy ... from' of data
**	    containing multiple rows, at least one of which contains a null
**	    blob.
**	21-nov-1995 (kch)
**	    In the function IIcpinit(), do not set copy_blk->cp_varrows to 1
**	    if the column is dummy. This change fixes bug 72678.
**	01-mar-1996 (kch)
**	    In the function IICPbbwBulkBlobWrite(), the null indicator
**	    variable, nulblob, is now reset to zero at the start of
**	    each domain in order to allow an bulk 'copy ... from' of data
**	    containing multiple rows, at least one of which contains a null
**	    blob. This change fixes bug 74057.
**      12-Dec-95 (fanra01)
**          Added definitions for extracting data for DLLs on windows NT
**      06-Feb-96 (fanra01)
**          Made data extraction generic.
**	15-feb-1996 (canor01)
**	    Bug 73304: on Windows NT.  Accept the default delimiters as
**	    text delimiters when determining how to open input file.
**	25-mar-96 (kch)
**	    In the functions IIcpgetrow() and IICPfbwFmtBlobWrite() we
**	    now set end_ptr once before the domain by domain read from the
**	    copy file (calls to IICPgcGetCol()). Previously, end_ptr was
**	    being wrongly moved after each domain, and this was preventing
**	    new memory from being allocated for the row buffer, row_ptr, in
**	    IICPgcGetCol(). Subsequently, if the row width was greater than
**	    8K, unallocated memory could be written to during the filling
**	    of the row buffer.
**	    In IICPgcGetCol(), end_ptr is now correctly reset after the
**	    allocation of a bigger row buffer.
**	    These changes fixes bug 75452.
**	26-mar-96 (kch)
**	    In the function IICPgcGetCol(), we test for delimited fixed width
**	    columns, and if found, increment the byte count in the call to
**	    IIcpgetbytes(). Similarly, the row buffer, row_ptr, is incremented
**	    by an extra byte. This change fixes bug 75437, which is a side
**	    effect of the previous fix for bug 72678 (the fix for 72678 has
**	    been backed out in the function IIcpinit()).
**	05-apr-1996 (loera01)
**	    Modified  IIcpinit() to support compressed variable-length
**	    datatypes.
**      14-jun-1996 (loera01)
**          Modified IIcpdbread(),IICPbbrBulkBlobRead(),IICPbdrBlobDbRead() 
**          so that variable-length datatype decompression is performed
**          in LIBQ, rather than in LIBQGCA.  A new routine, IICPdecompress(),
**          decompresses individual rows for bulk copy.
**	10-jul-96 (chech02)
**	    bug#77549: When calls to STprintf("%d"), arguments should be nat.
**      16-jul-96 (loera01)
**          Modified IIICPbdrBlobDbRead() so that single columns may be 
**          copied if the row contains one or more compressed varchars. Fixes
**          bug 77127.
**      13-aug-96 (loera01)
**          Modified IICPbbrBulkBlobRead() and IICPbdrBlobDbRead() to 
**          correctly handle partial tuples.  Fixes bug 77129.
**          Modified IIcpinit() so that the allocation of cp_rowbuf 
**          and cp_tupbuf consist of the row or tuple length plus MAX_SEG_FILE.
**          With variable page size, non-blob columns plus a blob may be sent 
**	    in the same message from the back end.  Fixes bug 77129.
**          Fixed memory leaks in IIcpdbread(),IICPdecompress,
**          IICPbdrBlobDbRead(), and IICPbbrBulkBlobRead().
**	    For IICPfbwFmtBlobWrite(), re-initialize copy_blk->cp_zerotup 
**          with saved pointer value, instead of decrementing by 
**	    copy_blk->cp_tup_length.  If the tuple contained a blob and a 
**          skipped non-blob column, the pointer did not advance by the 
**	    full tuple length.
**	21-aug-96 (chech02) bug#77549
**	    'stack overflow' if doing a 'copy ... from' statement from a file 
**	    that has too many rows. Fixes: Moved txt_buf, align_buf and 
**	    alignrow_buf in  IIcptupformat() from stack area to static data 
**	    area for windows 3.1 port.
**	04-sep-96 (thoda04)
**	    Last arg of IIcgcp3_write_data() takes addr of dbv, not dbv itself.
**	03-oct-1996 (chech02) bug#78827
**	    moved local variables to global heap area for windows 3.1
**	    port to prevent 'stack overflow'.
**	20-nov-96 (kch)
**	    In the function IIcpinit(), we now do not add the default delimiter
**	    ('\0') for a copy into of non varchar formats that have no
**	    delimiter specified. This change fixes bug 79476 (side effect of
**	    the fixes for bugs 71149 and 73304).
**	20-feb-97 (kch)
**	    In the function IICPfbwFmtBlobWrite(), we now save the width of
**	    each column in the zero tuple and use this to move to the start of
**	    the zero tuple when the domain by domain processing is done. This
**	    change fixes bug 79834 (side effect of fix for bug 73123).
**	02-dec-1996 (donc)
**	    Add an other arg to an SIfprintf call. This will 1) allow the
**	    module to compile with compilers w/ strict argument checking and 2)
**	    probably NOT gpf (the old call would have likely done this).
**	18-dec-96 (chech02)
**	    Fixed redefined symbols error: ii_copy_struct.
**	04-apr-1997 (canor01)
**	    Windows NT must always open the file in binary mode
**	    when reading, and do the CRLF=>LF translation at
**	    character read time to prevent ^Z character from
**	    causing EOF.
**	25-apr-1997 (somsa01)
**	    Due to the previous change to IIcpgetbytes(), the "status" variable
**	    was not being set, yet it was being checked. Thus, on success from
**	    the NT_GENERIC loop, set status=OK. Also, needed to propagate
**	    previous changes to IIcpgetrow() as well, since ^Z  characters are
**	    allowed in varchars as well.
**	22-may-97 (hayke02)
**	    In iicpendcopy(), tighten up the check for duplicate rows
**	    (E_CO003F) - the number of rows copied must be greater than zero.
**	    If zero rows were copied (probably due to running out of disk
**	    space during a copy into a non heap table), return E_CO0029.
**	    This change fixes bug 80478.
**  6-jun-1997 (thaju02)
**	    bug #82109 - 'copy into' with blob column resulted in E_LQ00E1
**	    error. Error due to #ifdef PTR_BITS_64 code, which mutilated
**	    cgc->cgc_tdescr->gca_col_desc[]; in the transposing of data to
**	    td_ptr, neglected to account for the gca_attname field, and
**	    also the MEcopy destination parameter is incorrect (it is
**	    (char *)&cgc->cgc_tdescr->gca_col_desc[0].gca_attdbv, but should
**	    be (char *)&cgc->cgc_tdescr->gca_col_desc[0]), resulting in
**	    us making a complete mess of cgc->cgc_tdesc->gca_col_desc[]. This
**	    #ifdef PTR_BITS_64 was added (17-feb-94) to remove extra
**	    alignment padding added by the compiler, because gca_to_fmt() was/is
**	    not expecting padding. But #ifdef PTR_BITS_64 is not necessary
**	    since the gca_col_desc[] structure/fields are being referenced
**	    through the structure/fields and not by offsets in gca_to_fmt(),
**	    thus as long as we are consistent in accessing the gca_col_desc[] 
**	    thru indices and structure/fields, no need to fiddle with it.
**	17-Sep-97 (fanra01)
**	    Bug 85435
**	    Switch off varchar compression when performing COPY FROM as this
**	    is not supported and can cause net to misinterpret the tuple
**	    contents.
**	18-Nov-1997 (thaju02)
**	    Back out of 6-jun-1997 (thaju02) change for bug #82109, caused
**	    hetnet problems. 
**	18-Nov-1997 (thaju02)
**	    Bug #82109 - for PTR_BITS_64, attempting to remove extra padding
**	    from cgc->cgc_tdescr, trashed the gca row descriptor. In IIcopy()
**	    build the cgc->cgc_tdescr from the column attributes in the
**	    gca row descriptor in gca byte stream format.
**	18-Dec-97 (gordy)
**	    Added support for multi-threaded applications.
**      04-Aug-1998 (wanya01)
**          X-integrate change #436526. Bug 91904. 	    
**          In IIcpinit().  no 'continue' when incrementing long_count on non
**          byte-aligned platforms. Led to exception in MEdoAlloc() when
**          coping in ascii tables with long byte/varchar columns. 
**	14-apr-1998 (shust01)
**	    In IICPfbwFmtBlobWrite(), the variable copy_blk->cp_zerotup
**	    is being incremented within the for loop.  If we prematurely
**	    exit the routine before the for loop completes, the variable
**	    is never reset.  If variable is not reset, could SIGBUS when
**	    MEfree storage.  Changed all return statements within loop to
**	    breaks, to allow the variable to be reset before returning to
**	    calling routine.  bug 93805.
**    	23-nov-98 (holgl01) bug 93563
**          In IIcpfopen(), set rms buffer for vms systems for a TXT (text) 
**          file to the maximum record that can be written (SI_MAX_TXT_REC) 
**	    for SIfopen. Otherwise, files greater than 4096 in length cannot
**	    be opened.
**	22-dec-1998 (holgl01) Revision of Bug 93563
**	    Pass SI_MAX_TXT_REC directly  to SIfopen in IIcpfopen
**	    instead of resetting  cp_row_length to SI_MAX_TXT_REC 
**          and passing that.
**	05-jan-1998 (kinte01) 
**	    Modify previous change to use #elif defined(VMS) instead of 
**	    #elif VMS to satisfy problems with DEC C compiler
**	23-Feb-1999 (toumi01)
**	    Add NO_OPTIM for Linux (lnx_us5) to avoid these errors
**		E_CO0040 COPY : Unexpected end of user data ...
**		E_CO002A COPY : Copy has been aborted.
**	    running (e.g.) sep tests acc00.sep and cpy15.sep.  This has also
**	    occurred in user situations.  This seems to be a compiler bug,
**	    and occurs in both libc5 and libc6 versions of Linux using the
**	    gcc compiler.  The problem occurs in routine IIcpdbread when
**	    calls are made to IICPdecompress and (it appears that) the bool
**	    "partial" is not set on return.  Debugging is difficult, since
**	    any code changes or OPTIM changes get rid of the bug.  Even adding
**	    an uncalled routine to the end of the assembler deck gets around
**	    the problem.  See bug 94449.
**	18-jun-1999 (schte01) 
**	    Added cast macros to avoid unaligned acccesses for axp_osf. 
**	16-Aug-1999 (hanch04)
**	    Bug 98420
**	    Need to check the return value for IIcpgetch in IICPgcGetCol
**	    Removed NO_OPTIM for Linux (lnx_us5)
**      02-Sep-1999 (hweho01)
**          Changed EXCONTINUE to EXCONTINUES to avoid compile error of
**          redefinition on AIX 4.3, it is used in <sys/context.h>.
**	17-jan-2000 (toumi01)
**	    NO_OPTIM for Linux has been removed; problem may be fixed by
**	    hanch04 change for bug 98420.  This is a hand-modification of
**	    the integration of change 444204 from oping20 to main, which
**	    would have changed the NO_OPTIM string from lnx_us5 to int_lnx.
**      10-Mar-2000 (hweho01)
**          Moved ex.h after erco.h to avoid compile errors from
**          replacement of jmpbuf for ris_u64. 
**      10-Oct-2000 (hweho01)
**          Removed the above change, it is not needed after change #446542 
**          in excl.h.
**      11-May-2001 (loera01)
**          Added support for Unicode varchar compression.
**	21-Dec-2001 (gordy)
**	    Removed PTR from GCA_COL_ATT and updated associated access code.
**	    Removed PTR_BITS_64 code as no longer relevant.  However, had to
**	    introduce a similar function for all platforms to generate a GCA
**	    tuple descriptor from a row descriptor since the two now differ.
**	20-jan-2004 (somsa01)
**	    Updated "COPY FROM/INTO... PROGRAM" case to work with BLOBs.
**	03-feb-2004 (somsa01)
**	    Make sure we pass an i4 as the segment length to get_row() in the
**	    the case of "COPY FROM/INTO... PROGRAM" with BLOBs.
**      05-jul-2005 (mutma03)
**          Filtered ctrl<M> from the column data from the files that were
**          generated from Windows to run on other platforms.
**	    if we are ending on a row boundary. (B116562)
**	07-sep-2005 (abbjo03)
**	    Replace xxx_FILE constants by SI_xxx.
**	12-oct-2006 (thaju02)
**	    In IICPfbrFmtBlobRead, also check lastattr to determine
*/ 

GCA_TD_DATA	*IIcptdescr( ROW_DESC * );
VOID		IIcplgrow();
VOID		IIcpsstatus();
VOID		IIcpcleanup();
VOID		IIcgcp1_write_ack();
VOID		IIcgcp2_write_response();
STATUS		IIcgcp3_write_data();
i4		IIcgcp5_read_data();
i4		IIcgcp4_write_read_poll();
STATUS		IIcp_handler(EX_ARGS *exargs);
void		IICPbbiBulkBlobInto();
void    	IICPdecompress();
void		IICPwemWriteErrorMsg();
void		IICPbbfBulkBlobFrom();
void		IICPfbfFmtBlobFrom();
void		IICPfbiFmtBlobInto();
i4		IICPfslFmtSegLen();
void		IICPfbFlushBlob();
void		IICPsebSendEmptyBlob();
STATUS		IIcc1_send();
void            IIrm_ctrl_m(II_CP_MAP *copy_map,char **row_ptr,char *start_ptr);
static char	*gtr_type();
 

/*{
+*  Name: IIcopy - Drives FE side of COPY protocol.
**
**  Description:
**	IIcopy does all of FE side of COPY.
**
**	During COPY INTO, it reads blocks of tuples from the DBMS, converts
**	them to the copy file format and writes them into the COPY FILE.
**
**	During COPY FROM, it reads rows from a COPY FILE, converts them to
**	tuples, and sends blocks of tuples to the DBMS for loading.
**
**	IIcopy is called from LIBQ.  The front end sends a copy statement off
**	to the DBMS, and the DBMS sends back a GCA_C_INTO or a GCA_C_FROM 
**	message block signalling that a copy is to be performed.  This block
**	contains all the information needed for the front end to process the
**	COPY statement.  When LIBQ receives the GCA_C_INTO or the GCA_C_FROM
**	block, it will call IIcopy.  IIcopy will remain in control until the 
**	copy is completed or interrupted by the user.
**
**  Inputs:
**	msg_type	- Message type to IIcopy.
**	user_routine	- user routine to get/put copy rows, rather than doing
**			  IO to a copy file.  Described above.
**
**  Outputs:
**	Returns: OK, FAIL
-*
**  Side Effects:
**	IICGC_COPY_MASK is set in IIlbqcb->ii_lq_gca->cgc_g_state.  This will
**	    prevent LIBQ from calling IIcopy again while already processing
**	    a COPY.  This bit will be turned off when COPY is done.
**	The libq control block contains an ADF_CB that is used throughout libq.
**	    This ADF control block keeps track of the number of noprintable
**	    characters that ADF encountered and just converted to blank.  This
**	    field is set to zero so that we can count just those that occur
**	    during copy.
**
**  History:
**	01-dec-1986 (rogerk)
**	    Written.
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr parameter.
**	10-sep-87 (puree)
**	    convert to GCF architecture.
**	04-aug-88 (neil)
**	    added check to trace statement.
**	31-oct-95 (kch)
**	    switch off II_LO_END flag for ascii 'copy ... from' in Large
**	    Object Info structure (IIlbqcb->ii_lq_lodata.ii_lo_flags).
**	    This allows multiple ascii 'copy ... froms' in the same
**	    tm/itm session. This change fixes bug 72235.
**	21-Dec-01 (gordy)
**	    Removed PTR_BITS_64 code since GCA_COL_ATT no longer has PTR. 
**	27-jan-2004 (somsa01)
**	    Pass user_routine to BLOB handler routines.
**	31-oct-2006 (kibro01) bug 116565
**	    If we can't open the output file for a 'copy ... into' then
**	    end cleanly rather than aborting so the server knows to get
**	    rid of the structures associated with the query.
*/

i4
IIcopy(msg_type, user_routine)
i4	    msg_type;			/* GCA message type */
i4	    (*user_routine)();		/* Function to get/put rows */
{
    II_THR_CB		*thr_cb = IILQthThread();
    II_LBQ_CB		*IIlbqcb = thr_cb->ii_th_session;
    IICGC_DESC		*cgc = IIlbqcb->ii_lq_gca;
    PTR			row;
    PTR			tuple;
    i4			status;
    i4			save_truncated;
#ifdef WIN16
    static II_CP_STRUCT	copy_blk;
#else
    II_CP_STRUCT	copy_blk;
#endif /* WIN16 */
    EX_CONTEXT		excontext;
    GCA_AK_DATA		gca_ack;
    II_CPY_BUFH		cpy_bufh;	/* COPY tuple buffer header */
    char		*copydbgbuf = NULL;
    char		*copydbgbuf2 = NULL;
 
    /*
    ** Set mask signifying that a COPY is in progress.
    **
    ** LIBQ calls IIcopy when it gets a GCA_C_INTO or GCA_C_FROM block from
    ** the DBMS.  If this routine has been called directly from LIBQ, we don't
    ** want to call IIcopy again, so this bit tells LIBQ that we are already
    ** in copy processing.  This also prevents multiple IIcopy calls if the 
    ** startup information requires more than one GCA_C_INTO/GCA_C_FROM block.
    */
 
    cgc->cgc_g_state |= IICGC_COPY_MASK;
    cgc->cgc_tdescr = (GCA_TD_DATA *)NULL;

    /* Trace COPY statement if LIBQ is in tracing mode */
    if ( ! (IIglbcb->ii_gl_flags & II_G_DEBUG) )
	IIdbg_dump( IIlbqcb );
 
    /*
    ** Set global pointer to copy struct and define interrupt handler.  If
    ** an interrupt is received, the interrupt handler will use the copy
    ** struct to determine what has to be cleaned up.  Zero out copy struct
    ** before declaring handler, so we know that handler can trust that the
    ** values in the struct aren't garbage.
    */
    MEfill((u_i2) sizeof(II_CP_STRUCT), (unsigned char)'\0', (PTR) &copy_blk);
    thr_cb->ii_th_copy = (PTR)&copy_blk;
    if (EXdeclare(IIcp_handler, &excontext))
    {
	/* Should never get to here!! */
	IIlocerr(GE_LOGICAL_ERROR, E_CO0022_COPY_INIT_ERR, II_FAT,
		 0, (char *)0);
	IIcpabort( IIlbqcb, &copy_blk );
	EXdelete();
	return (FAIL);
    }
    /*
    ** Build copy struct from GCA_C_INTO/GCA_C_FROM block.
    **
    ** This fills in all the information on the tuple and row formats, as
    ** well as the ADF conversion information on how go from one to the other.
    */
    if ((status = IIcpinit( IIlbqcb, &copy_blk, &cpy_bufh, msg_type)) != OK)
    {
	/*
	** If status is FAIL, then error has already been reported by IIcpinit.
	** Otherwise, we need to report the error.
	*/
	if (status != FAIL)
	    IIlocerr(GE_LOGICAL_ERROR, status, II_ERR, 0, (char *)0);
 
	IIcpabort( IIlbqcb, &copy_blk );
	EXdelete();
	return (FAIL);
    }

    /* print out the II_CP_STRUCT and II_CP_MAP structures for debugging */
    NMgtAt(ERx("II_COPY_DBG_DUMP"), &copydbgbuf);
    if ((copydbgbuf != NULL) && (*copydbgbuf != EOS))
	IIcpdbgdump( IIlbqcb, &copy_blk, copydbgbuf );

    /* Check for more debug detail */
    copy_blk.cp_dbgname = NULL;
    copy_blk.cp_dbg_ptr= NULL;
    NMgtAt(ERx("II_COPY_DEBUG"), &copydbgbuf2);
    if ((copydbgbuf2 != NULL) && (*copydbgbuf2 != EOS))
    {
	i4	dbg_len;

	dbg_len = STlength(copydbgbuf2);
	if (loc_alloc((i4)1, (i4)dbg_len, (PTR *)&copy_blk.cp_dbgname) == OK)
	    MEcopy(copydbgbuf2, dbg_len, copy_blk.cp_dbgname);
	IIcpdbgopen(&copy_blk);
    }

    /*
    ** If this is a normal (not in_memory) copy, then open the copy file.
    */
    if (!copy_blk.cp_program)
    {
	if ((status = IIcpfopen(&copy_blk)) != OK)
	{
	    IIlocerr(GE_LOGICAL_ERROR, E_CO0022_COPY_INIT_ERR, II_FAT,
		     0, (char *)0);
	    /* Note the error, but end nicely - this is a problem caused
	    ** entirely at the client end, so the server needs to be
	    ** told about it to get rid of QEF plans
	    ** Bug 116565 (kibro01)
	    */
	    copy_blk.cp_error = TRUE;
	    IIcpendcopy( IIlbqcb, &copy_blk, &cpy_bufh );
	    EXdelete();
	    return (FAIL);
	}
    }
    /*
    ** If error row logging has been requested, open the error row log.
    */
    if (copy_blk.cp_rowlog)
    {
	if ((status = IIcplgopen(&copy_blk)) != OK)
	{
	    IIlocerr(GE_LOGICAL_ERROR, E_CO0022_COPY_INIT_ERR, II_FAT,
		     0, (char *)0);
	    IIcpabort( IIlbqcb, &copy_blk );
	    EXdelete();
	    return (FAIL);
	}
    }

    if ( copy_blk.cp_has_blobs )
    {
	/* Turn on copy flag in libq */
	IIlbqcb->ii_lq_flags |= II_L_COPY;
    }

    /*
    ** Everything is set.  Send an ACK block to the DBMS.
    */
    gca_ack.gca_ak_data = 0;
    IIcgcp1_write_ack(cgc, &gca_ack);
    /*
    ** THE COPY LOOP:
    **	    Process each copy row.
    */
    if (copy_blk.cp_direction == CPY_INTO)
    {
	/*
	** Copy INTO (from DBMS to FILE).
	** Read tuples from a DBMS table and write to the copy file.
	*/
	for (;;)
	{
	    if ( copy_blk.cp_has_blobs )
	    {
		/* Get partial table tuple (may be a blob segment) from DBMS */
		if ((status = IICPbrBlobRead( IIlbqcb, &copy_blk, &cpy_bufh,
					      user_routine )) != OK)
		{
		    /*
		    ** If return is ENDFILE, then we have received all the
		    ** tuples - set status to OK as this is not an error
		    ** condition.
		    */
		    if (status == ENDFILE)
			status = OK;
		    else
                    {
			copy_blk.cp_error = TRUE;
                    }
		}
		break; /* handle whole table in IICPbrBlobRead() */
	    }
	    else
	    {
		/* Get a table tuple from DBMS */
		status = IIcpdbread( IIlbqcb, &copy_blk, &cpy_bufh, &tuple);
		if ( status != OK )
		{
		    /*
		    ** If return is ENDFILE, then we have received all the
		    ** tuples - set status to OK as this is not an error
		    ** condition.
		    */
		    if (status == ENDFILE)
			status = OK;
		    else
			copy_blk.cp_error = TRUE;
		    break; 
		}
	    }
 
	    /* Initialize pointer to file row buffer */
	    if ((status = IIcpinto_init(&copy_blk, &cpy_bufh, 
						&row, tuple)) != OK)
	    {
		copy_blk.cp_error = TRUE;
		break; 
	    }
	    /*
	    ** Convert tuple to row format and write to the copy file.
	    ** In a bulk copy, no conversion is necessary, since the
	    ** row and tuple formats are identical.
	    */
	    status = OK;
	    if (copy_blk.cp_format)
	    {
		status = IIcprowformat( IIlbqcb, &copy_blk, tuple, row);
		if ( status != OK)
		{
		    /* 
		    ** If row logging, then write the tuple to the loc file.
		    */ 
		    if (copy_blk.cp_rowlog)
			IIcplgrow(&copy_blk, tuple);
 
		    /* 
		    ** If we have exceeded the maximum errors allowed, then
		    ** terminate the copy.
		    */ 
		    copy_blk.cp_badrows++;
		    if ((!copy_blk.cp_continue) &&
			(copy_blk.cp_badrows >= copy_blk.cp_maxerrs))
		    {
			/*
			** Increment the rowcount, as if this row has been
			** processed. Bug 8725
			*/
			++copy_blk.cp_rowcount;
			copy_blk.cp_error = TRUE;
			break;
		    }
		}
	    }
	    /*
	    ** Only write out the row if the above conversion was successful.
	    */
	    if (status == OK)
	    {
		if ((status = IIcpputrow(&copy_blk, row, user_routine)) != OK)
		{
		    copy_blk.cp_error = TRUE;
		    break;
		}
	    }
	    ++copy_blk.cp_rowcount;	    /* update row count */
	}				    /* end COPY INTO loop */
    }
    else 
    {
	/*
	** Copy FROM (from FILE to DBMS).
	** Read rows from the copy file and append them to a DBMS table.
	*/
	for (;;)
	{
	    if ( copy_blk.cp_has_blobs )
	    {
		status = IICPbwBlobWrite( thr_cb, &copy_blk, &cpy_bufh,
					  user_routine );
		if ( status != OK )
		{
		    /*
		    ** If return is ENDFILE, then we have read all the file
		    ** rows - set status to OK as this is not an error
		    ** condition.
		    */
		    if (status == ENDFILE)
			status = OK;
		    else
			copy_blk.cp_error = TRUE;
		}
		break; /* handle whole table in IICPbwBlobWrite() */
	    }
	    if ( (status = IIcpfrom_init( IIlbqcb, &copy_blk, 
					  &cpy_bufh, &row, &tuple )) != OK )
	    {
		copy_blk.cp_error = TRUE;
		break; 
	    }
	    /* Save truncated count in case row is not copied to table */
	    save_truncated = copy_blk.cp_truncated;
	    if ((status = IIcpgetrow(&copy_blk, &row, user_routine)) != OK)
	    {
		/*
		** If return is ENDFILE, then we have read all the file
		** rows - set status to OK as this is not an error condition.
		*/
		if (status == ENDFILE)
		    status = OK;
		else
		    copy_blk.cp_error = TRUE;
		break; 
	    }
	    /*
	    ** Convert row to tuple format and send to the DBMS.
	    ** During a bulk copy, no conversion is necessary as the tuple and
	    ** row formats are identical.  If no conversion is needed, and we
	    ** don't need to check the data values in the copy rows, then don't
	    ** bother calling the formatter.
	    */
	    if (copy_blk.cp_format)
	    {
		status = IIcptupformat( thr_cb, &copy_blk, row, tuple );
		if ( status != OK )
		{
		    /* 
		    ** If row logging, then write the tuple to the loc file.
		    */ 
		    if (copy_blk.cp_rowlog)
			IIcplgrow(&copy_blk, row);

		    /* Don't include this row's count of domains truncated */
		    copy_blk.cp_truncated = save_truncated;
		    /* 
		    ** If we have exceeded the maximum errors allowed, then
		    ** terminate the copy.
		    */ 
		    ++copy_blk.cp_badrows;
		    if ((!copy_blk.cp_continue) &&
			(copy_blk.cp_badrows >= copy_blk.cp_maxerrs))
		    {
			/*
			** Increment the rowcount, as if this row has been
			** processed.
			*/
			++copy_blk.cp_rowcount;
			copy_blk.cp_error = TRUE;
			break;
		    }
		}
	    }
	    /*
	    ** Only send the tuple if the above conversion was successful.
	    */
	    if (status == OK)
	    {
		if ((status = IIcpdbwrite(IIlbqcb, &copy_blk, &cpy_bufh)) != OK)
		{
		    copy_blk.cp_error = TRUE;
		    break;
		}
	    }
	    ++copy_blk.cp_rowcount;	    /* update row count */
	    /*
	    ** Poll error from DBMS every 32 rows (an arbitary number) sent.
	    */
	    if ((copy_blk.cp_rowcount % (i4)32) == 0)
	    {
		if (IIcgcp4_write_read_poll(cgc, GCA_CDATA) 
					    != GCA_CONTINUE_MASK)
		{
		    copy_blk.cp_dbmserr = TRUE;
		    break;
		}
	    }
	}				    /* end COPY FROM loop */
    }

    if ( copy_blk.cp_has_blobs )
    {
	/* Turn off copy flag in libq */
	IIlbqcb->ii_lq_flags &= ~II_L_COPY;
	/* Bug 72235 - Turn off II_LO_END flag in libq */
	if (copy_blk.cp_convert)
	IIlbqcb->ii_lq_lodata.ii_lo_flags &= ~II_LO_END;
    }
    /*
    ** Print out any ending warnings, send status to the DBMS if copy ended
    ** due to an error.  Clean up memory, close files ...
    */
    IIcpendcopy( IIlbqcb, &copy_blk, &cpy_bufh );
    EXdelete();
    return (copy_blk.cp_error ? FAIL : OK);
}

/*{
+*  Name: IIcpinit - Initialize for COPY processing.
**
**  Description:
**	This routine initializes the copy structure.  This includes:
**	    - Read in the GCA_C_INTO/GCA_C_FROM information from the DBMS.
**	      This information includes:
**		- direction of COPY
**		- tuple format of copy table
**		- row format of copy file
**		- copy file name (if there is one)
**		- whether this is a PROGRAM copy
**		- action to take on errors (CONTINUE, TERMINATE)
**		- maximum error count
**		- error row log name (if there is one)
**	    - Figure out the row length, or if it is variable length
**	    - Figure out the offsets to each attribute in a tuple
**	    - Figure out the copy file type
**	    - Fill in the ADF function block for each copy domain with
**	      the conversion information, source and dest data types.
**	    - Figure out whether data value checking is necessary.
**	    - Figure out whether conversion is necessary.
**	    - Build an "empty" tuple that can be used to build tuples during
**		COPY FROM
**	    - Initialize the ADF count of nonprintable chars converted to blank
**		so we can count just the ones that occured in COPY.
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**	msg_type	- GCA message type
**
**  Outputs:
**	copy_blk	- copy control block is filled in
**	cpy_bufh
**	    cpy_dbv.db_data	- point to the beginning of tuple buffer.
**	    cpy_dbv.db_length	- available space in the tuple buffer.
**	    cpy_bytesused	= 0
**
**	Returns: OK, FAIL
-*
**  Side Effects:
**	The libq control block contains an ADF_CB that is used throughout libq.
**	This ADF control block keeps track of the number of noprintable chars
**	that ADF encountered and just converted to blank.  This field is set
**	to zero so that we can count just those that occur during copy.
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	25-aug-87 (puree)
**	    fix problem with variable length fields.
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
**	10-sep-87 (puree)
**	    convert to GCF architecture.
**	24-feb-88 (puree)
**	    Null terminated column names for better error reporting format.
**	09-mar-88 (puree)
**	    cannot use STtrmwhite to terminate the column name.  It strips
**	    off delimiter names (such as nl = d1).  Must prefill the
**	    field with 0.
**      10-nov-88 (barbara)
**          fix bug #3314 regarding rowlen for dummy domain on COPY FROM
**	21-jun-89 (barbara)
**	    fixed bug #6380 where cp_name is an empty string for null
**	    dummy columns.
**	22-sep-89 (barbara)
**	    added support for decimal datatype.
**	07-feb-90 (fredp)
**	    On BYTE_ALIGN machines, adc_getempty() expects aligned
**	    DB_DATA_VALUEs.  So, if needed, we use aligned temp. space 
**	    then subsequently copy the "empty" value into 
**	    copy_blk->cp_zerotup.
**	07-feb-90 (fredp)
**	    On BYTE_ALIGN machines, adc_getempty() expects aligned
**	    DB_DATA_VALUEs.  So, if needed, we use aligned temp. space 
**	    then subsequently copy the "empty" value into 
**	    copy_blk->cp_zerotup.
**	22-may-90 (fredb)
**	    Force CPY_NOBIN status for MPE/XL "copy Into" requests. MPE/XL's
**	    file system rounds odd record lengths up to an even length for
**	    BIN type files. Using a VAR type file gets around this problem.
**	    It is still not perfect, but it works ...
**	11-oct-90 (teresal)
**	    Put in Ken Watts fix so that copy will detect a newline dummy
**	    field (e.g., nl=d1) at the end of a copy format and create a
**	    TXT type file.
**	18-oct-90 (teresal)
**	    Put in fix for Unknown Exception error which occurred because row
**	    buffer was not big enough to handle a variable length copy format
**	    with "WITH NULL(value)" where value was longer than the column's
**	    length.  Row buffer size is now increased by the null length
**	    to make room for the large null string.
**	26-dec-90 (teresal)
**	    Bug fix 6936 - give an error if text filetype is used with a
**	    bulk copy.
**	04-jan-91 (teresal)
**	    ICL fix - only look for filetype if VMS.  Filetype is not
**	    applicable otherwise.
**	10-jan-91 (teresal)
**	    Bug fix 35154: Fixed to calculate row length correctly.
**	    For "varchar(0)" copy format, need to add the ascii length
**	    specifier (5 bytes) to row length.
**	22-apr-1991 (teresal)
**	    Put back declarations for "cp" and "cp2" - these
**	    were inadvertently deleted and are needed for VMS.
**	06-may-1991 (teresal)
**	    Bug fix 37341.
**	06-aug-1991 (teresal) 
**	    Accept new copy map format for the 'with null' value.
**	    Note the new copy map also sends precision as a 
**	    separate value.
**	23-sep-1993 (mgw) Bug #55199
**	    Don't build an "empty" tuple for use when converting from
**	    row format if there are no non-long columns
**	    (copy_blk->cp_tup_length == 0)
**      11-nov-1993 (smc)
**	    Bug #58882
**          Removed ALIGN_RESTRICT & NOT_ALIGNED both of which are more
**          elegently defined in mecl.h. NOT_ALIGNED is replaced with 
**          ME_NOT_ALIGNED_MACRO which has the advantage of not
**          generating wheelbarrows full of lint truncation warnings
**          on boxes where size of ptr > int.
**      17-feb-1993 (johnst)
**	    Bug #58882
**          Corrected file merge problem in previous change, where the
**          "#define NOT_ALIGNED..." line was not deleted in the submission.
**          Delete the line now.
**	07-nov-1995 (kch)
**	    Change blob check from (IIDT_LONGTYPE_MACRO && IIDT_UDT_MACRO)
**	    to (IIDT_LONGTYPE_MACRO || IIDT_UDT_MACRO). The AND case could
**	    never be true; with the two macros ORed, the value
**	    copy_map->cp_nuldata will be correctly set, and an ascii
**	    'copy .. into' will write the correct null blob values to the
**	    data file. This change fixes bug 72054.
**	21-nov-1995 (kch)
**	    Do not set copy_blk->cp_varrows to 1 if the column is dummy.
**	    This change fixes bug 72678.
**	13-dec-1995 (kch)
**	    Add counter for long types, long_count, and increment this
**	    counter if a long type column is encountered. Then use
**	    long_count to commpensate for the fact that the offset for
**	    a long type column is (-1) when filling the "empty" tuple,
**	    copy_blk->cp_zerotup. This change fixes bug 73123.
**	07-feb-1996 (kch)
**	    The long type counter, long_count, is now also incremented for
**	    non byte aligned platforms.
**	26-mar-1996 (kch)
**	    Back out previous fix for bug 72678 (this change was causing
**	    75437). Bug 72678 has now been fixed elsewhere as part of 'fix'
**	    for 75437.
**	05-apr-1996 (loera01)
** 	    Modified to support compressed variable-length datatypes.
**      13-aug-1996 (loera01)
**          Modified allocation of cp_rowbuf and cp_tupbuf so that the
**          buffer lengths consist of the row or tuple length plus
**          MAX_SEG_FILE.  With variable page size, non-blob columns plus 
**          a blob may be sent in the same message from the back end.
**          Fixes bug 77129.
**	20-nov-96 (kch)
**	    We now do not add the default delimiter ('\0') for a copy into
**	    of non varchar formats that have no delimiter specified. This
**	    change fixes bug 79476 (side effect of the fixes for bugs 71149
**	    and 73304).
**      17-Sep-97 (fanra01)
**          Ensure that the compression bit flag is only set if performing a
**          COPY INTO operation.
**          Switch off varchar compression when performing COPY FROM as this
**          is not supported and can cause net to misinterpret the tuple
**          contents.
**      23-feb-98 (thaju02)
**          Bug 89147 - changed STlcopy() to MEcopy().In copying the cp_nuldata
**          STlcopy appended null terminator which overwrote another memory
**          buffers header, resulting in either E_CO0037 or exception in
**          MEdoAlloc() or MEfree().
**      04-Aug-1998 (wanya01)
**          X-integrate change #436526. Bug 91904.      
**          No 'continue' when incrementing long_count on non
**          byte-aligned platforms. Led to exception in MEdoAlloc() when
**          coping in ascii tables with long byte/varchar columns.
**	21-Dec-2001 (gordy)
**	    Added function to generate GCA tuple descriptor.
**	03-Dec-2003 (gupsh01)
**	    Modified copy code so that nchar and nvarchar types will now use 
**	    UTF8 to read and write from the text file for a formatted copy.
**	17-Jan-2006 (thaju02)
**	    Test copy_map->cp_nultype for blob datatype. (B115747)
**	22-May-2006 (thaju02)
**	    For long nvarchar, coerce null value to UTF8. (B116124)
**	03-Aug-2006 (gupsh01)
**	    Added support for ANSI date/time types.
*/

i4
IIcpinit( II_LBQ_CB *IIlbqcb, 
	  II_CP_STRUCT *copy_blk, II_CPY_BUFH *cpy_bufh, i4  msg_type )
{
    IICGC_DESC		*cgc = IIlbqcb->ii_lq_gca;
    DB_DATA_VALUE	*col_desc;
    II_CP_MAP		*copy_map;
    II_CP_MAP		*last_map;		/* for error handling */
    ROW_DESC		*row_desc;		/* Row descriptor */
    DB_DATA_VALUE	dbv, dbv2, dbv4;
    i4		*attr_offset;
    char                *cp, *cp2;
    i4			i2_val;
    i4 		i4_val;
    i4 		lastattrmap;	/* chk row order with formatted blobs */
    i4			fname_length, lname_length;
    i4			i;
    i4			num_read;
    i4			cp_namelen;
    i4			long_count = 0;
    bool		same_desc;
    bool		value_check;
    bool		all_chr_type;
    bool		chr_row_type;
    PTR			nulladpbuf;
    i2			tmpnullen;

#ifdef	BYTE_ALIGN
    ALIGN_RESTRICT	align_buf[((DB_GW3_MAXTUP - 1) / sizeof(ALIGN_RESTRICT)) +1];
 
#endif	/* BYTE_ALIGN */
 
    copy_blk->cp_has_blobs = FALSE;
    copy_blk->cp_rowcount = 0;
 
    /*
    ** Fill in the Copy control block from the CGA_C_INTO/CGA_C_FROM
    ** block sent from the DBMS.
    */
 
    II_DB_SCAL_MACRO(dbv2, DB_INT_TYPE, sizeof(i4), &i2_val);
    II_DB_SCAL_MACRO(dbv4, DB_INT_TYPE, sizeof(i4), &i4_val);
 
    /* gca_status_cp */
    if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv4))
		!= sizeof(i4))
	return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
 
    copy_blk->cp_status = (i4)i4_val;
 
    /* gca_direction_cp */
    if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv4)) 
		!= sizeof(i4))
	return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
 
    copy_blk->cp_direction = (i4)i4_val;

# ifdef hp9_mpe         /* MPE/XL specific kludge */
    /*
    ** MPE/XL's copy files should not be fixed binary due to the file
    ** system rounding odd numbered recordlengths up to an even number.
    */
    if (copy_blk->cp_direction == CPY_INTO)
	copy_blk->cp_status |= CPY_NOTBIN;
# endif
 
    /* gca_maxerrs_cp */ 
    if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv4)) 
		!= sizeof(i4))
	return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
    copy_blk->cp_maxerrs = (i4)i4_val;
 
    /* gca_l_fname_cp */
    if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
		!= sizeof(i4))
	return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
    fname_length = (i4)i2_val;
    if (i2_val < 0 || i2_val > MAX_LOC)
	return (E_CO0022_COPY_INIT_ERR);
 
    /*
    ** If there is a filename, then allocate memory for it and read it in.
    ** The value of "fname_length" must be preserved as it is used below.
    */
    if (fname_length > 0)
    {
	if (loc_alloc((i4)1, (i4)fname_length, 
		(PTR *) &copy_blk->cp_filename) != OK)
	    return (E_CO0037_MEM_INIT_ERR);
 
	II_DB_SCAL_MACRO(dbv, DB_CHR_TYPE, fname_length, copy_blk->cp_filename);
 
	/* gca_fname_cp[] */
	if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv)) 
		    != fname_length)
	    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
    }
    else 
    {
	copy_blk->cp_filename = NULL;
    }
 
    /*
    ** Read in the logfile length and name (if there is one).
    */
    /* gca_l_logname_cp */
    if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
		!= sizeof(i4))
	return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
    lname_length = (i4)i2_val;
    if (i2_val < 0 || i2_val > MAX_LOC)
	return (E_CO0022_COPY_INIT_ERR);
 
    if (lname_length > 0)
    {
	if (loc_alloc((i4)1, (i4)lname_length, 
	    (PTR *)&copy_blk->cp_logname) != OK)
	{
	    return (E_CO0037_MEM_INIT_ERR);
	}
	II_DB_SCAL_MACRO(dbv, DB_CHR_TYPE, lname_length, copy_blk->cp_logname);
	/* gca_logname_cp[] */
	if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv)) 
		!= lname_length)
	    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
    }
    else 
    {
	copy_blk->cp_logname = NULL;
    }
 
    /* tuple descriptor - gca_tup_desc_cp */
    row_desc = &IIlbqcb->ii_lq_retdesc.ii_rd_desc;
    if (IIrdDescribe(msg_type, row_desc) != OK)
	return (E_CO0022_COPY_INIT_ERR);
 
    /*
    ** Set up descriptor for INGRES/NET - allows presentation layer
    ** to process the tuple data.
    */
    if ( copy_blk->cp_direction == CPY_FROM )
    {
	if ( (cgc->cgc_tdescr = IIcptdescr( row_desc )) == NULL )
	    return( E_CO0022_COPY_INIT_ERR );
    }
    else  if (row_desc->rd_flags & RD_CVCH)
	row_desc->rd_gca->rd_gc_res_mod |= GCA_COMP_MASK;
	
    copy_blk->cp_tup_length = (i4)row_desc->rd_gca->rd_gc_tsize;
 
    /*
    ** Get the number of row domains and allocate an II_CP_MAP struct for
    ** each one.  These structs will be filled in with the rest of the
    ** information in the copy block.
    */
    /* Number of file row domains - gca_l_row_desc_cp */
    if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv4)) 
		!= sizeof(i4))
	return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
    copy_blk->cp_num_domains = (i4)i4_val;
    if (i4_val < 0 || i4_val > RD_MAX_COLS)
	return (E_CO0022_COPY_INIT_ERR);
 
    if (loc_alloc((i4)1, 
	(i4)(copy_blk->cp_num_domains * sizeof(II_CP_MAP)),
	(PTR *) &copy_blk->cp_copy_map) != OK)
    {
	return (E_CO0037_MEM_INIT_ERR);
    }
 
    /*
    **  Read in the copy map information (II_CP_MAP) for each file row domain
    **  from gca_row_desc.
    */
    for (i = 0; i < copy_blk->cp_num_domains; i++)
    {
	copy_map = &copy_blk->cp_copy_map[i];
	MEfill((u_i2) sizeof(II_CP_MAP), (unsigned char)'\0', (PTR)copy_map);
 
	/* domain name - gca_domname_cp[] */
	II_DB_SCAL_MACRO(dbv, DB_CHR_TYPE, GCA_MAXNAME, copy_map->cp_name);
	if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv)) 
		    != GCA_MAXNAME)
	    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
 
	/* row type - gca_type_cp */
	if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
		    != sizeof(i4))
	    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
	copy_map->cp_rowtype = (DB_DT_ID)i2_val;
 
	/* Only read gca_precision_cp if we have a new copy map */
	if (cgc->cgc_version >= GCA_PROTOCOL_LEVEL_50)
	{
	    /* row precision - gca_precision_cp  */
	    if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
			!= sizeof(i4))
	    	return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
	    copy_map->cp_rowprec = (i4)i2_val;
	}
	else
	    copy_map->cp_rowprec = 0;	
		
	/* row length - gca_length_cp */ 
	if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv4)) 
		    != sizeof(i4))
	    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
	copy_map->cp_rowlen = (i4)i4_val;

	/* user delimiter flag - gca_user_delim_cp */
	if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
		    != sizeof(i4))
	    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
	copy_map->cp_isdelim = (i4)i2_val;
 
	/* number of delimiter characters - gca_l_delim_cp */ 
	if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
		    != sizeof(i4))
	    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
	copy_map->cp_l_delim = (i4)i2_val;
 
	/* delimiter characters - gca_delim_cp[] */
	II_DB_SCAL_MACRO(dbv, DB_CHR_TYPE, CPY_MAX_DELIM, copy_map->cp_delim);
	if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv)) 
		    != CPY_MAX_DELIM)
	    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
 
	/* attribute map - gca_tupmap_cp */
	if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv4)) 
		    != sizeof(i4))
	    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
	copy_map->cp_attrmap = (i4)i4_val;
 
 
	/* ADF function instance id - gca_cvid_cp */
	if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
		    != sizeof(i4))
	    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
	copy_map->cp_cvid = (ADI_FI_ID)i2_val;
 
	/* Only read gca_cvprec_cp if we have a new copy map */
	if (cgc->cgc_version >= GCA_PROTOCOL_LEVEL_50)
	{
	    /* precision of conversion result - gca_cvprec.cp  */
	    if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
			!= sizeof(i4))
	    	return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
	    copy_map->cp_cvprec = (i4)i2_val;
	}
	else	
	    copy_map->cp_cvprec = 0; 

	/* length of conversion result - gca_cvlen_cp */
	if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
		    != sizeof(i4))
	    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
	copy_map->cp_cvlen = i2_val;

	/* withnull option flag - gca_withnull_cp */    
	if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
		    != sizeof(i4))
	    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
	copy_map->cp_withnull = (bool)i2_val;

	/*
	** Check GCA protocol level to determine if we have a new
	** version of the copy map.  Old version of the copy map
	** has gca_nullen_cp, new version has the null value information
	** for type, length, prec and data in a GCA_DATA_VALUE format.  
	** The new copy map is required to fix bug 37564.
	*/
	if (cgc->cgc_version >= GCA_PROTOCOL_LEVEL_50)
	{
	    /* New version of copy map - gca_nullvalue_cp */		

	    /* with null value sent? - gca_l_nullvalue_cp */    
	    if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
	    		!= sizeof(i4))
	    	return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);

	    /* gca_nullvalue_cp is only sent if gca_l_nullvalue_cp is TRUE */
	    if (i2_val)
	    {
		/* null data type */
		if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
			!= sizeof(i4))
		    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
		copy_map->cp_nultype = (DB_DT_ID)i2_val;

		/* null precision */
		if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
			!= sizeof(i4))
		    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
		copy_map->cp_nulprec = (i2)i2_val;

		/* null length */
		if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv4)) 
			!= sizeof(i4))
		    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
		copy_map->cp_nullen = (i4)i4_val;

	    }
	    else
	    {
	        copy_map->cp_nultype = 0;
	        copy_map->cp_nullen = 0;
	        copy_map->cp_nulprec = 0;
	    }
	}
	else
	{
	    /* Old version of copy map */	

	    /* length of value for null - gca_nullen_cp */
	    if ((num_read = IIcgc_get_value(cgc, msg_type, IICGC_VVAL, &dbv2)) 
			!= sizeof(i4))
	    	return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
	    copy_map->cp_nullen = (i4)i2_val;
	    copy_map->cp_nultype = copy_map->cp_rowtype;
	    copy_map->cp_nulprec = 0;
	}

	/* Get value for null */
	if (copy_map->cp_nullen)
	{
	    dbv.db_prec = copy_map->cp_nulprec;

	    /* Check to see if this is a BLOB column */
	    col_desc = &row_desc->RD_DBVS_MACRO(copy_map->cp_attrmap);

	    if (IIDT_LONGTYPE_MACRO(copy_map->cp_nultype) ||
		IIDT_UDT_MACRO(copy_map->cp_nultype))  /* BLOB column */
	    {
		/*
		** Read the ADP_PERIPHERAL and get the cp_nullen and
		** cp_nuldata from there.
		*/
		if (loc_alloc((i4)1, (i4)copy_map->cp_nullen, 
			&nulladpbuf) != OK)
		    return (E_CO0037_MEM_INIT_ERR);

		II_DB_SCAL_MACRO(dbv, copy_map->cp_nultype, copy_map->cp_nullen,
		    nulladpbuf);

		if ((num_read = IIcgc_get_value(cgc,msg_type,IICGC_VVAL,&dbv))
			!= copy_map->cp_nullen)
		    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);

		MEcopy((PTR) &nulladpbuf[ADP_HDR_SIZE + sizeof(i4)],
		       sizeof(i2), (PTR) &tmpnullen);
		copy_map->cp_nullen = (i4) tmpnullen;
 
		if (loc_alloc((i4)1, (i4)copy_map->cp_nullen, 
			&copy_map->cp_nuldata) != OK)
		    return (E_CO0037_MEM_INIT_ERR);

		if (abs(col_desc->db_datatype) == DB_LNVCHR_TYPE)
		{
		    DB_DATA_VALUE	ucs2_dv, rdv;
		    char		utf8_buf[DB_MAXTUP];
		    ucs2_dv.db_datatype = DB_NCHR_TYPE;
		    ucs2_dv.db_length = copy_map->cp_nullen * sizeof(UCS2);
		    ucs2_dv.db_data = &nulladpbuf[ADP_HDR_SIZE + sizeof(i4) + sizeof(i2)];
		    rdv.db_datatype = DB_UTF8_TYPE;
		    rdv.db_length = copy_map->cp_nullen + sizeof(i2);
		    rdv.db_data = utf8_buf;
		    if (adu_nvchr_toutf8(IIlbqcb->ii_lq_adf, &ucs2_dv, &rdv) != E_DB_OK)
			return(E_CO0022_COPY_INIT_ERR);
		    MEcopy((char *)&utf8_buf[sizeof(i2)], copy_map->cp_nullen,
			copy_map->cp_nuldata);
		}
		else
		{
		    MEcopy((char *)
		        &nulladpbuf[ADP_HDR_SIZE + sizeof(i4) + sizeof(i2)],
		        copy_map->cp_nullen, copy_map->cp_nuldata);
		}
		MEfree(nulladpbuf);
	    }
	    else             /* NOT a BLOB column, just read the cp_nuldata */
	    {
		if (loc_alloc((i4)1, (i4)copy_map->cp_nullen, 
			&copy_map->cp_nuldata) != OK)
		    return (E_CO0037_MEM_INIT_ERR);
 
		/* value for null */
		II_DB_SCAL_MACRO(dbv, copy_map->cp_nultype, copy_map->cp_nullen,
		    copy_map->cp_nuldata);
 
		if ((num_read = IIcgc_get_value(cgc,msg_type,IICGC_VVAL,&dbv))
			!= copy_map->cp_nullen)
		    return ((num_read < 0) ? FAIL : E_CO0022_COPY_INIT_ERR);
	    }
	}
	else
	{
	    copy_map->cp_nuldata = (PTR)NULL;
	}
	
    }
 
 
    /*
    ** Figure out offsets to each tuple attribute.  We will need this when
    ** we fill in copy map for each file row domain.
    */
    if (loc_alloc((i4)1, (i4)(row_desc->rd_numcols * sizeof(i4)),
	    &copy_blk->cp_buffer) != OK)
    {
	return (E_CO0037_MEM_INIT_ERR);
    }
 
    attr_offset = (i4 *)copy_blk->cp_buffer;
    attr_offset[0] = 0;

    /* Log whether there are any blob columns in the table */
    col_desc = &row_desc->RD_DBVS_MACRO(0);
    /* Bug 65565
    ** If a user defined type is passed into iicopy,
    ** then it must be a long type.
    ** Change it to be a long byte
    */
    if (IIDT_UDT_MACRO(col_desc->db_datatype))
    {
	if (col_desc->db_datatype > 0)
	    col_desc->db_datatype = DB_LBYTE_TYPE;
   	else
	    col_desc->db_datatype = -DB_LBYTE_TYPE;
    }

    if ( IIDT_LONGTYPE_MACRO( col_desc->db_datatype ) )
	copy_blk->cp_has_blobs = TRUE;

    for (i = 1; i < row_desc->rd_numcols; i++)
    {
	attr_offset[i] =
		attr_offset[i-1] + row_desc->RD_DBVS_MACRO(i-1).db_length;

	/* Log whether there are any blob columns in the table */
	col_desc = &row_desc->RD_DBVS_MACRO(i);
        /* Bug 65565
        ** If a user defined type is passed into iicopy,
        ** then it must be a long type.
        ** Change it to be a long byte
        */
        if (IIDT_UDT_MACRO(col_desc->db_datatype))
        {
	    if (col_desc->db_datatype > 0)
		col_desc->db_datatype = DB_LBYTE_TYPE;
            else
		col_desc->db_datatype = -DB_LBYTE_TYPE;
    	}
	if ( IIDT_LONGTYPE_MACRO( col_desc->db_datatype ) )
	    copy_blk->cp_has_blobs = TRUE;
    }
 
    /*
    ** Initilize the rest of the copy map structs:
    **   - fill in the ADF function blocks for the row/tuple conversions.
    **   - fill in the tuple information from the tuple descriptor read earlier.
    **   - look for TEXT, DUMMY, and variable length fields for special 
    **	    processing.
    **   - count up the row length.
    */
    same_desc = TRUE;
    value_check = FALSE;
    all_chr_type = TRUE;
    copy_blk->cp_row_length = 0;
    copy_blk->cp_varrows = 0;
    for (i = 0; i < copy_blk->cp_num_domains; i++)
    {
	copy_map = &copy_blk->cp_copy_map[i];
 
	/*
	** Find the attribute descriptor and fill in the tuple information in
	** the copy map.  If this is a dummy domain, then there is no attibute
	** descriptor.
	*/
	if (copy_map->cp_rowtype != CPY_DUMMY_TYPE)
	{
	    /* Find the column descriptor corresponding to this domain.  */
	    col_desc = &row_desc->RD_DBVS_MACRO(copy_map->cp_attrmap);
 
	    copy_map->cp_tuptype = col_desc->db_datatype;
	    copy_map->cp_tuplen = col_desc->db_length;
	    copy_map->cp_tupprec = col_desc->db_prec;
	    copy_map->cp_attroffset = attr_offset[copy_map->cp_attrmap];
	}
	else
	{
	    copy_map->cp_tuptype = 0;
	    copy_map->cp_cvid = ADI_NOFI;
	}
 
	if (copy_map->cp_rowlen == ADE_LEN_UNKNOWN)
	{
	    if (copy_blk->cp_direction == CPY_FROM && 
		(!copy_map->cp_isdelim) &&
		(copy_map->cp_rowtype != DB_VCH_TYPE) &&
		(copy_map->cp_rowtype != DB_UTF8_TYPE) &&
		(copy_map->cp_rowtype != DB_VBYTE_TYPE) &&
		(!IIDT_LONGTYPE_MACRO(copy_map->cp_rowtype)))
	    {
		/*
		** If copy FROM with type other than VARCHAR, and no
		** delimiter was specified, then mark this to use default delim.
		** (VARCHAR(0) type does not need a delimiter).
		** Zero out the delimiter storage since we will check the value
		** stored there to see if it is double-byte (default delims are
		** single-byte.
		*/
                copy_map->cp_isdelim = -1;
                copy_map->cp_delim[0] = '\0';
	    }
	    copy_blk->cp_status |= CPY_NOTBIN;
	}
	/*
	** Fill in ADF function block for conversion calls.
	** Zero out unused second argument's datatype value so that ADF won't
	** think that it is a nullable type.
	*/
	copy_map->cp_adf_fnblk.adf_fi_id = copy_map->cp_cvid;
	copy_map->cp_adf_fnblk.adf_2_dv.db_datatype = 0;
	if (copy_blk->cp_direction == CPY_FROM)
	{
	    copy_map->cp_adf_fnblk.adf_1_dv.db_datatype = copy_map->cp_rowtype;
	    copy_map->cp_adf_fnblk.adf_1_dv.db_length = copy_map->cp_rowlen;
	    copy_map->cp_adf_fnblk.adf_1_dv.db_prec = copy_map->cp_rowprec;
	    copy_map->cp_adf_fnblk.adf_r_dv.db_datatype = copy_map->cp_tuptype;
	    copy_map->cp_adf_fnblk.adf_r_dv.db_length = copy_map->cp_tuplen;
	    copy_map->cp_adf_fnblk.adf_r_dv.db_prec = copy_map->cp_tupprec;
	}
	else
	{
	    copy_map->cp_adf_fnblk.adf_1_dv.db_datatype = copy_map->cp_tuptype;
	    copy_map->cp_adf_fnblk.adf_1_dv.db_length = copy_map->cp_tuplen;
	    copy_map->cp_adf_fnblk.adf_1_dv.db_prec = copy_map->cp_tupprec;
	    copy_map->cp_adf_fnblk.adf_r_dv.db_datatype = copy_map->cp_rowtype;
	    copy_map->cp_adf_fnblk.adf_r_dv.db_length = copy_map->cp_cvlen;
	    copy_map->cp_adf_fnblk.adf_r_dv.db_prec = copy_map->cp_cvprec;
	}
	/*
	** If the row type is varchar, then we will write length specifier in
	** ascii. Adjust the actual fieldlen (in the copy file) to show the 
	** ascii length value that preceeds the varchar text data.
	**
	** If the row type is text, then the count field will be stripped
	** off.  The field length is smaller than the rowlen by the size
	** of the count field.
	*/
	copy_map->cp_fieldlen = copy_map->cp_rowlen;
	if ((copy_map->cp_rowlen != ADE_LEN_UNKNOWN) &&
	    (!(copy_blk->cp_status & CPY_NOTARG)))
	{
	    if (   copy_map->cp_rowtype == DB_VCH_TYPE
		|| copy_map->cp_rowtype == -DB_VCH_TYPE
		|| copy_map->cp_rowtype == DB_UTF8_TYPE
		|| copy_map->cp_rowtype == -DB_UTF8_TYPE
		|| copy_map->cp_rowtype == DB_VBYTE_TYPE
		|| copy_map->cp_rowtype == -DB_VBYTE_TYPE)
		    copy_map->cp_fieldlen += (CPY_VCHLEN - DB_CNTSIZE);
	    if (   copy_map->cp_rowtype == DB_TXT_TYPE
		|| copy_map->cp_rowtype == -DB_TXT_TYPE)
		    copy_map->cp_fieldlen -= DB_CNTSIZE;
	}
	/*
	** Accumulate the copy row length.
	**
	** Normally, we just need to add the rowlen for this domain to the
	** total row length.  There are three exceptions.
	**
	** If this is a dummy domain for a COPY INTO, then the domain name
	** will be used as the value for the copy domain.  It will be
	** written as many times as the domain length specifies.  So add
	** domain length * name length to the copy row length.
	**
	** If this is a variable length field in a COPY FROM, then we
	** pick the maximum size.  If it's in a COPY INTO then we use ADF
	** conversion length.
	**
	** If this is a variable length field in a COPY INTO and the null
	** value specified by the WITH NULL(value) option is longer than the
	** column length.  Add the null string length to the copy row length.
	*/
	if (copy_map->cp_rowlen != ADE_LEN_UNKNOWN)
	{
 
	    if (copy_map->cp_rowtype == CPY_DUMMY_TYPE)
	    {
		if (copy_blk->cp_direction == CPY_INTO)
		{
		    /* 
		    ** Bug #6380 - cp_name will be an empty string
		    ** for null dummy columns
		    */
		    cp_namelen = STlength(copy_map->cp_name); 
		    if (cp_namelen == 0)
			cp_namelen = 1;
		    copy_blk->cp_row_length +=
			(copy_map->cp_rowlen * cp_namelen);
		}
		else
		    copy_blk->cp_row_length += copy_map->cp_rowlen;
	    }
	    else
		copy_blk->cp_row_length += copy_map->cp_fieldlen;
	}
	else
	{
	    /* Long varchar will come through here too */

	    copy_blk->cp_varrows = 1;
 
	    if (copy_blk->cp_direction == CPY_INTO)
	    {
	    /*
	    ** If row type is varchar, copy outputs an ascii length
	    ** specifier (5 bytes).  However, the length of the 
	    ** conversion result (cp_cvlen) as sent by the server includes 
	    ** the length of a binary length specifier (2 bytes).  Need
	    ** to adjust row length so it will be big enough to hold the
	    ** ascii length specifier. Bug 35154.
	    **
	    ** If the null string length is longer than the conversion
	    ** length, use the null length to calculate row length.
	    ** The length of the null string (cp_nullen) as sent
	    ** by the server includes a 2 byte length specifier, however
	    ** need to make room in the file for an ascii length 
	    ** specifier (5 bytes).  Bug 37341. 
	    */
	        if (   copy_map->cp_rowtype == DB_VCH_TYPE
		    || copy_map->cp_rowtype == -DB_VCH_TYPE
		    || copy_map->cp_rowtype == DB_UTF8_TYPE
		    || copy_map->cp_rowtype == -DB_UTF8_TYPE
		    || copy_map->cp_rowtype == DB_BYTE_TYPE
		    || copy_map->cp_rowtype == -DB_BYTE_TYPE)
		{
		    if (copy_map->cp_nullen > copy_map->cp_cvlen)
			copy_blk->cp_row_length += (copy_map->cp_nullen +
				(CPY_VCHLEN - DB_CNTSIZE));
		    else
			copy_blk->cp_row_length += (copy_map->cp_cvlen +
				(CPY_VCHLEN - DB_CNTSIZE));
		}
		else
		{
		    if (copy_map->cp_nullen > copy_map->cp_cvlen)
			copy_blk->cp_row_length += copy_map->cp_nullen;
		    else
			copy_blk->cp_row_length += copy_map->cp_cvlen;
		}
	    }
	}
	/*
	** If a delimiter is to be written to the copy file, add it to the
	** row length.
	*/
	if (copy_map->cp_isdelim)
	{
	    copy_blk->cp_row_length++;
	    if (CMdbl1st(copy_map->cp_delim))
		copy_blk->cp_row_length++;
	}
	/*
	** If all tuple attributes are specified in their correct order and
	** no conversion is necessary to/from row format, then we can optimize
	** this copy.  Same_desc records whether any attributes were found out
	** of order or needing conversion.
	*/
	if ((copy_map->cp_attrmap != i) ||
	    (copy_map->cp_cvid != ADI_NOFI) ||
	    (copy_map->cp_isdelim) ||
	    (copy_map->cp_rowtype == CPY_DUMMY_TYPE))
	{
		same_desc = FALSE;
	}
 
	/*
	** If any binary datatypes are specified for the copy file, turn off
	** 'all_chr_type' so that we won't use a text file for the copy.
	*/
	if ((copy_map->cp_rowtype != DB_CHR_TYPE) &&
	    (copy_map->cp_rowtype != DB_CHA_TYPE) &&
	    (copy_map->cp_rowtype != DB_VCH_TYPE) &&
	    (copy_map->cp_rowtype != DB_UTF8_TYPE) &&
	    (copy_map->cp_rowtype != DB_VBYTE_TYPE) &&
	    (copy_map->cp_rowtype != DB_TXT_TYPE) &&
	    (copy_map->cp_rowtype != CPY_DUMMY_TYPE))
	{
	    chr_row_type = FALSE;
	    all_chr_type = FALSE;
	}
	else
	{
	    chr_row_type = TRUE;
	}
 
	/*
	** If this is a COPY FROM, then we may have to check the validity
	** of the data being read from the COPY file (to make sure it specifies
	** a legal Ingres data value).
	**
	** We don't have to check if the result type is INT (there is no illegal
	** int value), if we are converting from char to numeric type (as we
	** will be generating the data value internally and we trust that we
	** will do it correctly), or if this is a dummy field. Likewise,
	** there is not illegal large object value so no validity check
	** required in that case either.
	*/
	if ((copy_map->cp_tuptype == DB_INT_TYPE) ||
	    (IIDT_LONGTYPE_MACRO(copy_map->cp_tuptype)) ||
	    (copy_map->cp_rowtype == CPY_DUMMY_TYPE) ||
		(((chr_row_type) ||
		    (copy_map->cp_rowtype == DB_TXT_TYPE) ||
		    (copy_map->cp_rowtype == DB_VBYTE_TYPE) ||
		    (copy_map->cp_rowtype == DB_VCH_TYPE) &&
		    (copy_map->cp_rowtype == DB_UTF8_TYPE)) &&
		((copy_map->cp_tuptype == DB_FLT_TYPE) ||
		    (copy_map->cp_tuptype == DB_MNY_TYPE) ||
		    (copy_map->cp_tuptype == DB_DTE_TYPE)||
		    (copy_map->cp_tuptype == DB_ADTE_TYPE)||
		    (copy_map->cp_tuptype == DB_TMWO_TYPE)||
		    (copy_map->cp_tuptype == DB_TME_TYPE)||
		    (copy_map->cp_tuptype == DB_TMW_TYPE)||
		    (copy_map->cp_tuptype == DB_TSWO_TYPE)||
		    (copy_map->cp_tuptype == DB_TSW_TYPE)||
		    (copy_map->cp_tuptype == DB_TSTMP_TYPE)||
		    (copy_map->cp_tuptype == DB_INYM_TYPE)||
		    (copy_map->cp_tuptype == DB_INDS_TYPE))))
	{
	    copy_map->cp_valchk = 0;
	}
	else
	{
	    copy_map->cp_valchk = 1;
	    value_check = TRUE;
	}
    }

    /* 
    ** if this row has blobs, be sure to allocate a buffer big
    ** enough for the maximum allowable segment size.
    */
 
    /*
    ** Figure out file type.
    */
    if ((copy_blk->cp_status & CPY_PROGRAM) == 0)
    {
#ifdef VMS 
	/*
	** VMS only: Check if filename includes the file type. 
	** Look for comma in filename.  "fname_length" is set above.
	*/
	cp = STindex(copy_blk->cp_filename, ",", fname_length);
	if (cp) 
	{
	    if ((cp2 = STindex(copy_blk->cp_filename, "]", fname_length)) ||
		(cp2 = STindex(copy_blk->cp_filename, ">", fname_length)))
	    {
		cp = STindex(cp, ",", STlength(cp2));
	    }
	}
	if (cp) 
	{
	    /*
	    ** Found a comma, now look for file type following it.  
	    ** Note: file types are only applicable for VMS.
	    */
	    *cp = '\0';
	    CMnext(cp);
	    while (CMspace(cp))
		CMnext(cp);
	    switch (*cp)
	    {
	      case 'B':
	      case 'b':
		copy_blk->cp_filetype = SI_BIN;
		break; 
	      case 'T':
	      case 't':
		copy_blk->cp_filetype = SI_TXT;
		/*
		** TEXT filetype is not allowed with
		** bulk copy.  A TEXT file written out using
		** bulk copy can't be copied back in because
		** the bulk copy won't handle the carriage
		** return after each record - give an error
		** for COPY INTO to catch this
		** situation up front. (Bug 6936) 
		*/
		if ((same_desc) &&
		    (copy_blk->cp_direction == CPY_INTO))
		{
		    return (E_CO0017_NOTEXT);
		}
		break; 
	      case 'V':
	      case 'v':
		copy_blk->cp_filetype = SI_VAR;
		break; 
	      default:
		IIlocerr(GE_SYNTAX_ERROR, E_CO0018_BAD_FILETYPE, II_ERR, 1, cp);
		copy_blk->cp_warnings++;
		return (FAIL);
	    }
 
	    /*
	    ** If we got a SI_BIN type and they are not allowed, then it is an
	    ** error.
	    */
	    if ((copy_blk->cp_filetype == SI_BIN) &&
		(copy_blk->cp_status & CPY_NOTBIN) &&
		(copy_blk->cp_direction == CPY_INTO))
	    {
		return (E_CO0019_NOBINARY);
	    }
	}
	else
#endif /* VMS */
	{
	    /*
	    ** Use default filetypes.
	    **   If all domains are character (c, char, text, vchar) and the
	    **   rows end in newlines, then use TEXT file. 
	    **   15-feb-96 (also if rows end with default delimiters)
	    **   Otherwise use SI_BIN unless this is a variable length file.
	    */
	    i = copy_blk->cp_num_domains - 1;
	    if ((all_chr_type) &&
		(
		 (copy_blk->cp_copy_map[i].cp_isdelim &&
		 (CMcmpcase(copy_blk->cp_copy_map[i].cp_delim, "\n") == 0))
		 ||
		 (copy_blk->cp_copy_map[i].cp_rowtype == CPY_DUMMY_TYPE &&
		 (CMcmpcase(copy_blk->cp_copy_map[i].cp_name, "\n") == 0))
		 ||
		 (copy_blk->cp_copy_map[i].cp_isdelim == -1)
		)
	       )	
	    {
		copy_blk->cp_filetype = SI_TXT;
	    }
	    else if (copy_blk->cp_status & CPY_NOTBIN)
	    {
		copy_blk->cp_filetype = SI_VAR;
	    }
	    else
	    {
		copy_blk->cp_filetype = SI_BIN;
	    }
	}
    }
 
    /*
    ** If a copy row and a tuple are equivalent then we can avoid doing
    ** row/tuple conversions.
    */
    if ((!same_desc) || (copy_blk->cp_num_domains != row_desc->rd_numcols))
	copy_blk->cp_convert = TRUE;
    else 
	copy_blk->cp_convert = FALSE;
 
    /*
    ** If we don't need to convert, and we don't need to check values, then
    ** we don't even need to call the format routine.  Note that we never
    ** need to check values on a COPY INTO.
    */
    if ((copy_blk->cp_convert) ||
	    ((value_check) && (copy_blk->cp_direction == CPY_FROM)))
	copy_blk->cp_format = TRUE;
    else 
	copy_blk->cp_format = FALSE;
 
    /*
    ** cp_continue specifies to not halt copy processing if an error occurs
    ** converting a row.
    */
    if (copy_blk->cp_status & CPY_CONTINUE)
	copy_blk->cp_continue = 1;
    else 
	copy_blk->cp_continue = 0;
 
    /*
    ** cp_rowlog specifies that bad rows or tuples (ones that cannot be
    ** converted to the specified format) should be written to a log file.
    */
    if (copy_blk->cp_logname)
	copy_blk->cp_rowlog = 1;
    else 
	copy_blk->cp_rowlog = 0;
 
    /*
    ** cp_program specifies that there is no copy file, we should use the
    ** user defined routines to read and write copy rows.
    */
    if (copy_blk->cp_status & CPY_PROGRAM)
	copy_blk->cp_program = 1;
    else 
	copy_blk->cp_program = 0;

    /* allocate tuple buffer and check FMT BLOB column order */
    if ( copy_blk->cp_has_blobs )
    {
	/*
	** in the blobs case, make sure the columns are in the proper
	** order for formatted copy. (May want to be a bit more
	** sophisticated about this in the future. E.g. allow reordering
	** of columns that have no intervening BLOBS). And of course it
	** would be nice to lift this restriction altogether when we
	** implement coupons for blobs in the frontends.
	*/
	if (copy_blk->cp_format || copy_blk->cp_convert)
	{
	    for (i = 0; i < copy_blk->cp_num_domains; i++)
	    {
		copy_map = &copy_blk->cp_copy_map[i];
		if (i == 0)
		{
		    lastattrmap = copy_map->cp_attrmap;
		    continue;
		}
		if (copy_map->cp_attrmap > 0 &&
			copy_map->cp_attrmap < lastattrmap)
		{
		    /* print error about blob formatted ordering */
		    last_map = &copy_blk->cp_copy_map[i-1];
		    IIlocerr(GE_LOGICAL_ERROR, E_CO0044_FMT_BLOB_ORDER, II_ERR, 
			     2, last_map->cp_name, copy_map->cp_name);
		    return (FAIL);
		}
		lastattrmap = copy_map->cp_attrmap;
	    }
	}

	/* allocate the tuple buffer */
	if (loc_alloc((i4)1, (i4)copy_blk->cp_tup_length +
           copy_blk->cp_direction == CPY_INTO ? MAX_SEG_INTO_FILE: MAX_SEG_FROM_FILE,
	   &copy_blk->cp_tupbuf) != OK)
	{
	    return (E_CO0037_MEM_INIT_ERR);
	}
    }
    else
    {
	if (loc_alloc((i4)1, (i4)copy_blk->cp_tup_length, 
	    &copy_blk->cp_tupbuf) != OK)
	{
	    return (E_CO0037_MEM_INIT_ERR);
	}
    }
 
    /*
    ** Build an "empty" tuple for use when converting from row format
    ** to tuple format.  Attributes with no corresponding domain in
    ** the copy file will be left with this default value.
    **
    ** 55199 - check cp_tup_length too, if no non-blob columns, don't allocate
    */
    if ((copy_blk->cp_convert) && (copy_blk->cp_direction == CPY_FROM) &&
	(copy_blk->cp_tup_length > 0))
    {
	if (loc_alloc((i4)1, (i4)copy_blk->cp_tup_length, 
		&copy_blk->cp_zerotup))
	    return (E_CO0037_MEM_INIT_ERR);
 
	for (i = 0; i < row_desc->rd_numcols; i++)
	{
#ifdef BYTE_ALIGN
	    PTR		ztup_ptr;
	    DB_DT_ID	ztuptype;
	    i4		align_it = 0;
 
	    ztup_ptr = (PTR) ((char *) copy_blk->cp_zerotup
					+ attr_offset[i] + long_count);
	    ztuptype = row_desc->RD_DBVS_MACRO(i).db_datatype;

	    if (IIDT_LONGTYPE_MACRO(ztuptype))
	    {
		long_count++;
		continue;
	    }

	    /*
	    ** ADF is used to get "empty" attribute values, so if the
	    ** alignment isn't right for this datatype, use aligned temp.
	    ** space. 
	    */
	    if (ME_NOT_ALIGNED_MACRO(ztup_ptr)
		&& (ztuptype !=  DB_CHR_TYPE)
		&& (ztuptype != -DB_CHR_TYPE)
		&& (ztuptype !=  DB_CHA_TYPE)
		&& (ztuptype != -DB_CHA_TYPE))
	    {
		row_desc->RD_DBVS_MACRO(i).db_data = (PTR) align_buf;
		align_it = 1;
	    }
	    else
#endif /* BYTE_ALIGN */
	    {
		row_desc->RD_DBVS_MACRO(i).db_data =
		    (PTR) ((char *) copy_blk->cp_zerotup
					+ attr_offset[i]) + long_count;
	    }

	    /* Increment long_count for the non byte aligned case */
#ifndef BYTE_ALIGN
	    if (IIDT_LONGTYPE_MACRO(row_desc->RD_DBVS_MACRO(i).db_datatype))
	    {
		long_count++;
                continue;
	    }
#endif /* NON BYTE_ALIGN */

	    if (adc_getempty(IIlbqcb->ii_lq_adf,
			     &row_desc->RD_DBVS_MACRO(i)) != E_DB_OK)
		return (E_CO0022_COPY_INIT_ERR);
 
#ifdef BYTE_ALIGN
	    if ( align_it )
	    {
		MEcopy((PTR)align_buf, 
			(u_i2)row_desc->RD_DBVS_MACRO(i).db_length, ztup_ptr);
	    }
#endif /* BYTE_ALIGN */
	}
    }
 
    /*
    ** Free up memory used to find attribute offset mappings
    */
    MEfree(copy_blk->cp_buffer);
    copy_blk->cp_buffer = NULL;
 
    /*
    ** Reset the count of chars converted to blank by ADF.  This is a
    ** Kludgy way of doing this, but we don't want to count only the
    ** warnings generated during COPY.
    */
    IIlbqcb->ii_lq_adf->adf_warncb.ad_noprint_cnt = 0;
 
    /*
    ** Initialize copy buffer header and GCA message buffer.
    */
    copy_blk->cp_not_flushed = FALSE;
    if (copy_blk->cp_direction == CPY_INTO)
    {
	cpy_bufh->cpy_dbv.db_length = (i4)0;
	cpy_bufh->cpy_bytesused = (i4)0;
    }
    else /* COPY FROM, needs a row buffer */
    {
	/* Need to allocate enough for MAX_SEG_FROM_FILE if has_blobs. */
	/* has_blobs test is probably unnecessary since varrows implies */
	/* this. But in the Bulk copy case, do we even need the rowbuf? */
	if ( copy_blk->cp_varrows  ||  copy_blk->cp_has_blobs )
	{
	    copy_blk->cp_rbuf_size = copy_blk->cp_row_length + MAX_SEG_FROM_FILE;
	    if(loc_alloc((i4)1, (i4)(copy_blk->cp_row_length + 
                MAX_SEG_FROM_FILE), &copy_blk->cp_rowbuf))
		return (E_CO0037_MEM_INIT_ERR);
	}
	else
	{
	    copy_blk->cp_rbuf_size = copy_blk->cp_row_length;
	    if(loc_alloc((i4)1, (i4)copy_blk->cp_row_length, 
		    &copy_blk->cp_rowbuf))
		return (E_CO0037_MEM_INIT_ERR);
	}
    }
    return (OK);
}

/*
**  Name: IIcptdescr - Create GCA tuple descriptor
**
**  Description:
**	Allocate and build a GCA tuple descriptor from a row descriptor.
**
**  Inputs:
**	row_desc	- copy row descriptor
**
**  Outputs:
**
**  Returns: 
**	GCA_TD_DATA *	- GCA tuple descriptor
**
**  Side Effects:
**	Allocates memory for tuple descriptor.
**
**  History:
**	21-Dec-01 (gordy)
**	    Created.
*/
 
GCA_TD_DATA *
IIcptdescr( ROW_DESC *row_desc )
{
    GCA_TD_DATA		*tdescr;
    char                *td_ptr;
    i2			tmpi2;
    i4                  tmpi4, i;
    i4                  hdrsize = sizeof(GCA_TD_DATA) - sizeof(GCA_COL_ATT);
    i4                  attsize = sizeof(tdescr->gca_col_desc[0].gca_attdbv) + 
				  sizeof(tdescr->gca_col_desc[0].gca_l_attname);
    i4			bufsize = hdrsize + 
				  (row_desc->rd_gca->rd_gc_l_cols * attsize);

    /*
    ** Allocate buffer for tuple descriptor
    */
    if ((td_ptr = (char *)MEreqmem(0, bufsize, FALSE, (STATUS *)NULL)) == NULL)
	return( NULL );

    tdescr = (GCA_TD_DATA *)td_ptr;

    /* 
    ** Build tuple descriptor header
    */
    tmpi4 = row_desc->rd_gca->rd_gc_tsize;	/* gca_tsize */
    MECOPY_CONST_MACRO( (PTR)&tmpi4, sizeof(tmpi4), (PTR)td_ptr );
    td_ptr += sizeof( tmpi4 );
    tmpi4 = row_desc->rd_gca->rd_gc_res_mod;	/* gca_result_modifier */
    MECOPY_CONST_MACRO( (PTR)&tmpi4, sizeof(tmpi4), (PTR)td_ptr );
    td_ptr += sizeof( tmpi4 );
    tmpi4 = row_desc->rd_gca->rd_gc_tid;	/* gca_id_tdscr */
    MECOPY_CONST_MACRO( (PTR)&tmpi4, sizeof(tmpi4), (PTR)td_ptr );
    td_ptr += sizeof( tmpi4 );
    tmpi4 = row_desc->rd_gca->rd_gc_l_cols;	/* gca_l_col_desc */
    MECOPY_CONST_MACRO( (PTR)&tmpi4, sizeof(tmpi4), (PTR)td_ptr );
    td_ptr += sizeof( tmpi4 );

    /*
    ** Build column attributes
    */
    for( i = 0; i < row_desc->rd_gca->rd_gc_l_cols; i++ )
    {
	 DB_DATA_VALUE	*dbv = &row_desc->rd_gca->rd_gc_col_desc[i].rd_dbv;

	 tmpi4 = 0;				/* db_data = 0 */
	 MECOPY_CONST_MACRO( (PTR)&tmpi4, sizeof(tmpi4), (PTR)td_ptr );
         td_ptr += sizeof( tmpi4 );
         tmpi4 = dbv->db_length;		/* db_length */
         MECOPY_CONST_MACRO( (PTR)&tmpi4, sizeof(tmpi4), (PTR)td_ptr );
         td_ptr += sizeof( tmpi4 );
         tmpi2 = dbv->db_datatype;		/* db_datatype */
         MECOPY_CONST_MACRO( (PTR)&tmpi2, sizeof(tmpi2), (PTR)td_ptr );
         td_ptr += sizeof( tmpi2 );
         tmpi2 = dbv->db_prec;			/* db_prec */
         MECOPY_CONST_MACRO( (PTR)&tmpi2, sizeof(tmpi2), (PTR)td_ptr );
         td_ptr += sizeof( tmpi2 );
         tmpi4 = 0;				/* gca_l_attname = 0 */
	 MECOPY_CONST_MACRO( (PTR)&tmpi4, sizeof(tmpi4), (PTR)td_ptr );
         td_ptr += sizeof( tmpi4 );
    }

    return( tdescr );
}


/*{
+*  Name: IIcpinto_init - Initialize for next row in COPY INTO
**
**  Description:
**	Initialize pointer to row buffer for next row in COPY INTO.
**
**	If a row/tuple conversion is necessary, make sure there is a buffer
**	allocated to format the next copy row.  If no conversion is needed,
**	point "row" to "tuple" so that we will just write the tuple directly
**	to the copy file.
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**	row		- address of row buffer pointer
**	tuple		- tuple buffer pointer
**
**  Outputs:
**	*tuple		- set to the next tuple buffer
**	*row		- set to row buffer
**
**	Returns: OK, FAIL
-*
**  Side Effects:
**	none
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
**	10-sep-87 (puree)
**	    convert to GCF architecture.
*/
 
i4
IIcpinto_init(copy_blk, cpy_bufh, row, tuple)
II_CP_STRUCT	*copy_blk;
II_CPY_BUFH	*cpy_bufh;		/* UNUSED */
char		**row;
char		*tuple;
{
    /*
    ** Find position for the copy file row.  If there is no conversion,
    ** then just use the tuple buffer directly.
    ** Otherwise, we have to allocate space for the file row.
    */
    if (copy_blk->cp_convert == FALSE)
    {
	*row = tuple;
    }
    else 
    {
	if (copy_blk->cp_rowbuf == NULL)
	{
	    if (loc_alloc((i4)1, (i4)copy_blk->cp_row_length, 
		&copy_blk->cp_rowbuf) != OK)
	    {
		char err_buf[15];
		CVla(copy_blk->cp_rowcount + 1, err_buf);
		IIlocerr(GE_NO_RESOURCE, E_CO0038_MEM_ERR, II_ERR, 1, err_buf);
		return (FAIL);
	    }
	    copy_blk->cp_rbuf_size = copy_blk->cp_row_length;
	}
	*row = copy_blk->cp_rowbuf;
    }
    return (OK);
}
 

/*{
+*  Name: IIcprowformat - Transform copy tuple into row format.
**
**  Description:
**	Builds a copy row given a tuple and descriptions of row and
**	tuple format.
**
**	For each field of the copy row, call ADF to transform the attribute
**	value to the row domain value.  The ADF function blocks for each
**	copy row domain are already set up at copy initialization time.
**	Fields that may not be set up are:
**
**	    - Pointers to the data buffers must be set.
**	    - If we were not able to predict the result length of the ADF
**	      conversion (because the row domain is variable length), then
**	      we need to call adc_calclen to figure it out now.
**
**	If the row domain is TEXT type, then we have to do special processing
**	since COPY uses different TEXT format than the internal DB_TEXT_TYPE.
**	We give ADF a real DB_TEXT_TYPE to convert into, and then we strip off
**	the two-byte length specifier for the row domain value.
**
**	For dummy domains, the domain value is derived from the domain name.
**
**  Inputs:
**	copy_blk	- copy control block
**	tuple		- pointer to tuple from the DBMS table
**
**  Outputs:
**	row		- filled in with copy row
**
**	Returns: OK, FAIL
-*
**  Side Effects:
**	none
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	25-aug-87 (puree)
**	    fix problem with varchar and text fields.
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
**	14-nov-88 (barbara)
**	    fixed bug #3912 -- value in db_t_count inaccurate causing av.
**	14-nov-88 (barbara)
**	    fixed bug #3695 -- isnull flag was never reset.
**	20-jan-89 (barbara)
**	    fixed bug whereby delimiter appears in the wrong place in output
**	    when a null varchar column is COPYed OUT as a fixed length field.
**	03-may-89 (kathryn) Bug #5583
**	    Set isnull to false when NOT NULL has been specified for a column.
**	    Fixes copy.out bug (AV) where col_1 has null, col_2 has been 
**	    specified NOT NULL so adc_isnull() is never called to reset isnull.
**	20-may-89 (kathryn) Bug #5605
**	    When value is null set txt_ptr->db_t_count to length of a NULL.
**	21-jun-89 (barbara)
**	    fixed bug #6380 where cp_name is an empty string for null
**	    dummy columns.
**	14-jun-89 (barbara)
**	    Fixed bug #5798 where default null format was not being blank
**	    padded into c0 or char(0) file format.
**	26-nov-89 (barbara)
**	    Fixed bug #8886.  On COPY TO don't pad default null value unless
**	    column width is wider than length of default null value.
**	24-jan-90 (fredp)
**	    String types represented by 'DB_TEXT_STRING's must be aligned
**	    in memory on BYTE_ALIGN machines. For these types, we MEcopy the
**	    tuple data to an aligned buffer and reference it instead of the
**	    (possibly) unaligned tuple.
**	06-feb-90 (fredp)
**	    Fixed alignment problems for non-string datatypes too.
**	23-apr-90 (mgw)
**	    Check to see if we need data to be BYTE_ALIGN'd on nullable
**	    text and varchar row formats too, not just non-nullable.
**	27-apr-90 (barbara)
**	    Nullable fixed-length varchar must put a 5-char count field
**	    into file.  Previously nullable text and varchar were "slipping
**	    through" the case statement and internal format was written to
**	    file.  Also, only MEfill with nulls if data is not null --
**	    otherwise the trailing null indicator byte will be overwritten.
**	12-oct-90 (teresal)
**	    Fixed alignment problems where an unaligned row buffer was
**	    being sent to ADF for non-string datatypes. Bug fix 33862.
**      03-Dec-2003 (gupsh01)
**          Modified copy code so that nchar and nvarchar types will now use
**          UTF8 to read and write from the text file for a formatted copy.
*/

i4
IIcprowformat( II_LBQ_CB *IIlbqcb, II_CP_STRUCT *copy_blk, PTR tuple, PTR row )
{
    DB_STATUS		status;
    II_CP_MAP		*cmap;
    DB_TEXT_STRING	*txt_ptr;
    i4			i, j;
    i4			name_len;
    i4			isnull = FALSE;
    char		*row_ptr;
    char		*tup_ptr;
    char		*delim_ptr;
    ALIGN_RESTRICT	temp_buf[((DB_GW3_MAXTUP - 1) / sizeof(ALIGN_RESTRICT)) +1];
    i4			fillcnt;
#ifdef BYTE_ALIGN
    ALIGN_RESTRICT	align_buf[((DB_GW3_MAXTUP - 1) / sizeof(ALIGN_RESTRICT)) +1];
    ALIGN_RESTRICT	alignrow_buf[((DB_GW3_MAXTUP - 1) / sizeof(ALIGN_RESTRICT)) +1];
    i4			align_row = FALSE;
 
#endif /* BYTE_ALIGN */
 
    copy_blk->cp_status &=  ~CPY_ERROR;
 
    row_ptr = (char *) row;
    txt_ptr = (DB_TEXT_STRING *) temp_buf;
 
    for (i = 0; i < copy_blk->cp_num_domains; i++)
    {
	cmap = &copy_blk->cp_copy_map[i];

	if (cmap->cp_rowlen == ADE_LEN_UNKNOWN)
	    cmap->cp_fieldlen = cmap->cp_cvlen;
 
	/*
	** If this is a dummy domain, construct the domain value.
	*/
	if (cmap->cp_rowtype == CPY_DUMMY_TYPE)
	{
	    if (cmap->cp_rowlen > 0)
	    {
		/* 
		** Bug #6380 - cp_name will be an empty string
		** for null dummy columns
		*/
		name_len = STlength(cmap->cp_name);
		if (name_len == 0)
		    name_len = 1;
		for (j = 0; j < cmap->cp_rowlen; j++)
		{
		    MEcopy(cmap->cp_name, (u_i2)name_len, row_ptr);
		    row_ptr += name_len;
		}
	    }
	}
 
	else if (cmap->cp_cvid != ADI_NOFI)
	{
#ifdef BYTE_ALIGN
	    if (ME_NOT_ALIGNED_MACRO((char*)tuple + cmap->cp_attroffset)
		&& (cmap->cp_tuptype !=  DB_CHR_TYPE)
		&& (cmap->cp_tuptype != -DB_CHR_TYPE)
		&& (cmap->cp_tuptype !=  DB_CHA_TYPE)
		&& (cmap->cp_tuptype != -DB_CHA_TYPE))
	    {
		/*
		**  This data type must be aligned, so we copy
		**  it into an aligned buffer.
		*/
		MEcopy((PTR)((char *) tuple + cmap->cp_attroffset),
			(u_i2)cmap->cp_tuplen, (PTR)align_buf);
 
		tup_ptr = (char *)align_buf;
	    }
	    else
#endif /* BYTE_ALIGN */
	    {
		tup_ptr = (char *) tuple + cmap->cp_attroffset;
	    }
	    cmap->cp_adf_fnblk.adf_1_dv.db_data = tup_ptr;
 
	    if (cmap->cp_withnull)
	    {
		/* check for null value in tuple data */
 
		if ((status = adc_isnull(IIlbqcb->ii_lq_adf,
			&cmap->cp_adf_fnblk.adf_1_dv, &isnull)) != E_DB_OK)
		{
		    char err_buf[15];
		    CVla(copy_blk->cp_rowcount + 1, err_buf);
		    IIlocerr(GE_LOGICAL_ERROR, E_CO003A_ROW_FORMAT_ERR, II_ERR, 
			     2, err_buf, cmap->cp_name);
		    copy_blk->cp_warnings++;
		    return (FAIL);
		}
	    }
	    else
		isnull = FALSE;		/* Bug 5583 */
	    /*
	    ** If this domain is a TEXT or VARCHAR field, convert into a
	    ** temp buffer and produce the final result in an external form.
	    */
	    if ((cmap->cp_rowtype == DB_TXT_TYPE)    ||
		(cmap->cp_rowtype == DB_VBYTE_TYPE)  ||
		(cmap->cp_rowtype == DB_VCH_TYPE)    ||
		(cmap->cp_rowtype == DB_UTF8_TYPE)   ||
		(cmap->cp_rowtype == -DB_UTF8_TYPE)  ||
	        (cmap->cp_rowtype == -DB_TXT_TYPE)   ||
		(cmap->cp_rowtype == -DB_VBYTE_TYPE) ||
		(cmap->cp_rowtype == -DB_VCH_TYPE))
	    {
		cmap->cp_adf_fnblk.adf_r_dv.db_data = (PTR) temp_buf;
	    }
#ifdef BYTE_ALIGN
	    else if (ME_NOT_ALIGNED_MACRO(row_ptr)
		&& (cmap->cp_rowtype !=  DB_CHR_TYPE)
		&& (cmap->cp_rowtype != -DB_CHR_TYPE)
		&& (cmap->cp_rowtype !=  DB_CHA_TYPE)
		&& (cmap->cp_rowtype != -DB_CHA_TYPE))
	    {
		/*
		** Non-string data must be formatted in an aligned buffer.
		** Before conversion, point to an aligned buffer to store
		** the result row data in and copy it the to row later on.
		** Bug 33862.
		*/
		cmap->cp_adf_fnblk.adf_r_dv.db_data = (PTR) alignrow_buf;
		align_row = TRUE;
	    }
#endif /* BYTE_ALIGN */
	    else
	    {
		cmap->cp_adf_fnblk.adf_r_dv.db_data = row_ptr;
	    }
 
	    /*
	    ** Do the conversion.
	    */
	    if (isnull && cmap->cp_nullen)
	    {
		MEcopy(cmap->cp_nuldata, (u_i2)(cmap->cp_nullen),
			cmap->cp_adf_fnblk.adf_r_dv.db_data);
 
	      /*
	      ** Don't reset fieldlen here.  It causes the delimiter
	      ** to be output in the wrong place on fixed-length
	      ** varchar fields (barbara).
	      */
	/*	cmap->cp_fieldlen = cmap->cp_nullen;	*/
		if ((cmap->cp_rowtype == DB_TXT_TYPE) ||
			(cmap->cp_rowtype == DB_VBYTE_TYPE) ||
			(cmap->cp_rowtype == DB_UTF8_TYPE) ||
			(cmap->cp_rowtype == DB_VCH_TYPE))
		{
 
			/* Bug 5605  */
 
		    DB_TEXT_STRING  	*nulptr;
 
		    nulptr = (DB_TEXT_STRING *)cmap->cp_nuldata;
		    txt_ptr->db_t_count = nulptr->db_t_count;
		}
		/* 
		** Bug 5798.  If variable character format type, blank pad null
		** default value up to cp_fieldlen.
		** Bug 8886.  Don't blank pad if length of null default value
		** >= cp_fieldlen.
		*/
		else if (cmap->cp_rowlen == ADE_LEN_UNKNOWN)
		{
		    fillcnt = cmap->cp_fieldlen - cmap->cp_nullen;
		    if (fillcnt > 0)
			MEfill((u_i2)fillcnt, ' ',
			    (PTR)((char *)cmap->cp_adf_fnblk.adf_r_dv.db_data +
			     cmap->cp_nullen));
		    if (cmap->cp_nullen > cmap->cp_fieldlen)
			cmap->cp_fieldlen = cmap->cp_nullen;
		}    
	    }
	    else
	    {
		status = adf_func(IIlbqcb->ii_lq_adf, &cmap->cp_adf_fnblk);
		if (copy_blk->cp_status & CPY_ERROR)
		{
		    status = E_DB_ERROR;
		}
		if (status)
		{
		    char err_buf[15];
		    CVla(copy_blk->cp_rowcount + 1, err_buf);
		    IIlocerr(GE_LOGICAL_ERROR, E_CO003A_ROW_FORMAT_ERR, II_ERR, 
			     2, err_buf, cmap->cp_name);
		    copy_blk->cp_warnings++;
		    return (FAIL);
		}
	    }
 
	    if ((cmap->cp_rowtype == DB_TXT_TYPE)    ||
		(cmap->cp_rowtype == -DB_TXT_TYPE)   ||
		(cmap->cp_rowtype == DB_VBYTE_TYPE)  ||
		(cmap->cp_rowtype == -DB_VBYTE_TYPE) ||
		(cmap->cp_rowtype == DB_VCH_TYPE)    ||
		(cmap->cp_rowtype == -DB_VCH_TYPE)   ||
		(cmap->cp_rowtype == DB_UTF8_TYPE)    ||
		(cmap->cp_rowtype == -DB_UTF8_TYPE))
	    {
		    char	*ptr;
 
		    ptr = row_ptr;
 
		    if (cmap->cp_rowlen == ADE_LEN_UNKNOWN)
		    {
			/*
			** Variable length varchar or byte type. Put length
			** specifier then the string into the row buffer.
			*/
			cmap->cp_fieldlen = txt_ptr->db_t_count;
			if (cmap->cp_rowtype == DB_VCH_TYPE ||
			    cmap->cp_rowtype == DB_UTF8_TYPE ||
			    cmap->cp_rowtype == DB_VBYTE_TYPE)
			{
			    cmap->cp_fieldlen += CPY_VCHLEN;
			    STprintf(ptr, "%5d", (i4)txt_ptr->db_t_count);
			    ptr += CPY_VCHLEN;
			}
			MEcopy((PTR)txt_ptr->db_t_text,
			    (u_i2)txt_ptr->db_t_count, (PTR)ptr);
		    }
		    else
		    {
			/*
			** Fixed length varchar type.  Put length specifier and
			** then the string.   If data is not null, pad with
			** nulls.
			*/
			if (   cmap->cp_rowtype == DB_VCH_TYPE
			    || cmap->cp_rowtype == -DB_VCH_TYPE
			    || cmap->cp_rowtype == DB_UTF8_TYPE
			    || cmap->cp_rowtype == -DB_UTF8_TYPE
			    || cmap->cp_rowtype == DB_VBYTE_TYPE
			    || cmap->cp_rowtype == -DB_VBYTE_TYPE)
			{
			    if (isnull && !cmap->cp_nullen)
				MEfill((u_i2)CPY_VCHLEN, ' ', (PTR)ptr);
			    else
				STprintf(ptr, "%5d", (i4)txt_ptr->db_t_count);
			    ptr += CPY_VCHLEN;
			}
			if (isnull && !cmap->cp_nullen)
			{
			    MEcopy((PTR)txt_ptr->db_t_text,
				    (u_i2)cmap->cp_cvlen-DB_CNTSIZE, (PTR)ptr);
			}
			else
			{
			    MEcopy((PTR)txt_ptr->db_t_text, 
				    (u_i2)txt_ptr->db_t_count, (PTR)ptr);
			    fillcnt = cmap->cp_rowlen - DB_CNTSIZE
					- txt_ptr->db_t_count;
			    if (fillcnt > 0)
				MEfill((u_i2)fillcnt, '\0',
					(PTR)(ptr + txt_ptr->db_t_count));
			}
		    }
	    }
#ifdef BYTE_ALIGN
	    else if (align_row)
	    {
		/*
		** Copy non-string data from the aligned buffer into the
		** row buffer.
		*/
		MECOPY_VAR_MACRO((PTR)alignrow_buf, (u_i2)cmap->cp_fieldlen,
				 (PTR) row_ptr);
		align_row = FALSE;
	    }
#endif /* BYTE_ALIGN */
 
	    row_ptr += cmap->cp_fieldlen;
	}
	else if (copy_blk->cp_convert)
	{
	    /*
	    ** If no ADF conversion id, then there is no conversion for this
	    ** field, just copy the tuple value to the row buffer.
	    */

	    i4 maxtup;

	    /*
	    ** cp_tuplen is the length of the data from the DBMS,
	    ** cp_rowlen is the length of the data after formatting,
	    ** if cp_rowlen is greater than cp_tuplen then only copy
	    ** cp_tuplen chars over into the format buffer and pad the rest
	    */
	    maxtup = (cmap->cp_rowlen > cmap->cp_tuplen) ? cmap->cp_tuplen :
							   cmap->cp_rowlen;
	    tup_ptr = (char *) tuple + cmap->cp_attroffset;
	    MEcopy((PTR)tup_ptr, (u_i2)maxtup, (PTR)row_ptr);
	    row_ptr += maxtup;
	    if (maxtup < cmap->cp_rowlen)
	    {   /* should this be '\0' filled? will this case ever happen? */
		MEfill((u_i2)(cmap->cp_rowlen - maxtup), ' ', (PTR) row_ptr);
		row_ptr += (cmap->cp_rowlen - maxtup);
	    }
	}
 
	/*
	** If there is a delimiter specified, write it to the row buffer.
	*/
	if (cmap->cp_isdelim)
	{
	    delim_ptr = cmap->cp_delim;
	    CMcpyinc(delim_ptr, row_ptr);
	}
    }
 
    /*
    ** If this row format has variable length, then set the copy control block
    ** row length field to the length of the current row so other routines will
    ** know how to handle it.
    */
    if (copy_blk->cp_varrows)
	copy_blk->cp_row_length = row_ptr - (char *) row;
 
    return (OK);
}
 

/*{
+*  Name: IIcpfrom_init - Initialize for next row in COPY FROM
**
**  Description:
**	Set the client's tuple pointer to the GCA buffer if it is big
**	enough to contain a complete tuple.  Otherwise, point it to
**	copy_blk->cp_tupbuf so that tuple can be transmitted by pieces.
**
**	Initialize pointer to row buffer for the next row in COPY FROM.
**
**	If a row/tuple conversion is necessary, use the preallocated row
**	buffer to the file row into.  If no conversion is needed, point
**	"row" to "tuple" so that we will read the row directly into the
**	tuple buffer.
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**	row		- address of row buffer pointer
**	tuple		- addr of tuple buffer pointer
**
**  Outputs:
**	*tuple		- set to the next tuple buffer
**	*row		- set to row buffer
**
**	Returns: OK, FAIL
-*
**  Side Effects:
**	none
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
**	10-sep-87 (puree)
**	    convert to GCF architecture.
**	29-nov-91 (seg)
**	    Can't dereference or do arithmetic on PTR
*/
i4
IIcpfrom_init
( 
    II_LBQ_CB		*IIlbqcb, 
    II_CP_STRUCT	*copy_blk, 
    II_CPY_BUFH		*cpy_bufh, 
    PTR			*row, 
    PTR			*tuple
)
{
    IICGC_DESC	    *cgc = IIlbqcb->ii_lq_gca;
    STATUS	    status = OK;
 
    /* 
    ** If there is nothing in GCA buffer, initialize it for a
    ** a new message.
    */
    if (copy_blk->cp_not_flushed == FALSE)
    {
	IIcgc_init_msg(cgc, GCA_CDATA, cgc->cgc_query_lang, 0);
	status = IIcgcp3_write_data(cgc, FALSE, (i4)0, &cpy_bufh->cpy_dbv);
	cpy_bufh->cpy_bytesused = (i4)0;
    }
 
    if (cpy_bufh->cpy_dbv.db_length < copy_blk->cp_tup_length)
    {
	*tuple = copy_blk->cp_tupbuf;
    }
    else
    {
	*tuple = (char *)cpy_bufh->cpy_dbv.db_data + cpy_bufh->cpy_bytesused;
    }
    /*
    ** Find position to read file row into.  If there is no data conversion,
    ** just read the file row into the space allocated for the tuple.
    ** Otherwise, we have to use the row buffer.
    */
    if (copy_blk->cp_convert == FALSE)
    {
	*row = *tuple;
    }
    else 
    {
	*row = copy_blk->cp_rowbuf;
    }
    return ((i4) status);
}
 

/*{
+*  Name: IIcptupformat - Transform copy row into tuple format.
**
**  Description:
**	Builds a tuple given a copy row and descriptions of row and
**	tuple format.  Also checks for legal data values in the newly
**	created tuple.
**
**	For each field of the copy row, call ADF to transform the row
**	value to the tuple value.  The ADF function blocks for each
**	copy row domain are already set up at copy initialization time.
**	Fields that may not be set up are:
**
**	    - Pointers to the data buffers must be set.
**	    - If we were not able to predict the result length of the ADF
**	      conversion (because the row domain is variable length), then
**	      we need to call adc_calclen to figure it out now.
**	    - If the row domain is TEXT or VARCHAR type, then we need to
**	      copy the data into a temp DB_TEXT datatype.
**
**	If the result length of the ADF conversion differs from the result
**	attribute length, then we need to convert to the correct length.  If
**	a character field, pad it out with blanks - otherwise call ADF to
**	do the conversion.
**
**	If value checking is necessary, then call ADF to validate each attribute
**	value.
**
**  Inputs:
**	copy_blk	- copy control block
**	row		- pointer to copy row
**
**  Outputs:
**	tuple		- filled in with tuple
**
**	Returns: OK, FAIL
-*
**  Side Effects:
**	The libq control block contains an ADF_CB that is used throughout libq.
**	This ADF control block keeps track of the number of noprintable chars
**	that ADF encountered and just converted to blank.  This field is checked
**	for any blanked chars and then resets the value back to zero.
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	25-aug-87 (puree)
**	    fix problem with varchar and text fields.
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
**	20-may-89 (kathryn)	Bug# 6059  -- IN VMS BETA II -- ONLY
**	    COPY FROM fixed length text file - the length copied should be 
**	    the shorter length of the two: 
**		cmap->cp_rowlen  cmap->cp_fieldlen 
**	24-may-89 (kathryn)	Bug# 6059	- Changed fix.
**	    COPY vchar/text  FROM fixed length text file - length copied should
**	    Always be cmap->cp_fieldlen.  fieldlen in this case is 2 bytes
**	    less than rowlen.  rowlen is length of data + 2 byte count.
**	04-oct-89 (kathryn)     Bug #7645
**	    On COPY (col=varchar(X)) FROM, check data for col to ensure the
**		count is a reasonable value. Ensure that count for varchar is
**		a numeric and is within the allowed lengths of an attribute.
**	24-jan-90 (fredp)
**	    String types represented by 'DB_TEXT_STRING's must be formatted
**	    in aligned memory on BYTE_ALIGN machines, so we use an aligned 
**	    buffer to format the column and then MEcopy it into the tuple.
**	06-feb-90 (fredp)
**	    Fixed alignment problems for non-string datatypes too.
**	14-feb-90 (fredp)
**	    Fix a bug introduced in the alignment fix of 06-feb-90. Colptr
**	    was not being set in all cases which was causing a SEGV in
**	    adc_valchk().
**	15-feb-90 (fredp)
**	    Initialize align_buf to "empty" values, if doing conversion.
**	23-apr-90 (mgw)
**	    Check to see if we need data to be BYTE_ALIGN'd on nullable
**	    text and varchar tuple formats too, not just non-nullable.
**	27-apr-90 (barbara)
**	    Nullable fixed-length varchar and text must copy the complete
**	    field in order to pick up a possible null indicator byte at the
**	    end.
**	03-may-90 (barbara)
**	    If bulk copy and no conversion needed, we do need to copy data
**	    into aligned buf for adc_valchk.  But we don't need to copy back.
**	24-jul-1990 (barbara)
**	    One more byte alignment problem.  When fieldlen is zero,
**	    adc_valchk was getting called with possibly non-aligned data.
**	07-aug-1990 (mgw) Bug #21432
**	    Report user error too if applicable.
**      02-oct-90 (teresal)
**          Bug fix 32562.  Give an error if COPY FROM attempts to put
**          NULL data into a non-nullable field using 'with null'.
**	12-oct-90 (teresal)
**	    Fixed alignment problems where an unaligned row buffer was
**	    being sent to ADF for non-string datatypes. Bug fix 33862.
**	07-feb-1992 (teresal)
**	    Make sure date fields are always validated before sending them
**	    on to the server.  It was assumed adf_func() would always
**	    validate for you. but this is not true. Bug fix 41780.
**	16-jun-2005 (gupsh01)
**	    Added call to adu_unorm to normalize the unicode data when 
**	    reading from a file.
**	24-Jun-2005 (schka24)
**	    Above edit broke unformatted copy with value checking, fix.
**	    Also, don't request/free memory on every single row when
**	    normalizing!
**	7-Jul-2005 (thaju02)
**	    Allocate cp_srcbuf to hold column source data for adu_unorm().
**	03-Aug-2006 (gupsh01)
**	    Added support for ANSI date/time types.
**	09-May-2007 (gupsh01)
**	    Added nfc_flag to adu_unorm to force normalization to NFC
**	    for UTF8 character sets.
*/

i4
IIcptupformat( II_THR_CB *thr_cb, II_CP_STRUCT *copy_blk, PTR row, PTR tuple )
{
    II_LBQ_CB		*IIlbqcb = thr_cb->ii_th_session;
    DB_STATUS		status;
    II_CP_MAP		*cmap;
    DB_TEXT_STRING	*txt_ptr;
    i4			valchk, i;
    char		*row_ptr;
#ifdef WIN16
static    ALIGN_RESTRICT	txt_buf[((DB_GW3_MAXTUP -1) / sizeof(ALIGN_RESTRICT)) +1];
#else
    ALIGN_RESTRICT	txt_buf[((DB_GW3_MAXTUP -1) / sizeof(ALIGN_RESTRICT)) +1];
#endif /* WIN16 */
    char		vchar_count[CPY_VCHLEN + 1];
    i4			tmp_nat;
    char		*tup_ptr;
    char		*col_ptr;
#ifdef BYTE_ALIGN
# ifdef WIN16
static     ALIGN_RESTRICT	align_buf[((DB_GW3_MAXTUP -1) / sizeof(ALIGN_RESTRICT)) +1];
static    ALIGN_RESTRICT	alignrow_buf[((DB_GW3_MAXTUP -1) / sizeof(ALIGN_RESTRICT)) +1];
# else
    ALIGN_RESTRICT	align_buf[((DB_GW3_MAXTUP -1) / sizeof(ALIGN_RESTRICT)) +1];
    ALIGN_RESTRICT	alignrow_buf[((DB_GW3_MAXTUP -1) / sizeof(ALIGN_RESTRICT)) +1];
# endif /* WIN16 */
#endif /* BYTE_ALIGN */
			
 
    copy_blk->cp_status &=  ~CPY_ERROR;
 
    /*
    ** Initialize 'tuple' to be an "empty" tuple.  Don't do this if we are
    ** not doing any conversions, as the copy row has already been read into
    ** the 'tuple' buffer.
    */
    if (copy_blk->cp_convert)
	MEcopy(copy_blk->cp_zerotup, (u_i2)copy_blk->cp_tup_length, tuple);
 
    row_ptr = (char *) row;
    for (i = 0; i < copy_blk->cp_num_domains; i++)
    {
	cmap = &copy_blk->cp_copy_map[i];
	valchk = cmap->cp_valchk;
 
	/* skip over a dummy column */
 
	if (cmap->cp_rowtype == CPY_DUMMY_TYPE)
	{
	    row_ptr += cmap->cp_fieldlen;
	    if (cmap->cp_isdelim)
		CMnext(row_ptr);
	    continue;
	}
 
	/*
	** Get pointer to the start of this column in the tuple buffer.
	*/
	tup_ptr = (char *)tuple + cmap->cp_attroffset; 
#ifdef BYTE_ALIGN
	if (ME_NOT_ALIGNED_MACRO(tup_ptr)
	    && (cmap->cp_tuptype !=  DB_CHR_TYPE)
	    && (cmap->cp_tuptype != -DB_CHR_TYPE)
	    && (cmap->cp_tuptype !=  DB_CHA_TYPE)
	    && (cmap->cp_tuptype != -DB_CHA_TYPE))
	{
	    /*
	    ** This datatype must be formatted in an aligned buffer.
	    ** So, we use 'align_buf' and copy it to the tuple later.
	    */
	    col_ptr = (char *)align_buf;
 
	    /*
	    ** If doing conversion, initialize the 'align_buf' to be 
	    ** an "empty" data value.  If not doing conversion, the column
	    ** has been read from the file into the tuple buffer, so copy
	    ** that into the aligned buffer.
	    */
	    MEcopy((PTR)tup_ptr, (u_i2)cmap->cp_tuplen, (PTR)col_ptr);
	}
	else
#endif /* BYTE_ALIGN */
	{
	    /*
	    ** Format the datatype directly into the tuple buffer.
	    */
	    col_ptr = tup_ptr;
	}


	/*
	** Check if we have to do a conversion on this data type.
	** If we don't have to do a conversion, then just copy the row value
	** to the tuple buffer.  However, if cp_convert is not set then the
	** row and tuple buffer are the same - no copy needed.
	*/
	if (cmap->cp_cvid == ADI_NOFI)
	{
	    if ((cmap->cp_tuptype !=  DB_NCHR_TYPE)
	      && (cmap->cp_tuptype != -DB_NCHR_TYPE)
	      && (cmap->cp_tuptype !=  DB_NVCHR_TYPE)
	      && (cmap->cp_tuptype != -DB_NVCHR_TYPE))
	    {
		if (copy_blk->cp_convert)
		    MEcopy((PTR)row_ptr, (u_i2)cmap->cp_rowlen, (PTR)col_ptr);
	    }
	    else
	    {
		/* Oops, unicode, need to normalize the representation */

		DB_DATA_VALUE  dst_dv;
		DB_DATA_VALUE  src_dv;

		if (cmap->cp_normbuf == NULL)
		    cmap->cp_normbuf = MEreqmem(0, cmap->cp_rowlen * 4, 0, NULL);

		if (cmap->cp_srcbuf == NULL)
		    cmap->cp_srcbuf = MEreqmem(0, cmap->cp_rowlen, 0, NULL);

		if (cmap->cp_rowtype < 0)
		{
		    src_dv.db_datatype = -cmap->cp_rowtype;
		    src_dv.db_length = (u_i2)cmap->cp_rowlen - 1;
		    dst_dv.db_length = (u_i2)cmap->cp_tuplen - 1;
		    dst_dv.db_datatype = -cmap->cp_rowtype;
		}
		else
		{
		    src_dv.db_datatype = cmap->cp_rowtype;
		    src_dv.db_length = (u_i2)cmap->cp_rowlen;
		    dst_dv.db_length = (u_i2)cmap->cp_tuplen;
		    dst_dv.db_datatype = cmap->cp_rowtype;
		}

		MEcopy (row_ptr, (u_i2)cmap->cp_rowlen, cmap->cp_srcbuf);

		src_dv.db_data = cmap->cp_srcbuf;   
		src_dv.db_prec = cmap->cp_cvprec;
		src_dv.db_collID = 0;

		dst_dv.db_data = cmap->cp_normbuf;
		dst_dv.db_prec = cmap->cp_cvprec;
		dst_dv.db_collID = 0;
		    
		if ((status = 
		    adu_unorm(IIlbqcb->ii_lq_adf, &dst_dv, &src_dv, 0)) != E_DB_OK)
		{
		    char err_buf[15];
		    CVla(copy_blk->cp_rowcount + 1, err_buf);
		    IIlocerr(GE_LOGICAL_ERROR, E_CO003A_ROW_FORMAT_ERR,
				    II_ERR, 2, err_buf, cmap->cp_name);
		    copy_blk->cp_warnings++;
		    status = FAIL;
		    break;
		}

		MEcopy((PTR)dst_dv.db_data, (u_i2)cmap->cp_tuplen, (PTR)src_dv.db_data);
	    }
	} /* if nofi */
	/*
	** If the field read in was not zero length (cp_fieldlen
	** set by IIcpgetrow), do the conversion.  Otherwise, just 
	** use the null value that was already initialized for this 
	** attribute.
	**
	** If "with null (value)" clause was specified for this field,
	** check for the value representing null.  If so, skip to the
	** the next field.  The null value has already been formatted
	** into the tuple buffer by IIcpinit.
	**
	** Bug fix 32562: If a "with null (value)" was specified for a
	** field that is not nullable and 'value' was found in the file,
	** give an error.
	*/
	else if (IIcp_isnull(cmap, row_ptr))	/* With null value found ? */
	{
	    if (cmap->cp_tuptype > 0)           /* Is field not nullable ? */
	    {
		/* Attempt to put NULL data into a not nullable field */
		IIlocerr(GE_LOGICAL_ERROR, E_CO003E_NOT_NULLABLE,
			II_ERR, 1, cmap->cp_name);
		copy_blk->cp_warnings++;
		return (FAIL);
	    }
	}
	else if (cmap->cp_fieldlen > 0)
	{
	    cmap->cp_adf_fnblk.adf_r_dv.db_data = col_ptr;
 
	    switch (cmap->cp_rowtype)	/* special cases for text and vchar */
	    {
		case DB_TXT_TYPE:
		case -DB_TXT_TYPE:
		{
		    txt_ptr = (DB_TEXT_STRING *) txt_buf;
		    if (cmap->cp_rowlen == ADE_LEN_UNKNOWN)
		    /* Variable length text field */
		    {
			txt_ptr->db_t_count = cmap->cp_fieldlen; 
		    }
		    else
		    /*
		    ** Fixed length text field:
		    ** Decide on length by counting data up to first null
		    ** However, we copy bytes according to value of
		    ** cp_fieldlen because terminating null indicator byte
		    ** is significant.
		    */
		    {
			/* Bug #6059 */
			txt_ptr->db_t_count = STnlength(cmap->cp_fieldlen,
								row_ptr);
		    }
		    MEcopy(row_ptr, (u_i2)cmap->cp_fieldlen,
					(PTR)txt_ptr->db_t_text);
		    /* 
		    ** Modify ADF function block to point to the temporary
		    ** buffer.
		    */ 
		    cmap->cp_adf_fnblk.adf_1_dv.db_data = (PTR) txt_ptr;
		    cmap->cp_adf_fnblk.adf_1_dv.db_length = 
			    cmap->cp_fieldlen + DB_CNTSIZE;
		    break;
		}
 
		case DB_VCH_TYPE:
		case -DB_VCH_TYPE:
		case DB_UTF8_TYPE:
		case -DB_UTF8_TYPE:
		case DB_VBYTE_TYPE:
		case -DB_VBYTE_TYPE:
		{
		    /* 
		    ** Move the character count field and character string
		    ** into the temporary buffer.  Set the ADF function block
		    ** to point to the buffer.
		    */ 
		    txt_ptr = (DB_TEXT_STRING *) txt_buf;
 
		    MEcopy((PTR)row_ptr, (u_i2)CPY_VCHLEN, (PTR)vchar_count);
		    vchar_count[CPY_VCHLEN] = '\0';
		    /* Bug 7645 */
		    status = CVan(vchar_count, &tmp_nat);
		    if ((status != OK) || (tmp_nat < 0) ||
			(tmp_nat > DB_GW4_MAXSTRING))
		    {
			char err_buf[15];
			CVla(copy_blk->cp_rowcount + 1, err_buf);
			IIlocerr(GE_DATA_EXCEPTION, E_CO000B_LEN_SPEC_ERR, 
				  II_ERR, 2, cmap->cp_name, err_buf);
			copy_blk->cp_warnings++;
			return (FAIL);
		    }

		    txt_ptr->db_t_count = (u_i2)tmp_nat;
 
		    if (cmap->cp_rowlen == ADE_LEN_UNKNOWN)
		    /*
		    ** Variable length varchar:
		    ** Copy number of chars according to count.
		    */
		    {
			MEcopy((PTR)(row_ptr + CPY_VCHLEN),
			    (u_i2)txt_ptr->db_t_count, (PTR)txt_ptr->db_t_text);
			cmap->cp_adf_fnblk.adf_1_dv.db_length = 
			    txt_ptr->db_t_count + DB_CNTSIZE;
		    }
		    else
		    /*
		    ** Fixed length varchar:
		    ** Copy up to width of field.  If nullable VCH, we
		    ** must read null indicator byte.
		    */
		    {
			MEcopy((PTR)(row_ptr + CPY_VCHLEN),
			    (u_i2)cmap->cp_cvlen-DB_CNTSIZE,
			    (PTR)txt_ptr->db_t_text);
			cmap->cp_adf_fnblk.adf_1_dv.db_length = cmap->cp_cvlen;
		    }
		    cmap->cp_adf_fnblk.adf_1_dv.db_data = (PTR) txt_ptr;
		    break;
		}
		default:
		{
#ifdef BYTE_ALIGN
		    if (ME_NOT_ALIGNED_MACRO(row_ptr)
	    		&& (cmap->cp_rowtype !=  DB_CHR_TYPE)
	    		&& (cmap->cp_rowtype != -DB_CHR_TYPE)
	    		&& (cmap->cp_rowtype !=  DB_CHA_TYPE)
	    		&& (cmap->cp_rowtype != -DB_CHA_TYPE))
		    {
			/*
			** Non-string data must be aligned in the row buffer
			** before conversion so copy data to an aligned buffer.
			** Bug 33862.
			*/
			MECOPY_VAR_MACRO((PTR)row_ptr, (u_i2)cmap->cp_fieldlen,
					(PTR) alignrow_buf);
		        cmap->cp_adf_fnblk.adf_1_dv.db_data = 
					(char *)alignrow_buf;
		    }
		    else
#endif /* BYTE_ALIGN */
		        cmap->cp_adf_fnblk.adf_1_dv.db_data = row_ptr;
		    break;
		}
	    }
	    /*
	    ** Do the conversion.
	    */
	    status = adf_func(IIlbqcb->ii_lq_adf, &cmap->cp_adf_fnblk);
	    valchk = 0;
	    if (copy_blk->cp_status & CPY_ERROR)
	    {
		status = E_DB_ERROR;
	    }
	    if (status)
	    {
		char err_buf[15];
		CVla(copy_blk->cp_rowcount + 1, err_buf);
		IIlocerr(GE_LOGICAL_ERROR, E_CO0039_TUP_FORMAT_ERR, II_ERR, 
			 2, err_buf, cmap->cp_name);
		copy_blk->cp_warnings++;

		/* Report user error if applicable (Bug 21432) */
		/* 
		** Display error message formatted by ADF, note:
		** these messages do not have timestamps (Bug 36059) 
		*/
		if (IIlbqcb->ii_lq_adf->adf_errcb.ad_errclass == ADF_USER_ERROR)
		{
		    IIsdberr( thr_cb, 
			&IIlbqcb->ii_lq_adf->adf_errcb.ad_sqlstate,
			ss_2_ge(
			  IIlbqcb->ii_lq_adf->adf_errcb.ad_sqlstate.db_sqlstate,
			  IIlbqcb->ii_lq_adf->adf_errcb.ad_usererr),
			IIlbqcb->ii_lq_adf->adf_errcb.ad_usererr,
			0, 
			IIlbqcb->ii_lq_adf->adf_errcb.ad_emsglen,
			IIlbqcb->ii_lq_adf->adf_errcb.ad_errmsgp,
			FALSE);
		}
		return (FAIL);
	    }

	    /*
	    ** When adf_func() converts a DATE type to a DATE type, it doesn't
	    ** verify that the input date is in a correct INGRES 
	    ** internal date format, it just does an MEcopy.  This is
	    ** correct behavior for other adf_func() users since INGRES has
	    ** already generated/validated the date; however, COPY
	    ** is getting an internal date field from an "outside" source
	    ** (a row in the file) and thus, we have to validate this date
	    ** before sending it on to the server.  (Bug fix 41780)
	    */
	    if ((cmap->cp_rowtype == DB_DTE_TYPE &&
		   cmap->cp_tuptype == DB_DTE_TYPE) &&
		(cmap->cp_rowtype == DB_ADTE_TYPE &&
		   cmap->cp_tuptype == DB_ADTE_TYPE) &&
		(cmap->cp_rowtype == DB_TMWO_TYPE &&
		   cmap->cp_tuptype == DB_TMWO_TYPE) &&
		(cmap->cp_rowtype == DB_TME_TYPE &&
		   cmap->cp_tuptype == DB_TME_TYPE) &&
		(cmap->cp_rowtype == DB_TMW_TYPE &&
		   cmap->cp_tuptype == DB_TMW_TYPE) &&
		(cmap->cp_rowtype == DB_TSWO_TYPE &&
		   cmap->cp_tuptype == DB_TSWO_TYPE) &&
		(cmap->cp_rowtype == DB_TSW_TYPE &&
		   cmap->cp_tuptype == DB_TSW_TYPE) &&
		(cmap->cp_rowtype == DB_TSTMP_TYPE &&
		   cmap->cp_tuptype == DB_TSTMP_TYPE) &&
		(cmap->cp_rowtype == DB_INYM_TYPE &&
		   cmap->cp_tuptype == DB_INYM_TYPE) &&
		(cmap->cp_rowtype == DB_INDS_TYPE &&
		   cmap->cp_tuptype == DB_INDS_TYPE))
	    {
		valchk = 1;
	    }
 
	    /*
	    ** Check if ADF converted any unprintable chars to blank here.
	    ** Although it is a bit kludgy, we look directly into the ADF
	    ** session control block to get this information.  The clean way
	    ** is to call adx_chkwarn, but that has its drawbacks:
	    **	   - it returns the first warning it finds - thus you may
	    **	     have to call it several times to get the warning we are
	    **	     looking for.
	    **	   - since it counts chars converted, rather than strings
	    **	     which had chars in them converted, we would have to do
	    **	     this loop of calls to adx_chkwarn after each conversion
	    **	     in order to get the number of domains affected.
	    **	     this could be massively expensive.
	    **	   - adx_chkwarn will format a message in the ADF control block
	    **	     if there is a pointer to a message buffer.  we don't want
	    **       a message, but we don't want to zero out the LIBQ adf
	    **       control block's error buffer either.
	    */
	    if (IIlbqcb->ii_lq_adf->adf_warncb.ad_noprint_cnt)
	    {
		copy_blk->cp_blanked++;
		IIlbqcb->ii_lq_adf->adf_warncb.ad_noprint_cnt = 0;
	    }
	}
 
	/*
	** Have ADF make sure that the data value just read in is a legal
	** Ingres data value.
	*/
	if (valchk)
	{
	    if (IICPvcValCheck( IIlbqcb, col_ptr, cmap, copy_blk ) != OK)
		return (FAIL);
	}
 
#ifdef BYTE_ALIGN
	/*
	** If we had to use a temp. aligned buffer to format the column,
	** then copy the formatted data into the tuple buffer.
	*/
	if (ME_NOT_ALIGNED_MACRO(tup_ptr)
	    && (cmap->cp_tuptype !=  DB_CHR_TYPE)
	    && (cmap->cp_tuptype != -DB_CHR_TYPE)
	    && (cmap->cp_tuptype !=  DB_CHA_TYPE)
	    && (cmap->cp_tuptype != -DB_CHA_TYPE)
	    && copy_blk->cp_convert)
	{
	    MEcopy((PTR)align_buf, (u_i2)cmap->cp_tuplen, (PTR)tup_ptr); 
	}
#endif /* BYTE_ALIGN */
 
	
	/*
	** Advance to the next domain of the copy row.  Skip the delimiter if
	** there is one.
	*/
	
	row_ptr += cmap->cp_fieldlen;
	if (cmap->cp_isdelim)
	    CMnext(row_ptr);
    }
 
    return (OK);
}
 

/*{
+*  Name: IIcp_isnull	- Check for a pattern that represents a null value
**			  in a row domain.
**
**  Description:
**
**  Inputs:
**	cmap		- copy map ptr
**	row_ptr		- ptr to the row domain
**
**  Outputs:
**	Returns:	1 if the pattern matches the null value.
**			0 otherwise.
-*
**  Side Effects:
**	none
**
**  History:
**	12-jun-87 (puree)
**	    written
**	20-jan-89 (barbara)
**	    Fixed bug #4370.  For varchar count field must be converted into
**		ascii representation before comparison is done.  For text
**		count field should be ignored.
**	20-may-89 (kathryn)  Bug #5605
**		Add CP_VCHLEN to nullen for the comparison to a variable length
**		field, so that actual data values and not just the blanks 
**		preceding the data are used for comparison. CP_VCHLEN is the
**		the number of blanks preceding both the null data and the
**		row data.
**      08-Jun-98 (wanya01) bug 91259
**              Add fieldlen to detect if cp_nuldata appears at begining of
**              row_ptr and also is a substring of row_ptr.
**      19-Jun-98 (wanya01) revised change for bug 91259
**              Trailing blanks should not be counted when comparing null
**              value with actual data value.
**	15-nov-2000 (gupsh01)
**		copying out of money fields was not done correctly when
**		specifying a with null clause. This is related to bug 
**		91259. The field length was not being calculated correctly.
**      03-Dec-2003 (gupsh01)
**          Modified copy code so that nchar and nvarchar types will now use
**          UTF8 to read and write from the text file for a formatted copy.
*/
i4
IIcp_isnull(cmap, row_ptr)
II_CP_MAP   *cmap;
char	    *row_ptr;
{
	char	    	temp_buf[100];
	char	    	*nullptr;
	DB_TEXT_STRING	*txt_ptr;
	char	    	*rowptr = row_ptr;
	i4	    	nullen = 0;
        i4         fieldlen = 0;

    if (cmap) fieldlen =  cmap->cp_fieldlen;

    /*
    ** NULL pattern allowed only when "with null (value)" clause
    ** was specified.
    */
 
    if (cmap->cp_withnull && cmap->cp_nullen)
    {
	if (cmap->cp_rowtype == DB_VCH_TYPE ||
	    cmap->cp_rowtype == DB_UTF8_TYPE ||
	    cmap->cp_rowtype == DB_VBYTE_TYPE)
	{
	    txt_ptr = (DB_TEXT_STRING *)cmap->cp_nuldata;
	    nullptr = temp_buf;
	    STprintf( nullptr, "%5d", (i4)txt_ptr->db_t_count );
	    nullptr += CPY_VCHLEN;
	    MEcopy( (PTR)txt_ptr->db_t_text, (u_i2)txt_ptr->db_t_count,
		    (PTR)nullptr );
	    nullptr = temp_buf;
	    nullen = txt_ptr->db_t_count + CPY_VCHLEN;  /* bug #5605 */
	}
	else if (cmap->cp_rowtype == DB_TXT_TYPE)
	{
	    txt_ptr = (DB_TEXT_STRING *)cmap->cp_nuldata;
	    nullptr = (char *)txt_ptr->db_t_text;
	    nullen = txt_ptr->db_t_count;
	}
	else
	{
	    nullptr = cmap->cp_nuldata;
	    nullen = cmap->cp_nullen;
	}

	while (nullen)
	{
	    if ((*nullptr++) != (*rowptr++))
		break; 
	    nullen--;
            fieldlen--;
	}
        if (nullen == 0)
        {
          /* for Varchar , char and varbyte type
          ** and if the length is known then
          ** there may be trailing blanks in the
          ** field length, we skip them for
          ** comparison.
          */
          if (fieldlen > 0)
          {
              while(fieldlen &&
                    (*rowptr == ' ' || *rowptr == '\0'))
              {
                  fieldlen--;
                  rowptr++;
              }
           }
           if (fieldlen == 0)
	     return(1);
        }
    }
 
    return(0);
}
 

/*{
+*  Name: IIcpfopen - Open the copy file.
**
**  Description:
**	Call SI to open the copy file.  Open in write mode if COPY INTO, read
**	mode if COPY FROM.
**
**  Inputs:
**	copy_blk	- copy control block
**
**  Outputs:
**	Returns: OK, FAIL
-*
**  Side Effects:
**	File is opened, possibly created (if COPY FROM).
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
**	22-jan-90 (barbara)
**	    Fixed up call to LOfroms to use correct sized buffer.
**	04-apr-1997 (canor01)
**	    Windows NT must always open the file in binary mode
**	    when reading, and do the CRLF=>LF translation at
**	    character read time to prevent ^Z character from
**	    causing EOF.
**    	23-nov-98 (holgl01) bug 93563
**         Set rms buffer for vms systems for a TXT (text) file to the 
**         maximum record that can be written (SI_MAX_TXT_REC) 
**	   for SIfopen. Otherwise, files greater than 4096 in length cannot
**	   be opened.
**	22-dec-1998 (holgl01) Revision of Bug 93563
**	    Pass SI_MAX_TXT_REC directly  to SIfopen in IIcpfopen
**	    instead of resetting  cp_row_length to SI_MAX_TXT_REC 
**          and passing that.
*/
i4
IIcpfopen(copy_blk)
II_CP_STRUCT	*copy_blk;
{
    LOCATION	loc;
    char	locbuf[MAX_LOC +1];	/* Holds location string */
    DB_STATUS	status;
 
    STcopy( copy_blk->cp_filename, locbuf );
    LOfroms(PATH & FILENAME, locbuf, &loc);
 
    if (copy_blk->cp_direction == CPY_INTO)
    {
	status = SIfopen(&loc, "w", copy_blk->cp_filetype,
	    copy_blk->cp_row_length, &copy_blk->cp_file_ptr);
	if (status)
	    IIlocerr(GE_HOST_ERROR, E_CO0006_FILE_CREATE_ERR, II_ERR, 
		     1, copy_blk->cp_filename);
    }
    else 
    {
# if defined(NT_GENERIC)
        status = SIfopen(&loc, "r", SI_BIN,
            copy_blk->cp_row_length, &copy_blk->cp_file_ptr);
# elif defined(VMS)
        /*
        **  holgl01 bug 93563
        **      Pass maximum record length allowed for text files
        **      so that the rms (vax) buffer will be large enough
        **      to read any record from a text file.
        */
        if(copy_blk->cp_filetype == SI_TXT)
        {
          status = SIfopen(&loc, "r", copy_blk->cp_filetype,
            SI_MAX_TXT_REC, &copy_blk->cp_file_ptr);
        }
        else
        {
          status = SIfopen(&loc, "r", copy_blk->cp_filetype,
            copy_blk->cp_row_length, &copy_blk->cp_file_ptr);
        }
# else
        status = SIfopen(&loc, "r", copy_blk->cp_filetype,
            copy_blk->cp_row_length, &copy_blk->cp_file_ptr);
#endif /* VMS or NT_GENERIC */
	if (status)
	    IIlocerr(GE_HOST_ERROR, E_CO0005_FILE_OPEN_ERR, II_ERR,
		     1, copy_blk->cp_filename);
    }
 
    if (status)
    {
	copy_blk->cp_warnings++;
	return (FAIL);
    }
    return (OK);
}

/*{
+*  Name: IIcpgetrow - Get next copy row.
**
**  Description:
**	If COPY FROM PROGRAM, calls user supplied input routine to get
**	next copy row.
**
**	If the row is variable length, then the row is read from the copy
**	file domain by domain.  If a domain is fixed length, it is read
**	using one SIread call.  If a domain is variable length, then the
**	file is read one character at a time, checking each character for
**	the delimiter char.
**
**	When a variable length domain is read in, the actual length read is
**	set in the ADF control block for this domain.
**
**	If we run out of room in the current row buffer, we free it and allocate
**	a new one.
**
**	Escaped chars are not matched against the delimiter.  If an escape char
**	is encountered in a variable length column, it is discarded - thus it
**	will not show up in the bad-row log if this row cannot be converted.
**
**	If we read more than the maximum allowed bytes before we get to the
**	delimiter, then we just discard the chars until we get to the delimiter
**	or end of file.  The truncated characters will not show up in the
**	bad-row log if this row cannot be converted.
**
**  Inputs:
**	copy_blk	- copy control block
**	get_row		- user supplied input routine for COPY FROM PROGRAM
**
**  Outputs:
**	row		- filled in with copy row
**
**	Returns: OK, FAIL
-*
**  Side Effects:
**	User supplied routine may be called.
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
**	08-mar-88 (puree)
**	    use cp_fieldlen instead of cp_rowlen to read data in.
**	08-oct-90 (teresal)
**	    Make sure to put out a warning message if character data has
**	    been truncated.  Also fixed a problem where warning was not
**	    being displayed if the character field is nullable.
**	29-mar-91 (barbara)
**	    Extended COPY FROM PROGRAM support to allow copying variable
**	    length rows.  Abstracted the code that "gets" a char (either
**	    from file or from user buffer).  This also hides away the
**	    double byte code.  Note that this change does add another
**	    couple of layers of function calls on top of SIgetc for every
**	    single char in a char(0), c0 or text field; an improvement might
**	    be to do an initial SIgetrec into a buffer and then get the chars
**	    from there.
**	11-apr-91 (barbara)
**	    Added extra argument to IIcpgetbytes/char to indicate whether
**	    we are at the start of a domain or not.  IIcpgetbytes needs this
**	    info to know whether or not an EOF is an error.
**	5-mar-92 (seg)
**	    Fixed bogus if statement.
**	25-mar-96 (kch)
**	    Set end_ptr once before the domain by domain read from the copy
**	    file (calls to IICPgcGetCol()). Previously, end_ptr was being
**	    wrongly reset after each domain. This change fixes bug 75452.
**	25-apr-97 (somsa01)
**	    Needed to propagate canor01's changes for manually handling the
**	    CRLF=>LF conversion.
*/

i4
IIcpgetrow(copy_blk, row, get_row)
II_CP_STRUCT	*copy_blk;
PTR		*row;
i4		(*get_row)();
{
    char	*row_ptr = (char *)(*row);
    char	*end_ptr;
    STATUS	status;
    i4	num_read;
    i4		i;
    i4		c;
 
 
    /*
    ** If the length of the row is constant, just go out and get it - otherwise
    ** process domain by domain.
    */
    if (!copy_blk->cp_varrows)
    {
	if (copy_blk->cp_program)
	    status = (*get_row)(&copy_blk->cp_row_length, row_ptr, 
			&num_read);
	else
# ifdef NT_GENERIC
	/*
	** On a READ, we always open the file as binary, since
	** text files will read ^Z as EOF.  However, this
	** means we must do the CRLF=>LF translation here.
	*/
	if ( copy_blk->cp_filetype == SI_TXT )
	{
	    for ( num_read = 0; num_read < (copy_blk->cp_row_length);
		  num_read++ )
	    {
		c = SIgetc( copy_blk->cp_file_ptr );
		if ( c == EOF )
		    return (ENDFILE);
		row_ptr[num_read] = (char) c;
		if ( c == '\r' )
		{
		    c = SIgetc( copy_blk->cp_file_ptr );
		    if ( c == '\n' )
			row_ptr[num_read] = (char) c;
		    else
			SIungetc( c, copy_blk->cp_file_ptr );
		}
	    }
	    status = OK;
	}
	else
# endif /* NT_GENERIC */
	    status = SIread(copy_blk->cp_file_ptr, (copy_blk->cp_row_length),
			&num_read, row_ptr);
 
	if (num_read == 0)
	{
	    return (ENDFILE);
	}
	else if ((status != OK) || (num_read != copy_blk->cp_row_length))
	{
	    char err_buf[15];
	    CVla(copy_blk->cp_rowcount + 1, err_buf);
	    if (copy_blk->cp_program)
		IIlocerr(GE_LOGICAL_ERROR, E_CO003C_PROG_READ_ERR, II_ERR,
			 1, err_buf);
	    else
		IIlocerr(GE_LOGICAL_ERROR, E_CO0024_FILE_READ_ERR, II_ERR,
			 1, err_buf);
	    return (FAIL);
	}
    }
    else 
    {
	if (copy_blk->cp_program)
	{
	    i4 count = DB_GW3_MAXTUP;
	    status = (*get_row)(&count, copy_blk->cp_usrrow.u_rowbuf,
			&num_read);
	    if (num_read == 0)
	    {
		return (ENDFILE);
	    }
	    else if (status != OK) 
	    {
		char err_buf[15];
		CVla(copy_blk->cp_rowcount + 1, err_buf);
		if (copy_blk->cp_program)
		    IIlocerr(GE_LOGICAL_ERROR, E_CO003C_PROG_READ_ERR, II_ERR,
			 1, err_buf);
		return (FAIL);
	    }
	    /* Set pointer to user row and length */
	    copy_blk->cp_usrrow.u_rowp = copy_blk->cp_usrrow.u_rowbuf;
	    copy_blk->cp_usrrow.u_rowlen = num_read;
	}
	/*
	** Read domain by domain.  On variable length domains, read char
	** by char looking for delimiter.
	*/
	/* Bug 75452 - Set end_ptr once before domain by domain read */
	end_ptr = row_ptr + copy_blk->cp_rbuf_size - 1;
	for (i = 0; i < copy_blk->cp_num_domains; i++)
	{
	    status = IICPgcGetCol(copy_blk, &row_ptr, &end_ptr, i, row,
				  get_row);
	    if (status != OK)
		return status;
	}
 
	/* Save row length */
	copy_blk->cp_row_length = row_ptr - (char *)(*row);
    }
 
    return (OK);
}

/*{
+*  Name: IICPgcGetCol - Read the requested column value from the Copy File
**
**  Description:             
**
**  Inputs:
**	copy_blk	- copy control block
**	row_ptr		- current position in cp_rowbuf
**	end_ptr		- end of cp_rowbuf
**	i		- index into copy maps
**	row		-
**	get_row		- user supplied input routine for COPY FROM PROGRAM
**
**  Outputs:
**	row_ptr		-
**	row		-
**
**  Returns: OK, FAIL
**
**  History:
**	jun-1993 (mgw)
**	    Extracted out of IIcpgetrow() so I could call this from
**	    IICPfbwFmtBlobWrite() too.
**	25-mar-96 (kch)
**	    Correctly reset end_ptr after the allocation of a bigger
**	    row buffer. This change fixes bug 75452.
**	26-mar-96 (kch)
**	    We now test for delimited fixed width columns, and if found,
**	    the byte count in the call to IIcpgetbytes() is incremented.
**	    Similarly, the row buffer, row_ptr, is incremented by an
**	    extra byte. This change fixes bug 75437, which is a side
**	    effect of the previous fix for bug 72678.
**	16-Aug-1999 (hanch04)
**	    Need to check the return value for IIcpgetch in IICPgcGetCol
**	03-aug-00 (wanfr01)
**	    Bug 102007, INGEMB 43
**	    on some platforms, strlen() takes an usually long time.
**	    initialized the column buffer so strlen returns length of
**	    the column, not the length of the buffer 
**      03-Dec-2003 (gupsh01)
**          Modified copy code so that nchar and nvarchar types will now use
**          UTF8 to read and write from the text file for a formatted copy.
**	27-jan-2004 (somsa01)
**	    Added "COPY FROM/INTO... PROGRAM" case for BLOBs.
**      05-Jul-2005 (mutma03)
**          dos2unix stuff, filtered ctrl<M> character from the column data
**          for the files that were generated from windows and run on other
**          platforms.
*/
STATUS
IICPgcGetCol(II_CP_STRUCT *copy_blk, char **row_ptr, char **end_ptr, i4  i,
             PTR *row, i4 (*get_row)())
{
    II_CP_MAP	*copy_map;
    i4	cp_nbuf_size;
    char	*cp_newbuf;
    i4	cp_max_col;
    STATUS	status;
    char	*startdom_ptr;
    i4		txt_count;
    i4	cp_max_dom;
    bool	is_double;
    char	cbuf[2];
    bool	cp_escaped_char;
    i4	num_read;

    copy_map = &copy_blk->cp_copy_map[i];

    /*
    ** If this field may not fit in our row buffer, then allocate
    ** a bigger row buffer.
    */
    if (copy_map->cp_rowlen > 0)
	num_read = copy_map->cp_fieldlen;
    else
	num_read = DB_GW3_MAXTUP;
    if (copy_map->cp_isdelim)
	num_read += CPY_MAX_DELIM;
    if (*row_ptr + num_read >= *end_ptr)
    {
	/*
	** Note: for variable length rows, I think this
	** allocation is unnecessary because the file is
	** opened with records of MAXTUP so why allocate an
	** internal buffer that is bigger?  Didn't have time
	** to verify this.  Barbara - 4/4/91
	*/
	cp_nbuf_size = copy_blk->cp_rbuf_size * 2;
	if (loc_alloc_i((i4)1, (i4)cp_nbuf_size, 
	    (PTR *) &cp_newbuf) != OK)
	{
	    char err_buf[15];
	    CVla(copy_blk->cp_rowcount + 1, err_buf);
	    IIlocerr(GE_NO_RESOURCE, E_CO0038_MEM_ERR, II_ERR,
		     1, err_buf);
	    return (FAIL);
	}
	if (*row_ptr - (char *)(*row) > 0)
	    MEcopy(copy_blk->cp_rowbuf, (u_i2)(*row_ptr - 
		(char *)(*row)), (PTR) cp_newbuf);
	MEfree(copy_blk->cp_rowbuf);
	*row_ptr = cp_newbuf + (i4)(*row_ptr - (char *)(*row));
	/* Bug 75452 - Reset end_ptr */
	*end_ptr = cp_newbuf + cp_nbuf_size;
	copy_blk->cp_rowbuf = (PTR) cp_newbuf;
	copy_blk->cp_rbuf_size = cp_nbuf_size;
	*row = (PTR) cp_newbuf;
    }
    /*
    ** Set maximum column width for character datatypes.  Column
    ** width is used later on to determine if data in the
    ** row will be truncated when copied into the column.
    ** Bug 20335.
    */
    switch(copy_map->cp_tuptype)
    {
   	case DB_CHR_TYPE:
   	case DB_CHA_TYPE:
      	     cp_max_col = copy_map->cp_tuplen;
             break;
   	case -DB_CHR_TYPE:
   	case -DB_CHA_TYPE:
      	     cp_max_col = copy_map->cp_tuplen - 1;
      	     break;
   	case DB_TXT_TYPE:	
   	case DB_VCH_TYPE:
   	case DB_UTF8_TYPE:
   	case DB_VBYTE_TYPE:
      	     cp_max_col = copy_map->cp_tuplen - DB_CNTSIZE;
      	     break;
   	case -DB_TXT_TYPE:
   	case -DB_VCH_TYPE:
   	case -DB_UTF8_TYPE:
   	case -DB_VBYTE_TYPE:
      	     cp_max_col = copy_map->cp_tuplen  - 1 - DB_CNTSIZE;
      	     break;
   	default:
      	     cp_max_col = DB_GW3_MAXTUP;
      	     break;
    }

    /*
    ** If this field is fixed len, read it in.
    */
    if (copy_map->cp_rowlen > 0)
    {
	if (!copy_map->cp_isdelim)
		status = IIcpgetbytes( copy_blk, *row_ptr,
				copy_map->cp_fieldlen, i, TRUE, get_row );
	else
		status = IIcpgetbytes( copy_blk, *row_ptr,
				(copy_map->cp_fieldlen + 1), i, TRUE, get_row );

	if (status != OK)
	    return status;

	if (!copy_map->cp_isdelim)
		*row_ptr += copy_map->cp_fieldlen;
	else
		*row_ptr += (copy_map->cp_fieldlen + 1);
    }
    else if (copy_map->cp_rowtype == DB_VCH_TYPE ||
    	     copy_map->cp_rowtype == DB_UTF8_TYPE ||
             copy_map->cp_rowtype == DB_VBYTE_TYPE)
    {
	/*
	** Variable length varchar type.  Read in length specifier
	** and then data string.
	*/
	startdom_ptr = *row_ptr;
	status = IIcpgetbytes(copy_blk, *row_ptr, CPY_VCHLEN, i, TRUE, get_row);
	if (status != OK)
	    return status;

	/* Null terminate length specifier */
	*(*row_ptr + CPY_VCHLEN) = '\0';
	txt_count = 0;
	status = CVan(*row_ptr, &txt_count);
	if ((status != OK) || (txt_count < 0) ||
	    (txt_count > DB_GW4_MAXSTRING))
	{
	    char err_buf[15];
	    CVla(copy_blk->cp_rowcount + 1, err_buf);
	    IIlocerr(GE_DATA_EXCEPTION, E_CO000B_LEN_SPEC_ERR, II_ERR, 
		     2, copy_map->cp_name, err_buf);
	    return FAIL;
	}

	*row_ptr += CPY_VCHLEN;

	if (txt_count > 0)
	{
	    status = IIcpgetbytes(copy_blk, *row_ptr, txt_count, i, FALSE,
				  get_row);
	    if (status != OK)
		return status;
	}

	*row_ptr += txt_count;
	copy_map->cp_fieldlen = txt_count + CPY_VCHLEN;
	copy_map->cp_adf_fnblk.adf_1_dv.db_length = txt_count + DB_CNTSIZE;

	/*
	** If there is a delimiter, read till we get to it.
	*/
	if (copy_map->cp_isdelim)
	{
	    cp_max_dom = DB_GW3_MAXTUP - txt_count - CPY_VCHLEN;
	    for (;;)
	    {
		status = IIcpgetch(copy_blk, cbuf, &is_double, i, FALSE,
				   get_row);
		if (status != OK)
		    return status;

		if ((cp_max_dom > 0) &&
		    (!is_double || (cp_max_dom > 1)))
		{
		    CMcpychar(cbuf, *row_ptr);
		    CMnext(*row_ptr);
		    cp_max_dom--;
		    if (is_double)
			cp_max_dom--;
		}
		else if ((cp_max_dom == 0) ||
		    (is_double && (cp_max_dom == 1)))
		{
		    cp_max_dom = -1;
		    copy_blk->cp_truncated++;
		}

		if (CMcmpcase(cbuf, copy_map->cp_delim) == 0)
		    break;

		if (cp_max_dom > 0)
		{
		    copy_map->cp_fieldlen++;
		    if (is_double)
			copy_map->cp_fieldlen++;
		}
	    }
	}
	/*
	** Assume data fit into the row buffer, will character
	** data be truncated when copied to the column?  If data
	** matches the null string, no need to display a truncation
	** warning.
	*/
	if ((cp_max_dom > 0) && (txt_count > cp_max_col))
	{
	    if (!IIcp_isnull(copy_map, startdom_ptr))
	        copy_blk->cp_truncated++;
	}
    }
    else
    {
	/* Set maximum domain size */
	cp_max_dom = DB_GW3_MAXTUP;
	copy_map->cp_fieldlen = 0;
	startdom_ptr = *row_ptr;
	for (;;)
	{
	    cp_escaped_char = FALSE;
	    status = IIcpgetch(copy_blk, cbuf, &is_double, i,
			copy_map->cp_fieldlen == 0, get_row);
	    if (status != OK)
	    	return status;

	    /* 
	    ** If this is an 'escape' char (single byte), then get
	    ** the next char and remember not to compare it against
	    ** the delimiter.
	    */ 
	    if ((cbuf[0] == CPY_ESCAPE) &&
		((copy_map->cp_rowtype == DB_CHR_TYPE) ||
		 (copy_map->cp_rowtype == CPY_DUMMY_TYPE)))
	    {
		status = IIcpgetch(copy_blk, cbuf, &is_double, i, FALSE,
				   get_row);
		cp_escaped_char = TRUE;
	    }

	    if (status != OK)
	    	return status;

	    /* 
	    ** Put this char in the row buffer unless it is already
	    ** full.  If full, incremented the truncated count and
	    ** just throw the character away.
	    */ 
	    if ((cp_max_dom > 0) &&
		(!is_double || (cp_max_dom > 1)))
	    {
		CMcpychar(cbuf, *row_ptr);
		CMnext(*row_ptr);
		cp_max_dom--;
		if (is_double)
		    cp_max_dom--;
	    }
	    else if ((cp_max_dom == 0) ||
		(is_double && cp_max_dom == 1))
	    {
		cp_max_dom = -1;
		copy_blk->cp_truncated++;
	    }

	    if (!cp_escaped_char)
	    {
		/*
		** If there is a delimiter, check for it.  If no
		** delimiter specified, check for default delimiters.
		*/
		if (copy_map->cp_isdelim)
		{
		    /*
		    ** cp_isdelim is -1 for char(0) and c0 formats
		    ** that don't have delimiters specified because
		    ** these are documented to have default delimiters
		    ** of tab, comma, or newline.
		    */ 
		    if (copy_map->cp_isdelim > 0)
		    { 
			if (CMcmpcase(cbuf, copy_map->cp_delim) == 0)
			{
			    /* filter ctrl <M> from the column data if it was  
			    generated from Windows */
#ifndef NT_GENERIC
			    if (CMcmpcase(copy_map->cp_delim,"\n") == 0)
			    {
			    	IIrm_ctrl_m(copy_map,&(*row_ptr),startdom_ptr);
			    }
#endif
			    break;
			}
		    } 
		    else
		    { 
			if ((CMcmpcase(cbuf, "\t") == 0) ||
			    (CMcmpcase(cbuf, ",")  == 0))
			{
			    break;
			}
			else
			    if (CMcmpcase(cbuf, "\n") == 0)
			    {
#ifndef NT_GENERIC
                                IIrm_ctrl_m(copy_map,&(*row_ptr),startdom_ptr);
#endif
				break;
			    }
		    } 
		}
	    }

	    if (cp_max_dom > 0)
	    {
		copy_map->cp_fieldlen++;
		if (is_double)
		    copy_map->cp_fieldlen++;
	    }
	} /* End for(;;) */
	copy_map->cp_adf_fnblk.adf_1_dv.db_length = copy_map->cp_fieldlen;
	/*
	** Assume data fit into the row buffer, will character
	** data be truncated when copied to the column?  If data
	** matches the null string, no need to display a truncation
	** warning.
	*/
	if ((cp_max_dom > 0) && (copy_map->cp_fieldlen > cp_max_col))
	{
	    if (!IIcp_isnull(copy_map, startdom_ptr))
	        copy_blk->cp_truncated++;
	}
    }
    return (OK);
}

/*{
+*  Name: IIcpputrow - write out copy row during COPY INTO.
**
**  Description:
**	If this is a COPY INTO PROGRAM, this routine calls the user supplied
**	"put_row" routine to dispose of the copy row.
**
**	If this is a normal copy, the row is written to the copy file.  The
**	copy file must be opened for writing.
**
**  Inputs:
**	copy_blk	- copy control block
**	row		- points to copy row
**	put_row		- user supplied routine to send row to (if PROGRAM copy)
**
**  Outputs:
**	Returns: OK, FAIL
-*
**  Side Effects:
**	none
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
*/
 
i4
IIcpputrow(copy_blk, row, put_row)
II_CP_STRUCT	*copy_blk;
PTR		row;
i4		(*put_row)();
{
    STATUS	status;
    i4	num_put;
    i4		num_written;
 
    /*
    ** Write row to copy file.
    **
    ** If this is an in-memory copy, just send this row to the user-specified
    ** output routine.
    */
    if (copy_blk->cp_program)
    {
	status = (*put_row)(&copy_blk->cp_row_length, row, &num_put);
	if (status != OK)
	{
	    char err_buf[15];
	    CVla(copy_blk->cp_rowcount + 1, err_buf);
	    IIlocerr(GE_LOGICAL_ERROR, E_CO003D_PROG_WRITE_ERR, II_ERR,
		     1, err_buf);
	    copy_blk->cp_warnings++;
	    return (FAIL);
	}
    }
    else 
    {
	status = SIwrite(copy_blk->cp_row_length, row, &num_written,
	    copy_blk->cp_file_ptr);
	if ((status != OK) || (num_written != copy_blk->cp_row_length))
	{
	    char err_buf[15];
	    CVla(copy_blk->cp_rowcount + 1, err_buf);
	    IIlocerr(GE_LOGICAL_ERROR, E_CO003B_FILE_WRITE_ERR, II_ERR,
		     1, err_buf);
	    copy_blk->cp_warnings++;
	    return (FAIL);
	}
    }
 
    return (OK);
}

/*{
+*  Name: IIcpfclose - Close copy file
**
**  Description:
**	Closes copy file.
**	Zero's out the cp_file_ptr in the copy control block.
**
**  Inputs:
**	copy_blk	- copy control block
**
**  Outputs:
**	Returns: OK, FAIL
-*
**  Side Effects:
**	none
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
*/
 
i4
IIcpfclose(copy_blk)
II_CP_STRUCT	*copy_blk;
{
    i4		status;
 
    status = SIclose(copy_blk->cp_file_ptr);
    if (status)
    {
	IIlocerr(GE_HOST_ERROR, E_CO0031_FILE_CLOSE_ERR, II_ERR,
		 1, copy_blk->cp_filename);
	copy_blk->cp_warnings++;
	return (FAIL);
    }
 
    copy_blk->cp_file_ptr = NULL;
    return (OK);
}

/*{
+*  Name: IIcpgetch - Get single or double byte character 
**
**  Description:
**	Returns a character in buffer passed in by caller and indicates
**	whether single or double byte.  Makes use of IIcpgetbytes to handle
**	reading from the file (or user-buffer in a COPY FROM PROGRAM)
**
**  Inputs:
**	copy_blk	- copy control block
**	i		- Domain index for copy_map
**	start_domain	- TRUE if this is first request for char in this domain
**	get_row		- user supplied input routine for COPY FROM PROGRAM
**
**  Outputs:
**	ch_ptr		- buffer in which to place char
**	is_double	- Set to TRUE if char is double-byte; otherwise single
**
**  Returns:
**	OK, or other status returned from IIcpgetbytes
**
**  History:
**	29-mar-1991 (barbara)
**	    Written to support COPY FROM PROGRAM and for general cleanup.
**	11-apr-1991 (barbara)
**	    Added 'start_domain' argument.  Another clue as to whether EOF
**	    is an error.
**	27-jan-2004 (somsa01)
**	    Added "COPY FROM/INTO... PROGRAM" case for BLOBs.
*/
STATUS
IIcpgetch(copy_blk, ch_ptr, is_double, i, start_domain, get_row)
II_CP_STRUCT	*copy_blk;
char		*ch_ptr;
bool		*is_double;
i4		i;
bool		start_domain;
i4		(*get_row)();
{
    STATUS	status;

    *is_double = FALSE;
    status = IIcpgetbytes(copy_blk, ch_ptr, 1, i, start_domain, get_row);
    if (status != OK)
	return status;

    if (CMdbl1st(ch_ptr))
    {
	status = IIcpgetbytes(copy_blk, &ch_ptr[1], 1, i, FALSE, get_row);
	if (status != OK)
	    return status;
	*is_double = TRUE;
    }
    return OK;
}


/*{
+*  Name: IIcpgetbytes - get bytes from file or user program
**
**  Description:
**	Returns the requested number of bytes to the user in a buffer.  These
**	bytes are read from the copy file ('cp_file_ptr') or from a buffer
**	that has been read in from the user program ('cp_usrrow.rowp').  If
**	the requested number of bytes cannot be copied into 'bytes_ptr'
**	and this is not the beginning of a row (copy from file only), an
**	error is issued.
**
**	Note: A future improvement is to do one SIgetrec instead of
**	SIgetc's for every character in a variable length field.  We
**	could assume that the maximum length record is DB_GW3_MAXWIDTH
**	because that is the length given to SIfopen on variable length rows.
**
**  Inputs:
**	copy_blk	- copy control block
**	num_req		- Requested number of bytes to be copied
**	i		- Domain index to copy_map
**	start_domain	- TRUE if this is first request for bytes in this domain
**	get_row		- user supplied input routine for COPY FROM PROGRAM
**
**  Outputs:
**	bytes_ptr	- buffer in which to place bytes
**	count		- Number of bytes of this char
**
**  Returns:
**	OK, FAIL, ENDFILE 
**
**  History:
**	29-mar-1991 (barbara)
**	    Written to support COPY FROM PROGRAM and for general cleanup.
**	11-apr-1991 (barbara)
**	    Added 'start_domain' argument.  Another clue as to whether EOF
**	    is an error.
**	04-apr-1997 (canor01)
**	    Windows NT must always open the file in binary mode
**	    when reading, and do the CRLF=>LF translation at
**	    character read time to prevent ^Z character from
**	    causing EOF.
**	25-apr-1997 (somsa01)
**	    Due to the previous change, the "status" variable was not being
**	    set, yet it was being checked. Thus, on success from the
**	    NT_GENERIC loop, set status=OK.
**	27-jan-2004 (somsa01)
**	    Added "COPY FROM/INTO... PROGRAM" case for BLOBs.
*/
STATUS
IIcpgetbytes(copy_blk, bytes_ptr, num_req, i, start_domain, get_row)
II_CP_STRUCT	*copy_blk;
char		*bytes_ptr;
i4		num_req;
i4		i;
bool		start_domain;
i4		(*get_row)();
{
    STATUS	status;
    i4		num_read = 0;
    i4		c;
    i4		usr_count;
    II_CP_MAP	*copy_map = &copy_blk->cp_copy_map[i];

    if (copy_blk->cp_program)
    {
	if (copy_blk->cp_has_blobs)
	{
	    status = (*get_row)(&num_req, bytes_ptr, &num_read);
	    if (status != OK || num_read != num_req)
		goto read_error;

	    return(OK);
	}
	else
	{
	    char err_buf[15];

	    usr_count = copy_blk->cp_usrrow.u_rowbuf +
			copy_blk->cp_usrrow.u_rowlen -
			copy_blk->cp_usrrow.u_rowp;
	    if (usr_count >= num_req)
	    {
		MECOPY_VAR_MACRO(copy_blk->cp_usrrow.u_rowp,
				 (u_i2)num_req, bytes_ptr);
		copy_blk->cp_usrrow.u_rowp += num_req;
		return OK;
	    }

	    CVla(copy_blk->cp_rowcount + 1, err_buf);
	    IIlocerr(GE_DATA_EXCEPTION, E_CO0040_END_OF_DATA, II_ERR, 2,
		     copy_map->cp_name, err_buf);
	    return FAIL;
	}
    }

    /* Get bytes from file */
    if (num_req > 1)
    {
# ifdef NT_GENERIC
	/*
	** On a READ, we always open the file as binary, since
	** text files will read ^Z as EOF.  However, this
	** means we must do the CRLF=>LF translation here.
	*/
	if ( copy_blk->cp_filetype == SI_TXT )
	{
	    for ( num_read = 0; num_read < num_req; num_read++ )
	    {
	        c = SIgetc( copy_blk->cp_file_ptr );
		if ( c == EOF )
		    goto read_error;
		bytes_ptr[num_read] = (char) c;
	        if ( c == '\r' )
	        {
		    c = SIgetc( copy_blk->cp_file_ptr );
		    if ( c == '\n' )
		        bytes_ptr[num_read] = (char) c;
		    else
		        SIungetc( c, copy_blk->cp_file_ptr );
	        }
	    }
	    status = OK;
	}
	else
# endif /* NT_GENERIC */
	    status = SIread( copy_blk->cp_file_ptr, num_req, 
			     &num_read, bytes_ptr );
	if (   status != OK
	    || num_read != num_req)
	    goto read_error;
	return OK;
    }
    else	/* Assume that we are to read 1 byte */
    {
	c = SIgetc(copy_blk->cp_file_ptr);
	if (c == EOF)
	    goto read_error;
# ifdef NT_GENERIC
	/*
	** On a READ, we always open the file as binary, since
	** text files will read ^Z as EOF.  However, this
	** means we must do the CRLF=>LF translation here.
	*/
	if ( copy_blk->cp_filetype == SI_TXT && c == '\r' )
	{
	    i4  save_char = c;

	    c = SIgetc(copy_blk->cp_file_ptr);
	    if ( c != '\n' )
	    {
	        SIungetc( c, copy_blk->cp_file_ptr );
		c = save_char;
	    }
	}
# endif /* NT_GENERIC */
	bytes_ptr[0] = (char)c;
	return OK;
    }

read_error:

    /*
    ** An EOF was read (or a read failed):
    ** EOF's are an error if this is not the first domain of a
    ** row (i != 0) or if we are "mid-way" into reading a domain, e.g
    ** we've read the data but not the terminator (!start_domain) or if
    ** we read some bytes but not the number requested (num_read).
    */
    if (i != 0 || !start_domain || num_read)
    {
	char err_buf[15];
	CVla(copy_blk->cp_rowcount + 1, err_buf);
	IIlocerr(GE_DATA_EXCEPTION, E_CO0040_END_OF_DATA, II_ERR, 2,
		copy_map->cp_name, err_buf);
	return FAIL;
    }
    return ENDFILE;
}

/*{
+*  Name: IIcpdbwrite - Send a tuple to the DBMS during COPY FROM.
**
**  Description:
**	Puts the tuple into communication block for sending to the DBMS.  Set
**	field in copy control block to remember that this block is not
**	empty and should be flushed later.
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**
**  Outputs:
**	cpy_bufh
**	    .cpy_bytesused  - updated.
**	Returns: OK, FAIL
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	10-sep-87 (puree)
**	    convert to GCF architecture.
*/
i4
IIcpdbwrite( II_LBQ_CB *IIlbqcb, II_CP_STRUCT *copy_blk, II_CPY_BUFH *cpy_bufh )
{
    IICGC_DESC		*cgc = IIlbqcb->ii_lq_gca;
    char		*bptr = (char *)(cpy_bufh->cpy_dbv.db_data);
    char		*tptr = (char *)(copy_blk->cp_tupbuf);
    i4		tuplen = copy_blk->cp_tup_length;
    i4		buflen = cpy_bufh->cpy_dbv.db_length;
    i4		bytescopy;
    bool		eom;
    /*
    ** If the GCA message buffer is smaller than a tuple, the tuple has
    ** been built in the tuple buffer(copy_blk->cp_tupbuf).  We have to
    ** copy it into GCA buffer and send it out.
    */
    if (buflen < tuplen)
    {
	while (tuplen)
	{
	    bytescopy = min(buflen, tuplen);
	    MEcopy(tptr, (u_i2)bytescopy, bptr);
	    tptr += bytescopy;
	    tuplen -= bytescopy;
	    eom = (tuplen == 0);
	    if (IIcgcp3_write_data(cgc, eom, bytescopy, &cpy_bufh->cpy_dbv)
			!= OK)
		return(FAIL);
	}
	cpy_bufh->cpy_bytesused = (i4)0;
	copy_blk->cp_not_flushed = FALSE;
    }
    else
    {
	/*
	** The GCA buffer is bigger than, or equal to a tuple.  We already put
	** the tuple directly into the message block - just need to increment
	** the 'bytesused' field.  If the remaining space in the message buffer
	** is not enough for a new tuple, send the message block to the DBMS
	** and reset the appropriate fields.
	*/
	cpy_bufh->cpy_bytesused += tuplen;
 
	if ((cpy_bufh->cpy_bytesused + tuplen) > cpy_bufh->cpy_dbv.db_length)
	{
	    if (IIcgcp3_write_data(cgc, TRUE, cpy_bufh->cpy_bytesused,
		    &cpy_bufh->cpy_dbv) != OK)
		return(FAIL);
 
	    cpy_bufh->cpy_bytesused = (i4)0;
	    copy_blk->cp_not_flushed = FALSE;
	}
	else
	{
	    copy_blk->cp_not_flushed = TRUE;
	}
    }
    return (OK);
}

/*{
+*  Name: IIcpdbread -	Read a tuple from the DBMS during COPY INTO.
**
**  Description:	This routine reads the next tuple from the GCA
**			message buffer.  If the GCA buffer contains a
**			full tuple, the tuple pointer is set directly into
**			The GCA buffer.  If only a part of tuple is found
**			in the GCA buffer, the routine saves that part of 
**			tuple in copy_blk->cp_tupbuf and reads the next
**			GCA message block.  The rest of the tuple is then
**			concatenated to the part saved away.  In this case,
**			the caller tuple pointer is set to cp_tupbuf.
**
**			Notice that a tuple may span multiple GCA messages.
**			The reverse is also true.
**	29-nov-91 (seg)
**	    Can't dereference or do arithmetic on PTR
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**	tuple		- addr of tuple buffer pointer
**
**  Outputs:
**	*tuple		- set to the next tuple buffer
**
**	Returns: OK, ENDFILE
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
**	10-sep-87 (puree)
**	    convert to GCF architecture.
**	15-jun-90 (barbara)
**	    IIcgcp5_read_data will return GCA_RESPONSE even when it really means
**	    that the association failed.
**      14-jun-96 (loera01)
**          Modified so that decompression is performed in LIBQ, rather than in
**          LIBQGCA. 
**      13-aug-96 (loera01)
**          Fixed memory leaks in compressed varchar case by using tptr, which
**          is pre-allocated, and deallocated at the end of the copy function. 
**	3-dec-1997 (nanpr01)
**	    bug 87198 : partial tuples are not handled correctly.
**      27-may-1998 (wanya01)
**          bug 90926 : for uncompressed data, if tuple size is wide enough 
**          and need to span more than one GCA message, complete tuple can 
**          not be returned to calling function correctly.  
**	22-feb-1999 (nicph02)
**	    Bug 94410: Rewrote the code that retrieves compressed tuples which
**	    spans more that one GCA message. Also documented it.
**	11-aug-1999 (hayke02)
**	    Modified fix for bug 94410 so that when bytesleft < tuplen and
**	    support_compression is true (vch_compression ON)
**	    bufh->cpy_bytesused is correctly updated for non-partial tuples
**	    (ie. full uncompressed tuples). Also removed two unnecessary
**	    assignments of bytesleft. This change fixes bug 98334.
**	21-Dec-2001 (gordy)
**	    IICPdecompress() now takes a ROW_DESC instead of GCA tuple desc.
*/
i4
IIcpdbread( II_LBQ_CB *IIlbqcb, 
	    II_CP_STRUCT *copy_blk, II_CPY_BUFH *cpy_bufh, PTR *tuple)
{
    IICGC_DESC		*cgc = IIlbqcb->ii_lq_gca;
    II_CPY_BUFH		*bufh = cpy_bufh;
    ROW_DESC		*rdesc;
    i4		tuplen = copy_blk->cp_tup_length;
    i4			msg_type;
    i4		bytesleft;
    i4		bytescopy;
    char		*bptr;
    char		*tptr = (char *)(copy_blk->cp_tupbuf);
    i4		tcount = 0;
    bool		support_compression;
    i4			dif;
    bool                partial;
    PTR                 loc_mem = NULL;

    /*
    ** Get data for the next tuple from the DBMS.
    */
    rdesc = &IIlbqcb->ii_lq_retdesc.ii_rd_desc;
    support_compression = rdesc->rd_flags & RD_CVCH ? TRUE : FALSE;

    for (;;)		/* loop until a complete tuple is received */
    {
	bptr = (char *)bufh->cpy_dbv.db_data + bufh->cpy_bytesused;
	bytesleft = bufh->cpy_dbv.db_length - bufh->cpy_bytesused;

	if (bytesleft == 0)
	{   /* Time to read a new message buffer */
            msg_type = IIcgcp5_read_data(cgc, &bufh->cpy_dbv);
	    if (msg_type == GCA_RESPONSE)	/* a RESPONSE block received */
	    {
		if ( ! (IIlbqcb->ii_lq_flags & II_L_CONNECT) )
		/* msg_type might be faked by IIcgcp5_read_data */
		    return FAIL;
		return (ENDFILE);
	    }
 
	    bptr = (char *)bufh->cpy_dbv.db_data;
	    bytesleft = bufh->cpy_dbv.db_length;
	    bufh->cpy_bytesused = 0;
	}
 
	if ((bytesleft < tuplen) && (!support_compression))
	{   /* Partial tuple left in GCA buffer */
	    /* Copy the partial tuple into our own buffer */
 
	    bytescopy = min(bytesleft, (tuplen - tcount));
	    MEcopy(bptr, (u_i2)bytescopy, tptr);
 
	    tptr += bytescopy;
	    tcount += bytescopy;
	    bufh->cpy_bytesused += bytescopy;
 
	    if (tcount == tuplen)
	    {
		*tuple = copy_blk->cp_tupbuf;
		break;				/* got a complete tuple */
	    }
	}
        else if ((bytesleft < tuplen) && (support_compression))
        {
            /* This code retrieves compressed tuples that span several 
	       GCA messages:
	       1- Copy the current GCA packet (bptr) to the reglued tuple (tptr)
	       2- Decompress this tuple: If it needs more GCA messages, 
		  it will be marked as partial.
	       3- If partial, then amend tptr else 
	          copy out the FULL DECOMPRESSED tuple.
	    */

	    if (loc_mem == NULL)
	    {
		if ( (loc_mem = MEreqmem(0, tuplen, 0, NULL)) == NULL)
		   return(FAIL);
	    }

	    /* A new GCA packet to glue at the end of the tuple pointer */
	    bytescopy = min(bytesleft, (tuplen - tcount));
	    MEcopy(bptr, (u_i2)bytescopy, tptr);

	    IICPdecompress( rdesc, copy_blk->cp_tupbuf, tuplen,
			    tcount+bytescopy, &dif, &loc_mem, &partial );

	    if (partial || tcount)
	    {
		tcount += bytescopy; /* records number of bytes copied so far */
		bufh->cpy_bytesused += bytescopy;
	    }
	    else
		bufh->cpy_bytesused += tuplen - dif;

            /* Tuple is not complete yet */
            if (partial)
                tptr += bytescopy;
	    else
	    {
		/* The actual decompressed tuple shoule be complete */
		if (tcount && (tcount + dif != tuplen))
		   return(FAIL);

		/* Copy out decompressed buffer */
		MEcopy(loc_mem, tuplen, copy_blk->cp_tupbuf);
		*tuple = copy_blk->cp_tupbuf;
		MEfree(loc_mem);
		break;
	    }
	}
	else
	{   /* one or more tuples in GCA buffer */
	    if (tcount == 0)
	    {	/* no partial tuple in our buffer */
                if (support_compression)
                {
                    IICPdecompress( rdesc, bptr, tuplen, 
				    bytesleft, &dif, &tptr, &partial );
                    *tuple = copy_blk->cp_tupbuf;
   	            bufh->cpy_bytesused += tuplen - dif;
                }
                else
                {
   		    *tuple = (char *)bufh->cpy_dbv.db_data + bufh->cpy_bytesused;
		    bufh->cpy_bytesused += tuplen;
                }
		break;				/* got a complete tuple */
	    }
	    else
	    {	/* have to fill up the partial tuple */
 
                if (support_compression)
                {
		    bytescopy = tuplen - tcount - dif;
                }
                else
                {
		    bytescopy = tuplen - tcount;
                }
		MEcopy(bptr, (u_i2)bytescopy, tptr);
 
		tptr += bytescopy;
		tcount += bytescopy;
		bytesleft -= bytescopy;
		bufh->cpy_bytesused += bytescopy;
 
		*tuple = copy_blk->cp_tupbuf;
		break;				/* got a complete tuple */
	    }
	}	    
    }
    return (OK);
}
 
 

/*{
+*  Name: IIcpendcopy - Perform end-of-copy protocol.
**
**  Description:
**	At finish of COPY, print out warning messages for following:
**	    - non-printable chars converted to blanks
**	    - copy domains that had to be truncated
**	    - rows that could not be processed
**	    - bad rows written to log file
**	    - rows not copied because duplicate key data found
**
**	If COPY finished successfully, but with warnings, give the ending
**	message to that effect.  If the COPY finished unsuccessfully, the
**	DBMS will generate the ending message.
**
**	If this is a COPY FROM and there are no errors, flush all the tuples
**	in the communication buffer with the EOM option and send a response
**	message to the DBMS.
**
**	Call IIcpcleanup to clean up memory and files.
**
**  Error Conditions Handling:
**	COPY INTO:
**	    1.  Errors from FE:
**		    - cp_error set by the calling routine.
**		    - report warning/error messages.
**		    - send interrupt query to DBMS if rollback is enabled.
**		    - send reponse message to DBMS if rollback is disabled. 
**		    - clean up.
**
**	    2. Errors from DBMS: (Message from the DBMS will be taken care
**	       of by LIBQ).
**		    - report FE warning messages.
**		    - clean up.
**		    
**	COPY FROM:
**	    1.  Errors from FE:
**		    - cp_error set by the calling routine.
**		    - report warning/error messages.
**		    - send interrupt query to DBMS
**		    - clean up.
**
**	    2. Errors from DBMS: (Message from the DBMS will be taken care
**	       of by LIBQ).
**		    - cp_dbmserr set by the error polling routine.
**		    - IIcpendcopy is called by IIcopy.
**		    - report warning messages.
**		    - clean up.
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**
**  Outputs:
**	Returns: none
-*
**  Side Effects:
**	none
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
**	10-sep-87 (puree)
**	    convert to GCF architecture.
**	17-nov-87 (puree)
**	    fixed rollback option and clean up error handling.
**	07-sep-88 (sandyh)
**	    fixed DBMS error handling to prevent GCA_RESPONSE
**	    from being sent in DBMS error condition.
**	26-feb-1990 (barbara)
**	    Don't turn off IICGC_COPY_MASK up here because IIcgc_read_results
**	    will test for that mask as part of fix to bug #7927.
**	05-apr-90 (teresal)
**	    Fix for bug 8725 so warning messages show correct row count for
**	    number of rows copied. Rowcount no longer incremented by one.
**	15-jun-90 (barbara)
**	    LIBQ now continues after association failures (bug 21832).  Make
**	    sure we don't attempt to read/write from/to the dbms.
**	13-dec-90 (teresal)
**	    Bug fix 34405 - read response block to detect if rows with
**	    duplicate keys were dropped by the dbms.
**	22-may-97 (hayke02)
**	    Tighten up the check for duplicate rows (E_CO003F) - the
**	    number of rows copied must be greater than zero. If zero rows
**	    were copied (probably due to running out of disk space during
**	    a copy into a non heap table), return E_CO0029. This change
**	    fixes bug 80478.
**      21-Oct-98 (wanya01)
**          X-integrate change 436382
**          Added new error message (E_CO0048_ALLDUPS_OR_DISKFULL) to
**          replace the E_CO0029 added by the previous change. This new
**          error message will inform the user that 0 rows were copied
**          because either all rows were duplcates or there was a disk
**          full problem. This change fixes bug 90968.
*/
VOID
IIcpendcopy(II_LBQ_CB *IIlbqcb, II_CP_STRUCT *copy_blk, II_CPY_BUFH *cpy_bufh)
{
    IICGC_DESC		*cgc = IIlbqcb->ii_lq_gca;
    GCA_RE_DATA		gca_rsp;
    char		num1_buf[20];
    char		num2_buf[20];
    bool		read_resp;		/* Read dbms response ? */
 
    /*
    ** 0.   If there has been an association failure, we just
    **	    	want to clean up and get out.
    */
    if ( ! (IIlbqcb->ii_lq_flags & II_L_CONNECT) )
    {
	IIcpcleanup(copy_blk);
	return;
    }

    /*
    **  1.  Report warning messages:
    **      1.a)  non-printing characters converted to blanks.
    */
    if ((copy_blk->cp_blanked) &&
	( (!copy_blk->cp_dbmserr) || copy_blk->cp_direction == CPY_INTO))
    {
	CVla(copy_blk->cp_blanked, num1_buf);
	IIlocerr(GE_DATA_EXCEPTION, E_CO0014_BLANKED_CHARS, II_ERR,
		 1, num1_buf);
	copy_blk->cp_warnings++;
    }
 
    /*	    1.b)  copy domains that had to be truncated */
 
    if ((copy_blk->cp_truncated) &&
	( (!copy_blk->cp_dbmserr) || copy_blk->cp_direction == CPY_INTO))
    {
	CVla(copy_blk->cp_truncated, num1_buf);
	IIlocerr(GE_DATA_EXCEPTION, E_CO0015_TRUNCATED_CHARS, II_ERR,
		 1, num1_buf);
	copy_blk->cp_warnings++;
    }
 
    /*
    **	    1.c)  copy rows that could not be processed.  if the rows were
    **	        logged, then say so - otherwise just give generic message.
    */
    if (copy_blk->cp_logged)
    {
	CVla(copy_blk->cp_logged, num1_buf);
	IIlocerr(GE_LOGICAL_ERROR, E_CO002C_LOGGED, II_ERR, 2, num1_buf,
		copy_blk->cp_logname);
	copy_blk->cp_warnings++;
    }
    else if (copy_blk->cp_badrows)
    {
	CVla(copy_blk->cp_badrows, num1_buf);
	IIlocerr(GE_LOGICAL_ERROR, E_CO002B_BADROWS, II_ERR, 1, num1_buf);
	copy_blk->cp_warnings++;
    }

    /*
    **  2. If COPY FROM and have not received an error from the DBMS, flush
    **     the remaining tuples to the DBMS even if we may have errors in
    **	   the front end.
    */
    if ((copy_blk->cp_direction == CPY_FROM) && (!copy_blk->cp_dbmserr))
    {
	if (copy_blk->cp_not_flushed)
	{
	    if (IIcpdbflush( IIlbqcb, copy_blk, cpy_bufh ) != OK)
		copy_blk->cp_error = TRUE;
	}
	if (IIcgcp4_write_read_poll(cgc, GCA_CDATA) != GCA_CONTINUE_MASK)
	{
	    copy_blk->cp_dbmserr = TRUE;
	}
    }
    /*
    **  3.  Take care of error situation, if any.  Keep track 
    **	    if an interrupt has been sent so we know if a 
    **	    response block needs to be read.
    **
    **	3.a For COPY INTO, if an error is detected by the front end, send
    **	    an interrupt block to the DBMS.
    */
    read_resp = TRUE;	/* Assume DBMS will send back a response */
    if (copy_blk->cp_direction == CPY_INTO)
    {
	if (copy_blk->cp_error)
	{
	    IIcgc_break(cgc, GCA_INTALL);
	    CVla((copy_blk->cp_rowcount - copy_blk->cp_badrows), num1_buf);
	    IIlocerr(GE_LOGICAL_ERROR, E_CO0029_COPY_TERMINATED, II_ERR,
			1, num1_buf);
	    read_resp = FALSE;	/* DBMS won't send back a response */
	}
    }
    /*
    **  3.b For COPY FROM, we will send an interrupt message to the DBMS
    **	    if there is an error detected in the front end AND the rollback
    **	    option is enabled.  If an error is detected but rollback is
    **	    disable, we will issue user error message (later on) but
    **	    send a response message to the DBMS as if COPY is successfully 
    **	    completed. 
    */
    else
    {
	gca_rsp.gca_rqstatus = GCA_OK_MASK;
	gca_rsp.gca_rowcount = copy_blk->cp_rowcount;
 
	if (copy_blk->cp_error)
	{
	    if (copy_blk->cp_status & CPY_NOABORT)
	    {
		IIcgcp2_write_response(cgc, &gca_rsp);
	    }
	    else
	    {
		IIcgc_break(cgc, GCA_INTALL);
		IIlocerr(GE_LOGICAL_ERROR, E_CO002A_COPY_ABORTED, II_ERR,
			 0, (char *)0);
		read_resp = FALSE;	/* DBMS won't send back a response */
	    }
	}
	else if (!copy_blk->cp_dbmserr)	/* DBMS ready to receive response */
	{
	    IIcgcp2_write_response(cgc, &gca_rsp);
	}
    }
    /*	4.  Read response from DBMS to get accurate row count */

    /*	4.a The DBMS won't send a response block for an IIbreak so
    **	    don't bother reading if an IIbreak was called.
    */

    copy_blk->cp_rowcount -= copy_blk->cp_badrows; /* # rows sent by COPY */

    if (read_resp && !copy_blk->cp_dbmserr)
    {
	/* If response block has not been read, don't compare rowcount */	
	if (IIcgc_read_results(cgc, TRUE, GCA_RESPONSE) == 0)
	{
	    /*	
	    **  Check response block - if the actual number of rows
	    **  added by the DBMS was less than the number of rows sent
	    **  by COPY, it is because those rows not copied contain
	    **  duplicate key data.  A warning message is displayed
	    **  for SQL only. (Bug 34405)
	    */
	    if ((cgc->cgc_resp.gca_rowcount > 0) &&
		(cgc->cgc_resp.gca_rowcount < copy_blk->cp_rowcount))
	    {
		if (IIlbqcb->ii_lq_dml == DB_SQL)
		{
		    /* Calculate no. duplicate rows found */
		    copy_blk->cp_rowcount -= cgc->cgc_resp.gca_rowcount; 
		    CVla(copy_blk->cp_rowcount, num1_buf);
		    IIlocerr(GE_LOGICAL_ERROR, E_CO003F_DUPLICATES_FOUND, 
				II_ERR, 1, num1_buf);
		    copy_blk->cp_warnings++;
		}
	    /* DBMS rowcount is accurate # rows copied */
	    copy_blk->cp_rowcount = cgc->cgc_resp.gca_rowcount;
	    }
	    /*
            ** Bugs 80478, 90968: out of disk space during copy into a non-heap
            **                    table or all rows are duplicates.
	    */
	    if ((cgc->cgc_resp.gca_rowcount == 0) &&
		(copy_blk->cp_rowcount > 0) && (IIlbqcb->ii_lq_dml == DB_SQL))
	    {
		char err_buf[15];
		CVla(0, err_buf);
                IIlocerr(GE_LOGICAL_ERROR, E_CO0048_ALLDUPS_OR_DISKFULL, II_ERR,
			    1, err_buf);
	    }
	}
    }

    /*	5.a Display error message if rollback disabled and error in
    **	    COPY FROM.  Error message displayed here so
    **	    it will include the correct rowcount (Bug 34405).
    */
    if ((copy_blk->cp_direction == CPY_FROM) &&
	(copy_blk->cp_error) && (copy_blk->cp_status & CPY_NOABORT))
    {
	CVla((copy_blk->cp_rowcount), num1_buf);
	IIlocerr(GE_LOGICAL_ERROR, E_CO0029_COPY_TERMINATED, II_ERR,
		1, num1_buf);
    }

    /*	5.b Display count of warnings during COPY - Use correct
    **	    rowcount (Bug 34405)
    */
    if ((copy_blk->cp_warnings) && (!copy_blk->cp_error))
    {
	CVla(copy_blk->cp_warnings, num1_buf);
	CVla((copy_blk->cp_rowcount), num2_buf);
	IIlocerr(GE_LOGICAL_ERROR, E_CO0028_WARNINGS, II_ERR,
		 2, num1_buf, num2_buf);
    }
    /*
    **	6.  Clean up.
    */
    IIcpcleanup(copy_blk);
    /*
    **	7.  Turn off IN_COPY bit.
    **	    Note that this flag is now turned off in IIcgc_read_results.
    **	    (IIsyncup-->IIqry_end-->IIcgc_read_results)
    **	    This is part of fix to bug #7927.
    */
    /* cgc->cgc_g_state &= (~IICGC_COPY_MASK); */
 
    return;
}
 

/*{
+*  Name: IIcpcleanup - Clean up before exiting copy.
**
**  Description:
**	Clean up memory allocated for COPY, close files opened during COPY.
**	Turn off ULC bit that marks us as in the middle of a COPY.
**
**	This routine must be called before exiting COPY processing.  This
**	routine may be called multiple times with no bad effects (other than
**	performance).  This routine should be called on interrupts if we
**	will longjump out of copy processing.
**
**  Inputs:
**	copy_blk	- copy control block
**
**  Outputs:
**	Returns: none
-*
**  Side Effects:
**	none
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	10-sep-87 (puree)
**	    convert to GCF architecture.
**	21-Dec-01 (gordy)
**	    Free GCA tuple descriptor allocated by IIcptdescr().
**	24-Jun-2005 (schka24)
**	    Free up unicode normalization work bufs.
*/
VOID
IIcpcleanup(copy_blk)
II_CP_STRUCT	*copy_blk;
{
    II_THR_CB		*thr_cb = IILQthThread();
    II_LBQ_CB		*IIlbqcb = thr_cb->ii_th_session;
    IICGC_DESC		*cgc = IIlbqcb->ii_lq_gca;
    i4			i;
    II_CP_MAP		*cmap;

    /*
    ** Free GCA tuple descriptor
    */
    if ( cgc->cgc_tdescr )
    {
	MEfree((PTR) cgc->cgc_tdescr);
	cgc->cgc_tdescr = (GCA_TD_DATA *)NULL;
    }

    /*
    ** Close copy file if there is one.
    */
    if ((!copy_blk->cp_program) && (copy_blk->cp_file_ptr))
	_VOID_ IIcpfclose(copy_blk);
 
    /*
    ** Close log file if there is one.
    */
    if ((copy_blk->cp_rowlog) && (copy_blk->cp_log_ptr))
	_VOID_ IIcplgclose(copy_blk);
 
    /*
    ** Close debug file if there is one
    */
    if (copy_blk->cp_dbg_ptr)
	_VOID_ IIcpdbgclose(copy_blk);

    /*
    ** Free up memory allocated for copy structure.
    */
    if (copy_blk->cp_filename)
    {
	MEfree((PTR) copy_blk->cp_filename);
	copy_blk->cp_filename = NULL;
    }
    if (copy_blk->cp_copy_map)
    {
	for (i = 0; i < copy_blk->cp_num_domains; i++)
	{
	    cmap = &copy_blk->cp_copy_map[i];
	    if (cmap->cp_normbuf != NULL)
		MEfree(cmap->cp_normbuf);
	    if (cmap->cp_srcbuf != NULL)
		MEfree(cmap->cp_srcbuf);
	}
	MEfree((PTR) copy_blk->cp_copy_map);
	copy_blk->cp_copy_map = NULL;
    }
    if (copy_blk->cp_zerotup)
    {
	MEfree((PTR) copy_blk->cp_zerotup);
	copy_blk->cp_zerotup = NULL;
    }
    if (copy_blk->cp_tupbuf)
    {
	MEfree((PTR) copy_blk->cp_tupbuf);
	copy_blk->cp_tupbuf = NULL;
    }
    if (copy_blk->cp_rowbuf)
    {
	MEfree((PTR) copy_blk->cp_rowbuf);
	copy_blk->cp_rowbuf = NULL;
    }
    if (copy_blk->cp_buffer)
    {
	MEfree((PTR) copy_blk->cp_buffer);
	copy_blk->cp_buffer = NULL;
    }
    if (copy_blk->cp_logname)
    {
	MEfree((PTR) copy_blk->cp_logname);
	copy_blk->cp_logname = NULL;
    }
 
    return;
}
 

/*{
+*  Name: IIcpdbflush - Flush output waiting in the communication block.
**
**  Description:
**	If there are tuples sitting in the communication block to send to the
**	DBMS, we need to flush them before ending the copy.
**
**	If we are ending the copy, mark this block as the last.
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- current status of copy buffer header	
**
**  Outputs:
**	Returns: OK, FAIL
-*
**  Side Effects:
**	none
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	10-sep-87 (puree)
**	    convert to GCF architecture.
*/
i4
IIcpdbflush(II_LBQ_CB *IIlbqcb, II_CP_STRUCT *copy_blk, II_CPY_BUFH *cpy_bufh)
{
    IICGC_DESC		*cgc = IIlbqcb->ii_lq_gca;
 
 
    if (IIcgcp3_write_data(cgc, TRUE, cpy_bufh->cpy_bytesused,
	    &cpy_bufh->cpy_dbv) != OK)
	return(FAIL);
 
    cpy_bufh->cpy_bytesused = (i4)0;
    copy_blk->cp_not_flushed = FALSE;
    return(OK);
}
 
 

/*{
+*  Name: IIcpabort - Abort COPY command during initialization phase.
**
**  Description:
**		    If a fatal error is detected when IIcopy receives
**		    the copy map from the DBMS, this routine is called
**		    to signal the DBMS to abort the COPY command.
**
**		    After sending the copy map to the front end,  the
**		    DBMS waits for an ACK block.  Instead of sending
**		    the ACK block, if IIcopy detects a fatal error, it
**		    send a RESPONSE block to the DBMS.  The DBMS aborts
**		    the COPY command and sends a RESPONSE block to the
**		    front end to go ahead with the next query.
**
**		    IIcpabort is reponsible for sending and reading the
**		    RESPONSE block to and from the DBMS for the purpose
**		    of aborting the COPY commands.
**
**  Inputs:
**	copy_blk	- copy control block
**
**  Outputs:
**	Returns: none
-*
**  Side Effects:
**	none
**
**  History:
**	24-sep-87 (puree)
**	    written for GCF architecture.
**	26-feb-1990 (barbara)
**	    Don't need to turn off IICGC_COPY_MASK because IIcgc_read_results
**	    now turns it off as part of fix to bug #7927
*/
VOID
IIcpabort( II_LBQ_CB *IIlbqcb, II_CP_STRUCT *copy_blk )
{
    IICGC_DESC		*cgc = IIlbqcb->ii_lq_gca;
    GCA_RE_DATA		gca_rsp;
 
 
 
    /* Free all memory acquired */
    IIcpcleanup(copy_blk);
 
    /* Format and send a RESPONSE message to the DBMS */
 
    gca_rsp.gca_rqstatus = GCA_FAIL_MASK;
    gca_rsp.gca_rowcount = GCA_NO_ROW_COUNT;
    IIcgcp2_write_response(cgc, &gca_rsp);
 
    /* Read and discard the corresponding RESPONSE message from the DBMS */
 
    while (IIcgc_read_results(cgc, TRUE, GCA_RESPONSE));
    
    /* Turn off IN_COPY bit */
 
    /* Mask will have been turned on in IIcgc_read_results */

    /* cgc->cgc_g_state &= (~IICGC_COPY_MASK); */
}
 
 

/*{
+*  Name: IIcplgopen - Open copy log file/
**
**  Description:
**	Open log file to be used for bad copy rows.  File is opened for
**	writing.
**
**	If COPY FROM, open file with same attributes as the copy file.
**	If COPY INTO, make it fixed lenght binary (it will hold tuples).
**
**  Inputs:
**	copy_blk	- copy control block
**
**  Outputs:
**	Returns: OK, FAIL
-*
**  Side Effects:
**	none
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
**	22-jan-90 (barbara)
**	    Fixed up call to LOfroms to use correct sized buffer.
*/
i4
IIcplgopen(copy_blk)
II_CP_STRUCT	*copy_blk;
{
    LOCATION	loc;
    char	locbuf[MAX_LOC +1];	/* Used to hold location string */
    i4	filewidth;
    i4		filetype;
 
    STcopy( copy_blk->cp_logname, locbuf );
    LOfroms(PATH & FILENAME, locbuf, &loc);
 
    if (copy_blk->cp_direction == CPY_FROM)
    {
	/*
	** Find out real attributes of copy file to create similiar one.
	*/
	/* SIgetattr(&filetype, &filewidth); */
 
	if (copy_blk->cp_row_length)
	{
	    filewidth = copy_blk->cp_row_length;
	    filetype = copy_blk->cp_filetype;
	}
	else
	{
	    filewidth = DB_GW3_MAXTUP;
	    filetype = SI_VAR;
	}
    }
    else 
    {
	filewidth = copy_blk->cp_tup_length;
	filetype = SI_BIN;
    }
 
    if (SIfopen(&loc, "w", filetype, filewidth,	&copy_blk->cp_log_ptr) != OK)
    {
	IIlocerr(GE_HOST_ERROR, E_CO0036_LOG_OPEN_ERR, II_ERR,
		 1, copy_blk->cp_logname);
	return (FAIL);
    }
 
    return (OK);
}

/*{
+*  Name: IIcplgrow - Write row to log file.
**
**  Description:
**	If copy row could not be processed, it is written to the log file.
**
**  Inputs:
**	copy_blk	- copy control block
**	row		- points to row to be logged.
**
**  Outputs:
**	Returns: none
-*
**  Side Effects:
**	none
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
*/
VOID
IIcplgrow(copy_blk, row)
II_CP_STRUCT	*copy_blk;
char		*row;
{
    STATUS	status;
    i4		num_written;
 
    status = SIwrite(copy_blk->cp_row_length, row, &num_written,
	copy_blk->cp_log_ptr);
    if ((status != OK) || (num_written != copy_blk->cp_row_length))
    {
	char err_buf[15];
	CVla(copy_blk->cp_rowcount + 1, err_buf);
	IIlocerr(GE_HOST_ERROR, E_CO002F_LOG_WRITE_ERR, II_ERR, 
		 2, copy_blk->cp_logname, err_buf);
    }
    copy_blk->cp_logged++;
}

/*{
+*  Name: IIcplgclose - Close copy log file.
**
**  Description:
**	Close the log file used to collect bad copy rows.
**	If no rows were written to log, then delete it.
**
**  Inputs:
**	copy_blk	- copy control block
**
**  Outputs:
**	Returns: OK, FAIL
-*
**  Side Effects:
**	none
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	04-sep-87 (puree)
**	    added severity flag to IIlocerr calls.
**	22-jan-90 (barbara)
**	    Fixed up call to LOfroms to pass correct sized buffer.
*/
i4
IIcplgclose(copy_blk)
II_CP_STRUCT	*copy_blk;
{
    LOCATION	loc;
    char	locbuf[MAX_LOC +1];	/* Used to hold location string */
    i4		status;
 
    status = SIclose(copy_blk->cp_log_ptr);
    if (status)
    {
	IIlocerr(GE_HOST_ERROR, E_CO0030_LOG_CLOSE_ERR, II_ERR,
		 1, copy_blk->cp_logname);
	copy_blk->cp_warnings++;
	return (FAIL);
    }
 
    copy_blk->cp_log_ptr = NULL;
    if (copy_blk->cp_logged == 0)
    {
	STcopy( copy_blk->cp_logname, locbuf );
	LOfroms(PATH & FILENAME, locbuf, &loc);
	LOdelete(&loc);
    }
 
    return (OK);
}
 

/*{
+*  Name: IIcp_handler - Handle exceptions during COPY.
**
**  Description:
**	This handler is declared at the start of copy processing.
**
**	It handles no exceptions on its own, it always passes signal on to
**	next handler.
**
**	If some other exception handler decides to longjump out of copy
**	processing, this routine will be called with EX_UNWIND.  In this
**	case we will call IIcpcleanup() to clean up the copy before
**	exiting.
**
**  Inputs:
**	exargs		- exception handler argument, gives reason for signal
**
**  Outputs:
**	Returns: EXRESIGNAL to pass exception on to next handler.
-*
**  Side Effects:
**	none
**
**  History:
**	01-dec-1986	- Written (rogerk).
**	27-aug-87 (puree)
**	    fix problem with control-C.
**	26-feb-1990 (barbara)
**	    Don't turn off IICGC_COPY_MASK up here because IIcgc_read_results
**	    will test for that mask as part of fix to bug #7927.
*/
STATUS
IIcp_handler(exargs)
EX_ARGS	*exargs;
{
    II_THR_CB	*thr_cb = IILQthThread();
    II_LBQ_CB	*IIlbqcb = thr_cb->ii_th_session;

    switch (exargs->exarg_num)
    {
	/*  If Control-C or unwound, clean up */
	case EXINTR:
	case EX_UNWIND:
	{
	    IIcpcleanup((II_CP_STRUCT *)thr_cb->ii_th_copy);
	    return (EXRESIGNAL);
	}
 
	/* For math exception */
	case EXFLTDIV:
	case EXFLTOVF:
	case EXFLTUND:
	case EXINTDIV:
	case EXINTOVF:
	case EXMNYDIV:
	case EXMNYOVF:
	case EXDECOVF:
	{
	    /* Check for ADF exception */
	 
	    IIlbqcb->ii_lq_adf->adf_errcb.ad_ebuflen = 0;
	    IIlbqcb->ii_lq_adf->adf_errcb.ad_errmsgp = (char *) NULL;
	    IIlbqcb->ii_lq_adf->adf_exmathopt = ADX_ERR_MATHEX;
	 
	    if (adx_handler(IIlbqcb->ii_lq_adf, exargs) != E_DB_OK)
	    {
		((II_CP_STRUCT *)thr_cb->ii_th_copy)->cp_status |= CPY_ERROR;
		return(EXCONTINUES);
	    }
	}
 
	/* For everything else, clean up and terminate copy */
	default:
	{
	    IIcpcleanup((II_CP_STRUCT *)thr_cb->ii_th_copy);
	    return (EXRESIGNAL);
	} 
    } 
}

/*{
+*  Name: IIcpdbgdump - Print out II_CP_STRUCT and II_CP_MAP structures
**
**  Description:
**
**  Inputs:
**	copy_blk	- The II_CP_STRUCT
**	copydbg_file	- name of file to dump structure info into.
**
**  Outputs:
-*
**  Side Effects:
**	Prints out data from the COPY data structures into the desired file.
**
**  History:
**	7-jan-1993 (mgw) Written
**	17-Jun-2004 (schka24)
**	    Avoid buffer overrun on filename.
*/
VOID
IIcpdbgdump( II_LBQ_CB *IIlbqcb, II_CP_STRUCT *copy_blk, char *copydbg_file )
{
	LOCATION	l;
	char		locbuf[MAX_LOC+1];
	FILE		*fp;
	char		buf[256];
	i4		i;
	i4		*attr_offset;
	ROW_DESC	*row_desc;		/* Row descriptor */

	/* open copydbg_file */
	STlcopy(copydbg_file, locbuf, sizeof(locbuf)-1);
	LOfroms(FILENAME, locbuf, &l);
	SIfopen(&l, "a", SI_TXT, 80, &fp);

	/* First print the query */
	SIfprintf(fp, "%s\n\n", IIlbqcb->ii_lq_gca->cgc_qry_buf.cgc_q_buf);

	/* Next, the structures */
	SIfprintf(fp, "II_CP_STRUCT copy_blk\n");
	SIfprintf(fp, "  cp_status (octal): %o\n", copy_blk->cp_status);
	if (copy_blk->cp_direction == CPY_FROM)
		STcopy("CPY_FROM", buf);
	else if (copy_blk->cp_direction == CPY_INTO)
		STcopy("CPY_INTO", buf);
	else
		STcopy("UNKNOWN", buf);
	SIfprintf(fp, "  cp_direction: %s\n", buf);
	SIfprintf(fp, "  cp_row_length: %d\n", copy_blk->cp_row_length);
	SIfprintf(fp, "  cp_tup_length: %d\n", copy_blk->cp_tup_length);
	SIfprintf(fp, "  cp_num_domains: %d\n", copy_blk->cp_num_domains);
	SIfprintf(fp, "  cp_rowcount: %d\n", copy_blk->cp_rowcount);
	SIfprintf(fp, "  cp_truncated: %d\n", copy_blk->cp_truncated);
	SIfprintf(fp, "  cp_blanked: %d\n", copy_blk->cp_blanked);
	SIfprintf(fp, "  cp_warnings: %d\n", copy_blk->cp_warnings);
	SIfprintf(fp, "  cp_badrows: %d\n", copy_blk->cp_badrows);
	SIfprintf(fp, "  cp_logged: %d\n", copy_blk->cp_logged);
	SIfprintf(fp, "  cp_maxerrs: %d\n", copy_blk->cp_maxerrs);
	SIfprintf(fp, "  cp_rbuf_size: %d\n", copy_blk->cp_rbuf_size);
	SIfprintf(fp, "  cp_filename: %s\n", copy_blk->cp_filename);
	SIfprintf(fp, "  cp_logname: %s\n", copy_blk->cp_logname);
	SIfprintf(fp, "  cp_sync_frequency: %d\n", copy_blk->cp_sync_frequency);
	if (copy_blk->cp_buffer != NULL)
	{
	    row_desc = &IIlbqcb->ii_lq_retdesc.ii_rd_desc;
	    attr_offset = (i4 *)copy_blk->cp_buffer;
	    SIfprintf(fp, "  cp_buffer (attr_offsets): ");
	    for (i = 0; i < row_desc->rd_numcols - 1; i++)
	    {
		SIfprintf(fp, "%3d, ", attr_offset[i]);
		if (i && i % 10 == 0)
		    SIfprintf( fp, "\n                            ");
	    }
	    SIfprintf(fp, "%d\n", attr_offset[i]);
	}
	SIfprintf(fp, "  cp_convert: %d\n", copy_blk->cp_convert);
	SIfprintf(fp, "  cp_format: %d\n", copy_blk->cp_format);
	SIfprintf(fp, "  cp_tupcopy: %d\n", copy_blk->cp_tupcopy);
	SIfprintf(fp, "  cp_varrows: %d\n", copy_blk->cp_varrows);
	SIfprintf(fp, "  cp_continue: %d\n", copy_blk->cp_continue);
	SIfprintf(fp, "  cp_rowlog: %d\n", copy_blk->cp_rowlog);
	SIfprintf(fp, "  cp_program: %d\n", copy_blk->cp_program);
	if (copy_blk->cp_filetype == SI_BIN)
		STcopy("SI_BIN", buf);
	else if (copy_blk->cp_filetype == SI_TXT)
		STcopy("SI_TXT", buf);
	else if (copy_blk->cp_filetype == SI_VAR)
		STcopy("SI_VAR", buf);
	else
		STcopy("UNKNOWN", buf);
	SIfprintf(fp, "  cp_filetype: %s\n", buf);
/*
** Not printed: 
**  i4		cp_not_flushed;		is output buffer flushed
**  i4		cp_error;		is there a fatal error
**  i4		cp_dbmserr;		is there an error on the dbms side
**  i4		cp_readerr;		was there error reading from dbms
**  II_CP_USR	cp_usrrow;		record buffer for COPY FROM
**  PTR		cp_rowbuf;		buffer for copy rows
**  PTR		cp_tupbuf;		buffer for tuples
**  PTR		cp_zerotup;		initialized tuple with no data
*/
	for (i=0; i < copy_blk->cp_num_domains; i++)
	{
		II_CP_MAP	*copy_map = &copy_blk->cp_copy_map[i];

		SIfprintf(fp, "  II_CP_MAP copy_blk->cp_copy_map[%d]\n", i);
		SIfprintf(fp, "    cp_rowlen: %d\n", copy_map->cp_rowlen);
		SIfprintf(fp, "    cp_fieldlen: %d\n", copy_map->cp_fieldlen);
		SIfprintf(fp, "    cp_tuplen: %d\n", copy_map->cp_tuplen);
		SIfprintf(fp, "    cp_attrmap: %d\n", copy_map->cp_attrmap);
		SIfprintf(fp, "    cp_attroffset: %d\n",
						copy_map->cp_attroffset);
		SIfprintf(fp, "    cp_rowtype: %s\n",
					gtr_type(copy_map->cp_rowtype, buf));
		SIfprintf(fp, "    cp_rowprec: %d\n", copy_map->cp_rowprec);
		SIfprintf(fp, "    cp_tuptype: %s\n",
					gtr_type(copy_map->cp_tuptype, buf));
		SIfprintf(fp, "    cp_tupprec: %d\n", copy_map->cp_tupprec);
		SIfprintf(fp, "    cp_cvid: %d\n", copy_map->cp_cvid);
		SIfprintf(fp, "    cp_cvlen: %d\n", copy_map->cp_cvlen);
		SIfprintf(fp, "    cp_cvprec: %d\n", copy_map->cp_cvprec);
		SIfprintf(fp, "    cp_withnull: %d\n", copy_map->cp_withnull);
		SIfprintf(fp, "    cp_nultype: %d\n", copy_map->cp_nultype);
		SIfprintf(fp, "    cp_nullen: %d\n", copy_map->cp_nullen);
		SIfprintf(fp, "    cp_nulprec: %d\n", copy_map->cp_nulprec);
		if (copy_map->cp_nuldata != (PTR) NULL)
			SIfprintf(fp, "    cp_nuldata: %s\n",
						(char *)copy_map->cp_nuldata);
		SIfprintf(fp, "    cp_valchk: %d\n", copy_map->cp_valchk);
		SIfprintf(fp, "    cp_isdelim: %d\n", copy_map->cp_isdelim);
		SIfprintf(fp, "    cp_l_delim: %d\n", copy_map->cp_l_delim);
		SIfprintf(fp, "    cp_name: %s\n", copy_map->cp_name);
		SIfprintf(fp, "    cp_delim: %s\n", copy_map->cp_delim);

/*
** Not printed:
**  ADF_FN_BLK	cp_adf_fnblk;		ADF function block set up for
**					row/tuple conversion calls
*/
	}
	SIfprintf(fp, "-------------------------------------------------\n\n");
	SIclose(fp);
}

/*{
+*  Name: gtr_type - Trace type name mapping routine.
**
**  Description:
**
**  Inputs:
**	type		- Type id.
**
**  Outputs:
**	typename	- Type name mapped as:
**				type id : [nullable] type name
**
**	Returns:
**	    	Pointer to typename.
**	Errors:
**		If invalid type sets typename to DB_unknown_TYPE.
**
**  Side Effects:
-*	
**  History:
**	19-sep-1988	- Written for GCA tracing (ncg)
**	19-oct-1992 (kathryn)
**	    Added DB_LVCH_TYPE and DB_LBIT_TYPE as valid types for new 
**	    large object support.
**	jan-1993 (mgw)	- copied wholesale into iicopy.c for debugging.
**      03-Dec-2003 (gupsh01)
**          Modified copy code so that nchar and nvarchar types will now use
**          UTF8 to read and write from the text file for a formatted copy.
**	03-Aug-2006 (gupsh01)
**	    Added support for ANSI date/time types.
*/

static char *
gtr_type(type, typename)
i4	type;
char	*typename;
{
    char	*tn;

    STprintf(typename, ERx("%d : "), type); 
    if (type < 0)
	STcat(typename, ERx("nullable "));
    switch (abs(type))
    {
      case DB_INT_TYPE:
	tn = ERx("DB_INT_TYPE");
        break;
      case DB_FLT_TYPE:
	tn = ERx("DB_FLT_TYPE");
        break;
      case DB_CHR_TYPE:
	tn = ERx("DB_CHR_TYPE");
        break;
      case DB_TXT_TYPE:
	tn = ERx("DB_TXT_TYPE");
        break;
      case DB_MNY_TYPE:
	tn = ERx("DB_MNY_TYPE");
        break;
      case DB_DTE_TYPE:
	tn = ERx("DB_DTE_TYPE");
        break;
      case DB_ADTE_TYPE:
	tn = ERx("DB_ADTE_TYPE");
        break;
      case DB_TMWO_TYPE:
	tn = ERx("DB_TMWO_TYPE");
        break;
      case DB_TME_TYPE:
	tn = ERx("DB_TME_TYPE");
        break;
      case DB_TMW_TYPE:
	tn = ERx("DB_TMW_TYPE");
        break;
      case DB_TSWO_TYPE:
	tn = ERx("DB_TSWO_TYPE");
        break;
      case DB_TSTMP_TYPE:
	tn = ERx("DB_TSTMP_TYPE");
        break;
      case DB_TSW_TYPE:
	tn = ERx("DB_TSW_TYPE");
        break;
      case DB_INYM_TYPE:
	tn = ERx("DB_INYM_TYPE");
        break;
      case DB_INDS_TYPE:
	tn = ERx("DB_INDS_TYPE");
        break;
      case DB_CHA_TYPE:
	tn = ERx("DB_CHA_TYPE");
        break;
      case DB_VCH_TYPE:
	tn = ERx("DB_VCH_TYPE");
        break;
      case DB_UTF8_TYPE:
	tn = ERx("DB_UTF8_TYPE");
        break;
      case DB_VBYTE_TYPE:
	tn = ERx("DB_VBYTE_TYPE");
        break;
      case DB_BYTE_TYPE:
	tn = ERx("DB_BYTE_TYPE");
        break;
      case DB_DEC_TYPE:
	tn = ERx("DB_DEC_TYPE");
        break;
      case DB_LTXT_TYPE:
	tn = ERx("DB_LTXT_TYPE");
        break;
      case DB_DMY_TYPE:
	tn = ERx("DB_DMY_TYPE");
        break;
      case DB_DBV_TYPE:
	tn = ERx("DB_DBV_TYPE");
        break;
      case DB_QTXT_TYPE:
	tn = ERx("DB_QTXT_TYPE");
        break;
      case DB_LBIT_TYPE:
        tn = ERx("DB_LBIT_TYPE");
        break;
      case DB_LVCH_TYPE:
	tn = ERx("DB_LVCH_TYPE");
	break;
      default:
	tn = ERx("DB_unknown_TYPE");
	break;
    }
    STcat(typename, tn);
    return typename;
}

/*{
+*  Name: IICPbrBlobRead - read the whole table from the DBMS during COPY INTO
**
**  Description:
**	This routine checks the cp_format indicator to see if this is a
**	formatted or bulk copy and calls the approriate blob handler
**	accordingly. That blob handler will copy the whole table to file
**	either in bulk or formatted as approptriate. If this is a
**	COPY INTO PROGRAM, this routine calls the user supplied "put_row"
**	routine to dispose of the copy row.
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**	put_row		- user routine to put copy rows, rather than doing
**			  IO, to a copy file.
**
**  Outputs:
**	none
**
**  Returns: OK, ENDFILE
**
**  History:
**	feb-1993 (mgw)
**	    Written.
**	20-jan-2004 (somsa01)
**	    Removed "COPY FROM/INTO... PROGRAM" restriction, and added
**	    put_row() function argument.
*/
i4
IICPbrBlobRead( II_LBQ_CB *IIlbqcb, 
		II_CP_STRUCT *copy_blk, II_CPY_BUFH *cpy_bufh, i4 (*put_row)() )
{
    if (copy_blk->cp_format)
    {
	return IICPfbrFmtBlobRead( IIlbqcb, copy_blk, cpy_bufh, put_row );
    }
    else
    {
	return IICPbbrBulkBlobRead( IIlbqcb, copy_blk, cpy_bufh, put_row );
    }
}


/*{
+*  Name: IICPbbrBulkBlobRead - COPY whole table from the DBMS to file
**
**  Description:
**	This routine reads the next partial tuple from the GCA message
**	buffer. Since the row contains one or more blob columns, the GCA
**	buffer will contain a partial tuple which should be written out
**	before attempting to get any more data. It will loop through the
**	whole table, using data handlers for long varchar (blob) columns.
**	If this is a COPY INTO PROGRAM, this routine calls the user
**	supplied "put_row" routine to dispose of the copy row.
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**	put_row		- user routine to put copy rows, rather than doing
**			  IO, to a copy file.
**
**  Outputs:
**	none
**
**  Returns: OK, ENDFILE, FAIL
**
**  History:
**	jan-1993 (mgw)
**	    Written
**	14-jun-96 (loera01)
**	    Modified so that decompression for variable-length datatype is
**	    performed in LIBQ, rathern than LIBQGCA. 
**	13-aug-96 (loera01)
**	    Modified to correctly handle partial tuples. Fixes Bug 77129.
**	    Fixed memory leaks for compressed varchar condition.
**	25-Aug-97 (thaju02) bug #85033 - Using copy into statement, 
**	    where column spans multiple gca buffers, resulted in "E_CO0041...
**	    Failed to write non-Large Object column." 
**	20-jan-2004 (somsa01)
**	    Updated to include the "COPY FROM/INTO... PROGRAM" case, and
**	    added put_row() argument.
**	17-Apr-2006 (thaju02) (B115961)
**	    Binary copy table into file may hang. Need to account for col's 
**	    data which span multiple gca buffers. 
*/
i4
IICPbbrBulkBlobRead( II_LBQ_CB *IIlbqcb, 
		     II_CP_STRUCT *copy_blk, II_CPY_BUFH *cpy_bufh,
		     i4 (*put_row)() )
{
    IICGC_DESC		*cgc = IIlbqcb->ii_lq_gca;
    II_CPY_BUFH		*bufh = cpy_bufh;
    II_CP_MAP		*copy_map;
    STATUS		status;
    char		*bptr;
    char                *decompress_buf = (char *)(copy_blk->cp_tupbuf);
    i2			nulblob = 0;	/* null indicator var */
    i4			msg_type;
    i4			curcol = 0;
    i4		num_written;	/* i4 dummy var for SIwrite */
    i4		total_written;
    i4			i = 0;
    char		nullval;	/* null value indicator */
    i4		left_in_gca_buf = 0;
    i4		bytes_to_write;
    bool		inpartial = FALSE;
    bool		row_not_done = FALSE; /* only true if GCA buf ends
					      ** in mid tuple */
    LOBULKINTO_HDL	bbi;		/* Data handler 'handler arg' */
    bool	        support_compression;	
    i4  		compressed_size;
    i4  		compressed_type;
    i4                  compressed_bytes;

    bbi.tupbuf = copy_blk->cp_tupbuf;
    bbi.file_ptr = copy_blk->cp_file_ptr;
    support_compression = IIlbqcb->ii_lq_retdesc.ii_rd_desc.rd_flags &
        RD_CVCH ? TRUE : FALSE;
    for (;;)
    {
	if (left_in_gca_buf == 0)
	{
            msg_type = IIcgcp5_read_data(cgc, &bufh->cpy_dbv);
	    if (msg_type == GCA_RESPONSE)	/* a RESPONSE block received */
	    {
		if ( ! (IIlbqcb->ii_lq_flags & II_L_CONNECT) )
		/* msg_type might be faked by IIcgcp5_read_data */
		    return FAIL;
		return (ENDFILE);
	    }

	    bptr = (char *)bufh->cpy_dbv.db_data;
	    left_in_gca_buf = cgc->cgc_result_msg.cgc_d_length;
	    cgc->cgc_result_msg.cgc_d_used = 0;
	    bufh->cpy_bytesused = 0;
	}

	/*
	** for each column in the row, if it's a regular col, just
	** print it. If it's a BLOB, call the handler to print it.
	*/
	for ( ; i < copy_blk->cp_num_domains; i++)
	{
            if (! inpartial)
            {
                compressed_size = 0;
                compressed_type = IICGC_NON_VCH;
		total_written = 0;
            }
	    copy_map = &copy_blk->cp_copy_map[i];

	    if (copy_blk->cp_dbg_ptr)
		SIfprintf(copy_blk->cp_dbg_ptr, "COPYINTO: %d %s \n", 
		copy_blk->cp_rowcount+1, copy_map->cp_name);

	    if (IIDT_LONGTYPE_MACRO(copy_map->cp_tuptype))
	    {
		/* Make sure libq knows what column we're on */
		IIlbqcb->ii_lq_retdesc.ii_rd_colnum = i;

		bbi.rowcount = copy_blk->cp_rowcount;
		bbi.col_name = copy_map->cp_name;
		bbi.failure = FALSE;
		bbi.cp_program = copy_blk->cp_program;
		bbi.put_row = put_row;
		bbi.cp_dbg_ptr = copy_blk->cp_dbg_ptr;
		bbi.tuptype = copy_map->cp_tuptype;

		/* BLOB COLUMN, Call the handler to print this. */

		IILQldh_LoDataHandler(1, &nulblob, IICPbbiBulkBlobInto,
					(PTR) &bbi);
		if (bbi.failure)
			return (FAIL);

		if (nulblob == DB_EMB_NULL)  /* NULL, handler wasn't called */
		{
		    /* put out nullability indictator if required */
		    if (copy_map->cp_tuptype < 0)    /* If column is nullable */
		    {
			/*
			** Reset nulblob and use it to write 0 length indicator.
			** This is needed because the data handler was never
			** called in this NULL case.
			*/
			nulblob = 0;
			if (copy_blk->cp_program)
			{
			    i4	l_nulblob = sizeof(nulblob);

			    status = (*put_row)(&l_nulblob, (char *)&nulblob,
						&num_written);
			    if (status != OK)
			    {
				IICPwemWriteErrorMsg(
						E_CO0049_PROG_WRITE_MSG_ERR,
						copy_blk->cp_rowcount + 1,
						copy_map->cp_name,
						ERget(S_CO0051_NO_ZERO_IND));
				return (FAIL);
			    }
			}
			else
			{
			    status = SIwrite(sizeof(nulblob), (char *)&nulblob,
					     &num_written,
					     copy_blk->cp_file_ptr);
			    if (status != OK || num_written != sizeof(nulblob))
			    {
				IICPwemWriteErrorMsg(
						E_CO0041_FILE_WRITE_MSG_ERR,
						copy_blk->cp_rowcount + 1,
						copy_map->cp_name,
						ERget(S_CO0051_NO_ZERO_IND));
				return (FAIL);
			    }
			}

			/* Write out NULL indicator */
			nullval = 1;
			if (copy_blk->cp_program)
			{
			    i4	l_nullval = sizeof(nullval);

			    status = (*put_row)(&l_nullval, &nullval,
						&num_written);
			    if (status != OK)
			    {
				IICPwemWriteErrorMsg(
						E_CO0049_PROG_WRITE_MSG_ERR,
						copy_blk->cp_rowcount + 1,
						copy_map->cp_name,
						ERget(S_CO0052_NO_NULL_IND));
				return (FAIL);
			    }
			}
			else
			{
			    status = SIwrite(sizeof(nullval), &nullval,
					     &num_written,
					     copy_blk->cp_file_ptr);
			    if (status != OK)
			    {
				IICPwemWriteErrorMsg(
						E_CO0041_FILE_WRITE_MSG_ERR,
						copy_blk->cp_rowcount + 1,
						copy_map->cp_name,
						ERget(S_CO0052_NO_NULL_IND));
				return (FAIL);
			    }
			}
		    }
		    else
		    {
			IICPwemWriteErrorMsg(
				(copy_blk->cp_program ?
					E_CO0049_PROG_WRITE_MSG_ERR :
					E_CO0041_FILE_WRITE_MSG_ERR),
				copy_blk->cp_rowcount + 1,
				copy_map->cp_name,
				ERget(S_CO0053_CANT_BE_NULL));
			return (FAIL);
		    }
		}
		else if (copy_map->cp_tuptype < 0) /* if column is nullable */
		{
		    /* Write out not NULL indicator */
		    nullval = 0;
		    if (copy_blk->cp_program)
		    {
			i4	l_nullval = sizeof(nullval);

			status = (*put_row)(&l_nullval, &nullval, &num_written);
			if (status != OK)
			{
			    IICPwemWriteErrorMsg(E_CO0049_PROG_WRITE_MSG_ERR,
						 copy_blk->cp_rowcount + 1,
						 copy_map->cp_name,
						 ERget(S_CO0054_NO_NOT_NULL));
			    return (FAIL);
			}
		    }
		    else
		    {
			status = SIwrite(sizeof(nullval), &nullval,
					 &num_written, copy_blk->cp_file_ptr);
			if (status != OK || num_written != sizeof(nullval))
			{
			    IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
						 copy_blk->cp_rowcount + 1,
						 copy_map->cp_name,
						 ERget(S_CO0054_NO_NOT_NULL));
			    return (FAIL);
			}
		    }
		}
		nulblob = 0; /* reset null indicator for next time */

		/* update bptr to just after blob */
		bptr = (char *)bufh->cpy_dbv.db_data +
			cgc->cgc_result_msg.cgc_d_used;
		left_in_gca_buf = cgc->cgc_result_msg.cgc_d_length -
			cgc->cgc_result_msg.cgc_d_used;
	    }
	    else	/* Non-Blob column */
	    {
                if (support_compression && ! inpartial)
                {
                    switch (abs(copy_map->cp_tuptype))
                    {
			case DB_NVCHR_TYPE:
                        case DB_TXT_TYPE:
                        case DB_VCH_TYPE:
                        case DB_VBYTE_TYPE:
                        case DB_LTXT_TYPE:
                        {
			    if (abs(copy_map->cp_tuptype) == DB_NVCHR_TYPE)
				compressed_type = (copy_map->cp_tuptype < 0 ? 
				    IICGC_NVCHR : IICGC_NVCHRN);
                            else
				compressed_type = (copy_map->cp_tuptype < 0 ? 
				    IICGC_VCH : IICGC_VCHN);
                            compressed_size = IIcgc_find_cvar_len(bptr,
                                compressed_type);
                            MEcopy((PTR)bptr,compressed_size,(PTR)decompress_buf);
                            /* 
                            **  If nullable datatype, place indicator at end of col
                            */
                            if (copy_map->cp_tuptype < 0)  
                            {
                                decompress_buf[copy_map->cp_tuplen-1] = 
                                    bptr[compressed_size-1];
                            }
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
		if (!inpartial)
                if (((left_in_gca_buf < copy_map->cp_tuplen) &&
                    ! compressed_type) || ((left_in_gca_buf <
                        compressed_size) && compressed_type))
		{
		    if (left_in_gca_buf > 0)
		    {
			/*
			** We only have a partial col val here, write it
			** off and get the rest. Will this ever happen?
			*/
			if (copy_blk->cp_program)
			{
			    if (compressed_type)
			    {
				status = (*put_row)(&left_in_gca_buf,
						    decompress_buf,
						    &num_written);
			    }
			    else
			    {
				status = (*put_row)(&left_in_gca_buf,
						    bptr, &num_written);
			    }

			    if (status != OK)
			    {
				IICPwemWriteErrorMsg(
					E_CO0049_PROG_WRITE_MSG_ERR,
					copy_blk->cp_rowcount + 1,
					copy_map->cp_name,
		    			ERget(S_CO0055_PARTIAL_NON_BLOB));
				return (FAIL);
			    }
			}
			else
			{
			    if (compressed_type)
			    {
				status = SIwrite(left_in_gca_buf,
						 decompress_buf, &num_written,
						 copy_blk->cp_file_ptr);
			    }
			    else
			    {
				status = SIwrite(left_in_gca_buf,
						 bptr, &num_written,
						 copy_blk->cp_file_ptr);
			    }

			    if (status != OK || num_written != left_in_gca_buf)
			    {
				IICPwemWriteErrorMsg(
					E_CO0041_FILE_WRITE_MSG_ERR,
					copy_blk->cp_rowcount + 1,
					copy_map->cp_name,
		    			ERget(S_CO0055_PARTIAL_NON_BLOB));
				return (FAIL);
			    }
			}

                        if (compressed_type)
                        {	
  		            decompress_buf += left_in_gca_buf;
                        }
                        else
                        {
  		            bptr += left_in_gca_buf;
                        }
		        cgc->cgc_result_msg.cgc_d_used += left_in_gca_buf;
			inpartial = TRUE;
			total_written += num_written;
			left_in_gca_buf = 0;
		    }
		    row_not_done = TRUE;
		    break; /* go get more columns and the rest of this one */
		    
		}
		if (inpartial)
                {
                    if (compressed_type)
                    {
                        MEcopy((PTR)bptr,compressed_size - num_written,
                            (PTR)decompress_buf);
                        /* 
                        **  If nullable datatype, place indicator at end of col
                        */
                        if (copy_map->cp_tuptype < 0)  
                        {
                            decompress_buf[copy_map->cp_tuplen-1-num_written] = 
                                bptr[compressed_size-1-num_written];
                        }
                        compressed_bytes = compressed_size - num_written;
                    }
		    bytes_to_write = copy_map->cp_tuplen - total_written;
		    if (bytes_to_write > left_in_gca_buf)
			bytes_to_write = left_in_gca_buf;
                }
                else
                {
		        bytes_to_write = copy_map->cp_tuplen;
                }

		if (copy_blk->cp_program)
		{
		    if (compressed_type)
		    {
			status = (*put_row)(&bytes_to_write, decompress_buf,
					    &num_written);
		    }
		    else
		    {
			status = (*put_row)(&bytes_to_write, bptr,
					    &num_written);
		    }

		    if (status != OK)
		    {
			IICPwemWriteErrorMsg(E_CO0049_PROG_WRITE_MSG_ERR,
					copy_blk->cp_rowcount + 1,
					copy_map->cp_name,
					ERget(S_CO0056_NO_NON_BLOB_COL));
			return (FAIL);
		    }
		}
		else
		{
		    if (compressed_type)
		    {
			status = SIwrite(bytes_to_write, decompress_buf,
					 &num_written, copy_blk->cp_file_ptr);
		    }
		    else
		    {
			status = SIwrite(bytes_to_write, bptr, &num_written,
					 copy_blk->cp_file_ptr);
		    }

		    /*
		    ** (thaju02, 8/25/97) bug #85033
		    ** changed check from 'num_written != copy_map->cp_tuplen'
		    ** to 'num_written != bytes_to_write' to account for
		    ** columns which span two gca buffers. 
		    ** Increment bptr by bytes_to_write rather than
		    ** copy_map->cp_tuplen.
		    */
		    if (status != OK || num_written != bytes_to_write)
		    {
			IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
					copy_blk->cp_rowcount + 1,
					copy_map->cp_name,
					ERget(S_CO0056_NO_NON_BLOB_COL));
			return (FAIL);
		    }
		}

		/* update bptr here. Also cgc_d_used!!! */
                if (compressed_type)
                {
                    if (inpartial)
                    {
  		        bptr += compressed_bytes;
		        left_in_gca_buf -= compressed_bytes;
		        cgc->cgc_result_msg.cgc_d_used += compressed_bytes;
                    }
                    else
                    {
  		        bptr += compressed_size;
		        left_in_gca_buf -= compressed_size;
		        cgc->cgc_result_msg.cgc_d_used += compressed_size;
                    }
                }
                else
                {
  		    bptr += bytes_to_write;
		    left_in_gca_buf -= bytes_to_write;
		    cgc->cgc_result_msg.cgc_d_used += bytes_to_write;
                }
		total_written += num_written;
		if (total_written < copy_map->cp_tuplen)
		{
		    row_not_done = TRUE;
		    break;
		}
		inpartial = FALSE;
	    }
	} /* foreach column */

	if (row_not_done)
	{
	    /* row's not done yet, don't increment row & column counters */
	    row_not_done = FALSE;
	}
	else
	{
	    ++copy_blk->cp_rowcount;	/* increment row counter */
	    i = 0;			/* restart column counter */
	}

    } /* for (;;) */
}

/*{
+*  Name: IICPbbiBulkBlobInto - Data Handler for reading a BLOB to file
**
**  Description:
**	This routine uses IILQlgd_LoGetData to get segments of the long
**	varchar (BLOB) from the database and put them out to the file
**	in bulk blob format. If this is a COPY INTO PROGRAM, this routine
**	calls the user supplied "put_row" routine to dispose of the copy row.
**
**  Inputs:
**	copy_blk	- copy control block
**
**  Outputs:
**	none
**
**  Returns: void
**
**  History:
**	feb-1993 (mgw)
**	    Written
**	20-jan-2004 (somsa01)
**	    Updated to include the "COPY FROM/INTO... PROGRAM" case.
*/
void
IICPbbiBulkBlobInto(LOBULKINTO_HDL *bbi)
{
    i4	got_seglen;
    i4		data_end;
    i2		slen;		/* i2 length to print to file */
    i4	numwrit;	/* i4 dummy var for SIwrite */
    STATUS	status;		/* return status from SIwrite */
    PTR		segbuf = bbi->tupbuf;
    i4		type = bbi->tuptype;
    i4		curseg = 1;	/* current segment */
    i4		seg_bytes;

    data_end = 0;

    /*
    ** Use DB_NCHR_TYPE for DB_LNVCHR_TYPE 
    **   - DB_CHR_TYPE is not byte swapped, but DB_NCHR_TYPE needs to be.
    **   - DB_CHR_TYPE seglen is (char == bytes)
    **   - DB_NCHR_TYPE seglen is chars
    ** Pass II_BCOPY to skip UCS2->wchar_t conversion (we want UCS2 format)
    */
    if (abs(bbi->tuptype) == DB_LNVCHR_TYPE)
	type = DB_NCHR_TYPE;
    else
	type = DB_CHR_TYPE;

    while (!data_end)
    {
	/* get data */
	IILQlgd_LoGetData(II_DATSEG | II_DATLEN | II_BCOPY, type, (i4) 0, 
			  segbuf, MAX_SEG_INTO_FILE, &got_seglen, &data_end);

	seg_bytes = got_seglen;
	if (abs(bbi->tuptype) == DB_LNVCHR_TYPE)
	    seg_bytes *= sizeof(UCS2);

	if (bbi->cp_dbg_ptr)
	    SIfprintf(bbi->cp_dbg_ptr, "COPYINTO: seg %d len %d bytes %d data_end %d\n", 
		curseg, got_seglen, seg_bytes, data_end);

	/*
	** This should be an impossible contingency because of the
	** MAX_SEG_INTO_FILE arg above.
	*/
        if (got_seglen > MAXI2)
	{
	    IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
	                          bbi->rowcount + 1, bbi->col_name,
	                          ERget(S_CO0057_SEG_TOO_LONG));
	    bbi->failure = TRUE;
	    return;
	}

	slen = (i2) got_seglen;

	/* write out the length first, then data */
	if (bbi->cp_program)
	{
	    i4	l_slen = sizeof(slen);

	    status = (*bbi->put_row)(&l_slen, (char *)&slen, &numwrit);
	    if (status != OK)
	    {
		IICPwemWriteErrorMsg(E_CO0049_PROG_WRITE_MSG_ERR,
				     bbi->rowcount + 1, bbi->col_name,
				     ERget(S_CO0058_NO_SEG_LEN));
		bbi->failure = TRUE;
		return;
	    }
	}
	else
	{
	    status = SIwrite(sizeof(slen), (char *) &slen, &numwrit,
			     bbi->file_ptr);
	    if (status != OK || numwrit != sizeof(slen))
	    {
		IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
				     bbi->rowcount + 1, bbi->col_name,
				     ERget(S_CO0058_NO_SEG_LEN));
		bbi->failure = TRUE;
		return;
	    }
	}

	if (slen)	/* don't write data if there is none */
	{
	    if (bbi->cp_program)
	    {
		status = (*bbi->put_row)(&seg_bytes, segbuf, &numwrit);
		if (status != OK)
		{
		    IICPwemWriteErrorMsg(E_CO0049_PROG_WRITE_MSG_ERR,
					 bbi->rowcount + 1, bbi->col_name,
					 ERget(S_CO0059_NO_SEG));
		    bbi->failure = TRUE;
		    return;
		}
	    }
	    else
	    {
		status = SIwrite(seg_bytes, segbuf, &numwrit, bbi->file_ptr);
		if (status != OK || numwrit != seg_bytes)
		{
		    IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
					 bbi->rowcount + 1, bbi->col_name,
					 ERget(S_CO0059_NO_SEG));
		    bbi->failure = TRUE;
		    return;
		}
	    }
	}

	curseg++;
    }

    /*
    ** Put out the "no more data" length specifier if blob was not empty.
    ** Had it been empty, that "no more data" length specifier would have
    ** been put out in the while loop above.
    */
    if (slen)
    {
	slen = 0;
	if (bbi->cp_program)
	{
	    i4	l_slen = sizeof(slen);

	    status = (*bbi->put_row)(&l_slen, (char *)&slen, &numwrit);
	    if (status != 0)
	    {
		IICPwemWriteErrorMsg(E_CO0049_PROG_WRITE_MSG_ERR,
				     bbi->rowcount + 1, bbi->col_name,
				     ERget(S_CO005A_NO_NO_MORE_DATA));
		bbi->failure = TRUE;
	    }
	}
	else
	{
	    status = SIwrite(sizeof(slen), (char *) &slen, &numwrit,
			     bbi->file_ptr);
	    if (status != 0 || numwrit != sizeof(slen))
	    {
		IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
				     bbi->rowcount + 1, bbi->col_name,
				     ERget(S_CO005A_NO_NO_MORE_DATA));
		bbi->failure = TRUE;
	    }
	}
    }
}

/*{
+*  Name: IICPfbrFmtBlobRead - COPY whole table from the DBMS to file
**
**  Description:
**	This routine reads the next partial tuple from the GCA message
**	buffer. Since the row contains one or more blob columns, the GCA
**	buffer will contain a partial tuple which should be formatted and
**	written out before attempting to get any more data. It will loop
**	through the whole table, using data handlers for long varchar (blob)
**	columns.
**
**	Since the backend sends the columns across in the order they were
**	created and we have no convenient way of storing blobs off for later
**	printing, we must assume here that the columns are in the correct
**	order. THIS IS AN IMPORTANT ASSUMPTION.
**
**	Must accomplish the tasks of non-blob routines IIcpdbread(),
**	IIcpinto_init(), IIcprowformat(), and IIcpputrow() here. That is,
**	read from the GCA buffer, make sure the format buffer is set up
**	correctly (cp_rowbuf), format the data into it, and then write it to
**	the file.
**
**	If this is a COPY INTO PROGRAM, this routine calls the user
**	supplied "put_row" routine to dispose of the copy row.
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**	put_row		- user routine to put copy rows, rather than doing
**			  IO, to a copy file.
**
**  Outputs:
**	none
**
**  Returns: OK, ENDFILE
**
**  History:
**	feb-1993 (mgw)
**	    Written
**	20-jan-2004 (somsa01)
**	    Updated to include the "COPY FROM/INTO... PROGRAM" case, and added
**	    put_row() argument.
**	15-Mar-2005 (schka24)
**	    Blob putter can allocate a conversion buffer, free it when
**	    we return.
**	23-Aug-2005 (gupsh01)
**	    Handle Ascii copy out in form of II_UTF8_TYPE format for 
**	    long nvarchar, nvarchar.
**	12-oct-2006 (thaju02)
**	    Also check lastattr to determine if we are ending on a 
**	    row boundary. (B116562)
*/
i4
IICPfbrFmtBlobRead( II_LBQ_CB *IIlbqcb, 
		    II_CP_STRUCT *copy_blk, II_CPY_BUFH *cpy_bufh,
		    i4 (*put_row)() )
{
    IICGC_DESC		*cgc = IIlbqcb->ii_lq_gca;
    II_CPY_BUFH		*bufh = cpy_bufh;
    II_CP_MAP		*cmap;
    STATUS		status;
    i2			nulblob = 0;	/* null indicator var */
    i4			curcol = 0;
    i4		num_written;	/* i4 dummy var for SIwrite */
    i4			i = 0;
    i4			j;
    i4		bytes_to_write;
    LOBULKINTO_HDL	fbi;		/* Data handler 'handler arg' */

    DB_TEXT_STRING	*txt_ptr;
    i4			name_len;
    i4			isnull = FALSE;
    char		*row_ptr;	/* points into formatted data buffer */
    char		*tup_ptr;	/* points either at DBMS data or
					** aligned data */
    char		*delim_ptr;
    ALIGN_RESTRICT	temp_buf[((DB_GW3_MAXTUP - 1) / sizeof(ALIGN_RESTRICT)) +1];
    i4			fillcnt;
    PTR			tuple;	/* points into data from DBMS */
    char		*null_buf;
    i4			stat;
    i4		maxtup;
    i4		lastattr = -1;
 
#ifdef BYTE_ALIGN
    ALIGN_RESTRICT	align_buf[((DB_GW3_MAXTUP - 1) / sizeof(ALIGN_RESTRICT)) +1];
    ALIGN_RESTRICT	alignrow_buf[((DB_GW3_MAXTUP - 1) / sizeof(ALIGN_RESTRICT)) +1];
    i4			align_row = FALSE;
#endif /* BYTE_ALIGN */
 
    copy_blk->cp_status &=  ~CPY_ERROR;
 
    txt_ptr = (DB_TEXT_STRING *) temp_buf;
 

    fbi.tupbuf = copy_blk->cp_tupbuf;
    fbi.file_ptr = copy_blk->cp_file_ptr;
    fbi.ucv_buf = NULL;
    fbi.libq_cb = IIlbqcb;

    /* from IIcpinto_init(): */
    if (copy_blk->cp_rowbuf == NULL)
    {
	if (loc_alloc((i4)1, (i4)(copy_blk->cp_row_length 
            + MAX_SEG_INTO_FILE), &copy_blk->cp_rowbuf) != OK)
	{
	    char err_buf[15];
	    CVla(copy_blk->cp_rowcount + 1, err_buf);
	    IIlocerr(GE_NO_RESOURCE, E_CO0038_MEM_ERR, II_ERR, 1, err_buf);
	    return (FAIL);
	}
	copy_blk->cp_rbuf_size = copy_blk->cp_row_length;
    }

    /* point row_ptr at buffer for formatting data into */
    row_ptr = (char *) copy_blk->cp_rowbuf;

    for (;;) /* for each row */
    {
	/*
	** for each column in the row, if it's a regular col, just
	** print it. If it's a BLOB, call the handler to print it.
	** Skip over unrequested columns in IICPbdrBlobDbRead().
	*/
	status = OK;
	for (i = 0; i < copy_blk->cp_num_domains; i++)
	{
	    cmap = &copy_blk->cp_copy_map[i];

	    if (cmap->cp_rowlen == ADE_LEN_UNKNOWN)
		cmap->cp_fieldlen = cmap->cp_cvlen;

	    /* if this is a BLOB */
	    if (IIDT_LONGTYPE_MACRO(cmap->cp_tuptype))
	    {
		/* first print any columns that went before this one */
		if (row_ptr != (char *) copy_blk->cp_rowbuf)
		{
		    bytes_to_write = (i4)
				     (row_ptr - (char *) copy_blk->cp_rowbuf);
		    if (copy_blk->cp_program)
		    {
			status = (*put_row)(&bytes_to_write,
					    (char *) copy_blk->cp_rowbuf,
					    &num_written);
			if (status != OK)
			{
			    /* Error writing non blob columns */
			    IICPwemWriteErrorMsg(E_CO0049_PROG_WRITE_MSG_ERR,
				copy_blk->cp_rowcount + 1,
				(i > 0 ? copy_blk->cp_copy_map[i-1].cp_name :
					cmap->cp_name),
				ERget(S_CO0056_NO_NON_BLOB_COL));
			    status = FAIL;
			    break;
			}
		    }
		    else
		    {
			status = SIwrite(bytes_to_write,
					 (char *) copy_blk->cp_rowbuf,
					 &num_written,
					 copy_blk->cp_file_ptr);
			if (status != OK || num_written != bytes_to_write)
			{
			    /* Error writing non blob columns */
			    IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
				copy_blk->cp_rowcount + 1,
				(i > 0 ? copy_blk->cp_copy_map[i-1].cp_name :
					cmap->cp_name),
				ERget(S_CO0056_NO_NON_BLOB_COL));
			    status = FAIL;
			    break;
			}
		    }
		}

		/* Make sure libq knows what column we're on */
		IIlbqcb->ii_lq_retdesc.ii_rd_colnum = cmap->cp_attrmap;

		/* now write the blob */

		if ((stat = IICPbdrBlobDbRead(IIlbqcb, copy_blk, cpy_bufh, 
					      &tuple, cmap, &lastattr)) != OK)
		{
		    /* should only end on row boundaries */
		    if (stat == ENDFILE && i != 0 && lastattr >= 0)
                    {
			stat = FAIL;
                    }
		    status = stat;
		    break;
		}

		fbi.rowcount = copy_blk->cp_rowcount;
		fbi.col_name = cmap->cp_name;
		fbi.failure = FALSE;
		fbi.cp_program = copy_blk->cp_program;
		fbi.put_row = put_row;
		fbi.cp_dbg_ptr = copy_blk->cp_dbg_ptr;
		fbi.tuptype = cmap->cp_tuptype;

		/* Call the handler to print this Blob Column */
		IILQldh_LoDataHandler(1, &nulblob, IICPfbiFmtBlobInto,
					(PTR) &fbi);
		if (fbi.failure)
		{
		    status = FAIL;
		    break;
		}

		cpy_bufh->cpy_bytesused = cgc->cgc_result_msg.cgc_d_used;
		cpy_bufh->cpy_dbv.db_length = cgc->cgc_result_msg.cgc_d_length;

		if (nulblob == DB_EMB_NULL)   /* NULL, handler wasn't called */
		{
		    /* put out nullability indicator if required */
		    if (cmap->cp_tuptype < 0)	/* if column is nullable */
		    {
			/*
			** As for varchar(0) and char(0), a long varchar(0)
			** format MUST have a WITH NULL clause if there is
			** null data for that column. There is no null
			** indicator as there is in Bulk copy.
			*/
			if (cmap->cp_withnull && cmap->cp_nullen)
			{
			    /*
			    ** Allocate enough space for the cp_nuldata
			    ** value and surrounding lengths and spaces
			    */
			    if (loc_alloc((i4)1,
					  (i4)(cmap->cp_nullen + 10),
					  (PTR *) &null_buf) != OK)
			    {
				char err_buf[15];
				CVla(copy_blk->cp_rowcount + 1, err_buf);
				IIlocerr(GE_NO_RESOURCE, E_CO0038_MEM_ERR,
					 II_ERR, 1, err_buf);
				status = FAIL;
				break;
			    }
			    /*
			    ** construct Null value and print it. E.g.
			    ** if cp_nuldata is 'n/a', then print
			    ** '3 n/a0 ' (length, null string, no more
			    ** data, space).
			    */
			    CVna(cmap->cp_nullen, null_buf);
			    STcat(null_buf, ERx(" "));
			    /*
			    ** Can't use STcat here because cp_nuldata is
			    ** probably not null terminated.
			    */
			    j = STlength(null_buf);
			    STlcopy(cmap->cp_nuldata, null_buf + j,
					 cmap->cp_nullen);
			    STcat(null_buf, ERx("0 "));
			    j = STlength(null_buf);
			    if (copy_blk->cp_program)
			    {
				status = (*put_row)(&j, null_buf, &num_written);
				if (status != OK)
				{
				    /*
				    ** Error writing NULL value for this
				    ** column
				    */
				    IICPwemWriteErrorMsg(
						E_CO0049_PROG_WRITE_MSG_ERR,
						copy_blk->cp_rowcount + 1,
						cmap->cp_name,
						ERget(S_CO0062_WRT_NULL_VAL));
				    status = FAIL;
				    break;
				}
			    }
			    else
			    {
				status = SIwrite((i4) j, null_buf,
						 &num_written,
						 copy_blk->cp_file_ptr);
				if (status != OK || num_written != (i4) j)
				{
				    /*
				    ** Error writing NULL value for this
				    ** column
				    */
				    IICPwemWriteErrorMsg(
						E_CO0041_FILE_WRITE_MSG_ERR,
						copy_blk->cp_rowcount + 1,
						cmap->cp_name,
						ERget(S_CO0062_WRT_NULL_VAL));
				    status = FAIL;
				    break;
				}
			    }
			}
			else	/* Must have a WITH NULL if value is NULL */
			{
			    IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
						copy_blk->cp_rowcount + 1,
						cmap->cp_name,
						ERget(S_CO0063_NO_WITH_NULL));
			    status = FAIL;
			    break;
			}
		    }
		    else  /* NULL value returned in NOT NULLABLE column */
		    {	  /* SHOULD NEVER HAPPEN */
			IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
						copy_blk->cp_rowcount + 1,
						cmap->cp_name,
						ERget(S_CO0053_CANT_BE_NULL));
			status = FAIL;
			break;
		    }
		}

		/* reset row_ptr for next col(s) */
		row_ptr = (char *) copy_blk->cp_rowbuf;
	    }
	    /* if this is a dummy domain, may need to construct the value */
	    else if (cmap->cp_rowtype == CPY_DUMMY_TYPE)
	    {
		/* MGW - not complete, could overrun cp_rowbuf (but unlikely) */
		if (cmap->cp_rowlen > 0)
		{
		    /* 
		    ** Bug #6380 - cp_name will be an empty string
		    ** for null dummy columns
		    */
		    name_len = STlength(cmap->cp_name);
		    if (name_len == 0)
			name_len = 1;
		    for (j = 0; j < cmap->cp_rowlen; j++)
		    {
			MEcopy(cmap->cp_name, (u_i2)name_len, row_ptr);
			row_ptr += name_len;
		    }
		}
	    }
	    /* else if ADF conversion function instance exists (non-Blob col) */
	    else if (cmap->cp_cvid != ADI_NOFI)
	    {
		if ((stat = IICPbdrBlobDbRead(IIlbqcb, copy_blk, cpy_bufh, 
					      &tuple, cmap, &lastattr)) != OK)
		{
		    /* should only end on row boundaries */
		    if (stat == ENDFILE && i != 0 && lastattr >= 0)
			stat = FAIL;
		    status = stat;
		    break;
		}

		/* set tup_ptr to point to the data from the DBMS */
#ifdef BYTE_ALIGN
		if (ME_NOT_ALIGNED_MACRO((char*)tuple)
		    && (cmap->cp_tuptype !=  DB_CHR_TYPE)
		    && (cmap->cp_tuptype != -DB_CHR_TYPE)
		    && (cmap->cp_tuptype !=  DB_CHA_TYPE)
		    && (cmap->cp_tuptype != -DB_CHA_TYPE))
		{
		    /*
		    **  This data type must be aligned, so we copy
		    **  it into an aligned buffer.
		    */
		    MEcopy(tuple, (u_i2)cmap->cp_tuplen, (PTR)align_buf);
 
		    tup_ptr = (char *) align_buf;
		}
		else
#endif /* BYTE_ALIGN */
		{
		    tup_ptr = (char *) tuple;
		}

		cmap->cp_adf_fnblk.adf_1_dv.db_data = tup_ptr;
 
		if (cmap->cp_withnull)
		{
		    /* check for null value in tuple data */
 
		    if ((status = adc_isnull(IIlbqcb->ii_lq_adf,
			&cmap->cp_adf_fnblk.adf_1_dv, &isnull)) != E_DB_OK)
		    {
			char err_buf[15];
			CVla(copy_blk->cp_rowcount + 1, err_buf);
			IIlocerr(GE_LOGICAL_ERROR, E_CO003A_ROW_FORMAT_ERR,
				II_ERR, 2, err_buf, cmap->cp_name);
			copy_blk->cp_warnings++;
			status = FAIL;
			break;
		    }
		}
		else
		    isnull = FALSE;		/* Bug 5583 */
		/*
		** If this domain is a TEXT or VARCHAR field, convert into a
		** temp buffer and produce the final result in an external form.
		*/
		if ((cmap->cp_rowtype == DB_TXT_TYPE)    ||
		    (cmap->cp_rowtype == DB_VBYTE_TYPE ) ||
		    (cmap->cp_rowtype == DB_VCH_TYPE)    ||
		    (cmap->cp_rowtype == DB_UTF8_TYPE)   ||
	            (cmap->cp_rowtype == -DB_TXT_TYPE)   ||
	            (cmap->cp_rowtype == -DB_VBYTE_TYPE) ||
		    (cmap->cp_rowtype == -DB_VCH_TYPE)   ||
		    (cmap->cp_rowtype == -DB_UTF8_TYPE))
		{
		    cmap->cp_adf_fnblk.adf_r_dv.db_data = (PTR) temp_buf;
		}
#ifdef BYTE_ALIGN
		else if (ME_NOT_ALIGNED_MACRO(row_ptr)
			&& (cmap->cp_rowtype !=  DB_CHR_TYPE)
			&& (cmap->cp_rowtype != -DB_CHR_TYPE)
			&& (cmap->cp_rowtype !=  DB_CHA_TYPE)
			&& (cmap->cp_rowtype != -DB_CHA_TYPE))
		{
		    /*
		    ** Non-string data must be formatted in an aligned buffer.
		    ** Before conversion, point to an aligned buffer to store
		    ** the result row data in and copy it the to row later on.
		    ** Bug 33862.
		    */
		    cmap->cp_adf_fnblk.adf_r_dv.db_data = (PTR) alignrow_buf;
		    align_row = TRUE;
		}
#endif /* BYTE_ALIGN */
		else
		{
		    cmap->cp_adf_fnblk.adf_r_dv.db_data = row_ptr;
		}
 
		/*
		** Do the conversion.
		*/
		if (isnull && cmap->cp_nullen)
		{
		    MEcopy(cmap->cp_nuldata, (u_i2)(cmap->cp_nullen),
			cmap->cp_adf_fnblk.adf_r_dv.db_data);
 
		    if ((cmap->cp_rowtype == DB_TXT_TYPE)   ||
			(cmap->cp_rowtype == DB_VBYTE_TYPE) ||
		        (cmap->cp_rowtype == DB_UTF8_TYPE)  ||
			(cmap->cp_rowtype == DB_VCH_TYPE))
		    {
 
			/* Bug 5605  */
 
			DB_TEXT_STRING  	*nulptr;
 
			nulptr = (DB_TEXT_STRING *)cmap->cp_nuldata;
			txt_ptr->db_t_count = nulptr->db_t_count;
		    }
		    /* 
		    ** Bug 5798.  If variable character format type, blank pad
		    ** null default value up to cp_fieldlen.
		    ** Bug 8886.  Don't blank pad if length of null default
		    ** value >= cp_fieldlen.
		    */
		    else if (cmap->cp_rowlen == ADE_LEN_UNKNOWN)
		    {
			fillcnt = cmap->cp_fieldlen - cmap->cp_nullen;
			if (fillcnt > 0)
			    MEfill((u_i2)fillcnt, ' ',
			      (PTR)((char *)cmap->cp_adf_fnblk.adf_r_dv.db_data
			      + cmap->cp_nullen));
			if (cmap->cp_nullen > cmap->cp_fieldlen)
			    cmap->cp_fieldlen = cmap->cp_nullen;
		    }
		}
		else
		{
		    status = adf_func(IIlbqcb->ii_lq_adf, &cmap->cp_adf_fnblk);
		    if (copy_blk->cp_status & CPY_ERROR)
		    {
			status = E_DB_ERROR;
		    }
		    if (status)
		    {
			char err_buf[15];
			CVla(copy_blk->cp_rowcount + 1, err_buf);
			IIlocerr(GE_LOGICAL_ERROR, E_CO003A_ROW_FORMAT_ERR,
					II_ERR, 2, err_buf, cmap->cp_name);
			copy_blk->cp_warnings++;
			status = FAIL;
			break;
		    }
		}
 
		if ((cmap->cp_rowtype == DB_TXT_TYPE)   ||
		    (cmap->cp_rowtype == -DB_TXT_TYPE)  ||
		    (cmap->cp_rowtype == DB_VBYTE_TYPE) ||
		    (cmap->cp_rowtype == -DB_VBYTE_TYPE)||
		    (cmap->cp_rowtype == DB_UTF8_TYPE)  ||
		    (cmap->cp_rowtype == -DB_UTF8_TYPE) ||
		    (cmap->cp_rowtype == DB_VCH_TYPE)   ||
		    (cmap->cp_rowtype == -DB_VCH_TYPE))
		{
		    char	*ptr;
 
		    ptr = row_ptr;
 
		    if (cmap->cp_rowlen == ADE_LEN_UNKNOWN)
		    {
			/*
			** Variable length varchar type.  Put length specifier
			** then the character string into the row buffer.
			*/
			cmap->cp_fieldlen = txt_ptr->db_t_count;
			if (cmap->cp_rowtype == DB_VBYTE_TYPE ||
		            cmap->cp_rowtype == DB_UTF8_TYPE  ||
			    cmap->cp_rowtype == DB_VCH_TYPE)
			{
			    cmap->cp_fieldlen += CPY_VCHLEN;
			    STprintf(ptr, "%5d", (i4)txt_ptr->db_t_count);
			    ptr += CPY_VCHLEN;
			}
			MEcopy((PTR)txt_ptr->db_t_text,
			    (u_i2)txt_ptr->db_t_count, (PTR)ptr);
		    }
		    else
		    {
			/*
			** Fixed length varchar type.  Put length specifier and
			** then the string.   If data is not null, pad with
			** nulls.
			*/
			if (   cmap->cp_rowtype == DB_VCH_TYPE
			    || cmap->cp_rowtype == -DB_VCH_TYPE
		            || cmap->cp_rowtype == DB_UTF8_TYPE 
		            || cmap->cp_rowtype == -DB_UTF8_TYPE
			    || cmap->cp_rowtype == DB_VBYTE_TYPE
			    || cmap->cp_rowtype == -DB_VBYTE_TYPE)
			{
			    if (isnull && !cmap->cp_nullen)
				MEfill((u_i2)CPY_VCHLEN, ' ', (PTR)ptr);
			    else
				STprintf(ptr, "%5d", (i4)txt_ptr->db_t_count);
			    ptr += CPY_VCHLEN;
			}
			if (isnull && !cmap->cp_nullen)
			{
			    MEcopy((PTR)txt_ptr->db_t_text,
				    (u_i2)cmap->cp_cvlen-DB_CNTSIZE, (PTR)ptr);
			}
			else
			{
			    MEcopy((PTR)txt_ptr->db_t_text, 
				    (u_i2)txt_ptr->db_t_count, (PTR)ptr);
			    fillcnt = cmap->cp_rowlen - DB_CNTSIZE
					- txt_ptr->db_t_count;
			    if (fillcnt > 0)
				MEfill((u_i2)fillcnt, '\0',
					(PTR)(ptr + txt_ptr->db_t_count));
			}
		    }
		}
#ifdef BYTE_ALIGN
		else if (align_row)
		{
		    /*
		    ** Copy non-string data from the aligned buffer into the
		    ** row buffer.
		    */
		    MECOPY_VAR_MACRO((PTR)alignrow_buf, (u_i2)cmap->cp_fieldlen,
				 (PTR) row_ptr);
		    align_row = FALSE;
		}
#endif /* BYTE_ALIGN */
 
		row_ptr += cmap->cp_fieldlen;
		tuple += cmap->cp_tuplen;

	    } /* if (cmap->cp_cvid != ADI_NOFI) - ADF conversion fcn exists */
	    else
	    {
		if ((stat = IICPbdrBlobDbRead(IIlbqcb, copy_blk, cpy_bufh, 
					      &tuple, cmap, &lastattr)) != OK)
		{
		    /* should only end on row boundaries */
		    if (stat == ENDFILE && i != 0 && lastattr >= 0)
			stat = FAIL;
		    status = stat;
		    break;
		}
		/*
		** If no ADF conversion id, then there is no conversion for this
		** field, just copy the tuple value to the row buffer.
		*/
		/*
		** cp_tuplen is the length of the data from the DBMS,
		** cp_rowlen is the length of the data after formatting,
		** if cp_rowlen is greater than cp_tuplen then only copy
		** cp_tuplen chars over into the format buffer and pad the rest
		*/
		maxtup = (cmap->cp_rowlen > cmap->cp_tuplen) ? cmap->cp_tuplen :
							   cmap->cp_rowlen;
		tup_ptr = (char *) tuple;
		MEcopy((PTR)tup_ptr, (u_i2)maxtup, (PTR)row_ptr);
		row_ptr += maxtup;
		if (maxtup < cmap->cp_rowlen)
		{ /* should this be '\0' filled? will this case ever happen? */
		    MEfill((u_i2)(cmap->cp_rowlen - maxtup), ' ',
				(PTR) row_ptr);
		    row_ptr += (cmap->cp_rowlen - maxtup);
		}
		tuple = tuple + cmap->cp_tuplen;
	    }
	    if (cmap->cp_isdelim)
	    {
		delim_ptr = cmap->cp_delim;
		CMcpyinc(delim_ptr, row_ptr);
	    }
	} /* for each column */

	if (status != OK)
	{
	    if (fbi.ucv_buf != NULL)
		MEfree(fbi.ucv_buf);
	    return (status);
	}

	/* Now print out any regular columns that came after the Blob(s) */
	if (row_ptr != (char *) copy_blk->cp_rowbuf)
	{
	    bytes_to_write = (i4) (row_ptr - (char *) copy_blk->cp_rowbuf);
	    if (copy_blk->cp_program)
	    {
		status = (*put_row)(&bytes_to_write,
				    (char *) copy_blk->cp_rowbuf,
				    &num_written);
		if (status != OK)
		{
		    /* Error writing non blob columns */
		    IICPwemWriteErrorMsg(E_CO0049_PROG_WRITE_MSG_ERR,
			copy_blk->cp_rowcount + 1,
			(i > 0 ? copy_blk->cp_copy_map[i-1].cp_name :
				cmap->cp_name),
			ERget(S_CO0056_NO_NON_BLOB_COL));
		    break;
		}
	    }
	    else
	    {
		status = SIwrite(bytes_to_write, (char *) copy_blk->cp_rowbuf,
				 &num_written, copy_blk->cp_file_ptr);
		if (status != OK || num_written != bytes_to_write)
		{
		    /* Error writing non blob columns */
		    IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
			copy_blk->cp_rowcount + 1,
			(i > 0 ? copy_blk->cp_copy_map[i-1].cp_name :
				cmap->cp_name),
			ERget(S_CO0056_NO_NON_BLOB_COL));
		    break;
		}
	    }

	    /* reset row_ptr for beginning of next row */
	    row_ptr = (char *) copy_blk->cp_rowbuf;
	}

	++copy_blk->cp_rowcount;	/* increment row counter */

    } /* for (;;) for each row */

    /* Here only if failure after row loop */
    if (fbi.ucv_buf != NULL)
	MEfree(fbi.ucv_buf);
    return (FAIL);
}

/*
** Name: IICPfbiFmtBlobInto - Data Handler for reading a BLOB to a file
**
** Description:
**
** Inputs:
**
** Outputs:
**
** Returns: void
**
** History:
**	apr-1993 (mgw)
**	    Written
**	20-jan-2004 (somsa01)
**	    Updated to include the "COPY FROM/INTO... PROGRAM" case.
**	11-Mar-2005 (schka24)
**	    Do all the right stuff for long nvarchar.
**	14-Mar-2005 (schka24)
**	    "Right stuff" needs to include conversion to UTF8.
**	23-Aug-2005 (gupsh01)
**	    Handle Ascii copy out in form of II_UTF8_TYPE format for 
**	    long nvarchar, nvarchar.
*/
void
IICPfbiFmtBlobInto(LOBULKINTO_HDL *fbi)
{
    i4	got_seglen;
    i4		data_end;
    i4	numwrit;	/* i4 dummy var for SIwrite */
    i4  get_type, get_flags;
    STATUS	status;		/* return status from SIwrite */
    PTR		segbuf = fbi->tupbuf;
    char	slen_buf[10];
    DB_DATA_VALUE ucs2_dv, rdv;	/* For UCS2 -> UTF8 conversion */
    DB_STATUS	dbstat;
    char	*row_addr;

    if (abs(fbi->tuptype) == DB_LNVCHR_TYPE)
    {
	/* Ask for size in chars, UCS2, etc -- nvarchar-ish stuff */
	get_type = DB_NCHR_TYPE;
	get_flags = II_DATSEG | II_DATLEN | II_BCOPY;
	/* UTF8 type acts like VARCHAR for the adu utilities */
	rdv.db_datatype = DB_UTF8_TYPE;
	ucs2_dv.db_datatype = DB_NCHR_TYPE;
	/* prec, collID uninteresting;  length, addr set below */
	/* Set up a conversion area if we haven't already.  Worst case
	** is 6 UTF8 octets for each input UCS2 char = 2 octets, ie a
	** 3x expansion.
	*/
	if (fbi->ucv_buf == NULL)
	{
	    fbi->ucv_len = sizeof(i2) + MAX_SEG_INTO_FILE * 3;
	    fbi->ucv_buf = MEreqmem(0, fbi->ucv_len, FALSE, NULL);
	    if (fbi->ucv_buf == NULL)
	    {
		IIlocerr(GE_LOGICAL_ERROR, E_CO0037_MEM_INIT_ERR, II_ERR,
			0, NULL);
		fbi->failure = TRUE;
		return;
	    }
	}
    }
    else
    {
	get_type = DB_CHR_TYPE;
	get_flags = II_DATSEG | II_DATLEN;
    }
    data_end = 0;
    while (!data_end)
    {
	/* get data */
	IILQlgd_LoGetData(get_flags, get_type, (i4) 0, 
			  segbuf, MAX_SEG_INTO_FILE, &got_seglen, &data_end);
	/*
	** This should be an impossible contingency because of the
	** MAX_SEG_INTO_FILE arg above.
	*/
        if (got_seglen > MAXI2)
	{
	    IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
	                          fbi->rowcount + 1, fbi->col_name,
	                          ERget(S_CO0057_SEG_TOO_LONG));
	    fbi->failure = TRUE;
	    return;
	}
	row_addr = segbuf;
	if (get_type == DB_NCHR_TYPE)
	{
	    /* For Unicode, convert the UCS2 segment into UTF8 */
	    ucs2_dv.db_data = row_addr;
	    ucs2_dv.db_length = got_seglen * sizeof(UCS2);
	    rdv.db_data = fbi->ucv_buf;
	    rdv.db_length = fbi->ucv_len;
	    ((DB_TEXT_STRING *) rdv.db_data)->db_t_count = fbi->ucv_len - sizeof(i2);
	    dbstat = adu_nvchr_toutf8(fbi->libq_cb->ii_lq_adf, &ucs2_dv, &rdv);
	    if (dbstat != E_DB_OK)
	    {
		IICPwemWriteErrorMsg(E_CO003A_ROW_FORMAT_ERR,
				     fbi->rowcount + 1, fbi->col_name, "");
		fbi->failure = TRUE;
		return;
	    }
	    row_addr = (char *) &((DB_TEXT_STRING *)rdv.db_data)->db_t_text[0];
	    got_seglen = ((DB_TEXT_STRING *)rdv.db_data)->db_t_count;
	}

	/* write out the (char) length first, then data */

	CVla(got_seglen, slen_buf);		/* length must be ascii */
	(void) STcat(slen_buf, ERx(" "));	/* and space delimitted */

	if (fbi->cp_program)
	{
	    i4	l_slen_buf = STlength(slen_buf);

	    status = (*fbi->put_row)(&l_slen_buf, slen_buf, &numwrit);
	    if (status != OK)
	    {
		IICPwemWriteErrorMsg(E_CO0049_PROG_WRITE_MSG_ERR,
				     fbi->rowcount + 1, fbi->col_name,
				     ERget(S_CO0058_NO_SEG_LEN));
		fbi->failure = TRUE;
		return;
	    }
	}
	else
	{
	    status = SIwrite(STlength(slen_buf), slen_buf, &numwrit,
			     fbi->file_ptr);
	    if (status != OK || numwrit != STlength(slen_buf))
	    {
		IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
				     fbi->rowcount + 1, fbi->col_name,
				     ERget(S_CO0058_NO_SEG_LEN));
		fbi->failure = TRUE;
		return;
	    }
	}

	if (got_seglen)	/* don't write data if there is none */
	{
	    if (fbi->cp_program)
	    {
		status = (*fbi->put_row)(&got_seglen, row_addr, &numwrit);
		if (status != OK)
		{
		    IICPwemWriteErrorMsg(E_CO0049_PROG_WRITE_MSG_ERR,
					 fbi->rowcount + 1, fbi->col_name,
					 ERget(S_CO0059_NO_SEG));
		    fbi->failure = TRUE;
		    return;
		}
	    }
	    else
	    {
		status = SIwrite(got_seglen, row_addr, &numwrit, fbi->file_ptr);
		if (status != OK || numwrit != got_seglen)
		{
		    IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
					 fbi->rowcount + 1, fbi->col_name,
					 ERget(S_CO0059_NO_SEG));
		    fbi->failure = TRUE;
		    return;
		}
	    }
	}
    }

    /*
    ** Put out the "no more data" length specifier if blob was not empty.
    ** Had it been empty, that "no more data" length specifier would have
    ** been put out in the while loop above.
    */
    if (got_seglen)
    {
	(void) STcopy(ERx("0 "), slen_buf);	/* length must be ascii */
						/* and space delimitted */

	if (fbi->cp_program)
	{
	    i4	l_slen_buf = STlength(slen_buf);

	    status = (*fbi->put_row)(&l_slen_buf, (char *)slen_buf, &numwrit);
	    if (status != 0)
	    {
		IICPwemWriteErrorMsg(E_CO0049_PROG_WRITE_MSG_ERR,
				     fbi->rowcount + 1, fbi->col_name,
				     ERget(S_CO005A_NO_NO_MORE_DATA));
		fbi->failure = TRUE;
	    }
	}
	else
	{
	    status = SIwrite(STlength(slen_buf), (char *) slen_buf, &numwrit,
			     fbi->file_ptr);
	    if (status != 0 || numwrit != STlength(slen_buf))
	    {
		IICPwemWriteErrorMsg(E_CO0041_FILE_WRITE_MSG_ERR,
				     fbi->rowcount + 1, fbi->col_name,
				     ERget(S_CO005A_NO_NO_MORE_DATA));
		fbi->failure = TRUE;
	    }
	}
    }
}

/*{
+*  Name: IICPwemWriteErrorMsg - Write the given error message and text to user
**
**  Description:                 Takes the given ernum and calls IIlocerr
**                               with the given row number, column name,
**                               and trailing message. Converts the row
**                               number to a char string and copies the
**                               msg to a private buffer since msg is the
**                               result of an ERget() call and as such, can't
**                               be used as the input to an ERlookup (which
**                               is what IIlocerr will do to it).
**  Inputs:
**	ernum		- Error message to be called
**	rownum		- Current row number to be inserted into message
**	col_name	- Current column name to be inserted into message
**	msg		- Descriptive text to be added to message
**
**  Outputs:
**	none
**
**  Returns: void
**
**  History:
**	jun-1993 (mgw)	- Written
*/
void
IICPwemWriteErrorMsg(i4 ernum, i4 rownum, char *col_name, char *msg)
{
    char msgbuf[80];
    char err_buf[15];

    CVla(rownum, err_buf);
    STlcopy(msg, msgbuf, 80);	/* Can't count on ERget() buffer */
    IIlocerr(GE_LOGICAL_ERROR, ernum, II_ERR, 3, err_buf, col_name, msgbuf);
}

/*{
+*  Name: IICPbcfBulkColFlush - Flush non-blob columns to DBMS in Bulk FROM.
**
**  Description:		This routine ships off to the backend
**				those columns that have been read in so
**				far. It then sets the column pointers back
**				to the beginning of the tuple buffer in
**				preparation for reading in more columns.
**
**  Inputs:
**	col_begin	- pointer to beginning of read in columns
**	col_end		- pointer to end of read in columns
**	eot		- End of Data (tuple)?
**	cgc		- Client general communications descriptor
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**
**  Outputs:
**	col_begin	- reset back to beginning of the tuple buffer
**	col_end		- reset back to beginning of the tuple buffer
**
**  Returns: OK, FAIL
**
**  History:
**	feb-1993 (mgw)	- Written
*/
STATUS
IICPbcfBulkColFlush(char **col_begin, char **col_end, bool eot,
                    IICGC_DESC  *cgc, II_CP_STRUCT *copy_blk,
                    II_CPY_BUFH *cpy_bufh)
{
	i4	bytes_to_copy;

	bytes_to_copy = *col_end - *col_begin;

	/* Copy cols to the GCA buffer */
	MEcopy((PTR)(*col_begin), (u_i2)bytes_to_copy,
				(PTR)cpy_bufh->cpy_dbv.db_data);

	/* Now print them out */
	if (IIcgcp3_write_data(cgc, eot, bytes_to_copy,
					    &cpy_bufh->cpy_dbv) != OK)
		return (FAIL);

	/* Now reset tuple buffer pointers */
	*col_end = copy_blk->cp_tupbuf;
	*col_begin = *col_end;

	return (OK);
}

/*{
+*  Name: IICPbwBlobWrite - read the whole table from the file during COPY FROM
**
**  Description:
**	This routine checks the cp_format indicator to see if this is a
**	formatted or bulk copy and calls the approriate blob handler
**	accordingly. That blob handler will copy the whole table from file
**	to the DBMS either in bulk or formatted as needed. If this is a
**	COPY FROM PROGRAM, it calls the user supplied input routine to get
**	the next copy row.
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**	get_row		- user supplied input routine for COPY FROM PROGRAM
**
**  Outputs:
**	none
**
**  Returns: OK, FAIL, ENDFILE
**
**  History:
**	feb-1993 (mgw)
**	    Written
**	20-jan-2004 (somsa01)
**	    Removed "COPY FROM/INTO... PROGRAM" restriction, and added
**	    get_row() argument.
*/
i4
IICPbwBlobWrite( II_THR_CB *thr_cb, 
		 II_CP_STRUCT *copy_blk, II_CPY_BUFH *cpy_bufh,
		 i4 (*get_row)() )
{
    if (copy_blk->cp_convert)
    {
	return IICPfbwFmtBlobWrite(thr_cb, copy_blk, cpy_bufh, get_row);
    }
    else
    {
	return IICPbbwBulkBlobWrite(thr_cb, copy_blk, cpy_bufh, get_row);
    }
}
/*{
+*  Name: IICPbbwBulkBlobWrite - COPY whole table from file to the DBMS
**
**  Description:
**	This routine reads the tuples from file and puts them into the GCA
**	message buffer to be sent to the DBMS table. It will loop through
**	the whole table, using data handlers for long varchar (blob) columns.
**
**	Must accomplish the tasks of non-blob routines IIcpfrom_init(),
**	IIcpgetrow(), IIcpdbwrite(), and IIcgcp4_write_read_poll() here.
**
**	If this is a COPY FROM PROGRAM, it calls the user supplied input
**	routine to get the next copy row.
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**	get_row		- user supplied input routine for COPY FROM PROGRAM
**
**  Outputs:
**	none
**
**  Returns: OK, FAIL, ENDFILE
**
**  History:
**	feb-1993 (mgw)
**	    Written
**	01-mar-1996 (kch)
**	    Reset the null indicator variable, nulblob, to zero at
**	    the start of each domain in order to allow a bulk
**	    'copy ... from' of data containing multiple rows, at least
**	    one of which contains a null blob. If nulblob is not reset,
**	    the next non-null blob value immediately after a null blob
**	    will cause a failure, because the LO data handler will deal
**	    with it as if it were a null blob. This change fixes bug 74057.
**	20-jan-2004 (somsa01)
**	    Updated to include the "COPY FROM/INTO... PROGRAM" case, and added
**	    get_row() argument.
*/
i4
IICPbbwBulkBlobWrite( II_THR_CB *thr_cb, 
		      II_CP_STRUCT *copy_blk, II_CPY_BUFH *cpy_bufh,
		      i4 (*get_row)() )
{
    II_LBQ_CB	*IIlbqcb = thr_cb->ii_th_session;
    IICGC_DESC	*cgc = IIlbqcb->ii_lq_gca;
    II_CP_MAP	*copy_map;
    i4		i;
    i2		firstseglen;	/* length of 1st segment of Large Object */
    i2		nulblob = 0;	/* null indicator variable */
    i1		nulfromfile;	/* null ind var is only 1 byte in file */
    char	*col_begin;	/* tup pointer to beginning of regular cols */
    char	*col_end;	/* tup pointer to end of regular cols */
    char	*col_aligned;	/* aligned tup pointer */
    STATUS	status;
    i4	num_read;
    i4	gca_buf_len;
#ifdef BYTE_ALIGN
    ALIGN_RESTRICT align_buf[((DB_GW3_MAXTUP -1) / sizeof(ALIGN_RESTRICT)) + 1];
#endif /* BYTE_ALIGN */
    LOBULKFROM_HDL bbf;
    char	phrase_buf[80];	/* to modify error messages E_CO004[23] */

    IIcgc_init_msg(cgc, GCA_CDATA, cgc->cgc_query_lang, 0);
    IIcgcp3_write_data(cgc, FALSE, (i4)0, &cpy_bufh->cpy_dbv);

    gca_buf_len = cpy_bufh->cpy_dbv.db_length;
    bbf.file_ptr = copy_blk->cp_file_ptr;
    bbf.cp_dbg_ptr = copy_blk->cp_dbg_ptr;

    /*
    ** can't write direct to the GCA buffer lest the ADP_PERIPHERAL
    ** overwrite us
    */
    bbf.tupbuf = copy_blk->cp_rowbuf;

    bbf.iseof = FALSE;
    bbf.failure = FALSE;
    bbf.firstseglen = 0;


    for(;;)	/* continue till whole table is copied */
    {
	/*
	** Read domain by domain. On blob columns, use the data
	** handler.
	*/

	bbf.rowcount = copy_blk->cp_rowcount;
	col_end = copy_blk->cp_tupbuf;
	col_begin = col_end;

	for (i = 0; i < copy_blk->cp_num_domains; i++)
	{
	    copy_map = &copy_blk->cp_copy_map[i];
	    bbf.tuptype = copy_map->cp_tuptype;
	    bbf.col_name = copy_map->cp_name;
	    if (copy_blk->cp_dbg_ptr)
		SIfprintf(copy_blk->cp_dbg_ptr, "COPYFROM: %d %s \n", 
		copy_blk->cp_rowcount+1, copy_map->cp_name);
 
	    /* Bug 74057 - reset nulblob to zero */
	    nulblob = 0;
 
	    if (copy_map->cp_tuptype < 0 &&
		IIDT_LONGTYPE_MACRO(copy_map->cp_tuptype )) /* Nullable BLOB */
	    {
		/* flush previous regular columns before sending blob */
		if (col_begin != col_end)
		{
		    if (IICPbcfBulkColFlush(&col_begin, &col_end, FALSE,
						cgc, copy_blk, cpy_bufh) != OK)
			return (FAIL);
		}

		/* Must read off the first 2 bytes to see if value is NULL */
		if (copy_blk->cp_program)
		{
		    i4	count = 2;

		    status = (*get_row)(&count, (char *)&firstseglen,
					&num_read);

		    if (status == ENDFILE && i == 0)
		    {
			/* It's OK to hit EOF at a row boundary */

			/* what's left in gca buffer */
			cpy_bufh->cpy_bytesused = 0;

			/* for EOD in IIcpendcopy */
			copy_blk->cp_not_flushed = TRUE;

			return (ENDFILE);
		    }
		}
		else
		{
		    status = SIread(copy_blk->cp_file_ptr, 2, &num_read,
				    (char *) &firstseglen);

		    if (num_read == 0 && i == 0)
		    {
			/* It's OK to hit EOF at a row boundary */

			/* what's left in gca buffer */
			cpy_bufh->cpy_bytesused = 0;

			/* for EOD in IIcpendcopy */
			copy_blk->cp_not_flushed = TRUE;

			return (ENDFILE);
		    }
		}

		if (status != OK || num_read != 2)
		{
		    /* Error row X: unexpected EOF reading length for segment */
		    /* 1 in column COL_NAME */
		    char err_buf[15];
		    CVla(copy_blk->cp_rowcount + 1, err_buf);
		    STlcopy(ERget(S_CO005B_FIRST_SEG_LEN), phrase_buf, 80);
		    IIlocerr(GE_LOGICAL_ERROR, E_CO0042_BLOB_READ_ERR, II_ERR,
				3, err_buf, copy_map->cp_name, phrase_buf);
		    return (FAIL);
		}
		if (firstseglen == 0)
		{
		    /* read null indicator */
		    if (copy_blk->cp_program)
		    {
			i4	count = 1;

			status = (*get_row)(&count, (char *)&nulfromfile,
					    &num_read);
		    }
		    else
		    {
			status = SIread(copy_blk->cp_file_ptr, 1, &num_read,
					(char *) &nulfromfile);
		    }

		    if (status != OK || num_read != 1)
		    {
			/* Error row X: Unexpected EOF reading NULL indicator */
			/* for segment 1 in column COL_NAME */
			char err_buf[15];
			CVla(copy_blk->cp_rowcount + 1, err_buf);
			STlcopy(ERget(S_CO005C_FIRST_NULL_IND), phrase_buf, 80);
			IIlocerr(GE_LOGICAL_ERROR, E_CO0042_BLOB_READ_ERR,
			         II_ERR, 3, err_buf, copy_map->cp_name,
			         phrase_buf);
			return (FAIL);
		    }
		    if (nulfromfile == 1)
			nulblob = (i2) DB_EMB_NULL;
		    else if (nulfromfile != 0)
		    {
			char err_buf[15];
			CVla(copy_blk->cp_rowcount + 1, err_buf);
			IIlocerr(GE_LOGICAL_ERROR, E_CO0045_BAD_NULL_BLOB,
			         II_ERR, 2, err_buf, copy_map->cp_name);
			return (FAIL);
		    }
		}

		bbf.firstseglen = firstseglen;
		bbf.cp_program = copy_blk->cp_program;
		bbf.get_row = get_row;

		/* so libq knows what type we're sending */
		IIlbqcb->ii_lq_lodata.ii_lo_datatype = copy_map->cp_tuptype;

		IILQldh_LoDataHandler(2, &nulblob, IICPbbfBulkBlobFrom,
						(PTR) &bbf);

		if (bbf.failure)
		    return (FAIL);
		if (bbf.iseof)
		{
		    cpy_bufh->cpy_bytesused = 0; /* what's left in gca buffer */
		    copy_blk->cp_not_flushed = TRUE;/* for EOD in IIcpendcopy */
		    return (ENDFILE);
		}

		/*
		** print rest of large object and then start back at the
		** beginning of the GCA buffer and row buffer
		*/
		IIcgcp3_write_data(cgc, FALSE, cgc->cgc_write_msg.cgc_d_used,
					&cpy_bufh->cpy_dbv);
		IIcgc_init_msg(cgc, GCA_CDATA, cgc->cgc_query_lang, 0);
	    }
	    else if (IIDT_LONGTYPE_MACRO(copy_map->cp_tuptype))
	    {						/* Not Nullable LO */
		/* flush previous regular columns before sending blob */
		if (col_begin != col_end)
		{
		    if (IICPbcfBulkColFlush(&col_begin, &col_end, FALSE,
						cgc, copy_blk, cpy_bufh) != OK)
			return (FAIL);
		}

		/* read seglen in 1st 2 bytes. Only call handler if no EOF */
		if (copy_blk->cp_program)
		{
		    i4	count = 2;

		    status = (*get_row)(&count, (char *)&firstseglen,
					&num_read);

		    if (status == ENDFILE && i == 0)
		    {
			/* It's OK to hit EOF at a row boundary */

			/* what's left in gca buffer */
			cpy_bufh->cpy_bytesused = 0;

			/* for EOD in IIcpendcopy */
			copy_blk->cp_not_flushed = TRUE;

			return (ENDFILE);
		    }
		}
		else
		{
		    status = SIread(copy_blk->cp_file_ptr, 2, &num_read,
				    (char *) &firstseglen);

		    if (num_read == 0 && i == 0)
		    {
			/* It's OK to hit EOF at a row boundary */

			/* what's left in gca buffer */
			cpy_bufh->cpy_bytesused = 0;

			/* for EOD in IIcpendcopy */
			copy_blk->cp_not_flushed = TRUE;

			return (ENDFILE);
		    }
		}

		if (status != OK || num_read != 2)
		{
		    /* Error row X: unexpected EOF reading length for segment */
		    /* 1 in column COL_NAME */
		    char err_buf[15];
		    CVla(copy_blk->cp_rowcount + 1, err_buf);
		    STlcopy(ERget(S_CO005B_FIRST_SEG_LEN), phrase_buf, 80);
		    IIlocerr(GE_LOGICAL_ERROR, E_CO0042_BLOB_READ_ERR, II_ERR,
				3, err_buf, copy_map->cp_name, phrase_buf);
		    return (FAIL);
		}

		bbf.firstseglen = firstseglen;
		bbf.cp_program = copy_blk->cp_program;
		bbf.get_row = get_row;
		nulblob = 0;

		/* so libq knows what type we're sending */
		IIlbqcb->ii_lq_lodata.ii_lo_datatype = copy_map->cp_tuptype;

		IILQldh_LoDataHandler(2, &nulblob, IICPbbfBulkBlobFrom,
							(PTR) &bbf);

		if (bbf.failure)
		    return (FAIL);
		if (bbf.iseof)
		{
		    cpy_bufh->cpy_bytesused = 0; /* what's left in gca buffer */
		    copy_blk->cp_not_flushed = TRUE;/* for EOD in IIcpendcopy */
		    return (ENDFILE);
		}

		/*
		** print rest of large object and then start back at the
		** beginning of the GCA buffer and row buffer
		*/
		IIcgcp3_write_data(cgc, FALSE, cgc->cgc_write_msg.cgc_d_used,
					&cpy_bufh->cpy_dbv);
		IIcgc_init_msg(cgc, GCA_CDATA, cgc->cgc_query_lang, 0);
	    }
	    else /* regular column */
	    {
		/*
		** read in the column, align as needed for adc_valchk() if
		** a valchk is needed. Then ship it off before next blob
		** col or at end of tuple.
		*/

		/* don't overflow the GCA Buffer */
		if (col_end - col_begin + copy_map->cp_tuplen > gca_buf_len)
		{
		    /* send down columns accumulated so far */
		    if (IICPbcfBulkColFlush(&col_begin, &col_end, FALSE,
						cgc, copy_blk, cpy_bufh) != OK)
			return (FAIL);
		}

		if (copy_blk->cp_program)
		{
		    status = (*get_row)(&copy_map->cp_tuplen, (char *)col_end,
					&num_read);

		    if (status == ENDFILE && i == 0)
		    {
			/* It's OK to hit EOF at a row boundary */

			/* what's left in gca buffer */
			cpy_bufh->cpy_bytesused = 0;

			/* for EOD in IIcpendcopy */
			copy_blk->cp_not_flushed = TRUE;

			return (ENDFILE);
		    }
		}
		else
		{
		    status = SIread(copy_blk->cp_file_ptr, copy_map->cp_tuplen,
				    &num_read, (char *) col_end);

		    if (num_read == 0 && i == 0)
		    {
			/* It's OK to hit EOF at a row boundary */

			/* what's left in gca buffer */
			cpy_bufh->cpy_bytesused = 0;

			/* for EOD in IIcpendcopy */
			copy_blk->cp_not_flushed = TRUE;

			return (ENDFILE);
		    }
		}

		if (status != OK || num_read != copy_map->cp_tuplen)
		{
		    /* Error row X: unexpected EOF reading value */
		    /* for column COL_NAME */
		    char err_buf[15];
		    CVla(copy_blk->cp_rowcount + 1, err_buf);
		    STlcopy(ERget(S_CO005D_REG_COL_VAL), phrase_buf, 80);
		    IIlocerr(GE_LOGICAL_ERROR, E_CO0042_BLOB_READ_ERR, II_ERR,
				3, err_buf, copy_map->cp_name, phrase_buf);
		    return (FAIL);
		}

		/* Do the valchk here */
		if (copy_map->cp_valchk)
		{
#ifdef BYTE_ALIGN
		    if (ME_NOT_ALIGNED_MACRO(col_end)
			&& (copy_map->cp_tuptype !=  DB_CHR_TYPE)
			&& (copy_map->cp_tuptype != -DB_CHR_TYPE)
			&& (copy_map->cp_tuptype !=  DB_CHA_TYPE)
			&& (copy_map->cp_tuptype != -DB_CHA_TYPE))
		    {
			/*
			** This datatype must be formatted in an aligned
			** buffer before going down to adc_valchk(). So
			** we use 'align_buf'.
			*/
			MEcopy((PTR)col_end, (u_i2)num_read, (PTR)align_buf);
			col_aligned = (PTR) align_buf;
		    }
		    else
#endif /* BYTE_ALIGN */
			col_aligned = col_end;

		    /*
		    ** Have ADF make sure that the data value just read
		    ** in is a legal Ingres data value.
		    */
		    if ( IICPvcValCheck( IIlbqcb, col_aligned, 
					 copy_map, copy_blk ) != OK )
			return (FAIL);

		}

		col_end += num_read;
		
		/*
		** IIlbqcb->ii_lq_gca.cgc_write_msg.cgc_d_used is reset
		** in IIcgcp3_write_data() when we flush these values.
		*/
	    }

	} /* for each column */

	/* flush previous regular columns if any not printed yet */
	if (col_begin != col_end)
	{
	    if (IICPbcfBulkColFlush(&col_begin, &col_end, TRUE, cgc,
						copy_blk, cpy_bufh) != OK)
		return (FAIL);
	}
	else
	{
	    status = IIcc1_send(cgc, TRUE);	/* nefarious End-of-Data */
	    if (status != OK)
		return (FAIL);
	}


	/*
	** Poll error from DBMS every row since BLOBS can span multiple msgs.
	*/
	if (IIcgcp4_write_read_poll(cgc, GCA_CDATA) != GCA_CONTINUE_MASK)
	{
	    copy_blk->cp_dbmserr = TRUE;
	    return (FAIL);
	}

	copy_blk->cp_rowcount++;

	IIcgc_init_msg(cgc, GCA_CDATA, cgc->cgc_query_lang, 0);
	IIcgcp3_write_data(cgc, FALSE, (i4)0, &cpy_bufh->cpy_dbv);
    } /* for(;;) */
}

/*{
+*  Name: IICPfbwFmtBlobWrite - COPY whole table from the file to the DBMS
**
**  Description:
**	This routine reads tuples from the copy file, formats them as
**	appropriate, and sends them to the DBMS. Since the row contains one
**	or more blob columns, this routine will read a partial tuple which
**	should be written out before attempting to get any more data. It will
**	loop **	through the whole file, using data handlers for long varchar
**	(blob) columns.
**
**	Must accomplish the tasks of non-blob routines IIcpfrom_init(),
**	IIcpgetrow(), IIcptupformat(), IIcpdbwrite(), and
**	IIcgcp4_write_read_poll() here.
**
**	If this is a COPY FROM PROGRAM, it calls the user supplied input
**	routine to get the next copy row.
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**	get_row		- user supplied input routine for COPY FROM PROGRAM
**
**  Outputs:
**	none
**
**  Returns: OK, FAIL, ENDFILE
**
**  History:
**	feb-1993 (mgw)	- Written
**	28-sep-1995 (kch)
**		Modify calls to IICPcfgCheckForGaps() to include the new
**		argument end_of_tuple. This change fixes bug 71595.
**	07-nov-1995 (kch)
**		Reset the null indicator variable, nulblob, to zero at
**		the start of each domain in order to allow an ascii
**		'copy ... from' of data containing multiple rows, at least
**		one of which contains a null blob. If nulblob is not reset,
**		the next non-null blob value immediately after a null blob
**		will cause a failure, because the LO data handler will deal
**		with it as if it were a null blob. This change fixes bug 72172.
**	13-dec-1995 (kch & prida01)
**		Correctly initialize col_end with the "empty" tuple,
**		copy_blk->cp_zerotup. The MEcopy of tup_ptr to col_ptr for the
**		misaligned case has been reinstated. This change fixes bug
**		73123.
**	07-feb-1996 (kch)
**		The buffer seg_l_buf[80] is now initialized with nulls. This
**		ensures that the test for Ingres NULLs will succeed.
**	25-mar-96 (kch)
**		Set end_ptr once before the domain by domain read from the
**		copy file (calls to IICPgcGetCol()). Previously, end_ptr was
**		being wrongly reset after each domain. This change fixes bug
**		75452.
**	20-feb-97 (kch)
**		We now save the width of each column in the zero tuple
**		(zero_width) and use this to move to the start of the zero
**		tuple when the domain by domain processing is done.
**		Previously, copy_blk->cp_tup_length was used - this did not
**		take into account 'skipped' columns in the copy. This change
**		fixes bug 79834 (side effect of fix for bug 73123).
**      13-aug-96 (loera01)
**		Re-initialize copy_blk->cp_zerotup with saved pointer value,
**              instead of decrementing by copy_blk->cp_tup_length.  If the
**		tuple contained a blob and a skipped non-blob column, the
**		pointer did not advance by the full tuple length.
**	14-apr-1998 (thaju02)
**		Bug #90503 - changed STequal to MEcmp in the comparision of 
**		the null data value. Rows are being inserted into the blob 
**		extension table for nullable blobs when blob data is null. 
**	14-apr-1998 (shust01)
**		In IICPfbwFmtBlobWrite(), the variable copy_blk->cp_zerotup
**		is being incremented within the for loop.  If we prematurely
**		exit the routine before the for loop completes, the variable
**		is never reset.  If variable is not reset, could SIGBUS when
**		MEfree storage.  Changed all return statements within loop to
**		breaks, to allow the variable to be reset before returning to
**		calling routine.  bug 93805.
**	20-jan-2004 (somsa01)
**		Updated to include the "COPY FROM/INTO... PROGRAM" case, and
**		added get_row() argument.
**	03-feb-2004 (somsa01)
**		Make sure we pass an i4 as the segment length to get_row().
**	14-Mar-2005 (schka24)
**		Free up Unicode conversion buffer.
**	28-Mar-2006 (drivi01)
**		After the row buffer is resized and the old chunk of memory is
**		freed fbf.tupbuf is never updated with new location of a buffer
**		and therefore it ends up pointing to freed memory. This is for
**		blobs only.
**	03-Aug-2006 (gupsh01)
**	    Added support for ANSI date/time types.
*/
i4
IICPfbwFmtBlobWrite( II_THR_CB *thr_cb, 
		     II_CP_STRUCT *copy_blk, II_CPY_BUFH *cpy_bufh,
		     i4 (*get_row)() )
{
    II_LBQ_CB	*IIlbqcb = thr_cb->ii_th_session;
    IICGC_DESC	*cgc = IIlbqcb->ii_lq_gca;
    II_CP_MAP	*copy_map;
    i4		i;
    i2		firstseglen;	/* length of 1st segment of Large Object */
    i2		nulblob = 0;	/* null indicator variable */
    char	*col_begin;	/* tup pointer to beginning of regular cols */
    char	*col_end;	/* tup pointer to end of regular cols */
    char	*col_ptr;	/* pointer to regular column data */
    char	*row_ptr;	/* pointer to cp_rowbuf conversion buffer */
    char	*end_ptr;	/* pointer to end of cp_rowbuf */
    char	*tup_ptr;	/* pointer into cp_tupbuf */
    i4		valchk;
    i4		tmp_nat;
    STATUS	status = OK;
    i4		num_read;
    i4		bytes_to_read;
    i4		gca_buf_len;
    i4		numtounread;
    i4		nextattr = 0;
    i4		zero_width = 0;
    PTR         zerotup = copy_blk->cp_zerotup;

#ifdef BYTE_ALIGN
    ALIGN_RESTRICT align_buf[((DB_GW3_MAXTUP -1) / sizeof(ALIGN_RESTRICT)) + 1];
    ALIGN_RESTRICT	alignrow_buf[((DB_GW3_MAXTUP - 1) / sizeof(ALIGN_RESTRICT)) +1];
#endif /* BYTE_ALIGN */
    LOBULKFROM_HDL fbf;
    char	seg_l_buf[80];	/* to modify error messages E_CO004[23] */
    char	vchar_count[CPY_VCHLEN + 1];
    DB_TEXT_STRING	*txt_ptr;
    ALIGN_RESTRICT	txt_buf[((DB_GW3_MAXTUP -1) / sizeof(ALIGN_RESTRICT)) +1];
    char	junkbuf;
    IILQ_LODATA	*lodata;


    MEfill(80, '\0', seg_l_buf);

    IIcgc_init_msg(cgc, GCA_CDATA, cgc->cgc_query_lang, 0);
    IIcgcp3_write_data(cgc, FALSE, (i4)0, &cpy_bufh->cpy_dbv);

    gca_buf_len = cpy_bufh->cpy_dbv.db_length;
    fbf.file_ptr = copy_blk->cp_file_ptr;

    /*
    ** can't write direct to the GCA buffer lest the ADP_PERIPHERAL
    ** overwrite us
    */
    fbf.tupbuf = copy_blk->cp_rowbuf;

    fbf.iseof = FALSE;
    fbf.failure = FALSE;
    fbf.firstseglen = 0;
    fbf.ucv_buf = NULL;
    fbf.libq_cb = IIlbqcb;

    for(;;)	/* continue till whole table is copied */
    {
	/* Read domain by domain. On blob columns use the data handler. */

	fbf.rowcount = copy_blk->cp_rowcount;
	col_end = (char *) copy_blk->cp_tupbuf;
	col_begin = col_end;
	row_ptr = (char *) copy_blk->cp_rowbuf;
	/* Bug 75452 - Set end_ptr once before domain by domain read */
	end_ptr = row_ptr + copy_blk->cp_rbuf_size -
				(row_ptr - copy_blk->cp_rowbuf) - 1;

	for (i = 0; i < copy_blk->cp_num_domains; i++)
	{
	    copy_map = &copy_blk->cp_copy_map[i];

	    /* Bug 73123  - initialize col_end with the "zero" tuple */
	    if (!IIDT_LONGTYPE_MACRO(copy_map->cp_tuptype))
	    {
		MEcopy( copy_blk->cp_zerotup, (u_i2)copy_map->cp_tuplen,
			col_end );
	    }

	    /* Bug 72172 - reset nulblob to zero */
	    nulblob = 0;

	    /* NEED TO READ AHEAD IN CASE OF EOF HERE */
	    if (copy_blk->cp_program)
	    {
		i4	count = 0;

		status = (*get_row)(&count, &junkbuf, &num_read);
		if (status == ENDFILE && i == 0)
		{
		    /* It's OK to hit EOF at a row boundary */
		    /* Tell LIBQ that we already sent DATAEND */
		    lodata = &IIlbqcb->ii_lq_lodata;
		    lodata->ii_lo_flags |= II_LO_END;
		    cpy_bufh->cpy_bytesused = 0; /* what's left in gca buffer */
		    copy_blk->cp_not_flushed = TRUE;/* for EOD in IIcpendcopy */
		    break;
		}
	    }
	    else
	    {
		status = SIread(copy_blk->cp_file_ptr, 1, &num_read, &junkbuf);
		if (num_read == 0 && i == 0)
		{
		    /* It's OK to hit EOF at a row boundary */
		    /* Tell LIBQ that we already sent DATAEND */
		    lodata = &IIlbqcb->ii_lq_lodata;
		    lodata->ii_lo_flags |= II_LO_END;
		    cpy_bufh->cpy_bytesused = 0; /* what's left in gca buffer */
		    copy_blk->cp_not_flushed = TRUE;/* for EOD in IIcpendcopy */
		    status = ENDFILE;
		    break;
		}
		status = SIfseek(copy_blk->cp_file_ptr, -1, SI_P_CURRENT);
		if (status != OK)
		{
		    /* Error SIfseek()'ing over EOF test byte, col n row m */
		    IICPwemWriteErrorMsg(E_CO0047_FILE_READ_MSG,
					 copy_blk->cp_rowcount + 1,
					 copy_map->cp_name,
					 ERget(S_CO0066_SEEK_VAL));
		    status = FAIL;
		    break;
		}
	    }

	    if ( IICPcfgCheckForGaps( IIlbqcb, copy_blk, copy_map, 
				      &col_begin, &col_end, cpy_bufh, 
				      &nextattr, FALSE ) != OK )
	    {
		status = FAIL;
		break;
	    }

	    if (IIDT_LONGTYPE_MACRO(copy_map->cp_tuptype))
	    {
		fbf.tuptype = copy_map->cp_tuptype;
		fbf.col_name = copy_map->cp_name;
		fbf.col_num = i;
		fbf.cp_program = copy_blk->cp_program;
		fbf.cp_dbg_ptr = copy_blk->cp_dbg_ptr;
		fbf.get_row = get_row;

		/* flush previous regular columns before sending blob */
		if (col_begin != col_end)
		{
		    if (IICPbcfBulkColFlush(&col_begin, &col_end, FALSE,
						cgc, copy_blk, cpy_bufh) != OK)
		    {
			status = FAIL;
			break;
		    }
		    if (copy_blk->cp_rowbuf != fbf.tupbuf)
		    {
			fbf.tupbuf = copy_blk->cp_rowbuf; 
		    }
		}

		/* if nullable and WITH NULL clause specified, check for null */
		if (copy_map->cp_tuptype < 0 && copy_map->cp_nullen > 0)
		{   /*
		    ** Read off length of first segment. If it is equal to the
		    ** length of the WITH NULL string then see if the values
		    ** match. If they do, then call the data handler with the
		    ** NULL indicator set to DB_EMB_NULL. If not then UNREAD
		    ** the value and length indicator and call the data handler
		    ** with NULL indicator set to 0.
		    */
		    numtounread = IICPfslFmtSegLen(copy_blk->cp_file_ptr,
		                                   copy_blk->cp_rowcount,
		                                   copy_map->cp_name,
		                                   &firstseglen, i, get_row);

		    if (numtounread == -1)
		    {
			if (i == 0)
			    status = ENDFILE;
			else
			    status = FAIL;
			break;
		    }

		    if ((i4) firstseglen == copy_map->cp_nullen)
		    {
			if (copy_blk->cp_program)
			{
			    i4	lseglen = firstseglen;

			    status = (*get_row)(&lseglen, seg_l_buf,
						&num_read);

			    if (status != OK)
			    {
				IICPwemWriteErrorMsg(
						E_CO004A_PROG_READ_MSG_ERR,
						copy_blk->cp_rowcount + 1,
						copy_map->cp_name,
						ERget(S_CO0068_FST_SEGMENT));
				status = FAIL;
				break;
			    }
			}
			else
			{
			    status = SIread(copy_blk->cp_file_ptr,
					    (i4) firstseglen,
					    &num_read, seg_l_buf);
			    if (status != OK || num_read != (i4) firstseglen)
			    {
				/* Error reading first segment row n col m */
				IICPwemWriteErrorMsg(E_CO0047_FILE_READ_MSG,
						copy_blk->cp_rowcount + 1,
						copy_map->cp_name,
						ERget(S_CO0068_FST_SEGMENT));
				status = FAIL;
				break;
			    }
			}

			numtounread += num_read;
			if (MEcmp(copy_map->cp_nuldata, seg_l_buf,
				firstseglen) == 0)
			{   /*
			    ** Found a match for the WITH NULL string.
			    ** Read off ending "no more data" indicator and
			    ** call the data handler with null indicator set
			    ** Can use the seg_l_buf buffer for this since we
			    ** don't need that anymore.
			    */
			    if (copy_blk->cp_program)
			    {
				i4	count = 2;

				status = (*get_row)(&count, seg_l_buf,
						    &num_read);
				if (status != OK)
				{
				    /*
				    ** Error reading 2nd segment length row n
				    ** col m
				    */
				    IICPwemWriteErrorMsg(
						E_CO004A_PROG_READ_MSG_ERR,
						copy_blk->cp_rowcount + 1,
						copy_map->cp_name,
						ERget(S_CO0064_SEC_SEG_LEN));
				    status = FAIL;
				    break;
				}
			    }
			    else
			    {
				status = SIread(copy_blk->cp_file_ptr, 2,
						&num_read, seg_l_buf);
				if (status != OK || num_read != 2)
				{
				    /*
				    ** Error reading 2nd segment length row n
				    ** col m
				    */
				    IICPwemWriteErrorMsg(
						E_CO0047_FILE_READ_MSG,
						copy_blk->cp_rowcount + 1,
						copy_map->cp_name,
						ERget(S_CO0064_SEC_SEG_LEN));
				    status = FAIL;
				    break;
				}
			    }

			    if (seg_l_buf[0] == '0' && seg_l_buf[1] == ' ')
			    {
				nulblob = DB_EMB_NULL;
			    }
			    else
			    {
				/*
				** Error, expected 'no more data' seg len
				** after null value col n row m. Note this
				** assumes that for WITH NULL ('JUNK'), user
				** won't try to specify a blob value like
				** '4 JUNK9 MORE JUNK0 '
				*/
				IICPwemWriteErrorMsg(
					(copy_blk->cp_program ?
					    E_CO004A_PROG_READ_MSG_ERR :
					    E_CO0047_FILE_READ_MSG),
					copy_blk->cp_rowcount + 1,
					copy_map->cp_name,
					ERget(S_CO0065_EXPECT_NMD));
				status = FAIL;
				break;
			    }
			}
			else
			{
			    /* UNREAD what we just read and call data handler */
			    if (copy_blk->cp_program)
			    {
				i4	count = -numtounread;

				status = (*get_row)(&count, NULL, NULL);

				if (status != OK)
				{
				    /*
				    ** Error SIfseek()'ing over null val
				    ** col n row m
				    */
				    IICPwemWriteErrorMsg(
						E_CO0047_FILE_READ_MSG,
						copy_blk->cp_rowcount + 1,
						copy_map->cp_name,
						ERget(S_CO0066_SEEK_VAL));
				    status = FAIL;
				    break;
				}
			    }
			    else
			    {
				status = SIfseek(copy_blk->cp_file_ptr,
						 -numtounread, SI_P_CURRENT);
				if (status != OK)
				{
				    /*
				    ** Error SIfseek()'ing over null val
				    ** col n row m
				    */
				    IICPwemWriteErrorMsg(
						E_CO0047_FILE_READ_MSG,
						copy_blk->cp_rowcount + 1,
						copy_map->cp_name,
						ERget(S_CO0066_SEEK_VAL));
				    status = FAIL;
				    break;
				}
			    }
			}
		    }
		    else
		    {
			/* seg length != cp_nullen, back up and call handler */
			if (copy_blk->cp_program)
			{
				i4	count = -numtounread;

				status = (*get_row)(&count, NULL, NULL);

				if (status != OK)
				{
				    /*
				    ** Error SIfseek()'ing over seg len
				    ** col n row m
				    */
				    IICPwemWriteErrorMsg(
						E_CO004A_PROG_READ_MSG_ERR,
						copy_blk->cp_rowcount + 1,
						copy_map->cp_name,
						ERget(S_CO0067_SEEK_LEN));
				    status = FAIL;
				    break;
				}
			}
			else
			{
			    status = SIfseek(copy_blk->cp_file_ptr,
					     -numtounread, SI_P_CURRENT);
			    if (status != OK)
			    {
				/*
				** Error SIfseek()'ing over seg len col n
				** row m
				*/
				IICPwemWriteErrorMsg(E_CO0047_FILE_READ_MSG,
						copy_blk->cp_rowcount + 1,
						copy_map->cp_name,
						ERget(S_CO0067_SEEK_LEN));
				status = FAIL;
				break;
			    }
			}
		    }
		}	/* End of NULLABILITY test */

		/* so libq knows what type we're sending */
		IIlbqcb->ii_lq_lodata.ii_lo_datatype = copy_map->cp_tuptype;

		/* call the handler */
		IILQldh_LoDataHandler(2, &nulblob, IICPfbfFmtBlobFrom,
					(PTR) &fbf);

		if (fbf.failure)
		{
		    status = FAIL;
		    break;
		}
		if (fbf.iseof)
		{
		    cpy_bufh->cpy_bytesused = 0;
		    copy_blk->cp_not_flushed = TRUE;
		    status = ENDFILE;
		    break;
		}

		/* read off the delimitter, if any, to junk buffer */
		if (copy_map->cp_isdelim)
		{
		    bytes_to_read = STlength(copy_map->cp_delim);
		    if (copy_blk->cp_program)
		    {
			status = (*get_row)(&bytes_to_read, seg_l_buf,
					    &num_read);

			if (status != OK)
			{
			    IICPwemWriteErrorMsg(E_CO004A_PROG_READ_MSG_ERR,
						 copy_blk->cp_rowcount + 1,
						 copy_map->cp_name,
						 ERget(S_CO0069_DELIM));
			    status = FAIL;
			    break;
			}
		    }
		    else
		    {
			status = SIread(copy_blk->cp_file_ptr, bytes_to_read,
					&num_read, seg_l_buf);
			if (status != OK || num_read != bytes_to_read)
			{
			    IICPwemWriteErrorMsg(E_CO0047_FILE_READ_MSG,
						 copy_blk->cp_rowcount + 1,
						 copy_map->cp_name,
						 ERget(S_CO0069_DELIM));
			    status = FAIL;
			    break;
			}
		    }

		    if (CMcmpcase(seg_l_buf, copy_map->cp_delim) != 0)
		    {
			IICPwemWriteErrorMsg(E_CO0047_FILE_READ_MSG,
				copy_blk->cp_rowcount + 1,
				copy_map->cp_name,
				ERget(S_CO006A_BAD_DELIM));
			status = FAIL;
			break;
		    }
		}

		/*
		** print rest of large object and then start back at the
		** beginning of the GCA buffer and row buffer
		*/
		IIcgcp3_write_data(cgc, FALSE, cgc->cgc_write_msg.cgc_d_used,
					&cpy_bufh->cpy_dbv);
		IIcgc_init_msg(cgc, GCA_CDATA, cgc->cgc_query_lang, 0);
	    }
	    else	/* regular column */
	    {
		status = IICPgcGetCol(copy_blk, &row_ptr,
		             &end_ptr, i, &copy_blk->cp_rowbuf, get_row);
		if (status != OK)
		    break;

		/*
		** IICPgcGetCol() moves row_ptr as it reads. Put it back
		** so we can copy the read in value to the tuple buffer.
		*/
		row_ptr -= copy_map->cp_fieldlen;
		if (copy_map->cp_isdelim)
		    CMprev(row_ptr, copy_blk->cp_rowbuf);
/* -------------------------------------------------------------------------- */
		/*
		** The following to the end of this else was lifted
		** from IIcptupformat()
		*/

		valchk = copy_map->cp_valchk;
 
		/* skip over a dummy column */
 
		if (copy_map->cp_rowtype == CPY_DUMMY_TYPE)
		{
		    row_ptr += copy_map->cp_fieldlen;
		    if (copy_map->cp_isdelim)
			CMnext(row_ptr);
		    continue;
		}
 
		/*
		** Get pointer to the start of this column in the tuple buffer.
		*/
		tup_ptr = col_end;
#ifdef BYTE_ALIGN
		if (ME_NOT_ALIGNED_MACRO(tup_ptr)
		    && (copy_map->cp_tuptype !=  DB_CHR_TYPE)
		    && (copy_map->cp_tuptype != -DB_CHR_TYPE)
		    && (copy_map->cp_tuptype !=  DB_CHA_TYPE)
		    && (copy_map->cp_tuptype != -DB_CHA_TYPE))
		{
		    /*
		    ** This datatype must be formatted in an aligned buffer.
		    ** So, we use 'align_buf' and copy it to the tuple later.
		    */

		    col_ptr = (char *)align_buf;
 
		    /*
		    ** If doing conversion, initialize the 'align_buf' to be 
		    ** an "empty" data value.  If not doing conversion, the column
		    ** has been read from the file into the tuple buffer, so copy
		    ** that into the aligned buffer.
		    */

		    MEcopy((PTR)tup_ptr, (u_i2)copy_map->cp_tuplen,
		            (PTR)col_ptr);
		}
		else
#endif /* BYTE_ALIGN */
		{
		    /*
		    ** Format the datatype directly into the tuple buffer.
		    */

		    col_ptr = tup_ptr;
		}
 
		/*
		** Check if we have to do a conversion on this data type.
		** If we don't have to do a conversion, then just copy the row value
		** to the tuple buffer.  However, if cp_convert is not set then the
		** row and tuple buffer are the same - no copy needed.
		*/
		if (copy_map->cp_cvid == ADI_NOFI)
		{
		    if (copy_blk->cp_convert)
			MEcopy((PTR)row_ptr, (u_i2)copy_map->cp_rowlen, (PTR)col_ptr);
		}
		/*
		** If the field read in was not zero length (cp_fieldlen
		** set by IIcpgetrow), do the conversion.  Otherwise, just 
		** use the null value that was already initialized for this 
		** attribute.
		**
		** If "with null (value)" clause was specified for this field,
		** check for the value representing null.  If so, skip to the
		** the next field.  The null value has already been formatted
		** into the tuple buffer by IIcpinit.
		**
		** Bug fix 32562: If a "with null (value)" was specified for a
		** field that is not nullable and 'value' was found in the file,
		** give an error.
		*/
		else if (IIcp_isnull(copy_map, row_ptr))	/* With null value found ? */
		{
		    if (copy_map->cp_tuptype > 0)           /* Is field not nullable ? */
		    {
			/* Attempt to put NULL data into a not nullable field */
			IIlocerr(GE_LOGICAL_ERROR, E_CO003E_NOT_NULLABLE,
				II_ERR, 1, copy_map->cp_name);
			copy_blk->cp_warnings++;
			status = FAIL;
			break;
		    }
		}
		else if (copy_map->cp_fieldlen > 0)
		{
		    copy_map->cp_adf_fnblk.adf_r_dv.db_data = col_ptr;
	 
		    switch (copy_map->cp_rowtype)	/* special cases for text and vchar */
		    {
			case DB_TXT_TYPE:
			case -DB_TXT_TYPE:
			{
			    txt_ptr = (DB_TEXT_STRING *) txt_buf;
			    if (copy_map->cp_rowlen == ADE_LEN_UNKNOWN)
			    /* Variable length text field */
			    {
				txt_ptr->db_t_count = copy_map->cp_fieldlen; 
			    }
			    else
			    /*
			    ** Fixed length text field:
			    ** Decide on length by counting data up to first null
			    ** However, we copy bytes according to value of
			    ** cp_fieldlen because terminating null indicator byte
			    ** is significant.
			    */
			    {
				/* Bug #6059 */
				txt_ptr->db_t_count = STnlength(copy_map->cp_fieldlen,
								row_ptr);
			    }
			    MEcopy(row_ptr, (u_i2)copy_map->cp_fieldlen,
					(PTR)txt_ptr->db_t_text);
			    /* 
			    ** Modify ADF function block to point to the temporary
			    ** buffer.
			    */ 
			    copy_map->cp_adf_fnblk.adf_1_dv.db_data = (PTR) txt_ptr;
			    copy_map->cp_adf_fnblk.adf_1_dv.db_length = 
				    copy_map->cp_fieldlen + DB_CNTSIZE;
			    break;
			}

			case DB_VCH_TYPE:
			case -DB_VCH_TYPE:
			case DB_UTF8_TYPE:
			case -DB_UTF8_TYPE:
			case DB_VBYTE_TYPE:
			case -DB_VBYTE_TYPE:
			{
			    /* 
			    ** Move the character count field and character string
			    ** into the temporary buffer.  Set the ADF function block
			    ** to point to the buffer.
			    */ 
			    txt_ptr = (DB_TEXT_STRING *) txt_buf;
			    MEcopy((PTR)row_ptr, (u_i2)CPY_VCHLEN, (PTR)vchar_count);
			    vchar_count[CPY_VCHLEN] = '\0';
			    /* Bug 7645 */
			    status = CVan(vchar_count, &tmp_nat);
			    if ((status != OK) || (tmp_nat < 0) ||
				(tmp_nat > DB_GW4_MAXSTRING))
			    {
				char err_buf[15];
				CVla(copy_blk->cp_rowcount + 1, err_buf);
				IIlocerr(GE_DATA_EXCEPTION, E_CO000B_LEN_SPEC_ERR, 
					  II_ERR, 2, copy_map->cp_name, err_buf);
				copy_blk->cp_warnings++;
				status = FAIL;
				break;
			    }

			    txt_ptr->db_t_count = (u_i2)tmp_nat;
 
			    if (copy_map->cp_rowlen == ADE_LEN_UNKNOWN)
			    /*
			    ** Variable length varchar:
			    ** Copy number of chars according to count.
			    */
			    {
				MEcopy((PTR)(row_ptr + CPY_VCHLEN),
				    (u_i2)txt_ptr->db_t_count, (PTR)txt_ptr->db_t_text);
				copy_map->cp_adf_fnblk.adf_1_dv.db_length = 
				    txt_ptr->db_t_count + DB_CNTSIZE;
			    }
			    else
			    /*
			    ** Fixed length varchar:
			    ** Copy up to width of field.  If nullable VCH, we
			    ** must read null indicator byte.
			    */
			    {
				MEcopy((PTR)(row_ptr + CPY_VCHLEN),
				    (u_i2)copy_map->cp_cvlen-DB_CNTSIZE,
				    (PTR)txt_ptr->db_t_text);
				copy_map->cp_adf_fnblk.adf_1_dv.db_length = copy_map->cp_cvlen;
			    }
			    copy_map->cp_adf_fnblk.adf_1_dv.db_data = (PTR) txt_ptr;
			    break;
			}
			default:
			{
#ifdef BYTE_ALIGN
			    if (ME_NOT_ALIGNED_MACRO(row_ptr)
		    		&& (copy_map->cp_rowtype !=  DB_CHR_TYPE)
		    		&& (copy_map->cp_rowtype != -DB_CHR_TYPE)
		    		&& (copy_map->cp_rowtype !=  DB_CHA_TYPE)
		    		&& (copy_map->cp_rowtype != -DB_CHA_TYPE))
			    {
				/*
				** Non-string data must be aligned in the row buffer
				** before conversion so copy data to an aligned buffer.
				** Bug 33862.
				*/
				MECOPY_VAR_MACRO((PTR)row_ptr, (u_i2)copy_map->cp_fieldlen,
						(PTR) alignrow_buf);
			        copy_map->cp_adf_fnblk.adf_1_dv.db_data = 
						(char *)alignrow_buf;
			    }
			    else
#endif /* BYTE_ALIGN */
			        copy_map->cp_adf_fnblk.adf_1_dv.db_data = row_ptr;
			    break;
			}
		    }
		    if (status != OK)
			break;

		    /*
		    ** Do the conversion.
		    */
		    status = adf_func(IIlbqcb->ii_lq_adf, &copy_map->cp_adf_fnblk);
		    valchk = 0;
		    if (copy_blk->cp_status & CPY_ERROR)
		    {
			status = E_DB_ERROR;
		    }
		    if (status)
		    {
			char err_buf[15];
			CVla(copy_blk->cp_rowcount + 1, err_buf);
			IIlocerr(GE_LOGICAL_ERROR, E_CO0039_TUP_FORMAT_ERR, II_ERR, 
				 2, err_buf, copy_map->cp_name);
			copy_blk->cp_warnings++;

			/* Report user error if applicable (Bug 21432) */
			/* 
			** Display error message formatted by ADF, note:
			** these messages do not have timestamps (Bug 36059) 
			*/
			if (IIlbqcb->ii_lq_adf->adf_errcb.ad_errclass == ADF_USER_ERROR)
			{
			    IIsdberr( thr_cb,
				&IIlbqcb->ii_lq_adf->adf_errcb.ad_sqlstate,
				ss_2_ge(
				  IIlbqcb->ii_lq_adf->adf_errcb.ad_sqlstate.db_sqlstate,
				  IIlbqcb->ii_lq_adf->adf_errcb.ad_usererr),
				IIlbqcb->ii_lq_adf->adf_errcb.ad_usererr,
				0, 
				IIlbqcb->ii_lq_adf->adf_errcb.ad_emsglen,
				IIlbqcb->ii_lq_adf->adf_errcb.ad_errmsgp,
				FALSE);
			}
			status = FAIL;
			break;
		    }

		    /*
		    ** When adf_func() converts a DATE type to a DATE type, it doesn't
		    ** verify that the input date is in a correct INGRES 
		    ** internal date format, it just does an MEcopy.  This is
		    ** correct behavior for other adf_func() users since INGRES has
		    ** already generated/validated the date; however, COPY
		    ** is getting an internal date field from an "outside" source
		    ** (a row in the file) and thus, we have to validate this date
		    ** before sending it on to the server.  (Bug fix 41780)
		    */
		    if ((copy_map->cp_rowtype == DB_DTE_TYPE &&
			copy_map->cp_tuptype == DB_DTE_TYPE) || 
			(copy_map->cp_rowtype == DB_ADTE_TYPE &&
		   	copy_map->cp_tuptype == DB_ADTE_TYPE) || 
			(copy_map->cp_rowtype == DB_TMWO_TYPE &&
		   	copy_map->cp_tuptype == DB_TMWO_TYPE) || 
			(copy_map->cp_rowtype == DB_TME_TYPE &&
		   	copy_map->cp_tuptype == DB_TME_TYPE) || 
			(copy_map->cp_rowtype == DB_TMW_TYPE &&
		   	copy_map->cp_tuptype == DB_TMW_TYPE) || 
			(copy_map->cp_rowtype == DB_TSWO_TYPE &&
		   	copy_map->cp_tuptype == DB_TSWO_TYPE) || 
			(copy_map->cp_rowtype == DB_TSW_TYPE &&
		   	copy_map->cp_tuptype == DB_TSW_TYPE) || 
			(copy_map->cp_rowtype == DB_TSTMP_TYPE &&
		   	copy_map->cp_tuptype == DB_TSTMP_TYPE) ||
			(copy_map->cp_rowtype == DB_INYM_TYPE &&
		   	copy_map->cp_tuptype == DB_INYM_TYPE) ||
			(copy_map->cp_rowtype == DB_INDS_TYPE &&
		   	copy_map->cp_tuptype == DB_INDS_TYPE))
		    {
			valchk = 1;
		    }
 
		    /*
		    ** Check if ADF converted any unprintable chars to blank here.
		    ** Although it is a bit kludgy, we look directly into the ADF
		    ** session control block to get this information.  The clean way
		    ** is to call adx_chkwarn, but that has its drawbacks:
		    **	   - it returns the first warning it finds - thus you may
		    **	     have to call it several times to get the warning we are
		    **	     looking for.
		    **	   - since it counts chars converted, rather than strings
		    **	     which had chars in them converted, we would have to do
		    **	     this loop of calls to adx_chkwarn after each conversion
		    **	     in order to get the number of domains affected.
		    **	     this could be massively expensive.
		    **	   - adx_chkwarn will format a message in the ADF control block
		    **	     if there is a pointer to a message buffer.  we don't want
		    **       a message, but we don't want to zero out the LIBQ adf
		    **       control block's error buffer either.
		    */
		    if (IIlbqcb->ii_lq_adf->adf_warncb.ad_noprint_cnt)
		    {
			copy_blk->cp_blanked++;
			IIlbqcb->ii_lq_adf->adf_warncb.ad_noprint_cnt = 0;
		    }
		}
 
		/*
		** Have ADF make sure that the data value just read in is a legal
		** Ingres data value.
		*/
		if (valchk)
		{
		    if ( IICPvcValCheck( IIlbqcb, col_ptr, 
					 copy_map, copy_blk ) != OK )
		    {
			status = FAIL;
			break;
		    }
		}
 
#ifdef BYTE_ALIGN
		/*
		** If we had to use a temp. aligned buffer to format the column,
		** then copy the formatted data into the tuple buffer.
		*/
		if (ME_NOT_ALIGNED_MACRO(tup_ptr)
		    && (copy_map->cp_tuptype !=  DB_CHR_TYPE)
		    && (copy_map->cp_tuptype != -DB_CHR_TYPE)
		    && (copy_map->cp_tuptype !=  DB_CHA_TYPE)
		    && (copy_map->cp_tuptype != -DB_CHA_TYPE)
		    && copy_blk->cp_convert)
		{
		    MEcopy((PTR)align_buf, (u_i2)copy_map->cp_tuplen, (PTR)tup_ptr); 
		}
#endif /* BYTE_ALIGN */
 
		/*
		** Advance to the next domain of the copy row.  Skip the delimiter if
		** there is one.
		*/
		row_ptr += copy_map->cp_fieldlen;
		if (copy_map->cp_isdelim)
		    CMnext(row_ptr);
		col_end += copy_map->cp_tuplen;
/* -------------------------------------------------------------------------- */
	    }
	
	    /* Bug 73123  - increment the "zero" tuple */
	    if (!IIDT_LONGTYPE_MACRO(copy_map->cp_tuptype))
	    {
		copy_blk->cp_zerotup = copy_blk->cp_zerotup + copy_map->cp_tuplen;
		zero_width += copy_map->cp_tuplen;
	    }
	}	/* end of for each column */

	/* Bug 73123  - move back to the start of the "zero" tuple */
	copy_blk->cp_zerotup = copy_blk->cp_zerotup - zero_width;
	zero_width = 0;

	if (status != OK) /* some error happened inside the for loop */
	{
	    if (fbf.ucv_buf != NULL)
		MEfree(fbf.ucv_buf);
	    return (status);
	}

	/* check for gaps at end of tuple */
	if ( IICPcfgCheckForGaps( IIlbqcb, copy_blk, copy_map, 
				  &col_begin, &col_end, cpy_bufh, 
				  &nextattr, TRUE ) != OK )
	    break;

	/* flush previous regular columns if any not printed yet */
	if (col_begin != col_end)
	{
	    if (IICPbcfBulkColFlush(&col_begin, &col_end, TRUE, cgc,
						copy_blk, cpy_bufh) != OK)
		break;
	}
	else
	{
	    status = IIcc1_send(cgc, TRUE);	/* nefarious End-of-Data */
	    if (status != OK)
		break;
	}

	/*
	** Poll error from DBMS every row since BLOBS can span multiple msgs.
	*/
	if (IIcgcp4_write_read_poll(cgc, GCA_CDATA) != GCA_CONTINUE_MASK)
	{
	    copy_blk->cp_dbmserr = TRUE;
	    break;
	}

	copy_blk->cp_rowcount++;

	IIcgc_init_msg(cgc, GCA_CDATA, cgc->cgc_query_lang, 0);
	IIcgcp3_write_data(cgc, FALSE, (i4)0, &cpy_bufh->cpy_dbv);
    } /* for(;;) */

    /* Here only if failure.  ("normal" exit is the return above.) */
    if (fbf.ucv_buf != NULL)
	MEfree(fbf.ucv_buf);
    return (FAIL);
}


/*{
+*  Name: IICPcfgCheckForGaps - Check for columns not requested in the Format
**
**  Description:                For each column not requested in the Format,
**                              this routine will either null pad and advance
**                              the col_end ptr for regular columns or send
**                              off a null or empty blob as appropriate for
**                              blob columns.
**
**  Inputs:
**	copy_blk	- copy control block
**	copy_map	- the copy map for the current column
**	col_begin	- pointer to beginning of tuple info
**	col_end		- pointer to end of tuple info (add next reg col here)
**	cpy_bufh	- copy buffer header
**	nextattr	- next column number expected
**	end_of_tuple	- TRUE if this function is called at end of tuple
**
**  Outputs:
**	none
**
**  Side Effects:
**	col_begin	- reset if tuple buffer overflows and tuple flushed
**	col_end		- points to just after last skipped column added
**	nextattr	- incremented for each column skipped and for the
**			  one expected. Reset to 0 after last column.
**
**  Returns: OK, FAIL
**
**  History:
**	jun-1993 (mgw)	- Written
**	28-sep-1995 (kch)
**		Add new argument (end_of_tuple). If it is set to TRUE, then we
**		know that this function is being called by IICPfbwFmtBlobWrite()
**		at the end of a tuple, and dummy columns will not cause a
**		return before nextattr can be reset to 0. This change fixes
**		bug 71595.
*/
STATUS
IICPcfgCheckForGaps
(
    II_LBQ_CB		*IIlbqcb,
    II_CP_STRUCT	*copy_blk, 
    II_CP_MAP		*copy_map,
    char		**col_begin, 
    char		**col_end, 
    II_CPY_BUFH		*cpy_bufh,
    i4		*nextattr, 
    bool		end_of_tuple
)
{
    i4			attrlen;
    ROW_DESC		*rdesc;
    i4			gca_buf_len;
    DB_DT_ID		nexttype;
    i2			nulblob = (i2) DB_EMB_NULL;
    IICGC_DESC		*cgc = IIlbqcb->ii_lq_gca;

    if (copy_map->cp_rowtype == CPY_DUMMY_TYPE && end_of_tuple == FALSE)
	return (OK);

    gca_buf_len = cpy_bufh->cpy_dbv.db_length;
    rdesc = &IIlbqcb->ii_lq_retdesc.ii_rd_desc;

    if (*nextattr >= rdesc->rd_numcols) /* if at end of previous row */
    {
	*nextattr = 0;
	return (OK);
    }

    if (*nextattr > copy_map->cp_attrmap)
    {
	while (*nextattr < rdesc->rd_numcols)
	{
	    /* Check for overflow of buffer first */
	    if (*col_end - *col_begin + (char *) copy_map->cp_tuplen >
						(char *) gca_buf_len)
	    {
		/* send down columns accumulated so far */
		if (IICPbcfBulkColFlush(col_begin, col_end, FALSE,
				IIlbqcb->ii_lq_gca, copy_blk, cpy_bufh) != OK)
		    return (FAIL);
	    }

	    nexttype = rdesc->RD_DBVS_MACRO(*nextattr).db_datatype;
	    attrlen = rdesc->RD_DBVS_MACRO(*nextattr).db_length;
	    /* if regular column, just blank pad */
	    if (!IIDT_LONGTYPE_MACRO(nexttype))
	    {
		MEfill((u_i2) attrlen, '\0', (PTR) *col_end);
		*col_end += attrlen;
	    }
	    else	/* skip the blob (ship an empty blob) */
	    {
		if (*col_begin != *col_end)
		{
		    /* send down regular columns accumulated so far */
		    if (IICPbcfBulkColFlush(col_begin, col_end, FALSE,
				IIlbqcb->ii_lq_gca, copy_blk, cpy_bufh) != OK)
			return (FAIL);
		}
		/* so libq knows what type we're sending */
		IIlbqcb->ii_lq_lodata.ii_lo_datatype = nexttype;

		/* Need to unset nulblob if column default is not null */
		if (nexttype > 0)
		    nulblob = 0;
		IILQldh_LoDataHandler(2, &nulblob, IICPsebSendEmptyBlob,
						(PTR) NULL);
		/*
		** print rest of large object and then start back at the
		** beginning of the GCA buffer and row buffer
		*/
		IIcgcp3_write_data(cgc, FALSE, cgc->cgc_write_msg.cgc_d_used,
					&cpy_bufh->cpy_dbv);
		IIcgc_init_msg(cgc, GCA_CDATA, cgc->cgc_query_lang, 0);
	    }
	    ++*nextattr;
	}
 
	*nextattr = 0;
	return (OK);
    }

    /* fill in intervening columns */
    while (*nextattr < copy_map->cp_attrmap)
    {
	/* Check for overflow of buffer first */
	if (*col_end - *col_begin + (char *) copy_map->cp_tuplen >
						(char *) gca_buf_len)
	{
	    /* send down columns accumulated so far */
	    if (IICPbcfBulkColFlush(col_begin, col_end, FALSE,
				IIlbqcb->ii_lq_gca, copy_blk, cpy_bufh) != OK)
		return (FAIL);
	}

	nexttype = rdesc->RD_DBVS_MACRO(*nextattr).db_datatype;
	attrlen = rdesc->RD_DBVS_MACRO(*nextattr).db_length;
	/* if regular column, just blank pad */
	if (!IIDT_LONGTYPE_MACRO(nexttype))
	{   
	    MEfill((u_i2) attrlen, '\0', (PTR) *col_end);
	    *col_end += attrlen;
	}
	else	/* skip the blob (ship an empty blob) */
	{
	    if (*col_begin != *col_end)
	    {
		/* send down regular columns accumulated so far */
		if (IICPbcfBulkColFlush(col_begin, col_end, FALSE,
				IIlbqcb->ii_lq_gca, copy_blk, cpy_bufh) != OK)
		    return (FAIL);
	    }
	    /* so libq knows what type we're sending */
	    IIlbqcb->ii_lq_lodata.ii_lo_datatype = nexttype;

	    /* Need to unset nulblob if column default is not null */
	    if (nexttype > 0)
		nulblob = 0;
	    IILQldh_LoDataHandler(2, &nulblob, IICPsebSendEmptyBlob,
						(PTR) NULL);
	    /*
	    ** print rest of large object and then start back at the
	    ** beginning of the GCA buffer and row buffer
	    */
	    IIcgcp3_write_data(cgc, FALSE, cgc->cgc_write_msg.cgc_d_used,
					&cpy_bufh->cpy_dbv);
	    IIcgc_init_msg(cgc, GCA_CDATA, cgc->cgc_query_lang, 0);

	}
	++*nextattr;
    }
    ++*nextattr;	/* skip current copy_map attribute */
    return (OK);
}


/*{
+*  Name: IICPfslFmtSegLen - Read the Segment Length of a Formatted Blob
**
**  Description:
**	Read the Segment Length of a Formatted Blob one character at a time
**	from the input file until a delimitting space is found. Then
**	translate that length to an i2 and send it back so the segment
**	can be read. Return the number of chars read from the copy file in
**	case we need to SIfseek() back when reading ahead for NULL vals.
**	Return -1 if there was an error.
**
**	If this is a COPY FROM PROGRAM, it calls the user supplied input
**	routine to get the next copy row.
**
**  Inputs:
**	fptr		- the copy file pointer
**	rowcount	- the current row count
**	colname		- the current column name
**	i		- current attribute number (EOF is OK if (i == 0))
**	get_row		- user supplied input routine for COPY FROM PROGRAM
**
**  Outputs:
**	seglen             - seg length read from file and converted to i4
**
**  Returns:               - # of chars read from file or -1 on error or EOF.
**
**  History:
**	jun-1993 (mgw)
**	    Written
**	20-jan-2004 (somsa01)
**	    Updated to include the "COPY FROM/INTO... PROGRAM" case, and added
**	    get_row() argument.
*/
i4
IICPfslFmtSegLen(FILE *fptr, i4 rowcount, char *colname, i2 *seglen, i4  i,
		 i4 (*get_row)())
{
    char	segbuf[15];	/* buffer to hold char representation of len */
    i4	lseglen;	/* i4 representation of seglen */
    char	*segptr;	/* pointer into the segbuf */
    i4	num_read;	/* # of chars read in an SIread() */
    i4	numtounread;	/* accumulator for # of chars read from file */
    STATUS	status;

    numtounread = 0;
    segptr = segbuf;
    *segptr = '\0';

    do {
	if (get_row)
	{
	    i4	count = 1;

	    status = (*get_row)(&count, segptr, &num_read);
	    if (status != OK)
	    {
		/* Error reading first seg length row n col m */
		if (i != 0)
		{
		    IICPwemWriteErrorMsg(E_CO004A_PROG_READ_MSG_ERR,
					 rowcount + 1, colname,
					 ERget(S_CO005F_FMT_SEG_LEN));
		}
		return(-1);
	    }
	}
	else
	{
	    status = SIread(fptr, 1, &num_read, segptr);
	    if (status != OK || num_read != 1)
	    {
		/* Error reading first seg length row n col m */
		if (i != 0)
		{
		    IICPwemWriteErrorMsg(E_CO0047_FILE_READ_MSG, rowcount + 1,
					 colname, ERget(S_CO005F_FMT_SEG_LEN));
		}
		return(-1);
	    }
	}

	numtounread++;
	if (CMspace(segptr))
	{
	    segptr++;
	    break;
	}
	if (!CMdigit(segptr))
	{
	    /* Error reading first seg length row n col m */
	    IICPwemWriteErrorMsg(E_CO0047_FILE_READ_MSG, rowcount + 1,
					colname, ERget(S_CO005F_FMT_SEG_LEN));
	    return(-1);
	}
	segptr++;
    } while (segptr - segbuf < 14);
    if (segptr - segbuf < 14)
	*segptr = '\0';
    else
    {
	IICPwemWriteErrorMsg(E_CO0047_FILE_READ_MSG, rowcount + 1,
				colname, ERget(S_CO005F_FMT_SEG_LEN));
	return(-1);
    }
    status = CVal(segbuf, &lseglen);
    if (status != OK || lseglen > MAXI2)
    {
	IICPwemWriteErrorMsg(E_CO0047_FILE_READ_MSG, rowcount + 1,
				colname, ERget(S_CO005F_FMT_SEG_LEN));
	return(-1);
    }
    *seglen = (i2) lseglen;
    return (numtounread);
}


/*{
+*  Name: IICPbbfBulkBlobFrom - Data handler for reading a blob from file
**
**  Description:
**	Read the Blob one segment at a time from the input file and ship it
**	down via calls to IILQlpd_LoPutData().
**
**  Inputs:
**	bbf	- Bulk Blob From block.
**
**  Outputs:
**	none
**
**  Returns: OK, ENDFILE
**
**  History:
**	feb-1993 (mgw)
**	    Written
**	20-jan-2004 (somsa01)
**	    Updated to include the "COPY FROM/INTO... PROGRAM" case.
**	03-feb-2004 (somsa01)
**	    Make sure we pass an i4 as the segment length to get_row().
*/
void
IICPbbfBulkBlobFrom(LOBULKFROM_HDL *bbf)
{
    STATUS	status;
    i2		seglen;
    i2		nextseglen;
    i4		num_read;
    i4		data_end = 0;
    i4		curseg = 1;	/* current segment */
    char	cursegbuf[15];
    char	phrase_buf[80];
    char	nullind;
    i4		seg_flags;
    i4		type;
    i4		seg_bytes;

    bbf->iseof = FALSE;		/* Haven't read EOF yet */
    bbf->failure = FALSE;

    /* 1st segment length was already read */
    /* NOTE: This routine won't be called if nullable and value was NULL */

    seglen = bbf->firstseglen;

    while (!data_end)
    {
	/* read the segment into the tupbuf */
	seg_bytes = seglen;
	if (abs(bbf->tuptype) == DB_LNVCHR_TYPE)
	    seg_bytes *= sizeof(UCS2);

	if (seglen != 0)	/* in case of 0 length segment */
	{
	    if (bbf->cp_program)
	    {
		i4	lseglen = seg_bytes;

		status = (*bbf->get_row)(&lseglen, (char *)bbf->tupbuf,
					 &num_read);
	    }
	    else
	    {
		status = SIread(bbf->file_ptr, (i4) seg_bytes, &num_read,
				(char *) bbf->tupbuf);
	    }

	    if (status != OK || num_read != (i4) seg_bytes)
	    {
		/* Error row X: unexpected EOF reading value for segment */
		/* curseg in column COL_NAME */
		char err_buf[15];
		CVla(bbf->rowcount + 1, err_buf);
		STlcopy(ERget(S_CO0060_SEG_VAL), phrase_buf, 80);
		CVla(curseg, cursegbuf);
		IIlocerr(GE_LOGICAL_ERROR, E_CO0043_BLOB_SEG_READ_ERR, II_ERR,
				4, err_buf, bbf->col_name, phrase_buf,
				cursegbuf);
		bbf->iseof = TRUE;
		bbf->failure = TRUE;
		return;
	    }

	    curseg++;

	    /* read the length of the next segment */
	    if (bbf->cp_program)
	    {
		i4	count = 2;

		status = (*bbf->get_row)(&count, (char *)&nextseglen,
					 &num_read);
	    }
	    else
	    {
		status = SIread(bbf->file_ptr, 2, &num_read,
				(char *) &nextseglen);
	    }

	    if (status != OK || num_read != 2)
	    {
		/* Error row X: unexpected EOF reading length for segment */
		/* curseg in column COL_NAME */
		char err_buf[15];
		CVla(bbf->rowcount + 1, err_buf);
		STlcopy(ERget(S_CO0061_SEG_LEN), phrase_buf, 80);
		CVla(curseg, cursegbuf);
		IIlocerr(GE_LOGICAL_ERROR, E_CO0043_BLOB_SEG_READ_ERR, II_ERR,
				4, err_buf, bbf->col_name, phrase_buf,
				cursegbuf);
		bbf->iseof = TRUE;
		bbf->failure = TRUE;
		return;
	    }
	}
	else
	    nextseglen = 0;

	if (nextseglen == 0)
	    data_end = 1;

	/*
	** Use DB_NCHR_TYPE for DB_LNVCHR_TYPE 
	**   - DB_CHR_TYPE is not byte swapped, but DB_NCHR_TYPE needs to be.
	**   - DB_CHR_TYPE seglen is (char == bytes)
	**   - DB_NCHR_TYPE seglen is chars
	** Pass II_BCOPY to skip wchar_t->UCS2 conversion (it already is UCS2)
	*/
	if (abs(bbf->tuptype) == DB_LNVCHR_TYPE)
	    type = DB_NCHR_TYPE;
	else
	    type = DB_CHR_TYPE;

	/* ship the segment */
	if (seglen)
	    seg_flags = (II_DATSEG | II_DATLEN | II_BCOPY);
	else
	    seg_flags = 0;

	if (bbf->cp_dbg_ptr)
	    SIfprintf(bbf->cp_dbg_ptr, "COPYFROM: seg %d len %d bytes %d data_end %d\n",
		curseg - 1, seglen, seg_bytes, data_end);

	IILQlpd_LoPutData(seg_flags, type, 0, 
		seglen ? bbf->tupbuf : (char *) 0, (i4) seglen, data_end);

	if (!data_end)
	{
	    seglen = nextseglen;
	}
	else if (bbf->tuptype < 0 && curseg != 1)
	{
	    /* Nullable, read off null indicator if not yet */
	    /* read. Note that if curseg == 1, then we read */
	    /* the null indicator in IICPbbwBulkBlobWrite() */
	    if (bbf->cp_program)
	    {
		i4	count = 1;

		status = (*bbf->get_row)(&count, &nullind, &num_read);
	    }
	    else
		status = SIread(bbf->file_ptr, 1, &num_read, &nullind);

	    if (status != OK || num_read != 1)
	    {
		/* Error row X: unexpected EOF reading Null indicator */
		/* for column COL_NAME */
		char err_buf[15];
		CVla(bbf->rowcount + 1, err_buf);
		STlcopy(ERget(S_CO005E_NULL_IND), phrase_buf, 80);
		IIlocerr(GE_LOGICAL_ERROR, E_CO0042_BLOB_READ_ERR, II_ERR,
				3, err_buf, bbf->col_name, phrase_buf);
		bbf->iseof = TRUE;
		bbf->failure = TRUE;
		return;
	    }
	    if (nullind != 0)
	    {
		char err_buf[15];
		CVla(bbf->rowcount + 1, err_buf);
		IIlocerr(GE_LOGICAL_ERROR, E_CO0046_NOZERO_NULL_BLOB, II_ERR,
				2, err_buf, bbf->col_name);
		bbf->failure = TRUE;
		return;
	    }
	}
    }
}


/*{
+*  Name: IICPfbfFmtBlobFrom - Data handler for reading a blob from file
**
**  Description:
**	Read the Blob one segment at a time from the input file and ship
**	it down via calls to IILQlpd_LoPutData().
**
**	If this is a COPY FROM PROGRAM, it calls the user supplied input
**	routine to get the next copy row.
**
**  Inputs:
**	fbf	- Formatted Blob From block.
**	get_row	- user supplied input routine for COPY FROM PROGRAM
**
**  Outputs:
**	none
**
**  Returns: OK, ENDFILE
**
**  History:
**	jun-1993 (mgw)
**	    Written
**	oct-1993 (kathryn)
**	    Added variable seg_flags for call to IILQlpd_LoPutData. 
**	    First parameter to IILQlpd_LoPutData is now bit mask.
**	20-jan-2004 (somsa01)
**	    Updated to include the "COPY FROM/INTO... PROGRAM" case.
**	03-feb-2004 (somsa01)
**	    Make sure we pass an i4 as the segment length to get_row().
**	11-Mar-2005 (schka24)
**	    Pass proper incantations to backend for long nvarchar.
**	    Note that long nvarchar stores a "char" count, need to convert
**	    it to bytes for reading.
**	14-Mar-2005 (schka24)
**	    Oops, above isn't enough -- convert incoming UTF8 to UCS2.
**	14-May-2007 (gupsh01)
**	    Pass 0 to adu_nvchr_fromutf8 for NFC normalization request.
*/
void
IICPfbfFmtBlobFrom(LOBULKFROM_HDL *fbf)
{
    II_THR_CB	*thr_cb = IILQthThread();
    II_LBQ_CB	*IIlbqcb = thr_cb->ii_th_session;
    STATUS	status;
    i2		seg_bytes;
    i2		nextsegbytes;
    i4		num_read;
    i4		data_end = 0;
    i4		curseg = 1;	/* current segment - Only used for Err msgs */
    i4		seg_chars;
    i4		put_type;
    i4		seg_flags;	
    char	cursegbuf[15];
    char	phrase_buf[80];
    IILQ_LODATA	*lodata;
    char	*tup_addr;
    DB_DATA_VALUE utf8_dv, rdv;	/* For UTF8 -> UCS2 conversion */
    DB_STATUS	dbstat;

    fbf->iseof = FALSE;		/* Haven't read EOF yet */
    fbf->failure = FALSE;
    put_type = DB_CHR_TYPE;
    if (abs(fbf->tuptype) == DB_LNVCHR_TYPE)
    {
	put_type = DB_NCHR_TYPE;
	/* Use NVARCHAR for the UTF8 to UCS2 conversion, so that the
	** adu utility will return the converted length.
	*/
	rdv.db_datatype = DB_NVCHR_TYPE;
	utf8_dv.db_datatype = DB_BYTE_TYPE;
	/* prec, collID uninteresting;  length, addr set below */
	/* Set up a conversion area if we haven't already.  Worst case
	** is 1 UCS2 char (= 2 octets) for each UTF8 char input.  (It's
	** possible to produce 4 octets out, but that requires a code point
	** above 0xffff which needs at least 3 octets input.)
	*/
	if (fbf->ucv_buf == NULL)
	{
	    fbf->ucv_len = sizeof(i2) + MAX_SEG_FROM_FILE * 2;
	    fbf->ucv_buf = MEreqmem(0, fbf->ucv_len, FALSE, NULL);
	    if (fbf->ucv_buf == NULL)
	    {
		IIlocerr(GE_LOGICAL_ERROR, E_CO0037_MEM_INIT_ERR, II_ERR,
			0, NULL);
		fbf->failure = TRUE;
		return;
	    }
	}
    }

    /* NOTE: This routine won't be called if nullable and value was NULL */

    /* read the 1st segment length in octets */
    num_read = IICPfslFmtSegLen(fbf->file_ptr, fbf->rowcount, fbf->col_name,
				&seg_bytes, fbf->col_num, fbf->get_row);
    if (num_read == -1)
    {
	fbf->iseof = TRUE;
	if (fbf->col_num != 0)
	    fbf->failure = TRUE;
	else
	{
	    /* Tell LIBQ that we already sent DATAEND */
	    lodata = &IIlbqcb->ii_lq_lodata;
	    lodata->ii_lo_flags |= II_LO_END;
	}
	return;
    }

    while (!data_end)
    {
	if (seg_bytes > 0)
	{
	    /* read the segment into the tupbuf */
	    if (fbf->cp_program)
	    {
		i4	lseg_bytes = seg_bytes;

		status = (*fbf->get_row)(&lseg_bytes, (char *) fbf->tupbuf,
					 &num_read);
	    }
	    else
	    {
		status = SIread(fbf->file_ptr, seg_bytes, &num_read,
				(char *) fbf->tupbuf);
	    }

	    if (status != OK || num_read != seg_bytes)
	    {
		/* Error row X: unexpected EOF reading value for segment */
		/* curseg in column COL_NAME */
		char err_buf[15];
		CVla(fbf->rowcount + 1, err_buf);
		STlcopy(ERget(S_CO0060_SEG_VAL), phrase_buf, 80);
		CVla(curseg, cursegbuf);
		IIlocerr(GE_LOGICAL_ERROR, E_CO0043_BLOB_SEG_READ_ERR, II_ERR,
				4, err_buf, fbf->col_name, phrase_buf,
				cursegbuf);
		fbf->iseof = TRUE;
		fbf->failure = TRUE;
		return;
	    }

	    curseg++;

	    /* read the length of the next segment */
	    /* (send a col_num of 1 so we can get an error message on EOF) */
	    num_read = IICPfslFmtSegLen(fbf->file_ptr, fbf->rowcount,
					fbf->col_name, &nextsegbytes, 1,
					fbf->get_row);
	    if (num_read == -1)
	    {
		fbf->iseof = TRUE;
		fbf->failure = TRUE;
		return;
	    }
	}
	else
	    nextsegbytes = 0;

	if (nextsegbytes == 0)
	    data_end = 1;

	/* ship the segment */
	seg_chars = seg_bytes;
        if (seg_bytes == 0)
	{
            seg_flags = 0;
	    tup_addr = NULL;
	}
	else
	{
            seg_flags = (II_DATSEG | II_DATLEN);
	    tup_addr = fbf->tupbuf;
	    /* If long nvarchar, file data is UTF8, need to convert that
	    ** to UCS2.
	    */
	    if (put_type == DB_NCHR_TYPE)
	    {
		utf8_dv.db_data = tup_addr;
		utf8_dv.db_length = seg_chars;
		rdv.db_data = fbf->ucv_buf;
		rdv.db_length = fbf->ucv_len;
		((DB_NVCHR_STRING *) rdv.db_data)->count = (fbf->ucv_len - sizeof(i2)) / sizeof(UCS2);
		dbstat = adu_nvchr_fromutf8(fbf->libq_cb->ii_lq_adf,
				&utf8_dv, &rdv, 0);
		if (dbstat != E_DB_OK)
		{
		    IICPwemWriteErrorMsg(E_CO0039_TUP_FORMAT_ERR,
				     fbf->rowcount + 1, fbf->col_name, "");
		    fbf->failure = TRUE;
		    return;
		}
		tup_addr = (char *) &((DB_NVCHR_STRING *)rdv.db_data)->element_array[0];
		seg_chars = ((DB_NVCHR_STRING *)rdv.db_data)->count;
		/* Tell data putter that it's UCS2, not wchar_t */
		seg_flags |= II_BCOPY;
	    }
	}

	IILQlpd_LoPutData(seg_flags, put_type, 0, tup_addr, seg_chars, data_end);

	if (!data_end)
	{
	    seg_bytes = nextsegbytes;
	}
    }
}

/*{
+*  Name: IICPvcValCheck - validate read in column value via adc_valchk()
**
**  Description:           Send the aligned read in column value to
**			   adc_valchk() to ensure that the value is legal.
**			   If it's a DB_CHR_TYPE (plus or minus) and there
**			   are non-printing characters, convert them to
**			   blanks and declare the value OK.
**
**  Inputs:
**	col_aligned	- aligned column value for validation
**	copy_map	- the copy map for this column
**	copy_blk	- the copy block for this table
**
**  Outputs:
**	none
**
**  Returns: OK, FAIL
**
**  History:
**	mar-1993 (mgw)	- Written
*/
STATUS
IICPvcValCheck( II_LBQ_CB *IIlbqcb, char *col_aligned, 
		II_CP_MAP *copy_map, II_CP_STRUCT *copy_blk )
{
    DB_DATA_VALUE	dbv;
    char		*str;
    STATUS		status;

    dbv.db_data = (PTR) col_aligned;
    dbv.db_datatype = copy_map->cp_tuptype;
    dbv.db_length = copy_map->cp_tuplen;
    dbv.db_prec = copy_map->cp_tupprec;
    if ((status = adc_valchk(IIlbqcb->ii_lq_adf, &dbv)) != OK)
    {
	/*
	** If the value is invalid because of non-printing
	** characters in a character field, convert them to
	** blanks.
	*/
	if ((IIlbqcb->ii_lq_adf->adf_errcb.ad_errcode ==
			    E_AD1014_BAD_VALUE_FOR_DT) &&
	    ((copy_map->cp_tuptype == DB_CHR_TYPE) ||
	     (copy_map->cp_tuptype == -DB_CHR_TYPE)))
	{
	    for (str = dbv.db_data; *str != NULLCHAR; CMnext(str))
	    {
		/*
		** Convert any non-printing chars to blanks. If
		** the nonprinting char is a double-byte, then
		** put in two blanks.
		*/
		if (!CMprint(str))
		{
		    if (CMdbl1st(str))
			*str++ = ' ';
		    *str = ' ';
		}
  	    }
	    copy_blk->cp_blanked++;
	    status = OK;
	}
 
	if (status != OK)
	{
	    char err_buf[15];
	    CVla(copy_blk->cp_rowcount + 1, err_buf);
	    if (IIlbqcb->ii_lq_adf->adf_errcb.ad_errcode ==
			    E_AD1014_BAD_VALUE_FOR_DT)
		IIlocerr(GE_DATA_EXCEPTION, E_CO0032_BAD_DATA,
			 II_ERR, 2, copy_map->cp_name, err_buf);
	    else
		IIlocerr(GE_DATA_EXCEPTION, E_CO0023_COPY_INTRNL_ERR,
			 II_ERR, 1, err_buf);
	    copy_blk->cp_warnings++;
	}
    }
    return (status);
}


/*{
+*  Name: IICPbdrBlobDbRead -	Read tuple data from the DBMS during COPY INTO.
**
**  Description:	This routine reads the tuple data from the GCA
**			message buffer. If the buffer doesn't have enough
**			data in it to process the next column, then more
**			is gotten.
**
**			If the data left in the GCA buffer contains a full
**			column or the beginning of a blob column then the
**			tuple pointer is set directly into the GCA buffer.
**			If only a partial column is found then the routine
**			saves that part in copy_blk->cp_tupbuf and reads the
**			next GCA message block.  The rest of the column is
**			then concatenated to the part saved away and the tuple
**			pointer is set to copy_blk->cp_tupbuf.
**
**			Note that the client routine (IICPfbrFmtBlobRead()) is
**			requesting a particular column pointed at by the copy
**			map in the cmap argument. The main loop in IIcopy()
**			ensures that these columns are in ascending order to
**			aviod problems inherent in outputting the last column
**			first when there are intervening blobs and we are not
**			using coupons, but there's no guarantee that some
**			columns won't be skipped in the format. Thus this
**			routine must keep track of which column is being
**			requested and which was requested last and skip over
**			any intervening columns. This is necessary since the
**			DBMS will just sent the full tuple in the proper column
**			order, regardless of what we ask for in the format.
**
**  Inputs:
**	copy_blk	- copy control block
**	cpy_bufh	- copy buffer header
**	tuple		- addr of tuple buffer pointer
**	cmap		- copy map for current column
**
**  Outputs:
**	*tuple		- set to the next tuple buffer
**
**	Returns: OK, ENDFILE, FAIL
**
**  History:
**	21-may-93 (mgw)
**	    Written - borrowed from IIcpdbread().
**      14-jun-96 (loera01)
**          Modified so that decompression for variable-length datatypes is
**          perfomed in LIBQ, rather than LIBQGCA.
**      16-jul-96 (loera01)
**          Modified so that single columns may be copied if the row
**          contains one or more compressed varchars.  Fixes bug 77127.
**      13-aug-96 (loera01)
**	    Modified to correctly handle partial tuples.  Fixes bug 77129.
**          Fixed memory leaks for compressed varchar case. 
**	28-Aug-97 (thaju02) Bug #85025 - With retrieval of tuple data, 
**	    where column spans over 2 gca buffers, neglected to 
**	    increment cgc->cgc_result_msg.cgc_d_used and 
**	    bufh->cpy_bytesused with bytesleft in gca buffer, resulting
**	    in unknown exception.
**	17-Oct-2001 (thaju02)
**	    After faking libq into thinking we got the blob, reset the
**	    bufh->cpy_dbv.db_length to cgc->cgc_result_msg.cgc_d_length.
**	    Since the blob may span multiple buffers, we need to update
**	    our local info (bufh) with that of the current libq cgc buffer.
**	    (B105381)
**	17-Apr-2006 (thaju02) (B115961)
**	    Ascii copy table into file may hang. Need to account for col's
**	    data which span multiple gca buffers.

*/
i4
IICPbdrBlobDbRead
( 
    II_LBQ_CB		*IIlbqcb, 
    II_CP_STRUCT	*copy_blk, 
    II_CPY_BUFH		*cpy_bufh, 
    PTR			*tuple,
    II_CP_MAP		*cmap, 
    i4		*lastattr
)
{
    IICGC_DESC		*cgc = IIlbqcb->ii_lq_gca;
    II_CPY_BUFH		*bufh = cpy_bufh;
    i4			msg_type;
    i4		bytesleft;
    i4		bytescopied;
    char		*bptr;
    i4		tcount = 0;
    char		*tptr = (char *)(copy_blk->cp_tupbuf); 
    i4		part_read = 0;
    i4		maptuplen;
    i4		cur_tuplen;
    DB_DT_ID		cur_tuptyp;
    ROW_DESC		*rdesc;
    i2			nulblob = 0;
    bool		support_compression;
    i4                  compressed_size;
    i4			compressed_type;

    rdesc = &IIlbqcb->ii_lq_retdesc.ii_rd_desc;

    if (cmap->cp_tuplen < 0)
	maptuplen = 0;
    else
	maptuplen = cmap->cp_tuplen;
    support_compression = IIlbqcb->ii_lq_retdesc.ii_rd_desc.rd_flags &
        RD_CVCH ? TRUE : FALSE;
    /*
    ** Get data for the next tuple from the DBMS.
    */
    for (;;)		/* loop until a complete tuple is received */
    {
        if (! tcount)
        {
            compressed_type = IICGC_NON_VCH;
            compressed_size = 0;
        }

	if (*lastattr + 1 >= rdesc->rd_numcols) /* if at end of previous row */
	{
	    *lastattr = -1;
	}
	bptr = (char *)bufh->cpy_dbv.db_data + bufh->cpy_bytesused;
	bytesleft = bufh->cpy_dbv.db_length - bufh->cpy_bytesused;
 
	if (bytesleft == 0)
	{   /* Time to read a new message buffer */
            msg_type = IIcgcp5_read_data(cgc, &bufh->cpy_dbv);
	    if (msg_type == GCA_RESPONSE)	/* a RESPONSE block received */
	    {
		if ( ! (IIlbqcb->ii_lq_flags & II_L_CONNECT) )
		/* msg_type might be faked by IIcgcp5_read_data */
                {
		    return FAIL;
                }
		return (ENDFILE);
	    }
 
	    bptr = (char *)bufh->cpy_dbv.db_data;
	    bytesleft = bufh->cpy_dbv.db_length;
	    bufh->cpy_bytesused = 0;
	}

        /*
        **  If this is a compressed variable-length datatype, get its
        **  compressed size.
        */
        if (support_compression && (! tcount))
        { 
            cur_tuptyp = rdesc->RD_DBVS_MACRO(*lastattr+1).db_datatype;
            switch (abs(cur_tuptyp))
            {
		case DB_NVCHR_TYPE:
                case DB_TXT_TYPE:
                case DB_VCH_TYPE:
                case DB_VBYTE_TYPE:
                case DB_LTXT_TYPE:
                {
	            if (abs(cur_tuptyp) == DB_NVCHR_TYPE)
	                compressed_type = (cur_tuptyp < 0 ? 
	                IICGC_NVCHR : IICGC_NVCHRN);
                    else
	                compressed_type = (cur_tuptyp < 0 ? 
	                IICGC_VCH : IICGC_VCHN);
                    compressed_size = IIcgc_find_cvar_len(bptr,compressed_type);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

	if ((*lastattr + 1) == cmap->cp_attrmap) /* get next value */
	{
	    /* make sure the whole column value is there */
	    if (((bytesleft + tcount) < maptuplen &&     /* put partial to bfr, get more */
                ! compressed_type) || 
                ((bytesleft + tcount) < compressed_size &&
                 compressed_type))
	    {
		MEcopy (bptr, (u_i2)bytesleft, tptr);
		tptr += bytesleft;
		tcount += bytesleft;
		bytescopied = bytesleft;
		/*
		** bug #85025, thaju02, 28-Aug-97
		*/ 
		bufh->cpy_bytesused += bytesleft;
		cgc->cgc_result_msg.cgc_d_used += bytesleft;
		continue;	/* get more data */
	    }
	    if (tcount != 0)	/* partial column in tupbuf */
	    {
                if (compressed_type)
                {
		    bufh->cpy_bytesused += (compressed_size - bytescopied);
		    cgc->cgc_result_msg.cgc_d_used += (compressed_size - 
                        bytescopied);
		    MEcopy(bptr, (u_i2)(compressed_size - bytescopied), tptr);
                }
                else
                {
		    bufh->cpy_bytesused += (maptuplen - tcount);
		    cgc->cgc_result_msg.cgc_d_used += (maptuplen - tcount);
		    MEcopy(bptr, (u_i2)(maptuplen - tcount), tptr);
                }
                *tuple = copy_blk->cp_tupbuf;
	    }
	    else
	    {
                if (compressed_type)
                {
                    MEcopy((PTR)bptr,compressed_size,(PTR)tptr);
                    /*
                    ** If nullable datatype, place null indicator at end of column.
                    */
                    if (cur_tuptyp < 0) 
                    {
                        tptr[maptuplen-1] = bptr[compressed_size-1];
                    }
		    bufh->cpy_bytesused += compressed_size;
		    cgc->cgc_result_msg.cgc_d_used += compressed_size;
                    *tuple = copy_blk->cp_tupbuf;
                }
                else
                {
  		    bufh->cpy_bytesused += maptuplen;
		    cgc->cgc_result_msg.cgc_d_used += maptuplen;
		    *tuple = bptr;
                }
	    }
	    *lastattr = cmap->cp_attrmap;
	    return (OK);
	}
	else	/* read off intervening values */
	{
            if (compressed_type)
            {
	        cur_tuplen = (i4) compressed_size - part_read;
            }
            else
            {
	        cur_tuplen = (i4) (rdesc->RD_DBVS_MACRO(*lastattr+1).db_length
					- part_read);
            }
	    cur_tuptyp = rdesc->RD_DBVS_MACRO(*lastattr+1).db_datatype;

	    part_read = 0;
	    if (IIDT_LONGTYPE_MACRO(cur_tuptyp)) /* read off BLOB */
	    {
		/*
		** Fake LIBQ into thinking we're inside a datahandler
		** and doing a get
		*/

		/* Make sure libq knows what column we're on */
		IIlbqcb->ii_lq_retdesc.ii_rd_colnum = *lastattr + 1;

		/* Call the handler to print this Blob Column */
		IILQldh_LoDataHandler(1, &nulblob, IICPfbFlushBlob, (PTR) NULL);

		/* reset flags and clean up */
		bufh->cpy_bytesused = cgc->cgc_result_msg.cgc_d_used;
		bufh->cpy_dbv.db_length = cgc->cgc_result_msg.cgc_d_length;
		++*lastattr;
	    }
            else
            {
                if (bytesleft < cur_tuplen)
		{
		    bufh->cpy_bytesused += bytesleft;
		    part_read = bytesleft;
                }
		else
		{
		    bufh->cpy_bytesused += cur_tuplen;
		    cgc->cgc_result_msg.cgc_d_used += cur_tuplen;
		    ++*lastattr;
		}
	    }
	}
    }
}

/*{
+*  Name: IICPfbFlushBlob - Data Handler for flushing a blob from the DBMS
**
**  Description:            This routine calls IILQled_LoEndData() to flush
**                          the current blob from the DBMS into oblivion
**                          since it wasn't asked for on the Format line.
**  Inputs:
**	none
**
**  Outputs:
**	none
**
**  Returns: void
**
**  History:
**	jun-1993 (mgw)	- Written
*/
void
IICPfbFlushBlob()
{
	/* Flush the blob */
	IILQled_LoEndData();
}

/*{
+*  Name: IICPsebSendEmptyBlob - Data Handler for sending an empty blob to DBMS
**
**  Description:                 This routine calls IILQlpd_LoPutData() with
**                               an empty segment buffer and DATAEND set to
**                               TRUE to send an empty blob to the DBMS.
**  Inputs:
**	none
**
**  Outputs:
**	none
**
**  Returns: void
**
**  History:
**	jun-1993 (mgw)	- Written
*/
void
IICPsebSendEmptyBlob()
{
	/* Send an empty blob */
	IILQlpd_LoPutData(0, DB_CHR_TYPE, 0, (char *) 0, 0, 1);
}
/*{
+*  Name: IICPdecompress - Decompress for bulk COPY
**
**  Description: This routine decompresses a tuple which contains no large
**               objects.
**  
**  Inputs:
**      rdesc     -     Row descriptor.
**	bptr      -     The compressed tuple.
**      len	  -     Tuple length.
**      bytesleft -     Bytes left in GCA buffer.
**
**  Outputs:
**	dif       -     Pointer to the difference in byte length between the 
**                      compressed and decompressed tuple.
**      tuple_ptr -     Pointer to the decompressed tuple.
**      partial   -     Indicates this is a partial tuple.
**
**  Returns: void
**
**  History:
**	04-jun-96 (loera01)
**	    Created.
**      13-aug-96 (loera01)
**          Fixed memory leaks by setting decompress_buf to passed tuple_ptr
**          argument.
**	21-Dec-01 (gordy)
**	    GCA tuple descriptor is now only available for COPY FROM, so
**	    replaced cgc parameter with a ROW_DESC.
**      03-Dec-2003 (gupsh01)
**          Modified copy code so that nchar and nvarchar types will now use
**          UTF8 to read and write from the text file for a formatted copy.
*/
void 
IICPdecompress( ROW_DESC *rdesc, PTR bptr, i4  len, i4  bytesleft, i4  *dif, 
    PTR *tuple_ptr, bool *partial )
{
    char                *decompress_buf;
    i4                  size;
    DB_DATA_VALUE	*cur_att;
    i4                  compressed_type;
    i4                  i;

    *dif = 0;
    *partial = FALSE;
    decompress_buf = *tuple_ptr;

    for( i = 0; i < rdesc->rd_gca->rd_gc_l_cols; i++ ) 
    {
	cur_att = (DB_DATA_VALUE *)&rdesc->rd_gca->rd_gc_col_desc[i].rd_dbv;

        if (! bytesleft)
        {
            *partial = TRUE;
            break; 
        }
#ifdef axp_osf
        switch (abs(I2CAST_MACRO(&cur_att->db_datatype)))
#else         
        switch (abs(cur_att->db_datatype))
#endif        
        {
	    case DB_NVCHR_TYPE:
            case DB_TXT_TYPE:
            case DB_VCH_TYPE:
            case DB_UTF8_TYPE:
            case DB_VBYTE_TYPE:
            case DB_LTXT_TYPE:
            {
#ifdef axp_osf
		if (abs(I2CAST_MACRO(&cur_att->db_datatype))
		    == DB_NVCHR_TYPE)
#else
	        if (abs(cur_att->db_datatype) == DB_NVCHR_TYPE)
#endif
#ifdef axp_osf
                    compressed_type = 
			 I2CAST_MACRO(&cur_att->db_datatype) < 0 ?
#else         
                    compressed_type = cur_att->db_datatype < 0 ?
#endif        
	            IICGC_NVCHR : IICGC_NVCHRN;
                else
#ifdef axp_osf
                    compressed_type = 
			 I2CAST_MACRO(&cur_att->db_datatype) < 0 ?
#else         
                    compressed_type = cur_att->db_datatype < 0 ?
#endif        
	            IICGC_VCH : IICGC_VCHN;
		if (bytesleft == sizeof(char))
		{
		  /* fragmented size */
		  *partial = TRUE;
		  break;
		}
	        size = IIcgc_find_cvar_len((PTR)bptr, compressed_type);
                MECOPY_VAR_MACRO((PTR)bptr, size, (PTR)decompress_buf);
#ifdef axp_osf
                if (I2CAST_MACRO(&cur_att->db_datatype) < 0)
#else         
                if (cur_att->db_datatype < 0)
#endif        
                {
                    decompress_buf[cur_att->db_length-1]
                    = bptr[size-1];
                }
		if (bytesleft < size)
		{
		  *partial = TRUE;
		  break;
		}
                bytesleft -= size;
                bptr += size;
                *dif += cur_att->db_length - size;
                decompress_buf += cur_att->db_length;
                break;
            }
            default:
            {
		if (bytesleft < cur_att->db_length)
		{
		  *partial = TRUE;
		  break;
		}
                MECOPY_VAR_MACRO((PTR)bptr,
                    cur_att->db_length,
                    (PTR)decompress_buf);
                bytesleft -= cur_att->db_length;
                bptr += cur_att->db_length;
                decompress_buf += cur_att->db_length;
                break;
            }
        }

	if (*partial) break;
    }
}

/*{
+*  Name: IIcpdbgopen - Open copy debug file
**
**  Description:
**	Open copy debug file.
**
**  Inputs:
**	copy_blk	- copy control block
**
**  Outputs:
**	Returns: OK
-*
**  Side Effects:
**	none
**
**  History:
**      24-jan-2005 (stial01)
**          Created from IIcplgopen
*/
i4
IIcpdbgopen(copy_blk)
II_CP_STRUCT	*copy_blk;
{
    LOCATION	loc;
    char	locbuf[MAX_LOC +1];	/* Used to hold location string */
    i4	filewidth;
    i4		filetype;
 
    STcopy( copy_blk->cp_dbgname, locbuf );
    LOfroms(PATH & FILENAME, locbuf, &loc);
 
    if (SIfopen(&loc, "a", SI_TXT, 80, &copy_blk->cp_dbg_ptr) != OK)
	copy_blk->cp_dbg_ptr = NULL;
    else
    {
	SIfprintf(copy_blk->cp_dbg_ptr, "Open copy debug file\n");
    }
    return (OK);
}

/*{
+*  Name: IIcpdbgclose - Close copy debug file.
**
**  Description:
**	Close the copy debug file 
**
**  Inputs:
**	copy_blk	- copy control block
**
**  Outputs:
**	Returns: OK
-*
**  Side Effects:
**	none
**
**  History:
**      24-jan-2005 (stial01)
**          Created from IIcplgclose
*/
i4
IIcpdbgclose(copy_blk)
II_CP_STRUCT	*copy_blk;
{
    LOCATION	loc;
    char	locbuf[MAX_LOC +1];	/* Used to hold location string */
    i4		status;
 
    if (copy_blk->cp_dbg_ptr)
    {
	SIfprintf(copy_blk->cp_dbg_ptr, "Closing copy debug file\n");
	status = SIclose(copy_blk->cp_dbg_ptr);
    }
    copy_blk->cp_dbg_ptr = NULL;

    return (OK);
}

/*{
+*  Name: IIrm_ctrl_m - Remove ctrl<M>(0x0d) from column data.
**
**  Description:
**	Remove ctl<M> from column data if it was followed by \n delimitor.
**  Inputs:
**      copy_map        - the copy map for this column,to update field length 
**      row_ptr         - current position in cp_rowbuf
**      start_dom       - pointer to start of the column
**
**  Outputs:
**	none
-*
**  Side Effects:
**	none
**
**  History:
**      07-Jul-2005 (mutma03)
**        Initial code . 
*/
void IIrm_ctrl_m(II_CP_MAP *copy_map, char **row_ptr, char *startdom_ptr)
{
    CMprev(*row_ptr,startdom_ptr);
    CMprev(*row_ptr,startdom_ptr);
    if (*row_ptr[0] == '\r')
    {
	CMcpychar("\n", *row_ptr);
	CMnext(*row_ptr);
	copy_map->cp_fieldlen--;
    }
    else
    {
	CMnext(*row_ptr); CMnext(*row_ptr);
    }
}
