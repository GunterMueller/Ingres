/*
** Copyright (c) 2004 Ingres Corporation
*/

# include	<compat.h>
# include	<gl.h>
# include    	<cm.h>
# include       <me.h>
# include	<sl.h>
# include	<bt.h>      
# include	<iicommon.h>
# include	<adf.h>
# include	<ulf.h>      
# include	<adfint.h>      
# include       <aduint.h>

/**
** Name:	adubitwise.c - Logical routines manipulating byte type(s)
**
** Description:
**	This file contains the logical routines which operate on the byte datatypes.
**
** This file defines:
**
**	adu_bit_add()	Provide a bitwise ADD on the two byte operands.
**	adu_bit_and()	Provide a bitwise AND on the two byte operands.
**	adu_bit_not()	Provide a bitwise NOT on the byte operand.
**	adu_bit_or()	Provide a bitwise OR on the two byte operands.
**	adu_bit_xor()	Provide a bitwise XOR on the two byte operands.
**	adu_ipaddr()	Convert an IP address string to a Int4
**	adu_intextract() Provide an integer from the selected byte
**
** History:
**	05-oct-1998 (shero03)
**	    Created.
**	04-feb-2000 (gupsh01)
**	    Added format checking for the ipaddress in adu_ipaddr. returns 
**	    Bad_IP_ADDRESS error if illegal format is found. (bug 100265)
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      31-May-2001 (hweho01)
**          Changed tptr and lptr from char pointer to uchar pointer in
**          adu_bit_add(), fix Bug #104698. 
**/

/* Internal Routines */
		
/*{
** Name:	adu_bit_prepop - Prepare Byte Operands
**
** Description:
**	This routine determines which operands is shorter.
**	Then moves the shorter to the result field padded with nulls on the right
**
** Inputs:
**	dv1		Pointer to DB_DATA_VALUE describing an existing byte type
**	dv2		Pointer to DB_DATA_VALUE describing an existing byte type
**	rdv		Pointer to DB_DATA_VALUE describing memory into which
**				the resultant byte string should be put.
**
** Outputs:
**	rdv		Contains the shorter
**
** Returns:
**	The address of the longer operand.
**
** History:
**	06-oct-1998 (shero03)
**	    Created.
*/
static char *
adu_bit_prepop(
DB_DATA_VALUE	*dv1,
DB_DATA_VALUE	*dv2,
DB_DATA_VALUE	*rdv)
{
    char	*sptr, *lptr, *tptr;
    i4		slen, llen, tlen;
    DB_STATUS	status = E_DB_OK;

    tlen = rdv->db_length;
    tptr = rdv->db_data;

    /* Find out the shorter operand and set sptr to it, lptr to other */

    if (dv1->db_length > dv2->db_length)
    {
        slen = dv2->db_length;
        sptr = (char*)dv2->db_data;
        llen = dv1->db_length;
        lptr = (char*)dv1->db_data;
    }
    else
    {
        slen = dv1->db_length;
        sptr = (char*)dv1->db_data;
        llen = dv2->db_length;
        lptr = (char*)dv2->db_data;
    }
	
    if (tlen == llen)  /* the target is always the size of the larger operand */
    {
    	if (slen == llen)
	    MEcopy(sptr, tlen, tptr);
	else
	{
	    /* Copy the operand to the right and NULL pad */
	    i4  diflen = llen - slen;
	    MEfill(diflen, 0x00, tptr);
            MEcopy(sptr, tlen - diflen, tptr + diflen);
	}
    }
    else
    	lptr = NULL;

    return(lptr);
}

