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
	"10:70:0"

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
	4,
	2,
	1,
	0,
	0,
	0,
	"5:40:1"

};
static TRIM _t03 = {
	5,
	2,
	0,
	0,
	0,
	0,
	"Log address for"

};
static TRIM _t04 = {
	46,
	3,
	1,
	0,
	0,
	0,
	"3:21:2"

};
static TRIM _t05 = {
	47,
	3,
	0,
	0,
	0,
	0,
	"Last Transaction ID"

};
static REGFLD _fC0 = {
	"lgh_begin",
	0,
	0,
	"Begin (BOF):",
	0,
	0,
	5,
	3,
	38,
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
	25,
	13,
	25,
	21,
	0,
	"",
	"",
	0,
	"c25",
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
	"lgh_end",
	1,
	0,
	"End (EOF):",
	0,
	0,
	7,
	4,
	36,
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
	25,
	11,
	25,
	21,
	0,
	"",
	"",
	0,
	"c25",
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
	"lgh_tran_id",
	2,
	0,
	"",
	0,
	0,
	47,
	4,
	17,
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
	"c16",
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
	"lgh_cp",
	3,
	0,
	"Previous CP:",
	0,
	0,
	5,
	5,
	38,
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
	25,
	13,
	25,
	21,
	0,
	"",
	"",
	0,
	"c25",
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
	"lga",
	4,
	0,
	"Journal Window:",
	0,
	0,
	2,
	8,
	58,
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
	82,
	0,
	42,
	16,
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
static IFIELD _f04 = {
	0,
	CAST_PINT &_fC4

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
	&_f04

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"fmorelghdr",
	(FIELD **) _af00,
	5,
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
	70,
	10,
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
GLOBALDEF	FRAME * fmorelghdr = {
	&_form0

};
