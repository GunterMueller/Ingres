/*
** Copyright (c) 2007 Ingres Corporation
*/
#ifndef EQDEFC_H_INCLUDED
#define EQDEFC_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
void	IIbreak(void);
void	IIcsClose(char *cursor_name,int num1,int num2);
void	IIcsDaGet(int lang,void * sqd);
void	IIcsDelete(char *table_name,char *cursor_name,int num1,
		int num2);
void	IIcsERetrieve(void);
void	IIcsERplace(char *cursor_name,int num1,int num2);
void	IIcsGetio(short *indaddr,int isvar,int type,
		 int length,void * address);
void	IIcsOpen(char *cursor_name,int num1,int num2);
void	IIcsParGet(char *target_list,char **argv,
		int (*transfunc)());
void	IIcsQuery(char *cursor_name,int num1,int num2);
void	IIcsRdO(int what,char *rdo);
void	IIcsReplace(char *cursor_name,int num1,int num2);
int	IIcsRetrieve(char *cursor_name,int num1,
		int num2);
int	IIcsRetScroll(char *cursor_name, int num1,
		int num2, int fetcho, int fetchn);
void	IIeqiqio(short *indaddr,int isvar,int type,
		 int len,void * addr,char *name);
void	IIeqstio(char *name, short *indaddr,int isvar,
		 int type,int len,void * data);
int	IIerrtest(void);
void	IIexDefine(int what,char *name,int num1,
		int num2);
int	IIexExec(int what,char *name,int num1,
		int num2);
void	IIexit(void);
void	IIflush(char *file_nm,int line_no);
int	IIgetdomio(short *indaddr,int isvar,
		int hosttype,long hostlen,
		void * addr);
void	IIingopen(int lang,char *p1,char *p2,char *p3,
		char *p4,char *p5,char *p6,char *p7,char *p8,char *p9,
		char *p10,char *p11,char *p12,char *p13,char *p14,char *p15);
void	IILQcnConName(char *con_name);
void *	IILQdbl(double dblval);
void	IILQesEvStat(int escall,int eswait);
void *	IILQint(int intval);
void	IILQisInqSqlio(short *indaddr,int isvar,
		int type,int len,void * addr,
		int attrib);
void	IILQldh_LoDataHandler(int type,short *indvar, 
		int ((*datahdlr)(void *)),
		void * hdlr_arg);
long	IILQled_LoEndData(void);
void	IILQlgd_LoGetData(int flags,int hosttype, 
		long hostlen,char *addr,
		long maxlen,long *seglen, 
		int *dataend);
void	IILQlpd_LoPutData(int flags,int hosttype,
		long hostlen,char *addr,long seglen,
		int dataend);
void	IILQpriProcInit(int proc_type,char *proc_name);
int	IILQprsProcStatus(int);
void	IILQprvProcValio(char *param_name,int param_byref,
		short *indaddr,int isref,int type,
		int length,void * addr1, ...);
void	IILQprvProcGTTParm(char *param_name, char *temptab_name);
void	IILQshSetHandler(int hdlr,int (*funcptr)());
void	IILQsidSessID(int session_id);
void	IILQssSetSqlio(int attrib,short *indaddr,
		int isvar,int type,int len,
		void * data);
int	IInexec(void);
int	IInextget(void);
int	IIparret(char *tlist,char **argv,int (*xfunc)());
int	IIparset(char *string,char **argv,
		char *(*transfunc)());
void	IIputctrl(int ctrl);
void	IIputdomio(short *indaddr,int isvar,
		int type,long length,
		void * addr);
void	IIretinit(char *file_nm,int line_no);
long	(* IIseterr(int (*proc)(int *errno)))();
void	IIsexec(void);
void	IIsqConnect(int lang,char *db,char *a1,char *a2,char *a3,
		char *a4,char *a5,char *a6,char *a7,char *a8,char *a9,
		char *a10,char *a11, char *a12,char *a13);
