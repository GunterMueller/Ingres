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
	"16:39:0"

};
static TRIM _t01 = {
	1,
	0,
	2048,
	0,
	0,
	0,
	"Server Detail"

};
static TRIM _t02 = {
	4,
	1,
	1,
	0,
	0,
	0,
	"6:26:1"

};
static TRIM _t03 = {
	4,
	3,
	1,
	0,
	0,
	0,
	"1:26:4"

};
static TRIM _t04 = {
	5,
	2,
	0,
	0,
	0,
	0,
	"Sessions"

};
static TRIM _t05 = {
	5,
	4,
	0,
	0,
	0,
	0,
	"Total"

};
static TRIM _t06 = {
	5,
	5,
	0,
	0,
	0,
	0,
	"Active"

};
static TRIM _t07 = {
	14,
	1,
	1,
	0,
	0,
	0,
	"6:1:2"

};
static TRIM _t08 = {
	15,
	2,
	0,
	0,
	0,
	0,
	"Current"

};
static TRIM _t09 = {
	22,
	1,
	1,
	0,
	0,
	0,
	"6:1:3"

};
static TRIM _t010 = {
	25,
	2,
	0,
	0,
	0,
	0,
	"Max"

};
static REGFLD _fC0 = {
	"ssn_cur",
	0,
	0,
	"",
	6,
	0,
	16,
	4,
	6,
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
	0,
	6,
	30,
	0,
	"",
	"",
	0,
	"-i6",
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
	"ssn_max",
	1,
	0,
	"",
	5,
	0,
	24,
	4,
	5,
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
	5,
	0,
	5,
	30,
	0,
	"",
	"",
	0,
	"-i5",
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
	"act_cur",
	2,
	0,
	"",
	6,
	0,
	16,
	5,
	6,
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
	0,
	6,
	30,
	0,
	"",
	"",
	0,
	"-i6",
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
	"act_max",
	3,
	0,
	"",
	5,
	0,
	24,
	5,
	5,
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
	5,
	0,
	5,
	30,
	0,
	"",
	"",
	0,
	"-i5",
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
	"name",
	4,
	0,
	"Name:",
	0,
	0,
	1,
	7,
	36,
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
	33,
	0,
	30,
	6,
	30,
	-21,
	0,
	"",
	"",
	0,
	"c30",
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
	"type",
	5,
	0,
	"Type:",
	0,
	0,
	1,
	8,
	36,
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
	33,
	0,
	30,
	6,
	30,
	-21,
	0,
	"",
	"",
	0,
	"c30",
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
	"q_cur_max",
	6,
	0,
	"Idle Quantums:",
	0,
	0,
	1,
	9,
	36,
	1,
	0,
	16777216,
	512,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	24,
	0,
	21,
	15,
	21,
	-21,
	0,
	"",
	"",
	0,
	"-c21",
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
	"connecting",
	7,
	0,
	"Connecting to:",
	0,
	0,
	1,
	11,
	36,
	4,
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
	93,
	0,
	90,
	6,
	30,
	-21,
	0,
	"",
	"",
	0,
	"c90.30",
	0,
	0,
	0,
	0,
	0,
	0,
	1

};
static IFIELD _f07 = {
	0,
	CAST_PINT &_fC7

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
	&_f00,
	&_f01,
	&_f02,
	&_f03,
	&_f04,
	&_f05,
	&_f06,
	&_f07

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"fservdetail",
	(FIELD **) _af00,
	8,
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
	39,
	16,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	128,
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
GLOBALDEF	FRAME * fservdetail = {
	&_form0

};
