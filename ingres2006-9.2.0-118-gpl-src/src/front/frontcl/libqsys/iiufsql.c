# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<eqlang.h>
# include	<eqrun.h>
# if defined(UNIX) || defined(hp9_mpe) || defined(NT_GENERIC)
# include	<iiufsys.h>
# include	<iisqlda.h>

/*
** Copyright (c) 2004 Ingres Corporation
*/

/*
+* Filename:	iisqlf.c
** Purpose:	Runtime translation layer for Unix ESQL/F77 programs only
**
** Defines:			Mapsto:
**	iisqco_()		IIsqConnect
**	iisqdi_()		IIsqDisconnect
**	iisqfl_()		IIsqFlush
**	iisqin_()		IIsqInit
**	iisqlc_()		IIsqlcdInit
**	iisqgd_()		IIsqGdInit
**	iisqst_()		IIsqStop
**	iisqpr_()		IIsqPrint
**	iisqus_()		IIsqUser
**	iisqpe_()		IIsqPrepare
**	iisqei_()		IIsqExImmed
**	iisqex_()		IIsqExStmt
**	iicsda_()		IIcsDaGet
**	iisqmo_()		IIsqMods
**	iisqpa_()		IIsqParms
**	iisqtp_()		IIsqTPC
**	iilqsi_()		IILQsidSessID
**	iisqde_()		IIsqDescribe
**	iisqdt_()		IIsqDaIn
**	iilqcc_()		IILQcnConName
-*
** Notes:
** 0)	Files in F77 runtime layer:
**		iiuflibq.c	- libqsys directory
**		iiufutil.c	- libqsys directory
**		iiufrunt.c	- runsys directory
**		iiuftb.c	- runsys directory
**
** 1)   Purpose of F77 runtime layer:
**	The above files contain routines which map subroutine calls from a UNIX 
**	F77 program (i.e., generated by esqlf on UNIX) to our runtime system.
**	These files should be easily portable to Unix on the Alliant
**	box.  Read (5) and (6) below regarding Alliant porting considerations.
**
** 2)	F77 symbol conventions:
**	F77 subroutine names need translating because the F77 compiler lower
**	cases and appends an underscore to symbol names.  Also, some F77 
**	compilers, such as Pyramid's, complain about names > 6 chars.  
**	Therefore, for Unix F77 ESQL, we generate calls to our runtime 
**	routines using abbreviated names.  The code generator's abbreviation 
**	is to emit no more than six chars altogether. 
**	e.g.  a call to IIwritio becomes "call IIwrit(....)".  
**	Note that all our names are unique to seven, not six, characters
**	so in order to pass unique 6-letter names to this layer, we have had 
**	to make special F77 entries in the code generator's table of runtime 
**	routines in the module forgen.c
**
** 3)   Parameter passing by reference:
**	The F77 compiler passes everything by reference, so this translation
**	layer also serves to dereference numerics and strings where necessary.
**	Sometimes an argument may be a numeric zero (a null pointer) or a 
**	string pointer.  This layer tests for a non-zero (sent by reference 
**	before deciding whether to send the pointer or a zero. 
**
** 4)   Null-terminated input strings:
**	This layer assumes that strings and string variables which are
**	SENDING data to INGRES/forms system have had a null concatenated 
**	to them.  The ESQL F77 code generator generates calls to IIsd()
**	and IIsdno().  These routines return a pointer into a buffer where
**	the string has been copied and null terminated.  Since the compiler
**	passes everything by reference, this pointer is passed by reference
**	(char **) to these run time routines!!  
**
** 5)   String Lengths as parameters:
**	In those instances where string variables are being passed
**	for RECEIVING character data from INGRES/forms system, this
**	layer must distinguish between null-terminted strings, used
**	for input, and host-language blank padded string, used for
**	output.  All character strings are passed in the DB_CHR_TYPE
**	indicating that they are null terminated, but for routines
**	which return strings to host language variables the interface
**	layer must specify the type as DB_CHA_TYPE.  Then the system
**	blank pad the output to the length specified.
**
**	In previous incarnations, the interface layer has received
**	an extra parameter, usually occuring at the end of the parameter
**	list, specifying the actual number of character bytes allocated
**	to a variable or string constant by the compiler.  This nasty
**	interface is now restricted to a few routines, found in 'iiufutil.c'.
**	The routines perform a variety of functions associated with hiding
**	compiler specific string quirks form the runtime system.
**
** 6)	External variable sent via IIaddform:
**	The "## addform formvar" statement uses an external integer (formvar).
**	An F77 external integer is actually a pointer to an integer.
**	After sending this extern as a parameter, it doubly dereferenced
**	and must be declared that way (see iadfor_ in the file iifrunt.c).
**	For Alliant FORTRAN, the external integer is not dereferenced
**	(I think).
**
** 7)   IIxname versus IIname:
**	The code generator for VMS calls the "IIx" version of a runtime
**	routine whenever a string variable is receiving and sending data.  
**	This was done so that VMS EQUEL/ESQL programs could go through a string 
**	descriptor handling layer.  For Unix we don't need the "IIx" versions
**	of the routines.  However, in this runtime layer, which EVERY call
**	goes through, we must do some work if we are retrieving into a
**	string (see point 5 above).  
**
** History:
**	11-sep-86	- Written for 5.0 UNIX ESQL/FORTRAN (cmr)
**	2/19/87	(daveb)	-- Hide fortran details in iiufsys.h by using
**			  FLENTYPE and FSTRLEN for string lengths.
**			  Add wrappers for apollo's different way of
**			  creating C names from the fortran compiler.
**	15-jun-87 (cmr) - use NO_UNDSCR (defined in iiufsys.h) for
**			  for compilers that do NOT append an underscore
**			  to external symbol names.
**	16-aug-89 (bjb) - Added new calls for 7.0.
**	04-sep-90 (kathryn) - Added functions for F77 Dynamic SQL SQLDA support.
**			      (iicsda - iisqde - iisqdt).
**      10-jul-90 (fredb) - Added cases for HP3000 MPE/XL (hp9_mpe) which
**                          needs these routines.
**	12-sep-90 (barbara)
**	    Incorporated part of Russ's mixed case changes here using the
**	    same code as NO_UNDSCR.  The other redefines for external
**	    symbols are in separate files.
**	14-sep-90 (barbara)
**	    Undid previous change.  It won't work.
**	30-Jul-1992 (fredv)
**	    For m88_us5, we don't want to redefine these symbols because
**	    m88_us5 FORTRAN compiler is NO_UNDSCR and MIXEDCASE_FORTRAN.
**	    If we do that, symbols in iiufsqlM.o will be all screwed up.
**	    This is a quick and dirty change for this box for 6.4. The
**	    NO_UNSCR and MIXEDCASE_FORTRAN issue should be revisited in
**	    6.5. Me and Rudy both have some idea what should be the change
**	    to make this FORTRAN stuff a lot cleaner.
**	17-dec-1992 (larrym)
**	    Added IIsqlcdInit and IIsqGdInit
**	09-mar-1993 (larrym)
**	    added IILQcnConName
**	10-01-1993 (kathryn) Bug 55505
**	    Changed iisqpe to pass the sqlda structure thru to IIsqPrepare.
**	11-22-1993 (teresal)
**	    Fixed sqlstate so it is dereferenced correctly; otherwise, 
**	    a null pointer was being passed which caused the user's program
** 	    to core dump. Bug #57190.
**	15-dec-93 (donc) Bug 56875
**	    Modified iisqpe to use dereferenced sqlda pointer when calling
**	    IIsqPrepare.
**      14-feb-1996 (morayf)
**          Made access to sqlstate through iisadr_() as it depends on the
**	    brand of FORTRAN as to the format of string data passed through
**	    to C.
**          eg. In the case of SNI (rmx_us5) a structure pointer to a char *
**          followed by the (length) integer is used, so using a simple
**          char *sqlstate is incorrect.
**	    iisadr_() hides all this detail and returns the relevant char *
**	    constituent.
**	    Also added it to list of defines, here and in U.c, L.c, M.c
**	    versions of this file (these sets of 4 MUST be kept in sync. !
**	22-mar-1996 (morayf)
**	    Made iisadr_ definition to iisadr conditional on NO_UNDSCR here.
**	    It's possible that this is _too_ restrictive a condition, as I
**	    believe NO_UNDSCR is incompatible with MIXEDCASE_FORTRAN,
**	    hence the use of the m88_us5 exception in these function name
**	    redefinitions.
**	    However, as MIXEDCASE_FORTRAN strips out trailing underscore
**	    elsewhere, this should also strip the iisadr_ one here.
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**  22-Sep-2003 (fanra01)
**      Bug 110909
**      Add windows define to enable mixed case iisadr to be defined.
*/

