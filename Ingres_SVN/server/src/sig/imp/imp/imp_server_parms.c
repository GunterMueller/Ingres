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
	1,
	0,
	0,
	0,
	"4:64:0"

};
static TRIM _t01 = {
	0,
	3,
	1,
	0,
	0,
	0,
	"1:64:2"

};
static TRIM _t02 = {
	0,
	3,
	1,
	0,
	0,
	0,
	"7:1:1"

};
static TRIM _t03 = {
	0,
	9,
	1,
	0,
	0,
	0,
	"1:64:4"

};
static TRIM _t04 = {
	0,
	9,
	1,
	0,
	0,
	0,
	"7:1:5"

};
static TRIM _t05 = {
	0,
	15,
	1,
	0,
	0,
	0,
	"4:64:7"

};
static TRIM _t06 = {
	63,
	3,
	1,
	0,
	0,
	0,
	"7:1:3"

};
static TRIM _t07 = {
	63,
	9,
	1,
	0,
	0,
	0,
	"7:1:6"

};
static REGFLD _fC0 = {
	"server",
	0,
	0,
	"Parameters for server:",
	0,
	0,
	1,
	1,
	58,
	1,
	0,
	65552,
	512,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	37,
	0,
	35,
	23,
	35,
	21,
	0,
	"",
	"",
	0,
	"c35",
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
	"cursors",
	1,
	0,
	"Cursors:",
	0,
	0,
	10,
	16,
	22,
	1,
	0,
	65552,
	512,
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
	13,
	9,
	13,
	30,
	0,
	"",
	"",
	0,
	"-f13",
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
	"svr_state",
	2,
	0,
	"Svr State:",
	0,
	0,
	1,
	2,
	22,
	1,
	0,
	65552,
	512,
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
	11,
	11,
	11,
	21,
	0,
	"",
	"",
	0,
	"c11",
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
	"capabilities",
	3,
	0,
	"Capabilities:",
	0,
	0,
	2,
	10,
	59,
	1,
	0,
	65552,
	512,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	47,
	0,
	45,
	14,
	45,
	21,
	0,
	"",
	"",
	0,
	"c45",
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
	"cursor_flags",
	4,
	0,
	"Cursor Flags:",
	0,
	0,
	5,
	17,
	44,
	1,
	0,
	65552,
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
	14,
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
static IFIELD _f04 = {
	0,
	CAST_PINT &_fC4

};
static REGFLD _fC5 = {
	"fastcommit",
	5,
	0,
	"Fast Commit:",
	0,
	0,
	3,
	12,
	16,
	1,
	0,
	65552,
	512,
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
	13,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
	"connects",
	6,
	0,
	"Connections:",
	0,
	0,
	2,
	7,
	26,
	1,
	0,
	65552,
	512,
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
	13,
	13,
	13,
	30,
	0,
	"",
	"",
	0,
	"-f13",
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
	"wbehind",
	7,
	0,
	"Write Behinds:",
	0,
	0,
	1,
	14,
	28,
	1,
	0,
	65552,
	512,
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
	13,
	15,
	13,
	30,
	0,
	"",
	"",
	0,
	"-f13",
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
	"listen_fails",
	8,
	0,
	"Listen Fails:",
	0,
	0,
	34,
	5,
	27,
	1,
	0,
	65552,
	512,
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
	13,
	14,
	13,
	30,
	0,
	"",
	"",
	0,
	"-f13",
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
	"shut_state",
	9,
	0,
	"Shutdown State:",
	0,
	0,
	26,
	2,
	23,
	1,
	0,
	65552,
	512,
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
	7,
	16,
	7,
	21,
	0,
	"",
	"",
	0,
	"c7",
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
	"listen_state",
	10,
	0,
	"Listen State:",
	0,
	0,
	34,
	4,
	20,
	1,
	0,
	65552,
	512,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	8,
	0,
	6,
	14,
	6,
	21,
	0,
	"",
	"",
	0,
	"c6",
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
	"server_name",
	11,
	0,
	"Server Name:",
	0,
	0,
	2,
	5,
	29,
	1,
	0,
	65552,
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
	16,
	13,
	16,
	21,
	0,
	"",
	"",
	0,
	"c16",
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
	"max_connects",
	12,
	0,
	"Max. Connections:",
	0,
	0,
	30,
	7,
	31,
	1,
	0,
	65552,
	512,
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
	13,
	18,
	13,
	30,
	0,
	"",
	"",
	0,
	"-f13",
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
	"name_service",
	13,
	0,
	"Name Service:",
	0,
	0,
	1,
	4,
	17,
	1,
	0,
	65552,
	512,
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
	14,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
	"res_connects",
	14,
	0,
	"Res. Connections:",
	0,
	0,
	30,
	8,
	31,
	1,
	0,
	65552,
	512,
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
	13,
	18,
	13,
	30,
	0,
	"",
	"",
	0,
	"-f13",
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
	"rule_depth",
	15,
	0,
	"Rule Depth:",
	0,
	0,
	35,
	14,
	25,
	1,
	0,
	65552,
	512,
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
	13,
	12,
	13,
	30,
	0,
	"",
	"",
	0,
	"-f13",
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
	"nostar_recov",
	16,
	0,
	"2PC recovery:",
	0,
	0,
	33,
	13,
	17,
	1,
	0,
	65552,
	512,
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
	14,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
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
static REGFLD _fC17 = {
	"nousers",
	17,
	0,
	"Max. Users:",
	0,
	0,
	3,
	8,
	25,
	1,
	0,
	65552,
	512,
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
	13,
	12,
	13,
	30,
	0,
	"",
	"",
	0,
	"-f13",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f017 = {
	0,
	CAST_PINT &_fC17

};
static REGFLD _fC18 = {
	"soleserver",
	18,
	0,
	"Sole Server:",
	0,
	0,
	3,
	13,
	16,
	1,
	0,
	65552,
	512,
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
	13,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f018 = {
	0,
	CAST_PINT &_fC18

};
static REGFLD _fC19 = {
	"incons_risk",
	19,
	0,
	"Incons Risk:",
	0,
	0,
	34,
	12,
	16,
	1,
	0,
	65552,
	512,
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
	13,
	3,
	-21,
	0,
	"",
	"",
	0,
	"c3",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f019 = {
	0,
	CAST_PINT &_fC19

};
static TRIM * _at00[] = {
	&_t00,
	&_t01,
	&_t02,
	&_t03,
	&_t04,
	&_t05,
	&_t06,
	&_t07

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
	&_f016,
	&_f017,
	&_f018,
	&_f019

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"imp_server_parms",
	(FIELD **) _af00,
	20,
	(FIELD **) _an00,
	0,
	_at00,
	8,
	0,
	0,
	0,
	0,
	0,
	0,
	78,
	19,
	0,
	0,
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
GLOBALDEF	FRAME * imp_server_parms = {
	&_form0

};