void	IIsqDaIn(int lang, void * sqd);
void 	IIsqDescInput(int lang,char *stmt_name,void * sqd);
void 	IIsqDescribe(int lang,char *stmt_name,void * sqd,
		int using_flag);
void	IIsqDisconnect(void);
void	IIsqExImmed(char *query);
void	IIsqExStmt(char *stmt_name,int using_vars);
void	IIsqFlush(char *filename,int linenum);
void	IIsqGdInit(short sqlsttype,char *sqlstdata);
void	IIsqInit(void * sqc);
void	IIsqMods(int mod_flag);
void	IIsqParms(int flag,char * name_spec,
		int val_type,char *str_val, int int_val);
void	IIsqPrepare(int lang,char *stmt_name,void * sqd,
		int using_flag,char *query);
void	IIsqPrint(void * sqlca);
void	IIsqStop(void * sqc);
void	IIsqTPC(int highdxid,int lowdxid);
void	IIsqUser(char * usename);
void	IIsqlcdInit(void * sqc,int *sqlcd);
void	IIsyncup(char *file_nm,int line_no);
int	IItupget(void);
void	IIutsys(int uflag,char *uname,char *uval);
void	IIvarstrio(short *indnull,int isvar,
		int type,int length,void * addr);
void	IIwritio(int trim,short *ind_unused,
		int isref_unused,int type,
		int length,char *qry);
void	IIxact(int what);
int	IIactcomm(int commval,int val);
int	IIactscrl(char *tabname,int mode,int val);
int	IIaddform(void * frame);
int	IIchkfrm(void);
int	IIclrflds(void);
int	IIclrscr(void);
int	IIdispfrm(char *nm,char *md);
void	IIendforms(void);
int	IIendfrm(void);
int	IIendmu(void);
void	IIendnest(void);
int	IIfldclear(char *strvar);
int	IIfnames(void);
int	IIforminit(char *nm);
int	IIforms(int language);
int	IIFRaeAlerterEvent(int intr_val);
int	IIFRafActFld(char *strvar,int entry_act,
		int val);
void	IIFRgotofld(int dir);
void	IIFRgpcontrol(int state,int alt);
void	IIFRgpsetio(int pid,short *nullind,
		char isvar,int type,int len,
		void * val);
void	IIFRInternal(int dummy);
int	IIFRitIsTimeout(void);
int	IIFRreResEntry(void);
void	IIFRsaSetAttrio(int fsitype,char *cname, 
		short *nullind,int isvar,
		int datatype,int datalen,
		void * value);
char	IIfrshelp(int type,char *subj,char *flnm);
void	IIFRsqDescribe(int lang,int is_form,
		char *form_name,char *table_name,char *mode,
		void * sqd);
void	IIFRsqExecute(int lang,int is_form,
		int is_input,void * sqd);
int	IIFRtoact(int timeout,int intr_val);
int	IIfsetio(char *nm);
int	IIfsinqio(short *nullind,int variable,
		int type,int len,void * data,
		char *name);
int	IIfssetio(char *name,short *nullind,
		int isvar,int type,int len,
		void * data);
int	IIgetfldio(short *ind,int variable,
		int type,int len,void * data,
		char *name);
int	IIgetoper(int set);
int	IIgtqryio(short *ind,int isvar,
		int type,int len,int *variable,
		char *name);
char	IIhelpfile(char *subj,char *flnm);
char  IIhelptoggle(char flag);
int	IIinitmu(void);
int	IIinqset(char *object,char *p0,char *p1,char *p2,char *p3);
int	IIiqfsio(short *nullind,int isvar,
		int type,int length,void * data,
		int attr,char *name,int row);
int	IIiqset(int objtype,int row,
		char *formname,char *fieldname,char *term);
int	IImessage(char *buf);
int	IImuonly(void);
int	IInestmu(void);
int	IInfrskact(int frsknum,char *exp,int val,
		int act,int intrval);
int	IInmuact(char *strvar,char *exp,int val,
		int act,int intrp);
void	IIprmptio(int noecho,char *prstring,
		short *nullind,int isvar,int type,
		int length,void * data);
