/*
** Copyright (c) 2004 Ingres Corporation
*/

#include <compat.h>
#include <gl.h>
#include <ci.h>
#include <gc.h>
#include <mu.h>
#include <st.h>

#include <iicommon.h>
#include <gca.h>
#include <gcaint.h>
#include <gcu.h>

/*
**
** Name: gcucode.c
**
** Description:
**	GCF utility functions for encryption.
**
**          gcu_encode		Encrypt a string.
**
** History:
**	26-Feb-97 (gordy)
**	    Extracted from GCN.
**	26-apr-1999 (hanch04)
**	    Added casts to quite compiler warnings
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	01-aug-2001 (somsa01)
**	    Cleaned up compiler warnings.
*/

/*
** CI requires string lengths to be a multiple of
** a base block size.  The CL spec defines this to
** be CRYPT_SIZE, but this is not defined in CL
** header files.  CI defines the value locally
** and also uses hard coded values.
*/
# define CRYPT_SIZE		8

# define GCN_PASSWD_END		'0'



/*
** Name: gcu_encode
**
** Description:
**	This routine encrypts a string.  Uses the inverse algorithm
**	of gcn_decrypt() so that passwords encrypted using this
**	routine are acceptable for Name Server usage.
**
** Inputs:
**	key		Encryption key.  Max length GC_L_USER_ID.
**	str		String to encrypt.  Max length (GC_L_PASSWORD/2) - 1.
**
** Outputs:
**	buff		Encrypted string.  Max length GC_L_PASSWORD.
**
** Returns:
**	STATUS		OK or FAIL.
**
** History:
**	26-Feb-97 (gordy)
**	    Extracted from GCN.
*/

STATUS
gcu_encode( char *key, char *str, char *buff )
{
    CI_KS	ksch;
    i4		len;
    char	kbuff[ GC_L_USER_ID ];
    char	ibuff[ GC_L_PASSWORD ];
    char	tmp[ GC_L_PASSWORD ];

    if ( ! key  ||  ! buff )  return( FAIL );

    if ( ! str  ||  ! *str )
    {
    	*buff = EOS;
	return( OK );
    }

    /* 
    ** Coerce key to length CRYPT_SIZE.
    */
    if ( (len = (i4)STlength( key )) > (GC_L_USER_ID - 1) )
	return(FAIL);

    STcopy( key, kbuff );
    for ( ; len < CRYPT_SIZE; len++ )  kbuff[ len ] = ' ';
    kbuff[ CRYPT_SIZE ] = EOS;

    /*
    ** Coerce encryption string to multiple of CRYPT_SIZE.
    */
    if ( (len = (i4)STlength( str )) > (GC_L_PASSWORD - 1) )
	return( FAIL );
    
    if ( len > (GC_L_PASSWORD/2 - 1) )
	return( FAIL );

    /* Add trailing marker to handle strings with trailing SPACE */
    STcopy( str, ibuff );
    ibuff[ len++ ] = GCN_PASSWD_END;
	
    /* Pad string with spaces to modulo CRYPT_SIZE length. */
    while( len % CRYPT_SIZE )  ibuff[ len++ ] = ' ';
    ibuff[ len ] = EOS;

    /*
    ** Now encrypt the string.
    */
    CIsetkey( kbuff, ksch );
    CIencode( ibuff, len, ksch, tmp );
    CItotext( (u_char *)tmp, len, buff );

    return( OK );
}
