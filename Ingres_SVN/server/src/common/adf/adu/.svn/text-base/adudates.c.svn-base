/*
** Copyright (c) 1986, 2001, 2008 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cm.h>
#include    <cv.h>
#include    <me.h>
#include    <st.h>
#include    <tm.h>
#include    <tmtz.h>
#include    <tr.h>
#include    <iicommon.h>
#include    <adf.h>
#include    <ulf.h>
#include    <adfint.h>
#include    <aduint.h>
#include    <adftrace.h>
#include    <adudate.h>
#include    <clfloat.h>
#include    "adu1dates.h"
#include    "adutime.h"
#include    "adu2date.h"
#include    "adumonth.h"
/*  [@#include@]... */

/**
**
**  Name: ADUDATES.C - high-level date functions.
**
**  Description:
**        This file contains the routines used to implement high-level
**	date functions.
**
**	This file defines:
**	    adu_1datetodate -	Convert a date to a date.  This is basically
**				used to copy a date value into a tuple buffer.
**          adu_14strtodate() -	Convert a string to a date.
**	    adu_6datetostr() -	Convert a date to a string.
**	    adu_3datehmap() -	Map a date value to its histogram value.
**	    adu_1dayofweek() -	Get the abbreviation for the day of the week
**				corresponding to an absolute date given as
**				input.
**	    adu_1date_addu() -	Add two dates.
**	    adu_7date_subu() -	Subtract one date from another.
**	    adu_date_mul() -	Multiply interval by number
**	    adu_date_div() -	Divide interval by number
**	    adu_date_plus() -	Does not a lot but better be an interval
**	    adu_date_minus() -	Subtract interval from 0 interval
**	    adu_intrvl_in() -	Gets the number of "date units" in an Ingres
**				date.
**	    adu_dtruncate() -	Truncates an Ingres date to a given granularity.
**	    adu_dextract() -	Extract component of a date and return as an
**				integer.
**	    adu_dgmt() -	Converts an Ingres absolute date into a char(25)
**				in the form `yyyy_mm_dd hh:mm:ss GMT  '.
**	    adu_hrtimetodate() -Convert SYSTIME to Ingres internal date.
**	    adu_date_isdst() -	Returns 1 if the date falls in daylight savings
**				time.
**	    adu_21ansi_strtodt()-Converts a valid text input to internal
**				storage format for ANSI datetime data type
**
**  Function prototypes defined in ADUINT.H except ad0_13tab_look(), which
**  is defined in ADU2DATE.H
**
**  History:
**      Revision 3.22.177.1  85/07/13  00:49:43  greg
**      date_hmp() must align DATETRNL variable before referencing members
**      dn_year and/or dn_month else can get bus error.  ATTIS bug 019.
**      
**      Revision 3.22.177.0  85/06/21  11:49:36  wong
**      Used 'I1_CHECK_MACRO()' to hide unsigned character
**      problems with 'AD_DATENTRNL' member 'dn_highday'.
**      
**      03/25/85 (lichtman) -- $%^*$#*%$ Someone changed the variable i to the
**		    constant 1 in the Morrison/Knudsen calculations.  Changed
**		    it back to i.  Fix of bug #4729.
**      03/25/85 (lichtman) -- Fixed bug #4918.  Add_date was adding month and
**		    year from b to a, then checking whether a's month or year
**		    were set in order to tell whether to adjust a's month.
**		    Changed it to remember whether a's month and year were
**                  originally set to anything.
**      05/29/85 (lin) -- Changed dates() to disallow the user enter more than
**		    60 minutes or 60 seconds.
**      05/29/85 (lin)      -- Fixed bug #4850, which didn't convert
**		    "0:0:0 GMT" to local time zone.
**      8/2/85 (peb)        Added multinational support data.
**      12/03/85 (seputis)  -- added dtruncate, dextract, ad0_11fromgregorian,
**			       ad0_8getdateclass, ad0_10togregorian, changed
**			       intrvl_in to not default the unit spec string
**			       to year.
**	25-may-86 (ericj)
**	    Converted for Jupiter.
**	      Deleted the global writable Datetmplt[].
**	    Replaced with global read-only date templates in datetab.roc.
**	    There is now an array with all possible date formats for each
**	    of our supported date formats.  
**	      Removed the error numbers defined in this file and redefined
**	    in adf.h
**	19-jun-86 (ericj)
**	    Removed histogram  min and max routines and constants.
**	    These functions and constants can now be found in the
**	    external interface histogram files and adfhist.h respectively.
**	2-jul-86 (ericj)
**	    Added adu_1datetodate() routine for copying a date data value
**	    into a tuple buffer for the adc_cvinto interface.
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	16-sep-86 (thurston)
**	    Removed #include <ex.h> since it never seems to be needed.
**      25-feb-87 (thurston)
**          Made changes for using server CB instead of GLOBALDEF/REFs.
**	17-mar-87 (thurston)
**	    Added cast to pacify Whitesmith compiler's READONLY stuff.
**	07-jun-87 (thurston)
**	    Added the adu_dgmt() routine:  INGRES `date_gmt()' function.
**	13-jul-87 (thurston)
**	    Changed adu_intrvl_in() to use the symbolic constants
**	    AD_13DTE_DAYPERMONTH and AD_14DTE_DAYPERYEAR instead of hardwired
**	    numbers ... especially since the hardwired numbers were wrong!
**	05-jul-87 (thurston)
**	    Added code to adu_dextract() (the `date_part' function) that allows
**          the input to be an `empty date'.  In this case, the routine just
**          returns zero.  (This change was originally added to the 4.0 or 5.0
**          code.)  Also, this routine now allows the result to be an i1, i2, or
**          i4; although the function instance table should limit this to i2's. 
**	05-jul-87 (thurston)
**	    Added code to adu_dtruncate() (the `date_trunc' function) that
**	    allows the input to be an `empty date'.  In this case, the routine
**	    just returns an empty date.  (This change was originally added to
**	    the 4.0 or 5.0 code.) 
**	13-aug-87 (thurston)
**	    Allowed the input to `date_gmt()' to be the empty date ... return a
**	    blank string. 
**	15-sep-87 (thurston)
**	    Changed ad0_6valcopy() to return a bool, and made all of the returns
**          consistently return a bool.  (Some used to return DB_STATUS, some
**          nats, etc.)  Also fixed bug that looked at wrong character.  Also
**	    fixed all calls to this routine to expect a bool to be returned.
**      29-sep-87 (thurston)
**	    Changed the static routine ad0_1cur_init() to return a DB_STATUS
**	    and take an ADF_CB as an argument so that it could use the query
**	    constant `_bintim' instead of calling TMnow() directly.  This is
**	    important since we want `date("now")' to evaluate to the same value
**	    for the life of the query.  Thus, since these changes were made, the
**          adu_4date() function, which calls ad0_1cur_init() needed to be
**          modified appropriately.
**	06-nov-87 (thurston)
**	    Added a missing return () stmt at the end of ad0_1cur_init().
**	24-nov-87 (thurston)
**	    Converted CH calls into CM calls.
**	22-jan-88 (thurston)
**	    Modified adu_dgmt() to produce output as `1988_01_22' instead of
**	    `1988.01.22' since the previous method conflicted with the German
**	    date format.  Also, in adu_14strtodate(), I added the underscore as
**	    another acceptable delimiter.
**	20-may-88 (thurston)
**	    Changes to static routine ad0_dtcvrt():  (1) Corrected the erroneous
**	    comment for the 'j' case; this had said "ddmmyy", when in reality it
**	    should be "YYMMDD".  (2) Added the 'g' case for the German date
**	    formats; DMMYY, DDMMYY, DMMYYYY, and DDMMYYYY.
**	20-may-88 (thurston)
**          Fixed bug in adu_6datetostr() that occurred when the string
**	    representation of a time interval was too long to fit into the
**	    destination string (which should be 25 characters max).  This is
**	    supposed to truncate the string after the last piece that completely
**	    fits but was just truncating the string instead.
**	    Example:  The string
**			"22 yrs 11 mos 14 days 5 hrs"
**	    was being returned as
**			"22 yrs 11 mos 14 days 5 h"
**          instead of
**			"22 yrs 11 mos 14 days"
**	01-sep-88 (thurston)
**	    Increased size of local buf in adu_14strtodate() from DB_MAXSTRING+1
**	    to DB_GW4_MAXSTRING+1.
**	13-jul-89 (jrb)
**	    Fixed bug where a time spec after "now" and "today" was not
**	    caught as an invalid date.
**	16-aug-89 (jrb)
**	    Fixed bug introduced by removal of preliminary normalization
**	    routine.  I am going to reinstate this routine and re-write it (it
**	    is horribly slow as it is written).
**	25-sep-89 (jrb)
**	    Fix bug #8155; dow function returns wrong results for some inputs.
**	    Problem is that date should be adjusted for time zone and normalized
**	    before computing function.
**	26-sep-89 (jrb)
**	    Changed date_trunc routine to minutes and hours truncation to first
**	    adjust to local time and then to normalize.  This is because not
**	    all GMT differentials are a multiple of 60 minutes.
**	29-sep-89 (jrb)
**	    Got rid of one of the huge local stack variables in adu_14strtodate
**	    since the stack consumption was causing problems with STAR.
**	26-mar-90 (jrb)
**	    Fixed bug 20807: AV when converting invalid date followed by time.
**	16-may-90 (jrb)
**	    Added an array which parallels d_val_array.  It keeps track of the
**	    original source string sizes of the values in d_val_array.  Then
**	    made use of this and fixed bug #21403: ISO yymmdd format was
**	    defaulting year to 1900 instead of current year.
**	21-jun-90 (jrb)
**	    South Australian timezone bug: we didn't handle a #-of-mins offset
**	    from GMT that was not a multiple of 60.  Changed units of
**	    dv_timezone to be minutes instead of hours and adjusted code as
**	    necessary.  Bug #30745.
**	06-sep-90 (jrb)
**	    Use adf_timezone (session specific time zone) instead of calling
**	    TMzone throughout this file.
**	14-dec-90 (jrb)
**	    Fixed bug where date_trunc('weeks', date) would return 31-dec
**	    instead of 30-dec whenever 'date' was between 30-dec on a leap year
**	    through 5-jan on the following year.  (Bug number 30328)
**	29-aug-91 (vijay)
**	    I1_CHK_MACRO needed for dn_highday which is a char. (bug 39552)
**	25-sep-91 (stevet)
**	    Bug 39998: Fix date_trunc() problem with negative timezone setting.
**	28-oct-91 (stevet)
**	    Fixed problem with month_interval + absolute_date, this problem 
**	    occurred when the resulting date is a new year. (bug 40558)
**      06-may-92 (stevet)
**          Added new date formats YMD, MDY and DMY to ad0_dtcvrt() 
**          and adu_6datetostr().  
**      23-aug-92 (stevet)
**          Added new timezone support.
**      31-dec-1992 (stevet)
**          Added function prototypes.
**      07-jun-1993 (stevet)
**          Fixed problem with '- month_interval' + absolute_date, this
**          problem occurred when the resulting date is the previous
**          year. (B48678)  Also fixed 'month_interval' + absolute_date
**          problem where incorrect date can return. (B49376)
**      16-jun-1993 (stevet)
**          Make special arragement for handling empty date and 
**          date-without-time date datatypes when calculate histogram
**          values.  In case of empty date, the histogram value will be 
**          set to less than MIN_INV_YR and for date-without-time, 
**          timezone offset will be adjusted. (B44059, B47291)
**      14-jul-93 (ed)
**          replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	25-aug-93 (ed)
**	    remove dbdbms.h
**	07-apr-95 (pchang)
**	    Fixed bug which caused time intervals that were within the
**	    25-characters limit to be truncated in time-interval units so as
**	    to fit into user-specified string buffers.  They should simply be
**	    string truncated to avoid unacceptable data loss when used in a
**	    copy/into. (b51185).  Examples:
**	    (1) if (str_dv->db_length == 12)
**	    the time interval
**			"10 yrs 11 mos"
**	    were being returned as
**			"10 yrs"
**	    (2) if (str_dv->db_length == 6)
**	    the time intervals
**			"100 yrs",  "10 days",  "11 mins"  and  "12 secs"
**	    were being returned as empty strings.
**	13-nov-95 (nick)
**	    Add support for II_DATE_CENTURY_BOUNDARY.
**	27-feb-96 (thoda04)
**	    Nudge up with an explicit cast the definition of AD_100_IDAYPER100
**	    so that intermediate arithmetic doesn't overflow on 16 bit machines.
**	16-mar-1996 (angusm)
**	    Add support for MULTINATIONAL4
**	29-may-96 (chech02)
**	    cast arguments to i4  in STprintf(%d) calls for windows 3.1 port.
**	31-jul-96 (nick)
**	    Changes to stop things breaking come year 2000.
**	10-feb-1998 (hanch04)
**	    Changes to support dates from 01-jan-0001 to 31-dec-9999,
**	    including updating MAX_INV_YR,MIN_INV_YR, added check to
**	    see if a 4 digit year was entered with leading zeros,
**	    1582 < year < 2382 changed to 0001 < year < 9999
**	    dates between, but not including  02-sep-1752 and 14-sep-1752
**	    do not exist.
**	13-feb-1998 (walro03)
**	    aduadtdate.h and adudate.h are redundant.  aduadtdate.h deleted and
**	    references (such as here) changed.
**	23-jun-98 (kitch01)
**	    Bug 91559. In add_date() ensure both input dates are normalized 
**	    before return.
**	26-apr-1999 (hanch04)
**	    Added casts to quite compiler warnings
**      28-apr-1999 (hanch04)
**          Replaced STlcopy with STncpy
**	07-Jun-1999 (shero03)
**	    Add ISO-6801 week in dextract
**	24-Jun-1999 (shero03)
**	    Add ISO4 date format.
**	15-jan-1999 (kitch01)
**	    Bug 94791. Amend adu_14strtodate() to allow '-' and digits if a
**	    timezone has been specified.
**	    Also amended ad0_dtcvrt() as timezone offsets are now specified in 
**	    minutes in the Adu_datename[] table.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      19-May-2000 (hanal04) Bug 101064 INGSRV 1145
**          For date("now") case force a call to TMnow() to override the
**          _bintim value which is constant for the duration of a TX.
**	19-mar-2001 (somsa01)
**	    In adu_14strtodate(), to save stack space, allocate space for
**	    the temp buffer array dynamically.
**	26-apr-01 (inkdo01)
**	    Added one more entry (a -1 entry) to month_hours array for null 
**	    dates with a month value of 0.
**       5-Dec-2002 (hanal04) Bug 109218 INGSRV 2021
**          Handle the subtraction of negative time intervals when
**          calculating the number of days in a month in ad0_5sub_date().
**	20-apr-04 (drivi01)
**		Added routines to handle date coercions to/from unicode in
**      functions adu_14strtodate and adu_6datetostr.
**	01-aug-06 (gupsh01)
**	    Added prec to ad0_dtcvrt() function to support the interpretation
**	    of the input time fraction value as per the precision.
**	13-sep-2006 (gupsh01)
**	    Enable proper ANSI date/time coercion based on ANSI rules.
**	02-feb-2007 (gupsh01)
**	    Added support for timezone_hour and timezone_minute.
**	24-Sep-2007 (kiria01) b119051
**	    Corrected handling of date parsing relating to fractional seconds
**	    and the imposing of size limits.
**	08-Feb-2008 (kiria01) b119885
**	    Change dn_time to dn_seconds to avoid the inevitable confusion with
**	    the dn_time in AD_DATENTRNL.
*/

/*  Static variable references	*/
#define  MAX_INV_YR   AD_26DTE_MAX_INT_YR
				/* Maximum year value in date interval */
#define  MIN_INV_YR   AD_25DTE_MIN_INT_YR
				/* Minimum year value in date interval */
#define  DTE_ABS_NOW		 1
#define  DTE_OFFSET_OR_TODAY	-1

/* Literals for "date_part" function calls. */
static DB_DATA_VALUE year_spec = {"year", 4, DB_CHA_TYPE, 0, 0};
static DB_DATA_VALUE quarter_spec = {"quarter", 7, DB_CHA_TYPE, 0, 0};
static DB_DATA_VALUE month_spec = {"month", 5, DB_CHA_TYPE, 0, 0};
static DB_DATA_VALUE week_spec = {"week", 4, DB_CHA_TYPE, 0, 0};
static DB_DATA_VALUE weekiso_spec = {"iso-week", 8, DB_CHA_TYPE, 0, 0};
static DB_DATA_VALUE day_spec = {"day", 3, DB_CHA_TYPE, 0, 0};
static DB_DATA_VALUE hour_spec = {"hour", 4, DB_CHA_TYPE, 0, 0};
static DB_DATA_VALUE minute_spec = {"minute", 6, DB_CHA_TYPE, 0, 0};
static DB_DATA_VALUE second_spec = {"second", 6, DB_CHA_TYPE, 0, 0};
static DB_DATA_VALUE microsecond_spec = {"microsecond", 11, DB_CHA_TYPE, 0, 0};
static DB_DATA_VALUE nanosecond_spec = {"nanosecond", 10, DB_CHA_TYPE, 0, 0};
static DB_DATA_VALUE tzhour_spec = {"timezone_hour", 13, DB_CHA_TYPE, 0, 0};
static DB_DATA_VALUE tzminute_spec = {"timezone_minute", 15, DB_CHA_TYPE, 0, 0};



# ifdef xDEBUG
static	i4	    dum1;   /* Dummy to give to ult_check_macro() */
static	i4	    dum2;   /* Dummy to give to ult_check_macro() */
# endif /* xDEBUG */
static const i4 month_hours[] = {0, 0, 744, 1440, 2184, 2904, 3648, 4368,
			5112, 5856, 6576, 7320, 8040};
static const i4 nano_mult[] = {
    1000000000,  /* 0 */
     100000000,  /* 1 */
      10000000,  /* 2 */
       1000000,  /* 3 */
        100000,  /* 4 */
         10000,  /* 5 */
          1000,  /* 6 */
           100,  /* 7 */
            10,  /* 8 */
             1,  /* 9 */
};

/*  Static function references	*/

DB_STATUS
ad0_1cur_init(
ADF_CB		    *adf_scb,
struct	timevect    *tv,
bool                now,
bool		    highprec);

/* convert date to internal INGRES time. */
static DB_STATUS ad0_4date(ADF_CB        *adf_scb,
			   DATEVECT      *dv,
			   AD_NEWDTNTRNL *dn,
			   DB_DATA_VALUE *date_dv,
                           i1            time_type); 
                                        
/* add two dates */				
static DB_STATUS add_date(ADF_CB         *adf_scb,
			  DB_DATA_VALUE   *rdv,
			  AD_NEWDTNTRNL   *a,
			  AD_NEWDTNTRNL   *b);
					
/* subtract two dates. */
static DB_STATUS ad0_5sub_date(ADF_CB         *adf_scb,
			       DB_DATA_VALUE   *rdv,
			       AD_NEWDTNTRNL   *a,
			       AD_NEWDTNTRNL   *b);

/* Multiply a AD_NEWDTNTRNL */
static DB_STATUS ad0_date_mul_intrv(ADF_CB  *adf_scb,
				AD_NEWDTNTRNL *dn,
				f8 mul,
				DB_DATA_VALUE *rdv);

/* Convert interval to float */
static DB_STATUS ad0_dte_dtntrnl2f(ADF_CB *adf_scb,
				AD_NEWDTNTRNL *dn,
				f8 *flt,
				bool isDTE);

/* Convert float to interval */
static DB_STATUS ad0_dte_f2dtntrnl(ADF_CB *adf_scb,
				f8 flt,
				AD_NEWDTNTRNL *dn,
				bool isDTE);

/* copy DB_DATA_VALUE string into a char buffer. */
static	bool ad0_6valcopy(ADF_CB         *adf_scb,
			  DB_DATA_VALUE  *str_dv,
			  char           *char_ptr,
			  i4             char_len,
			  i4             *result_len);
					
/* Report an error with a string from a DB_DATA_VALUE struct. */
static DB_STATUS ad0_7errvaldsc(ADF_CB         *adf_scb,
				i4        error_code,
				DB_DATA_VALUE  *str_dv);	

/* given a date interval return the date class */
static DB_STATUS ad0_8getdateclass(ADF_CB         *adf_scb,
				   DB_DATA_VALUE  *inter_spec,
				   char           *date_class);

/* the day number with respect to beginning of year. */
static VOID ad0_9daysinyear(AD_NEWDTNTRNL  *datep,
			    i4            *daynumber);

/* Given an absolute date convert to number of days */
static VOID ad0_10togregorian(AD_NEWDTNTRNL  *datep,
			      i4            *daycount,
			      i4            *dayofweek);

/* Given an a number of days convert to absolute date. */
static VOID ad0_11fromgregorian(i4            daycount,
				AD_NEWDTNTRNL  *datep);

/* check time format and create template. */
static bool ad0_dttmok(char  *f);       

static bool ad0_dtintvlok(char  *f,      
			  i4    time);	

/* convert the input values according to the format string. */
static bool ad0_dtcvrt(ADF_CB	 *adf_scb,
		       char      *format,
		       i4        **dvalp,
		       i4        **dsizp,
		       DATEVECT  *dv,
		       i4        prec,
		       i4	 *now);	

/* preliminary normalization of time field of internal date */
static VOID ad0_prelim_norml(DATEVECT  *dv);

static VOID print_date(AD_NEWDTNTRNL *dn);

/*{
** Name: adu_1datetodate() -	convert a date data value into a date
**				data value.
**
** Description:
**        This routine converts a date data value into a date data value.
**	This is basically a copy of the data.  It will be used primarily
**	to copy a date data value into a tuple buffer.
**
** NOTE ON coercion between ANSI date/time types.
** For ANSI date/time types we need to manipulate the date/time fields
** as follows:
** If STDZ = SQL session default time zone displacement
**      SV = source value, 
**      TV = target  value,
**     UTC = UTC component of time zone value in SV or TV.
** Case 1: Time adjustment for the Time zone
** Adjust time in the dn with session time zone and
** place the session time zone in the time zone value on dn
** for following cases:-
**  i.e  TV.UTC = SV - STZD (modulo 24)
**       TV.TZ = STZD
**  From Date -> Timestamp with TZ 
**  From TIME without TZ -> Time with TZ 
**  From TIME without TZ -> Timestamp with TZ
**  From Timestamp without TZ -> Time without TZ 
**
** Case 2: For the following cases ANSI specifies that we 
** adjust the Time zone as follows
**    SV.UTC + SV.TZ (modulo 24)
**    TV.TZ = STZD
**  From TIME      with TZ -> Time without TZ 
**  From TIME      with TZ -> Timestamp without TZ 
**  From Timestamp with TZ -> DATE 
**  From Timestamp with TZ -> TIME without TZ 
**  From Timestamp with TZ -> Timestamp without TZ 
** 
** Case 3: For the following cases the date portion must
** be filled with current date
**  From TIME without TZ -> Timestamp without TZ
**  From TIME without TZ -> Timestamp with TZ
**  From TIME with TZ    -> Timestamp without TZ
**  From TIME with TZ    -> Timestamp with TZ
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**	date_dv1			DB_DATA_VALUE describing the source
**					date data value.
**	    .db_data			Ptr to a internal date type struct.
**      date_dv2                        DB_DATA_VALUE describing the destination
**					date data value.
**	    .db_data			Ptr to internal date type struct to be copied
**					into.
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	date_dv2
**	    *.db_data			Ptr to internal date type struct to be 
**					copied into.
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	2-jul-86 (ericj)
**          Initial creation
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	13-feb-03 (inkdo01)
**	    Normalize status bits after copy (status bits were inconsistently
**	    set for date and date/time values). This is part of 109553 fix.
**	    Note: it depends on the fact that date coercions are aligned by
**	    adecompile.
**	17-jun-2006 (gupsh01)
**	    Added support for new ANSI datetime types.
**	12-sep-2006 (gupsh01)
**	    Fixed case for ANSI date/time types as they need to follow
**	    ANSI spec for conversion.
**	21-sep-2006 (gupsh01)
**	    Fixed coercion between ingresdate, time with local time zone,
**	    timestamp with local time zone and other types.
**	25-sep-2006 (gupsh01)
**	    Fixed the case for Interval processing.
**	04-oct-2006 (gupsh01)
**	    Timestamp without time zone to Local timestamp conversion
**	    should take into account the DST of the date value not current
**	    time.
**	25-oct-2006 (gupsh01)
**	    Fixed DST calculation when converting from timestamp without 
**	    time zone to date.
**	02-nov-2006 (gupsh01)
**	    When copying the values to a type which supports it, account 
**	    for precision of fractional seconds field.
**	16-nov-2006 (gupsh01)
**	    Added error message E_AD5066_DATE_COERCION.
**	22-nov-2006 (gupsh01)
**	    Declare the string constants before hand for brevity.
**	    and fix the coercions for ingresdate to ansidate.
**	21-Feb-2008 (kiria01) b119632
**	    Apply precision to DTE too to cause it to truncate nanoseconds.
*/

DB_STATUS
adu_2datetodate(
ADF_CB              *adf_scb,
DB_DATA_VALUE	    *date_dv1,
DB_DATA_VALUE	    *date_dv2)
{
    AD_NEWDTNTRNL    d;
    AD_NEWDTNTRNL    *dn;
    struct timevect  cur_tv;
    bool             ctv_init = FALSE;
    AD_NEWDTNTRNL    curnd;
    i4		     stdz;
    STATUS	     db_stat = E_DB_OK;
    i4		     displacement = 0;

    char dt_ansidate[9] = {'a', 'n', 's', 'i', 'd', 'a', 't', 'e', '\0'};
    char dt_time[5] = {'t', 'i', 'm', 'e', '\0'};
    char dt_interval[9] = {'i', 'n', 't', 'e', 'r', 'v', 'a', 'l', '\0'};
    char dt_anyother[10] = {'a', 'n', 'y', ' ', 'o', 't', 'h', 'e', 'r', '\0'};
    char dt_target[7] = {'t', 'a', 'r', 'g', 'e', 't', '\0'};
    char dt_absolute[15] = 
	{'a','b','s','o','l','u','t','e',',',' ','d','a','t','e','\0'};
    char dt_datetime[9] = {'d','a','t','e','t','i','m','e','\0'};

    MEfill ((u_i2)sizeof(d), NULLCHAR, (PTR)&d);
    MEfill ((u_i2)sizeof(curnd), NULLCHAR, (PTR)&curnd);
    dn = &d;

    adu_6to_dtntrnl (date_dv1, dn);

    /* Initialize current time vector */ 
    if (db_stat = ad0_1cur_init(adf_scb, &cur_tv, TRUE, TRUE))
        return (db_stat);
    ctv_init = TRUE;

    curnd.dn_year	= cur_tv.tm_year + AD_TV_NORMYR_BASE;
    curnd.dn_month      = cur_tv.tm_mon + AD_TV_NORMMON;
    curnd.dn_day	= cur_tv.tm_mday;
    curnd.dn_seconds	= cur_tv.tm_hour * AD_39DTE_ISECPERHOUR +
			  cur_tv.tm_min * AD_10DTE_ISECPERMIN +
			  cur_tv.tm_sec;
    curnd.dn_nsecond    = cur_tv.tm_nsec;

	/* Record the Session time zone displacement */
    stdz = TMtz_search (adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL, 
				    adu_5time_of_date(&curnd));
    switch (abs(date_dv1->db_datatype))
    {
	case DB_ADTE_TYPE:
	if ((abs(date_dv2->db_datatype) != DB_TSWO_TYPE) && 
	    (abs(date_dv2->db_datatype) != DB_TSW_TYPE)  && 
	    (abs(date_dv2->db_datatype) != DB_TSTMP_TYPE)&&
	    (abs(date_dv2->db_datatype) != DB_DTE_TYPE)  &&
	    (abs(date_dv2->db_datatype) != DB_ADTE_TYPE))
	{
	  /* datatypes cannot be coerced */ 
	  return (adu_error(adf_scb, E_AD5066_DATE_COERCION, 4, 
	    sizeof(dt_ansidate), dt_ansidate, sizeof(dt_target), dt_target));
	}
	else if (abs(date_dv2->db_datatype) == DB_TSW_TYPE)
	{
	  /* copy year, month day from t and set time components
	  ** to zero - for without case it will be done by adu_7fromntrnl
	  */ 
	  dn->dn_tzhour = ((i4 )stdz/3600);
	  dn->dn_tzminute = ((i4) stdz % 3600)/60;
	}
	else if ((abs(date_dv2->db_datatype) == DB_TSTMP_TYPE) ||
		 (abs(date_dv2->db_datatype) == DB_DTE_TYPE))
	{
	  /* Convert to UTC */
	  dn->dn_seconds -= TMtz_search (adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL, 
				    adu_5time_of_date(dn));
          adu_1normldate(dn);
	}
	break;

	case DB_TMWO_TYPE:
	if (abs(date_dv2->db_datatype) == DB_ADTE_TYPE)
	{
	  return (adu_error(adf_scb, E_AD5066_DATE_COERCION, 4,
		sizeof(dt_time), dt_time, sizeof(dt_ansidate), dt_ansidate));
	}
	else if (abs(date_dv2->db_datatype) == DB_TMW_TYPE)
	{
	  dn->dn_tzhour = ((i4 )stdz/3600);
	  dn->dn_tzminute = ((i4) stdz % 3600)/60;
	}
	else if ((abs(date_dv2->db_datatype) == DB_TSWO_TYPE) ||
                 (abs(date_dv2->db_datatype) == DB_DTE_TYPE) ||
	         (abs(date_dv2->db_datatype) == DB_TME_TYPE)||
	         (abs(date_dv2->db_datatype) == DB_TSTMP_TYPE)||
	         (abs(date_dv2->db_datatype) == DB_TSW_TYPE))
	{
	  dn->dn_year = curnd.dn_year;
	  dn->dn_month = curnd.dn_month; 
	  dn->dn_day = curnd.dn_day; 
	  if (abs(date_dv2->db_datatype) == DB_TSW_TYPE)
	  {
	    dn->dn_tzhour = ((i4 )stdz/3600);
	    dn->dn_tzminute = ((i4) stdz % 3600)/60;
	  }
	  else if ((abs(date_dv2->db_datatype) == DB_DTE_TYPE) ||
	  	  (abs(date_dv2->db_datatype) == DB_TME_TYPE) ||
	        (abs(date_dv2->db_datatype) == DB_TSTMP_TYPE))
	  { /* convert to UTC */
	    dn->dn_seconds -= stdz;
            dn->dn_tzhour = 0;
            dn->dn_tzminute = 0;
            adu_1normldate(dn);
	  }
	}
	break;

	case DB_TMW_TYPE:
	if (abs(date_dv2->db_datatype) == DB_ADTE_TYPE)
	{
	  return (adu_error(adf_scb, E_AD5066_DATE_COERCION, 4,
		sizeof(dt_time), dt_time, sizeof(dt_ansidate), dt_ansidate));
	} 
        else if((abs(date_dv2->db_datatype) == DB_TSW_TYPE) ||
                (abs(date_dv2->db_datatype) == DB_DTE_TYPE) ||
                (abs(date_dv2->db_datatype) == DB_TME_TYPE) ||
	        (abs(date_dv2->db_datatype) == DB_TSTMP_TYPE)||
                (abs(date_dv2->db_datatype) == DB_TSWO_TYPE))
	{
	  /* copy current year month and day */
	  dn->dn_year = curnd.dn_year;
	  dn->dn_month = curnd.dn_month; 
	  dn->dn_day = curnd.dn_day; 
          if (abs(date_dv2->db_datatype) == DB_TSWO_TYPE)
	  {
            dn->dn_tzhour = 0;
            dn->dn_tzminute = 0;
	  }
	  else if ((abs(date_dv2->db_datatype) == DB_DTE_TYPE) ||
	  	   (abs(date_dv2->db_datatype) == DB_TME_TYPE) ||
	           (abs(date_dv2->db_datatype) == DB_TSTMP_TYPE))
	  { /* convert to UTC */
	    dn->dn_seconds -= ((dn->dn_tzhour * 3600) + (dn->dn_tzminute * 60));
            dn->dn_tzhour = 0;
            dn->dn_tzminute = 0;
            adu_1normldate(dn);
	  }
	}
	break;

	case DB_TSWO_TYPE:
        if ((abs(date_dv2->db_datatype) == DB_TMW_TYPE) ||
            (abs(date_dv2->db_datatype) == DB_TSW_TYPE))
	{
	    dn->dn_tzhour = ((i4 )stdz/3600);
	    dn->dn_tzminute = ((i4) stdz % 3600)/60;
	}
        else if ((abs(date_dv2->db_datatype) == DB_TME_TYPE) ||
                 (abs(date_dv2->db_datatype) == DB_DTE_TYPE) ||
	         (abs(date_dv2->db_datatype) == DB_TSTMP_TYPE))
	{
            dn->dn_tzhour = 0;
            dn->dn_tzminute = 0;

	    /* convert the time to UTC */
	    if (abs(date_dv2->db_datatype) != DB_TME_TYPE)
	      dn->dn_seconds -= TMtz_search (adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL, 
				    adu_5time_of_date(dn));
	    else
	      dn->dn_seconds -= stdz;
            adu_1normldate(dn);
	}
	break;

	case DB_TSW_TYPE:
        if ((abs(date_dv2->db_datatype) == DB_TME_TYPE) ||
            (abs(date_dv2->db_datatype) == DB_DTE_TYPE) ||
	     (abs(date_dv2->db_datatype) == DB_TSTMP_TYPE))
	{
	    /* convert the time to UTC */
	    dn->dn_seconds -= ((dn->dn_tzhour * 3600) + (dn->dn_tzminute * 60));
            dn->dn_tzhour = 0;
            dn->dn_tzminute = 0;
            adu_1normldate(dn);
	}
	break;

	case DB_TME_TYPE:
   	if (abs(date_dv2->db_datatype) == DB_ADTE_TYPE)
	{
	  return (adu_error(adf_scb, E_AD5066_DATE_COERCION, 4, 
		sizeof(dt_time), dt_time, sizeof(dt_ansidate), dt_ansidate));
	}
        else if ((abs(date_dv2->db_datatype) == DB_TMWO_TYPE) ||
        	 (abs(date_dv2->db_datatype) == DB_TMW_TYPE))
        {
	    dn->dn_seconds += stdz;
	    dn->dn_seconds %= (3600 * 24); /* modulo 24 */
            if (abs(date_dv2->db_datatype) == DB_TMW_TYPE)
	    {
              dn->dn_tzhour = ((i4 )stdz/3600);
              dn->dn_tzminute = ((i4) stdz % 3600)/60;
	    }
            adu_1normldate(dn);
        }
        else if ((abs(date_dv2->db_datatype) == DB_TSWO_TYPE) ||
                 (abs(date_dv2->db_datatype) == DB_TSW_TYPE) ||
                 (abs(date_dv2->db_datatype) == DB_TSTMP_TYPE) ||
                 (abs(date_dv2->db_datatype) == DB_DTE_TYPE))
        {
            dn->dn_year = curnd.dn_year;
            dn->dn_month = curnd.dn_month;
            dn->dn_day = curnd.dn_day;
            if ((abs(date_dv2->db_datatype) == DB_TSW_TYPE)||
                (abs(date_dv2->db_datatype) == DB_TSWO_TYPE))
            {
	      displacement = TMtz_search (adf_scb->adf_tzcb, TM_TIMETYPE_GMT, 
				    adu_5time_of_date(dn));
              if (abs(date_dv2->db_datatype) == DB_TSW_TYPE)
	      {
                dn->dn_tzhour = ((i4 )displacement/3600);
                dn->dn_tzminute = ((i4) displacement % 3600)/60;
	      }
              adu_1normldate(dn);
            }
        }
	break;

	case DB_TSTMP_TYPE:
        if ((abs(date_dv2->db_datatype) == DB_ADTE_TYPE) ||
	    (abs(date_dv2->db_datatype) == DB_TMWO_TYPE) ||
	    (abs(date_dv2->db_datatype) == DB_TMW_TYPE) ||
	    (abs(date_dv2->db_datatype) == DB_TSWO_TYPE) ||
            (abs(date_dv2->db_datatype) == DB_TSW_TYPE))
        {
	    displacement = TMtz_search (adf_scb->adf_tzcb, TM_TIMETYPE_GMT, 
				    adu_5time_of_date(dn));
	    dn->dn_seconds += displacement;
            dn->dn_tzhour = ((i4 )displacement/3600);
            dn->dn_tzminute = ((i4) displacement% 3600)/60;
            adu_1normldate(dn);
        }
	break;

	case DB_INYM_TYPE:
	if ((abs(date_dv2->db_datatype) != DB_INYM_TYPE) && 
            (abs(date_dv2->db_datatype) != DB_DTE_TYPE))
	{
	  return (adu_error(adf_scb, E_AD5066_DATE_COERCION, 4, 
	    sizeof(dt_interval), dt_interval, sizeof(dt_anyother), dt_anyother));
	}  
        break;

	case DB_INDS_TYPE:
	if ((abs(date_dv2->db_datatype) != DB_INDS_TYPE) && 
            (abs(date_dv2->db_datatype) != DB_DTE_TYPE))
	 {
	  return (adu_error(adf_scb, E_AD5066_DATE_COERCION, 4,
	    sizeof(dt_interval), dt_interval, sizeof(dt_anyother), dt_anyother));
	}
        break;

	case DB_DTE_TYPE:
	{
	  if (dn->dn_status & AD_DN_ABSOLUTE)
	  {
	    if ((abs(date_dv2->db_datatype) == DB_INYM_TYPE) ||
	        (abs(date_dv2->db_datatype) == DB_INDS_TYPE))
	    {
	        return (adu_error(adf_scb, E_AD5066_DATE_COERCION, 4, 
	        	  sizeof(dt_absolute), dt_absolute, 
			  sizeof(dt_interval), dt_interval));
	    }
	  }
	  else
	  {
	    if ((abs(date_dv2->db_datatype) != DB_INYM_TYPE) &&  
	        (abs(date_dv2->db_datatype) != DB_INDS_TYPE) &&
	        (abs(date_dv2->db_datatype) != DB_DTE_TYPE))
	    {
	        return (adu_error(adf_scb, E_AD5066_DATE_COERCION, 4, 
			  sizeof(dt_interval), dt_interval,
	        	  sizeof(dt_absolute), dt_absolute));
	    }
	  }

	  displacement = TMtz_search (adf_scb->adf_tzcb, TM_TIMETYPE_GMT, 
				    adu_5time_of_date(dn));
	  if ((dn->dn_seconds & AD_DN_TIMESPEC) && 
	     ((abs(date_dv2->db_datatype) == DB_ADTE_TYPE) ||
	      (abs(date_dv2->db_datatype) == DB_TMWO_TYPE) ||
	      (abs(date_dv2->db_datatype) == DB_TMW_TYPE) ||
	      (abs(date_dv2->db_datatype) == DB_TSW_TYPE) ||
	      (abs(date_dv2->db_datatype) == DB_TSWO_TYPE)))
	  {
	    dn->dn_seconds += displacement;
	    if((abs(date_dv2->db_datatype) == DB_TMW_TYPE) ||
	        (abs(date_dv2->db_datatype) == DB_TSW_TYPE)) 
	    {
              dn->dn_tzhour = ((i4 )displacement/3600);
              dn->dn_tzminute = ((i4) displacement% 3600)/60;
	    }
            adu_1normldate(dn);
	  }
	  else if (abs(date_dv2->db_datatype) == DB_TSTMP_TYPE)
	  {
            dn->dn_tzhour = ((i4 )displacement/3600);
            dn->dn_tzminute = ((i4) displacement % 3600)/60;
	  }
	}
	break;
    }

    if ((dn->dn_nsecond) &&
	(abs(date_dv2->db_datatype) != DB_ADTE_TYPE) && 
	(abs(date_dv2->db_datatype) != DB_INYM_TYPE)) 
    {
	i4 i = 0;
	i4 prec = date_dv2->db_prec;
	/* truncate nanosecond portion to target prec */
	if (prec <= 0 || prec > 9)
	    dn->dn_nsecond = 0;
	else
	{
	    dn->dn_nsecond /= nano_mult[prec];
	    dn->dn_nsecond *= nano_mult[prec];
	}
    }

    adu_7from_dtntrnl (date_dv2, dn);

    if ((date_dv1->db_datatype == DB_DTE_TYPE) && 
	(dn->dn_status & AD_DN_ABSOLUTE))
    {
	/* Assure status bits are set correctly (for hashing, if nothing 
	** else). */
	if (dn->dn_day)
	    dn->dn_status |= AD_DN_DAYSPEC;
	if (dn->dn_month)
	    dn->dn_status |= AD_DN_MONTHSPEC;
	if (dn->dn_year)
	    dn->dn_status |= AD_DN_YEARSPEC;
	if (dn->dn_seconds)
	    dn->dn_status |= AD_DN_TIMESPEC;
    }
    return (E_DB_OK);
}


