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
	"3:74:0"

};
static TRIM _t01 = {
	1,
	0,
	2048,
	0,
	0,
	0,
	"Lock Detail Display"

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
	"RQ"

};
static TRIM _t04 = {
	14,
	1,
	0,
	0,
	0,
	0,
	"State"

};
static TRIM _t05 = {
	21,
	1,
	0,
	0,
	0,
	0,
	"Lock Type"

};
static TRIM _t06 = {
	33,
	1,
	0,
	0,
	0,
	0,
	"DB Table                              Pg"

};
static FLDCOL _c0_0 = {
	"id",
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
	"request",
	1,
	0,
	"RQ",
	10,
	-1,
	10,
	0,
	3,
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
	3,
	10,
	3,
	21,
	0,
	"",
	"",
	0,
	"+c3",
	0,
	0

};
static FLDCOL _c0_2 = {
	"state",
	2,
	0,
	"State",
	14,
	-1,
	14,
	0,
	6,
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
	6,
	14,
	6,
	21,
	0,
	"",
	"",
	0,
	"c6",
	0,
	0

};
static FLDCOL _c0_3 = {
	"lock_type",
	3,
	0,
	"Lock Type",
	21,
	-1,
	21,
	0,
	11,
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
	17,
	0,
	11,
	21,
	11,
	21,
	0,
	"",
	"",
	0,
	"c11",
	0,
	0

};
static FLDCOL _c0_4 = {
	"key",
	4,
	0,
	"Key",
	33,
	-1,
	33,
	0,
	40,
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
	102,
	0,
	40,
	33,
	40,
	21,
	0,
	"",
	"",
	0,
	"c40",
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
	"lockdetail",
	0,
	0,
	"",
	1,
	1,
	0,
	2,
	74,
	10,
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
	8,
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
	"fshowlklock",
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
	74,
	12,
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
GLOBALDEF	FRAME * fshowlklock = {
	&_form0

};
