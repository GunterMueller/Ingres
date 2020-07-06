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
	3,
	13,
	1,
	0,
	0,
	0,
	"3:72:1"

};
static TRIM _t01 = {
	56,
	2,
	1,
	0,
	0,
	0,
	"5:22:0"

};
static TRIM _t02 = {
	62,
	13,
	0,
	0,
	0,
	0,
	")"

};
static TRIM _t03 = {
	63,
	2,
	0,
	0,
	0,
	0,
	"Blocks"

};
static REGFLD _fC0 = {
	"vnode",
	0,
	0,
	"IMP Logging System Monitor for vnode:",
	0,
	0,
	0,
	0,
	58,
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
	38,
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
	"block_count",
	1,
	0,
	"Block Count:",
	0,
	0,
	3,
	3,
	21,
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
	8,
	13,
	8,
	-30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"logfull_interval",
	2,
	0,
	"Logfull Interval:",
	0,
	0,
	26,
	3,
	26,
	1,
	0,
	256,
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
	8,
	18,
	8,
	-30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"reserved",
	3,
	0,
	"Reserved:",
	0,
	0,
	59,
	3,
	18,
	1,
	0,
	256,
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
	8,
	10,
	8,
	-30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"block_size",
	4,
	0,
	"Block Size:",
	0,
	0,
	4,
	4,
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
	5,
	0,
	8,
	12,
	8,
	-30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"abort_interval",
	5,
	0,
	"Abort Interval:",
	0,
	0,
	28,
	4,
	24,
	1,
	0,
	256,
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
	8,
	16,
	8,
	-30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"in_use",
	6,
	0,
	"In Use:",
	0,
	0,
	61,
	4,
	16,
	1,
	0,
	256,
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
	8,
	8,
	8,
	-30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"buffer_count",
	7,
	0,
	"Buffer Count:",
	0,
	0,
	2,
	5,
	22,
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
	8,
	14,
	8,
	-30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"cp_interval",
	8,
	0,
	"CP Interval:",
	0,
	0,
	31,
	5,
	21,
	1,
	0,
	256,
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
	8,
	13,
	8,
	-30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"available",
	9,
	0,
	"Available:",
	0,
	0,
	58,
	5,
	19,
	1,
	0,
	256,
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
	8,
	11,
	8,
	-30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"bof",
	10,
	0,
	"Begin (BOF):",
	0,
	0,
	3,
	7,
	21,
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
	8,
	13,
	8,
	-30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"previous_cp",
	11,
	0,
	"Previous CP:",
	0,
	0,
	31,
	7,
	21,
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
	8,
	13,
	8,
	-30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"eof",
	12,
	0,
	"End (EOF):",
	0,
	0,
	5,
	8,
	19,
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
	8,
	11,
	8,
	-30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"next_cp",
	13,
	0,
	"Expected Next CP:",
	0,
	0,
	26,
	8,
	26,
	1,
	0,
	256,
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
	8,
	18,
	8,
	-30,
	0,
	"",
	"",
	0,
	"+i8",
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
	"emergency_status",
	14,
	0,
	"",
	1,
	1,
	58,
	9,
	17,
	3,
	0,
	16777473,
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
	1,
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
	1

};
static IFIELD _f014 = {
	0,
	CAST_PINT &_fC14

};
static REGFLD _fC15 = {
	"status",
	15,
	0,
	"Status:",
	0,
	0,
	8,
	10,
	48,
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
	43,
	0,
	40,
	8,
	40,
	-21,
	0,
	"",
	"",
	0,
	"c40",
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
	"log_pct",
	16,
	0,
	"Log File Diagram (% in use or reserved):",
	0,
	0,
	18,
	13,
	44,
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
	3,
	41,
	3,
	-30,
	0,
	"",
	"",
	0,
	"+i3",
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
	"barchart",
	17,
	0,
	"",
	0,
	0,
	4,
	14,
	70,
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
	73,
	0,
	70,
	0,
	70,
	-21,
	0,
	"",
	"",
	0,
	"c70",
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
static TRIM * _at00[] = {
	&_t00,
	&_t01,
	&_t02,
	&_t03

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
	&_f017

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"imp_lg_header",
	(FIELD **) _af00,
	18,
	(FIELD **) _an00,
	0,
	_at00,
	4,
	0,
	0,
	0,
	0,
	0,
	0,
	80,
	23,
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
	0,
	0,
	0,
	0

};
GLOBALDEF	FRAME * imp_lg_header = {
	&_form0

};
