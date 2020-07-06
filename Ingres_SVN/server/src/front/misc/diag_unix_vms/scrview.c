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
	23,
	3,
	0,
	0,
	0,
	0,
	"VERSION:"

};
static REGFLD _fC0 = {
	"evidenceset",
	0,
	0,
	"EVIDENCE SET:",
	0,
	0,
	23,
	0,
	18,
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
	5,
	0,
	4,
	14,
	4,
	-30,
	0,
	"",
	"",
	0,
	"-\"nnnn\"",
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
	"created",
	1,
	0,
	"CREATED:",
	0,
	0,
	23,
	1,
	34,
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
	23,
	0,
	20,
	14,
	20,
	-21,
	0,
	"",
	"",
	0,
	"c20",
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
	"title",
	2,
	0,
	"TITLE:",
	0,
	0,
	23,
	2,
	34,
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
	23,
	0,
	20,
	14,
	20,
	-21,
	0,
	"",
	"",
	0,
	"c20",
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
static FLDCOL _c3_0 = {
	"type",
	0,
	0,
	"Type",
	1,
	1,
	1,
	0,
	6,
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
	9,
	0,
	6,
	1,
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
static FLDCOL _c3_1 = {
	"comment",
	1,
	0,
	"Comment",
	8,
	1,
	8,
	0,
	70,
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
	73,
	0,
	70,
	8,
	70,
	-21,
	0,
	"",
	"",
	0,
	"c70",
	0,
	0

};
static FLDCOL * _ac30[] = {
	&_c3_0,
	&_c3_1

};
static TBLFLD _fC3 = {
	"data",
	3,
	0,
	"",
	1,
	1,
	1,
	4,
	79,
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
	2,
	0,
	3,
	1,
	0,
	_ac30,
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f03 = {
	1,
	CAST_PINT &_fC3

};
static REGFLD _fC4 = {
	"version",
	4,
	0,
	"",
	0,
	0,
	37,
	3,
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
	23,
	0,
	20,
	0,
	20,
	-21,
	0,
	"",
	"",
	0,
	"c20",
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
	&_t00

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
	"view_screen",
	(FIELD **) _af00,
	5,
	(FIELD **) _an00,
	0,
	_at00,
	1,
	0,
	0,
	0,
	0,
	0,
	0,
	80,
	25,
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
	0,
	0,
	0,
	0

};
GLOBALDEF	FRAME * view_screen = {
	&_form0

};
