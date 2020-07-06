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
	"4:75:0"

};
static TRIM _t01 = {
	1,
	0,
	2048,
	0,
	0,
	0,
	"Lock List Display"

};
static TRIM _t02 = {
	1,
	2,
	0,
	0,
	0,
	0,
	"ID"

};
static TRIM _t03 = {
	10,
	2,
	0,
	0,
	0,
	0,
	"Session"

};
static TRIM _t04 = {
	23,
	1,
	1,
	0,
	0,
	0,
	"1:19:1"

};
static TRIM _t05 = {
	23,
	2,
	0,
	0,
	0,
	0,
	"Count"

};
static TRIM _t06 = {
	29,
	2,
	0,
	0,
	0,
	0,
	"Logical"

};
static TRIM _t07 = {
	30,
	1,
	0,
	0,
	0,
	0,
	"Locks"

};
static TRIM _t08 = {
	37,
	2,
	0,
	0,
	0,
	0,
	"MaxL"

};
static TRIM _t09 = {
	43,
	2,
	0,
	0,
	0,
	0,
	"Status"

};
static FLDCOL _c0_0 = {
	"id",
	0,
	0,
	"Id",
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
	"session_name",
	1,
	0,
	"Session Name",
	10,
	-1,
	10,
	0,
	12,
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
	34,
	0,
	12,
	10,
	12,
	21,
	0,
	"",
	"",
	0,
	"c12",
	0,
	0

};
static FLDCOL _c0_2 = {
	"llb_lkb_count",
	2,
	0,
	"LKNum",
	23,
	-1,
	23,
	0,
	5,
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
	23,
	5,
	30,
	0,
	"",
	"",
	0,
	"-i5",
	0,
	0

};
static FLDCOL _c0_3 = {
	"llb_llkb_count",
	3,
	0,
	"LK  Lgc",
	29,
	-1,
	29,
	0,
	7,
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
	29,
	5,
	30,
	0,
	"",
	"",
	0,
	"-i5",
	0,
	0

};
static FLDCOL _c0_4 = {
	"llb_max_lkb",
	4,
	0,
	"LKMax",
	37,
	-1,
	37,
	0,
	5,
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
	37,
	5,
	30,
	0,
	"",
	"",
	0,
	"-i5",
	0,
	0

};
static FLDCOL _c0_5 = {
	"liststat",
	5,
	0,
	"Status",
	43,
	-1,
	43,
	0,
	31,
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
	42,
	0,
	31,
	43,
	31,
	21,
	0,
	"",
	"",
	0,
	"c31",
	0,
	0

};
static FLDCOL * _ac00[] = {
	&_c0_0,
	&_c0_1,
	&_c0_2,
	&_c0_3,
	&_c0_4,
	&_c0_5

};
static TBLFLD _fC0 = {
	"locklists",
	0,
	0,
	"",
	1,
	1,
	0,
	3,
	75,
	17,
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
	15,
	0,
	6,
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
	&_t09

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
	"fdisplklists",
	(FIELD **) _af00,
	1,
	(FIELD **) _an00,
	0,
	_at00,
	10,
	0,
	0,
	0,
	0,
	0,
	0,
	75,
	20,
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
GLOBALDEF	FRAME * fdisplklists = {
	&_form0

};
