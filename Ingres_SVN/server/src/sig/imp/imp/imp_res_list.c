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

static TRIM _t00 = {
	0,
	0,
	0,
	0,
	0,
	0,
	"IMP Locking System Monitor"

};
static TRIM _t01 = {
	0,
	2,
	131072,
	0,
	0,
	0,
	"Resource List Display"

};
static FLDCOL _c0_0 = {
	"computed_id",
	0,
	0,
	"ID",
	1,
	1,
	1,
	0,
	8,
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
	11,
	0,
	8,
	1,
	8,
	-21,
	0,
	"",
	"",
	0,
	"+c8",
	0,
	0

};
static FLDCOL _c0_1 = {
	"rsb_gr_state",
	1,
	0,
	"GR",
	10,
	1,
	10,
	0,
	2,
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
	5,
	0,
	2,
	10,
	2,
	-21,
	0,
	"",
	"",
	0,
	"+c2",
	0,
	0

};
static FLDCOL _c0_2 = {
	"rsb_cv_state",
	2,
	0,
	"CV",
	13,
	1,
	13,
	0,
	2,
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
	5,
	0,
	2,
	13,
	2,
	-21,
	0,
	"",
	"",
	0,
	"c2",
	0,
	0

};
static FLDCOL _c0_3 = {
	"rsb_type",
	3,
	0,
	"Lock Type",
	16,
	1,
	16,
	0,
	15,
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
	18,
	0,
	15,
	16,
	15,
	-21,
	0,
	"",
	"",
	0,
	"c15",
	0,
	0

};
static FLDCOL _c0_4 = {
	"rsb_db",
	4,
	0,
	"DB",
	32,
	1,
	32,
	0,
	16,
	1,
	0,
	0,
	64,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	23,
	0,
	16,
	32,
	16,
	-21,
	0,
	"",
	"",
	0,
	"c16",
	0,
	0

};
static FLDCOL _c0_5 = {
	"rsb_table",
	5,
	0,
	"Table",
	49,
	1,
	49,
	0,
	16,
	1,
	0,
	0,
	64,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	25,
	0,
	16,
	49,
	16,
	-21,
	0,
	"",
	"",
	0,
	"c16",
	0,
	0

};
static FLDCOL _c0_6 = {
	"rsb_page",
	6,
	0,
	"Page",
	66,
	1,
	66,
	0,
	4,
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
	5,
	0,
	4,
	66,
	4,
	-30,
	0,
	"",
	"",
	0,
	"-i4",
	0,
	0

};
static FLDCOL _c0_7 = {
	"lkb_rsb_id_id",
	7,
	0,
	"Res",
	71,
	1,
	71,
	0,
	4,
	1,
	0,
	16777216,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	5,
	0,
	4,
	71,
	4,
	-30,
	0,
	"",
	"",
	0,
	"-i4",
	0,
	0

};
static FLDCOL * _ac00[] = {
	&_c0_0,
	&_c0_1,
	&_c0_2,
	&_c0_3,
	&_c0_4,
	&_c0_5,
	&_c0_6,
	&_c0_7

};
static TBLFLD _fC0 = {
	"lock_details",
	0,
	0,
	"",
	1,
	1,
	0,
	3,
	76,
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
	8,
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
static TRIM * _at00[] = {
	&_t00,
	&_t01

};
static IFIELD * _af00[] = {
	&_f00

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"imp_res_list",
	(FIELD **) _af00,
	1,
	(FIELD **) _an00,
	0,
	_at00,
	2,
	0,
	0,
	0,
	0,
	0,
	0,
	76,
	25,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	64,
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
GLOBALDEF	FRAME * imp_res_list = {
	&_form0

};
