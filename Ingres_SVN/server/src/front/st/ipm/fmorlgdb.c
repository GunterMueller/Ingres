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
	"6:70:0"

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
	"db_id",
	0,
	0,
	"ID:",
	0,
	0,
	1,
	1,
	12,
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
	4,
	8,
	21,
	0,
	"",
	"",
	0,
	"+c8",
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
	"owner",
	1,
	0,
	"Owner:",
	0,
	0,
	15,
	1,
	19,
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
	7,
	12,
	21,
	0,
	"",
	"",
	0,
	"c12",
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
	"force",
	2,
	0,
	"Force:",
	0,
	0,
	36,
	1,
	15,
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
	7,
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
static IFIELD _f02 = {
	0,
	CAST_PINT &_fC2

};
static REGFLD _fC3 = {
	"wait",
	3,
	0,
	"Wait:",
	0,
	0,
	53,
	1,
	14,
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
	6,
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
	"location",
	4,
	0,
	"Location:",
	0,
	0,
	5,
	2,
	64,
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
	66,
	0,
	54,
	10,
	54,
	21,
	0,
	"",
	"",
	0,
	"c54",
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
	"lga",
	5,
	0,
	"Jnl Window:",
	0,
	0,
	3,
	3,
	66,
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
	56,
	0,
	54,
	12,
	54,
	21,
	0,
	"",
	"",
	0,
	"c54",
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
	"backup",
	6,
	0,
	"Start Backup:",
	0,
	0,
	1,
	4,
	68,
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
	56,
	0,
	54,
	14,
	54,
	21,
	0,
	"",
	"",
	0,
	"c54",
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
	&_f06

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"fmorelgdb",
	(FIELD **) _af00,
	7,
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
	70,
	6,
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
GLOBALDEF	FRAME * fmorelgdb = {
	&_form0

};
