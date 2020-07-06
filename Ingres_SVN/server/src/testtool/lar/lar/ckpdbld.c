# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
#include    <compat.h>
#include    <gl.h>
#include    <iicommon.h>
#include    <pc.h>
#include    <mh.h>
#include    <tr.h>
#include    <tm.h>
/*
**  CKPDB1_RELOAD - used for online checkpoint tests - loads database for
**	checkpoint test #1.
**
**	Tests loaded for:
**
**	    CKPDB_ABORT	 - abort tests.
**	    CKPDB_APPEND - append tests.
**	    CKPDB_DELAPP - delete/append tests.
**
**  CKPDB_RELOAD dbname [options]
**
**  Required arguments
**	dbname	        - database name
**
**  Optional arguments
**	none
**
**  History
**      13-aug-1991 (lauraw)
**              Filename changed to conform to standard
**		(ckpdb1_load.qc changed to ckpdbld.qc)
**	9-Aug-1993 (fredv)
**		Include <gl.h> and <iicommon.h> so that it will be compiled.
**	29-sep-1993 (dianeh)
**		Remove #include of <lg.h>.
**
**	19-sep-1995 (somsa01)
**		Added defined(DESKTOP) for NT, OS/2, and Netware platforms.
**  08-Sep-1997 (merja01)
**      Changed longs to i4 to make compatable with 64 bit platforms
**      such as axp_osf.
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*
** Ming hints
NEEDLIBS = LIBINGRES
PROGRAM = ckpdb1_load
**
*/
main(argc, argv)
int	argc;
char	*argv[];
{
  char *dbname;
    CL_SYS_ERR	err_code;
    i4	start_time, end_time;
#ifdef VMS
    TRset_file(TR_T_OPEN, "SYS$OUTPUT", 10, &err_code);
#endif
#if defined(UNIX) || defined(DESKTOP)
    TRset_file(TR_T_OPEN, "stdio", 5, &err_code);
#endif
    if (argc < 2)
    {
	TRdisplay("USAGE: CKPDB_RELOAD dbname\n");
	TRset_file(TR_T_CLOSE, 0, 0, &err_code);
	exit(1);
    }
    dbname = argv[1];
/* # line 83 "ckpdbld.qc" */	/* ingres */
  {
    IIingopen(0,dbname,(char *)0);
  }
/* # line 85 "ckpdbld.qc" */	/* host code */
	start_time = TMsecs();
	load_abort();
	load_append();
	load_delapp();
	end_time = TMsecs();
/* # line 93 "ckpdbld.qc" */	/* exit */
  {
    IIexit();
  }
/* # line 95 "ckpdbld.qc" */	/* host code */
    TRset_file(TR_T_CLOSE, 0, 0, &err_code);
}
/*
** LOAD_ABORT - Load data for abort tests.
**
**
*/
load_abort()
{
  i4 i;
	/*
	** Load data for abort tests - CKPDB_ABORT.
	*/
/* # line 110 "ckpdbld.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ckpdb_ab_control");
    IIsyncup((char *)0,0);
  }
/* # line 111 "ckpdbld.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ckpdb_abort1");
    IIsyncup((char *)0,0);
  }
/* # line 112 "ckpdbld.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ckpdb_abort2");
    IIsyncup((char *)0,0);
  }
/* # line 113 "ckpdbld.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ckpdb_ab1_nojnl");
    IIsyncup((char *)0,0);
  }
/* # line 114 "ckpdbld.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ckpdb_ab2_nojnl");
    IIsyncup((char *)0,0);
  }
/* # line 116 "ckpdbld.qc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"create ckpdb_ab_control(cnt=\
i4)");
    IIsyncup((char *)0,0);
  }
/* # line 117 "ckpdbld.qc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create ckpdb_abort1(a=i4, b=c100, c=c1500)");
    IIsyncup((char *)0,0);
  }
/* # line 118 "ckpdbld.qc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create ckpdb_abort2(a=i4, b=c100)");
    IIsyncup((char *)0,0);
  }
/* # line 119 "ckpdbld.qc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create ckpdb_ab1_nojnl(a=i4, b=c100, c=c1500)");
    IIsyncup((char *)0,0);
  }
/* # line 120 "ckpdbld.qc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create ckpdb_ab2_nojnl(a=i4, b=c100)");
    IIsyncup((char *)0,0);
  }
/* # line 122 "ckpdbld.qc" */	/* host code */
	for (i = 1; i <= 100; i++)
	{
/* # line 124 "ckpdbld.qc" */	/* append */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"ckpdbld1",7791,30170);
      IIputdomio((short *)0,1,30,4,&i);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"ckpdbld1",7791,30170);
        IIwritio(0,(short *)0,1,32,0,(char *)"append ckpdb_abort2(a=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ckpdbld1",7791,30170);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 125 "ckpdbld.qc" */	/* append */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"ckpdbld2",7792,31480);
      IIputdomio((short *)0,1,30,4,&i);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"ckpdbld2",7792,31480);
        IIwritio(0,(short *)0,1,32,0,(char *)"append ckpdb_abort2(a=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&i);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ckpdbld2",7792,31480);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 126 "ckpdbld.qc" */	/* host code */
	}
