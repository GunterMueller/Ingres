/*
**  Copyright (c) 2004, 2008 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <me.h>
#include    <tm.h>
#include    <tmtz.h>
#include    <tr.h>
#include    <iicommon.h>
#include    <adf.h>
#include    <ulf.h>
#include    <adftrace.h>
#include    <adfint.h>
#include    <aduint.h>
#include    <adudate.h>
#include    "adutime.h"
#include    "adu1dates.h"


/**
** Name:  ADULLDATES.C -    Low level date routines which primarily support ADF
**			    interface.
**
** Description:
**	  This file defines low level date routines which primarily support
**	the ADF interface.  Other routines are used for the normalization
**	of dates.
**
**	This file defines:
**
**	    adu_1normldate()   - Normalizes an absolute date.
**	    adu_2normldint()   - Normalizes a internal interval date.
**          adu_4date_cmp()    - Internal routine used for comparing dates.
**          adu_dlenchk()      - Required function for checking create length
**                                 and precision
**          adu_3day_of_date() - Compute absolute number of days since epoch
**                                 for the given ABSOLUTE date.
**          adu_5time_of_date()- Calculate the number of second since 1/1/70.
**
**  Function prototypes defined in ADUDATE.H except adu_4date_cmp, which is
**  defined in ADUINT.H file.
** 
**  History:
**	21-jun-85 (wong)
**	    Used 'I1_CHECK_MACRO' macro to hide unsigned character problems
**	    with AD_NEWDTNTRNL member 'dn_highday'.
**	11-apr-86 (ericj)
**	    Assimilated into Jupiter from 4.0/02 code.
**	03-feb-87 (thurston)
**	    Upgraded the adu_4date_cmp() routine to be able to be a function
**	    instance routine, as well as the adc_compare() routine for dates.
**      25-feb-87 (thurston)
**          Made changes for using server CB instead of GLOBALDEF/REFs.
**	16-aug-89 (jrb)
**	    Put adu_prelim_norml() back in and re-wrote for performance.
**	24-jan-90 (jrb)
**	    Fixed bug 9859.  date('60 mins') was returning "2 hours".
**	06-sep-90 (jrb)
**	    Use adf_timezone (session specific) rather than call TMzone.
**      29-aug-92 (stevet)
**          Added adu_5time_of_date to calculate the number of second
**          since 1/1/70 for a given Ingres date.  Modified the timezone
**          adjustment method in adu_4date_cmp().
**      04-jan-1993 (stevet)
**          Added function prototypes and moved adu_prelim_norml to adudates.c
**          since this routine only used in adudates.c file.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	25-aug-93 (ed)
**	    remove dbdbms.h
**	10-feb-1998 (hanch04)
**	    changed PRE_EPOCH_DAYS when calculating days to support
**	    years 0001 - 9999, EPOCH will be calculated as 14-sep-1752
**	    dates between, but not including  02-sep-1752 and 14-sep-1752
**	    do not exist.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	08-Feb-2008 (kiria01) b119885
**	    Change dn_time to dn_seconds to avoid the inevitable confusion with
**	    the dn_time in AD_DATENTRNL.
**	19-Feb-2008 (kiria01) b119943
**	    Include dn_nseconds with dn_seconds in checks for setting
**	    dn_status);
**/

FUNC_EXTERN DB_STATUS
ad0_1cur_init(
ADF_CB		    *adf_scb,
struct	timevect    *tv,
bool                now);

/*  Static variable references	*/

static	i4	    dum1;   /* Dummy to give to ult_check_macro() */
static	i4	    dum2;   /* Dummy to give to ult_check_macro() */

static const int daysBefore[] = {
	0,	/* notused, dn_month is 1 origin */
	0,	/* Jan */
	31,	/* Feb */
	59,	/* Mar */
	90,	/* Apr */
	120,	/* May */
	151,	/* Jun */
	181,	/* Jul */
	212,	/* Aug */
	243,	/* Sep */
	273,	/* Oct */
	304,	/* Nov */
	334	/* Dec */
};


/*
**  Name:   adu_1normldate() - normalize an internal date.
**
**  Description:
**	Normalizes an absolute time as follows:
**	    1 <= month <= 12
**	    1 <= day <= adu_2monthsize(month, year)
**	    0 <= time <= 24 * 3600000
**
**	normldate works for both positive dates and negative
**      dates (for subtraction).
**
**  Inputs:
**      d   - Pointer to date to normalize.
**
**  Outputs:
**	*d  - normalized date.
**
**  Returns:
**      (none)
**
**  History:
**      03/25/85 (lichtman) -- fixed bug #5281: timezone of 0 allows
**              24:00:00 to be stored.  Fixed normalization
**              of time part of date to check for >= AD_5DTE_MSPERDAY
**              instead of > AD_5DTE_MSPERDAY.
**	10-apr-86 (ericj) - snatched from 4.0 code and modified for Jupiter.
**	13-jan-87 (daved) - performance enhancements
**	10-Dec-2004 (schka24)
**	    More (minor) speed tweaks.
**	17-jul-2006 (gupsh01)
**	    Added support in adu_1normldate for new internal format 
**	    AD_NEWDTNTRNL for ANSI datetime support.
**	01-aug-2006 (gupsh01)
**	    Added support for nanosecond precision level.
*/