# if defined(NO_UNDSCR) && !defined(m88_us5)
# define	iisqco_	iisqco
# define	iisqdi_	iisqdi
# define	iisqfl_	iisqfl
# define	iisqin_	iisqin
# define	iisqlc_	iisqlc
# define	iisqgd_	iisqgd
# define	iisqst_	iisqst
# define	iisqpr_	iisqpr
# define	iisqus_	iisqus
# define	iisqpe_	iisqpe
# define	iisqei_	iisqei
# define	iisqex_	iisqex
# define	iicsda_	iicsda
# define	iisqmo_	iisqmo
# define	iisqpa_	iisqpa
# define	iisqtp_	iisqtp
# define	iilqsi_	iilqsi
# define	iisqde_ iisqde
# define	iisqdt_ iisqdt
# define	iilqcn_	iilqcn
# define	iisadr_ iisadr
# endif

# if defined(MIXEDCASE_FORTRAN)
# define	iisadr_ iisadr
# endif


/*
** IIsqConnect
**	- error handling layer between ESQL program and IIingopen
** 	- Sends a variable number of arguments, up to a maximum of 15
**	  character string args plus language argument.
**	- For F77 only we assume an extra first argument, which is a
**	  count representing the number of string args.  This is present
**	  on all calls which have a varying number of arguments.
**	  (See which calls go through gen_var_args in code generator.)
*/
void
iisqco_( count, lan, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, 
	    arg9, arg10, arg11, arg12, arg13, arg14, arg15 )
