/*
**	Name:
**		SEPspawn.c
**
**	Function:
**		SEPspawn
**
**	Arguments:
**		i4		argc;
**		char		**argv;
**		LOCATION	*in_name;
**		LOCATION	*out_name;
**		PID		*pid;
**
**	Result:
**		Spawn a DCL subprocess.
**
**		Create a new process with its standard input and output
**		linked to LOCATIONs named by the caller.  Null LOCATION
**		means use current stdin or stdout.
**
**		Returns:
**			OK		-- success
**			FAIL		-- failure
**
**	Side Effects:
**		None
**
**	History:
**	    ??-may-1989 (eduardo)
**		Created.
**	    18-jun-1990 (rog)
**		Standardized usage of SEPspawn().
**	    27-jun-1990 (rog)
**		Added SEPkill() to kill spawned INGRES processes.
**	    27-jun-1990 (rog)
**		Left out a ';'.
**	    27-jun-1990 (rog)
**		Need to include <er.h>.
**	    27-jun-1990 (rog)
**		Exit status on VMS needs to be != 0.
**	    24-jul-1991 (donj)
**		SEPspawn needs to retry if SS$_DUPLNAM is returned
**		from lib$spawn.
**	14-jan-1992 (donj)
**	    Modified all quoted string constants to use the ERx("") macro.
**	    Reformatted variable declarations to one per line for clarity.
**	24-feb-1992 (donj)
**	    Added tracing vars and si.h and sepdefs.h inclusions and code
**	    to trace the command line sent to SEPSON.
**	15-apr-1992 (donj)
**	    Added CM functions to string handling. Removed useless vars.
**	    Made exitvalue less complicated. Added some extra tracing and
**	    comments. Moved #include's to below header text.
**	18-feb-1993 (donj)
**	    Fixed a problem spotted by Andy Cowling at Ingres Ltd., (European
**	    Engineering Centre) with proc_res.
**      25-Jun-98 (rosga02)
**          Added SEVMS conditional logic for a special processing of 
** 	    of the QASETLAB command. This command will cause a deatached process
**	    to start, we need to let it know where to redirect its sys$output
**	    and sys$error before it performes its commands 	
**	13-mar-2001	(kinte01)
**	    Bug 103393 - removed nat, longnat, u_nat, & u_longnat
**	    from testtool code as the use is no longer allowed
**	09-sep-2003 (abbjo03)
**	    Change to use OS headers provided by HP.
**	03-nov-2003 (abbjo03)
**	    Properly initialize all the descriptors used to call lib$spawn.
*/
#include <compat.h>  
#include <lo.h>  
#include <si.h>  
#include <st.h>  
#include <pc.h>
#include <cm.h>
#include <cv.h>
#include <er.h>
#include <descrip.h>
#include <ssdef.h>
#include <starlet.h>
#include <lib$routines.h>
#include <sepdefs.h>

GLOBALREF    i4            PCeventflg ;
GLOBALREF    i4            tracing ;
GLOBALREF    i4            trace_nr ;
GLOBALREF    FILE         *traceptr ;

FUNC_EXTERN  i4            SEP_CMstlen () ;

#define      PCEVENTFLGP  &PCeventflg

