
/*
**	Copyright (c) 2004 Ingres Corporation
*/

/**
** Name:	vifrbft.h - common include for vifred and rbf.
**
** Description:
**	Types used in communication between RBF and VIFRED.
**
** History:
**	??? - written.
**	12-apr-87 (rdesmond) - deleted num_ags from AGG_INFO.
**	16-oct-89 (cmr) -	deleted copt_whenprint, copt_skip
**				from COPT.
**  11/27/89 (martym) - Added the field 'sec_y_count' to the 
**      Sec_node structure, to keep track of the Y positions 
**      within a section.
**	22-nov-89 (cmr)	Added agformat to AGG_INFO, deleted
**			apage, areport and abreak.
**	30-jan-90 (cmr)	Added new field to Sec_node to deal with
**			underlining.
**	19-apr-90 (cmr)	Added new field to COPT (agu_list) which
**			is a linked list of unique agg fields.
**	5/15/90 (martym) Added copt_whenprint, & copt_newpage to COPT.
**	9-mar-1993 (rdrane) Added copt_var_name to COPT.  This introduces a
**			    dependency on fe.h (FE_MAXNAME definition);
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	28-Mar-2005 (lakvi01)
**	    Added function prototypes.
**	09-Apr-2005 (lakvi01)
**	    Moved the declaration of Reset_copt() into #ifdef NT_GENERIC since
**	    it was causing compilation problems on non-windows compilers.
**/

/*
**	The SEC structure contains the Y coordinate of the 
**		starts of the sections of the RBF frame.
*/

typedef struct sec_node
{
	struct sec_node	*next;
	struct sec_node	*prev;
	char		*sec_name;
	int		sec_type;
	int		sec_y;
	int		sec_brkseq;
	i2		sec_brkord;
	int     	sec_y_count;
	char		sec_under;
} Sec_node;

typedef struct
{
	Sec_node	*head;
	Sec_node	*tail;
	int		count;
} Sec_List;


/*
**	The CS structure contains the link between a linked list of
**		field structures and
**		a linked list of trim structures.  There will
**		be an array En_n_attribs i4  of these structures.
*/

typedef struct
{
	LIST	*cs_flist;		/* start of linked list of FIELD
					** structures for this attribute */
	LIST	*cs_tlist;		/* start of linked list of TRIM
					** structures for this attribute */
	char	*cs_name;		/* Name of Attribute  */
}  CS;

/*
**	The AGG_INFO structure contains the information associated with
**		one aggregate function for one attribute of a relation.  
**		In addition there is a pointer to an ADF_AGNAMES structure, 
**		which contains the various names for the aggregate and
**		a prime flag.
*/

typedef struct
{
	ADF_AGNAMES	*agnames;	/* names for aggregate, uniqueness */
	FMT		*agformat;
} AGG_INFO;

/*
**	The COPT structure contains the storage of options associated
**		with an attribute.  These are entered by the user in
**		the column options form and the report structure form.
*/

typedef struct
{
	i2		copt_sequence;	/* Sort sequence for attribute */
	char		copt_order;	/* sort order (a/d) */
	char		copt_break;	/* y/n if attribute is break */
	char            copt_whenprint; /* when to print (b/t/p/a) */
	bool            copt_newpage;   /* do a new page after a break or not */
	char		copt_brkhdr;	/* y/n if break footer */
	char		copt_brkftr;	/* y/n if break footer */
	char		copt_select;	/* selection criteria (v/r/n) */
	AGG_INFO	*agginfo;	/* aggregate information */
	i4		num_ags;	/* number of aggs for attribute */
	LIST		*agu_list;	/* linked list of unique agg fields */
	char		copt_var_name[(FE_MAXNAME + 1)];
					/*
					** Unique name for generated RW
					** variables since the base attribute
					** name may "collapse" (delim id's).
					** Assumes value of an empty string						** unused or not yet set.
					*/
}   COPT;

/*
** Function declarations
*/
#ifdef NT_GENERIC
FUNC_EXTERN VOID	Reset_copt(PTR);
#endif /* NT_GENERIC */
