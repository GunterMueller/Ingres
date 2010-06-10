/*
**    Copyright (c) 1989, 2005 Ingres Corporation
**
*/

#include    <compat.h>
#include    <gl.h>
#include    <st.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ulf.h>
#include    <ulm.h>
#include    <cs.h>
#include    <adf.h>
#include    <add.h>
#include    <scf.h>
#include    <dmf.h>
#include    <dmtcb.h>
#include    <dmrcb.h>
#include    <dmucb.h>
#include    <gwf.h>
#include    <gwfint.h>
#include    <rms.h>
#include    <cm.h>
#include    <descrip.h>
#include    "gwrms.h"
#include    "gwrmsdt.h"
#include    <cv.h>

/*
** Name: GW02RMS.C	- internal support functions for the RMS Gateway
**
** Description:
**	This file contains a number of internal support functions for use
**	by the RMS Gateway. In particular, it contains functions which
**	parse the extended format strings from the REGISTER TABLE command
**	which describe the particular attributes of particular fields in
**	the RMS record.
**
**	NOTE:  The assumption is made that any error returned from these
**	routines is a user error (i.e., due to bad syntax in extended format
**	string).  The caller of this routine will directly send the returned
**	error code & formatted message to the frontend; no further message
**	logging will be done.
**
**	Routines:
**
**	gw02__xfmt	- parse extended format string
**	gw02__fmt	- parse normal format strings
**	gw02__getprms	- get offset, length or precision, and scale params
**			    for extended format specifications
**	gw02__padlen	- find length of RMS string before trailing pad chars.
**	gw02__padstr	- pad string out to length requested.
**
** History:
**	17-jan-90 (linda)
**	    Created.
**	17-apr-90 (bryanp)
**	    Updated error handling, added 'error' parameter to functions.
**	    Added some comments throughout the file..
**	21-jun-90 (linda)
**	    Bug fixes to extended format parsing; correct error detection;
**	    put in range checks for length, scale, & precision; fix length
**	    calculations for different types without extended formats; change
**	    some types (varying strings no longer get pad qualifiers, float
**	    types get their own RMS Gateway type id RMSGW_NATFLOAT so that we
**	    can provide desired coercions).
**	26-jun-90 (linda)
**	    Varying strings again get blankpad/nullpad qualifiers.  Allow for
**	    RMSGW_BL_VARSTR and RMSGW_NL_VARSTR datatypes, because even though
**	    they are treated the same as the fixed types for data conversion
**	    purposes, we need to remember what the user specified when we write
**	    out to a varying record length file.
**	5-jul-90 (linda)
**	    Changes to gw02__fmt(), so that it provides the proper defaults
**	    from Ingres column types to RMS field types.
**	5-jul-90 (linda)
**	    Changes to gw02__fmt(), gw02__xfmt(), so that they report errors to
**	    the user while we still have specific information to pass back up
**	    (i.e., no default coercion for this type; or bad datatype specified
**	    in extended format string).  Changed the name of gw02__getmods to
**	    gw02__getprms for clarity.
**	7-sep-90 (linda)
**	    Added support for "trailing separate numeric" datatype.  This type
**	    is a string type, one byte per ascii digit, with a separate sign
**	    byte at the end.  It is generated by COBOL PIC statements.  Its
**	    internal type definition is RMSGW_TRL_NUM.
**      18-may-92 (schang)
**          GW merge.
**      06-nov-92 (schang)
**          add dmrcb before dmtcb  
**      16-nov-92 (schang)
**          prototyping. ANSI conforming
**	11-aug-93 (ed)
**	    unnest dbms.h
**      07-jun-93 (schang)
**          add extended format datatype "imaginary" for repeating group 
**          support
**      24-aug-93 (ed)
**          added missing includes
**      26-feb-97 (schang)
**          add s_ and t_ type float support for alpha vms.  Also, explicitly
**          define f_, d_, g_, and h_folat for VAX, invalidate NATFLOAT and
**          EXTFLOAT  
**      03-mar-1999 (chash01)
**          integrate previous changes.
**	28-feb-2001	(kinte01)
**	    Bug 103393 - removed nat, longnat, u_nat, & u_longnat
**	    from RMS GW code as the use is no longer allowed
**	24-feb-2005 (abbjo03)
**	    Remove include of obsolete gwxit.h.
**  16-Jun-2009 (thich01)
**      Treat GEOM type the same as LBYTE.
**  20-Aug-2009 (thich01)
**      Treat all spatial types the same as LBYTE.
**	12-feb-2010 (abbjo03, wanfr01) Bug 123139
**	    cv.h needs to be added for function definitions
*/

/*
**	Forward function declarations
*/
static DB_STATUS gw02__getprms();



