# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
  struct priv_tbl_ {
    char dbname[33];
    char grantee[33];
    short gtype;
    short dbflags;
    i4 control;
    i4 flags;
    i4 qrow_limit;
    i4 qdio_limit;
    i4 qcpu_limit;
    i4 qpage_limit;
    i4 qcost_limit;
    i4 idle_time_limit;
    i4 connect_time_limit;
    i4 priority_limit;
    char reserve[33];
  } priv_tbl;