/*{
** Name: adu_14strtodate() - convert a date string to an internal date format.
**
** Description:
**        This routine converts a "date" string into an internal date format.
**	The valid date string input formats and their interpretation are
**	determined by the global read-only date template arrays defined in
**	datetab.roc.  The date template array to use is determined by
**	the variable adf_scb->adf_dfmt.
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**	    .adf_dfmt			A DB_DATE_FMT describing which template
**					array should be used in the inter-
**					pretation of the date string.
**	str_dv				DB_DATA_VALUE describing the string.
**	    .db_datatype		Its datatype.
**	    .db_length			Its length.
**	    .db_data			If the datatype is character, this is
**					a pointer to the string to be converted.
**					If the datatype is text, this is a ptr
**					to a DB_TEXT_STRING.
**		[.db_t_count]		The number of character in a
**					DB_TEXT_STRING string.
**		[.db_t_text]		Ptr to the "text" string to be
**					converted.
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**      date_dv                         DB_DATA_VALUE describing the internal
**					date.
**	    .db_data			Ptr to AD_NEWDTNTRNL struct.
**		.dn_status		A character whose bits describe the
**					internal date format.
**		.dn_highday		A character for large day intervals.
**		.dn_year		The date's year.
**		.dn_month		The date's month.
**		.dn_lowday		If the date is absolute, this is the
**					day of the month.  If the date is an
**					interval, this is the low-order part
**					of the number of days in the interval.
**		.dn_seconds		The time in seconds from 00:00.
**		.dn_nsecond		Fractional seconds in nanoseconds
[@PARAM_DESCR@]...
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**
** History:
**	26-may-86 (ericj)
**          Converted for Jupiter.
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	24-nov-87 (thurston)
**	    Converted CH calls into CM calls.
**	22-jan-88 (thurston)
**	    Added the underscore as another acceptable delimiter.
**	01-sep-88 (thurston)
**	    Increased size of local buf from DB_MAXSTRING+1 to
**	    DB_GW4_MAXSTRING+1.
**	20-dec-88 (billc)
**	    Made changes to improve performance and clean up old code.
**	19-jan-89 (jrb)
**	    Fixed problem where errors were not being caught when date
**	    templates didn't match.  Also added '+' as a valid delimeter.
**	23-jan-89 (jrb)
**	    Got rid of adu_prelim_norm call and added adu_2normldint for
**	    intervals.
**	13-jul-89 (jrb)
**	    Fixed bug where a time spec after "now" and "today" was not
**	    caught as an invalid date.
**	16-aug-89 (jrb)
**	    Made large numbers give invalid date error.  Also, added call to
**	    adu_prelim_norml() function to normalize date vectors.
**	29-sep-89 (jrb)
**	    Got rid of one of the huge local stack variables since the stack
**	    consumption was causing problems with STAR.
**	26-mar-90 (jrb)
**	    Bug 20807: AV when invalid date followed by time (or anything with a
**	    colon).
**	16-may-90 (jrb)
**	    Added an array which parallels d_val_array.  It keeps track of the
**	    original source string sizes of the values in d_val_array.  For
**	    instance, if we converted '001234' into the value 1234, the
**	    corresponding size entry would be 6.  This allows us to later know
**	    whether there were leading zeros on a particular number.
**      29-aug-92 (stevet)
**          Modified timezone adjustment method by calling TMtz_search
**          to calculate correct timezone offset.
**	15-jan-1999 (kitch01)
**	    Bug 94791. Amend check for alpha characters to allow '-'
**	    and digits if a time has been specified. This is to allow
**	    timezones of, for example, gmt-3.
**      19-May-2000 (hanal04) Bug 101064 INGSRV 1145
**          Pass the new time_type parameter to ad0_4date(). This
**          signifies that we wish to override the value returned from
**          _bintim.
**	19-may-2001 (somsa01)
**	    To save stack space, allocate space for the temp buffer array
**	    dynamically.
**	12-aug-02 (inkdo01)
**	    Modification to above to impose MEreqmem overhead only if string is
**	    bigger than 2004.
**	1-apr-05 (inkdo01)
**	    Fix date(nvarchar) to NOT overlay a bunch of memory.
**	17-jul-2006 (gupsh01)
**	    Added support for ANSI datetime datatypes.
**	01-aug-2006 (gupsh01)
**	    Added support to specify the ANSI date/time types only in the 
**	    ANSI format (new format DB_ANSI_DFMT introduced for internal use).
**	29-aug-2006 (gupsh01)
**	    Removed support for ANSI datetime datatypes from here. 
**	    These types are now dealt by adu_21ansi_strtodt().
**	05-sep-2006 (gupsh01)
**	    Added new error message for ANSI datetime types.
**	08-sep-2006 (gupsh01)
**	    Reinstate handling of ANSI date/time types by this routine. This
**	    allows for string to ANSI date/time coercion. Remove the error
**	    message previously added. 
**	25-oct-06 (kibro01) bug 109562
**	    If the date spec reaches the buffer limit for specbuf, don't
**	    go further - it won't be valid that long anyway 
**	14-nov-06 (kibro01) bug 116936
**	    If the d_valp structure is overflowed, check and ensure it
**	    flags up a bad date gracefully - same reasoning as for bug 109562
**	02-feb-07 (gupsh01)
**	    Pass a pointer to the d_sizp array instead of the array
**	    itself to ad0_dtcvrt.
*/

DB_STATUS
adu_14strtodate(
ADF_CB		*adf_scb,
DB_DATA_VALUE	*str_dv,
DB_DATA_VALUE	*date_dv)
{
#define D_VAL_ARRAY_SIZE	16
    i4		    in_len;		    /* length of the input string */
    bool	    isempty;
    DB_STATUS       db_stat;
    register char   *datespec;
    char	    specbuf[64];
    i4		    d_val_array[D_VAL_ARRAY_SIZE];
    i4		    d_val_size[D_VAL_ARRAY_SIZE];
    i4              d_val_index;
    i4		    *d_valp;
    i4		    *d_sizp;
    DATEVECT	    dv;
    AD_NEWDTNTRNL    dn;
    AD_NEWDTNTRNL   ndn;
    bool	    time = FALSE;
    i4		    now;
    register char   *p;
    char   	    *ptemp;
    char 	    *dbuf;
    char	    localbuf[2004];
    bool	    uselocall = TRUE;
    i4		    error_number = 0;
    DB_DATA_VALUE   str2_dv;
    bool            fractional = FALSE;

    /* Zero table used in parse */
    MEfill(sizeof(dv), NULLCHAR, (PTR) &dv);
    MEfill(sizeof(dn), NULLCHAR, (PTR) &dn);

    if (str_dv->db_datatype == DB_NCHR_TYPE
	|| str_dv->db_datatype == DB_NVCHR_TYPE)
    {
	bool	local = TRUE;

	if (str_dv->db_datatype == DB_NVCHR_TYPE)
	    str2_dv.db_length = (str_dv->db_length-DB_CNTSIZE)/sizeof(UCS2)+DB_CNTSIZE;
	else str2_dv.db_length = str_dv->db_length/sizeof(UCS2);

	/* See if "localbuf" is big enough. */
	if (str2_dv.db_length >= 2004)
	{
	    str2_dv.db_data = (char *)MEreqmem(0, str2_dv.db_length, 
					FALSE, NULL);
	    local = FALSE;
	}
	else str2_dv.db_data = (char *)&localbuf;

	MEfill(str2_dv.db_length, NULLCHAR, (PTR) str2_dv.db_data);
	if (str_dv->db_datatype == DB_NCHR_TYPE)
			str2_dv.db_datatype = DB_CHA_TYPE;	
	else if (str_dv->db_datatype == DB_NVCHR_TYPE)
			str2_dv.db_datatype = DB_VCH_TYPE;	
	str2_dv.db_prec = str_dv->db_prec;
	adu_nvchr_coerce(adf_scb,  str_dv, &str2_dv);
	MEcopy(str2_dv.db_data, str2_dv.db_length, str_dv->db_data);
	str_dv->db_length = str2_dv.db_length;
	str_dv->db_datatype = str2_dv.db_datatype;
	str_dv->db_prec = str2_dv.db_prec;

	/* localbuf wasn't big enough - free what we allocated. */
	if (!local)
	    MEfree((PTR)str2_dv.db_data);
    }

    /*
    ** Parse an input string into a legal date.  We go through the string
    ** creating an array of values (d_val_array) along with the sizes of the
    ** section of string which generated them (d_val_size), and a template
    ** (datespec) which is used to look up a format that controls
    ** interpretation of the values we find.
    */

    if ((db_stat = adu_3straddr(adf_scb, str_dv, &ptemp)) != E_DB_OK)
	return (db_stat);
    if ((db_stat = adu_5strcount(adf_scb, str_dv, &in_len)) != E_DB_OK)
	return (db_stat);

    d_valp = d_val_array;
    d_sizp = d_val_size;
    d_val_index = 0;
    now	= 0;
    datespec = specbuf;

    isempty = TRUE;
    dv.dv_datetype = DV_ABSOLUTE;

    /* It's handy (and cheap) to have a zero-terminated input string. */
    if (in_len >= 2004)
    {
	/* Call MEreqmem to avoid blowing call stack with local allocation. */
	dbuf = (char *)MEreqmem(0, DB_GW4_MAXSTRING + 1, FALSE, NULL);
	uselocall = FALSE;
    }
    else dbuf = &localbuf[0];

    STncpy(dbuf, ptemp, in_len);
    dbuf[ in_len ] = EOS;
    
    if (date_dv->db_datatype != DB_DTE_TYPE)
    {
        /* For ANSI datetiem types we should call adu_21ansi_strtodt */
	db_stat = adu_21ansi_strtodt( adf_scb, str_dv, date_dv);
        return (db_stat);
    }
    
    /*
    ** Parse the input string building a date specification string and
    ** putting the values on a queue to be interpreted later.
    */

    p = dbuf;
    while (*p != EOS) 
    {
	i4 value;
	i4 valcnt;

        switch(*p)
        {
        default:
	    if (CMdigit(p))
	    {
		/* this is the start of a number, only integers
		** are allowed in dates so scan to first non-digit 
		*/
		char	    *oldp = p;
    
		valcnt = 1;
		value = 0;
		do
		{
		    if (fractional)
		    {
			/* NOTE: that the value for fractional is pre-scaled
			** for nano-seconds precision and that the excess digits
			** are passed in the string but have been ignored. No
			** attempt is made to round these up
			*/
			if (valcnt < 10)
			    value += (*p - '0')*nano_mult[valcnt];
		    }
		    else
		    {
			/* check for overflow (a little on the safe side) */
			if (value > MAXI4/10-1)
			{
			    error_number = E_AD5052_DATEVALID;
			    goto adu_14strtodate_error;
			}
			value = value * 10 + (*p - '0');
		    }
		    valcnt++;
		    p++;
		} while (CMdigit(p));
    
		/* Add to value list and to template. */
		*d_valp++ = value;
		*d_sizp++ = p - oldp;
		*datespec++     = 'n';
    
		d_val_index++;
		if (d_val_index >= D_VAL_ARRAY_SIZE)
		{
		    error_number = E_AD5052_DATEVALID;
		    goto adu_14strtodate_error;
		}
    
		isempty = FALSE;
		fractional = FALSE;
	    }
	    else if (CMwhite(p))
	    {
		CMnext(p);
	    }
	    else
	    {
		char *oldp;
		char c;
		ADU_DATENAME *d;
    
		/* Alphabetic chars, here.  We hope. */
		/* Bug 94791. Amended to also accept '-' and digits if
		** a time has been specified.
		*/
		for (oldp = p; ;p++)
		{
		    CMtolower(p, p);
		    if (!CMlower(p))
			    {
			    if (!time)
				break;
			    else
			    {
				if ((*p != '-') && (!CMdigit(p)))
				    break;
			    }
			    }
		    }
    
		/* if we didn't find any legal chars, then it's bad news. */
		if (oldp == p)
		{
		    /* Bad character. Generate a user error */
		    error_number = E_AD5058_DATEBADCHAR;
		    goto adu_14strtodate_error;
		}
    
		/* Hacky: save the next char and zero terminate for lookup. */
		c = *p;
		*p = EOS;
		d = ad0_13tab_look(oldp);
		*p = c;
    
		/* Add magic char to the template. */
		CMtolower(&(d->d_class), datespec);
		datespec++;
    
		/* if magic char was uppercase, we don't add the value
		** to the list.
		*/
		if (CMlower(&(d->d_class)))
		{
		    *d_valp++ = d->d_value;
		    *d_sizp++ = p - oldp;
		    d_val_index++;
		    if (d_val_index >= D_VAL_ARRAY_SIZE)
		    {
			error_number = E_AD5052_DATEVALID;
			goto adu_14strtodate_error;
		    }
		}
		else if (d->d_value == -1)
		{
		    dv.dv_datetype = DV_LENGTH;
		}
    
		isempty = FALSE;
	    }
	    break;
	
	case ':':
	    /* This is a time delimiter. */
	    time = TRUE;
	    isempty = FALSE;
	    *datespec++ = *p++;
	    break;

	case '.':
	    /* If in time part treat '.' as fractional introducer */
	    fractional = time;
	    /* FALLTHROUGH */
	case '-':
	case '_':
	case '+':
	    /* These are delimiters, place them in the string directly */
	    isempty = FALSE;
	    /* FALLTHROUGH */
	case '/':
	    /* This is a delimiter, legal in an empty date. */
	    *datespec++ = *p++;
	    break;
	}
	/* If we have reached the maximum length of the date spec.,
	** don't read any further
	*/
	if ((datespec - specbuf) >= ( sizeof(specbuf)-1 ))
	{
	    error_number = E_AD5052_DATEVALID;
	    goto adu_14strtodate_error;
	}
   }
   *datespec = EOS;

   /* By now we know whether the date is empty. */
    if (isempty)
    {
	/*
	** An empty date can have the format " / / ".  Or it can be 
	** just whitespace.  If we saw nothing but '/' and/or whitespace,
	** we assume it's an empty date.
	*/
        dn.dn_status    = AD_DN_NULL;
        dn.dn_year	= 0;
        dn.dn_month	= 0;
        dn.dn_day	= 0;
        dn.dn_seconds	= 0;
	dn.dn_nsecond	= 0;
    }
    else
    {
	const DATETMPLT   *tmptab;

#       ifdef xDEBUG
	if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_003_DATEFI_TRACE,
		&dum1, &dum2))
	    TRdisplay("strtodate: date specification is '%s'\n", specbuf);
#       endif

	/*
        ** Now that we've got the template, lets match it with the
	** possible official templates to make sure it's a legit format.
	*/

        d_valp = d_val_array;
	d_sizp = d_val_size;
        p = specbuf;

	if (dv.dv_datetype == DV_ABSOLUTE)
	{
	    if (date_dv->db_datatype == DB_DTE_TYPE)
	      tmptab = Adu_multi_tmplts[adf_scb->adf_dfmt];
	    else 
	      tmptab = Adu_multi_tmplts[DB_ANSI_DFMT];

	    for (; tmptab->dt_tmplt != NULL; tmptab++)
	    {
		char *tp;
		char *sp;

		/* fast compare for performance */
		for (tp = tmptab->dt_tmplt, sp = specbuf; 
			*tp == *sp  &&  *tp != EOS; 
		   	 tp++, sp++)
		    continue;

		if (*tp == EOS)
		    if (time  ||  (!time && *sp == EOS))
		    {
			p = sp;
			break;
		    }
	    }

	    /* not a date if no template match or a colon follows
	    ** the date which was found, implying a time spec.
	    ** Error if no date, time, or interval.
	    */
	    if (tmptab->dt_tmplt == NULL  ||  *p == ':')
	    {
		if (!time)
		{
		    error_number = E_AD5052_DATEVALID;
		    goto adu_14strtodate_error;
		}

		p = specbuf;
	    }
	    else
	    {
		/* interpret date specification */
		if (ad0_dtcvrt(adf_scb, tmptab->dt_interp, &d_valp, &d_sizp, &dv, 
					date_dv->db_prec, &now))
		{
		    error_number = E_AD5052_DATEVALID;
		    goto adu_14strtodate_error;
		}
	    }

	    if (time)
	    {
		/* If z or r format ("today" or "now") or error converting
		** date, then give error
		*/
		if (	ad0_dttmok(p)
		    ||  ad0_dtcvrt(adf_scb, p, &d_valp, &d_sizp, &dv, date_dv->db_prec, &now)
		    ||  (tmptab->dt_tmplt != NULL &&
			    (	STcompare(tmptab->dt_tmplt, "r") == 0
			     ||	STcompare(tmptab->dt_tmplt, "z") == 0)
			)
		   ) 
		{
		    error_number = E_AD5052_DATEVALID;
		    goto adu_14strtodate_error;
		}
	    }
        }
	else /* interval */
	{
	    if (    ad0_dtintvlok(p, time)
		||  ad0_dtcvrt(adf_scb, p, &d_valp, &d_sizp, &dv, date_dv->db_prec, &now)
	       )
	    {
		error_number = E_AD5052_DATEVALID;
		goto adu_14strtodate_error;
	    }
	}

        /* finish filling in the date conversion struct, do the conversion */
	if ((db_stat = ad0_4date(adf_scb, &dv, &dn, date_dv, now)) != E_DB_OK)
	{
	   if (!uselocall)
	        MEfree((PTR)dbuf);
	    return (db_stat);
	}
        /* check for 'now' case and set status bit for timespec */

        if (now == DTE_ABS_NOW)
            dn.dn_status |= AD_DN_TIMESPEC;
        else if (now == DTE_OFFSET_OR_TODAY)
            dn.dn_seconds = dn.dn_nsecond = 0;
    }

    /* Now we convert the AD_NEWDTNTRNL to respective storage format
    ** Based on the output datatype 
    */ 
    adu_7from_dtntrnl(date_dv, &dn);

#   ifdef xDEBUG
    if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_003_DATEFI_TRACE,
	    &dum1, &dum2))
    {
        TRdisplay("Converted date is: year = %d, month = %d, day = %d, ",
            dn.dn_year, dn.dn_month, dn.dn_day);
        TRdisplay("time = %d, status = %d\n", dn.dn_seconds, dn.dn_status);
    }
#   endif

    if (!uselocall)
	MEfree((PTR)dbuf);

    return (E_DB_OK);

/* Make sure we don't copy too much of the date into the error buffer
** in case the buffer is too large and blows the error function.  Copy
** at most MINIDBFLEN(60) characters
** I chose 60 because while ebuflen is 1500, a high value causes a failure
** in the error-passing routines, and 60 is enough to give an idea why the
** date failed.
*/
adu_14strtodate_error:
#define MINIDBUFLEN   60
    if (strlen(dbuf) > MINIDBUFLEN)
    {
       char tmp_dbuf[MINIDBUFLEN];
       STncpy(tmp_dbuf, dbuf, MINIDBUFLEN-1);
       tmp_dbuf[MINIDBUFLEN-1] = '\0';
       db_stat = adu_error(adf_scb, error_number, 2, (i4) 0, (i4 *) tmp_dbuf);
    } else
    {
       db_stat = adu_error(adf_scb, error_number, 2, (i4) 0, (i4 *) dbuf);
    }
    if (!uselocall)
       MEfree((PTR)dbuf);
    return (db_stat);
}

/* check time format and create template */

static bool
ad0_dttmok(
char *f)
{
    /* legal time formats are:
    ** n:n[:n][a][t]
    ** where n is a number, a is am/pm and t is timezone
    */
    if (MEcmp("n:n", f, 3) != 0)
	return (1);
    *f++ = 'h';	    /* replace first n with 'h'. step over colon */
    f++;	    
    *f++ = 'i';
    if (MEcmp(":n", f, 2) == 0)
    {
	*++f = 's';
	f++;
	if (MEcmp(".n", f, (u_i2)2) == 0)
	{
	    *++f = 'l';
	    f++;
	}
    }
    if (*f == 'a')
	f++;
    if (*f == 't')
	f++;
    if (*f)
	return (1);
    return (0);
}


/* check format of intervals */

static bool
ad0_dtintvlok(
char	*f,
i4	time)
{
	char	c;

	/* legal interval formats are:
	    [[-|+]h:[-|+]m[:[-|+]s]hrs]{[-|+]n[yrs|mths|days|hrs|mins|secs]}
	    where [] means optional, {} means 0 or more occurrences, and
	    [x|y|...] means one of x, y, ...
	*/

	/* see if the time portion (that part with ':') is ok */
	if (time)
	{
		if (*f == '-'  ||  *f == '+')
			f++;
		if (MEcmp("n:", f, 2))
			return (1);
		*f++ = 'h';
		if (*++f == '-'  ||  *f == '+')
			f++;
		if (*f != 'n')
			return (1);
		*f++ = 'i';

		/* see if optional seconds part is there */
		if (*f == ':')
		{
			if (*++f == '-'  ||  *f == '+')
				f++;
			if (*f != 'n')
				return (1);
			*f++ = 's';
		}

		/* this format must be terminated by 'h' */
		if (*f != 'h')
			return (1);
		else
			*f++ = 'n';
	}

	/* see if interval part is there */
	while (*f)
	{
		if (*f == '-' || *f == '+')
			f++;
		if (*f != 'n')
			return (1);
		c = *++f;
		if (c != 'y' && c != 'm' && c != 'd' &&
		    c != 'h' && c != 'i' && c != 's' &&
		    c != 'a' && c != 't')
			return (1);
		f++;
	}
	return (0);
}


/* 
**  ad0_dtcvrt() - convert the input values according to the format string 
**
**  History:
**  	
**  	07-may-92 (stevet)
**  	    Added special formats 'Y', 'M' and 'D' for YMD,
**  	    MDY and DMY date formats repectively.  Also changed the
**  	    'k', 'j' and 'g' to uppercase: 'K', 'J' and 'G' to
**  	    distinguish these special date format options from other
**  	    format options.  We are running out of lowercase
**  	    characters for new format options.
**	15-jan-1999 (kitch01)
**	    Bug 94791. Amended the timezone case to remove conversion to
**	    minutes. As some timezones have half hours and we use integers
**	    to hold the GMT offset, timezone values are already in minutes.
**	17-jul-2006 (gupsh01)
**	    Millisecond support added.
**	25-jul-2006 (gupsh01)
**	    Precision support added for new ANSI date/time types.
**	29-aug-2006 (gupsh01)
**	    Added support for ANSI datetime datatypes.
**	02-nov-2006 (gupsh01)
**	    Remove rounding off the nanoseconds value upto given 
**	    precision, we just truncate the value now.
**	12-dec-2006 (gupsh01)
**	    Added checking for valid values for time zone
**	02-feb-07 (gupsh01)
**	    Pass a pointer to the d_sizp array instead of the array
**	    itself to ad0_dtcvrt. Truncate the nanosecond values 
**	     beyond 9 digits.
**	12-mar-2007 (gupsh01)
**	    If precision is 0 eg for ingresdates etc, and fraction
**	    second portion is provided, skip storing it.
**	28-jun-2007 (gupsh01)
**	    Fixed the -ive ingresdates.
**      23-Jul-2007 (hanal04) Bug 118788
**          Change AD_47DTE_MIN_TZ_SECS to AD_47DTE_MIN_TZ_MN and
**          AD_48DTE_MAX_TZ_SECS to AD_48DTE_MAX_TZ_MN. The defines
**          are the min and max minutes for a TZ.
**	3-oct-2007 (dougi)
**	    Fix nanoseconds in negative intervals.
*/ 

static bool
ad0_dtcvrt(
ADF_CB		*adf_scb,
char		*format,
i4		**dvalp,
i4		**pdsizp,
register DATEVECT	*dv,
i4		prec,
i4		*now)
{
	register char	*p;
	register i4	*ip;
	i4	i;
	i4	s;
	i4	yrmod;
    	i4 	monmod;
    	i4 	daymod;
	i4	tz_hour_component = 0;
	bool    negative_interval = FALSE;
	i1	tzmn;
	i4	*dsizp = *pdsizp;

#       ifdef xDEBUG
	if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_003_DATEFI_TRACE,
		&dum1, &dum2))
	    TRdisplay("strtodate: Interpreting date string '%s'\n", format);
#       endif

    ip = *dvalp;
    for (p = format; *p != EOS; p++)
    {
	i2 status = 0;

	switch (*p)
	{
	  case '-':
	    *ip = -(*ip);
	    break;

	  case ':':
	  case 'n':
	  case '+':
	  case '.':
	    break;

	  case 'y':
	  case 'x':
	    /* This is a year... we make the assumption that years < 100 are
	    ** references to the 1900's (ie. the current century). We do not
	    ** allow single digit years to default to the current decade.
	    */
	    s = dsizp[ip - *dvalp];
	    dv->dv_year = *ip++;
            status = DV_YEAR;

	    if ( (dv->dv_year < 1000) && (s < 4) )
		status |= DV_YEARLT4;

	    if ( (dv->dv_year < 100) && (s > 2) )
		status |= DV_YEAR0;

	    if (*p == 'x')
	    {
	      dv->dv_year = -dv->dv_year;
	      if (dv->dv_datetype == DV_LENGTH) 
		negative_interval = TRUE;
	    }
	    break;

	  case 'q':
	    /* This is a month for an ANSI interval year to month */
	    dv->dv_month = *ip++;
	    if ((negative_interval) || 
		((dv->dv_datetype == DV_LENGTH) && (dv->dv_year < 0)))
	    {
	      dv->dv_month = -dv->dv_month;	
	      negative_interval = TRUE;
	    }
	      status = DV_MONTH;
	    break;

	  case 'm':
	    /* This is a month */
	    dv->dv_month = *ip++;
	    status = DV_MONTH;
	    break;

	  case 'd':
	  case 'e':
	    /* This is a day */
	    dv->dv_day = *ip++;
	    if (*p == 'e')
	    {
	      dv->dv_day = -dv->dv_day;
	      if (dv->dv_datetype == DV_LENGTH)
	        negative_interval = TRUE;
	    }
	    status = DV_DAY;
	    break;

	  case 'w':
	    /* This is week - valid only in length and offset dates */

	    dv->dv_week = *ip++;
	    status = DV_WEEK;
	    break;

	  case 'o':
	    /* This is day-of-week - valid only in offset dates */

	    dv->dv_dow = *ip++;
	    status = DV_DOW;
	    break;

	  case 'h':
	  case 'j':
	    /* This is hour */

	    dv->dv_hour = *ip++;
	    status = DV_HOUR;
	    if ((negative_interval) || 
		((dv->dv_datetype == DV_LENGTH) && (dv->dv_day < 0)) ||
	    	(*p == 'j'))
	    {
		negative_interval = TRUE;
		dv->dv_hour = -dv->dv_hour;	
	    }
	    break;

	  case 'i':
	  case 'k':
	    /* This is minute */

	    dv->dv_minute = *ip++;
	    status = DV_MINUTE;
	    if ((negative_interval) || 
		((dv->dv_datetype == DV_LENGTH) && (dv->dv_hour < 0)) ||
	    	(*p == 'k'))
	    {
	      if (dv->dv_datetype == DV_LENGTH) 
		negative_interval = TRUE;
	      dv->dv_minute = -dv->dv_minute;	
	    }
	    break;

	  case 's':
	  case 'p':
	    /* This is second */

	    dv->dv_second = *ip++;
	    if (*p == 'p')
	    {
	      dv->dv_second = -dv->dv_second;
	      if (dv->dv_datetype == DV_LENGTH) 
		negative_interval = TRUE;
	    }
	    else if ((negative_interval) || 
		((dv->dv_datetype == DV_LENGTH) && (dv->dv_minute < 0)))
	    {
		negative_interval = TRUE;
		dv->dv_second = -dv->dv_second;	
	    }
	    status = DV_SECOND;
	    break;

	  case 'l':
	  case 'g':
	    /* This is nanosecond */

	    /* The value passed in will be a pre-scaled count of nanosecomds */
	    dv->dv_nanosecond = *ip++;

	    /* Scale up (or down) to nanoseconds. */
	    if (prec <= 0 || prec > 9)
		dv->dv_nanosecond = 0;
	    else
	    {
		dv->dv_nanosecond /= nano_mult[prec];
		dv->dv_nanosecond *= nano_mult[prec];
	    }
	    if ((*p == 'g') || (negative_interval) || 
		((dv->dv_datetype == DV_LENGTH) && (dv->dv_minute < 0)))
	    {
		if (dv->dv_datetype == DV_LENGTH) 
		    negative_interval = TRUE;
		dv->dv_nanosecond = -dv->dv_nanosecond;	
	    }
	    status = DV_NANOSECOND;
	    break;

	  case 'a':
	    /*
	    ** This is am/pm specification.  We assume the specification is
	    ** 0 or 12 for number of hours offset.  Only valid in absolute
	    ** date specifications.
	    */

	    dv->dv_ampm = *ip++;
	    status = DV_AMPM;
	    break;

	  case 'b':
	  case 'c':
	    /*
	    ** This is timezone hour component in the numeral form
	    ** +hh:mm
	    */
	    tz_hour_component = *ip++;
	    if (*p == 'c')
	      tz_hour_component = -tz_hour_component;
	    dv->dv_timezone = tz_hour_component * 60;
            if ((tz_hour_component < AD_34DTE_MIN_TZ_HR) ||
                (tz_hour_component > AD_35DTE_MAX_TZ_HR))
             return (adu_error(adf_scb, (i4) E_AD5077_ANSITMZONE_BADFMT, 0));
	    status = DV_TIMEZONE;
	    break;

	  case 'v':
	    /*
	    ** This is timezone minute component in the numeral form
	    ** +hh:mm
	    */
	    tzmn = *ip;
	    if (dv->dv_timezone < 0)
	      dv->dv_timezone -= *ip++;
	    else 
	      dv->dv_timezone += *ip++;
            if ( ( tzmn < AD_36DTE_MIN_TZ_MN ) ||
                 ( tzmn > AD_37DTE_MAX_TZ_MN ) ||
		 ( dv->dv_timezone < AD_47DTE_MIN_TZ_MN) ||
		 ( dv->dv_timezone > AD_48DTE_MAX_TZ_MN))
              return (adu_error(adf_scb, (i4) E_AD5077_ANSITMZONE_BADFMT, 0));
	    if ((status & DV_TIMEZONE) != 0)
	      status = DV_TIMEZONE;
	    break;

	  case 't':
	    /*
	    ** This is timezone -- only valid in absolute time references.
	    ** We assume this is always set to offset from GMT so we
	    ** normally subtract for internal rep
	    */
	    /* Bug 94791. timezone offsets are now already in minutes. */
	    dv->dv_timezone = *ip++;
	    status = DV_TIMEZONE;
	    break;

	  case 'f':
	    /* specifying time as offset or today as date */

	    dv->dv_datetype = DV_OFFSET;
	    *now = DTE_OFFSET_OR_TODAY;
	    break;

	  case 'r':
	    /* specifying now as absolute date and time */

	    dv->dv_datetype = DV_OFFSET;
	    *now = DTE_ABS_NOW;
	    break;

	  case 'K':
	    /*
	    ** Special Morrison-Knudsen format peb 10/24/83.
	    ** Allow dates to be entered as six digits MMDDYY.
	    */
	    i = *ip++;
	    dv->dv_year = i % 100;
	    dv->dv_month = i / 10000;
	    /*
	    **  Some clown changed the i to a 1 in the following statement.
	    **  This introduced bug #4729.  I changed it back to an i.
	    */
	    dv->dv_day = i / 100 - (dv->dv_month * 100);
	    status = (DV_YEAR | DV_MONTH | DV_DAY);
	    break;

	  case 'J':
	    /*
	    ** Allow dates to be entered as six digits
	    ** in the form YYMMDD. This is added for Finnish
	    ** national conventions. peb 8/2/85
	    */
	    s = dsizp[ip - *dvalp];
	    i = *ip++;
	    dv->dv_day = i % 100;
	    dv->dv_year = i / 10000;
	    dv->dv_month = i / 100 - (dv->dv_year * 100);

	    /* if the original number was bigger than 4 it must have had a
	    ** year specified, otherwise there was no year
	    */
	    if (s > 4)
		status = (DV_YEAR | DV_MONTH | DV_DAY);
	    else
		status = (DV_MONTH | DV_DAY);
	    break;

	  case 'G':
	    /*
	    ** Allow dates to be entered as five to eight digits
	    ** in one of the forms:
	    **		DMMYY
	    **		DDMMYY
	    **		DMMYYYY
	    **		DDMMYYYY
	    ** Added for the German date format (Gene Thurston  20-may-88)
	    */
	    i = *ip++;
	    if (i > 99999999)  /* if > 8 digits ... error */
		return (1);
	    else if (i > 999999)  /* if 7 or 8 digits ... YYYY */
		yrmod = 10000;
	    else if (i > 9999)  /* if 5 or 6 digits ... YY */
		yrmod = 100;
	    else                    /* if < 5 digits ... error */
		return (1);
	    
	    dv->dv_year = i % yrmod;
	    i /= yrmod;
	    dv->dv_month = i % 100;
	    dv->dv_day = i / 100;
	    if ( (dv->dv_year < 100) && (yrmod == 10000))
		status = (DV_YEAR0 | DV_YEAR | DV_MONTH | DV_DAY);
	    else
		status = (DV_YEAR | DV_MONTH | DV_DAY);
	    break;

	  case 'Y':
	    /*
	    ** Allow dates to be entered as four to eight digits
	    ** in one of the forms:
    	    **	    	MDD
    	    **	    	MMDD
	    **		YYMDD
	    **		YYMMDD
	    **		YYYYMDD
	    **		YYYYMMDD
	    ** Added for YMD date format (Steve Tai 06-may-92)
	    */
	    s = dsizp[ip - *dvalp];
	    i = *ip++;
    	    switch( s)
    	    {
    	    	case 8:	    /* if 8 digits ... YYYYMMDD */
    	    	    yrmod = 10000;
    	    	    monmod = 100;
    	    	    break;
    	    	case 7:	    /* if 7 digits ... YYYYMDD */
    	    	    yrmod = 1000;
    	    	    monmod = 100;
    	    	    break;
    	    	case 6:	    /* if 6 digits ... YYMMDD */
   	    	    yrmod = 10000;
    	    	    monmod = 100;
    	    	    break;
    	    	case 5:     /* if 5 digist ... YYMDD */
    	    	    yrmod = 1000;
    	    	    monmod = 100;
    	    	    break;
    	    	case 4:     /* if 4 digits ... MMDD  */
    	    	    yrmod = 0;
    	    	    monmod = 100;
    	    	    break;
    	    	default:      /* < 4 digits ... error */
    	    	    return (1);
    	    }
    	    /* Always have months and days */
	    status = (DV_MONTH | DV_DAY);
    	    if( yrmod)
    	    {
    	    	dv->dv_year = i / yrmod;
	    	i = i % yrmod;
		if ( (dv->dv_year < 100) && (yrmod == 10000))
		    status = status | DV_YEAR | DV_YEAR0;
		else
		    status = status | DV_YEAR;
    	    }
	    dv->dv_month = i / monmod;
	    dv->dv_day = i % monmod;
	    break;


	  case 'M':
	    /*
	    ** Allow dates to be entered as four to eight digits
	    ** in one of the forms:
    	    **	    	MMDD
	    **		MDDYY
	    **		MMDDYY
	    **		MDDYYYY
	    **		MMDDYYYYY
	    ** Added for MDY date format (Steve Tai 06-may-92)
	    */
	    s = dsizp[ip - *dvalp];
	    i = *ip++;
    	    switch( s)
    	    {
    	    	case 8:	    /* if 7 or 8 digits ... MDDYYYY */
    	    	case 7:
    	    	    monmod = 1000000;
    	    	    daymod = 10000;
    	    	    break;
    	    	case 6:	    /* if 6 or 5 digits ... MDDYYY */
    	    	case 5:
    	    	    monmod = 10000;
    	    	    daymod = 100;
    	    	    break;
    	    	case 4:	    /* if 4 digits ... MMDD */
    	    	    monmod = 100;
    	    	    daymod = 0;
    	    	    break;
    	    	default:      /* < 4 digits ... error */
    	    	    return (1);
    	    }
    	    /* Always have months and days */
	    status = (DV_MONTH | DV_DAY);
	    dv->dv_month = i / monmod;
    	    i = i % monmod;
    	    if( daymod)
    	    {
    	    	dv->dv_day = i / daymod;
    	    	dv->dv_year = i % daymod;
		if ((dv->dv_year < 100) && (daymod == 1000000))
		    status = status | DV_YEAR | DV_YEAR0;
		else
		    status = status | DV_YEAR;
    	    }
    	    else
	    	dv->dv_day = i;
	    break;

	  case 'D':
	    /*
	    ** Allow dates to be entered as four to eight digits
	    ** in one of the forms:
    	    **	    	DDMM
	    **		DDMYY
	    **		DDMMYY
	    **		DDMYYYY
	    **		DDMMYYYY
	    ** Added for DMY date format (Steve Tai 06-may-92)
	    */
	    s = dsizp[ip - *dvalp];
	    i = *ip++;
    	    switch( s)
    	    {	
    	    	case 8:     /* if 8 digits ... DDMMYYYY */
    	    	    daymod = 1000000;
    	    	    monmod = 10000;
    	    	    break;
    	    	case 7:	    /* if 7 digits ... DDMYYYY */
    	    	    daymod = 100000;
    	    	    monmod = 10000;
    	    	    break;
    	    	case 6:	    /* if 6 digits ... DDMMYY */
    	    	    daymod = 10000;
    	    	    monmod = 100;
    	    	    break;
    	    	case 5:     /* if 5 digist ... DDMYY */
    	    	    daymod = 1000;
    	    	    monmod = 100;
    	    	    break;
    	    	case 4:     /* if 4 digits ... MMDD  */
    	    	    daymod = 100;
    	    	    monmod = 0;
    	    	    break;
    	    	default:      /* < 4 digits ... error */
    	    	    return (1);
    	    }
    	    /* Always have months and days */
	    status = (DV_MONTH | DV_DAY);
    	    dv->dv_day = i / daymod;
    	    i = i % daymod;

    	    if( monmod)
    	    {
    	    	dv->dv_month = i / monmod;
	    	dv->dv_year = i % monmod;
		if ((dv->dv_year < 100) && (monmod == 10000))
		    status = status | DV_YEAR | DV_YEAR0;
		else
		    status = status | DV_YEAR;
    	    }
    	    else
    	    	dv->dv_month = i;
	    break;

	  default:
	    return (1);
	}

	/* if we've already set this value, it's an error. 
	** if status is still zero, these are no-ops.
	*/
	if (dv->dv_status & status)
	    return (1);
	dv->dv_status |= status;
    }
    *dvalp = ip;
    return (0);
}


