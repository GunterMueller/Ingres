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
	"5:37:0"

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
	"value",
	0,
	0,
	"Value:",
	0,
	0,
	11,
	3,
	25,
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
	20,
	0,
	18,
	7,
	18,
	21,
	0,
	"",
	"",
	0,
	"c18",
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
	"invalid",
	1,
	0,
	"Invalid:",
	0,
	0,
	9,
	2,
	19,
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
	"<no>",
	12,
	0,
	10,
	9,
	10,
	21,
	0,
	"",
	"",
	0,
	"c10",
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
	"rsb_count",
	2,
	0,
	"Number of locks:",
	0,
	0,
	1,
	1,
	22,
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
	17,
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
static IFIELD _f02 = {
	0,
	CAST_PINT &_fC2

};
static TRIM * _at00[] = {
	&_t00,
	&_t01

};
static IFIELD * _af00[] = {
	&_f00,
	&_f01,
	&_f02

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"fmorelkres",
	(FIELD **) _af00,
	3,
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
	37,
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
GLOBALDEF	FRAME * fmorelkres = {
	&_form0

};
