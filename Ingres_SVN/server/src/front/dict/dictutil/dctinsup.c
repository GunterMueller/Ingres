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
# include	<si.h>
# include	<st.h>
# include	<ui.h>
# include	<nm.h>
# include	<uigdata.h>
# include	<ut.h>
# include <dictutil.h>
# include	"dctdll.h"	 
# include	"erdu.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/*
** FOOL MING
**
#include "dictutil.h"
*/
/**
** Name:	dctinsup.qsc - support routines for upgradefe
**
** Description:
**	These are some routines which support upgradefe.  This file defines:
**
**	installallmodules	- install the requested modules.
**	removeclidep		- remove client dependency in the dictionary.
**	enterclidep		- enter a client dependency in the dictionary.
**	installmodule		- install one module in the dictionary.
**	createmodule		- create one module in the dictionary.
**	upgrademodule		- upgrade one module in the dictionary.
**	confirmincompat		- get user's confirmation for noncompat upgrade.
**	updatedictmodules	- register a pending module in 'ii_dict_modules'
**	addtodictmodules	- register a new module in 'ii_dict_modules'
**	create_from_file	- create one module using its script file.
**	upgrade_from_file	- upgrade one module using upgrade executable.
**	IIDDiccInstallCompatibleClients() - Install compatible clients.
**	ssicSetStatusInsClients() - set status of installed clients.
**	iccInsCompatClients()	- install compatible clients.
**	samsSetAllModuleStatus()- set status of all modules.
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	19-jul-1990 (pete)
**		Add support for "-s" (silent) flag.
**	20-sep-1990 (pete)
**		change dictinstall to upgradefe.
**      18-oct-1990 (sandyd)
**              Fixed #include of local erdu.h to use "" instead of <>.
**	2-jan-1991 (pete)
**		Make changes to get this all working on Ingres/Star. Basic
**		idea: on Star, we do the Module creations while connected
**		to the CDB, and when all done, we reconnect with Star and
**		register all the newly created tables with Star.
**	31-jul-1991 (pete)
**		Add code to install (by inserting rows in ii_client_dep_mod)
**		all clients that are compatible with installed dictionary
**		modules. Bug 38984.
**	13-aug-1991 (pete)
**		Moved fmsFindModuleStatus(), fmFindModule() and fcFindClient()
**		to UI directory, while working on bug 39165.
**	17-aug-91 (leighb) DeskTop Porting Changes:
**		Added include of si.h;
**		Reference data via a function call instead of referencing
**		data across facilities (for DLL's on IBM/PC).
**	05-sep-91 (leighb) DeskTop Porting Change:
**		Unconditionally include dctdll.h header in all modules 
**		that reference data across facilities; 
**		move cross facilitiy GLOBALREF's to dctdll.h.
**      20-sep-93 (huffman) 
**              Readonly memory is passed to CVlower function, causing
**              Access Violation on AXP VMS.
**	9-nov-1993 (jpk)
**		Fixing my oversight in cat_exists() removes need for
**		previous change; cat_exists now supplies the storage
**		so callers don't have to.
**      11-jan-1996 (toumi01; from 1.1 axp_osf port)
**              Added kchin's change (from 6.4) for axp_osf
**              04-mar-93 (kchin)
**              Added function declaration for  IIxflag(), since it returns
**              char *.  On 64-bit platform, the default int return value
**              for a function will truncate an address which is 64-bit long.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      19-jan-2006 (stial01)
**          iccInsCompatClients() skip clients not requested or uninstallable 
**      25-jan-2006 (stial01)
**          iccInsCompatClients() only skip clients not installable so
**          createdb -> upgradefe will install all clients.
**/
GLOBALREF bool IIDDsilent;	/* TRUE if "-s" (silent) was specified */
GLOBALREF char IIDDResolvefilemsg[];
FUNC_EXTERN i4		IIDDscwSetCatWriteLevel();
FUNC_EXTERN i4		IIDDsxlSetXactLevel();
FUNC_EXTERN bool	IIDDidIsDistributed();
FUNC_EXTERN STATUS	IIDDccCdbConnect();
FUNC_EXTERN STATUS	IIDDcdCdbDisconnect();
FUNC_EXTERN VOID	IIDDcd2CdbDisconnect2();
FUNC_EXTERN STATUS	IIDDiccInstallCompatibleClients();
FUNC_EXTERN STATUS	installallmodules();
FUNC_EXTERN char	*IIDDscStatusDecode();
FUNC_EXTERN char        *IIxflag();
FUNC_EXTERN i4		IIUIfmsFindModuleStatus();
FUNC_EXTERN MODULEDESC  *IIUIfmFindModule();
FUNC_EXTERN CLIENTDESC  *IIUIfcFindClient();
FUNC_EXTERN STATUS	cat_exists(char *catname, char *dummy);
static STATUS		ssicSetStatusInsClients();
static STATUS		iccInsCompatClients();
static VOID		samsSetAllModuleStatus();
static bool		Print_trace = FALSE;
/*{
** Name:	installallmodules - install the requested modules.
**
** Description:
**	This installs all of the modules which are required by the clients
**	previously registered in the clienttable using calls to 'enter_client'
**	(which is in dctsuprt.qsc in UI).
**
** Inputs:
**	noupgrade	- If this flag is set then any client which requires an
**			    upgrade to install any of its required modules will
**			    not be installed.
**	maxcoreversion	- This is the version number of the highest version of
**			    the module CORE which has a definition file in the
**			    dictionary definition directory (DD_path).  This is
**			    the version of CORE which will be created if we are
**			    operating on a database with NO frontend catalogs
**			    (i.e. >= 6.4 Ingres).
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	all modules installed.
**				FAIL:	installation of at least one required
**					  module failed.
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	27-sep-1990 (pete)
**		Add comments for readability.
**	13-mar-1991 (pete)
**		Added more comments!
**	24-jul-1991 (pete)
**		Added even more comments!
**	8-jan-1993 (jpk)
**		maintained FErelexists, added second parameter
**		for owner of table (FIPS support)
**	22-july-1993 (jpk)
**		replaced call to FErelexists with call to cat_exists
**	9-nov-1993 (jpk)
**		Fixing my oversight in cat_exists() removes need for
**		previous change; cat_exists now supplies the storage
**		so callers don't have to.
**	30-nov-1993 (jpk)
**		Fix my bug in my fix to my oversight.  Too much
**		cut and paste: second catalog to check for is
**		ii_objects, not ii_dict_modules, and the sense of
**		the test is if present, not if absent.
*/
STATUS
installallmodules(noupgrade, maxcoreversion)
bool	noupgrade;
i4	maxcoreversion;
{
    char	*dummy;
    CLIENTDESC	*thisclient;
    bool	scanflag;
    bool	atleastonefailed;
    /* Core must exist before going further (it contains ii_dict_modules) */
    if (cat_exists(ERx("ii_dict_modules"), "") != OK)
    {
	/* this database has not been upgraded by upgradefe or upgradewin4gl */
	MODULEDESC	hashkey;
	MODULEDESC	*thismodule;
	i4		coreversion;
	bool		core61;
	if (cat_exists(ERx("ii_objects"), "") == OK)
	{
	    /*
	    ** ASSERTION: this database contains all the 6.[123] front-end
	    ** catalogs. That's equivalent to modules:
	    **			(CORE_61 + APPLICATION_DEVELOPMENT_1 v1)
	    ** We need to install CORE v1. That will give us the same
	    ** set of catalogs as CORE v2.
	    ** (note that CORE v1 + CORE_61 = CORE v2).
	    */
	    core61 = TRUE;
	    coreversion = 1;
	}
	else
	{
	    /* 
	    ** ASSERTION: This database has no front-end catalogs.
	    ** Install most recent version of CORE. CORE v2 is the same
	    ** as CORE_61 + CORE v1.
	    ** (APPLICATION_DEVELOPMENT_1 will be created later, based on
	    ** Client module dependencies).
	    */
	    core61 = FALSE;
	    coreversion = maxcoreversion;
	}
	/* find entry in hash module table for CORE to install */
	STcopy(ERx("CORE"), hashkey.modname);
	hashkey.modversion = coreversion;
	if (IIUGhfHtabFind(moduletable, &hashkey, &thismodule) != OK)
	{
	    IIUGerr(E_DD000D_CANT_FIND_MOD_DEF, UG_ERR_ERROR, 2,
	      hashkey.modname, &hashkey.modversion);
	    return (FAIL);
	}
	if (createmodule(thismodule) != OK)
	{
	    IIUGerr(E_DD000E_CANT_CREATE_MOD, UG_ERR_ERROR, 2,
		thismodule->modname, &thismodule->modversion);
	    return (FAIL);
	}
	/*
	** The catalogs for CORE v2 (or CORE_61 + CORE v1 which are
	** equivalent to CORE v2) now exist. APPLICATION_DEVELOPMENT_1 v1
	** also exists if (core61 == TRUE), but it's not registered.
	*/
	if (core61)
	{
	    /* this was originally a 6.[123] database. It now contains
	    ** catalogs for modules CORE v1, CORE_61 and
	    ** APPLICATION_DEVELOPMENT_1 v1; the later 2 aren't
	    ** registered in ii_dict_modules yet; do that now.
	    */
	    char	*modname;
	    char	*remark;
	    char	one;
	    one = 1;
	    modname = ERx("CORE_61");
	    remark = ERx("Front-end catalogs for release 6.1-6.3");
	    if (addtodictmodules(modname, 1, remark) != OK)
	    {
		IIUGerr(E_DD000F_CANT_REGISTER_MOD, UG_ERR_ERROR, 2,
		  modname, &one);
		return (FAIL);
	    }
	    modname = ERx("APPLICATION_DEVELOPMENT_1");
	    remark = ERx("Base Ingres Catalogs <=6.3");
	    if (addtodictmodules(modname, 1, remark) != OK)
	    {
		IIUGerr(E_DD000F_CANT_REGISTER_MOD, UG_ERR_ERROR, 2,
		  modname, &one);
		return (FAIL);
	    }
	}
    }
    /* ASSERTION: the database is ready to create and register dictionary
    ** modules in. Any bootstrapping (creating module CORE) was done above.
    */
    /* SELECT from ii_dict_modules; for each module update the in-memory
    ** module table with info on modules installed in this database
    ** and status of each (INSTALLED or UPGRADE_ABORT).
    **
    ** Note that ii_dict_modules only contains name and version of
    ** the currently installed modules. If, for example, module Core v1
    ** was upgraded to v2, then only a row for Core v2 will exist in
    ** ii_dict_modules.
    ** However, catalog ii_client_dep_mod retains info on all present and
    ** past clients and which modules they are dependent on. But when a
    ** compatible upgrade is done, the module info for the prior versions of
    ** the client are updated to = the currently installed module version
    ** numbers. Thus, for compatible upgrades, the module info for
    ** all versions of a client in ii_client_dep_mod should be the same.
    */
    if (mod_dict_status() != OK)
	return (FAIL);
    atleastonefailed = FALSE;
    /* Scan the client table for all clients which need to be supported */
    for (scanflag = FALSE; 
      IIUGhsHtabScan(clienttable, scanflag, &dummy, &thisclient) != 0;
      scanflag = TRUE)
    {
	MODULEDESC	*thismodule;
	STATUS		modstatus;
	STATUS		installstatus;
	bool		installable;
	bool		goodinstall;
	i4		index;
	/* if not requested to be installed then ignore it */
	if (!thisclient->required)
	    continue;
	/* check if client already installed (select from ii_client_dep_mod)*/
	if ((installstatus = IIUIClientInstalled(thisclient->cliname,
	  thisclient->cliversion)) == FAIL)
	    return (FAIL);
	if (installstatus == OK)
	{
	    /* Requested client is already installed. Give informatory msg */
	    if (!IIDDsilent)
		IIUGmsg(ERget(S_DD000C_CLI_ALRDY_INSTALLED), (bool)FALSE, 2,
		    thisclient->cliname, &thisclient->cliversion);
	    continue;
	}
	else if (installstatus == UI_DESUPPORTED)
	{
	    /* Requested client is no longer supported.  An earlier module
	    ** upgrade must have produced a module that is not upwardly
	    ** compatible with a module that this client requires.
	    ** For example module FOO v1 was upgraded to FOO v2 and v2
	    ** clients who need v1 cannot run on v2.
	    */
	    IIUGerr(E_DD0008_CLI_UNINSTALLABLE, UG_ERR_ERROR, 2,
	      thisclient->cliname, &thisclient->cliversion);
	    continue;
	}
	/* ASSERTION: user wants the current client to be installed,
	**	and it is not currently installed (not sure yet if
	**	its installable).
	*/
	/*
	** Check if all required modules for this client are
	** installable. Also, if any module upgrades
	** are not upwardly compatible and will cause a client to be
	** desupported then get user confirmation
	*/
	for (installable = TRUE, index = 0; 
	  installable && index < thisclient->numsupmods; index++)
	{
	    thismodule = thisclient->supmods[index];
	    modstatus = modulestatus(thismodule);
	    if (noupgrade &&			/* -c flag specified */
		 (modstatus == ANC_COMPAT ||	/* Ancestor module is installed
						** & upgrade from it to this
						** module would be backward
						** compatible. */
	          modstatus == ANC_INCOMPAT ||	/* Ancestor module is installed
						** & upgrade from it to this
						** module wouldn't be backward
						** compatible. */
		  modstatus == DESC_COMPAT ||	/* A compatible descendent
						** (child -- e.g. one with
						** higher version nmbr) of
						** this module is installed. */
	          modstatus == UPGRADE_ABORT)	/* Previous installation of
						** this module aborted before
						** finished. */
		)
	    {
		/* User specified -c flag on command line, and
		** a module must be upgraded to do the installation.
		** The -c flag requires that no module upgrades be done
		** to install a client.
		*/
		IIUGerr(S_DD0012_REQUIRES_UPGRADE, UG_ERR_ERROR, 4,
		  thismodule->modname, &thismodule->modversion,
		  thisclient->cliname, &thisclient->cliversion);
		installable = FALSE;
	    }
	    else if (modstatus == ANC_INCOMPAT)
	    {
		/* Get confirmation that some clients will be desupported */
		if (!confirmincompat(thisclient, thismodule->parents[0]))
		    installable = FALSE;
	    }
	    else if (modstatus == DESC_INCOMPAT)
	    {
		IIUGerr(E_DD0008_CLI_UNINSTALLABLE, UG_ERR_ERROR, 2,
		  thisclient->cliname, &thisclient->cliversion);
		installable = FALSE;
	    }
	    else if (modstatus == FAIL)
	    {
		    installable = FALSE;
	    }
	}
	/* If the client is installable then install here */
	if (installable)
	{
	    /* First remove all of the client dependencies */
	    if (removeclidep(thisclient) != OK)
		goodinstall = FALSE;
	    else
		goodinstall = TRUE;
	    /*
		installmodule() is called once for each module required,
		regardless of whether it is already installed.
	    */
	    for (index = 0; goodinstall && index < thisclient->numsupmods;
	      index++)
	    {
		thismodule = thisclient->supmods[index];
		if (installmodule(thismodule) != OK)
		    goodinstall = FALSE;
	    }
	    if (goodinstall)
	    {
		/* If the modules have been installed then enter client deps */
		IIDDbnxBeginNestedXact();
		for (index = 0; goodinstall && index < thisclient->numsupmods;
		  index++)
		{
		    thismodule = thisclient->supmods[index];
		    if (enterclidep(thisclient, thismodule) != OK)
			goodinstall = FALSE;
		}
	    }
	    if (goodinstall)
	    {
		IIDDenxEndNestedXact();
		if (!IIDDsilent)
		    IIUGmsg(ERget(S_DD000B_CLI_INSTALLED), (bool)FALSE, 2,
			thisclient->cliname, &thisclient->cliversion);
	    }
	    else
	    {
		IIDDanxAbortNestedXact();
		IIUGerr(E_DD0009_CANT_INSTALL_CLI, UG_ERR_ERROR, 2,
		    thisclient->cliname, &thisclient->cliversion);
		atleastonefailed = TRUE;
	    }
	}
    }
    return (atleastonefailed ? FAIL : OK);
}
/*{
** Name:	removeclidep - remove client dependency in the dictionary.
**
** Description:
**	This will remove all dependency entries from 'ii_client_dep_mod' for
**	one client.
**
** Inputs:
**	clientdesc	- CLIENTDESC for client to deregister.
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	client deregistered.
**				FAIL:	client not deregistered.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
*/
STATUS
removeclidep(clientdesc)
  CLIENTDESC *clientdesc;
{
  i4 local_errno;
  char errtext[257];
    IIDDbnxBeginNestedXact();
    IIDDnw1NestedWriteOn();
/* # line 477 "dctinsup.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"dctinsup1",7042,10750);
      IIputdomio((short *)0,1,32,0,clientdesc->cliname);
      IIputdomio((short *)0,1,30,4,&clientdesc->cliversion);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"dctinsup1",7042,10750);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_client_dep_mod where client_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,clientdesc->cliname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and client_version=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&clientdesc->cliversion);
        IIexDefine(0,(char *)"dctinsup1",7042,10750);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 480 "dctinsup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&local_errno,2);
    IILQisInqSqlio((short *)0,1,32,256,errtext,63);
  }
/* # line 481 "dctinsup.sc" */	/* host code */
    if (local_errno != 0)
    {
	IIDDnw0NestedWriteOff();
	IIDDanxAbortNestedXact();
	IIUGerr(E_DD0019_DBMS_ERROR, UG_ERR_ERROR, 1, errtext);
	return (FAIL);
    }
    IIDDnw0NestedWriteOff();
    IIDDenxEndNestedXact();
    return (OK);
}
/*{
** Name:	enterclidep - enter a client dependency in the dictionary.
**
** Description:
**	Enter a dependency of a client on a module.  At least one entry must 
**	exist in 'ii_client_dep_mod' for each client in order to be registered.
**
** Inputs:
**	clientdesc	- CLIENTDESC for client to enter dependency for.
**	moduledesc	- MODULEDESC for module which client is dependent on.
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	dependency entered.
**				FAIL:	dependency not entered.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
*/
STATUS
enterclidep(clientdesc, moduledesc)
  CLIENTDESC *clientdesc;