/*
** Name: ad0_13tab_look() -   lookup a date keyword in our table.
**
** Description:
**	  This routine uses a binary search to look up a date keyword
**	in our global table Adu_datename.  This routine replaces
**	ad0_2nameclass and ad0_3namevalue, redundant routines which did
**	linear scans of the table.  (This fix lowered the number of STcompare
**	calls from an average of 31 per keyword to 2.)
**
** Inputs:
**	str				char * to keyword to be looked up.
**
**	Returns:
**		an ADU_DATENAME pointer to the table entry matching 'str'.
**		If 'str' wasn't found, the pointer points to a dummy
**		entry with 'u' as the class.  (This matches the semantics
**		of previous routines that did this job.)
**	history
**	    14-dec-88 (billc)
**		first written.
*/

static ADU_DATENAME _No_datename = 
	{ "not found", 'u', 0 };

ADU_DATENAME *
ad0_13tab_look(
char *str)
{
	i4 low = 0;
	i4 high = Adu_dt_size - 1;
	i4 middle;
	i4 direction;
	ADU_DATENAME *ret = &_No_datename;

	while (low <= high)
	{
		register char *p1;
		register char *p2;

		/* divide the list in half */
		middle = (low + high) / 2;

# ifdef xDEBUG
	if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_004_DATE_SUPPORT,
		&dum1, &dum2))
            TRdisplay("ad0_13tab_look: comparing '%s' with '%s'\n", str,
		      Adu_datename[middle].d_name);
# endif

		/*
		** This next bit of skullduggery is OK because we're dealing
		** with lowercased strings, and we know it's in English
		** so we don't sweat double-char stuff.
		*/
		p1 = str; 
		p2 = Adu_datename[middle].d_name; 
		while (*p1 == *p2 && *p1 != EOS)
		{
			p1++;
			p2++;
		}

		direction = (i4) *p1 - (i4) *p2;

		if (direction < 0)
			high = middle - 1;
		else if (direction > 0)
			low = middle + 1;
		else
		{
			/* found it. */
			ret = (ADU_DATENAME *)&Adu_datename[middle];
			break;
		}
	}

	return (ret);
}

/*
** Name: ad0_4date() -   convert date to internal INGRES time.
**
** Description:
**	  This routine converts a date vector into an internal Ingres
**	date, AD_NEWDTNTRNL.  It performs validity checks on the date
**	values in the process.  If the date format is an "offset", it
**	gets the current date, using adu_cvtime(), to derive an absolute date.
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      dv                              Ptr DATEVECT struct.
**	    .tm_year			year (1970 <= year <= 2035)
**	    .tm_month			month (1 <= month <= 12)
**	    .tm_day			day (1 <= day <= 31)
**	    .tm_hour		        hour (0 <= hour <= 23)
**	    .tm_min			minutes (0 <= min <= 59)
**	    .tm_sec			seconds (0 <= sec <= 59)
**	dn				Ptr to AD_NEWDTNTRNL struct to be
**                                      initialized.
**	date_dv				The date type we wish to convert to.
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	dn
**		.dn_status		A character whose bits describe the
**					internal date format.
**		.dn_highday		A character for large day intervals.
**		.dn_year		The date's year.
**		.dn_month		The date's month.
**		.dn_lowday		If the date is absolute, this is the
**					day of the month.  If the date is an
**					interval, this is the low-order part
**					of the number of days in the interval.
**		.dn_time		The time in milliseconds from 00:00.
**	Returns:
**	history
**	    16-jan-87 (daved)
**		error parameters for user errors 4302,4304,4305 are numbers
**		not strings
**	    29-sep-87 (thurston)
**		Since the static routine ad0_1cur_init() now returns a DB_STATUS
**              and takes an ADF_CB ptr as an argument, I needed to make the
**              appropriate changes to this routine, which calls
**              ad0_1cur_init().
**	    23-jan-89 (jrb)
**		Fixed bug I found where '2/29' was a valid date even in non-leap
**		years.  The problem was that dv->dv_year was being passed when
**		not set to current year (was 0 instead) and 0 is a leap year!
**          29-aug-92 (stevet)
**              Modified timezone adjustment method by calling TMtz_search
**              to calculate correct timezone offset.
**	    13-nov-95 (nick)
**		Support II_DATE_CENTURY_BOUNDARY.
**	31-jul-96 (nick)
**	    Until the arrival of II_DATE_CENTURY_BOUNDARY ( see my change 
**	    above ) all releases of Ingres have added a hard coded 1900 to
**	    all two digit years.  II_DATE_CENTURY_BOUNDARY changed this
**	    to a hard coded 1900 OR 2000 depending upon the year specified
**	    in relation to this boundary.  Now changed this to be aware
**	    of the current century ( i.e. 1900, 2000, 2100 etc. ) and also
**	    the method of determination of which century the input is most
**	    likely to be in.  This is now as follows ...
**
**		if (current_year < boundary)
**		    if (input_year < boundary)
**			output_year = input_year + current_century
**		    else
**			output_year = input_year + previous_century
**		else
**		    if (input_year < boundary)
**			output_year = input_year + next_century
**		    else
**			output_year = input_year + current_century
**	28-jan-97 (inkdo01)
**	    Normalize result of coercion (computations already normalize)
**	    to solve bugs 66329 and 74033.
**	7-feb-97 (inkdo01)
**	    Didn't quite get the above fix right (grrrr).
**	31-mar-97 (inkdo01)
**	    Yet another attempt to get it right (DON'T normalize intervals!).
**      19-May-2000 (hanal04) Bug 101064 INGSRV 1145
**          Use new 'now' parameter in ad0_1cur_init() to determine
**          whether we wish to force acquisition of the current time.
**      31-Nov-2001 (hanal04) Bug 103533 INGSRV 1347
**          Normalise date('now') values to avoid join failures caused
**          by different internal date formats resulting from the date in
**          GMT being different to the date in the current time zone.
**      23-apr-2001 (horda03)
**          Set the appropriate EPOCH bit.
**      06-Sep-2001 (hanal04) Bug 105417 INGSRV 1515
**          Normalise date('now') values as absolute dates and not
**          as interval dates. This prevents differences in the hex
**          value of the date and the hex value of a date that is
**          generated as a select from of the date('now').
**          This information is passed into this function in the new
**          time_type parameter.
**      07-May-2002 (zhahu02) b107667/ingsrv1760
**          When the dv->dv_month (data type long) is outside the the range of 
**          dn_month (data type short), the following calculation method needs
**          to be used:
**                   dn->dn_year += (dv->dv_month / 12);
**                   dn->dn_month = dv->dv_month % 12;
**	2-may-02 (inkdo01)
**	    Generate correct status flags for date('now') and date('today').
**      12-apr-2004 (huazh01)
**          Make adjustments for AM/PM values before we subtract the 
**          timezone offset by calling TMtz_search(). This fixes errors 
**          when select/insert date-type values under DST timezone.
**          This fixes INGSRV2787, b112144.
**	17-jul-2006 (gupsh01)
**	    The input parameter dn is modified to be of new internal date
**	    format to support ANSI datetime types.
**	01-aug-2006 (gupsh01)
**	    Modify all other calculations to calculate dn_time in seconds
**	    not milliseconds.
**	06-sep-2006 (gupsh01)
**	    fixed Date('now') not to return date in milliseconds.
**	19-sep-2006 (gupsh01)
**	    Fix setting of highprec based on the correct value of 
**	    datatype.
**	21-nov-2006 (gupsh01)
**	    Remove the II_CENTURY_BOUNDARY support for ANSI date/time
**	    types.
*/

static	DB_STATUS
ad0_4date(
ADF_CB              *adf_scb,
DATEVECT	    *dv,
AD_NEWDTNTRNL	    *dn,
DB_DATA_VALUE	    *date_dv,
i1                  time_type)
{
    i4		secs;
    i4	       	status;
    i2		datetype;
    struct timevect cur_tv;
    bool	ctv_init = FALSE;
    DB_STATUS	db_stat;
    bool	now;
    bool	highprec = TRUE;
    bool	ansitype = FALSE;

/*
**   Define constants for tranforming a timevect absolute year spec into an
** Ingres internal date absolute year spec and for transforming a timevect
** absolute month spec into an Ingres internal date absolute month spec.
*/
#define	    AD_TV_NORMYR_BASE		1900
#define	    AD_TV_NORMMON		   1

    status = dv->dv_status;
    datetype = dv->dv_datetype; /* avoid multiple dereference. */
    if (date_dv)
      dn->dn_dttype = date_dv->db_datatype;

    if ((date_dv->db_datatype == DB_ADTE_TYPE) || 
        (date_dv->db_datatype == DB_TSTMP_TYPE) || 
    	(date_dv->db_datatype == DB_TSWO_TYPE) || 
    	(date_dv->db_datatype == DB_TSW_TYPE) || 
    	(date_dv->db_datatype == DB_TME_TYPE) || 
    	(date_dv->db_datatype == DB_TMWO_TYPE) || 
    	(date_dv->db_datatype == DB_TMW_TYPE) || 
    	(date_dv->db_datatype == DB_INDS_TYPE) || 
    	(date_dv->db_datatype == DB_INYM_TYPE))
	ansitype = TRUE;
	
    now = (time_type == DTE_ABS_NOW)?TRUE:FALSE;

    if (date_dv->db_datatype == DB_DTE_TYPE)
      highprec = FALSE;

    /* if this is offset time, initialize current setting to now */

    if (datetype == DV_OFFSET)
    {
	/* Initialize current time vector for use in defaults */
	if (db_stat = ad0_1cur_init(adf_scb, &cur_tv, now, highprec))
	    return (db_stat);
	ctv_init = TRUE;

        dn->dn_year	= cur_tv.tm_year + AD_TV_NORMYR_BASE;
        dn->dn_month    = cur_tv.tm_mon + AD_TV_NORMMON;
        dn->dn_day	= cur_tv.tm_mday;
        dn->dn_seconds	= cur_tv.tm_hour * AD_39DTE_ISECPERHOUR +
			  cur_tv.tm_min * AD_10DTE_ISECPERMIN +
			  cur_tv.tm_sec;
	dn->dn_nsecond  = cur_tv.tm_nsec;

        /* adjust for timezone */
	if ((dn->dn_dttype != DB_TMWO_TYPE) && 
	    (dn->dn_dttype != DB_TMW_TYPE) && 
	    (dn->dn_dttype != DB_TSWO_TYPE) && 
	    (dn->dn_dttype != DB_TSW_TYPE))
	{
	  dn->dn_seconds -= TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL, 
				    adu_5time_of_date(dn));
	}
    }

#   ifdef xDEBUG
    if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_004_DATE_SUPPORT,
	    &dum1, &dum2))
        TRdisplay("date: dv->dv_timezone=%d\n", dv->dv_timezone);
#   endif

    if (datetype == DV_ABSOLUTE || datetype == DV_OFFSET)
    {
        /* make this an absolute date */
        dn->dn_status = AD_DN_ABSOLUTE;
    }
    else
    {
        /* This is a time interval */
        dn->dn_status = AD_DN_LENGTH;

#       ifdef xDEBUG
	if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_004_DATE_SUPPORT,
			    &dum1,&dum2))
        {
            TRdisplay("date: before adu_prelim_norml, status=%d\n",
			dv->dv_status);
            TRdisplay("date: and days=%d\n",
			dv->dv_day);
        }
#       endif

        /* Do some preliminary normalization of the interval */
        ad0_prelim_norml(dv);

	/* update with new status */
	status = dv->dv_status;	

#       ifdef xDEBUG
	if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_004_DATE_SUPPORT,
			    &dum1, &dum2))
        {
            TRdisplay("date: after adu_prelim_norml, status=%d\n",
			dv->dv_status);
            TRdisplay("date: and now days=%d\n",
			dv->dv_day);
        }
#       endif

    }

    /* set year to an absolute value */

    if (status & DV_YEAR)
    {
	/*
	** Year was specified, use it ...
	** if year < 100, assume it refers to current century
	** unless we have specified a date/century boundary and the
	** year falls below this
	*/

        if (datetype == DV_ABSOLUTE)
        {
	    if (ansitype) 
	    {
	      if (status & DV_YEARLT4)
                return (adu_error(adf_scb, (i4) E_AD5067_ANSIDATE_4DIGYR, 0));
	    }
            else if ((dv->dv_year < 100) && !(status & DV_YEAR0))
	    {
		i4	current_century;
		i4	current_year;

		if (!ctv_init)
		{
		    db_stat = ad0_1cur_init(adf_scb, &cur_tv, now, highprec);
		    if (db_stat != E_DB_OK)
		    {
			return (db_stat);
		    }
		    else
		    {
			ctv_init = TRUE;
		    }
		}
		/* N.B. cur_tv.tm_year is # years since 1900 */
		current_century = 
			((cur_tv.tm_year + AD_TV_NORMYR_BASE) / 100) * 100;
		current_year = cur_tv.tm_year % 100;

		if (adf_scb->adf_year_cutoff == TM_DEF_YEAR_CUTOFF)
		{
		    /*
		    ** No II_DATE_CENTURY_BOUNDARY so 
		    ** default to old behaviour i.e. assume current
		    ** century
		    */
                    dv->dv_year += current_century;
		}
		else if (current_year < adf_scb->adf_year_cutoff)
		{
		    if (dv->dv_year < adf_scb->adf_year_cutoff)
			dv->dv_year += current_century;
		    else
			dv->dv_year += (current_century - 100);
		}
		else /* current_year >= adf_scb->adf_year_cutoff */
		{
		    if (dv->dv_year < adf_scb->adf_year_cutoff)
			dv->dv_year += (current_century + 100);
		    else
			dv->dv_year += current_century;
		}
	    }

            /* validate parameters */

            if (dv->dv_year < AD_23DTE_MIN_ABS_YR || 
		dv->dv_year > AD_24DTE_MAX_ABS_YR)
            {
                return (adu_error(adf_scb, (i4) E_AD5053_DATEYEAR, 2, 
				  (i4) sizeof(dv->dv_year),
				  (i4 *) &dv->dv_year));
            }
        }

        /* check if number of years for interval is in range */

        if (datetype == DV_LENGTH && (dv->dv_year > MAX_INV_YR 
				      || dv->dv_year < MIN_INV_YR))
	{
	    return (adu_error(adf_scb, (i4) E_AD505A_DATEYROVFLO, 2, 
			      (i4) sizeof(dv->dv_year),
			      (i4 *) &dv->dv_year));
        }

        if (datetype == DV_OFFSET)
            dv->dv_year += (cur_tv.tm_year + AD_TV_NORMYR_BASE);

        dn->dn_year  = dv->dv_year;
        dn->dn_status   |= AD_DN_YEARSPEC;
    }
    else
    {
        /* year was not specified, use current year */

        if (datetype == DV_ABSOLUTE || datetype == DV_OFFSET)
	{
	    /* Initialize current time vector for use in defaults */
	    if (!ctv_init && (db_stat = ad0_1cur_init(adf_scb, &cur_tv, 
						now, highprec)))
		return (db_stat);
	    ctv_init = TRUE;

            dn->dn_year = cur_tv.tm_year + AD_TV_NORMYR_BASE;
	}
    }


    if (status & DV_MONTH)
    {
        if (datetype == DV_ABSOLUTE && (dv->dv_month < 1 || dv->dv_month > 12))
        {
            return (adu_error(adf_scb, (i4) E_AD5054_DATEMONTH, 2,
			      (i4) sizeof(dv->dv_month),
			      (i4 *) &dv->dv_month));
        }

	if (dv->dv_month > MAXI2 || dv->dv_month < MINI2)
	{
	    dn->dn_year += (dv->dv_month / 12);
	    dn->dn_month = dv->dv_month % 12;

            /* check if number of years for interval is in range */
            if (datetype == DV_LENGTH && (dn->dn_year > MAX_INV_YR 
				      || dn->dn_year < MIN_INV_YR))
	    {
		char mnths[7] = {'m','o','n','t','h','s','\0'};

	        return (adu_error(adf_scb, (i4) E_AD5062_DATEOVFLO, 4, 
			      (i4) sizeof(dv->dv_month),
			      (PTR) &dv->dv_month, (i4)sizeof(mnths), 
			      (PTR) mnths ));
            }
	    dn->dn_status |= AD_DN_MONTHSPEC;
	    dn->dn_status   |= AD_DN_YEARSPEC;
	}
	else
	{
            dn->dn_month = dv->dv_month;
            dn->dn_status |= AD_DN_MONTHSPEC;
	}
    }
    else
    {
        /* assume current month */
        if (datetype == DV_ABSOLUTE)
	{
	    /* Initialize current time vector for use in defaults */
	    if (!ctv_init && (db_stat = ad0_1cur_init(adf_scb, &cur_tv, 
						now, highprec)))
		return (db_stat);
	    ctv_init = TRUE;

            dn->dn_month = cur_tv.tm_mon + AD_TV_NORMMON;
	}
    }

    if (status & DV_DAY)
    {
        if (datetype == DV_ABSOLUTE  &&
            (dv->dv_day < 1         ||
             dv->dv_day > adu_2monthsize((i4) dn->dn_month,
                        (i4) dn->dn_year)))
        {
            return (adu_error(adf_scb, (i4) E_AD5055_DATEDAY, 2, 
			      (i4) sizeof(dv->dv_day),
			      (i4 *) &dv->dv_day));
        }

        /* check if number of days for interval is in range */
        if (datetype == DV_LENGTH && (dv->dv_day > AD_28DTE_MAX_INT_DAY
				|| dv->dv_day < AD_27DTE_MIN_INT_DAY))
	{
	    char days[5] = {'d','a','y','s','\0'};
	    return (adu_error(adf_scb, (i4) E_AD5062_DATEOVFLO, 4, 
			      (i4) sizeof(dv->dv_day),
			      (i4 *) &dv->dv_day, (i4) sizeof(days), 
				(PTR) days));
        }

        dn->dn_day    = dv->dv_day;
        dn->dn_status   |= AD_DN_DAYSPEC;
    }
    else if (datetype == DV_ABSOLUTE)
    {
	/* Initialize current time vector for use in defaults */
	if (!ctv_init && (db_stat = ad0_1cur_init(adf_scb, &cur_tv, 
						now, highprec)))
	    return (db_stat);
	ctv_init = TRUE;

        dn->dn_day  = cur_tv.tm_mday;
    }

    /*
    ** Have year month day so far, try to set time.  The time is stored in
    ** seconds and nanoseconds since the transformation to hr, min, sec 
    ** is transitive.
    */

    secs = 0;

    if (status & (DV_HOUR | DV_MINUTE | DV_SECOND | DV_NANOSECOND))
    {
        /* Check time range for absolute date */

        if (dn->dn_status & AD_DN_ABSOLUTE)
        {
#           ifdef xDEBUG
	    if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_004_DATE_SUPPORT,
		    &dum1, &dum2))
            {
                TRdisplay("date: absolute date with dv->dv_hour=%d\n", 
						dv->dv_hour);
                TRdisplay("date: and dv->dv_minute=%d\n", dv->dv_minute);
                TRdisplay("date: and dv->dv_second=%d\n", dv->dv_second);
                TRdisplay("date: and dv->dv_nanosecond=%d\n", dv->dv_nanosecond);
            }
#           endif

            if (dv->dv_hour >= 24 || dv->dv_hour < 0 || dv->dv_minute >= 60
                || dv->dv_second >= 60)
            {
                return (adu_error(adf_scb, (i4) E_AD5056_DATETIME, 6,
				  (i4) sizeof(dv->dv_hour),
				  (i4 *) &dv->dv_hour, 
				  (i4) sizeof(dv->dv_minute),
				  (i4 *) &dv->dv_minute,
				  (i4) sizeof(dv->dv_second),
				  (i4 *) &dv->dv_second));
            }
        }

        /* add in hours */
        secs += dv->dv_hour;

#       ifdef xDEBUG
	if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_004_DATE_SUPPORT,
		&dum1, &dum2))
            TRdisplay("date: After hours we have %d hours\n", secs);
#       endif

        secs *= 60;
        /* add in minutes */
        secs += dv->dv_minute;

#       ifdef xDEBUG
	if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_004_DATE_SUPPORT,
		&dum1, &dum2))
            TRdisplay("date: after minutes we have %d mins.\n", secs);
#       endif

        secs *= 60;

        /* add in seconds */
        secs += dv->dv_second;

	if (status & DV_AMPM)
	{
            /* adjust for AM or PM on absolute tiime references */
            if (datetype == DV_ABSOLUTE || datetype == DV_OFFSET)
            {
                /* if hour specified is 12, adjust time value */
                if (dv->dv_hour == 12)
                     secs -= (dv->dv_hour * AD_39DTE_ISECPERHOUR);

                secs += (dv->dv_ampm * AD_39DTE_ISECPERHOUR); 
	    }
	    else
	    {
                /* this is illegal in any other date type */
                return (adu_error(adf_scb, (i4)E_AD5059_DATEAMPM, 0)); 
	    }
	}

#       ifdef xDEBUG
        if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_004_DATE_SUPPORT,
		&dum1, &dum2))
            TRdisplay("date: after seconds we have %d secs\n", secs);
#       endif

	/*
        ** Adjust to timezone specified, or adjust to GMT from local timezone.
        ** We assume timezone is correct for offset to
	** local time in milliseconds if not changed.
	** Note: For time/timestamp with/without timezone the secs is 
	** adjusted as follows:
	** For time/timestamp without timezone - It is truncated.
	** For time/timestamp with timezone - It is added to internal struct.
	*/

        if (datetype != DV_LENGTH)
	{
	    dn->dn_seconds = secs;
	    dn->dn_nsecond = dv->dv_nanosecond;
	    if(status & DV_TIMEZONE)
	    {
		if ((dn->dn_dttype == DB_TMW_TYPE) || 
	    	    (dn->dn_dttype == DB_TSW_TYPE))
		{
                    dn->dn_tzhour = ((i4) dv->dv_timezone / 60);
                    dn->dn_tzminute = ((i4) dv->dv_timezone % 60);
		}
	        else if ((dn->dn_dttype != DB_TMWO_TYPE) &&
	   	    (dn->dn_dttype != DB_TSWO_TYPE))
                    secs -= ((i4) dv->dv_timezone * AD_10DTE_ISECPERMIN);
	    }
	    else if ((dn->dn_dttype != DB_TMWO_TYPE) && 
			(dn->dn_dttype != DB_TMW_TYPE) && 
	   		(dn->dn_dttype != DB_TSWO_TYPE) && 
	    		(dn->dn_dttype != DB_TSW_TYPE))
	    {
		secs -= TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL, 
			     adu_5time_of_date(dn));
#   ifdef xDEBUG
                TRdisplay("ad0_4date: TMtz_search to adjust for  datatype=%d \n", 
					dn->dn_dttype);
#   endif
	    }
	    else if ((dn->dn_dttype == DB_TMW_TYPE) || 
		     (dn->dn_dttype == DB_TSW_TYPE))
	    {
		/* Store the local timezone if none provided */ 
		i4 tztemp = TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL, 
			     adu_5time_of_date(dn));
                dn->dn_tzhour = ((i4) tztemp / 3600);
                dn->dn_tzminute = ((i4) tztemp % 3600)/60;
	    }
	}

        dn->dn_seconds = secs;
	dn->dn_nsecond = dv->dv_nanosecond;
        dn->dn_status |= AD_DN_TIMESPEC;
	if (!(dn->dn_status & AD_DN_LENGTH))
	    adu_1normldate(dn);		/* normalize the sucker - no intervals,
					** though */

#       ifdef xDEBUG
	if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_004_DATE_SUPPORT,
		&dum1, &dum2))
            TRdisplay("date: final time is %d\n", secs);
#       endif

    }
    else
    {
        /* no time specified, assume it's just a date */
        if (datetype == DV_ABSOLUTE)
            dn->dn_seconds = 0;
    }

    /* if interval, normalize and check range before returning */

    if (dn->dn_status & AD_DN_LENGTH)
    {
        adu_2normldint(dn);

        if (dn->dn_year > MAX_INV_YR || dn->dn_year < MIN_INV_YR)
        {
            return (adu_error(adf_scb, (i4) E_AD505A_DATEYROVFLO, 2,
			      (i4)sizeof(dn->dn_year),
			      (i4 *) &dn->dn_year));
        }
    }
    else
    {
        if(now)
	{
            adu_1normldate(dn);
	    dn->dn_status |= AD_DN_YEARSPEC | AD_DN_MONTHSPEC |
		AD_DN_DAYSPEC | AD_DN_TIMESPEC;
	}      
	else if (time_type == DTE_OFFSET_OR_TODAY)
	    dn->dn_status |= AD_DN_YEARSPEC | AD_DN_MONTHSPEC |
		AD_DN_DAYSPEC;
    }

    return (E_DB_OK);
}

/*
** Name: ad0_1cur_init() -   initialize the time vector with current values.
**
** Description:
**
** History:
**	01-aug-06 (gupsh01)
**	    History section was added. Also now use TMhrnow() instead of 
**	    TMnow to initialize the time vector as we now support values 
**	    upto nanoseconds.
**	06-sep-06 (gupsh01)
**	    Added highprec parameter to indicate whether nanoseconds
**	    capability is needed.
*/

DB_STATUS
ad0_1cur_init(
ADF_CB		    *adf_scb,
struct	timevect    *tv,
bool                now,
bool		    highprec)
{
    VOID		adu_cvtime();
    i4			bintim_secs;
    i4		        sec_time_zone;
    DB_DATA_VALUE	dv_tmp;
    DB_STATUS		db_stat;

    HRSYSTIME		t_now;
    
    dv_tmp.db_datatype = DB_INT_TYPE;
    dv_tmp.db_length = 4;
    dv_tmp.db_data = (PTR) &bintim_secs;

    if (db_stat = adu_dbconst(adf_scb, &Adf_globs->Adk_bintim_map, &dv_tmp))
        return (db_stat);
	
    /* For date("now") case update with current time */
    if(now)
    {
	TMhrnow(&t_now);
        bintim_secs = t_now.tv_sec;
    }

    sec_time_zone = TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_GMT, 
				 bintim_secs);
    if (highprec)
      adu_cvtime((i4) bintim_secs + sec_time_zone, t_now.tv_nsec, tv);
    else
      adu_cvtime((i4) bintim_secs + sec_time_zone, 0, tv);

    return (E_DB_OK);
}



/*
** Name: adu_6datetostr() -	convert internal date to string representation.
**
** Description:
**	  This will convert either an absolute time or a time interval to a
**	string representation.  At the present time, we always represent times
**	as 25 character strings with trailing blanks.
**
**	For an absolute time one of the following representations is used:
**	    Date only   "dd-mmm-yyyy"
**	    Time only   "hh:mm:ss.xxxx"
**	    Date and Time   "dd-mmm-yyyy hh:mm:ss.xxxx
**
**	  Times are always displayed for the current timezone as specified when
**	INGRES is built.
**
**	  For a time interval, we display the most significant portions of the
**	interval which will fit in the 25 characters as follows:
**
**	    %d yrs %d mos %d days %d hrs %d mins %d secs %d msec
**
**	  Thus, if you fill in all fields in an interval you will most likely
**	see only yrs, mos and days.
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**	    .adf_dfmt			The output date format to be used in
**					converting to the string
**	date_dv				DB_DATA_VALUE describing the date to
**					be converted.
**	    .db_data			Ptr to date type internal struct.  Dates are
**					always stored in GMT.
**		.dn_status		A character whose bits describe the
**					internal date format.  (For more
**                                      detailed information see adudates.h).
**		.dn_highday		A character for large day intervals.
**		.dn_year		The date's year.
**		.dn_month		The date's month.
**		.dn_lowday		If the date is absolute, this is the
**					day of the month.  If the date is an
**					interval, this is the low-order part
**					of the number of days in the interval.
**		.dn_seconds		The time in seconds from 00:00.
**		.dn_nsecond		Fractional seconds in nanoseconds
**      str_dv                          DB_DATA_VALUE describing the string
**					that the date is to be converted into.
**	    .db_datatype		The type of string.
**	    .db_length			The length allocated for it.
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	str_dv
**	    .db_data			If the type is character, this is a ptr
**					to the string date produced.  If the
**					type is "text" or "varchar", this is a
**					ptr to a DB_TEXT_STRING holding the date
**					string produced.
**		[.db_t_count]		The length of the date string produced.
**		[.db_t_text]		Ptr to the text date string produced.
**
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**	    Who knows what else.
**
** History:
**	27-may-86 (ericj)
**	    Converted for Jupiter.
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	20-may-88 (thurston)
**          Fixed bug that occurred when the string representation of a time
**          interval was too long to fit into the destination string (which
**          should be 25 characters max).  This is supposed to truncate the
**          string after the last piece that completely fits but was just
**          truncating the string instead.
**	    Example:  The string
**			"22 yrs 11 mos 14 days 5 hrs"
**	    was being returned as
**			"22 yrs 11 mos 14 days 5 h"
**          instead of
**			"22 yrs 11 mos 14 days"
**	19-jan-89 (jrb)
**	    Fixed problem with above fix from thurston where the last field
**	    really could fit into the 25 character field, but he was chopping
**	    it off.
**	23-jan-89 (jrb)
**	    Changed so that if TIMESPEC was set on an interval which was all
**	    zeros, we will print '0 hrs' instead of all blanks.
**  	06-may-92 (stevet)
**  	    Added support for DB_YMD_DFMT, DB_MDY_DFMT, DB_DMY_DFMT
**          date formats.
**      29-aug-92 (stevet)
**          Modified timezone adjustment method by calling TMtz_search
**          to calculate correct timezone offset.
**	07-apr-95 (pchang)
**	    Fixed bug which caused time intervals that were within the
**	    25-characters limit to be truncated in time-interval units so as
**	    to fit into user-specified string buffers.  They should simply be
**	    string truncated to avoid unacceptable data loss when used in a
**	    copy/into. (b51185).  Examples:
**	    (1) if (str_dv->db_length == 12)
**	    the time interval
**			"10 yrs 11 mos"
**	    were being returned as
**			"10 yrs"
**	    (2) if (str_dv->db_length == 6)
**	    the time intervals
**			"100 yrs",  "10 days",  "11 mins"  and  "12 secs"
**	    were being returned as empty strings.
**	31-jul-96 (nick)
**	    Change break loop to avoid compiler warning.
**	3-feb-98 (stephenb)
**	    Milisecond part of the date should be a 3-digit format not 
**	    4-digit.
**	17-jul-2006 (gupsh01)
**	    Added support for new datetime types.
**	24-jul-2006 (gupsh01)
**	    Fixed time with local time zone output adjustment according
**	    to local DST.
**	30-jul-2006 (gupsh01)
**	    Added support for nanoseconds. Print ANSI date/time only in 
**	    TMD format.
**	02-aug-2006 (gupsh01)
**	    Fix the format for Time and timezone types.
**	09-aug-2006 (gupsh01)
**	    Fix time and timezone formatting to match ANSI spec.
**	21-sep-2006 (gupsh01)
**	    Fix time with timezone output format.
**	05-oct-2006 (gupsh01)
**	    For ingresdate data type. Don't print the fractional portion
**	    of the seconds value as this breaks existing applications like 
**	    JDBC.
**	31-oct-2006 (gupsh01)
**	    For Interval types do not print in the ingresdate format but
**	    use ANSI specification for intervals.
**	13-dec-2006 (gupsh01)
**	    If value of time sepcified are all 0 then no values are being 
**	    printed
**	15-feb-2007 (gupsh01)
**	    Fix the time with local time zone and timestamp with local time
**	    zone case.
**	12-mar-2007 (gupsh01)
**	    Fix the fractional seconds, which are not printed out correctly
**	    if there are leading 0's. 
**	10-apr-2007 (gupsh01)
**	    If only the microsecond value is nonzero and negative the 
**	    interval day to second does not print out as negative.
*/

