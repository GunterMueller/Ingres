/*
** Copyright (c) 2001, Ingres Corporation
*/

/**CL_SPEC
** Name: CXCL.H - CX compatibility library definitions.
**
** Description:
**      This header contains those declarations which may vary
**	from platform to platform, but which need to be referenced
**	by cx.h which defines the public interface to CX.
**
** Specification:
**  
**  Description:
**      The CX ('C'luster e'X'tension) sub-facility provides an OS 
**	independent interface for services needed to support Ingres 
**	clusters.   
**
**  Intended Uses:
**	The CX routines are mainly used by the LG & LK facilities
**	in direct support of Ingres clusters.
**      
**  Assumptions:
**	Caller should not assume that this facility will be fully functional 
**	on all platforms.  In those environments in which clustering is not 
**	supported or configured, many CX routines return a status of
**	E_CL2C10_CX_E_NOSUPPORT, and calling programs must allow for this.
**
**  Definitions:
**
**  Concepts:
**	
**	Please see design Spec for SIR 103715.
**
** History:
**      06-feb-2001 (devjo01)
**          Created.
**	29-may-2002 (devjo01)
**	    Add "extended" (long) message functions.
**	12-sep-2002 (devjo01)
**	    Add CX_MSG_CHIT_MASK.
**	23-oct-2003 (kinte01)
**	    Move include of clconfig.h to axp_osf specific area as it is 
**	    only required for tru64
**	08-jan-2004 (devjo01)
**	    Most of the contents of this file split off into new header
**	    !cl!clf!hdr cxprivate.h, which now holds all declarations private
**	    to the CX facility.  Residual contents of this header are
**	    limited to those declarations which are platform specific,
**	    and need to be referenced by the public interface to CX (cx.h).
**	01-Apr-2004 (devjo01)
**	    Move CX_MAX_MSG_STOW, CX_MSG_FRAGSIZE to cxprivate.h
**/


#ifndef	CXCL_H_INCLUDED
#define CXCL_H_INCLUDED

/*
**  Symbolic constants
*/

/* CMO implementation "styles" */
#define	CX_CMO_NONE   		0	/* None or uninitialized.   */
#define	CX_CMO_DLM		1	/* Use DLM value blks (all) */
#define	CX_CMO_IMC		2	/* Use memory channel (axp) */

#define CX_MSG_MAX_CHIT	      256	/* High range, and # of, valid chits */
#define CX_MSG_CHIT_MASK      0x1FF	/* Used to mask out base chit id */

#define CX_ALIGN_BOUNDRY	sizeof(PTR) /* Alignment requirements */

/*
** Implementation dependent declares for CX <-> DLM
*/

/*
**  Lock value block size used by underlying DLM
**
**  This must be equal to or greater than underlying DLM value block size.
**  Ingres itself will only use 8 bytes for data, plus 4 for keeping the
**  valid/invalid status from Ingres'es perspective.  This is less
**  than the minimun value block size for supported DLM's (16).
*/
#if defined(axp_osf)
#	define CX_DLM_VALUE_SIZE       32
#else
#	define CX_DLM_VALUE_SIZE       16
#endif

typedef u_i4    CX_VALUE[CX_DLM_VALUE_SIZE/sizeof(u_i4)];

/*
**  "Private" OS specific implementation portion of CX_REQ_CB.
*/
typedef struct _CX_DLM_CB CX_DLM_CB;

#if defined(VMS)	/* All flavors of VMS */

typedef i4	 CX_LOCK_ID;

#define CX_ID_EQUAL(a,b) \
	( ((*(i4 *)(a)) == (*(i4 *)(b)) )
#define CX_NONZERO_ID(p) \
	( 0 != *(i4 *)(p) )
#define CX_ZERO_OUT_ID(p) \
	( *(i4 *)(p) = 0 )
#define CX_EXTRACT_LOCK_ID(d,s) \
	( ((LK_UNIQUE*)(d))->lk_uhigh = 0, \
          ((LK_UNIQUE*)(d))->lk_ulow = *(i4 *)(s) )

/* Layout of following must match LKSB passed to SYS$ENQ */
struct _CX_DLM_CB
{
	CX_LOCK_ID	cx_dlm_lock_id;
	i4		cx_vms_status;
	CX_VALUE	cx_dlm_valblk;
};

#elif defined(LNX)	/* All flavors of Linux */

typedef int	 CX_LOCK_ID;

#define CX_ID_EQUAL(a,b) \
	( ((*(int *)(a)) == (*(int *)(b)) )
#define CX_NONZERO_ID(p) \
	( 0 != *(int *)(p) )
#define CX_ZERO_OUT_ID(p) \
	( *(int *)(p) = 0 )
#define CX_EXTRACT_LOCK_ID(d,s) \
	( ((LK_UNIQUE*)(d))->lk_uhigh = 0, \
          ((LK_UNIQUE*)(d))->lk_ulow = *(int *)(s) )

/* Layout of following must match LKSB passed to OpenDLM */
struct _CX_DLM_CB
{
	int		cx_dlm_status;
	CX_LOCK_ID	cx_dlm_lock_id;
	CX_VALUE	cx_dlm_valblk;
	unsigned int    cx_dlm_timeout;
};

#else	/* Generic implementation */

typedef LK_UNIQUE CX_LOCK_ID;

#define CX_ID_EQUAL(a,b) \
	( (((LK_UNIQUE*)(a))->lk_ulow == ((LK_UNIQUE*)(b))->lk_ulow) && \
	(((LK_UNIQUE*)(a))->lk_uhigh == ((LK_UNIQUE*)(b))->lk_uhigh) )
#define CX_NONZERO_ID(p) \
	( ((LK_UNIQUE*)(p))->lk_uhigh || ((LK_UNIQUE*)(p))->lk_ulow )
#define CX_ZERO_OUT_ID(p) \
	( ((LK_UNIQUE*)(p))->lk_uhigh = ((LK_UNIQUE*)(p))->lk_ulow = 0 )
#define CX_EXTRACT_LOCK_ID(d,s) \
	( *(LK_UNIQUE*)(d) = *(LK_UNIQUE*)(s) )

struct _CX_DLM_CB
{
	CX_LOCK_ID	cx_dlm_lock_id;
	CX_VALUE	cx_dlm_valblk;
};

#endif

#endif /* CXCL_H_INCLUDED */