/* # line 128 "ckpdbld.qc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"set journaling on ckpdb_ab_control");
    IIsyncup((char *)0,0);
  }
/* # line 129 "ckpdbld.qc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"set journaling on ckpdb_abor\
t1");
    IIsyncup((char *)0,0);
  }
/* # line 130 "ckpdbld.qc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"set journaling on ckpdb_abor\
t2");
    IIsyncup((char *)0,0);
  }
/* # line 132 "ckpdbld.qc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"modify ckpdb_ab_control to h\
ash");
    IIsyncup((char *)0,0);
  }
/* # line 133 "ckpdbld.qc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ckpdb_abort1 to btree on a, b");
    IIsyncup((char *)0,0);
  }
/* # line 134 "ckpdbld.qc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"modify ckpdb_abort2 to hash \
on a");
    IIsyncup((char *)0,0);
  }
/* # line 135 "ckpdbld.qc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ckpdb_ab1_nojnl to btree on a, b");
    IIsyncup((char *)0,0);
  }
/* # line 136 "ckpdbld.qc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ckpdb_ab2_nojnl to hash on a");
    IIsyncup((char *)0,0);
  }
/* # line 137 "ckpdbld.qc" */	/* host code */
}
/*
** LOAD_APPEND - Load data for append tests.
**
**
*/
load_append()
{
	/*
	** Load data for append tests - CKPDB_APPEND.
	*/
/* # line 149 "ckpdbld.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ckpdb_ap1");
    IIsyncup((char *)0,0);
  }
/* # line 150 "ckpdbld.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ckpdb_ap2");
    IIsyncup((char *)0,0);
  }
/* # line 151 "ckpdbld.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ckpdb_ap3");
    IIsyncup((char *)0,0);
  }
/* # line 152 "ckpdbld.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ckpdb_ap4");
    IIsyncup((char *)0,0);
  }
/* # line 153 "ckpdbld.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ckpdb_ap5_nojnl");
    IIsyncup((char *)0,0);
  }
/* # line 155 "ckpdbld.qc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create ckpdb_ap1(run=i4, key=i4, sequence=i4, pad=c40)");
    IIsyncup((char *)0,0);
  }
/* # line 156 "ckpdbld.qc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create ckpdb_ap2(run=i4, key=i4, sequence=i4, pad=c40)");
    IIsyncup((char *)0,0);
  }
/* # line 157 "ckpdbld.qc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create ckpdb_ap3(run=i4, key=i4, sequence=i4, pad=c40)");
    IIsyncup((char *)0,0);
  }
/* # line 158 "ckpdbld.qc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create ckpdb_ap4(run=i4, key=i4, sequence=i4, pad=c40)");
    IIsyncup((char *)0,0);
  }
/* # line 159 "ckpdbld.qc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create ckpdb_ap5_nojnl(run=i4, key=i4, sequence=i4, pad=c40)");
    IIsyncup((char *)0,0);
  }
/* # line 161 "ckpdbld.qc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"set journaling on ckpdb_ap1");
    IIsyncup((char *)0,0);
  }
/* # line 162 "ckpdbld.qc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"set journaling on ckpdb_ap2");
    IIsyncup((char *)0,0);
  }
/* # line 163 "ckpdbld.qc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"set journaling on ckpdb_ap3");
    IIsyncup((char *)0,0);
  }
/* # line 164 "ckpdbld.qc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"set journaling on ckpdb_ap4");
    IIsyncup((char *)0,0);
  }
/* # line 166 "ckpdbld.qc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ckpdb_ap2 to hash on run, key");
    IIsyncup((char *)0,0);
  }
/* # line 167 "ckpdbld.qc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ckpdb_ap3 to isam on run, key");
    IIsyncup((char *)0,0);
  }
/* # line 168 "ckpdbld.qc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ckpdb_ap4 to btree on run, key, sequence");
    IIsyncup((char *)0,0);
  }
/* # line 169 "ckpdbld.qc" */	/* index */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"index on ckpdb_ap4=ap4_ind(key)with structure=hash");
    IIsyncup((char *)0,0);
  }