STATUS
SEPspawn(
i4        argc,
char    **argv,
i1        waitflg,
LOCATION *in_name,
LOCATION *out_name,
PID      *pid,
i4       *proc_res,
i4        dummy_arg)
{
    STATUS                 ioerr ;

    PID                    mypid ;

    char                  *ct ;
    char                  *in_fname ;
    char                  *out_fname ;
    char                   cmdline [256] ;
    char                   pidstring [6] ;
    char                   procName [20] ;

    struct dsc$descriptor_s         calldsc ;
    struct dsc$descriptor_s         infdsc ;
    struct dsc$descriptor_s         outfdsc ;
    struct dsc$descriptor_s         namedsc ;
    struct dsc$descriptor_s        *infdscp ;
    struct dsc$descriptor_s        *outfdscp ;
    struct dsc$descriptor_s        *namedscp ;

    static u_i4            exitvalue ;
	
    register i4            i ;
    register i4            j ;
    register i4            k ;

    i4                     arglen ;
    i4                     spflag ;

    if ((argc < 1) || (argv[0] == NULL))
	return(FAIL);

    infdscp = outfdscp = NULL;

    if (in_name != NULL)
    {
	LOtos(in_name, &in_fname);
	infdsc.dsc$w_length = STlength(in_fname);
	infdsc.dsc$b_dtype = DSC$K_DTYPE_T;
	infdsc.dsc$b_class = DSC$K_CLASS_S;
	infdsc.dsc$a_pointer = in_fname;
	infdscp = &infdsc;
    }
    if (out_name != NULL)
    {
	LOtos(out_name, &out_fname);
	outfdsc.dsc$w_length = STlength(out_fname);
	outfdsc.dsc$b_dtype = DSC$K_DTYPE_T;
	outfdsc.dsc$b_class = DSC$K_CLASS_S;
	outfdsc.dsc$a_pointer = out_fname;
	outfdscp = &outfdsc;
    }

#ifdef SEVMSBUILD
    CVlower(argv[0]);
    k = -1;
    if ( !STcompare(argv[0],"qasetlab") )
    {
         if (argc > 2)
	 {
	 	CVlower(argv[2]);
         	if ( !STcompare(argv[2],"-u") )
	    		k = 4;
         	else
	 		k = 2;
	 }	
    }
#endif
    calldsc.dsc$a_pointer = ct = cmdline;
    calldsc.dsc$b_dtype = DSC$K_DTYPE_T;
    calldsc.dsc$b_class = DSC$K_CLASS_S;
    for (i = 0; i < argc; i++)
    {
#ifdef SEVMSBUILD
         /* insert result file name as an extra argument for QASETLAB */
         if (k == i) 
         {
		STcopy("-$o", ct);
		STcat(ct, out_fname);
		arglen = SEP_CMstlen(out_fname,0) + 3;
		for (j=0; j < arglen; j++)
		    CMnext(ct);
		CMcpychar(ERx(" "),ct);
		CMnext(ct);
	 }
#endif
	STcopy(argv[i], ct);
	arglen = SEP_CMstlen(argv[i],0);

	for (j=0; j < arglen; j++)
	    CMnext(ct);

	CMcpychar(ERx(" "),ct);
	CMnext(ct);
    }
    *ct = EOS;
    calldsc.dsc$w_length = STlength(calldsc.dsc$a_pointer);

    if (tracing&TRACE_DIALOG)
	SIfprintf(traceptr,ERx("SEPspawn cmd> %s\n"),cmdline);

    if (waitflg) 
	spflag = 0;
    else
	spflag = 1;
    /*
    ** assign process name
    */
    PCpid(&mypid);
    CVla((mypid & 0x0ffff),pidstring);
    STprintf(procName, ERx("SEPSON%s"), pidstring);
    namedsc.dsc$w_length = STlength(procName);
    namedsc.dsc$a_pointer = procName;
    namedsc.dsc$b_dtype = DSC$K_DTYPE_T;
    namedsc.dsc$b_class = DSC$K_CLASS_S;
    namedscp = &namedsc;
    /*
    ** spawn the process
    */
    i = 0;
    do
    {
	if (i > 0)
	    PCsleep(2000);

	ioerr = lib$spawn(&calldsc,	/* command-string,
					**	char str by descriptor (RO).*/
			  infdscp,	/* input-file,
					**	char str by descriptor (RO).*/
			  outfdscp,	/* output-file,
					**	char str by descriptor (RO).*/
			  &spflag,	/* flags,
					**	longword by reference (RO). */
			  namedscp,	/* process-name,
					**	char str by descriptor (RO).*/
			  pid,		/* process-id,
					**	longword by reference (WO). */
			  &exitvalue,	/* completion-status-address,
					**	address by reference (WO).  */
			  PCEVENTFLGP	/* byte-integer-event-flag-num,
					**	byte by reference (RO).	    */
			 );

	if (tracing&TRACE_DIALOG)
	{
	    SIfprintf(traceptr,ERx("SEPspawn lib$spawn exitvalue> %x (hex)\n"),
		      exitvalue);
	    SIfprintf(traceptr,ERx("SEPspawn lib$spawn ioerr> %x (hex)\n"),
		      ioerr);
	}

    } while ((ioerr == SS$_DUPLNAM) && (i++ < 5));

    /* 
	Either spawn completed successfully and subproc is 
	gone or we got an error.	
    */

    if (proc_res)
	*proc_res = exitvalue;		/* This exitvalue is NOT the number
					** of diffs returned by SEP when this
					** routine is used by executor to call
					** SEP. Executor receives the number
					** of diffs (or the Abort Condition)
					** from TC input. This value corresponds
					** to the $STATUS DCL symbol.
					*/
    if (ioerr == SS$_NORMAL)
	return(OK);
    else
	return(ioerr);
}

/*
**	SEPkill()
**
**	Arguments:
**		i4 	childpid;	Child Process ID to be terminated
**		char   *parentpidstr;	String containing parent's PID
**	Returns:
**		STATUS			Completion status of sys$forcex command
**	
*/

STATUS
SEPkill(
PID   childpid,
char *parentpidstr)
{
    STATUS                 ioerr ;
    char                   buffer [20] ;
    struct dsc$descriptor_s         namedsc ;
	
    STprintf(buffer, ERx("SEPSON%s"), parentpidstr);

    namedsc.dsc$w_length = STlength(buffer);
    namedsc.dsc$a_pointer = buffer;

    ioerr = sys$forcex(&childpid, &namedsc, 1);

    if (ioerr == SS$_NORMAL)
	return(OK);
    else
	return(ioerr);
}
