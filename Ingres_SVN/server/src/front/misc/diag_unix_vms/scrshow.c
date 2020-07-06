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
	21,
	0,
	0,
	0,
	0,
	0,
	"INGRES EVIDENCE SET MANAGEMENT UTILITY"

};
static REGFLD _fC0 = {
	"title",
	0,
	0,
	"Title:",
	0,
	0,
	1,
	2,
	67,
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
	61,
	0,
	60,
	7,
	60,
	-20,
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
static IFIELD _f00 = {
	0,
	CAST_PINT &_fC0

};
static FLDCOL _c1_0 = {
	"data",
	0,
	0,
	"data",
	1,
	-1,
	1,
	0,
	78,
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
	81,
	0,
	78,
	1,
	78,
	-21,
	0,
	"",
	"",
	0,
	"c78",
	0,
	0

};
static FLDCOL * _ac10[] = {
	&_c1_0

};
static TBLFLD _fC1 = {
	"data",
	1,
	0,
	"",
	1,
	1,
	0,
	3,
	80,
	20,
	0,
	16417,
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
	18,
	0,
	1,
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
	&_t00

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
	"show_screen",
	(FIELD **) _af00,
	2,
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
GLOBALDEF	FRAME * show_screen = {
	&_form0

};
