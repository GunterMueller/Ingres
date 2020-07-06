# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<st.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include 	<me.h>
# include	<ug.h>
# include <ui.h>
# include <uigdata.h>
# include	<erfe.h>
# include	"ertu.h"
# include	"tuconst.h"
# include "tutblinf.h"
#include        <lo.h>
#include        <uf.h>
typedef struct TU_DEFSTRUCT {
	i4	dkey;
	char	*dfltvalue;
	struct	TU_DEFSTRUCT	*next;
} TU_DLE;
static	bool	Tudefvalformready	= FALSE;
  static char *promptform = ERx("tudefval");
static	i4	Uniq_key;
static	struct	TU_DEFSTRUCT	*deflist = NULL;   /* the defaults list */
struct	TU_DEFSTRUCT		*defitem;   /* utility use */
/* prototypes */
char		*IITUgcdGetColumnDefault( i4  dkey );
bool		IITUedEditDefault( i4  *dkey );
VOID		IITUpdPutDefault( i4  dkey );
VOID		IITUcdlClearDefaultList();
ER_MSGID	IITUvunValueUserNull(char *response);
VOID		IITUdndDefaultNodeDelete( i4  dkey );
VOID		IITUdlcDefaultListCreate( char *table, char *owner );
i4		IITUgukGenUniqKey();
VOID		IITUrukResetUniqKey();
i4		IITUadAddDefault( char *default_value );
/**
** Name:	tudefault.qsc -	Create and Display User defined Defaults
**				for columns for the Tables Utility.
**
** Description:
**	Contains the modules for creating and displaying User defined Default
**	values for a column of a table in the Tables Utility.
**
**	This file defines:
**	IITUedEditDefault	- Edit the Default value for a column
**	IITUpdPutDefault	- Display the Default value for a column
**	IITUcdlClearDefaultList	- Free and reset allocated default value list
**	IITUvunValueUserNull	- Check for Defaultability string
**	IITUfdlFillDefaultList	- Fill default list from catalogs.
**	IITUdnaDefaultNodeAdd	- Add node
**	IITUdlDefaultLookup	- Find node
**	IITUgcdGetColumnDefault - Get default value from linked list
**	IITUdviDefaultValueInstall - Install a default value
**	IITUdndDefaultNodeDelete - Delete a node given key
**	IITUdlcDefaultListCreate - Create defaults list from DBMS info
**	IITUgukGenUniqKey	- Generate uniq key
**	IITUrukResetUniqKey	- Reset uniq key (start from 1)
**	IITUadAddDefault	- Add default node, from DBMS
**
** History:
**	23-sep-1992 (mgw)
**		First created.
**	8-apr-1993 (rogerl)
**		Converted to hidden column defaults list keying.  Prototyped
**		functions.  Wrote list manipulation utility routines.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	29-may-1997 (canor01)
**	    Cleaned up compiler warnings from CL prototypes.
**      02-Jul-1999 (hweho01)
**              Included uf.h header file, it contains the function     
**              prototype of IIUFlcfLocateForm(). Without the explicit
**              declaration, the default int return value for a  
**              function will truncate a 64-bit address on ris_u64 
**              platform.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*
** Name:	deffrmrdy - Make sure forms been added.
*/
static bool
deffrmrdy()
{
        if (!Tudefvalformready)
	{
            if ( IIUFgtfGetForm(IIUFlcfLocateForm(), promptform) != OK )
	    {
                IIUGerr (E_TU0041_Get_Form_Failed, UG_ERR_ERROR, 1,
				                         promptform);
                return FALSE;
            }
            Tudefvalformready = TRUE;
        }
	return TRUE;
}
/*
** Name:	gemem - get (<=maxui2) block of memory; error & exit if fail
**
** Description: consolidate memory allocation error checking for this file
**
** 26-mar-93 (rogerl)
*/
static PTR
gemem(
    i4  size
) {
PTR	blk;
STATUS	stat;
u_i2	maxui2 = (u_i2)(-1);  /* MAXUI2 */
	    /* requested block too big for MEreqmem, or PCs 64k limit */
    if ( size > (i4)maxui2 )
	IIUGerr(E_TU0033_tunmadd_bad_mem_alloc, UG_ERR_FATAL, (i4)0);
    blk = MEreqmem( (u_i4)0, (u_i4)size, FALSE, &stat );
    if ( stat != OK )
	IIUGerr(E_TU0033_tunmadd_bad_mem_alloc, UG_ERR_FATAL, (i4)0);
    return( blk );
}
/*{
** Name:	IITUdnaDefaultNodeAdd - add a blank node to defaults node list
**
** Side Effect:
**	Adds generic node to defaults list - 'deflist'
**
** Returns:
**	TU_DLE * node added
**
** History:
**	23-mar-1993 (rogerl)
*/
static TU_DLE *
IITUdnaDefaultNodeAdd()
{
	TU_DLE *newnode;
	newnode = (TU_DLE *)gemem( sizeof( TU_DLE ) );
	if ( deflist == NULL )
	{
	    deflist = newnode;
	    newnode->next = NULL;
	}
	else
	{
	    newnode->next = deflist;
	    deflist = newnode;
	}
	newnode->dfltvalue = NULL;
	return( newnode );
}
/*{
** Name:	IITUdlDefaultLookup - find node in def list keyed on col name
**
** Description:
**	Find node in defaults list with 'dkey'
**
** Inputs:
**	i4	key of column to find
** Returns:
**	TU_DLE * NULL if not found
**	TU_DLE *node if found
**
** History:
**	23-mar-1993 (rogerl)
*/
static TU_DLE *
IITUdlDefaultLookup(
    i4	dkey
) {
    struct TU_DEFSTRUCT *node = deflist;
    while ( node != NULL )
    {
	if ( dkey == node->dkey )
		return( node );
	node = node->next;
    }
    return( NULL );
}
/*{
** Name:	IITUgcdGetColumnDefault - Get the default value for 'key' col
**
** Description:
**	Given a def list key, return the default value
**
** Inputs:
**	i4	key into defaults list
**
** Returns:
**		NULL if no default value for current row, otherwise
**		pointer to the character string default value.
**
** History:
**	19-oct-1992 (mgw)
**		First created.
**	23-mar-1993 (rogerl)
**		Use utility routines; key to column name from row number
*/
char *
IITUgcdGetColumnDefault( i4  dkey )
{
	TU_DLE *node = IITUdlDefaultLookup( dkey );
	if ( node == NULL )
		return( NULL );
	else
		return( node->dfltvalue );
}
/*{
** Name:	IITUdviDefaultValueInstall - install default value in node
**
** Description:
**	Update or install default value into node as necessary.
**
** Inputs:
**	TU_DLE * node	- pt to node to install default value into
**	char * def_value - copy of default value to install
** Returns:
**	TU_DLE * to updated node
**
** History:
**	23-mar-1993 (rogerl)
*/
static TU_DLE *
IITUdviDefaultValueInstall(
    TU_DLE *node,
    char   *def_value
) {
    i4  size;
	if ( node->dfltvalue != NULL )
	    MEfree( (PTR)node->dfltvalue );
	size = STlength( def_value );
	node->dfltvalue = (char *)gemem( size + 1 );
	STcopy( def_value, node->dfltvalue );
	return( node );
}
/*{
** Name:	IITUedEditDefault - Edit default value for current column.
**
** Description:
**	Display defaults value popup - if key is > 0, find and display current
**	defaults value.  Allow user to either change the current value or
**	create one if one didn't exist.  If key ==0, create a key that caller
**	will install into hidden column 'default_lkey' on table field.
**
**	This routine is called by Create-a-Table.
**
** Inputs:
**	i4   *dkey	if dkey >0, use as key into defaults list.
**			if = 0, need new node in list.
**
** Outputs:
**	If dkey is 0 on input, and user types a valid default value, then
**	the key of a new node is written into this location.
**
** Returns:
**		TRUE if value input, FALSE otherwise
**
** Side Effects:
**		May create a node in the defaults list; may change defaults
**		value if node exists; may write key into input dkey location
**
** History:
**	23-sep-1992 (mgw)
**		First created.
**	7-apr-1993 (rogerl)
**		Rework for conversion to hidden column for defaults keying.
**		If a user leaves nothing but whitespace in defaults popup,
**		assume no default is wanted.
*/
bool
IITUedEditDefault(
    i4  *dkey
) {
  char *tmpptr;
	i4	deflen;
	bool	retval = FALSE;
  char defaultv[ DB_MAX_COLUMN_DEFAULT_LENGTH + 1 ];
	if (!deffrmrdy())	/* get default value popup form ready */
	{
		return retval;
	}
/* # line 327 "tudefault.qsc" */	/* display */
  {
    if (IIdispfrm(promptform,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 330 "tudefault.qsc" */	/* host code */
			/* if called with dkey == 0, need to install
			** a new list node, get a uniq key for the new node
			** and get that into dkey so caller can install it
			** into the hidden column. Otherwise, need to pull
			** the existing default value up for possible editing.
			*/
		if ( *dkey != 0 )
		{
		    defitem = IITUdlDefaultLookup( *dkey );
		    tmpptr = defitem->dfltvalue;
/* # line 340 "tudefault.qsc" */	/* putform */
      {
        if (IIfsetio((char *)0) != 0) {
          IIputfldio((char *)"defval",(short *)0,1,32,0,tmpptr);
        } /* IIfsetio */
      }
/* # line 341 "tudefault.qsc" */	/* host code */
		}
		else
		{
			defitem = NULL;
		}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 351 "tudefault.qsc" */	/* host code */
		*defaultv = EOS;
/* # line 353 "tudefault.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,DB_MAX_COLUMN_DEFAULT_LENGTH + 1-1,
              defaultv,(char *)"defval");
            } /* IIfsetio */
          }
/* # line 355 "tudefault.qsc" */	/* host code */
                if ( *defaultv == EOS )     /* if it's a null value */
		{
				/* user deleted the value */
			if ( *dkey != 0 )
			{
			    IITUdndDefaultNodeDelete( *dkey );
			    *dkey = 0;
			}
			retval = TRUE;
/* # line 364 "tudefault.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 365 "tudefault.qsc" */	/* host code */
		}
			/* defitem was set to correct default struct if
			** that exists (in initialize), or NULL if no exist
			*/
		if ( defitem == NULL )
		{
			    /* add a new node to local defaults list */
			defitem = IITUdnaDefaultNodeAdd();
			    /* gen unique key for new node; inform caller;
			    ** add the key to the node.  note that generation
			    ** of the key is delayed until there is really
			    ** a value to insert
			    */
			defitem->dkey = *dkey = IITUgukGenUniqKey();
		}
			    /* replace or add value */
		defitem = IITUdviDefaultValueInstall( defitem, defaultv );
		retval = TRUE;
/* # line 385 "tudefault.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 391 "tudefault.qsc" */	/* host code */
		retval = FALSE;
/* # line 392 "tudefault.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 398 "tudefault.qsc" */	/* host code */
                FEhelp(ERx("tudflprm.hlp"),
			ERget(F_TU0072_Help_Default_Prompt));
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    goto IIfdE1;
IImuI1:
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_OK),ERget(F_TU0059_ExplGetOk),2,2,1) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,2,2) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,3) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,3) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 403 "tudefault.qsc" */	/* host code */
	return retval;
}
/*{
** Name:	IITUpdPutDefault - Put up default value for 'key' column.
**
** Description:
**	Display a popup screen with default value for 'key' column
**
** Inputs:
**	i4	dkey
**
** History:
**	30-sep-1992 (mgw)
**		First created.
**	7-apr-1993 (rogerl)
**		Convert from database access to list access.
**    15-dec-2004 (wanfr01)
**	  Bug 113804, INGSRV3137
**        removed extra parameter on deffrmrdy call 
**        compile fails on HP itanium.
**	24-dec-2004 (shaha03)
**		SIR #113754 Removed unnecessary parameter while calling 
**	        deffrmrdy(),to avoid ansi-c compiler error.
*/
VOID
IITUpdPutDefault(
    i4	dkey
) {
  char *defv;
	if (!deffrmrdy())	/* get default value popup form ready */
	{
		return;
	}
		/* assumption is value exists */
	defv = IITUgcdGetColumnDefault( dkey );
/* # line 442 "tudefault.qsc" */	/* display */
  {
    if (IIdispfrm(promptform,(char *)"f") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    {
/* # line 445 "tudefault.qsc" */	/* set_frs */
      {
        if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIstfsio(21,(char *)0,0,(short *)0,1,32,0,(char *)"r");
        } /* IIiqset */
      }
/* # line 446 "tudefault.qsc" */	/* putform */
      {
        if (IIfsetio((char *)0) != 0) {
          IIputfldio((char *)"defval",(short *)0,1,32,0,defv);
        } /* IIfsetio */
      }
    }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 452 "tudefault.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else {
        if (1) goto IIfdE2;
      } /* IIretval */
    } /* IIrunform */
