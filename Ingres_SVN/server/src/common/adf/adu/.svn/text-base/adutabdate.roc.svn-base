/*
** Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    "adu2date.h"
#include    "iicommon.h"

/**
**
**  Name: ADUTABDATE.ROC - defines read-only data associated with dates.
**
**  Description:
**        This file defines and initializes read-only global data used in the
**	interpretation of dates.  This is put in a ROC, read-only-C, file so
**	that it can be loaded as a shared text segment on Unix.
**
**  History:    
**      03-dec-85 (seputis) added quarters and weeks
**      25-may-86   (ericj)    
**          Converter for Jupiter.
**	26-may-86 (ericj)
**	    Changed the four read-only templates supporting dates templates
**	    so that each one would have all of the possible date formats.
**	    These will be used directly to interpret dates instead of the
**	    global updateble table, Datetmplt[], which has been deleted.
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	30-jan-87 (thurston)
**	    Changed the spec letter for day from 'G' to 'D'.
**	21-jan-88 (thurston)
**	    Added the `1988_01_21' date format to all of the international
**	    date format tables.  This is produced by the INGRES date_gmt()
**	    function (recent change; used to use `1988.01.21'), and is depended
**	    on by the standard catalog interface.  I also spiffed up the
**	    comments a bit, where I could.
**	06-nov-88 (thurston)
**	    Temporarily commented out the READONLYs on various tables.
**	15-jun-89 (jrb)
**	    Made all tables GLOBALCONSTDEF instead of just GLOBALDEF (this is
**	    for MVS).
**      06-may-92 (stevet)
**          Added Adu_5ymd[], Adu_6mdy[] and Adu_7dmy[] tables for
**          YMD, MDY and DMY new date formats repectively.
**	13-mar-1996 (angusm)
**		bug 48458: MULTINATIONAL4 added:  as MULTINATIONAL with
**		4-digit year.
**	09-feb-1998 (kinte01)
**	   Made adu_dt_size GLOBALCONSTDEF instead of GLOBALDEF READONLY
**	   This corrects problem with DEC C compiler complaints regarding
**	   the incompatibility between the definition here and the one in
**	   adu2date.h.
**	07-Jun-1999 (shero03)
**	    Add ISO-6801 week in dextract
**	24-Jun-1999 (shero03)
**	    Add ISO4.
**	15-jan-1999 (kitch01)
**	   Bug 94791. Added Canadian timezones and GMT zones currently 
**	   defined in iiworld.tz, plus gmt-3.5. For those zones that have
**	   half hours the convention is to omit the decimal point, thus
**	   GMT-3.5 becomes GMT-35. Also changed the values to be the 
**	   offset from GMT in minutes rather than hours, again to facilitate
**	   zones with half hours.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	1-Jul-2004 (schka24)
**	    1-Jan-0001 (day 1 in our reckoning) was Saturday, not Friday.
**	    Fix the weekday table now that day numbers are being properly
**	    computed.
**	29-Sep-2004 (drivi01)
**	    Added LIBRARY jam hint to put this file into IMPADFLIBDATA
**	    in the Jamfile.
**	31-Jul-2006 (gupsh01)
**	    Added new format adu_10ansi.
**	19-oct-2006 (dougi)
**	    Added microsecond and nanosecond.
**	02-feb-2007 (gupsh01)
**	    Added timezone_hour and timezone_minute.
**	18-Oct-2007 (kibro01) b119318
**	    Use a common function in adu to determine valid date formats
**/

/* Jam hints
**
LIBRARY = IMPADFLIBDATA
**
*/


/*
** Definition of all global variables owned by this file.
*/

/*
**  Define `Adu_weekdays[]', the integer code as used by the day-of-week
**  function, dow(), and the capitalized abbreviation for a weekday.
*/

GLOBALCONSTDEF   ADU_WEEKDAYS Adu_weekdays[]=
{
    0,	    "Fri",
    1,	    "Sat",
    2,	    "Sun",
    3,	    "Mon",
    4,	    "Tue",
    5,	    "Wed",
    6,	    "Thu"
};


