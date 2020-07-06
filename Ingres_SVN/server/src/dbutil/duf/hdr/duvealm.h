# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
  struct alarm_tbl_ {
    char alarm_name[33];
    i4 alarm_num;
    i4 obj_id1;
    i4 obj_id2;
    char obj_name[33];
    char obj_type[2];
    short subj_type;
    char subj_name[33];
    short alarm_flags;
    i4 alarm_txtid1;
    i4 alarm_txtid2;
    i4 alarm_opctl;
    i4 alarm_opset;
    i4 event_id1;
    i4 event_id2;
    char event_text[257];
    i4 alarm_id1;
    i4 alarm_id2;
    char alarm_reserve[33];
  } alarm_tbl;