#   define  xDEBUG 1
VOID
adu_1normldate(
AD_NEWDTNTRNL   *d)
{
    register i4	    days;
    register i4     t;

    days = d->dn_day;

#   ifdef xDEBUG
    if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_006_NORMDATE, &dum1, &dum2))
    {
        TRdisplay("normldate:abs.date input:year=%d, month=%d, day=%d\n",
            d->dn_year, d->dn_month, days);
        TRdisplay("normldate:and time=%d.%09d, status=%d\n",
            d->dn_seconds, d->dn_nsecond, d->dn_status);
    }
#   endif

    if (d->dn_nsecond < 0 || d->dn_nsecond > AD_33DTE_MAX_NSEC)
    {
        if (d->dn_nsecond < 0) 
	{
	    d->dn_nsecond += AD_42DTE_INSUPPERLIMIT; 
	    d->dn_seconds -= 1;
	}
	else 
	{
	    d->dn_nsecond -= AD_42DTE_INSUPPERLIMIT; 
	    d->dn_seconds += 1;
	}
    }

    if (d->dn_seconds < 0 || d->dn_seconds >= AD_41DTE_ISECPERDAY)
    {
	t = d->dn_seconds / AD_41DTE_ISECPERDAY;
	days += t;
	d->dn_seconds -= (t * AD_41DTE_ISECPERDAY);
	/* if time is still negative, then bring it positive */
	if (d->dn_seconds < 0)
	{
	    days--;
	    d->dn_seconds += AD_41DTE_ISECPERDAY;
	}
    }

    /* normalize months so we can handle days correctly */

    if ((d->dn_month < 1 || d->dn_month > 12) && d->dn_year > 0)
    {
	while ((d->dn_month < 1) && (d->dn_year > 0))
	{
	    d->dn_year--;
	    d->dn_month += 12;
	}
	t = (d->dn_month - 1) / 12;
	d->dn_year += t;
	d->dn_month -= (t*12);
    }


    /* now do days remembering to handle months on overflow/underflow.
    ** Don't bother if # of days is ok for any month.
    */

    if (days > 28)
    {
	while (days > (t = adu_2monthsize((i4) d->dn_month, (i4) d->dn_year)))
	{
	    days -= t;
	    d->dn_month++;

	    if (d->dn_month > 12)
	    {
		d->dn_year++;
		d->dn_month -= 12;
	    }
	}
    }

    while ((days < 1) && (d->dn_month > 0))
    {
        d->dn_month--;

        if (d->dn_month < 1)
        {
            d->dn_year--;
            d->dn_month += 12;
        }

        days += adu_2monthsize((i4) d->dn_month, (i4) d->dn_year);
    }

#   ifdef xDEBUG
    if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_006_NORMDATE, &dum1, &dum2))
    {
        TRdisplay("normldate:abs.date output:year=%d, month=%d, day=%d\n",
            d->dn_year, d->dn_month, days);
        TRdisplay("normldate:and time=%d, status=%d\n",
            d->dn_seconds, d->dn_status);
    }
#   endif

    d->dn_day    = days;
}


/*
**  Name:   adu_2normldint() - Internal routine used to normalize interval dates
**
**  Description:
**      Normalizes an interval date as follows:
**      0 <= time  <= 24 * 3600000
**      0 <= month <  12
**
**  NOTE: days and times are not normalized to months and years
**      since these are inexact transformations.
**
**  Returns number of days in interval, used for comparisons.
**  Computed using 365 days/year and 30.5 days/month
**
**      11/02/84 (lin) - if time interval = 0, reset DAYSPEC status bit instead
**           of the original YEARSPEC.
**
**  12/9/85 (ac) - bug # 6399. Rounding error in normalizing the dn_time in
**             mini second.
**	17-jan-02 (inkdo01)
**	    All these years, and MONTHSPEC hasn't been set for date values.
**	17-jul-2006 (gupsh01)
**	    Added support in adu_2normldint for new internal format 
**	    AD_NEWDTNTRNL for ANSI datetime support.
**	28-jul-2006 (gupsh01)
**	    Added support for nanoseconds.
**	16-oct-2006 (gupsh01)
**	    Fix day calculation.
**	13-feb-2006 (gupsh01)
**	    Another fix to days calculation.
**	4-sep-2007 (dougi)
**	    Fix normalization involving nsecs. 3 unit, multi-radix, mixed sign
**	    normalization ain't very easy.
*/

