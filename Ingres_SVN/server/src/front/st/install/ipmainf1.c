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
	"INGBUILD - Ingres Installation Utility"

};
static TRIM _t01 = {
	3,
	2,
	1,
	0,
	0,
	0,
	"4:74:0"

};
static TRIM _t02 = {
	4,
	7,
	2048,
	0,
	0,
	0,
	"Custom"

};
static TRIM _t03 = {
	4,
	12,
	2048,
	0,
	0,
	0,
	"Package"

};
static TRIM _t04 = {
	4,
	14,
	2048,
	0,
	0,
	0,
	"Help"

};
static TRIM _t05 = {
	9,
	4,
	0,
	0,
	0,
	0,
	"Distribution:"

};
static TRIM _t06 = {
	12,
	3,
	0,
	0,
	0,
	0,
	"II_SYSTEM:"

};
static TRIM _t07 = {
	12,
	7,
	0,
	0,
	0,
	0,
	"- Select this option in order to view the contents of the"

};
static TRIM _t08 = {
	12,
	12,
	0,
	0,
	0,
	0,
	"- Select predefined packages for installation on your system."

};
static TRIM _t09 = {
	12,
	14,
	0,
	0,
	0,
	0,
	"- See more instructions on using this program."

};
static TRIM _t010 = {
	14,
	8,
	0,
	0,
	0,
	0,
	"distribution medium and/or select components for installation"

};
static TRIM _t011 = {
	14,
	9,
	0,
	0,
	0,
	0,
	"on your system.  This option will also allow you to upgrade"

};
static TRIM _t012 = {
	14,
	10,
	0,
	0,
	0,
	0,
	"an existing installation of INGRES products."

};
static REGFLD _fC0 = {
	"root_dir",
	0,
	0,
	"",
	0,
	0,
	23,
	3,
	52,
	1,
	0,
	0,
	576,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	258,
	0,
	52,
	0,
	52,
	21,
	0,
	"",
	"",
	0,
	"c52",
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
	"dist_dev",
	1,
	0,
	"",
	0,
	0,
	23,
	4,
	52,
	1,
	0,
	1024,
	64,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	258,
	0,
	52,
	0,
	52,
	21,
	0,
	"",
	"",
	0,
	"c52",
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
	"menu_prompt_1",
	2,
	0,
	"",
	0,
	0,
	4,
	20,
	72,
	1,
	0,
	2048,
	512,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	75,
	0,
	72,
	0,
	72,
	-21,
	0,
	"",
	"",
	0,
	"c72",
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
	"menu_prompt_2",
	3,
	0,
	"",
	0,
	0,
	4,
	21,
	72,
	1,
	0,
	2048,
	512,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"",
	75,
	0,
	72,
	0,
	72,
	-21,
	0,
	"",
	"",
	0,
	"c72",
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
	&_t012

};
static IFIELD * _af00[] = {
	&_f00,
	&_f01,
	&_f02,
	&_f03

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"ipmainform1",
	(FIELD **) _af00,
	4,
	(FIELD **) _an00,
	0,
	_at00,
	13,
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
GLOBALDEF	FRAME * ipmainform1 = {
	&_form0

};
