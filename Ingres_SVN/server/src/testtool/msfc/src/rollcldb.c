# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  ROLL_CLEANDB    - destroy all test tables in database used for the
**			Rollforward/MSFC tests.
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
**      01-Mar-1991     Jeromef
**              Modified Multi-Server Fast Commit test to include ckpdb and
**              rollforwarddb.
**      21-Nov-1991     Jeromef
**              Add rollforward/msfc test to piccolo library
**      20-Jan-1994 (huffman)
**              Correct include files (xxcl.h) should look at the
**              common version (xx.h) for compiling.
**      17-feb-1994 (donj)
**              Get rid of "trigraph replaced warning"
**
**
NEEDLIBS = LIBINGRES
**
**
*/
#include        <compat.h>
#include        <tr.h>
#include        <pc.h>
#include        <st.h>
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
	roll_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
    dbname = argv[1];
    CVal(argv[2], &base_num);
    CVal(argv[3], &sess_num);
    if (base_num < 0 || sess_num < 0)
    {
	roll_usage();
	TRset_file(TR_T_CLOSE, 0, 0, &syserr);
	PCexit(0);
    }
/* # line 74 "rollcldb.qc" */	/* ingres */
    {
      IIingopen(0,dbname,(char *)0);
    }
/* # line 76 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "rtab_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 78 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 80 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "rtab2_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 82 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 84 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "ctab1_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 86 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 88 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab1_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 90 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 92 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab2_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 94 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 96 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab3_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 98 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 100 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab9_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 102 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 104 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab10_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 106 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 108 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab1_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 110 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 112 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab2_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 114 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 116 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab3_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 118 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 120 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab9_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 122 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 124 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab10_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 126 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 128 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab1_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 130 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 132 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab2_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 134 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 136 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab3_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 138 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 140 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab9_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 142 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 144 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab10_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 146 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 148 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab1_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 150 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 152 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab2_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 154 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 156 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab3_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 158 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 160 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab9_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 162 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 164 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab10_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 166 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 168 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab4_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 170 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 172 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab5_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 174 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 176 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab6_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 178 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 180 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab7_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 182 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 184 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab8_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 186 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 188 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab4_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 190 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 192 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab5_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 194 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 196 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab6_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 198 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 200 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab7_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 202 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 204 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab8_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 206 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 208 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab4_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 210 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 212 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab5_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 214 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 216 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab6_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 218 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 220 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab7_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 222 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 224 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab8_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 226 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 228 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab4_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 230 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 232 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab5_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 234 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 236 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab6_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 238 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 240 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab7_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 242 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 244 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab8_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 246 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 248 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab11_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 250 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 252 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab12_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 254 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 256 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab13_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 258 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 260 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "btab14_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 262 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 264 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab11_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 266 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 268 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab12_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 270 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 272 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab13_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 274 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 276 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "htab14_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 278 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 280 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab11_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 282 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 284 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab12_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 286 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 288 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab13_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 290 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 292 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "itab14_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 294 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 296 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab11_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 298 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 300 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab12_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 302 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 304 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab13_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 306 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 308 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "stab14_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 310 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 312 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "dtab1_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 314 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 316 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "xtab1_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 318 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 320 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "xtab2_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 322 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 324 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "xtab4_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 326 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 328 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "ctab2_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 330 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 332 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "ctab3_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 334 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 336 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "ctab4_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 338 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 340 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "ctab5_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 342 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 344 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "ctab6_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 346 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 348 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "ctab7_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 350 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 352 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "ctab8_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 354 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 356 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "ctab9_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 358 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 360 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "ttab1_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 362 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 364 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "mtab1_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 366 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 368 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "mtab2_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 370 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 372 "rollcldb.qc" */	/* host code */
    STprintf(table_name, "mtab3_%d_%d", base_num, sess_num);
    if ( file_exist(table_name) )
/* # line 374 "rollcldb.qc" */	/* destroy */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"destroy ");
      IIwritio(1,(short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
    }
/* # line 376 "rollcldb.qc" */	/* exit */
    {
      IIexit();
    }
/* # line 377 "rollcldb.qc" */	/* host code */
    PCexit(0);
  }
roll_usage()
{
    TRdisplay("USAGE: ROLL_TEST dbname test base session\n");
    TRdisplay("    dbname - database.\n");
    TRdisplay("    test   - test number to execute.\n");
    TRdisplay("    base   - base # of driver.\n");
    TRdisplay("    session - which slave this is.\n");
    TRdisplay("\n");
}
int file_exist(table_name)
  char table_name[32];
{
  int file_test;
/* # line 396 "rollcldb.qc" */	/* retrieve */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"rollcldb1",7801,9137);
      IIputdomio((short *)0,1,32,0,table_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"rollcldb1",7801,9137);
        IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(file_test=count(iitables.table_name where iitables.table_nam\
e=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,table_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"))");
        IIexDefine(0,(char *)"rollcldb1",7801,9137);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&file_test);
      if (IIerrtest() != 0) goto IIrtB1;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 399 "rollcldb.qc" */	/* host code */
	if (file_test != 0)
		return(1);
	else
		return(0);
}
