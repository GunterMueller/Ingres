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
	"RDF cache Info for Server:",
	0,
	0,
	0,
	0,
	57,
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
	32,
	0,
	30,
	27,
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
	"max_defaults",
	1,
	0,
	"Max Defaults:",
	0,
	0,
	0,
	7,
	27,
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
static IFIELD _f01 = {
	0,
	CAST_PINT &_fC1

};
static REGFLD _fC2 = {
	"state_string",
	2,
	0,
	"State:",
	0,
	0,
	0,
	2,
	67,
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
	62,
	0,
	60,
	7,
	60,
	21,
	0,
	"",
	"",
	0,
	"c60",
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
	"cache_size",
	3,
	0,
	"Cache Size:",
	0,
	0,
	2,
	5,
	25,
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
	12,
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
	"max_tables",
	4,
	0,
	"Max Tables:",
	0,
	0,
	42,
	5,
	25,
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
	12,
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
	"max_qtrees",
	5,
	0,
	"Max Qtrees:",
	0,
	0,
	2,
	6,
	25,
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
	12,
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
	"max_ldb_descs",
	6,
	0,
	"Max Ldb Descs:",
	0,
	0,
	40,
	6,
	28,
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
static IFIELD _f06 = {
	0,
	CAST_PINT &_fC6

};
static TRIM * _at00[] = {
	0

};
static IFIELD * _af00[] = {
	&_f00,
	&_f01,
	&_f02,
	&_f03,
	&_f04,
	&_f05,
	&_f06

};
static IFIELD * _an00[] = {
	0

};
static FRAME _form0 = {
	"",
	9,
	"ima_rdf_cache_info",
	(FIELD **) _af00,
	7,
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
	71,
	8,
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
GLOBALDEF	FRAME * ima_rdf_cache_info = {
	&_form0

};
