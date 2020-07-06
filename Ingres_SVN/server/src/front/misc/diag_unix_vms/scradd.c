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
	8,
	3,
	0,
	0,
	0,
	0,
	"Please supply the following details:"

};
static TRIM _t01 = {
	8,
	7,
	0,
	0,
	0,
	0,
	"Description:"

};
static TRIM _t02 = {
	8,
	10,
	0,
	0,
	0,
	0,
	"Type:"

};
static TRIM _t03 = {
	8,
	13,
	0,
	0,
	0,
	0,
	"Filename:"

};
static TRIM _t04 = {
	21,
	0,
	0,
	0,
	0,
	0,
	"INGRES EVIDENCE SET MANAGEMENT UTILITY"

};
static TRIM _t05 = {
	31,
	10,
	0,
	0,
	0,
	0,
	"TEXT or BINARY"

};
static REGFLD _fC0 = {
	"description",
	0,
	0,
	"",
	1,
	1,
	21,
	6,
	52,
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
	53,
	0,
	50,
	1,
	50,
	-21,
	0,
	"",
	"",
	0,
	"c50",
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
	"type",
	1,
	0,
	"",
	1,
	1,
	21,
	9,
	8,
	3,
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
	"TEXT",
	9,
	0,
	6,
	1,
	6,
	-21,
	0,
	"type=\"TEXT\" or type=\"BINARY\"",
	"Type must be \"TEXT\" or \"BINARY\"",
	0,
	"c6",
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
static REGFLD _fC2 = {
	"filename",
	2,
	0,
	"",
	1,
	1,
	21,
	12,
	52,
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
	53,
	0,
	50,
	1,
	50,
	-21,
	0,
	"",
	"",
	0,
	"c50",
	0,
	0,
	0,
	0,
	0,
	0,
	1

};
static IFIELD _f02 = {
	0,
	CAST_PINT &_fC2

};
static TRIM * _at00[] = {
	&_t00,
	&_t01,
	&_t02,
	&_t03,
	&_t04,
	&_t05

};
static IFIELD * _af00[] = {
	&_f00,
	&_f01,
	&_f02

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"add_screen",
	(FIELD **) _af00,
	3,
	(FIELD **) _an00,
	0,
	_at00,
	6,
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
GLOBALDEF	FRAME * add_screen = {
	&_form0

};
