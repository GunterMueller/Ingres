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
	"Log Process Display for Vnode:",
	0,
	0,
	0,
	0,
	51,
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
	31,
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
static FLDCOL _c1_0 = {
	"computed_id",
	0,
	0,
	"ID",
	1,
	1,
	1,
	0,
	8,
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
	11,
	0,
	8,
	1,
	8,
	-21,
	0,
	"",
	"",
	0,
	"+c8",
	0,
	0

};
static FLDCOL _c1_1 = {
	"pid",
	1,
	0,
	"PID",
	10,
	1,
	10,
	0,
	8,
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
	0

};
static FLDCOL _c1_2 = {
	"process_type",
	2,
	0,
	"Type",
	19,
	1,
	19,
	0,
	9,
	1,
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
	"",
	103,
	0,
	9,
	19,
	9,
	-21,
	0,
	"",
	"",
	0,
	"c9",
	0,
	0

};
static FLDCOL _c1_3 = {
	"open_db",
	3,
	0,
	"DBs",
	29,
	1,
	29,
	0,
	4,
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
	5,
	0,
	4,
	29,
	4,
	-30,
	0,
	"",
	"",
	0,
	"+i4",
	0,
	0

};
static FLDCOL _c1_4 = {
	"writes",
	4,
	0,
	"Write",
	34,
	1,
	34,
	0,
	8,
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
	5,
	0,
	8,
	34,
	8,
	-30,
	0,
	"",
	"",
	0,
	"-i8",
	0,
	0

};
static FLDCOL _c1_5 = {
	"forces",
	5,
	0,
	"Force",
	43,
	1,
	43,
	0,
	8,
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
	5,
	0,
	8,
	43,
	8,
	-30,
	0,
	"",
	"",
	0,
	"-i8",
	0,
	0

};
static FLDCOL _c1_6 = {
	"waits",
	6,
	0,
	"Wait",
	52,
	1,
	52,
	0,
	8,
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
	5,
	0,
	8,
	52,
	8,
	-30,
	0,
	"",
	"",
	0,
	"-i8",
	0,
	0

};
static FLDCOL _c1_7 = {
	"begins",
	7,
	0,
	"Begin",
	61,
	1,
	61,
	0,
	8,
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
	5,
	0,
	8,
	61,
	8,
	-30,
	0,
	"",
	"",
	0,
	"-i8",
	0,
	0

};
static FLDCOL _c1_8 = {
	"ends",
	8,
	0,
	"End",
	70,
	1,
	70,
	0,
	8,
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
	5,
	0,
	8,
	70,
	8,
	-30,
	0,
	"",
	"",
	0,
	"-i8",
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
	&_c1_8

};
static TBLFLD _fC1 = {
	"process_tbl",
	1,
	0,
	"",
	1,
	1,
	0,
	2,
	79,
	11,
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
	7,
	0,
	9,
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
	"imp_lg_processes",
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
	82,
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
GLOBALDEF	FRAME * imp_lg_processes = {
	&_form0

};