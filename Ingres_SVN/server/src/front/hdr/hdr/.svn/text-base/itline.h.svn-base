/*
**	Copyright (c) 2004 Ingres Corporation
*/

/**
** Name:	ITline.h	- header file for drawing line functions
**
** Description:
**	This file contains the definitions for the drawing line
**
** History:
**	06-nov-1986 (yamamoto)
**		first written
**
**	15-nov-91 (leighb) DeskTop Porting Change:
**		Changed internal codes of the line drawing characters so
**		that they do not conflict with the section marker (^U) on
**		the IBM/PC.
**/

/*
**	internal character codes of the drawing line
*/

# define DRCH_LR	'\016'		/* lower-right corner of a box	*/	 
# define DRCH_UR	'\017'		/* upper-right corner of a box	*/	 
# define DRCH_UL	'\020'		/* upper-left corner of a box	*/	 
# define DRCH_LL	'\021'		/* lower-left corner of a box	*/	 
# define DRCH_X		'\022'		/* crossing lines		*/	 
# define DRCH_H		'\023'		/* horizontall line		*/	 
# define DRCH_LT	'\024'		/* left		'T'		*/	 
# define DRCH_SEC	'\025'		/* Section Mark Character	*/	 
# define DRCH_RT	'\026'		/* right	'T'		*/	 
# define DRCH_BT	'\027'		/* bottom	'T'		*/	 
# define DRCH_TT	'\030'		/* top		'T'		*/	 
# define DRCH_V         '\031'		/* vertical bar                 */	 


/*
**	declare functions that return characer pointer.
*/

FUNC_EXTERN char	*ITldopen();
FUNC_EXTERN char	*ITldclose();
