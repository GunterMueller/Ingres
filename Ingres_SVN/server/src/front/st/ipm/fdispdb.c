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

static FLDCOL _c0_0 = {
	"database",
	0,
	0,
	"database",
	1,
	-1,
	1,
	0,
	32,
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
	34,
	0,
	32,
	1,
	32,
	21,
	0,
	"",
	"",
	0,
	"c32",
	0,
	0

};
static FLDCOL * _ac00[] = {
	&_c0_0

};
static TBLFLD _fC0 = {
	"dblist",
	0,
	0,
	"",
	1,
	1,
	0,
	0,
	34,
	10,
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
	8,
	0,
	1,
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
static TRIM * _at00[] = {
	0

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
	"fdispdb",
	(FIELD **) _af00,
	1,
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
	34,
	10,
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
GLOBALDEF	FRAME * fdispdb = {
	&_form0

};
