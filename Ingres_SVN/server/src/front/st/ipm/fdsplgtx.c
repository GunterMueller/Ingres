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
	"3:75:0"

};
static TRIM _t01 = {
	1,
	0,
	2048,
	0,
	0,
	0,
	"Log Transaction Display"

};
static TRIM _t02 = {
	1,
	1,
	0,
	0,
	0,
	0,
	"External TX ID"

};
static TRIM _t03 = {
	18,
	1,
	0,
	0,
	0,
	0,
	"Database"

};
static TRIM _t04 = {
	29,
	1,
	0,
	0,
	0,
	0,
	"Session"

};
static TRIM _t05 = {
	42,
	1,
	0,
	0,
	0,
	0,
	"Status"

};
static TRIM _t06 = {
	61,
	1,
	0,
	0,
	0,
	0,
	"Write"

};
static TRIM _t07 = {
	68,
	1,
	0,
	0,
	0,
	0,
	"Split"

};
static FLDCOL _c0_0 = {
	"tr_eid",
	0,
	0,
	"External TX ID",
	1,
	-1,
	1,
	0,
	16,
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
	16,
	1,
	16,
	21,
	0,
	"",
	"",
	0,
	"+c16",
	0,
	0

};
static FLDCOL _c0_1 = {
	"db_name",
	1,
	0,
	"DB Name",
	18,
	-1,
	18,
	0,
	10,
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
	10,
	18,
	10,
	21,
	0,
	"",
	"",
	0,
	"c10",
	0,
	0

};
static FLDCOL _c0_2 = {
	"tr_sessname",
	2,
	0,
	"Sess Name",
	29,
	-1,
	29,
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
	26,
	0,
	12,
	29,
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
static FLDCOL _c0_3 = {
	"tr_status",
	3,
	0,
	"Status",
	42,
	-1,
	42,
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
	52,
	0,
	18,
	42,
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
static FLDCOL _c0_4 = {
	"write",
	4,
	0,
	"Write",
	61,
	-1,
	61,
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
	61,
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
static FLDCOL _c0_5 = {
	"split",
	5,
	0,
	"Split",
	68,
	-1,
	68,
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
	68,
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
	&_c0_5

};
static TBLFLD _fC0 = {
	"txlist",
	0,
	0,
	"",
	1,
	1,
	0,
	2,
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
	&_t07

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
	"fdisplgtx",
	(FIELD **) _af00,
	1,
	(FIELD **) _an00,
	0,
	_at00,
	8,
	0,
	0,
	0,
	0,
	0,
	0,
	75,
	19,
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
GLOBALDEF	FRAME * fdisplgtx = {
	&_form0

};
