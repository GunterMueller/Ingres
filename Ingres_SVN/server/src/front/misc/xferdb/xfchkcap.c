# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<me.h>		/* 6-x_PC_80x86 */
# include	<st.h>
# include	<si.h>
# include	<lo.h>
# include	<er.h>
# include	<fe.h>
# include	<ui.h>
# include	<ug.h>
# include	<adf.h>
# include	<afe.h>
# include	<uigdata.h>
# include <xf.h>
# include	"erxf.h"
/*
** Fool MING, which doesn't know about EXEC SQL INCLUDE
# include <xf.qsh>
*/
/**
** Name:	xfckcap.sc - Check the dbcapabilities.
**
** Description:
**	This file defines:
**
**	xfcapset	Find and cache all capability information.
**
** History:
**	21-apr-89 (marian) written.
**	26-apr-89 (marian)
**		Check the INGRES/QUEL_LEVEL instead of INGRES.
**	09-apr-90 (marian)
**		Integrate porting change (piccolo # 130926).
**		Fixed programming error - 'with_rules' was being set to value
**		returned by IIUIdcl_logkeys() and 'with_log_key' was being set
**		to value returned by IIUIdcu_rules().
**	14-may-91 (billc)
**		Fix 37530, 37531, 37532 - STAR stuff.
**      27-jul-1992 (billc)
**              Rename from .qsc to .sc suffix.
**	17-nov-93 (robf)
**              Add With_hidden_columns for secure,
**	        With_security_alarm
**	17-dec-93 (robf)
**              Rework With_hidden_columns to With_physical_columns
**	        for secure
**	03-feb-95 (wonst02) Bug #63766
**		Look for both $ingres and $INGRES, for fips
**	 8-dec-1995 (nick)
**		Convert catalog to uppercase in a FIPS installation. #73053
**	06-mar-96 (harpa06)
**		Changed UI_LEVEL_66 to UI_LEVEL_66 for VPS project.
**	29-jan-1997 (hanch04)
**		Changed UI_LEVEL_66 to UI_LEVEL_65.
**	27-July-1998 Alan.J.Thackray (thaal01)
**		Bug 87810.Unloaddb does not unload procedures when
**		certain collation sequences are used. Events, and
**		comments are also not unloaded. Convert '$ingres' to 
**		uppercase for FIPS, lower case for traditional Ingres.
**		Remove the OR statement which will result in incorrect
**		optimization with certain collation sequences are used.
**		The optimizer does not seem to know that the system tables
**		do not inherit the custom collation sequence. This is 
**		a cross integration of 433817 from oping12.
**	7-Jun-2004 (schka24)
**	    Check for release 3.
**/
/* # define's */
/* GLOBALREF's */
GLOBALREF bool With_distrib;
GLOBALREF bool With_log_key;
GLOBALREF bool With_multi_locations;
GLOBALREF bool With_comments ;
GLOBALREF bool With_UDT ;
GLOBALREF bool With_registrations ;
GLOBALREF bool With_sequences;
GLOBALREF bool With_dbevents ;
GLOBALREF bool With_procedures ;
GLOBALREF bool With_rules ;
GLOBALREF bool With_synonyms ;
GLOBALREF bool With_defaults ;
GLOBALREF bool With_sec_audit;
GLOBALREF bool With_physical_columns;
GLOBALREF bool With_security_alarms ;
GLOBALREF bool With_64_catalogs ;
GLOBALREF bool With_65_catalogs ;
GLOBALREF bool With_20_catalogs ;
GLOBALREF bool With_r3_catalogs ;
GLOBALREF bool With_r302_catalogs ;
/* extern's */
/* These should be declared in ui.h! */
FUNC_EXTERN char	*IIUIscl_StdCatLevel(); 
FUNC_EXTERN bool	IIUIdcd_dist();
FUNC_EXTERN bool	IIUIdcl_logkeys();
FUNC_EXTERN bool	IIUIdcu_rules();
FUNC_EXTERN bool	IIUIdcy_udt();
/* static's */
static bool cat_exist();
/**
** Name:	xfcapset - Load info from the dbcapabilities.
**
** Description:
**	Get information about the characteristics of this database.
**	Yes, the IIUI stuff caches, making this unneccessary.  But it
**	is much more readable this way.  (Could also have used #defines.)
**
** Inputs:
**	none.
**
** Outputs:
**	none.
**
**	Returns:
**		none
**
** History:
**	27-may-89 (marian)
**		Written.
**	06-mar-96 (harpa06)
**		Changed UI_LEVEL_65 to UI_LEVEL_66.
**	29-jan-1997 (hanch04)
**		Changed UI_LEVEL_66 to UI_LEVEL_65.
**	22-sep-1997 (nanpr01)
**		bug 85565 : unloaddb of 1.2 database over the net fails.
**	29-jan-03 (inkdo01)
**	    Add support for iisequences.
**	13-oct-05 (inkdo01)
**	    Add With_r302_catalogs.
**/
void
xfcapset()
{
    With_distrib = IIUIdcd_dist();
    With_log_key = IIUIdcl_logkeys();
    With_UDT = IIUIdcy_udt();
    With_rules = IIUIdcu_rules();
    With_multi_locations = cat_exist(ERx("iimulti_locations"));
    With_comments = cat_exist(ERx("iidb_comments"));
    With_registrations = cat_exist(ERx("iiregistrations"));
    With_sequences = cat_exist(ERx("iisequences"));
    With_dbevents = cat_exist(ERx("iievents"));
    With_procedures = cat_exist(ERx("iiprocedures"));
    With_physical_columns = cat_exist(ERx("iiphysical_columns"));
    With_security_alarms = cat_exist(ERx("iisecurity_alarms"));
    if (STcompare(IIUIscl_StdCatLevel(), UI_LEVEL_64) >= 0)
    {
	With_64_catalogs = TRUE;
    }
    if (STcompare(IIUIscl_StdCatLevel(), UI_LEVEL_65) >= 0)
    {
	With_defaults = TRUE;
	With_synonyms = TRUE;
	With_65_catalogs = TRUE;
	With_sec_audit=TRUE;
    }
    if (STcompare(IIUIscl_StdCatLevel(), UI_LEVEL_800) >= 0)
    {
	With_20_catalogs = TRUE;
    }
    if (STcompare(IIUIscl_StdCatLevel(), UI_LEVEL_900) >= 0)
    {
	With_r3_catalogs = TRUE;
    }
    if (STcompare(IIUIscl_StdCatLevel(), UI_LEVEL_902) >= 0)
    {
	With_r302_catalogs = TRUE;
    }
}
/*{
** Name:	cat_exist - Called to see if a table exists.
**
** Description:
**	This routine looks to see if the given table exists.
**	xferdb uses this to check for the existence of catalogs.
**	This routine used to cache the answers, but no-one does multiple
**	calls anymore.
**
** Inputs:
**	name		name of table.
**
** Outputs:
**	none.
**
**	Returns:
**		TRUE	exists
**		FALSE	nope
**
** History:
**	3-may-1990 (billc) written.
**
**	 8-dec-1995 (nick)
**	    Convert to uppercase in a FIPS installation. Note that we don't
**	    handle delimited ids or convert to lowercase as this function 
**	    is currently only ever called from xfcapset() above. #73053
**	27-July-1998 (thaal01) Bug 87810
**	    unloaddb does not work with certain collation sequences on 
**	    a non-SQL92 system
*/
static bool
cat_exist(name)
  char *name;
{
  char tname[FE_MAXNAME + 1];
  char uname[FE_MAXNAME + 1];
    tname[0] = EOS;
    STcopy("$ingres", uname );
    if (IIUIdbdata()->name_Case == UI_UPPERCASE)
	{
	CVupper(name);
	CVupper(uname);
	}
/* # line 239 "xfchkcap.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"xfchkcap1",7157,15021);
      IIputdomio((short *)0,1,32,0,name);
      IIputdomio((short *)0,1,32,0,uname);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"xfchkcap1",7157,15021);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name from iitables where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uname);
        IIexDefine(0,(char *)"xfchkcap1",7157,15021);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,tname);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 245 "xfchkcap.sc" */	/* host code */
    return ((bool) (tname[0] != EOS));
}