/*{
** Name:	adu_bit_add - Logically ADD two byte types
**
** Description:
**	This routine produces the logical ADD of the two byte operands.
**	If the operands are different lengths, then the shorter is padded
**	with nulls on the left.
**
** Note:  There are additional improvements that could be made later,
**	by using integer addition in i4  sized pieces.  However, using integers
**	creates Big and Little Edian concerns.  This technique is simple, 
**	straight-forward, explainable, and slow.
**
** Re-entrancy:
**      This function is re-entrant.
**
** Inputs:
**	adf_scb		Pointer to ADF session control block
**	dv1		Pointer to DB_DATA_VALUE describing an existing byte type
**	dv2		Pointer to DB_DATA_VALUE describing an existing byte type
**	rdv		Pointer to DB_DATA_VALUE describing memory into which
**				the resultant byte string should be put.
**
** Outputs:
**	rdv		Byte string result is supplied.
**
** Returns:
**	DB_STATUS
**
** Exceptions: None.
**
** Side Effects: None.
**
** History:
**	06-oct-1998 (shero03)
**	    Created.
*/
DB_STATUS
adu_bit_add(
ADF_CB          *adf_scb,
DB_DATA_VALUE	*dv1,
DB_DATA_VALUE	*dv2,
DB_DATA_VALUE	*rdv)
{
    uchar *lptr, *tptr;
    i4		llen, tlen;
    i4		i, carry;
    i4		i3;
    DB_STATUS	status = E_DB_OK;

    for (;;)
    {
	if ((dv1->db_datatype == DB_BYTE_TYPE) &&
	    (dv2->db_datatype == DB_BYTE_TYPE) &&
	    (rdv->db_datatype == DB_BYTE_TYPE))
	{}
	else
	{
	    status = adu_error(adf_scb, E_AD9999_INTERNAL_ERROR, 0);
	    break;
	}
	
        lptr = adu_bit_prepop(dv1, dv2, rdv);
	if (lptr == NULL)
	{
	    status = adu_error(adf_scb, E_AD9999_INTERNAL_ERROR, 0);
	    break;
	}
	
	tlen = rdv->db_length;
	tptr = rdv->db_data;

	/* point to the rightmost byte */
	tptr += tlen - 1;
	lptr += tlen - 1;
	for (i = tlen, carry = 0; i--; tptr--, lptr--)
	{
	    i3 = *tptr + *lptr + carry;
	    carry = (i3 > 255) ? 1 : 0;
	    *tptr = (char)i3;
	}		      

	break;
    }	/* LOOP */

    return(status);
}

/*{
** Name:	adu_bit_and - Logically AND two byte types
**
** Description:
**	This routine produces the logical AND of the two byte operands.
**	If the operands are a different length, then the shorter is padded
**	with nulls on the right.
**
** Re-entrancy:
**      This function is re-entrant.
**
** Inputs:
**	adf_scb		Pointer to ADF session control block
**	dv1		Pointer to DB_DATA_VALUE describing an existing byte type
**	dv2		Pointer to DB_DATA_VALUE describing an existing byte type
**	rdv		Pointer to DB_DATA_VALUE describing memory into which
**				the resultant byte string should be put.
**
** Outputs:
**	rdv		Byte string result is supplied.
**
** Returns:
**	DB_STATUS
**
** Exceptions: None.
**
** Side Effects: None.
**
** History:
**	05-oct-1998 (shero03)
**	    Created.
*/
DB_STATUS
adu_bit_and(
ADF_CB          *adf_scb,
DB_DATA_VALUE	*dv1,
DB_DATA_VALUE	*dv2,
DB_DATA_VALUE	*rdv)
{
    char	*lptr, *tptr;
    i4		tlen;
    DB_STATUS	status = E_DB_OK;

    for (;;)
    {
	if ((dv1->db_datatype == DB_BYTE_TYPE) &&
	    (dv2->db_datatype == DB_BYTE_TYPE) &&
	    (rdv->db_datatype == DB_BYTE_TYPE))
	{}
	else
	{
	    status = adu_error(adf_scb, E_AD9999_INTERNAL_ERROR, 0);
	    break;
	}
	
        lptr = adu_bit_prepop(dv1, dv2, rdv);
	if (lptr == NULL)
	{
	    status = adu_error(adf_scb, E_AD9999_INTERNAL_ERROR, 0);
	    break;
	}
	
	tlen = rdv->db_length;
	tptr = rdv->db_data;

	BTand(tlen * BITSPERBYTE, lptr, tptr); 
	break;
    }	/* LOOP */

    return(status);
}

