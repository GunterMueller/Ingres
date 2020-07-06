/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <clconfig.h>
#include    <gl.h>
#include    <cs.h>
#include    <cm.h>
#include    <nm.h>
#include    <st.h>
#include    <lo.h>
#include    <me.h>
#include    <meprivate.h>
#include    <mu.h>
#include    <errno.h>

#ifdef	UNIX
#include    <ernames.h>
#endif

#ifdef	VMS
#include    <si.h>
#include    <cs.h>
#include    <lnmdef.h>
#include    <iodef.h>
#include    <fab.h>
#include    <rab.h>
#include    <ssdef.h>
#endif

#ifdef DESKTOP
#include    <er.h>
#endif /* DESKTOP */

/**
**
**  Name: CMFILE.C - Routines to read and write collation files
**
**  Description:
**	The file contains routines to read and write collation sequence
**	description file.  This seporate from DI because it is needed
**	by FE programs and it is not SI because that is not legal for
**	the DB server.
**
**		CMopen_col	- open collation file for reading
**		CMread_col	- read collation file
**		CMclose_col	- close collation file
**		CMdump_col	- create and write a collation file.
**
**  History:
**      16-Jun-89 (anton)
**	    Created.
**	31-may-90 (blaise)
**	    Added include <me.h> to pick up redefinition of MEfill().
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
**	16-apr-95 (emmag)
**	    Desktop porting changes.
**	16-apr-95 (emmag)
**	    8.3 file name restrictions require us to change collation
**	    to collate.
**	19-may-95 (canor01)
**	    added <errno.h>
**	06-jun-1995 (canor01)
**	    added VMS semaphore protection for Unix (MCT)
**      12-jun-1995 (canor01)
**          semaphore protect NMloc()'s static buffer (MCT)
**	03-jun-1996 (canor01)
**	    Remove NMloc() semaphore.
**	11-Nov-1999 (jenjo02)
**	    Use CL_CLEAR_ERR instead of SETCLERR to clear CL_ERR_DESC.
**	    The latter causes a wasted ___errno function call.
**      24-Feb-2000 (consi01) Bug 100051 INGSRV 1114
**          Modified CMclose_col to use fclose() for NT. The original
**          call to close() was failing causing exhaustion of the
**          available file descriptors.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      17-jan-2002 (stial01)
**          Synchronize ER_SEMFUNCS with the ER_SEMFUNCS in erloc.h
**	05-sep-2002 (hanch04)
**          For the 32/64 bit build,
**	    Add lp64 directory for 64-bit add on.
**	14-jan-2004 (gupsh01)
**	    Added CM_dumpUniMap function, to write and read data from
**	    $II_SYSTEM/ingres/files/unicharmaps, which contain mapping
**	    between unicode and local character sets.
**	16-jan-2004 (gupsh01)
**	    Fixed CMopen_col function.
**      18-feb-2004 (gupsh01)
**          Added CM_DEFAULTFILE_LOC which allows looking for default unicode 
**          coercion mapping file in $II_SYSTEM/ingres/files directory which 
**          is not yet copied to $II_SYSTEM/ingres/files/ucharmaps directory,
**          at install time.
**	20-Jul-2004 (lakvi01)
**		SIR #112703, cleaned-up warnings.
**      26-Jul-2004 (lakvi01)
**          Backed-out the above change to keep the open-source stable.
**          Will be revisited and submitted at a later date. 
**      07-Mar-2005 (hanje04)
**          SIR 114034
**          Add support for reverse hybrid builds. First part of much wider
**          change to allow support of 32bit clients on pure 64bit platforms.
**          Changes made here allow application statically linked against
**          the standard 32bit Intel Linux release to run against the new
**          reverse hybrid ports without relinking.
**      10-Oct-2006 (hanal04) Bug 116717
**          Move collation file statics into TLS if we are OS threaded.
**          Whilst the dcb_mutex may limit or even prevent concurrent
**          access for the same DB it can not prevent concurrent access
**          as multiple DBs are opened at the same time.
**/