i4	*count, *lan;
char	**arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8,
        **arg9, **arg10,**arg11,**arg12,**arg13, **arg14, **arg15;
{
# define 	ING_ARGS_MAX	15
    char	*v_args[ING_ARGS_MAX];
    i4  	i;

    for (i = 0; i < ING_ARGS_MAX; i++)
  	v_args[i] = (char *)0;		/* initialize arg array */

  /* 
  ** Enter this switch statement according to argument count.
  ** Fill v_args, starting at last arg and dropping through
  ** all subsequent ones until first arg
  */
    switch( *count - 1 ) {
	case 15:  v_args[14] = *arg15;
	case 14:  v_args[13] = *arg14;
	case 13:  v_args[12] = *arg13;	
	case 12:  v_args[11] = *arg12;	
	case 11:  v_args[10] = *arg11;	
	case 10:  v_args[ 9] = *arg10;	
	case  9:  v_args[ 8] = *arg9;	
	case  8:  v_args[ 7] = *arg8;	
	case  7:  v_args[ 6] = *arg7;	
	case  6:  v_args[ 5] = *arg6;	
	case  5:  v_args[ 4] = *arg5;	
	case  4:  v_args[ 3] = *arg4;	
	case  3:  v_args[ 2] = *arg3;	
	case  2:  v_args[ 1] = *arg2;	
	case  1:  v_args[ 0] = *arg1;	
    }
    IIsqConnect( hostC, v_args[0], v_args[1], v_args[2], v_args[3], 
	v_args[4], v_args[5], v_args[6], v_args[7], v_args[8], v_args[9],
	v_args[10], v_args[11], v_args[12], v_args[13], v_args[14] );
}

/*
** IIsqDisconnect
** 	- close up any open cursors before calling IIexit
*/
void
iisqdi_()
{
    IIsqDisconnect();
}


/*
** IIsqFlush
**	- processes sqlca  and resets error handling
*/
void
iisqfl_( file_name, lineno )
char	**file_name;
i4	 *lineno;
{
    IIsqFlush( *file_name, *lineno );
}

/*
** IIsqInit
**	- error handling layer between ESQL program and libq routines
*/
void
iisqin_( sqlca )
i4	*sqlca;
{
    IIsqInit( sqlca );
}

