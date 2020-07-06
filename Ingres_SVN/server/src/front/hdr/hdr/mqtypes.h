# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include <ooclass.h>
# include <oocat.h>
#  include	<mqtf.h>
/**
** Name:	mqtypes.qsh  -  CONSTANTS and STRUCTURES FOR MQBF
**	
**	This file contains the structures used by MQBF 
**	
** History:
**	17-dec-1987 (prs)
**		Integrated bug fix 10328.
**	25-feb-88 (marian)
**		Add mdtype to attribinfo structure.
**	26-may-1989 (Mike S)
**		Add constants for QBF/Emerald form generation
**		Move ATTRIBINFO constants here from mqstates.h
**	10-jul-89 (Mike S)
**		Add MQTF structure
**	9-aug-89 (Mike S)
**		Add flags fields to MQTF
**	5-mar-90 (Mike S)
**		Move MQTF definition to mqtf.h
**	1-oct-90 (esd)
**		Add lkeyinfo to ATTRIBINFO structure.
**	11-oct-90 (esd)
**		Add join_att and join_seqno to ATTRIBINFO structure.
**	30-jul-1993 (rdrane)
**		Add mqappendok to MQQDESC structure as part of fix for b51411.
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# ifdef		SEINGRES
# define	REDUCE
# endif
# ifdef		PCINGRES
# define	REDUCE
# endif
# ifndef REDUCE
# define	MQ_MAXRELS	10	/* max number of tables allowed
					   in one mqbf instance */
# define	MQ_MAXJOINS	50	/* max number of joins allowed
					   in one mqbf instance */
# define	MQ_MAXATTS	(2 * DB_GW2_MAX_COLS) 
					/* max number of attributes
					   retrieved in one mqbf instance */
# define	MAXFLNM		64
# define	MQ_MAXQRYSPEC	2000	/* max no. QRY_SPECs per list */
# define	MQ_MAXCOLS	DB_GW2_MAX_COLS
# define	mqmaxrows	100
# define	SEQ_OFFSET	256	/* offset (8 bits) for sequence
					** number within qtype in iiqbfinfo
					** table. */
# else
# define	MQ_MAXRELS	6	/* max number of tables allowed
					   in one mqbf instance */
# define	MQ_MAXJOINS	30	/* max number of joins allowed
					   in one mqbf instance */
# ifdef	PCINGRES
# define	MQ_MAXATTS	128	/* max number of attributes
					   retrieved in one mqbf instance */
# else
# define	MQ_MAXATTS	50	/* max number of attributes
					   retrieved in one mqbf instance */
# endif	/* PCINGRES */
# define	MAXFLNM		64
# define	MQ_MAXSIZ	2048	/* max record size */
# define	MQ_WSIZE	1500	/* max qual cause */
# ifdef	PCINGRES
# define	MQ_MAXCOLS	50
# else
# define	MQ_MAXCOLS	50
# endif	/* PCINGRES */
# define	mqmaxrows	50
# define	SEQ_OFFSET	256	/* offset (8 bits) for sequence
					** number within qtype in iiqbfinfo
					** table. */
