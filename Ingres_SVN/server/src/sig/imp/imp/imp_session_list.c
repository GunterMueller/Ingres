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
	"Session List for server:"

};
static FLDCOL _c0_0 = {
	"session_id",
	0,
	0,
	"ID",
	1,
	1,
	1,
	0,
	12,
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
	15,
	0,
	12,
	1,
	12,
	-21,
	0,
	"",
	"",
	0,
	"c12",
	0,
	0

};
static FLDCOL _c0_1 = {
	"session_name",
	1,
	0,
	"Name",
	14,
	1,
	14,
	0,
	12,
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
	15,
	0,
	12,
	14,
	12,
	-21,
	0,
	"",
	"",
	0,
	"c12",
	0,
	0

};
static FLDCOL _c0_2 = {
	"terminal",
	2,
	0,
	"TTY",
	27,
	1,
	27,
	0,
	6,
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
	9,
	0,
	6,
	27,
	6,
	-21,
	0,
	"",
	"",
	0,
	"c6",
	0,
	0

};
static FLDCOL _c0_3 = {
	"database",
	3,
	0,
	"Database",
	34,
	1,
	34,
	0,
	8,
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
	11,
	0,
	8,
	34,
	8,
	-21,
	0,
	"",
	"",
	0,
	"c8",
	0,
	0

};
static FLDCOL _c0_4 = {
	"state",
	4,
	0,
	"State",
	43,
	1,
	43,
	0,
	6,
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
	9,
	0,
	6,
	43,
	6,
	-21,
	0,
	"",
	"",
	0,
	"c6",
	0,
	0

};
static FLDCOL _c0_5 = {
	"facility",
	5,
	0,
	"Fac",
	50,
	1,
	50,
	0,
	3,
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
	6,
	0,
	3,
	50,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
	0,
	0

};
static FLDCOL _c0_6 = {
	"query",
	6,
	0,
	"Query",
	54,
	1,
	54,
	0,
	20,
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
	54,
	20,
	-21,
	0,
	"",
	"",
	0,
	"c20",
	0,
	0

};
static FLDCOL * _ac00[] = {
	&_c0_0,
	&_c0_1,
	&_c0_2,
	&_c0_3,
	&_c0_4,
	&_c0_5,
	&_c0_6

};
static TBLFLD _fC0 = {
	"svr_sess_tbl",
	0,
	0,
	"",
	1,
	1,
	0,
	1,
	75,
	12,
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
	8,
	0,
	7,
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
static REGFLD _fC1 = {
	"server_name",
	1,
	0,
	"",
	0,
	0,
	25,
	0,
	20,
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
	0,
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
	"imp_session_list",
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
	75,
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
GLOBALDEF	FRAME * imp_session_list = {
	&_form0

};