/*
**  Define `Adu_datename[]', used to define valid parts of an INGRES date.
**  This also defines values associated with these parts where appropriate.
**  
**  NOTE: this table is binary-searched, so all entries MUST be alphabetic.
*/

GLOBALCONSTDEF ADU_DATENAME Adu_datename[] =
{
    "adt",      't',    -180,
    "am",       'a',    0,
    "apr",      'm',    4,
    "april",    'm',    4,
    "ast",      't',	-240,
    "aug",      'm',    8,
    "august",   'm',    8,

    "bst",	't',	60,

    "cdt",      't',    -300,
    "cst",      't',    -360,

    "day",      'D',    -1,
    "days",     'D',    -1,
    "dec",      'm',    12,
    "december", 'm',    12,

    "edt",      't',    -240,
    "est",      't',    -300,

    "feb",	'm',    2,
    "february", 'm',    2,

    "gmt",      't',    0,
    "gmt-1",    't',	-60,
    "gmt-10",	't',	-600,
    "gmt-11",	't',	-660,
    "gmt-12",	't',	-720,
    "gmt-2",	't',	-120,
    "gmt-3",	't',	-180,
    "gmt-35",	't',	-210,
    "gmt-4",	't',	-240,
    "gmt-5",	't',	-300,
    "gmt-6",	't',	-360,
    "gmt-7",	't',	-420,
    "gmt-8",	't',	-480,
    "gmt-9",	't',	-540,
    "gmt1",	't',	60,
    "gmt10",	't',	600,
    "gmt105",	't',	630,
    "gmt11",	't',	660,
    "gmt12",	't',	720,
    "gmt13",	't',	780,
    "gmt2",	't',	120,
    "gmt3",	't',	180,
    "gmt35",	't',	210,
    "gmt4",	't',	240,
    "gmt5",	't',	300,
    "gmt55",	't',	330,
    "gmt6",	't',	360,
    "gmt7",	't',	420,
    "gmt8",	't',	480,
    "gmt9",	't',	540,
    "gmt95",	't',	570,


    "hour",     'H',    -1,
    "hours",    'H',    -1,
    "hr",       'H',    -1,
    "hrs",      'H',    -1,

    "iso-week", 'w',    -1,
    "iso-wk",   'w',    -1,
    "iso-wks",  'w',    -1,

    "jan",	'm',    1,
    "january",  'm',    1,
    "jul",      'm',    7,
    "july",     'm',    7,
    "jun",      'm',    6,
    "june",     'm',    6,

    "mar",      'm',    3,
    "march",    'm',    3,
    "may",      'm',    5,
    "mdt",      't',    -360,
    "microsecond", 'x', -1,
    "min",      'I',    -1,
    "mins",     'I',    -1,
    "minute",   'I',    -1,
    "minutes",  'I',    -1,
    "mo",       'M',    -1,
    "month",    'M',    -1,
    "months",   'M',    -1,
    "mos",      'M',    -1,
    "mst",      't',    -420,

    "nanosecond", 'X',  -1,
    "ndt",	't',	-150,
    "nov",      'm',    11,
    "november", 'm',    11,
    "now",      'R',    0,
    "nst",	't',	-210,

    "oct",      'm',    10,
    "october",  'm',    10,

    "pdt",      't',    -420,
    "pm",       'a',    12,
    "pst",      't',    -480,

    "qtr",      'Q',    -1,
    "qtrs",     'Q',    -1,  
    "quarter",  'Q',    -1,
    "quarters", 'Q',    -1,

    "sec",      'S',    -1,
    "second",   'S',    -1,
    "seconds",  'S',    -1,
    "secs",     'S',    -1,
    "sep",      'm',    9,
    "september",'m',    9,

    "timezone_hour",    'R',    -1,
    "timezone_minute",    'T',    -1,
    "today",    'Z',    0,

    "week",     'W',    -1,
    "weeks",    'W',    -1,
    "wk",       'W',    -1,
    "wks",      'W',    -1,

    "ydt",	't',	-420,
    "year",     'Y',    -1,
    "years",    'Y',    -1,
    "yr",       'Y',    -1,
    "yrs",      'Y',    -1,
    "yst",	't',	-480,
};

