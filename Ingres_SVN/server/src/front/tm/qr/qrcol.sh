# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
/*
** Name:        qrcol.qsh -- Definition(s) for Column Access
**
** Description:
**	This structure is used to pull in all the info used when
** gathering column information for the help command that looks like:
** 
** Column Information:
**                                                                   Key
** Column Name                      Type       Length Nulls Defaults Seq
** team_no                          integer         4  yes      no     1
** team_name                        varchar        20  yes      no   
** conf_no                          integer         4  yes      no   
**
** History:
**	29-dec-1992 (daver)
**	First Written.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
EXEC SQL BEGIN DECLARE SECTION;
typedef struct
{
	char    colname[FE_MAXNAME+1];
	char    datatype[FE_MAXNAME+1];
	i4	extern_len;
	i4	scale;
	char    nulls[2];
	char    defaults[2];
	i4     seq;
	i4     keyseq;
	char    sort_direction[2];
	char    idatatype[FE_MAXNAME + 1];
	i4     intern_len;
	char    sysmaint[2];
	char    has_default[2];
	char	def_value[1501];
	i2	nulli;			/* only default value can be null */
	char    def_type[11];
	i4	maxstats;
	/* internal ADF datavalue description */
	i2      adf_type;
	i2      intern_prec;
	i4 intern_length;
} QR_COL_INFO;
EXEC SQL END DECLARE SECTION;
