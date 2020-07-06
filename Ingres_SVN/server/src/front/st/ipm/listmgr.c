# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
/*
** includes and defines
*/
#include <compat.h>
#include <cs.h>
#include <dbms.h>
#include <fe.h>
#include <pc.h>
#include <lg.h>
#include <lk.h>
#include <me.h>
#include <er.h>
#include <st.h>
#include "lists.h"
#include "dba.h"
/*
**  File: listmgr.qc
**
**  This file contains the routines necessary to manipulate the linked
**  lists needed by LOCKINFO
**
**  This file contains:
**	l_add() - given a list name, a sublist and a ptr to the info,
**		this routine will add the info to the specified linked
**		list.  All calls to l_add() must be complete for a particular
**		sublist or list before calling l_retrieve.  This is because
**		list_cb[].current is used in the l_add to remember the
**		previous element's address.
**	l_free() - given a list name and a sublist, this routine will either
**		return the memory to the OS or put it in a free list.  If
**		a only a list name is provided, the entire list and sublists
**		will be freed.  The only memory which will be returned are
**		the locklist detail (includes lock list info and individual
**		lock info) and the resource list detail (includes resource list
**		info and individual lock info).  This is because those lists
**		are allocated in their entirety -- access from one lock to the
**		next is not by linked list, but by incrementing a pointer.
**	l_retrieve() - given a list name and a sublist, this routine will
**		return a pointer to the info requested by the caller.
**		Successive calls return a pointer to the next piece of
**		data.  If no sublist is provided, the next item in the
**		master list is provided.  When the last piece of info
**		has been returned, the next call to l_retrieve will return
**		a null pointer.  The next call after a null is returned will
**		return the first item for the list and/or sublist requested.
**		l_retrieve should only be used after the list(s) are completely
**		built.
**	l_num_elements() - given a list name, this function returns the
**		number of elements in the specified linked list.
**		This is used by routines which must retrieve all sublists
**		for a given list.
**	l_ressort() - this routine sorts the resource list to minimize the
**		number of times databases must be opened to get tablenames.
**		This routine should only be called after the list is fully
**		built.
**	l_resswap() - called by l_ressort to swap elements
**
**  Here is a diagram of the list structure:
**
**     listcb[0]             listcb[1]           listcb[2]
**    +--------+             +--------+          +--------+
**    |        |             |        |          |        |
**    |        |             |        |          |        |
**    +--------+             +--------+          +--------+
**         |                      |                   |
**         | ptr to head          | ptr to head       | ptr to head
**         |                      |                   |
**         V                      V                   V
**      +-----+                +-----+             +-----+
**      |serv |___[]=[]=[]     | db  |             | lock|___[][][][][]
**      |list |    sessions    |list |             | list|    locks
**      +-----+                +-----+             +-----+
**        | |                    | |                 | |
**        | |                    | |                 | |
**        | |                    | |                 | |
**      +-----+                +-----+             +-----+
**      |serv |___[]=[]=[]     | db  |             | lock|___[][][][][]
**      |list |    sessions    |list |             | list|    locks
**      +-----+                +-----+             +-----+
**        | |                    | |                 | |
**        | |                    | |                 | |
**        | |                    | |                 | |
**      +-----+                +-----+             +-----+
**      |serv |___[]=[]=[]     | db  |             | lock|___[][][][][]
**      |list |    sessions    |list |             | list|    locks
**      +-----+                +-----+             +-----+
**         .                      .                   .
**         .                      .                   .
**         .                      .                   .
**         .                      .                   .
**
**   []=[]=[] is a linked list
**   [][][] is a contiguous block of memory accessed by incrementing
**        a pointer.
*/
/*
** Forward References
*/
FUNC_EXTERN VOID geting_error();
i4  getlockcat();
static VOID l_resswap();
/*
** Globals and statics
*/
static LISTCB list_cb[] = {
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, 0, "SERVER_LIST"},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, 0, "DB_LIST"},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, 0, "LOCK_LIST"},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, 0, "RES_LIST"},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, 0, "LOG_DBLIST"},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, 0, "LOG_XACTLIST"},
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, FALSE, 0, "LOG_PROCLIST"}
};	/* list control blocks */
static bool in_free_routine = FALSE;	/* This static is used to prevent
					** recursive calls to l_free from
					** running away.  It is set to 0
					** initially.  When l_free is called
					** it is set to 1 if it was 0.  If
					** it is one on entry, control is
					** returned to the caller
					*/