i4
adu_2normldint(
AD_NEWDTNTRNL   *d)
{
    i1	status = 0;
    i4  time = 0;
    i4  days = 0;
    i2  months = 0;
    i2  years = 0;
    f4  month_days = 0.0;
    f8  flt_days = 0.0;
    f8  flt_years = 0.0;
    f8  flt_months = 0.0;
    f8  flt_time = 0.0;
    f8  flt_temp = 0.0;
    f8  flt_dn_time = 0.0;
    i4  nsec = 0;

    days = d->dn_day;

    /* normalize days and time */

    nsec	+= d->dn_nsecond;
    /* Does nsec have more than a whole second? */
    if (nsec >  AD_42DTE_INSUPPERLIMIT)
    {
        d->dn_seconds += 1;
        nsec = d->dn_nsecond -= AD_42DTE_INSUPPERLIMIT;
    }

    /* Normalize seconds and roll over into days. */
    flt_days     = days;
    flt_time     = d->dn_seconds;
    flt_days    += flt_time / AD_40DTE_SECPERDAY;
    days         = flt_days;
    flt_dn_time  = (flt_days - days) * AD_40DTE_SECPERDAY;
    d->dn_seconds   = time = flt_dn_time + (flt_dn_time >= 0.0 ? 0.5 : -0.5);

    /* If there are nanoseconds in this value, normalize across all 3 
    ** units (days, seconds, nanoseconds). */
    if (nsec)
    {
	i1	switchval = 0;

	/* Set switchval to encode combinations of signs in units. */
	switchval = (nsec > 0) ? 2 : 1;
	switchval += (time > 0) ? 6 : ((time < 0) ? 3 : 0);
	switchval += (days > 0) ? 18 : ((days < 0) ? 9 : 0);

	flt_days -= days;		/* save days fraction */

	/* Switch on encoding (ternary number, in which leftmost digit is
	** for days units, middle is time units and rightmost is nanoseconds,
	** and 2 means value is > 0, 1 means value is < 0 and 0 means value
	** is 0). All combinations of signs of the 3 units are represented,
	** many of which don't require any wor. E.g. all values with 
	** nanosecond value 0 are ignored, because normalization of days/
	** seconds has already been done. And all combinations of days and
	** seconds with different, non-zero signs will have been normalized
	** to the same sign. */

	/* This code may look a little unconventional, but it handles the
	** problems of multi-radix normalization. If anyone can think of
	** a better way of doing it, be my guest. */
	switch (switchval) {
	  case 5:
		/* days = 0, seconds < 0, nsecs > 0 */
		nsec -= AD_42DTE_INSUPPERLIMIT;		/* remove a second */
		time++;					/* add a second */
		break;

	  case 7:
		/* days = 0, seconds > 0, nsecs < 0 */
		nsec += AD_42DTE_INSUPPERLIMIT;		/* add a second */
		time--;					/* remove a second */
		break;

	  case 11:
		/* days < 0, seconds = 0, nsecs > 0 */
		nsec -= AD_42DTE_INSUPPERLIMIT;		/* remove a second */
		time = -(AD_40DTE_SECPERDAY - 1);	/* subtract a day, less
							** the second removed 
							** to adjust nsecs */
		days++;					/* add a day */
		break;

	  case 14:
		/* days < 0, seconds < 0, nsecs > 0 */
		nsec -= AD_42DTE_INSUPPERLIMIT;		/* remove a second */
		time++;					/* add a second */
		break;

	  case 16:
		/* days < 0, seconds > 0, nsecs < 0 */
		time -= AD_40DTE_SECPERDAY;		/* remove a day */
		days++;					/* add a day */
		break;

	  case 17:
		/* days < 0, seconds > 0, nsecs > 0 */
		nsec -= AD_42DTE_INSUPPERLIMIT;		/* remove a second */
		time++;					/* add a second */
		if (time > 0)
		{
		    time -= AD_40DTE_SECPERDAY;		/* remove a day */
		    days++;				/* add a day */
		}
		break;

	  case 19:
		/* days > 0, seconds = 0, nsecs < 0 */
		nsec += AD_42DTE_INSUPPERLIMIT;		/* add a second */
		time = AD_40DTE_SECPERDAY - 1;		/* add a day from which
							** to remove a second */
		days--;					/* remove a day */
		break;

	  case 22:
		/* days > 0, seconds < 0, nsecs < 0 */
		nsec += AD_42DTE_INSUPPERLIMIT;		/* add a second */
		time += AD_40DTE_SECPERDAY - 1;		/* add a day less the
							** second added to 
							** nsec */
		days--;					/* remove a day */
		break;

	  case 23:
		/* days > 0, seconds < 0, nsecs > 0 */
		break;

	  case 25:
		/* days > 0, seconds > 0, nsecs < 0 */
		nsec += AD_42DTE_INSUPPERLIMIT;		/* add a second */
		time--;					/* remove a second */
		break;
	}

	d->dn_day = days;
	d->dn_seconds = time;
	d->dn_nsecond = nsec;
	flt_days += days;		/* recompute flt_days */
    }

    /* normalize years and months */

    flt_years    = (i4)d->dn_year;
    flt_months   = (i4)d->dn_month;
    flt_years   += flt_months / 12;
    d->dn_year   = years = flt_years;

    /*
        Since conversion from double to long truncates the double,
            round here to hide any floating point accuracy
            problems.
        Bug 1344 peb 6/25/83.
    */

    flt_temp = (flt_years - d->dn_year) * 12;
    d->dn_month = months = flt_temp + (flt_temp >= 0.0 ? 0.5 : -0.5);

    /* calculate number of days in interval */
    month_days   = flt_years * AD_14DTE_DAYPERYEAR + flt_days;

#   ifdef xDEBUG
    if (ult_check_macro(&Adf_globs->Adf_trvect, ADF_006_NORMDATE, &dum1, &dum2))
        TRdisplay("normldint: interval converted to days = %d\n", (i4)month_days); 
#   endif

    /* now normalize status bits according to LRC-approved rules */

    status = d->dn_status;

    if (years != 0)
    {
	status |= AD_DN_YEARSPEC;
	
	if (months == 0)
	    status &= ~AD_DN_MONTHSPEC;
	else status |= AD_DN_MONTHSPEC;
    }
    else
    {
	if (months != 0)
	{
	    status &= ~AD_DN_YEARSPEC;
	    status |= AD_DN_MONTHSPEC;
	}
	else
	{
	    if (status & AD_DN_YEARSPEC)
		status &= ~AD_DN_MONTHSPEC;
	}
    }

    if (days != 0)
    {
	status |= AD_DN_DAYSPEC;
	
	if (time == 0)
	    status &= ~AD_DN_TIMESPEC;
    }
    else
    {
	if (time != 0)
	{
	    status &= ~AD_DN_DAYSPEC;
	    status |= AD_DN_TIMESPEC;
	}
	else
	{
	    if (status & AD_DN_DAYSPEC)
		status &= ~AD_DN_TIMESPEC;
	}
    }

    d->dn_status    = status;
    d->dn_day    = days;

    return((i4)month_days);
}


