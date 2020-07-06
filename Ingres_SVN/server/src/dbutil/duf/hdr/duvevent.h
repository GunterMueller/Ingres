# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
  struct evnt_tbl_ {
    char event_name[33];
    char event_owner[33];
    char event_create[26];
    i4 event_type;
    i4 event_idbase;
    i4 event_idx;
    i4 event_qryid1;
    i4 event_qryid2;
    char event_free[9];
  } evnt_tbl;
