# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
  struct uprof_tbl_ {
    char name[33];
    i4 status;
    char default_group[33];
    char reserve[9];
    i4 default_priv;
    char expire_date[26];
    i4 flags_mask;
  } uprof_tbl;