/*
**  Name:  adu_4date_cmp() - Internal routine used for comparing dates
**
**  Parameters:
**	adf_scb -- ADF session control block
**      dv1     -- first date
**      dv2     -- second date
**	cmp     -- Result of comparison, as follows:
**			<0  if  dv1 <  dv2
**			>0  if  dv1 >  dv2
**			=0  if  dv1 == dv2
**
**  Since intervals are always < absolute dates, the status of the dates
**  are first checked. If one date is absolute and the other an interval
**  the return code is set (absolute > interval) without looking at the
**  actual dates.
**
**  Empty dates are not compared to either intervals or absolute dates.
**  The empty date is considered to be less than all other dates values,
**  but equal to another empty date.
**
**  Returns:
**	E_DB_OK
**
**  History:
**	03-feb-87 (thurston)
**	    Upgraded the adu_4date_cmp() routine to be able to be a function
**	    instance routine, as well as the adc_compare() routine for dates.
**	06-sep-90 (jrb)
**	    Use adf_timezone (session specific) rather than call TMzone.
**      29-aug-92 (stevet) 
**          Modified timezone adjustment method by call TMtz_search()
**          to calcuate correct timezone offset based on a given time
**          value.
**	17-jul-2006 (gupsh01)
**	    Added support in adu_4date_cmp for new internal format 
**	    AD_NEWDTNTRNL for ANSI datetime support.
**	25-sep-2006 (gupsh01)
**	    Added support for normalizing time to UTC for time/timestamp
**	    with time zone types.
**	07-nov-2006 (gupsh01)
**	    Fixed the second calculations.
**	02-jan-2007 (gupsh01)
**	    Account for the nanosecond difference if the absolute dates/time
**	    values are same upto nanoseconds.
**	04-feb-2007 (gupsh01)
**	    Fixed the sorting of the time values.
*/

/*ARGSUSED*/
DB_STATUS
adu_4date_cmp(
ADF_CB		*adf_scb,
DB_DATA_VALUE	*dv1,
DB_DATA_VALUE	*dv2,
i4		*cmp)
{
    i4	    days1;
    i4	    days2;
    i4	    tmp_val;
    AD_NEWDTNTRNL    *d1;
    AD_NEWDTNTRNL    *d2;
    AD_NEWDTNTRNL    tempd1;
    AD_NEWDTNTRNL    tempd2;     /* aligned storage */

    
    /*
    **  MEcopy()'s were put in for BYTE_ALIGN but are needed even
    **      on VAX as we can't do comparisions in place since
    **      we normalize the values in place.
    **  Think what happens when the sorter does these comparisons
    **      (effectively changing the internal representation
    **      to the normalized representation) and then returns
    **      these to the query processor to be stuffed into a
    **      temporary (or a new relation in modify!!!).
    **
    **  Change 8/24/83 by peb.
    */

    adu_6to_dtntrnl (dv1, &tempd1);
    adu_6to_dtntrnl (dv2, &tempd2);

    d1 = &tempd1;
    d2 = &tempd2;

    if     (     d1->dn_status == AD_DN_NULL && d2->dn_status == AD_DN_NULL)
        tmp_val = 0;
    else if(     d1->dn_status == AD_DN_NULL && d2->dn_status != AD_DN_NULL)
        tmp_val = -1;
    else if(     d1->dn_status != AD_DN_NULL && d2->dn_status == AD_DN_NULL)
        tmp_val = 1;
    else if(!(d1->dn_status & AD_DN_ABSOLUTE && d2->dn_status & AD_DN_ABSOLUTE))
    {
        if (d1->dn_status & AD_DN_ABSOLUTE)
        {
            /* d1 is absolute and d2 an interval; d1 > d2 */

            tmp_val = 1;
        }
        else if (d2->dn_status & AD_DN_ABSOLUTE)
        {
            /* d1 is an interval and d2 absolute; d1 < d2 */

            tmp_val = -1;
        }
        else        /* both dates are intervals */
        {
            days1   = adu_2normldint(d1);
            days2   = adu_2normldint(d2);

            if (!(tmp_val = days1 - days2))
                tmp_val = d1->dn_seconds - d2->dn_seconds;
        }
    }
    else
    {
	if ((d1->dn_dttype == DB_TMW_TYPE) || (d1->dn_dttype == DB_TSW_TYPE))
		d1->dn_seconds -= (d1->dn_tzhour * 3600 + d1->dn_tzminute * 60);

	if ((d2->dn_dttype == DB_TMW_TYPE) || (d2->dn_dttype == DB_TSW_TYPE))
		d2->dn_seconds -= (d2->dn_tzhour * 3600 + d2->dn_tzminute * 60);

        /* Normalize both absolute dates before comparing */
	if ( (d1->dn_dttype == DB_DTE_TYPE) || 
	     (d1->dn_dttype == DB_TME_TYPE) || 
	     (d1->dn_dttype == DB_TSTMP_TYPE)) 
	{
          if (d1->dn_status & AD_DN_TIMESPEC)
          {
            if (!(d2->dn_status & AD_DN_TIMESPEC))
            {
                d2->dn_status |= AD_DN_TIMESPEC;
		d2->dn_seconds = -(TMtz_search(adf_scb->adf_tzcb, 
					     TM_TIMETYPE_LOCAL, 
					     adu_5time_of_date(d2)));
            }
          }
          else
          {
            if (d2->dn_status & AD_DN_TIMESPEC)
            {
                d1->dn_status |= AD_DN_TIMESPEC;
		d1->dn_seconds = -(TMtz_search(adf_scb->adf_tzcb, 
					     TM_TIMETYPE_LOCAL, 
					     adu_5time_of_date(d1)));
            }
          }
        }

        adu_1normldate(d1);
        adu_1normldate(d2);

        /* both dates are absolute */

        if (
		!(tmp_val = d1->dn_year - d2->dn_year)
	    &&
            	!(tmp_val = d1->dn_month - d2->dn_month)
	    &&
            	!(tmp_val = d1->dn_day - d2->dn_day)
           )
            tmp_val = d1->dn_seconds - d2->dn_seconds;
    }

    if (tmp_val == 0)
    {
	/* check the nanosecond diff */
        tmp_val = d1->dn_nsecond - d2->dn_nsecond;
    }

    if (tmp_val < 0)
	*cmp = -1;
    else if (tmp_val > 0)
	*cmp = 1;
    else
	*cmp = 0;

    return(E_DB_OK);
}