typedef	struct _ER_SEMFUNCS
{
/* 
** WARNING !!!
** This structure must be kept in sync with the one in cl/clf/er/erloc.h
*/
    STATUS	(*er_p_semaphore)();
    STATUS	(*er_v_semaphore)();
    u_i2	    sem_type;
#define		    CS_SEM	0x01	/* CS semaphore routines */
#define		    MU_SEM	0x02	/* MU semaphore routines */

    CS_SEMAPHORE    er_sem;
    MU_SEMAPHORE    er_mu_sem;
} ER_SEMFUNCS;

typedef	struct _CMCOLDFLE
{
# ifdef	VMS
    struct	RAB	rab;
    struct	FAB	fab;
# endif
    ER_SEMFUNCS		*sems;
# if defined (UNIX) || defined (DESKTOP)
    int			desc;
# endif
#ifdef DESKTOP
    FILE        *collation_file;
    FILE        *charmap_file;
#endif /* DESKTOP */
} CMCOLDFLE;

static CMCOLDFLE file;

# ifdef OS_THREADS_USED
static ME_TLS_KEY       CMcolkey = 0;
# endif /* OS_THREADS_USED */


/*{
** Name: CMopen_col - open collation file for reading
**
** Description:
**	open collation file for reading
**
** Inputs:
**	colname				collation name
**
** Outputs:
**	syserr				OS specific error
**
**	Returns:
**	    E_DB_OK
**	    FAIL
**
**	Exceptions:
**	    none
**
** Side Effects:
**	May acquire a semaphore to protect itself from reentry and
**	hold that semaphore until the file is closed.
**	>>>>>CMclose_col must be called to release this semaphore<<<<<<
**
** History:
**      16-Jun-89 (anton)
**          Created.
**	26-mar-1996 (canor01)
**	    Windows NT: Convert the FILE pointer to a file descriptor for 
**	    subsequent Unix-compatible file operations.
**      05-sep-2002 (hanch04)
**          Add lp64 directory for 64-bit add on.
**	14-jan-2004 (gupsh01)
**	    Added CM_dumpUniMap function, to write and read data from
**	    $II_SYSTEM/ingres/files/unicharmaps, which contain mapping
**	    between unicode and local character sets.
**	02-feb-2004 (gupsh01) 
**	    Fixed DESKTOP case to correctly open the mapping files.
**      07-Mar-2005 (hanje04)
**          SIR 114034
**          Add support for reverse hybrid builds. First part of much wider
**          change to allow support of 32bit clients on pure 64bit platforms.
**          Changes made here allow application statically linked against
**          the standard 32bit Intel Linux release to run against the new
**          reverse hybrid ports without relinking.
*/
STATUS
CMopen_col(colname, syserr, loctype)
char		*colname;
CL_ERR_DESC	*syserr;
i4		loctype;
{
    LOCATION	loc;
    char	*chrp;
    STATUS	status;
    CMCOLDFLE   *cmcoldle;

# ifdef OS_THREADS_USED
    if ( CMcolkey == 0 )
    {
        ME_tls_createkey( &CMcolkey, &status );
        ME_tls_set( CMcolkey, NULL, &status );
    }
    if ( CMcolkey == 0 )
    {
        /* not linked with threading libraries */
        CMcolkey = -1;
    }
    if ( CMcolkey == -1 )
    {
        cmcoldle = &file;
    }
    else
    {
        ME_tls_get( CMcolkey, (PTR *)&cmcoldle, &status );
        if ( cmcoldle == NULL )
        {
            cmcoldle = (CMCOLDFLE *) MEreqmem( 0, sizeof(CMCOLDFLE),
                                               TRUE, NULL );
            ME_tls_set( CMcolkey, (PTR)cmcoldle, &status );
        }
    }
# else /* OS_THREADS_USED */
    cmcoldle = &file;
# endif /* OS_THREADS_USED */

# ifdef DESKTOP
    NMloc(FILES, PATH, (char *)NULL, &loc);
    if (loctype == CM_UCHARMAPS_LOC)
      LOfaddpath(&loc, "ucharmaps", &loc);
    else if (loctype == CM_COLLATION_LOC)
      LOfaddpath(&loc, "collation", &loc);
#ifdef ADD_ON64
    LOfaddpath(&loc, "lp64", &loc);
#endif /* ADD_ON64 */
#ifndef LP64
    {
	/*
	** Reverse hybrid support must be available in ALL
	** 32bit binaries
	*/
	char	*rhbsup;
	NMgtAt("II_LP32_ENABLED", &rhbsup);
	if ( (rhbsup && *rhbsup) &&
       ( !(STbcompare(rhbsup, 0, "ON", 0, TRUE)) ||
         !(STbcompare(rhbsup, 0, "TRUE", 0, TRUE))))
	    LOfaddpath(&loc, "lp32", &loc);
    }
#endif /* ! LP64 */
    LOfstfile(colname, &loc);
    LOtos(&loc, &chrp);

    if (loctype == CM_UCHARMAPS_LOC)
    {
      if ((cmcoldle->charmap_file = fopen(chrp, "rb")) == NULL)
      {
        SETCLERR(syserr, 0,0);
        return(FAIL);
      }
      cmcoldle->desc = fileno( cmcoldle->charmap_file);
      return OK;
    }
    else
    {
      if ((cmcoldle->collation_file = fopen(chrp, "rb")) == NULL)
      {
        SETCLERR(syserr, 0,0);
        return(FAIL);
      }
      cmcoldle->desc = fileno( cmcoldle->collation_file );
      return OK;

    }
# else /* DESKTOP */

    CL_CLEAR_ERR( syserr );

    if (cer_issem(&cmcoldle->sems))
    {
	(*cmcoldle->sems->er_p_semaphore)(TRUE, &cmcoldle->sems->er_sem);
    }

    NMloc(FILES, PATH, (char *)NULL, &loc);
    if (loctype == CM_UCHARMAPS_LOC)
      LOfaddpath(&loc, "ucharmaps", &loc);
    else if (loctype == CM_COLLATION_LOC)
      LOfaddpath(&loc, "collation", &loc);
#ifdef ADD_ON64
    LOfaddpath(&loc, "lp64", &loc);
#endif /* ADD_ON64 */
#ifndef LP64
    {
        /*
        ** Reverse hybrid support must be available in ALL
        ** 32bit binaries
        */
        char    *rhbsup;
        NMgtAt("II_LP32_ENABLED", &rhbsup);
        if ( (rhbsup && *rhbsup) &&
       ( !(STbcompare(rhbsup, 0, "ON", 0, TRUE)) ||
         !(STbcompare(rhbsup, 0, "TRUE", 0, TRUE))))
            LOfaddpath(&loc, "lp32", &loc);
    }
#endif /* ! LP64 */
    LOfstfile(colname, &loc);
    LOtos(&loc, &chrp);

# ifdef	UNIX
    if ((cmcoldle->desc = open(chrp, 0)) < 0)
    {
	SETCLERR(syserr, ER_open, 0);
	if (cmcoldle->sems)
	{
	    (*cmcoldle->sems->er_v_semaphore)(&cmcoldle->sems->er_sem);
	}
	return FAIL;
    }
# endif
# ifdef	VMS
    MEfill(sizeof cmcoldle->fab, '\0', (PTR)&cmcoldle->fab);
    MEfill(sizeof cmcoldle->rab, '\0', (PTR)&cmcoldle->rab);
    cmcoldle->fab.fab$b_bid = FAB$C_BID;
    cmcoldle->fab.fab$b_bln = FAB$C_BLN;
    cmcoldle->rab.rab$b_bid = RAB$C_BID;
    cmcoldle->rab.rab$b_bln = RAB$C_BLN;
    cmcoldle->rab.rab$l_fab = &cmcoldle->fab;
    cmcoldle->fab.fab$l_fna = chrp;
    cmcoldle->fab.fab$b_fns = STlength(chrp);
    cmcoldle->fab.fab$b_rfm = FAB$C_FIX;
    cmcoldle->fab.fab$b_fac = FAB$M_GET;
    cmcoldle->fab.fab$b_shr = FAB$M_SHRGET;
    syserr->errnum = status = sys$open(&cmcoldle->fab);
    if ((status & 1) == 0)
    {
	if (cmcoldle->sems)
	{
	    (*cmcoldle->sems->er_v_semaphore)(&cmcoldle->sems->er_sem);
	}
	return FAIL;
    }
    syserr->errnum = status = sys$connect(&cmcoldle->rab);
    if ((status & 1) == 0)
    {
	if (cmcoldle->sems)
	{
	    (*cmcoldle->sems->er_v_semaphore)(&cmcoldle->sems->er_sem);
	}
	return FAIL;
    }
    cmcoldle->rab.rab$w_usz = COL_BLOCK;
# endif
    return OK;
# endif /* DESKTOP */
}

