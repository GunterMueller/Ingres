# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  Copyright (c) 1992, 2005 Ingres Corporation
**
**
**  Name: txlogs.sc -- contains function which controls the transaction
**	log configuration form of the OpenINGRES Configuration Utility.
**
**  History:
**	14-dec-92 (tyler)
**		Created.
**	16-feb-93 (tyler)
**		Added code (totally untested due to dependency on "rcpstat"
**		utility) to lend some functionality to this form.
**	21-jun-93 (tyler)
**		Fixed many bugs.
**	23-jul-93 (tyler)
**		Added missing functionality.
**	19-oct-93 (tyler)
**		The code sections which call csinstall to install
**		shared memory and deal with "raw" transaction logs
**		is now included using #ifdef UNIX (rather than
**		#ifndef VMS) directives.  Modified transaction_logs_form()
**		to return a bitmask which tells the top level form
**		which logs are enabled.  Changed type of global variable
**		"host" to (char *).
**	22-nov-93 (tyler)
**		Changed message identifiers.
**	22-nov-93 (tyler)
**		Don't assume NMgtIngAt() returns with FAIL STATUS if
**		symbol isn't defined.  Copy buffer which is returned by
**		NMgtIngAt().  Only call "iichkraw" if UNIX defined. 
**		Fixed various other bugs.
**	01-dec-93 (tyler)
**		Fixed bad calls to IIUGerr().  Make this form read-only
**		only when "rcpstat -online" exits OK.
**	05-jan-94 (tyler)
**		Fixed typo in display_message() callback which caused
**		forminit error messages.  Fixed BUG 56871 which prevented 
**		lookup of raw transaction log size.
**	31-jan-94 (tyler)
**		Don't blindly remove shared memory (on Unix) before
**		exiting without first checking whether the server is
**		on-line, because it may have been started since the
**		screen was entered.  Pipe output of "rcpstat -online"
**		to /dev/null in case server has been shut down so
**		CSinitiate() error messages aren't spewed to the screen.
**	22-feb-94 (tyler)
**		Fixed BUG 59321 introduced by previous change.  Fixed BUG
**		59434: asssign Help menu item to PF2.  Fixed BUG 59023:
**		added prompt to Create operation explaining that both
**		transaction logs must be deleted in order to change
**		transaction log sizes.
**	30-nov-94 (nanpr01)
**		Fixed BUG 60371. Added frskey mappings so that key mappings
**		are uniform in CBF from screen to screen.
**	18-jul-95 (duursma)
**		As part of fix for BUG 69827, removed #ifdef VMS code that
**		would call NMgtAt() instead of NMgtIngAt() to translate
**		II_DUAL_LOG and II_DUAL_LOG_NAME.  VMS now calls its new
**		NMgtIngAt() just like Unix.
**	23-aug-95 (albany)  Bug #69432
**		Added code to display log file info on clustered VMS
**		installations.  In transaction_logs_form(), the host 
**		name must be appended to the log file name.
**	29-sep-95 (albany)
**		Remove incorrect preprocessor assumption on previous
**		fix; correct formatting.
**	02-oct-95 (duursma)
**		Put back fix for bug #69560 by harpa06 which was lost earlier:
**		Change minimum transaction log size from 8 to 16 MB.
**	16-oct-95 (tutto01)
**		Changed "/dev/null" to "NUL" for the output of rcpstat on
**		Windows NT.
**    	12-mar-96 (hanch04)
**        	Solaris 2.5 returns 0x7FFFFFFF for size in the stat call in
**		LOinfo for a raw partition.  2.4 and earlier returned 0.
**		We must check for both.
**	22-mar-96 (nick)
**	    Remove above change to test for magic number 0x7FFFFFFF by
**	    hanch04 ; it missed the case for a raw dual log and should
**	    be fixed in LO anyway.
**	17-oct-96 (mcgem01)
**		Misc error messages now take product name as a variables.
**		Replace hard-coded 'ii' references with SystemCfgPrefix.
**	03-nov-1997 (canor01)
**	    Add support for files greater than 2GB in size.  Replace 
**	    rcp.file.size with rcp.file.kbytes.
**      20-jul-1998 (nanpr01)
**          Added support for stripped log project.
**	24-aug-1998 (hanch04)
**	    Added support for stripped log project using multiple log locations
**	    instead of multiple log files.
**	    Added insert and delete location options.
**	01-sep-1998 (hanch04)
**	    Need to get return status from PMmGet.  It's not guaranteed to 
**	    return NULL for a config value that is not found.
**	17-Sep-1999 (bonro01)
**	    Modified to support Unix Clusters.
**	03-nov-1999 (hanch04)
**	    Added increase_log_partions and decrease_log_partions to help
**	    calculate the number of log_writer threads through a rule.
**	20-jan-2000 (hanch04)
**	    Bug #100091
**	    Set num_part for number of log partitions to be LG_MAX_FILE if 
**	    the maximum number of parts was read in.
**	    Restrict the minimum log file partition size to be 16 meg.
**	28-feb-2000 (somsa01)
**	    To fully support LARGEFILE64, changed the type of size-keeping
**	    variables relevent to log size to OFFSET_TYPE in
**	    transaction_log_form().
**	02-mar-2000 (hanch04)
**	    Bug #100091
**	    Corrrectly set num_part to be the number of log parts read in.
**	    After destroying the log, config.dat must be written to disk.
**	01-may-2000 (somsa01)
**	    Updated use of 'iichkraw' to reflect the product prefix.
**	02-may-2000 (hanch04)
**	    Cast the log file size in meg to an i4.
**	16-jun-2000 (somsa01)
**	    The product prefix of 'iichkraw' should be SystemCfgPrefix,
**	    not systemVarPrefix.
**	16-jun-2000 (hanch04)
**	    Dual log needs to look for file.kbytes.  This change already 
**	    was made for the primary log.
**	    iraw2 should be set to true if the dual log is raw.
**	19-jun-2000 (hanch04)
**	    decrease_log_partions when destroying a raw log.
**	17-aug-2000 (hanch04)
**	    Before destroying the primary/dual log file it should be disabled
**	    so that if the other log file is active, the header will be
**	    updated.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	19-sep-2000 (hanch04)
**	    If the disable fails before destroying, continue anyway.
**	23-jul-2001 (devjo01)
**	    Use CX to determine if and how we "decorate" log file 
**	    names for clustering.  Correct a few memory stomps.
**	01-Aug-2002 (devjo01)
**	    Make sure "decorated" transaction file name does not exceed
**	    DI_FILENAME_MAX chars in length.
**	08-Oct-2002 (devjo01)
**	    Establish default PM context from 'node' when playing with
**	    logs.
**	19-jun-2003 (drivi01)
**	    Updated code to have cbf look for a record of a raw log in  
**	    config.dat in the case of someone deleting a raw log manually  
**	    from $II_SYSTEM/ingres/log, and allow user to destroy it.
**	17-jun-2004 (xu$we01)
**	    Now "yes" or "no" will be displayed in the "Raw" column of
**	    CBF Transaction Log Configuration screen (bug 112517).
**	28-Sep-2004 (lakvi01)
**	    Stopped creation of secondary log file, if the actual size of
**	    primary log file is not same as the one mentioned in config.dat
**      28-jan-2005 (zhahu02)
**          Updated so that when on-line, the log file cannot be erased (INGSRV2966/b113075).
**      14-Feb-2005 (jammo01) b111830 INGSRV 2713
**          options for reformatting the log file, along with info about
**          it being raw or not, gets disappeared. The options for erase
**          reformat and disable menu get disabled.
**	06-Apr-2005 (mutma03)
**	    Fixed the logname for clustered application to reflect the 
**	    node.
**	13-May-2005 (kodse01)
**	    replace %ld with %d for i4 values.
**	13-sep-2005 (abbjo03)
**	    On VMS, bypass checking the first part of the log directory which
**	    cannot be created even if we find it doesn't exist, since it has
**	    to be an actual or concealed device.
**	19-Jul-2007 (bonro01)
**	    Restrict the minimum log file partition size to be 32 meg.
**	13-Aug-2007 (kibro01) b118802
**	    Use CMsetPMval instead of PMmInsert so that the derived parameters
**	    get calculated correctly.
**	12-Feb-2008 (kibro01) b119545
**	    Update change for bug 118802 so that change to tx log size is
**	    logged in config.log.
**	26-Feb-2008 (hanje04)
**	    SIR 119978
**	    Replace GLOBALDEF for change_log_file an change_log_buf to GLOBALREF
**	    as both are already defined and having the GLOBALDEF in the libconfig.a
**	    causes link errors on some platforms.
*/
# include	<compat.h>
# include	<gl.h>
# include	<iicommon.h>
# include	<si.h>
# include	<pc.h>
# include	<cs.h>
# include	<ci.h>
# include	<di.h>
# include	<st.h>
# include	<me.h>
# include	<lo.h>
# include	<ex.h>
# include	<er.h>
# include	<cm.h>
# include	<nm.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ug.h>
# include	<lk.h>
# include	<cx.h>
# include	<lg.h>
# include	<ui.h>
# include	<te.h>
# include	<erst.h>
# include	<uigdata.h>
# include	<stdprmpt.h>
# include	<pc.h>
# include	<pm.h>
# include	<pmutil.h>
# include	<util.h>
# include	<simacro.h>
# include	"cr.h"
# include	"config.h"
# define TX_MIN_LOG_FILE_MB 32
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
/*
** global variables
*/
  GLOBALREF char *host;
  GLOBALREF char *node;
  GLOBALREF char *ii_system;
  GLOBALREF char *ii_installation;
GLOBALREF char		*local_host;
GLOBALREF char		Affinity_Arg[16];
GLOBALREF LOCATION	config_file;
GLOBALREF PM_CONTEXT	*config_data;
GLOBALREF FILE          *change_log_fp;
GLOBALREF LOCATION      change_log_file;
GLOBALREF char          change_log_buf[];
  static char thermometer[ BIG_ENOUGH ];
/*
** local forms
*/
  static char *log_graph = ERx( "log_graph" );
