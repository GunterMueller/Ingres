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
	"server",
	0,
	0,
	"Relative I/O monitor for Server:",
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
static IFIELD _f00 = {
	0,
	CAST_PINT &_fC0

};
static FLDCOL _c1_0 = {
	"session_id",
	0,
	0,
	"Session",
	1,
	1,
	1,
	0,
	13,
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
	16,
	0,
	13,
	1,
	13,
	-21,
	0,
	"",
	"",
	0,
	"c13",
	0,
	0

};
static FLDCOL _c1_1 = {
	"terminal",
	1,
	0,
	"Tty",
	15,
	1,
	15,
	0,
	6,
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
	6,
	15,
	6,
	-21,
	0,
	"",
	"",
	0,
	"c6",
	0,
	0

};
static FLDCOL _c1_2 = {
	"username",
	2,
	0,
	"User",
	22,
	1,
	22,
	0,
	15,
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
	18,
	0,
	15,
	22,
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
static FLDCOL _c1_3 = {
	"barchart",
	3,
	0,
	"",
	38,
	1,
	38,
	0,
	35,
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
	38,
	0,
	35,
	38,
	35,
	-21,
	0,
	"",
	"",
	0,
	"c35",
	0,
	0

};
static FLDCOL _c1_4 = {
	"io_rate",
	4,
	0,
	"I/O",
	74,
	1,
	74,
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
	74,
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
static FLDCOL * _ac10[] = {
	&_c1_0,
	&_c1_1,
	&_c1_2,
	&_c1_3,
	&_c1_4

};
static TBLFLD _fC1 = {
	"ima_dio",
	1,
	0,
	"",
	1,
	1,
	0,
	3,
	79,
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
	5,
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
static REGFLD _fC2 = {
	"highval",
	2,
	0,
	"High Value :",
	0,
	0,
	61,
	15,
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
	4,
	13,
	4,
	-30,
	0,
	"",
	"",
	0,
	"+i4",
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
static TRIM * _at00[] = {
	0

};
static IFIELD * _af00[] = {
	&_f00,
	&_f01,
	&_f02

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"ima_dio",
	(FIELD **) _af00,
	3,
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
	90,
	18,
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
GLOBALDEF	FRAME * ima_dio = {
	&_form0

};