/*
** Name: GWTYP_NAM	- definition of a RMS datatype
**
** Description:
**	This structure is used to map an RMS datatype name to an internal
**	Ingres datatype, size, and other goodies (precision, scale...)
**
** History:
**	17-apr-90 (bryanp)
**	    Documented.
*/
typedef struct
{
    char    *name;
		    /*
		    ** 'name' is the name of the RMS datatype. These names
		    ** correspond to names which users give in the REGISTER
		    ** TABLE command.
		    */
    int	    type_len;
		    /*
		    ** 'type_len' is the Ingres internal length which this type
		    ** should have. If 0, the type must be explicitly provided
		    ** in the REGISTER TABLE command.
		    */
    int	    type_id;
		    /*
		    ** 'type_id' is the Ingres internal datatype which this RMS
		    ** type is to be converted to. In many cases, these
		    ** datatypes are not native Ingres types; rather, they are
		    ** specially handled through ADF's User Defined Datatypes
		    ** facility.
		    */
    int	    nparms;
		    /*
		    ** 'nparms' is the number of additional parameters which
		    ** this RMS datatype specifier may have. Additional
		    ** parameters are used to provide explicit length and
		    ** scale values for datatypes which need them.
		    */
}   GWTYP_NAM;

/*
** Name: GWTYP_TRN	- Type translations
**
** Description:
**	This structure is used to translate incompletely-specified types into
**	their fully qualified type with default modifiers applied.
**
** History:
**	17-apr-90 (bryanp)
**	    Documented.
*/
typedef struct
{
    char    *name;
    char    *nrml_name;
}   GWTYP_TRN;

/*
**  Structure trans_typ is a translation table from type names without modifiers
**  to their normalized form -- i.e. with default modifiers applied.
*/

static	GWTYP_TRN   trans_typ[] =
{
    {"byte",		    "signed byte"},
    {"word",		    "signed word"},
    {"longword",	    "signed longword"},
    {"quadword",	    "signed quadword"},
    {"octaword",	    "signed octaword"},
    {"string",		    "blankpad fixed string"},
    {"fixed string",	    "blankpad fixed string"},
    {"varying string",	    "blankpad varying string"},
    {"trailing numeric",    "trailing separate numeric"},
    {"",		""}
};

/*
*	Structure norml_typ represents the normalized form of all legal RMS
**	Gateway-specific datatypes.  Normalized == whitespace in multi-word
**	types is collapsed to a single blank, and type modifiers are prepended
**	(i.e., type "word" without signed/unsigned modifier gets the default
**	modifier "signed" so the normalized typename is "signed word").
*/

static	GWTYP_NAM   norml_typ[] =
{
    {"signed byte",			1,	RMSGW_INT,	    1},
    {"unsigned byte",			1,	RMSGW_UNSINT,	    1},
    {"signed word",			2,	RMSGW_INT,	    1},
    {"unsigned word",			2,	RMSGW_UNSINT,	    1},
    {"signed longword",			4,	RMSGW_INT,	    1},
    {"unsigned longword",		4,	RMSGW_UNSINT,	    1},
    {"signed quadword",			8,	RMSGW_INT,	    1},
    {"unsigned quadword",		8,	RMSGW_UNSINT,	    1},
    {"signed octaword",			16,	RMSGW_INT,	    1},
    {"unsigned octaword",		16,	RMSGW_UNSINT,	    1},
/*
**  feb-26_97 (schang) with alpha platform, these definitions have to be
**     changeded.
**    {"d_floating",			8,	RMSGW_NATFLOAT,	    1},
**    {"f_floating",			4,	RMSGW_NATFLOAT,	    1},
**    {"g_floating",			8,	RMSGW_EXTFLOAT,	    1},
**    {"h_floating",			16,	RMSGW_EXTFLOAT,	    1},
*/
    {"d_floating",			8,	RMSGW_DFLOAT,	    1},
    {"f_floating",			4,	RMSGW_FFLOAT,	    1},
    {"g_floating",			8,	RMSGW_GFLOAT,	    1},
    {"h_floating",			16,	RMSGW_HFLOAT,	    1},
    {"s_floating",			4,	RMSGW_SFLOAT,	    1},
    {"t_floating",			8,	RMSGW_TFLOAT,	    1},
    
    {"blankpad fixed string",		0,	RMSGW_BL_FIXSTR,    2},
    {"nullpad fixed string",		0,	RMSGW_NL_FIXSTR,    2},
    {"blankpad varying string",		0,	RMSGW_BL_VARSTR,    2},
    {"nullpad varying string",		0,	RMSGW_NL_VARSTR,    2},
    {"vax date",			8,	RMSGW_VAXDATE,	    1},
    {"packed decimal",			0,	RMSGW_PACKED_DEC,   3},
    {"unsigned numeric",		0,	RMSGW_UNS_NUM,	    3},
    {"leading numeric",			0,	RMSGW_LEADING_NUM,  3},
    {"zoned numeric",			0,	RMSGW_ZONED_NUM,    3},
    {"overpunched numeric",		0,	RMSGW_OVR_NUM,	    3},
    {"trailing separate numeric",	0,	RMSGW_TRL_NUM,	    3},
    /* 
    ** schang : for repeating group support
    */
    {"imaginary",                       0,      RMSGW_IMAGINE,      0},
    {"",				-1,	-1,		    -1}
};


