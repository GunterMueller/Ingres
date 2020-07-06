/*
** Copyright (c) 2004 Ingres Corporation
*/

/**
** Name: ADU2DATETAB.H - defines read-only tables used for the Ingres
**			 date datatype.
**
** Description:
**        This file defines the read-only structures which are used when
**	processing the Ingres date datatype.  These structures for the most
**	part are initialized in datetab.roc.
**
**	Dependencies:
**	    compat.h
**
** History: $Log-for RCS$
**      25-may-86 (ericj)
**          Converted from adttab.h to datetab.h for Jupiter.
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	18-mar-87 (thurston)
**	    Added some WSCREADONLY to pacify the Whitesmith compiler on MVS.
**	17-mar-87 (thurston)
**	    Added typedef names ADU_WEEKDAYS and ADU_DATENAME.
**	25-may-88 (thurston)
**	    Added German date format, and made GLOBAL DATETMPLT's have more
**	    meaningfull names.
**	14-jun-89 (jrb)
**	    Changed GLOBALREF WSCREADONLY to GLOBALCONSTREF for all applicable
**	    items.
**      31-dec-1992 (stevet)
**          Added function prototypes.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	18-Oct-2007 (kibro01) b119318
**	    Use a common function in adu to determine valid date formats
**/


/*}
** Name: ADU_WEEKDAYS - describes the day of the week.
**
** Description:
**        This structure contains the abbreviations for the days of the
**	week and their corresponding integer codes (as computed in dayofweek
**	function).
**
** History:
**     25-may-86 (ericj)
**          Converted for Jupiter.
[@history_line@]...
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
*/

typedef struct _ADU_WEEKDAYS
{
    i2              day_val;            /* integer code for day of week. */
    char	    *day_name;		/* Abbreviated name for day of week. */
}   ADU_WEEKDAYS;


/*}
** Name: ADU_DATENAME - defines various date semantic types.
**
** Description:
**        This struct defines various date parts, such as months, standard
**	timezones, and intervals of time.  Where values are associated with
**	the "date part", they are initialized in datetab.roc
**
** History:
**     25-may-86 (ericj)
**          Converted for Jupiter.
[@history_line@]...
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
*/

typedef struct _ADU_DATENAME
{
    char            *d_name;            /* Name defining the date part. */
    char	    d_class;		/* The part of a date it is. */
    i2		    d_value;		/* The value, if any, associated with
					** this kind of date part.
					*/
}   ADU_DATENAME;

/* The primary lookup routine. */
FUNC_EXTERN ADU_DATENAME *ad0_13tab_look(char *str);


/*}
** Name: DATETMPLT - structure for defining date input templates.
**
** Description:
**        This is a structure for defining valid date input templates and
**	how they should be interpreted.
**
** History:
**     25-may-86 (ericj)
**          Converted for Jupiter
[@history_line@]...
**      27-jul-86 (ericj)
**	    Converted for new ADF error handling.
**	25-may-88 (thurston)
**	    Added German date format, and made GLOBAL DATETMPLT's have more
**	    meaningfull names.
*/
typedef struct _DATETMPLT
{

    char            *dt_tmplt;          /* A valid date input format. */
    char	    *dt_interp;		/* How to interpret the format. */
}   DATETMPLT;

/*}
** Name: ANSIDATETMPLT - structure for 
**
** Description:
**        This structure for defining valid ANSI date input templates
** History:
**	29-aug-2006 (gupsh01)
**	    Added.
*/
typedef struct _ANSIDATETMPLT
{
    i2		dttype;       	/* Date data type. */
    char	*dt_tmplt;      /* A valid date input format. */
    char	*dt_interp;	/* How to interpret the format. */
} ANSIDATETMPLT;

GLOBALCONSTREF	    DATETMPLT		Adu_0us[],
					Adu_1multi[],
					Adu_2finland[],
					Adu_3iso[],
					Adu_4german[];

GLOBALCONSTREF	    DATETMPLT   	*Adu_multi_tmplts[];


/*
** References to globals variables declared in other C files.
*/

GLOBALCONSTREF			ADU_WEEKDAYS	  Adu_weekdays[];
GLOBALCONSTREF			ADU_DATENAME      Adu_datename[];
GLOBALCONSTREF			i4		  Adu_dt_size;

typedef struct _DATEFMTLIST
{
        char    *date_name;
        i4      date_id;
} DATEFMTLIST;

GLOBALCONSTREF   DATEFMTLIST Adu_date_format_list[];


