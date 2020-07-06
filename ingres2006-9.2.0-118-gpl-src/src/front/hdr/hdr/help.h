/*
**	Copyright (c) 2004 Ingres Corporation
*/

/**
** Name:	help.h	-	Defines for FEhelp
**
** Description:
**
** History:	$Log - for RCS$
**	<automatically entered by code control>
**/

# define	H_EQ	0	/*  EQUEL program */
# define	H_VFRBF	1	/*  Vifred/RBF */
# define	H_IQUEL	2	/*  ISQL/IQUEL */
# define	H_VIG	3	/*  VIGRAPH layout (graphics on screen ) */
# define	H_QBFSRT 4	/*  QBF Sort form (suppresses field help) */
# define	H_GREQ	 5	/*  EQUEL with display-only graphics */
# define	H_FE	 6	/*  Standard ingres written front-end */

/*
**	External declarations.
*/

FUNC_EXTERN	bool	FEfehelp();	/* Returns TRUE if no redraw needed */
