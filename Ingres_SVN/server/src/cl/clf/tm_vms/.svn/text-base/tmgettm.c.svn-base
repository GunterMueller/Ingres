/*
** Copyright (c) 1986, 2000 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <tm.h>
#include    <tmtz.h>
#include    <starlet.h>

/**
**
**  Name: TMGETTM.C - get time
**
**  Description:
**      get time
**
{@func_list@}...
**
**
**  History:    $Log-for RCS$
**      15-sep-86 (seputis)
**          initial creation
[@history_template@]...
**      16-jul-93 (ed)
**	    added gl.h
**	19-jul-2000 (kinte01)
**	   Correct prototype definitions by adding missing includes &
*8	   external function references
**/


FUNC_EXTERN long int TMconv();


/*{
** Name: TMgettm	- Return the current time in a 'timeb' structure.
**
** Description:
**
**	Use the VMS function to get the time and then stuff 
** 	the appropriate parts of it into the appropriate parts
**	of the 'timeb' struct
**
** Inputs:
**
** Outputs:
**      tm                              timeb structure
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	10/4/83 (dd) -- Written for VMS CL
**	12/21/84 (kooi) -- changed timeadr from i4 to unsigned i4
**			since we need unsigned arithmetic when computing
**			millitm.
**      15-sep-86 (seputis)
**          initial creation
**	25-sep-92 (stevet)
**	    Add timezone adjustment using new TMtz routines.  TMconv
**	    no longer returns time in GMT (with timezone adjusted).
[@history_template@]...
*/
VOID
TMgettm(tm)
TMSTRUCT           *tm;
{
    u_i4	timeadr[2];
    PTR		tz_cb;
    STATUS	status;

    sys$gettim(timeadr);
    tm->time = TMconv(timeadr);	/* get seconds since Jan 1, 1970 */
    /* Adjust for timezone */
    status = TMtz_init(&tz_cb);
    if( status == OK)
        tm->time -= TMtz_search( tz_cb, TM_TIMETYPE_LOCAL, tm->time);
    /* get milliseconds since last second.  VMS gives 64 bit time as number
       of 100 nsec units since Nov 17, 1858.  We mod with 10 million to get
       number of 100 nsec units since last second, then convert to msec.  
       Since timeadr[1] is ignored, our mod is off by a constant number, but
       this should not matter */
    tm->millitm = (timeadr[0] % 10000000) / 10000;
    tm->timezone = 0;
    tm->dstflag = 0;
}

