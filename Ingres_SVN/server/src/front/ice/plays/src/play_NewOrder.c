# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/**
** Copyright (c) 2004 Ingres Corporation
** Name: play_NewOrderNr.sc
**
** Description:
**     Returns a new order number from the counters table
**     Uses a database procedure to get the latest number.
**
## History:
##     27-oct-1998 (peeje01)
##         created
##      21-Nov-00 (fanra01)
##          Bug 103306
##          The out_OrderNumber once freed should be nulled so that
##          the ice server does not treat the pointer as an output tag and
##          assume it can be freed using MEfree.
*/
# include "play_NewOrder.h"
# define    MAX_SIZE    20  /* about the right size for an int */
/*
** Parameter name list.
** A NULL pointer terminates the list.
*/
static char* pszNewOrderParams[] =
    {"out_orderNumber", NULL};
/*
** Function Description
*/
static SERVER_DLL_FUNCTION FunctionTable[] =
{
	    { "newOrder", { pszNewOrderParams } },
	    { NULL }
};
/*
** Name: InitICEServerExtension
**
** Description:
**     Mandatory function for providing function description to the server.
**
** Inputs:
**     None.
**
** Outputs:
**     ppServerDllFunction: pointer to the function description structure.
**
** Returns:
**     pointer to error text
**     NULL on success
*/
ICE_EXT_API ICE_STATUS
InitICEServerExtension(PSERVER_DLL_FUNCTION* ppServerDllFunction)
{
	ICE_STATUS status = NULL;
	*ppServerDllFunction = FunctionTable;
	return status;
}
/*
** Name: newOrder
**
** Description:
**     Return the next order number
**
** Inputs:
**     None.
**
** Outputs:
**     out_OrderNumber
**
** Return:
**     pointer to error text
**     NULL on success
*/
ICE_EXT_API ICE_STATUS
newOrder(char** out_OrderNumber, BOOL* print, MEMORY_P *context)
{
    ICE_STATUS  status = NULL;
    *print = (char)FALSE;
    /*
    ** if first invocation allocate some memory for the result
    */
    if (*context == NULL)
    {
  long x;
        *context = mt_safe_malloc(MAX_SIZE);
        if (*context == NULL)
        {
            return ("Memory error\n");
        }
/* # line 98 "play_NewOrder.sc" */	/* connect */
  {
    IIsqInit((char *)0);
    IIsqUser((char *)"icedbuser");
    IIsqConnect(0,(char *)"icetutor",(char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0);
  }
/* # line 99 "play_NewOrder.sc" */	/* execute procedure */
  {
    IIsqInit((char *)0);
    IILQpriProcInit(2,(char *)"new_order");
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
    IIeqiqio((short *)0,1,30,4,&x,(char *)"iiret");
  }
/* # line 101 "play_NewOrder.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 102 "play_NewOrder.sc" */	/* disconnect */
  {
    IIsqInit((char *)0);
    IIsqDisconnect();
  }
/* # line 104 "play_NewOrder.sc" */	/* host code */
        sprintf (*context, "%d", x);
        *out_OrderNumber = *context;
        *print = TRUE;
    }
    else
    {
        mt_safe_free (*context);
        *context = NULL;
        *out_OrderNumber = NULL;
    }
    return (status);
}
