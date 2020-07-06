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
	2048,
	0,
	0,
	0,
	"INGRES IPM Option Selection - Currently set options."

};
static TRIM _t01 = {
	2,
	8,
	1,
	0,
	0,
	0,
	"4:36:0"

};
static TRIM _t02 = {
	2,
	12,
	1,
	0,
	0,
	0,
	"4:36:1"

};
static REGFLD _fC0 = {
	"flag_null",
	0,
	0,
	"Display null resources\?",
	1,
	1,
	2,
	2,
	36,
	3,
	0,
	1089,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"n",
	1,
	0,
	1,
	34,
	1,
	32,
	0,
	"flag_null = \"y\" or flag_null = \"n\"",
	"Please enter \"y\" or \"n\"...",
	0,
	"c1",
	0,
	0,
	0,
	0,
	0,
	0,
	1

};
static IFIELD _f00 = {
	0,
	CAST_PINT &_fC0

};
static REGFLD _fC1 = {
	"flag_all_lk",
	1,
	0,
	"Display all resource types\?",
	1,
	1,
	44,
	2,
	32,
	3,
	0,
	1089,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"n",
	1,
	0,
	1,
	30,
	1,
	32,
	0,
	"flag_all_lk = \"y\" or flag_all_lk = \"n\"",
	"Please enter \"y\" or \"n\"...",
	0,
	"c1",
	0,
	0,
	0,
	0,
	0,
	0,
	1

};
static IFIELD _f01 = {
	0,
	CAST_PINT &_fC1

};
static REGFLD _fC2 = {
	"flag_lktype",
	2,
	0,
	"Display specified resource type\?",
	1,
	1,
	2,
	5,
	36,
	3,
	0,
	1089,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"n",
	1,
	0,
	1,
	34,
	1,
	32,
	0,
	"flag_lktype = \"y\" or flag_lktype = \"n\"",
	"Please enter \"y\" or \"n\"...",
	0,
	"c1",
	0,
	0,
	0,
	0,
	0,
	0,
	1

};
static IFIELD _f02 = {
	0,
	CAST_PINT &_fC2

};
static REGFLD _fC3 = {
	"locktype",
	3,
	0,
	"Type to display:",
	0,
	0,
	45,
	6,
	29,
	1,
	0,
	256,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	12,
	0,
	12,
	17,
	12,
	32,
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
static IFIELD _f03 = {
	0,
	CAST_PINT &_fC3

};
static REGFLD _fC4 = {
	"flag_db",
	4,
	0,
	"Display locks for one DB\?",
	0,
	0,
	3,
	9,
	34,
	1,
	0,
	1088,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"n",
	1,
	0,
	1,
	33,
	1,
	32,
	0,
	"flag_db = \"y\" or flag_db = \"n\"",
	"Please enter \"y\" or \"n\"...",
	0,
	"c1",
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
	"dbname",
	5,
	0,
	"Specify database (\?<CR> for list)",
	1,
	1,
	44,
	8,
	35,
	4,
	0,
	321,
	0,
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
	33,
	1,
	33,
	32,
	0,
	"",
	"",
	0,
	"c33",
	0,
	0,
	0,
	0,
	0,
	0,
	2

};
static IFIELD _f05 = {
	0,
	CAST_PINT &_fC5

};
static REGFLD _fC6 = {
	"flag_table",
	6,
	0,
	"Display locks for one table\?",
	0,
	0,
	3,
	13,
	34,
	1,
	0,
	1088,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"n",
	1,
	0,
	1,
	33,
	1,
	32,
	0,
	"flag_table = \"y\" or flag_table = \"n\"",
	"Please enter \"y\" or \"n\"...",
	0,
	"c1",
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
	"tablename",
	7,
	0,
	"Specify a table name",
	1,
	1,
	44,
	12,
	35,
	4,
	0,
	321,
	0,
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
	33,
	1,
	33,
	32,
	0,
	"",
	"",
	0,
	"c33",
	0,
	0,
	0,
	0,
	0,
	0,
	2

};
static IFIELD _f07 = {
	0,
	CAST_PINT &_fC7

};
static REGFLD _fC8 = {
	"frs_timeout",
	8,
	0,
	"Display Refresh (seconds)",
	1,
	1,
	2,
	16,
	36,
	3,
	0,
	1089,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"0",
	4,
	0,
	3,
	32,
	3,
	30,
	0,
	"frs_timeout >= 0 and frs_timeout < 1000",
	"Please enter a value between 0 and 999 inclusive",
	0,
	"-i3",
	0,
	0,
	0,
	0,
	0,
	0,
	1

};
static IFIELD _f08 = {
	0,
	CAST_PINT &_fC8

};
static REGFLD _fC9 = {
	"flag_nonprot_lklists",
	9,
	0,
	"Display system locklists\?",
	1,
	1,
	44,
	16,
	32,
	3,
	0,
	1089,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	3,
	0,
	1,
	30,
	1,
	21,
	0,
	"flag_nonprot_lklists in [\"y\", \"n\"]",
	"Please enter \"y\" or \"n\"...",
	0,
	"c1",
	0,
	0,
	0,
	0,
	0,
	0,
	1

};
static IFIELD _f09 = {
	0,
	CAST_PINT &_fC9

};
static REGFLD _fC10 = {
	"flag_inactive_tx",
	10,
	0,
	"Display inactive transactions\?",
	1,
	1,
	2,
	19,
	36,
	3,
	0,
	1089,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	3,
	0,
	1,
	34,
	1,
	21,
	0,
	"flag_inactive_tx in [\"y\", \"n\"]",
	"Please enter \"y\" or \"n\"...",
	0,
	"c1",
	0,
	0,
	0,
	0,
	0,
	0,
	1

};
static IFIELD _f010 = {
	0,
	CAST_PINT &_fC10

};
static REGFLD _fC11 = {
	"flag_internal_sess",
	11,
	0,
	"Display internal sessions\?",
	1,
	1,
	44,
	19,
	32,
	3,
	0,
	1089,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	3,
	0,
	1,
	30,
	1,
	21,
	0,
	"flag_internal_sess = \"y\" or flag_internal_sess = \"n\"",
	"",
	0,
	"c1",
	0,
	0,
	0,
	0,
	0,
	0,
	1

};
static IFIELD _f011 = {
	0,
	CAST_PINT &_fC11

};
static TRIM * _at00[] = {
	&_t00,
	&_t01,
	&_t02

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
	&_f011

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"fshowopts",
	(FIELD **) _af00,
	12,
	(FIELD **) _an00,
	0,
	_at00,
	3,
	0,
	0,
	0,
	0,
	0,
	0,
	80,
	22,
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
GLOBALDEF	FRAME * fshowopts = {
	&_form0

};
