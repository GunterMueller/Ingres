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
	"13:19:0"

};
static TRIM _t01 = {
	0,
	2,
	1,
	0,
	0,
	0,
	"1:19:1"

};
static TRIM _t02 = {
	0,
	4,
	1,
	0,
	0,
	0,
	"1:19:2"

};
static TRIM _t03 = {
	0,
	6,
	1,
	0,
	0,
	0,
	"1:19:5"

};
static TRIM _t04 = {
	0,
	8,
	1,
	0,
	0,
	0,
	"1:19:6"

};
static TRIM _t05 = {
	0,
	10,
	1,
	0,
	0,
	0,
	"1:19:8"

};
static TRIM _t06 = {
	0,
	12,
	1,
	0,
	0,
	0,
	"3:19:9"

};
static TRIM _t07 = {
	6,
	4,
	1,
	0,
	0,
	0,
	"5:1:3"

};
static TRIM _t08 = {
	9,
	8,
	1,
	0,
	0,
	0,
	"3:1:7"

};
static TRIM _t09 = {
	12,
	4,
	1,
	0,
	0,
	0,
	"5:1:4"

};
static REGFLD _fC0 = {
	"gca_field",
	0,
	0,
	"",
	0,
	0,
	8,
	1,
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
	0,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
	"scf_field",
	1,
	0,
	"",
	0,
	0,
	8,
	3,
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
	0,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
	"psf_field",
	2,
	0,
	"",
	0,
	0,
	2,
	5,
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
	0,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
	"opf_field",
	3,
	0,
	"",
	0,
	0,
	8,
	5,
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
	0,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
	"qef_field",
	4,
	0,
	"",
	0,
	0,
	14,
	5,
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
	0,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
	"rdf_field",
	5,
	0,
	"",
	0,
	0,
	2,
	7,
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
	0,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
	"qsf_field",
	6,
	0,
	"",
	0,
	0,
	8,
	7,
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
	0,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
	"adf_field",
	7,
	0,
	"",
	0,
	0,
	14,
	7,
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
	0,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
	"sxf_field",
	8,
	0,
	"",
	0,
	0,
	4,
	9,
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
	0,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
	"gwf_field",
	9,
	0,
	"",
	0,
	0,
	12,
	9,
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
	0,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
	"dmf_field",
	10,
	0,
	"",
	0,
	0,
	8,
	11,
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
	0,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
static REGFLD _fC11 = {
	"di_field",
	11,
	0,
	"",
	0,
	0,
	6,
	13,
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
	11,
	0,
	8,
	0,
	8,
	-21,
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
static IFIELD _f011 = {
	0,
	CAST_PINT &_fC11

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
	&_t09

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
	&_f010,
	&_f011

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"imp_server_diag",
	(FIELD **) _af00,
	12,
	(FIELD **) _an00,
	0,
	_at00,
	10,
	0,
	0,
	0,
	0,
	0,
	0,
	20,
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
GLOBALDEF	FRAME * imp_server_diag = {
	&_form0

};