/*{
** Name: gw02__xfmt
**
** Description:
**
**	This routine is internal to the RMS Gateway.  It is called from
**	gw02_tabf() at table registration time, to parse and interpret the
**	extended format string (if provided for this attribute, determined
**	by gw02_tabf() before the call).
**
**	gw02__xfmt() parses the string and puts appropriate values into the
**	fields of the gwatt structure, values to be stored in the tuple of the
**	iigw02_attribute extended catalog.
**
** Inputs:
**	gwatt	    DMU_GWATTR_ENTRY, passed down from DMF and contains the
**		    extended format string associated with this attribute (in
**		    gwat_xbuffer).
**	xatt	    GWRMS_XATT, this is the structure that represents the
**		    iigw02_attribute tuple which must be filled in according
**		    to the information in the extended format string, if
**		    provided.
**
** Output:
**	xatt	    The fields of this structure are filled in to reflect
**		    interpretation of the extended format string.  The values
**		    will be stored in the iigw02_attribute extended catalog.
**	error->err_code		Detailed error code if an error occurred:
**				E_GW54A0_WRONG_NUM_PARMS
**				E_GW54A1_NO_SUCH_DATATYPE
**				E_GW54A3_BAD_XFMT_PARAM
**
**      Returns:
**	    E_DB_OK
**          E_DB_ERROR	cannot perform the operation
**
** History:
**	17-jan-90 (linda)
**	    Created.
**	17-apr-90 (bryanp)
**	    Added error handling, 'error' parameter.
**	29-may-90 (edwin)
**	    Fixed calculation of RMS Leading Sign Numeric precision.
**	13-jun-90 (edwin)
**	    Set length, precision, scale before checking them, rather than
**	    vice-versa.
**	18-jun-90 (linda)
**	    Bug fixing, also revamp so that errors are reported accurately.
**	21-jun-90 (linda)
**	    Remove nullpad/blankpad modifiers from varying string type, since
**	    we never extend varying strings on output to rms -- except in one
**	    case, that of sequential files with varying length records; here
**	    we will reject longer records, but pad shorter records with blanks.
**	    Both types were set to corresponding fixed string types; I just
**	    picked the blankpad fixed type.  We really should define a new rms
**	    type for varying.
**	5-jul-90 (linda)
**	    Put back RMS types for blankpad fixed, blankpad varying, nullpad
**	    fixed and nullpad varying.  We have to remember what the user
**	    specified in certain situations.  Add calls to gw02_send_error() to
**	    report "user" errors from this level, so that we can send specific
**	    information about the error in the extended format syntax.  NOTE:
**	    gw02_log_error() is used if the gw02_send_error() call fails;
**	    however, gw02_log_error() does not take parameterized messages.
**	    This is probably okay, since it is an unexpected situation and also
**	    the types of errors we are dealing with are not severe, so lost
**	    information is not crucial.  Made "string" a legal type specifier,
**	    becomes blankpad fixed string.
**	7-sep-90 (linda)
**	    Added support for RMSGW_TRL_NUM (trailing separate numeric)
**	    datatype.
*/
DB_STATUS
gw02__xfmt
(
    GWRMS_XATT          *xatt,
    DMU_GWATTR_ENTRY    *gwatt,
    DB_ERROR            *error
)
{
    char	*xfmt = &gwatt->gwat_xbuffer[0];
    char	*xptr;
    char	xbuf1[RMS_MAXLIN+1];
    char	xbuf2[RMS_MAXLIN+1];
    char	nrmlbuf[RMS_MAXLIN+1];   /* need a symbolic constant */
    i4		len;
    int		i;
    char	*cptr;
    i4		nwrd = RMS_MAXWRDS; /* So STgetwords knows when to stop. */
    i4		nprm = RMS_MAXPRMS; /* So gw02__getprms knows when to stop. */
    char	*wrd[RMS_MAXWRDS];
    i4		prm[RMS_MAXPRMS];
    STATUS	cvstat;
    DB_STATUS	status;
    int		precision = 0;
    int		scale = 0;

    error->err_code = E_DB_OK;

    /*
    **	Type names in extended format descriptions can be >1 word.  The current
    **	list of allowed types is:
    **	    (see norml_type array, above).
    **	Also, some types can have type modifiers.  Valid type modifiers for
    **	numbers are "signed" or "unsigned"; these apply to the following types:
    **	    byte, word, longword, quadword, octaword
    **	The "unsigned" modifier can also apply to numeric (==numeric string).
    **	Other numeric string types are leading, overpunched, zoned, trailing
    **	    separate
    **	The type modifiers "nullpad" and "blankpad" apply to type:
    **	    fixed string, varying string
    **	Defaults (no type modifier specified) are:
    **	    signed (for numbers) and blankpad (for strings)
    */

    /*
    **	First grab the tokens.  The list of delimiters is blank, tab, left
    **	paren, comma, right paren.
    */

    len = STlength(xfmt);
    STlcopy(xfmt,xbuf1,len);
    STlcopy(xfmt,xbuf2,len);
    CVlower(xbuf1);
    cptr = &xbuf1[0];
    STgetwords(cptr, &nwrd, wrd);

    if ((nwrd <= 0) || (nwrd > RMS_MAXWRDS))
    {
	if (gw02_send_error(E_GW54A6_BAD_FORMAT_STRING, 1,
		    (i4)sizeof(nrmlbuf), (PTR)nrmlbuf) == E_DB_OK)
	{
	    error->err_code = E_GW050E_ALREADY_REPORTED;
	}
	else
	{
	    error->err_code = E_GW54A1_NO_SUCH_DATATYPE;
	    gw02_log_error(error->err_code, (struct FAB *)NULL,
			   (struct RAB *)NULL, (ULM_RCB *)NULL,
			   (SCF_CB *)NULL);
	}
	return(E_DB_ERROR);
    }

    /*
    **	Any token beginning with an alphabetic character is part of the type
    **	specification; concatenate them into a single string separated by
    **	single blanks.
    */
    for (i=0; (wrd[i] && CMalpha(wrd[i])); i++)
    {
	if (cptr=STindex(wrd[i], "(", 0))
	{
	    *cptr='\0';
	}
    }

    nwrd = i;
    for (i=0; i<nwrd; i++)
    {
	if (i==0)
	    STcopy(wrd[0], nrmlbuf);
	else
	    STpolycat(3, nrmlbuf, " ", wrd[i], nrmlbuf);
    }

    /*
    **	Now "normalize" the type name by translating it into its default
    **	modified counterpart (e.g., "byte" becomes "signed byte").
    */
    for (i=0; *trans_typ[i].name; i++)
    {
	if (!STcompare(nrmlbuf, trans_typ[i].name))
	{
	    STcopy(trans_typ[i].nrml_name, nrmlbuf);
	    break;
	}
    }

    for (i=0; *norml_typ[i].name; i++)
	if (!STcompare(nrmlbuf, norml_typ[i].name))
	    break;

    /* Now see if we found a valid normalized type identifier */
    if (norml_typ[i].type_id == -1)
    {
	if (gw02_send_error(E_GW54A1_NO_SUCH_DATATYPE, 1,
		    (i4)sizeof(nrmlbuf), (PTR)nrmlbuf) == E_DB_OK)
	{
	    error->err_code = E_GW050E_ALREADY_REPORTED;
	}
	else
	{
	    error->err_code = E_GW54A1_NO_SUCH_DATATYPE;
	    gw02_log_error(error->err_code, (struct FAB *)NULL,
			   (struct RAB *)NULL, (ULM_RCB *)NULL,
			   (SCF_CB *)NULL);
	}
	return(E_DB_ERROR);
    }

    /*
    **	Get the offset[, length][, scale] parameters.
    */
    if (norml_typ[i].type_id == RMSGW_IMAGINE)
    {
        nprm = 0;
    }
    else
    { 
        if ((cptr = STindex(xbuf2, "(", 0)) == NULL)
        {
	    if (gw02_send_error(E_GW54A6_BAD_FORMAT_STRING, 1,
	     	        (i4)sizeof(nrmlbuf), (PTR)nrmlbuf) == E_DB_OK)
	    {
	        error->err_code = E_GW050E_ALREADY_REPORTED;
	    }
	    else
	    {
	        error->err_code = E_GW54A1_NO_SUCH_DATATYPE;
	        gw02_log_error(error->err_code, (struct FAB *)NULL,
			   (struct RAB *)NULL, (ULM_RCB *)NULL,
			   (SCF_CB *)NULL);
	    }
	    return(E_DB_ERROR);
        }

        if ((status = gw02__getprms(cptr, &nprm, prm, error)) != E_DB_OK)
        {
	    /*
            ** gw02__getprms() has set the appropriate error code into the error
	    ** parameter. Return to our caller, who will log the error and reject
            ** the REGISTER TABLE command.
	    **
	    ** NOTE, we "know" that gw02__getprms() can only set error->err_code to
	    ** E_DB_OK or E_GW54A2_INVALID_INTEGER.  If that changes, the logic
	    ** here must change.
	    */
	    if (gw02_send_error(E_GW54A2_INVALID_INTEGER, 1,
		    (i4)sizeof(nrmlbuf), (PTR)nrmlbuf) == E_DB_OK)
	    {
	        error->err_code = E_GW050E_ALREADY_REPORTED;
	    }
	    else
	    {
	        error->err_code = E_GW54A2_INVALID_INTEGER;
	        gw02_log_error(error->err_code, (struct FAB *)NULL,
	         		   (struct RAB *)NULL, (ULM_RCB *)NULL,
		        	   (SCF_CB *)NULL);
	    }
	    return(E_DB_ERROR);
        }
    }
    /*
    **	We have a valid type identifier and a parameter list.  Set the type id,
    **	offset, length, and scale appropriately. The length and scale may have
    **	been provided explicitly on the REGISTER TABLE command. If so, nprm
    **	will be checked against the table to see if explicit values may legally
    **	be provided, and the explicit values will be used. If no explicit
    **	values were provided, defaults are used from the table.
    */

    status = E_DB_OK;
    error->err_code = E_DB_OK;

    if (nprm != norml_typ[i].nparms)
    {
	if (gw02_send_error(E_GW54A0_WRONG_NUM_PARMS, 3,
		    (i4)sizeof(nprm), (PTR)&nprm,
		    (i4)sizeof(nrmlbuf), (PTR)nrmlbuf,
		    (i4)sizeof(norml_typ[i].nparms),
		    (PTR)&norml_typ[i].nparms) == E_DB_OK)
	{
	    error->err_code = E_GW050E_ALREADY_REPORTED;
	}
	else
	{
	    error->err_code = E_GW54A0_WRONG_NUM_PARMS;
	    gw02_log_error(error->err_code, (struct FAB *)NULL,
			   (struct RAB *)NULL, (ULM_RCB *)NULL,
			   (SCF_CB *)NULL);
	}
	return(E_DB_ERROR);
    }

    /* set type id */
    xatt->gtype = norml_typ[i].type_id;

    /* set offset */
    if (norml_typ[i].type_id != RMSGW_IMAGINE)
    {
        if ((prm[0] < 0) || (prm[0] > 32767))
        {
	    if (gw02_send_error(E_GW54A3_BAD_XFMT_PARAM, 2,
		    (i4)sizeof(prm[0]), (PTR)&prm[0],
		    (i4)sizeof(nrmlbuf), (PTR)nrmlbuf) == E_DB_OK)
	    {
	        error->err_code = E_GW050E_ALREADY_REPORTED;
	    }
	    else
	    {
	        error->err_code = E_GW54A3_BAD_XFMT_PARAM;
	        gw02_log_error(error->err_code, (struct FAB *)NULL,
			   (struct RAB *)NULL, (ULM_RCB *)NULL,
			   (SCF_CB *)NULL);
	    }
	    return(E_DB_ERROR);
        }
        else
        {
	    xatt->goffset = prm[0];
        }
    }
    else
    {
        /* old goffset is correctly set for RMSGW_IMAGINE */ 
    }

    /* initialize prec_scale to 0, may not be set below. */
    xatt->gprec_scale = 0;

    /* set length[, precision if applicable] */
    if (nprm > 1)
    {
	switch (xatt->gtype)
	{
	    case RMSGW_PACKED_DEC:
		precision = prm[1];
		if ((precision < 1) || (precision > 31))
		{
		    error->err_code = E_GW54A3_BAD_XFMT_PARAM;
		    break;
		}
		xatt->glength = (prm[1]/2)+1;
		break;
	    case RMSGW_LEADING_NUM:
	    case RMSGW_TRL_NUM:
		precision = prm[1];
		if ((precision < 1) || (precision > 31))
		{
		    error->err_code = E_GW54A3_BAD_XFMT_PARAM;
		    break;
		}
		xatt->glength = prm[1]+1;
		break;
	    case RMSGW_UNS_NUM:
	    case RMSGW_ZONED_NUM:
	    case RMSGW_OVR_NUM:
		precision = prm[1];
		if ((precision < 1) || (precision > 31))
		{
		    error->err_code = E_GW54A3_BAD_XFMT_PARAM;
		    break;
		}
		xatt->glength = prm[1];
		break;
	    default:
		/*
		** NOTE:  should check max. length against max.
		** record length ...
		*/
		xatt->glength = prm[1];
		if (xatt->glength < 1)
		{
		    error->err_code = E_GW54A3_BAD_XFMT_PARAM;
		    break;
		}
		break;
	}		    
	/* NOTE:  only E_GW54A3... or E_DB_OK can occur at this point. */
	if (error->err_code == E_GW54A3_BAD_XFMT_PARAM)
	{
	    if (gw02_send_error(E_GW54A3_BAD_XFMT_PARAM, 2,
		    (i4)sizeof(prm[1]), (PTR)&prm[1],
		    (i4)sizeof(nrmlbuf), (PTR)nrmlbuf) == E_DB_OK)
	    {
		error->err_code = E_GW050E_ALREADY_REPORTED;
	    }
	    else
	    {
		gw02_log_error(error->err_code, (struct FAB *)NULL,
			   (struct RAB *)NULL, (ULM_RCB *)NULL,
			   (SCF_CB *)NULL);
	    }
	    return(E_DB_ERROR);
	}
    }
    else    /* no length given, use implicit length. */
    {
	xatt->glength = norml_typ[i].type_len;
    }

    /* Check that specified length + offset won't exceed max. allowed */
    if ((xatt->glength + xatt->goffset) > 32767)
    {
	if (gw02_send_error(E_GW54A5_BAD_RECORD_LENGTH, 3,
		    (i4)sizeof(xatt->glength), (PTR)&xatt->glength,
		    (i4)sizeof(xatt->goffset), (PTR)&xatt->goffset,
		    (i4)sizeof(nrmlbuf), (PTR)nrmlbuf) == E_DB_OK)
	{
	    error->err_code = E_GW050E_ALREADY_REPORTED;
	}
	else
	{
	    error->err_code = E_GW54A5_BAD_RECORD_LENGTH;
	    gw02_log_error(error->err_code, (struct FAB *)NULL,
			   (struct RAB *)NULL, (ULM_RCB *)NULL,
			   (SCF_CB *)NULL);
	}
	return(E_DB_ERROR);
    }

    /* set scale if applicable */
    if (nprm > 2)
    {
	scale = prm[2];
	if ((scale < 0) || (scale > 31) || (scale > precision))
	{
	    if (gw02_send_error(E_GW54A3_BAD_XFMT_PARAM, 2,
		    (i4)sizeof(prm[2]), (PTR)&prm[2],
		    (i4)sizeof(nrmlbuf), (PTR)nrmlbuf) == E_DB_OK)
	    {
		error->err_code = E_GW050E_ALREADY_REPORTED;
	    }
	    else
	    {
		error->err_code = E_GW54A3_BAD_XFMT_PARAM;
		gw02_log_error(error->err_code, (struct FAB *)NULL,
			   (struct RAB *)NULL, (ULM_RCB *)NULL,
			   (SCF_CB *)NULL);
	    }
	    return(E_DB_ERROR);
	}
    }

    /*
    ** Put scale, precision info into xatt->gprec_scale field.  Note that these
    ** variables were initialized to 0 so this will work for all cases.
    */
    xatt->gprec_scale = DB_PS_ENCODE_MACRO(precision, scale);

    return(status);
}

