# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**		Copyright (c) 2004 Ingres Corporation
**		All rights reserved.
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<cv.h>
# include	<er.h>
# include	<ex.h>
# include	<lo.h>
# include	<ug.h>
# include	<me.h>
# include	<nm.h>
# include	<si.h>
# include	<st.h>
# include	<ui.h>
# include	<uigdata.h>
# include	<ut.h>
# include <dictutil.h>
/*
** FOOL MING
**
#include "dictutil.h"
*/
/**
** Name:	dctsuprt.qsc - routines to support dictionary check and
**			installation.
**
** Description:
**	These are support routines which are called from upgradefe, modifyfe
**	and IIUICheckDictVer().
**	This file defines:
**
**	init_moduletable	- initialize module table from dict files.
**	init_clienttable	- initialize client table from dict files.
**	enter_client		- request a client to be supported.
**	modulecompare		- hash table comparison function (STATIC).
**	modulehash		- hash table hash function (STATIC).
**	modulestatus		- get status of one module.
**	descstatus		- get status of a module's descendants.
**	getchild		- determine the child of a module.
**	ancstatus		- get status of a module's ancestors.
**	mod_dict_status		- determine which modules are installed.
**	get_dictloc		- get the path for dictionary files (STATIC).
**	IIUIfcFindClient	- find a client in client table.
**	IIUIfmFindModule	- find a module in hash table.
**	IIUIfmsFindModuleStatus - find a module status.
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	26-sep-1990 (pete)
**		Add calls to IIUGbmaBadMemoryAllocation after bad MEreqmem call.
**	2-aug-1991 (pete)
**		Add more comments. Use new module status UNKNOWN (rather
**		than 0).
**	13-aug-1991 (pete)
**		Move couple utility routines from dict!dictutil to this file
**		as part of fix for bug 39165.
**	28-Oct-1992 (fredb)
**		Modifications to get_dictloc, init_moduletable, and
**		init_clienttable for MPE/iX (hp9_mpe).
**		ifdef'd definition of DD_Cre (file extension) - it now defines
**		the last 3 chars of the base file name, as does DD_Cli.
**	29-mar-1993 (rdrane)
**		Integrate above hp9_mpe changes, but correct #ifdef's to
**		conform to Coding Standard (#ifndef usage vs. #ifdef usage).
**      25-sep-96 (mcgem01)
**              Global data moved to uidata.c
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
#define CLIENTABLESIZE 100
#define MODULETABLESIZE 200
GLOBALREF PTR	clienttable; /* hash table containing details of clients */
GLOBALREF PTR	moduletable; /* hash table containing details of modules */
GLOBALREF char	DD_dev[MAX_LOC];
GLOBALREF char	DD_path[MAX_LOC];
FUNC_EXTERN	MODULEDESC *IIUIfmFindModule();
FUNC_EXTERN	CLIENTDESC *IIUIfcFindClient();
static LOCATION	DD_loc;
static char	DD_loc_buf[MAX_LOC];
static i4	modulecompare();
static i4	modulehash();
static i4	clientcompare();
static i4	clienthash();
static STATUS	get_dictloc();
#ifdef hp9_mpe
static READONLY char	DD_Cre[]	= ERx("cin");	/* see comment above */
#else
static READONLY char	DD_Cre[]	= ERx("cre");
#endif
static READONLY char	DD_Cli[]	= ERx("cli");
static READONLY char	DD_Init_modtbl[]   = ERx("init_moduletable");
static READONLY char	DD_Init_clnttbl[]  = ERx("init_clienttable");
static READONLY char	DD_Enter_client[]  = ERx("enter_client");
/*{
** Name:	init_moduletable - initialize module table from dict files.
**
** Description:
**	This reads the dictionary definition files for all of the known
**	dictionary modules (i.e. all of those described in files in the
**	dictionary definition path) and adds a MODULEDESC for each one to
**	the moduletable.
**
** Inputs:
**	none
**
** Outputs:
**	maxcoreversion	- if not null the version number of the module 'CORE'
**			    with the highest version number is returned.
**
**	Returns:
**		STATUS	-	OK:	module table initialized.
**				FAIL:	module table not initialized.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	04-Jun-1990 (rdesmond).
**		now uses DD_dev and DD_path to construct location.
**	05-Jun-1990 (rdesmond).
**		initializes *maxcoreversion to 0 so correct max is set.
**	17-Sep-1990 (pete)
**		add "return(OK)" as last statement in routine.
**	24-sep-1990 (pete)
**		Change version argument to LOwcard from NULL to ERx("0"),
**		since we only want to see the most recent version of each
**		file. Also, only process "*in.cre" files with an 8
**		character filename.
**	26-sep-1990 (pete)
**		Add more error messages.
**	2-aug-1991 (pete)
**		Initialize module->status to UNKNOWN.
**	13-aug-1991 (pete)
**		Added check for duplicate module names (bug 39165).
**	28-Oct-1992 (fredb) hp9_mpe
**		Modifications to use the base file names without extensions;
**		all dictionary files are in the dictfile group of the
**		installation account.  A fully qualified name looks like:
**		"ad101cin.dictfile.ingres".
*/
STATUS
init_moduletable(maxcoreversion)
i4	*maxcoreversion;
{
    LO_DIR_CONTEXT	lc;
    LOCATION		search_loc;
    char		search_loc_buf[MAX_LOC];
    FILE		*filedesc;
    char		inbuf[DO_INBUFSIZE];
    char		*inarray[3];
    char		filebase[9];
    MODULEDESC		*thismodule;
    char		*dummy;
    bool		scanflag;
    i4			i;
    i4			wordcount;
    char		dmy[MAX_LOC];
    char		wcard_prefix[MAX_LOC];  /* used by MPE/iX LOwcard */
    /* get location of dictionary files and set globals */
    if (get_dictloc() != OK)
	return (UI_NOTDIRECTORY);
    if (IIUGhiHtabInit(MODULETABLESIZE, NULL, modulecompare, modulehash,
      &moduletable) != OK)
    {
	IIUGerr(E_UI0031_MODULE_TABLE, UG_ERR_ERROR, 0);
	return(FAIL);
    }
    /* initialize *maxcoreversion to 0 */
    if (maxcoreversion != NULL)
	*maxcoreversion = 0;
    /*
	Populate hash table from all the '*cin.cre' files.
	For the first pass through the '*cin.cre' files we read in the
	module name, module version and remark and create an entry in the
	hash table for each one.  In addition we store the basename for the
	file so that LOCATIONS for the installation files can be created later.
    */
    /* Copy location to use for search */
    LOcopy(&DD_loc, search_loc_buf, &search_loc);
#ifdef hp9_mpe
    /* build MPE wildcard prefix: "@cin"; then call LOwcard with it */
    STcopy("@", wcard_prefix);
    STcat(wcard_prefix, DD_Cre);
    if (LOwcard(&search_loc, wcard_prefix, NULL, ERx("0"), &lc) != OK)
#else
    if (LOwcard(&search_loc, NULL, DD_Cre, ERx("0"), &lc) != OK)
#endif
    {
	IIUGerr(E_UI0032_LOWCARD, UG_ERR_ERROR, 1, DD_Cre);
	return(FAIL);
    }
    do
    {
	/* get file base and check if this is a module creation file */
	if (LOdetail(&search_loc, dmy, dmy, filebase, dmy, dmy) != OK)
	{
	    IIUGerr(E_UI0033_LODETAIL, UG_ERR_ERROR, 3, filebase, DD_Cre,
	    		DD_Init_modtbl);
	    return(FAIL);
	}
	/* skip *in.cre files without 8 character prefix & "in" in col 7&8 */
	if ( (STlength(filebase) != 8) ||
	     (STbcompare(&filebase[6], 2, ERx("in"), 2, TRUE) != 0)
	   )
	    continue;
	if (SIfopen(&search_loc, ERx("r"), SI_TXT, DO_INBUFSIZE,
	  &filedesc) != OK)
	{
	    IIUGerr(E_UI0034_SIFOPEN, UG_ERR_ERROR, 2, DD_Cre,
	    		DD_Init_modtbl);
	    return(FAIL);
	}
	if (SIgetrec(inbuf, DO_INBUFSIZE, filedesc) == ENDFILE)
	{
	    IIUGerr(E_UI0035_SIGETREC, UG_ERR_ERROR, 2, DD_Cre,
	    		DD_Init_modtbl);
	    return(FAIL);
	}
	/* create new element for moduletable */
	if ((thismodule = (MODULEDESC *)
	  MEreqmem(NULL, sizeof(MODULEDESC), TRUE, NULL)) == NULL)
        {
            IIUGbmaBadMemoryAllocation(DD_Init_modtbl);
            /*NOTREACHED*/
        }
	wordcount = 3;
	STgetwords(inbuf, &wordcount, inarray);
	/* assign name, version and remark to thismodule */
	if (wordcount != 3 || CVal(inarray[1], &thismodule->modversion) != OK)
	{
	    /* error in dictionary definition */
	    char	*filename;
	    i4		linenum;
	    LOtos(&search_loc, &filename);
	    linenum = 1;
	    IIUGerr(E_UI002D_FORMAT_ERROR, UG_ERR_ERROR, 2, filename, &linenum);
	    return(FAIL);
	}
	STcopy(inarray[0], thismodule->modname);
	STcopy(inarray[2], thismodule->remark);
	thismodule->status = UNKNOWN;
	if (IIUIfmFindModule(thismodule->modname, thismodule->modversion)
		!= (MODULEDESC *) NULL)
	{
	    /* another .cre file already defined this module. Error! */
    	    char *filename;
	    LOtos(&search_loc, &filename);
	    IIUGerr(E_UI0039_DUPLICATE_MODULE, UG_ERR_ERROR, 3, filename,
		thismodule->modname, &thismodule->modversion);
	    return(FAIL);
	}
	/* save file prefix in moduledesc (don't copy type or database code) */
	STlcopy(filebase, thismodule->filebase, 5);
	/* add element to moduletable */
	if (IIUGheHtabEnter(moduletable, (char *)thismodule, thismodule) != OK)
	    return(FAIL);
	/* determine how many parents this module has */
	thismodule->numparents = 0;
	while (SIgetrec(inbuf, DO_INBUFSIZE, filedesc) != ENDFILE &&
	  STbcompare(inbuf, 3, ERx("END"), 3, TRUE) != 0)
	    thismodule->numparents++;
	SIclose(filedesc);
	/*
	    If this is CORE module save highest version, in case we need to
	    create it later (in installallmodules()).
	*/
	if (STcompare(thismodule->modname, ERx("CORE")) == 0 &&
	  maxcoreversion != NULL && thismodule->modversion > *maxcoreversion)
	    *maxcoreversion = thismodule->modversion;
    } while (LOwnext(&lc, &search_loc) == OK);
    LOwend(&lc);
    /*
	Now iterate through the moduletable and, for each moduledesc,
	open its "*in.cre" file to determine its parents and place ptrs to
	them in it.
    */
    for (scanflag = FALSE;
      IIUGhsHtabScan(moduletable, scanflag, &dummy, &thismodule) != 0;
      scanflag = TRUE)
    {
	char    full_filebase[9];
	if (thismodule->numparents == 0)
	    continue;
	/* compose name of '*cin.cre' file for this module */
	STcopy(thismodule->filebase, full_filebase);
	STcat(full_filebase, ERx("cin"));
#ifdef hp9_mpe
	/* no extension (suffix) for MPE/iX file names here */
	if (LOcompose(DD_dev, DD_path, full_filebase, NULL,
	  NULL, &search_loc) != OK)
#else
	if (LOcompose(DD_dev, DD_path, full_filebase, DD_Cre,
	  NULL, &search_loc) != OK)
#endif
	    return(FAIL);
	if (SIfopen(&search_loc, ERx("r"), SI_TXT, DO_INBUFSIZE,
	  &filedesc) != OK)
	{
	    IIUGerr(E_UI0034_SIFOPEN, UG_ERR_ERROR, 2, DD_Cre,
	    		DD_Init_modtbl);
	    return(FAIL);
	}
	/* first line of file has already been read */
	if (SIgetrec(inbuf, DO_INBUFSIZE, filedesc) == ENDFILE)
	{
	    IIUGerr(E_UI0035_SIGETREC, UG_ERR_ERROR, 2, DD_Cre,
	    		DD_Init_modtbl);
	    return(FAIL);
	}
	/* allocate array of parent ptrs */
	if ((thismodule->parents = (MODULEDESC **)
	  MEreqmem(NULL, (sizeof(PTR) * thismodule->numparents), TRUE, NULL))
	  == NULL)
        {
            IIUGbmaBadMemoryAllocation(DD_Init_modtbl);
            /*NOTREACHED*/
        }
	/* for each parent module, place ptr to it in parent array */
	for (i = 0; i < thismodule->numparents; i++)
	{
	    MODULEDESC	hashkey;
	    /* determine one parent from file */
	    SIgetrec(inbuf, DO_INBUFSIZE, filedesc);
	    wordcount = 3;
	    STgetwords(inbuf, &wordcount, inarray);
	    /* check if line in file is OK */
	    if (wordcount != 3 || CVal(inarray[1], &hashkey.modversion) != OK)
	    {
		char	*filename;
		i4	linenum;
		LOtos(&search_loc, &filename);
		linenum = i + 2;
		IIUGerr(E_UI002D_FORMAT_ERROR, UG_ERR_ERROR, 2, filename,
		  &linenum);
		return(FAIL);
	    }
	    STcopy(inarray[0], hashkey.modname);
	    if (IIUGhfHtabFind(moduletable, &hashkey, &thismodule->parents[i])
	      == FAIL)
		return(FAIL);
	    /* Place backward compatibility flag into moduledesc */
	    thismodule->compat = STcompare(inarray[2], ERx("Y")) == 0;
	}
	SIclose(filedesc);
    }
    return(OK);
}
/*{
** Name:	init_clienttable - initialize client table from dict files.
**
** Description:
**	This reads the dictionary definition files for all of the known
**	dictionary clients (i.e. all of those described in files in the
**	dictionary definition path) and adds a CLIENTDESC for each one to
**	the clienttable.  For each client of a given name, only the highest
**	version of it found in the client definition files will be described
**	in the clienttable.
**
** Inputs:
**	none
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	client table initialized.
**				FAIL:	client table not initialized.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	24-sep-1990 (pete)
**		Change version argument to LOwcard from NULL to ERx("0"),
**		since we only want to see the most recent version of each
**		file.
**	31-jul-1991 (pete)
**		Add initialization of CLIENTDESC.status. This field not
**		used before.
**	13-aug-1991 (pete)
**		Added check for duplicate client names (bug 39165).
**	28-Oct-1992 (fredb) hp9_mpe
**		Modifications to use the base file names without extensions;
**		all dictionary files are in the dictfile group of the
**		installation account.  A fully qualified name looks like:
**		"dbd1cli.dictfile.ingres".  The 'cli' extension for client
**		files has been mashed into the base file name.
*/
STATUS
init_clienttable()
{
    CLIENTDESC		*thisclient;
    LO_DIR_CONTEXT	lc;
    LOCATION		search_loc;
    char		search_loc_buf[MAX_LOC];
    FILE		*filedesc;
    char		inbuf[DO_INBUFSIZE];
    char		*inarray[3];
    char		filebase[9];
    char		*client_name;
    i4			client_version;
    char		dmy[MAX_LOC];
    char		wcard_prefix[MAX_LOC];  /* used by MPE/iX LOwcard */
    if (IIUGhiHtabInit(CLIENTABLESIZE, NULL, clientcompare, clienthash,
      &clienttable) != OK)
	return(FAIL);
    /* Copy location to use for search */
    LOcopy(&DD_loc, search_loc_buf, &search_loc);
    /* load client table with all supportable clients (from .cli files) */
#ifdef hp9_mpe
    /* build MPE wildcard prefix: "@cli"; then call LOwcard with it */
    STcopy("@", wcard_prefix);
    STcat(wcard_prefix, DD_Cli);
    if (LOwcard(&search_loc, wcard_prefix, NULL, ERx("0"), &lc) != OK)
#else
    if (LOwcard(&search_loc, NULL, DD_Cli, ERx("0"), &lc) != OK)
#endif
    {
	IIUGerr(E_UI0032_LOWCARD, UG_ERR_ERROR, 1, DD_Cli);
	return(FAIL);
    }
    do
    {
	i4	wordcount;
	if (SIfopen(&search_loc, ERx("r"), SI_TXT, DO_INBUFSIZE,
	  &filedesc) != OK)
	{
	    IIUGerr(E_UI0034_SIFOPEN, UG_ERR_ERROR, 2, DD_Cli,
	    		DD_Init_clnttbl);
	    return(FAIL);
	}
	if (SIgetrec(inbuf, DO_INBUFSIZE, filedesc) == ENDFILE)
	{
	    IIUGerr(E_UI0035_SIGETREC, UG_ERR_ERROR, 2, DD_Cli,
	    		DD_Init_clnttbl);
	    return(FAIL);
	}
	wordcount = 2;
	STgetwords(inbuf, &wordcount, inarray);
	/* check if line in file is OK */
	if (wordcount != 2 || CVal(inarray[1], &client_version) != OK)
	{
	    char	*filename;
	    i4		linenum;
	    LOtos(&search_loc, &filename);
	    linenum = 1;
	    IIUGerr(E_UI002D_FORMAT_ERROR, UG_ERR_ERROR, 2, filename, &linenum);
	    return(FAIL);
	}
	client_name = inarray[0];
	if (IIUIfcFindClient(client_name, client_version)
		!= (CLIENTDESC *) NULL)
	{
	    /* another .cli file already defined this client. Error! */
    	    char *filename;
	    LOtos(&search_loc, &filename);
	    IIUGerr(E_UI0038_DUPLICATE_CLIENT, UG_ERR_ERROR, 3, filename,
		client_name, &client_version);
	    return(FAIL);
	}
	/* create new element for clienttable */
	if ((thisclient = (CLIENTDESC *)
	  MEreqmem(NULL, sizeof(CLIENTDESC), TRUE, NULL)) == NULL)
        {
            IIUGbmaBadMemoryAllocation(DD_Init_clnttbl);
            /*NOTREACHED*/
        }
	/* assign name and version of client to thisclient */
	STcopy(client_name, thisclient->cliname);
	thisclient->cliversion = client_version;
	thisclient->status = NOTINSTALLED;
	/* add element to clienttable */
	if (IIUGheHtabEnter(clienttable, (char *)thisclient, thisclient)
	  != OK)
	    return(FAIL);
	/* determine file prefix for current .cli file */
	if (LOdetail(&search_loc, dmy, dmy, filebase, dmy, dmy) != OK)
	{
	    IIUGerr(E_UI0033_LODETAIL, UG_ERR_ERROR, 3, filebase, DD_Cli,
	    		DD_Init_clnttbl);
	    return(FAIL);
	}
	STcopy(filebase, thisclient->filebase);
	/* determine how many modules this client is dependent on */
	thisclient->numsupmods = 0;
	while (SIgetrec(inbuf, DO_INBUFSIZE, filedesc) != ENDFILE)
	    thisclient->numsupmods++;
	SIclose(filedesc);
    } while (LOwnext(&lc, &search_loc) == OK);
    LOwend(&lc);
    return (OK);
}
/*{
** Name:	enter_client - request a client to be supported.
**
** Description:
**	This must be called for each client which needs to supported or,
**	for dictionary checking from IIUICheckDictVer(), which needs to be
**	checked.  If a client_version of 0 is passed then the highest version
**	of the client should be used.
**
** Inputs:
**	client_name	- name of client which needs to be supported or checked.
**	client_version	- version number of the client, 0 for highest known.
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	module table initialized.
**				FAIL:	module table not initialized.
**				UI_NO_CLIENT: specified client & version not
**						found in client table.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	04-Jun-1990 (rdesmond).
**		now uses DD_dev and DD_path to construct location.
**	15-Jun-1990 (pete)
**		do the CVupper on a local copy of "client_name".
**	31-Jul-1990 (rdesmond).
**		added client_version.
**	1-nov-1990 (pete)
**		Change NO_CLIENT to UI_NO_CLIENT.
**	28-Oct-1992 (fredb) hp9_mpe
**		Modifications to use the base file name without an extension
**		for MPE/iX.  See comments in 'init_clienttable'.
*/
STATUS
enter_client(client_name, client_version)
char	*client_name;
i4	client_version;
{
    /* returns OK | FAIL | UI_NO_CLIENT */
    CLIENTDESC		*thisclient;
    CLIENTDESC		hashkey;
    FILE		*filedesc;
    char		inbuf[DO_INBUFSIZE];
    char		tmp_client_name[FE_MAXNAME+1];
    char		*dummy;
    bool		scanflag;
    i4			i;
    /*
	Iterate through the client table to determine highest version of
	client, if required.
    */
    if (client_version == 0)
    {
	for (scanflag = FALSE;
	  IIUGhsHtabScan(clienttable, scanflag, &dummy, &thisclient) != 0;
	  scanflag = TRUE)
	{
	    if (STbcompare(thisclient->cliname, 0, client_name, 0, TRUE) == 0)
	    {
		if (thisclient->cliversion > client_version)
		    client_version = thisclient->cliversion;
	    }
	}
    }
    /* "client_name" may be a READONLY literal string, so we must make a
    ** writeable copy of it so CVupper won't crash (esp. on VMS).
    */
    STcopy (client_name, tmp_client_name);
    CVupper(tmp_client_name);
    /*
    **	Access clientdesc in clienttable, set 'required' flag, and read list
    **	of supporting modules from the file.
    */
    /* Need to find specific version of client */
    STcopy(tmp_client_name, hashkey.cliname);
    hashkey.cliversion = client_version;
    if (IIUGhfHtabFind(clienttable, &hashkey, &thisclient) != OK)
	return (UI_NO_CLIENT);
    thisclient->required = TRUE;
    /*
	Now iterate through the client table and, for each clientdesc,
	open its ".cli" file to determine its supporting modules and place 
	ptrs to them in it.
    */
    for (scanflag = FALSE;
      IIUGhsHtabScan(clienttable, scanflag, &dummy, &thisclient) != 0;
      scanflag = TRUE)
    {
	if (thisclient->numsupmods == 0)
	    continue;
	/* open '.cli' file for this client */
#ifdef hp9_mpe
	/* no extension (suffix) for MPE/iX file names here */
	if (LOcompose(DD_dev, DD_path, thisclient->filebase, NULL,
	  NULL, &DD_loc) != OK)
#else
	if (LOcompose(DD_dev, DD_path, thisclient->filebase, DD_Cli,
	  NULL, &DD_loc) != OK)
#endif
	{
	    return(FAIL);
	}
	if (SIfopen(&DD_loc, ERx("r"), SI_TXT, DO_INBUFSIZE,
	  &filedesc) != OK)
	{
	    IIUGerr(E_UI0034_SIFOPEN, UG_ERR_ERROR, 2, DD_Cli,
	    		DD_Enter_client);
	    return(FAIL);
	}
	/* first line of file already read (client name & version) */
	if (SIgetrec(inbuf, DO_INBUFSIZE, filedesc) == ENDFILE)
	{
	    IIUGerr(E_UI0035_SIGETREC, UG_ERR_ERROR, 2, DD_Cli,
	    		DD_Enter_client);
	    return(FAIL);
	}
	/* allocate array of required module ptrs */
	if ((thisclient->supmods = (MODULEDESC **)
	  MEreqmem(NULL, (sizeof(PTR) * thisclient->numsupmods), TRUE, NULL))
	  == NULL)
	{
	    IIUGbmaBadMemoryAllocation(DD_Enter_client);
	    /*NOTREACHED*/
	}
	/* for each required module, place ptr to it in supmods array */
	for (i = 0; i < thisclient->numsupmods; i++)
	{
	    MODULEDESC	hashkey;
	    char	*inarray[2];
	    i4		wordcount;
	    /* determine one supporting client from file */
	    SIgetrec(inbuf, DO_INBUFSIZE, filedesc);
	    wordcount = 2;
	    STgetwords(inbuf, &wordcount, inarray);
	    /* check if line in file is OK */
	    if (wordcount != 2 || CVal(inarray[1], &hashkey.modversion) != OK)
	    {
		char	*filename;
		i4	linenum;
		LOtos(&DD_loc, &filename);
		linenum = i + 2;
		IIUGerr(E_UI002D_FORMAT_ERROR, UG_ERR_ERROR, 2, filename,
		  &linenum);
		return(FAIL);
	    }
	    STcopy(inarray[0], hashkey.modname);
	    /* make sure .cre file for this module was seen earlier
	    ** by init_moduletable, and added to moduletable.
	    ** If can't be found then there are problems with the dictfiles:
	    ** a module is referenced in a .cli file for which we do not
	    ** have a corresponding .cre file.
	    */
	    if (IIUGhfHtabFind(moduletable, &hashkey, &thisclient->supmods[i])
	      == FAIL)
	    {
		IIUGerr(E_UI002F_MODULE_FILE_MISSING, UG_ERR_ERROR,
		    3, inarray[0], inarray[1], thisclient->filebase);
		return(FAIL);
	    }
	}
	SIclose(filedesc);
    }
    return (OK);
}
/*{
** Name:	modulecompare - hash table comparison function (STATIC).
**
** Description:
**	This routine is required by the IIUGhash functions to check if
**	an element in the table matches the search key.  For MODULEDESC
**	the key consists of the module name and module version.
**
** Inputs:
**	id1	- ptr to first MODULEDESC to compare.
**	id2	- ptr to second MODULEDESC to compare.
**
** Outputs:
**
**	Returns:
**		i4	-	0:	keys are equal.
**				FAIL:	keys are not equal.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
*/
static
i4
modulecompare(id1, id2)
char	*id1;
char	*id2;
{
    MODULEDESC	*module1;
    MODULEDESC	*module2;
    module1 = (MODULEDESC *)id1;
    module2 = (MODULEDESC *)id2;
    if (STcompare(module1->modname, module2->modname) == 0 && 
      module1->modversion == module2->modversion)
	return (0);
    else
	return (1);
}
/*{
** Name:	modulehash - hash table hash function (STATIC).
**
** Description:
**	This routine is required by the IIUGhash functions to generate an
**	offset into the modultable given the name of a module.
**
** Inputs:
**	key	- ptr to string to hash on.
**	size	- size of hash table.
**
** Outputs:
**
**	Returns:
**		i4	-	offset into table to begin comparing keys.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
*/
static
i4
modulehash(key, size)
char	*key;
i4	size;
{
    register i4 rem;
    register char *s;
    s = (char *)((MODULEDESC *)key)->modname;
    for (rem = 0; *s != EOS; s++)
	rem = (rem * 128 + *s) % size;
    return (i4)rem;
}
/*{
** Name:	clientcompare - hash table comparison function (STATIC).
**
** Description:
**	This routine is required by the IIUGhash functions to check if
**	an element in the table matches the search key.  For CLIENTDESC
**	the key consists of the client name and client version.
**
** Inputs:
**	id1	- ptr to first CLIENTDESC to compare.
**	id2	- ptr to second CLIENTDESC to compare.
**
** Outputs:
**
**	Returns:
**		i4	-	0:	keys are equal.
**				FAIL:	keys are not equal.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
*/
static
i4
clientcompare(id1, id2)
char	*id1;
char	*id2;
{
    CLIENTDESC	*client1;
    CLIENTDESC	*client2;
    client1 = (CLIENTDESC *)id1;
    client2 = (CLIENTDESC *)id2;
    if (STcompare(client1->cliname, client2->cliname) == 0 && 
      client1->cliversion == client2->cliversion)
	return (0);
    else
	return (1);
}
/*{
** Name:	clienthash - hash table hash function (STATIC).
**
** Description:
**	This routine is required by the IIUGhash functions to generate an
**	offset into the client table given the name of a client.
**
** Inputs:
**	key	- ptr to string to hash on.
**	size	- size of hash table.
**
** Outputs:
**
**	Returns:
**		i4	-	offset into table to begin comparing keys.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
*/
static
i4
clienthash(key, size)
char	*key;
i4	size;
{
    register i4 rem;
    register char *s;
    s = (char *)((CLIENTDESC *)key)->cliname;
    for (rem = 0; *s != EOS; s++)
	rem = (rem * 128 + *s) % size;
    return (i4)rem;
}
/*{
** Name:	modulestatus - get status of one module.
**
** Description:
**	This returns the status of the given module. Module status gives
**	information about ancestors or descendents of the module.
**	DON'T CALL FROM INSIDE A MODULE TABLE SCAN LOOP.
**
** Inputs:
**	moduledesc	- MODULEDESC for module to check status of.
**
** Outputs:
**
**	Returns:
**		STATUS	-	INSTALLED:
**				    Module is currently installed.
**				ANC_COMPAT:
**				    An ancestor of the module is currently
**				    installed.  The requested module CAN be
**				    upgraded to in a backwardly compatible
**				    manner.
**				ANC_INCOMPAT:
**				    An ancestor of the module is currently
**				    installed.  The requested module CANNOT be
**				    upgraded to in a backwardly compatible
**				    manner.
**				DESC_COMPAT:
**				    An descendant of the module is currently
**				    installed.  The installed module is
**				    backwardly compatible with the requested
**				    module.
**				DESC_INCOMPAT:
**				    An descendant of the module is currently
**				    installed.  The installed module is NOT
**				    backwardly compatible with the requested
**				    module.
**				NOTINSTALLED:
**				    The module is not installed and has no
**				    ancestors or descendants installed.
**				UPGRADE_ABORT:
**				    The module is in the process of being
**				    upgraded to, but the upgrade has not
**				    been completed.
**
**	Exceptions:
**
** Side Effects:
**      WARNING: DON'T call this from inside an IIUGhsHtabScan() loop, because
**      it calls descstatus(), which calls getchild(), which does an
**	IIUGhsHtabScan() loop (hash table loops can't be nested).
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	26-sep-1990 (pete)
**		Add descriptive comments.
*/
STATUS
modulestatus(moduledesc)
MODULEDESC	*moduledesc;
{
    /* if status already set in cache then use it */
    if (moduledesc->status == UNKNOWN &&
      (moduledesc->status = ancstatus(moduledesc)) == NOTINSTALLED)
    {
	/* The module has no ancestors (lower version nmbrs) installed. Check
	** for installed descendents (higher version nmbrs).
	*/
	moduledesc->status = descstatus(moduledesc);
    }
    return (moduledesc->status);
}
/*{
** Name:	descstatus - get status of a module's descendants.
**
** Description:
**	This determines if any of the modules descendants is installed and,
**	if so, whether the installed module is backwardly compatible with
**	the requested module.
**
** Inputs:
**	moduledesc	- MODULEDESC for module to check descendant status of.
**
** Outputs:
**
**	Returns:
**		STATUS	-	DESC_COMPAT:
**				    An descendant of the module is currently
**				    installed.  The installed module is
**				    backwardly compatible with the requested
**				    module.
**				DESC_INCOMPAT:
**				    An descendant of the module is currently
**				    installed.  The installed module is NOT
**				    backwardly compatible with the requested
**				    module.
**				NOTINSTALLED:
**				    The module has no descendants installed.
**
**	Exceptions:
**
** Side Effects:
**	WARNING: DON'T call this from inside an IIUGhsHtabScan() loop, because
**	it calls getchild(), which also does an IIUGhsHtabScan() loop
**	(hash table loops can't be nested).
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	27-sep-1990 (pete)
**		Add descriptive comments.
*/
STATUS
descstatus(moduledesc)
MODULEDESC	*moduledesc;
{
    STATUS	status;
    MODULEDESC	*child;
    if ((child = getchild(moduledesc)) == (MODULEDESC *)NULL)
    {
	/* There are no known children of this module (i.e. there are no
	** higher version modules, which this module could be upgraded to;
	** there are no .cre files with this module listed after the first line)
	*/
	moduledesc->status = NOTINSTALLED;
	return (moduledesc->status);
    }
    moduledesc->installed_child = child;
    if (child->status == INSTALLED || child->status == UPGRADE_ABORT)
    {
	moduledesc->status = child->compat ? DESC_COMPAT : DESC_INCOMPAT;
	return (moduledesc->status);
    }
    /* Child found, but child->status indicates no trace of him in dictionary.
    ** (recursively) Check for children of this child.
    */
    status = descstatus(child);
    /* Propagate ptr to child-of-child so installed descendent can be
    ** found later.
    */
    moduledesc->installed_child = child->installed_child;
    if (status == DESC_COMPAT && !child->compat)
    {
	moduledesc->status = DESC_COMPAT;
	return (moduledesc->status);
    }
    else
    {
	moduledesc->status = status;
	return (moduledesc->status);
    }
}
/*{
** Name:	getchild - determine the child of a module.
**
** Description:
**	This returns the child of the given module.  The child of a module
**	is the module which can be created as an immediate upgrade of the
**	given module, usually of the same name as the given module but with
**	the version number equal to the version number of the given module
**	plus one.
**
** Inputs:
**	moduledesc	- MODULEDESC for module to get child of.
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	module table initialized.
**				FAIL:	module table not initialized.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
*/
MODULEDESC *
getchild(moduledesc)
MODULEDESC *moduledesc;
{
    char	*dummy;
    MODULEDESC	*thismodule;
    bool	scanflag;
    /* Search for a module with the given module name and version (*moduledesc)
    ** in its list of MODULEDESC->parents
    */
    for (scanflag = FALSE; 
      IIUGhsHtabScan(moduletable, scanflag, &dummy, &thismodule) != 0;
      scanflag = TRUE)
    {
	i4		index;
	MODULEDESC	*thisparent;
	for (index = 0; index < thismodule->numparents; index++)
	{
	    thisparent = thismodule->parents[index];
	    if (STcompare(thisparent->modname, moduledesc->modname) == 0 && 
	      thisparent->modversion == moduledesc->modversion)
		return (thismodule);
	}
    }
    return ((MODULEDESC *)NULL);
}
/*{
** Name:	ancstatus - get status of a module's ancestors.
**
** Description:
**	This determines if any of the modules ancestors is installed and,
**	if so, whether the requested module is a backwardly compatible 
**	upgrade to the installed module.
**
** Inputs:
**	moduledesc	- MODULEDESC for module to check ancestor status of.
**
** Outputs:
**
**	Returns:
**		STATUS	-	ANC_COMPAT:
**				    An ancestor of the module is currently
**				    installed.  The requested module CAN be
**				    upgraded to in a backwardly compatible
**				    manner.
**				ANC_INCOMPAT:
**				    An ancestor of the module is currently
**				    installed.  The requested module CANNOT be
**				    upgraded to in a backwardly compatible
**				    manner.
**				NOTINSTALLED:
**				    The module has no ancestors installed.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	May 8, 1991 (pete)
**		Changed to not clobber the moduledesc->status when
**		it's UPGRADE_ABORT. Bug 37150.
*/
STATUS
ancstatus(moduledesc)
MODULEDESC	*moduledesc;
{
    STATUS	status;
    MODULEDESC	*thisparent;
    i4		index;
    status = NOTINSTALLED;
    for (index = 0; index < moduledesc->numparents; index++)
    {
	thisparent = moduledesc->parents[index];
	if (thisparent->status == INSTALLED ||
	  thisparent->status == UPGRADE_ABORT)
	{
	    if (!moduledesc->compat)
	    {
		moduledesc->status = ANC_INCOMPAT;
		return (moduledesc->status);
	    }
	    status = ANC_COMPAT;
	}
	else
	{
	    status = ancstatus(thisparent);
	    /* propagate incompatiblity from parent if ancestor installed */
	    if (status == ANC_COMPAT && !moduledesc->compat)
	    {
		moduledesc->status = ANC_INCOMPAT;
		return (moduledesc->status);
	    }
	}
    }
    if (moduledesc->status != UPGRADE_ABORT)
	moduledesc->status = status;
    return (moduledesc->status);
}
/*{
** Name:	mod_dict_status - determine which modules are installed.
**
** Description:
**	This sets the status in the moduletable of all modules which are
**	installed or which are in the process of being upgrade to.  Those
**	are the only two conditions which can be detected by looking at
**	'ii_dict_modules'.
**
** Inputs:
**	none
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	moduletable updated.
**				FAIL:	moduletable not updated.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	21-sep-1990 (pete)
**		Fixed so it is not considered an error if a module is
**		found in ii_dict_modules, which does not exist in
**		the module table (which was built from:
**		  [ingres.files.dictfiles]*.cre)
**	26-sep-1990 (pete)
**		Print out message if error on SELECT query.
**	17-apr-1991 (pete)
**		Don't assume ii_dict_modules exists. We may be upgrading
**		a database with no front-end catalogs at all! Bug 37112.
**	27-jul-1992 (rdrane)
**		Change parameterization of call to FErelexists() to include
**		owner (actually, we can get away with using (char *)NULL -
**		users shouldn't have ii* tables).
**	22-jul-1993 (rdrane)
**		Effect case translation of catalog names prior to calling
**		FErelexists() for FIPS support.
*/
STATUS
mod_dict_status()
{
  i4 local_errno;
  char errtext[257];
  MODULEDESC *thismodule;
  MODULEDESC tempmodule;
    char	table_buf[(FE_MAXNAME + 1)];
    bool	upgrade_abort;
    STcopy(ERx("ii_dict_modules"),&table_buf[0]);
    IIUGdbo_dlmBEobject(&table_buf[0],FALSE);
    if  (FErelexists(&table_buf[0],NULL) != OK)
    {
	goto done;	/* don't attempt to query ii_dict_modules. No modules
			** are installed.
			*/
    }
    /* get status from database */
/* # line 1180 "dctsuprt.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"dctsuprt1",6507,23274);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"dctsuprt1",6507,23274);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select module_name, module_version from ii_dict_modules");
        IIexDefine(0,(char *)"dctsuprt1",6507,23274);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,32,tempmodule.modname);
      IIgetdomio((short *)0,1,30,4,&tempmodule.modversion);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 1188 "dctsuprt.sc" */	/* host code */
	/* find entry in hash table and enter status of INSTALLED */
	if (tempmodule.modversion < 0)
	{
	    /* Upgrade was aborted previously, take absolute value of version */
	    tempmodule.modversion = -tempmodule.modversion;
	    upgrade_abort = TRUE;
	}
	else
	{
	    upgrade_abort = FALSE;
	}
	/* Get module description from cache */
	if (IIUGhfHtabFind(moduletable, &tempmodule, &thismodule) == OK)
	{
	   thismodule->status = upgrade_abort ? UPGRADE_ABORT : INSTALLED;
	}
	else
	    ; /* it's ok if we can't find a module in module table;
	      ** we must allow modules to exist in a database that DON'T exist
	      ** in current FE installation's [ingres.files.dictfiles]*.cre
	      */
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 1210 "dctsuprt.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&local_errno,2);
    IILQisInqSqlio((short *)0,1,32,256,errtext,63);
  }