DB_STATUS
adu_6datetostr(
ADF_CB		    *adf_scb,
DB_DATA_VALUE	    *date_dv,
DB_DATA_VALUE	    *str_dv)
{
    register AD_NEWDTNTRNL  *nd;
    AD_NEWDTNTRNL	local_offset;	    /* timezone offset in internal
					    ** date format.
					    */
    DB_STATUS		db_stat;
    i4		hr,
			mn,
			sc,
			ns;
    i4			signmult;
    char		tmpbuffer[15];
    char		temp[128];
    i4			max_str_len;
    AD_NEWDTNTRNL	ndatev;
    DB_DATA_VALUE	str2_dv;
    DB_DATA_VALUE	tdate_dv;
    AD_DATENTRNL	tdate;
    char		formatbuf[50];
    char		adtformat[15] = "%04d-%02d-%02d"; 
    char		atmformat[18] = "%02d:%02d:%02d.%0";
    char		atzdformat[10] = "%02d:%02d";
    char		aptzdformat[11] = "+%02d:%02d";
    char		antzdformat[11] = "-%02d:%02d";
    i4			mod_str_len;
    i4 			cnt = 0;

    /* determine max output string length... should be 25, but hey, who knows */
    switch (str_dv->db_datatype)
    {
      case DB_CHA_TYPE:
      case DB_CHR_TYPE:
	  case DB_NCHR_TYPE:
	max_str_len = str_dv->db_length;
	break;
	
      case DB_VCH_TYPE:
      case DB_TXT_TYPE:
      case DB_LTXT_TYPE:
	  case DB_NVCHR_TYPE:
	max_str_len = str_dv->db_length - DB_CNTSIZE;
	break;

      default:
	return (adu_error(adf_scb, (i4) E_AD9999_INTERNAL_ERROR, 0));
    }

    /* max length of date output string */
    MEfill ((u_i2) sizeof(temp), NULLCHAR, (PTR) &temp);

    /* for interval types set the max length */
    switch (date_dv->db_datatype)
    {
	case DB_DTE_TYPE:
    	    if (max_str_len > AD_1DTE_OUTLENGTH)
	        max_str_len = AD_1DTE_OUTLENGTH;
	    break;
	case DB_INYM_TYPE:
    	    if (max_str_len > AD_9INYM_OUTLENGTH)
	        max_str_len = AD_9INYM_OUTLENGTH;
	    break;
	case DB_INDS_TYPE:
    	    if (max_str_len > AD_10INDS_OUTLENGTH)
	        max_str_len = AD_10INDS_OUTLENGTH;
	    break;
    }

    nd = &ndatev;
    MEfill ((u_i2) sizeof(ndatev), NULLCHAR, (PTR) &ndatev);
    adu_6to_dtntrnl (date_dv, &ndatev);

    /* set up local_offset for use by add_date */
    MEfill(sizeof(local_offset), NULLCHAR, (PTR) &local_offset);

    /* For time with local timezone need to initialize 
    ** the year month and day to current ymd so that the
    ** right DST is applied in output.
    */
    if (nd->dn_dttype == DB_TME_TYPE)
    {
        MEfill ((u_i2) sizeof(tdate), NULLCHAR, (PTR) &tdate);
        tdate_dv.db_data = (PTR) &tdate;
        tdate_dv.db_datatype = DB_DTE_TYPE;
        tdate_dv.db_length = sizeof (AD_DATENTRNL);
        tdate_dv.db_prec = 0;
      
        if (db_stat = adu_datenow( adf_scb, &tdate_dv))
        {
	  /* return an error */
        }
        else 
        {
	  nd->dn_year = tdate.dn_year;
	  nd->dn_month = tdate.dn_month;
	  nd->dn_day = tdate.dn_lowday;
        }
    }
    /* for ingresdate data type. Don't print the fractional portion of 
    ** the seconds value as this breaks existing applications like JDBC.
    */
    else if (nd->dn_dttype == DB_DTE_TYPE)
	  nd->dn_nsecond = 0;

    local_offset.dn_seconds = TMtz_search(adf_scb->adf_tzcb, 
			     TM_TIMETYPE_GMT, adu_5time_of_date(nd)); 
    local_offset.dn_status = AD_DN_LENGTH | AD_DN_TIMESPEC;

    /* decide if this is absolute time or interval */

    if ((nd->dn_status & AD_DN_ABSOLUTE) && 
	((nd->dn_dttype != DB_INYM_TYPE) ||
	 (nd->dn_dttype != DB_INDS_TYPE)))
    {

#       ifdef xDEBUG
        if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_003_DATEFI_TRACE,
		&dum1, &dum2))
            TRdisplay("datetostr: converting absolute time\n");
            TRdisplay("datetostr: The time is : year = %d, ",
			(i4)nd->dn_year);
	    TRdisplay("month = %d, day = %d, time = %d.%09d, status = %d\n",
			(i4)nd->dn_month,
			(i4)((I1_CHECK_MACRO(nd->dn_day))),
			nd->dn_seconds, nd->dn_nsecond, nd->dn_status);
#       endif

        /* times are stored as GMT, make it local time */

        if (((nd->dn_dttype == DB_DTE_TYPE) && (nd->dn_status & AD_DN_TIMESPEC)) || 
	    (nd->dn_dttype == DB_TSTMP_TYPE) || 
	    (nd->dn_dttype == DB_TME_TYPE))
	{
	    /*	Add the dates together putting the results in local_offset,
	    **	so that the input date doesn't get altered as a side effect,
	    **	and then treat local_offset as the date to be converted.
	    */
	    DB_DATA_VALUE result;
	    char result_buffer[AD_10INDS_OUTLENGTH];
	    result.db_data = result_buffer; 
	    result.db_length = sizeof (result_buffer);
	    result.db_datatype = date_dv->db_datatype;
	    result.db_prec = date_dv->db_prec;
            if ((db_stat = add_date(adf_scb, &result, &local_offset, nd)) 
			!= E_DB_OK)
		return (db_stat);
	    adu_6to_dtntrnl (&result, nd);
	}

#       ifdef xDEBUG
	if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_003_DATEFI_TRACE,
		&dum1, &dum2))
	{
            TRdisplay("datetostr: timezone adjust returns: year = %d, ",
			(i4)nd->dn_year);
	    TRdisplay("month = %d, day = %d, time = %d.%09d, status = %d\n",
			(i4)nd->dn_month,
			(i4)((I1_CHECK_MACRO(nd->dn_day))),
			nd->dn_seconds, nd->dn_nsecond, nd->dn_status);
	}
#       endif

        /* If hours, minutes, seconds specified print full time */

	if ((( nd->dn_dttype == DB_DTE_TYPE ) &&
             (!(nd->dn_status & AD_DN_TIMESPEC))) ||
            ( nd->dn_dttype == DB_ADTE_TYPE ))
        {
            /* Just print date */
	  if (nd->dn_dttype == DB_DTE_TYPE)
	  {
            switch (adf_scb->adf_dfmt)
            {
              case DB_US_DFMT:
              case DB_DMY_DFMT:
                STprintf(temp, "%02d-%3s-%04d", (i4)nd->dn_day,
		    Adu_monthtab[nd->dn_month-1].code, (i4)nd->dn_year);
                break;

              case DB_MLTI_DFMT:
                STprintf(temp, "%02d/%02d/%02d",
		    (i4)nd->dn_day, (i4)nd->dn_month, (i4)(nd->dn_year % 100));
                break;

              case DB_MLT4_DFMT:
                STprintf(temp, "%02d/%02d/%04d",
		    (i4)nd->dn_day, (i4)nd->dn_month, (i4)nd->dn_year);
                break;

              case DB_FIN_DFMT:
                STprintf(temp, "%04d-%02d-%02d",
		    (i4)nd->dn_year, (i4)nd->dn_month, (i4)nd->dn_day);
                break;

              case DB_ISO_DFMT:
                STprintf(temp, "%02d%02d%02d",
                    (i4)(nd->dn_year % 100), (i4)nd->dn_month, (i4)nd->dn_day);
                break;

              case DB_ISO4_DFMT:
                STprintf(temp, "%04d%02d%02d",
                    (i4)nd->dn_year, (i4)nd->dn_month, (i4)nd->dn_day);
                break;

              case DB_GERM_DFMT:
                STprintf(temp, "%02d.%02d.%04d",
                    (i4)nd->dn_day, (i4)nd->dn_month, (i4)nd->dn_year);
                break;

              case DB_YMD_DFMT:
                STprintf(temp, "%04d-%3s-%02d", (i4)nd->dn_year,
		    Adu_monthtab[nd->dn_month-1].code, (i4)nd->dn_day);
    	    	break;

              case DB_MDY_DFMT:
                STprintf(temp, "%3s-%02d-%04d", 
    	    	    Adu_monthtab[nd->dn_month-1].code, (i4)nd->dn_day, 
    	    	    (i4)nd->dn_year);
    	    	break;

              default:
                return (adu_error(adf_scb, (i4) E_AD5060_DATEFMT, 2,
				  (i4) sizeof (adf_scb->adf_dfmt),
				  (i4 *) adf_scb->adf_dfmt));
            }
	  }
	  else if ( nd->dn_dttype == DB_ADTE_TYPE )
	  {
		/* ANSI DATES are in form 'yyyy-mm-dd' */
                STprintf(temp, "%04d-%02d-%02d", (i4)nd->dn_year,
		    nd->dn_month, (i4)nd->dn_day);
	  }
        }
        else if ( nd->dn_dttype == DB_TME_TYPE ||
		  nd->dn_dttype == DB_TMW_TYPE ||
		  nd->dn_dttype == DB_TMWO_TYPE)
	{

	    /* Just the time */
            hr = nd->dn_seconds;
            if (date_dv->db_prec <= 0 || date_dv->db_prec > 9)
                ns = 0;
            else
                ns = nd->dn_nsecond / nano_mult[date_dv->db_prec];

	    mn = hr % AD_39DTE_ISECPERHOUR;

            sc = hr % 60;
            hr /= 60;

            mn = hr % 60;
            hr /= 60;

	    if ( nd->dn_dttype != DB_TMW_TYPE) 
	    {
	      if (date_dv->db_prec != 0)
	      {
            	STprintf(formatbuf, "%s0%dd", atmformat, date_dv->db_prec);
                STprintf(temp, formatbuf, hr, mn, sc, ns);
	      }
	      else
                STprintf(temp, "%02d:%02d:%02d",
                        hr, mn, sc);
	    }
	    else
	    {
	      i1 hrnorm = nd->dn_tzhour;
	      i1 minnorm = nd->dn_tzminute;
	      bool useplus = TRUE;

	      if (hrnorm < 0) 
	      {
	        hrnorm = -hrnorm;
	        useplus = FALSE;
	      }
	      if (minnorm < 0) 
	      {
	        minnorm = -minnorm;
	        useplus = FALSE;
	      }
	      /* Print the timezone if with time zone data type*/
	      if (date_dv->db_prec != 0)
	      {
		if (useplus == TRUE)
		{
            	    STprintf(formatbuf, "%s0%dd%s", atmformat, date_dv->db_prec, 
			aptzdformat);
		}	
		else 
		{
            	    STprintf(formatbuf, "%s0%dd%s", atmformat, date_dv->db_prec, 
			antzdformat);
		}
                STprintf(temp, formatbuf, hr, mn, sc, ns, 
				hrnorm, minnorm);
	      }
	      else
	      {
		if (useplus == TRUE)
		{
                  STprintf(temp, "%02d:%02d:%02d+%02d:%02d",
                        hr, mn, sc, nd->dn_tzhour, nd->dn_tzminute);
		}
		else
		{
                  STprintf(temp, "%02d:%02d:%02d-%02d:%02d",
                        hr, mn, sc, hrnorm, minnorm);
		}
	      }
	    }
	}
	else if (( nd->dn_dttype == DB_TSW_TYPE ) ||
		 ( nd->dn_dttype == DB_TSWO_TYPE ) ||
		 ( nd->dn_dttype == DB_TSTMP_TYPE ))
        {
            hr = nd->dn_seconds;
	    mn = hr % AD_39DTE_ISECPERHOUR;

            if (date_dv->db_prec <= 0 || date_dv->db_prec > 9)
                ns = 0;
            else
                ns = nd->dn_nsecond / nano_mult[date_dv->db_prec];

            sc = hr % 60;
            hr /= 60;

            mn = hr % 60;
            hr /= 60;

	    if ( date_dv->db_prec != 0 )
        	MEfill ((u_i2) sizeof(formatbuf), NULLCHAR, (PTR) formatbuf);

	    if ( nd->dn_dttype == DB_TSW_TYPE )
	    {
	      i1 hrnorm = nd->dn_tzhour;
	      i1 minnorm = nd->dn_tzminute;
	      bool useplus = TRUE;

	      if (hrnorm < 0) 
	      {
	        hrnorm = -hrnorm;
	        useplus = FALSE;
	      }
	      if (minnorm < 0) 
	      {
	        minnorm = -minnorm;
	        useplus = FALSE;
	      }

              if (date_dv->db_prec != 0)
              {
		if (useplus == TRUE)
            	    STprintf(formatbuf, "%s %s%dd%s", adtformat, atmformat, 
			  (date_dv->db_prec) ? (date_dv->db_prec):6, 
			  aptzdformat);
		else 
            	    STprintf(formatbuf, "%s %s%dd%s", adtformat, atmformat, 
			  (date_dv->db_prec) ? (date_dv->db_prec):6, 
			  antzdformat);

                STprintf(temp, formatbuf,
	                 (i4)nd->dn_year, (i4)nd->dn_month,
                	(i4)nd->dn_day, hr, mn, sc, ns,
	  		hrnorm, minnorm);
              }
              else
              {
		if (useplus == TRUE)
                  STprintf(temp, "%04d-%02d-%02d %02d:%02d:%02d+%02d:%02d",
                    	(i4)nd->dn_year, (i4)nd->dn_month,
                   	(i4)nd->dn_day, hr, mn, sc,
	       		hrnorm, minnorm);
		else 
                  STprintf(temp, "%04d-%02d-%02d %02d:%02d:%02d-%02d:%02d",
                    	(i4)nd->dn_year, (i4)nd->dn_month,
                   	(i4)nd->dn_day, hr, mn, sc,
	       		hrnorm, minnorm);
              }
	    } 
	    else
	    {
                if (date_dv->db_prec != 0)
	        {
            	  STprintf(formatbuf, "%s %s%d%s", adtformat, atmformat, 
			  (date_dv->db_prec) ? (date_dv->db_prec):6, "d");
                  STprintf(temp, formatbuf,
                        (i4)nd->dn_year, (i4)nd->dn_month, 
                        (i4)nd->dn_day, hr, mn, sc, ns);
	        }	
		else 
	        {
                    STprintf(temp, "%04d-%02d-%02d %02d:%02d:%02d",
                        (i4)nd->dn_year, (i4)nd->dn_month,
                        (i4)nd->dn_day, hr, mn, sc);
	        }	
	    }
        }
	else
	{
            /* full time specified or else input was for another timezone */
	    /*                    ^_________________ huh? _______________^ */
            hr = nd->dn_seconds;
            ns = nd->dn_nsecond;
	    mn = hr % AD_39DTE_ISECPERHOUR;

            sc = hr % 60;
            hr /= 60;

            mn = hr % 60;
            hr /= 60;

            if (ns != 0)
            {
		i4 ms = ns / AD_29DTE_NSPERMS;

                switch (adf_scb->adf_dfmt)
                {
                  case DB_US_DFMT:
                  case DB_DMY_DFMT:
                    STprintf(temp, "%02d-%3s-%04d %02d:%02d:%02d.%03d",
                        (i4)nd->dn_day,
                        Adu_monthtab[nd->dn_month-1].code,
                        (i4)nd->dn_year, hr, mn, sc, ms);
                    break;

                  case DB_MLTI_DFMT:
                    STprintf(temp, "%02d/%02d/%02d %02d:%02d:%02d.%03d",
                        (i4)nd->dn_day,
                        (i4)nd->dn_month, (i4)(nd->dn_year % 100),
                        hr, mn, sc, ms);
                    break;

                  case DB_MLT4_DFMT:
                    STprintf(temp, "%02d/%02d/%04d %02d:%02d:%02d.%03d",
                        (i4)nd->dn_day,
                        (i4)nd->dn_month, (i4)nd->dn_year,
                        hr, mn, sc, ms);
                    break;

                  case DB_FIN_DFMT:
                    STprintf(temp, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
                        (i4)nd->dn_year,
                        (i4)nd->dn_month, (i4)nd->dn_day, hr, mn, sc, ms);
                    break;

                  case DB_ISO_DFMT:
                    STprintf(temp, "%02d%02d%02d %02d:%02d:%02d.%03d",
                        (i4)(nd->dn_year % 100),
                        (i4)nd->dn_month, (i4)nd->dn_day, hr, mn, sc, ms);
                    break;

                  case DB_ISO4_DFMT:
                    STprintf(temp, "%04d%02d%02d %02d:%02d:%02d.%03d",
                        (i4)nd->dn_year,
                        (i4)nd->dn_month, (i4)nd->dn_day, hr, mn, sc, ms);
                    break;

                  case DB_GERM_DFMT:
                    STprintf(temp, "%02d.%02d.%04d %02d:%02d:%02d.%03d",
			(i4)nd->dn_day, (i4)nd->dn_month, (i4)nd->dn_year,
			hr, mn, sc, ms);
                    break;

                  case DB_YMD_DFMT:
                    STprintf(temp, "%04d-%3s-%02d %02d:%02d:%02d.%03d",
                        (i4)nd->dn_year,
                        Adu_monthtab[nd->dn_month-1].code,
                        (i4)nd->dn_day, hr, mn, sc, ms);
                    break;

                  case DB_MDY_DFMT:
                    STprintf(temp, "%3s-%02d-%04d %02d:%02d:%02d.%03d",
                        Adu_monthtab[nd->dn_month-1].code,
                        (i4)nd->dn_day, (i4)nd->dn_year, hr, mn, sc, ms);
                    break;

                  default:
		    return (adu_error(adf_scb, (i4) E_AD5060_DATEFMT, 2,
				      (i4) sizeof (adf_scb->adf_dfmt), 
				      (i4 *) adf_scb->adf_dfmt));
                }
            }
            else
            {
                switch (adf_scb->adf_dfmt)
                {
                  case DB_US_DFMT:
                  case DB_DMY_DFMT:
                    STprintf(temp, "%02d-%3s-%04d %02d:%02d:%02d",
                        (i4)nd->dn_day,
                        Adu_monthtab[nd->dn_month-1].code,
                        (i4)nd->dn_year, hr, mn, sc);
                    break;

                  case DB_MLTI_DFMT:
                    STprintf(temp, "%02d/%02d/%02d %02d:%02d:%02d",
                        (i4)nd->dn_day,
                        (i4)nd->dn_month, (i4)(nd->dn_year % 100),
                        hr, mn, sc);
                    break;

                  case DB_MLT4_DFMT:
                    STprintf(temp, "%02d/%02d/%04d %02d:%02d:%02d",
                        (i4)nd->dn_day,
                        (i4)nd->dn_month, (i4)nd->dn_year,
                        hr, mn, sc);
                    break;

                  case DB_FIN_DFMT:
                    STprintf(temp, "%04d-%02d-%02d %02d:%02d:%02d",
                        (i4)nd->dn_year,
                        (i4)nd->dn_month, (i4)nd->dn_day, hr, mn, sc);
                    break;

                  case DB_ISO_DFMT:
                    STprintf(temp, "%02d%02d%02d %02d:%02d:%02d",
                        (i4)(nd->dn_year % 100),
                        (i4)nd->dn_month, (i4)nd->dn_day, hr, mn, sc);
                    break;

                  case DB_ISO4_DFMT:
                    STprintf(temp, "%04d%02d%02d %02d:%02d:%02d",
                        (i4)nd->dn_year,
                        (i4)nd->dn_month, (i4)nd->dn_day, hr, mn, sc);
                    break;

                  case DB_GERM_DFMT:
                    STprintf(temp, "%02d.%02d.%04d %02d:%02d:%02d",
			(i4)nd->dn_day, (i4)nd->dn_month, (i4)nd->dn_year,
			hr, mn, sc);
                    break;

                  case DB_YMD_DFMT:
                    STprintf(temp, "%04d-%3s-%02d %02d:%02d:%02d",
                        (i4)nd->dn_year,
                        Adu_monthtab[nd->dn_month-1].code,
                        (i4)nd->dn_day, hr, mn, sc);
                    break;

                  case DB_MDY_DFMT:
                    STprintf(temp, "%03s-%02d-%04d %02d:%02d:%02d",
                        Adu_monthtab[nd->dn_month-1].code,
                        (i4)nd->dn_day, (i4)nd->dn_year, hr, mn, sc);
                    break;

                  default:
		    return (adu_error(adf_scb, (i4) E_AD5060_DATEFMT, 2,
				      (i4) sizeof (adf_scb->adf_dfmt),
				      (i4 *) adf_scb->adf_dfmt));
                }
            }
        }
    }
    else
    {
        /*
        ** This is a time interval or empty date which must be printed, we will
	** determine what to print by the specification.  i.e. each component
	** which is specified in the status field will be displayed.
        */

#       ifdef xDEBUG
        if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_003_DATEFI_TRACE,
		&dum1, &dum2))
            TRdisplay("datetostr: printing time interval or empty date\n");
#       endif

        temp[0] = EOS;

	do
	{
	    i4	    last_len = 0;
	    i4	    temp_len;
	    i4	    tp;
	    bool    need_space = FALSE;
	    char    fmtbuffer[25];

	    if (nd->dn_dttype == DB_INYM_TYPE)
	    { 
               signmult = 1;
	       if ((nd->dn_year < 0) || (nd->dn_month < 0))
		 signmult = -1;

	       if (date_dv->db_prec)
	       {	
		 if (signmult < 0)
                   STprintf(fmtbuffer, "-%s%d%s-%s", "%0", 
				date_dv->db_prec, "d", "%d");
		 else
                   STprintf(fmtbuffer, "%s%d%s-%s", "%0", 
				date_dv->db_prec, "d", "%d");

                 STprintf(temp, fmtbuffer, abs(nd->dn_year), abs(nd->dn_month));
	       }	
	       else
	       {
	 	 if (signmult < 0)	
                   STprintf(temp, "-%d-%02d", 
			abs(nd->dn_year), abs(nd->dn_month) );
		 else
                   STprintf(temp, "%d-%02d", 
			nd->dn_year, nd->dn_month * signmult);
		}
	    }
	    else if (nd->dn_dttype == DB_INDS_TYPE)
	    {
		hr = nd->dn_seconds;

                signmult = 1;

                if (hr < 0)
                {
                    signmult = -1;
                    hr = -hr;
                }

                ns = nd->dn_nsecond;
                if (ns < 0)
	        {
                    signmult = -1;
                    ns = -ns;
                }

		if (date_dv->db_prec <= 0 || date_dv->db_prec > 9)
		    ns = 0;
		else
		    ns /= nano_mult[date_dv->db_prec];

                mn = hr % AD_39DTE_ISECPERHOUR;

                sc = hr % 60;
                hr /= 60;

                mn = hr % 60;
                hr /= 60;

	       if (date_dv->db_prec)
	       {
		  if ((signmult < 0) || (nd->dn_day < 0))
                   STprintf(fmtbuffer, "-%s%s0%d%s", "%d %02d:%02d:%02d.", 
				"%0", date_dv->db_prec, "d");
		  else
                   STprintf(fmtbuffer, "%s%s0%d%s", "%d %02d:%02d:%02d.", 
				"%0", date_dv->db_prec, "d");

                   STprintf(temp, fmtbuffer, abs(nd->dn_day), 
				abs(hr), abs(mn), abs(sc), ns);
	       }
	       else
	       { 
		  if ((signmult < 0) || (nd->dn_day < 0))
                     STprintf(temp, "-%d %02d:%02d:%02d", 
			   abs(nd->dn_day), abs(hr), 
			   abs(mn), abs(sc));
		  else
                     STprintf(temp, "%d %02d:%02d:%02d", 
			   nd->dn_day, hr, mn, sc);
	       } 
	    }
	    else 
	    {
	      if (nd->dn_status & AD_DN_YEARSPEC)
	      {
		_VOID_ CVla((i4) nd->dn_year, temp);

		STcat(temp, " yrs");

		if ((temp_len = STlength(temp)) > max_str_len)
		{
		    temp[temp_len] = EOS;
		    break;
		}
		last_len = temp_len;
		need_space = TRUE;
	      }

	      if (nd->dn_status & AD_DN_MONTHSPEC)
	      {
		_VOID_ CVla((i4) nd->dn_month, tmpbuffer);

		STpolycat((i4)4,
			    temp,
			    (need_space ? " " : ""),
			    tmpbuffer,
			    " mos",
			    temp);
		if ((temp_len = STlength(temp)) > max_str_len)
		{
		    if (temp_len > max_str_len)
			temp[last_len] = EOS;
		    else
			temp[temp_len] = EOS;
		    break;
		}
		last_len = temp_len;
		need_space = TRUE;
	      }

	    if (nd->dn_status & AD_DN_DAYSPEC)
	    {
		_VOID_ CVla((i4) (I1_CHECK_MACRO(nd->dn_day)), tmpbuffer);

		STpolycat((i4)4,
			    temp,
			    (need_space ? " " : ""),
			    tmpbuffer,
			    " days",
			    temp);
		if ((temp_len = STlength(temp)) > max_str_len)
		{
		    if (temp_len > AD_1DTE_OUTLENGTH)
			temp[last_len] = EOS;
		    else
			temp[temp_len] = EOS;
		    break;
		}
		last_len = temp_len;
		need_space = TRUE;
	    }

	    if (nd->dn_status & AD_DN_TIMESPEC)
	    {

		hr = nd->dn_seconds;

		signmult = 1;

		if (hr < 0)
		{
		    signmult = -1;
		    hr = -hr;
		}

                ns = nd->dn_nsecond;
	        mn = hr % AD_39DTE_ISECPERHOUR;

		sc = hr % 60;
		hr /= 60;

		mn = hr % 60;
		hr /= 60;

		if (hr != 0  ||  (mn == 0  &&  sc == 0  &&  ns == 0))
		{
		    _VOID_ CVla((i4)(hr * signmult), tmpbuffer);
		    STpolycat((i4)4,
				temp,
				(need_space ? " " : ""),
				tmpbuffer,
				" hrs",
				temp);
		    if ((temp_len = STlength(temp)) > max_str_len)
		    {
			if (temp_len > max_str_len)
			    temp[last_len] = EOS;
			else
			    temp[temp_len] = EOS;
			break;
		    }
		    last_len = temp_len;
		    need_space = TRUE;
		}

		if (mn != 0)
		{
		    _VOID_ CVla((i4)(mn * signmult), tmpbuffer);
		    STpolycat((i4)4,
				temp,
				(need_space ? " " : ""),
				tmpbuffer,
				" mins",
				temp);
		    if ((temp_len = STlength(temp)) > max_str_len)
		    {
			if (temp_len > max_str_len)
			    temp[last_len] = EOS;
			else
			   temp[temp_len] = EOS;
			break;
		    }
		    last_len = temp_len;
		    need_space = TRUE;
		}

		if (sc != 0)
		{
		    _VOID_ CVla((i4)(sc * signmult), tmpbuffer);
		    STpolycat((i4)4,
				temp,
				(need_space ? " " : ""),
				tmpbuffer,
				" secs",
				temp);
		    if ((temp_len = STlength(temp)) > max_str_len)
		    {
			if (temp_len > max_str_len)
			    temp[last_len] = EOS;
			else
			    temp[temp_len] = EOS;
			break;
		    }
		    last_len = temp_len;
		    need_space = TRUE;
		}

		if (ns != 0 && (ns % AD_29DTE_NSPERMS) == 0)
		{
		    _VOID_ CVla((i4)(ns * signmult / AD_29DTE_NSPERMS), tmpbuffer);
		    STpolycat((i4)4,
				temp,
				(need_space ? " " : ""),
				tmpbuffer,
				" msec",
				temp);
		    if ((temp_len = STlength(temp)) > max_str_len)
		    {
			if (temp_len > max_str_len)
			    temp[last_len] = EOS;
			else
			    temp[temp_len] = EOS;
			break;
		    }
		    last_len = temp_len;
		    need_space = TRUE;
		}
		else if (ns != 0)
		{
		    _VOID_ CVla((i4)(ns * signmult), tmpbuffer);
		    STpolycat((i4)4,
				temp,
				(need_space ? " " : ""),
				tmpbuffer,
				" nsec",
				temp);
		    if ((temp_len = STlength(temp)) > max_str_len)
		    {
			if (temp_len > max_str_len)
			    temp[last_len] = EOS;
			else
			    temp[temp_len] = EOS;
			break;
		    }
		    last_len = temp_len;
		    need_space = TRUE;
		}
	      }
	    }

	} while (FALSE);
    }


    /* now we can pad out to datelength and return as result */
	switch (str_dv->db_datatype)
    {
	  case DB_NCHR_TYPE:
	  case DB_NVCHR_TYPE:
		if (str2_dv.db_datatype = DB_NVCHR_TYPE)
			str2_dv.db_length = (str_dv->db_length - DB_CNTSIZE)/sizeof(UCS2) + DB_CNTSIZE;
		else
			str2_dv.db_length = str_dv->db_length/sizeof(UCS2);
		str2_dv.db_data = (PTR)MEreqmem(0, str2_dv.db_length, TRUE, NULL);
		//str2_dv.db_data = &data;
		//MEfill(AD_1DTE_OUTLENGTH + 1, NULLCHAR, (PTR)&data);
		if (str_dv->db_datatype == DB_NCHR_TYPE)
			str2_dv.db_datatype = DB_CHA_TYPE;	
		else if (str_dv->db_datatype == DB_NVCHR_TYPE)
			str2_dv.db_datatype = DB_VCH_TYPE;
		adu_movestring(adf_scb, (u_char *) temp,(i4) STlength(temp), &str2_dv);
		db_stat = adu_nvchr_coerce(adf_scb,  &str2_dv, str_dv);
		MEfree((PTR)str2_dv.db_data);
		return db_stat;
		break;

	  default:
		return (adu_movestring(adf_scb, (u_char *) temp,
			  (i4) STlength(temp), str_dv));
	}
}



/*  
** Name: adu_dgmt() - Converts an Ingres absolute date into a char(25)
**		      in the form `yyyy_mm_dd hh:mm:ss GMT  '.
**
** Description:
**	  Input must be either an absolute date or an empty date.  If the input
**      is an absolute date, the output is a 25 character string representing
**      the date in Greenwich Mean Time (GMT), and in a format that when sorted
**      as a character string maintains the same ordering as the original date
**      format would if sorted.  If the input is the empty date, the output will
**      be a blank string. 
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      date_dv                         DB_DATA_VALUE describing date to be
**					converted.
**		.dn_status		A character whose bits describe the
**					internal date format.
**		.dn_highday		A character for large day intervals.
**		.dn_year		The date's year.
**		.dn_month		The date's month.
**		.dn_lowday		If the date is absolute, this is the
**					day of the month.  If the date is an
**					interval, this is the low-order part
**					of the number of days in the interval.
**		.dn_seconds		The time in seconds from 00:00.
**		.dn_nsecond		Fractional seconds in nanoseconds
**	gmt_dv				DB_DATE_VALUE describing string to
**					receive the output character string.
**					It is assumed this string will be long
**					enough (i.e. 25 characters).
**	    .db_length			It length.
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	gmt_dv
**	    .db_data			Resulting character string will be
**					placed at location pointed to by this.
**
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**	    E_AD5061_DGMT_ON_INTERVAL	The date_gmt() function does not work
**					on dates that are time intervals.
**
** History:
**	07-jun-87 (thurston)
**	    Initial creation.
**	13-aug-87 (thurston)
**	    Allowed the input to be the empty date ... return a blank string.
**	22-jan-88 (thurston)
**	    Modified to produce output as `1988_01_22' instead of `1988.01.22'
**	    since the previous method conflicted with the German date format.
**	30-Jan-2002 (inifa01) Bug 106972 INGSRV 1675/INGREP 148.
**	    When a date field which is a few milliseconds from midnight GMT is 
**	    passed in to date_gmt() the time returned is not normalized properly
**	    so that 24:00:00 is returned instead of 00:00:00.  
**	    Roundup d.dn_time to the nearest second before 'du_1normldate(&d)' is
**	    called otherwise we end up with hr = 86400. 
**	17-jul-2006 (gupsh01)
**	    Modified adu_dgmt to support new internal format 
**	    AD_NEWDTNTRNL for ANSI datetime types.
**	01-aug-2006 (gupsh01)
**	    The new AD_NEWDTNTRNL structure for ANSI date/time supports
**	    dn_seconds in seconds and fraction seconds part in dn_nseconds.
**	07-nov-2006 (gupsh01)
**	    Fixed seconds calculations.
*/