/*
** Forward declares.
*/
STATUS do_csinstall(void);
STATUS call_rcpstat( char *arguments );
STATUS call_rcpconfig( char *arguments );
void do_ipcclean(void);
static void
init_graph( bool create, i4 size )
{
  char msg[ BIG_ENOUGH ];
	if( create )
		STprintf( msg, ERget( S_ST0446_creating_log ), size );
	else
		STprintf( msg, ERget( S_ST0447_erasing_log ), size );
/* # line 278 "txlogs.sc" */	/* putform */
  {
    if (IIfsetio((char *)"log_graph") != 0) {
      IIputfldio((char *)"msg",(short *)0,1,32,0,msg);
    } /* IIfsetio */
  }
/* # line 280 "txlogs.sc" */	/* host code */
	*thermometer = EOS;
	/* clear the menu line */ 
/* # line 283 "txlogs.sc" */	/* message */
  {
    IImessage((char *)"");
  }
/* # line 284 "txlogs.sc" */	/* host code */
}
static void
update_graph( void )
{
	STcat( thermometer, ERx( "|" ) );
/* # line 290 "txlogs.sc" */	/* putform */
  {
    if (IIfsetio((char *)"log_graph") != 0) {
      IIputfldio((char *)"thermometer",(short *)0,1,32,0,thermometer);
    } /* IIfsetio */
  }
/* # line 291 "txlogs.sc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 292 "txlogs.sc" */	/* host code */
}
static void
display_message( char *text )
{
  char *msg = text;
/* # line 301 "txlogs.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(msg);
  }
/* # line 302 "txlogs.sc" */	/* host code */
}
static void
set_log_partions( char * new_value )
{
	char configStr[ MAX_LOC ];		/* config str to look up */
	if ( STcompare(new_value, ERx( "0" )) == 0 )
	    new_value = ERx( "1" );
	STprintf( configStr, "%s.%s.rcp.log.log_file_parts",
	    SystemCfgPrefix, node );
	set_resource( configStr, new_value);
}
static void
format_log_name( char *outbuf, char *basename, i4 partno, char *fileext )
{
	i4	choppos;
	char	svch = '\0';
	choppos = DI_FILENAME_MAX - ( STlength(basename) + 4 );
	if ( choppos < CX_MAX_NODE_NAME_LEN+1 )
	{
	    svch = *(fileext + choppos);
	    *(fileext + choppos) = '\0';
	}
	STprintf( outbuf, "%s.l%02d%s", basename, partno, fileext );
	if ( '\0' != svch )
	{
	    *(fileext + choppos) = svch;
	}
}
u_i2
transaction_logs_form( char *log_sym ) 
{ 
  char state[ 100 ];
  char *tmp1, *tmp2, *tmp3;
  char size1[ 25 ], size2[ 25 ];
  char enabled1[ 25 ], enabled2[ 25 ];
  char raw1[ 25 ], raw2[ 25 ];
  char filename1[MAX_LOC + 1], filename2[ MAX_LOC + 1];
  char parts1[ 25 ], parts2[ 25 ];
  i4 LOinfo_flag;
  char transaction_log[25];
  char *path1[LG_MAX_FILE + 1], *path2[LG_MAX_FILE + 1];
  char loc1_buf[LG_MAX_FILE + 1][ MAX_LOC + 1 ];
  char loc2_buf[LG_MAX_FILE + 1][ MAX_LOC + 1 ];
  char file1_buf[LG_MAX_FILE + 1][ MAX_LOC + 1 ];
  char file2_buf[LG_MAX_FILE + 1][ MAX_LOC + 1 ];
  char dir1_loc[LG_MAX_FILE + 1][ MAX_LOC + 1 ];
  char dir2_loc[LG_MAX_FILE + 1][ MAX_LOC + 1 ];
  char dir1_rawloc[LG_MAX_FILE + 1][ MAX_LOC + 1 ];
  char dir2_rawloc[LG_MAX_FILE + 1][ MAX_LOC + 1 ];
  char *dir1[LG_MAX_FILE + 1], *dir2[LG_MAX_FILE + 1];
  char *file1[LG_MAX_FILE + 1], *file2[LG_MAX_FILE + 1];
  char tmp_buf[ BIG_ENOUGH ];
  char fileext[CX_MAX_NODE_NAME_LEN+2];
	OFFSET_TYPE lsize1 = 0, lsize2 = 0;
	char cmd[ BIG_ENOUGH ];
	LOCATION loc1[LG_MAX_FILE + 1];
	LOCATION loc2[LG_MAX_FILE + 1];
	LOINFORMATION loc_info;
	LOCATION *IIUFlcfLocateForm();
	STATUS IIUFgtfGetForm();
	bool changed = FALSE;
	bool read_only = FALSE;
	CL_ERR_DESC cl_err;
	bool transactions = FALSE;
	bool log_file_exists = FALSE;
	bool dual_log_exists = FALSE;
	u_i2 log_status = 0;
	u_i2 loop = 0;
	u_i2 num_parts = 0;
	u_i2 num_parts1 = 0;
	u_i2 num_parts2 = 0;
	bool israw1 = FALSE;
	bool israw2 = FALSE;
	char configStr[ MAX_LOC ];		/* config str to look up */
	char configPfx[ MAX_LOC ];		/* config str to look up */
# ifdef UNIX
	i4   csinstall;
# endif /*UNIX*/
	STprintf( configPfx, "%s.%s.rcp", SystemCfgPrefix, node );
	if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
	    STcopy( "transaction_logs", transaction_log );
	else
	    STcopy( "transaction_dual", transaction_log );
	if( IIUFgtfGetForm( IIUFlcfLocateForm(), transaction_log ) != OK )
	{
	    IIUGerr( S_ST031A_form_not_found, UG_ERR_FATAL, 1,
		transaction_log );
	}
/* # line 407 "txlogs.sc" */	/* message */
  {
    IImessage(ERget( S_ST038A_examining_logs ));
  }
/* # line 409 "txlogs.sc" */	/* host code */
# ifdef UNIX
	if( OK != (csinstall = do_csinstall()) )
	{
/* # line 412 "txlogs.sc" */	/* message */
  {
    IImessage(ERget( S_ST038A_examining_logs ));
  }
/* # line 413 "txlogs.sc" */	/* host code */
	}
# endif /*UNIX*/
	/*
	** ### CHECK FOR ACTIVE LOCAL OR REMOTE CLUSTER SERVER PROCESS.
	*/
	if ( OK == call_rcpstat( ERx( "-online -silent" ) ) )
	{
		STprintf( tmp_buf, ERget( S_ST039A_log_ops_disabled ),
			  SystemProductName );
/* # line 423 "txlogs.sc" */	/* message */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    IImessage(tmp_buf);
  }
/* # line 425 "txlogs.sc" */	/* host code */
		read_only = TRUE;
		STcopy( ERget( S_ST0424_on_line ), state ); 
	}
	else if( OK == call_rcpstat( ERx( "-transactions -silent" ) ) )
	{
		STcopy( ERget( S_ST0426_off_line_transactions ), state ); 
		transactions = TRUE;
	}
	else
		STcopy( ERget( S_ST0425_off_line ), state ); 
