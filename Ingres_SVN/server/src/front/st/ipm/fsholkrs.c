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
	"3:54:0"

};
static TRIM _t01 = {
	1,
	0,
	2048,
	0,
	0,
	0,
	"Resource Detail Display"

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
	"List ID"

};
static TRIM _t04 = {
	19,
	1,
	0,
	0,
	0,
	0,
	"Session"

};
static TRIM _t05 = {
	32,
	1,
	0,
	0,
	0,
	0,
	"RQ"

};
static TRIM _t06 = {
	36,
	1,
	0,
	0,
	0,
	0,
	"State"

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
	"llb",
	1,
	0,
	"List ID",
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
	"session_name",
	2,
	0,
	"Session",
	19,
	-1,
	19,
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
	19,
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
	"request",
	3,
	0,
	"RQ",
	32,
	-1,
	32,
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
	32,
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
static FLDCOL _c0_4 = {
	"state",
	4,
	0,
	"State",
	36,
	-1,
	36,
	0,
	17,
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
	17,
	36,
	17,
	21,
	0,
	"",
	"",
	0,
	"c17",
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
	54,
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
	"fshowlkres",
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
	54,
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
GLOBALDEF	FRAME * fshowlkres = {
	&_form0

};
