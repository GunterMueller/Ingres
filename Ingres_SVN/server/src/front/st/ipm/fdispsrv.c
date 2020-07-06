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
	"3:72:0"

};
static TRIM _t01 = {
	1,
	0,
	2048,
	0,
	0,
	0,
	"Server List"

};
static TRIM _t02 = {
	1,
	1,
	0,
	0,
	0,
	0,
	"Name"

};
static TRIM _t03 = {
	17,
	1,
	0,
	0,
	0,
	0,
	"Type"

};
static TRIM _t04 = {
	26,
	1,
	0,
	0,
	0,
	0,
	"Sessions"

};
static TRIM _t05 = {
	36,
	1,
	0,
	0,
	0,
	0,
	"Active"

};
static TRIM _t06 = {
	44,
	1,
	0,
	0,
	0,
	0,
	"Connecting to database(s)"

};
static FLDCOL _c0_0 = {
	"server",
	0,
	0,
	"Server",
	1,
	-1,
	1,
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
	26,
	0,
	15,
	1,
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
static FLDCOL _c0_1 = {
	"service",
	1,
	0,
	"Type",
	17,
	-1,
	17,
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
	17,
	8,
	21,
	0,
	"",
	"",
	0,
	"c8",
	0,
	0

};
static FLDCOL _c0_2 = {
	"ssn_cur",
	2,
	0,
	"Sessions",
	26,
	-1,
	26,
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
	26,
	8,
	30,
	0,
	"",
	"",
	0,
	"+i8",
	0,
	0

};
static FLDCOL _c0_3 = {
	"act_cur",
	3,
	0,
	"Active",
	35,
	-1,
	35,
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
	5,
	0,
	7,
	35,
	7,
	-30,
	0,
	"",
	"",
	0,
	"+i7",
	0,
	0

};
static FLDCOL _c0_4 = {
	"database",
	4,
	0,
	"Database(s)",
	43,
	-1,
	43,
	0,
	28,
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
	259,
	0,
	28,
	43,
	28,
	-21,
	0,
	"",
	"",
	0,
	"c28",
	0,
	0

};
static FLDCOL * _ac00[] = {
	&_c0_0,
	&_c0_1,
	&_c0_2,
	&_c0_3,
	&_c0_4

};
static TBLFLD _fC0 = {
	"serverlist",
	0,
	0,
	"",
	1,
	1,
	0,
	2,
	72,
	8,
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
	6,
	0,
	5,
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
	&_t06

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
	"fdispsrv",
	(FIELD **) _af00,
	1,
	(FIELD **) _an00,
	0,
	_at00,
	7,
	0,
	0,
	0,
	0,
	0,
	0,
	72,
	10,
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
GLOBALDEF	FRAME * fdispsrv = {
	&_form0

};
