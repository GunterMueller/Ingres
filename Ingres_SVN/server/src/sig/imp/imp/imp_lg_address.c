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
	1,
	1,
	1,
	0,
	0,
	0,
	"5:40:0"

};
static TRIM _t01 = {
	1,
	7,
	1,
	0,
	0,
	0,
	"3:57:2"

};
static TRIM _t02 = {
	2,
	1,
	0,
	0,
	0,
	0,
	"Log address for"

};
static TRIM _t03 = {
	2,
	7,
	0,
	0,
	0,
	0,
	"Journal Window"

};
static TRIM _t04 = {
	44,
	2,
	1,
	0,
	0,
	0,
	"3:22:1"

};
static TRIM _t05 = {
	45,
	2,
	0,
	0,
	0,
	0,
	"Last Transaction ID"

};
static REGFLD _fC0 = {
	"bof",
	0,
	0,
	"Begin (BOF):",
	0,
	0,
	2,
	2,
	38,
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
	28,
	0,
	25,
	13,
	25,
	-21,
	0,
	"",
	"",
	0,
	"c25",
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
	"eof",
	1,
	0,
	"End (EOF):",
	0,
	0,
	4,
	3,
	36,
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
	28,
	0,
	25,
	11,
	25,
	-21,
	0,
	"",
	"",
	0,
	"c25",
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
	"last_tx_id",
	2,
	0,
	"",
	0,
	0,
	45,
	3,
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
static IFIELD _f02 = {
	0,
	CAST_PINT &_fC2

};
static REGFLD _fC3 = {
	"prev_cp",
	3,
	0,
	"Previous CP:",
	0,
	0,
	2,
	4,
	38,
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
	28,
	0,
	25,
	13,
	25,
	-21,
	0,
	"",
	"",
	0,
	"c25",
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
	"jnl_window",
	4,
	0,
	"",
	0,
	0,
	2,
	8,
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
	58,
	0,
	55,
	0,
	55,
	-21,
	0,
	"",
	"",
	0,
	"c55",
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
	&_t05

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
	"imp_lg_address",
	(FIELD **) _af00,
	5,
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
	69,
	10,
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
GLOBALDEF	FRAME * imp_lg_address = {
	&_form0

};
