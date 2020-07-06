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
	"Server List on Vnode:"

};
static FLDCOL _c0_0 = {
	"serveraddress",
	0,
	0,
	"Name",
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
static FLDCOL _c0_1 = {
	"serverclass",
	1,
	0,
	"Type",
	17,
	1,
	17,
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
	17,
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
static FLDCOL _c0_2 = {
	"numsessions",
	2,
	0,
	"Sessions",
	26,
	1,
	26,
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
	5,
	0,
	4,
	26,
	4,
	-30,
	0,
	"",
	"",
	0,
	"-i4",
	0,
	0

};
static FLDCOL _c0_3 = {
	"serverdblist",
	3,
	0,
	"Connecting to database(s)",
	35,
	1,
	35,
	0,
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
	35,
	30,
	-21,
	0,
	"",
	"",
	0,
	"c30",
	0,
	0

};
static FLDCOL * _ac00[] = {
	&_c0_0,
	&_c0_1,
	&_c0_2,
	&_c0_3

};
static TBLFLD _fC0 = {
	"gcn_entry_tbl",
	0,
	0,
	"",
	1,
	1,
	0,
	1,
	66,
	10,
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
	6,
	0,
	4,
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
	"vnode",
	1,
	0,
	"",
	0,
	0,
	22,
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
	"imp_server_list",
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
	67,
	11,
	2,
	4,
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
GLOBALDEF	FRAME * imp_server_list = {
	&_form0

};