/*{
** Name: gw02__fmt
**
** Description:
**
**	This routine is internal to the RMS Gateway.  It is called from
**	gw02_tabf() at table registration time, to place appropriate values in
**	the xatt structure to represent correct types in the iigw02_attribute
**	extended catalog.  This routine is called if there is no extended
**	format specification for this attribute.
**
** Inputs:
**	gwatt	    DMU_GWATTR_ENTRY, passed down from DMF and contains the
**		    extended format string associated with this attribute (in
**		    gwat_xbuffer).
**	xatt	    GWRMS_XATT, this is the structure that represents the
**		    iigw02_attribute tuple which must be filled in according
**		    to the information in the extended format string, if
**		    provided.
**
** Output:
**	xatt	    The fields of this structure are filled in to reflect
**		    interpretation of the extended format string.  The values
**		    will be stored in the iigw02_attribute extended catalog.
**	error->err_code		Detailed error code if an error occurred:
**				CURRENTLY, THIS ROUTINE CANNOT FAIL.
**
**      Returns:
**	    E_DB_OK
**          E_DB_ERROR	cannot perform the operation
**
** History:
**	17-jan-90 (linda)
**	    Created.
**	17-apr-90 (bryanp)
**	    Updated to enhance error handling. Added error parm, which is not
**	    currently used.
**	21-jun-90 (linda)
**	    Correct length calculations, type assignments for different data
**	    types.
**	5-jul-90 (linda)
**	    Added code to produce the correct defaults from Ingres column types
**	    to RMS field types.  Also added error parameters for routine
**	    gw02_send_error() and modified that routine, so users will get more
**	    informative messages.
**	12-feb-91 (linda)
**	    Changed error handling so that a status of E_DB_ERROR is returned
**	    even if the error is of type "already reported".  Otherwise the
**	    table will be created even if there was an error encountered while
**	    performing this mapping.
*/
DB_STATUS
gw02__fmt
(
    GWRMS_XATT      *xatt,
    DMT_ATT_ENTRY   *att,
    DB_ERROR        *error
)
{
    DB_STATUS	status = E_DB_OK;

    error->err_code = E_DB_OK;

    /*
    ** Subtract 2 from RMS length if Ingres type is text or varchar.
    */
    switch (att->att_type)
    {
	case	DB_TXT_TYPE:
	case	DB_VCH_TYPE:
	case	DB_VBYTE_TYPE:
	    xatt->glength = att->att_width - 2;
	    break;
	case	DB_LVCH_TYPE:
	case	DB_LBYTE_TYPE:
	case	DB_GEOM_TYPE:
        case    DB_POINT_TYPE:
        case    DB_MPOINT_TYPE:
        case    DB_LINE_TYPE:
        case    DB_MLINE_TYPE:
        case    DB_POLY_TYPE:
        case    DB_MPOLY_TYPE:
	    xatt->glength = att->att_width;
            break;
	default:
	    xatt->glength = att->att_width;
	    break;
    }

    /*
    ** Set precision and scale directly from the Ingres type.
    */
    xatt->gprec_scale = att->att_prec;

    /*
    ** RMS string type is same as our CHAR type, no 2-byte length indicator.
    */
    switch (att->att_type)
    {
	case	DB_LVCH_TYPE:
	case	DB_BYTE_TYPE:
	case	DB_VBYTE_TYPE:
	case	DB_LBYTE_TYPE:
	case	DB_GEOM_TYPE:
        case    DB_POINT_TYPE:
        case    DB_MPOINT_TYPE:
        case    DB_LINE_TYPE:
        case    DB_MLINE_TYPE:
        case    DB_POLY_TYPE:
        case    DB_MPOLY_TYPE:
	    xatt->gtype = RMSGW_NL_FIXSTR;
	    break;

	case	DB_TXT_TYPE:
	case	DB_VCH_TYPE:
	case	DB_CHA_TYPE:
	case	DB_CHR_TYPE:
	    xatt->gtype = RMSGW_BL_FIXSTR;
	    break;

	case	DB_INT_TYPE:
	    xatt->gtype = RMSGW_INT;
	    break;

	case	DB_FLT_TYPE:
#ifdef VAX
            if (att->att_width == 4)
	        xatt->gtype = RMSGW_FFLOAT;
            else
	        xatt->gtype = RMSGW_DFLOAT;
  
#else /* alpha vms */
            if (att->att_width == 4)
	        xatt->gtype = RMSGW_SFLOAT;
            else
	        xatt->gtype = RMSGW_TFLOAT;
#endif
	    break;

	case	DB_DTE_TYPE:
	    xatt->gtype = RMSGW_VAXDATE;
	    xatt->glength = 8;
	    break;

	case	DB_DEC_TYPE:
	    /*
	    ** Can't actually generate this type in Ingres until 6.5 -- but
	    ** then the default will be RMS packed decimal, so just default it.
	    */
	    xatt->gtype = RMSGW_PACKED_DEC;
	    break;

	case	DB_MNY_TYPE:
	    /*
	    ** No default for money type.  Report the error to the user and
	    ** return with a 'user' error.  If there's an error trying to
	    ** report it to the user, log the error and set return code
	    ** appropriately.  Money is currently the only Ingres type with no
	    ** corresponding RMS default; however, that could change so the
	    ** message is parameterized with the type name.
	    */
	    if (gw02_send_error(E_GW54A4_RMS_NO_DEFAULT, 1,
			(i4)sizeof("money")-1, (PTR)"money") == E_DB_OK)
	    {
		error->err_code = E_GW050E_ALREADY_REPORTED;
	    }
	    else
	    {
		error->err_code = E_GW54A4_RMS_NO_DEFAULT;
		gw02_log_error(error->err_code, (struct FAB *)NULL,
			       (struct RAB *)NULL, (ULM_RCB *)NULL,
			       (SCF_CB *)NULL);
	    }
	    status = E_DB_ERROR;
	    break;

	default:
	    xatt->gtype = att->att_type;
	    break;
    }

    return(status);
}

