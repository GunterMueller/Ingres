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

static FLDCOL _c0_0 = {
	"di_slaveno",
	0,
	0,
	"Slave",
	1,
	1,
	1,
	0,
	5,
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
	4,
	0,
	3,
	1,
	3,
	30,
	0,
	"",
	"",
	0,
	"+f3",
	0,
	0

};
static FLDCOL _c0_1 = {
	"di_dio",
	1,
	0,
	"I/Os",
	7,
	1,
	7,
	0,
	7,
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
	4,
	0,
	7,
	7,
	7,
	30,
	0,
	"",
	"",
	0,
	"+f7",
	0,
	0

};
static FLDCOL _c0_2 = {
	"di_reads",
	2,
	0,
	"Reads",
	15,
	1,
	15,
	0,
	7,
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
	4,
	0,
	7,
	15,
	7,
	30,
	0,
	"",
	"",
	0,
	"+f7",
	0,
	0

};
static FLDCOL _c0_3 = {
	"di_writes",
	3,
	0,
	"Writes",
	23,
	1,
	23,
	0,
	7,
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
	4,
	0,
	7,
	23,
	7,
	30,
	0,
	"",
	"",
	0,
	"+f7",
	0,
	0

};
static FLDCOL _c0_4 = {
	"di_stime_tm_secs",
	4,
	0,
	"Sys s",
	31,
	1,
	31,
	0,
	5,
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
	4,
	0,
	5,
	31,
	5,
	30,
	0,
	"",
	"",
	0,
	"+f5",
	0,
	0

};
static FLDCOL _c0_5 = {
	"di_stime_tm_msecs",
	5,
	0,
	"Sys ms",
	37,
	1,
	37,
	0,
	6,
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
	4,
	0,
	5,
	37,
	5,
	30,
	0,
	"",
	"",
	0,
	"+f5",
	0,
	0

};
static FLDCOL _c0_6 = {
	"di_utime_tm_secs",
	6,
	0,
	"Usr s",
	44,
	1,
	44,
	0,
	5,
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
	4,
	0,
	5,
	44,
	5,
	30,
	0,
	"",
	"",
	0,
	"+f5",
	0,
	0

};
static FLDCOL _c0_7 = {
	"di_utime_tm_msecs",
	7,
	0,
	"Usr ms",
	50,
	1,
	50,
	0,
	6,
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
	4,
	0,
	5,
	50,
	5,
	30,
	0,
	"",
	"",
	0,
	"+f5",
	0,
	0

};
static FLDCOL _c0_8 = {
	"cpu_tm_secs",
	8,
	0,
	"Sec",
	57,
	1,
	57,
	0,
	5,
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
	4,
	0,
	5,
	57,
	5,
	30,
	0,
	"",
	"",
	0,
	"+f5",
	0,
	0

};
static FLDCOL _c0_9 = {
	"cpu_tm_msecs",
	9,
	0,
	"Msec",
	63,
	1,
	63,
	0,
	5,
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
	4,
	0,
	5,
	63,
	5,
	30,
	0,
	"",
	"",
	0,
	"+f5",
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
	&_c0_6,
	&_c0_7,
	&_c0_8,
	&_c0_9

};
static TBLFLD _fC0 = {
	"ima_di_slave_info",
	0,
	0,
	"",
	0,
	0,
	0,
	2,
	69,
	12,
	0,
	33,
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
	10,
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
	"server",
	1,
	0,
	"DI Slave information for server:",
	0,
	0,
	0,
	0,
	53,
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
	33,
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
	"ima_di_slave_info",
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
	69,
	14,
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
GLOBALDEF	FRAME * ima_di_slave_info = {
	&_form0

};