DB_STATUS
adu_dgmt(
ADF_CB			*adf_scb,
DB_DATA_VALUE		*date_dv,
DB_DATA_VALUE		*gmt_dv)
{
    AD_NEWDTNTRNL    d;
    char	    *s = (char *) gmt_dv->db_data;
    i4	    ltmp;
    i4	    hr;
    i4	    mn;
    i4	    sc;

    MEfill ((u_i2)sizeof(d), NULLCHAR, (PTR)&d);
    adu_6to_dtntrnl (date_dv, &d);

    if (d.dn_status == AD_DN_NULL)
    {
	/* Date is the empty date; return blank string */
	MEfill(25, ' ', (PTR) s);
	return (E_DB_OK);
    }

    if ((!(d.dn_status & AD_DN_ABSOLUTE)) || 
	  (d.dn_dttype == DB_INYM_TYPE) || 
	  (d.dn_dttype == DB_INDS_TYPE))
    {
	/* Date is not an absolute date; generate user error */
	return (adu_error(adf_scb, (i4) E_AD5061_DGMT_ON_INTERVAL, 0));
    }

    adu_1normldate(&d);	    /* Normalize the date */

    /* Format the year */
    ltmp = d.dn_year;
    _VOID_ CVla(ltmp, s);
    s += 4;
    *s++ = '_';

    /* Format the month */
    ltmp = d.dn_month;
    if (ltmp < 10)
    {
	*s++ = '0';
	_VOID_ CVla(ltmp, s);
	s++;
    }
    else
    {
	_VOID_ CVla(ltmp, s);
	s += 2;
    }
    *s++ = '_';

    /* Format the day */
    ltmp = d.dn_day;
    if (ltmp < 10)
    {
	*s++ = '0';
	_VOID_ CVla(ltmp, s);
	s++;
    }
    else
    {
	_VOID_ CVla(ltmp, s);
	s += 2;
    }
    *s++ = ' ';

    /* Now check for a time spec, if none assume 00:00:00 */
    hr = mn = sc = 0;
    if (d.dn_status & AD_DN_TIMESPEC)
    {
            hr = d.dn_seconds ; /* time in secs */

	    sc = hr % 60;
            hr /= 60;

            mn = hr % 60;
            hr /= 60;
    }

    /* Format the hour */
    if (hr < 10)
    {
	*s++ = '0';
	_VOID_ CVla(hr, s);
	s++;
    }
    else
    {
	_VOID_ CVla(hr, s);
	s += 2;
    }
    *s++ = ':';

    /* Format the minute */
    if (mn < 10)
    {
	*s++ = '0';
	_VOID_ CVla(mn, s);
	s++;
    }
    else
    {
	_VOID_ CVla(mn, s);
	s += 2;
    }
    *s++ = ':';

    /* Format the second */
    if (sc < 10)
    {
	*s++ = '0';
	_VOID_ CVla(sc, s);
	s++;
    }
    else
    {
	_VOID_ CVla(sc, s);
	s += 2;
    }
    *s++ = ' ';

    /* Finally, format `GMT  ' */
    *s++ = 'G';
    *s++ = 'M';
    *s++ = 'T';
    *s++ = ' ';
    *s = ' ';

    return (E_DB_OK);
}



/*  
** Name: adu_1dayofweek() - function to find the day of the week corresponding
**			    to the absolute date given.
**
** Description:
**	This is the internal implementation of the dow() SQL/QUEL function.
**  
**	Input must be an absolute date and output is a 3 character string
**	for the abbreviation of the day of week.  If an empty date is input,
**	an empty day of week is returned (ie- the 3 character string is all
**	blank).
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      date_dv                         DB_DATA_VALUE describing date to get
**					the day of week abbreviation for.
**		.dn_status		A character whose bits describe the
**					internal date format.
**		.dn_highday		A character for large day intervals.
**		.dn_year		The date's year.
**		.dn_month		The date's month.
**		.dn_lowday		If the date is absolute, this is the
**					day of the month.  If the date is an
**					interval, this is the low-order part
**					of the number of days in the interval.
**		.dn_seconds		The time in seconds from 00:00.
**		.dn_nsecond		Fractional seconds in nanoseconds
**	dow_dv				DB_DATE_VALUE describing string to
**					receive the day of week abbreviation.
**					It is assumed this string will be long
**					enough to hold abbreviation.  Further
**					it looks like it is assumed this will
**					be a character string.
**	    .db_length			It length.
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	    .db_data			Ptr to the resulting abbreviated dow.
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**
** History:
**	27-may-86 (ericj)
**	    Converted for Jupiter.
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	25-sep-89 (jrb)
**	    Fix bug #8155; dow function returns wrong results for some inputs.
**	    Problem is that date should be adjusted for time zone and normalized
**	    before computing function.
**      29-aug-92 (stevet)
**          Modified timezone adjustment method by calling TMtz_search
**          to calculate correct timezone offset.
**	17-jul-2006 (gupsh01)
**	    Modified adu_1dayofweek to support new internal format 
**	    AD_NEWDTNTRNL for ANSI datetime types.
*/

DB_STATUS
adu_1dayofweek(
ADF_CB              *adf_scb,
DB_DATA_VALUE	    *date_dv,
DB_DATA_VALUE	    *dow_dv)
{
    AD_NEWDTNTRNL	*nd;
    i4			daycode;
    char		temp[10];
    AD_NEWDTNTRNL	dateval;

    nd = &dateval;
    MEfill ((u_i2) sizeof (dateval), NULLCHAR, (PTR) &dateval);
    adu_6to_dtntrnl (date_dv, nd);

    if (((nd->dn_dttype == DB_DTE_TYPE) && (nd->dn_status & AD_DN_LENGTH))
	|| (nd->dn_dttype == DB_INYM_TYPE)
	|| (nd->dn_dttype == DB_TMWO_TYPE)
	|| (nd->dn_dttype == DB_TMW_TYPE)
	|| (nd->dn_dttype == DB_TME_TYPE)
	|| (nd->dn_dttype == DB_INDS_TYPE))
    {
        /* day of week function invalid on time interval */
        return (adu_error(adf_scb, (i4) E_AD505C_DOWINVALID, 0));
    }

#   ifdef xDEBUG
    if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_003_DATEFI_TRACE,
	    &dum1, &dum2))
    {
        TRdisplay("dayofweek: absolute date input: year=%d, month=%d,",
		  (i4)nd->dn_year, (i4)nd->dn_month);
	TRdisplay(" day=%d\n", (i4)nd->dn_day);
    }
#   endif

    if (nd->dn_status == AD_DN_NULL)
    {
        /* empty date */
        STcopy("   ", temp);
    }
    else
    {
	/* 
	** Move date parameter to local storage so it can be normalized 
	** for the time zone.
	*/

	if (nd->dn_status & AD_DN_TIMESPEC)
	{
	    /*
	    ** If TIMESPEC then set to local time and normalize.
	    */
	    if ((nd->dn_dttype != DB_TMWO_TYPE) && 
	        (nd->dn_dttype != DB_TMW_TYPE) && 
	        (nd->dn_dttype != DB_TSWO_TYPE) && 
	        (nd->dn_dttype != DB_TSW_TYPE))
	    {
	        nd->dn_seconds += TMtz_search(adf_scb->adf_tzcb, 
			     TM_TIMETYPE_GMT, 
			     adu_5time_of_date(nd));
	        adu_1normldate(nd);
	    }
	}

        daycode = adu_3day_of_date(nd) % AD_20DTE_WEEKLENGTH;

#       ifdef xDEBUG
	if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_003_DATEFI_TRACE,
		&dum1, &dum2))
            TRdisplay("dayofweek: daycode (where 0=Thu) =%d\n", daycode);
#       endif

        /* get day name from Adu_weekdays structure using daycode */
        STcopy(Adu_weekdays[daycode].day_name, temp);
    }

    /* move result to output */
    STmove(temp, ' ', (i2)dow_dv->db_length, (char *)dow_dv->db_data);
    
    return (E_DB_OK);
}


/*
** Name: add_date() -	add two dates.
**
** Description:
**    Add_date supports the addition of two time intervals or an interval
**  and an absolute time. The addition of two absolute times is an error.
**  Note that the addition of two intervals results in an interval, all other
**  additions result in an absolute time.
**  As well, if one of the dates is empty, the result is the empty date.
**
** Parameters:
**      a   -- first date
**      b   -- second date
**
** Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**      a = a + b
**
** Exceptions:
**	  This routine can detect several kinds of errors.  When this occurs
**	the routine adu_error(adf_scb, ...) is called to process the error.
**	It will signal an exception that will not be returned from.
**
**      28-oct-91 (stevet)
**          Fixed problem with month_interval + absolute_date, this problem
**          occurred when the resulting date is a new year. (bug 40558)
**      29-aug-92 (stevet)
**          Modified timezone adjustment method by calling TMtz_search
**          to calculate correct timezone offset.
**      09-dec-1992 (stevet)
**	    Fixed a problem where tmp_a is passed to adu_5time_of_date
**          instead of &tmp_a.
**      07-jun-1993 (stevet)
**          Fixed problem with '- month_interval' + absolute_date, this
**          problem occurred when the resulting date is the previous
**          year. (B48678)  Also fixed 'month_interval' + absolute_date
**          problem where incorrect date can return. (B49376)
**	17-feb-97 (inkdo01)
**	    Fixed problem adding interval to absolute with time portion.
**	    Seems to happen in Eastern Hemisphere (i.e., local time > GMT).
**	23-jun-98 (kitch01)
**		Bug 91559. The above fix will convert either/both input dates to local
**		time and then normalize them. Both dates are converted back to GMT but
**		only the first date is re-normalized. This causes a problem when
**		we reuse the second date in a subsequent calculation. The fix is to 
**		re-normalize this second date.
**      23-apr-2001 (horda03)
**              Don't set EPOCH states in the date variables, keep them local. Thus
**              dates will be compatible across Ingres versions.
**	17-jul-2006 (gupsh01)
**	    Modified to support new internal format AD_NEWDTNTRNL for 
**	    ANSI datetime types.
**	01-aug-2006 (gupsh01)
**	    Added nanosecond precision support in calculations.
**	10-dec-2006 (gupsh01)
**	    For time types the range checking for valid year/month etc
**	    is not needed.
**	08-jan-2007 (gupsh01)
**	    Added function instances for Adding ANSI date/time tyeps.
*/

static DB_STATUS
add_date(
ADF_CB         *adf_scb,
DB_DATA_VALUE   *rdv,
AD_NEWDTNTRNL   *a,
AD_NEWDTNTRNL   *b)
{
    i4 	    n1,
	    n2;
    i4	    adays,
	    bdays;
    i4	    tzadjust = 0;
    i4 old_monthval,
    	    old_yearval;
    AD_NEWDTNTRNL tmp_a;
    bool	tza = ((a->dn_status & AD_DN_ABSOLUTE) && 
	(a->dn_status & AD_DN_TIMESPEC) && (b->dn_status & AD_DN_LENGTH));
    bool	tzb = ((b->dn_status & AD_DN_ABSOLUTE) && 
	(b->dn_status & AD_DN_TIMESPEC) && (a->dn_status & AD_DN_LENGTH));

    bool a_after_epoch  = FALSE;
    bool b_after_epoch  = FALSE;

    MEcopy( (PTR)a, (u_i2)(sizeof(tmp_a)), (PTR)&tmp_a); 
    if ((a->dn_status & AD_DN_ABSOLUTE)  && (b->dn_status & AD_DN_ABSOLUTE))
    {
        /* user error */
        return (adu_error(adf_scb, (i4) E_AD5050_DATEADD, 0));
    }

    if (((abs(a->dn_dttype) == DB_INDS_TYPE) && (a->dn_day != 0)) &&
	((abs(b->dn_dttype) == DB_TMWO_TYPE) || 
	 (abs(b->dn_dttype) == DB_TMW_TYPE) ||
         (abs(b->dn_dttype) == DB_TME_TYPE)
        ) ||
        ((abs(b->dn_dttype) == DB_INDS_TYPE) && (b->dn_day != 0)) &&
	((abs(a->dn_dttype) == DB_TMWO_TYPE) || 
	 (abs(a->dn_dttype) == DB_TMW_TYPE) ||
         (abs(a->dn_dttype) == DB_TME_TYPE)
	) 
       )
        return (adu_error(adf_scb, (i4) E_AD5078_ANSITIME_INTVLADD, 0));

    if ((a->dn_status == AD_DN_NULL) || (b->dn_status == AD_DN_NULL))
    {
        /* result is the empty date */

#       ifdef xDEBUG
	if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_005_NULLDATE,
		&dum1, &dum2))
            TRdisplay("add_date: empty result\n");
#       endif

        a->dn_year	= 0;
        a->dn_month	= 0;
        a->dn_day       = 0;
        a->dn_seconds	= 0;
        a->dn_nsecond   = 0;
        a->dn_status    = AD_DN_NULL;
    }
    else
    {
        /* add two times */

	/* When adding to date with time portion, just convert it to local
	** time first. This avoids a variety of anomalies concerning month-end
	** determination. */
	if (tza)
	{
	    if ((a->dn_dttype != DB_TMWO_TYPE) && 
	        (a->dn_dttype != DB_TMW_TYPE) && 
	        (a->dn_dttype != DB_TSWO_TYPE) && 
	        (a->dn_dttype != DB_TSW_TYPE))
	    {
	        tzadjust = TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL,
		    adu_5time_of_date(&tmp_a));
	        a->dn_seconds += tzadjust;
	        adu_1normldate(a);
	    }
	}
	else if (tzb)
	{
	    if ((b->dn_dttype != DB_TMWO_TYPE) && 
	        (b->dn_dttype != DB_TMW_TYPE) && 
	        (b->dn_dttype != DB_TSWO_TYPE) && 
	        (b->dn_dttype != DB_TSW_TYPE))
	    {
	        tzadjust = TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL,
		    adu_5time_of_date(b));
	        b->dn_seconds += tzadjust;
	        adu_1normldate(b);
	    }
	}

        old_monthval = n1 = a->dn_month;    /* remember whether a originally
					    ** had a month specified
					    */
        n2 = b->dn_month;
        old_yearval = a->dn_year;	    /* remember whether a originally
					    ** had a year specified
					    */
	if (a->dn_status & AD_DN_ABSOLUTE) 
	{
	    if (ADDT_GT_EPOCH(a))
		     a_after_epoch = TRUE;
	    else
	    {
		    if (!(ADDT_LT_EPOCH(a)))
		    {
                       a->dn_day = 02;
		    }
	    }
	}
	if (b->dn_status & AD_DN_ABSOLUTE) 
	{
	    if (ADDT_GT_EPOCH(b))
		b_after_epoch = TRUE;
	    else
	    {
		if (!(ADDT_LT_EPOCH(b)))
		{
                    b->dn_day = 02;
		}
	    }
	}

        adays = (I1_CHECK_MACRO( a->dn_day));
        bdays = (I1_CHECK_MACRO( b->dn_day));


        a->dn_year  += b->dn_year;
        a->dn_month += b->dn_month;

        if ((a->dn_status & AD_DN_ABSOLUTE) && (b->dn_year || b->dn_month))
        {
            i4  t_days;

            /*
            ** This takes care of the case where you add 1 month to
            ** Jan 31.  You should get Feb 28, not Feb 31.
	    ** Also, interval can be negative value so we need to normalize
	    ** year/month for negative interval as well. (B48678)
            */

            while (n2 >= 12)
            {
                a->dn_year++;
                n2      -= 12;
                a->dn_month -= 12;
            }

	    while (n2 <= 0)
	    {
		a->dn_year--;
		n2      += 12;
		a->dn_month += 12;
	    }

            while (a->dn_month > 12)
            {
                a->dn_year++;
                a->dn_month -= 12;
            }

	    while (a->dn_month <= 0)
	    {
		a->dn_year--;
		a->dn_month += 12;
	    }

            t_days = adu_2monthsize((i4) a->dn_month, (i4)a->dn_year);

            if (t_days < adays)
                adays = t_days;
        }

        /* adjust month iff we have added months */

        if ((b->dn_status & AD_DN_ABSOLUTE) &&
            (old_yearval || old_monthval))
        {
            i4  t_days;

            /*
            ** This takes care of the case where you add 1 month
            ** to Jan 31.  You should get Feb 28, not Feb 31.
	    ** Also, interval can be negative value so we need to normalize
	    ** year/month for negative interval as well. (B48678)
            */

            while (n1 >= 12)
            {
                a->dn_year++;
                n1      -= 12;
                a->dn_month -= 12;
            }

	    while (n1 <= 0)
	    {
		a->dn_year--;
		n1      += 12;
		a->dn_month += 12;
	    }

	    /*
	    ** Make sure 'a' is a valid date before calling adu_2monthsize. 
	    ** (B40558)
	    */
	    while (a->dn_month > 12)
	    {
                a->dn_year++;
                a->dn_month -= 12;
            }

	    while (a->dn_month <= 0)
	    {
		a->dn_year--;
		a->dn_month += 12;
	    }

            t_days = adu_2monthsize((i4)a->dn_month, (i4)a->dn_year);

            if (t_days < bdays)
                bdays = t_days;
        }

        a->dn_seconds  += b->dn_seconds;
        a->dn_nsecond  += b->dn_nsecond;

        adays       += bdays;

        a->dn_day    = adays;

	/* Reset to GMT - it gets normalized later. Note, b may be ref'd
	** elsewhere, too (call by ref!!). So we must back out the adjust
	** from it, even though a is the result. */
	if (tza || tzb) a->dn_seconds -= tzadjust;
 	/* Bug 91559. Re-normalize b here as it does not get done later*/
 	if (tzb)
 	{
 		b->dn_seconds -= tzadjust;
 		adu_1normldate(b);
 	}

        if (a->dn_status & AD_DN_ABSOLUTE || b->dn_status & AD_DN_ABSOLUTE)
        {

            if (a->dn_status & AD_DN_ABSOLUTE)
            {
	        if (((a->dn_dttype != DB_TMWO_TYPE) && 
	             (a->dn_dttype != DB_TMW_TYPE) && 
	             (a->dn_dttype != DB_TSWO_TYPE) && 
	             (a->dn_dttype != DB_TSW_TYPE)
		     ) &&
                     (! (a->dn_status & AD_DN_TIMESPEC)
		       && (b->dn_status & AD_DN_TIMESPEC)))
                {
		    a->dn_seconds -= 
			TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL, 
			     adu_5time_of_date(&tmp_a));
                }
            }
            else
            {
	        if (((a->dn_dttype != DB_TMWO_TYPE) && 
	             (a->dn_dttype != DB_TMW_TYPE) && 
	             (a->dn_dttype != DB_TSWO_TYPE) && 
	             (a->dn_dttype != DB_TSW_TYPE)
		     ) &&
                     (!(b->dn_status & AD_DN_TIMESPEC) &&
		      (a->dn_status & AD_DN_TIMESPEC)))
                {
		    a->dn_seconds -= 
			TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL, 
			     adu_5time_of_date(b));
                }
            }

            /* absolute time, normalize representation */

            adu_1normldate(a);

	    if ( !((a_after_epoch)  &&
		   (a->dn_status & AD_DN_ABSOLUTE) &&
		   (ADDT_GT_EPOCH(a))))
	    {
		if ( !(a_after_epoch)  &&
		      (a->dn_status & AD_DN_ABSOLUTE) &&
		      (ADDT_GTE_EPOCH(a)))
		{
		    a->dn_day += 11;
		    adu_1normldate(a);
		}
		else
		if ( !(b_after_epoch) &&
		      (b->dn_status & AD_DN_ABSOLUTE) &&
		      (ADDT_GTE_EPOCH(a)))
		{
		    a->dn_day += 11;
		    adu_1normldate(a);
		}
		else
		if ( (a_after_epoch) &&
		     (a->dn_status & AD_DN_ABSOLUTE) &&
		     (ADDT_LTE_EPOCH(a)))
		{
		    adays 	    = a->dn_day - 11;
		    a->dn_day    = adays;
		    adu_1normldate(a);
		}
		else
		if ( (b_after_epoch) &&
		     (b->dn_status & AD_DN_ABSOLUTE) &&
		     (ADDT_LTE_EPOCH(a)))
		{
		    adays 	    = a->dn_day - 11;
		    a->dn_day    = adays;
		    adu_1normldate(a);
		}
	    }

            /* check for date out of range */
	    /* For time types year is not present */
            if ((a->dn_year > AD_24DTE_MAX_ABS_YR || 
		 a->dn_year < AD_23DTE_MIN_ABS_YR)
		&& (a->dn_dttype)
		&& ((a->dn_dttype != DB_TME_TYPE) && 
		    (a->dn_dttype != DB_TMW_TYPE) && 
		    (a->dn_dttype != DB_TMWO_TYPE)))
                return (adu_error(adf_scb, (i4) E_AD505B_DATEYR, 0));

            /*
            ** Be sure that the result has all components of
            **  both addends.
            **  Fix of bug #1828 - Jeff Lichtman
            */
            a->dn_status |= b->dn_status &
                    (AD_DN_YEARSPEC | AD_DN_MONTHSPEC |
                     AD_DN_DAYSPEC | AD_DN_TIMESPEC);

            a->dn_status &= ~AD_DN_LENGTH;
            a->dn_status |= AD_DN_ABSOLUTE;
        }
        else
        {
            /* added two intervals */

            /*
            ** Be sure that the result has all components
            ** of both addends.
            **  Fix of bug #1828 - Jeff Lichtman
            */
            a->dn_status |= b->dn_status;

	    adu_2normldint(a);

            /* check if exceeded max. interval allowed */

            if (a->dn_year > MAX_INV_YR || a->dn_year < MIN_INV_YR)
            {
                return (adu_error(adf_scb, (i4) E_AD505A_DATEYROVFLO, 2,
				  (i4)sizeof(a->dn_year),
				  (i4 *) &a->dn_year));
            }
        }
    }

#   ifdef xDEBUG
    print_date(a);
#   endif
    adu_7from_dtntrnl (rdv, a);

    return (E_DB_OK);
}


/*
** Name: ad0_5sub_date() - subtract two dates.
**
** Description:
**	  ad0_5sub_date() supports subtracting two absolute dates, two
**	intervals or an interval from an absolute date.
**	The results are the obvious ones:
**	    interval = absolute - absolute
**	    interval = interval - interval
**	    absolute = absolute - interval
**
**	If one of the dates is the empty date, the result is empty.
**
** Parameters:
**      a   -- first date
**      b   -- second date
**
** Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**      a = a - b
**
** Exceptions:
**	  This routine can detect several kinds of errors.  When this occurs
**	the routine adu_error(adf_scb, ...) is called to process the error.
**	It will signal an exception that will not be returned from.
**
**	05-feb-91 (jrb)
**	    Bug 34574.  When doing subtraction with dates where one value had
**	    a time specification and the other didn't, we were wrongly applying
**	    the timezone shift in some cases (applying it in the wrong
**	    direction).
**      29-aug-92 (stevet)
**          Modified timezone adjustment method by calling TMtz_search
**          to calculate correct timezone offset.
**      09-dec-1992 (stevet)
**	    Fixed a problem where tmp_a is passed to adu_5time_of_date
**          instead of &tmp_a.
**	17-feb-97 (inkdo01)
**	    Fixed problem subbing interval from absolute with time portion.
**	    Seems to happen in Eastern Hemisphere (i.e., local time > GMT).
**	20-apr-1998 (kitch01)
**		Bug 90569. When checking for no. of days in month use the year
**		that we will actualy be in after the subtraction rather than the
**		year of the input date. Bug occurred because result month was
**		february and input date was a leap year. eg. 29/01/00 - 11 months.
**       5-Dec-2002 (hanal04) Bug 109218 INGSRV 2021
**          Extend the fix for bug 90569 in order to handle the
**          subtraction of a -ve time interval (we are actually adding
**          time).
**	17-jul-2006 (gupsh01)
**	    Modified to support new internal format AD_NEWDTNTRNL for 
**	    ANSI datetime types.
**	01-aug-2006 (gupsh01)
**	    Added nanosecond precision support in calculations.
**	10-dec-2006 (gupsh01)
**	    For time types the range checking for valid year/month etc
**	    is not needed.
*/

static	DB_STATUS
ad0_5sub_date(
ADF_CB         *adf_scb,
DB_DATA_VALUE	*rdv,
AD_NEWDTNTRNL   *a,
AD_NEWDTNTRNL   *b)
{
    i4		astat,
		bstat,
		adays,
		bdays,
		n2,
		msm,
		msy;
    AD_NEWDTNTRNL tmp_a;
    i4		tzadjust = 0;
    bool	tza = ((a->dn_status & AD_DN_ABSOLUTE) && 
	(a->dn_status & AD_DN_TIMESPEC) && (b->dn_status & AD_DN_LENGTH));
    bool        a_after_epoch  = FALSE;
    bool        b_after_epoch  = FALSE;
    
    MEcopy( (PTR)a, sizeof(tmp_a), (PTR)&tmp_a); 

    astat       = a->dn_status;
    bstat       = b->dn_status;

    /* check for legal combinations */

    if (astat & AD_DN_LENGTH && bstat & AD_DN_ABSOLUTE)
    {
        /* user error */
        return (adu_error(adf_scb, (i4) E_AD5051_DATESUB, 0));
    }

    if ((astat == AD_DN_NULL) || (bstat == AD_DN_NULL))
    {
        /* result is the empty date */

        a->dn_status  = AD_DN_NULL;
        a->dn_year  = 0;
        a->dn_month = 0;
        a->dn_day    = 0;
        a->dn_seconds  = 0;
        a->dn_nsecond  = 0;
        a->dn_tzhour  = 0;
        a->dn_tzminute  = 0;
    }
    else
    {
        /* subtract two dates */

        /*
        ** If both dates are absolute dates, we wish to end up with an interval
        ** in days. This is a special case we handle here.
        */
	if  ((astat & AD_DN_ABSOLUTE)  &&  (bstat & AD_DN_ABSOLUTE))
        {
            adays       = adu_3day_of_date(a) - adu_3day_of_date(b);
            a->dn_year  = 0;
            a->dn_month = 0;
            a->dn_day    = adays;

            /* subtract time portion (if any) */

            a->dn_seconds  -= b->dn_seconds;
            a->dn_nsecond  -= b->dn_nsecond;

            /*
            ** If only one element included a time, we must compensate.
            ** Assume the other date is midnight current time.
            */

            if ((astat & AD_DN_TIMESPEC)  &&  !(bstat & AD_DN_TIMESPEC))
	    {
	        if ((a->dn_dttype != DB_TMWO_TYPE) && 
	            (a->dn_dttype != DB_TMW_TYPE) && 
	            (a->dn_dttype != DB_TSWO_TYPE) && 
	            (a->dn_dttype != DB_TSW_TYPE))
		{
		  a->dn_seconds += 
		    TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL, 
				 adu_5time_of_date(b));
		}
	    }

            if (!(astat & AD_DN_TIMESPEC)  &&  (bstat & AD_DN_TIMESPEC))
	    {
	        if ((a->dn_dttype != DB_TMWO_TYPE) && 
	            (a->dn_dttype != DB_TMW_TYPE) && 
	            (a->dn_dttype != DB_TSWO_TYPE) && 
	            (a->dn_dttype != DB_TSW_TYPE))
		{
		    a->dn_seconds -= 
		      TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL, 
				 adu_5time_of_date(&tmp_a));
		}
	    }

            a->dn_status  = (astat & AD_DN_TIMESPEC) | (bstat & AD_DN_TIMESPEC);
            a->dn_status |= (AD_DN_LENGTH | AD_DN_DAYSPEC);

            /* Finally normalize time and days */

            adu_2normldint(a);
	    adu_7from_dtntrnl (rdv, a);
#   ifdef xDEBUG
    	    print_date(a);
#   endif
            return (E_DB_OK);
        }

	/* Convert to local time before arithmetic to save month end 
	** anomalies caused by stupid internal date format. */
	if (tza)
	{
	   if ((a->dn_dttype != DB_TMWO_TYPE) && 
	       (a->dn_dttype != DB_TMW_TYPE) && 
	       (a->dn_dttype != DB_TSWO_TYPE) && 
	       (a->dn_dttype != DB_TSW_TYPE))
	    {
	         tzadjust = TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL,
		     adu_5time_of_date(&tmp_a));
	         a->dn_seconds += tzadjust;
	         adu_1normldate(a);
	    }
	}

        n2 = b->dn_month;

	if ((a->dn_status & AD_DN_ABSOLUTE) || 
	    ((a->dn_dttype != DB_INYM_TYPE) &&
	     (a->dn_dttype != DB_INDS_TYPE))) 
	{
	    if (ADDT_GT_EPOCH(a))
		a_after_epoch = TRUE;
	    else
	    {
		if (!(ADDT_LT_EPOCH(a)))
		{
                    a->dn_day = 02;
		}
	    }
	}
	if ((b->dn_status & AD_DN_ABSOLUTE) ||
	    ((b->dn_dttype != DB_INYM_TYPE) &&
	     (b->dn_dttype != DB_INDS_TYPE))) 
	{
	    if (ADDT_GT_EPOCH(b))
		b_after_epoch = TRUE;
	    else
	    {
		if (!(ADDT_LT_EPOCH(b)))
		{
                    b->dn_day = 02;
		}
	    }
	}

        adays = (I1_CHECK_MACRO(a->dn_day));
        bdays = (I1_CHECK_MACRO(b->dn_day)); 

        a->dn_year  -= b->dn_year;
        a->dn_month -= b->dn_month;

        if ((a->dn_status & AD_DN_ABSOLUTE)    &&
            (b->dn_status & AD_DN_LENGTH)      &&
            (b->dn_year || b->dn_month))
        {
            i4  t_days;

            /*
            ** This takes care of the case where you subtract 1
            ** month from Mar. 31.
            ** You should get Feb 28, not Feb 31.
            */

            while (n2 >= 12)
            {
                a->dn_year--;
                n2      -= 12;
                a->dn_month += 12;
            }

	    /* Bug 90569. As we are subtracting dates check month size for
	    ** the year will actually be in after the subtraction.
	    */
	    if((i4)a->dn_month < 1)
	    {
                msm = 12 + (i4)a->dn_month;
		msy = (i4)a->dn_year - 1; 
	    }
	    else if ((i4)a->dn_month > 12)
	    {
		/* Subtracting a negative interval can lead to a
		** month which is greater than 12.
		*/
                msm = (i4)a->dn_month - 12;
		msy = (i4)a->dn_year + 1;
	    }
	    else
	    {
                msm = (i4)a->dn_month;
		msy = (i4)a->dn_year;
	    }
            t_days = adu_2monthsize(msm, msy);

            if (t_days < adays)
                adays = t_days;
        }

        a->dn_seconds  -= b->dn_seconds;
        a->dn_nsecond  -= b->dn_nsecond;
        adays       -= bdays;
        a->dn_day    = adays;

	if (tza) a->dn_seconds -= tzadjust;
				/* convert back to GMT */

        if (a->dn_status & AD_DN_ABSOLUTE && b->dn_status & AD_DN_LENGTH)
        {
            if (!(a->dn_status & AD_DN_TIMESPEC) &&
                 (b->dn_status & AD_DN_TIMESPEC))
            {
	   	if ((a->dn_dttype != DB_TMWO_TYPE) && 
	           (a->dn_dttype != DB_TMW_TYPE) && 
	           (a->dn_dttype != DB_TSWO_TYPE) && 
	           (a->dn_dttype != DB_TSW_TYPE))
		{
		    a->dn_seconds -= 
			TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL, 
			     adu_5time_of_date(&tmp_a));
		}
            }

            /*
            ** Make sure that the result has all components
            ** of both subtrahend and minuhend.
            **  Fix of bug #1828 - Jeff Lichtman
            */
            a->dn_status |= b->dn_status &
                    (AD_DN_YEARSPEC | AD_DN_MONTHSPEC |
                     AD_DN_DAYSPEC | AD_DN_TIMESPEC);

            adu_1normldate(a);

	    if ( !((a_after_epoch)  &&
		   (a->dn_status & AD_DN_ABSOLUTE) &&
		   (ADDT_GT_EPOCH(a))))
	    {
		if ( !(a_after_epoch)  &&
		     (a->dn_status & AD_DN_ABSOLUTE) &&
		     (ADDT_GTE_EPOCH(a)))
		{
		    a->dn_day += 11;
		    adu_1normldate(a);
		}
		else
		if ( !(b_after_epoch) &&
		      (b->dn_status & AD_DN_ABSOLUTE) &&
		      (ADDT_GTE_EPOCH(a)))
		{
		    a->dn_day += 11;
		    adu_1normldate(a);
		}
		else
		if ( (a_after_epoch) &&
		     (a->dn_status & AD_DN_ABSOLUTE) &&
		     (ADDT_LTE_EPOCH(a)))
		{
		    adays 		= a->dn_day - 11;
		    a->dn_day   = adays;
		    adu_1normldate(a);
		}
		else
		if ( (b_after_epoch) &&
		     (b->dn_status & AD_DN_ABSOLUTE) &&
		     (ADDT_LTE_EPOCH(a)))
		{
		    adays 		= a->dn_day - 11;
		    a->dn_day   = adays;
		    adu_1normldate(a);
		}
	    }

            /* check for date out of range */
	    /* For time types year is not present */
            if ((a->dn_year > AD_24DTE_MAX_ABS_YR || 
		 a->dn_year < AD_23DTE_MIN_ABS_YR)
		 && ((a->dn_dttype != DB_TME_TYPE) && 
		    (a->dn_dttype != DB_TMW_TYPE) && 
		    (a->dn_dttype != DB_TMWO_TYPE)))
                return (adu_error(adf_scb, (i4) E_AD505B_DATEYR, 0));
        }
        else
        {

            /*
            ** Make sure that result has all components of
            ** both subtrahend and minuhend.
            **  Fix of bug #1828 - Jeff Lichtman
            */
            a->dn_status |= b->dn_status &
                    (AD_DN_YEARSPEC | AD_DN_MONTHSPEC |
                     AD_DN_DAYSPEC | AD_DN_TIMESPEC);

            a->dn_status &= ~AD_DN_ABSOLUTE;
            a->dn_status |=  AD_DN_LENGTH;

            adu_2normldint(a);

            /* check if exceeded max. interval allowed */

            if (a->dn_year > MAX_INV_YR || a->dn_year < MIN_INV_YR)
            {
                return (adu_error(adf_scb, (i4) E_AD505A_DATEYROVFLO, 2,
				  (i4)sizeof(a->dn_year),
				  (i4 *) &a->dn_year));
            }
        }
    }

#   ifdef xDEBUG
    print_date(a);
#   endif
    adu_7from_dtntrnl (rdv, a);

    return (E_DB_OK);
}


/*
** Name: adu_1date_addu() -	ADF interface to add dates.
**
** Description:
**	  This routine implements the function instance for adding two dates
**	together.
**	  Add_date supports the addition of two time intervals or an interval
**	and an absolute time. The addition of two absolute times is an error.
**	Note that the addition of two intervals results in an interval, all
**	other additions result in an absolute time.
**	As well, if one of the dates is empty, the result is the empty date.
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      dv1                             DB_DATA_VALUE describing the first date
**					operand.
**	    .db_data			Ptr to AD_NEWDTNTRNL struct.
**		.dn_status		A character whose bits describe the
**					internal date format.
**		.dn_highday		A character for large day intervals.
**		.dn_year		The date's year.
**		.dn_month		The date's month.
**		.dn_lowday		If the date is absolute, this is the
**					day of the month.  If the date is an
**					interval, this is the low-order part
**					of the number of days in the interval.
**		.dn_seconds		The time in seconds from 00:00.
**		.dn_nsecond		Fractional seconds in nanoseconds
**	dv2				DB_DATA_VALUE describing the second
**					date operand.  The fields are the same
**					as listed for dv1
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	rdv				DB_DATA_VALUE describing the result
**					date operand.  The fields are the same
**					as listed for dv1.
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**
**	Exceptions:
**	      There are several types of error conditions that can be detected
**	    in add_date().  If one of these errors occurs, an exception will
**	    will be generated from which this routine will not return.
**
** History:
**	27-may-86 (ericj)
**	    Converted for Jupiter.
**
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	17-jul-2006 (gupsh01)
**	    Modified to support new internal format
**	    AD_NEWDTNTRNL for ANSI datetime types.
**	10-dec-2006 (gupsh01)
**	    Added error checks for invalid combinations
**	    of inputs.
**	21-dec-2006 (gupsh01)
**	    Fixed error checking when adding Ingresdate.
**	08-jan-2007 (gupsh01)
**	    Fixed addition of various date/time types.
*/

