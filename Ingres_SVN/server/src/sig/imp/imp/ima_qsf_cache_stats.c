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
	2,
	1,
	0,
	0,
	0,
	"4:78:0"

};
static TRIM _t01 = {
	0,
	6,
	1,
	0,
	0,
	0,
	"6:33:1"

};
static TRIM _t02 = {
	0,
	12,
	1,
	0,
	0,
	0,
	"6:33:3"

};
static TRIM _t03 = {
	0,
	18,
	1,
	0,
	0,
	0,
	"5:33:5"

};
static TRIM _t04 = {
	1,
	2,
	0,
	0,
	0,
	0,
	"QSF Pool Information:"

};
static TRIM _t05 = {
	1,
	6,
	0,
	0,
	0,
	0,
	"QSF Object Information:"

};
static TRIM _t06 = {
	1,
	12,
	0,
	0,
	0,
	0,
	"QSF Peak Object Information:"

};
static TRIM _t07 = {
	1,
	18,
	0,
	0,
	0,
	0,
	"QSF LRU Information:"

};
static TRIM _t08 = {
	37,
	6,
	1,
	0,
	0,
	0,
	"6:40:2"

};
static TRIM _t09 = {
	37,
	12,
	1,
	0,
	0,
	0,
	"4:34:4"

};
static TRIM _t010 = {
	37,
	18,
	1,
	0,
	0,
	0,
	"4:31:6"

};
static TRIM _t011 = {
	38,
	6,
	0,
	0,
	0,
	0,
	"QSF Bucket Information:"

};
static TRIM _t012 = {
	38,
	12,
	0,
	0,
	0,
	0,
	"QSF Session Information:"

};
static TRIM _t013 = {
	38,
	18,
	0,
	0,
	0,
	0,
	"Misc Statistics:"

};
static REGFLD _fC0 = {
	"server",
	0,
	0,
	"QSF Cache Statistics for Server:",
	0,
	0,
	1,
	0,
	63,
	1,
	0,
	65536,
	512,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	32,
	0,
	30,
	33,
	30,
	21,
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
	"qsr_bkts_used",
	1,
	0,
	"Buckets in use   :",
	0,
	0,
	38,
	8,
	32,
	1,
	0,
	65536,
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
	19,
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
	"qsr_bmaxobjs",
	2,
	0,
	"Most ever in any bucket:",
	0,
	0,
	38,
	10,
	38,
	1,
	0,
	65536,
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
	25,
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
static IFIELD _f02 = {
	0,
	CAST_PINT &_fC2

};
static REGFLD _fC3 = {
	"qsr_decay_factor",
	3,
	0,
	"Decay Factor   :",
	0,
	0,
	1,
	21,
	30,
	1,
	0,
	65536,
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
	17,
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
static IFIELD _f03 = {
	0,
	CAST_PINT &_fC3

};
static REGFLD _fC4 = {
	"qsr_memleft",
	4,
	0,
	"Memory (remaining/total) :",
	0,
	0,
	1,
	3,
	40,
	1,
	0,
	65536,
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
	27,
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
static IFIELD _f04 = {
	0,
	CAST_PINT &_fC4

};
static REGFLD _fC5 = {
	"qsr_memtot",
	5,
	0,
	"/",
	0,
	0,
	41,
	3,
	16,
	1,
	0,
	65536,
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
	3,
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
static IFIELD _f05 = {
	0,
	CAST_PINT &_fC5

};
static REGFLD _fC6 = {
	"qsr_mx_index",
	6,
	0,
	"Indexed Objects:",
	0,
	0,
	1,
	15,
	30,
	1,
	0,
	65536,
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
	17,
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
	"qsr_mx_named",
	7,
	0,
	"Named Objects  :",
	0,
	0,
	1,
	14,
	30,
	1,
	0,
	65536,
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
	17,
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
	"qsr_mx_rsize",
	8,
	0,
	"Qsr Mx Rsize:",
	0,
	0,
	38,
	20,
	27,
	1,
	0,
	65536,
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
	"qsr_mx_size",
	9,
	0,
	"Qsr Mx Size:",
	0,
	0,
	38,
	19,
	28,
	1,
	0,
	65536,
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
static IFIELD _f09 = {
	0,
	CAST_PINT &_fC9

};
static REGFLD _fC10 = {
	"qsr_mx_unnamed",
	10,
	0,
	"Unnamed Objects:",
	0,
	0,
	1,
	13,
	30,
	1,
	0,
	65536,
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
	17,
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
static IFIELD _f010 = {
	0,
	CAST_PINT &_fC10

};
static REGFLD _fC11 = {
	"qsr_mxbkts_used",
	11,
	0,
	"Peak buckets used:",
	0,
	0,
	38,
	9,
	32,
	1,
	0,
	65536,
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
	19,
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
static IFIELD _f011 = {
	0,
	CAST_PINT &_fC11

};
static REGFLD _fC12 = {
	"qsr_mxobjs",
	12,
	0,
	"Total Objects  :",
	0,
	0,
	1,
	16,
	30,
	1,
	0,
	65536,
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
	17,
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
	"qsr_mxsess",
	13,
	0,
	"Peak Sessions    :",
	0,
	0,
	38,
	14,
	32,
	1,
	0,
	65536,
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
	19,
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
static IFIELD _f013 = {
	0,
	CAST_PINT &_fC13

};
static REGFLD _fC14 = {
	"qsr_named_requests",
	14,
	0,
	"Object Requests:",
	0,
	0,
	1,
	19,
	30,
	1,
	0,
	65536,
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
	17,
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
	"qsr_nbuckets",
	15,
	0,
	"Hash Buckets     :",
	0,
	0,
	38,
	7,
	32,
	1,
	0,
	65536,
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
	19,
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
	"qsr_no_destroyed",
	16,
	0,
	"Objs Destroyed :",
	0,
	0,
	1,
	20,
	30,
	1,
	0,
	65536,
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
	17,
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
static IFIELD _f016 = {
	0,
	CAST_PINT &_fC16

};
static REGFLD _fC17 = {
	"qsr_no_index",
	17,
	0,
	"Indexed Objects:",
	0,
	0,
	1,
	9,
	30,
	1,
	0,
	65536,
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
	17,
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
	"qsr_no_named",
	18,
	0,
	"Named Objects  :",
	0,
	0,
	1,
	8,
	30,
	1,
	0,
	65536,
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
	17,
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
static IFIELD _f018 = {
	0,
	CAST_PINT &_fC18

};
static REGFLD _fC19 = {
	"qsr_no_unnamed",
	19,
	0,
	"Unnamed Objects:",
	0,
	0,
	1,
	7,
	30,
	1,
	0,
	65536,
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
	17,
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
static IFIELD _f019 = {
	0,
	CAST_PINT &_fC19

};
static REGFLD _fC20 = {
	"qsr_nobjs",
	20,
	0,
	"Total Objects  :",
	0,
	0,
	1,
	10,
	30,
	1,
	0,
	65536,
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
	17,
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
static IFIELD _f020 = {
	0,
	CAST_PINT &_fC20

};
static REGFLD _fC21 = {
	"qsr_nsess",
	21,
	0,
	"Current Sessions :",
	0,
	0,
	38,
	13,
	32,
	1,
	0,
	65536,
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
	19,
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
static IFIELD _f021 = {
	0,
	CAST_PINT &_fC21

};
static REGFLD _fC22 = {
	"pct_used",
	22,
	0,
	"Percentage used:",
	0,
	0,
	1,
	4,
	23,
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
	4,
	0,
	6,
	17,
	6,
	31,
	0,
	"",
	"",
	0,
	"-f6.2",
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
	"bar_chart",
	23,
	0,
	"",
	0,
	0,
	26,
	4,
	50,
	1,
	0,
	1024,
	512,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	52,
	0,
	50,
	0,
	50,
	21,
	0,
	"",
	"",
	0,
	"c50",
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
	&_t013

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
	"ima_qsf_cache_stats",
	(FIELD **) _af00,
	24,
	(FIELD **) _an00,
	0,
	_at00,
	14,
	0,
	0,
	0,
	0,
	0,
	0,
	80,
	30,
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
GLOBALDEF	FRAME * ima_qsf_cache_stats = {
	&_form0

};
