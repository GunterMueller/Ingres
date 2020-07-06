# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
  struct cdbs_tbl_ {
    char ddb_name[33];
    char ddb_owner[33];
    char cdb_name[33];
    char cdb_owner[33];
    char cdb_node[33];
    char cdb_dbms[33];
    char scheme_desc[33];
    char create_date[26];
    char original[9];
    i4 cdb_id;
    i4 cdb_capability;
  } cdbs_tbl;