/*{
** Name: CMread_col - read from collation file
**
** Description:
**	read a COL_BLOCK size record from open collation file
**
** Inputs:
**	bufp				pointer to buffer
**
** Outputs:
**	syserr				OS specific error
**
**	Returns:
**	    E_DB_OK
**	    FAIL
**
**	Exceptions:
**	    none
**
** Side Effects:
**	moves open file to next record
**
** History:
**      16-Jun-89 (anton)
**          Created.
*/
STATUS
CMread_col(bufp, syserr)
char		*bufp;
CL_ERR_DESC	*syserr;
{
    STATUS      status;
    CMCOLDFLE   *cmcoldle;

# ifdef OS_THREADS_USED
    if ( CMcolkey == -1 )
    {
        cmcoldle = &file;
    }
    else
    {
        ME_tls_get( CMcolkey, (PTR *)&cmcoldle, &status );
    }
# else /* OS_THREADS_USED */
    cmcoldle = &file;
# endif /* OS_THREADS_USED */

# if defined (UNIX) || defined (DESKTOP)
    if (read(cmcoldle->desc, bufp, COL_BLOCK) != COL_BLOCK)
    {
	SETCLERR(syserr, ER_read, 0);
	return FAIL;
    }
# endif
# ifdef	VMS
    cmcoldle->rab.rab$l_ubf = bufp;
    if (((syserr->errnum = sys$get(&cmcoldle->rab)) & 1) == 0)
    {
	return FAIL;
    }
# endif
    return OK;
}

