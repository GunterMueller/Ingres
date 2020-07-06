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
	"8:52:0"

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
static TRIM _t02 = {
	1,
	1,
	1,
	0,
	0,
	0,
	"5:18:1"

};
static TRIM _t03 = {
	2,
	1,
	0,
	0,
	0,
	0,
	"Page in log file"

};
static TRIM _t04 = {
	20,
	1,
	1,
	0,
	0,
	0,
	"4:31:2"

};
static TRIM _t05 = {
	21,
	1,
	0,
	0,
	0,
	0,
	"Logging Statistics"

};
static REGFLD _fC0 = {
	"tr_first",
	0,
	0,
	"First:",
	0,
	0,
	2,
	2,
	16,
	1,
	0,
	256,
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
	8,
	8,
	30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"write",
	1,
	0,
	"Write:",
	0,
	0,
	21,
	2,
	13,
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
	6,
	7,
	6,
	30,
	0,
	"",
	"",
	0,
	"+i6",
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
	"split",
	2,
	0,
	"Split:",
	0,
	0,
	37,
	2,
	13,
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
	6,
	7,
	6,
	30,
	0,
	"",
	"",
	0,
	"+i6",
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
	"tr_last",
	3,
	0,
	"Last:",
	0,
	0,
	2,
	3,
	16,
	1,
	0,
	256,
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
	8,
	8,
	30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"force",
	4,
	0,
	"Force:",
	0,
	0,
	21,
	3,
	13,
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
	6,
	7,
	6,
	30,
	0,
	"",
	"",
	0,
	"+i6",
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
	"wait",
	5,
	0,
	"Wait:",
	0,
	0,
	37,
	3,
	13,
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
	6,
	7,
	6,
	30,
	0,
	"",
	"",
	0,
	"+i6",
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
	"tr_cp",
	6,
	0,
	"CP:",
	0,
	0,
	2,
	4,
	16,
	1,
	0,
	256,
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
	8,
	8,
	30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"tr_pid",
	7,
	0,
	"Process ID:",
	0,
	0,
	21,
	5,
	20,
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
	10,
	0,
	8,
	12,
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
static IFIELD _f07 = {
	0,
	CAST_PINT &_fC7

};
static REGFLD _fC8 = {
	"tr_status",
	8,
	0,
	"Status:",
	0,
	0,
	1,
	6,
	50,
	1,
	0,
	256,
	576,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	72,
	0,
	42,
	8,
	42,
	21,
	0,
	"",
	"",
	0,
	"c42",
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
static TRIM * _at00[] = {
	&_t00,
	&_t01,
	&_t02,
	&_t03,
	&_t04,
	&_t05

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
	&_f08

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"fmorelghdtx",
	(FIELD **) _af00,
	9,
	(FIELD **) _an00,
	0,
	_at00,
	6,
	0,
	0,
	0,
	0,
	0,
	0,
	52,
	8,
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
GLOBALDEF	FRAME * fmorelghdtx = {
	&_form0

};