/* # line 436 "txlogs.sc" */	/* inittable */
  {
    if (IItbinit(transaction_log,(char *)"log_locations",(char *)"r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 437 "txlogs.sc" */	/* inittable */
  {
    if (IItbinit(transaction_log,(char *)"log_file_atts",(char *)"r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 439 "txlogs.sc" */	/* host code */
# ifndef UNIX
	/* make 'raw' column in both tablefields invisible unless UNIX */
/* # line 441 "txlogs.sc" */	/* set_frs */
  {
    if (IIiqset(4,0,transaction_log,(char *)"log_file_atts",(char *)0) != 0) 
    {
      IIstfsio(54,(char *)"raw",0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 445 "txlogs.sc" */	/* host code */
# endif /* UNIX */
/* # line 446 "txlogs.sc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(30),(void *)IILQint(30));
    } /* IIiqset */
  }
/* # line 448 "txlogs.sc" */	/* display */
  {
    if (IIdispfrm(transaction_log,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    IIputfldio((char *)"host",(short *)0,1,32,0,node);
    IIputfldio((char *)"ii_installation",(short *)0,1,32,0,ii_installation);
    IIputfldio((char *)"ii_system",(short *)0,1,32,0,ii_system);
    IIputfldio((char *)"state",(short *)0,1,32,0,state);
/* # line 460 "txlogs.sc" */	/* host code */
		/* 
		** if this is a clustered installation, we need to
		** append the host name to the log file...
		*/
		fileext[0] = '\0';
		if ( CXcluster_configured() )
		{
		    CXdecorate_file_name( fileext, node );
		}
		for  (loop = 0; loop < LG_MAX_FILE; loop++)
		{
		    dir1[loop] = NULL;
		    dir2[loop] = NULL;
		    file1[loop] = NULL;
		    file2[loop] = NULL;
		    path1[loop] = NULL;
		    path2[loop] = NULL;
		}
		tmp1 = tmp2 = tmp3 = NULL;
        	/* get primrary transaction log information */
		for  (loop = 0; loop < LG_MAX_FILE; loop++)
		{
		    /* if not stripped log and defined old fashioned way */
		    STprintf( configStr, "%s.log.log_file_%d",
			 configPfx, loop + 1);
		    if( OK == PMmGet( config_data, configStr, &tmp1))
			tmp1 = STalloc( tmp1 );
		    else
			tmp1 = NULL;
		    STprintf( configStr, "%s.log.log_file_name",
			configPfx );
		    if( OK == PMmGet( config_data, configStr, &tmp2))
			tmp2 = STalloc( tmp2 );
		    else
			tmp2 = NULL;
		    STprintf( configStr, "%s.log.raw_log_file_%d",
			 configPfx, loop + 1);
		    if( OK == PMmGet( config_data, configStr, &tmp3))
			tmp3 = STalloc( tmp3 );
		    else
			tmp3 = NULL;
		    if( tmp2 == NULL || *tmp2 == EOS )		
		    {
			STcopy( "", filename1 );
		    }
		    else
		    {
			STcopy( tmp2, filename1 );
		    }
		    if( tmp1 == NULL || *tmp1 == EOS ||
			tmp2 == NULL || *tmp2 == EOS )
		    {
			path1[loop] = ERx( "" );
			break;
		    }
		    else
		    {
        		STcopy( tmp1, loc1_buf[loop] );
        		STcopy( tmp1, dir1_loc[loop] );
			if( tmp3 != NULL && *tmp3 != EOS )
			{
        		    STcopy( tmp3, dir1_rawloc[loop] );
			    israw1 = TRUE;
			}
			if ( israw1 )
			   STcopy( ERget( S_ST0330_yes ), raw1 );
			else
			   STcopy( ERget( S_ST0331_no ), raw1 ); 
        		LOfroms( PATH, loc1_buf[loop], &loc1[loop] );
        		LOfaddpath( &loc1[loop], SystemLocationSubdirectory,
			    &loc1[loop] );
        		LOfaddpath( &loc1[loop], ERx( "log" ), &loc1[loop] );
        		LOtos( &loc1[loop], &dir1[loop] );
			dir1[loop] = STalloc( dir1[loop] );
			format_log_name( file1_buf[loop], tmp2,
					 loop + 1, fileext );
			file1[loop] = STalloc( file1_buf[loop] );
        		LOfstfile( file1[loop], &loc1[loop] );
        		LOtos( &loc1[loop], &path1[loop] );
			num_parts1 = loop + 1;
		    }
		    MEfree( tmp1 );
		    MEfree( tmp2 );
		    MEfree( tmp3 );
		    tmp1 = tmp2 = tmp3 = NULL;
		}
		    tmp1 = tmp2 = NULL;
		for  (loop = 0; loop < num_parts1; loop++)
		{
		    if( path1[loop] && *path1[loop] != EOS &&
			LOexist( &loc1[loop] ) == OK )
		    {
			log_file_exists = TRUE;
			/* primary transaction log exists */
			LOinfo_flag = LO_I_SIZE;
			if (LOinfo(&loc1[0], &LOinfo_flag, &loc_info) != OK
			    || loc_info.li_size == 0) 
			{
			    char *value;
			    bool havevalue = FALSE;
				/* use ii.$.rcp.file.size */
			    STprintf( tmp_buf, "%s.file.kbytes",
				      configPfx );
			    if ( PMmGet( config_data, tmp_buf,
					 &value ) == OK )
			    {
				int ivalue;
				havevalue = TRUE;
				CVal( value, &ivalue );
				loc_info.li_size = ivalue;
				STprintf( size1, ERx( "%d" ),
					  (i4)(loc_info.li_size / 1024L) );
			    }
			    if ( havevalue == FALSE )
			    {
				STprintf( tmp_buf, "%s.file.size",
					  configPfx );
				if( PMmGet( config_data, tmp_buf, 
					    &value ) == OK )
				{
				    int ivalue;
				    havevalue = TRUE;
				    CVal( value, &ivalue );
				    loc_info.li_size = ivalue;
				    STprintf( size1, ERx( "%d" ),
					      (i4)(loc_info.li_size / 1048576L));
				}
			    }
			    if ( havevalue == FALSE )
			    {
/* # line 599 "txlogs.sc" */	/* message */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IImessage(ERget(
					S_ST038B_no_primary_size ));
    }
/* # line 603 "txlogs.sc" */	/* host code */
				STprintf( size1, ERx( "???????" ) );
			    }
			    break;
			}
			else
			{
			    lsize1 += loc_info.li_size;
			    STprintf( size1, ERx( "%d" ),
				(i4)(lsize1 / 1048576L ));
			}
		    }
		}
		if (log_file_exists)
		{
		    /* determine whether primary log is enabled */
		    if (OK == call_rcpstat( ERx( "-enable -silent" ) ))
		    {
			STcopy( ERget( S_ST0330_yes ), enabled1 );
			log_status |= PRIMARY_LOG_ENABLED;
		    }
		    else
			STcopy( ERget( S_ST0331_no ), enabled1 );
		}
		else
		{
		    *size1 = EOS;
		    *enabled1 = EOS;
		    *raw1 = EOS;
		}
		STprintf( parts1, ERx( "%d" ), num_parts1);
		/* get secondary transaction log information */
		for  (loop = 0; loop < LG_MAX_FILE; loop++)
		{
		    STprintf( configStr, "%s.log.dual_log_%d",
			 configPfx, loop + 1);
		    if( OK == PMmGet( config_data, configStr, &tmp1))
			tmp1 = STalloc( tmp1 );
		    else
			tmp1 = NULL;
		    STprintf( configStr, "%s.log.dual_log_name",
			configPfx );
		    if( OK == PMmGet( config_data, configStr, &tmp2))
			tmp2 = STalloc( tmp2 );
		    else
			tmp2 = NULL;
		    STprintf( configStr, "%s.log.raw_dual_log_%d",
			 configPfx, loop + 1);
		    if( OK == PMmGet( config_data, configStr, &tmp3))
			tmp3 = STalloc( tmp3 );
		    else
			tmp3 = NULL;
		    if( tmp2 == NULL || *tmp2 == EOS )		
		    {
			STcopy( "", filename2 );
		    }
		    else
		    {
			STcopy( tmp2, filename2 );
		    }
		    if( tmp1 == NULL || *tmp1 == EOS ||
			tmp2 == NULL || *tmp2 == EOS )		
		    {
			path2[loop] = ERx( "" );
			break;
		    }
		    else
		    {
        		STcopy( tmp1, loc2_buf[loop] );
        		STcopy( tmp1, dir2_loc[loop] );
			if( tmp3 != NULL && *tmp3 != EOS )
			{
        		    STcopy( tmp3, dir2_rawloc[loop] );
		            israw2 = TRUE;	
			}
			if ( israw2 )
			   STcopy( ERget( S_ST0330_yes ), raw2 );
			else
			   STcopy( ERget( S_ST0331_no ), raw2 );
        		LOfroms( PATH, loc2_buf[loop], &loc2[loop] );
        		LOfaddpath( &loc2[loop], SystemLocationSubdirectory,
			    &loc2[loop] );
        		LOfaddpath( &loc2[loop], ERx( "log" ), &loc2[loop] );
        		LOtos( &loc2[loop], &dir2[loop] );
			dir2[loop] = STalloc( dir2[loop] );
			format_log_name( file2_buf[loop], tmp2,
					 loop + 1, fileext );
			file2[loop] = STalloc( file2_buf[loop] );
        		LOfstfile( file2[loop], &loc2[loop] );
        		LOtos( &loc2[loop], &path2[loop] );
			num_parts2 = loop + 1;
		    }
		    MEfree( tmp1 );
		    MEfree( tmp2 );
		    MEfree( tmp3 );
		    tmp1 = tmp2 = tmp3 = NULL;
		}
		for  (loop = 0; loop < num_parts2; loop++)
		{
		    if( path2[loop] && *path2[loop] != EOS &&
			LOexist( &loc2[loop] ) == OK )
		    {
			/* secondary transaction log exists */
			dual_log_exists = TRUE;
 			LOinfo_flag = LO_I_SIZE;
			if( LOinfo( &loc2[0], &LOinfo_flag, &loc_info ) != OK
			    || loc_info.li_size == 0 )
			{
			    char *value;
			    bool havevalue = FALSE;
				/* use ii.$.rcp.file.size */
			    STprintf( tmp_buf, "%s.file.kbytes", configPfx );
			    if ( PMmGet( config_data, tmp_buf,
					 &value ) == OK )
			    {
				int ivalue;
				havevalue = TRUE;
				CVal( value, &ivalue );
				loc_info.li_size = ivalue;
				STprintf( size2, ERx( "%d" ),
					  (i4)(loc_info.li_size / 1024L) );
			    }
			    if ( havevalue == FALSE )
			    {
				STprintf( tmp_buf, "%s.file.size",
					  configPfx );
				if( PMmGet( config_data, tmp_buf, 
					    &value ) == OK )
				{
				    int ivalue;
				    havevalue = TRUE;
				    CVal( value, &ivalue );
				    loc_info.li_size = ivalue;
				    STprintf( size2, ERx( "%d" ),
					      (i4)(loc_info.li_size / 1048576L));
				}
			    }
			    if ( havevalue == FALSE )
			    {
/* # line 751 "txlogs.sc" */	/* message */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      IImessage(ERget(
					S_ST038C_no_secondary_size));
    }
/* # line 755 "txlogs.sc" */	/* host code */
				STprintf( size2, ERx( "???????" ) );
			    }
			    break;
			}
			else
			{
			    lsize2 += loc_info.li_size;
			    STprintf( size2, ERx( "%d" ), 
				(i4)(lsize2 / 1048576L ));
			}
		    }
		}
		if (dual_log_exists)
		{
		    /* determine whether primary log is enabled */
		    if( OK == call_rcpstat( ERx( "-enable -dual -silent" ) ) )
		    {
			STcopy( ERget( S_ST0330_yes ), enabled2 );
			log_status |= SECONDARY_LOG_ENABLED;
		    }
		    else
			STcopy( ERget( S_ST0331_no ), enabled2 );
		}
		else
		{
		    *size2 = EOS;
		    *enabled2 = EOS;
		    *raw2 = EOS;
		}
		STprintf( parts2, ERx( "%d" ), num_parts2);
		if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
		{
/* # line 790 "txlogs.sc" */	/* loadtable */
    {
      if (IItbact(transaction_log,(char *)"log_file_atts",1) != 0) {
        IItcoputio((char *)"parts",(short *)0,1,32,0,parts1);
        IItcoputio((char *)"size",(short *)0,1,32,0,size1);
        IItcoputio((char *)"enabled",(short *)0,1,32,0,enabled1);
        IItcoputio((char *)"raw",(short *)0,1,32,0,raw1);
        IItcoputio((char *)"name",(short *)0,1,32,0,filename1);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 798 "txlogs.sc" */	/* host code */
		    for  (loop = 0; loop < num_parts1; loop++)
		    {
			if ( israw1 )
			{
/* # line 802 "txlogs.sc" */	/* loadtable */
    {
      if (IItbact(transaction_log,(char *)"log_locations",1) != 0) {
        IItcoputio((char *)"path",(short *)0,1,32,0,dir1_rawloc[loop]);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 804 "txlogs.sc" */	/* host code */
			}
			else
			{
/* # line 807 "txlogs.sc" */	/* loadtable */
    {
      if (IItbact(transaction_log,(char *)"log_locations",1) != 0) {
        IItcoputio((char *)"path",(short *)0,1,32,0,dir1_loc[loop]);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 809 "txlogs.sc" */	/* host code */
			}
		    }
		}
		else
		{
/* # line 814 "txlogs.sc" */	/* loadtable */
    {
      if (IItbact(transaction_log,(char *)"log_file_atts",1) != 0) {
        IItcoputio((char *)"parts",(short *)0,1,32,0,parts2);
        IItcoputio((char *)"size",(short *)0,1,32,0,size2);
        IItcoputio((char *)"enabled",(short *)0,1,32,0,enabled2);
        IItcoputio((char *)"raw",(short *)0,1,32,0,raw2);
        IItcoputio((char *)"name",(short *)0,1,32,0,filename2);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 822 "txlogs.sc" */	/* host code */
		    for  (loop = 0; loop < num_parts2; loop++)
		    {
			if ( israw2 )
			{
/* # line 826 "txlogs.sc" */	/* loadtable */
    {
      if (IItbact(transaction_log,(char *)"log_locations",1) != 0) {
        IItcoputio((char *)"path",(short *)0,1,32,0,dir2_rawloc[loop]);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 828 "txlogs.sc" */	/* host code */
			}
			else
			{
/* # line 831 "txlogs.sc" */	/* loadtable */
    {
      if (IItbact(transaction_log,(char *)"log_locations",1) != 0) {
        IItcoputio((char *)"path",(short *)0,1,32,0,dir2_loc[loop]);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 833 "txlogs.sc" */	/* host code */
			}
		    }
		}
		/* deactivate invalid options */
		if( read_only )
		{
/* # line 841 "txlogs.sc" */	/* set_frs */
    {
      if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
        IIstfsio(67,ERget( FE_Create ),0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 845 "txlogs.sc" */	/* set_frs */
    {
      if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
        IIstfsio(67,ERget( FE_Insert ),0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 849 "txlogs.sc" */	/* set_frs */
    {
      if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
        IIstfsio(67,ERget( FE_Delete ),0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 853 "txlogs.sc" */	/* set_frs */
    {
      if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
        IIstfsio(67,ERget( S_ST0359_reformat_menu ),0,(short *)0,1,30,sizeof(0
        ),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 857 "txlogs.sc" */	/* set_frs */
    {
      if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
        IIstfsio(67,ERget( S_ST035A_erase_menu ),0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 861 "txlogs.sc" */	/* set_frs */
    {
      if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
        IIstfsio(67,ERget( S_ST035B_disable_menu ),0,(short *)0,1,30,sizeof(0)
        ,(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 865 "txlogs.sc" */	/* set_frs */
    {
      if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
        IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 868 "txlogs.sc" */	/* host code */
		}
/* # line 870 "txlogs.sc" */	/* resume */
    {
      IIrescol((char *)"log_locations",(char *)"path");
      if (1) goto IIfdB1;
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 876 "txlogs.sc" */	/* host code */
	{
		/*
		** Check whether the logging system has gone on-line
		** at each timeout (if form is not in read_only mode),
		** and make it read-only if it is on-line. 
		*/
		if( PCcmdline( (LOCATION *) NULL,
			ERx( "rcpstat -online -silent"), PC_WAIT, (LOCATION *) NULL,
			&cl_err ) == OK )
		{
			STprintf( tmp_buf, ERget( S_ST039A_log_ops_disabled ),
				  SystemProductName );
/* # line 888 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(tmp_buf);
        }
/* # line 891 "txlogs.sc" */	/* host code */
			read_only = TRUE;
			STcopy( ERget( S_ST0424_on_line ), state ); 
/* # line 895 "txlogs.sc" */	/* putform */
        {
          if (IIfsetio(transaction_log) != 0) {
            IIputfldio((char *)"state",(short *)0,1,32,0,state);
          } /* IIfsetio */
        }
/* # line 897 "txlogs.sc" */	/* host code */
			/* deactivate invalid options */
/* # line 898 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Create ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 902 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Insert ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 906 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Delete ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 910 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0359_reformat_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 914 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST035A_erase_menu ),0,(short *)0,1,30,sizeof(
            0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 918 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST035B_disable_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 922 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 925 "txlogs.sc" */	/* host code */
		}
/* # line 926 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 927 "txlogs.sc" */	/* host code */
	}
      } else if (IIretval() == 2) {
	{
        char size[ BIG_ENOUGH ];
        i4 startrow;
        char **log_dir, **log_file;
	    i4 kbytes;
	    bool format = FALSE;
	    /*
	    ** If no log exists, prompt for new size (in megabytes),
	    ** enforce 32 megabytes.  SIR: It would be better to
	    ** refer to the rules for the minimum megabytes.
	    */
	    if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
	    {
		if ( num_parts1 == 0 )
/* # line 949 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 950 "txlogs.sc" */	/* host code */
		log_dir = dir1;
		log_file = file1;
		STcopy( size2, size );
	    }
	    else
	    {
		if ( num_parts2 == 0 )
/* # line 957 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 958 "txlogs.sc" */	/* host code */
		log_dir = dir2;
		log_file = file2;
		STcopy( size1, size );
	    }
	    startrow = 14;
	    if ((( *enabled1 != EOS ) || ( *enabled2 != EOS )) &&
		( num_parts1 != num_parts2 ))
	    {
/* # line 967 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0572_same_no_log_partition ));
        }
/* # line 970 "txlogs.sc" */	/* host code */
	    }
	    else
	    {		
		if( *size == EOS )
		{
		    i4 mbytes;
		    char value[ BIG_ENOUGH ];
prompt_log_size:
		    if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
		    {
        char size_prompt[ER_MAX_LEN];
			STprintf(size_prompt,
			    ERget( S_ST039C_log_size_prompt),
			    num_parts1, TX_MIN_LOG_FILE_MB * num_parts1  );
/* # line 989 "txlogs.sc" */	/* prompt */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IIprmptio(0,size_prompt,(short *)0,1,32,BIG_ENOUGH-1,size);
        }
/* # line 993 "txlogs.sc" */	/* host code */
		    }
		    else
		    {
        char size_prompt[ER_MAX_LEN];
			STprintf(size_prompt,
			    ERget( S_ST039C_log_size_prompt),
			    num_parts2, TX_MIN_LOG_FILE_MB * num_parts2  );
/* # line 1004 "txlogs.sc" */	/* prompt */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IIprmptio(0,size_prompt,(short *)0,1,32,BIG_ENOUGH-1,size);
        }
/* # line 1009 "txlogs.sc" */	/* host code */
		    }
		    if( CVal( size, &mbytes ) != OK )
		    {
/* # line 1013 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST039D_integer_expected ));
        }
/* # line 1016 "txlogs.sc" */	/* host code */
			goto prompt_log_size;
		    }
		    if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
		    {
			if( (mbytes/num_parts1) < TX_MIN_LOG_FILE_MB )
			{
        char too_small[ER_MAX_LEN];
			    STprintf( too_small,
				ERget( S_ST039E_log_too_small),
				num_parts1, TX_MIN_LOG_FILE_MB * num_parts1  );
/* # line 1031 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(too_small);
        }
/* # line 1034 "txlogs.sc" */	/* host code */
			    goto prompt_log_size;
			}
		    }
		    else
		    {
			if( (mbytes/num_parts2) < TX_MIN_LOG_FILE_MB )
			{
        char too_small[ER_MAX_LEN];
			    STprintf( too_small,
				ERget( S_ST039E_log_too_small),
				num_parts2, TX_MIN_LOG_FILE_MB * num_parts2  );
/* # line 1049 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(too_small);
        }
/* # line 1052 "txlogs.sc" */	/* host code */
			    goto prompt_log_size;
			}
		    }
		    /* open change log file for appending */
		    if( SIfopen( &change_log_file , ERx( "a" ), SI_TXT, SI_MAX_TXT_REC,
		            &change_log_fp ) != OK )
		    {
	                EXdelete();
	                IIUGerr( S_ST0323_unable_to_open, UG_ERR_FATAL, 1,
       			        change_log_buf );
		    }
		    STprintf( value, ERx( "%ld" ), mbytes * 1024L );
		    STprintf( tmp_buf, "%s.file.kbytes", configPfx );
		    /* Now set the value and all dependent parameters
		    ** (kibro01) b118802
		    ** And use change_log_fp so it's logged in config.log
		    ** (kibro01) b119545
		    */
		    CRsetPMval(tmp_buf, value, change_log_fp, FALSE, TRUE);
		    /* close change log after appending */
		    (void) SIclose( change_log_fp );
		    if( PMmWrite( config_data, NULL ) != OK )
		    {
			IIUGerr( S_ST0314_bad_config_write,
			    UG_ERR_FATAL, 1, SystemProductName );
		    }
		}
		else
		{
 			if(STcompare(log_sym, ERx("II_DUAL_LOG")) == 0)
 			{
 			int ival=0, n_ival=0, isize1=0, size_b=0;
			char* value=NULL;
			char configStr[BIG_ENOUGH], new_value[BIG_ENOUGH];
			char msg_config_size[BIG_ENOUGH]={0};
			char msg_primary_size[BIG_ENOUGH]={0};
				STprintf(tmp_buf, "%s.file.kbytes", configPfx);
				if (PMmGet(config_data, tmp_buf, &value) == OK)
					{
						CVal(value, &ival);
						n_ival=ival/1024;
					}
				CVal(size1, &isize1);
				if(isize1 == n_ival)
				{
 				if(CONFCH_YES!= IIUFccConfirmChoice(CONF_GENERIC,
									NULL, NULL, NULL, NULL,
									S_ST0428_log_size_warning,
									S_ST0429_log_size_ok,
									S_ST042A_log_size_not_ok,
									NULL, TRUE ) )
								{
/* # line 1112 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1113 "txlogs.sc" */	/* host code */
								}
				}
				else
				{
					STprintf(msg_config_size, "%d", n_ival);
					STprintf(msg_primary_size, "%d", isize1);
					if(CONFCH_YES!= IIUFccConfirmChoice(CONF_GENERIC, 
										msg_config_size, msg_primary_size, NULL, NULL,
										S_ST043A_inconsistent_log, 
										S_ST043B_correct_config, 
										S_ST043C_no_correction, 
										NULL, 
										TRUE))
					{
/* # line 1127 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1128 "txlogs.sc" */	/* host code */
					}
					else
					{
						STprintf(tmp_buf, "%s.file.kbytes", configPfx);
						size_b = (isize1*1024);
						CVna(size_b, new_value); 
						reset_resource(tmp_buf, &new_value, TRUE);
					}            
 				}
			}
		}
		if( IIUFgtfGetForm( IIUFlcfLocateForm(), log_graph ) != OK )
		{
		    IIUGerr( S_ST031A_form_not_found, UG_ERR_FATAL, 1,
			log_graph );
		}
/* # line 1146 "txlogs.sc" */	/* display */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpsetio(2,(short *)0,1,30,sizeof(4),(void *)IILQint(4));
          IIFRgpsetio(1,(short *)0,1,30,4,&startrow);
          IIFRgpcontrol(2,0);
          if (IIdispfrm((char *)"log_graph",(char *)"f") == 0) goto IIfdE2;
          goto IImuI2;
IIfdI2:;
          if (IIfsetio((char *)0) == 0) goto IIfdE2;
/* # line 1154 "txlogs.sc" */	/* redisplay */
          {
            IIredisp();
          }
/* # line 1156 "txlogs.sc" */	/* host code */
		PMmSetDefault( config_data, 0, SystemCfgPrefix );
		PMmSetDefault( config_data, 1, node );
		if( (kbytes = write_transaction_log( TRUE,
		    config_data, log_dir, log_file,
		    display_message, init_graph, 63,
		    update_graph )) == 0 )
		{
/* # line 1163 "txlogs.sc" */	/* message */
          {
            IIFRgpcontrol(1,0);
            IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
            IIFRgpcontrol(2,0);
            IImessage(ERget( S_ST038D_log_creation_failed ));
          }
/* # line 1166 "txlogs.sc" */	/* host code */
		}
		else
		{
          char *no_str = ERget( S_ST0331_no );
		    STprintf( size, ERx( "%ld" ),
			kbytes / 1024L );
/* # line 1176 "txlogs.sc" */	/* putrow */
          {
            if (IItbsetio(3,transaction_log,(char *)"log_file_atts",1) != 0) 
            {
              IItcoputio((char *)"size",(short *)0,1,32,0,size);
              IItcoputio((char *)"enabled",(short *)0,1,32,0,no_str);
              IItcoputio((char *)"raw",(short *)0,1,32,0,no_str);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 1182 "txlogs.sc" */	/* host code */
				/* update log_status mask */
		    if( STcompare( log_sym,
			ERx( "II_LOG_FILE" ) ) == 0 )	
		    {
			u_i2 mod_status = log_status |
			    PRIMARY_LOG_ENABLED;
			log_status = mod_status -
			    PRIMARY_LOG_ENABLED;
			STcopy(ERget( S_ST0330_yes ),enabled1);
		    }
		    else
		    {
			u_i2 mod_status = log_status |
			    SECONDARY_LOG_ENABLED;
			log_status = mod_status -
			    SECONDARY_LOG_ENABLED;
			STcopy(ERget( S_ST0330_yes ),enabled2);
		    }
/* # line 1201 "txlogs.sc" */	/* set_frs */
          {
            if (IIiqset(7,0,transaction_log,(char *)0, (char *)0) != 0) {
              IIstfsio(67,ERget( FE_Create ),0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 1205 "txlogs.sc" */	/* set_frs */
          {
            if (IIiqset(7,0,transaction_log,(char *)0, (char *)0) != 0) {
              IIstfsio(67,ERget( FE_Insert ),0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 1209 "txlogs.sc" */	/* set_frs */
          {
            if (IIiqset(7,0,transaction_log,(char *)0, (char *)0) != 0) {
              IIstfsio(67,ERget( FE_Delete ),0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 1213 "txlogs.sc" */	/* set_frs */
          {
            if (IIiqset(7,0,transaction_log,(char *)0, (char *)0) != 0) {
              IIstfsio(67,ERget( S_ST0359_reformat_menu ),0,(short *)0,1,30,
              sizeof(1),(void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 1218 "txlogs.sc" */	/* set_frs */
          {
            if (IIiqset(7,0,transaction_log,(char *)0, (char *)0) != 0) {
              IIstfsio(67,ERget( S_ST035A_erase_menu ),0,(short *)0,1,30,
              sizeof(1),(void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 1223 "txlogs.sc" */	/* set_frs */
          {
            if (IIiqset(7,0,transaction_log,(char *)0, (char *)0) != 0) {
              IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(1),
              (void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 1227 "txlogs.sc" */	/* host code */
		    format = TRUE;
		}
/* # line 1230 "txlogs.sc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
IIfdB2:
          while (IIrunform() != 0) {
          } /* IIrunform */
          if (IIFRitIsTimeout() != 0) goto IIfdE2;
IIfdF2:
          if (IIchkfrm() == 0) goto IIfdB2;
          goto IIfdE2;
IImuI2:
          goto IIfdI2;
IIfdE2:;
          IIendfrm();
        }
/* # line 1234 "txlogs.sc" */	/* host code */
		if( format )
		    goto reformat;
	    }
	}
      } else if (IIretval() == 3) {
	{
		char *cmd;
		if( transactions )
		{
/* # line 1247 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0389_transaction_warning ));
        }
/* # line 1251 "txlogs.sc" */	/* host code */
			if( CONFCH_YES != IIUFccConfirmChoice( CONF_GENERIC,
				NULL, NULL, NULL, NULL,
				S_ST0386_reformat_prompt,
				S_ST0387_reformat_log,
				S_ST0388_no_reformat_log,
				NULL, TRUE ) )
			{
/* # line 1258 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1259 "txlogs.sc" */	/* host code */
			}
		}
reformat:
		if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )	
		{
			if ( num_parts1 == 0 )
/* # line 1267 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1268 "txlogs.sc" */	/* host code */
			cmd = ERx( "-force_init_log -silent" );
		}
		else
		{
			if ( num_parts2 == 0 )
/* # line 1273 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1274 "txlogs.sc" */	/* host code */
			cmd = ERx( "-force_init_dual -silent" );
		}
/* # line 1277 "txlogs.sc" */	/* message */
        {
          IImessage(ERget( S_ST038E_formatting_log ));
        }
/* # line 1279 "txlogs.sc" */	/* host code */
		if( OK != call_rcpconfig( cmd ) )
		{
/* # line 1281 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST038F_log_format_failed ));
        }
/* # line 1283 "txlogs.sc" */	/* host code */
		}
		else
		{
/* # line 1286 "txlogs.sc" */	/* putrow */
        {
          if (IItbsetio(3,transaction_log,(char *)"log_file_atts",1) != 0) {
            IItcoputio((char *)"enabled",(short *)0,1,32,0,
            ERget( S_ST0330_yes ));
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1290 "txlogs.sc" */	/* host code */
			/* update log_status mask */
			if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
			{
				log_status |= PRIMARY_LOG_ENABLED;
				STcopy(ERget( S_ST0330_yes ),enabled1);
			}
			else
			{
				log_status |= SECONDARY_LOG_ENABLED;
				STcopy(ERget( S_ST0330_yes ),enabled2);
			}
/* # line 1302 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,transaction_log,(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST035B_disable_menu ),0,(short *)0,1,30,
            sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1305 "txlogs.sc" */	/* host code */
		}
	}
      } else if (IIretval() == 4) {
	{
		char *cmd;
		char other_enabled[ BIG_ENOUGH ];
		if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
		{
			if ( num_parts1 == 0 )
/* # line 1318 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1319 "txlogs.sc" */	/* host code */
			cmd = ERx( "-disable_log -silent" );
			STcopy( enabled2, other_enabled);
		}
		else
		{
			if ( num_parts2 == 0 )
/* # line 1326 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1327 "txlogs.sc" */	/* host code */
			cmd = ERx( "-disable_dual -silent" );
			STcopy( enabled1, other_enabled);
		}
		if( *other_enabled == EOS )
		{
/* # line 1334 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0390_cant_disable_log ));
        }
/* # line 1336 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1337 "txlogs.sc" */	/* host code */
		}
/* # line 1339 "txlogs.sc" */	/* message */
        {
          IImessage(ERget( S_ST0391_disabling_log ));
        }
/* # line 1341 "txlogs.sc" */	/* host code */
		if( OK != call_rcpconfig( cmd ) )
		{
/* # line 1343 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0392_log_disable_failed ));
        }
/* # line 1345 "txlogs.sc" */	/* host code */
		}
		else
		{
/* # line 1348 "txlogs.sc" */	/* putrow */
        {
          if (IItbsetio(3,transaction_log,(char *)"log_file_atts",1) != 0) {
            IItcoputio((char *)"enabled",(short *)0,1,32,0,
            ERget( S_ST0331_no ));
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1352 "txlogs.sc" */	/* host code */
			/* update log_status mask */
			if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
			{
				u_i2 mod_status = log_status | 
					PRIMARY_LOG_ENABLED;
				log_status = mod_status -
					PRIMARY_LOG_ENABLED;
			}
			else
			{
				u_i2 mod_status = log_status |
					SECONDARY_LOG_ENABLED;
				log_status = mod_status -
					SECONDARY_LOG_ENABLED;
			}
/* # line 1368 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,transaction_log,(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST035B_disable_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1372 "txlogs.sc" */	/* host code */
			if( *other_enabled != EOS && STcompare( other_enabled,
				ERget( S_ST0331_no ) ) == 0 )
			{
				/* update log_status mask */
			    if( STcompare( log_sym,
				ERx( "II_LOG_FILE" ) ) == 0 )
			    {
				log_status |= PRIMARY_LOG_ENABLED;
				STcopy(ERget( S_ST0330_yes ),enabled1);
			    }
			    else
			    {
				log_status |= SECONDARY_LOG_ENABLED;
				STcopy(ERget( S_ST0330_yes ),enabled2);
			    }
/* # line 1388 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0393_other_log_enabled ));
        }
/* # line 1391 "txlogs.sc" */	/* host code */
			}
		}
	}
      } else if (IIretval() == 5) {
	{
        i4 startrow;
        char **log_dir, **log_file;
		bool format = FALSE;
		if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
		{
			if ( num_parts1 == 0 )
/* # line 1408 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1409 "txlogs.sc" */	/* host code */
			log_dir = dir1;
			log_file = file1;
		}
		else
		{
			if ( num_parts2 == 0 )
/* # line 1415 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1416 "txlogs.sc" */	/* host code */
			log_dir = dir2;
			log_file = file2;
		}
		startrow = 14;
		if( IIUFgtfGetForm( IIUFlcfLocateForm(), log_graph ) != OK )
		{
			IIUGerr( S_ST031A_form_not_found, UG_ERR_FATAL, 1,
				log_graph );
		}
/* # line 1427 "txlogs.sc" */	/* display */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpsetio(2,(short *)0,1,30,sizeof(4),(void *)IILQint(4));
          IIFRgpsetio(1,(short *)0,1,30,4,&startrow);
          IIFRgpcontrol(2,0);
          if (IIdispfrm((char *)"log_graph",(char *)"f") == 0) goto IIfdE3;
          goto IImuI3;
IIfdI3:;
          if (IIfsetio((char *)0) == 0) goto IIfdE3;
/* # line 1435 "txlogs.sc" */	/* redisplay */
          {
            IIredisp();
          }
/* # line 1437 "txlogs.sc" */	/* host code */
                if(  PCcmdline( (LOCATION *) NULL,
                        ERx( "rcpstat -online -silent"), PC_WAIT, (LOCATION *) NULL,
                        &cl_err ) == OK )
                {
                        STprintf( tmp_buf, ERget( S_ST039A_log_ops_disabled ),
                                  SystemProductName );
/* # line 1443 "txlogs.sc" */	/* message */
          {
            IIFRgpcontrol(1,0);
            IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
            IIFRgpcontrol(2,0);
            IImessage(tmp_buf);
          }
/* # line 1446 "txlogs.sc" */	/* host code */
                        read_only = TRUE;
                        STcopy( ERget( S_ST0424_on_line ), state );
/* # line 1450 "txlogs.sc" */	/* putform */
          {
            if (IIfsetio(transaction_log) != 0) {
              IIputfldio((char *)"state",(short *)0,1,32,0,state);
            } /* IIfsetio */
          }
/* # line 1452 "txlogs.sc" */	/* host code */
                }else
			if( write_transaction_log( FALSE, config_data,
				log_dir, log_file, display_message,
				init_graph, 63, update_graph ) == 0 )
			{
/* # line 1457 "txlogs.sc" */	/* message */
          {
            IIFRgpcontrol(1,0);
            IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
            IIFRgpcontrol(2,0);
            IImessage(ERget( S_ST0394_log_erase_failed ));
          }
/* # line 1460 "txlogs.sc" */	/* host code */
			}
			else
				format = TRUE;
/* # line 1464 "txlogs.sc" */	/* breakdisplay */
          {
            if (1) goto IIfdE3;
          }
IIfdB3:
          while (IIrunform() != 0) {
          } /* IIrunform */
          if (IIFRitIsTimeout() != 0) goto IIfdE3;
IIfdF3:
          if (IIchkfrm() == 0) goto IIfdB3;
          goto IIfdE3;
IImuI3:
          goto IIfdI3;
IIfdE3:;
          IIendfrm();
        }
/* # line 1468 "txlogs.sc" */	/* host code */
		if( format )
			goto reformat;
	}
      } else if (IIretval() == 6) {
	{
        char *empty;
		LOCATION *loc;
		char  *cmd;
		if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
		{
			if ( num_parts1 == 0 )
/* # line 1485 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1486 "txlogs.sc" */	/* host code */
			loc = &loc1[0];
			cmd = ERx( "-disable_log -silent" );
		}
		else
		{
			if ( num_parts2 == 0 )
/* # line 1493 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1494 "txlogs.sc" */	/* host code */
			loc = &loc2[0];
			cmd = ERx( "-disable_dual -silent" );
		}
                if(  PCcmdline( (LOCATION *) NULL,
                        ERx( "rcpstat -online -silent"), PC_WAIT, (LOCATION *) NULL,
                        &cl_err ) == OK )
                {
                        STprintf( tmp_buf, ERget( S_ST039A_log_ops_disabled ),
                                  SystemProductName );
/* # line 1504 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(tmp_buf);
        }
/* # line 1507 "txlogs.sc" */	/* host code */
                        read_only = TRUE;
                        STcopy( ERget( S_ST0424_on_line ), state );
/* # line 1511 "txlogs.sc" */	/* putform */
        {
          if (IIfsetio(transaction_log) != 0) {
            IIputfldio((char *)"state",(short *)0,1,32,0,state);
          } /* IIfsetio */
        }
/* # line 1513 "txlogs.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 1514 "txlogs.sc" */	/* host code */
                }
		if( transactions )
		{
/* # line 1518 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0389_transaction_warning ));
        }
/* # line 1521 "txlogs.sc" */	/* host code */
		}
		if( CONFCH_YES != IIUFccConfirmChoice( CONF_GENERIC,
			NULL, NULL, NULL, NULL,
			S_ST0396_destroy_log_prompt,
			S_ST0397_destroy_log,
			S_ST0398_no_destroy_log,
			NULL, TRUE ) )
		{
/* # line 1530 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1531 "txlogs.sc" */	/* host code */
		}
/* # line 1533 "txlogs.sc" */	/* message */
        {
          IImessage(ERget( S_ST0391_disabling_log ));
        }
/* # line 1535 "txlogs.sc" */	/* host code */
		if( OK != call_rcpconfig( cmd ) )
		{
		    /*
		    ** If the disable fails before destroying, continue anyway.
		    */
		}
/* # line 1542 "txlogs.sc" */	/* message */
        {
          IImessage(ERget( S_ST039B_destroying_log ));
        }
/* # line 1544 "txlogs.sc" */	/* host code */
		if( STcompare( log_sym, ERx( "II_LOG_FILE" )) == 0)
		    num_parts = num_parts1;
		else
		    num_parts = num_parts2;
		for  (loop = 0; loop < num_parts; loop++)
		{
		    log_status = LOexist( &loc[loop] );
		    if(( LOdelete( &loc[loop] ) != OK ) &&  (log_status != LO_NO_SUCH))
		    {
/* # line 1553 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(ERget( S_ST0399_log_destroy_failed ));
        }
/* # line 1555 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1556 "txlogs.sc" */	/* host code */
		    }
		    if( STcompare( log_sym, ERx( "II_LOG_FILE" )) == 0)
		    {
			if ( israw1 )
			{
			    if( dir1[loop] != NULL )
			    {
				MEfree( dir1[loop] );
				dir1[loop] = NULL;
				MEfree( file1[loop] );
				file1[loop] = NULL;
			    }
/* # line 1569 "txlogs.sc" */	/* deleterow */
        {
          if (IItbsetio(4,transaction_log,(char *)"log_locations",-2) != 0) {
            if (IItdelrow(0) != 0) {
            } /* IItdelrow */
          } /* IItbsetio */
        }
/* # line 1570 "txlogs.sc" */	/* host code */
			    STprintf( tmp_buf, "%s.log.log_file_%d",
				 configPfx, loop + 1);
			    (void) PMmDelete( config_data, tmp_buf );
			    STprintf( tmp_buf, "%s.log.raw_log_file_%d",
				 configPfx, loop + 1);
			    (void) PMmDelete( config_data, tmp_buf );
			    if( PMmWrite( config_data, NULL ) != OK )
			    {
				IIUGerr( S_ST0314_bad_config_write,
				    UG_ERR_FATAL, 1, SystemProductName );
			    }
			 }
		    }
		    else
		    {
			if ( israw2 )
			{
			    if( dir2[loop] != NULL )
			    {
				MEfree( dir2[loop] );
				dir2[loop] = NULL;
				MEfree( file2[loop] );
				file2[loop] = NULL;
			    }
/* # line 1594 "txlogs.sc" */	/* deleterow */
        {
          if (IItbsetio(4,transaction_log,(char *)"log_locations",-2) != 0) {
            if (IItdelrow(0) != 0) {
            } /* IItdelrow */
          } /* IItbsetio */
        }
/* # line 1595 "txlogs.sc" */	/* host code */
			    STprintf( tmp_buf, "%s.log.dual_log_%d",
				 configPfx, loop + 1);
			    (void) PMmDelete( config_data, tmp_buf );
			    STprintf( tmp_buf, "%s.log.raw_dual_log_%d",
				 configPfx, loop + 1);
			    (void) PMmDelete( config_data, tmp_buf );
			    if( PMmWrite( config_data, NULL ) != OK )
			    {
				IIUGerr( S_ST0314_bad_config_write,
				    UG_ERR_FATAL, 1, SystemProductName );
			    }
			 }
		    }
		}
		if( STcompare( log_sym, ERx( "II_LOG_FILE" )) == 0)
		{
		    if ( israw1 )
		    {
			num_parts1 = 0;
			STprintf( parts1, ERx( "%d" ), num_parts1);
/* # line 1615 "txlogs.sc" */	/* putrow */
        {
          if (IItbsetio(3,transaction_log,(char *)"log_file_atts",1) != 0) {
            IItcoputio((char *)"parts",(short *)0,1,32,0,parts1);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1616 "txlogs.sc" */	/* host code */
		    }
		}
		else
		{
		    if ( israw2 )
		    {
			num_parts2 = 0;
			STprintf( parts2, ERx( "%d" ), num_parts2);
/* # line 1624 "txlogs.sc" */	/* putrow */
        {
          if (IItbsetio(3,transaction_log,(char *)"log_file_atts",1) != 0) {
            IItcoputio((char *)"parts",(short *)0,1,32,0,parts2);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1625 "txlogs.sc" */	/* host code */
		    }
		}
		empty = ERx( "" );
/* # line 1630 "txlogs.sc" */	/* putrow */
        {
          if (IItbsetio(3,transaction_log,(char *)"log_file_atts",1) != 0) {
            IItcoputio((char *)"size",(short *)0,1,32,0,empty);
            IItcoputio((char *)"enabled",(short *)0,1,32,0,empty);
            IItcoputio((char *)"raw",(short *)0,1,32,0,empty);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1636 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Create ),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1640 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Insert ),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1644 "txlogs.sc" */	/* host code */
		if( STcompare( log_sym, ERx( "II_LOG_FILE" )) == 0)
		{
		    if ( num_parts1 > 0 )
/* # line 1647 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Delete ),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1649 "txlogs.sc" */	/* host code */
		}
		else
		{
		    if ( num_parts2 > 0 )
/* # line 1653 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Delete ),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1655 "txlogs.sc" */	/* host code */
		}
/* # line 1657 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0359_reformat_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1661 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST035A_erase_menu ),0,(short *)0,1,30,sizeof(
            0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1665 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST035B_disable_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1669 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1673 "txlogs.sc" */	/* host code */
		/* update log_status mask */
		if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
		{
			u_i2 mod_status = log_status | PRIMARY_LOG_ENABLED;
			log_status = mod_status - PRIMARY_LOG_ENABLED;
			*enabled1 = EOS;
			*size1 = EOS;
		}
		else
		{
			u_i2 mod_status = log_status | SECONDARY_LOG_ENABLED;
			log_status = mod_status - SECONDARY_LOG_ENABLED;
			*enabled2 = EOS;
			*size2 = EOS;
		}
	}
      } else if (IIretval() == 7) {
	{
        char newlocation[ BIG_ENOUGH ];
        char size[ BIG_ENOUGH ];
        i4 rowstate;
        i4 rowno;
        char errmsg[ BIG_ENOUGH ];
	    LOCATION newloc;
	    char newloc_buf[ BIG_ENOUGH ];
/* # line 1704 "txlogs.sc" */	/* getrow */
        {
          if (IItbsetio(2,transaction_log,(char *)"log_file_atts",1) != 0) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,size,(char *)"size");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1707 "txlogs.sc" */	/* host code */
	    if( *size == EOS )
	    {
		if(((STcompare( log_sym, ERx( "II_LOG_FILE" )) == 0) && 
		    (num_parts1 < LG_MAX_FILE )) || 
		    ((STcompare( log_sym, ERx( "II_DUAL_LOG" )) == 0) &&
			(num_parts2 < LG_MAX_FILE )))
		{
/* # line 1716 "txlogs.sc" */	/* prompt */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IIprmptio(0,ERget( S_ST0571_log_location_prompt),(short *)0,1,32,
          BIG_ENOUGH-1,newlocation);
        }
/* # line 1719 "txlogs.sc" */	/* host code */
		    if (*newlocation  ==  EOS)
		    {
/* # line 1721 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1722 "txlogs.sc" */	/* host code */
		    }
		    STcopy( newlocation, newloc_buf );
		    LOfroms( PATH, newloc_buf, &newloc );
#ifndef VMS
		    /*
		    ** On VMS, the first part of a path has to be an actual or
		    ** concealed device, which even if it's found not to exist
		    ** cannot be created, so bypass this check.
		    */
		    if ( LOexist( &newloc ) != OK )
			if ( LOcreate( &newloc ) != OK )
			{
			    STcopy( ERget( S_ST0573_mkdir_failed ), errmsg );
			    STcat( errmsg, newloc_buf );
/* # line 1736 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(errmsg);
        }
/* # line 1739 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1740 "txlogs.sc" */	/* host code */
			}
#endif
		    LOfaddpath( &newloc, SystemLocationSubdirectory, &newloc);
		    if ( LOexist( &newloc ) != OK )
			if ( LOcreate( &newloc ) != OK )
			{
			    STcopy( ERget( S_ST0573_mkdir_failed ), errmsg );
			    STcat( errmsg, newloc_buf );
/* # line 1748 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(errmsg);
        }
/* # line 1751 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1752 "txlogs.sc" */	/* host code */
			}
		    LOfaddpath( &newloc, ERx( "log" ), &newloc );
		    if ( LOexist( &newloc ) != OK )
			if ( LOcreate( &newloc ) != OK )
			{
			    STcopy( ERget( S_ST0573_mkdir_failed ), errmsg );
			    STcat( errmsg, newloc_buf );
/* # line 1759 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage(errmsg);
        }
/* # line 1762 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 1763 "txlogs.sc" */	/* host code */
			}
/* # line 1765 "txlogs.sc" */	/* inquire_frs */
        {
          if (IIiqset(3,0,transaction_log,(char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&rowno,29,(char *)0,0);
          } /* IIiqset */
        }
/* # line 1768 "txlogs.sc" */	/* host code */
		    if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
		    {
			num_parts1++;
			STprintf( parts1, ERx( "%d" ), num_parts1);
			set_log_partions( parts1 );
/* # line 1773 "txlogs.sc" */	/* putrow */
        {
          if (IItbsetio(3,transaction_log,(char *)"log_file_atts",1) != 0) {
            IItcoputio((char *)"parts",(short *)0,1,32,0,parts1);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1775 "txlogs.sc" */	/* host code */
			if ( num_parts1 > LG_MAX_FILE )
/* # line 1776 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Insert ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1778 "txlogs.sc" */	/* host code */
			if ( num_parts1 == 1 )
			    rowno = 0;
		    }
		    else
		    {
			num_parts2++;
			STprintf( parts2, ERx( "%d" ), num_parts2);
			set_log_partions( parts2 );
/* # line 1786 "txlogs.sc" */	/* putrow */
        {
          if (IItbsetio(3,transaction_log,(char *)"log_file_atts",1) != 0) {
            IItcoputio((char *)"parts",(short *)0,1,32,0,parts2);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1788 "txlogs.sc" */	/* host code */
			if ( num_parts2 > LG_MAX_FILE )
/* # line 1789 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Insert ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1791 "txlogs.sc" */	/* host code */
			if ( num_parts2 == 1 )
			    rowno = 0;
		    }
/* # line 1794 "txlogs.sc" */	/* insertrow */
        {
          if (IItbsetio(5,transaction_log,(char *)"log_locations",rowno) != 0
          ) {
            if (IItinsert() != 0) {
              IItcoputio((char *)"path",(short *)0,1,32,0,newlocation);
              IITBceColEnd();
            } /* IItinsert */
          } /* IItbsetio */
        }
/* # line 1797 "txlogs.sc" */	/* host code */
		    loop = 0;
/* # line 1798 "txlogs.sc" */	/* unloadtable */
        {
          if (IItbact(transaction_log,(char *)"log_locations",0) == 0) goto 
          IItbE1;
          while (IItunload() != 0) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,newlocation,(char *)
"path");
            IItcogetio((short *)0,1,30,4,&rowstate,(char *)"_state");
            IITBceColEnd();
/* # line 1801 "txlogs.sc" */	/* host code */
		    {
			if( rowstate != 4 )
			{
			    if( STcompare( log_sym, ERx( "II_LOG_FILE" )) == 0)
			    {
				if( dir1[loop] != NULL )
				{
				    MEfree( dir1[loop] );
				    dir1[loop] = NULL;
				    MEfree( file1[loop] );
				    file1[loop] = NULL;
				}
				STprintf( tmp_buf, "%s.log.log_file_%d",
					   configPfx, loop + 1);
				STcopy( newlocation, loc1_buf[loop] );
				STcopy( newlocation, dir1_loc[loop] );
				LOfroms( PATH, loc1_buf[loop], &loc1[loop] );
				LOfaddpath( &loc1[loop],
				    SystemLocationSubdirectory,
				    &loc1[loop] );
				LOfaddpath( &loc1[loop], ERx( "log" ),
				    &loc1[loop] );
				LOtos( &loc1[loop], &dir1[loop] );
				dir1[loop] = STalloc( dir1[loop] );
				format_log_name( file1_buf[loop], filename1,
					 loop + 1, fileext );
				file1[loop] = STalloc( file1_buf[loop] );
				LOfstfile( file1[loop], &loc1[loop] );
				LOtos( &loc1[loop], &path1[loop] );
			    }
			    else
			    {
				if( dir2[loop] != NULL )
				{
				    MEfree( dir2[loop] );
				    dir2[loop] = NULL;
				    MEfree( file2[loop] );
				    file2[loop] = NULL;
				}
				STprintf( tmp_buf, "%s.log.dual_log_%d",
					   configPfx, loop + 1);
				STcopy( newlocation, loc2_buf[loop] );
				STcopy( newlocation, dir2_loc[loop] );
				LOfroms( PATH, loc2_buf[loop], &loc2[loop] );
				LOfaddpath( &loc2[loop],
				    SystemLocationSubdirectory,
				    &loc2[loop] );
				LOfaddpath( &loc2[loop], ERx( "log" ),
				    &loc2[loop] );
				LOtos( &loc2[loop], &dir2[loop] );
				dir2[loop] = STalloc( dir2[loop] );
				format_log_name(file2_buf[loop], filename2,
					 loop + 1, fileext );
				file2[loop] = STalloc( file2_buf[loop] );
				LOfstfile( file2[loop], &loc2[loop] );
				LOtos( &loc2[loop], &path2[loop] );
			    }
			    (void) PMmDelete( config_data, tmp_buf );
			    (void) PMmInsert(config_data, tmp_buf, newlocation);
			    loop++;
			}
		    }
          } /* IItunload */
IItbE1:
          IItunend();
        }
		    if (PMmWrite(config_data, NULL ) != OK) 
		    {
			IIUGerr( S_ST0314_bad_config_write, UG_ERR_FATAL, 1, 
			    SystemProductName );
		    }
		}
	    }
	}
      } else if (IIretval() == 8) {
	{
        i4 rowno;
        i4 rowstate;
        char size[ BIG_ENOUGH ];
        char location[ BIG_ENOUGH ];
/* # line 1888 "txlogs.sc" */	/* getrow */
        {
          if (IItbsetio(2,transaction_log,(char *)"log_file_atts",1) != 0) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,size,(char *)"size");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1891 "txlogs.sc" */	/* host code */
	    if( *size == EOS )
	    {
		if(((STcompare( log_sym, ERx( "II_LOG_FILE" )) == 0) && 
		    (num_parts1 > 0)) ||
		    ((STcompare( log_sym, ERx( "II_DUAL_LOG" )) == 0) &&
			(num_parts2 > 0)))
		{
/* # line 1898 "txlogs.sc" */	/* inquire_frs */
        {
          if (IIiqset(3,0,transaction_log,(char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&rowno,29,(char *)0,0);
          } /* IIiqset */
        }
/* # line 1900 "txlogs.sc" */	/* deleterow */
        {
          if (IItbsetio(4,transaction_log,(char *)"log_locations",-2) != 0) {
            if (IItdelrow(0) != 0) {
            } /* IItdelrow */
          } /* IItbsetio */
        }
/* # line 1901 "txlogs.sc" */	/* host code */
		    if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
		    {
			num_parts1--;
			STprintf( parts1, ERx( "%d" ), num_parts1);
			set_log_partions( parts1 );
/* # line 1906 "txlogs.sc" */	/* putrow */
        {
          if (IItbsetio(3,transaction_log,(char *)"log_file_atts",1) != 0) {
            IItcoputio((char *)"parts",(short *)0,1,32,0,parts1);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1908 "txlogs.sc" */	/* host code */
		    }
		    else
		    {
			num_parts2--;
			STprintf( parts2, ERx( "%d" ), num_parts2);
			set_log_partions( parts2 );
/* # line 1914 "txlogs.sc" */	/* putrow */
        {
          if (IItbsetio(3,transaction_log,(char *)"log_file_atts",1) != 0) {
            IItcoputio((char *)"parts",(short *)0,1,32,0,parts2);
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 1916 "txlogs.sc" */	/* host code */
		    }
/* # line 1917 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Insert ),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1920 "txlogs.sc" */	/* host code */
		    loop = 0;
/* # line 1921 "txlogs.sc" */	/* unloadtable */
        {
          if (IItbact(transaction_log,(char *)"log_locations",0) == 0) goto 
          IItbE2;
          while (IItunload() != 0) {
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,location,(char *)"\
path");
            IItcogetio((short *)0,1,30,4,&rowstate,(char *)"_state");
            IITBceColEnd();
/* # line 1924 "txlogs.sc" */	/* host code */
		    {
			if( rowstate != 4 )
			{
			    if( STcompare( log_sym, ERx( "II_LOG_FILE" )) == 0)
			    {
				if( dir1[loop] != NULL )
				{
				    MEfree( dir1[loop] );
				    dir1[loop] = NULL;
				    MEfree( file1[loop] );
				    file1[loop] = NULL;
				}
				STprintf( tmp_buf, "%s.log.log_file_%d",
					   configPfx, loop + 1);
				STcopy( location, loc1_buf[loop] );
				STcopy( location, dir1_loc[loop] );
				LOfroms( PATH, loc1_buf[loop], &loc1[loop] );
				LOfaddpath( &loc1[loop],
				    SystemLocationSubdirectory,
				    &loc1[loop] );
				LOfaddpath( &loc1[loop], ERx( "log" ),
				    &loc1[loop] );
				LOtos( &loc1[loop], &dir1[loop] );
				dir1[loop] = STalloc( dir1[loop] );
				format_log_name(file1_buf[loop], filename1,
					 loop + 1, fileext );
				file1[loop] = STalloc( file1_buf[loop] );
				LOfstfile( file1[loop], &loc1[loop] );
				LOtos( &loc1[loop], &path1[loop] );
			    }
			    else
			    {
				if( dir2[loop] != NULL )
				{
				    MEfree( dir2[loop] );
				    dir2[loop] = NULL;
				    MEfree( file2[loop] );
				    file2[loop] = NULL;
				}
				STprintf( tmp_buf, "%s.log.dual_log_%d",
					   configPfx, loop + 1);
				STcopy( location, loc2_buf[loop] );
				STcopy( location, dir2_loc[loop] );
				LOfroms( PATH, loc2_buf[loop], &loc2[loop] );
				LOfaddpath( &loc2[loop],
				    SystemLocationSubdirectory,
				    &loc2[loop] );
				LOfaddpath( &loc2[loop], ERx( "log" ),
				    &loc2[loop] );
				LOtos( &loc2[loop], &dir2[loop] );
				dir2[loop] = STalloc( dir2[loop] );
				format_log_name( file2_buf[loop], filename2,
				 loop + 1, fileext );
				file2[loop] = STalloc( file2_buf[loop] );
				LOfstfile( file2[loop], &loc2[loop] );
				LOtos( &loc2[loop], &path2[loop] );
			    }
			    (void) PMmDelete( config_data, tmp_buf );
			    (void) PMmInsert( config_data, tmp_buf, location);
			    loop++;
			}
		    }
          } /* IItunload */
IItbE2:
          IItunend();
        }
		    if( STcompare( log_sym, ERx( "II_LOG_FILE" ) ) == 0 )
		    {
			STprintf( tmp_buf, "%s.log.log_file_%d", 
			  configPfx, loop + 1);
			num_parts1 = loop;
			path1[loop] = ERx( "" );
			if( dir1[loop] != NULL )
			{
			    MEfree( dir1[loop] );
			    dir1[loop] = NULL;
			    MEfree( file1[loop] );
			    file1[loop] = NULL;
			}
		    }
		    else
		    {
			STprintf( tmp_buf, "%s.log.dual_log_%d",
			  configPfx, loop + 1);
			num_parts2 = loop;
			path2[loop] = ERx( "" );
			if( dir2[loop] != NULL )
			{
			    MEfree( dir2[loop] );
			    dir2[loop] = NULL;
			    MEfree( file2[loop] );
			    file2[loop] = NULL;
			}
		    }
		    (void) PMmDelete( config_data, tmp_buf );
		    if (PMmWrite(config_data, NULL ) != OK) 
		    {
			IIUGerr( S_ST0314_bad_config_write, UG_ERR_FATAL, 1, 
			    SystemProductName );
		    }
		}
	    }
	}
      } else if (IIretval() == 9) {
		if( !get_help( ERx( "dbms.txlogs" ) ) )
/* # line 2035 "txlogs.sc" */	/* message */
        {
          IIFRgpcontrol(1,0);
          IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
          IIFRgpcontrol(2,0);
          IImessage((char *)"No help available.");
        }
      } else if (IIretval() == 10) {
/* # line 2041 "txlogs.sc" */	/* host code */
# ifdef UNIX
		if ( csinstall == OK )
		{
/* # line 2044 "txlogs.sc" */	/* message */
        {
          IImessage(ERget( S_ST0395_cleaning_up_memory ));
        }
/* # line 2045 "txlogs.sc" */	/* host code */
			do_ipcclean();
		}
# endif /* UNIX */
		/* unset timeout */
/* # line 2050 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(
            0));
          } /* IIiqset */
        }
/* # line 2052 "txlogs.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
      } else if (IIretval() == 11) {
/* # line 2057 "txlogs.sc" */	/* host code */
	{
        char path[LG_MAX_FILE][ MAX_LOC + 1 ];
        char enabled[ 25 ];
        char size[ 25 ];
        char raw[ 25 ];
        char parts[ 25 ];
		if( read_only )
/* # line 2068 "txlogs.sc" */	/* resume */
        {
          if (1) goto IIfdB1;
        }
/* # line 2070 "txlogs.sc" */	/* getrow */
        {
          if (IItbsetio(2,transaction_log,(char *)"log_file_atts",1) != 0) {
            IItcogetio((short *)0,1,32,24,parts,(char *)"parts");
            IItcogetio((short *)0,1,32,BIG_ENOUGH-1,size,(char *)"size");
            IItcogetio((short *)0,1,32,24,enabled,(char *)"enabled");
            IItcogetio((short *)0,1,32,24,raw,(char *)"raw");
            IITBceColEnd();
          } /* IItbsetio */
        }
/* # line 2077 "txlogs.sc" */	/* host code */
		if( *parts == '0' )
		{
			/* symbols missing */
/* # line 2081 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Create ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2085 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Insert ),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 2089 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Delete ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2093 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST0359_reformat_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2097 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST035A_erase_menu ),0,(short *)0,1,30,sizeof(
            0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2101 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( S_ST035B_disable_menu ),0,(short *)0,1,30,
            sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2105 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2108 "txlogs.sc" */	/* host code */
		}
		else
		if (( israw1 || israw2 ) && (LO_NO_SUCH == LOexist( &loc1[loop] )))
		{
/* # line 2112 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Create ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2115 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Insert ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2118 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Delete ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2121 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(
					S_ST0359_reformat_menu ),0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2125 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(
					S_ST035A_erase_menu ),0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2129 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(
					S_ST035B_disable_menu ),0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2133 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 2136 "txlogs.sc" */	/* host code */
		}
		else
		{
			/* symbols are set */
			if( *size == EOS )
			{
				/* transaction log doesn't exist */
/* # line 2145 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Create ),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 2149 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Insert ),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 2153 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Delete ),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 2157 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(
					S_ST0359_reformat_menu ),0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2162 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(
					S_ST035A_erase_menu ),0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2167 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(
					S_ST035B_disable_menu ),0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2173 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2176 "txlogs.sc" */	/* host code */
			}
			else
			{
				/* transactio log exists */
/* # line 2181 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Create ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2185 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Insert ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2189 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Delete ),0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2193 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(
					S_ST0359_reformat_menu ),0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 2198 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(
					S_ST035A_erase_menu ),0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 2203 "txlogs.sc" */	/* host code */
				if( STcompare( enabled, ERget( S_ST0330_yes ) )
					== 0 )
				{
/* # line 2206 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(
						S_ST035B_disable_menu ),0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 2210 "txlogs.sc" */	/* host code */
				}
				else
				{
/* # line 2213 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(
						S_ST035B_disable_menu ),0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 2217 "txlogs.sc" */	/* host code */
				}
/* # line 2219 "txlogs.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget( FE_Destroy ),0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 2222 "txlogs.sc" */	/* host code */
			}
		}
	}
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    goto IIfdE1;
IImuI1:
    if (IIFRtoact(0,1) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget( FE_Create ),(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(11,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget( S_ST0359_reformat_menu ),(char *)0,2,2,3) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(12,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IInmuact(ERget( S_ST035B_disable_menu ),(char *)0,2,2,4) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(13,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact(ERget( S_ST035A_erase_menu ),(char *)0,2,2,5) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(14,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact(ERget( FE_Destroy ),(char *)0,2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(15,(char *)0,2,2,6) == 0) goto IIfdE1;
    if (IInmuact(ERget( FE_Insert ),(char *)0,2,2,7) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(16,(char *)0,2,2,7) == 0) goto IIfdE1;
    if (IInmuact(ERget( FE_Delete ),(char *)0,2,2,8) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(18,(char *)0,2,2,8) == 0) goto IIfdE1;
    if (IInmuact(ERget( FE_Help ),(char *)0,2,2,9) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,9) == 0) goto IIfdE1;
    if (IInmuact(ERget( FE_End ),(char *)0,2,2,10) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,10) == 0) goto IIfdE1;
    if (IITBcaClmAct((char *)"log_locations",(char *)"all",1,11) == 0) goto 
    IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
	PMmSetDefault( config_data, 1, NULL );
	return( log_status );
}
STATUS
do_csinstall()
{
# ifdef UNIX
    CL_ERR_DESC cl_err;
    char	cmd[80];
    STprintf( cmd, ERx( "csinstall %s >/dev/null" ), Affinity_Arg );
    return ( PCcmdline((LOCATION *) NULL, cmd,
		    PC_WAIT, (LOCATION *) NULL, &cl_err ) );
# else
    return OK;
# endif /* UNIX */
}
STATUS
call_rcpstat( char *arguments )
{
    CL_ERR_DESC cl_err;
    char	cmd[80+CX_MAX_NODE_NAME_LEN];
    char	*target, *arg;
    char	*redirect;
# ifdef VMS
	redirect = ERx( "" );
# else /* VMS */
# ifdef NT_GENERIC 
	redirect = ERx( ">NUL" );
# else
	redirect = ERx( ">/dev/null" );
# endif /* NT_GENERIC */
# endif /* VMS */
    target = arg = ERx( "" );
    if ( CXcluster_enabled() && STcompare( node, local_host ) != 0 )
    {
	target = node;
	arg = ERx( "-node" );
    }
    STprintf( cmd, ERx( "rcpstat %s %s %s %s" ), Affinity_Arg,
	      arg, target, arguments, redirect );
    return PCcmdline( (LOCATION *) NULL, cmd, 
		      PC_WAIT, (LOCATION *) NULL, &cl_err );
}
STATUS
call_rcpconfig( char *arguments )
{
    CL_ERR_DESC cl_err;
    char	cmd[80+CX_MAX_NODE_NAME_LEN];
    char	*target, *arg;
    char	*redirect;
# ifdef VMS
	redirect = ERx( "" );
# else /* VMS */
# ifdef NT_GENERIC 
	redirect = ERx( ">NUL" );
# else
	redirect = ERx( ">/dev/null" );
# endif /* NT_GENERIC */
# endif /* VMS */
    target = arg = ERx( "" );
    if ( CXcluster_enabled() && STcompare( node, local_host ) != 0 )
    {
	target = node;
	arg = ERx( "-node" );
    }
    STprintf( cmd, ERx( "rcpconfig %s %s %s %s" ), Affinity_Arg,
	      arg, target, arguments, redirect );
    return PCcmdline( (LOCATION *) NULL, cmd, 
		      PC_WAIT, (LOCATION *) NULL, &cl_err );
}
void
do_ipcclean(void)
{
# ifdef UNIX
    CL_ERR_DESC cl_err;
    char	cmd[80];
    STprintf( cmd, ERx( "ipcclean %s" ), Affinity_Arg );
    (void) PCcmdline((LOCATION *) NULL, cmd,
	    PC_WAIT, (LOCATION *) NULL, &cl_err );
# endif /* UNIX */
}