IIfdF2:
    if (IIchkfrm() == 0) goto IIfdB2;
    goto IIfdE2;
IImuI2:
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_End),ERget(F_TU000F_End_expl),2,1,1) == 0) goto 
    IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,1,1) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 454 "tudefault.qsc" */	/* host code */
	return;
}
/*{
** Name:	IITUcdlClearDefaultList - Reset the default value linked list
**
** Description:
**	This routine will free up all the memory allocated in the default
**	value linked list for the current table and reset the linked list
**	header, deflist, for the next table.
**
**	This routine is called by Examine and Create-a-Table.
**
** History:
**	04-oct-1992 (mgw)
**		First created.
**	24-mar-1993 (rogerl)
**		Recursively delete list; reset uniq key
*/
static VOID
destroy_list( TU_DLE *node )
{
	if ( node != NULL && node->next != NULL )
	    destroy_list( node->next );
	MEfree( (PTR)node->dfltvalue );
	MEfree( (PTR)node );
	return;
}
VOID
IITUcdlClearDefaultList()
{
	if ( deflist != NULL )
	{
	    destroy_list( deflist );
	    deflist = NULL;
	}
	IITUrukResetUniqKey();
	return;
}
/*{
** Name:    IITUvunValueUserNull() - Check for Defaultability string
**
** Description:
**	Check for user response of "value", "user", or "null" or
**	unique subsets of these responses. Note that "n" is taken
**	to be "no" so the shortest string taken to be "null" is "nu".
**	This routine is modeled after IIUGyn().
**
** Inputs:
**	response	{char *}  Buffer containing user response.
**
** Outputs:
**	none
**
** Returns:
**	ER_MSGID	ER_MSGID of the intended proper response or NULL
**			if response is neither "value", "user", nor "null".
**
** History:
**	17-oct-1992 (mgw)
**		Written.
**	9-apr-1993 (rogerl)
**		Delete if/else; only 'if' necessary.
*/
ER_MSGID
IITUvunValueUserNull(char *response)
{
    static READONLY ER_MSGID	values[] = { F_TU0073_Value1,
						F_TU0074_Value2,
						F_TU0075_Value3,
						F_TU0076_Value4,
						F_TU0077_Value5
				};
    static READONLY ER_MSGID	users[] = { F_TU0078_User1,
						F_TU0079_User2,
						F_TU007A_User3,
						F_TU007B_User4,
						F_TU0078_User1
				};
    static READONLY ER_MSGID	nulls[] = { F_TU007C_Null1,
						F_TU007D_Null2,
						F_TU007E_Null3,
						F_TU007C_Null1,
						F_TU007C_Null1
				};
    if (response != NULL && *response != EOS)
    { /* Response is given */
	char	testbuf[FE_PROMPTSIZE+1];
	STlcopy(response, testbuf, sizeof(testbuf)-1);
	STtrmwhite(testbuf);
	CVlower(testbuf);
	if (*testbuf == EOS)
	{ /* empty response */
	    return (ER_MSGID) NULL;
	}
	else
	{
	    register i4	i;
	    for (i = 0 ; i < (sizeof(values)/sizeof(values[0])) ; ++i)
	    {
		if (STequal(testbuf, ERget(values[i])))
		{
		    return (F_TU0074_Value2);
		}
		else if (STequal(testbuf, ERget(users[i])))
		{
		    return (F_TU0079_User2);
		}
		else if (STequal(testbuf, ERget(nulls[i])))
		{
		    return (F_TU007D_Null2);
		}
	    } /* end for */
	}
    }
    return (ER_MSGID) NULL;
}
/*{
** Name:	IITUdndDefaultNodeDelete - delete node identified by dkey
**
** Description:
**	Delete dkey key 'node'.  Deallocates memory.
**
** Inputs:
**	i4	dkey	key for node of intererst
** Returns:
**	TU_DLE * to list head
**	NULL if node not found
**
** History:
**	30-mar-1993 (rogerl)
*/
VOID
IITUdndDefaultNodeDelete(
    i4		dkey
) {
     TU_DLE *node;
     TU_DLE *preceding_node;
	node = IITUdlDefaultLookup( dkey );
	if ( node == NULL )
		return;		/* not found */
		/* find node just before the node of interest */
	preceding_node = deflist;
	if ( node != deflist )  /* if node to delete is not first node */
	{
				   /* cut 'node' out of list */
	    while ( preceding_node && preceding_node->next
	    				&& preceding_node->next != node )
		preceding_node = preceding_node->next;
		/* cut it out of list */
	    preceding_node->next = node->next;
	}
	else	/* it was the first node, ditch it */
	{
	    deflist = deflist->next;
	}
	MEfree( (PTR)node->dfltvalue );
	MEfree( (PTR)node );
	return;
}
/*{
** Name:	IITUdlcDefaultListCreate - create defaults list for owner/tab
**
** Description:
**	Query DBMS for defaults values for tab/owner, create deflist
**
** Inputs:
**	char * table
**	char * owner
**
** Returns:
**	VOID
**
** Side Effect:
**	creates defaults list, points global deflist at it; assume there's
**	nothing important attatched to 'deflist' or 'defitem' global pointers
**	(should destroy ((free)) deflist before calling this function)
**
** History:
**	31-mar-1993 (rogerl)
*/
VOID
IITUdlcDefaultListCreate(
    char *table,
    char *owner
) {
  char *t = table;
  char *o = owner;
  char defaultv[ DB_MAX_COLUMN_DEFAULT_LENGTH + 1 ];
  char column[ FE_MAXNAME + 1 ];
  i2 nulli;
/* # line 691 "tudefault.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select column_default_val, column_name from iicolumns where(table_nam\
e=");
    IIputdomio((short *)0,1,32,0,t);
    IIwritio(0,(short *)0,1,32,0,(char *)")and(table_owner=");
    IIputdomio((short *)0,1,32,0,o);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio(&nulli,1,32,DB_MAX_COLUMN_DEFAULT_LENGTH + 1-1,defaultv);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,column);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 697 "tudefault.sc" */	/* host code */
		defitem = IITUdnaDefaultNodeAdd();
		defitem->dkey = IITUgukGenUniqKey();
/* rogerl - make sure null values are handled correctly here */
		(VOID) IITUdviDefaultValueInstall( defitem, defaultv );
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	return;
}
/*{
** Name:	IITUgukGenUniqKey, IITUrukResetUniqKey
**
** Description:
**	Returns (beginning at 1), or resets integral values of static Uniq_key
**
** Returns:
**	IITUgukGenUniqKey - key generated
**	IITUrukResetUniqKey - nothing
**
** Side Effect:
**	Edits static global Uniq_key.  Keys used for correspondence between
**	nodes in list with hidden row on table field '_Def_lkey'.
**
** History:
**	31-mar-1993 (rogerl)
*/
i4
IITUgukGenUniqKey()
{
    Uniq_key++;
    return Uniq_key;
}
VOID
IITUrukResetUniqKey()
{
    Uniq_key = 0;
    return;
}
/*{
** Name:	IITUadAddDefault
**
** Description: creates a node, inserts default value in node, adds
**		node to list, returns the key of the node.
**
** Returns:	Key of node/default added
**
** History:
**	31-mar-1993 (rogerl)
**	19-jul-1993 (rogerl) pass in defaults value instead of query DBMS
*/
i4
IITUadAddDefault(
    char *default_value
) {
	    /* add a new node to local defaults list */
	defitem = IITUdnaDefaultNodeAdd();
	    /* add the key to the node */
	defitem->dkey = IITUgukGenUniqKey();
	    /* add the default value to the node */
	defitem = IITUdviDefaultValueInstall( defitem, default_value );
	return( defitem->dkey );
}
