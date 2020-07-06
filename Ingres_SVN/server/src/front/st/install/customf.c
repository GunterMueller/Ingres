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
	4,
	20,
	2048,
	0,
	0,
	0,
	"To add or remove products from the list of products to be installed,"

};
static TRIM _t01 = {
	4,
	21,
	2048,
	0,
	0,
	0,
	"enter \'y\' or \'n\' in the \'Install\?\' column."

};
static TRIM _t02 = {
	10,
	0,
	0,
	0,
	0,
	0,
	"- Custom Install from Distribution Medium"

};
static TRIM _t03 = {
	15,
	18,
	0,
	0,
	0,
	0,
	":"

};
static FLDCOL _c0_0 = {
	"package",
	0,
	0,
	"               Custom Component Options",
	1,
	1,
	1,
	0,
	54,
	1,
	0,
	0,
	576,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	131,
	0,
	54,
	1,
	54,
	-21,
	0,
	"",
	"",
	0,
	"c54",
	0,
	0

};
static FLDCOL _c0_1 = {
	"size",
	1,
	0,
	" Size",
	56,
	1,
	56,
	0,
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
	8,
	0,
	5,
	56,
	5,
	-21,
	0,
	"",
	"",
	0,
	"c5",
	0,
	0

};
static FLDCOL _c0_2 = {
	"inst_yorn",
	2,
	0,
	" Install\?",
	62,
	1,
	62,
	0,
	10,
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
	12,
	0,
	10,
	62,
	10,
	21,
	0,
	"",
	"",
	0,
	"c10",
	0,
	0

};
static FLDCOL * _ac00[] = {
	&_c0_0,
	&_c0_1,
	&_c0_2

};
static TBLFLD _fC0 = {
	"dist_tbl",
	0,
	0,
	"",
	1,
	1,
	3,
	3,
	73,
	15,
	0,
	1073741857,
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
	11,
	0,
	3,
	0,
	3,
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
static REGFLD _fC1 = {
	"installation_area",
	1,
	0,
	"",
	0,
	0,
	17,
	18,
	60,
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
	62,
	0,
	60,
	0,
	60,
	21,
	0,
	"",
	"",
	0,
	"c60",
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
	"dist_medium",
	2,
	0,
	"Distribution:",
	0,
	0,
	4,
	2,
	74,
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
	63,
	0,
	60,
	14,
	60,
	-21,
	0,
	"",
	"",
	0,
	"c60",
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
	"ii_system_name",
	3,
	0,
	"",
	0,
	0,
	4,
	18,
	10,
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
	13,
	0,
	10,
	0,
	10,
	-21,
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
static IFIELD _f03 = {
	0,
	CAST_PINT &_fC3

};
static REGFLD _fC4 = {
	"ii_build_name",
	4,
	0,
	"",
	0,
	0,
	0,
	0,
	9,
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
	12,
	0,
	9,
	0,
	9,
	-21,
	0,
	"",
	"",
	0,
	"c9",
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
	&_t03

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
	"custform",
	(FIELD **) _af00,
	5,
	(FIELD **) _an00,
	0,
	_at00,
	4,
	0,
	0,
	0,
	0,
	0,
	0,
	87,
	23,
	0,
	0,
	0,
	0,
	0,
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
	0,
	0,
	0

};
GLOBALDEF	FRAME * custform = {
	&_form0

};
