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
	"Distributed"

};
static TRIM _t01 = {
	0,
	1,
	0,
	0,
	0,
	0,
	"Transaction"

};
static TRIM _t02 = {
	0,
	2,
	0,
	0,
	0,
	0,
	"Hex Dump:"

};
static REGFLD _fC0 = {
	"lxb_dis_tran_id_hexdump",
	0,
	0,
	"",
	0,
	0,
	11,
	0,
	55,
	7,
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
	353,
	0,
	378,
	1,
	54,
	-21,
	0,
	"",
	"",
	0,
	"c350.54",
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
static TRIM * _at00[] = {
	&_t00,
	&_t01,
	&_t02

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
	"imp_transaction_hexdump",
	(FIELD **) _af00,
	1,
	(FIELD **) _an00,
	0,
	_at00,
	3,
	0,
	0,
	0,
	0,
	0,
	0,
	66,
	7,
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
GLOBALDEF	FRAME * imp_transaction_hexdump = {
	&_form0

};
