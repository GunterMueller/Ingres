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

static REGFLD _fC0 = {
	"vnode",
	0,
	0,
	"Lock List Display for Vnode:",
	0,
	0,
	0,
	0,
	59,
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
	29,
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
static FLDCOL _c1_0 = {
	"scb_pid",
	0,
	0,
	"scbpid",
	1,
	1,
	1,
	0,
	6,
	1,
	0,
	16777216,
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
	4,
	1,
	4,
	-30,
	0,
	"",
	"",
	0,
	"-i4",
	0,
	0

};
static FLDCOL _c1_1 = {
	"pid",
	1,
	0,
	"pid",
	8,
	1,
	8,
	0,
	4,
	1,
	0,
	16777216,
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
	4,
	8,
	4,
	-30,
	0,
	"",
	"",
	0,
	"-i4",
	0,
	0

};
static FLDCOL _c1_2 = {
	"sid",
	2,
	0,
	"sid",
	13,
	1,
	13,
	0,
	12,
	1,
	0,
	16777216,
	64,
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
	12,
	13,
	12,
	-21,
	0,
	"",
	"",
	0,
	"c12",
	0,
	0

};
static FLDCOL _c1_3 = {
	"computed_id",
	3,
	0,
	"ID",
	26,
	1,
	26,
	0,
	8,
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
	11,
	0,
	8,
	26,
	8,
	-21,
	0,
	"",
	"",
	0,
	"c8",
	0,
	0

};
static FLDCOL _c1_4 = {
	"id_id",
	4,
	0,
	"id_id",
	35,
	1,
	35,
	0,
	5,
	1,
	0,
	16777216,
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
	4,
	35,
	4,
	-30,
	0,
	"",
	"",
	0,
	"-i4",
	0,
	0

};
static FLDCOL _c1_5 = {
	"id_instance",
	5,
	0,
	"id_instance",
	41,
	1,
	41,
	0,
	11,
	1,
	0,
	16777216,
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
	4,
	41,
	4,
	-30,
	0,
	"",
	"",
	0,
	"-i4",
	0,
	0

};
static FLDCOL _c1_6 = {
	"username",
	6,
	0,
	"Session",
	53,
	1,
	53,
	0,
	15,
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
	53,
	15,
	-21,
	0,
	"",
	"",
	0,
	"c15",
	0,
	0

};
static FLDCOL _c1_7 = {
	"lkb_count",
	7,
	0,
	"Count",
	69,
	1,
	69,
	0,
	5,
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
	69,
	4,
	-30,
	0,
	"",
	"",
	0,
	"-i4",
	0,
	0

};
static FLDCOL _c1_8 = {
	"llkb_count",
	8,
	0,
	"Logical",
	75,
	1,
	75,
	0,
	7,
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
	75,
	4,
	-30,
	0,
	"",
	"",
	0,
	"-i4",
	0,
	0

};
static FLDCOL _c1_9 = {
	"max_lkb",
	9,
	0,
	"MaxL",
	83,
	1,
	83,
	0,
	4,
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
	83,
	4,
	-30,
	0,
	"",
	"",
	0,
	"-i4",
	0,
	0

};
static FLDCOL _c1_a = {
	"llb_status",
	10,
	0,
	"Status",
	88,
	1,
	88,
	0,
	30,
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
	88,
	30,
	-21,
	0,
	"",
	"",
	0,
	"c30",
	0,
	0

};
static FLDCOL _c1_b = {
	"state",
	11,
	0,
	"State",
	119,
	1,
	119,
	0,
	20,
	1,
	0,
	16777216,
	0,
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
	119,
	20,
	-21,
	0,
	"",
	"",
	0,
	"c20",
	0,
	0

};
static FLDCOL _c1_c = {
	"wait_reason",
	12,
	0,
	"wait_reason",
	140,
	1,
	140,
	0,
	20,
	1,
	0,
	16777216,
	0,
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
	140,
	20,
	-21,
	0,
	"",
	"",
	0,
	"c20",
	0,
	0

};
static FLDCOL _c1_d = {
	"server",
	13,
	0,
	"server",
	161,
	1,
	161,
	0,
	6,
	1,
	0,
	16777216,
	64,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	67,
	0,
	5,
	161,
	5,
	-21,
	0,
	"",
	"",
	0,
	"c5",
	0,
	0

};
static FLDCOL * _ac10[] = {
	&_c1_0,
	&_c1_1,
	&_c1_2,
	&_c1_3,
	&_c1_4,
	&_c1_5,
	&_c1_6,
	&_c1_7,
	&_c1_8,
	&_c1_9,
	&_c1_a,
	&_c1_b,
	&_c1_c,
	&_c1_d

};
static TBLFLD _fC1 = {
	"locktbl",
	1,
	0,
	"",
	1,
	1,
	0,
	2,
	168,
	19,
	0,
	1073741857,
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
	15,
	0,
	14,
	0,
	3,
	1,
	0,
	_ac10,
	0,
	0,
	0,
	0,
	0,
	0,
	0

};
static IFIELD _f01 = {
	1,
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
	"imp_lk_list",
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
	187,
	23,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	64,
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
GLOBALDEF	FRAME * imp_lk_list = {
	&_form0

};