MODULEDESC	*moduledesc;
{
  MODULEDESC *thismodule;
  i4 local_errno;
  char errtext[257];
    thismodule = moduledesc->status != DESC_COMPAT ? moduledesc :
      moduledesc->installed_child;
    IIDDnw1NestedWriteOn();
/* # line 533 "dctinsup.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"dctinsup2",7043,15228);
      IIputdomio((short *)0,1,32,0,clientdesc->cliname);
      IIputdomio((short *)0,1,30,4,&clientdesc->cliversion);
      IIputdomio((short *)0,1,32,0,thismodule->modname);
      IIputdomio((short *)0,1,30,4,&thismodule->modversion);
      IIputdomio((short *)0,1,32,0,thismodule->remark);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"dctinsup2",7043,15228);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_client_dep_mod(client_name, client_version, module_nam\
e, module_version, short_remark)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,clientdesc->cliname);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&clientdesc->cliversion);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,thismodule->modname);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&thismodule->modversion);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,thismodule->remark);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"dctinsup2",7043,15228);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 549 "dctinsup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&local_errno,2);
    IILQisInqSqlio((short *)0,1,32,256,errtext,63);
  }
/* # line 550 "dctinsup.sc" */	/* host code */
    if (local_errno != 0)
    {
	IIDDnw0NestedWriteOff();
	IIUGerr(E_DD0019_DBMS_ERROR, UG_ERR_ERROR, 1, errtext);
	return (FAIL);
    }
    IIDDnw0NestedWriteOff();
    return (OK);
}
/*{
** Name:	installmodule - install one module in the dictionary.
**
** Description:
**	Install one module in the dictionary, regardless of whether it is
**	to be created from scratch or upgraded to.  It is assumed that the
**	module has been checked for installability prior to calling this.
**
** Inputs:
**	moduledesc	- MODULEDESC for module to install.
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	module installed.
**				FAIL:	module not installed.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	22-apr-1991 (pete)
**		Add an additional argument (the status return from UTexe)
**		to error E_DD0011_CANT_UPGRADE_MOD, which necessitates
**		adding new error: E_DD0026_CANT_UPGRADE_MOD2.
*/
STATUS
installmodule(moduledesc)
MODULEDESC	*moduledesc;
{
    STATUS	status;
    status = modulestatus(moduledesc);
    if (status == DESC_COMPAT)
    {
	return (OK);
    }
    else if (status == INSTALLED)
    {
	if (!IIDDsilent)
	    IIUGmsg(ERget(S_DD0013_MOD_ALRDY_INSTALLED), (bool)FALSE, 2,
		moduledesc->modname, &moduledesc->modversion);
	return (OK);
    }
    else if (status == NOTINSTALLED)
    {
	status = createmodule(moduledesc);
	if (status == OK)
	    moduledesc->status = INSTALLED;
	return (status);
    }
    else
    {
	if (!IIDDsilent)
	    IIUGmsg(ERget(S_DD0015_UPGRADING_MOD), (bool)FALSE, 2,
		moduledesc->modname, &moduledesc->modversion);
	status = upgrademodule(moduledesc);
	if (status != OK)
	    IIUGerr(E_DD0026_CANT_UPGRADE_MOD2, UG_ERR_ERROR, 3,
	      moduledesc->modname, &moduledesc->modversion, &status);
	else
	    moduledesc->status = INSTALLED;
	return (status);
    }
}
/*{
** Name:	createmodule - create one module in the dictionary.
**
** Description:
**	Creates one module from scratch.  Assumes module has been checked for
**	installability and that no ancestors of the module are installed.
**
** Inputs:
**	moduledesc	- MODULEDESC of module to create.
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	module created.
**				FAIL:	module not created.
**					  module failed.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	2-jan-1991 (pete)
**              Make changes for Star. If Star, we'll connect
**              to the CDB, via a "DIRECT CONNECT" stmt. Reason:
**              Can't do GRANTs or MODIFYs in Star. So strategy = issue
**              statements directly in the CDB and save info on any CREATE,DROP
**		or MODIFY statements issued; then do DIRECT DISCONNECT
**              to return to star and issue a REGISTER (CREATE TABLE) or
**              REMOVE (DROP TABLE) statement on each new or dropped table.
**              Complication: if upgradefe has been run directly on the CDB,
**              then tables may exist that aren't registered,
**              hence a CREATE may fail but the REGISTER would work (so we
**		ignore a "create dup table" error for star).
*/
STATUS
createmodule(moduledesc)
  MODULEDESC *moduledesc;
{
    STATUS	status;
    bool	fail;
    if (!IIDDsilent)
	IIUGmsg(ERget(S_DD0014_CREATING_MOD), (bool)FALSE, 2,
	    moduledesc->modname, &moduledesc->modversion);
    fail = FALSE;
    if (IIDDidIsDistributed())
    {
	/* We're connected to Star; must execute statements on CDB. Routines
	** "create_from_file" and "modifymodule" each save up names
	** of tables involved in CREATE TABLE, DROP TABLE & MODIFY statements.
	*/
	if (IIDDccCdbConnect() != OK)
	{
	    fail = TRUE;
	    goto done;
	}
    }
    IIDDbnxBeginNestedXact();
    IIDDnw1NestedWriteOn();
    /* create catalogs from file */
    if ((status = create_from_file(moduledesc)) != OK)
    {
	fail = TRUE;
	goto done;
    }
    if ((status = modifymodule(moduledesc, (bool)FALSE)) != OK)
    {
	/* modify catalogs from file */
	fail = TRUE;
	goto done;
    }
    if (addtodictmodules(moduledesc->modname, moduledesc->modversion,
      moduledesc->remark) != OK)
    {
	fail = TRUE;
	goto done;
    }
    if (IIDDidIsDistributed())
    {
	/* Above work was done while attached to CDB; register it with Star
	** & then disconnect from CDB.
	*/
	if (IIDDcdCdbDisconnect() != OK)	/* note: this COMMITs */
	{
	    fail = TRUE;
	    goto done;
	}
    }
done:
    IIDDnw0NestedWriteOff();
    if (fail)
    {
	IIDDanxAbortNestedXact();
	/* error above could have left us still connected to CDB */
	if (IIDDidIsDistributed())
	{
	    IIDDcd2CdbDisconnect2();    /* direct disconnect from CDB. */
	}
	IIUGerr(E_DD000E_CANT_CREATE_MOD, UG_ERR_ERROR, 2,
	  moduledesc->modname, &moduledesc->modversion);
    }
    else
    {
	IIDDenxEndNestedXact();
    }
    return (fail ? FAIL : OK);
}
/*{
** Name:	upgrademodule - upgrade one module in the dictionary.
**
** Description:
**	Install one module by upgrading from its currently installed ancestor.
**	If the currently installed ancestor is more than one generation away
**	then each successive descendant of it will be installed iteratively
**	until the requested module is installed.
**	Assumes module has been checked for installability.
**
** Inputs:
**	moduledesc	- MODULEDESC of module to upgrade to.
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	module upgraded to.
**				FAIL:	module not upgraded to.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	04-Sep-1990 (rdesmond).
**		Fixed bug testing if parent is installed.
**	2-Oct-1990 (pete)
**		Fixed bug 33669 with upgradefe not recovering after an
**		upgrade abort.
**	22-apr-1991 (pete)
**		Changed to return "status", rather than FAIL if call to
**		upgrademodule doesn't return OK.
*/
STATUS
upgrademodule(moduledesc)
  MODULEDESC *moduledesc;
{
  i4 local_errno;
  char errtext[257];
  MODULEDESC *thisparent;
    STATUS	status;
    i4		index;
    /*
    ** If current Module has status UPGRADE_ABORT, then previous run
    ** didn't complete successfully AND it failed while the upgrade
    ** executable was being run. We can assume that any
    ** parent modules are already installed (in fact trying to install them
    ** again will cause errors about trying to create a duplicate table)
    ** and skip the check of parent modules.
    */
    if (moduledesc->status == UPGRADE_ABORT)
	goto chkthismod;
    /* Need to install parents (ancestors) if not already installed. */
    for (index = 0; index < moduledesc->numparents; index++)
    {
	thisparent = moduledesc->parents[index];
	if (modulestatus(thisparent) != INSTALLED)
	{
	    status = ancstatus(thisparent);
	    /* propagate incompatiblity from parent if ancestor installed */
	    if (status == ANC_COMPAT || status == ANC_INCOMPAT ||
	      status == UPGRADE_ABORT)
	    {
		/* need to upgrade to parent module */
		if ((status = upgrademodule(thisparent)) != OK)
		    return (status);
	    }
	    else if (status == NOTINSTALLED)
	    {
		/* need to create parent module */
		if ((status = createmodule(thisparent)) != OK)
		    return (FAIL);
	    }
	}
    }
chkthismod:
    /* If this upgrade was previously aborted we don't need to register it */
    if (modulestatus(moduledesc) != UPGRADE_ABORT)
    {
	IIDDbnxBeginNestedXact();
	IIDDnw1NestedWriteOn();
	for (index = 0; index < moduledesc->numparents; index++)
	{
	    if (moduledesc->compat)
	    {
		thisparent = moduledesc->parents[index];
		/* change dependencies on prev version to depend on this ver */
/* # line 831 "dctinsup.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"dctinsup3",7044,11121);
      IIputdomio((short *)0,1,32,0,moduledesc->modname);
      IIputdomio((short *)0,1,30,4,&moduledesc->modversion);
      IIputdomio((short *)0,1,32,0,thisparent->modname);
      IIputdomio((short *)0,1,30,4,&thisparent->modversion);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"dctinsup3",7044,11121);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_client_dep_mod set module_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,moduledesc->modname);
        IIwritio(0,(short *)0,1,32,0,(char *)", module_version=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&moduledesc->modversion);
        IIwritio(0,(short *)0,1,32,0,(char *)"where module_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,thisparent->modname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and module_version=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&thisparent->modversion);
        IIexDefine(0,(char *)"dctinsup3",7044,11121);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 839 "dctinsup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&local_errno,2);
    IILQisInqSqlio((short *)0,1,32,256,errtext,63);
  }
/* # line 841 "dctinsup.sc" */	/* host code */
		if (local_errno != 0)
		{
		    IIDDnw0NestedWriteOff();
		    IIDDanxAbortNestedXact();
		    IIUGerr(E_DD0019_DBMS_ERROR, UG_ERR_ERROR, 1, errtext);
		    return (FAIL);
		}
	    }
/* # line 852 "dctinsup.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"dctinsup4",7045,6907);
      IIputdomio((short *)0,1,32,0,thisparent->modname);
      IIputdomio((short *)0,1,30,4,&thisparent->modversion);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"dctinsup4",7045,6907);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_dict_modules where module_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,thisparent->modname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and module_version=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&thisparent->modversion);
        IIexDefine(0,(char *)"dctinsup4",7045,6907);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 855 "dctinsup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&local_errno,2);
    IILQisInqSqlio((short *)0,1,32,256,errtext,63);
  }
/* # line 857 "dctinsup.sc" */	/* host code */
	    if (local_errno != 0)
	    {
		IIDDnw0NestedWriteOff();
		IIDDanxAbortNestedXact();
		IIUGerr(E_DD0019_DBMS_ERROR, UG_ERR_ERROR, 1, errtext);
		return (FAIL);
	    }
	}
	/*
	    update ii_dict_modules to reflect new module (with negative version
	    number to show that an upgrade is in progress).
	*/
	if (addtodictmodules(moduledesc->modname, -(moduledesc->modversion),
	  moduledesc->remark) != OK)
	{
	    IIDDnw0NestedWriteOff();
	    IIDDanxAbortNestedXact();
	    return (FAIL);
	}
	IIDDnw0NestedWriteOff();
	IIDDenxEndNestedXact();
    }
    if ((status = upgrade_from_file(moduledesc)) == OK)
    {
	/*
	    Conversion succeeded, update ii_dict_modules.
	    If conversion program cannot tell the difference between an
	    unstarted conversion and a completed conversion, then it
	    should update ii_dict_modules itself in the same transaction
	    that it removes any temp table to keep track of its conversion
	    status.  If it doesn't update ii_dict_modules then it may later
	    be called to recover from an aborted upgrade when it actually
	    succeeded (but ii_dict_modules failed to be updated to indicate
	    completion).  It would then get confused and screw up everything.
	*/
	return(updatedictmodules(moduledesc->modname, moduledesc->modversion));
    }
    else
    {
	return (status);
    }
}
/*{
** Name:	confirmincompat - get user's confirmation for noncompat upgrade.
**
** Description:
**	This will get the user's confirmation to proceed with an installation
**	when at least one client will be desupported as a result of the
**	installation.  The list of clients which will be desupported will be
**	displayed to the user along with the appropriate confirmation prompt.
**
** Inputs:
**	thisclient	- CLIENTDESC for client which is being installed.
**	thismodule	- MODULEDESC for module whose installation will cause
**			    at least one installed client to be desupported.
**
** Outputs:
**
**	Returns:
**		bool	-	TRUE:	user confirmed request to upgrade.
**				FAIL:	user cancelled upgrade request.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
*/
bool
confirmincompat(thisclient, thismodule)
CLIENTDESC	*thisclient;
  MODULEDESC *thismodule;
{
  i4 local_errno;
  char errtext[257];
  char cliname[FE_MAXNAME +1];
  i4 cliversion;
    char            yorn[6];
    /* Print 3 part message for user; first part tells them that
    ** a client(s) will be desupported if this installation proceeds.
    ** Second part is the query which shows which clients are dependent
    ** on the Module that needs to be upgraded. Third part is a prompt
    ** for whether they want to proceed with the upgrade.
    */
    IIUGmsg(ERget(S_DD0017_CLI_DESUPPORTED), (bool)FALSE, 2,
	thisclient->cliname, &thisclient->cliversion);
    IIDDnr1NestedReadOn(ERx("ii_client_dep_mod"));
/* # line 951 "dctinsup.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"dctinsup5",7046,5745);
      IIputdomio((short *)0,1,32,0,thismodule->modname);
      IIputdomio((short *)0,1,30,4,&thismodule->modversion);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"dctinsup5",7046,5745);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select client_name, client_version from ii_client_dep_mod where modul\
e_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,thismodule->modname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and module_version=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&thismodule->modversion);
        IIexDefine(0,(char *)"dctinsup5",7046,5745);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME +1-1,cliname);
      IIgetdomio((short *)0,1,30,4,&cliversion);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 964 "dctinsup.sc" */	/* host code */
	IIUGmsg(ERget(S_DD0018_MOD_OR_CLI_NAME), (bool)FALSE, 2,
	  cliname, &cliversion);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 967 "dctinsup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&local_errno,2);
    IILQisInqSqlio((short *)0,1,32,256,errtext,63);
  }