/*{
** Name: CMclose_col - close collation file
**
** Description:
**	close an open collation file
**
** Inputs:
**	none
**
** Outputs:
**	syserr				OS specific error
**
**	Returns:
**	    E_DB_OK
**	    FAIL
**
**	Exceptions:
**	    none
**
** Side Effects:
**	Releases semaphore which may have been acquired durring CMopen_col
**
** History:
**      16-Jun-89 (anton)
**          Created.
**      24-Fep-2000 (consi01) Bug 100051 INGSRV 1114
**          Changed close() to fclose() if DESKTOP is defined as file
**          was opened with fopen(), not open()
**	14-Jan-2003 (gupsh01)
**	    Added the file location type in input parameter, it can now 
**	    be CM_COLLATION_LOC for $II_SYSTEM/ingres/files/collation or 
**	    CM_UCHARMAPS_LOC for $II_SYSTEM/ingres/filese/ucharmaps 
**	    for unicode and local encoding mapping files.
*/
STATUS
CMclose_col(syserr, loctype)
CL_ERR_DESC *syserr;
i4	loctype;
{
    STATUS      status;
    CMCOLDFLE   *cmcoldle;
# if defined (DESKTOP)
    FILE *f_temp;
# endif 

# ifdef OS_THREADS_USED
    if ( CMcolkey == -1 )
    {
        cmcoldle = &file;
    }
    else
    {
        ME_tls_get( CMcolkey, (PTR *)&cmcoldle, &status );
    }
# else /* OS_THREADS_USED */
    cmcoldle = &file;
# endif /* OS_THREADS_USED */


# if defined (DESKTOP)
    if (loctype == CM_UCHARMAPS_LOC)
      f_temp = cmcoldle->charmap_file;
    else
      f_temp = cmcoldle->collation_file;
# endif 

# ifdef	VMS
    syserr->errnum = sys$close(&cmcoldle->fab);
    if (cmcoldle->sems)
    {
	(*cmcoldle->sems->er_v_semaphore)(&cmcoldle->sems->er_sem);
    }
    if ((syserr->errnum & 1) == 0)
    {
	return FAIL;
    }
# else
# if defined (DESKTOP)
    if (fclose(f_temp))
# endif 
# if defined (UNIX) 
    if (close(cmcoldle->desc) < 0)
# endif
    {
	SETCLERR(syserr, ER_close, 0);
    	if (cmcoldle->sems)
    	{
		(*cmcoldle->sems->er_v_semaphore)(&cmcoldle->sems->er_sem);
    	}
	return FAIL;
    }
    if (cmcoldle->sems)
    {
	(*cmcoldle->sems->er_v_semaphore)(&cmcoldle->sems->er_sem);
    }
# endif

    return OK;
}

