# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
  struct db_tbl_ {
    char name[33];
    char own[33];
    char dbdev[33];
    char ckpdev[33];
    char jnldev[33];
    char sortdev[33];
    i4 access;
    i4 dbservice;
    char dbcmptlvl[5];
    i4 dbcmptminor;
    i4 db_id;
    char dmpdev[33];
    char dbfree[9];
  } db_tbl;