/*{
**  gw02__getprms
**
**	Internal function for parsing the extended format string in the RMS
**	Gateway "register table" command.  This function gets the offset,
**	length and scale parameters provided in the extended format string for
**	an attribute.  Parameters:
**
**	    string  --	    pointer to string to be parsed for offset, [length],
**			    [scale] parameters.  This string is located in the
**			    original extended format string by locating the'('
**			    character in the format.
**	    count   --	    set by this routine before returning to the number
**			    of parameters found and placed into parmarray.
**	    parmarray	--  address of an array of integers to hold the values
**			    of the offset, [length], [scale] parameters found.
**
**	    error->err_code Detailed error code if an error occurred:
**			    E_GW54A2_INVALID_INTEGER
**
**  History:
**
**	26-jan-90   (linda)
**	    Written.
**	17-apr-90 (bryanp)
**	    Added error parameter and enhanced error handling.
**	8-jul-90 (linda)
**	    Changed its name from gw02__getmods to gw02__getprms.
*/
static DB_STATUS
gw02__getprms
(
    char      *string,
    i4        *count,
    i4	      parmarray[],
    DB_ERROR  *error
)
{
    char	*delim1 = "(";		/* delimiters other than whitespace */
    char	*delim2 = ",";
    char	*delim3 = ")";
    char	*cur;
    i4		counter=0;
    char	numbuf[RMS_MAXNMLN];
    char	*nptr;
    i4		i;
    bool	end_xfmt = FALSE;
    STATUS	cvstat;
    DB_STATUS	status = E_DB_OK;

    /*
    ** Note:  we are guaranteed by the calling routine that the first character
    ** is "(".
    */

    cur = string;

    while (*cur && counter <= *count)
    {
	nptr = &numbuf[0];

	while (CMwhite(cur) || !CMcmpnocase(cur, delim1) ||
	       !CMcmpnocase(cur, delim2) || !CMcmpnocase(cur, delim3))
	{
	    if (!CMcmpnocase(cur, delim3))
	    {
		end_xfmt = TRUE;
		break;	/* end of extended format string */
	    }
	    else
	    {
		CMnext(cur);
	    }
	}

	if (end_xfmt == TRUE)
	    break;

	if (!CMdigit(cur))
	{
/***FIXME***ERROR IN EXTENDED FORMAT STRING***NUMBER EXPECTED***/
	    return(E_DB_ERROR);
	}
	else
	{
	    for (i = 0; CMdigit(cur) && i < RMS_MAXNMLN; i++)
	    {
		CMcpychar(cur, nptr);
		CMnext(cur);
		CMnext(nptr);
	    }
	    if (i >= RMS_MAXNMLN)
	    {
/***FIXME***ERROR IN EXTENDED FORMAT STRING***NUMBER TOO LONG***/
		return(E_DB_ERROR);
	    }
	    else
	    {
		CMcpychar("", nptr);	/* add terminating null */
	    }
	}

	cvstat = CVal(numbuf, (i4 *)&parmarray[counter]);
	if (cvstat != OK)
	{
	    /*
	    ** One of the offset, length, or scale arguments was not a valid
	    ** integer. We don't really care to decide which, just report that
	    ** an invalid integer was encountered.
	    */
	    error->err_code = E_GW54A2_INVALID_INTEGER;
	    status = E_DB_ERROR;
	    break;
	}
	counter++;
    }

    *count = counter;

    return(status);
}

