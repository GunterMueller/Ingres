/*
**Copyright (c) 2004 Ingres Corporation
*/

/*
**  Name: uleint.h - Internal typedefs and defines needed by ULE.
**
**  Description:
**	This file contains definitions of all of the structures and
**	symbolic constants needed by the ULE facility.
**
**	This file defines the following typedefs:
**	    ULE_MHDR - Message header in the log
**
**  History:
**	17-oct-2000 (somsa01)
**	    Created.
*/


/*}
**  Name: ULE_MHDR - Message header in the log
**
**  Description:
**	This structure is a set of character arrays to build
**	the header with.
**
**  History:
**	03-aug-1987 (fred)
**	    Created.
**	17-oct-2000 (somsa01)
**	    Moved here from uleformat.c .
**      26-Apr-2007 (hanal04) SIR 118196
**          Add process pid to ULE header.
*/
typedef struct _ULE_MHDR
{
    char	ule_node[8];	/* node name (where applicable) */
    char	ule_pad1[3];	/* ::[ */
    char	ule_server[18];	/* server name */
    char	ule_pad2[2];	/* ', ' */
    char        ule_pid[10];    /* server pid */
    char        ule_pad3[3];    /* ', ' */
    char	ule_session[sizeof(SCF_SESSION)*2];	/* session id */
    char	ule_pad4[3];	/* ']: ' */
}   ULE_MHDR;


/*
**  Defines of other constants.
*/

#define	ULE_MSG_HDR_SIZE	sizeof(ULE_MHDR)

