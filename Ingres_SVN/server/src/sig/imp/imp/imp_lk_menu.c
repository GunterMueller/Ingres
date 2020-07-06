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
	0,
	0,
	0,
	0,
	"IMP - Locking System Menu"

};
static TRIM _t01 = {
	9,
	12,
	0,
	0,
	0,
	0,
	"Place the cursor on your choice and press \"Select\""

};
static REGFLD _fC0 = {
	"vnode",
	0,
	0,
	"Vnode:",
	0,
	0,
	56,
	0,
	27,
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
	7,
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
static IFIELD _f00 = {
	0,
	CAST_PINT &_fC0

};
static FLDCOL _c1_0 = {
	"item_number",
	0,
	0,
	"",
	1,
	-1,
	1,
	0,
	4,
	1,
	0,
	16777216,
	0,
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
	1,
	4,
	-30,
	0,
	"",
	"",
	0,
	"-i4",
	0,
	0

};
static FLDCOL _c1_1 = {
	"short_name",
	1,
	0,
	"",
	6,
	-1,
	6,
	0,
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
	18,
	0,
	15,
	6,
	15,
	-21,
	0,
	"",
	"",
	0,
	"c15",
	0,
	0

};
static FLDCOL _c1_2 = {
	"long_name",
	2,
	0,
	"",
	22,
	-1,
	22,
	0,
	45,
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
	48,
	0,
	45,
	22,
	45,
	-21,
	0,
	"",
	"",
	0,
	"c45",
	0,
	0

};
static FLDCOL * _ac10[] = {
	&_c1_0,
	&_c1_1,
	&_c1_2

};
static TBLFLD _fC1 = {
	"imp_menu",
	1,
	0,
	"",
	1,
	1,
	9,
	5,
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
	3,
	0,
	1,
	1,
	0,
	_ac10,
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f01 = {
	1,
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
	"imp_lk_menu",
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
	83,
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
GLOBALDEF	FRAME * imp_lk_menu = {
	&_form0

};