/*
**  Name:  adu_dlenchk() - Required function for checking create length
**                         and precision
**
**  Parameters:
**      length  -- length in create specification
**      prec    -- precision in create specification
**
**  Returns:
**      true    -- valid size and precision
**      false   -- invalid size or precision
**
*/
bool
adu_dlenchk(
i4  *length,
i4  prec)
{
    return(*length == ADF_DTE_LEN && prec == 0);
}



/*
**  Name:  adu_3day_of_date() - Compute absolute number of days since epoch
**                              for the given ABSOLUTE date.
**
**  Description:
**	This is used for computing intervals in date subtraction and for
**	computing the day of week for a given date.
**
**	NOTE: The epoch is Jan 1, 1582 for all date operations.
**
**  Parameters:
**      date    -- internal date
**
**  Returns:
**      days since epoch for this date.
**
**  History
**	13-jan-87 (daved)
**	    avoid loop in computing days in year.
**	10-feb-1998 (hanch04)
**	    changed PRE_EPOCH_DAYS when calculating days to support
**	    years 0001 - 9999, EPOCH will be calculated as 14-sep-1752
**	    dates between, but not including  02-sep-1752 and 14-sep-1752
**	    do not exist.
**	19-May-2004 (schka24)
**	    Eliminate time consuming loop calculating days-before-
**	    this-month, use table lookup.
**	1-Jul-2004 (schka24)
**	    Back out year optimization, wrong because of integer divide.
**	    Century dates prior to 1752 were in fact leap years.
**	    Note that we're assuming the English calendar here.
**	17-jul-2006 (gupsh01)
**	    Added support in adu_3day_of_date for new internal format 
**	    AD_NEWDTNTRNL for ANSI datetime support.
*/

i4
adu_3day_of_date(
register AD_NEWDTNTRNL  *d)
{
    register i4 i;
    register i4 days;
    i4		yr;

    days = 0;

    /* Compute number of days in previous years , 14-sep-1752 */
#define PRE_EPOCH_DAYS 639810

    yr = d->dn_year - 1;
    /* 365 days a year plus one on leap year, except every hundred years
    ** not divisible by 400.
    ** Can't do y/4 - y/100 + y/400 = 97y/400, because the individual
    ** divisions truncate differently than the combined.
    ** 1800 was the first century year to not be a leap year, after which
    ** 2000 was the first quadricentury year to be a leap year.
    ** 1700 WAS a leap year, hence the -1 (adding it back in), and
    ** setting i=1 for early dates makes it all work out...
    */
    i = 1;
    if (yr >= 1800) i = (yr-1600) / 100;
    days = 365 * yr + (yr >> 2) - (i-1) + (i >> 2);

    /* Compute number of days in previous months */

    days += daysBefore[d->dn_month];

    /* Check for leap year, minimizing divides on modern day hardware.
    ** According to the Julian calendar, there was no century leap year
    ** adjustment.  The English calendar switched in 1752.
    ** Very old dates assume a backwards extension of the Julian calendar,
    ** but of course that's strictly a convenience and becomes increasingly
    ** bogus as you go backwards.
    */

    i = d->dn_year % 400;
    if (d->dn_month >= 3 && ((d->dn_year & 3) == 0)
      && (days < PRE_EPOCH_DAYS || (i != 100 && i != 200 && i != 300)) )
	++ days;

    /* For absolute date, high order day byte is always zero */
    /* Add in number of days in current month and return */

    days += d->dn_day;
    if (days < PRE_EPOCH_DAYS)
    {
	if (days > (PRE_EPOCH_DAYS - 11))
	    days = (PRE_EPOCH_DAYS - 12);
    }
    else
	days -= 11;

    return ( days ); 
              
}   



