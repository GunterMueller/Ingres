# include	<compat.h>
# include	<cm.h>
# include	<cv.h>
# include	<st.h>


/*
** Name: BS_tcp_port - turn a tcp port name into a tcp port number
**
** Description:
**	This routines provides mapping from II_INSTALLATION into a
**	unique tcp port number for the installation.  This scheme
**	was developed by the NCC (an RTI committee).
**
**	If pin is of the form:
**		XY
**	or	XY#
**	where
**		X is [a-zA-Z]
**		Y is [a-zA-Z0-9]
**	and	# is [0-7]
**
**	then pout is the string representation of the decimal number:
**
**        15  14  13  12  11  10  09  08  07  06  05  04  03  02  01  00
**       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
**       ! 0 ! 1 !  low 5 bits of X  !   low 6 bits of Y     !     #     !
**       +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
**
**	If # is not provided, then the subport parameter is used.  This
**	allows programmatic permutations of the XY form of the port name.
**	If subport > 0 and port is not the XY form, fail.
**
**	If pin is not of the form XY or XY#; copy pin to pout.
**
** Inputs:
**	portin - source port name
**	subport - added as offset to port number
**	portout - resulting port name 
**
** History
**	23-jun-89 (seiwald)
**	    Written.
**	28-jun-89 (seiwald)
**	    Use wild CM macros instead of the sane but defunct CH ones.
**	2-feb-90 (seiwald)
**	    New subport parameter, so we can programmatically generate
**	    successive listen ports to use on listen failure.
**	24-May-90 (seiwald)
**	    Renamed from GC_tcp_port.
**	27-may-97 (mcgem01)
**	    Cleaned up compiler warnings.
**      06-aug-1999 (mcgem01)
**          Replace nat and longnat with i4.
**	20-Jul-2004 (lakvi01)
**		SIR #112703, cleaned-up warnings.
**      26-Jul-2004 (lakvi01)
**          Backed-out the above change to keep the open-source stable.
**          Will be revisited and submitted at a later date. 
*/

STATUS
BS_tcp_port( pin, subport, pout )
char	*pin;
i4 	subport;
char	*pout;
{
	if( CMalpha( &pin[0] ) && ( CMalpha( &pin[1] ) | CMdigit( &pin[1] ) ) )
	{
		long	portid;
		char	p[ 3 ];

		if( subport > 7 || subport && pin[2] )
			return FAIL;

		CMtoupper( &pin[0], &p[0] );
		CMtoupper( &pin[1], &p[1] );
		p[2] = CMdigit( &pin[2] ) ? pin[2] : subport;

		portid = 1 << 14 
			| ( p[0] & 0x1f ) << 9
			| ( p[1] & 0x3f ) << 3
			| ( p[2] & 0x07 );

		CVla( portid, pout );

		return OK;
	} else {
		if( subport )
			return FAIL;

		STcopy( pin, pout );

		return OK;
	}
}
