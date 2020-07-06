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

static REGFLD _fC0 = {
	"title",
	0,
	0,
	"IPM - Help on",
	0,
	0,
	3,
	0,
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
static IFIELD _f00 = {
	0,
	CAST_PINT &_fC0

};
static FLDCOL _c1_0 = {
	"disline",
	0,
	0,
	"",
	1,
	-1,
	1,
	0,
	78,
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
	"disbox",
	1,
	0,
	"",
	1,
	1,
	0,
	1,
	80,
	22,
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
	20,
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
	0

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
	"fmyhelp",
	(FIELD **) _af00,
	2,
	(FIELD **) _an00,
	0,
	_at00,
	0,
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
GLOBALDEF	FRAME * fmyhelp = {
	&_form0

};
