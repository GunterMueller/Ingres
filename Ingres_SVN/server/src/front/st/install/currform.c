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
	10,
	0,
	0,
	0,
	0,
	0,
	"- Currently Installed Components"

};
static TRIM _t01 = {
	14,
	2,
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
	"             Component",
	1,
	1,
	1,
	0,
	36,
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
	36,
	1,
	36,
	-21,
	0,
	"",
	"",
	0,
	"c36",
	0,
	0

};
static FLDCOL _c0_1 = {
	"rev_level",
	1,
	0,
	"        Version",
	38,
	1,
	38,
	0,
	25,
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
	28,
	0,
	25,
	38,
	25,
	-21,
	0,
	"",
	"",
	0,
	"c25",
	0,
	0

};
static FLDCOL _c0_2 = {
	"size",
	2,
	0,
	" Size",
	64,
	1,
	64,
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
	64,
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
static FLDCOL _c0_3 = {
	"status",
	3,
	0,
	"Ready\?",
	70,
	1,
	70,
	0,
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
	9,
	0,
	6,
	70,
	6,
	-21,
	0,
	"",
	"",
	0,
	"c6",
	0,
	0

};
static FLDCOL * _ac00[] = {
	&_c0_0,
	&_c0_1,
	&_c0_2,
	&_c0_3

};
static TBLFLD _fC0 = {
	"inst_tbl",
	0,
	0,
	"",
	1,
	1,
	2,
	3,
	77,
	19,
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
	15,
	0,
	4,
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
	"curr_inst",
	1,
	0,
	"",
	0,
	0,
	16,
	2,
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
	63,
	0,
	60,
	0,
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
static IFIELD _f01 = {
	0,
	CAST_PINT &_fC1

};
static REGFLD _fC2 = {
	"ii_system_name",
	2,
	0,
	"",
	0,
	0,
	3,
	2,
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
static IFIELD _f02 = {
	0,
	CAST_PINT &_fC2

};
static REGFLD _fC3 = {
	"ii_build_name",
	3,
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
	"currform",
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
	80,
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
GLOBALDEF	FRAME * currform = {
	&_form0

};