/*{
** Name: CMdump_col - open collation file for reading
**
** Description:
**	open collation file for reading
**
** Inputs:
**	colname				collation / target file name
**	tablep				collation table pointer
**	tablen				collation table length
**	loctype				type of location, collation 
**					or charmap	
**
** Outputs:
**	syserr				OS specific error
**
**	Returns:
**	    E_DB_OK
**	    FAIL
**
**	Exceptions:
**	    none
**
** Side Effects:
**	none
**
** History:
**      16-Jun-89 (anton)
**          Created.
**	14-Jan-2003 (gupsh01)
**	    Added the file location type in input parameter, it can now 
**	    be CM_COLLATION_LOC for $II_SYSTEM/ingres/files/collation or 
**	    CM_UCHARMAPS_LOC for $II_SYSTEM/ingres/filese/ucharmaps 
**	    for unicode and local encoding mapping files.
**      07-Mar-2005 (hanje04)
**          SIR 114034
**          Add support for reverse hybrid builds. First part of much wider
**          change to allow support of 32bit clients on pure 64bit platforms.
**          Changes made here allow application statically linked against
**          the standard 32bit Intel Linux release to run against the new
**          reverse hybrid ports without relinking.
*/
STATUS
CMdump_col(colname, tablep, tablen, syserr, loctype)
char		*colname;
PTR		tablep;
i4		tablen;
CL_ERR_DESC	*syserr;
i4		loctype;
{
    LOCATION	loc;
    char	*tp, *chrp;
    STATUS	status;
# ifdef DESKTOP
    FILE        *fp;
# endif
    CMCOLDFLE   *cmcoldle;

# ifdef OS_THREADS_USED
    if ( CMcolkey == 0 )
    {
        ME_tls_createkey( &CMcolkey, &status );
        ME_tls_set( CMcolkey, NULL, &status );
    }
    if ( CMcolkey == 0 )
    {
        /* not linked with threading libraries */
        CMcolkey = -1;
    }
    if ( CMcolkey == -1 )
    {
        cmcoldle = &file;
    }
    else
    {
        ME_tls_get( CMcolkey, (PTR *)&cmcoldle, &status );
        if ( cmcoldle == NULL )
        {
            cmcoldle = (CMCOLDFLE *) MEreqmem( 0, sizeof(CMCOLDFLE),
                                               TRUE, NULL );
            ME_tls_set( CMcolkey, (PTR)cmcoldle, &status );
        }
    }
# else /* OS_THREADS_USED */
    cmcoldle = &file;
# endif /* OS_THREADS_USED */

    NMloc(FILES, PATH, (char *)NULL, &loc);
    if (loctype == CM_UCHARMAPS_LOC)
      LOfaddpath(&loc, "ucharmaps", &loc);
    else if (loctype == CM_COLLATION_LOC)
      LOfaddpath(&loc, "collation", &loc);

#ifdef ADD_ON64
    LOfaddpath(&loc, "lp64", &loc);
#endif /* ADD_ON64 */
#ifndef LP64
    {
        /*
        ** Reverse hybrid support must be available in ALL
        ** 32bit binaries
        */
        char    *rhbsup;
        NMgtAt("II_LP32_ENABLED", &rhbsup);
        if ( (rhbsup && *rhbsup) &&
       ( !(STbcompare(rhbsup, 0, "ON", 0, TRUE)) ||
         !(STbcompare(rhbsup, 0, "TRUE", 0, TRUE))))
            LOfaddpath(&loc, "lp32", &loc);
    }
#endif /* ! LP64 */
    LOfstfile(colname, &loc);
    LOtos(&loc, &chrp);
# ifdef	VMS
    MEfill(sizeof cmcoldle->fab, '\0', (PTR)&cmcoldle->fab);
    MEfill(sizeof cmcoldle->rab, '\0', (PTR)&cmcoldle->rab);
    cmcoldle->fab.fab$b_bid = FAB$C_BID;
    cmcoldle->fab.fab$b_bln = FAB$C_BLN;
    cmcoldle->fab.fab$b_fac = FAB$M_PUT|FAB$M_GET;
    cmcoldle->fab.fab$l_fna = chrp;
    cmcoldle->fab.fab$b_fns = STlength(chrp);
    cmcoldle->fab.fab$b_rfm = FAB$C_FIX;
    cmcoldle->fab.fab$w_mrs = COL_BLOCK;
    status = syserr->errnum = sys$create(&cmcoldle->fab);
    if ((status & 1) == 0)
	return FAIL;
    cmcoldle->rab.rab$b_bid = RAB$C_BID;
    cmcoldle->rab.rab$b_bln = RAB$C_BLN;
    cmcoldle->rab.rab$l_fab = &cmcoldle->fab;
    status = syserr->errnum = sys$connect(&cmcoldle->rab);
    if ((status & 1) == 0)
	return FAIL;
    tp = (char *)tablep;
    while (tablen > 0)
    {
	cmcoldle->rab.rab$w_rsz = COL_BLOCK;
	cmcoldle->rab.rab$l_rbf = tp;
	status = syserr->errnum = sys$put(&cmcoldle->rab);
	if ((status & 1) == 0)
	    return FAIL;
	tp += COL_BLOCK;
	tablen -= COL_BLOCK;
    }
    _VOID_ sys$disconnect(&cmcoldle->rab);
    status = syserr->errnum = sys$close(&cmcoldle->fab);
    if ((status & 1) == 0)
	return FAIL;
    return OK;
# else 
# ifdef DESKTOP
    if ((fp = fopen(chrp, "wb")) == NULL)
    {
        SETCLERR(syserr, 0,0);
        return(FAIL);
    }

    if ((i4) fwrite((char *) tablep, 1, (size_t) tablen, fp) != tablen)
    {
        SETCLERR(syserr, 0,0);
        return(FAIL);
    }

    if (fclose(fp) != 0)
    {
        SETCLERR(syserr, 0,0);
        return(FAIL);
    }
    return(OK);
# else /* !DESKTOP */
    cmcoldle->desc = creat(chrp, 0777);
    if (cmcoldle->desc == -1)
    {
	SETCLERR(syserr, ER_open, 1);
	perror(chrp);
	return FAIL;
    }
    write(cmcoldle->desc, (char *)tablep, tablen);
    close(cmcoldle->desc);
    return OK;
# endif /* DESKTOP */
# endif
}
