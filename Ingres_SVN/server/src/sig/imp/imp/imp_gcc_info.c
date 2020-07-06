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
	6,
	1,
	0,
	0,
	0,
	"6:29:1"

};
static TRIM _t01 = {
	2,
	6,
	0,
	0,
	0,
	0,
	"Encryption"

};
static TRIM _t02 = {
	31,
	2,
	1,
	0,
	0,
	0,
	"7:24:0"

};
static TRIM _t03 = {
	31,
	9,
	1,
	0,
	0,
	0,
	"6:24:2"

};
static TRIM _t04 = {
	32,
	2,
	0,
	0,
	0,
	0,
	"Connections"

};
static TRIM _t05 = {
	32,
	9,
	0,
	0,
	0,
	0,
	"Traffic"

};
static REGFLD _fC0 = {
	"net_server",
	0,
	0,
	"Net Server:",
	0,
	0,
	6,
	0,
	44,
	1,
	0,
	65808,
	64,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	66,
	0,
	32,
	12,
	32,
	21,
	0,
	"",
	"",
	0,
	"c32",
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
	"trace_level",
	1,
	0,
	"Trace Level  :",
	0,
	0,
	1,
	2,
	21,
	1,
	0,
	65552,
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
	15,
	6,
	-30,
	0,
	"",
	"",
	0,
	"-i6",
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
	"pl_protocol",
	2,
	0,
	"Pl Protocol  :",
	0,
	0,
	1,
	3,
	21,
	1,
	0,
	65552,
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
	15,
	6,
	-30,
	0,
	"",
	"",
	0,
	"-i6",
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
	"conn_count",
	3,
	0,
	"Total   :",
	0,
	0,
	32,
	3,
	20,
	1,
	0,
	66576,
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
	10,
	10,
	10,
	-30,
	0,
	"",
	"",
	0,
	"-i10",
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
	"registry_mode",
	4,
	0,
	"Registry Mode:",
	0,
	0,
	1,
	4,
	27,
	1,
	0,
	65552,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	14,
	0,
	12,
	15,
	12,
	21,
	0,
	"",
	"",
	0,
	"c12",
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
	"inbound_current",
	5,
	0,
	"Inbound :",
	0,
	0,
	32,
	4,
	20,
	1,
	0,
	66576,
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
	10,
	10,
	10,
	-30,
	0,
	"",
	"",
	0,
	"-i10",
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
static REGFLD _fC6 = {
	"inbound_max",
	6,
	0,
	"I/B Max :",
	0,
	0,
	32,
	5,
	20,
	1,
	0,
	65552,
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
	10,
	10,
	10,
	-30,
	0,
	"",
	"",
	0,
	"-i10",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f06 = {
	0,
	CAST_PINT &_fC6

};
static REGFLD _fC7 = {
	"outbound_current",
	7,
	0,
	"Outbound:",
	0,
	0,
	32,
	6,
	20,
	1,
	0,
	66576,
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
	10,
	10,
	10,
	-30,
	0,
	"",
	"",
	0,
	"-i10",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f07 = {
	0,
	CAST_PINT &_fC7

};
static REGFLD _fC8 = {
	"ib_encrypt_mech",
	8,
	0,
	"Inbound Mech :",
	0,
	0,
	1,
	7,
	27,
	1,
	0,
	65552,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	14,
	0,
	12,
	15,
	12,
	21,
	0,
	"",
	"",
	0,
	"c12",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f08 = {
	0,
	CAST_PINT &_fC8

};
static REGFLD _fC9 = {
	"outbound_max",
	9,
	0,
	"O/B Max :",
	0,
	0,
	32,
	7,
	20,
	1,
	0,
	65552,
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
	10,
	10,
	10,
	-30,
	0,
	"",
	"",
	0,
	"-i10",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f09 = {
	0,
	CAST_PINT &_fC9

};
static REGFLD _fC10 = {
	"ib_encrypt_mode",
	10,
	0,
	"Inbound Mode :",
	0,
	0,
	1,
	8,
	27,
	1,
	0,
	65552,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	14,
	0,
	12,
	15,
	12,
	21,
	0,
	"",
	"",
	0,
	"c12",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f010 = {
	0,
	CAST_PINT &_fC10

};
static REGFLD _fC11 = {
	"ob_encrypt_mech",
	11,
	0,
	"Outbound Mech:",
	0,
	0,
	1,
	9,
	27,
	1,
	0,
	65552,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	14,
	0,
	12,
	15,
	12,
	21,
	0,
	"",
	"",
	0,
	"c12",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f011 = {
	0,
	CAST_PINT &_fC11

};
static REGFLD _fC12 = {
	"ob_encrypt_mode",
	12,
	0,
	"Outbound Mode:",
	0,
	0,
	1,
	10,
	27,
	1,
	0,
	65552,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	14,
	0,
	12,
	15,
	12,
	21,
	0,
	"",
	"",
	0,
	"c12",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f012 = {
	0,
	CAST_PINT &_fC12

};
static REGFLD _fC13 = {
	"data_in",
	13,
	0,
	"Data In :",
	0,
	0,
	32,
	10,
	22,
	1,
	0,
	66576,
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
	12,
	10,
	12,
	-30,
	0,
	"",
	"",
	0,
	"-i12",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f013 = {
	0,
	CAST_PINT &_fC13

};
static REGFLD _fC14 = {
	"data_out",
	14,
	0,
	"Data Out:",
	0,
	0,
	32,
	11,
	22,
	1,
	0,
	66576,
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
	12,
	10,
	12,
	-30,
	0,
	"",
	"",
	0,
	"-i12",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f014 = {
	0,
	CAST_PINT &_fC14

};
static REGFLD _fC15 = {
	"msgs_in",
	15,
	0,
	"Msgs In :",
	0,
	0,
	32,
	12,
	22,
	1,
	0,
	66576,
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
	12,
	10,
	12,
	-30,
	0,
	"",
	"",
	0,
	"-i12",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f015 = {
	0,
	CAST_PINT &_fC15

};
static REGFLD _fC16 = {
	"msgs_out",
	16,
	0,
	"Msgs Out:",
	0,
	0,
	32,
	13,
	22,
	1,
	0,
	66576,
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
	12,
	10,
	12,
	-30,
	0,
	"",
	"",
	0,
	"-i12",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f016 = {
	0,
	CAST_PINT &_fC16

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
	&_f04,
	&_f05,
	&_f06,
	&_f07,
	&_f08,
	&_f09,
	&_f010,
	&_f011,
	&_f012,
	&_f013,
	&_f014,
	&_f015,
	&_f016

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"imp_gcc_info",
	(FIELD **) _af00,
	17,
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
	55,
	15,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	193,
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
GLOBALDEF	FRAME * imp_gcc_info = {
	&_form0

};
