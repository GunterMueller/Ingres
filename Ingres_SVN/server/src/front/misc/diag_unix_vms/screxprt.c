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
	9,
	7,
	0,
	0,
	0,
	0,
	"Please supply the name of a file to export the evidence set to"

};
static TRIM _t01 = {
	21,
	0,
	0,
	0,
	0,
	0,
	"INGRES EVIDENCE SET MANAGEMENT UTILITY"

};
static REGFLD _fC0 = {
	"data",
	0,
	0,
	"",
	1,
	1,
	0,
	10,
	80,
	3,
	0,
	1,
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
static TRIM * _at00[] = {
	&_t00,
	&_t01

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
	"export_screen",
	(FIELD **) _af00,
	1,
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
GLOBALDEF	FRAME * export_screen = {
	&_form0

};
