/*
**	Copyright (c) 2004 Ingres Corporation
*/

/**
** Name:	ITdrloc.h	- header file for drawing char tbl
**
** Description:
**	This file declares the drawing character table
**	and the external variables of default drawing
**	characters.
**
** History:
**	30-jan-1987 (yamamoto)
**		first written
**	04/14/90 (dkh) -  Removed extern of lseflg.
**	24-sep-96 (mcgem01)
**	    externs changed to GLOBALREF
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/

/*
**	definition of the drawing character table.
*/

typedef struct
{
	u_char	*drchar;	/* pointer to the drawing characters */
	i4	drlen;		/* length of the string 'drchar' */
	u_char	*prchar;	/* pointer to the characters that
				** are consisted by ASCII char
				*/
	i4	prlen;		/* length of the string 'prchar' */
} DRLINECHAR;

GLOBALREF DRLINECHAR	drchtbl[];


/*
**	index of drchtbl[]
*/

# define DRLD	0	/* init terminal to drawing lines		*/
# define DRLE	1	/* interpret subseq. chars as regular chars	*/
# define DRLS	2	/* interpret subseq. chars for drawing chars	*/
# define DRQA	3	/* lower-right corner of a box	*/
# define DRQB	4	/* upper-right corner of a box	*/
# define DRQC	5	/* upper-left corner of a box	*/
# define DRQD	6	/* lower-left corner of a box	*/
# define DRQE	7	/* corssing lines		*/
# define DRQF	8	/* horizontal line		*/
# define DRQG	9	/* left		'T'		*/
# define DRQH	10	/* right	'T'		*/
# define DRQI	11	/* bottom	'T'		*/
# define DRQJ	12	/* top		'T'		*/
# define DRQK	13	/* vertical bar			*/

# define NDRINX	14	/* number of DR index	*/


/*
**	declaration for character pointer that hold the 
**	default value of drchtbl[].drchar and drchtbl[].prchar
*/

GLOBALREF char	*CHLD;
GLOBALREF char	*CHLE;
GLOBALREF char	*CHLS;
GLOBALREF char	*CHQA;
GLOBALREF char	*CHQB;
GLOBALREF char	*CHQC;
GLOBALREF char	*CHQD;
GLOBALREF char	*CHQE;
GLOBALREF char	*CHQF;
GLOBALREF char	*CHQG;
GLOBALREF char	*CHQH;
GLOBALREF char	*CHQI;
GLOBALREF char	*CHQJ;
GLOBALREF char	*CHQK;

/*
**	flags used by ITlndraw()
*/

GLOBALREF i4	dblflg;
GLOBALREF i4	dbpflg;
