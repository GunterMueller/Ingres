# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
  struct user_tbl_ {
    char name[33];
    i2 gid;
    i2 mid;
    i4 status;
    char default_group[33];
    char password[25];
    char reserve[9];
    char profile_name[33];
    i4 default_priv;
    char expire_date[26];
    i4 flags_mask;
    i4 user_priv;
  } user_tbl;
