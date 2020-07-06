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
	"vnode",
	0,
	0,
	"Log Transaction Display for Vnode:",
	0,
	0,
	0,
	0,
	55,
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
	35,
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
static FLDCOL _c1_0 = {
	"ext_tx_id",
	0,
	0,
	"External Tx ID",
	1,
	1,
	1,
	0,
	15,
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
	18,
	0,
	15,
	1,
	15,
	-21,
	0,
	"",
	"",
	0,
	"c15",
	0,
	0

};
static FLDCOL _c1_1 = {
	"database",
	1,
	0,
	"Database",
	17,
	1,
	17,
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
	17,
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
static FLDCOL _c1_2 = {
	"username",
	2,
	0,
	"Session",
	30,
	1,
	30,
	0,
	15,
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
	18,
	0,
	15,
	30,
	15,
	-21,
	0,
	"",
	"",
	0,
	"c15",
	0,
	0

};
static FLDCOL _c1_3 = {
	"status",
	3,
	0,
	"Status",
	46,
	1,
	46,
	0,
	18,
	1,
	0,
	0,
	64,
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
	18,
	46,
	18,
	-21,
	0,
	"",
	"",
	0,
	"c18",
	0,
	0

};
static FLDCOL _c1_4 = {
	"writes",
	4,
	0,
	"Write",
	65,
	1,
	65,
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
	5,
	0,
	6,
	65,
	6,
	-30,
	0,
	"",
	"",
	0,
	"+i6",
	0,
	0

};
static FLDCOL _c1_5 = {
	"splits",
	5,
	0,
	"Split",
	72,
	1,
	72,
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
	5,
	0,
	6,
	72,
	6,
	-30,
	0,
	"",
	"",
	0,
	"+i6",
	0,
	0

};
static FLDCOL * _ac10[] = {
	&_c1_0,
	&_c1_1,
	&_c1_2,
	&_c1_3,
	&_c1_4,
	&_c1_5

};
static TBLFLD _fC1 = {
	"log_txtbl",
	1,
	0,
	"",
	1,
	1,
	0,
	2,
	79,
	19,
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
	15,
	0,
	6,
	0,
	3,
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
	"imp_lg_transactions",
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
	85,
	23,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	64,
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
GLOBALDEF	FRAME * imp_lg_transactions = {
	&_form0

};