DB_STATUS
adu_1date_addu(
ADF_CB         *adf_scb,
DB_DATA_VALUE  *dv1,
DB_DATA_VALUE  *dv2,
DB_DATA_VALUE  *rdv)
{

    AD_NEWDTNTRNL       ndatev1;
    AD_NEWDTNTRNL       ndatev2;
    register AD_NEWDTNTRNL  *datep1 = &ndatev1;
    register AD_NEWDTNTRNL  *datep2 = &ndatev2;
    DB_DT_ID id1, id2;

    MEfill ((u_i2) sizeof (ndatev1), NULLCHAR, (PTR) &ndatev1);
    MEfill ((u_i2) sizeof (ndatev2), NULLCHAR, (PTR) &ndatev2);
    adu_6to_dtntrnl (dv1, datep1);
    adu_6to_dtntrnl (dv2, datep2);

    /* At least one of the operands must be an interval or a 
    ** date type. Sort operands to reduce cases.
    */
    if (dv1->db_datatype > dv2->db_datatype)
    {
	id1 = abs(dv2->db_datatype);
	id2 = abs(dv1->db_datatype);
    }
    else
    {
	id1 = abs(dv1->db_datatype);
	id2 = abs(dv2->db_datatype);
    }

    if ((id2 != DB_INYM_TYPE) && (id2 != DB_INDS_TYPE) && 
	(id2 != DB_DTE_TYPE)
       )
         return (adu_error(adf_scb, (i4) E_AD5069_ANSIDATE_DATEADD, 0));

    if ( (id2 == DB_INYM_TYPE) && 
	      ((id1 == DB_TMWO_TYPE) || (id1 == DB_TMW_TYPE) || 
	       (id1 == DB_TME_TYPE))
       )
         return (adu_error(adf_scb, (i4) E_AD5078_ANSITIME_INTVLADD, 0));

    if ((id1 == DB_INYM_TYPE) && (id2 == DB_INDS_TYPE))
         return (adu_error(adf_scb, (i4) E_AD5069_ANSIDATE_DATEADD, 0));

    MEcopy((PTR) dv1->db_data, (u_i2) dv1->db_length, (PTR) rdv->db_data);
    return (add_date(adf_scb, rdv, datep1, datep2));
}


/*
** Name: adu_7date_subu() - ADF interface to subtract dates.
**
** Description:
**	  adu_7date_subu() supports subtracting two absolute dates, two
**	intervals or an interval from an absolute date.
**	The results are the obvious ones:
**	    interval = absolute - absolute
**	    interval = interval - interval
**	    absolute = absolute - interval
**
**	If one of the dates is the empty date, the result is empty.
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      dv1                             DB_DATA_VALUE describing the first date
**					operand.
**	    .db_data			Ptr to AD_NEWDTNTRNL struct.
**		.dn_status		A character whose bits describe the
**					internal date format.
**		.dn_highday		A character for large day intervals.
**		.dn_year		The date's year.
**		.dn_month		The date's month.
**		.dn_lowday		If the date is absolute, this is the
**					day of the month.  If the date is an
**					interval, this is the low-order part
**					of the number of days in the interval.
**		.dn_seconds		The time in seconds from 00:00.
**		.dn_nsecond		Fractional seconds in nanoseconds
**	dv2				DB_DATA_VALUE describing the second
**					date operand.  The fields are the same
**					as listed for dv1
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	rdv				DB_DATA_VALUE describing the result
**					date operand.  The fields are the same
**					as listed for dv1.
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**
**	Exceptions:
**	      There are several types of error conditions that can be detected
**	    in ad0_5sub_date().  If one of these errors occurs, an exception
**	    will be generated from which this routine will not return.
**
** History:
**	27-may-86 (ericj)
**	    Converted for Jupiter.
**
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	17-jul-2006 (gupsh01)
**	    Modified to support new internal format
**	    AD_NEWDTNTRNL for ANSI datetime types.
**	10-dec-2006 (gupsh01)
**	    Fixed to avoid mix mode operations.
*/

DB_STATUS
adu_7date_subu(
ADF_CB         *adf_scb,
DB_DATA_VALUE  *dv1,
DB_DATA_VALUE  *dv2,
DB_DATA_VALUE  *rdv)
{
    AD_NEWDTNTRNL       ndatev1;
    AD_NEWDTNTRNL       ndatev2;
    register AD_NEWDTNTRNL  *datep1 = &ndatev1;
    register AD_NEWDTNTRNL  *datep2 = &ndatev2;

    MEfill ((u_i2) sizeof (ndatev1), NULLCHAR, (PTR) &ndatev1);
    MEfill ((u_i2) sizeof (ndatev2), NULLCHAR, (PTR) &ndatev2);
    adu_6to_dtntrnl (dv1, datep1);
    adu_6to_dtntrnl (dv2, datep2);

    if (abs(dv1->db_datatype) != abs(dv2->db_datatype))
    {
    	if ((abs(dv1->db_datatype) == DB_INYM_TYPE) ||
	    (abs(dv1->db_datatype) == DB_INDS_TYPE))
	  /* Only Like interval types can be subtracted */
          return (adu_error(adf_scb, (i4) E_AD5068_ANSIDATE_DATESUB, 0));
	else if ((abs(dv2->db_datatype) != DB_INYM_TYPE) && 
    	         (abs(dv2->db_datatype) != DB_INDS_TYPE))
	  /* Second operand can only be an interval types */
          return (adu_error(adf_scb, (i4) E_AD5068_ANSIDATE_DATESUB, 0));

    }
    return (ad0_5sub_date(adf_scb, rdv, datep1, datep2));
}


/*
** Name: adu_date_mul() - ADF interface to multiply intervals
** 
** Description:
**	  adu_date_mul() supports  multiplying an interval with a float
**	The results are the obvious ones:
**	    interval = interval * float
**	    interval = float * interval
**
**	If one of the dates is the empty date, the result is empty.
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      dv1                             DB_DATA_VALUE describing the first date
**					operand.
**	    .db_data			Ptr to AD_NEWDTNTRNL struct.
**		.dn_status		A character whose bits describe the
**					internal date format.
**		.dn_highday		A character for large day intervals.
**		.dn_year		The date's year.
**		.dn_month		The date's month.
**		.dn_lowday		If the date is absolute, this is the
**					day of the month.  If the date is an
**					interval, this is the low-order part
**					of the number of days in the interval.
**		.dn_seconds		The time in seconds from 00:00.
**		.dn_nsecond		Fractional seconds in nanoseconds
**	dv2				DB_DATA_VALUE describing the second
**					date operand.  The fields are the same
**					as listed for dv1
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	rdv				DB_DATA_VALUE describing the result
**					date operand.  The fields are the same
**					as listed for dv1.
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**	    E_AD1098_BAD_LEN_SEC_INTRNL	Bad float length
**	    E_AD5090_DATE_MUL		Date multiply type usage
**
**	Exceptions:
**	    None
**
** History:
**	05-Mar-2008 (kiria01) b120043
**	    Created
*/

DB_STATUS
adu_date_mul(
ADF_CB         *adf_scb,
DB_DATA_VALUE  *dv1,
DB_DATA_VALUE  *dv2,
DB_DATA_VALUE  *rdv)
{
    
    /* We will be called as either INTV * FLT or FLT * INTV
    ** so swap parameters to simplify code.
    */
    if (abs(dv1->db_datatype) == DB_FLT_TYPE)
    {
	DB_DATA_VALUE  *tmp;
	tmp = dv1; dv1 = dv2; dv2 = tmp;
    }
    if (abs(dv2->db_datatype) == DB_FLT_TYPE)
    {
	f8 f;
	AD_NEWDTNTRNL dn;
	switch (dv2->db_length)
	{
	case 4:
	    f = *(f4*)dv2->db_data;
	    break;
	case 8:
	    f = *(f8*)dv2->db_data;
	    break;
	default:
	    return (adu_error(adf_scb, (i4) E_AD1098_BAD_LEN_SEC_INTRNL, 0));
	}
	switch (abs(dv1->db_datatype))
	{
	case DB_DTE_TYPE:
	case DB_INYM_TYPE:
	case DB_INDS_TYPE:
    	    adu_6to_dtntrnl (dv1, &dn);
	    return ad0_date_mul_intrv(adf_scb, &dn, f, rdv);
	default:
	    break; /* Drop to general type error */
	}
    }
    
    return (adu_error(adf_scb, (i4) E_AD5090_DATE_MUL, 0));
}


/*
** Name: adu_date_div() - ADF interface to divide intervals
**
** Description:
**	  adu_date_div() supports  multiplying an interval with a float
**	The results are the obvious ones:
**	    interval = interval / float
**	    float = interval / interval
**
**	If one of the dates is the empty date, the result is empty.
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      dv1                             DB_DATA_VALUE describing the first date
**					operand.
**	    .db_data			Ptr to AD_NEWDTNTRNL struct.
**		.dn_status		A character whose bits describe the
**					internal date format.
**		.dn_highday		A character for large day intervals.
**		.dn_year		The date's year.
**		.dn_month		The date's month.
**		.dn_lowday		If the date is absolute, this is the
**					day of the month.  If the date is an
**					interval, this is the low-order part
**					of the number of days in the interval.
**		.dn_seconds		The time in seconds from 00:00.
**		.dn_nsecond		Fractional seconds in nanoseconds
**	dv2				DB_DATA_VALUE describing the second
**					date operand.  The fields are the same
**					as listed for dv1
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	rdv				DB_DATA_VALUE describing the result
**					date operand.  The fields are the same
**					as listed for dv1.
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**	    E_AD1098_BAD_LEN_SEC_INTRNL	Bad float length
**	    E_AD5090_DATE_DIV		Date divide type usage
**	    E_AD5092_DATE_DIV_BY_ZERO	Date divide by zero
**	    E_AD5093_DATE_DIV_ANSI_INTV	INDS and INMY can't be divided together
**	    E_AD5094_DATE_ARITH_NOABS	Dates must be intervals
**
**	Exceptions:
**	    None
**
** History:
**	05-Mar-2008 (kiria01) b120043
**	    Created
*/

DB_STATUS
adu_date_div(
ADF_CB         *adf_scb,
DB_DATA_VALUE  *dv1,
DB_DATA_VALUE  *dv2,
DB_DATA_VALUE  *rdv)
{
    f8 f = 0;
    /* The following bools allow us to note when an ingresdate is present
    ** and if this is the case we cause the ym vs dhmsn barrier to be lowered.
    */
    bool dv1_is_DTE = FALSE;
    bool dv2_is_DTE = FALSE;

    switch (abs(dv2->db_datatype))
    {
    case DB_DTE_TYPE:
	dv2_is_DTE = TRUE;
	/*FALLTHROUGH*/
    case DB_INYM_TYPE:
    case DB_INDS_TYPE:
	{
	    AD_NEWDTNTRNL num, den;
	    switch (abs(dv1->db_datatype))
	    {
	    case DB_DTE_TYPE:
		dv1_is_DTE = TRUE;
		/*FALLTHROUGH*/
	    case DB_INYM_TYPE:
	    case DB_INDS_TYPE:
		/* INTERVAL / INTERVAL form */
		if (!dv1_is_DTE && !dv2_is_DTE &&
		    dv1->db_datatype != dv2->db_datatype &&
		    dv1->db_datatype != -dv2->db_datatype)
		{
		    /* We have an illegal combination of INDS & INYM */
		    return adu_error(adf_scb, (i4) E_AD5093_DATE_DIV_ANSI_INTV, 0);
		}
		else
		{
    		    adu_6to_dtntrnl (dv1, &num);
		    adu_6to_dtntrnl (dv2, &den);

		    /* Make sure both are interval forms */
		    if ((num.dn_status&den.dn_status)&AD_DN_LENGTH)
		    {
			f8 fdvr;
			ad0_dte_dtntrnl2f(adf_scb, &num, &f, dv2_is_DTE);
			ad0_dte_dtntrnl2f(adf_scb, &den, &fdvr, dv1_is_DTE);
			if (fdvr == 0.0)
			    return adu_error(adf_scb, (i4) E_AD5092_DATE_DIV_BY_ZERO, 0);
			f /= fdvr;
			switch (rdv->db_length)
			{
			case 4:
			    *(f4*)rdv->db_data = (f4)f;
			    break;
			case 8:
			    *(f8*)rdv->db_data = f;
			    break;
			default:
			    return (adu_error(adf_scb, (i4) E_AD1098_BAD_LEN_SEC_INTRNL, 0));
			}
			return E_DB_OK;
		    }
		    else
			return (adu_error(adf_scb, (i4)E_AD5094_DATE_ARITH_NOABS, 0));
		}
	    }
	}
	break;

    case DB_FLT_TYPE:
	{
	    /* INTERVAL / FLOAT form */
	    AD_NEWDTNTRNL dn;
	    switch (dv2->db_length)
	    {
	    case 4:
		f = *(f4*)dv2->db_data;
		break;
	    case 8:
		f = *(f8*)dv2->db_data;
		break;
	    default:
		return (adu_error(adf_scb, (i4) E_AD1098_BAD_LEN_SEC_INTRNL, 0));
	    }
	    if (f == 0.0)
		return (adu_error(adf_scb, (i4) E_AD5092_DATE_DIV_BY_ZERO, 0));
	    switch (abs(dv1->db_datatype))
	    {
	    case DB_DTE_TYPE:
	    case DB_INYM_TYPE:
	    case DB_INDS_TYPE:
		adu_6to_dtntrnl (dv1, &dn);
		return ad0_date_mul_intrv(adf_scb, &dn, 1/f, rdv);
	    default:
		break; /* Drop to general error */
	    }
	}
    }
    return (adu_error(adf_scb, (i4) E_AD5091_DATE_DIV, 0));
}

/*
** Name: adu_date_plus() - ADF interface to + intervals
**
** Description:
**	  adu_date_plus() supports  + operator
**	The results is the obvious one:
**	    interval = + interval
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      dv1                             DB_DATA_VALUE describing the first date
**					operand.
**	    .db_data			Ptr to AD_NEWDTNTRNL struct.
**		.dn_status		A character whose bits describe the
**					internal date format.
**		.dn_highday		A character for large day intervals.
**		.dn_year		The date's year.
**		.dn_month		The date's month.
**		.dn_lowday		If the date is absolute, this is the
**					day of the month.  If the date is an
**					interval, this is the low-order part
**					of the number of days in the interval.
**		.dn_seconds		The time in seconds from 00:00.
**		.dn_nsecond		Fractional seconds in nanoseconds
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	rdv				DB_DATA_VALUE describing the result
**					date operand.  The fields are the same
**					as listed for dv1.
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**	    E_AD5094_DATE_ARITH_NOABS	Dates must be intervals
**
**	Exceptions:
**	      None
**
** History:
**	05-Mar-2008 (kiria01) b120043
**	    Created
*/

DB_STATUS
adu_date_plus(
ADF_CB         *adf_scb,
DB_DATA_VALUE  *dv1,
DB_DATA_VALUE  *rdv)
{
    AD_NEWDTNTRNL dn;

    switch (abs(dv1->db_datatype))
    {
    case DB_DTE_TYPE:
    case DB_INYM_TYPE:
    case DB_INDS_TYPE:
	adu_6to_dtntrnl (dv1, &dn);
	if (dn.dn_status & AD_DN_LENGTH)
	{
	    adu_7from_dtntrnl (rdv, &dn);
	    return E_DB_OK;
	}
	/*FALLTHROUGH*/
default:
	return adu_error(adf_scb, (i4)E_AD5094_DATE_ARITH_NOABS, 0);
    }
}

/*
** Name: adu_date_minus() - ADF interface to - intervals
**
** Description:
**	  adu_date_minus() supports  - operator
**	The results is the obvious one:
**	    interval = - interval
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      dv1                             DB_DATA_VALUE describing the first date
**					operand.
**	    .db_data			Ptr to AD_NEWDTNTRNL struct.
**		.dn_status		A character whose bits describe the
**					internal date format.
**		.dn_highday		A character for large day intervals.
**		.dn_year		The date's year.
**		.dn_month		The date's month.
**		.dn_lowday		If the date is absolute, this is the
**					day of the month.  If the date is an
**					interval, this is the low-order part
**					of the number of days in the interval.
**		.dn_seconds		The time in seconds from 00:00.
**		.dn_nsecond		Fractional seconds in nanoseconds
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	rdv				DB_DATA_VALUE describing the result
**					date operand.  The fields are the same
**					as listed for dv1.
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**	    E_AD5094_DATE_ARITH_NOABS	Need to used on intervals
**
**	Exceptions:
**	    None
**
** History:
**	05-Mar-2008 (kiria01) b120043
**	    Created
*/

DB_STATUS
adu_date_minus(
ADF_CB         *adf_scb,
DB_DATA_VALUE  *dv1,
DB_DATA_VALUE  *rdv)
{
    AD_NEWDTNTRNL dn;

    switch (abs(dv1->db_datatype))
    {
    case DB_DTE_TYPE:
    case DB_INYM_TYPE:
    case DB_INDS_TYPE:
	adu_6to_dtntrnl (dv1, &dn);
	if (dn.dn_status & AD_DN_LENGTH)
	{
	    if (dn.dn_status & AD_DN_YEARSPEC)
		dn.dn_year = -dn.dn_year;
	    if (dn.dn_status & AD_DN_MONTHSPEC)
		dn.dn_month = -dn.dn_month;
	    if (dn.dn_status & AD_DN_DAYSPEC)
		dn.dn_day = -dn.dn_day;
	    if (dn.dn_status & AD_DN_TIMESPEC)
	    {
		dn.dn_seconds = -dn.dn_seconds;
		dn.dn_nsecond = -dn.dn_nsecond;
	    }
	    /* Let normalise do its job */
	    adu_1normldate(&dn);
	    adu_7from_dtntrnl (rdv, &dn);
	    return E_DB_OK;
	}
	/*FALLTHROUGH*/
default:
	return adu_error(adf_scb, (i4)E_AD5094_DATE_ARITH_NOABS, 0);
    }
}


/*
** Name: ad0_date_mul_intrv() - ADF interface to multiply interval by float
**
** Description:
**	  ad0_date_mul_intrv() supports  multiplying an interval with a float
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      dv1                             DB_DATA_VALUE describing the first date
**					operand.
**	    .db_data			Ptr to AD_NEWDTNTRNL struct.
**		.dn_status		A character whose bits describe the
**					internal date format.
**		.dn_highday		A character for large day intervals.
**		.dn_year		The date's year.
**		.dn_month		The date's month.
**		.dn_lowday		If the date is absolute, this is the
**					day of the month.  If the date is an
**					interval, this is the low-order part
**					of the number of days in the interval.
**		.dn_seconds		The time in seconds from 00:00.
**		.dn_nsecond		Fractional seconds in nanoseconds
**	dv2				DB_DATA_VALUE describing the second
**					date operand.  The fields are the same
**					as listed for dv1
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	rdv				DB_DATA_VALUE describing the result
**					date operand.  The fields are the same
**					as listed for dv1.
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**	    E_AD5094_DATE_ARITH_NOABS	Needs interval dates
**
**	Exceptions:
**	    None
**
** History:
**	05-Mar-2008 (kiria01) b120043
**	    Created
*/

static DB_STATUS
ad0_date_mul_intrv(
ADF_CB         *adf_scb,
AD_NEWDTNTRNL  *dn,
f8		mul,
DB_DATA_VALUE  *rdv)
{
    DB_STATUS status = E_DB_OK;
    f8 temp;
    if (dn->dn_status & AD_DN_LENGTH)
    {
	ad0_dte_dtntrnl2f(adf_scb, dn, &temp, FALSE);
	temp *= mul;
	ad0_dte_f2dtntrnl(adf_scb, temp, dn, FALSE);
	adu_7from_dtntrnl (rdv, dn);
    }
    else if (!(dn->dn_status & AD_DN_NULL))
        status = adu_error(adf_scb, (i4) E_AD5094_DATE_ARITH_NOABS, 0);
    return status;
}


/*
** Name: ad0_dte_dtntrnl2f() - ADF convert interval to float
**
** Description:
**	  ad0_dte_dtntrnl2f() supports  converting an interval to a float
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      dn                              Ptr to AD_NEWDTNTRNL to convert
**		.dn_status		A character whose bits describe the
**					internal date format - must have LENGTH set.
**		.dn_year		The number of years
**		.dn_month		The number of months
**		.dn_day   		The number of days
**		.dn_seconds		The number of seconds
**		.dn_nsecond		The number of nanoseconds
**	isDTE				If set override, the ANSI interval
**					bounding.
** Outputs:
**	flt				Float8 pointer for result = num of nanoseconds
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**	    E_AD5094_DATE_ARITH_NOABS	Needs interval dates
**
** History:
**	05-Mar-2008 (kiria01) b120043
**	    Created
*/

static DB_STATUS
ad0_dte_dtntrnl2f(
ADF_CB         *adf_scb,
AD_NEWDTNTRNL  *dn,
f8	       *flt,
bool		isDTE)
{
    DB_STATUS status = E_DB_OK;

    if (dn->dn_status & AD_DN_LENGTH)
    {
	f8 f = 0.0;

	if ((isDTE || dn->dn_dttype != DB_INDS_TYPE) && (dn->dn_status & AD_DN_YEARSPEC))
	    f += dn->dn_year;
	f *= AD_19DTE_QUARPERYEAR * AD_17DTE_MONPERQUAR;
	if ((isDTE || dn->dn_dttype != DB_INDS_TYPE) && (dn->dn_status & AD_DN_MONTHSPEC))
	    f += dn->dn_month;
	f *= AD_13DTE_DAYPERMONTH;
	if ((isDTE || dn->dn_dttype != DB_INYM_TYPE) && (dn->dn_status & AD_DN_DAYSPEC))
	    f += dn->dn_day;
	f *= AD_40DTE_SECPERDAY;
	if ((isDTE || dn->dn_dttype != DB_INYM_TYPE) && (dn->dn_status & AD_DN_TIMESPEC))
	{
	    f += dn->dn_seconds;
	    f *= AD_50DTE_NSPERS;
	    f += dn->dn_nsecond;
	}
	else
	    f *= AD_50DTE_NSPERS;
	*flt = f;
    }
    else
        status = adu_error(adf_scb, (i4) E_AD5094_DATE_ARITH_NOABS, 0);
    return status;
}


/*
** Name: ad0_dte_f2dtntrnl() - ADF interface to convert interval to nanoseconds
**
** Description:
**	  ad0_dte_f2dtntrnl() supports conversion to nanoseconds
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**	flt				Float8 value to convert.
**	isDTE				If set override, the ANSI interval
**					bounding.
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**      dn                              Ptr to AD_NEWDTNTRNL to convert
**		.dn_status		A character whose bits describe the
**					internal date format - must have LENGTH set.
**		.dn_year		The number of years
**		.dn_month		The number of months
**		.dn_day   		The number of days
**		.dn_seconds		The number of seconds
**		.dn_nsecond		The number of nanoseconds
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**	    E_AD5094_DATE_ARITH_NOABS	Needs interval dates
**
** History:
**	05-Mar-2008 (kiria01) b120043
**	    Created
*/

static DB_STATUS
ad0_dte_f2dtntrnl(
ADF_CB         *adf_scb,
f8		flt,
AD_NEWDTNTRNL  *dn,
bool		isDTE)
{
    DB_STATUS status = E_DB_OK;

    if (dn->dn_status & AD_DN_LENGTH)
    {
	i4 temp;

	/* Clear flags & reset later */
	dn->dn_status &= ~(AD_DN_YEARSPEC|AD_DN_MONTHSPEC|AD_DN_DAYSPEC|AD_DN_TIMESPEC);

	/* If pure DB_INDS_TYPE - leave years & months in flt
	** Doing this means that we get correct deltas with big
	** differences without resorting to using months and
	** years and the fiddling that that implies due to
	** varability of both.
	*/
	if (isDTE || dn->dn_dttype != DB_INDS_TYPE)
	{
	    /* Years */
	    temp = flt / (AD_19DTE_QUARPERYEAR * AD_17DTE_MONPERQUAR * AD_13DTE_DAYPERMONTH * AD_40DTE_SECPERDAY * AD_50DTE_NSPERS);
	    if (dn->dn_year = temp)
		dn->dn_status |= AD_DN_YEARSPEC;
	    else
		dn->dn_status &= ~AD_DN_YEARSPEC;
	    /* Subtract years */
	    flt -= temp *(AD_19DTE_QUARPERYEAR * AD_17DTE_MONPERQUAR * AD_13DTE_DAYPERMONTH * AD_40DTE_SECPERDAY * AD_50DTE_NSPERS);

	    /* Months */
	    temp = flt / (AD_13DTE_DAYPERMONTH * AD_40DTE_SECPERDAY * AD_50DTE_NSPERS);
	    if (dn->dn_month = temp)
		dn->dn_status |= AD_DN_MONTHSPEC;
	    else
		dn->dn_status &= ~AD_DN_MONTHSPEC;
	    /* Subtract months */
	    flt -= temp *(AD_13DTE_DAYPERMONTH * AD_40DTE_SECPERDAY * AD_50DTE_NSPERS);
	}

	if (isDTE || dn->dn_dttype != DB_INYM_TYPE)
	{
	    /* Days */
	    temp = flt / (AD_40DTE_SECPERDAY * AD_50DTE_NSPERS);
	    if (dn->dn_day = temp)
		dn->dn_status |= AD_DN_DAYSPEC;
	    else
		dn->dn_status &= ~AD_DN_DAYSPEC;
	    /* Subract days */
	    flt -= temp *(AD_40DTE_SECPERDAY * AD_50DTE_NSPERS);

	    /* Seconds etc */
	    dn->dn_seconds = flt / AD_50DTE_NSPERS;
	    dn->dn_nsecond = flt - dn->dn_seconds * AD_50DTE_NSPERS;
	    if (dn->dn_seconds | dn->dn_nsecond)
		dn->dn_status |= AD_DN_TIMESPEC;
	    else
		dn->dn_status &= ~AD_DN_TIMESPEC;
	}
    }
    else
        status = adu_error(adf_scb, (i4) E_AD5094_DATE_ARITH_NOABS, 0);
    return status;
}


/*{
** Name: adu_date_histmap()	-   convert a date value into its corresponding
**				    histogram value.
**
** Description:
**        This routine converts a date value into its corresponding histogram
**	value.  Currently, the algorithm used converts the date into total
**	number of hours with some fudge factor?
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      from_dv                         DB_DATA_VALUE describing date data value
**					to convert to a histogram value.
**	    .db_data			Ptr to a AD_NEWDTNTRNL struct.
**	hg_dv				DB_DATA_VALUE describing the histogram
**					data to be made.  Currently, this is
**					assumed to be an integer of length 4.
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**      hg_dv
**	    *.db_data			An i4 corresponding to the total number
**					of hours in the input date.
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	15-jun-85 (ericj)
**          Converted for Jupiter.
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**      16-jun-1993 (stevet)
**          Make special arragement for handling empty date and 
**          date-without-time date datatypes.  In case of empty date,
**          the histogram value will be be less than MIN_INV_YR and
**          for date-without-time, timezone offset will be adjusted.
**	 6-apr-2001 (hayke02)
**	    We now ensure rounding down to the correct hour when we have
**	    a negative dn_time by subtracting 0.999...9 (f8val). This
**	    change fixes bug 91872.
**	12-may-98 (inkdo01)
**	    Changes to remove huge discontinuities at end of each year
**	    in date histogram values. 
**	26-apr-01 (inkdo01)
**	    Added entry at start of month_hours, then changed reference
**	    from dn_month-1 to dn_month to accomodate null dates with 
**	    dn_month value of 0.
**	17-jul-2006 (gupsh01)
**	    Modified to support new AD_NEWDTNTRNL internal format.
**	26-sep-2006 (dougi)
**	    Define distinct values for TIME and INTERVAL types.
**	07-nov-2006 (gupsh01)
**	    Fixed seconds calculations.
**	08-feb-2007 (gupsh01)
**	    Fixed the dtype. It must be of type of from_dv, not 
**	    hg_dv.
**	05-Nov-2007 (hweho01)
**	    Avoid compiler/optimizer problem on a64_sol, rearrange   
**	    the for loop. It looped more times than the counter was set,
**	    caused memory overlay. Version: Sun C 5.7 Patch 117837.
*/
DB_STATUS
adu_3datehmap(
ADF_CB          *adf_scb,
DB_DATA_VALUE	*from_dv,
DB_DATA_VALUE	*hg_dv)
{
    register i4		mapval;
    AD_NEWDTNTRNL	*datep;
    AD_NEWDTNTRNL	local_datep;
    DB_DT_ID		dtype = abs(from_dv->db_datatype);

    datep = &local_datep;
    adu_6to_dtntrnl (from_dv, datep);

    /* If empty date, set histogram value to 1 year less than MIN_INV_YR */
    if (local_datep.dn_status == AD_DN_NULL)
    {
	local_datep.dn_year = MIN_INV_YR - 1;
    }

    /* 
    ** If we are dealing with date-without-time, we want to add timezone 
    ** adjustment so that if date column is mixing date-with-time and
    ** date-without-time, the histogram order is the same as order-by 
    ** ordering.
    */
    if (local_datep.dn_status & AD_DN_ABSOLUTE && 
	!(local_datep.dn_status & AD_DN_TIMESPEC))
    {
	   if ((local_datep.dn_dttype != DB_TMWO_TYPE) && 
	       (local_datep.dn_dttype != DB_TMW_TYPE) && 
	       (local_datep.dn_dttype != DB_TSWO_TYPE) && 
	       (local_datep.dn_dttype != DB_TSW_TYPE))
	   {
		local_datep.dn_seconds -=
	    	     TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL, 
			adu_5time_of_date(&local_datep));
	   }
    }
 
    /* Select transform based on type. */
    switch (dtype) {
      case DB_DTE_TYPE:
      case DB_ADTE_TYPE:
      case DB_TSTMP_TYPE:
      case DB_TSW_TYPE:
      case DB_TSWO_TYPE:
	/* Convert to number of hours since 1/1/1. Note: no attempt is made to 
	** account for non-leapyears, because this guy is supposed to be fast.
	** A 24 hour discontinuity once per year isn't too bad! */
	mapval = local_datep.dn_year * 8784;       /* 366 days * 24 hours */
	mapval += month_hours[local_datep.dn_month];  /* hours up to that month */
	mapval += ((i4) (local_datep.dn_day)) * 24;
	if (local_datep.dn_seconds > 0)
	    mapval += local_datep.dn_seconds / AD_39DTE_ISECPERHOUR ;
	else if (local_datep.dn_seconds < 0)
	{
	    f8	f8val;
	    char	fstr[DBL_DIG + 4];
	    i1	i;

	    STcopy("0.", fstr);
	    for ( i = DBL_DIG - 1; i > 0 ;  i-- )
		STcat(fstr, "9");

	    CVaf(fstr, '.', &f8val);

	    mapval += (i4)(((f8)local_datep.dn_seconds / AD_39DTE_ISECPERHOUR) - f8val);
	}
	break;

      case DB_TME_TYPE:
      case DB_TMW_TYPE:
      case DB_TMWO_TYPE:
      case DB_INDS_TYPE:
	/* For times and day-second intervals, value is number of 
	** milliseconds in value. */
	mapval = (local_datep.dn_day * AD_40DTE_SECPERDAY + local_datep.dn_seconds)
			* AD_6DTE_IMSPERSEC + (local_datep.dn_nsecond / AD_29DTE_NSPERMS);
	break;

      case DB_INYM_TYPE:
	/* For year-month intervals, value is simply the number of months. */
	mapval = local_datep.dn_year * 12 + local_datep.dn_month;
	break;
    }

    *(i4 *) hg_dv->db_data = mapval;

    return (E_DB_OK);
}


/*{
** Name: ad0_6valcopy - copy DB_DATA_VALUE string into a char buffer
**
** Description:
**        The routine will take the string associated with a DB_DATA_VALUE
**	structure and extract the string component.  The routine will then
**	move the string component to the char buffer.  The routine will also
**	indicate if any nonblank characters where found in any truncation of
**	the string.  Blanks will be eliminated from the beginning of the string.
**      The string returned in the char buffer will be \0 terminated. 
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      str_spec                        A DB_DATA_VALUE struct containing the
**					the string to be moved
**      char_len                        The max length of the buffer to which
**					the string will be moved
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**      char_ptr                        A ptr to the buffer to which the string
**					will be moved.
**  result_len				A ptr to the number of characters moved
**					into the string pointed at by char_ptr
**  Returns:
**      Boolean which is TRUE if there was a truncation of the string
**      and the truncated portion of the string contained some non blank
**      characters.
**
** History:
**	08-dec-85 (seputis)
**          initial creation
**	28-may-86 (ericj)
**	    Converted for Jupiter.
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	15-sep-87 (thurston)
**	    Changed to return a bool, and made all of the returns consistently
**	    return a bool.  (Some used to return DB_STATUS, some nats, etc.)
**	    Also fixed bug that looked at wrong character.
*/

static	bool
ad0_6valcopy(
ADF_CB          *adf_scb,
DB_DATA_VALUE	*str_dv,
char	        *char_ptr,
i4		char_len,
i4		*result_len)
{
    register char   *str_ptr;   /* ptr to unit spec */
    char	    *tmp_ptr;	/* temporary so address can be passed for
				** assignment.
				*/
    register i4     str_len;    /* length of temp string for unit spec */
    i4		    tmp_len;	/* temp to pass address for assignment */
    bool	    errorfound;	/* TRUE if str truncated with nonblnks*/

    /* PERF-useless repeated procedure call on unchanging object */
    errorfound = FALSE;
    if (adu_3straddr(adf_scb, str_dv, &tmp_ptr) != E_DB_OK)
	return (TRUE);
    str_ptr = tmp_ptr;    
    if (adu_5strcount(adf_scb, str_dv, &tmp_len) != E_DB_OK)
	return (TRUE);
    str_len = tmp_len;

    /* eliminate any blanks from the beginning of the string */
    while (str_len > 0  &&  *str_ptr == ' ')
    {
	str_ptr++;
	str_len--;
    }

    char_len -= 1;	    /* reserve place for EOS terminator */
    while (str_len > char_len)
    {
	/* check the length of the string and return an error if the
	** truncated portion of the string is not blank
	*/
        if (str_ptr[--str_len] != ' ')
	{
	    errorfound = TRUE;
	    str_len = char_len;     /* report only truncated input string */
	    break;
	}
    }

    /* Eliminate trailing blanks */
    while (str_len > 0  &&  str_ptr[str_len - 1] == ' ')
    {
    	str_len--;
    }
    
    MEcopy((PTR)str_ptr, str_len, (PTR)char_ptr);	/* copy the string */
    char_ptr[str_len] = EOS;	    /* terminate the string */
    *result_len = str_len;          /* return length of string */
    return (errorfound);            /* report success of op */
}


/*{
** Name: ad0_7errvaldsc() - Report an error with a string from a DB_DATA_VALUE
**		            struct.
**
** Description:
**        This routine will not return.  It will report an error with a string 
**      from a DB_DATA_VALUE structure attached to the error message as a
**	parameter. 
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      error_code                      The error code to be passed to adu_error
**      DB_DATA_VALUE                   Ptr to DB_DATA_VALUE string which will
**					be attached to the error message.
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**      none 
**  Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**      DOES NOT RETURN
**
** History:
**  08-dec-85 (seputis)
**          initial creation
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
*/

static	DB_STATUS
ad0_7errvaldsc(
ADF_CB              *adf_scb,
i4		    error_code,
DB_DATA_VALUE	    *str_dv)
{
    char            emessage[80];	/* buffer in which to move
					** string from str_dv
					*/
    i4              str_len;		/* length of string in emessage */

    _VOID_ ad0_6valcopy(adf_scb, str_dv, emessage,
			    (i4) sizeof(emessage), &str_len);  

    return (adu_error(adf_scb, error_code, 2,
		      (i4) 0,
		      (i4 *) emessage));
}


/*{
** Name: ad0_8getdateclass() - given a date interval return the date class
**
** Description:
**      The is an internal routine used to translate the date unit string 
**      into an internal token which is more convenient for processing 
**      purposes. 
**        
**      Example 1, 
**          GETDATECLASS("month" , token) 
**      would return the token 'M' 
**        
**      Example 2, 
**          GETDATECLASS("second", token) 
**      would return the token 'S' 
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      inter_spec                      This can be one of the following :
**                      year    yr
**                      quarter qtr
**                      month   mo
**                      week    wk
**                      day     
**                      hour    hr
**                      minute  min
**                      second  sec 
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**      date_class          char token with following association :
**                      year    yr    'Y'
**                      quarter qtr   'Q'
**                      month   mo    'M'
**                      week    wk    'W'
**                      day           'D'
**                      hour    hr    'H'
**                      minute  min   'I'
**                      second  sec   'S'
**			iso_week      'w'
**  Exceptions:
**      E_AD505F_DATEINTERVAL - error indicates that the interval
**				specification was not recognized.
**
** History:
**	03-dec-85 (seputis)
**          initial creation
**	28-may-86 (ericj)
**	    Converted for Jupiter.
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	18-jan-2007 (gupsh01)
**	    Increase the limits to support timezone_minute.
*/