/*
**  Name:  adu_5time_of_date() - Compute the number of seconds since 1/1/70
**                               00:00. 
**
**  Description:
**	This internal routine is used for calculating number of seconds since
**      1/1/70 00:00.
**
**  Parameters:
**      d    -- internal date
**
**  Returns:
**      seconds since 1/1/70 00:00.
**
**  History
**	25-sep-1992 (stevet)
**	    Initial creation.
**	17-jul-2006 (gupsh01)
**	    Added support in adu_5time_of_date for new internal format 
**	    AD_NEWDTNTRNL for ANSI datetime support.
**	07-nov-2006 (gupsh01)
**	    Fixed the seconds calculations.
*/
i4
adu_5time_of_date(
register AD_NEWDTNTRNL  *d)
{
    register i4  i;
    register i4  days;
    register i4 sec;
    register i4  yr;
    i4	maxi4 = MAXI4;
    i4	maxval;
    i4	mini4 = MINI4;
    i4	minval;
    days = 0;

    /* Compute number of days in previous years */
#define DAYS_SINCE_1970 719163

    yr = d->dn_year - 1;
    /* 365 days a year plus one on leap year, except every hundred years
    ** not divisible by 400.
    */
    days = 365 * yr - DAYS_SINCE_1970 + yr/4 - yr/100 + yr/400;

    /* Compute number of days in previous months */

    for (i = 1; i < d->dn_month; i++)
        days += adu_2monthsize((i4) i, (i4) d->dn_year);

    /* Always absolute date, do not need to look at high day */

    days += d->dn_day;

    /* Convert to minutes */
    sec = days*24*60;

    /* Make sure it does not overflow */
    maxval = MAXI4 - (d->dn_seconds)/60;
    minval = MINI4 + (d->dn_seconds)/60;

    /* if( sec > (MAXI4 - (d->dn_seconds)/60)) */
    if (sec > maxval)
	sec = MAXI4;
    if (sec < minval)
    /* else if( sec < (MINI4 - (d->dn_seconds)/60)) */
	sec = MINI4;
    else
	sec = sec*60 + d->dn_seconds;

    return( sec);
}   


/*
**  Name:  adu_6to_dtntrnl	- map a value of one of the supported types
**	to the ADF internal format
**
**  Description:
**
**  Parameters:
**      indv		- ptr to DB_DATA_VALUE for input value
**	outval		- ptr to AD_NEWDTNTRNL into which value is copied
**
**  Returns:
**      none
**
**  History
**	21-apr-06 (dougi)
**	    Written for date/time enhancements.
**	29-jun-2006 (gupsh01)
**	    Modified setting up the status bit and verifying.
**	19-jul-2006 (gupsh01)
**	    Modified setting up the vacant fields.
**	01-aug-2006 (gupsh01)
**	    Fixed memory alignment for ANSI date/time internal
**	    data.
**	02-aug-2006 (gupsh01)
**	    Fixed the copying of Timestamp without timezone 
**	    datatypes.
**	22-sep-06 (gupsh01)
**	    Even for empty values of ANSI date/times set the 
**	    flags so 0 is printed out.
**	16-sep-06 (gupsh01)
**	    Make sure that we check for absolute data type value.
**	20-oct-06 (gupsh01)
**	    Protect this routine from segv if the input and output
**	    pointers are uninitialized.
**	22-nov-06 (gupsh01)
**	    Modified so that the status flag is set only for non 
**	    zero values of date and time.
**	13-nov-06 (gupsh01)
**	    Fixed the time with time zone case and timestamp with
**	    local time zone.
*/
VOID
adu_6to_dtntrnl(
	DB_DATA_VALUE	*indv,
	AD_NEWDTNTRNL	*outval)

