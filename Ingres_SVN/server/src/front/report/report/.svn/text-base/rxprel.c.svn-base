/*
** Copyright (c) 2004 Ingres Corporation
*/

/* static char	Sccsid[] = "@(#)rxprel.c	30.1	11/14/84"; */

# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<me.h>
# include	<adf.h>
# include	<fmt.h>
# include	 <rtype.h> 
# include	 <rglob.h> 
# include	<afe.h>
# include	<er.h>

/*
**   R_X_PRELEMENT -	This routine evaluates an expression and prints it with
**		an optional format.  If a format was given in the PRINT
**		statement or if the expression is an attribute or an aggregate
**		of an attribute and the format of the attribute was previously
**		specified in a format statement, then that format is used.
**		Otherwise, a default format is created from the datatype of
**		the expression.  If the format specifies more than one line
**		(e.g. for text), each of the lines will be printed and the
**		current line will end up on the first line printed.
**
**	Parameters:
**		pel	pointer to print element
**
**	Returns:
**		none.
**
**	Side Effects:
**		none.
**
**	Called by:
**		r_x_tcmd.
**
**	Error messages:
**		none.
**
**	Trace Flags:
**		none.
**
**	History:
**		1/18/84 (gac)	written.
**		12-dec-88 (sylviap)
**			Changed memory allocation from FEalloc to FEreqmem.
**		7/24/89 (martym)
**			Fixed bug #6417, by checking to see if the print -
**			element is a expression before setting its format.
**		8/1/89 (elein) b7333
**			Add parameter, TRUE, to reitem.
**		9/19/89 (elein) sir 8146
**			Check for WARN as well as OK after format.
**			Don't display it if it is only a warning
**			The only case this will really happen is
**			if data overflows the format (e_fm1000).
**		9/20/89 (elein) B7885
**			Dont save formats built on the fly, they are
**			based on data which could change!  This is
**			related to fix 6417 and 4883.  In the rlog
**			for this file, you can see that these changes
**			went back and forth several times.  I think
**			this piece was lost when it was added back in.
**		02-oct-89 (sylviap)
**			Added control sequence support (Q format). r_x_print
**			has a new parameter to state if printable (TRUE)
**			or a control sequence (FALSE).
**		27-feb-90 (sylviap)
**			Increased the size of  AFE_DCL_TXT_MACRO.  Need to
**			large enough to handle the longest varchar. (US #425)
**		3/21/90 (elein) performance
**			Check St_no_write before calling r_set_dflt
**		29-mar-90 (cmr) fix for bug #9963
**	    		Use DB_MAXSTRING for the max char width for fmt_deffmt
**			so that we don't end up with a CF format; we want the 
**			regular C format. Now printing several elements, one 
**			of which is a string that is > St_right_margin, works. 
**      	3/21/91 (elein)         (b35574)
**			Add FALSE parameter to call to
**                      fmt_multi. TRUE is used internally for boxed
**                      messages only.  They need to have control
**                      characters suppressed.
**              09/26/91 (jillb/jroy--DGC) (from 6.3)
**                      Changed fmt_init to IIfmt_init since it is a fairly
**                      global routine.  The old name, fmt_init, conflicts
**                      with a Green Hills Fortran function name.
**		23-oct-1992 (rdrane)
**			Ensure set/propagate precision in DB_DATA_VALUE
**			structures.  Remove declarations of r_gt_att() and
**			r_n_fmt() since they're already declared in hdr
**			files already included by this module.
**		29-dec-1992 (rdrane)
**			Ensure that FMT structures which are not saved are
**			freed so that we don't run out of memory on long
**			(many rows) reports with expressions and/or variable
**			length strings (bug 45104).  Assign the memory
**			allocated for FMT structures to the Rst5_tag pool.
**		18-may-1993 (rdrane)
**			Ensure that q0 format strings are left alone, and
**			not sent through fmt_format()!
**		6-aug-1993 (rdrane)
**			Not sending q0 format strings fmt_format() resulted
**			in non-hex values being not converted to DB_LTEXT_TYPE
**			which r_x_print() expects.  So ...
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/



r_x_prelement(pel)
PEL	*pel;
{
	/* internal declarations */

	DB_DATA_VALUE	val;
	DB_DATA_VALUE	display;
	PTR		workspace;
	PTR		fmtarea;
	i4		rows;
	i4		cols;
	char		fmtstr[MAX_FMTSTR];
	i4		size;
	FMT		*fmt;
	ATT		*att;
	ACC		*acc;
	CUM		*cum;
	AFE_DCL_TXT_MACRO(DB_GW4_MAXSTRING)	buffer;
	STATUS		status;
	bool		more;
	DB_DATA_VALUE	*pdb;
	DB_DATA_VALUE	dbdv;
	bool		save_fmt = TRUE;
	i4		kind;

	/* start of routine */

	if( St_no_write)
	{
		r_set_dflt();
	}


	r_ges_reset();
	r_e_item(&pel->pel_item, &val, TRUE);

	fmt = pel->pel_fmt;

	if (pel->pel_item.item_type == I_ATT)
	{
		att = r_gt_att(pel->pel_item.item_val.i_v_att);
		if (att->att_tformat != NULL)
		{
			fmt = att->att_tformat;
			att->att_tformat = NULL;
		}
		else if (fmt == NULL)
		{
			fmt = att->att_format;
		}
	}
	else if (fmt == NULL)
	{
		switch (pel->pel_item.item_type)
		{
		case(I_ACC):
			acc = pel->pel_item.item_val.i_v_acc;
			fmt = r_n_fmt(acc->acc_attribute,
				      acc->acc_ags.adf_agfi);
			break;

		case(I_CUM):
			cum = pel->pel_item.item_val.i_v_cum;
			fmt = r_n_fmt(cum->cum_attribute,
				      cum->cum_ags.adf_agfi);
			break;

		}
	}

	if (fmt == NULL)
	{
	    pdb = &val;

	    if (pdb->db_datatype == DB_MNY_TYPE && St_compatible)
	    {
		/* if -a set and attribute is money, use the old format
		** for money which is the same as the current float */

		dbdv.db_datatype = DB_FLT_TYPE;
		dbdv.db_length = sizeof(f8);
		dbdv.db_prec = 0;
		pdb = &dbdv;
	    }

	    /* Create a default format string for the type of the value.*/
	    /* Fix for bug #9963 */
	    /* Use DB_MAXSTRING for the max char width so that we don't	*/
	    /* end up with a CF format; we want the regular C format.	*/
	    /* Now printing several elements, one of which is a string 	*/
	    /* that is > St_right_margin, will print properly.		*/
	    status = fmt_deffmt(Adf_scb, pdb, DB_MAXSTRING, TRUE, fmtstr);
	    if (status != OK && status != E_DB_WARN)
	    {
		FEafeerr(Adf_scb);
	    }
	    
	    /*
            ** fix for bug 4883
            ** if the item to be printed is a variable length
            ** string and it did not come from a database column,
            ** change the format to c(whatever the length of the string).
            */
            if ((abs(pdb->db_datatype) == DB_TXT_TYPE)
                                || (abs(pdb->db_datatype) == DB_VCH_TYPE))
	    {
		i2	str_length = *((i2 *) pdb->db_data);
	        save_fmt = FALSE;
                STprintf(fmtstr, ERx("c%d"), str_length);
	    }


	    /* allocate memory area for format */
	    status = fmt_areasize(Adf_scb, fmtstr, &size);
	    if (status != OK && status != E_DB_WARN)
	    {
		FEafeerr(Adf_scb);
	    }

	    if ((fmtarea = (PTR)MEreqmem(Rst5_tag,(u_i4)size,TRUE, 
					 (STATUS *)NULL)) == NULL)
            {
	       IIUGbmaBadMemoryAllocation(ERx("r_x_prelement - fmtarea"));
	    }

	    /* create a format structure from the default string format */
	    status = fmt_setfmt(Adf_scb, fmtstr, fmtarea, &fmt, NULL);
	    if (status != OK && status != E_DB_WARN)
	    {
		FEafeerr(Adf_scb);
	    }

	    /*
	    ** Bug #6417. Don't set the print element's fmt, because it 
	    ** may change for each data value, if the print element is 
	    ** an expression:
	    ** Bug #7885--don't save the fmt if it was built on the fly
	    ** Bug 45104--explicitly set save_fmt to FALSE if we're not going
	    **		  to save it so we can free it when we're done!
	    */
	    if  ((pel->pel_item.item_type != I_EXP) && (save_fmt == TRUE))
	    {
	    	pel->pel_fmt = fmt;
	    }
	    else
	    {
		save_fmt = FALSE;
	    }
	}

	/* determine how many rows and columns the formatted value takes up */
	status = fmt_size(Adf_scb, fmt, &val, &rows, &cols);
	if (status != OK && status != E_DB_WARN)
	{
	    FEafeerr(Adf_scb);
	}

	/* allocate space for the display area */
	display.db_datatype = DB_LTXT_TYPE;
	display.db_length = sizeof(buffer);
	display.db_prec = 0;
	display.db_data = (PTR) &buffer;

	if (rows == 1)
	{
	    /* 
	    ** The Q format assumes user is sending a control sequence, and
	    ** so RW should NOT increment report counter.  This sequence
	    ** does not take up any space on the report, and so will never
	    ** reflect a rows value other than one.  If not the Q format,
	    ** then the string is printable and must first be formatted.
	    */
	    status = fmt_kind(Adf_scb, fmt, &kind);
	    if (status != OK)
	    {
		FEafeerr(Adf_scb);
	    }
	    if (kind == FM_Q_FORMAT)
	    {
	    	/* FALSE = non-printable, a control sequence */
		afe_cvinto(Adf_scb,&val,&display);
	    	r_x_print(&display, 0, FALSE);
	    }
	    else
	    {
	    	/* format the value */
	    	status = fmt_format(Adf_scb, fmt, &val, &display, FALSE);
	    	if (status != OK && status != E_DB_WARN)
		{
		    FEafeerr(Adf_scb);
		}
	    	/* print it - TRUE = printable */
	    	r_x_print(&display, 0, TRUE);
	    }
	}
	else	/* multi-line format */
	{
	    i4	 width;		/* column width */
	    LN	 *oldln;	/* save original value of ST_cline */
	    i4	 loc;		/* position in St_cline to start */
	    bool frstline=TRUE;	/* TRUE on the first line written by r_x_print.
				** This is used to make sure that the current
				** position is left at the line position
				** returned from r_x_print (in case wraparound
				** occurs) */

	    /* allocate the workspace for formatting multiple lines */
	    status = fmt_workspace(Adf_scb, fmt, &val, &size);
	    if (status != OK && status != E_DB_WARN)
	    {
		FEafeerr(Adf_scb);
	    }

	    workspace = MEreqmem(0,size,FALSE,NULL);

	    /* initialize the workspace */
	    status = IIfmt_init(Adf_scb, fmt, &val, workspace);
	    if (status != OK && status != E_DB_WARN)
	    {
		FEafeerr(Adf_scb);
	    }

	    width = cols;
	    oldln = St_cline;		/* save aside because
					    ** when this routine returns
					    ** it should be here */
	    loc = St_cline->ln_curpos;	/* all columns start here */
	    if (loc >= En_lmax)
	    {   /* by any chance */
		loc = loc % En_lmax;
	    }
	    if ((loc+width) > En_lmax)
	    {
		width = En_lmax - loc;
	    }

	    while ((status = fmt_multi(Adf_scb, fmt, &val, workspace, &display,
			&more,FALSE, FALSE)) == OK || status == E_DB_WARN)
	    {
		if (!more)
		{
		    break;	/* no more lines left to format */
		}

		if (!frstline)
		{   /* go to the next line buffer */
		    St_cline = r_gt_ln(St_cline);
		}

		/* now write out a line of the formatted value */
		r_x_tab(loc, B_ABSOLUTE);
		r_x_print(&display, width, TRUE); /* TRUE-string is printable */

		if (frstline)
		{   /* change oldln if wraparound occurred */
		    if (oldln != St_cline)
		    {	/* it did */
			oldln = St_cline;
		    }
		    frstline = FALSE;
		}
	    }

	    if (status != OK && status != E_DB_WARN ) 
	    {
		FEafeerr(Adf_scb);
	    }

	    _VOID_ MEfree(workspace);

	    /* finished up.  Switch back to old LN buffer */
	    St_cline = oldln;
	    St_pos_number = St_cline->ln_curpos;
	}

	/*
	** If we're not saving the FMT structure, then free it now
	** so we don't run out of memory! (bug 45104)
	*/
	if  (!save_fmt)
	{
	    _VOID_ MEfree(fmtarea);
	}

	return;
}