/* # line 968 "dctinsup.sc" */	/* host code */
    IIDDnr0NestedReadOff(ERx("ii_client_dep_mod"));
    if (local_errno != 0)
    {
	IIUGerr(E_DD0019_DBMS_ERROR, UG_ERR_ERROR, 1, errtext);
	return (FALSE);
    }
    yorn[0] = EOS;
    FEprompt(ERget(F_DD0000_INCOMPAT_PROMPT), FALSE, 5, yorn);
    if(!IIUGyn(yorn, (STATUS *)NULL))
	return (FALSE);
    else
	return (TRUE);
}
/*{
** Name:	updatedictmodules - register pending module in 'ii_dict_modules'
**
** Description:
**	Register a module which is currently pending installation.  A module
**	which is created through an upgrade of an existing module is registered
**	'pending' before the upgrade begins (by the addition of a row in
**	'ii_dict_modules' having the negative of the version number of the
**	module).  After the upgrade is successful this routine is called to
**	register the module as fully installed.  It is assumed that there
**	already exists an entry in 'ii_dict_modules' for the module for which
**	the absolute value of the version number is the version number of the
**	module.
**
** Inputs:
**	modname		- name of module to register.
**	modversion	- version of module to register.
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	module registered.
**				FAIL:	module not registered.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
*/
STATUS
updatedictmodules(modname, modversion)
  char *modname;
  i4 modversion;
{
  i4 local_errno;
  char errtext[257];
  i4 neg_modversion;
    neg_modversion = -modversion;
    IIDDnw1NestedWriteOn();
/* # line 1028 "dctinsup.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"dctinsup6",7047,28783);
      IIputdomio((short *)0,1,30,4,&modversion);
      IIputdomio((short *)0,1,32,0,modname);
      IIputdomio((short *)0,1,30,4,&modversion);
      IIputdomio((short *)0,1,30,4,&neg_modversion);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"dctinsup6",7047,28783);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_dict_modules set module_version=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&modversion);
        IIwritio(0,(short *)0,1,32,0,(char *)"where module_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,modname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(module_version=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&modversion);
        IIwritio(0,(short *)0,1,32,0,(char *)"or module_version=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&neg_modversion);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"dctinsup6",7047,28783);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 1039 "dctinsup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&local_errno,2);
    IILQisInqSqlio((short *)0,1,32,256,errtext,63);
  }
/* # line 1040 "dctinsup.sc" */	/* host code */
    IIDDnw0NestedWriteOff();
    if (local_errno != 0)
    {
	IIUGerr(E_DD0019_DBMS_ERROR, UG_ERR_ERROR, 1, errtext);
	return (FAIL);
    }
    return (OK);
}
/*{
** Name:	addtodictmodules - register a new module in 'ii_dict_modules'
**
** Description:
**	Inserts a new row into 'ii_dict_modules' to register the module.
**	When a module has been fully installed this is called with the version
**	number of the module. When an upgrade is to be registered this is called
**	with the negative of the version number of the module.
**
** Inputs:
**	modname		- name of module to register.
**	modversion	- version of module to register.
**	remark		- remark for module to register.
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	module registered.
**				FAIL:	module not registered.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
*/
STATUS
addtodictmodules(modname, modversion, remark)
  char *modname;
  i4 modversion;
  char *remark;
{
  i4 local_errno;
  char errtext[257];
    IIDDnw1NestedWriteOn();
/* # line 1090 "dctinsup.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit(&sqlca);
      IIexExec(1,(char *)"dctinsup7",7048,29040);
      IIputdomio((short *)0,1,32,0,modname);
      IIputdomio((short *)0,1,30,4,&modversion);
      IIputdomio((short *)0,1,32,0,remark);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit(&sqlca);
        IIexDefine(1,(char *)"dctinsup7",7048,29040);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_dict_modules(module_name, module_version, short_remark\
)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,modname);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&modversion);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,remark);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"dctinsup7",7048,29040);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 1102 "dctinsup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&local_errno,2);
    IILQisInqSqlio((short *)0,1,32,256,errtext,63);
  }
/* # line 1103 "dctinsup.sc" */	/* host code */
    IIDDnw0NestedWriteOff();
    if (local_errno != 0)
    {
	IIUGerr(E_DD0019_DBMS_ERROR, UG_ERR_ERROR, 1, errtext);
	return (FAIL);
    }
    return (OK);
}
/*{
** Name:	create_from_file - create one module using its script file.
**
** Description:
**	Create module from script file.  This assumes that the module is
**	installable and that no ancestor of the module is currently installed.
**
** Inputs:
**	moduledesc	- MODULEDESC of module to create.
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	module created..
**				FAIL:	module not created.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	04-jun-1990 (rdesmond).
**		now uses DD_dev and DD_path to construct location.
*/
STATUS
create_from_file(moduledesc)
MODULEDESC	*moduledesc;
{
    LOCATION	*tmp_loc;
    if (resolve_file(moduledesc->filebase, CREATE, &tmp_loc) == OK)
	return (execsql_from_file(tmp_loc, moduledesc->numparents + 3));
    else
	return (FAIL);
}
/*{
** Name:	upgrade_from_file - upgrade one module using upgrade executable.
**
** Description:
**	Install one module using the executable to upgrade from its parent.
**	This assumes that the module is installable and that the module's
**	parent is installed.
**
** Inputs:
**	moduledesc	- MODULEDESC of module to upgrade to.
**
** Outputs:
**
**	Returns:
**		STATUS	-	OK:	module upgraded to.
**				FAIL:	module not upgraded to.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	May 1990 (rdesmond).
**		first created.
**	04-jun-1990 (rdesmond).
**		now uses DD_dev and DD_path to construct location.
**	27-sep-1990 (pete)
**		Change code to require that an upgrade executable exist.
**		(had to also create several dummy executables for modules
**		that get upgraded, but for which no upgrade executable
**		exists: msc01u.sc, pdm01u.sc, cor02u.sc).
**	4-jan-1991 (pete)
**		Make changes for Star. Upgrade executables will connect
**		to the CDB to do their work. If they get an error, then
**		they could return with us still connected to the CDB.
**	11-apr-1991 (pete)
**		Add message E_DD0025_NO_UPGRADE_FILE and report full path
**		name of file when upgradefe can't find an upgrade
**		executable. Tech support had couple calls from VMS sites
**		where executable couldn't be found -- but appeared to be
**		perfectly ok.
*/
STATUS
upgrade_from_file(moduledesc)
MODULEDESC	*moduledesc;
{
    STATUS	status;
    LOCATION	*tmp_loc;
    /* find the UPGRADE executable in $II_BINARY */
    if ((status = resolve_file(moduledesc->filebase, UPGRADE, &tmp_loc)) == OK)
    {
	char		*arglist;
	char		*execname;
        CL_ERR_DESC	err_code;
	/* spawn upgrade program */
	LOtos(tmp_loc, &execname);
	arglist = ERx("executable = %S, database = %S, equel = %S, user = %S");
	status = UTexe(UT_WAIT, NULL, NULL, NULL, ERx("dictupgrade"), &err_code,
	  arglist, 4, execname, IIuiDatabase, IIxflag(), IIuiUser);
        /* guard against being in unknown transaction state in conversion */
/* # line 1207 "dctinsup.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 1208 "dctinsup.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT on");
    IIsyncup((char *)0,0);
  }
/* # line 1209 "dctinsup.sc" */	/* host code */
	if (IIDDidIsDistributed())
	{
	    if (status != OK)
	    {
		/* If error occurs in upgrade executable, we may be left
		** connected to the CDB. Connect back to Star.
		*/
/* # line 1217 "dctinsup.sc" */	/* direct disconnect */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"direct disconnect");
    IIsyncup((char *)0,0);
  }
/* # line 1218 "dctinsup.sc" */	/* host code */
	    }
	}
    }
    else	/* status == FAIL */
    {
	char *locstring;
	LOtos(tmp_loc, &locstring);
	/* An upgrade executable should be in [ingres.bin],  but isn't */
	IIUGerr(E_DD0025_NO_UPGRADE_FILE, UG_ERR_ERROR, 4,
		moduledesc->modname, &moduledesc->modversion, locstring,
		IIDDResolvefilemsg);
    }
    return (status);
}
/*{
** Name:        IIDDiccInstallCompatibleClients - install all compatible clients
**
** Description:
**	SELECT list of installed modules
**	    set status for each installed module in module table
**	    (already done?: MODULEDESC.status=INSTALLED
**	End
**	Scan module table
**	    For each not-installed module (MODULEDESC.status!=INSTALLED):
**		Find a compatible version of that module that is
**		installed. Set MODULEDESC.childCompatVers.
**		"compatible version" = same name and higher version
**		number and compatible all the way up.
**	        (already done?: check MODULEDESC.installed_child when
**		 MODULEDESC.status=DESC_COMPAT
**	    End
**	End
**	SELECT from ii_client_dep_mod.
**	    set status for installed clients (set CLIENTDESC.status=INSTALLED)
**	    ("installed" = all rows in ii_client_dep_mod exist in
**	     ii_dict_modules & module version numbers match)
**	End
**	Scan client table.
**	    For each not-installed client:
**		For each module in the client
**		    Is it upward compatible with an installed module?
**		End
**		If ALL modules upward compatible, then install client,
**		but use module vers. nmbrs from compatible installed modules.
**	    End
**	End
**
**	The client and module tables are assumed to be consistent. No
**	checking or error reporting is done about integrity problems in
**	these in-memory tables.
**
** Inputs:
**
** Outputs:
**
**      Returns:
**		STATUS  -	OK:     all clients added.
**				FAIL:   had problems adding compatible clients.
** History:
**	31-jul-1991 (pete)
**		Initial version.
*/
STATUS
IIDDiccInstallCompatibleClients()
{
	STATUS stat;
	char   *p_nm;
        /* to print runtime info */
        NMgtAt(ERx("II_DDTRACE"), &p_nm);
        if ( (p_nm != NULL) && (*p_nm != EOS))
        {
            /* II_DDTRACE set; write dictionary trace info to stdout */
	    Print_trace = TRUE;
        }
	/* Set MODULEDESC.status for every module. Currently status is only
	** set for installed modules, and for modules required by requested
	** clients.
	*/
	samsSetAllModuleStatus();
	/* set CLIENTDESC.status=INSTALLED for installed clients */
	if ((stat = ssicSetStatusInsClients()) == OK)
	{
	    /* Install compatible not-installed clients by inserting rows
	    ** into ii_client_dep_mod.
	    */
	    stat = iccInsCompatClients();
	}
	return stat;
}
/*{
** Name: samsSetAllModuleStatus - Set status of all modules.
**
** Description:
**	Set status of every module.
**
** Inputs:
**	NONE
**
** Outputs:
**
**      Returns:
**		VOID
** History:
**	2-aug-1991 (pete)
**		Initial version.
**	15-aug-1991 (pete)
**		Force re-evaluation of all module status's by changing
**		status to UNKNOWN; current round of upgradefe processing
**		may have affected status. Subsequent call to
**		modulestatus() will set it correct again. Bug 39282.
**		Fix up trace output a bit.
*/
static VOID
samsSetAllModuleStatus()
{
    STATUS	stat = OK;
    bool	scanflag;
    MODULEDESC	*thismodule;
    char	*dummy;
    i4		i, j;
    bool	first = TRUE;		
#define	MAX_NOSTATUS_MODULES	100
    MODULEDESC	*(mods[MAX_NOSTATUS_MODULES]);
    /* find list of modules which don't have a status */
    for (scanflag = FALSE, i=0;
      IIUGhsHtabScan(moduletable, scanflag, &dummy, &thismodule) != 0;
      scanflag = TRUE)
    {
	if ((thismodule->status != INSTALLED) &&
	    (thismodule->status != UPGRADE_ABORT) &&
	    (thismodule->status != UNKNOWN))
	{
	    if (Print_trace)
	    {
		if (first)
		{
	     SIprintf("\n  ********************************************");
	     SIprintf("\n  Change module status to force re-evaluation:");
    	     SIprintf("\n");
		    first = FALSE;
		}
	    	SIprintf("\n    <mod %s, vers %d; change stat %s-->%s>",
		    thismodule->modname,
		    thismodule->modversion,
		    IIDDscStatusDecode(thismodule->status),
		    IIDDscStatusDecode((i4) UNKNOWN));
	    }
	    thismodule->status = UNKNOWN;
	}
	if (thismodule->status == UNKNOWN)
	    mods[i++] = thismodule;
    }
    if (Print_trace)
	SIprintf("\n");
    for (j=0; j < i; j++)
    {
    	/* Set status for modules that don't have a status.
	** (Note: can't call modulestatus() inside above IIUGhsHtabScan() loop
	** cause it does Hash table functions and they can't be nested)
	*/
    	_VOID_ modulestatus(mods[j]);
    }
}
/*{
** Name: ssicSetStatusInsClients - Set status of installed clients.
**
** Description:
**	Determine which clients are installed and set their status in the
**	client table.
**
**	processing logic:
**
**	SELECT from ii_client_dep_mod.
**	    set CLIENTDESC.status=INSTALLED** for installed clients
**	End
**
**	**"INSTALLED" = all module info for the client in ii_client_dep_mod
**	also exists in ii_dict_modules.
**
**	Note: we don't need to select from ii_dict_modules because this
**	has already been done. MODULEDESC.status already tells the module
**	status.
**
** Inputs:
**	NONE
**
** Outputs:
**
**      Returns:
**		STATUS  -	OK:     client status set for all clients.
**				FAIL:   had problems.
** History:
**	31-jul-1991 (pete)
**		Initial version.
*/
static STATUS
ssicSetStatusInsClients()
{
    STATUS	stat = OK;
    bool	allmodsinstalled = TRUE;	/* Are all modules for the
						** client installed?
						*/
    char	prevcliname[FE_MAXNAME+1];
    i4		prevcliversion;
    MODULEDESC  *thismodule = NULL;
    CLIENTDESC  *thisclient = NULL;
    i4		modstat;
  i4 local_errno;
  char errtext[257];
  char cliname[FE_MAXNAME +1];
  i4 cliversion;
  char modname[FE_MAXNAME +1];
  i4 modversion;
    prevcliname[0] = EOS;
    prevcliversion = 0;
/* # line 1436 "dctinsup.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select client_name, client_version, module_name, module_version from \
ii_client_dep_mod order by client_name, client_version");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME +1-1,cliname);
      IIgetdomio((short *)0,1,30,4,&cliversion);
      IIgetdomio((short *)0,1,32,0,modname);
      IIgetdomio((short *)0,1,30,4,&modversion);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 1450 "dctinsup.sc" */	/* host code */
	if ((STcompare(prevcliname, cliname) == 0) &&
	    (prevcliversion == cliversion))
	    ;	/* same client, new module */
	else
	{
	    /* Different client or version (first one comes thru here too).
	    ** Set status of previous client if all its modules installed.
	    */
	    if ((allmodsinstalled) && (thisclient != (CLIENTDESC *)NULL))
		thisclient->status = INSTALLED;
	    allmodsinstalled = TRUE;
	    /* find new client in client table */
	    thisclient = IIUIfcFindClient(cliname, cliversion);
	}
	/* Check if this module installed. If not installed, leave
	** client status alone (defaults to NOTINSTALLED).
	*/
	if (((modstat = IIUIfmsFindModuleStatus(modname, modversion))
				!= INSTALLED)
	 && (modstat != UNKNOWN))
	    allmodsinstalled = FALSE;	/* Found a known module that isn't
					** installed. This will (later when
					** reach new client) cause this
					** client status to NOT
					** be set = INSTALLED.
					*/
	/* about to get new client. save old one */
	STcopy (cliname, prevcliname);
	prevcliversion = cliversion;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
    /* above loop ends before can proces last client. handle it here */
    if ((allmodsinstalled) && (thisclient != (CLIENTDESC *)NULL))
	thisclient->status = INSTALLED;
/* # line 1483 "dctinsup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&local_errno,2);
    IILQisInqSqlio((short *)0,1,32,256,errtext,63);
  }
/* # line 1484 "dctinsup.sc" */	/* host code */
    if (local_errno != 0)
    {
	IIUGerr(E_DD0019_DBMS_ERROR, UG_ERR_ERROR, 1, errtext);
	stat = FAIL;
    }
    return stat;
}
/*{
** Name: iccInsCompatClients - Install Compatible Clients in ii_client_dep_mod.
**
** Description:
**	Scan client table.
**	    For each not-installed client:
**		For each module in the client
**		    Is it upward compatible with an installed module?
**		End
**
**		If ALL modules upward compatible, then install client,
**		but use module vers. nmbr values from compatible installed
**		modules.
**	    End
**	End
**
** Inputs:
**
** Outputs:
**
**      Returns:
**		STATUS  -	OK:     clients installed (zero or more of them)
**				FAIL:   had problems.
** History:
**	31-jul-1991 (pete)
**		Initial version.
**	13-aug-1991 (pete)
**		Fix bug 39190; watch for improperly initialized clientdesc
**		entries (numsupmods > 0) but (supmods == NULL). Can happen
**		if only a single bad client name is specified. Winds up never
**		calling:  utils!ui!enter_client().
*/
static STATUS
iccInsCompatClients()
{
	STATUS		stat = OK;
	bool		scanflag;
	bool		installable;
	char		*dummy;
	i4		i;
	CLIENTDESC	*thisclient;
	MODULEDESC	*insmodule;	/* installed module */
	STATUS		installstatus;
	if (Print_trace)
	{
    SIprintf("\n  *********************************************************");
    SIprintf("\n  Check product/module status; install compatible products:");
    SIprintf("\n");
	}
	for (scanflag = FALSE;
	    IIUGhsHtabScan(clienttable, scanflag, &dummy, &thisclient) != 0;
	    scanflag = TRUE)
	{
	    /* check if client already installed (select from ii_client_dep_mod)*/
	    if ((installstatus = IIUIClientInstalled(thisclient->cliname,
	      thisclient->cliversion)) == FAIL)
		return (FAIL);
	    if (installstatus == UI_DESUPPORTED)
	    {
		/* Requested client is no longer supported.  An earlier module
		** upgrade must have produced a module that is not upwardly
		** compatible with a module that this client requires.
		** For example module FOO v1 was upgraded to FOO v2 and v2
		** clients who need v1 cannot run on v2.
		*/
		if (Print_trace)
		    SIprintf("\n    <product %s, vers %d is uninstallable>",
				thisclient->cliname,
				thisclient->cliversion);
		continue;
	    }
            if (thisclient->status == INSTALLED)
	    {
		if (Print_trace)
		    SIprintf("\n    <product %s, vers %d is installed>",
				thisclient->cliname,
				thisclient->cliversion);
        	continue;
	    }
	    installable = TRUE;
	    /* Check status of every module that this not-installed client
	    ** is dependent on:
	    */
	    if ((thisclient->numsupmods > 0) &&
	        (thisclient->supmods == (MODULEDESC **) NULL))
	    {
		/* this CLIENTDESC not initialized properly */
		installable = FALSE;
	    }
	    else
	    {
	        for (i = 0; i < thisclient->numsupmods; i++)
	        {
		    if ((thisclient->supmods[i]->status == INSTALLED) ||
		        (thisclient->supmods[i]->status == DESC_COMPAT))
		        ;
		    else
		        installable = FALSE;
	        }
	    }
	    /* If client is installable, start transaction and install
	    ** rows in ii_client_dep_mod.
	    */
	    if (installable)
	    {
		if (Print_trace)
		    SIprintf("\n    <installing product %s, vers %d . . .>",
				thisclient->cliname,
				thisclient->cliversion);
	    	stat = OK;
		IIDDbnxBeginNestedXact();
		IIDDnw1NestedWriteOn();
	    	for (i = 0; i < thisclient->numsupmods; i++)
	    	{
		    if (thisclient->supmods[i]->status == INSTALLED)
		    	insmodule = thisclient->supmods[i];
		    else     /* thisclient->supmods[i]->status == DESC_COMPAT */
		    	insmodule = thisclient->supmods[i]->installed_child;
		    /* insert row in ii_client_dep_mod */
		    if ((stat = enterclidep(thisclient, insmodule)) == OK)
		    {
			if (Print_trace)
	      SIprintf("\n        <INSERTing %s, vers %d in ii_client_dep_mod>",
				insmodule->modname,
				insmodule->modversion);
		    }
		    else
			break;
	    	}
		if (stat == OK)
		{
		    IIDDnw0NestedWriteOff();
		    IIDDenxEndNestedXact();
		}
		else
		{
		    IIDDnw0NestedWriteOff();
		    IIDDanxAbortNestedXact();
		    break;
		}
	    }
	    else
	    {
		if (Print_trace)
		{
		    SIprintf("\n    <product %s, vers %d not installable>",
				thisclient->cliname,
				thisclient->cliversion);
	    	    if ((thisclient->numsupmods > 0) &&
	        	(thisclient->supmods == (MODULEDESC **) NULL))
	    	    {
			/* this CLIENTDESC not initialized properly */
		 SIprintf("\n        <array of %s module info NOT INITIALIZED>",
			thisclient->cliname);
	    	    }
	    	    else
	    	    {
	    	        for (i = 0; i < thisclient->numsupmods; i++)
	    	        {
		            SIprintf("\n        <module %s, vers %d status %s>",
				thisclient->supmods[i]->modname,
				thisclient->supmods[i]->modversion,
			    IIDDscStatusDecode(thisclient->supmods[i]->status));
		        }
		    }
		}
	    }
	}
	if (Print_trace)
	    SIprintf("\n");
	return stat;
}
