# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/* 
** Copyright (c) 2004 Ingres Corporation
*/
# include       <compat.h>
# include       <raat.h>
/*
** Name: IIraat_tx_abort - Abort raat transaction.
**
** Description:
**      Use embedded QUEL to abort transaction
**
** Inputs:
**	raat_cb		RAAT control block
**
** Outputs:
**      None.
**
** Returns:
**	STATUS		OK or FAIL
**
** History:
**      7-jul-1995 (shust01)
**        Created.
**	16-jul-1996 (sweeney)
**	    Add tracing.
**	08-sep-1997 (somsa01)
**	    Since we are aborting the transaction, we need to also mark that
**	    the table is closed.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
STATUS
IIraat_tx_abort (RAAT_CB    *raat_cb)
{
  i4 errno;
    /* log any trace information */
    IIraat_trace(RAAT_TX_ABORT, raat_cb);
    /*
    ** Start Session
    */
/* # line 48 "raatxabort.qc" */	/* abort */
  {
    IIxact(2);
  }
/* # line 49 "raatxabort.qc" */	/* inquire_ingres */
  {
    IIeqiqio((short *)0,1,30,4,&errno,(char *)"dbmserror");
  }
/* # line 51 "raatxabort.qc" */	/* host code */
    if (errno)
    {
        raat_cb->err_code = errno;
        return (FAIL);
    }
    if (raat_cb->table_handle)
	raat_cb->table_handle->table_status &= ~(TABLE_OPEN);
    return (OK);
}
