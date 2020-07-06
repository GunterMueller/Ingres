/*
** Copyright (c) 2004 Ingres Corporation
*/
# define	NULL		0
/*
** These should be changed for each installation.
** to match what is in compat.h
*/
/*
# define	i2		short
# define	i4		long
# define	f8		double
# define	i4		int
*/

/*
** FDESC
*/
typedef struct
{
	char	*fdsc_name;	/* name of the field or column */
	char	*fdsc_tblname;	/* name of the table field (if any) */
	char	*fdsc_ptr;	/* ptr to the local variable for this field */
	char	fdsc_type;	/* type--c, i or f */
	char	fdsc_visible;
	i2	fdsc_order;
	i4	fdsc_size;	/* length of the field */
} OFDESC;

typedef struct
{
	i4	fdsc_zero;	/* Always zero */
	i4	fdsc_version;	/* Version number */
	i4	fdsc_cnt;	/* Number of elements */
	OFDESC	*fdsc_ofdesc;	/* Old version of FDESC */
} FDESC;


# define	QI_BREAK	0101
# define	QI_GET		0102
# define	QI_START	0103
# define	QI_NMASTER	0104
# define	QI_OSL		0105
# define	QI_USTART	0106
# define	QI_UOSL		0107
# define	QO_QDONE	0201
# define	QO_DDONE	0202
# define	QO_QIN		0203
typedef struct qdesctype
{
	char			*qg_param;	/* The param target list */
	char			**qg_argv;	/* The argument vector */
	i4			*qg_sizelist;	/* The sizes of the values */
	char			*qg_wparam;	/* A param where clause */
	char			**qg_wargv;	/* arguments for the where */
	char			**qg_sortlist;	/* arguments for sort clause */
	char			**qg_sortdir;	/* directions for sort args */
	struct qdesctype	*qg_child;	/* A detail query */
	struct qginternal	*qg_internal;
	char			**qg_tlargv;	/* fields referenced in
						   aggregates of target list */
} QDESC;
# define	QE_NOINTERNAL	10
# define	QE_TFILE	11
# define	QE_NOSTART	12
# define	QE_MEM		13
typedef struct
{
	char	*tf_name;
	char	*tf_column;
	i4	*tf_expr;
	i4	*tf_value;
} ABRTSTFLD;
typedef struct qrytype
{
	char		**qr_qlist;	/* names of fields in the form */
	char		*qr_putparam;	/* target list for putform */
	char		**qr_argv;	/* argument vector for this param */
	char		*qr_wparam;	/* target list for where clause */
	char		**qr_wargv;	/* arg vector for where clause */
	char		*qr_form;	/* name of current form */
	char		*qr_table;	/* name of table field, if any */
	struct qrytype	*qr_child;	/* child of this QRY */
	QDESC		*qr_qdesc;	/* QDESC for this QRY */
} QRY;
typedef struct
{
	char		*pr_tlist;
	char		**pr_argv;
	ABRTSTFLD	*pr_tfld;
	QRY		*pr_qry;
} ABRTSOPRM;
typedef struct
{
	i4		abpvtype;
	char		*abpvvalue;
} OABRTSPV;
typedef struct
{
	i4		abpvtype;
	char		*abpvvalue;
	i4		abpvsize;
} ABRTSPV;
typedef struct
{
	i4		pr_zero;
	i4		pr_version;
	ABRTSOPRM	*pr_oldprm;
	ABRTSPV		*pr_actuals;
	char		**pr_formals;
	i4		pr_argcnt;
	i4		pr_rettype;
	i4		*pr_retvalue;
	i4		pr_flags;
} ABRTSPRM;

# define	ABPIGNORE	01

typedef struct
{
	char	*abprtable;
	char	*abprfld;
	char	*abprexpr;
	i4	abprtype;
} ABRTSPR;

typedef union
{
	i4	OL_int;
	f8	OL_float;
	char	*OL_string;
} OL_RET;

double	MHpow();
char	*abstrsave();
