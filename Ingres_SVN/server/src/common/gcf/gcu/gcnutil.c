
/*
** Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <erglf.h>
#include    <sl.h>
#include    <cv.h>
#include    <me.h>
#include    <sp.h>
#include    <mo.h>
#include    <qu.h>
#include    <si.h>
#include    <st.h>
#include    <tm.h>
#include    <iicommon.h>
#include    <gca.h>
#include    <gcn.h>
#include    <gcnint.h>
#include    <gcu.h>

/* 
**  INSERT_AUTOMATED_FUNCTION_PROTOTYPES_HERE
*/

/*
** Name: gcnutil.c
**
** Description:
**	Internal Name Server utility functions.
**
**	gcn_copy_str	Copy string.
**	gcn_put_tup	Put GCN_TUP.
**	gcn_get_tup	Get GCN_TUP.
**	gcn_getflag	Convert hex string to integer.
**
** History:
**	01-Mar-89 (seiwald)
**	    Created.
**	23-Apr-89 (jorge)
**	    Upgraded gcn_get_str() to bne sure that ALL strings are null
**	    terminated.
**	27-Apr-89 (jorge)
**	    Made GCN_TRACE a dummy constant so that this common module
**	    can be loaded into a VMS shared segment.
**	05-May-89 (GordonW)
**	    It is against the coding standards  to bury statics within a
**	    routine. They should reside at the top of the module.
**	27-Nov-89 (seiwald)
**	    Removed code which copied data from the stream into a static
**	    buffer.  Added gcn_copy_str() to copy data from the stream
**	    into the client's buffer.
**	24-Apr-91 (brucek)
**	    Added include for tm.h
**	14-Aug-91 (seiwald)
**	    Put a 'nat' after 'register' for OS/2.
**	17-aug-91 (leighb) DeskTop Porting Change:
**	    Added include for me.h and st.h;
**	    gcn_put_str, gcn_put_int, gcn_get_str, gcn_copy_str, 
**	    gcn_get_int, gcn_put_tup, gcn_get_tup, gcn_words must return nat.
**	22-Jul-92 (brucek)
**	    Corrected values returned by gcn_get_int, gcn_put_str,
**	    gcn_get_str, and gcn_copy_str.  All were computing returns
**	    based on sizeof (i4) rather than sizeof (i4).
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	25-aug-93 (ed)
**	    remove dbdbms.h
**	 4-Dec-95 (gordy)
**	    Added prototypes.
**	30-sep-1996 (canor01)
**	    Move gcn_getflags() from iigcn.c so it can be put into a
**	    library for exporting in NT.
**	11-Mar-97 (gordy)
**	    Moved gcn_get_int(), gcn_put_int(), gcn_get_str(), 
**	    gcn_put_str(), gcn_words() to gcu.
**	27-May-97 (thoda04)
**	    Included cv.h for the function prototypes.
**	26-apr-1999 (hanch04)
**	    Added casts to quite compiler warnings
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/


/*
** Name: gcn_copy_str() - copy var string from buffer.
**
** Description:
**	Copies the string from buffer p into s, null terminating s.
**
** Returns:
**	Amount of buffer used.
**
** History:
**	27-Nov-89 (seiwald)
**	    Written.
*/

i4
gcn_copy_str( char *p, char *s )
{
	i4	len;

	I4ASSIGN_MACRO( *p, len );

	MEcopy( p + sizeof( len ), len, s );
	if( !len || s[ len - 1 ] )
		s[ len ] = '\0';

	return sizeof( len ) + len;
}


/*
** Name: gcn_put_tup() - place a GCN_TUP into buffer
**
** Description:
**	Places GCN_TUP t (GCN_VAL strings uid, obj, and val)
**	into buffer p.
**
** Returns:
**	Amount of buffer used.
*/

i4
gcn_put_tup( char *p, GCN_TUP *t )
{
	char *pp = p;

	p += gcu_put_str( p, t->uid );
	p += gcu_put_str( p, t->obj );
	p += gcu_put_str( p, t->val );

	return p - pp;
}

/*
** Name: gcn_get_tup() - get a GCN_TUP from buffer
**
** Description:
**	Points GCN_TUP t's three GCN_VAL's at the next three strings
**	in buffer p.  
**
** Returns:
**	Amount of buffer used.
*/

i4
gcn_get_tup( char *p, GCN_TUP *t )
{
	char *pp = p;

	p += gcu_get_str( p, &t->uid );
	p += gcu_get_str( p, &t->obj );
	p += gcu_get_str( p, &t->val );

	return p - pp;
}


/*{
** Name: gcn_getflag() - MO get method function for flag value
**
** Description:
**	Outputs a hex string flag value in decimal.
**
** History:
**      19-Nov-92 (brucek)
**          Created.
*/

STATUS 
gcn_getflag( i4  offset, i4  size, PTR object, i4  lsbuf, char *sbuf )
{
    STATUS	stat = OK;
    u_i4	ival = 0;
    char	**cobj = (char **)(object + offset);

    if ( (stat = CVahxl( *cobj, &ival )) == OK )
	stat = MOlongout( MO_VALUE_TRUNCATED, ival, lsbuf, sbuf );

    return( stat );
}