void	IIprnscr(char *filename);
int	IIputfldio(char *s1,short *ind,int isvar,
		int type,int len,void * data);
int	IIputoper(int set);
int	IIredisp(void);
int	IIrescol(char *tabname,char *colname);
int	IIresfld(char *strvar);
int	IIresmu(void);
int	IIresnext(void);
int	IIretval(void);
int	IIrf_param(char *infmt,char **inargv,
		int (*transfunc)());
int	IIrunform(void);
int	IIrunmu(int dispflg);
int	IIrunnest(void);
void	IIset_random(int seed);
int	IIsf_param(char *infmt,char **inargv,
		int (*transfunc)());
int	IIsleep(int sec);
int	IIstfsio(int attr,char *name,int row,
		short *nullind,int isvar,int type,
		int length,void * tdata);
int	IItbact (char *frmnm,char *tabnm,int loading);
int	IITBcaClmAct(char *tabname,char *colname,
		int entry_act,int val);
void	IITBceColEnd(void);
int	IItbinit (char *formname,char *tabname,char *tabmode);
int	IItbsetio(int cmnd,char *formname,char *tabname,
		int rownum);
int	IItbsmode(char *modestr);
int	IItclrcol(char *colname);
int	IItclrrow(void);
int	IItcogetio(short *ind,int variable,
		int type,int len,void * data,
		char *colname);
int	IItcolval(char *colname);
int	IItcoputio(char *colname,short *ind,
		int variable,int type,
		int len,void * data);
int	IItdata(void);
int	IItdatend(void);
int	IItdelrow(int tofill);
int	IItfill(void);
int	IIthidecol(char *colname,char *coltype);
int	IItinsert(void);
int	IItrc_param(char *infmt,char **inargv,
		int (*transfunc)());
int	IItscroll(int tofill,int recnum);
int	IItsc_param(char *infmt,char **inargv,
		int (*transfunc)());
int	IItunend(void);
char	IItunload(void);
int	IItvalrow(void);
int	IItvalval(int set);
int	IIvalfld(char *strvar);
int   IIG4acArrayClear(int array);
int   IIG4bpByrefParam(short *ind, int isval, 
                int type, int length, 
                void * data, char *name);
int   IIG4ccCallComp();
int   IIG4chkobj(int object, int access,
                int index,  int  caller);
int   IIG4drDelRow(int array, int  index);
int   IIG4fdFillDscr(int object, int  language, 
                void * sqd);
int   IIG4gaGetAttr(short *ind, int  isvar, 
                int  type, int  length, 
                void * data, char *name);
int   IIG4ggGetGlobal(short *ind, int  isvar, 
                int  type, int  length, 
                void * data, char *name, int  gtype);
int   IIG4grGetRow(short *rowind, int  isvar, 
                int  rowtype, int  rowlen, 
                void * rowptr, int array, int  index);
int   IIG4i4Inq4GL(short *ind, int  isvar, 
                int  type, int  length, 
                void * data, int object, int  code);
int   IIG4icInitCall(char *name, int  type);
int   IIG4irInsRow(int array, int  index, 
                int row, int  state, int  which);
int   IIG4rrRemRow(int array, int  index);
int   IIG4rvRetVal(short *ind, int  isval, 
                int  type, int  length, void * data);
int   IIG4s4Set4GL(int object, int  code, 
                short *ind, int  isvar, int  type, 
                int  length, void * data);
int   IIG4saSetAttr(char *name, short *ind, 
                int  isvar, int  type, 
                int  length, void * data);
int   IIG4seSendEvent(int frame);
int   IIG4sgSetGlobal(char *name, short *ind, 
                int  isvar, int type, int length,
                void * data);
int   IIG4srSetRow(int array, int index, 
                int row);
int   IIG4udUseDscr(int language, int direction, 
                void * *sqd);
int   IIG4vpValParam(char *name, short *ind, 
                int  isval, int type, int length,
                void * data);
#ifdef __cplusplus
}
#endif
#endif /* EQDEFC_H_INCLUDED */
