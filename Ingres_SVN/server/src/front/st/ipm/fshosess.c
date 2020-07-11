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
	"3:80:0"

};
static TRIM _t01 = {
	1,
	0,
	2048,
	0,
	0,
	0,
	"Session List"

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
	"Name"

};
static TRIM _t04 = {
	25,
	1,
	0,
	0,
	0,
	0,
	"TTY"

};
static TRIM _t05 = {
	32,
	1,
	0,
	0,
	0,
	0,
	"Database"

};
static TRIM _t06 = {
	44,
	1,
	0,
	0,
	0,
	0,
	"State"

};
static TRIM _t07 = {
	50,
	1,
	0,
	0,
	0,
	0,
	"Facil"

};
static TRIM _t08 = {
	56,
	1,
	0,
	0,
	0,
	0,
	"Query"

};
static FLDCOL _c0_0 = {
	"session_id",
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
	64,
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
	8,
	1,
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
static FLDCOL _c0_1 = {
	"session_name",
	1,
	0,
	"Name",
	10,
	-1,
	10,
	0,
	13,
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
	13,
	10,
	13,
	21,
	0,
	"",
	"",
	0,
	"c13",
	0,
	0

};
static FLDCOL _c0_2 = {
	"tty",
	2,
	0,
	"TTY",
	24,
	-1,
	24,
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
	24,
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
	"database",
	3,
	0,
	"Database",
	31,
	-1,
	31,
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
	31,
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
static FLDCOL _c0_4 = {
	"cs_state",
	4,
	0,
	"State",
	44,
	-1,
	44,
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
	44,
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
static FLDCOL _c0_5 = {
	"facility",
	5,
	0,
	"Fac",
	51,
	-1,
	51,
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
	6,
	0,
	3,
	51,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
	0,
	0

};
static FLDCOL _c0_6 = {
	"query",
	6,
	0,
	"Query",
	55,
	-1,
	55,
	0,
	24,
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
	27,
	0,
	24,
	55,
	24,
	-21,
	0,
	"",
	"",
	0,
	"c24",
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
	"sessdet",
	0,
	0,
	"",
	1,
	1,
	0,
	2,
	80,
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
	"fshowsess",
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
	80,
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
GLOBALDEF	FRAME * fshowsess = {
	&_form0

};