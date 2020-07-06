/*
** Copyright (c) 1987, 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>

#include    <gcccl.h>
#include    <cv.h>		 
#include    <cvgcc.h>
#include    <me.h>
#include    <qu.h>
#include    <tm.h>
#include    <tr.h>

#include    <sl.h>
#include    <iicommon.h>
#include    <adf.h>
#include    <gca.h>
#include    <gcocomp.h>
#include    <gcc.h>
#include    <gccpci.h>
#include    <gccpl.h>
#include    <gccgref.h>

/*
**
**  Name: gccalcv.c - perform data conversion for the AL
**
**  These routines exist to do the work of perform_conversion in
**  special cases by the GCC AL.  These special cases should be
**  removed.
**
**  External entry points:
**	GCn{get,add}{nat,long,s} - get and put data in NTS
**
**  History:
**      12-Feb-90 (seiwald)
**	    Collected from gccpl.c and gccutil.c.
**	14-Mar-94 (seiwald)
**	    Extracted from the defunct gccpdc.c.
**	24-mar-95 (peeje01)
**	    Integrate doublebyte changes from 6500db_su4_us42
**	20-Nov-95 (gordy)
**	    Added prototypes.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	12-Dec-02 (gordy)
**	    Renamed gcd component to gco.
**	12-May-03 (gordy)
**	    Changed frag parameter in gco_do_16src() to return fragment
**	    type using DOC status.
**	20-Apr-04 (gordy)
**	    CV header files cleaned up.  NAT/LONGNAT replaced by I4.
*/


/*
** Name: GCn{get,add}{nat,long,s} - get and put data in NTS
**
** Description:
**
**	GCngetnat - get a i4  from NTS stream
**	GCnaddnat - put a i4  into NTS stream
**	GCngetlong - get a i4 from NTS stream
**	GCnaddlong - put a i4 into NTS stream
**	GCngets - get a string from NTS stream
**	GCnadds - put a string into NTS stream
**
** 	These routines convert between C datatypes and NTS (network 
**	transfer syntax). 
**
** Inputs:
**	src	pointer to source C datatype (add)
**		pointer to NTS stream (get)
**	dst	pointer to destination C datatype (get)
**		pointer to NTS stream (add)
**	len	length of source string (add)
**		length of destination string (get)
** Output:
**	status	failure status - set only upon failure!
**
** Returns:
**	number of source bytes used (get)
**	number of destination bytes used (add)
**
** History:
**	26-Oct-89 (seiwald)
**	    Written.
**	15-Feb-90 (seiwald)
**	    Rebuild upon CV macros.
**	6-Apr-92 (seiwald)
**	    Added support for doublebyte mappings, using macros for
**	    speed.
**	24-mar-95 (peeje01)
**	    Integrate doublebyte changes from 6500db_su4_us42
**	20-Nov-95 (gordy)
**	    Added prototypes.
**	12-May-03 (gordy)
**	    Changed frag parameter in gco_do_16src() to return fragment
**	    type using DOC status.
*/

i4
GCngetnat( char *src, i4  *dst, STATUS *status )
{
	CV2L_I4_MACRO( src, dst, status );
	return CV_N_I4_SZ;
}

i4
GCnaddnat( i4  *src, char *dst, STATUS *status )
{
	CV2N_I4_MACRO( src, dst, status );
	return CV_N_I4_SZ;
}

i4
GCngetlong( char *src, i4 *dst, STATUS *status )
{
	CV2L_I4_MACRO( src, dst, status );
	return CV_N_I4_SZ;
}

i4
GCnaddlong( i4 *src, char *dst, STATUS *status )
{
	CV2N_I4_MACRO( src, dst, status );
	return CV_N_I4_SZ;
}

i4
GCnadds( char *src, i4  len, char *dst )
{
	u_char  *usrc = (u_char *)src;
	u_char  *udst = (u_char *)dst;
	u_char	*odst = (u_char *)dst;
	u_char	*x2y;
	u_i2	*x2y16;
	
# ifdef DOUBLEBYTE
	if( x2y16 = ((GCC_CSET_XLT *)IIGCc_global->gcc_cset_xlt)->l2n16 )
	{
	    i4  dstlim = 32767;
	    i4  vec = len;
	    i4  vec_out = 0;
	    i4  len_adjust = 0;
	    i1  frag = DOC_IDLE;

	    while( len )
		gco_do_16src( x2y16, &usrc, &len, &udst, &dstlim, &vec,
			      &vec_out, 0, &len_adjust, &frag );
	}
	else
# endif /* DOUBLEBYTE */
	{
	    x2y = ((GCC_CSET_XLT *)IIGCc_global->gcc_cset_xlt)->l2n;

	    while( len-- )
	    {
		*udst = x2y[ *usrc ];
		usrc += CV_L_CHAR_SZ;
		udst += CV_N_CHAR_SZ;
	    }
	}

	return udst - odst;
}

i4
GCngets( char *src, i4  len, char *dst )
{
	u_char	*osrc = (u_char *)src;
	u_char	*usrc = (u_char *)src;
	u_char	*udst = (u_char *)dst;
	u_char	*x2y;
	u_i2	*x2y16;
	
# ifdef DOUBLEBYTE
	if( x2y16 = ((GCC_CSET_XLT *)IIGCc_global->gcc_cset_xlt)->n2l16 )
	{
	    i4  dstlim = 32767;
	    i4  vec = len;
	    i4  vec_out = 0;
	    i4  len_adjust = 0;
	    i1  frag = DOC_IDLE;

	    while( len )
		gco_do_16src( x2y16, &usrc, &len, &udst, &dstlim, &vec,
			      &vec_out, 0, &len_adjust, &frag );
	}
	else
# endif /* DOUBLEBYTE */
	{
	    x2y = ((GCC_CSET_XLT *)IIGCc_global->gcc_cset_xlt)->n2l;

	    while( len-- )
	    {
		*udst = x2y[ *usrc ];
		usrc += CV_N_CHAR_SZ;
		udst += CV_L_CHAR_SZ;
	    }
	}

	return usrc - osrc;
}

