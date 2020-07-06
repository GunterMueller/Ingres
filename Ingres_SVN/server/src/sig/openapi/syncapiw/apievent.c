/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <string.h>
# include <stdlib.h>
# include <iiapi.h>
# include <sapiw.h>
# include "sapiwi.h"

/**
** Name:	apievent.c - synchronous API wrapper dbevent functions
**
** Description:
**	Defines
**		IIsw_catchEvent		- catch a dbevent
**		IIsw_waitEvent		- wait for a dbevent
**		IIsw_getEventInfo	- get dbevent information
**		IIsw_cancelEvent	- cancel a dbevent
**		IIsw_raiseEvent		- raise a dbevent
**		gdescCallback		- getDescriptor callback
**		gcolCallback		- getColumns callback
##
## History:
##	27-apr-2000 (abbjo03)
##		Created.
##	31-mar-2004 (gupsh01)
##		Modified to pass getQinfoParm to api calls. 
**/

typedef struct
{
	SW_EVT_TEXT		*evtText;
	IIAPI_GETDESCRPARM	gdescParm;
	IIAPI_GETCOLPARM	gcolParm;
	IIAPI_DATAVALUE		dataVal;
} SW_EVT_PARMS;


static II_VOID II_FAR II_CALLBACK gdescCallback(II_PTR closure,
	II_PTR parmBlock);
static II_VOID II_FAR II_CALLBACK gcolCallback(II_PTR closure,
	II_PTR parmBlock);


/*{
** Name:	IIsw_catchEvent - catch a dbevent
**
** Description:
**	Issue an IIapi_catchEvent.
**
** Inputs:
**	connHandle	- connection handle
**	catchParm	- catch event parameter block
**	callback	- callback function for IIapi_catchEvent
**	closure		- closure parameter for IIapi_catchEvent callback
**	evtHandle	- event handle
**
** Outputs:
**	evtHandle	- event handle
**	errParm		- error parameter block
**
** Returns:
**	Status from genParm.gp_status.
*/
IIAPI_STATUS
IIsw_catchEvent(
II_PTR				connHandle,
IIAPI_CATCHEVENTPARM		*catchParm,
II_VOID (II_FAR II_CALLBACK	*callback)(II_PTR closure, II_PTR parmBlock),
II_PTR				closure,
II_PTR				*evtHandle,
IIAPI_GETEINFOPARM		*errParm)
{
	catchParm->ce_genParm.gp_callback = callback;
	catchParm->ce_genParm.gp_closure = closure;
	catchParm->ce_connHandle = connHandle;
	catchParm->ce_selectEventName = NULL;
	catchParm->ce_selectEventOwner = NULL;
	catchParm->ce_eventHandle = *evtHandle;
	IIapi_catchEvent(catchParm);
	if (catchParm->ce_genParm.gp_errorHandle)
	{
		IIsw_getErrorInfo(catchParm->ce_genParm.gp_errorHandle,
			errParm);
	}
	else
	{
		*evtHandle = catchParm->ce_eventHandle;	
		errParm->ge_errorHandle = NULL;
	}
	return (catchParm->ce_genParm.gp_status);
}


/*{
** Name:	IIsw_waitEvent - wait for a dbevent
**
** Description:
**	Issue an IIapi_getEvent.
**
** Inputs:
**	connHandle	- connection handle
**	timeout		- number of milliseconds to wait for event
**
** Outputs:
**	errParm		- error parameter block
**
** Returns:
**	Status from genParm.gp_status.
*/
IIAPI_STATUS
IIsw_waitEvent(
II_PTR		connHandle,
II_LONG		timeout,
IIAPI_GETEINFOPARM	*errParm)
{
	IIAPI_GETEVENTPARM	gevtParm;

	gevtParm.gv_genParm.gp_callback = NULL;
	gevtParm.gv_genParm.gp_closure = NULL;
	gevtParm.gv_connHandle = connHandle;
	gevtParm.gv_timeout = timeout;
	IIapi_getEvent(&gevtParm);
	IIsw_wait(&gevtParm.gv_genParm);
	if (gevtParm.gv_genParm.gp_errorHandle)
		IIsw_getErrorInfo(gevtParm.gv_genParm.gp_errorHandle, errParm);
	else
		errParm->ge_errorHandle = NULL;
	return (gevtParm.gv_genParm.gp_status);
}


/*{
** Name:	IIsw_cancelEvent - cancel for a dbevent
**
** Description:
**	Issue an IIapi_cancel on a dbevent handle.
**
** Inputs:
**	evtHandle	- dbevent handle
**
** Outputs:
**	errParm		- error parameter block
**
** Returns:
**	Status from genParm.gp_status.
*/
IIAPI_STATUS
IIsw_cancelEvent(
II_PTR			evtHandle,
IIAPI_GETEINFOPARM	*errParm)
{
	IIAPI_CANCELPARM	cancParm;

	cancParm.cn_genParm.gp_callback = NULL;
	cancParm.cn_genParm.gp_closure = NULL;
	cancParm.cn_stmtHandle = evtHandle;
	IIapi_cancel(&cancParm);
	IIsw_wait(&cancParm.cn_genParm);
	if (cancParm.cn_genParm.gp_errorHandle)
		IIsw_getErrorInfo(cancParm.cn_genParm.gp_errorHandle, errParm);
	else
		errParm->ge_errorHandle = NULL;
	return (cancParm.cn_genParm.gp_status);
}