/*
** l_add - this routine is used to add a new element to a list or
**	sublist.  Elements are always added on to the end of the
**	specified list.
**
** Input
**	list_id - must be one of SERVERINFO, DBINFO, LOCKINFO, RESINFO,
**		LOGDBINFO, LOGXACTINFO or LOGPROCINFO
**	sub_list - must be > 0 if adding to a sublist and <= number of
**		elements in the main list.  0 if not adding a sublist element.
**	arg3 - in the case of the lock and resource lists, we need to know
**		how much of the the buffer was filled by the INGRES LKshow()
**		call.  This is used to calculate how many locks are in each
**		resource/lock list.  l_retrieve() uses this to determine
**		when the end of the res/lock sublists are reached.
**	info_ptr - a pointer to the information the caller wants to add
**
** Returns:
**	none
**
** History
**	1/89		tomt	Written
**	5/89		tomt	fixed up to be portable
**	8/16/89		tomt	added logging lists
**	9/3/89		tomt	various fixes and consolidations
**	9/3/89		tomt	l_free(): fixed bug where attempt to free
**				session info would accvio because
**				session list was already freed.  Just
**				needed to check if sesslist ptrs are null
**	9/8/89		tomt	l_retrieve(): fixed bug where session list
**				could be empty - never checked head ptr
**      5/26/93 (vijay)
**              include pc.h
**      21-Sep-1993 (smc)
**              Added <cs.h> for CS_SID.
**	15-Nov-1993 (tad)
**		Bug #56449
**		Replace %x with %p for pointer values where necessary.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	04-dec-2001 (devjo01)
**	    Add usage of LK_S_[LOCKS|RESOURCE]_HEADER structs, to
**	    explicitly inform compiler of memory layout for LKshow
**	    information returned.
**	18-sep-2003 (abbjo03)
**	    Include pc.h ahead of lg.h.
*/
VOID
l_add(list_id, sub_list, arg3, info_ptr)
i4  list_id;
i4  sub_list;
i4  arg3;	/* some lists need additional info - LOCK & RES */
PTR info_ptr;
{
    SERVER_LIST 	*servlist;
    SESSION_LIST 	*sesslist;
    LOCK_LIST 		*locklist;
    LOCK_RES 		*lockres;
    DB_LIST 		*dblist;
    LOGDB_LIST		*logdblist;
    LOGXACT_LIST	*logxactlist;
    LOGPROC_LIST	*logproclist;
    COMMON_LIST		*p;
    PTR			memptr;
    u_i4 		cnt;
    /*
    ** Access the listcb for the specified list.  Walk to the proper list
    ** and add the passed element
    */
    if ((list_id > MAX_LISTS - 1) || (list_id < 0))  /* error */
    {
	terminate(FAIL, "l_add: list number out of bounds (%d)\n", list_id);
    }
    if (sub_list == 0) 	/* add a new list element */
    {
	switch (list_id) 	/* have different sized list structs */
	{
	    case SERVERINFO:
		cnt = sizeof(SERVER_LIST);
		break;
	    case DBINFO:
		cnt = sizeof(DB_LIST);
		break;
	    case LOGDBINFO:
		cnt = sizeof(LOGDB_LIST);
		break;
	    case LOGXACTINFO:
		cnt = sizeof(LOGXACT_LIST);
		break;
	    case LOGPROCINFO:
		cnt = sizeof(LOGPROC_LIST);
		break;
	    case LOCKINFO:
		cnt = sizeof(LOCK_LIST);
		if (!arg3) /* MUST pass to l_add how big the buffer is */
		{
		    terminate(FAIL,
			"l_add: missing list length (arg3) for lock list");
		}
		break;
	    case RESINFO:
		cnt = sizeof(LOCK_RES);
		if (!arg3) /* MUST pass to l_add how big the buffer is */
		{
		    terminate(FAIL,
			"l_add: missing list length (arg3) for resource list");
		}
	    default:
		break;
	}
	/*
	**	Allocate memory or get it from free list
	*/
	if (list_cb[list_id].freehead == NULL)	/* need to get more */
	{
	    if ((memptr = (PTR)MEreqmem((u_i4)0, (u_i4)cnt, 
			FALSE, (STATUS*)NULL)) == NULL) 
	    {
		terminate(FAIL,
		    "l_add: error allocating memory for list structure (%s)",
		    list_cb[list_id].list_name);
	    }
	}
	else	/* have some around - use it */
	{
            p = (COMMON_LIST *)list_cb[list_id].freehead; /* pointer to mem */
            memptr = list_cb[list_id].freehead;	/* pointer to mem */
	    if (p->next == NULL)	/* last element on freelist */
	    {
		list_cb[list_id].freehead =
		    list_cb[list_id].freetail = NULL;
	    }
	    else	/* fixup pointers */
	    {
		p = (COMMON_LIST *)p->next;	/* walk to next one */
		p->previous = NULL; /* this is the head of the freelist */
		list_cb[list_id].freehead = (PTR)p;	/* point to new head */
	    }
	}
	list_cb[list_id].tail = memptr;	/* add to end */
	switch (list_id) 	/* do specialized ptr work where appropriate */
	{
	    case SERVERINFO:
		servlist = (SERVER_LIST *)memptr;
		/* put ptr to info in list */
		servlist->srvinfo = (SERVER_INFO *)info_ptr;
		servlist->ssnlst.head = NULL;	/* zero out session list head */
		servlist->ssnlst.tail = NULL;  /* same here */
		break;
	    case DBINFO:
		dblist = (DB_LIST *)memptr;
		/* put ptr to info in list */
		dblist->dbinfo = (DB_INFO *)info_ptr;
		break;
	    case LOGDBINFO:
		logdblist = (LOGDB_LIST *)memptr;
		/* put ptr to info in list */
		logdblist->lgdbinfo = (LG_DATABASE *)info_ptr;
		break;
	    case LOGXACTINFO:
		logxactlist = (LOGXACT_LIST *)memptr;
		/* put ptr to info in list */
		logxactlist->lgxactinfo = (LG_TRANSACTION *)info_ptr;
		break;
	    case LOGPROCINFO:
		logproclist = (LOGPROC_LIST *)memptr;
		/* put ptr to info in list */
		logproclist->lgprocinfo = (LG_PROCESS *)info_ptr;
		break;
	    case LOCKINFO:
		locklist = (LOCK_LIST *)memptr;
		/* put ptr to info in list */
		locklist->llbinfo = (LK_LLB_INFO *)info_ptr;
		/*
		** Calc how many locks are in list and save it for use
		** by l_retrieve()
		*/
		locklist->locks_left = locklist->cnt =
		    (arg3 - ((PTR)((LK_S_LOCKS_HEADER*)info_ptr)->lkbi -
		      (PTR)info_ptr))/ sizeof(LK_LKB_INFO);
		break;
	    case RESINFO:
		lockres = (LOCK_RES *)memptr;
		/* put ptr to info in list */
		lockres->rsbinfo = (LK_RSB_INFO *)info_ptr;
		/*
		** Calculate how many locks are in list and save it for use
		** by l_retrieve()
		*/
		lockres->locks_left = lockres->cnt =
		    (arg3 - ((PTR)((LK_S_RESOURCE_HEADER*)info_ptr)->lkbi -
		      (PTR)info_ptr))/ sizeof(LK_LKB_INFO);
	}
	if (list_cb[list_id].head == NULL) /* first element in list */
	{
	    /*
	    ** fix up linked list pointers - this is first element.  The
	    ** head and tail pointers must be the same.  The listcb[].current
	    ** holds the address of the current element - its used to remember
	    ** the last element allocated for subsequent addition of elements
	    */
	    list_cb[list_id].head = list_cb[list_id].tail; /* init head */
	    p = (COMMON_LIST *)list_cb[list_id].head;
	    p->previous = NULL;		/* no previous element */
	    p->next = NULL;		/* no next one for that matter */
	    list_cb[list_id].current = list_cb[list_id].tail;
	    /* no retrievals from this slist */
	    list_cb[list_id].retstat = FALSE;
	    list_cb[list_id].element_count = 1; /* how many elements */
	} 
	else /* nth element */
	{
	    /*
	    ** fix up linked list pointers in list.  The previous element
	    ** address is in listcb[].current.  After initing next, previous,
	    ** save the current address for next time
	    */
            p = (COMMON_LIST *)list_cb[list_id].tail;
	    p->previous = (PTR)list_cb[list_id].current;
	    p->next = NULL; /* when walking this tells us we reached the end */
	    p = (COMMON_LIST *)list_cb[list_id].current;
	    p->next = (PTR)list_cb[list_id].tail;
	    list_cb[list_id].current = (PTR)list_cb[list_id].tail;
	    list_cb[list_id].element_count++; /* count another elements */
	}
    } 
    else /* add an element to a sublist */
    {
	/*
	**    Walk to the proper list element in the proper list
	**    When the proper sublist number or the end of the list
	**    is encountered, we now have the pointer to the proper list
	**    element.
	*/
	for (p = (COMMON_LIST *)list_cb[list_id].head, cnt = 1;
	    (p != NULL) && (cnt < sub_list);
	    p = (COMMON_LIST *)p->next, cnt++);
	if (p == NULL) 	/* ERROR - sub list out of bounds */
	{
	    terminate(FAIL, "l_add: sub_list out of bounds (%d)\n", sub_list);
	}
	/*
	**    add the element into the sublist
	*/
	switch (list_id) 
	{
	    case SERVERINFO:
		servlist = (SERVER_LIST *)p;
		if (list_cb[list_id].subfreehead == NULL)
		{	/* need to get more */
		    if ((sesslist = (SESSION_LIST *)MEreqmem((u_i4)0, 
			(u_i4)sizeof(SESSION_LIST), (bool) FALSE, 
			(STATUS*)NULL)) == NULL)  /*error */
		    {
			terminate(FAIL,
			    "l_add: can't allocate memory for sublist");
		    }
		}
		else	/* have some around - use it */
		{
		    /* pointer to mem */
	            p = (COMMON_LIST *)list_cb[list_id].subfreehead;
		    /* pointer to mem */
	            sesslist = (SESSION_LIST *)list_cb[list_id].subfreehead;
		    if (p->next == NULL)	/* last element on freelist */
		    {
			list_cb[list_id].subfreehead =
			    list_cb[list_id].subfreetail = NULL;
		    }
		    else	/* fixup pointers */
		    {
			p = (COMMON_LIST *)p->next;	/* walk to next one */
			p->previous = NULL; /* the head of the freelist */
			/* point to new head */
			list_cb[list_id].subfreehead = (PTR)p;
		    }
		}
		sesslist->sessinfo = (SESSION_INFO *)info_ptr; /* save info */
		servlist->ssnlst.tail = (PTR)sesslist;	/* add to list */
		if (servlist->ssnlst.head == NULL)  /* first in sublist */
		{
		    /*
		    ** fix up linked list pointers - this is first element.
		    ** The head and tail pointers must be the same.  The
		    ** listcb[].current holds the address of the current
		    ** element - used to remember the last element
		    ** allocated for subsequent addition of elements
		    */
		    /* init head */
		    servlist->ssnlst.head = servlist->ssnlst.tail;
		    servlist->ssnlst.current = servlist->ssnlst.tail;
		    /* no retrievals made against this list */
		    servlist->ssnlst.retstat = FALSE;
		    sesslist->common.previous = NULL; 	/* no previous elt */
		    sesslist->common.next = NULL;	/* jpk: end of list? */
		} 
		else /* nth element in sublist */
		{
		    /*
		    ** fix up linked list pointers in list.  The previous
		    ** element address is in servlist->ssnlst.current.  After
		    ** initing next, previous, save the current address for
		    ** next time.
		    */
		    sesslist->common.previous = servlist->ssnlst.current;
		    sesslist->common.next = NULL; /* tells us end of list */
		    sesslist = (SESSION_LIST *)servlist->ssnlst.current;
		    sesslist->common.next = servlist->ssnlst.tail;
		    servlist->ssnlst.current = servlist->ssnlst.tail;
		}
		break;
	    default:
		terminate(FAIL, "l_add: the specified list_id (%s) can't have sublist info \n\tadded by this routine\n",
		    list_cb[list_id].list_name);
	}
    }
    return;
}
/*
** l_free - frees memory for the requested list.  In the case of lock info
**	the memory is free'd.  In the case of the lists themselves, they
**	are put into a free list
**
**	Inputs
**	list_id - must be one of SERVERINFO, DBINFO, LOCKINFO, RESINFO,
**		LOGXACTINFO, LOGPROCINFO or LOGDBINFO
**	sub_list - the identifier of the sub list. Starts at 1.
**
**	Returns
**		none
**
**	History
**		6/21/89		tomt	added logic to prevent recursive
**					calls to this routine from running
**					away. This can happen if there is a
**					fatal error and a call to clean_up is
**					made.  Clean_up calls l_free();
**		8/16/89		tomt	added code for log lists
**		9/3/89		tomt	fixed bug where attempt to free
**					session info would accvio because
**					session list was already freed.  Just
**					needed to check if sesslist ptrs are
**					null
**		9 April 1991	jpk	terminate calls clean_up()
*/
VOID
l_free(list_id, sub_list)
i4  list_id;
i4  sub_list;
{
    /*
    ** Declare Variables
    */
    SERVER_LIST 	*servlist;
    SESSION_LIST 	*sesslist;
    LOCK_LIST 		*locklist;
    LOCK_RES 		*lockres;
    DB_LIST 		*dblist;
    LOGDB_LIST		*logdblist;
    LOGXACT_LIST	*logxactlist;
    LOGPROC_LIST	*logproclist;
    COMMON_LIST		*p, *ptr;
    u_i4		cnt;
    bool		freesub;
    STATUS		status;
    char		errtxt[200];
    /*
    **   Check if this routine active i.e. we have been called recursively 
    */
    if (in_free_routine) /* we have been called recursively, return control */
    {
	return;
    }
    else
    {
	in_free_routine = TRUE;	/* turn it on */
    }
    /*
    **   Check for list out of bounds
    */
    if ((list_id > MAX_LISTS - 1) || (list_id < 0))  /* error */
    {
	terminate(FAIL, "l_free: list number out of bounds (%d)", list_id);
    }
    /*
    **   Check if there is no memory to free - if none, return
    */
    if (list_cb[list_id].head == NULL) 
    {
	in_free_routine = FALSE;	/* turn it off - we're leaving */
	return;
    }
    /*
    **   Set flag if user wants to free a sublist
    */
    if (sub_list > 0)
    {
	freesub = TRUE;
    }
    else
    {
	freesub = FALSE;
    }
    /*
    **   Walk thru list and free up memory where info is held.
    **   In the case of SERVERINFO, we free up the memory where
    **   server information is held.  The session linked list hanging off
    **   every server element is put in the list_cb session free list.
    **
    **   If we are freeing a specific sub list, we don't place
    **   any list elements in the freelist and we don't free the
    **   the list pointer to the information passed by the user.
    */
    for (p = (COMMON_LIST *)list_cb[list_id].head, cnt = 1;
	p != NULL; p = (COMMON_LIST *)p->next, cnt++) 
    {
	/*
	**   If caller wants to free only a sublist, free only the specified
	**   sublist, then get out
	*/
	if (freesub) 
	{
	    if (cnt < sub_list)  /* skip this one */
	    {
		continue;
	    }
	    else if (cnt > sub_list)  /* break out of the for loop - DONE */
	    {
		break;
	    }
	}
	switch (list_id) 
	{
	    case SERVERINFO:
		servlist = (SERVER_LIST *)p;
		if (!freesub) 
		{
		    if (servlist->srvinfo == NULL)
		    {
			SIprintf("l_free: serverlist memory already free.\n");
		    }
		    else
		    {
			if ((status = MEfree((PTR)servlist->srvinfo)) != OK)
			{
			    geting_error(status, errtxt);
			    terminate(FAIL,
				"l_free: error %d freeing serverlist: %s",
				status, errtxt);
			}
		    }
		}
		/* walk the session list, free mem and fill freelist */
		for (sesslist = (SESSION_LIST *)servlist->ssnlst.head;
		    sesslist != NULL;
		    sesslist = (SESSION_LIST *)sesslist->common.next)
		{
		    if (MEfree((PTR)sesslist->sessinfo))
		    {
			terminate(FAIL, "l_free: error freeing sessioninfo");
		    }
		}
		/*
		** Find out if sublist freelists are empty.  If they are, initialize 
		** them, else, move the current sublist into the sublist freelist.  
		*/
		if (list_cb[list_id].subfreehead == NULL)  /* empty */
		{
		    list_cb[list_id].subfreehead = (PTR)servlist->ssnlst.head;
		    list_cb[list_id].subfreetail = (PTR)servlist->ssnlst.tail;
		} 
		else /* already stuff in free list */
		{
		    COMMON_LIST *tmp;
		    /*
		    ** get tail of freelist
		    */
		    tmp = (COMMON_LIST *)list_cb[list_id].subfreetail;
		    /*
		    ** Get head of ssnlist.  Make sure its NOT null as
		    ** ssnlist may have already been freed by previous
		    ** call to l_free	tomt  9/3/89
		    */
		    if ((ptr = (COMMON_LIST *)servlist->ssnlst.head) == NULL)
		    {
			break;
		    }
		    tmp->next = (PTR)ptr;			/* point freelist tail at head*/
		    ptr->previous = (PTR)tmp;		/* point head to freelist tail */
		    list_cb[list_id].subfreetail = (PTR)servlist->ssnlst.tail;
		}
		servlist->ssnlst.tail = servlist->ssnlst.head = NULL;
		break;
	    case DBINFO:
		dblist = (DB_LIST *)p;
		if (!freesub) 
		{
		    if (MEfree((PTR)dblist->dbinfo))
		    {
			terminate(FAIL, "l_free: error freeing dblist");
		    }
		}
		else 
		{
		    in_free_routine = FALSE; /* turn it off - we're leaving */
		    return;
		}
		break;
	    case LOGDBINFO:
		logdblist = (LOGDB_LIST *)p;
		if (!freesub) 
		{
		    if (MEfree((PTR)logdblist->lgdbinfo))
		    {
			terminate(FAIL, "l_free: error freeing logdblist");
		    }
		}
		else 
		{
		    in_free_routine = FALSE; /* turn it off - we're leaving */
		    return;
		}
		break;
	    case LOGXACTINFO:
		logxactlist = (LOGXACT_LIST *)p;
		if (!freesub) 
		{
		    if (MEfree((PTR)logxactlist->lgxactinfo))
		    {
			terminate(FAIL, "l_free: error freeing log xact");
		    }
		}
		else 
		{
		    in_free_routine = FALSE; /* turn it off - we're leaving */
		    return;
		}
		break;
	    case LOGPROCINFO:
		logproclist = (LOGPROC_LIST *)p;
		if (!freesub) 
		{
		    if (MEfree((PTR)logproclist->lgprocinfo))
		    {
			terminate(FAIL, "l_free: error freeing log proc");
		    }
		}
		else 
		{
		    in_free_routine = FALSE; /* turn it off - we're leaving */
		    return;
		}
		break;
	    case LOCKINFO:
		locklist = (LOCK_LIST *)p;
		if (!freesub) 
		{
		    if (MEfree((PTR)locklist->llbinfo))
		    {
			terminate(FAIL, "l_free: error freeing locklist");
		    }
		}
		else 
		{
		    in_free_routine = FALSE; /* turn it off - we're leaving */
		    return;
		}
		break;
	    case RESINFO:
		lockres = (LOCK_RES *)p;
		if (!freesub) 
		{
		    if (MEfree((PTR)lockres->rsbinfo))
		    {
			terminate(FAIL, "l_free: error freeing reslist");
		    }
		}
		else 
		{
		    in_free_routine = FALSE;	/* turn it off - we're gone */
		    return;
		}
	}
    };
    /*
    ** Find out if freelists are empty.  If they are, initialize them,
    ** else, move the current list into the freelist.  Don't do this
    ** if caller only wants to free a sublist
    */
    if (!freesub) 
    {
	if (list_cb[list_id].freehead == NULL)  /* empty */
	{
	    list_cb[list_id].freehead = list_cb[list_id].head;
	    list_cb[list_id].freetail = list_cb[list_id].tail;
	} 
	else  /* already stuff in free list */
	{
	    p = (COMMON_LIST *)list_cb[list_id].freetail;	/* get tail of freelist */
	    ptr = (COMMON_LIST *)list_cb[list_id].head;	/* get head of current list */
	    p->next = (PTR)ptr;			/* point freelist tail at head*/
	    ptr->previous = (PTR)p;			/* point head to freelist tail */
	    list_cb[list_id].freetail = list_cb[list_id].tail;
	}
	list_cb[list_id].element_count = 0;	/* no elements in list */
	list_cb[list_id].tail = list_cb[list_id].head = NULL;
    }
    in_free_routine = FALSE;	/* turn it off - we're leaving */
    return;
}
/*
** l_retrieve() - this function returns a pointer to the piece
**	of requested information.  The user specifies a list
**	and an optional sublist.  For each list being accessed,
**	a pointer is kept for the last item retrieved.  Thus
**	successive calls for info from a particular list returns
**	successive pieces of info from that list.  When the last
**	element has been returned, the next call to l_retrieve() will
**	return a NULL pointer.  The next call then returns the first
**	element of the specified list
**
**	Inputs
**	list_id - must be one of SERVERINFO, DBINFO, LOCKINFO, RESINFO,
**		LOGXACTINFO, LOGPROCINFO or LOGDBINFO
**	sub_list - a number starting from 1.  This number is referenced
**		to the row position in a display.  Since calls to this
**		return info in successive order, the row position on a
**		screen is sufficient to identify a sublist.  
**	reset_flag - if set, it indicates to start at the beginning
**		of the list no matter what (geo...).  If the flag is
**		set, it will be unset to prevent an infinite loop as
**		the first element is always returned on each call.
**		This means that you cant call l_retrieve and specify
**		this parameter as a constant.
**
**      Returns
**	a pointer to the requested info.  The caller should cast it
**	to the particular type.
**
*/
PTR
l_retrieve(list_id, sub_list, reset_flag)
i4  list_id;
i4  sub_list;
bool *reset_flag;
{
    SERVER_LIST		*servlist;
    SESSION_LIST	*sesslist;
    DB_LIST		*dblist;
    LOCK_LIST		*locklist;
    LOCK_RES		*lockres;
    LOGDB_LIST		*logdblist;
    LOGXACT_LIST	*logxactlist;
    LOGPROC_LIST	*logproclist;
    COMMON_LIST		*p;
    u_i4		cnt;
    bool		reset;
    /* Check for list out of bounds */
    if (list_id > (MAX_LISTS - 1) || list_id < 0)  /* error */
    {
	terminate(FAIL, "l_retrieve: list number out of bounds (%d)\n",
	    list_id);
    }
    /* Are there any elements in list? */
    if (list_cb[list_id].head == NULL)
	return(NULL);
    /* See if caller wants to start at beginning of list */
    reset = FALSE;	/* init local flag */
    if (reset_flag != NULL)  /* check if passed pointer is NOT null */
    {
	if (*reset_flag) 	/* reset to beginning - set flags */
	{
	    *reset_flag = FALSE;	/* got it - unset */
	    reset = TRUE;		/* set local flag */
	}
    }
    /*
    **   Retrieve a list element or a sublist element?
    **   The first call to retrieve (on each list) will return a pointer
    **   to the head element.  Each subsequent call will return the next
    **   element (in that list).  The call which would step beyond the end
    **   of the list returns a null pointer.
    */
    if (sub_list == 0) 	/* Caller wants a list element */
    {
	if (!list_cb[list_id].retstat || reset) 
	{
	    /* first retrieval */
	    /* we have retrieved from this list */
	    list_cb[list_id].retstat = TRUE; 
	    p = (COMMON_LIST *)list_cb[list_id].head;
	}
	else  
	{
	    /* nth retrieval */
	    if (list_cb[list_id].current == NULL)  /* end of list */
	    {
		/* END OF LIST - set retstat to FALSE and return NULL */
		list_cb[list_id].retstat = FALSE;
	        return(NULL);
	    }
	    p = (COMMON_LIST *)list_cb[list_id].current;
	}	    
	list_cb[list_id].current = p->next; /* advance to next element */
	switch (list_id) 	/* structs for each list are different */
	{
	    case SERVERINFO:
		servlist = (SERVER_LIST *)p;
		return((PTR)servlist->srvinfo);
	    case DBINFO:
		dblist = (DB_LIST *)p;
		return((PTR)dblist->dbinfo);
	    case LOGDBINFO:
		logdblist = (LOGDB_LIST *)p;
		return((PTR)logdblist->lgdbinfo);
	    case LOGXACTINFO:
		logxactlist = (LOGXACT_LIST *)p;
		return((PTR)logxactlist->lgxactinfo);
	    case LOGPROCINFO:
		logproclist = (LOGPROC_LIST *)p;
		return((PTR)logproclist->lgprocinfo);
	    case LOCKINFO:
		locklist = (LOCK_LIST *)p;
		return((PTR)locklist->llbinfo);
	    case RESINFO:
		lockres = (LOCK_RES *)p;
		return((PTR)lockres->rsbinfo);
	}
    } 
    else  /* Caller wants a sublist element */
    {
	/*
	**    Walk to the proper list element in the proper list
	**    When the proper sublist number or the end of the list
	**    is encountered, we now have the pointer to the proper list
	**    element.
	*/
	for (p = (COMMON_LIST *)list_cb[list_id].head, cnt = 1;
	    (p != NULL) && (cnt < sub_list);
	    p = (COMMON_LIST *)p->next, cnt++);
	if (p == NULL)   /* ERROR: sub list out of bounds - RETURN NULL */
	{
	    return(NULL);
	}
	/*
	**    get the next element from the sublist
	*/
	switch (list_id) 
	{
	    case SERVERINFO:
		servlist = (SERVER_LIST *)p;
		/*
		** Check that sublist has something in it.  sublist does not
		** have to be populated.	(tmt 9/8/89)
		*/
		if (servlist->ssnlst.head == NULL)
		    return(NULL);
		if (!servlist->ssnlst.retstat || reset)  /* first retrieval */
		{
		    d_print("first retrieval\n");
		    /* we have now, retrieved from this list, remember that */
		    servlist->ssnlst.retstat = TRUE;
		    p = (COMMON_LIST *)servlist->ssnlst.head;
		} 
		else  /* nth retrieval */
		{
		    d_print("nth retrieval: %p %p %p\n",
			servlist->ssnlst.current, p->next, servlist->ssnlst.tail);
		    if ((servlist->ssnlst.current == NULL) ||
			(servlist->ssnlst.current == p->next))
		    { 		 /* end of list */
			d_print("end of list\n");
			/* END OF LIST - set retstat to FALSE, return NULL */
			servlist->ssnlst.retstat = FALSE;
			return(NULL);
		    }
		    p = (COMMON_LIST *)servlist->ssnlst.current;
		}
		d_print("about to die\n");
	        /* advance to next element */
	        servlist->ssnlst.current = (PTR)p->next;
		d_print("survived!\n");
		sesslist = (SESSION_LIST *)p;
		return((PTR)sesslist->sessinfo);
		/* NOTREACHED */
	    case LOCKINFO:
		locklist = (LOCK_LIST *)p;
		if (sub_list == 0)  /* user wants lock list header info */
		    return((PTR)locklist->llbinfo);
		 /* first retrieval */
		if (reset || (locklist->cnt == locklist->locks_left))
		{
		    /* head of memory for this list */
		    LK_LLB_INFO	*llb = locklist->llbinfo;
		    LK_LKB_INFO *lkb = ((LK_S_LOCKS_HEADER *)llb)->lkbi;
		    if (locklist->locks_left == 0) /* none in the list */
			return(NULL);
                    locklist->locks_left--;	/* decrement how many to go */
		    p = (COMMON_LIST *)lkb;	/* save pointer to first */
		    locklist->current = ++lkb;	/* save ptr to next element */
		    return((PTR)p);
		} 
		else  /* nth retrieval */
		{
		    LK_LKB_INFO *lkb = (LK_LKB_INFO *)locklist->current;
		    if (locklist->locks_left == 0)	/* end of list */
		    {
			/* END OF LIST - reset locks left to tot # of locks */
			locklist->locks_left = locklist->cnt;
			return(NULL);			/* end of list */
		    }
                    locklist->locks_left--; /* decrement how many to go */
		    p = (COMMON_LIST *)locklist->current;
		    locklist->current = ++lkb;
		    return((PTR)p);		/* advance to next element */
		}	    
	    case RESINFO:
		lockres = (LOCK_RES *)p;
		/* user wants lock by resource header */
		if (sub_list == 0)
		    return((PTR)lockres->rsbinfo);
		if (reset || (lockres->cnt == lockres->locks_left))
		{				/* first retrieval */
		    /* head of memory for this list */
		    LK_RSB_INFO	*rsb = lockres->rsbinfo;
		    LK_LKB_INFO *lkb = ((LK_S_RESOURCE_HEADER *)rsb)->lkbi;
		    if (lockres->locks_left == 0) /* none in the list */
			return(NULL);
                    lockres->locks_left--;	/* decrement how many to go */
		    p = (COMMON_LIST *)lkb;	/* save pointer to first */
	            lockres->current = ++lkb;	/* save ptr to next element */
		    return((PTR)p);		/* return the first one */
	        } 
		else 
		{
		    LK_LKB_INFO *lkb = (LK_LKB_INFO *)lockres->current;
		    if (lockres->locks_left == 0)  /* end of list */
		    {
			/* END OF LIST - reset locks left to tot # of locks */
			lockres->locks_left = lockres->cnt;
			return(NULL);	/* end of list */
		    }
                    lockres->locks_left--;	/* decrement how many to go */
		    /* save ptr to current element */
		    p = (COMMON_LIST *)lockres->current;
		    lockres->current = ++lkb;	/* save ptr to next element */
		    return((PTR)p);
	        }
		/* NOTREACHED */
	    default:
		terminate(FAIL, "l_retrieve: the specified list_id (%d) can't have sublist info \n\tretrieved by this routine\n", list_id);
	}
    }
    /* NOTREACHED */
}
/*
** l_num_elements - this routine returns the number of elements in a
**	a specified list.  This is needed by routines which walk all
**	sublists in a given list.
**
**	History
**		2.28.89		tomt	Written
**
**	Inputs
**		list_num - the desired list to get the count on
**
**	Outputs
**		none
**
**	Returns
**		the number of elements in the specified list
*/
i4
l_num_elements(list_num)
i4  list_num;
{
    /*
    **   Check for list out of bounds
    */
    if ((list_num > MAX_LISTS - 1) || (list_num < 0))  /* error */
    {
	terminate(FAIL, "l_num_elements: list number out of bounds (%d)\n",
	    list_num);
    }
    return(list_cb[list_num].element_count);
}
/*
** l_ressort() - this routine will sort the resource list such that
**	db accesses are minimized.  This routine must be called after
**	the resource list is fully built.
**
** Inputs
**	none
**
** Output
**	none
**
** Returns
**	none	
**
** History
**	3.6.89		tomt	written
**	3.9.89  	tomt	took the shotgun approach.  There are three
**				sorts.  The first one sorts by db.  The second 
**				sorts by locktypes within a db.  The third 
**				places the page locks in line after the 
**				respective table locks
**	9/24/89		tomt	did not swap the resinfo struct completely.
**				Now, all relevant elements are switched.  The
**				*correct* way is to fix up the linked list
**				pointers to switch elements instead of
**				switching the element contents - TAIL wagging
**				the DOG.
**	9/24/89		tomt	pulled out code to make the swap - l_resswap()
**
**	11/22/96	dilma04 added code to sort row and value locks 
*/
VOID
l_ressort()
{
    COMMON_LIST	*p, *q;
    LK_RSB_INFO *resinfo1, *resinfo2;
    LOCK_RES	*lockres1, *lockres2;
    u_i4	tdb_id, db_id1, db_id2, ilock_cat, olock_cat;
    /*
    ** Sort the values by walking the lists and swapping the pointers
    ** to the resource info
    **
    ** The sort will appear something like
    **
    ** all resources for db1
    ** all resources for db2
    **  and so on
    **
    */
    /* The database sort */
    for (p = (COMMON_LIST *)list_cb[RESINFO].head;
	p != NULL;
	p = (COMMON_LIST *)p->next) 
    {
	lockres1 = (LOCK_RES *)p;
	resinfo1 = lockres1->rsbinfo;
	switch(olock_cat = getlockcat(resinfo1->rsb_key[0])) /* ltype catagory */
	{
	    case DATABASE_LOCK:
		if ((resinfo1->rsb_key[0] == LK_BM_DATABASE) ||
		    (resinfo1->rsb_key[0] == LK_CKP_DB)) /* dbname handling */
		    db_id1 = resinfo1->rsb_key[1]; /* no need to look this up */
		else
		    db_id1 = get_db_id((PTR) &resinfo1->rsb_key[1]);
		break;
	    default:
		db_id1 = resinfo1->rsb_key[1];
	}
	for (q = (COMMON_LIST *)p->next; q != NULL; q = (COMMON_LIST *)q->next) 
	{
	    lockres2 = (LOCK_RES *)q;
	    resinfo2 = lockres2->rsbinfo;
	    switch(ilock_cat = getlockcat(resinfo2->rsb_key[0])) 
	    {
		case DATABASE_LOCK:
		    if ((resinfo2->rsb_key[0] == LK_BM_DATABASE) ||
		        (resinfo2->rsb_key[0] == LK_CKP_DB)) /* dbname handling */
		        db_id2 = resinfo2->rsb_key[1]; /* no need to look this up */
		    else
		        db_id2 = get_db_id((PTR) &resinfo2->rsb_key[1]);
		    break;
		default:
		    db_id2 = resinfo2->rsb_key[1];
	    }
	    if (db_id1 > db_id2)  /* sort by database */
	    {
		resinfo1 = lockres2->rsbinfo;
		l_resswap(lockres1, lockres2);	/* swap */
		olock_cat = getlockcat(resinfo1->rsb_key[0]);
		tdb_id = db_id1;
	        db_id1 = db_id2; /* need for proper comparison next time */
		db_id2 = tdb_id;
	    } 
	    else if ((db_id2 == db_id1)	/* sort database locks first */
		&& (ilock_cat == DATABASE_LOCK)
		&& (olock_cat != DATABASE_LOCK)) 
	    {
		resinfo1 = lockres2->rsbinfo;
		l_resswap(lockres1, lockres2);	/* swap */
		olock_cat = getlockcat(resinfo1->rsb_key[0]);
	    } 
	    else if ((db_id2 == db_id1) /* sort by table major (reltid) name */
		&& ((ilock_cat == PAGE_LOCK) || (ilock_cat == TABLE_LOCK))
		&& ((olock_cat == PAGE_LOCK) || (olock_cat == TABLE_LOCK))
		&& (resinfo1->rsb_key[2] > resinfo2->rsb_key[2])) 
	    {
		resinfo1 = lockres2->rsbinfo;
		l_resswap(lockres1, lockres2);
		olock_cat = getlockcat(resinfo1->rsb_key[0]);
	    } 
	    else if ((db_id2 == db_id1) /* sort by table minor (reltidx) name */
		&& ((ilock_cat == PAGE_LOCK) || (ilock_cat == TABLE_LOCK))
		&& ((olock_cat == PAGE_LOCK) || (olock_cat == TABLE_LOCK))
		&& (resinfo1->rsb_key[2] == resinfo2->rsb_key[2])
		&& (resinfo1->rsb_key[3] > resinfo2->rsb_key[3])) 
	    {
		resinfo1 = lockres2->rsbinfo;
		l_resswap(lockres1, lockres2);
		olock_cat = getlockcat(resinfo1->rsb_key[0]);
	    } 
	    else if ((db_id2 == db_id1)  /* sort page locks by table locks */
		&& ((ilock_cat == PAGE_LOCK) || (ilock_cat == TABLE_LOCK))
		&& ((olock_cat == PAGE_LOCK) || (olock_cat == TABLE_LOCK))
		&& (resinfo1->rsb_key[2] == resinfo2->rsb_key[2])
		&& (resinfo1->rsb_key[3] == resinfo2->rsb_key[3])) 
	    {
		if ((ilock_cat == TABLE_LOCK) && (olock_cat == PAGE_LOCK)) 
		{
		    resinfo1 = lockres2->rsbinfo;
		    l_resswap(lockres1, lockres2);
		    olock_cat = getlockcat(resinfo1->rsb_key[0]);
		} 
		else if ((ilock_cat == PAGE_LOCK) && (olock_cat == PAGE_LOCK)
		    && (resinfo1->rsb_key[4] > resinfo2->rsb_key[4])) 
		{
		    resinfo1 = lockres2->rsbinfo;
		    l_resswap(lockres1, lockres2);	/* swap */
		    olock_cat = getlockcat(resinfo1->rsb_key[0]);
		}
	    }
            else if ((db_id2 == db_id1)  /* sort value locks by table locks */
                && ((ilock_cat == VALUE_LOCK) || (ilock_cat == TABLE_LOCK))
                && ((olock_cat == VALUE_LOCK) || (olock_cat == TABLE_LOCK))
                && (resinfo1->rsb_key[2] == resinfo2->rsb_key[2])
                && (resinfo1->rsb_key[3] == resinfo2->rsb_key[3]))
            {
                if ((ilock_cat == TABLE_LOCK) && (olock_cat == VALUE_LOCK))
                {
                    resinfo1 = lockres2->rsbinfo;
                    l_resswap(lockres1, lockres2);
                    olock_cat = getlockcat(resinfo1->rsb_key[0]);
                }
                else if ((ilock_cat == VALUE_LOCK) && (olock_cat == VALUE_LOCK)
                    && (resinfo1->rsb_key[4] > resinfo2->rsb_key[4]))
                {
                    resinfo1 = lockres2->rsbinfo;
                    l_resswap(lockres1, lockres2);      /* swap */
                    olock_cat = getlockcat(resinfo1->rsb_key[0]);
                }
            }
            else if ((db_id2 == db_id1)  /* sort row locks by page locks */
                && ((ilock_cat == ROW_LOCK) || (ilock_cat == PAGE_LOCK))
                && ((olock_cat == ROW_LOCK) || (olock_cat == PAGE_LOCK))
                && (resinfo1->rsb_key[2] == resinfo2->rsb_key[2])
                && (resinfo1->rsb_key[3] == resinfo2->rsb_key[3]))
            {
                if ((ilock_cat == PAGE_LOCK) && (olock_cat == ROW_LOCK))
                {
                    resinfo1 = lockres2->rsbinfo;
                    l_resswap(lockres1, lockres2);
                    olock_cat = getlockcat(resinfo1->rsb_key[0]);
                }
                else if ((ilock_cat == ROW_LOCK) && (olock_cat == ROW_LOCK)
                    && (resinfo1->rsb_key[4] > resinfo2->rsb_key[4]))
                {
                    resinfo1 = lockres2->rsbinfo;
                    l_resswap(lockres1, lockres2);      /* swap */
                    olock_cat = getlockcat(resinfo1->rsb_key[0]);
                }
            }
	}
    }
    return;
}
/*
** getlockcat() - this function returns a locktype classification
**	As far as the sort routines are concerned, there are only
**	three relevant classes of locks DATABASE_LOCK, TABLE_LOCK, PAGE_LOCK.
**	All other INGRES lock types are mapped to OTHER_LOCK.
**
**
**    22-nov-1996     dilma04 Added LK_PH_PAGE, LK_ROW and LK_VAL_LOCK for 
**		      row level loking project.
*/
i4
getlockcat(lock_type)
i4	lock_type;
{
    switch(lock_type) 
    {
	case LK_DATABASE:
	case LK_SV_DATABASE:
	case LK_OWNER_ID:
	case LK_JOURNAL:
	case LK_DB_TEMP_ID:
	case LK_CONFIG:
	case LK_OPEN_DB:
	case LK_BM_DATABASE:
	case LK_CKP_DB:
	    return(DATABASE_LOCK);
	case LK_TABLE:
	case LK_BM_TABLE:
	case LK_SV_TABLE:
	case LK_TBL_CONTROL:
	case LK_EXTEND_FILE:
	case LK_CREATE_TABLE:
	    return(TABLE_LOCK);
	case LK_PAGE:
	case LK_BM_PAGE:
	case LK_PH_PAGE:
	    return(PAGE_LOCK);	    
 	case LK_ROW:
 	    return(ROW_LOCK);
 	case LK_VAL_LOCK:
            return(VALUE_LOCK);
	default:
	    return(OTHER_LOCK);
    }
}
/*
** l_resswap() - this routine is called by l_ressort to swap elements
**	during the sort
**
** Inputs:
**	resinfo1 - pointer to first element
**	resinfo2 - pointer to second element
**
** Outputs:
**	the elements pointed to by the input are switched
**
** Returns:
**	none
**
** History
**	9/24/89		tomt	created
*/
static VOID
l_resswap(lockres1, lockres2)
LOCK_RES *lockres1;
LOCK_RES *lockres2;
{
    /*
    ** Declare
    */
    LOCK_RES	resswap;
    /*
    ** Swap values
    */
    resswap.current = lockres1->current;
    resswap.cnt = lockres1->cnt;
    resswap.locks_left = lockres1->locks_left;
    resswap.rsbinfo = lockres1->rsbinfo;
    lockres1->current = lockres2->current;
    lockres1->cnt = lockres2->cnt;
    lockres1->locks_left = lockres2->locks_left;
    lockres1->rsbinfo = lockres2->rsbinfo;
    lockres2->current = resswap.current;
    lockres2->cnt = resswap.cnt;
    lockres2->locks_left = resswap.locks_left;
    lockres2->rsbinfo = resswap.rsbinfo;
    return;
}
/*
** l_srvsort() - called to order the server list by connection id - this
**	allows us to easily remove duplicates ( as a server with -dblist
**	appears once for each database specified in the list ).
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Side effects:
**	the current SERVERINFO list is sorted by connection id 
**	( srvinfo->server ) and also merged to remove duplicates.
**	Memory used by duplicate entries is either released ( for the
**	SERVER_INFO structure ) or put on the list control block 
**	freelist ( the actual list element ).
**
** Returns:
**	none
**
** History
**	06-jun-1995 (nick)
**	    Created.
*/
void
l_srvsort()
{
    COMMON_LIST	*p, *q;
    SERVER_LIST	*servlist1, *servlist2;
    char	*server1, *server2;
    STATUS	status;
    char	errtxt[200];
    for (p = (COMMON_LIST *)list_cb[SERVERINFO].head;
	p != NULL;
	p = (COMMON_LIST *)p->next)
    {
	servlist1 = (SERVER_LIST *)p;
	server1 = servlist1->srvinfo->server;
	q = (COMMON_LIST *)p->next;
	while (q != NULL)
	{
	    register i4	cmpres;
	    PTR			qtmp = NULL;
	    servlist2 = (SERVER_LIST *)q;
	    server2 = servlist2->srvinfo->server;
	    cmpres = STcompare(server1, server2);
	    if (cmpres > 0)
	    {
		/* need to swap */
		SERVER_LIST	tmpservlist;
		server1 = server2;
		tmpservlist.srvinfo = servlist1->srvinfo;
		STRUCT_ASSIGN_MACRO(servlist1->ssnlst, tmpservlist.ssnlst);
		servlist1->srvinfo = servlist2->srvinfo;
		STRUCT_ASSIGN_MACRO(servlist2->ssnlst, servlist1->ssnlst);
		servlist2->srvinfo = tmpservlist.srvinfo;
		STRUCT_ASSIGN_MACRO(tmpservlist.ssnlst, servlist2->ssnlst);
	    }
	    else if (cmpres == 0)
	    {
		/* same so merge database lists and free duplicate */
		register char	*dblist1, *dblist2;
		register i4	len1, len2;
		char		tmpbuf[GCN_OBJ_MAX_LEN + 1];
		COMMON_LIST	*ptr;
		dblist1 = servlist1->srvinfo->database;
		dblist2 = servlist2->srvinfo->database;
		/* only do this if we can fit some more in */
		if ((len1 = STlength(dblist1)) < GCN_OBJ_MAX_LEN)
		{
		    len2 = STlength(dblist2) + 1;
		    if ((len1 + len2) > GCN_OBJ_MAX_LEN)
			len2 = GCN_OBJ_MAX_LEN - len1;
		    if (STlcopy(dblist2, tmpbuf, (len2 - 1)))
		    	STcat(STcat(dblist1, ","), tmpbuf);
		}
		/* 
		** free the server info. - originally allocated in
		** st_nameinfo()
		*/
		if ((status = MEfree((PTR)servlist2->srvinfo)) != OK)
		{
		    geting_error(status, errtxt);
		    terminate(FAIL,
			"l_srvsort: error %d freeing serverlist: %s",
			status, errtxt);
		}
		/* remove this element from linked list */
		if (q->previous)
		{
		    /* should always get here I think but just in case */
		    ptr = (COMMON_LIST *)q->previous;
		    ptr->next = q->next;
		    if (list_cb[SERVERINFO].tail == (PTR)q)
			list_cb[SERVERINFO].tail = q->previous;
		}
		if (q->next)
		{
		    ptr = (COMMON_LIST *)q->next;
		    ptr->previous = q->previous;
		    if (list_cb[SERVERINFO].head == (PTR)q)
			list_cb[SERVERINFO].head = q->next;
		}
		/*
		** and move the list structure onto the CB
		** free list - save the pointer to the next
		** element as we trash this as part of the 
		** manipulation
		*/
		qtmp = q->next;
		if (list_cb[SERVERINFO].freehead == NULL) /* empty */
		{
		    list_cb[SERVERINFO].freehead = (PTR)q;
		    list_cb[SERVERINFO].freetail = (PTR)q;
		    q->previous = q->next = NULL;
		}
		else
		{
		    ptr = (COMMON_LIST *)list_cb[SERVERINFO].freetail;
		    ptr->next = (PTR)q;
		    q->previous = (PTR)ptr;
		    q->next = NULL;
		    list_cb[SERVERINFO].freetail = (PTR)q;
		}
	    }
	    if (qtmp != NULL)
		q = (COMMON_LIST *)qtmp;
	    else
		q = (COMMON_LIST *)q->next;
	}
    }
}