/*{
** Name:	adu_bit_or - Logically OR two byte types
**
** Description:
**	This routine produces the logical OR of the two byte operands.
**	If the operands are a different length, then the shorter is padded
**	with nulls on the right.
**
** Re-entrancy:
**      This function is re-entrant.
**
** Inputs:
**	adf_scb		Pointer to ADF session control block
**	dv1		Pointer to DB_DATA_VALUE describing an existing byte type
**	dv2		Pointer to DB_DATA_VALUE describing an existing byte type
**	rdv		Pointer to DB_DATA_VALUE describing memory into which
**				the resultant byte string should be put.
**
** Outputs:
**	rdv		Byte string result is supplied.
**
** Returns:
**	DB_STATUS
**
**
** Exceptions: None.
**
** Side Effects: None.
**
** History:
**	05-oct-1998 (shero03)
**	    Created.
*/
DB_STATUS
adu_bit_or(
ADF_CB          *adf_scb,
DB_DATA_VALUE	*dv1,
DB_DATA_VALUE	*dv2,
DB_DATA_VALUE	*rdv)
{
    char	*lptr, *tptr;
    i4		tlen;
    DB_STATUS	status = E_DB_OK;

    for (;;)
    {
	if ((dv1->db_datatype == DB_BYTE_TYPE) &&
	    (dv2->db_datatype == DB_BYTE_TYPE) &&
	    (rdv->db_datatype == DB_BYTE_TYPE))
	{}
	else
	{
	    status = adu_error(adf_scb, E_AD9999_INTERNAL_ERROR, 0);
	    break;
	}
	
        lptr = adu_bit_prepop(dv1, dv2, rdv);
	if (lptr == NULL)
	{
	    status = adu_error(adf_scb, E_AD9999_INTERNAL_ERROR, 0);
	    break;
	}
	
	tlen = rdv->db_length;
	tptr = rdv->db_data;

	BTor(tlen * BITSPERBYTE, lptr, tptr); 
	break;
    }	/* LOOP */

    return(status);
}

/*{
** Name:	adu_bit_xor - Logically XOR two byte types
**
** Description:
**	This routine produces the logical XOR of the two byte operands.
**	If the operands are a different length, then the shorter is padded
**	with nulls on the right.
**
** Re-entrancy:
**      This function is re-entrant.
**
** Inputs:
**	adf_scb		Pointer to ADF session control block
**	dv1		Pointer to DB_DATA_VALUE describing an existing byte type
**	dv2		Pointer to DB_DATA_VALUE describing an existing byte type
**	rdv		Pointer to DB_DATA_VALUE describing memory into which
**				the resultant byte string should be put.
**
** Outputs:
**	rdv		Byte string result is supplied.
**
** Returns:
**	DB_STATUS
**
** Exceptions: None.
**
** Side Effects: None.
**
** History:
**	05-oct-1998 (shero03)
**	    Created.
*/
DB_STATUS
adu_bit_xor(
ADF_CB          *adf_scb,
DB_DATA_VALUE	*dv1,
DB_DATA_VALUE	*dv2,
DB_DATA_VALUE	*rdv)
{
    char	*lptr, *tptr;
    i4		tlen;
    DB_STATUS	status = E_DB_OK;

    for (;;)
    {
	if ((dv1->db_datatype == DB_BYTE_TYPE) &&
	    (dv2->db_datatype == DB_BYTE_TYPE) &&
	    (rdv->db_datatype == DB_BYTE_TYPE))
	{}
	else
	{
	    status = adu_error(adf_scb, E_AD9999_INTERNAL_ERROR, 0);
	    break;
	}
	
        lptr = adu_bit_prepop(dv1, dv2, rdv);
	if (lptr == NULL)
	{
	    status = adu_error(adf_scb, E_AD9999_INTERNAL_ERROR, 0);
	    break;
	}
	
	tlen = rdv->db_length;
	tptr = rdv->db_data;

	BTxor(tlen * BITSPERBYTE, lptr, tptr); 
	break;
    }	/* LOOP */

    return(status);
}