GLOBALCONSTDEF i4  Adu_dt_size = 
		(sizeof(Adu_datename) / sizeof(ADU_DATENAME));


/*
**  INTERNATIONAL DATE FORMAT ARRAYS:
**  ---------------------------------
**
**  The former global array, `Datetmplt[]', no longer exists, since it was
**  updatable and settable for every session.  Now a code is stored in the
**  ADF_CB (the session control block for ADF) which tells which of the
**  following tables to use, depending on the setting of II_DATE_FORMAT; which
**  is sent by the FE's to the DBMS at session startup time.  If II_DATE_FORMAT
**  is not set, the default is to use U.S. conventions.
**
**  The following date templates contain the special multinational formats that
**  are supported.  Currently, there are a total of five independent formats
**  which are supported (see the tables below): 
**
**	    II_DATE_FORMAT	Code found in ADF_CB	Table to use
**	    --------------	--------------------	------------
**	    "US"		DB_US_DFMT		Adu_0us[]
**
**	    "MULTINATIONAL"	DB_MLTI_DFMT		Adu_1multi[]
**
**	    "FINLAND"		DB_FIN_DFMT		Adu_2finland[]
**	    or "SWEDEN"
**
**	    "ISO"		DB_ISO_DFMT		Adu_3iso[]
**
**	    "GERMAN"		DB_GERM_DFMT		Adu_4german[]
**
**	    "YMD"		DB_YMD_DFMT		Adu_5ymd[]
**
**	    "MDY"		DB_MDY_DFMT		Adu_6mdy[]
**
**	    "DMY"		DB_DMY_DFMT		Adu_7dmy[]
**
**	    "MULTINATIONAL4"	DB_MLT4_DFMT		Adu_8multi[]
**
**	    "ISO4"		DB_ISO4_DFMT		Adu_9iso4[]
**		
**
**  The only `time' formats that will be allowed for each of the multinational
**  types will be:
**			date		(00:00:00 will be assumed)
**			date hh:mm:ss
**			date hh:mm
**
**			    0 <= hh <= 23
**			    0 <= mm <= 59
**			    0 <= ss <= 59
**
**
**	    =========================================================
**	    *****     I M P O R T A N T     N O T E     # 1     *****
**	    =========================================================
**  In all of the tables below, the one format that MUST EXIST in every table
**  is the { "n_n_n", "ymd" }, which is produced by the INGRES date_gmt()
**  function, and is depended on by the standard catalog interface.
**  For performance reasons, it is nice to have this format as the first
**  in the table.
**
**	    =========================================================
**	    *****     I M P O R T A N T     N O T E     # 2     *****
**	    =========================================================
**  The values of the DB_*_DFMT constants are assumed to be the index into the
**  Adu_multi_tmplts[] array.  That is, DB_ISO_DFMT is defined to 3, therefore
**  Adu_multi_tmplts[3] *MUST* hold the ISO date formats.
*/


/*
** This is the U.S. format (the default).
*/

GLOBALCONSTDEF	DATETMPLT   Adu_0us[] =	    /* used for DB_US_DFMT */
{
    "n_n_n",	    "ymd",	/* date_gmt() format for std catalogs */
    "n/n/n",	    "mdy",
    "n-m-n",	    "dmy",
    "n-n-n",	    "mdy",
    "n.n.n",	    "ymd",
    "n/n",	    "md",
    "n-n",	    "md",
    "n",	    "K",       /* Special Morrison-Knudsen
			       **   MMDDYY format peb 10/24/83
			       */
    "z",	    "f",
    "r",	    "r",
     0,		     0
};


/*
** This is the most common european date format
*/

GLOBALCONSTDEF  DATETMPLT   Adu_1multi[] =	    /* used for DB_MLTI_DFMT */
{
    "n_n_n",	    "ymd",	/* date_gmt() format for std catalogs */
    "n/n/n",	    "dmy",	/* differs from US format */
    "n-m-n",	    "dmy",
    "n-n-n",	    "mdy",
    "n.n.n",	    "ymd",
    "n/n",	    "md",
    "n-n",	    "md",
    "n",	    "K",       /* Special Morrison-Knudsen
			       **   MMDDYY format peb 10/24/83
			       */
    "z",	    "f",
    "r",	    "r",
     0,		     0
};


