/*
** Copyright (c) 2005 Ingres Corporation
**
*/
#include    <compat.h>

/*
** Name: GCpportaddr
**
** Description:
**      Retrieve the port address by using portmap function in BS driver   
**      for the specified protocol.
**
** Inputs:
**      protname        Name of the protocol for which the port is requested.
**      addrlen         Length available in the target portaddr including the
**                      EOS terminator.
**
** Outputs:
**      portaddr        Returned port address.
**
** Returns:
**      OK              Command completed successfully.
**      FAIL
**
** History:
**      02-feb-2005 (abbjo03)
**          Created as a stub for VMS.
*/
STATUS
GCpportaddr(
char	*protname,
i4	addrlen,
char	*portaddr)
{
    *portaddr = EOS;
    return OK;
}