static	DB_STATUS
ad0_8getdateclass(
ADF_CB		*adf_scb,
DB_DATA_VALUE   *inter_spec,
char            *date_class)
{
    register ADU_DATENAME    *d;

#define  AD_MAXDUNIT  16    /* max len of interval string (i.e. unit spec) */
    char        intspec[AD_MAXDUNIT+1];	/* temp for lookup of unit spec*/
    i4          dummy;

    /* copy interval spec string to intspec and check for garbage characters */
    if (ad0_6valcopy(adf_scb, inter_spec, intspec,
		     (i4) sizeof(intspec), &dummy))
    {
	/* garbage characters found in the truncation so report error */
	return (ad0_7errvaldsc(adf_scb, (i4) E_AD505F_DATEINTERVAL, inter_spec));
    }

    _VOID_ CVlower(intspec); /* the interval spec shouldn't be case sensitive */

    d = ad0_13tab_look(intspec);
    if (d->d_class == 'u')
    {
	/*report an error if the interval string cannot be found*/
	return (ad0_7errvaldsc(adf_scb, (i4) E_AD505F_DATEINTERVAL, inter_spec));
    }

    *date_class = d->d_class;
    return (E_DB_OK);
}


/*{
** Name: ad0_9daysinyear() - the day number with respect to beginning of year
**
** Description:
**      The routine will accept an absolute date and return the number of 
**      days in the current year, where Jan 1 is returned as 1.
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      datep                           ptr to absolute date, the date must
**					have been adjusted for Greenwich mean
**					time.
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**      daynumber                       number of days from Jan 1 to date
**					contained in datep
**
** History:
**	09-dec-85 (seputis)
**          initial creation
**	28-may-86 (ericj)
**	    Converted for Jupiter.
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	17-jul-2006 (gupsh01)
**	    Modify input parameter of ad0_9daysinyear to AD_NEWDTNTRNL
**	    from AD_DATENTRNL.
*/

static	VOID
ad0_9daysinyear(
AD_NEWDTNTRNL    *datep,
i4		*daynumber)
{
    i4          month;      /* variable to index thru months */
    i4          year;       /* local variable use for speed */
    i4          days;       /* calculate number of days */

    days = 0;
    year = datep->dn_year;
    month = datep->dn_month - 1;

    /* calculate the days in the previous month */
    for (; month > 0 ; days += adu_2monthsize(month--, year));

    *daynumber = days + datep->dn_day; /* add days in current month */
}


/*{
** Name: ad0_10togregorian() - Given an absolute date convert to number of days
**
** Description:
**      ad0_10togregorian(datep, daycount, dayofweek)
**          The date is verified to be an absolute date and it is converted 
**      into a number of days from 0 A.D. which is returned in daycount.  The 
**      function will also return the day of the week.  It is assumed that
**  datep is already adjusted for the local time zone.
**
**      Quoted from Encyclopaedia Britannica.
**  "The orbital period (of earth) is 365.242 days, or, to a first
**  approximation, 365.25 days.  To account, then, for the odd quarter day,
**  an extra calendar day is added once every four years, as was first
**  done in 46 B.C. ...  However, over many centuries, the difference
**  between the approximete value 0.25 days and the more exact 0.242 days
**  accumulates significantly and is adjusted by adding the extra day
**  to those century years exactly divisible by 400."
**
**  Quoted from Webster's
**  "Gregorian calendar - a calendar in general use introduced in 1582
**  by Pope Gregory XIII as a revision of the Julian calendar, adopted in
**  Great Britian and the American colonies in 1752, and marked by the
**  suppression of 10 days or after 1700 11 days and the restriction
**  that only those centesimal years divisible by 400 should be leap
**  years"
**
**  There was a further adjustment which causes a leap year to not exist
**  every 4000 years.  This was not implemented in this routine.
**
**  In America the dates Sept 3-13 1752 do not exist due to the transition
**  from the Julian to the Gregorian Calendar.  This routine will operate
**  only for Gregorian time which means that the absolute date will be
**  converted into a number of days for 1 A.D. with the assumption that
**  Gregorian time was used.
**
**  Note, that these Gregorian routines do not try to match the American
**  calendar exactly and in fact would be in error for days prior to
**  Sept 14 1752, where the Julian calendar was used.  The proper way
**  to solve this problem would be to have a similiar set of Julian
**  calendar routines and check on transition date.  The problem is that
**  the transition date was not uniform across the world.  The best
**  solution would be to standardize on Gregorian time from 1 A.D., and
**  introduce more functions which can be used to convert Julian or
**  American time to and from Gregorian.  It seems that other countries
**  using Ingres would want to modify adjust the algorithm given the
**  historical transition within their respective countries.  FIXME.
**
**  As far as I can tell the dow function uses a Julian calendar but
**  the monthsize function uses a Gregorian algorithm from 1582.
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      datep                           ptr to absolute date (already adjusted
**                  			for the local time zone).
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**      daycount                        number of days from 0 A.D.
**      dayofweek                       day of week with following association
**                      1 monday 
**                      2 tuesday
**                      3 wednesday
**                      4 thursday
**                      5 friday 
**                      6 saturday
**                      7 sunday 
**  Exceptions:
**      none
**
** History:
**  03-dec-85 (seputis)
**          initial creation
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
*/

static	VOID
ad0_10togregorian(
AD_NEWDTNTRNL          *datep,
i4                 *daycount,
i4                 *dayofweek)
{
#define         AD_IYRPERLEAP     4 	/* interval for leap years */
#define         AD_IYRPERCEN    100 	/* years for a century */
#define         AD_IYRPERQUAD   400 	/* years for a quadcentury */
#define         AD_FIRSTADDAY     1 	/* the first day on Jan 1, 1 A.D. */
					/*     Gregorian is a Monday == 1 */

    register i4     totaldays;      /* working variable for totaling */
    i4		    year;	    /* local variable use for speed */
    i4		    daynumber;      /* # of days in current year */

    ad0_9daysinyear(datep, &daynumber);	    /* get # of days in current year */

    year = datep->dn_year - 1;      /* get # of days from prev years */
    totaldays = year * AD_16DTE_IDAYPERYEAR  /* # of days per year * years  */
        + (year / AD_IYRPERLEAP)    /* add one day for each leap year       */
        - (year / AD_IYRPERCEN)	    /* minus one leap year for each century */
        + (year / AD_IYRPERQUAD)    /* except for every quad century        */
        + daynumber;		    /* plus # of days in current year       */

    /* the total number of days size 1 A.D. is now in totaldays */
    *daycount = totaldays;

    /*
    ** Calculate day of week and place it into return operand.  Note that
    ** AD_15DTE_IDAYPERWEEK is added so that mod operand will not be negative.
    */
    *dayofweek = (((totaldays + AD_15DTE_IDAYPERWEEK) - AD_FIRSTADDAY)
			% AD_15DTE_IDAYPERWEEK) + 1;
}


/*{
** Name: ad0_11fromgregorian() - Given an a number of days convert to
**				 absolute date
**
** Description:
**      ad0_11fromgregorian(daycount, datep)
**          The daycount is a number of days from 1 A.D. and it is converted
**      into an absolute date which is returned in datep
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**  daycount            number of days from 1 A.D.
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**      datep                           ptr to absolute date.
**       
**  Exceptions:
**      none
**
** Side Effects:
**      none
**
** History:
**  03-dec-85 (seputis)
**          initial creation
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	14-dec-90 (jrb)
**	    Fixed bug where we were off by one if date was 30-dec in a leap
**	    year; we were returning 31-dec which was incorrect.
**	17-jul-2006 (gupsh01)
**	    Modify input parameter of ad0_11fromgregorian to AD_NEWDTNTRNL
**	    from AD_DATENTRNL.
*/

static	VOID
ad0_11fromgregorian(
i4            daycount,
AD_NEWDTNTRNL  *datep)
{
#define  AD_I4YEAR	    (365*4+1)               /* # days in 4 yr intrvl */
#define  AD_100_IDAYPER100  ((i4)25*AD_I4YEAR-1)    /* # days in century */
#define  AD_400_IDAYPER400  (1+4*AD_100_IDAYPER100) /* # days in quad century */

    i4          quadcentury;    /* number of 400 year intervals */
    i4          century;	/* + remainder 100 year intervals */
    i4          fouryear;	/* + remainder 4 year intervals */
    i4          year;		/* + remaining years */
    i4          totaldays;	/* working variable for days */

    totaldays = daycount;
    quadcentury = totaldays / AD_400_IDAYPER400;
    totaldays %= AD_400_IDAYPER400;
    century = totaldays / AD_100_IDAYPER100;
    totaldays %= AD_100_IDAYPER100;
    fouryear = totaldays / AD_I4YEAR;
    totaldays %= AD_I4YEAR;
    year = totaldays / AD_16DTE_IDAYPERYEAR;
    totaldays %= AD_16DTE_IDAYPERYEAR;
    datep->dn_year = quadcentury*400 + century*100 + fouryear*4 + year;

    if (totaldays == 0)
    {
	datep->dn_month = 12;

	/* if year is 4 then we're in a leap year which means that this is the
	** second to last day of the year (30-dec); 31-dec on a leap year
	** would cause totaldays to be zero in the mod by AD_I4YEAR and year
	** would be 0, so if year is anything but 4, the day is 31-dec.
	*/
	if (year == 4)
	    datep->dn_day = 30;
	else
	    datep->dn_day = 31;
    }
    else
    {
	i4                cmonsize;           /* save no. of days of month */
	i4                month;              /* counter for no. of months */
	month = 0;
	cmonsize = 0;
	datep->dn_year += 1;            /* go to next year */
	year = datep->dn_year;

	do
	{
	    totaldays -= cmonsize;
	    cmonsize = adu_2monthsize(++month, year);
	} while (totaldays > cmonsize);

	datep->dn_month = month;
	datep->dn_day = totaldays;
    }
    datep->dn_seconds = 0;             /* initialize time = 00:00:00*/
    datep->dn_nsecond = 0;
    datep->dn_status = AD_DN_ABSOLUTE |
		       AD_DN_YEARSPEC |
		       AD_DN_MONTHSPEC |
		       AD_DN_DAYSPEC;
}


/*
** Name: adu_intrvl_in() -	return number of units in date_spec.
**
** Description:
**	This is the internal implementation of the interval() SQL/QUEL function.
**
**	  Returns the number of minutes, hours, days, etc. (as indicated in
**	  inter_spec) that exist in the date (indicated in date_spec). This
**	  value is returned as an f8 in result.
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      inter_spec                      DB_DATA_VALUE describing the interval
**					specification.
**	    .db_datatype		Its string type.
**	    .db_length			Its length.
**	    .db_data			Either a pointer to the interval spec
**					string or a pointer to a DB_TEXT_STRING
**					struct containing string.
**	date_spec			DB_DATA_VALUE describing the date to
**					calculate the units from.
**	    .db_data			Ptr to AD_NEWDTNTRNL struct.
**		.dn_status		A character whose bits describe the
**					internal date format.
**		.dn_highday		A character for large day intervals.
**		.dn_year		The date's year.
**		.dn_month		The date's month.
**		.dn_lowday		If the date is absolute, this is the
**					day of the month.  If the date is an
**					interval, this is the low-order part
**					of the number of days in the interval.
**		.dn_time		The time in milliseconds from 00:00.
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	result				DB_DATA_VALUE that will hold the result.
**					It is assumed this is of type float
**					with a length of 8.
**	    .db_data			Ptr to an area to hold an f8.
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**
**	Exceptions:
**	    E_AD505F_DATEINTERVAL	An invalid date interval was passed in.
**  History
**      9-oct-85 (dreyfus) comment added.
**      9-oct-85 (mgw)  fix compiler induced bug when computing
**          number nsecs specified in 'date_spec'.
**	28-may-86 (ericj)
**	    Converted for Jupiter.
**	13-jul-87 (thurston)
**	    Changed to use the symbolic constants AD_13DTE_DAYPERMONTH and
**	    AD_14DTE_DAYPERYEAR instead of hardwired numbers ... especially
**	    since the hardwired numbers were wrong!
**	24-jul-89 (jrb)
**	    This fix got lost so I am reapplying it.  We now disallow absolute
**	    dates for this function and we define the result to be zero for the
**	    empty date.
**	14-dec-90 (jrb)
**	    Changes for Dave Hung; Sun compiler has problems casting unsigned to
**	    double, so we need temp variables to get around it.
**	17-jul-2006 (gupsh01)
**	    Added support for new ANSI datetime types.
**	03-oct-06 (toumi01)
**	    Correct interval computation - need AD_40DTE_SECPERDAY multiplier.
**	    Remove unused variable tlowday.
*/

DB_STATUS
adu_intrvl_in(
ADF_CB         *adf_scb,
DB_DATA_VALUE  *inter_spec,        /* interval spec */
DB_DATA_VALUE  *date_spec,
DB_DATA_VALUE  *result)
{
    DB_STATUS	db_stat;
    char        dateclass;      /* represents the date interval */

    if (db_stat = ad0_8getdateclass(adf_scb, inter_spec, &dateclass)) 
	return (db_stat);

    {
        register f8	    *res;
        f8		    secs;
        register AD_NEWDTNTRNL  *datep;
	AD_NEWDTNTRNL       ndatev;

        datep = &ndatev;
        MEfill ((u_i2) sizeof (ndatev), NULLCHAR, (PTR) &ndatev);
        adu_6to_dtntrnl (date_spec, datep);

        res	= (f8 *)result->db_data;

        if ((
	     (datep->dn_dttype == DB_DTE_TYPE) &&
	     (datep->dn_status & AD_DN_ABSOLUTE)
	    )
         || (datep->dn_dttype == DB_INYM_TYPE)
         || (datep->dn_dttype == DB_INDS_TYPE)
        )
	    return(adu_error(adf_scb, E_AD505E_NOABSDATES, 0));

	if (datep->dn_status == AD_DN_NULL)
	{
	    *res = 0.0;
	    return(E_DB_OK);
	}

        /* Cast values to f8 to overcome DEC compiler bug. */
        secs = (
                      (f8) ((i4)(I1_CHECK_MACRO(datep->dn_day))
                )
                + AD_13DTE_DAYPERMONTH * (f8)datep->dn_month
                + AD_14DTE_DAYPERYEAR  * (f8)datep->dn_year
            )
	    * (f8)AD_40DTE_SECPERDAY
            + (f8)datep->dn_seconds;

        switch (dateclass)
        {
        case 'S':
            *res = secs;
            break;

        case 'I':
            *res = secs / AD_43DTE_SECPERMIN;
            break;

        case 'H':
            *res = secs / AD_44DTE_SECPERHOUR;
            break;

        case 'D':
            *res = secs / AD_40DTE_SECPERDAY;
            break;

        case 'M':
            *res = secs / (AD_40DTE_SECPERDAY * AD_13DTE_DAYPERMONTH);
            break;

        case 'Q':
            *res = secs /
		   (AD_40DTE_SECPERDAY * AD_14DTE_DAYPERYEAR
			/ AD_19DTE_QUARPERYEAR);
            break;

        case 'W':
            *res = secs / (AD_40DTE_SECPERDAY * AD_12DTE_DAYPERWEEK);
            break;

        case 'Y':
            *res = secs / (AD_40DTE_SECPERDAY * AD_14DTE_DAYPERYEAR);
            break;
        default:
            /* if class was not an interval then return an error */
            return (ad0_7errvaldsc(adf_scb, E_AD505F_DATEINTERVAL, inter_spec));
            break;
        }
    }

    return (E_DB_OK);
}


/*{
** Name: adu_dtruncate()  - truncate date to given granularity.
**
** Description:
**	This is the internal implemenation of the date_trunc() SQL/QUEL
**	function. 
**	
**      Definition of this function: 
**          adu_dtruncate(adf_scb, date_result, inter_spec, date_spec)
**        
**      This function returns a date value which represents the input date 
**      truncated to the level of granularity specified by the <inter_spec>
**
**      Example 1, 
**      adu_dtruncate(adf_scb, date_result, "month", date("23-oct-1985 12:33:00"))
**      would return the value 
**      date_result = date("01-oct-1985 00:00:00").
**
**      Example 2, 
**      adu_dtruncate(adf_scb, date_result, "year", date("23-oct-1985 12:33:00"))
**      would return the value
**      date_result = date("01-jan-1985 00:00:00"). 
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      inter_spec                       This can be one of the following :
**						year    yr
**						quarter qtr
**						month   mo
**						week    wk
**						day     
**						hour    hr
**						minute  min
**						second  sec 
**      date_spec                       Ptr to location at which the date
**					to be truncated can be found
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**      date_result                     Ptr to location at which to return
**					the truncated date
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**
**  Exceptions:
**      E_AD505D_DATEABS		error if absolute date specification
**					was not supplied.
**      E_AD505F_DATEINTERVAL		error if invalid interval spec
**					discovered.  
**
** History:
**	02-dec-85 (seputis)
**          initial creation
**	28-may-86 (ericj)
**	    Converted for Jupiter.
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	05-jul-87 (thurston)
**	    Added code that allows the input to be an `empty date'.  In this
**	    case, the routine just returns an empty date.  (This change was
**	    originally added to the 4.0 or 5.0 code.)
**	26-sep-89 (jrb)
**	    Changed minutes and hours truncation to first adjust to local time
**	    and then to normalize.  This is because not all GMT differentials
**	    are a multiple of 60 minutes.
**	25-sep-91 (stevet)
**	    Fixed B39998, date_trunc() problem with negative timezone setting.
**	    The internal date return in this case will have negative dn_time
**	    value, which can return incorrect result when applying truncation 
**	    formula for 'hour' and 'min' truncations.  By normalizing the 
**          date before applying truncation formula correct this problem.
**      29-aug-92 (stevet)
**          Modified timezone adjustment method by calling TMtz_search
**          to calculate correct timezone offset.
**	17-jul-2006 (gupsh01)
**	    Added support for new ANSI datetime data types.
**	01-aug-2006 (gupsh01)
**	    Adjust time value for nanosecond precision support.
**	09-aug-2006 (gupsh01)
**	    Fixed the date truncation to seconds.
**	12-nov-2007 (kibro01) b119452
**	    Blank out the result if we're returning a blank string.
*/

DB_STATUS
adu_dtruncate(
ADF_CB		    *adf_scb,
DB_DATA_VALUE       *inter_spec,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *date_result)
{
    char                dateclass;  /* token representing date interval */
    AD_NEWDTNTRNL       ndatev;
    register AD_NEWDTNTRNL  *datep;  /* ptr to working copy of date */
    DB_STATUS		db_stat;
    bool		tzadjust = TRUE;

    datep = &ndatev;
    MEfill ((u_i2) sizeof (ndatev), NULLCHAR, (PTR) &ndatev);
    datep = &ndatev;
    adu_6to_dtntrnl (date_spec, datep);

    /* For timezone for time and timestamp types
    ** don't adjust the timezone except for 
    ** the with local timezone type. 
    */
    if (( datep->dn_dttype == DB_TMW_TYPE ) ||
	( datep->dn_dttype == DB_TMWO_TYPE ) ||
	( datep->dn_dttype == DB_TSWO_TYPE ) ||
	( datep->dn_dttype == DB_TSW_TYPE ))
	  tzadjust = FALSE;

    if (db_stat = ad0_8getdateclass(adf_scb, inter_spec, &dateclass))
	return (db_stat);

    /* validate parameter to be an absolute date (or an `empty date') */

    if (datep->dn_status == AD_DN_NULL)
    {
	/* If the empty date, just set (b119452) and return an empty date */
	MEfill (date_result->db_length, NULLCHAR, date_result->db_data);
	return (E_DB_OK);
    }
    else if (((datep->dn_dttype == DB_DTE_TYPE) &&
		((datep->dn_status & AD_DN_ABSOLUTE) == 0))
        || (datep->dn_dttype == DB_INYM_TYPE)
        || (datep->dn_dttype == DB_INDS_TYPE))
    {
	/* ... otherwise, if not an absolute date, return an error */
	return (adu_error(adf_scb, (i4) E_AD505D_DATEABS, 0));
    }

    /* Date is an absolute date, process normally */

    /* process date interval parameter and obtain the corresponding token */
    switch (dateclass)
    {
      case 'S':
        /* truncate seconds */
        datep->dn_nsecond = 0;
        break;
      case 'I':
        /* truncate minutes */
        if (datep->dn_status & AD_DN_TIMESPEC)
        {
	    /*
	    ** Swap the following sequence, first normalize and then apply 
	    ** truncation formula.
	    */
	    adu_1normldate(datep);
	    datep->dn_seconds -= (datep->dn_seconds % AD_10DTE_ISECPERMIN);
        }
        break;
      case 'H':
        /* truncate hours */
        if (datep->dn_status & AD_DN_TIMESPEC)
        {
	    /*
	    ** Swap the following sequence, first normalize and then apply 
	    ** truncation formula.
	    */
	    adu_1normldate(datep);
	    datep->dn_seconds -= (datep->dn_seconds % AD_39DTE_ISECPERHOUR);
        }
        break;
      case 'D':
        /* truncate days */
        if (datep->dn_status & AD_DN_TIMESPEC)
        {
	    if (tzadjust)
	    {
	      datep->dn_seconds += TMtz_search(adf_scb->adf_tzcb, 
			      TM_TIMETYPE_GMT, 
			      adu_5time_of_date(datep));  
	      adu_1normldate(datep);
	    }
	    datep->dn_seconds = 0;
	    datep->dn_status &= ~AD_DN_TIMESPEC;
        }
        break;
      case 'M':
        /* truncate months */
        if (datep->dn_status & AD_DN_TIMESPEC)
        {
	    if (tzadjust)
	    {
	      datep->dn_seconds += TMtz_search(adf_scb->adf_tzcb, 
			      TM_TIMETYPE_GMT, 
			      adu_5time_of_date(datep));  
	      adu_1normldate(datep);
	    }
	    datep->dn_seconds = 0;
	    datep->dn_status &= ~AD_DN_TIMESPEC;
        }
        datep->dn_day = 1;
        break;
      case 'Y':
        /* truncate years */
        if (datep->dn_status & AD_DN_TIMESPEC)
        {
	    if (tzadjust)
	    {
	      datep->dn_seconds += TMtz_search(adf_scb->adf_tzcb, 
			      TM_TIMETYPE_GMT, 
			      adu_5time_of_date(datep));  
	      adu_1normldate(datep);
	    }
	    datep->dn_seconds = 0;
	    datep->dn_status &= ~AD_DN_TIMESPEC;
        }
        datep->dn_month = 1;
        datep->dn_day = 1;
        break;
      case 'Q':
        /* truncate quarters */
        if (datep->dn_status & AD_DN_TIMESPEC)
        {
	    if (tzadjust)
	      datep->dn_seconds += TMtz_search(adf_scb->adf_tzcb, 
			      TM_TIMETYPE_GMT, 
			      adu_5time_of_date(datep));  
	    adu_1normldate(datep);
	    datep->dn_seconds = 0;
	    datep->dn_status &= ~AD_DN_TIMESPEC;
        }
        datep->dn_month -= ((datep->dn_month-1) % AD_18DTE_IMONPERQUAR);
        datep->dn_day = 1;
        break;
      case 'W':
        /* truncate weeks */
        {
	    i4                daycount;           /* days from 1 A.D. */
	    i4                dayofweek;          /* day of week, monday=1*/

	    if (datep->dn_status & AD_DN_TIMESPEC)
	    {
	        if (tzadjust)
		  datep->dn_seconds += TMtz_search(adf_scb->adf_tzcb, 
				  TM_TIMETYPE_GMT, 
	                          adu_5time_of_date(datep));
		adu_1normldate(datep);
		datep->dn_seconds = 0;
		datep->dn_status &= ~AD_DN_TIMESPEC;
	    }
	    ad0_10togregorian(datep, &daycount, &dayofweek);
	    daycount = daycount - dayofweek + 1;
	    ad0_11fromgregorian(daycount, datep);
        }
        break;
      default:
        /* if class was not an interval then return an error */
        return (ad0_7errvaldsc(adf_scb, (i4) E_AD505F_DATEINTERVAL, inter_spec));
    }

#   ifdef xDEBUG
    print_date(datep);
#   endif
    adu_7from_dtntrnl (date_result, datep);
    return (E_DB_OK);
}


/*{
** Name: adu_dextract()   - extract component of date and return as integer.
**
** Description:
**	This is the internal implementation of the date_part() SQL/QUEL
**	function.
**	
**      Definition of function: 
**          adu_dextract(adf_scb, date_result, inter_spec, date_spec) 
**        
**      This function returns an integer which represents the extracted date
**      component from <date_spec> specified by the <inter_spec>.
**
**      Example 1, 
**	    adu_dextract(adf_scb, "month", date("23-oct-1985 12:33:00"))
**      would return the integer 10
**
**      Example 2, 
**	    adu_dextract(adf_scb, "day", date("23-oct-1985 12:33:00"))
**      would return the integer 23
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      inter_spec                       This can be one of the following :
**					    year    yr
**					    quarter qtr
**					    month   mo
**					    week    wk
**					    day     
**					    hour    hr
**					    minute  min
**					    second  sec 
**					    iso-week iso-wk
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**	    E_AD0000_OK			Completed successfully.
**
**	Exceptions:
**	    E_AD505D_DATEABS		error is absolute date specification
**					was not supplied.
**	    E_AD505F_DATEINTERVAL	error if unknown internal spec
**					discovered.  
**
** History:
**	02-dec-85 (seputis)
**          initial creation
**	28-may-86 (ericj)
**	    Converted for Jupiter.
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	14-aug-86 (thurston)
**	    At the end of this routine, after "component" was set, the code was
**	    incorrectly assigning it to the result DB_DATA_VALUE using an u_i2
**	    instead of an i2.  The following line:
**		    *((u_i2 *)result->db_data) = component;
**	    has been changed to:
**		    *((i2 *)result->db_data) = component;
**	    It just so happens, that for all possible values of "component"
**	    an i2 looks like an u_i2, so it probably worked fine.  However,
**	    the new line is "more" correct.
**	05-jul-87 (thurston)
**	    Added code that allows the input to be an `empty date'.  In this
**	    case, the routine just returns zero.  (This change was originally
**	    added to the 4.0 or 5.0 code.)  Also, this routine now allows the
**	    result to be an i1, i2, or i4; although the function instance table
**	    should limit this to i2's.
**      12-may-92 (stevet)
**          When extracting days, only looks at datep->dn_lowday because
**          for absolute dates, the datep->db_highday value is always 0.  
**          By looking at datep->db_highdy and assign it to an i2 caused
**          compiler warnings.
**      29-aug-92 (stevet)
**          Modified timezone adjustment method by calling TMtz_search
**          to calculate correct timezone offset.
**	07-Jun-1999 (shero03)
**	    Add ISO-6801 week.  The first week is the one that contains
**	    Jan 4th.  The week starts on Monday.
**	17-Jul-2006 (gupsh01)
**	    Added support for new ANSI datetime data type.
**	18-Jan-2007 (gupsh01)
**	    Added support for extracting timezone_hour and 
**	    timezont_minute.
*/

