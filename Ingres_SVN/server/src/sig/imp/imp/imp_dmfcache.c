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
	5,
	0,
	0,
	0,
	0,
	0,
	"Select Cache Size"

};
static FLDCOL _c0_0 = {
	"page_size",
	0,
	0,
	"Page Size",
	1,
	1,
	1,
	0,
	13,
	1,
	0,
	65552,
	0,
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
	13,
	1,
	13,
	30,
	0,
	"",
	"",
	0,
	"-f13",
	0,
	0

};
static FLDCOL * _ac00[] = {
	&_c0_0

};
static TBLFLD _fC0 = {
	"cache",
	0,
	0,
	"",
	0,
	0,
	6,
	2,
	15,
	8,
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
	4,
	0,
	1,
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
static TRIM * _at00[] = {
	&_t00

};
static IFIELD * _af00[] = {
	&_f00

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"imp_dmfcache",
	(FIELD **) _af00,
	1,
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
	27,
	10,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	129,
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
GLOBALDEF	FRAME * imp_dmfcache = {
	&_form0

};