/*
** This is used in Finland and Sweden
*/

GLOBALCONSTDEF	DATETMPLT   Adu_2finland[] =    /* used for DB_FIN_DFMT */
{
    "n_n_n",	    "ymd",	/* date_gmt() format for std catalogs */
    "n/n/n",	    "mdy",
    "n-m-n",	    "dmy",
    "n-n-n",	    "ymd",	/* differs from US format */
    "n.n.n",	    "ymd",
    "n/n",	    "md",
    "n-n",	    "md",
    "n",	    "K",       /* Special Morrison-Knudsen
			       **   MMDDYY format peb 10/24/83
			       */
    "z",	    "f",
    "r",	    "r",
     0,		     0
};


/*
**  This is also used in Finland (the "ISO" format as per Peter Madams).
**  The difference is the special Morrison-Knudsen format is interpretted as
**  "YYMMDD" instead of "MMDDYY" as in all of the other tables.
*/

GLOBALCONSTDEF  DATETMPLT   Adu_3iso[] =	    /* used for DB_ISO_DFMT */
{
    "n_n_n",	    "ymd",	/* date_gmt() format for std catalogs */
    "n/n/n",	    "mdy",
    "n-m-n",	    "dmy",
    "n-n-n",	    "mdy",
    "n.n.n",	    "ymd",
    "n/n",	    "md",
    "n-n",	    "md",
    "n",	    "J",       /* Special ISO format for Finnish national
			       ** conventions:  YYMMDD format peb 10/24/83
			       */ /* differs from US format */
    "z",	    "f",
    "r",	    "r",
     0,		     0
};


/*
** This is used in Germany and elsewhere.
*/

GLOBALCONSTDEF  DATETMPLT   Adu_4german[] =    /* used for DB_GERM_DFMT */
{
    "n_n_n",	    "ymd",	/* date_gmt() format for std catalogs */
    "n/n/n",	    "mdy",
    "n-m-n",	    "dmy",
    "n-n-n",	    "mdy",
    "n.n.n",	    "dmy",	/* differs from US format */
    "n/n",	    "md",
    "n-n",	    "md",
    "n",	    "G",       /* Special German date format
			       **   "xxxxx"    = DMMYY
			       **   "xxxxxx"   = DDMMYY
			       **   "xxxxxxx"  = DMMYYYY
			       **   "xxxxxxxx" = DDMMYYYY
			       */ /* differs from US format */
    "z",	    "f",
    "r",	    "r",
     0,		     0
};



/*
** This is a YMD format.
*/

GLOBALCONSTDEF  DATETMPLT   Adu_5ymd[] =    /* used for DB_YMD_DFMT */
{
    "n_n_n",	    "ymd",	/* date_gmt() format for std catalogs */
    "n/n/n",	    "ymd",
    "n-m-n",	    "ymd",
    "n-n-n",	    "ymd",
    "n.n.n",	    "ymd",	/* differs from US format */
    "n/n",	    "md",
    "n-n",	    "md",
    "n",	    "Y",       /* Special YMD date format
			       **   "xxxx"     = MMDD
			       **   "xxxxx"    = YYMDD
			       **   "xxxxxx"   = YYMMDD
			       **   "xxxxxxx"  = YYYYMDD
			       **   "xxxxxxxx" = YYYYMMDD
			       */ /* differs from US format */
    "z",	    "f",
    "r",	    "r",
     0,		     0
};



/*
** This is a MDY format.
*/

GLOBALCONSTDEF  DATETMPLT   Adu_6mdy[] =    /* used for DB_MDY_DFMT */
{
    "n_n_n",	    "ymd",	/* date_gmt() format for std catalogs */
    "n/n/n",	    "mdy",
    "m-n-n",	    "mdy",
    "n-n-n",	    "mdy",
    "n.n.n",	    "mdy",	/* differs from US format */
    "n/n",	    "md",
    "n-n",	    "md",
    "n",	    "M",       /* Special MDY date format
			       **   "xxxx"     = MMDD
			       **   "xxxxx"    = MDDYY
			       **   "xxxxxx"   = MMDDYY
			       **   "xxxxxxx"  = MDDYYYY
			       **   "xxxxxxxx" = MMDDYYYY
			       */ /* differs from US format */
    "z",	    "f",
    "r",	    "r",
     0,		     0
};



