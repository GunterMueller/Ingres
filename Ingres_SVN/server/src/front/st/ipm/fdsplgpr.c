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
	1,
	0,
	0,
	0,
	"3:79:0"

};
static TRIM _t01 = {
	1,
	0,
	2048,
	0,
	0,
	0,
	"Log Process Display"

};
static TRIM _t02 = {
	1,
	1,
	0,
	0,
	0,
	0,
	"ID"

};
static TRIM _t03 = {
	10,
	1,
	0,
	0,
	0,
	0,
	"PID"

};
static TRIM _t04 = {
	19,
	1,
	0,
	0,
	0,
	0,
	"Type"

};
static TRIM _t05 = {
	27,
	1,
	0,
	0,
	0,
	0,
	"OpenDB"

};
static TRIM _t06 = {
	34,
	1,
	0,
	0,
	0,
	0,
	"Write"

};
static TRIM _t07 = {
	43,
	1,
	0,
	0,
	0,
	0,
	"Force"

};
static TRIM _t08 = {
	52,
	1,
	0,
	0,
	0,
	0,
	"Wait"

};
static TRIM _t09 = {
	61,
	1,
	0,
	0,
	0,
	0,
	"Begin"

};
static TRIM _t010 = {
	70,
	1,
	0,
	0,
	0,
	0,
	"End"

};
static FLDCOL _c0_0 = {
	"pr_id",
	0,
	0,
	"ID",
	1,
	-1,
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
	10,
	0,
	8,
	1,
	8,
	21,
	0,
	"",
	"",
	0,
	"+c8",
	0,
	0

};
static FLDCOL _c0_1 = {
	"pr_pid",
	1,
	0,
	"PID",
	10,
	-1,
	10,
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
	10,
	0,
	8,
	10,
	8,
	21,
	0,
	"",
	"",
	0,
	"+c8",
	0,
	0

};
static FLDCOL _c0_2 = {
	"pr_type",
	2,
	0,
	"Type",
	19,
	-1,
	19,
	0,
	7,
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
	22,
	0,
	7,
	19,
	7,
	21,
	0,
	"",
	"",
	0,
	"c7",
	0,
	0

};
static FLDCOL _c0_3 = {
	"pr_open",
	3,
	0,
	"OpenDB",
	27,
	-1,
	27,
	0,
	6,
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
	4,
	0,
	5,
	27,
	5,
	30,
	0,
	"",
	"",
	0,
	"+i5",
	0,
	0

};
static FLDCOL _c0_4 = {
	"pr_write",
	4,
	0,
	"Write",
	34,
	-1,
	34,
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
	4,
	0,
	8,
	34,
	8,
	30,
	0,
	"",
	"",
	0,
	"-i8",
	0,
	0

};
static FLDCOL _c0_5 = {
	"pr_force",
	5,
	0,
	"Force",
	43,
	-1,
	43,
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
	"-i8",
	0,
	0

};
static FLDCOL _c0_6 = {
	"pr_wait",
	6,
	0,
	"Wait",
	52,
	-1,
	52,
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
	"-i8",
	0,
	0

};
static FLDCOL _c0_7 = {
	"pr_begin",
	7,
	0,
	"Begin",
	61,
	-1,
	61,
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
	4,
	0,
	8,
	61,
	8,
	30,
	0,
	"",
	"",
	0,
	"-i8",
	0,
	0

};
static FLDCOL _c0_8 = {
	"pr_end",
	8,
	0,
	"End",
	70,
	-1,
	70,
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
	4,
	0,
	8,
	70,
	8,
	30,
	0,
	"",
	"",
	0,
	"-i8",
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
	&_c0_7,
	&_c0_8

};
static TBLFLD _fC0 = {
	"logprocess",
	0,
	0,
	"",
	1,
	1,
	0,
	2,
	79,
	7,
	0,
	1073758241,
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
	5,
	0,
	9,
	0,
	1,
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
	&_t01,
	&_t02,
	&_t03,
	&_t04,
	&_t05,
	&_t06,
	&_t07,
	&_t08,
	&_t09,
	&_t010

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
	"fdisplgproc",
	(FIELD **) _af00,
	1,
	(FIELD **) _an00,
	0,
	_at00,
	11,
	0,
	0,
	0,
	0,
	0,
	0,
	79,
	9,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	192,
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
GLOBALDEF	FRAME * fdisplgproc = {
	&_form0

};
