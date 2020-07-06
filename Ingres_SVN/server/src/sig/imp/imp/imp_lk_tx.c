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
	1024,
	0,
	0,
	0,
	"More Info:"

};
static REGFLD _fC0 = {
	"tx_id",
	0,
	0,
	"Transaction ID:",
	0,
	0,
	0,
	1,
	46,
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
	33,
	0,
	30,
	16,
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
static IFIELD _f00 = {
	0,
	CAST_PINT &_fC0

};
static REGFLD _fC1 = {
	"related_lock_list_id",
	1,
	0,
	"Related Lock List ID:",
	0,
	0,
	0,
	2,
	37,
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
	18,
	0,
	15,
	22,
	15,
	-21,
	0,
	"",
	"",
	0,
	"c15",
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
	"related_count",
	2,
	0,
	"Count:",
	0,
	0,
	40,
	2,
	11,
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
	5,
	0,
	4,
	7,
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
static IFIELD _f02 = {
	0,
	CAST_PINT &_fC2

};
static REGFLD _fC3 = {
	"waiting_resource_list_id",
	3,
	0,
	"Waiting Resource List ID:",
	0,
	0,
	0,
	3,
	41,
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
	18,
	0,
	15,
	26,
	15,
	-21,
	0,
	"",
	"",
	0,
	"c15",
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
static TRIM * _at00[] = {
	&_t00

};
static IFIELD * _af00[] = {
	&_f00,
	&_f01,
	&_f02,
	&_f03

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"imp_lk_tx",
	(FIELD **) _af00,
	4,
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
	52,
	4,
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
GLOBALDEF	FRAME * imp_lk_tx = {
	&_form0

};