/**
** Name:	gw02__padlen -- Find length of string before padding.
**
** Description:
**	Find length of string before padding.  Pad character is passed in by
**	caller.  The caller will decide whether to write out to the length
**	before padding, or to the length with padding, based on user's extended
**	format specification (or default) of string type (nullpadd/blankpad
**	varying/fixed string), and on the RMS file and record type.
**
** Inputs:
**	string	    a character pointer to the string
**	pad_char    the pad character (presently limited to either a null
**		    character or a blank)
**	maxlen	    the maximum length of the string to be checked.
**
** Output:
**		    none.
**
**      Returns:
**		    the trimmed length of the string, for use by the caller
**		    when writing out to a varying string type at the end of a
**		    record, when the file is non-sequential and allows varying
**		    record lengths.  Otherwise the caller will write out the
**		    whole, padded string.
**
** History:
**  26-jun-90 (linda)
**	Used by RMS Gateway to find trailing pad characters and mark the length
**	of the string up to that point.  At present only space or null chars
**	are used for padding -- but this routine won't char when and if the set
**	is expanded.
**/

i4
gw02__padlen
(
    char  *string,
    char  pad_char,
    i4    maxlen
)
{
    char    *strptr;
    i4	    len;
    i4	    newlen;
    i4	    i;

    len = 0;
    newlen = 0;
    strptr = string;

    for (i = 0; i < maxlen; i++)
    {
	len++;

	if (*strptr != pad_char)
	    newlen = len;

	strptr++;
    }

    return newlen;
}

