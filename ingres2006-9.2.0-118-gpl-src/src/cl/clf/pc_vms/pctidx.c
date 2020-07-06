/*
** Copyright (c) 2007 Ingres Corporation
*/
# include       <compat.h>
# include       <pc.h>
 
/*
** Name: PCtidx
**
** Description:
**      This function is merely a wrapper for PCtid() on OpenVMS.
**
**  History:
**      05-mar-2007 (abbjo03)
**          Created.
**
*/
u_i4
PCtidx(void)
{
    /* Use PCtid() itself */
    return PCtid();
}
