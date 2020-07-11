# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<st.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ergr.h>
# include	<er.h>
# include	<me.h>
# include	<ug.h>
# include	<ui.h>
# include	<lo.h>
# include	<si.h>
# include	<adf.h>
# include	<afe.h>
# include	<ooclass.h>
# include	<graf.h>
# include	<grmap.h>
# include	<dvect.h>
# include	<grerr.h>
# include	"qry.h"
# include	"grcoerce.h"
/**
** Name:    mapview.qc	-	Retrieve and connect data to a graph
**
** Description:
**
**
** History:
**	7/30/86 (bruceb) fix for bug 9173.  Dates prior to 1902 (not 1901)
**		are removed from the retrieved data since can't be
**		graphed anyway (VE limitation).	 Warning message is
**		printed.
**	8/4/86 (bruceb) fix for bug 9331.  Sparse data in first series
**		no longer affects label ordering or visibility.	 Sorted
**		data is now retrieved from the db sorted only on X, not
**		on Z, X.  Thus, X values are properly ordered.	Z ordering
**		is handled by sorting the zarray before reading back in
**		the temporary file.
**	8/9/86 (bruceb) correction of fix for 9331.  Sort Z based on
**		numerics if not a string column.  Bad new to have
**		14-jul-86 < 4-feb-86, or 120 < 86.
**	9/17/86 (bruceb) changed so that grfmt only uses
**		colptr->data if (effectively) colptr is z_col;
**		memory has not been allocated otherwise (and A.V.)
**	10/20/86 (sandyd)  Removed the retrieve of "_version" because of
**		SATURN limitations.  Replaced it with reference to "_Version"
**		global set by FEingres();
**	13-oct-86 (bruceb) Fix for bug 9246
**		In getmaxima(), set max_pts to min(max_pts, MAX_BAR) when
**		bar chart with numeric data.  Previously, data would
**		be truncated with out the warning message to that affect.
**	2/87 (bobm) Changes for ADT and NULL support.
**	20-may-88 (bruceb)
**		Changed FEtalloc call into FEreqmem call.  Also, changed
**		loc_alloc interface so that it returns a PTR rather than
**		passing back the allocated memory via a parameter.
**		(For DG.)
**	5-Jun-89 (Mike S)
**		Return GD_NOROWS for an empty table.
**	01-oct-91 (jillb/jroy--DGC)
**		make function declarations consistent with their definitions
**      21-Aug-92 (kevinm) B44150
**		Backed out the previous fix for this bug.  We now call
**	 	ME_ALIGN_MACRO to determine the amount to offset for 
**		alignment.
**	03-Sep-92 (ricka)
**		ME_ALIGN_MACRO requires an "include <me.h>" above.
**	10/02/92 (dkh) - Added support for owner.table and delim ids.
**	26-Aug-1993 (fredv)
**		Included <st.h>.
**      11-jan-1996 (toumi01; from 1.1 axp_osf port)
**              Added casts for args to FEmsg() for axp_osf
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# define	MAXC	  3	/* max no. cols written/read from file */
# define	MAXWARN	  7	/* Max no. warnings returned */
# define	DSTRLEN	  48	/* Length for date string as text */
# define	MAXSTRLEN 100	/* Length for integers, floats as text */
/*
** format strings for parameterized target lists
** and sort clause.  Note implicit use of range variable "r".
*/
# define	SCFORM	  ERx("RET_VAR%d")
# define	TRGFORM	  ERx("%%a=r.%s")
# define	TRGCNCT	  ERx(", ")
/*
** character space required for length sort clause and target strings.
** Using size of format strings will generate a few extra bytes
** because we're counting the format characters, but so what.  TARGSLEN
** is the per-column increment (again, a few extra bytes).
*/
# define	SORTSLEN  (sizeof(SCFORM)+8)
# define	TARGSLEN  (sizeof(TRGFORM)+sizeof(TRGCNCT)+FE_MAXNAME)
typedef struct
{
	char	*str;		/* x string value */
	i4	val;		/* numeric string identifier */
} STDESC;
typedef struct
{
	DB_DATA_VALUE	db;		/* actual z data */
	i4		z_code;		/* int assigned to this 'z' value */
	i4		numrecs;	/* number of rows with this 'z' value */
	char		*zdesc;		/* series description string */
} ZDESC;
typedef struct xel
{
	struct	xel	*x_next;	/* next in x-list */
	struct	xel	*x_prev;	/* previous in x-list */
	i4	x_ser[ MAX_DS ];	/* series array for this x */
	DB_DATA_VALUE	db;
} X_EL;
STATUS	map_view();
static STATUS	eqsetup();
static VOID	xyzmap();
static STATUS	find_ds();
static VOID	datastore();
static STATUS	getident();
char	*grgennm();
static STATUS	chkmap();
static VOID	getchart();
static VOID	statinit();
static STATUS	getdata();
static VOID	getfields();
static STATUS	getmaxima();
static VOID	addwarn();
static STATUS	lookupz();
static VOID	wrtotmp();
static bool	rdfrtmp();
static STATUS	writexyz();
static VOID	listx();
static bool	lookupx();
static bool	findx();
static VOID	fillinx();
static VOID	sortx();
static VOID	bubble();
static VOID	sortz();
static VOID	bubblez();
static bool	okrow();
static VOID	loc_compare();
static PTR	loc_alloc();
static VOID	loc_isnull();
VOID	zero_ser();
GLOBALDEF char iigrBadColName[FE_MAXNAME+1];
extern	ADF_CB	*GR_cb;
FUNC_EXTERN	GR_DVEC		*GTdvfetch();
FUNC_EXTERN	GR_DVEC		*GTdvcreate();
FUNC_EXTERN	char		*GRtxtstore();
FUNC_EXTERN	VOID		GRcoerce_type();
FUNC_EXTERN	VOID		GRzero_pt();
FUNC_EXTERN	char		*FEtalloc();
FUNC_EXTERN	COLDATA		*qry_col();
FUNC_EXTERN	TABLEATT	*qry_ftab();
FUNC_EXTERN	STATUS		qry_addtab();
FUNC_EXTERN	VOID		qry_dalltab();
FUNC_EXTERN	char		*qry_fstr();
FUNC_EXTERN	i4		(*IIseterr())();
static	i4	nwarn = 0;		/* No. of warnings so far */
static	i4	warnings[ MAXWARN + 1]; /* Array of warning msg no's */
static	OOID	chartclass = GROB_NULL; /* Type of chart being mapped */
static	i4	maxdep = 0;		/* max no. dependent series */
static	GR_OBJ	*chartobj = NULL;	/* chart being mapped */
static	i4	max_strs = 0;	/* Max no. strings for this chart class */
static	i4	max_pts = 0;	/* Number of rows in view being mapped */
static	i2	deftag = 0;	/* map_view control structures tag */
static	STDESC	xstrings[ MAX_STRINGS ];	/* array of x-string vals */
static	i4	n_xstr = 0;	/* Number of x_string values so far */
static	i4	xid = 0;	/* Last used xstring identifier */
static	STDESC	ystrings[ MAX_STRINGS ];	/* array of y-string vals */
static	i4	n_ystr = 0;	/* Number of y_string values so far */
static	i4	yid = 0;	/* Last used ystring identifier */
static	COLDATA *x_col = NULL;	/* column descriptor for x axis */
static	COLDATA *y_col = NULL;	/* column descriptor for y axis */
static	COLDATA *z_col = NULL;	/* column descriptor for z axis */
static	LOCATION	*locptr;
static	LOCATION	tmploc;	 /* location for stream read/write file */
static	FILE		*fileptr;
static	ZDESC	zarray[ MAX_DS ];	/* array of Z column descriptors */
static	i4	zused;			/* number of elements in zarray */
static	bool	zsort;			/* true if sorting on series values */
static	DB_DATA_VALUE	miny;		/* minimum Y-val if date */
static	GRC_STORE	miny_store;	/* minimum Y-val data area */
static	bool	dense;			/* TRUE if dense matrix required */
static	X_EL	*xlist;			/* list of unique x-values */
static	i4	nxes;			/* no. entries in xlist */
/*{
** Name:	map_view	-	 Map data from a view/table to graph
**
** Description:
**	Called when the viewname of the map structure is not null.
**	This is the only public entry point in this file.
**
** Inputs:
**	frame		Pointer to graph frame
**	map		Pointer to map structure
**	retwarn		Returned with pointer to a 0-delimited array
**			of non-fatal warning message numbers.
**
** Outputs:
**	Returns:
**		STATUS - OK
**			 GDNOTAB
**			 GDNOX
**			 GDNOY
**			 GDNOZ
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	16-dec-85 (drh) First written.
**	2/87 (bobm)	changes for ADT / NULL support.	 In this routine,
**			mostly changed memory allocation to free local
**			control structures before leaving / modified
**			interface to qry_ utilities.
**	2/27/89 (Mike S) Call getdata with correct number of args, even for 
**			pie charts.
*/
STATUS
map_view( frame, map, retwarn )
GR_FRAME	*frame;
GR_MAP		*map;
i4		**retwarn;
{
	TABLEATT	*tabptr = NULL;
	TABLEATT	*oldptr = NULL;
	GR_TEXT		*lab_ptr = NULL;
	PIECHART	*pieptr = NULL;
	BARCHART	*barptr = NULL;
	LINECHART	*lineptr = NULL;
	SCATCHART	*scatptr = NULL;
	DEPDAT		*depptr = NULL;
	i2		nds = 0;
	STATUS		retstat = OK;
	i4		n_col_ret;
	i4		oldtype;
	DB_DATA_VALUE   *target_addr[DB_GW2_MAX_COLS];
	char		target_list[DB_GW2_MAX_COLS * FE_MAXNAME];
	char		sort_buf[FE_MAXNAME + 1];
	*retwarn = warnings;
	/*
	**  Free all existing data vectors, and storage used in mapping
	*/
	GTdvrestart();
	deftag = FEgettag();
	/*
	**  Initialize statics
	*/
	statinit();
	/*
	**  Get information about chart class
	*/
	getchart( frame );
	/* Get info about the table/view to map.  Delet old one first. */
	qry_dalltab();
	if ( ( qry_addtab( map->gr_view ) ) != OK )
	{
		FEfree( deftag );
		return( GDNOTAB );
	}
	tabptr = qry_ftab( map->gr_view, &oldptr );
	/*
	**  Check that mapped columns exist in the view
	**  Flag tabptr entries with their usage.
	**  Allocate target string / var using number of columns queried.
	*/
	if ( ( retstat = ( chkmap( tabptr, map, frame, &n_col_ret ) ) ) != OK )
	{
		FEfree( deftag );
		return( retstat );
	}
	/*
	**  Get the number of rows in the table/view to be retrieved
	*/
	if ( ( retstat = ( getmaxima( map, tabptr ) ) ) != OK )
	{
		FEfree( deftag );
		return( retstat );
	}
	if ((retstat = eqsetup(target_addr, target_list, map, tabptr, sort_buf))
		!= OK)
	{
		FEfree( deftag );
		return ( retstat );
	}
	/*
	**  Build data vectors
	*/
	switch ( chartclass )
	{
	   case GROB_PIE:
		/*
		** for pie charts, we coerce all x types into GRDV_STRING
		** we set it back before exit so we have the "proper"
		** coercion in memory in case we ever decide to avoid
		** unneccesary table fetches in qry_util when we remap
		** with the same view.
		*/
		oldtype = x_col->type;
		x_col->type = GRDV_STRING;
		pieptr = ( PIECHART * )(chartobj->attr );
		depptr = &pieptr->ds;
		depptr->field = NULL;
		if ( ( retstat = ( getdata( target_addr, target_list, sort_buf, 
			&pieptr->ds, &nds, frame, tabptr ) )) != OK )
		{
			x_col->type = oldtype;
			FEfree( deftag );
			return( retstat );
		}
		x_col->type = oldtype;
		pieptr->lab.l_field = pieptr->ds.xname;
		break;
	   case GROB_BAR:
		barptr = ( BARCHART * )(chartobj->attr );
		barptr->ds_num = 0;
		depptr = barptr->ds;
		depptr->field = NULL;
		if ( ( retstat = ( getdata( target_addr, target_list,
			sort_buf, barptr->ds,
			&(barptr->ds_num), frame, tabptr ))) != OK )
		{
			FEfree( deftag );
			return( retstat );
		}
		zero_ser(barptr->ds,&(barptr->ds_num));
		break;
	   case GROB_SCAT:
		scatptr = ( SCATCHART * )(chartobj->attr );
		scatptr->ds_num = 0;
		depptr = scatptr->ds;
		depptr->field = NULL;
		if ( ( retstat = ( getdata( target_addr, target_list,
			sort_buf, scatptr->ds,
			&(scatptr->ds_num), frame, tabptr ) )) != OK )
		{
			FEfree( deftag );
			return( retstat );
		}
		break;
	   case GROB_LINE:
		lineptr = ( LINECHART * )(chartobj->attr );
		lineptr->ds_num = 0;
		depptr = lineptr->ds;
		depptr->field = NULL;
		if ( ( retstat = ( getdata( target_addr, target_list,
			sort_buf, lineptr->ds,
			&(lineptr->ds_num), frame, tabptr ) )) != OK )
		{
			FEfree( deftag );
			return( retstat );
		}
		break;
	  default:		/* no chart */
		if ( ( retstat = ( getdata( target_addr, target_list,
			sort_buf, NULL,
			NULL, frame, tabptr ) )) != OK )
		{
			FEfree( deftag );
			return( retstat );
		}
		break;
	}
	FEfree( deftag );
	return( retstat );
}
/*{
** Name:	zero_ser	- remove zero-length series from the data.
**
** Description:
**	Post-scans constructed data sets, compressing out zero-length
**	series.  These can result from length limitations when constructing
**	sparsely populated barcharts.  User has already been given a
**	warning message that there are too many data points to plot.
**
** Inputs:
**	dsarr - dataset array.
**	dsnum - two-directional parameter containing intial size,
**		which may be decremented.
**
** History:
**	11/21/90 (bobm)	written
**
*/
VOID
zero_ser(dsarr, dsnum)
DEPDAT	dsarr[];
i2	*dsnum;
{
	i4	i, j;
	GR_DVEC	*v;
	for (i=0; i < *dsnum; ++i)
	{
		v = GTdvfetch(dsarr[i].xname);
		if (v != NULL && v->len == 0)
		{
			--(*dsnum);
			for (j=i+1; j < *dsnum; ++j)
				STRUCT_ASSIGN_MACRO(dsarr[j],dsarr[j-1]);
			--i;
		}
	}
}
/*{
** Name:	eqsetup		-	Set up for equel retrieve
**
** Description:
**	Build a target list, variable array, and optional sort string.
**
** Inputs:
**	map		Pointer to map structure
**	tabptr		Pointer to table descriptor
**
** Outputs:
**	targ		Returned target string
**	var		Returned variable array to retrieve
**	sort		Returned sort string
**	Returns:
**		OK
**		GDBADTBL
**		GDBADCOL
**	Exceptions:
**
** Side Effects:
**
** History:
**	2/87 (bobm)	basically rewritten for using ADT's.
**	4-aug-1988 (danielt)
**		rewrote to set up QDESC structure for use
**		with IIQG_generate() rather than set up 
**		param retrieve structures.
**	15-Feb-89 (Mike S)
**		Fixed off-by-one error in column loop.  Init returned strings
**		to blanks.
**	4/90 (Mike S)
**		Check for SQL-keyword table and column names.
*/
static STATUS
eqsetup( t_addr, qbuf, map, tabptr, sort_buf)
DB_DATA_VALUE	*t_addr[];
char		*qbuf;
GR_MAP		*map;
TABLEATT	*tabptr;
char		*sort_buf;
{
	i4 		i, xnum;
	char	*ptr;
	char	quoted[FE_UNRML_MAXNAME + 1];
	*qbuf = EOS;
	*sort_buf = EOS;
	xnum = 0;
	for (i=0; i < tabptr->colnum; ++i)
	{
		if ((tabptr->col)[i].qflag && (tabptr->col)[i].supported)
		{
			if ((tabptr->col)[i].xflag)
				xnum = i+1;
			if (i != 0)
			{
				STcat(qbuf,ERx(", ")); 
			}
			t_addr[i] = &(tabptr->col[i].db);
			ptr = tabptr->col[i].name;
			if (IIUGdlm_ChkdlmBEobject(ptr, NULL, TRUE) ==
				UI_DELIM_ID)
			{
				IIUGrqd_Requote_dlm(ptr, quoted);
				ptr = quoted;
			}
			STcat(qbuf, ptr);
		}
	}
	if (xnum <= 0)
		IIUGerr(S_GR0014_x_flag, UG_ERR_FATAL, 0);
	/*
	**  Build a sort clause for X, if this is a sorted graph.
	**  Z will be sorted later, so merely indicate the necessity.
	*/
	if (!(map->flags & MAP_NOSORT))
	{
		ptr = tabptr->col[xnum - 1].name;
		if (IIUGdlm_ChkdlmBEobject(ptr, NULL, TRUE) == UI_DELIM_ID)
		{
			IIUGrqd_Requote_dlm(ptr, quoted);
			ptr = quoted;
		}
		STcat(sort_buf, ptr);
		if (z_col != NULL)
			zsort = TRUE;
		else
			zsort = FALSE;
	}
	else
	{
		zsort = FALSE;
	}
	return OK;
}
/*{
** Name:	xyzmap		- Build dependent series for xyz model
**
** Description:
**	Reads the temporary file containing the x, y, and z data, and
**	builds dependent data series (depdat(s)) for each value to
**	be plotted.
**
**	If this is a graph with no chart (i.e. just fields), it simply
**	closes the temporary data file and deletes it.
**
**	Note that the logic is slightly different if this is a bar chart
**	that requires a dense matrix.
**
** Inputs:
**	dsarray		Array of depdats to be filled in
**
** Outputs:
**	None.
**	Returns:
**		VOID
**	Exceptions:
**
** Side Effects:
**
** History:
**	16-dec-85 (drh) First written.
**	2/87 (bobm)	ADT changes - use of DB_DATA_VALUE's.
*/
static VOID
xyzmap( dsarray )
DEPDAT	dsarray[];
{
	DB_DATA_VALUE	xd;
	DB_DATA_VALUE	yd;
	DB_DATA_VALUE	zd;
	i4	xall;
	i4	yall;
	i4	zall;
	ZDESC *zptr;
	DEPDAT	*depptr;
	GR_DVEC *arrptr = NULL;
	i4	cnt = 0;
	i4	zidx = 0;
	i4	npoints = 0;
	i4	seriesno = 0;
	i4	xno = 0;
	bool	storeit = FALSE;
	i4 i;
	/* initialize depdat desc's to NULL for use in find_ds */
	for (i=0; i < maxdep; ++i)
		dsarray[i].desc = NULL;
	/*
	**  Close the temporary file containing the xyz data and open
	**  it for reading
	*/
	if ( SIclose( fileptr ) != OK )
		IIUGerr(S_GR0015_tmpfil_close, UG_ERR_FATAL, 0);
	if ( ( SIopen( locptr, ERx("r"), &fileptr ) ) != OK )
		IIUGerr(S_GR0016_tmpfil_open, UG_ERR_FATAL, 0);
	/*
	**  If a dense matrix is to be built (for bar charts), sort the
	**  list of unique x values so that the data vectors will be
	**  built in sorted order.
	*/
	if ( dense == TRUE )
		sortx();
	/*
	**  Read xyz records from the temporary file and add them to
	**  the appropriate data vectors.  The allocation length arguments
	**  are used by rdfrtmp to keep large enough buffers allocated using
	**  deftag for the returned types.  We start at 0, and they are
	**  adjusted upwards, as larger buffers are needed.  Remember the
	**  use of the z DB_DATA_VALUE to contain the "z code" as a type.
	*/
	xall = yall = zall = 0;
	while (rdfrtmp(&xd,&xall) && rdfrtmp(&yd,&yall) && rdfrtmp(&zd,&zall))
	{
		zidx = zd.db_datatype;	/* index into the z array */
		zptr = zarray + zidx;	/* z array element for this z value */
# ifdef DEBUG
		FEmsg(ERx("Just read Z array index %d"),(bool) TRUE, (PTR)zidx);
# endif
		/*  Get the dependent series descriptor to store in */
		if ( ( find_ds( zptr, dsarray, &depptr, &seriesno )) != OK)
			continue;	/* skip this xyz record */
		/* Get the x_value ready to store */
		arrptr = GTdvfetch( depptr->xname );	/* x data array ptr */
		if ( dense == TRUE )		/* need to build dense matrix */
		{
			if ( ( lookupx( &xd, seriesno, &storeit, &xno ) )
				!= TRUE )
			{
# ifdef DEBUG
				FEmsg(ERx("LOOKUPX - couldn't find X"), (bool) TRUE );
# endif
				continue;
			}
			if ( storeit == TRUE )	/* if not a duplicate point */
			{
				/*  Store the X value */
				datastore( &xd, arrptr, 'x', xno );
				arrptr->len = nxes;
				/*  Store the Y value */
				arrptr = GTdvfetch( depptr->field );
				datastore( &yd, arrptr, 'y', xno );
				arrptr->len = nxes;
			}
		}
		else				/* not dense matrix */
		{
			/*  Check that max no. points won't be exceeded */
			npoints = arrptr->len + 1;
			if ( npoints > max_pts )
			{
				addwarn( MAX_DATA );
				continue;
			}
			/*  Store the X value */
			datastore( &xd, arrptr, 'x', arrptr->len );
			arrptr->len = npoints;
			/*  Store the Y value */
			arrptr = GTdvfetch( depptr->field );
			datastore( &yd, arrptr, 'y', arrptr->len );
			arrptr->len = arrptr->len + 1;
		}
	}
	if ( SIclose( fileptr ) != OK )
		IIUGerr(S_GR0017_tmpfil_close, UG_ERR_FATAL, 0);
	LOdelete( locptr );
	/*
	**  If we are building a dense matrix for a bar chart, fill in any
	**  missing X/Z points now.
	*/
	if ( dense == TRUE )
	{
		fillinx( dsarray, zused);
	}
	return;
}
/*{
** Name:	find_ds		-	Find dependent data series
**
** Description:
**	Find the appropriate dependent series descriptor for the
**	current selector series value.	If there is no selector
**	series, always use the first (and only) one.  Otherwise,
**	compare the current Z value to each dependent series descriptor
**	(depdat.desc) to find the matching one.	 If none is found, create
**	a new data series (GTdvcreate), and store info about in the 'next'
**	depdat in the depdat array.
**
** Inputs:
**	zptr		Pointer to z-array element to use
**	dsarray		Array of dependent series descriptors to look in
**
**
** Outputs:
**	deptouse	Pointe to the depdat element for this series.
**	seriesno	0-based number of the series in the depdat array.
**
**	Returns:
**		STATUS
**	Exceptions:
**
**
** Side Effects:
**
**	May set MAX_DEP in the warning array if the maximum number of
**	dependent series is exceeded.
**
** History:
**	16-dec-85 (drh) First written.
**	02-apr-86 (drh) Modified to return seriesno for dense matrix.
**	8/4/86 (bab)	modified to use 'i' as an indirect index through
**		zarray rather than as a direct indication of dataset number.
**	2/87 (bobm)	ADT changes - DB_DATA_VALUE's in ZDESC, plus
**			simplified logic.
*/
static STATUS
find_ds( zptr, dsarray, deptouse, seriesno )
ZDESC	*zptr;
DEPDAT	dsarray[];
DEPDAT	**deptouse;
i4	*seriesno;
{
	GR_DVEC *grptr;
	DEPDAT	*depptr;
	char	bufr[35];
	i4	i;
	i4	ds_no;
	i4	vecsize;
	/*
	**  Establish number of data points the data vector will hold
	**  for allocation
	*/
	if ( dense == TRUE )
		vecsize = nxes;
	else
		vecsize = zptr->numrecs;
	if ((ds_no = zptr->z_code) > (maxdep-1))
		return( FAIL );
	*seriesno = ds_no;
	depptr = dsarray + ds_no;
	*deptouse = depptr;
	/*  If no description for DEPDAT, fill in */
	if ( depptr->desc == NULL )
	{
		if (z_col == NULL)
			depptr->desc = GRtxtstore( y_col->name );
		else
			depptr->desc = zptr->zdesc;
		STprintf(bufr,ERx("y%d"),ds_no);
		depptr->field = GRtxtstore(bufr);
		grptr = GTdvcreate( depptr->field, y_col->type, vecsize );
		grptr->len = 0;
		STprintf(bufr,ERx("x%d"),ds_no);
		depptr->xname = GRtxtstore(bufr);
		grptr = GTdvcreate( depptr->xname, x_col->type, vecsize );
		grptr->len = 0;
	}
	return( OK );
}
/*{
** Name:	datastore	-	Store data value in vector
**
** Description:
**	Stores data value in the graphics data vector passed in as
**	a parameter.   Since the data vector is an array, uses the
**	'where' parameter passed in by the caller to index to the
**	array cell where the value should be stored.
**	The only additional interesting thing this routine
**	does is, if the data value is a 'string' type, it calls another
**	routine to assign a numeric value to the string.
**	The DB_DATA_VALUE passed to this routine is one that has been
**	coerced by GRcoerce_type to the type of the data vector.  This
**	should be the one place that we really take data out of the
**	DB_DATA_VALUE.
**
**
** Inputs:
**	val		Value to be stored in the data vector
**	dvec		Data vector to store it in
**	which		'x' or 'y'
**	where		0-based index into the data vector where this
**			  value should be stored.
**
** Outputs:
**	Returns:
**		VOID
**	Exceptions:
**
**
** Side Effects:
**
**	If the data value being stored is a 'string' type, potentially
**	adds it to the string list.
**
** History:
**	13-dec-85 (drh) First written.
**	02-apr-86 (drh) Modified to use 'where' to determine array cell to
**			store the value in.
**	2/87 (bobm)	ADT changes - use DB_DATA_VALUES.  Also change
**			string storage allocation to use GRtxtstore, so
**			we don't have DVECT data pointing at internal
**			allocation.
*/
static VOID
datastore( val, dvec, which, where )
DB_DATA_VALUE	*val;
GR_DVEC *dvec;
char	which;
i4	where;
{
	char	*strptr;
	i4 erarg;
	DB_TEXT_STRING	*sptr;
	switch ( dvec->type & DVEC_MASK )
	{
	case DVEC_I:
	  dvec->ar.idat[ where ] = *((i4 *)(val->db_data));
	  break;
	case DVEC_F:
	  dvec->ar.fdat[ where ] = *((f8 *)(val->db_data));
	  break;
	case DVEC_S:
	  sptr = (DB_TEXT_STRING *)(val->db_data);
	  sptr->db_t_text[sptr->db_t_count] = EOS;	/* paranoia */
	  strptr = GRtxtstore (sptr->db_t_text);
	  if ( ( getident( which,strptr, &dvec->ar.sdat.val[ where ] )) == OK )
	  {
		dvec->ar.sdat.str[ where ] = strptr;
	  }
	  break;
	default:
	  erarg = dvec->type;
	  IIUGerr(S_GR0018_ds_bad_type, UG_ERR_FATAL, 1, &erarg);
	  break;
	}
}
/*{
** Name:	getident	-	Get identifier for a string value
**
** Description:
**	Since it is possible for either the x- or y-axis to be 'strings',
**	this routine keeps track of all the 'string' values encountered
**	for each axis, assigning a numeric identifier to each string.
**
**	C-chart will plot the numeric values, using the 'strings' as labels.
**
** Inputs:
**	which		'x' or 'y' axis
**	strptr		the string to get an identifier
**	idnum		returning with the identifier number
**
** Outputs:
**	Returns:
**		STATUS - OK or FAIL if the max no. strings is exceeded.
**	Exceptions:
**
** Side Effects:
**
**	May set MAX_STR in the warning array when the maximum number of
**	strings for a string axis (x or y) is exceeded.
**
** History:
*/
static STATUS
getident( which, strptr, idnum )
char	which;
char	*strptr;
i4	*idnum;
{
	i4	retval = OK;
	i4	i;
	bool	found = FALSE;
	*idnum = 0;
	if ( which == 'x' )	/* this is an x-value */
	{
		for ( i = 0; found == FALSE && i < n_xstr; i++ )
		{
			if ( ( STcompare( strptr, xstrings[i].str ) ) == 0 )
				found = TRUE;
		}
		if ( found == TRUE )
		{
			*idnum = xstrings[(--i)].val;
		}
		else
		{
			if (i >= max_strs )
			{
				addwarn( (i4) MAX_STR );
				retval = FAIL;
			}
			else
			{
				xstrings[i].str = strptr;
				xstrings[i].val = ++xid;
				n_xstr++;
				*idnum = xstrings[i].val;
			}
		}
	}
	else		/* this is a y-value */
	{
		for ( i = 0; found == FALSE && i < n_ystr; i++ )
		{
			if ( ( STcompare( strptr, ystrings[i].str ) ) == 0 )
				found = TRUE;
		}
		if ( found == TRUE )
			*idnum = ystrings[--i].val;
		else
		{
			if ( i >= max_strs )
			{
				addwarn( (i4) MAX_STR );
				retval = FAIL;
			}
			else
			{
				ystrings[i].str = strptr;
				ystrings[i].val = ++yid;
				n_ystr++;
				*idnum = ystrings[i].val;
			}
		}
	}
	return( retval );
}
/*{
** Name:	chkmap		- Check that mapped columns exist
**
** Description:
**	Check that (1) a name of an existing column in the view has
**	been provided for the independent and dependent axes, and (2)
**	for all other columns that have been named, the column exists in
**	the view.
**
**	Set the qflag / fld items for column.  Note that we simply retrieve
**	all columns (qflag = TRUE).  This makes handling new fields easier.
**	If we were to be smarter about this, this routine would set qflags
**	selectively, and return the number it actually set.  We just return
**	the number of columns.
**
** Inputs:
**	tabptr		Pointer to a table descriptor (TABLEATT)
**	map		Pointer to the map descriptor
**
** Outputs:
**	ncolret		Number of columns to be retrieved
**	Returns:
**		STATUS - OK
**			 GDNOX
**			 GDNOY
**			 GDNOZ
**			 GDXDTYPE
**			 GDYDTYPE
**			 GDZDTYPE
**	Exceptions:
**
** Side Effects:
**
**
** History:
**	17-dec-85 (drh) First written.
**	08-apr-86 (drh) Eliminated check on sort columns, since 'sort' is
**			now a yes/no condition that always sorts on the
**			Z and X columns.
**	2/87 (bobm)	Because of ADT's we no longer retrieve columns
**			multiple times - we can return an accurate
**			ncolret.  New flags on COLDATA structure, removing
**			old irrelevent ones.
*/
static STATUS
chkmap( tabptr, map, frame, ncolret )
TABLEATT	*tabptr;
GR_MAP		*map;
GR_FRAME	*frame;
i4		*ncolret;
{
	COLDATA *colptr;
	i4	ndates;
	i4	i;
	GR_OBJ	*comp;
	GR_TEXT *aptr;
	/* initialize the column flags */
	for (i=0; i < tabptr->colnum; ++i)
	{
		(tabptr->col)[i].fld = FALSE;
		(tabptr->col)[i].qflag = TRUE;
		(tabptr->col)[i].xflag = FALSE;
	}
	/*
	** First the independent axis column - required
	** For pie charts, the type item is set to GRDV_STRING,
	** WHATEVER the type of the column.  The "type" item tells
	** what we coerce into.
	*/
	if ( map->indep == NULL || *(map->indep) == '\0' )
		return( GDNOX );
	if ( ( colptr = qry_col( tabptr->name, map->indep ) ) == NULL )
	{
		return( GDNOX );	/* error - no indep axis col */
	}
	else
	{
		if (!colptr->supported)
		{
			return(GDXDTYPE);
		}
		x_col = colptr;
		x_col->xflag = TRUE;
	}
	/* Then the dependent axis column - required */
	if ( map->dep == NULL || *(map->dep) == '\0' )
		return( GDNOY );
	if ( ( colptr = qry_col( tabptr->name, map->dep ) ) == NULL )
	{
		return( GDNOY );	/* error - no dep axis col */
	}
	else
	{
		if (!colptr->supported)
		{
			return(GDYDTYPE);
		}
		y_col = colptr;
	}
	/*
	** The selector series - optional (ignored, if a pie chart).
	**
	*/
	if ( map->series == NULL || *(map->series) == '\0' ||
		chartclass == GROB_PIE )
	{
		z_col = NULL;
	}
	else
	{
		if ((colptr = qry_col(tabptr->name, map->series)) == NULL)
		{
			return( GDNOZ );	/* no dep axis col */
		}
		else
		{
			if (!colptr->supported)
			{
				return(GDZDTYPE);
			}
			z_col = colptr;
		}
	}
	*ncolret = tabptr->colnum;
	return( OK );
}
/*{
** Name:	getchart	-  Locate chart and chart class in frame
**
** Description:
**	Loop through the graphics frame structure looking for one of
**	the four chart classes.	 When found, save a pointer to it, and
**	an indication of the class of chart being mapped in statics.
**
** Inputs:
**	frame		Pointer to the graphics frame
**
** Outputs:
**	Returns:
**		VOID
**	Exceptions:
**		<exception codes>
**
** Side Effects:
**
**	Updates chartobj and chartclass statics
**
** History:
**	16-dec-85 (drh) First written.
*/
static VOID
getchart( frame )
GR_FRAME	*frame;
{
	GR_OBJ	*comp_ptr;
	for ( comp_ptr = frame->head; comp_ptr != NULL;
		comp_ptr = comp_ptr->next )
	{
		switch( comp_ptr->type )
		{
		   case GROB_PIE:
			chartobj = comp_ptr;
			chartclass = GROB_PIE;
			maxdep = MAX_P_DS;
			break;
		   case GROB_BAR:
			dense = TRUE;
			chartobj = comp_ptr;
			chartclass = GROB_BAR;
			maxdep = MAX_B_DS;
			break;
		   case GROB_LINE:
			chartobj = comp_ptr;
			chartclass = GROB_LINE;
			maxdep = MAX_L_DS;
			break;
		   case GROB_SCAT:
			chartobj = comp_ptr;
			chartclass = GROB_SCAT;
			maxdep = MAX_S_DS;
			break;
		   default:
			break;
		}
	}
	return;
}
/*{
** Name:	statinit	-	initialize mapview statics
**
** Description:
**	Since mapview can be called repeatedly in the graph editing
**	process, this routine initializes all of it's statics.
**
** Inputs:
**	none
**
** Outputs:
**	none.
**	Returns:
**		VOID
**	Exceptions:
**
** Side Effects:
**
**	Initializes all local statics.
**
** History:
**	2/87 (bobm)	ADT changes
*/
static VOID
statinit()
{
	nwarn = 0;
	warnings[0] = 0;
	zused = 0;
	chartclass = GROB_NULL;
	chartobj = NULL;
	maxdep = 0;
	max_pts = 0;
	n_xstr = 0;
	n_ystr = 0;
	xid = 0;
	yid = 0;
	max_strs = 0;
	x_col = NULL;
	y_col = NULL;
	z_col = NULL;
	miny.db_data = (PTR) &miny_store;
	xlist = NULL;
	dense = FALSE;
	nxes = 0;
	return;
}
/*{
** Name:	getmaxima	-	Get maxima
**
** Description:
**
**	This routine retrieves the current version number of the
**	back end and sets a flag that is used to determine how to handle
**	date data.
**
**	It also establishes the worst-case maximum no. data points.
**
** Inputs:
**	map		Pointer to the map structure
**	tabptr		Pointer to the table descriptor
**
** Outputs:
**	Returns:
**		status - OK
**	Exceptions:
**
** Side Effects:
**
** History:
**	19-dec-85 (drh) First written.
**	20-oct-86 (sandyd) Removed retrieve on "_version" for SATURN.
**	13-nov-86 (bab) Properly set max_pts when bar chart w/ numeric data.
**	2/87 (bobm)	remove flags no longer needed for ADT changes, change
**			name to getmaxima, since that's now all it does.
*/
static STATUS
getmaxima( map, tabptr )
GR_MAP		*map;
TABLEATT	*tabptr;
{
	/*  Check that c-chart maximums won't be exceeded */
	max_pts = MAX_POINTS;
	if ( ( (x_col->type & DVEC_MASK) == DVEC_S ) ||
	     ( (y_col->type & DVEC_MASK) == DVEC_S ) )
	{
		max_pts = min(max_pts, MAX_STRINGS);
		if ( chartclass != GROB_PIE )  /* if a chart with axes */
		{
			max_pts = min(max_pts, MAX_XSTRINGS);
		}
	}
	if ( chartclass == GROB_PIE )
	{
		max_pts = min(max_pts, MAX_SLICE );
	}
	if ( chartclass == GROB_BAR )
	{
		max_pts = min(max_pts, MAX_BAR );
	}
	/*  Get the maximum number of strings allowed */
	max_strs = MAX_STRINGS;
	if ( chartclass != GROB_PIE )
	{
		max_strs = min( max_strs, MAX_XSTRINGS );
	}
	return( OK );
}
/*{
** Name:	getdata		-	Retrieve data to plot
**
** Description:
**	Using the target list, variable array, and sort clause built
**	by eqsetup, retrieve the data to be plotted in an EQUEL retrieve
**	loop.  The first time through the loop, call getfields to get a
**	'text' version of every field, for potential label use.	 Then
**	every time through the loop, store the x, y, and z values.  If
**	the chartclass is GROB_NULL, we are just filling in fields, and
**	we bail out after having done so (1st row of retrieve).
**
** Inputs:
**	dsarray		dependent series array for chart to tplot
**	nds		updated with number of dependent series
**	frame		pointer to graphics frame
**	tabptr		pointer to table descriptor
**
**	Returns:
**		retstat
**	Exceptions:
**
**
** Side Effects:
**
** History:
**	17-dec-85 (drh) First written
**	7/29/86 (bab)	Check for, and remove, dates prior to
**		January 1, 1902--the earliest date that we're
**		handling due to VE limitations.
**	8/4/86 (bab)	if user requested sorted data, than need
**		to sort the series' values (since no longer done
**		in the equel retrieve)
**	2/87 (bobm)	ADT changes.
**	4-aug-1988 (danielt)
**		changed to use IIQG_generate() instead of
**		a param retrieve.
**	5-Jun-1989 (Mike S)
**		Return GD_NOROWS for an empty table
**	28-apr-1992 (mgw) Bug #37448
**		Be sure to LOdelete(locptr) if we're returning after it's
**		creation, but before xyzmap() (which delete's it) is called.
**      18-aug-1992 (mgw) Bug #46037
**              Fix previous fix. Only close files that are actually open.
**	03-jun-1996 (canor01)
**		Remove unused "errno".
*/
static STATUS
getdata( t_addr, t_list, sort_clause, dsarray, nds, frame, tabptr )
DB_DATA_VALUE	*t_addr[];
char		*t_list;
char		*sort_clause;
DEPDAT	dsarray[];
i2	*nds;
GR_FRAME	*frame;
TABLEATT	*tabptr;
{
	bool	first = TRUE;
	i4	nrows = 0;
	i4	output;
	i4	(*olderr)();
	DB_DATA_VALUE	dx, dy, dz;
	GRC_STORE	dxs, dys, dzs;
	char		zdesc[GRC_SLEN];
	LOCATION	l_org;
	char		lbuf[MAX_LOC];
	/* set up storage for DB_DATA_VALUE's for dataset elements */
	dx.db_data = (PTR) &dxs;
	dy.db_data = (PTR) &dys;
	dz.db_data = (PTR) &dzs;
	/*
	**  Initialize the stream file information.  We will write x, y,
	**  and z values to this file, for later reading and massaging
	**  into vectors for C-Chart.  File not needed for simple
	**  field fill in.
	*/
	if (chartclass != GROB_NULL)
	{
		locptr = &tmploc;
		if ( ( FEt_open_bin( ERx("w"), ERx("vg"), ERx(""), &l_org, &fileptr ) ) != OK )
			return( GDNOTEMP );
		LOcopy(&l_org, lbuf, locptr);
	}
	/*
	**  tabptr->name should already be in unnormalized form and
	**  can be used directly.  the other names and sort clause
	**  name have already been requoted, if need, so there is
	**  no real work here for delim ids and owner.table support.
	*/
	if (IIUIrao_relOpen(tabptr->name, t_list, t_addr, (char *) NULL, 
				sort_clause, tabptr->scolnum) != OK)
	{
		if (chartclass != GROB_NULL)    /* for bug 46037 */
		{		
			if (SIclose(fileptr) != OK)
				IIUGerr(S_GR0017_tmpfil_close, UG_ERR_FATAL, 0);
			(VOID) LOdelete(locptr);	/* for bug 37448 */
		}
		return(GDNOTAB);
	}
	while(IIUIraf_relFetch() == OK)
	{
		/* count this row only if "OK" */
		if ( okrow(&dx,&dy,&dz,zdesc) )
		{
			if ( chartclass != GROB_NULL &&
			writexyz(first,&dx,&dy,&dz,zdesc) != OK )
			{
				break;
			}
			nrows++;
			if ( first )
			{
				getfields( frame, tabptr );
				if (chartclass == GROB_NULL)
				{
					break;
				}
				first = FALSE;
			}
		}
	}
	IIUIrac_relClose();
	if ( nrows <= 0 )
	{
		if (chartclass != GROB_NULL)    /* for bug 46037 */
		{
			if (SIclose(fileptr) != OK)
				IIUGerr(S_GR0017_tmpfil_close, UG_ERR_FATAL, 0);
			(VOID) LOdelete(locptr);	/* for bug 37448 */
		}
		return( GDNOROWS );
	}
	/* if all there only fields, we're done */
	if (chartclass == GROB_NULL)
	{
		return(OK);
	}
	max_pts = min( max_pts, nrows );
	/*
	**  Sort the list of series values.  Number of series should be
	**  in zused if z_col exists, otherwise we know it's 1.	 If too many,
	**  add warning, and set back to maxdep.  If *nds comes out < 1,
	**  the logic around find_ds which should have bumped zused if any
	**  rows got taken (*nrows > 0) was fouled up somehow.
	*/
	if ((*nds = z_col == NULL ? 1 : zused) > maxdep)
	{
		addwarn((i4) MAX_DEP);
		*nds = maxdep;
	}
	if (*nds < 1)
	{
		if (chartclass != GROB_NULL)    /* for bug 46037 */
		{
			if (SIclose(fileptr) != OK)
				IIUGerr(S_GR0017_tmpfil_close, UG_ERR_FATAL, 0);
			(VOID) LOdelete(locptr);	/* for bug 37448 */
		}
		IIUGerr(S_GR0019_Num_ser_err, UG_ERR_FATAL, 0);
	}
	if (zsort)
		sortz();
	/*
	**  Now read the file of x, y, and z values, and build vectors
	**  for C-Chart.
	*/
	xyzmap( dsarray );
	/*
	** the sortz / xyzmap logic SHOULD have filled in all the DEPDAT
	** descriptions.  It's cheap to make a check at this point
	*/
	for(nrows = 0; nrows < *nds; ++nrows)
		if(dsarray[nrows].desc == NULL)
			IIUGerr(S_GR001A_depdat_array, UG_ERR_FATAL, 0);
	return( OK );
}
/*{
** Name:	getfields	-	Get all field values (once)
**
** Description:
**	Converts the DB_DATA_VALUE's for fields into strings and associates
**	them with the graph.
**
** Inputs:
**	frame		Ptr to graphics frame structure
**	tabptr		Ptr to table descriptor
**
** Outputs:
**	<param name>	<output value description>
**	Returns:
**		VOID
**	Exceptions:
**		<exception codes>
**
** Side Effects:
**
**	Associates a stored text string with each GROB_TRIM item in the
**	frame list that is a field.  Stores the DB_DATA_VALUE for a
**	retrieved columns in the field DB_DATA_VALUE's, and sets the
**	flag for it.
**
** History:
**	19-dec-85 (drh) First written.
**	2/87 (bobm)	ADT changes.  Field value is now in a DB_DATA_VALUE.
*/
static VOID
getfields( frame, tabptr )
GR_FRAME	*frame;
TABLEATT	*tabptr;
{
	GR_OBJ	*comp_ptr;
	GR_TEXT *lab_ptr;
	COLDATA *colptr;
	i4	i;
	char	*str;
	/* Copy DB_DATA_VALUE into dbf item's and flag */
	colptr = tabptr->col;
	for (i=0; i < tabptr->colnum; ++i,++colptr)
	{
		MEcopy(colptr->db.db_data, (u_i2) colptr->db.db_length,
							colptr->dbf.db_data);
		colptr->fld = TRUE;
	}
	/* Find all 'fields' and associate strings with them */
	for ( comp_ptr = frame->head; comp_ptr != NULL;
		comp_ptr = comp_ptr->next )
	{
		if ( comp_ptr->type == GROB_TRIM )
		{
			lab_ptr = ( GR_TEXT * ) comp_ptr->attr;
			if ( lab_ptr->name != NULL &&
				*(lab_ptr->name) != '\0')
			{
				lab_ptr->text = qry_fstr( tabptr->name,
					lab_ptr->name, lab_ptr->column);
			}
		}
	}
}
/*{
** Name:	addwarn		-	Add a warning to the warn array
**
** Description:
**	If the warning to be added is not in the array already, and the
**	array has room, add the warning number to the warning array.
**
** Inputs:
**	warn		Warning number to add
**
** Outputs:
**
**	Returns:
**		VOID
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	08-jan-1986 (drh)	First written.
*/
static VOID
addwarn( warn )
i4	warn;
{
	i4	i;
	bool	found;
	if ( nwarn == MAXWARN ) /* if array is full */
		return;
	for ( i = 0, found = FALSE; i < nwarn; i++ )
	{
		if ( warnings[i] == warn )
			found = TRUE;
	}
	if ( found == FALSE )
	{
		warnings[ nwarn++ ] = warn;
		warnings[ nwarn ] = 0;
	}
	return;
}
/*{
** Name:	lookupz		-	Add current 'z' value to array
**
** Description:
**	Look up the current 'z' column value in an array of all z values
**	seen thus far.	When the array entry for the z value is found,
**	increment the number of records with this  z value, and return to
**	the caller the 'nat' identifier that has been assigned to the
**	z value.
**
** Inputs:
**	z	coerced DB_DATA_VALUE for comparison.
**	desc	z description string - not permanently stored yet.
**
**
** Outputs:
**	code		Returned with the 'nat' code for this z value, which
**			will be written to the data file.
**	Returns:
**		<function return values>
**		STATUS - OK
**			 FAIL if this entry is not in the z-array and the
**			      z-array is full.	This means that this data
**			      value must be thrown away (because VIG would
**			      do so anyway).
**	Exceptions:
**
**
** Side Effects:
**
** History:
**	01-31-86 (drh)	First written.
**	2/87 (bobm)	ADT changes.  ZDESC now contains DB_DATA_VALUE's.
**			use GRtxtstore to store the ZDESC string which will
**			get used as a DEPDAT description.
*/
static STATUS
lookupz( z, code, desc )
DB_DATA_VALUE	*z;		/* new z value */
i4		*code;		/* returned with code for z value */
char		*desc;		/* description string */
{
	i4	i;
	i4	cmp;
	bool	nullzval;
	if ( z_col == NULL )	/* if no z, code will be zero */
	{
		zarray[i=0].z_code = 0;
	}
	else
	{
		if (AFE_NULLABLE_MACRO(z->db_datatype))
			loc_isnull(z, &nullzval);
		else
			nullzval = FALSE;
		if (nullzval)
		{
			for ( i=0; i < zused; ++i)
			{
				loc_isnull(&(zarray[i].db), &cmp);
				if (cmp == 0)
					break;
			}
		}
		else
		{
			for ( i=0; i < zused; ++i)
			{
				loc_compare(z, &(zarray[i].db), &cmp);
				if (cmp == 0)
					break;
			}
		}
		if ( i >= zused )	/* first time we've seen this 'z' */
		{
			if ( zused >= MAX_DS )
			{
				addwarn ( (i4) MAX_DEP );
				return( FAIL );
			}
			/*
			** make a copy of the DB_DATA_VALUE - have to
			** allocate storage for the data (z gets overwritten).
			** make copy of description string.  This is the copy
			** that will be used in DEPDAT's.
			*/
			++zused;
			zarray[i].z_code = i;
			zarray[i].zdesc = GRtxtstore(desc);
			zarray[i].db.db_length = z->db_length;
			zarray[i].db.db_datatype = z->db_datatype;
			zarray[i].db.db_data =
				loc_alloc(2, 1, (i4)z->db_length);
			MEcopy(z->db_data,
				(i2) z->db_length, zarray[i].db.db_data);
		}
	}
	(zarray[i].numrecs)++;
	*code = zarray[i].z_code;
	return( OK );
}
/*{
** Name:	rdfrtmp, wrtotmp -	read/write DB_DATA_VALUE's.
**
** Description:
**	read / write temporary file.  Read or write a DB_DATA_VALUE.
**	Simply stream out the DB_DATA_VALUE, followed by its data,
**	read and relocate same on reading back in.  The read routine
**	and its caller maintain a high water mark for allocated memory,
**	the read routine allocating more and pointing the DB_DATA_VALUE
**	to it when a larger buffer is needed.  The caller must pass in
**	a pointer to a size variable which goes with the DB_DATA_VALUE
**	passed in, and is initialized to 0 on the first call.
**
** History:
**	2/87 (bobm)	written.
*/
static bool
rdfrtmp(db,asize)
DB_DATA_VALUE *db;
i4  *asize;
{
	i4 cnt;
	i4 need;
	if (SIread( fileptr, sizeof(DB_DATA_VALUE), &cnt, db ) != OK)
		return(FALSE);
	if (db->db_length > 0)
	{
		/*
		** need one more than db_length so we can assure null
		** terminated DB_LTXT_TYPE string easily.
		*/
		if (*asize < (need = db->db_length+1))
		{
			*asize = need;
			db->db_data = loc_alloc(3, 1, need);
		}
		if (SIread( fileptr, db->db_length, &cnt, db->db_data ) != OK)
			IIUGerr(S_GR001B_EOF, UG_ERR_FATAL, 0);
		/*
		** if this ISN'T a string, or is actually shorter than its
		** full length, this does no harm.
		*/
		*(((char *)(db->db_data))+db->db_length) = EOS;
	}
	return (TRUE);
}
static VOID
wrtotmp(db)
DB_DATA_VALUE *db;
{
	i4 cnt;
	if (SIwrite( sizeof(DB_DATA_VALUE), db, &cnt, fileptr ) != OK)
		IIUGerr(S_GR001C_Write, UG_ERR_FATAL, 0);
	if (db->db_length > 0)
	{
		if (SIwrite( db->db_length, db->db_data, &cnt, fileptr ) != OK)
			IIUGerr(S_GR001C_Write, UG_ERR_FATAL, 0);
	}
}
/*{
** Name:	writexyz	-	Write x, y, and z to a tempfile
**
** Description:
**	Write the coerced x, y, and z code values to a temp file.
**	Also builds the series description structure.
**
** Inputs:
**	first		first row flag.	 Used to store first miny.
**	dx,dy,dz	coerced DB_DATA_VALUE's.
**	zdesc		description string for z.
**
** Outputs:
**	none
**	Returns:
**		STATUS - OK
**	Exceptions:
**		<exception codes>
**
** Side Effects:
**	setups globals, destroys dz contents.
**
** History:
**	01-31-86 (drh)	First written.
**	2/87 (bobm)	ADT changes - use DB_DATA_VALUE's, which are coerced
**			via GRcoerce_type before being written out.  The string
**			for ZDESC table is constructed with GRcoerce_type also.
**			save miny as a DB_DATA_VALUE.
*/
static STATUS
writexyz(first,dx,dy,dz,zdesc)
bool first;
DB_DATA_VALUE *dx;
DB_DATA_VALUE *dy;
DB_DATA_VALUE *dz;
char *zdesc;
{
	i4	zcodeval;	/* 'code' for this z value */
	i4	cmp;
	/*
	**  Look up the z column value in the array of z values, and
	**  get it's code to write to the file.
	*/
	if ( ( lookupz( dz, &zcodeval, zdesc) ) != OK )
		return( OK );
	/*
	**  If a 'dense matrix' is required (for bar charts), add the x value
	**  to the list of unique x values, and keep track of the minumum
	**  y value so that we can use it later to fake up null data points.
	*/
	if ( dense == TRUE )
	{
		listx( dx );
		if ( first )
			cmp = -1;
		else
			loc_compare(dy,&miny,&cmp);
		if ( cmp < 0 )
		{
			miny.db_datatype = dy->db_datatype;
			miny.db_length = dy->db_length;
			MEcopy(dy->db_data, (i2) dy->db_length, miny.db_data);
		}
	}
	/*
	** Write the z code, and the x and y values to the temp file.
	** The zcode is written as a zero length DB type with "type"
	** equal to code.   This allows us simply to read/write DB
	** values & not mess with anything else, although we're abusing
	** the intended use for the structure somewhat.
	*/
	wrtotmp(dx);
	wrtotmp(dy);
	dz->db_datatype = zcodeval;
	dz->db_length = 0;
	wrtotmp(dz);
	return( OK );
}
/*{
** Name:	listx	-	Maintain list of all unique X-values
**
** Description:
**	Search for the X-value passed as a parameter in the XLIST, and, if
**	not found, add it.
**
** Inputs:
**	x_val		X-value to search for
**
** Outputs:
**	Returns:
**		VOID
**	Exceptions:
**		none
**
** Side Effects:
**	Adds the x-value to the list of unique X-values, if it is not found
**	in the list already
**
** History:
**	01-apr-86 (drh)		Created.
**	2/87 (bobm)	DB_DATA_VALUES for X_EL elements.  Compare using
**			adc routine.
*/
static VOID
listx( x_val )
DB_DATA_VALUE *x_val;
{
	X_EL	*xelptr = 0;	/* pointer to x-list */
	i4	*xloc;
	i4	xnum = 0;		/* number of this X in the x-list */
	xloc = &xnum;
	if ( xlist == NULL || ( ( findx( x_val, &xelptr, &xloc ) ) == FALSE ))
	{
		i4 	align_xel_size;
		PTR	tptr;
		/*
		** If this 'x' will cause the maximum number of points
		** to be exceeded, set a warning flag and don't add it.
		*/
		if ( ++nxes > max_pts )
		{
			nxes--;
			addwarn( MAX_DATA );
			return;
		}
		/*
		**  This x-value has not been seen yet.	 Add it to the
		**  x-list.  Get space for data, space for X_EL and space
		**  to align on restricted boundry in one alloc.
		*/
		align_xel_size = (i4)ME_ALIGN_MACRO(sizeof(X_EL), 
				       sizeof(ALIGN_RESTRICT)); /* B44150 */
		xelptr = (X_EL *)loc_alloc(4, 1,
			(align_xel_size + x_val->db_length)); /* B44150 */
		tptr = (PTR)xelptr;  /* B44150 */
		xelptr->x_next = xelptr;
		xelptr->x_prev = xelptr;
		xelptr->db.db_length = x_val->db_length;
		xelptr->db.db_datatype = x_val->db_datatype;
		xelptr->db.db_data = tptr + align_xel_size;  /* B44150 */
		MEcopy(x_val->db_data,(i2) x_val->db_length,xelptr->db.db_data);
		if ( xlist == NULL )
			xlist = xelptr;
		else
		{
			/*  add to the end of the x-list */
			xelptr->x_next = xlist;
			xelptr->x_prev = xlist->x_prev;
			xlist->x_prev = xelptr;
			xelptr->x_prev->x_next = xelptr;
		}
	}
	return;
}
/*{
** Name:	lookupx		- look up x value in x-list
**
** Description:
**	Look up the x value passed as an input parameter in the x-list
**	(list of unique x values).  When found, check the array of
**	series that have that x-value to see if it has been stored
**	already (i.e. whether this x point is a duplicate).  If it
**	is a duplicate, set the 'ok-to-store' flag to be FALSE, and
**	set the 'duplicate data points thrown away' warning.
**
** Inputs:
**	x_val		X value to be looked up
**	seriesno	0-based no. of the data series this x belongs to
**
** Outputs:
**	storeok		TRUE if OK to store, FALSE if a duplicate point
**	xno		0-based no. of this x value in the x-list.
**
**	Returns:
**		TRUE if found, FALSE otherwise
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	01-apr-86 (drh)		Created.
**	2/87 (bobm)		Use DB_DATA_VALUE.
*/
static bool
lookupx( x_val, seriesno, storeok, xno )
DB_DATA_VALUE	*x_val;
i4	seriesno;
bool	*storeok;
i4	*xno;
{
	X_EL	*xelptr = 0;
	*storeok = FALSE;
	if ( ( findx( x_val, &xelptr, &xno ) ) == FALSE )
		return( FALSE );
	if ( xelptr->x_ser[ seriesno ] ==  0 )
	{
		*storeok = TRUE;
		xelptr->x_ser[ seriesno ] =  1;
# ifdef DEBUG
		FEmsg(ERx("LOOKUPX: OK to store"), (bool) TRUE );
# endif
	}
	else
	{
# ifdef DEBUG
		FEmsg(ERx("Duplicate point found for series %d"), (bool)
			TRUE, (PTR)seriesno );
# endif
		addwarn( (i4) DUP_POINTS );
	}
	return( TRUE );
}
/*{
** Name:	findx	-	Look for an x-value in the x-list
**
** Description:
**	Look for the x-value passed as an input parameter in the x-list,
**	a list of all the unique x-values to be plotted.  If it's found,
**	return TRUE, as well as the 0-based number of the x-list element.
**	This can be used as in index into the data vector (GR_DVEC) for
**	this particular x value.
**
** Inputs:
**	x_val		X column value to look for
**
** Outputs:
**	xelptr		ptr to the x-value's entry in the x-list, if found
**	xnum		0-based number of this x-value in the x-list.
**
**	Returns:
**		TRUE if found, FALSE otherwise
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	01-apr-86 (drh)		Created.
**	2/87 (bobm)	Use DB_DATA_VALUE.
*/
static bool
findx( x_val, xelptr, xnum )
DB_DATA_VALUE	*x_val;
X_EL	**xelptr;
i4	**xnum;
{
	X_EL	*xptr = 0;
	i4	num = 0;
	i4	cmp;
	xptr = NULL;
	**xnum = 0;
	if ( xlist == NULL )
		return( FALSE );
	xptr = xlist;
	cmp = 1;
	do
	{
		loc_compare(x_val,&(xptr->db),&cmp);
		if ( cmp != 0)
		{
			num++;
			xptr = xptr->x_next;
		}
	}while ( cmp != 0 && xptr != xlist );
	if ( cmp == 0)
	{
		**xnum = num;
		*xelptr = xptr;
		return(TRUE);
	}
	return(FALSE);
}
/*{
** Name:	fillinx		-	Fill in missing x/z points
**
** Description:
**	For BAR CHARTS ONLY, fill in any missing x/z points.
**
**	If the Y axis is dates, the points we add will (unfortunately, but
**	unaVOIDably) show, so add a warning.
**
** Inputs:
**	dsarray		Data set array for the chart
**	nds		Number of data sets that actually were used
**
** Outputs:
**
**	Returns:
**		VOID
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	01-Apr-1986 (drh)	Created
**	2/87 (bobm)	ADT changes.  Use GRzero_pt routine to get fill
**			value, work in terms of DB_DATA_VALUE's.
*/
static VOID
fillinx( dsarray, nds )
DEPDAT	dsarray[];
i2	nds;
{
	X_EL	*xelptr;
	DEPDAT	*depptr;
	GR_DVEC *arrptr;
	DB_DATA_VALUE	yval;
	GRC_STORE	yvstore;
	i4	i = 0;
	i4	xno = 0;
	if ( xlist == NULL  || nds == 0 )
		return;
	yval.db_data = (PTR) &yvstore;
	GRzero_pt(y_col->type,&miny,&yval);
	for ( i = 0; i < nds; i++ )
	{
		depptr = &dsarray[i];
		xno = 0;
		xelptr = xlist;
		do
		{
			if ( xelptr->x_ser[i] == 0 )	/* if pt missing */
			{
# ifdef DEBUG
				FEmsg(ERx("Missing X number %d for series %d"),
					(bool) TRUE, (PTR)xno, (PTR)i);
# endif
				/*
				**  Add warning if Y is a date - the points
				**  we create will show as bars, unaVOIDably
				**  because of the way C Chart scales a date
				**  axis.
				*/
				if ( y_col->type == GRDV_DATE )
					  addwarn( (i4) ADD_BAR );
				arrptr = GTdvfetch( depptr->xname );
				datastore( &(xelptr->db), arrptr, 'x', xno );
				arrptr = GTdvfetch( depptr->field );
				datastore( &yval, arrptr, 'y', xno );
			}
			xelptr = xelptr->x_next;
			xno++;
		}while( xelptr != xlist );
	} /* end of for loop */
	return;
}
/*{
** Name:	sortx	-	Sort the list of unique X's, ascending
**
** Description:
**	Builds an array of the xlist (list of unique X data values.  Sorts
**	the array into ascending order using a simple bubble sort, then
**	re-creates the list in sorted order.
**
**	Note that we DO NOT SORT if X is a character field - presumably
**	the user has figured out what order he wants those in.	We will sort
**	integers, floating point, dates, and money.
**
** Inputs:
**	none
**
** Outputs:
**	none
**	Returns:
**		VOID
**	Exceptions:
**		none
**
** Side Effects:
**
**	Re-creates the xlist in sorted order, ascending.
**
** History:
**	04-apr-86 (drh) Created.
**	2/87 (bobm)	ADT changes.  X values are now DB_DATA_VALUE's.
**			localize memory allocation of pointer array and
**			free it when we're done.
*/
static VOID
sortx()
{
	X_EL	**xarray;
	X_EL	*xptr;
	X_EL	*next;
	i4	i;
	i2	oldtag;
	i2	mytag;
	if ( nxes <= 1 || ( ( x_col->type & DVEC_MASK ) == DVEC_S ) )
		return;
	/*
	**  Build an array of all the xlist elements.  It's easier to sort
	**  that way.  Do this on a different tag so we may release it at the
	**  end of this routine.  We do this by temporarily replacing deftag
	*/
	oldtag = deftag;
	mytag = deftag = FEgettag();
	xarray = (X_EL **)loc_alloc(5, nxes, sizeof(X_EL *));
	deftag = oldtag;
	for ( xptr = xlist, i = 0; i < nxes; i++ )
	{
		xarray[i] = xptr;
		next = xptr->x_next;
		xptr->x_next = xptr;
		xptr->x_prev = xptr;
		xptr = next;
	}
	/*  Sort the array */
	bubble( xarray );
	/*
	**  Re-build the list in sorted order, and then blow away
	**  the pointer array.
	*/
	xlist = xarray[0];
	for ( i = 1; i < nxes; i++ )
	{
		xptr = xarray[i];
		xptr->x_next = xlist;
		xptr->x_prev = xlist->x_prev;
		xlist->x_prev = xptr;
		xptr->x_prev->x_next = xptr;
	}
	FEfree(mytag);
	return;
}
/*{
** Name:	bubble	-	Sort the xlist array into ascending
**					order.
**
** Description:
**	Does a bubble sort of an array of pointers to X_EL descriptors of all
**	unique X values.
**
** Inputs:
**	xarray		Array to be sorted into ascending order by X value
**
** Outputs:
**	Returns:
**		VOID
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	04-apr-1986 (drh)	Created.
**	2/87 (bobm)	Sort on basis of adc_compare, using DB_DATA_VALUE's.
*/
static VOID
bubble( xarray )
X_EL	*xarray[];
{
	X_EL	*hold;
	i4	j;
	i4	bound;
	i4	t;
	i4	cmp;
	bound = nxes - 1;
	do
	{
		for ( j = 0, t = 0; j < bound ; j++ )
		{
			loc_compare(&((xarray[j])->db),
						&((xarray[j+1])->db), &cmp);
			if (cmp > 0)
			{
				hold = xarray[j];
				xarray[j] = xarray[j+1];
				xarray[j+1] = hold;
				t = j;
			}
		}
		bound = t;
	}while ( bound > 0 );
	return;
}
/*{
** Name:	sortz	-	Sort the list of Z's (series), ascending
**
** Description:
**	Use array of pointers to elements of zarray to order the pointers
**	based on string representation of z values.  Then, alter the
**	series' codes to match the new ordering.  Must not alter the
**	actual ordering of the zarray elements since values stored in
**	temp file contain references by index.
**
** Inputs:
**	none
**
** Outputs:
**	none
**	Returns:
**		VOID
**	Exceptions:
**		none
**
** Side Effects:
**	Modifies the series values in zarray to match a lexigraphic
**	ordering of the z_val strings.
**
** History:
**	8/4/86 (bab)	Written; copied from sortx.
**	8/27 (bobm)	Use DB_DATA_VALUE's.
*/
static VOID
sortz()
{
	ZDESC *zptr[MAX_DS];
	i4	i;
	for (i = 0; i < zused; i++ )
		zptr[i] = &zarray[i];
	/*  Sort the array if there's anything to sort */
	if (zused > 1)
		bubblez( zptr );
	/*
	**  Modify the dataset ordering
	*/
	for (i = 0; i < zused; i++ )
		zptr[i]->z_code = i;
}
/*{
** Name:	bubblez -	Sort the zptr array into ascending
**					order.
**
** Description:
**	Does a bubble sort of an array of pointers to ZDESC descriptors of all
**	unique Z values.
**
** Inputs:
**	zptr	Array of pointers to elements to be sorted into ascending
**		order by Z value
**
** Outputs:
**	Returns:
**		VOID
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	8/4/86 (bab)	Written; copied from bubble() code.
**	8/9/86 (bab)	Changed to call ismorez() since now
**		sorting on numeric values when col type isn't
**		GRDV_STRING.
**	2/87 (bobm)	Sort using adc_compare.
*/
static VOID
bubblez( zptr )
ZDESC *zptr[];
{
	ZDESC *hold;
	i4 cmp;
	i4	j;
	i4	bound;
	i4	t;
	bound = zused - 1;
	do
	{
		for ( j = 0, t = 0; j < bound ; j++ )
		{
			loc_compare(&((zptr[j])->db), &((zptr[j+1])->db), &cmp);
			if (cmp > 0)
			{
				hold = zptr[j];
				zptr[j] = zptr[j+1];
				zptr[j+1] = hold;
				t = j;
			}
		}
		bound = t;
	}while ( bound > 0 );
}
/*
** okrow -- verify data in row.	 This routine also issues the appropriate
**	warnings.  Ways in which data be not OK:
**
**	row contains date prior to VE_MINDATE, or an interval (which is 'bad').
**
**	row contains a NULL x or y data point.
**
** Because dates MUST be coerced to check them, this routine also performs
** all coercions, and passes the results back to avoid doing multiple
** expensive coercions.
**
** History:
**	7/29/86 (bab)	written
**	2/87 (bobm)	rewrite for ADT's.  Use GRcoerce_type to check
**			dates, also throw out NULL x and y points.
*/
static bool
okrow(dx,dy,dz,zdesc)
DB_DATA_VALUE *dx;
DB_DATA_VALUE *dy;
DB_DATA_VALUE *dz;
char *zdesc;
{
	i4		ny,nx;
	DB_DATA_VALUE	dchk;
	GRC_STORE	store;
	/*
	** check for nullable before checking date.  If there is a NULL
	** x or y, forget the date checks.  Using the NULLABLE macro
	** saves procedure calls for non-nullable types, at the expense
	** of a little extra work on nullable types.
	*/
	if (AFE_NULLABLE_MACRO(x_col->db.db_datatype))
		loc_isnull(&(x_col->db),&nx);
	else
		nx = FALSE;
	if (AFE_NULLABLE_MACRO(y_col->db.db_datatype))
		loc_isnull(&(y_col->db),&ny);
	else
		ny = FALSE;
	if (nx || ny)
	{
		addwarn( (i4) NULL_X_OR_Y );
		return(FALSE);
	}
	/*
	** coerce types appropriately.	NOTE:  these "types" will
	** be controlled by the GRDV codes, but will be legitimate
	** DB_DATA_VALUE types, albeit maybe not what you think
	** they should be.  For instance, "dates" will become the
	** VE date integers, and so on.	 The xc and yc will be non-null,
	** but the type coerced to may still be nullable.
	**
	** if column y is a date prior to 1902, or column x is a date
	** prior to 1902, or column z exists and is a date prior to 1902
	** then this row gets tossed on the heap.  < GRC_INTERVAL indicates
	** that it was really interval data.  GRC_INTERVAL < VE_MINDATE.
	*/
	if (z_col != NULL)
	{
		GRcoerce_type(z_col,&(z_col->db),dz,zdesc);
		if (z_col->type == GRDV_DATE &&
				*((i4 *)(dz->db_data)) < VE_MINDATE)
		{
			if (*((i4 *)(dz->db_data)) < GRC_INTERVAL)
				addwarn ((i4) IS_INTERVAL);
			else
				addwarn((i4) TOO_EARLY);
			return(FALSE);
		}
	}
	GRcoerce_type(y_col,&(y_col->db),dy,NULL);
	if (y_col->type == GRDV_DATE && *((i4 *)(dy->db_data)) < VE_MINDATE)
	{
		if (*((i4 *)(dy->db_data)) < GRC_INTERVAL)
			addwarn ((i4) IS_INTERVAL);
		else
			addwarn((i4) TOO_EARLY);
		return(FALSE);
	}
	GRcoerce_type(x_col,&(x_col->db),dx,NULL);
	if (x_col->type == GRDV_DATE && *((i4 *)(dx->db_data)) < VE_MINDATE)
	{
		if (*((i4 *)(dx->db_data)) < GRC_INTERVAL)
			addwarn ((i4) IS_INTERVAL);
		else
			addwarn((i4) TOO_EARLY);
		return(FALSE);
	}
	return (TRUE);
}
/*
** loc_xxx routines cover things which aren't supposed to fail, so we
** don't have to error check them all the time.	 Simply fatal error if they
** do fail:
**
**	loc_compare - adc_compare using standard FE context block.
**	loc_isnull - adc_isnull using standard FE context block.
**	loc_alloc - FEtalloc on deftag.	 Passes in a number to identify
**		calling point, since allocation failures can be hard
**		to duplicate.
**
** History:
**	2/15/89 (Mike S)  
**		Allocate zeroed memory.
*/
static VOID
loc_compare(d1,d2,cmp)
DB_DATA_VALUE *d1;
DB_DATA_VALUE *d2;
i4  *cmp;
{
	if (adc_compare(GR_cb, d1, d2, cmp) != OK)
		IIUGerr(S_GR001D_adc_compare, UG_ERR_FATAL, 0);
}
static VOID
loc_isnull(d,cmp)
DB_DATA_VALUE *d;
i4  *cmp;
{
	if (adc_isnull(GR_cb, d, cmp) != OK)
		IIUGerr(S_GR001E_adc_isnull, UG_ERR_FATAL, 0);
}
static PTR
loc_alloc(wh, s1, s2)
i4  wh;
i4  s1, s2;
{
	char	bufr[48];
	PTR	addr;
	STprintf(bufr, ERx("map_view - %d"), wh);
	if ((addr = FEreqmem((u_i4)deftag, (u_i4)(s1 * s2), (bool)TRUE,
		(STATUS *)NULL)) == NULL)
	{
		IIUGbmaBadMemoryAllocation(bufr);
	}
	return(addr);
}