/*
** Copyright (c) 1995, Ingres Corporation
*/

/*
**
**  Name: jacket.c
**
**  Description:
**      Contains jacket routines to force the use of iosb for selected system
**      services.  This works around an ALPHA VMS bug.
**
**	History:
**		27-JUN-95 (whitfield) Created.
**/

#include <compat.h>
#undef sys$getjpiw
#undef sys$getdviw
#undef sys$getsyiw
#undef sys$getlkiw
#undef sys$qiow

#undef SYS$GETJPIW
#undef SYS$GETDVIW
#undef SYS$GETSYIW
#undef SYS$GETLKIW
#undef SYS$QIOW

int iisys_getjpiw(efn,pidadr,prcnam,itmlst,iosb,astadr,astprm) {
    u_i8 dummy_iosb;

    if (iosb)
	return sys$getjpiw(efn,pidadr,prcnam,itmlst,iosb,astadr,astprm);
    else
	return sys$getjpiw(efn,pidadr,prcnam,itmlst,&dummy_iosb,astadr,
			     astprm);
}

int iisys_getdviw(efn,chan,devnam,itmlst,iosb,astadr,astprm,nullarg) {
    u_i8 dummy_iosb;

    if (iosb)
	return sys$getdviw(efn,chan,devnam,itmlst,iosb,astadr,astprm,
			     nullarg);
    else
	return sys$getdviw(efn,chan,devnam,itmlst,&dummy_iosb,
			   astadr,astprm,nullarg);
}

int iisys_getsyiw(efn,csidadr,nodename,itmlst,iosb,astadr,astprm) {
    u_i8 dummy_iosb;

    if (iosb)
	return sys$getsyiw(efn,csidadr,nodename,itmlst,iosb,astadr,astprm);
    else
	return sys$getsyiw(efn,csidadr,nodename,itmlst,&dummy_iosb,astadr,
			   astprm);
}

int iisys_getlkiw(efn,lkidadr,itmlst,iosb,astadr,astprm,nullarg) {
    u_i8 dummy_iosb;

    if (iosb)
	return sys$getlkiw(efn,lkidadr,itmlst,iosb,astadr,astprm, nullarg);
    else
	return sys$getlkiw(efn,lkidadr,itmlst,&dummy_iosb,astadr,
			   astprm, nullarg);
}

int iisys_qiow(efn,chan,func,iosb,astadr,astprm,p1,p2,p3,p4,p5,p6) {
    u_i8 dummy_iosb;

    if (iosb)
	return sys$qiow(efn,chan,func,iosb,astadr,astprm,p1,p2,p3,p4,p5,p6);
    else
	return sys$qiow(efn,chan,func,&dummy_iosb,astadr,astprm,
			p1,p2,p3,p4,p5,p6);
}