# endif
# define	MQ_HIDDENMAX	2000	/* max length of hidden column string */
# define	MQ_MAXHIDECOLS	60	/* max number of hidden columns in QBF*/
  typedef struct relinfo {
    char mqrelid[FE_MAXNAME + 1];
    char mqrangevar[FE_MAXNAME + 1];
    char mqrelown[FE_MAXNAME + 1];
    i2 mqtype;
    i4 opdelete;
    char *mquparam;
    DB_DATA_VALUE **mquargv;
    char *mqaparam;
    DB_DATA_VALUE **mqaargv;
    i4 mqrecl;
    DB_DATA_VALUE mqmtid;
	char	*mqdparam;			/* param string for deletes */
	DB_DATA_VALUE	**mqdargv;		/* arguments for deletes */
	i4	keytype;			/* key type used for updates */
  } RELINFO;
  typedef struct attribinfo {
    char rvar[FE_MAXNAME+1];
    char col[FE_MAXNAME+1];
    char formfield[FE_MAXNAME+1];
    i2 ttype;
    DB_DATA_VALUE dbv;
    DB_DATA_VALUE *rdbv;
    i2 opupdate;
    i2 jpart;
    i2 intbl;
    i2 found;
	i2	keyinfo;    		/* true means attrib is part of key */
    i2 mdtype;
    i2 mandfld;
	i2	lkeyinfo;    		/* logical key information */
	i2	join_seqno;    		/* sequence number indicating the
					** relative position of this attribute
					** in the list of joins.  irrelevant if
					** this attribute isn't joined by QBF */
	struct attribinfo *join_att;	/* if this attribute is joined by QBF,
					** points to the primary join attribute
					** to which this attribute is joined
					** (either directly or indirectly).
					** while the join list is being built,
					** join_att is used to link a group of
					** joined attr's into a circular list.
					** if this attr isn't joined by QBF,
					** its join_att points to itself. */
  } ATTRIBINFO;
  typedef struct joininfo {
    char rv1[FE_MAXNAME + 1];
    char col1[FE_MAXNAME + 1];
    char rv2[FE_MAXNAME + 1];
    char col2[FE_MAXNAME + 1];
    char jcode[3];
  } JOININFO;
  typedef struct qdefinfo {
    i4 d_tblfield;
    i4 d_qdef_type;
    i4 d_mq_func;
    bool d_mq_debug;
    bool d_mq_deffrm;
    i4 d_tblwidth;
    char d_mq_frame[FE_MAXNAME+1];
    char d_mq_tbl[FE_MAXNAME+1];
    char d_mqflnm[MAXFLNM];
    bool d_onetbl;
    i4 d_any_views;
    i4 d_any_cats;
    i4 d_mqnummasters;
    i4 d_mqnumdetails;
    bool d_mq_name;
  } QDEFINFO;
  typedef struct mqqdef {
    OO_CATREC mqcr;
    RELINFO *mqtabs[MQ_MAXRELS];
    ATTRIBINFO *mqatts[MQ_MAXATTS];
    JOININFO *mqjoins[MQ_MAXJOINS];
    char *mqmpform;
    char **mqmpargv;
    char *mqdpform;
    char **mqdpargv;
    char *mqmgform;
    char **mqmgargv;
    char *mqdgform;
    char **mqdgargv;
    i4 mqmrecl;
    i4 mqdrecl;
	QDESC		*mqqdesc;	/* param RETRIEVE structure */
    i4 mqnumtabs;
    i4 mqnumatts;
    i4 mqnumjoins;
    bool mqdeadtabs[MQ_MAXRELS];
    QDEFINFO *mqdata;
    i2 mqtag;
    bool mqappendok;
  } MQQDEF;
/*  Frame commands */
# define	mqopENDF 	00			/* end of frame */
/* Form gneration type */
# define	MQFORM_QBF	0			/* QBF form */
# define	MQFORM_VISION	1			/* Emerald form */
/* indicates states of various ATTRIBINFO fields */
/* jpart */
# define	mqHIDDEN_JOIN	0	/* Subordinate join field */
# define	mqJOIN_FIELD	1	/* Primary master-detail join field */
# define	mqLU_JOIN_FIELD	2	/* Primary lookup join field (Emerald)*/
# define	mq2_JOIN_FIELD	3	/* Primary master-detail and lookup join field (Emerald)*/
# define	mqNOT_JOIN	-1	/* No join on field */
/* ttype, mdtype */
# define	mqIN_MASTER	0	/* In master table */
# define	mqIN_DETAIL	1	/* In detail table */
# define	mqIN_LU_MASTER	2	/* In lookup master table (Emerald) */
# define	mqIN_LU_DETAIL	3	/* In detail lookup table (Emerald) */
/* intbl */
# define	mqIN_TABLE	1	/* In table field */
/* lkeyinfo (bit mask) */
# define	mqTABLE_KEY	1	/* table_key logical key */
# define	mqOBJECT_KEY	2	/* object_key logical key */
# define	mqSYSTEM_MAINT	4	/* system-maintained logical key */
/* states of global variable qdef_type */
# define	mqMD_JOIN	0	/* Master-detail join */
# define	mq1to1_JOIN	1	/* Master-master join */