/*
** IIsqlcdInit
**      - SQLCODE handling layer between ESQL program and libq routines
*/
void
iisqlc_( sqlca, sqlcode )
i4      *sqlca;
i4	*sqlcode;
{
    IIsqlcdInit( sqlca, sqlcode );
}

/*
** IIsqGdInit
**      - SQLSTATE handling layer between ESQL program and libq routines
*/
void
iisqgd_( op_type, sqlstate )
i4      *op_type;
char	*sqlstate;
{
    IIsqGdInit( *op_type, iisadr_(sqlstate) );
}

/*
** IIsqStop
**	- handles SQL STOP command 
*/
void
iisqst_( sqlca )
i4	*sqlca;
{
    IIsqStop( sqlca );
}

/*
** IIsqPrint
**	- in-house method of dumping error messages 
*/
void
iisqpr_( sqlca )
i4	*sqlca;
{
    IIsqPrint( sqlca );
}

/*
** IIsqUser
**	- simulates and saves a "-u" flag for use by IIsqConnect
*/
void
iisqus_( uname )
char	**uname;
{
    IIsqUser( *uname );
}

/*
** IIsqPrepare
**	- PREPARE stmt from query
*/
void
iisqpe_( lang, stmt_name, sqlda, using_flag, qry )
i4	*lang;
char	**stmt_name;
IISQLDA **sqlda;
i4	*using_flag;
char	**qry;
{
    IIsqPrepare( *lang, *stmt_name, *sqlda, *using_flag, *qry );
}

/*
** IIsqExImmed
**	- EXECUTE IMMEDIATE query
*/
void
iisqei_( qry_str )
char	**qry_str;
{
    IIsqExImmed( *qry_str );
}

/*
** IIsqExStmt
**	- EXECUTE statement [USING ...]
*/
void
iisqex_( qry_str, using )
char	**qry_str;
i4	 *using;
{
    IIsqExStmt( *qry_str, *using );
}


/*
** IIsqMods
**	- Singleton Select or Gateway CONNECT WITH clause
*/
void
iisqmo_( flag )
i4	*flag;
{
    IIsqMods( *flag );
}


/*
** IIsqParms
**	- Gateway WITH clause on CONNECT
*/
void
iisqpa_( op_type, name, type, var_ptr, val )
i4	*op_type;
char	**name;
i4	*type;
i4	*var_ptr;
i4	*val;
{
    if (*type == DB_CHR_TYPE)
	IIsqParms( *op_type, *name, DB_CHR_TYPE, *(char **)var_ptr, *val );
    else
	IIsqParms( *op_type, *name, *type, var_ptr, *val );
}

/*
** IIsqTPC
**	- WITH clause on CONNECT for reassociating to a distributed transaction.
*/
void
iisqtp_( highdxid, lowdxid )
i4	*highdxid;
i4	*lowdxid;
{
    IIsqTPC( *highdxid, *lowdxid );
}

/*
** IILQcnConName
**	- simulates and saves a connection_name for use by IIsqConnect
*/
void
iilqcn_( cname )
char	**cname;
{
    IILQcnConName( *cname );
}

/*
** IILQsidSessID
**	- Pass session id on CONNECT statement
*/
void
iilqsi_( session_id )
i4	*session_id;
{
    IILQsidSessID( *session_id );
}

/*
** IIcsDaGet
**	- Cursor Fetch of Values via an SQLDA
** 	- EXEC SQL FETCH c1 USING DESCRIPTOR :sqlda
*/
void
iicsda_(lang,sqd)
i4	*lang;
IISQLDA	*sqd;
{
	IIcsDaGet(*lang,sqd);
}

/*
** IIsqDescribe
**	- EXEC SQL DESCRIBE s1 INTO :sqlda
*/
void
iisqde_(lang, stmt, sqd, using)
i4      *lang;
char    **stmt;
IISQLDA	*sqd;
i4      *using;
{
	IIsqDescribe(*lang, *stmt, sqd, *using);
}

/*
** IIsqDaIn
**	- USING DESCRIPTOR clause of EXECUTE and OPEN CURSOR stmts.
*/
void
iisqdt_(lang,sqd)
i4      *lang;
IISQLDA	*sqd;
{
	IIsqDaIn(*lang, sqd);
}

# endif