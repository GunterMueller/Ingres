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
	"lastquery",
	0,
	0,
	"Previously Running Query:",
	0,
	0,
	0,
	0,
	71,
	9,
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
	563,
	0,
	560,
	1,
	70,
	-21,
	0,
	"",
	"",
	0,
	"c560.70",
	0,
	0,
	0,
	0,
	0,
	0,
	1

};
static IFIELD _f00 = {
	0,
	CAST_PINT &_fC0

};
static REGFLD _fC1 = {
	"query",
	1,
	0,
	"Currently Running Query:",
	0,
	0,
	0,
	9,
	71,
	9,
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
	563,
	0,
	560,
	1,
	70,
	-21,
	0,
	"",
	"",
	0,
	"c560.70",
	0,
	0,
	0,
	0,
	0,
	0,
	1

};
static IFIELD _f01 = {
	0,
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
	"imp_query_info",
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
	71,
	18,
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
GLOBALDEF	FRAME * imp_query_info = {
	&_form0

};
