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
	"3:73:0"

};
static TRIM _t01 = {
	1,
	0,
	2048,
	0,
	0,
	0,
	"Log Database Display"

};
static TRIM _t02 = {
	1,
	1,
	0,
	0,
	0,
	0,
	"DB Name"

};
static TRIM _t03 = {
	20,
	1,
	0,
	0,
	0,
	0,
	"Status"

};
static TRIM _t04 = {
	36,
	1,
	0,
	0,
	0,
	0,
	"TX Cnt"

};
static TRIM _t05 = {
	43,
	1,
	0,
	0,
	0,
	0,
	"Begin"

};
static TRIM _t06 = {
	51,
	1,
	0,
	0,
	0,
	0,
	"End"

};
static TRIM _t07 = {
	59,
	1,
	0,
	0,
	0,
	0,
	"Read"

};
static TRIM _t08 = {
	66,
	1,
	0,
	0,
	0,
	0,
	"Write"

};
static FLDCOL _c0_0 = {
	"db_name",
	0,
	0,
	"Database",
	1,
	-1,
	1,
	0,
	18,
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
	18,
	1,
	18,
	21,
	0,
	"",
	"",
	0,
	"c18",
	0,
	0

};
static FLDCOL _c0_1 = {
	"db_s",
	1,
	0,
	"Status",
	20,
	-1,
	20,
	0,
	15,
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
	15,
	20,
	15,
	21,
	0,
	"",
	"",
	0,
	"c15",
	0,
	0

};
static FLDCOL _c0_2 = {
	"trans",
	2,
	0,
	"TX Cnt",
	36,
	-1,
	36,
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
	6,
	36,
	6,
	30,
	0,
	"",
	"",
	0,
	"+i6",
	0,
	0

};
static FLDCOL _c0_3 = {
	"begin",
	3,
	0,
	"Begin",
	43,
	-1,
	43,
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
	7,
	43,
	7,
	30,
	0,
	"",
	"",
	0,
	"+i7",
	0,
	0

};
static FLDCOL _c0_4 = {
	"end",
	4,
	0,
	"End",
	51,
	-1,
	51,
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
	7,
	51,
	7,
	30,
	0,
	"",
	"",
	0,
	"+i7",
	0,
	0

};
static FLDCOL _c0_5 = {
	"read",
	5,
	0,
	"Read",
	59,
	-1,
	59,
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
	6,
	59,
	6,
	30,
	0,
	"",
	"",
	0,
	"+i6",
	0,
	0

};
static FLDCOL _c0_6 = {
	"write",
	6,
	0,
	"Write",
	66,
	-1,
	66,
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
	6,
	66,
	6,
	30,
	0,
	"",
	"",
	0,
	"+i6",
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
	&_c0_6

};
static TBLFLD _fC0 = {
	"dblist",
	0,
	0,
	"",
	1,
	1,
	0,
	2,
	73,
	12,
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
	10,
	0,
	7,
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
	&_t08

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
	"fdisplgdb",
	(FIELD **) _af00,
	1,
	(FIELD **) _an00,
	0,
	_at00,
	9,
	0,
	0,
	0,
	0,
	0,
	0,
	73,
	14,
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
GLOBALDEF	FRAME * fdisplgdb = {
	&_form0

};
