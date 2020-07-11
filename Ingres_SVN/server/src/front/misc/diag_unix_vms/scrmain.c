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
	18,
	2,
	0,
	0,
	0,
	0,
	"Select evidence set and then select an option"

};
static TRIM _t01 = {
	21,
	0,
	0,
	0,
	0,
	0,
	"INGRES EVIDENCE SET MANAGEMENT UTILITY"

};
static FLDCOL _c0_0 = {
	"data",
	0,
	0,
	"data",
	1,
	-1,
	1,
	0,
	78,
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
	81,
	0,
	78,
	1,
	78,
	-21,
	0,
	"",
	"",
	0,
	"c78",
	0,
	0

};
static FLDCOL * _ac00[] = {
	&_c0_0

};
static TBLFLD _fC0 = {
	"data",
	0,
	0,
	"",
	1,
	1,
	0,
	4,
	80,
	19,
	0,
	1073758241,
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
	17,
	0,
	1,
	0,
	1,
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
static TRIM * _at00[] = {
	&_t00,
	&_t01

};
static IFIELD * _af00[] = {
	&_f00

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"main_screen",
	(FIELD **) _af00,
	1,
	(FIELD **) _an00,
	0,
	_at00,
	2,
	0,
	0,
	0,
	0,
	0,
	0,
	85,
	26,
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
GLOBALDEF	FRAME * main_screen = {
	&_form0

};