/**
** Name:	gw02__padstr -- Pad string out to length requested.
**
** Description:
**	Pad string out to length requested, starting at offset position sent
**	by caller.
**
** Inputs:
**	string	    a character pointer to the string
**	pad_char    the pad character (presently limited to either a null
**		    character or a blank)
**	start	    byte offset from start of string to begin padding
**	maxlen	    the maximum length of the string to be checked.
**
** Output:
**		    The string is padded out to maxlen with requested pad
**		    character.
**
**      Returns:
**		    Number of bytes padded.
**
** History:
**  11-jul-90 (linda)
**	Used by RMS Gateway to pad strings used in key comparisons.  If the
**	user has a column registered thus:
**	    (col1   varchar(20) is 'blankpad fixed string',
**		...
**	and there is a query with the qualification
**	    where col1 = 'STRING'
**		...
**	then internally it must become 'STRING              ' before it is used
**	by the RMS Gateway for key positioning.
**/

i4
gw02__padstr
(
    char *string,
    char pad_char,
    i4   start,
    i4   maxlen
)
{
    char    *strptr;
    i4	    npad = 0;
    i4	    i;

    strptr = string;

    for (i = start; i < maxlen; i++)
    {
	npad++;
	strptr[i] = pad_char;
    }

    return npad;
}
