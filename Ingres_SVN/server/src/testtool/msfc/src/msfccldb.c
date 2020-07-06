# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  MSFC_CLEANDB    - destroy all test tables in database used for MSFC tests.
**
**	This program cleans out a database used for fast commit tests so that
**	disk space will not be wasted following a successful run of the tests.
**
**  Inputs:
**	dbname	    - database to use
**	base_num    - test driver identifier
**	sess_num    - session #
**
**      History:
**
**      DD-MMM-YYYY     Unknown
**              Created Multi-Server Fast Commit test.
**      21-Nov-1991     Jeromef
**              Add msfc test to piccolo library
**      20-Jan-1994 (huffman)
**              Correct include files (xxcl.h) should look at the
**              common version (xx.h) for compiling.
**      17-feb-1994 (donj)
**              Get rid of "trigraph replaced warning"
**
**
NEEDLIBS = LIBINGRES
**
*/
#include	<compat.h>
#include	<tr.h>
#include	<pc.h>
#include	<st.h>
main(argc, argv)
int	    argc;
char	    *argv[];
  {
    char *dbname;
    int base_num = -1;
    int sess_num = -1;
    char table_name[32];
    CL_SYS_ERR	syserr;
#ifdef VMS
    TRset_file(TR_T_OPEN, "SYS$OUTPUT", 10, &syserr);
#endif
#ifdef UNIX
    TRset_file(TR_T_OPEN, "stdio", 5, &syserr);
#endif
    if (argc < 4)
    {
	msfc_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
    dbname = argv[1];
    CVal(argv[2], &base_num);
    CVal(argv[3], &sess_num);
    if (base_num < 0 || sess_num < 0)
    {
	msfc_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
/* # line 69 "msfccldb.qc" */	/* ingres */
    {
      IIingopen(0,dbname,(char *)0);
    }
/* # line 71 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "rtab_%d_%d", base_num, sess_num);
/* # line 72 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 73 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "rtab2_%d_%d", base_num, sess_num);
/* # line 74 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 75 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "ctab1_%d_%d", base_num, sess_num);
/* # line 76 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 77 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab1_%d_%d", base_num, sess_num);
/* # line 78 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 79 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab2_%d_%d", base_num, sess_num);
/* # line 80 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 81 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab3_%d_%d", base_num, sess_num);
/* # line 82 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 83 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab9_%d_%d", base_num, sess_num);
/* # line 84 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 85 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab10_%d_%d", base_num, sess_num);
/* # line 86 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 87 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab1_%d_%d", base_num, sess_num);
/* # line 88 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 89 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab2_%d_%d", base_num, sess_num);
/* # line 90 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 91 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab3_%d_%d", base_num, sess_num);
/* # line 92 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 93 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab9_%d_%d", base_num, sess_num);
/* # line 94 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 95 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab10_%d_%d", base_num, sess_num);
/* # line 96 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 97 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab1_%d_%d", base_num, sess_num);
/* # line 98 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 99 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab2_%d_%d", base_num, sess_num);
/* # line 100 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 101 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab3_%d_%d", base_num, sess_num);
/* # line 102 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 103 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab9_%d_%d", base_num, sess_num);
/* # line 104 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 105 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab10_%d_%d", base_num, sess_num);
/* # line 106 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 107 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab1_%d_%d", base_num, sess_num);
/* # line 108 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 109 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab2_%d_%d", base_num, sess_num);
/* # line 110 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 111 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab3_%d_%d", base_num, sess_num);
/* # line 112 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 113 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab9_%d_%d", base_num, sess_num);
/* # line 114 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 115 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab10_%d_%d", base_num, sess_num);
/* # line 116 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 117 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab4_%d_%d", base_num, sess_num);
/* # line 118 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 119 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab5_%d_%d", base_num, sess_num);
/* # line 120 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 121 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab6_%d_%d", base_num, sess_num);
/* # line 122 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 123 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab7_%d_%d", base_num, sess_num);
/* # line 124 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 125 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab8_%d_%d", base_num, sess_num);
/* # line 126 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 127 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab4_%d_%d", base_num, sess_num);
/* # line 128 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 129 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab5_%d_%d", base_num, sess_num);
/* # line 130 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 131 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab6_%d_%d", base_num, sess_num);
/* # line 132 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 133 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab7_%d_%d", base_num, sess_num);
/* # line 134 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 135 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab8_%d_%d", base_num, sess_num);
/* # line 136 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 137 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab4_%d_%d", base_num, sess_num);
/* # line 138 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 139 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab5_%d_%d", base_num, sess_num);
/* # line 140 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 141 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab6_%d_%d", base_num, sess_num);
/* # line 142 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 143 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab7_%d_%d", base_num, sess_num);
/* # line 144 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 145 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab8_%d_%d", base_num, sess_num);
/* # line 146 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 147 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab4_%d_%d", base_num, sess_num);
/* # line 148 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 149 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab5_%d_%d", base_num, sess_num);
/* # line 150 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 151 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab6_%d_%d", base_num, sess_num);
/* # line 152 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 153 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab7_%d_%d", base_num, sess_num);
/* # line 154 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 155 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab8_%d_%d", base_num, sess_num);
/* # line 156 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 157 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab11_%d_%d", base_num, sess_num);
/* # line 158 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 159 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab12_%d_%d", base_num, sess_num);
/* # line 160 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 161 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab13_%d_%d", base_num, sess_num);
/* # line 162 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 163 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "btab14_%d_%d", base_num, sess_num);
/* # line 164 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 165 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab11_%d_%d", base_num, sess_num);
/* # line 166 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 167 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab12_%d_%d", base_num, sess_num);
/* # line 168 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 169 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab13_%d_%d", base_num, sess_num);
/* # line 170 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 171 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "htab14_%d_%d", base_num, sess_num);
/* # line 172 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 173 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab11_%d_%d", base_num, sess_num);
/* # line 174 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 175 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab12_%d_%d", base_num, sess_num);
/* # line 176 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 177 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab13_%d_%d", base_num, sess_num);
/* # line 178 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 179 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "itab14_%d_%d", base_num, sess_num);
/* # line 180 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 181 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab11_%d_%d", base_num, sess_num);
/* # line 182 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 183 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab12_%d_%d", base_num, sess_num);
/* # line 184 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 185 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab13_%d_%d", base_num, sess_num);
/* # line 186 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 187 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "stab14_%d_%d", base_num, sess_num);
/* # line 188 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 189 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "dtab1_%d_%d", base_num, sess_num);
/* # line 190 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 191 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "xtab1_%d_%d", base_num, sess_num);
/* # line 192 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 193 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "xtab2_%d_%d", base_num, sess_num);
/* # line 194 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 195 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "xtab3_%d_%d", base_num, sess_num);
/* # line 196 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 197 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "xtab4_%d_%d", base_num, sess_num);
/* # line 198 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 199 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "ctab2_%d_%d", base_num, sess_num);
/* # line 200 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 201 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "ctab3_%d_%d", base_num, sess_num);
/* # line 202 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 203 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "ctab4_%d_%d", base_num, sess_num);
/* # line 204 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 205 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "ctab5_%d_%d", base_num, sess_num);
/* # line 206 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 207 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "ctab6_%d_%d", base_num, sess_num);
/* # line 208 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 209 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "ctab7_%d_%d", base_num, sess_num);
/* # line 210 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 211 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "ctab8_%d_%d", base_num, sess_num);
/* # line 212 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 213 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "ctab9_%d_%d", base_num, sess_num);
/* # line 214 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 215 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "ttab1_%d_%d", base_num, sess_num);
/* # line 216 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 217 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "mtab1_%d_%d", base_num, sess_num);
/* # line 218 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 219 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "mtab2_%d_%d", base_num, sess_num);
/* # line 220 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 221 "msfccldb.qc" */	/* host code */
    STprintf(table_name, "mtab3_%d_%d", base_num, sess_num);
/* # line 222 "msfccldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 224 "msfccldb.qc" */	/* exit */
    {
      IIexit();
    }
/* # line 225 "msfccldb.qc" */	/* host code */
    PCexit(0);
  }
msfc_usage()
{
    TRdisplay("USAGE: MSFC_TEST dbname test base session\n");
    TRdisplay("    dbname - database.\n");
    TRdisplay("    test   - test number to execute.\n");
    TRdisplay("    base   - base # of driver.\n");
    TRdisplay("    session - which slave this is.\n");
    TRdisplay("\n");
}
