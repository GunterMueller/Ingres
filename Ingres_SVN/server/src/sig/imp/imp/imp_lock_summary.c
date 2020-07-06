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
	"Locking System Summary for Vnode:"

};
static TRIM _t01 = {
	0,
	2,
	1,
	0,
	0,
	0,
	"1:9:0"

};
static TRIM _t02 = {
	0,
	2,
	1,
	0,
	0,
	0,
	"5:1:1"

};
static TRIM _t03 = {
	0,
	6,
	1,
	0,
	0,
	0,
	"1:41:9"

};
static TRIM _t04 = {
	0,
	8,
	1,
	0,
	0,
	0,
	"10:1:11"

};
static TRIM _t05 = {
	0,
	8,
	1,
	0,
	0,
	0,
	"1:32:10"

};
static TRIM _t06 = {
	0,
	17,
	1,
	0,
	0,
	0,
	"1:77:14"

};
static TRIM _t07 = {
	9,
	2,
	0,
	0,
	0,
	0,
	"Lock List Information"

};
static TRIM _t08 = {
	30,
	2,
	1,
	0,
	0,
	0,
	"1:11:2"

};
static TRIM _t09 = {
	32,
	8,
	0,
	0,
	0,
	0,
	"Lock information"

};
static TRIM _t010 = {
	40,
	2,
	1,
	0,
	0,
	0,
	"5:1:3"

};
static TRIM _t011 = {
	43,
	2,
	1,
	0,
	0,
	0,
	"1:9:5"

};
static TRIM _t012 = {
	43,
	2,
	1,
	0,
	0,
	0,
	"3:1:4"

};
static TRIM _t013 = {
	43,
	4,
	1,
	0,
	0,
	0,
	"1:35:8"

};
static TRIM _t014 = {
	48,
	8,
	1,
	0,
	0,
	0,
	"1:29:12"

};
static TRIM _t015 = {
	52,
	2,
	0,
	0,
	0,
	0,
	"Hash Table Sizes"

};
static TRIM _t016 = {
	68,
	2,
	1,
	0,
	0,
	0,
	"1:10:6"

};
static TRIM _t017 = {
	76,
	8,
	1,
	0,
	0,
	0,
	"10:1:13"

};
static TRIM _t018 = {
	77,
	2,
	1,
	0,
	0,
	0,
	"3:1:7"

};
static REGFLD _fC0 = {
	"vnode_name",
	0,
	0,
	"",
	0,
	0,
	34,
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
static IFIELD _f00 = {
	0,
	CAST_PINT &_fC0

};
static REGFLD _fC1 = {
	"ll_created",
	1,
	0,
	"Created:",
	0,
	0,
	2,
	3,
	17,
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
	9,
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
	"ll_inuse",
	2,
	0,
	"In use:",
	0,
	0,
	24,
	3,
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
static IFIELD _f02 = {
	0,
	CAST_PINT &_fC2

};
static REGFLD _fC3 = {
	"lock_tbl_size",
	3,
	0,
	"Lock:",
	0,
	0,
	44,
	3,
	14,
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
	6,
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
	"res_tbl_size",
	4,
	0,
	"Resource:",
	0,
	0,
	59,
	3,
	18,
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
static IFIELD _f04 = {
	0,
	CAST_PINT &_fC4

};
static REGFLD _fC5 = {
	"ll_released",
	5,
	0,
	"Released:",
	0,
	0,
	1,
	4,
	18,
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
static IFIELD _f05 = {
	0,
	CAST_PINT &_fC5

};
static REGFLD _fC6 = {
	"ll_remaining",
	6,
	0,
	"Remaining:",
	0,
	0,
	21,
	4,
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
static IFIELD _f06 = {
	0,
	CAST_PINT &_fC6

};
static REGFLD _fC7 = {
	"ll_total",
	7,
	0,
	"Total available:",
	0,
	0,
	15,
	5,
	25,
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
	17,
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
	"res_in_use",
	8,
	0,
	"Resources in use:",
	0,
	0,
	48,
	5,
	26,
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
	18,
	8,
	-30,
	0,
	"",
	"",
	0,
	"-i8",
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
	"locks_requested",
	9,
	0,
	"Requested:",
	0,
	0,
	4,
	9,
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
static IFIELD _f09 = {
	0,
	CAST_PINT &_fC9

};
static REGFLD _fC10 = {
	"locks_released",
	10,
	0,
	"Released:",
	0,
	0,
	29,
	9,
	18,
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
static IFIELD _f010 = {
	0,
	CAST_PINT &_fC10

};
static REGFLD _fC11 = {
	"locks_in_use",
	11,
	0,
	"In use:",
	0,
	0,
	60,
	9,
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
static IFIELD _f011 = {
	0,
	CAST_PINT &_fC11

};
static REGFLD _fC12 = {
	"locks_rerequested",
	12,
	0,
	"Re-requested:",
	0,
	0,
	1,
	10,
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
static IFIELD _f012 = {
	0,
	CAST_PINT &_fC12

};
static REGFLD _fC13 = {
	"locks_cancelled",
	13,
	0,
	"Cancelled:",
	0,
	0,
	28,
	10,
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
static IFIELD _f013 = {
	0,
	CAST_PINT &_fC13

};
static REGFLD _fC14 = {
	"locks_remaining",
	14,
	0,
	"Remaining:",
	0,
	0,
	57,
	10,
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
static IFIELD _f014 = {
	0,
	CAST_PINT &_fC14

};
static REGFLD _fC15 = {
	"locks_converted",
	15,
	0,
	"Converted:",
	0,
	0,
	4,
	11,
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
static IFIELD _f015 = {
	0,
	CAST_PINT &_fC15

};
static REGFLD _fC16 = {
	"locks_escalated",
	16,
	0,
	"Escalated:",
	0,
	0,
	28,
	11,
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
static IFIELD _f016 = {
	0,
	CAST_PINT &_fC16

};
static REGFLD _fC17 = {
	"locks_total",
	17,
	0,
	"Total available:",
	0,
	0,
	51,
	11,
	25,
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
	17,
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
static IFIELD _f017 = {
	0,
	CAST_PINT &_fC17

};
static REGFLD _fC18 = {
	"deadlock_search",
	18,
	0,
	"Deadlock search:",
	0,
	0,
	2,
	13,
	25,
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
	17,
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
static IFIELD _f018 = {
	0,
	CAST_PINT &_fC18

};
static REGFLD _fC19 = {
	"deadlock",
	19,
	0,
	"Deadlock:",
	0,
	0,
	29,
	13,
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
static IFIELD _f019 = {
	0,
	CAST_PINT &_fC19

};
static REGFLD _fC20 = {
	"convert_wait",
	20,
	0,
	"Convert wait:",
	0,
	0,
	54,
	13,
	22,
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
static IFIELD _f020 = {
	0,
	CAST_PINT &_fC20

};
static REGFLD _fC21 = {
	"convert_deadlock",
	21,
	0,
	"Convert deadlock:",
	0,
	0,
	1,
	14,
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
static IFIELD _f021 = {
	0,
	CAST_PINT &_fC21

};
static REGFLD _fC22 = {
	"lock_wait",
	22,
	0,
	"Lock wait:",
	0,
	0,
	57,
	14,
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
static IFIELD _f022 = {
	0,
	CAST_PINT &_fC22

};
static REGFLD _fC23 = {
	"locks_per_tx",
	23,
	0,
	"Locks per Transaction:",
	0,
	0,
	1,
	16,
	31,
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
	23,
	8,
	-30,
	0,
	"",
	"",
	0,
	"-i8",
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
	&_t011,
	&_t012,
	&_t013,
	&_t014,
	&_t015,
	&_t016,
	&_t017,
	&_t018

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
	&_f023

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"imp_lock_summary",
	(FIELD **) _af00,
	24,
	(FIELD **) _an00,
	0,
	_at00,
	19,
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
GLOBALDEF	FRAME * imp_lock_summary = {
	&_form0

};
