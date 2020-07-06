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
	"5:44:0"

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
	"rllb",
	0,
	0,
	"Related Lock List ID:",
	0,
	0,
	1,
	2,
	30,
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
	22,
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
	"rcnt",
	1,
	0,
	"Count:",
	0,
	0,
	32,
	2,
	11,
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
	4,
	7,
	4,
	30,
	0,
	"",
	"",
	0,
	"-i4",
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
	"wait",
	2,
	0,
	"Waiting Resource List ID:",
	0,
	0,
	1,
	3,
	34,
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
	26,
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
	"key",
	3,
	0,
	"Transaction ID:",
	0,
	0,
	1,
	1,
	32,
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
	16,
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
static IFIELD _f03 = {
	0,
	CAST_PINT &_fC3

};
static TRIM * _at00[] = {
	&_t00,
	&_t01

};
static IFIELD * _af00[] = {
	&_f00,
	&_f01,
	&_f02,
	&_f03

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"fmorelklists",
	(FIELD **) _af00,
	4,
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
	44,
	5,
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
GLOBALDEF	FRAME * fmorelklists = {
	&_form0

};
