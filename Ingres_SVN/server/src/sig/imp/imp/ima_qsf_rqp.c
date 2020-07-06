# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ft.h>
# include	<fmt.h>
# include	<adf.h>
# include	<frame.h>

typedef struct ifldstr
{
	i4	fltag;
	int	*fld;
} IFIELD;

# define	CAST_PINT	(int *)

static FLDCOL _c0_0 = {
	"rqp_name",
	0,
	0,
	"Name",
	1,
	1,
	1,
	0,
	30,
	1,
	0,
	65552,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	32,
	0,
	30,
	1,
	30,
	21,
	0,
	"",
	"",
	0,
	"c30",
	0,
	0

};
static FLDCOL _c0_1 = {
	"rqp_dbname",
	1,
	0,
	"DB",
	32,
	1,
	32,
	0,
	10,
	1,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	13,
	0,
	10,
	32,
	10,
	-21,
	0,
	"",
	"",
	0,
	"c10",
	0,
	0

};
static FLDCOL _c0_2 = {
	"rqp_size",
	2,
	0,
	"Size",
	43,
	1,
	43,
	0,
	8,
	1,
	0,
	65552,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	4,
	0,
	8,
	43,
	8,
	30,
	0,
	"",
	"",
	0,
	"+f8",
	0,
	0

};
static FLDCOL _c0_3 = {
	"rqp_usage_count",
	3,
	0,
	"Usage",
	52,
	1,
	52,
	0,
	8,
	1,
	0,
	65552,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	4,
	0,
	8,
	52,
	8,
	30,
	0,
	"",
	"",
	0,
	"+f8",
	0,
	0

};
static FLDCOL * _ac00[] = {
	&_c0_0,
	&_c0_1,
	&_c0_2,
	&_c0_3

};
static TBLFLD _fC0 = {
	"ima_qsf_rqp",
	0,
	0,
	"",
	0,
	0,
	0,
	2,
	61,
	20,
	0,
	1073741857,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	16,
	0,
	4,
	0,
	3,
	1,
	0,
	_ac00,
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f00 = {
	1,
	CAST_PINT &_fC0

};
static REGFLD _fC1 = {
	"server",
	1,
	0,
	"Repeat Queries in QSF for Server:",
	0,
	0,
	0,
	0,
	64,
	1,
	0,
	0,
	512,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	33,
	0,
	30,
	34,
	30,
	-21,
	0,
	"",
	"",
	0,
	"c30",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f01 = {
	0,
	CAST_PINT &_fC1

};
static TRIM * _at00[] = {
	0

};
static IFIELD * _af00[] = {
	&_f00,
	&_f01

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"ima_qsf_rqp",
	(FIELD **) _af00,
	2,
	(FIELD **) _an00,
	0,
	_at00,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	185,
	22,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
GLOBALDEF	FRAME * ima_qsf_rqp = {
	&_form0

};