/* # line 170 "ckpdbld.qc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ckpdb_ap5_nojnl to hash on run");
    IIsyncup((char *)0,0);
  }
/* # line 171 "ckpdbld.qc" */	/* index */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"index on ckpdb_ap5_nojnl=ap5_ind(key)with structure=btree");
    IIsyncup((char *)0,0);
  }
/* # line 172 "ckpdbld.qc" */	/* host code */
}
/*
** LOAD_DELAPP - Load data for delete/append tests.
**
**
*/
load_delapp()
{
  i4 j;
	/*
	** Load data for append tests - CKPDB_APPEND.
	*/
/* # line 186 "ckpdbld.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ckpdb_btree");
    IIsyncup((char *)0,0);
  }
/* # line 187 "ckpdbld.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ckpdb_hash");
    IIsyncup((char *)0,0);
  }
/* # line 188 "ckpdbld.qc" */	/* destroy */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"destroy ckpdb_btree_nojnl");
    IIsyncup((char *)0,0);
  }
/* # line 190 "ckpdbld.qc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create ckpdb_btree(value=i4, pad1=c16, pad2=c20)");
    IIsyncup((char *)0,0);
  }
/* # line 191 "ckpdbld.qc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create ckpdb_hash(value=i4, pad1=c16, pad2=c20)");
    IIsyncup((char *)0,0);
  }
/* # line 192 "ckpdbld.qc" */	/* create */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"create ckpdb_btree_nojnl(value=i4, pad1=c16, pad2=c20)");
    IIsyncup((char *)0,0);
  }
/* # line 194 "ckpdbld.qc" */	/* host code */
	for (j = 1; j <= 1000; j++)
	{
/* # line 196 "ckpdbld.qc" */	/* append */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"ckpdbld3",7793,30459);
      IIputdomio((short *)0,1,30,4,&j);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"ckpdbld3",7793,30459);
        IIwritio(0,(short *)0,1,32,0,(char *)"append ckpdb_btree(value\
=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&j);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ckpdbld3",7793,30459);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 197 "ckpdbld.qc" */	/* append */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"ckpdbld4",7794,19354);
      IIputdomio((short *)0,1,30,4,&j);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"ckpdbld4",7794,19354);
        IIwritio(0,(short *)0,1,32,0,(char *)"append ckpdb_hash(value=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&j);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ckpdbld4",7794,19354);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 199 "ckpdbld.qc" */	/* append */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIexExec(1,(char *)"ckpdbld5",7795,18648);
      IIputdomio((short *)0,1,30,4,&j);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIexDefine(1,(char *)"ckpdbld5",7795,18648);
        IIwritio(0,(short *)0,1,32,0,(char *)
"append ckpdb_btree_nojnl(value=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&j);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ckpdbld5",7795,18648);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 200 "ckpdbld.qc" */	/* host code */
	}
/* # line 202 "ckpdbld.qc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"set journaling on ckpdb_btre\
e");
    IIsyncup((char *)0,0);
  }
/* # line 203 "ckpdbld.qc" */	/* set */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"set journaling on ckpdb_hash");
    IIsyncup((char *)0,0);
  }
/* # line 205 "ckpdbld.qc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ckpdb_btree to btree on value, pad1");
    IIsyncup((char *)0,0);
  }
/* # line 206 "ckpdbld.qc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ckpdb_hash to hash on value");
    IIsyncup((char *)0,0);
  }
/* # line 207 "ckpdbld.qc" */	/* modify */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ckpdb_btree_nojnl to btree on value");
    IIsyncup((char *)0,0);
  }
/* # line 208 "ckpdbld.qc" */	/* host code */
}
