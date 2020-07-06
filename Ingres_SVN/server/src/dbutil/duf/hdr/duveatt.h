# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
  struct att_tbl_ {
    i4 attrelid;
    i4 attrelidx;
    i2 attid;
    i2 attxtra;
    char attname[33];
    i4 attoff;
    i4 attfrml;
    i2 attkdom;
    i2 attflag;
    i4 attdefid1;
    i4 attdefid2;
    i2 attintl_id;
    i2 attver_added;
    i2 attver_dropped;
    i2 attval_from;
    i2 attfrmt;
    i2 attfrmp;
    i2 attver_altcol;
    i2 attcollid;
    char attfree[17];
  } att_tbl;
