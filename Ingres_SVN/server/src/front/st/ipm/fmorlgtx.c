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
	2049,
	0,
	0,
	0,
	"10:77:0"

};
static TRIM _t01 = {
	1,
	0,
	2048,
	0,
	0,
	0,
	"More Info"

};
static REGFLD _fC0 = {
	"tr_id",
	0,
	0,
	"Internal TX ID:",
	0,
	0,
	1,
	1,
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
	10,
	0,
	8,
	16,
	8,
	21,
	0,
	"",
	"",
	0,
	"c8",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f00 = {
	0,
	CAST_PINT &_fC0

};
static REGFLD _fC1 = {
	"tr_sess_id",
	1,
	0,
	"Session ID:",
	0,
	0,
	5,
	2,
	28,
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
	12,
	16,
	21,
	0,
	"",
	"",
	0,
	"c16",
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
static REGFLD _fC2 = {
	"tr_db_id",
	2,
	0,
	"Database ID:",
	0,
	0,
	4,
	3,
	21,
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
	13,
	8,
	21,
	0,
	"",
	"",
	0,
	"c8",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f02 = {
	0,
	CAST_PINT &_fC2

};
static REGFLD _fC3 = {
	"tr_pr_id",
	3,
	0,
	"Internal PID:",
	0,
	0,
	54,
	3,
	22,
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
	14,
	8,
	21,
	0,
	"",
	"",
	0,
	"c8",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f03 = {
	0,
	CAST_PINT &_fC3

};
static REGFLD _fC4 = {
	"tr_pid",
	4,
	0,
	"External PID:",
	0,
	0,
	54,
	2,
	22,
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
	14,
	8,
	21,
	0,
	"",
	"",
	0,
	"c8",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f04 = {
	0,
	CAST_PINT &_fC4

};
static REGFLD _fC5 = {
	"tr_first",
	5,
	0,
	"First:",
	0,
	0,
	1,
	4,
	34,
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
	32,
	0,
	27,
	7,
	27,
	21,
	0,
	"",
	"",
	0,
	"c27",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f05 = {
	0,
	CAST_PINT &_fC5

};
static REGFLD _fC6 = {
	"tr_last",
	6,
	0,
	"Last:",
	0,
	0,
	43,
	4,
	33,
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
	32,
	0,
	27,
	6,
	27,
	21,
	0,
	"",
	"",
	0,
	"c27",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f06 = {
	0,
	CAST_PINT &_fC6

};
static REGFLD _fC7 = {
	"tr_cp",
	7,
	0,
	"CP:",
	0,
	0,
	4,
	5,
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
	32,
	0,
	27,
	4,
	27,
	21,
	0,
	"",
	"",
	0,
	"c27",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f07 = {
	0,
	CAST_PINT &_fC7

};
static REGFLD _fC8 = {
	"tr_dis_id",
	8,
	0,
	"Dis TX ID:",
	0,
	0,
	1,
	6,
	71,
	3,
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
	152,
	0,
	180,
	11,
	60,
	21,
	0,
	"",
	"",
	0,
	"c150.60",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f08 = {
	0,
	CAST_PINT &_fC8

};
static REGFLD _fC9 = {
	"force",
	9,
	0,
	"Force:",
	0,
	0,
	35,
	1,
	15,
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
	4,
	0,
	8,
	7,
	8,
	30,
	0,
	"",
	"",
	0,
	"-i8",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f09 = {
	0,
	CAST_PINT &_fC9

};
static REGFLD _fC10 = {
	"wait",
	10,
	0,
	"Wait:",
	0,
	0,
	62,
	1,
	14,
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
	4,
	0,
	8,
	6,
	8,
	30,
	0,
	"",
	"",
	0,
	"-i8",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f010 = {
	0,
	CAST_PINT &_fC10

};
static TRIM * _at00[] = {
	&_t00,
	&_t01

};
static IFIELD * _af00[] = {
	&_f00,
	&_f01,
	&_f02,
	&_f03,
	&_f04,
	&_f05,
	&_f06,
	&_f07,
	&_f08,
	&_f09,
	&_f010

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"fmorelgtx",
	(FIELD **) _af00,
	11,
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
	77,
	10,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	192,
	537657344,
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
GLOBALDEF	FRAME * fmorelgtx = {
	&_form0

};
