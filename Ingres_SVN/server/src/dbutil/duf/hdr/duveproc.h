# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
  struct proc_tbl_ {
    char dbp_name[33];
    char dbp_owner[33];
    i4 dbp_txtlen;
    i4 dbp_txtid1;
    i4 dbp_txtid2;
    i4 dbp_mask1;
    i4 dbp_mask2;
    i4 dbp_id;
    i4 dbp_idx;
    i4 dbp_parameter_count;
    i4 dbp_record_width;
    i4 dbp_ubt_id;
    i4 dbp_ubt_idx;
    i4 dbp_create_date;
    char dbp_free[17];
  } proc_tbl;