/* # line 1211 "dctsuprt.sc" */	/* host code */
    if (local_errno != 0)
    {
	/* Any error above will print automatically. Just give the
	** context.
	*/
	IIUGerr(E_UI0030_UI_DBMS_ERROR, UG_ERR_ERROR, 1,
		ERget(F_UI0001_SELECT_DICTMODULES));
	return (FAIL);
    }
done:
    return (OK);
}
/*{
** Name:	get_dictloc - get the path for dictionary files (STATIC).
**
** Description:
**	This sets the static variables DD_loc and DD_loc_buf to contain the
**	location where the dictionary definition files are kept.  If the
**	environment variable II_DICTFILES is set then the files are contained
**	in that path, otherwise the files are in a subdirectory of the INGRES
**	'files' directory called 'dictfiles'.
**
** Inputs:
**	none
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	location set properly.
**				FAIL:	location could not be set.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	04-Jun-1990 (rdesmond).
**		now sets global variables DD_dev and DD_path.
**	28-Oct-1992 (fredb)
**		If II_DICTFILES is not set for MPE/iX, consider the
**		call to get_dictloc a failure.  MPE/iX has a very limited
**		'directory' structure, so we force the use of II_DICTFILES.
**	17-Jun-2004 (schka24)
**	    Safer env var handling.
*/
static
STATUS
get_dictloc()
{
    char	*dictinstall_dir;
    char	dmy[MAX_LOC];
    /*
	Set up path for dictionary installation files.
	Read environment variable for II_DICTFILES and set up path.
    */
    NMgtAt(ERx("II_DICTFILES"), &dictinstall_dir);
    if (dictinstall_dir == NULL || *dictinstall_dir == EOS)
    {
#ifdef hp9_mpe
	return (FAIL);	/* II_DICTFILES must be set for MPE/iX */
#else
	/* II_DICTFILES is not set, use files/dictfiles */
	NMloc(FILES, PATH, NULL, &DD_loc);
	LOcopy(&DD_loc, DD_loc_buf, &DD_loc);
	LOfaddpath(&DD_loc, ERx("dictfiles"), &DD_loc);
#endif
    }
    else
    {
	/* II_DICTFILES is set, use it */
	i2	loc_type;
	STlcopy(dictinstall_dir, DD_loc_buf, sizeof(DD_loc_buf)-1);
	LOfroms(PATH, DD_loc_buf, &DD_loc);
	/* verify that location is a directory */
	LOisdir(&DD_loc, &loc_type);
	if (loc_type != ISDIR)
	    return (FAIL);
    }
    /* save device and path for use later */
    if (LOdetail(&DD_loc, DD_dev, DD_path, dmy, dmy, dmy) != OK)
	return(FAIL);
    else
	return (OK);
}
/*{
** Name:	IIUIfcFindClient - find client in client table.
**
** Description:
**	Do hashed lookup to find a client in client table.
**
** Inputs:
**	char    *cliname	name of client.
**	i4      cliversion	client version.
**
** Outputs:
**
**	Returns:
**		pointer to client, or NULL.
**
** History:
**	13-aug-1991 (pete)
**		Initial Version. Moved over from dict!dictutil directory.
*/
CLIENTDESC *
IIUIfcFindClient(cliname, cliversion)
char    *cliname;
i4      cliversion;
{
        CLIENTDESC  *thisclient;
        CLIENTDESC  clientkey;
        STcopy(cliname, clientkey.cliname);
        clientkey.cliversion = cliversion;
        /* find client */
        if (IIUGhfHtabFind(clienttable, &clientkey, &thisclient) == OK)
            return (thisclient);
        else
            return ((CLIENTDESC *)NULL);
}
/*{
** Name:	IIUIfmFindModule - find module in module table.
**
** Description:
**	Do hashed lookup to find a module in module table.
**
** Inputs:
**	char    *modname	module name.
**	i4      modversion	module version.
**
** Outputs:
**
**	Returns:
**		pointer to module, or NULL.
**
** History:
**	13-aug-1991 (pete)
**		Initial Version. Moved over from dict!dictutil directory.
*/
MODULEDESC *
IIUIfmFindModule(modname, modversion)
char    *modname;
i4      modversion;
{
        MODULEDESC  *thismodule;
        MODULEDESC  modulekey;
        STcopy(modname, modulekey.modname);
        modulekey.modversion = modversion;
        /* find module */
        if (IIUGhfHtabFind(moduletable, &modulekey, &thismodule) == OK)
            return (thismodule);
        else
            return ((MODULEDESC *)NULL);
}
/*{
** Name:	IIUIfmsFindModuleStatus - find status of a module.
**
** Description:
**
** Inputs:
**	char    *cliname	name of client.
**	i4      cliversion	client version.
**
** Outputs:
**
**	Returns:
**		Module status, or 0 if couldn't find module.
**
** History:
**	13-aug-1991 (pete)
**		Initial Version. Moved over from dict!dictutil directory.
*/
i4
IIUIfmsFindModuleStatus(modname, modversion)
char    *modname;
i4      modversion;
{
        MODULEDESC  *thismodule = NULL;
        /* find module and check if installed */
        if ((thismodule = IIUIfmFindModule(modname, modversion)) !=
                                                        (MODULEDESC *)NULL)
            return (thismodule->status);
        else
            return 0;   /* Couldn't find module. ii_client_dep_mod could
                        ** have a module that's not known to current
                        ** installation's *.cre files.
                        */
}
/*
**	Routines to avoid data references across facilities.
**	This makes using Dynamic Link and Shared Libraries possible.
**
**	define DLLSHARELIB to enable for a specific platform.
**
**	17-aug-91 (leighb) DeskTop Porting Change: Created	 6.4_PC_80x86
*/
#ifdef	DLLSHARELIB 				 
PTR
get_clienttable(void)				 
{
	return(clienttable);
}
PTR
get_moduletable(void)				 
{
	return(moduletable);
}
char *
get_DD_dev(void)				 
{
	return(DD_dev);
}
char *
get_DD_path(void)				 
{
	return(DD_path);
}
#endif						 