/*{
** Name:	adu_bit_not - Logically NOT one byte type
**
** Description:
**	This routine produces the logical NOT of the byte operand.
**
** Re-entrancy:
**      This function is re-entrant.
**
** Inputs:
**	adf_scb		Pointer to ADF session control block
**	dv1		Pointer to DB_DATA_VALUE describing an existing byte type
**	rdv		Pointer to DB_DATA_VALUE describing memory into which
**				the resultant byte string should be put.
**
** Outputs:
**	rdv		Byte string result is supplied.
**
** Returns:
**	DB_STATUS
**
** Exceptions: None.
**
** Side Effects: None.
**
** History:
**	05-oct-1998 (shero03)
**	    Created.
*/
DB_STATUS
adu_bit_not(
ADF_CB          *adf_scb,
DB_DATA_VALUE	*dv,
DB_DATA_VALUE	*rdv)
{
    char	*lptr, *tptr;
    i4		llen, tlen;
    DB_STATUS	status = E_DB_OK;

    for (;;)
    {
	if ((dv->db_datatype == DB_BYTE_TYPE) &&
	    (rdv->db_datatype == DB_BYTE_TYPE))
	{}
	else
	{
	    status = adu_error(adf_scb, E_AD9999_INTERNAL_ERROR, 0);
	    break;
	}
	

	llen = dv->db_length;
	lptr = (char*)dv->db_data;
	tlen = rdv->db_length;
	tptr = rdv->db_data;

	if (tlen != llen)
	{
	    status = adu_error(adf_scb, E_AD9999_INTERNAL_ERROR, 0);
	    break;
	}

	MEcopy(lptr, llen, tptr);
	BTnot(tlen * BITSPERBYTE, tptr); 
		    	      
	break;
    }	/* LOOP */

    return(status);
}

/*{
** Name:	adu_ii_ipaddr - Convert an IP address (string) to a byte 
**
** Description:
**	This routine converts a character IP address to a binary form
**	Each set of digits is converted to a single byte.
**	For instance, 141.202.36.10 -> 0x8DCA240A
**
** Re-entrancy:
**      This function is re-entrant.
**
** Inputs:
**	adf_scb		Pointer to ADF session control block
**	dv 		Pointer to DB_DATA_VALUE describing an IP address
**	rdv		Pointer to DB_DATA_VALUE describing memory into which
**				the resultant byte string should be put.
**
** Outputs:
**	rdv		Byte string result is supplied.
**
** Returns:
**	DB_STATUS
**
** Exceptions: None.
**
** Side Effects: None.
**
** History:
**	07-oct-1998 (shero03)
**	    Created.
**      04-feb-2000 (gupsh01)
**          Added format checking for the ipaddress in adu_ipaddr. returns
**          Bad_IP_ADDRESS error if illegal format is found. (bug 100265)
*/
DB_STATUS
adu_ipaddr(
ADF_CB          *adf_scb,
DB_DATA_VALUE	*dv,
DB_DATA_VALUE	*rdv)
{
    DB_STATUS           db_stat;
    i4			i;
    i4			lensrc;
    unsigned char	ipbyte;
    char		*srcptr, *destptr, *endsrc;

    if ((rdv->db_datatype != DB_BYTE_TYPE) ||
    	(rdv->db_length != AD_LEN_IPADDR)) 
        return (adu_error(adf_scb, E_AD5001_BAD_STRING_TYPE, 0));
    
    if ((dv->db_datatype == DB_CHR_TYPE) ||
    	(dv->db_datatype == DB_CHA_TYPE) ||
	(dv->db_datatype == DB_TXT_TYPE) ||
	(dv->db_datatype == DB_VCH_TYPE))
    {}
    else
        return (adu_error(adf_scb, E_AD5001_BAD_STRING_TYPE, 0));
    	
    if (db_stat = adu_lenaddr(adf_scb, dv, &lensrc, &srcptr))
	return (db_stat);
    if (db_stat = adu_3straddr(adf_scb, rdv, &destptr))
	return (db_stat);

    endsrc = srcptr + lensrc;
    
    for (i = ipbyte = 0; (srcptr < endsrc) && (i < AD_LEN_IPADDR); CMnext(srcptr))
    {
    	if (CMdigit(srcptr)) 
	{	
	    if (ipbyte == 0)
	        ipbyte = *srcptr - '0';
	    else
	    {   
		if (((ipbyte * 10) + (*srcptr - '0')) > 255 )
		    return (adu_error(adf_scb, E_AD5008_BAD_IP_ADDRESS, 0)); 
	        ipbyte = (ipbyte * 10) + (*srcptr - '0');
	    }
	}
	else if (CMoper(srcptr))
	{	
	    if (*srcptr == '.')
	    {	
		*destptr = ipbyte;
	        destptr++; i++;
		ipbyte = 0;
	    }
	    else
		return (adu_error(adf_scb, E_AD5008_BAD_IP_ADDRESS, 0));
        }
	else
	    return (adu_error(adf_scb, E_AD5008_BAD_IP_ADDRESS, 0)); 
    }
    if (i < AD_LEN_IPADDR)
        *destptr = ipbyte;
    if (i != (AD_LEN_IPADDR - 1))
	 return (adu_error(adf_scb, E_AD5008_BAD_IP_ADDRESS, 0));    
    return (E_DB_OK);
}