DB_STATUS
adu_dextract(
ADF_CB              *adf_scb,
DB_DATA_VALUE	    *inter_spec,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{
    char                dateclass;  /* token representing date interval */
    register AD_NEWDTNTRNL  *datep;
    AD_NEWDTNTRNL	dateval;
    i4			component;  /* contains extracted date component */
    DB_STATUS		db_stat;
    AD_NEWDTNTRNL       ndatev;

    MEfill((u_i2) sizeof(ndatev), NULLCHAR, (PTR) &ndatev);
    datep = &ndatev;
    adu_6to_dtntrnl (date_spec, datep);

    if (datep->dn_status == AD_DN_NULL)
    {
	/* If `empty date', just return zero */
	component = 0;
    }
    else if (((datep->dn_dttype == DB_DTE_TYPE) &&
	      (!(datep->dn_status & AD_DN_ABSOLUTE))
	     )
             || (datep->dn_dttype == DB_INYM_TYPE)
             || (datep->dn_dttype == DB_INDS_TYPE))
    {
	/* ... otherwise, if not an absolute date, return error */
	return (adu_error(adf_scb, (i4) E_AD505D_DATEABS, 0));
    }
    else
    {
	/* ... otherwise, process normally */

	/* 
	** Move date parameter to local storage so it can be normalized 
	** for the time zone.
	*/
	STRUCT_ASSIGN_MACRO(*datep, dateval);
	datep = &dateval;

	if (datep->dn_status & AD_DN_TIMESPEC)
	{
	    /*
	    ** If timespec is selected then the time is Greenwich mean time.
	    ** Normalize the time to adjust from Greenwich mean time.
	    */
	    if ((datep->dn_dttype != DB_TMWO_TYPE) && 
		(datep->dn_dttype != DB_TMW_TYPE) && 
		(datep->dn_dttype != DB_TSWO_TYPE) && 
		(datep->dn_dttype != DB_TSW_TYPE))
	    {	    
	      datep->dn_seconds += TMtz_search(adf_scb->adf_tzcb, 
			      TM_TIMETYPE_GMT, 
			      adu_5time_of_date(datep));  
	    }	    
	    adu_1normldate(datep);
	}

	if (db_stat = ad0_8getdateclass(adf_scb, inter_spec , &dateclass))
	    return (db_stat);

	/* select appropriate component to extract */
	switch (dateclass)
	{
	  case 'S':
	    /* extract seconds */
	    component = datep->dn_seconds % AD_10DTE_ISECPERMIN;
	    break;

	  case 'I':
	    /* extract minutes */
	    component= (datep->dn_seconds/AD_10DTE_ISECPERMIN) % AD_11DTE_IMINPERHOUR;
	    break;

	  case 'H':
	    /* extract hours */
	    component = (datep->dn_seconds/AD_11DTE_IMINPERHOUR/
						AD_10DTE_ISECPERMIN);
	    break;

	  case 'D':
	    /* extract days */
	    component = (datep->dn_day);
	    break;

	  case 'M':
	    /* extract months */
	    component = (datep->dn_month);
	    break;

	  case 'Y':
	    /* extract years */
	    component = (datep->dn_year);
	    break;

	  case 'Q':
	    /* extract quarters */
	    component = ((datep->dn_month-1) / AD_18DTE_IMONPERQUAR) + 1;
	    break;

	  case 'W':
	    {
		i4          daycount;       /* number of days from 1 A.D. */
		i4          dayofweek;      /* 1-7 day of week, monday=1*/
		i4	    daynumber;      /* 1-366 day number of year */

		/* find the current day of the week */
		ad0_10togregorian(datep, &daycount, &dayofweek);

		/* get the number of the day in the year 1 - 366 */
		ad0_9daysinyear(datep, &daynumber);

		/*
		** return 0 if the monday falls in the previous year
		** return the week number otherwise where monday begins the week
		*/
		component = (daynumber + AD_15DTE_IDAYPERWEEK - dayofweek)
				    / AD_15DTE_IDAYPERWEEK;
	    }
	    break;

	  case 'w':	/* ISO 6801 week */	
	    {
		i4      daycount;       /* number of days from 1 A.D. */
		i4      dayofweek;      /* 1-7 day of week, monday=1*/
		i4	daynumber;      /* 1-366 day number of year */
		i4	targdaycount;
		i4	targdayofweek;
                AD_NEWDTNTRNL ldate;	    /* temp work area */

		STRUCT_ASSIGN_MACRO(*datep, ldate);
		ldate.dn_month = 1;
		ldate.dn_day = 4;
		component = 0;
		
		/* find the day of the week for 1/4/yy */
		ad0_10togregorian(&ldate, &daycount, &dayofweek);
		
		/* Get the number of days from the 1st day of 1st week	*/
		daycount = daycount - dayofweek + 1;

		/* find the daycount for target */
		ad0_10togregorian(datep, &targdaycount, &targdayofweek);
		
		/* See if target date is in the previous year's weeks */
		if ((targdaycount < daycount) &&
		    (dayofweek < 4) )
		{	
		  ldate.dn_year--;	/* day falls in previous year */
		  /* find the day of the week for 1/4/yy-- */
		  ad0_10togregorian(&ldate, &daycount, &dayofweek);

		  /* find number of days from the 1st day in 1st week */
		  daycount = daycount - dayofweek + 1;
		}
		/* See if the target date is in next year's weeks */
		else if ((datep->dn_month == 12) &&
			 (datep->dn_day > 28))
		{
		   if ((datep->dn_day == 31) &&
		       (targdayofweek < 4))
		     component = 1;
		   else if ((datep->dn_day == 30) &&
		       (targdayofweek < 3))
		     component = 1;
		   else if ((datep->dn_day == 29) &&
		       (targdayofweek < 2))
		     component = 1;
		}
		if (component == 0)
		{		      
		  /* Get the number of days from the first day	*/
		  /* of the first week to target relative to 1	*/
		  daynumber = targdaycount - daycount;

		  component = daynumber / AD_15DTE_IDAYPERWEEK;
		  component++;	/* make it relative to 1 */
		}
	    }
	    break;

	  case 'x':
	    /* extract microsecoonds */
	    component = datep->dn_nsecond / 1000;
	    break;

	  case 'X':
	    /* extract nanosecoonds */
	    component = datep->dn_nsecond;
	    break;

	  case 'R':
	    /* extract timezone_hour */
	    component = datep->dn_tzhour;
	    break;

	  case 'T':
	    /* extract timezone_minute */
	    component = datep->dn_tzminute;
	    break;

	  default:
	    /* invalid interval specification supplied */
	    return (ad0_7errvaldsc(adf_scb, (i4) E_AD505F_DATEINTERVAL, inter_spec));
	}
    }


    switch (result->db_length)
    {
      case 1:
	*((i1 *)result->db_data) = component;
	break;
      case 2:
	*((i2 *)result->db_data) = component;
	break;
      case 4:
	*((i4 *)result->db_data) = component;
	break;
      default:
	return (adu_error(adf_scb, (i4) E_AD9999_INTERNAL_ERROR, 0));
    }

    return (E_DB_OK);
}


/*{
** Name: adu_year_part() - stub to drive adu_dextract() to extract 'year'
**
** Description:
**	This is the function instance called for the year() function. It 
**	simply redirects the call to adu_dextract() with "year" as
**	the unit to extract.
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
** History:
**	18-oct-2006 (dougi)
**	    Written to support date part extraction functions.
*/

DB_STATUS
adu_year_part(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{

    /* This is a simple matter of redirecting the call with "inter_spec" 
    ** chosen from constant values. */
    return(adu_dextract(adf_scb, &year_spec, date_spec, result));
}


/*{
** Name: adu_quarter_part() - stub to drive adu_dextract() to extract 'quarter'
**
** Description:
**	This is the function instance called for the quarter() function. It 
**	simply redirects the call to adu_dextract() with "quarter" as
**	the unit to extract.
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
** History:
**	18-oct-2006 (dougi)
**	    Written to support date part extraction functions.
*/

DB_STATUS
adu_quarter_part(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{

    /* This is a simple matter of redirecting the call with "inter_spec" 
    ** chosen from constant values. */
    return(adu_dextract(adf_scb, &quarter_spec, date_spec, result));
}


/*{
** Name: adu_month_part() - stub to drive adu_dextract() to extract 'month'
**
** Description:
**	This is the function instance called for the month() function. It 
**	simply redirects the call to adu_dextract() with "month" as
**	the unit to extract.
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
** History:
**	18-oct-2006 (dougi)
**	    Written to support date part extraction functions.
*/

DB_STATUS
adu_month_part(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{

    /* This is a simple matter of redirecting the call with "inter_spec" 
    ** chosen from constant values. */
    return(adu_dextract(adf_scb, &month_spec, date_spec, result));
}


/*{
** Name: adu_week_part() - stub to drive adu_dextract() to extract 'week'
**
** Description:
**	This is the function instance called for the week() function. It 
**	simply redirects the call to adu_dextract() with "week" as
**	the unit to extract.
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
** History:
**	18-oct-2006 (dougi)
**	    Written to support date part extraction functions.
*/

DB_STATUS
adu_week_part(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{

    /* This is a simple matter of redirecting the call with "inter_spec" 
    ** chosen from constant values. */
    return(adu_dextract(adf_scb, &week_spec, date_spec, result));
}


/*{
** Name: adu_weekiso_part() - stub to drive adu_dextract() to extract 'weekiso'
**
** Description:
**	This is the function instance called for the week_iso() function. It 
**	simply redirects the call to adu_dextract() with "iso-week" as
**	the unit to extract.
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
** History:
**	18-oct-2006 (dougi)
**	    Written to support date part extraction functions.
*/

DB_STATUS
adu_weekiso_part(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{

    /* This is a simple matter of redirecting the call with "inter_spec" 
    ** chosen from constant values. */
    return(adu_dextract(adf_scb, &weekiso_spec, date_spec, result));
}


/*{
** Name: adu_day_part() - stub to drive adu_dextract() to extract 'day'
**
** Description:
**	This is the function instance called for the day() function. It 
**	simply redirects the call to adu_dextract() with "day" as
**	the unit to extract.
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
** History:
**	18-oct-2006 (dougi)
**	    Written to support date part extraction functions.
*/

DB_STATUS
adu_day_part(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{

    /* This is a simple matter of redirecting the call with "inter_spec" 
    ** chosen from constant values. */
    return(adu_dextract(adf_scb, &day_spec, date_spec, result));
}


/*{
** Name: adu_hour_part() - stub to drive adu_dextract() to extract 'hour'
**
** Description:
**	This is the function instance called for the hour() function. It 
**	simply redirects the call to adu_dextract() with "hour" as
**	the unit to extract.
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
** History:
**	18-oct-2006 (dougi)
**	    Written to support date part extraction functions.
*/

DB_STATUS
adu_hour_part(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{

    /* This is a simple matter of redirecting the call with "inter_spec" 
    ** chosen from constant values. */
    return(adu_dextract(adf_scb, &hour_spec, date_spec, result));
}


/*{
** Name: adu_minute_part() - stub to drive adu_dextract() to extract 'minute'
**
** Description:
**	This is the function instance called for the minute() function. It 
**	simply redirects the call to adu_dextract() with "minute" as
**	the unit to extract.
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
** History:
**	18-oct-2006 (dougi)
**	    Written to support date part extraction functions.
*/

DB_STATUS
adu_minute_part(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{

    /* This is a simple matter of redirecting the call with "inter_spec" 
    ** chosen from constant values. */
    return(adu_dextract(adf_scb, &minute_spec, date_spec, result));
}


/*{
** Name: adu_second_part() - stub to drive adu_dextract() to extract 'second'
**
** Description:
**	This is the function instance called for the second() function. It 
**	simply redirects the call to adu_dextract() with "second" as
**	the unit to extract.
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
** History:
**	18-oct-2006 (dougi)
**	    Written to support date part extraction functions.
*/

DB_STATUS
adu_second_part(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{

    /* This is a simple matter of redirecting the call with "inter_spec" 
    ** chosen from constant values. */
    return(adu_dextract(adf_scb, &second_spec, date_spec, result));
}



/*{
** Name: adu_microsecond_part() - stub to drive adu_dextract() to extract 
**	'microsecond'
**
** Description:
**	This is the function instance called for the microsecond() function. It 
**	simply redirects the call to adu_dextract() with "microsecond" as
**	the unit to extract.
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
** History:
**	18-oct-2006 (dougi)
**	    Written to support date part extraction functions.
*/

DB_STATUS
adu_microsecond_part(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{

    /* This is a simple matter of redirecting the call with "inter_spec" 
    ** chosen from constant values. */
    return(adu_dextract(adf_scb, &microsecond_spec, date_spec, result));
}



/*{
** Name: adu_nanosecond_part() - stub to drive adu_dextract() to extract 
**	'nanosecond'
**
** Description:
**	This is the function instance called for the nanosecond() function. It 
**	simply redirects the call to adu_dextract() with "nanosecond" as
**	the unit to extract.
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
** History:
**	18-oct-2006 (dougi)
**	    Written to support date part extraction functions.
*/

DB_STATUS
adu_nanosecond_part(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{

    /* This is a simple matter of redirecting the call with "inter_spec" 
    ** chosen from constant values. */
    return(adu_dextract(adf_scb, &nanosecond_spec, date_spec, result));
}

/*{
** Name: adu_tzhour_part() - stub to drive adu_dextract() to extract 
**	'timezone_hour'
**
** Description:
**	This is the function instance called for extracting the timezone_hour
**	by extract syntax. It simply redirects the call to adu_dextract() with 
**	"timezone_hour" as the unit to extract.
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
** History:
**	16-jan-2007 (dougi)
**	    Written to support timezone hour part extraction functions.
*/

DB_STATUS
adu_tzhour_part(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{

    /* This is a simple matter of redirecting the call with "inter_spec" 
    ** chosen from constant values. */
    return(adu_dextract(adf_scb, &tzhour_spec, date_spec, result));
}
/*{
** Name: adu_tzminut_part() - stub to drive adu_dextract() to extract 
**	'timezone_minute'
**
** Description:
**	This is the function instance called for extracting the timezone_hour
**	by extract syntax. It simply redirects the call to adu_dextract() with 
**	"timezone_hour" as the unit to extract.
**        
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**      date_spec                       Ptr to location at which the date
**					from which the component will be
**					extracted
**
** Outputs:
**      result                          Ptr to location at which to return
**					the integer value of the component
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
** History:
**	16-jan-2007 (dougi)
**	    Written to support timezone hour part extraction functions.
*/

DB_STATUS
adu_tzminute_part(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date_spec,
DB_DATA_VALUE       *result)
{

    /* This is a simple matter of redirecting the call with "inter_spec" 
    ** chosen from constant values. */
    return(adu_dextract(adf_scb, &tzminute_spec, date_spec, result));
}

/*
**  Name:   ad0_prelim_norml() - Internal routine used to do preliminary
**                               normalization of time field of interval date.
**
**  Description:
**	This normalization is done before the time is converted to
**	milliseconds for internal storage.
**
**  History:
**	16-aug-89 (jrb)
**	    This routine was removed for date interval work, but I am
**	    reinstating it (it was a mistake to take it out).  I re-wrote this
**	    routine because it sometimes took 20 minutes to run (this is
**	    considered slow).
**	24-jan-90 (jrb)
**	    Bug 9859.  When a field went to zero I was just resetting its
**	    status bit, but some callers look at the field regardless of the
**	    status bits, so the fields should be zeroed as well.
**	14-dec-90 (jrb)
**	    No Bug Number.  Shouldn't reset status bits when a field goes to
**	    zero because regular normalization routine needs to know which
**	    fields were explicitly mentioned when the interval was entered.
**      04-jan-1993 (stevet)
**          Renamed from adu_prelim_norml to ad0_prelim_norml and moved here
**          from adulldates.c file.
**	17-jul-2006 (gupsh01)
**	    Rename DV_MILLISECOND to DV_NANOSECOND in anticipation of nano-
**	    second support for new ANSI datetime types.
**	01-aug-2006 (gupsh01)
**	    Added nanosecond calculations.
*/
static VOID
ad0_prelim_norml(
DATEVECT    *dv)
{
    register i4		stat;
    register i4		acc;
    i4			i;
    i4			nsecs = 0;
    i4			secs = 0;
    i4			mins = 0;
    i4			hours = 0;
    i4			days = 0;
        
    stat = dv->dv_status;

#   ifdef xDEBUG
    if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_006_NORMDATE, &dum1, &dum2))
        TRdisplay("prelim_norml: at start, dv->dv_status=%d\n", dv->dv_status);
#   endif
    
    if (!(stat & DV_NANOSECOND))
        dv->dv_nanosecond = 0;

    if (!(stat & DV_SECOND))
        dv->dv_second = 0;

    if (!(stat & DV_MINUTE))
        dv->dv_minute = 0;

    if (!(stat & DV_HOUR))
        dv->dv_hour = 0;

    if (!(stat & DV_DAY))
        dv->dv_day = 0;
    else
	days = dv->dv_day;

#   ifdef xDEBUG
    if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_006_NORMDATE, &dum1, &dum2))
    {
        TRdisplay("prelim_norml: dv->dv_day=%d\n", dv->dv_day);
        TRdisplay("prelim_norml: dv->dv_hour=%d\n", dv->dv_hour);
    }
#   endif
     

    for (i=0; i < 4; i++)
    {
	switch (i)
	{
	  case 0:
	    acc = dv->dv_nanosecond;
	    break;
	  case 1:
	    acc = dv->dv_second;
	    break;
	  case 2:
	    acc = dv->dv_minute;
	    break;
	  case 3:
	    acc = dv->dv_hour;
	    break;
	}

	/* we allow flow to fall through without breaking in the following
	** switch statement in some cases
	*/
	switch (i)
	{
	  case 0:
	    nsecs += acc;
		break;
	  case 1:
	    secs += acc % 60;
	    acc /= 60;
	    if (acc == 0)
		break;
	  case 2:
	    mins += acc % 60;
	    acc /= 60;
	    if (acc == 0)
		break;
	  case 3:
	    hours += acc % 24;
	    acc /= 24;
	    days += acc;
	}
    }
	

    /* set statuses and update vector's values */
    if (days != 0)
        dv->dv_status |= DV_DAY;

    if (hours != 0)
        dv->dv_status |= DV_HOUR;

    if (mins != 0)
        dv->dv_status |= DV_MINUTE;

    if (secs != 0)
        dv->dv_status |= DV_SECOND;

    if (nsecs != 0)
        dv->dv_status |= DV_NANOSECOND;

    dv->dv_day = days;
    dv->dv_hour = hours;
    dv->dv_minute = mins;
    dv->dv_second = secs;
    dv->dv_nanosecond = nsecs;

#   ifdef xDEBUG
    if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_006_NORMDATE, &dum1, &dum2))
    {
        TRdisplay("prelim_norml: after normalizing: status=%d\n",dv->dv_status);
        TRdisplay("prelim_norml: dv->dv_day=%d\n", dv->dv_day);
        TRdisplay("prelim_norml: dv->dv_hour=%d\n", dv->dv_hour);
    }
#   endif

}

/*
** Name: adu_hrtimetodate - convert HRSYSTIME to Ingres internal date
**
** Description:
**	Converts a HRSYSTIME structure to an Ingres Internal date format. Written
**	primarily so that we can obtain an Ingres internal date acurate to
**	the nearest milisecond, which is supported by the structure but not
**	currently used.
**	
**	Notes: Both HRSYSTIME and Ingres internal AD_DATENTRNL are expected
**	to be an absolute time relative to GMT. AD_DATENTRNL is currently stored
**	this way and converted in retrieval, HRSYSTIME should be in this format
**	when returned from TMhrnow(). This means than no timezone conversion
**	is required.
**	The returned error will always be OK or E_AD5063_BAD_SYSTIME, but the
**	dn_status bits are updated as the date is filled in, so the caller
**	can tell where the conversion failed.
**
** Inputs:
**	time	- HRSYSTIME (in secs and nanosecs since 1-jan-1970)
**	date	- Internal date to initialize
**
** Outputs:
**	date	- Initialized internal date
**
** Returns:
**	status
**
** History:
**	december-98 (stephenb)
**	    Initial creation
**	19-jan-99 (stephenb)
**	    Correct range for month
**	2-apr-99 (stephenb)
**	    Correct range for day of month
**	01-aug-06 (gupsh01)
**	    supply nanosecond parameter in adu_cvtime().
**	07-nov-2006 (gupsh01)
**	    Fixed seconds calculations.
**	3-Jul-2007 (kibro01) b118586
**	    Undo above two changes related to ansidate (apart from passing
**	    new new time->tv_nsec parameter to adu_cvtime) given that 
**	    they always blank out the day field (leaving us always in the
**	    last day of last month), and the time field is always a few
**	    minutes after midnight due to being 1000-times too low.  This
**	    function is only used by replicator, so showed up doing simple
**	    replication and looking in the dd_distrib_queue.
*/
DB_STATUS
adu_hrtimetodate(
	     HRSYSTIME		*time,
	     AD_DATENTRNL	*date,
	     i4		*error)
{
    DB_STATUS		status;
    struct timevect	tv;

    adu_cvtime(time->tv_sec, time->tv_nsec, &tv);

    date->dn_status = AD_DN_ABSOLUTE;

    /* year */
    /* time vector is in years since 1900 */
    date->dn_year = tv.tm_year + AD_TV_NORMYR_BASE;
    if (date->dn_year < 1 || date->dn_year > 9999)
    {
	*error = E_AD5063_BAD_SYSTIME;
	return(E_DB_ERROR);
    }
    date->dn_status |= AD_DN_YEARSPEC;

    /* month */
    if (tv.tm_mon >= 0 && tv.tm_mon <= 11)
    {
	date->dn_month = tv.tm_mon + AD_TV_NORMMON;
	date->dn_status |= AD_DN_MONTHSPEC;
    }
    else
    {
	*error = E_AD5063_BAD_SYSTIME;
	return(E_DB_ERROR);
    }

    /* day */
    if (tv.tm_mday >= 1 && 
	tv.tm_mday <= adu_2monthsize((i4) date->dn_month,(i4) date->dn_year))
    {
	date->dn_lowday = tv.tm_mday;
	date->dn_status |= AD_DN_DAYSPEC;
    }
    else
    {
	*error = E_AD5063_BAD_SYSTIME;
	return(E_DB_ERROR);
    }
    date->dn_highday = 0;
    
    /* time */
    if (tv.tm_hour >= 0 && tv.tm_hour < 24 && tv.tm_min >= 0 && tv.tm_min < 60 &&
	tv.tm_sec >= 0 && tv.tm_sec < 60 && 
	time->tv_nsec >= 0 && time->tv_nsec <= AD_33DTE_MAX_NSEC)
    {
	date->dn_time = tv.tm_hour * AD_8DTE_IMSPERHOUR +
	    tv.tm_min * AD_7DTE_IMSPERMIN +
	    tv.tm_sec * AD_6DTE_IMSPERSEC +
	    (time->tv_nsec / AD_29DTE_NSPERMS);
	date->dn_status |= AD_DN_TIMESPEC;
    }
    else
    {
	*error = E_AD5063_BAD_SYSTIME;
	return(E_DB_ERROR);
    }

    return (E_DB_OK);
}


/*
** Name: adu_date_isdst -	determine if date falls within daylight 
**	savings time.
**
** Description:
**
** Inputs:
**	date	- date value to test.
**
** Outputs:
**	result	- returned integer value - 1 if date is an absolute date
**		and falls within users daylight savings time, else 0.
**
** Returns:
**	status
**
** History:
**	13-apr-06 (dougi)
**	    Written.
**	17-jul-2006 (gupsh01)
**	    Added support for new ANSI datetime types.
*/
DB_STATUS
adu_date_isdst(
ADF_CB              *adf_scb,
DB_DATA_VALUE       *date,
DB_DATA_VALUE       *result)

{
    AD_NEWDTNTRNL	nd;
    AD_NEWDTNTRNL	*d;
    i4			datesecs, retval;
    DB_STATUS		status;

    MEfill((u_i2) sizeof(nd), NULLCHAR, (PTR) &nd);
    d = &nd;
    adu_6to_dtntrnl (date, d);

    /* Check for absolute date (anything else returns 0), convert it
    ** to seconds since 1/1/1970. */
    if ((d->dn_dttype == DB_DTE_TYPE) &&(!(d->dn_status & AD_DN_ABSOLUTE))
	|| (d->dn_dttype == DB_INYM_TYPE) 
	|| (d->dn_dttype == DB_INDS_TYPE) 
	)
	retval = 0;
    else
    {
	datesecs = adu_5time_of_date(d);
	if (datesecs == MAXI4 || datesecs == MINI4)
	    retval = 0;
	else retval = TMtz_isdst(adf_scb->adf_tzcb, datesecs);
    }

    *(i4 *)(result->db_data) = retval;

    return(E_DB_OK);

}

/*
** Name: print_date -	Print the new datetime internal struct.
**
** Description:
**
** Inputs:
**	dn	- Printing AD_NEWDTNTRNL structure. 
**
** History:
**    17-jun-2006 (gupsh01)
**	Added.
*/
static VOID 
print_date(AD_NEWDTNTRNL *dn)
{
   TRdisplay("Date is: year = %d, month = %d, day = %d, ",
            dn->dn_year, dn->dn_month, dn->dn_day);
   TRdisplay("time = %d.%09d\n", dn->dn_seconds, dn->dn_nsecond);
   TRdisplay("status = ");
   if (dn->dn_status & AD_DN_NULL)
    	TRdisplay(" AD_DN_NULL,  ");
   if (dn->dn_status & AD_DN_ABSOLUTE)
    	TRdisplay(" AD_DN_ABSOLUTE,  ");
   if (dn->dn_status & AD_DN_LENGTH)
    	TRdisplay(" AD_DN_LENGTH,  ");
   if (dn->dn_status & AD_DN_YEARSPEC)
    	TRdisplay(" AD_DN_YEARSPEC,  ");
   if (dn->dn_status & AD_DN_MONTHSPEC)
    	TRdisplay(" AD_DN_MONTHSPEC,  ");
   if (dn->dn_status & AD_DN_DAYSPEC)
    	TRdisplay(" AD_DN_DAYSPEC,  ");
   if (dn->dn_status & AD_DN_TIMESPEC)
    	TRdisplay(" AD_DN_TIMESPEC,  ");
   if (dn->dn_status & AD_DN_AFTER_EPOCH)
    	TRdisplay(" AD_DN_AFTER_EPOCH,  ");
   if (dn->dn_status & AD_DN_BEFORE_EPOCH)
    	TRdisplay(" AD_DN_BEFORE_EPOCH");
   TRdisplay("\n");
}

/* Format Legend For ANSI datetime template.
**  y = year, x =-y,
**  m = month, q=-m (dash m), d = day,  e = -d,
**  h = hour, j = -h, i = minute, k = -i
**  s = second p = -s
**  b = hr component of time zone, c = -b
**  v = minute component of time zone,
**  l = second fraction precision, g = -l
*/
GLOBALCONSTDEF  ANSIDATETMPLT   Adu_ansidtfmt[] =
{ 
    {DB_ADTE_TYPE, "n-n-n", "ymd"},    
    {DB_TSWO_TYPE, "n-n-n n:n:n", "ymdhis"},
    {DB_TSWO_TYPE, "n-n-n n:n:n.n", "ymdhisl"},
	/* yyyy-mm-dd  hh:mm:ss.sss... +/-hh:ss */
    {DB_TSW_TYPE,  "n-n-n n:n:n-n:n", "ymdhiscv"}, 
    {DB_TSW_TYPE,  "n-n-n n:n:n.n-n:n", "ymdhislcv"}, 
    {DB_TSW_TYPE,  "n-n-n n:n:n.n+n:n", "ymdhislbv"}, 
    {DB_TSW_TYPE,  "n-n-n n:n:n+n:n", "ymdhisbv"}, 
    {DB_TSW_TYPE,  "n-n-n n:n:n-n:n", "ymdhiscv"}, 
    {DB_TMWO_TYPE, "n:n:n", "his"},   
    {DB_TMWO_TYPE, "n:n:n.n", "hisl"},   
	/* hh:mm:ss.sss... +/-hh:ss */
    {DB_TMW_TYPE,  "n:n:n+n:n", "hisbv"},  
    {DB_TMW_TYPE,  "n:n:n.n+n:n", "hislbv"},  
    {DB_TMW_TYPE,  "n:n:n-n:n", "hiscv"},  
    {DB_TMW_TYPE,  "n:n:n.n-n:n", "hislcv"},  
    {DB_INYM_TYPE, "n", "y"},	   
    {DB_INYM_TYPE, "n-n", "yq"},	  
    {DB_INYM_TYPE, "+n", "y"},	   
    {DB_INYM_TYPE, "+n-n", "yq"},	  
    {DB_INYM_TYPE, "-n",  "x"},		
    {DB_INYM_TYPE, "-n-n", "xq"},	    
    {DB_INDS_TYPE, "n n", "dh"},
    {DB_INDS_TYPE, "n n:n", "dhi"},
    {DB_INDS_TYPE, "n n:n:n", "dhis"},
    {DB_INDS_TYPE, "n n:n:n.n", "dhisl"},
    {DB_INDS_TYPE, "+n n", "dh"},
    {DB_INDS_TYPE, "+n n:n", "dhi"},
    {DB_INDS_TYPE, "+n n:n:n", "dhis"},
    {DB_INDS_TYPE, "+n n:n:n.n", "dhisl"},
    {DB_INDS_TYPE, "-n n", "eh"},
    {DB_INDS_TYPE, "-n n:n", "ehi"},
    {DB_INDS_TYPE, "-n n:n:n", "ehis"},
    {DB_INDS_TYPE, "-n n:n:n.n", "ehisl"},  
    0
};

/*{
** Name: adu_21ansi_strtodt - converts a valid text input to internal 
**	storage format for ANSI datetime data type
**
** Description:
**        This routine converts a valid string into an internal format.
**	The valid date string input formats and their interpretation are
**	determined by the global read-only ANSI date template Adu_ansidtfmt
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**	    .adf_dfmt			A DB_DATE_FMT describing which template
**					array should be used in the inter-
**					pretation of the date string.
**	str_dv				DB_DATA_VALUE describing the string.
**	    .db_datatype		Its datatype.
**	    .db_length			Its length.
**	    .db_data			If the datatype is character, this is
**					a pointer to the string to be converted.
**					If the datatype is text, this is a ptr
**					to a DB_TEXT_STRING.
**		[.db_t_count]		The number of character in a
**					DB_TEXT_STRING string.
**		[.db_t_text]		Ptr to the "text" string to be
**					converted.
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**      date_dv                         DB_DATA_VALUE describing the internal
**					date.
**	    .db_data			Ptr to AD_NEWDTNTRNL struct.
[@PARAM_DESCR@]...
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**	06-sep-2006 (gupsh01)
**	    Trim the spec buffer before lookup. This will fix the 
**	    coercion of char data columns to ANSI date/time.
**
** History:
**    29-aug-2006 (gupsh01)
**	Added.
**    02-feb-2006 (gupsh01)
**	Truncate the scanned digits if the number scanned exceeds 
**	9 digits.
*/
DB_STATUS
adu_21ansi_strtodt(
ADF_CB          *adf_scb,
DB_DATA_VALUE   *str_dv,
DB_DATA_VALUE   *date_dv)
{
    i4		    in_len;    /* length of the input string */
    bool	    isempty;
    DB_STATUS       db_stat;
    register char   *datespec;
    char	    specbuf[64];
    i4		    d_val_array[16];
    i4		    d_val_size[16];
    i4		    *d_valp;
    i4		    *d_sizp;
    DATEVECT	    dv;
    AD_NEWDTNTRNL    dn;
    AD_NEWDTNTRNL   ndn;
    bool	    time = FALSE;
    i4		    now;
    register char   *p;
    char   	    *ptemp;
    char 	    *dbuf;
    char	    localbuf[2004];
    bool	    uselocall = TRUE;
    DB_DATA_VALUE   str2_dv;
    const ANSIDATETMPLT   *tmptab;
    i4		    adf_errcode = E_AD5052_DATEVALID;
    bool            fractional = FALSE;

    /* Zero fill tables used in parsing */
    MEfill(sizeof(dv), NULLCHAR, (PTR) &dv);
    MEfill(sizeof(dn), NULLCHAR, (PTR) &dn);

    if (str_dv->db_datatype == DB_NCHR_TYPE
	|| str_dv->db_datatype == DB_NVCHR_TYPE)
    {
	bool	local = TRUE;

	if (str_dv->db_datatype == DB_NVCHR_TYPE)
	    str2_dv.db_length = (str_dv->db_length-DB_CNTSIZE)/sizeof(UCS2)+
				DB_CNTSIZE;
	else str2_dv.db_length = str_dv->db_length/sizeof(UCS2);
	/* See if "localbuf" is big enough. */
	if (str2_dv.db_length >= 2004)
	{
	    str2_dv.db_data = (char *)MEreqmem(0, str2_dv.db_length, 
					FALSE, NULL);
	    local = FALSE;
	}
	else str2_dv.db_data = (char *)&localbuf;

	MEfill(str2_dv.db_length, NULLCHAR, (PTR) str2_dv.db_data);
	if (str_dv->db_datatype == DB_NCHR_TYPE)
			str2_dv.db_datatype = DB_CHA_TYPE;	
	else if (str_dv->db_datatype == DB_NVCHR_TYPE)
			str2_dv.db_datatype = DB_VCH_TYPE;	
	str2_dv.db_prec = str_dv->db_prec;
	adu_nvchr_coerce(adf_scb,  str_dv, &str2_dv);
	MEcopy(str2_dv.db_data, str2_dv.db_length, str_dv->db_data);
	str_dv->db_length = str2_dv.db_length;
	str_dv->db_datatype = str2_dv.db_datatype;
	str_dv->db_prec = str2_dv.db_prec;

	/* localbuf wasn't big enough - free what we allocated. */
	if (!local)
	    MEfree((PTR)str2_dv.db_data);
    }
    else if ((str_dv->db_datatype == DB_DTE_TYPE) ||
             (str_dv->db_datatype == DB_ADTE_TYPE) ||
             (str_dv->db_datatype == DB_TMWO_TYPE) ||
             (str_dv->db_datatype == DB_TMW_TYPE) ||
             (str_dv->db_datatype == DB_TME_TYPE) ||
             (str_dv->db_datatype == DB_TSWO_TYPE) ||
             (str_dv->db_datatype == DB_TSW_TYPE) ||
             (str_dv->db_datatype == DB_TSTMP_TYPE) ||
             (str_dv->db_datatype == DB_INDS_TYPE) ||
             (str_dv->db_datatype == DB_INYM_TYPE))
    {
	/* ADD check that intervals cannot be converted to dates and
	   dates cannot be converted to intervals */

     	return (adu_2datetodate (adf_scb, str_dv, date_dv));
    }
    /* Apart from this only string types are permitted checked in 
    ** adu_3straddr();	
    */

    /*
    ** Parse an input string into a legal date.  We go through the string
    ** creating an array of values (d_val_array) along with the sizes of the
    ** section of string which generated them (d_val_size), and a template
    ** (datespec) which is used to look up a format that controls
    ** interpretation of the values we find.
    */

    if ((db_stat = adu_3straddr(adf_scb, str_dv, &ptemp)) != E_DB_OK)
	return (db_stat);
    if ((db_stat = adu_5strcount(adf_scb, str_dv, &in_len)) != E_DB_OK)
	return (db_stat);

    switch (date_dv->db_datatype)
    {
      case DB_ADTE_TYPE:
	adf_errcode = E_AD5070_ANSIDATE_BADFMT;
	break;
      case DB_TMWO_TYPE:
        adf_errcode = E_AD5071_ANSITMWO_BADFMT;
	break;
      case DB_TMW_TYPE:
      case DB_TME_TYPE:
        adf_errcode = E_AD5072_ANSITMW_BADFMT;
	break;
      case DB_TSWO_TYPE:
        adf_errcode = E_AD5073_ANSITSWO_BADFMT;
	break;
      case DB_TSW_TYPE:
      case DB_TSTMP_TYPE:
        adf_errcode = E_AD5074_ANSITSW_BADFMT;
	break;
      case DB_INYM_TYPE:
        adf_errcode = E_AD5075_ANSIINYM_BADFMT;
	break;
      case DB_INDS_TYPE:
        adf_errcode = E_AD5076_ANSIINDS_BADFMT;
	break;
      default:
        adf_errcode = E_AD5052_DATEVALID;
	break;
    }
    d_valp = d_val_array;
    d_sizp = d_val_size;
    now	= 0;
    datespec = specbuf;

    isempty = TRUE;

    /* Initialize the Date vector */
    if ((date_dv->db_datatype != DB_INYM_TYPE) &&
        (date_dv->db_datatype != DB_INDS_TYPE))
    	dv.dv_datetype = DV_ABSOLUTE;
    else
    	dv.dv_datetype = DV_LENGTH;

    /* Zero-terminated input string. */
    if (in_len >= 2004)
    {
     /* FIX ME - This should already raise a red flag and should return
     ** Error - Input string is too long for a valid ANSI date
     */
     /* Call MEreqmem to avoid blowing call stack with local allocation. */

	dbuf = (char *)MEreqmem(0, DB_GW4_MAXSTRING + 1, FALSE, NULL);
	uselocall = FALSE;
    }
    else dbuf = &localbuf[0];

    STncpy(dbuf, ptemp, in_len);
    dbuf[ in_len ] = EOS;

    /*
    ** Parse the input string building a date specification string and
    ** putting the values on a queue to be interpreted later.
    */
    p = dbuf;
    while (*p != EOS) 
    {
	i4 value;
	i4 valcnt;

        switch(*p)
        {
        default:
	    if (CMdigit(p))
	    {
		/* this is the start of a number, only integers
		** are allowed in dates so scan to first non-digit 
		*/
		char	    *oldp = p;
    
		valcnt = 1;
		value = 0;
		do
		{
		    if (fractional)
		    {
			/* NOTE: that the value for fractional is pre-scaled
			** for nano-seconds precision and that the excess digits
			** are passed in the string but have been ignored. No
			** attempt is made to round these up
			*/
			if (valcnt < 10)
			    value += (*p - '0')*nano_mult[valcnt];
		    }
		    else
		    {
			/* check for overflow (a little on the safe side) */
			if (value > MAXI4/10-1)
			{
			    db_stat = adu_error(adf_scb, adf_errcode, 2, (i4) 0, (i4 *) dbuf);
			    if (!uselocall)
				MEfree((PTR)dbuf);
			    return (db_stat);
			}
			value = value * 10 + (*p - '0');
		    }
		    valcnt++;
		    p++;
		} while (CMdigit(p));

		/* Add to value list and to template. */
		*d_valp++ = value;
		*d_sizp++ = p - oldp;
		*datespec++     = 'n';

		isempty = FALSE;
		fractional = FALSE;
	    }
	    else if (CMwhite(p))
	    {
		CMnext(p);
		*datespec++ = ' ';
	    }
	    else
	    {
		db_stat = adu_error(adf_scb, adf_errcode, 2, (i4) 0, (i4 *) dbuf);
		return (db_stat);
	    }
	    break;

	case ':':
	    /* This is a time delimiter. */
	    time = TRUE;
	    isempty = FALSE;
	    *datespec++ = *p++;
	    break;

	case '.':
	    /* If in time part treat '.' as fractional introducer */
	    fractional = time;
	    /* FALLTHROUGH */
	case '-':
	case '+':
	    /* These are delimiters, place them in the string directly */
	    isempty = FALSE;
	    /* FALLTHROUGH */

	case '/':
	    /* This is a delimiter, legal in an empty date. */
	    *datespec++ = *p++;
	    break;
	}
    }

    /* Trim the date spec before terminating */
    if ((datespec > specbuf) && (*(datespec - 1 ) == ' '))
    {
      datespec--;
      while ((datespec > specbuf) && (*datespec == ' '))
        datespec--;

     /* Advance by one space */
      if ((datespec >= specbuf) && (*datespec != ' '))
        datespec++;
    }
    *datespec = EOS;


    /* By now we know whether the date is empty. */
    if (isempty)
    {
	/*
	** An empty dates can have only whitespaces. 
	** We assume it's an empty date.
	*/
        dn.dn_status    = AD_DN_NULL;
        dn.dn_year	= 0;
        dn.dn_month	= 0;
        dn.dn_day	= 0;
        dn.dn_seconds	= 0;
        dn.dn_nsecond	= 0;
    }
    else
    {

#       ifdef xDEBUG
	if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_003_DATEFI_TRACE,
		&dum1, &dum2))
	    TRdisplay("strtodate: date specification is '%s'\n", specbuf);
#       endif

	/*
        ** Match the template with ANSI templates to make sure 
	** it's valid.
	*/

        d_valp = d_val_array;
	d_sizp = d_val_size;
        p = specbuf;

	tmptab = Adu_ansidtfmt;

	for (; tmptab->dt_tmplt != NULL; tmptab++)
	{
	    char *tp;
	    char *sp;
	    tp = tmptab->dt_tmplt;

	    if (((date_dv->db_datatype == DB_ADTE_TYPE) ||
	         (date_dv->db_datatype == DB_INDS_TYPE) ||
	         (date_dv->db_datatype == DB_INYM_TYPE)) &&
		 (tmptab->dttype != date_dv->db_datatype)
	       )
		    continue;

	    /* skip to the actual type */
	    for (tp = tmptab->dt_tmplt, sp = specbuf; 
		    *tp == *sp  &&  *tp != EOS; 
		   	 tp++, sp++)
		    continue;

	    if ((*tp == EOS) && (*sp == EOS))
	    {
	       break;
	    }
	}

	/* Error not found */
	if (tmptab->dt_tmplt == NULL)
	{
    	    db_stat = adu_error(adf_scb, adf_errcode, 2, (i4) 0, (i4 *) dbuf);
	    if (!uselocall)
		MEfree((PTR)dbuf);
	    return (db_stat);

	}
	else
	{
	/* interpret date specification
	** Check if the string supplied conforms to a template which
	** is valid for the intended datatype 
	*/

	    if (db_stat = ad0_dtcvrt(adf_scb, tmptab->dt_interp, &d_valp, &d_sizp, &dv, 
				date_dv->db_prec, &now) != E_DB_OK)
	    {
    	         db_stat = adu_error(adf_scb, adf_errcode, 2, (i4) 0, (i4 *) dbuf);
		 if (!uselocall)
		    MEfree((PTR)dbuf);
		 return (db_stat);
	    }
	}

        /* finish filling in the date conversion struct, do the conversion */
	if ((db_stat = ad0_4date(adf_scb, &dv, &dn, date_dv, now)) != E_DB_OK)
	{
	   if (!uselocall)
	        MEfree((PTR)dbuf);
	    return (db_stat);
	}
        /* check for 'now' case and set status bit for timespec */

        if (now == DTE_ABS_NOW)
            dn.dn_status |= AD_DN_TIMESPEC;
        else if (now == DTE_OFFSET_OR_TODAY)
            dn.dn_seconds = 0;
    }

    /* Now we convert the AD_NEWDTNTRNL to respective storage format
    ** Based on the output datatype 
    */ 
    adu_7from_dtntrnl(date_dv, &dn);

#   ifdef xDEBUG
    if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_003_DATEFI_TRACE,
	    &dum1, &dum2))
    {
        TRdisplay("Converted date is: year = %d, month = %d, day = %d, ",
            dn.dn_year, dn.dn_month, dn.dn_day);
        TRdisplay("time = %d.%09d, status = %d\n", dn.dn_seconds, dn.dn_nsecond, dn.dn_status);
    }
#   endif

    if (!uselocall)
	MEfree((PTR)dbuf);

    return (E_DB_OK);
}

/*{
** Name: adu_date_format
**	translate a date format string into the code value
**
** Description:
**      This routine converts a date format (e.g. multinational)
**	into its code.
**
** Inputs:
**	date_format		date format string
**
** Outputs:
**      None
**
**  Returns:
**	Returns a code value such as DB_US_DFMT, or -1 for failure.
**
**
** History:
**	16-Oct-2007 (kibro01) b119318
**	    Written
*/
i4
adu_date_format(
char	*date_format)
{
    i4 loop;
    for (loop = 0; Adu_date_format_list[loop].date_name != NULL; loop++)
    {
	if (!STcasecmp(date_format, Adu_date_format_list[loop].date_name))
		return (Adu_date_format_list[loop].date_id);
    }
    return (-1);
}

/*{
** Name: adu_date_string
**	translate a date code value into the format string
**
** Description:
**      This routine converts a date id into its code.
**
** Inputs:
**	date_id		
**
** Outputs:
**      None
**
**  Returns:
**	Returns a string such as "multinational4" or "" for failure
**
**
** History:
**	7-Nov-2007 (kibro01) b119428
**	    Written
*/
char *
adu_date_string(
i4	date_id)
{
    i4 loop;
    for (loop = 0; Adu_date_format_list[loop].date_name != NULL; loop++)
    {
	if (Adu_date_format_list[loop].date_id == date_id)
		return (Adu_date_format_list[loop].date_name);
    }
    return "";
}

