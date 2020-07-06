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
	"Server Detail"

};
static TRIM _t01 = {
	0,
	10,
	0,
	0,
	0,
	0,
	"Connecting to:"

};
static TRIM _t02 = {
	4,
	1,
	1,
	0,
	0,
	0,
	"5:26:0"

};
static TRIM _t03 = {
	4,
	3,
	1,
	0,
	0,
	0,
	"1:26:3"

};
static TRIM _t04 = {
	5,
	2,
	0,
	0,
	0,
	0,
	"Sessions"

};
static TRIM _t05 = {
	5,
	4,
	0,
	0,
	0,
	0,
	"Total"

};
static TRIM _t06 = {
	14,
	1,
	1,
	0,
	0,
	0,
	"5:1:1"

};
static TRIM _t07 = {
	15,
	2,
	0,
	0,
	0,
	0,
	"Current"

};
static TRIM _t08 = {
	23,
	1,
	1,
	0,
	0,
	0,
	"5:1:2"

};
static TRIM _t09 = {
	24,
	2,
	0,
	0,
	0,
	0,
	"Max"

};
static REGFLD _fC0 = {
	"current_sessions",
	0,
	0,
	"",
	0,
	0,
	16,
	4,
	4,
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
	4,
	0,
	4,
	0,
	4,
	30,
	0,
	"",
	"",
	0,
	"-i4",
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
	"max_sessions",
	1,
	0,
	"",
	0,
	0,
	24,
	4,
	4,
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
	5,
	0,
	4,
	0,
	4,
	-30,
	0,
	"",
	"",
	0,
	"-i4",
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
	"server_type",
	2,
	0,
	"Type:",
	0,
	0,
	0,
	8,
	26,
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
	23,
	0,
	20,
	6,
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
static REGFLD _fC3 = {
	"server_dblist",
	3,
	0,
	"",
	0,
	0,
	0,
	11,
	30,
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
	33,
	0,
	30,
	0,
	30,
	-21,
	0,
	"",
	"",
	0,
	"c30",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f03 = {
	0,
	CAST_PINT &_fC3

};
static REGFLD _fC4 = {
	"server_name",
	4,
	0,
	"Name:",
	0,
	0,
	0,
	7,
	16,
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
	13,
	0,
	10,
	6,
	10,
	-21,
	0,
	"",
	"",
	0,
	"c10",
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
	&_t00,
	&_t01,
	&_t02,
	&_t03,
	&_t04,
	&_t05,
	&_t06,
	&_t07,
	&_t08,
	&_t09

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
	"imp_server_popup",
	(FIELD **) _af00,
	5,
	(FIELD **) _an00,
	0,
	_at00,
	10,
	0,
	0,
	0,
	0,
	0,
	0,
	32,
	13,
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
GLOBALDEF	FRAME * imp_server_popup = {
	&_form0

};