{
    DB_STATUS   	db_stat;
    PTR			datap;
    AD_DATENTRNL	ing_date;
    AD_ADATE		adate;
    AD_TIME		atime;
    AD_TIMESTAMP	atimestamp;
    AD_INTYM 		aintym;
    AD_INTDS 		aintds;

    if (indv && indv->db_data && outval)
    {
      datap = indv->db_data;
      outval->dn_dttype = abs(indv->db_datatype);
      outval->dn_status = 0;

      /* Just switch to the relevant conversion code and do it. */
      switch(abs(outval->dn_dttype)) 
      {
      case DB_DTE_TYPE:		/* old Ingres DATE */
	MEcopy((PTR)indv->db_data, ADF_DTE_LEN, (PTR)&ing_date);
	outval->dn_year	= ing_date.dn_year;
	outval->dn_month = ing_date.dn_month;
	outval->dn_day = ing_date.dn_lowday |
	     (ing_date.dn_highday << AD_21DTE_LOWDAYSIZE);
	outval->dn_seconds = ing_date.dn_time/AD_6DTE_IMSPERSEC;
	outval->dn_nsecond = 
		(ing_date.dn_time % AD_6DTE_IMSPERSEC) * AD_38DTE_NSPERMS;
	outval->dn_tzhour = outval->dn_tzminute = 0;
	outval->dn_status = ing_date.dn_status;
	break;

      case DB_ADTE_TYPE:	/* new standard DATE */
	MEcopy((PTR)indv->db_data, ADF_ADATE_LEN, (PTR)&adate);
	outval->dn_year	= adate.dn_year;
	outval->dn_month = adate.dn_month;
	outval->dn_day = adate.dn_day;
	outval->dn_seconds = outval->dn_nsecond = 
		outval->dn_tzhour = outval->dn_tzminute = 0;
	outval->dn_status = AD_DN_NULL; 
	if (outval->dn_year)
	  outval->dn_status |= AD_DN_YEARSPEC; 
	if (outval->dn_month)
	  outval->dn_status |= AD_DN_MONTHSPEC; 
	if (outval->dn_day)
	  outval->dn_status |= AD_DN_DAYSPEC; 

	if (outval->dn_status != AD_DN_NULL)
	  outval->dn_status |= AD_DN_ABSOLUTE; 

	break;

      case DB_TMW_TYPE:		/* TIME WITH TIME ZONE */
      case DB_TME_TYPE:		/* Ingres TIME (WITH LOCAL TIME ZONE) */
      case DB_TMWO_TYPE:	/* TIME WITHOUT TIME ZONE */
	MEcopy((PTR)indv->db_data, ADF_TIME_LEN, (PTR)&atime);
	if (abs(outval->dn_dttype) == DB_TMW_TYPE)
	{
	  outval->dn_tzhour = atime.dn_tzhour;
	  outval->dn_tzminute = atime.dn_tzminute;
	}
	else
	  outval->dn_tzhour = outval->dn_tzminute = 0;
        outval->dn_year = outval->dn_month = outval->dn_day = 0;
	outval->dn_seconds = atime.dn_seconds;
	outval->dn_nsecond = atime.dn_nsecond;
	outval->dn_status = AD_DN_ABSOLUTE; 
	if (outval->dn_seconds | outval->dn_nsecond)
	{
	  outval->dn_status |= AD_DN_TIMESPEC; 
	  outval->dn_status |= AD_DN_ABSOLUTE; 
	}
	break;

      case DB_TSW_TYPE:		/* TIMESTAMP WITH TIME ZONE */
      case DB_TSTMP_TYPE:	/* Ingres TIMESTAMP (WITH LOCAL TIME ZONE) */
      case DB_TSWO_TYPE:	/* TIMESTAMP WITHOUT TIME ZONE */
	MEcopy((PTR)indv->db_data, ADF_TSTMP_LEN, (PTR)&atimestamp);
	if (abs(outval->dn_dttype) == DB_TSW_TYPE)
 	{	
	  outval->dn_tzhour = atimestamp.dn_tzhour; 
	  outval->dn_tzminute = atimestamp.dn_tzminute;
 	}	
	else
	  outval->dn_tzhour = outval->dn_tzminute = 0;
	outval->dn_year	= atimestamp.dn_year;
	outval->dn_month = atimestamp.dn_month;
	outval->dn_day = atimestamp.dn_day;
	outval->dn_seconds = atimestamp.dn_seconds;
	outval->dn_nsecond = atimestamp.dn_nsecond;

	if (outval->dn_year)
	  outval->dn_status |= AD_DN_YEARSPEC; 
	if (outval->dn_month)
	  outval->dn_status |= AD_DN_MONTHSPEC; 
	if (outval->dn_day)
	  outval->dn_status |= AD_DN_DAYSPEC; 
	if (outval->dn_seconds | outval->dn_nsecond)
	  outval->dn_status |= AD_DN_TIMESPEC; 
	if (outval->dn_status != AD_DN_NULL)
	  outval->dn_status |= AD_DN_ABSOLUTE; 
	break;

      case DB_INYM_TYPE:	/* INTERVAL YEAR TO MONTH */
	MEcopy((PTR)indv->db_data, ADF_INTYM_LEN, (PTR)&aintym);
	outval->dn_year	= aintym.dn_years;
	outval->dn_month = aintym.dn_months;
	outval->dn_day = outval->dn_seconds = outval->dn_nsecond = 
		outval->dn_tzhour = outval->dn_tzminute = 0;
	  outval->dn_status = AD_DN_LENGTH; 
	if (outval->dn_year)
	  outval->dn_status |= AD_DN_YEARSPEC; 
	if (outval->dn_month)
	  outval->dn_status |= AD_DN_MONTHSPEC; 
	if (outval->dn_status != AD_DN_NULL)
	  outval->dn_status |= AD_DN_LENGTH; 
	break;

      case DB_INDS_TYPE:	/* INTERVAL DAY TO SECOND */
	MEcopy((PTR)indv->db_data, ADF_INTDS_LEN, (PTR)&aintds);
	outval->dn_day	= aintds.dn_days;
	outval->dn_seconds = aintds.dn_seconds;
	outval->dn_nsecond = aintds.dn_nseconds;
	outval->dn_year = outval->dn_month =
		outval->dn_tzhour = outval->dn_tzminute = 0;
	  outval->dn_status = AD_DN_LENGTH; 
	if (outval->dn_day)
	  outval->dn_status |= AD_DN_DAYSPEC; 
	if (outval->dn_seconds | outval->dn_nsecond)
	  outval->dn_status |= AD_DN_TIMESPEC; 
	if (outval->dn_status != AD_DN_NULL)
	  outval->dn_status |= AD_DN_LENGTH; 
	break;

      default:		/* error */
 	break;

      }
    }
}


/*
**  Name:  adu_7from_dtntrnl	- map a value of one of the supported types
**	from the ADF internal format to the storage format
**
**  Description:
**
**  Parameters:
**      outdv		- ptr to DB_DATA_VALUE for output value
**	inval		- ptr to AD_NEWDTNTRNL from which value is copied
**
**  Returns:
**      none
**
**  History
**	21-apr-06 (dougi)
**	    Written for date/time enhancements.
**	17-jun-06 (gupsh01)
**	    Fixed the DB_DTE_TYPE low/high day calculation.
**	01-aug-06 (gupsh01)
**	    Fixed time calculation for DB_DTE_TYPE considering
**	    the internal structure now holds nanoseconds.
**	08-sep-06 (gupsh01)
**	    Fixed the sigbus error calling this function.
**	16-sep-06 (gupsh01)
**	    Make sure that we check for absolute data type value.
**	20-oct-06 (gupsh01)
**	    Protect this routine from segv if the input and output
**	    pointers are uninitialized.
**	5-Nov-07 (kibro01) b119412
**	    Apply timezone and normalise since AD_DATENTRNL doesn't
**	    have a timezone, so it would get lost.
*/
VOID
adu_7from_dtntrnl(
	DB_DATA_VALUE	*outdv,
	AD_NEWDTNTRNL	*inval)

