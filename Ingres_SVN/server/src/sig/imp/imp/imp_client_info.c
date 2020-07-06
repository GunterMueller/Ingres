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
	"Client Connection Information"

};
static REGFLD _fC0 = {
	"client_host",
	0,
	0,
	"on host:",
	0,
	0,
	3,
	3,
	29,
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
	9,
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
static REGFLD _fC1 = {
	"client_tty",
	1,
	0,
	"Terminal:",
	0,
	0,
	2,
	4,
	30,
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
	10,
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
static IFIELD _f01 = {
	0,
	CAST_PINT &_fC1

};
static REGFLD _fC2 = {
	"client_user",
	2,
	0,
	"Username:",
	0,
	0,
	2,
	2,
	30,
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
	10,
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
	"client_pid",
	3,
	0,
	"Pid:",
	0,
	0,
	7,
	5,
	25,
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
	5,
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
static IFIELD _f03 = {
	0,
	CAST_PINT &_fC3

};
static REGFLD _fC4 = {
	"client_info",
	4,
	0,
	"Info:",
	0,
	0,
	6,
	8,
	36,
	4,
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
	123,
	0,
	120,
	6,
	30,
	-21,
	0,
	"",
	"",
	0,
	"c120.30",
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
static REGFLD _fC5 = {
	"client_connection",
	5,
	0,
	"Requesting:",
	0,
	0,
	0,
	6,
	42,
	2,
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
	12,
	30,
	-21,
	0,
	"",
	"",
	0,
	"c60.30",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f05 = {
	0,
	CAST_PINT &_fC5

};
static TRIM * _at00[] = {
	&_t00

};
static IFIELD * _af00[] = {
	&_f00,
	&_f01,
	&_f02,
	&_f03,
	&_f04,
	&_f05

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"imp_client_info",
	(FIELD **) _af00,
	6,
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
	43,
	12,
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
GLOBALDEF	FRAME * imp_client_info = {
	&_form0

};
