/******************************************************************************
**
** Copyright (c) 1983, 2002 Ingres Corporation
**
******************************************************************************/

# include	<compat.h>
# include	<clconfig.h>
# include	<meprivate.h>
# include	<me.h>

/******************************************************************************
**
** External definitions
**
******************************************************************************/

GLOBALREF SIZE_TYPE  i_meactual;
GLOBALREF SIZE_TYPE  i_meuser;

/******************************************************************************
** Name:
** 	MEfree.c
**
** Function:
** 	MEfree
**
** Arguments:
** 	PTR	block;
**
** Result:
** 	Frees the block of memory pointed to by 'block'.
**
** 	Removes the block from the tag queue if appropriate.
**
** Returns:
**	STATUS: OK, ME_00_FREE, ME_FREE_FIRST
**
** Side Effects:
** 	None
**
** History:
**	21-mar-1996 (canor01)
**	    Free memory from calling process's heap.  Compact the
**	    heap after every several frees.
**	03-jun-1996 (canor01)
**	    Internally, store the tag as an i4 instead of an i2. This makes
**	    for more efficient code on byte-aligned platforms that do fixups.
**	08-aug-1999 (mcgem01)
**	    Changed longnat to i4.
**	08-feb-2001 (somsa01)
**	    Changed types of i_meactual and i_meuser  to be SIZE_TYPE.
**	21-jun-2002 (somsa01)
**	    Sync'ed up with UNIX. Rely on ME_NODE rather than a ptr UNION.
**	    Also, removed call to HeapCompact() logic.
**	05-Jul-2005 (drivi01)
**		Replaced HeapFree call with free.
**
******************************************************************************/
STATUS
MEfree(PTR block)
{
    register ME_NODE	*this;
    STATUS		rv = OK;

    if ( block == NULL )
	rv = ME_00_FREE;
    else 
    {
	this = (ME_NODE *)((char *)block - sizeof(ME_NODE));

	/*
	** assume block is legit if the node looks like it points to an
	** allocated block.
	*/
	if (this->MEaskedfor == 0)
	    rv = ME_NO_FREE;

	if (rv == OK)
	{
	    i_meactual -= this->MEsize;
	    i_meuser -= this->MEaskedfor;

	    if (this->MEtag)
		QUremove((QUEUE *)this);

	    free((char *)this);
	}
    }

    return(rv);
}