/*{
** Name:	adu_intextract - Extract an integer from a byte string 
**
** Description:
**	This routine picks a specified byte from a byte string 
**	and returns it as an integer.  If the index is out of range, then 
**	a 0 is returned.  This routine returns a 2 byte integer. 
**
** Re-entrancy:
**      This function is re-entrant.
**
** Inputs:
**	adf_scb		Pointer to ADF session control block
**	dv1 		Pointer to DB_DATA_VALUE describing an IP address
**	dv2 		Pointer to DB_DATA_VALUE describing an IP address
**	rdv		Pointer to DB_DATA_VALUE describing memory into which
**				the resultant integer should be put.
**
** Outputs:
**	rdv		Byte string result is supplied.
**
** Returns:
**	DB_STATUS
**
** Exceptions: None.
**
** Side Effects: None.
**
** History:
**	09-oct-1998 (shero03)
**	    Created.
**      23-may-2007 (smeke01) b118342/b118344
**	    Work with i8.
*/
DB_STATUS
adu_intextract(
ADF_CB          *adf_scb,
DB_DATA_VALUE	*dvbyte,
DB_DATA_VALUE	*dvint,
DB_DATA_VALUE	*rdv)
{
    DB_STATUS           db_stat;
    unsigned char	*s;
    i4		    	slen, n, r;
    i8			i8n;
    	
    if ((db_stat = adu_3straddr(adf_scb, dvbyte, (char**)&s)) != E_DB_OK)
	return (db_stat);
    if ((db_stat = adu_5strcount(adf_scb, dvbyte, &slen)) != E_DB_OK)
	return (db_stat);

    switch (dvint->db_length)
    {
    	case 1:
    	  n = I1_CHECK_MACRO(*(i1*)dvint->db_data);
	  break;
    	case 2:
    	  n = *(i2*)dvint->db_data;
	  break;
 	case 4:
    	  n = *(i4*)dvint->db_data;
	  break;
    	case 8:
	  i8n = *(i8 *)dvint->db_data;

	  /* limit to i4 values */
	  if ( i8n > MAXI4 || i8n < MINI4LL )
		return (adu_error(adf_scb, E_AD9998_INTERNAL_ERROR, 2, 0, "intextract count overflow"));

	  n = (i4) i8n;
	  break;
	default:
	  return (adu_error(adf_scb, E_AD9998_INTERNAL_ERROR, 2, 0, "intextract count length"));
    }

    if (n < 0 || n > slen)
    	r = 0;	
    else
    	r = s[n-1];

    *(i2*)rdv->db_data = r;

    return (E_DB_OK);
}