/*
** This is a DMY format.
*/

GLOBALCONSTDEF  DATETMPLT   Adu_7dmy[] =    /* used for DB_DMY_DFMT */
{
    "n_n_n",	    "ymd",	/* date_gmt() format for std catalogs */
    "n/n/n",	    "dmy",
    "n-m-n",	    "dmy",
    "n-n-n",	    "dmy",
    "n.n.n",	    "dmy",	/* differs from US format */
    "n/n",	    "dm",
    "n-n",	    "dm",
    "n",	    "D",       /* Special DMY date format
			       **   "xxxx"     = DDMM
			       **   "xxxxx"    = DDMYY
			       **   "xxxxxx"   = DDMMYY
			       **   "xxxxxxx"  = DDMYYYY
			       **   "xxxxxxxx" = DDMMYYYY
			       */ /* differs from US format */
    "z",	    "f",
    "r",	    "r",
     0,		     0
};
/*
** This is the most common european date format + 4-digit year
*/

GLOBALCONSTDEF  DATETMPLT   Adu_8multi[] =	    /* used for DB_MLT4_DFMT */
{
    "n_n_n",	    "ymd",	/* date_gmt() format for std catalogs */
    "n/n/n",	    "dmy",	/* differs from US format */
    "n-m-n",	    "dmy",
    "n-n-n",	    "mdy",
    "n.n.n",	    "ymd",
    "n/n",	    "md",
    "n-n",	    "md",
    "n",	    "K",       /* Special Morrison-Knudsen
			       **   MMDDYY format peb 10/24/83
			       */
    "z",	    "f",
    "r",	    "r",
     0,		     0
};


/*
**  This is identical to the "ISO" format, but displays the date in yyyy.
*/

GLOBALCONSTDEF  DATETMPLT   Adu_9iso4[] =	    /* used for DB_ISO4_DFMT */
{
    "n_n_n",	    "ymd",	/* date_gmt() format for std catalogs */
    "n/n/n",	    "mdy",
    "n-m-n",	    "dmy",
    "n-n-n",	    "mdy",
    "n.n.n",	    "ymd",
    "n/n",	    "md",
    "n-n",	    "md",
    "n",	    "J",       /* Special ISO format for Finnish national
			       ** conventions:  YYMMDD format peb 10/24/83
			       */ /* differs from US format */
    "z",	    "f",
    "r",	    "r",
     0,		     0
};

GLOBALCONSTDEF  DATETMPLT   Adu_10ansi[] =         /* used for ansi date/time */
{
    "n-n-n",        "ymd",
     0,		     0
};


GLOBALCONSTDEF  DATETMPLT   *Adu_multi_tmplts[] =
{
    Adu_0us, Adu_1multi, Adu_2finland, Adu_3iso, Adu_4german,
    Adu_5ymd, Adu_6mdy, Adu_7dmy ,  Adu_8multi, Adu_9iso4, 
    Adu_10ansi

};

GLOBALCONSTDEF   DATEFMTLIST Adu_date_format_list[] = 
{
	{	"us",			DB_US_DFMT	},
	{	"multinational",	DB_MLTI_DFMT	},
	{	"finland",		DB_FIN_DFMT	},
	{	"sweden",		DB_FIN_DFMT	},
	{	"iso",			DB_ISO_DFMT	},
	{	"german",		DB_GERM_DFMT	},
	{	"ymd",			DB_YMD_DFMT	},
	{	"mdy",			DB_MDY_DFMT	},
	{	"dmy",			DB_DMY_DFMT	},
	{	"multinational4",	DB_MLT4_DFMT	},
	{	"iso4",			DB_ISO4_DFMT	},
	{	"ansi",			DB_ANSI_DFMT	},
	{	NULL,			0		}
};