{
    PTR		datep;
    char	workday[4];
    char	*workp = &workday[0];
    AD_DATENTRNL	ing_date;
    AD_ADATE		adate;
    AD_TIME		atime;
    AD_TIMESTAMP	atimestamp;
    AD_INTYM 		aintym;
    AD_INTDS 		aintds;

    /* Switch on target data type and copy the fields over. */
    if (outdv && outdv->db_data && inval)
    {
      datep = outdv->db_data;

      switch (abs(outdv->db_datatype))
      {
      case DB_DTE_TYPE:
	/* If this is an absolute point in time, then apply any timezone
	** at this stage since we don't have a timezone in the
	** AD_DATENTRNL output field  (kibro01) b119412
	*/
	if (inval->dn_status & AD_DN_ABSOLUTE)
	{
	    inval->dn_seconds -= (inval->dn_tzhour * AD_39DTE_ISECPERHOUR
			+ inval->dn_tzminute * AD_10DTE_ISECPERMIN);
            adu_1normldate(inval);
	}
	ing_date.dn_year = inval->dn_year;
	ing_date.dn_month = inval->dn_month;
	ing_date.dn_highday = 
			(inval->dn_day >> AD_21DTE_LOWDAYSIZE);
	ing_date.dn_lowday = 
			(inval->dn_day & AD_22DTE_LOWDAYMASK);
	ing_date.dn_time = inval->dn_seconds * AD_6DTE_IMSPERSEC + 
					inval->dn_nsecond/AD_29DTE_NSPERMS;
	ing_date.dn_status = inval->dn_status;

	/* Make sure that the status field is 
	** consistent This will help fix up conversion to string 
	** If the date value may change due to various operations. 
	*/
	if (!(ing_date.dn_status & AD_DN_TIMESPEC) && ing_date.dn_time)
	  ing_date.dn_status |= AD_DN_TIMESPEC;
	if (!(ing_date.dn_status * AD_DN_YEARSPEC) && ing_date.dn_year)
	  ing_date.dn_status |= AD_DN_YEARSPEC;
	if (!(ing_date.dn_status * AD_DN_MONTHSPEC) && ing_date.dn_month)
	  ing_date.dn_status |= AD_DN_MONTHSPEC;
	if (!(ing_date.dn_status * AD_DN_DAYSPEC) && ing_date.dn_lowday)
	  ing_date.dn_status |= AD_DN_DAYSPEC;

	MEcopy((PTR)&ing_date, ADF_DTE_LEN, (PTR)datep);
	break;

      case DB_ADTE_TYPE:
	adate.dn_year = inval->dn_year;
	adate.dn_month = inval->dn_month;
	adate.dn_day = inval->dn_day;
	MEcopy((PTR)&adate, ADF_ADATE_LEN, (PTR)datep);
	break;

      case DB_TMW_TYPE:		/* TIME WITH TIME ZONE */
	atime.dn_tzhour = inval->dn_tzhour;
	atime.dn_tzminute = inval->dn_tzminute;
      case DB_TME_TYPE:		/* Ingres TIME (WITH LOCAL TIME ZONE) */
      case DB_TMWO_TYPE:	/* TIME WITHOUT TIME ZONE */
	if (outdv->db_datatype != DB_TMW_TYPE)
	{
	  atime.dn_tzhour = atime.dn_tzminute = 0;
	}
	atime.dn_seconds = inval->dn_seconds;
	atime.dn_nsecond = inval->dn_nsecond;
	MEcopy((PTR)&atime, ADF_TIME_LEN, (PTR)datep);
	break;

      case DB_TSW_TYPE:		/* TIMESTAMP WITH TIME ZONE */
	atimestamp.dn_tzhour = inval->dn_tzhour;
	atimestamp.dn_tzminute = inval->dn_tzminute;
      case DB_TSTMP_TYPE:	/* Ingres TIMESTAMP (WITH LOCAL TIME ZONE) */
      case DB_TSWO_TYPE:	/* TIMESTAMP WITHOUT TIME ZONE */
	if (outdv->db_datatype != DB_TSW_TYPE)
	{
	  atimestamp.dn_tzhour = atimestamp.dn_tzminute = 0;	  
	}
	atimestamp.dn_year = inval->dn_year;
	atimestamp.dn_month = inval->dn_month;
	atimestamp.dn_day = inval->dn_day;
	atimestamp.dn_seconds = inval->dn_seconds;
	atimestamp.dn_nsecond = inval->dn_nsecond;
	MEcopy((PTR)&atimestamp, ADF_TSTMP_LEN, (PTR)datep);
	break;

      case DB_INYM_TYPE:	/* INTERVAL YEAR TO MONTH */
	aintym.dn_years	= inval->dn_year;
	aintym.dn_months = inval->dn_month;
	MEcopy((PTR)&aintym, ADF_INTYM_LEN, (PTR)datep);
	break;

      case DB_INDS_TYPE:	/* INTERVAL DAY TO SECOND */
	aintds.dn_days	= inval->dn_day;
	aintds.dn_seconds = inval->dn_seconds;
	aintds.dn_nseconds = inval->dn_nsecond;
	MEcopy((PTR)&aintds, ADF_INTDS_LEN, (PTR)datep);
	break;

      default:
	break;
      }
    }
}
