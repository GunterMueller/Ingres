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
	2048,
	0,
	0,
	0,
	"IPM"

};
static TRIM _t01 = {
	8,
	18,
	0,
	0,
	0,
	0,
	"Place the cursor on your choice and press \"Select\""

};
static FLDCOL _c0_0 = {
	"option",
	0,
	0,
	"Option",
	1,
	-1,
	1,
	0,
	14,
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
	16,
	0,
	14,
	1,
	14,
	21,
	0,
	"",
	"",
	0,
	"c14",
	0,
	0

};
static FLDCOL _c0_1 = {
	"description",
	1,
	0,
	"Description",
	16,
	-1,
	16,
	0,
	51,
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
	54,
	0,
	51,
	16,
	51,
	-21,
	0,
	"",
	"",
	0,
	"c51",
	0,
	0

};
static FLDCOL * _ac00[] = {
	&_c0_0,
	&_c0_1

};
static TBLFLD _fC0 = {
	"options",
	0,
	0,
	"",
	1,
	1,
	8,
	8,
	68,
	5,
	0,
	1073758241,
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
	3,
	0,
	2,
	0,
	1,
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
	"autorefresh",
	1,
	0,
	"Autorefresh:",
	0,
	0,
	44,
	0,
	35,
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
	25,
	0,
	22,
	13,
	22,
	-21,
	0,
	"",
	"",
	0,
	"c22",
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
static TRIM * _at00[] = {
	&_t00,
	&_t01

};
static IFIELD * _af00[] = {
	&_f00,
	&_f01

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"fingdba",
	(FIELD **) _af00,
	2,
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
	81,
	23,
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
GLOBALDEF	FRAME * fingdba = {
	&_form0

};