/*{
** Name:	IIsw_getEventInfo - get dbevent information
**
** Description:
**	Get dbevent information. This should only be called if ce_eventInfoAvail
**	is TRUE.
**
** Inputs:
**	evtHandle	- dbevent handle
**	eventInfo	- event information (text)
**
** Outputs:
**	errParm		- error parameter block
**
** Returns:
**	IIAPI_ST_OUT_OF_MEMORY or status from genParm.gp_status.
*/
IIAPI_STATUS
IIsw_getEventInfo(
II_PTR		evtHandle,
SW_EVT_TEXT	*eventInfo,
IIAPI_GETEINFOPARM	*errParm)
{
	SW_EVT_PARMS		*evtParms;

	evtParms = (SW_EVT_PARMS *)calloc(1, sizeof(SW_EVT_PARMS));
	if (!evtParms)
	{
		IIsw_getErrorInfo(&IISW_internalError, errParm);
		return (IIAPI_ST_OUT_OF_MEMORY);
	}
	evtParms->evtText = eventInfo;
	evtParms->gdescParm.gd_genParm.gp_callback = gdescCallback;
	evtParms->gdescParm.gd_genParm.gp_closure = evtParms;
	evtParms->gdescParm.gd_stmtHandle = evtHandle;
	IIapi_getDescriptor(&evtParms->gdescParm);
	if (evtParms->gdescParm.gd_genParm.gp_errorHandle)
		IIsw_getErrorInfo(evtParms->gdescParm.gd_genParm.gp_errorHandle,
			errParm);
	else
		errParm->ge_errorHandle = NULL;
	return (evtParms->gdescParm.gd_genParm.gp_status);
}


/*{
** Name:	gdescCallback - getDescriptor callback
**
** Description:
**	Callback function for IIsw_getEventInfo's IIapi_getDescriptor.
**
** Inputs:
**	closure		- closure
**	parmBlock	- parameter block
**
** Outputs:
**	none
**
** Returns:
**	none
*/
static II_VOID II_FAR II_CALLBACK
gdescCallback(
II_PTR  closure,
II_PTR  parmBlock)
{
	IIAPI_GETDESCRPARM	*gdescParm = (IIAPI_GETDESCRPARM *)parmBlock;
	SW_EVT_PARMS		*evtParms = (SW_EVT_PARMS *)closure;
	
	if (gdescParm->gd_genParm.gp_status != IIAPI_ST_SUCCESS)
	{
		free(evtParms);
		return;
	}
	if (gdescParm->gd_descriptor->ds_dataType != IIAPI_VCH_TYPE ||
		gdescParm->gd_descriptor->ds_length >
		sizeof(*evtParms->evtText))
	{
		free(evtParms);
		return;
	}
	SW_COLDATA_INIT(evtParms->dataVal, *evtParms->evtText);
	evtParms->gcolParm.gc_genParm.gp_callback = gcolCallback;
	evtParms->gcolParm.gc_genParm.gp_closure = closure;
	evtParms->gcolParm.gc_stmtHandle = gdescParm->gd_stmtHandle;
	evtParms->gcolParm.gc_rowCount = 1;
	evtParms->gcolParm.gc_columnCount = 1;
	evtParms->gcolParm.gc_columnData = &evtParms->dataVal;
	IIapi_getColumns(&evtParms->gcolParm);
}


/*{
** Name:	gcolCallback - getColumns callback
**
** Description:
**	Callback function for gdescCallback's IIapi_getColumns.
**
** Inputs:
**	closure		- closure
**	parmBlock	- parameter block
**
** Outputs:
**	none
**
** Returns:
**	none
*/
static II_VOID II_FAR II_CALLBACK
gcolCallback(
II_PTR  closure,
II_PTR  parmBlock)
{
	IIAPI_GETCOLPARM	*gcolParm = (IIAPI_GETCOLPARM *)parmBlock;
	SW_EVT_PARMS		*evtParms = (SW_EVT_PARMS *)closure;

	if (gcolParm->gc_genParm.gp_status == IIAPI_ST_SUCCESS)
		SW_VCH_TERM(gcolParm->gc_columnData[0].dv_value);
	else
		*evtParms->evtText->text = '\0';
	free(evtParms);
}


/*{
** Name:	IIsw_raiseEvent - raise a dbevent
**
** Description:
**	Issue an IIapi_catchEvent followed by an IIapi_getEvent.
**
** Inputs:
**	connHandle	- connection handle
**	tranHandle	- transaction handle
**	evtName		- event name
**	evtText		- event text
**
** Outputs:
**	stmtHandle	- statement handle
**	errParm		- error parameter block
**
** Returns:
**	Status from call to IIsw_query.
*/
IIAPI_STATUS
IIsw_raiseEvent(
II_PTR	connHandle,
II_PTR	*tranHandle,
II_CHAR	*evtName,
II_CHAR	*evtText,
II_PTR	*stmtHandle,
IIAPI_GETEINFOPARM	*errParm)
{
	char		stmt[512];
	IIAPI_GETQINFOPARM	*getQinfoParm;

	if (evtText && *evtText != '\0')
		sprintf(stmt, "RAISE DBEVENT %s '%s'", evtName, evtText);
	else
		sprintf(stmt, "RAISE DBEVENT %s", evtName);
	return (IIsw_query(connHandle, tranHandle, stmt, 0, NULL, NULL, NULL,
		NULL, stmtHandle, &getQinfoParm, errParm));
}
