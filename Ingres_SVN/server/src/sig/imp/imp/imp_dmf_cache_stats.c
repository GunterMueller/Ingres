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
	"8:28:0"

};
static TRIM _t01 = {
	1,
	9,
	1,
	0,
	0,
	0,
	"9:28:2"

};
static TRIM _t02 = {
	1,
	18,
	1,
	0,
	0,
	0,
	"5:28:4"

};
static TRIM _t03 = {
	2,
	1,
	0,
	0,
	0,
	0,
	"Group Buffers"

};
static TRIM _t04 = {
	2,
	9,
	0,
	0,
	0,
	0,
	"Counts:"

};
static TRIM _t05 = {
	2,
	18,
	0,
	0,
	0,
	0,
	"Waits:"

};
static TRIM _t06 = {
	17,
	2,
	0,
	0,
	0,
	0,
	"pages)"

};
static TRIM _t07 = {
	32,
	5,
	1,
	0,
	0,
	0,
	"7:26:1"

};
static TRIM _t08 = {
	32,
	12,
	1,
	0,
	0,
	0,
	"6:26:3"

};
static TRIM _t09 = {
	32,
	21,
	0,
	0,
	0,
	0,
	"Cache Contents (D=dirty, f=free)"

};
static TRIM _t010 = {
	33,
	5,
	0,
	0,
	0,
	0,
	"Single Pages"

};
static TRIM _t011 = {
	55,
	5,
	0,
	0,
	0,
	0,
	")"

};
static REGFLD _fC0 = {
	"server",
	0,
	0,
	"Server:",
	0,
	0,
	32,
	2,
	33,
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
	27,
	0,
	25,
	8,
	25,
	21,
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
	"hit_ratio",
	1,
	0,
	"",
	0,
	0,
	32,
	19,
	45,
	1,
	0,
	1024,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	48,
	0,
	45,
	0,
	45,
	-21,
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
static IFIELD _f01 = {
	0,
	CAST_PINT &_fC1

};
static REGFLD _fC2 = {
	"hit_ratio_pct",
	2,
	0,
	"Hit Rate % :",
	0,
	0,
	32,
	18,
	21,
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
	8,
	13,
	8,
	-31,
	0,
	"",
	"",
	0,
	"+f8",
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
	"force_count",
	3,
	0,
	"Force      :",
	0,
	0,
	2,
	16,
	26,
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
	"+f13",
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
	"io_wait_count",
	4,
	0,
	"I/O     :",
	0,
	0,
	2,
	20,
	23,
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
	4,
	0,
	13,
	10,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
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
	"free_buffer_count",
	5,
	0,
	"Free     :",
	0,
	0,
	33,
	8,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
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
	"group_buffer_size",
	6,
	0,
	"x",
	0,
	0,
	9,
	2,
	7,
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
	4,
	0,
	5,
	2,
	5,
	30,
	0,
	"",
	"",
	0,
	"+f5",
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
	"group_buffer_read_count",
	7,
	0,
	"Reads    :",
	0,
	0,
	2,
	3,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
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
	"group_buffer_write_count",
	8,
	0,
	"Writes   :",
	0,
	0,
	2,
	4,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
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
	"fix_count",
	9,
	0,
	"Fix        :",
	0,
	0,
	2,
	12,
	26,
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
	"+f13",
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
	"unfix_count",
	10,
	0,
	"Unfix      :",
	0,
	0,
	2,
	14,
	26,
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
	"+f13",
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
	"read_count",
	11,
	0,
	"Reads    :",
	0,
	0,
	33,
	6,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
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
	"write_count",
	12,
	0,
	"Writes   :",
	0,
	0,
	33,
	7,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
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
	"hit_count",
	13,
	0,
	"Hit        :",
	0,
	0,
	2,
	13,
	26,
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
	"+f13",
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
	"dirty_unfix_count",
	14,
	0,
	"Unfix Dirty:",
	0,
	0,
	2,
	15,
	26,
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
	"+f13",
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
	"free_group_buffer_count",
	15,
	0,
	"Free     :",
	0,
	0,
	2,
	5,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
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
	"fixed_buffer_count",
	16,
	0,
	"Fixed    :",
	0,
	0,
	33,
	9,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
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
	"fixed_group_buffer_count",
	17,
	0,
	"Fixed    :",
	0,
	0,
	2,
	6,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
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
	"modified_buffer_count",
	18,
	0,
	"Dirty    :",
	0,
	0,
	33,
	10,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
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
	"page_buffer_count",
	19,
	0,
	"(",
	0,
	0,
	46,
	5,
	9,
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
	4,
	0,
	8,
	1,
	8,
	30,
	0,
	"",
	"",
	0,
	"+f8",
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
static REGFLD _fC20 = {
	"flimit",
	20,
	0,
	"Flimit   :",
	0,
	0,
	33,
	14,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f020 = {
	0,
	CAST_PINT &_fC20

};
static REGFLD _fC21 = {
	"mlimit",
	21,
	0,
	"Mlimit   :",
	0,
	0,
	33,
	13,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f021 = {
	0,
	CAST_PINT &_fC21

};
static REGFLD _fC22 = {
	"wbstart",
	22,
	0,
	"W/B start:",
	0,
	0,
	33,
	15,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f022 = {
	0,
	CAST_PINT &_fC22

};
static REGFLD _fC23 = {
	"wbend",
	23,
	0,
	"W/B end  :",
	0,
	0,
	33,
	16,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f023 = {
	0,
	CAST_PINT &_fC23

};
static REGFLD _fC24 = {
	"cache",
	24,
	0,
	"",
	0,
	0,
	32,
	22,
	45,
	1,
	0,
	1024,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	48,
	0,
	45,
	0,
	45,
	-21,
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
static IFIELD _f024 = {
	0,
	CAST_PINT &_fC24

};
static REGFLD _fC25 = {
	"modified_group_buffer_count",
	25,
	0,
	"Dirty    :",
	0,
	0,
	2,
	7,
	24,
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
	4,
	0,
	13,
	11,
	13,
	30,
	0,
	"",
	"",
	0,
	"+f13",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f025 = {
	0,
	CAST_PINT &_fC25

};
static REGFLD _fC26 = {
	"group_buffer_count",
	26,
	0,
	"(",
	0,
	0,
	2,
	2,
	6,
	1,
	0,
	66576,
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
	5,
	1,
	5,
	30,
	0,
	"",
	"",
	0,
	"+f5",
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f026 = {
	0,
	CAST_PINT &_fC26

};
static REGFLD _fC27 = {
	"page_size",
	27,
	0,
	"Page Size:",
	0,
	0,
	32,
	3,
	24,
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
	4,
	0,
	13,
	11,
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
static IFIELD _f027 = {
	0,
	CAST_PINT &_fC27

};
static TRIM * _at00[] = {
	&_t00,
	&_t01,
	&_t02,
	&_t03,
	&_t04,
	&_t05,
	&_t06,
	&_t07,
	&_t08,
	&_t09,
	&_t010,
	&_t011

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
	&_f019,
	&_f020,
	&_f021,
	&_f022,
	&_f023,
	&_f024,
	&_f025,
	&_f026,
	&_f027

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"imp_dmf_cache_stats",
	(FIELD **) _af00,
	28,
	(FIELD **) _an00,
	0,
	_at00,
	12,
	0,
	0,
	0,
	0,
	0,
	0,
	83,
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
GLOBALDEF	FRAME * imp_dmf_cache_stats = {
	&_form0

};
