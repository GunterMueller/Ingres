/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cv.h>
#include    <er.h>
#include    <ex.h>
#include    <er.h>
#include    <pc.h>
#include    <cs.h>
#include    <me.h>
#include    <tm.h>
#include    <st.h>
#include    <sl.h>

#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <ulf.h>
#include    <ulm.h>
#include    <adf.h>
#include    <ade.h>
#include    <adp.h>
#include    <qsf.h>
#include    <scf.h>
#include    <gca.h>
#include    <dmf.h>
#include    <dmrcb.h>
#include    <copy.h>
#include    <qefmain.h>
#include    <qefrcb.h>
#include    <qefqeu.h>
#include    <qefcopy.h>

#include <dudbms.h>
#include <dmccb.h>

#include    <sc.h>
#include    <sc0e.h>
#include    <sc0m.h>
#include    <scc.h>
#include    <scs.h>
#include    <scd.h>
#include    <scfcontrol.h>
#include    <sxf.h>
#include    <sc0a.h>

/*
**
**  Name: SCSCOPY.C - Copy related sub-routines.
**
**  Description:
**	This file contains various sub-routines that are used during
**	copy processing. They will usually be called by the sequencer
**	during a copy.
**
**	  scs_scopy()		- Send GCA_C_{INTO,FROM} block to FE
**	  scs_copy_error()	- Process a COPY error.
**	  scs_cpsave_err()	- Stash a COPY error code.
**	  scs_cpxmit_errors()	- Transmit a batch of copy errors
**	  scs_cpinterrupt()	- Send interrupt to the FE.
**        scs_cp_cpnify()       - Prepare read copy blocks containing
**                                  large objects
**        scs_cp_redeem()       - Expand large objects in copy blocks
**
**  History:
**	25-jun-1992 (fpang)
**	    Created this source file header description as part of Sybil merge.
**	13-jul-92 (rog)
**	    Included ddb.h for Sybil, and er.h because of a change to scs.h.
**	21-jul-92 (rog)
**	    Fix compilation error: control-L was a carat-L.
**	10-nov-92 (robf)
**	    Make sure SXF audit data is flushed prior to returning anything
**	    to the user. 
**  	23-Feb-1993 (fred)
**  	    Added large object support.  Added scs_cp_cpnify() and
**          scs_cp_redeem() to enable the management of large object
**          values during a COPY statement.
**	12-Mar-1993 (daveb)
**	    Add include <st.h> <me.h>
**      23-Jun-1993 (fred)
**          Fixed bug in copy null indicator value handling.  See
**          scs_scopy(). 
**	2-Jul-1993 (daveb)
**	    prototyped, remove func extern refs from headers.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	7-jul-93 (rickh)
**	    Prototyped qef_call invocations.
**	08-sep-93 (swm)
**	    Cast completion id. parameter to IIGCa_call() to PTR as it is
**	    no longer a i4, to accomodate pointer completion ids.
**	24-jan-1994 (gmanning)
**	    Change errno to local_errno because of NT macro problems.
**      05-nov-1993 (smc)
**          Bug #58635
**          Made l_reserved & owner elements PTR for consistency with
**	    DM0M_OBJECT. This results in owner within sc0m_allocate having to
**	    be PTR.
**          In addition calls to sc0m_allocate within this module had the
**          3rd and 4th parameters transposed, this has been remedied.
**      22-mar-1996 (stial01)
**          Cast length passed to sc0m_allocate to i4.
**	19-nov-1996 (kch)
**	    In the function scs_cp_redeem(), we now calculate the number of
**	    non LO bytes in the row and decrement this by the number of non
**	    LO bytes copied. When we come to an LO column, we call scs_redeem()
**	    with the remaining length of the copy buffer minus the number of
**	    non LO bytes still to copy, thus preventing scs_redeem() from
**	    leaving an insufficient amount of space in the buffer for the
**	    remaining non LO data. This change fixes bug 79287.
**	14-jan-1999 (nanpr01)
**	    Changed the sc0m_deallocate to pass pointer to a pointer.
**	26-apr-1999 (hanch04)
**	    Added casts to quite compiler warnings
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	21-Dec-2001 (gordy)
**	    GCA_COL_ATT no longer defined with DB_DATA_VALUE.
**      25-mar-2004 (stial01)
**          scs_redeem: restrict blob output to MAXI2
**          Fixed incorrect casting of length arguments.
*/

/*
**  Forward and/or External function/variable references.
*/
static DB_STATUS scs_redeem(SCD_SCB   	 *scb,
			    i4           continuation,
			    DB_DT_ID     datatype,
			    i4           prec,
			    i4      out_size,
			    PTR          cpn_ptr,
			    i4           *res_prec,
			    i4           *res_len,
			    PTR          out_area);
GLOBALREF	SC_MAIN_CB	*Sc_main_cb;

/*}
** Name: scs_scopy	- send GCA_C_{INTO,FROM} block to FE
**
** Description:
**
** Inputs:
**      scb                             session control block for this session.
**	qe_copy				copy control block
**
** Outputs:
**	Returns:
**	    DB_STATUS
**	Exceptions:
**	    none
**
** Side Effects:
**	    Copy map message is created.
**
** History:
**      21-Jul-1987 (fred)
**          GCA/GCF Support introduced.
**      18-sep-89 (jrb)
**          Added code to put prec*256+scale into length fields of GCA_CPRDD
**          message for attribute descriptors of DECIMAL values.
**	07-nov-91 (ralph)
**	    6.4->6.5 merge:
**		22-jul-91 (stevet)
**		    Added support for new copy map message for
**		    GCA_PROTOCOL_LEVEL_50.  This new map supports
**		    GCA_DATA_VALUE format for NULL value description
**		    and added precision type to column description.
**	6-jan-1993 (stevet)
**	    Remove code to put prec*256+scale into length field because 
**	    we have added new precision fields to GCA_PROTOCOL_LEVEL_50.
**	    Also start passing the precisions to the FE.
**      26-Feb-1993 (fred)
**  	    Added large object support.  Initialized new sscb_copy
**          fields which are used to keep information across copy
**          block transmissions when large objects are in use.  Also,
**          fixed latent bug made evident in this code, wherein the
**          fact that the gca_result_modifer was assumed to always be
**          zero (it isn't with large objects).
**      23-Jun-1993 (fred)
**          Fixed bug in copy null indicator value handling.  When an
**          indicator value was provided, the parser converts it into
**          a value of the type being sent back.  When this value is a
**          large type, then a coupon is build.  However, this routine
**          was not cognizant of the fact that there was a coupon
**          there;  therefore it didn't redeem it, causing the coupon
**          to be sent as is to the frontend, where it was really
**          pretty worthless.  This routine now keeps an eye out for
**          coupons, redeeming them for transmission when necessary.
**	2-Jul-1993 (daveb)
**	    prototyped, use proper PTR bor allocation.
**      05-nov-1993 (smc)
**          Bug #58635
**          Made l_reserved & owner elements PTR for consistency with DM_SVCB.
**          This results in owner within sc0m_allocate having to be PTR.
**      22-mar-1996 (stial01)
**          Cast length passed to sc0m_allocate to i4.
**	21-Dec-2001 (gordy)
**	    GCA_COL_ATT no longer defined with DB_DATA_VALUE.
[@history_template@]...
*/

DB_STATUS
scs_scopy(SCD_SCB *scb,
	  QEU_COPY  *qe_copy )

{
    char	    *data;
    char            *data_start;
    QEU_CPDOMD	    *cpdom_desc;
    GCA_TD_DATA	    *sqlda;
    GCA_DATA_VALUE  gdv;
    SCC_GCMSG	    *message;
    i4		    filename_length;
    i4		    logname_length;
    i4		    data_size, buffer_size;
    i4		    status, i, error;
    i4	    ln_val;
    i4	    cp_num_attr;
    i4		    n_val;
    i4              data_size_suspect = 0;
    PTR             block;
    char            *msg_start;

    if (qe_copy->qeu_moving_data == FALSE)
    {
	scb->scb_sscb.sscb_copy.cp_cur_att = -1;
	scb->scb_sscb.sscb_copy.cp_att_need = -1;
	scb->scb_sscb.sscb_copy.cp_in_offset = -1;
	scb->scb_sscb.sscb_copy.cp_out_offset = -1;
	scb->scb_sscb.sscb_copy.cp_in_lobj = 0;
	scb->scb_sscb.sscb_copy.cp_att_count = -1;

	if (qe_copy->qeu_fdesc->cp_filename)
	    filename_length = STlength(qe_copy->qeu_fdesc->cp_filename) + 1;
	else
	    filename_length = 0;

	if (qe_copy->qeu_logname)
	    logname_length = STlength(qe_copy->qeu_logname) + 1;
	else
	    logname_length = 0;

	sqlda = (GCA_TD_DATA *) qe_copy->qeu_tdesc;

	/* 
	** Make sure ULC_COPY struct will fit in the output buffer.
	*/

	/*
	**  Add new copy map support for GCA_PROTOCOL_LEVEL_50 
	*/

	cp_num_attr = (i4)qe_copy->qeu_fdesc->cp_filedoms;

	if (scb->scb_cscb.cscb_version >= GCA_PROTOCOL_LEVEL_50)
	{
	    data_size = (sizeof(i4)	+	/* cp_status */
			sizeof(i4)		+	/* cp_direction */
			sizeof(i4)		+	/* cp_maxerrs */
			sizeof(i4)		+	/* cp_fname length */
			sizeof(i4)		+	/* cp_logname length */
			sizeof(i4)		+	/* cp_filedoms */
			sizeof(i4)		+	/* da_tup_size */
			sizeof(i4)		+	/* gca_result */
			sizeof(u_i4)	+	/* gca_id_tdscr */  
			sizeof(i4))	+	/* da_num_atts */
			(sqlda->gca_l_col_desc	*	/* da_attribute */
			 (sizeof(sqlda->gca_col_desc[0].gca_attdbv) +
			  sizeof(sqlda->gca_col_desc[0].gca_l_attname))) +
			(filename_length)	+	/* cp_fname */
			(logname_length)	+	/* cp_logname */
			(cp_num_attr		*	/* cp_fdesc */
			    (GCA_MAXNAME	+	/* cp_domname */
			    sizeof(i4)		+	/* cp_type */
			    sizeof(i4)		+	/* cp_precision */
			    sizeof(i4)	+	/* cp_length */
			    sizeof(i4)		+	/* cp_user_delim */
			    sizeof(i4)		+	/* size of ... */
			    CPY_MAX_DELIM	+	/* cp_delimiter */
			    sizeof(i4)	+	/* cp_tupmap */
			    sizeof(i4)		+	/* cp_cvid */
			    sizeof(i4)		+	/* cp_cvprec */
			    sizeof(i4)		+	/* cp_cvlen */
			    sizeof(i4)		+	/* cp_withnull */
			    sizeof(i4)));		/* cp_nulldata_value */
	    for (i = 0, cpdom_desc = qe_copy->qeu_fdesc->cp_fdom_desc;
		i < cp_num_attr;
	        i++, cpdom_desc = cpdom_desc->cp_next)
	    {
		if( cpdom_desc->cp_nullen)
		{
		    data_size += sizeof(gdv.gca_type) + 
		 	         sizeof(gdv.gca_precision) +
			         sizeof(gdv.gca_l_value);
		    if ((sqlda->gca_result_modifier & GCA_LO_MASK)
			                       == 0)
		    {
			/*
			** In this case, there can be no large object
			** types in the copy, so we needn't worry
			** about them individually.
			*/

			data_size += cpdom_desc->cp_nullen;
		    }
		    else
		    {
			/*
			** if this att is a large object,
			** then
			**     add MAXTUP
			** else
			**     add length as above
			*/

			
			status = adi_dtinfo((ADF_CB *) scb->scb_sscb.sscb_adscb,
					    cpdom_desc->cp_type,
					    &n_val);
			if (DB_FAILURE_MACRO(status))
			{
			    sc0e_0_put(((ADF_CB *)
			       scb->scb_sscb.sscb_adscb)->adf_errcb.ad_errcode,
				     0);
			    sc0e_0_uput(((ADF_CB *)
			       scb->scb_sscb.sscb_adscb)->adf_errcb.ad_errcode,
				     0);
			    sc0e_0_uput(E_SC0206_CANNOT_PROCESS, 0);
			    return (E_DB_ERROR);
			}
			
			if (n_val & AD_PERIPHERAL)
			{
			    data_size += DB_MAXTUP;
			    data_size_suspect = 1;
			}
			else
			{
			    data_size += cpdom_desc->cp_nullen;
			}
		    }
		}
	    }
	}
	else
	{
	    data_size = (sizeof(i4)	+	/* cp_status */
			sizeof(i4)		+	/* cp_direction */
			sizeof(i4)		+	/* cp_maxerrs */
			sizeof(i4)		+	/* cp_fname length */
			sizeof(i4)		+	/* cp_logname length */
			sizeof(i4)		+	/* cp_filedoms */
			sizeof(i4)		+	/* da_tup_size */
			sizeof(i4)		+	/* gca_result */
			sizeof(u_i4)	+	/* gca_id_tdscr */  
			sizeof(i4))	+	/* da_num_atts */
			(sqlda->gca_l_col_desc	*	/* da_attribute */
			 (sizeof(sqlda->gca_col_desc[0].gca_attdbv) +
			  sizeof(sqlda->gca_col_desc[0].gca_l_attname))) +
			(filename_length)	+	/* cp_fname */
			(logname_length)	+	/* cp_logname */
			(cp_num_attr		*	/* cp_fdesc */
			    (GCA_MAXNAME	+	/* cp_domname */
			    sizeof(i4)		+	/* cp_type */
			    sizeof(i4)	+	/* cp_length */
			    sizeof(i4)		+	/* cp_user_delim */
			    sizeof(i4)		+	/* size of ... */
			    CPY_MAX_DELIM	+	/* cp_delimiter */
			    sizeof(i4)	+	/* cp_tupmap */
			    sizeof(i4)		+	/* cp_cvid */
			    sizeof(i4)		+	/* cp_cvlen */
			    sizeof(i4)		+	/* cp_withnull */
			    sizeof(i4)));		/* cp_nullen */

	    for (i = 0, cpdom_desc = qe_copy->qeu_fdesc->cp_fdom_desc;
		i < cp_num_attr;
	        i++, cpdom_desc = cpdom_desc->cp_next)
	    {
		data_size += cpdom_desc->cp_nullen;
	    }
	}

	if (qe_copy->qeu_sbuf != NULL)
	    sc0m_deallocate(0, &qe_copy->qeu_sbuf);
	status = sc0m_allocate(0,
			    (i4)(sizeof(SCC_GCMSG)
			    + Sc_main_cb->sc_gchdr_size
			    + data_size),
			    DB_SCF_ID,
			    (PTR) SCS_MEM,
			    CV_C_CONST_MACRO('S','C','P','Y'),
			    &block);
	message = (SCC_GCMSG *)block;
	if (DB_FAILURE_MACRO(status))
	{
	    sc0e_0_put(status, 0);
	    sc0e_0_uput(status, 0);
	    sc0e_0_uput(E_SC0206_CANNOT_PROCESS, 0);
	    return (E_DB_ERROR);
	}
	message->scg_next = (SCC_GCMSG *) &scb->scb_cscb.cscb_mnext;
	message->scg_prev = scb->scb_cscb.cscb_mnext.scg_prev;
	scb->scb_cscb.cscb_mnext.scg_prev->scg_next = message;
	scb->scb_cscb.cscb_mnext.scg_prev = message;
	message->scg_msize = data_size;

        /*
        **  New message type for GCA_PROTOCOL_LEVEL_50
        */
        if (scb->scb_cscb.cscb_version >= GCA_PROTOCOL_LEVEL_50)
	{
	    if (qe_copy->qeu_direction == CPY_FROM)
	    {
		message->scg_mtype = GCA1_C_FROM;
	    }
	    else
	    {
		message->scg_mtype = GCA1_C_INTO;
	    }
	}
	else
        {
            if (qe_copy->qeu_direction == CPY_FROM)
            {
                message->scg_mtype = GCA_C_FROM;
            }
            else
            {
                message->scg_mtype = GCA_C_INTO;
            }
        }

	if (	(scb->scb_cscb.cscb_different == TRUE)
	     || (sqlda->gca_result_modifier & GCA_LO_MASK))
	{
	    message->scg_mask =
		(SCG_NODEALLOC_MASK |SCG_QDONE_DEALLOC_MASK);
	}
	else
	{
	    message->scg_mask = 0;
	}

	scb->scb_cscb.cscb_gcp.gca_fo_parms.gca_buffer =
	    message->scg_marea =
		((char *) message + sizeof(SCC_GCMSG));
	scb->scb_cscb.cscb_gcp.gca_fo_parms.gca_b_length =
	    Sc_main_cb->sc_gchdr_size + data_size;

	status = IIGCa_call(GCA_FORMAT,
			    &scb->scb_cscb.cscb_gcp,
			    GCA_SYNC_FLAG,
			    (PTR)0,
			    0,
			    &error);
	if (status)
	{
	    sc0e_0_put(error, 0);
	    scd_note(E_DB_SEVERE, DB_GCF_ID);
	}

	data = (char *) scb->scb_cscb.cscb_gcp.gca_fo_parms.gca_data_area;
	msg_start = data;
	buffer_size = scb->scb_cscb.cscb_gcp.gca_fo_parms.gca_d_length;

	ln_val = (i4)qe_copy->qeu_stat;
	MEcopy((PTR)&ln_val, sizeof(i4), (PTR)data);
	data += sizeof(i4);

	ln_val = (i4)qe_copy->qeu_direction;
	MEcopy((PTR)&ln_val, sizeof(i4), (PTR)data);
	data += sizeof(i4);

	ln_val = (i4)qe_copy->qeu_maxerrs;
	MEcopy((PTR)&ln_val, sizeof(i4), (PTR)data);
	data += sizeof(i4);

	n_val = (i4)filename_length;
	MEcopy((PTR)&n_val, sizeof(i4), (PTR)data);
	data += sizeof(i4);

	if (filename_length)
	{
	    MEcopy((PTR)qe_copy->qeu_fdesc->cp_filename, filename_length,
		(PTR)data);
	    data += filename_length;
	}

	n_val = (i4)logname_length;
	MEcopy((PTR)&n_val, sizeof(i4), (PTR)data);
	data += sizeof(i4);

	if (logname_length)
	{
	    MEcopy((PTR)qe_copy->qeu_logname, logname_length, (PTR)data);
	    data += logname_length;
	}

	/* Now let's copy the tuple descriptor */
	if (	(scb->scb_cscb.cscb_different == TRUE)
	     || (sqlda->gca_result_modifier & GCA_LO_MASK))
	{
	    scb->scb_sscb.sscb_cquery.cur_rdesc.rd_tdesc = data;
	}
	else
	{
	    scb->scb_sscb.sscb_cquery.cur_rdesc.rd_tdesc = NULL;
	}
	ln_val = (i4)sqlda->gca_tsize;
	MEcopy((PTR)&ln_val, sizeof(i4), (PTR)data);
	data += sizeof(i4);

	n_val = sqlda->gca_result_modifier;  /* gca_result_modifier */
	scb->scb_sscb.sscb_cquery.cur_rdesc.rd_modifier =
	    sqlda->gca_result_modifier;
	MEcopy((PTR)&n_val, sizeof(i4), (PTR)data);
	data += sizeof(i4);

	ln_val = scb->scb_sscb.sscb_nostatements;   /* gca_id_tdscr */
	MEcopy((PTR) &ln_val, sizeof(u_i4), (PTR) data);
	data += sizeof (u_i4);

	ln_val = (i4)sqlda->gca_l_col_desc;
	scb->scb_sscb.sscb_copy.cp_att_count = ln_val;
	MEcopy((PTR)&ln_val, sizeof(i4), (PTR)data);
	data += sizeof(i4);

	for (i = 0; i < sqlda->gca_l_col_desc; i++)
	{
	    MEcopy((PTR)&sqlda->gca_col_desc[i].gca_attdbv,
		   sizeof(sqlda->gca_col_desc[0].gca_attdbv),
		   (PTR)data);
	    data += sizeof(sqlda->gca_col_desc[0].gca_attdbv);

	    /* no names are sent */
	    MEfill( sizeof(sqlda->gca_col_desc[0].gca_l_attname), 
		    '\0', (PTR)data);
	    data += sizeof(sqlda->gca_col_desc[0].gca_l_attname);
	}

	ln_val = cp_num_attr;
	MEcopy((PTR)&ln_val, sizeof(i4), (PTR)data);
	data += sizeof(i4);

	for (i = 0, cpdom_desc = qe_copy->qeu_fdesc->cp_fdom_desc;
	     i < cp_num_attr;
	     i++, cpdom_desc = cpdom_desc->cp_next)
	{
	    MEcopy((PTR) cpdom_desc->cp_domname, DB_MAXNAME, (PTR)data);
	    MEfill((i4) GCA_MAXNAME - DB_MAXNAME, ' ', (PTR) data + DB_MAXNAME);
	    data += GCA_MAXNAME;

	    n_val = (i4)cpdom_desc->cp_type;
	    MEcopy((PTR)&n_val, sizeof(i4), (PTR)data);
	    data += sizeof(i4);
	    
	    /*
	    **  Add precision for GCA_PROTOCOL_LEVEL_50
	    */
	    if (scb->scb_cscb.cscb_version >= GCA_PROTOCOL_LEVEL_50)
	    {
		n_val = (i4)cpdom_desc->cp_prec;	
		MEcopy((PTR)&n_val, sizeof(i4), (PTR)data);
		data += sizeof(i4);
	    }		

	    ln_val = (i4)cpdom_desc->cp_length;
	    MEcopy((PTR)&ln_val, sizeof(i4), (PTR)data);
	    data += sizeof(i4);

	    n_val = (i4)cpdom_desc->cp_user_delim;
	    MEcopy((PTR)&n_val, sizeof(i4), (PTR)data);
	    data += sizeof(i4);

	    n_val = CPY_MAX_DELIM;
	    MEcopy((PTR)&n_val, sizeof(i4), (PTR)data);
	    data += sizeof(i4);
	    
	    MEcopy((PTR)cpdom_desc->cp_delimiter,CPY_MAX_DELIM,(PTR)data);
	    data += CPY_MAX_DELIM;

	    ln_val = (i4)cpdom_desc->cp_tupmap;
	    MEcopy((PTR)&ln_val, sizeof(i4), (PTR)data);
	    data += sizeof(i4);

	    n_val = (i4)cpdom_desc->cp_cvid;
	    MEcopy((PTR)&n_val, sizeof(i4), (PTR)data);
	    data += sizeof(i4);

	    /*
	    **  Add cp_cvprec for GCA_PROTOCOL_LEVEL_50
	    */
	    if (scb->scb_cscb.cscb_version >= GCA_PROTOCOL_LEVEL_50)
	    {
		n_val = (i4)cpdom_desc->cp_cvprec;
		MEcopy((PTR)&n_val, sizeof(i4), (PTR)data);
		data += sizeof(i4);
	    }		

	    ln_val = (i4)cpdom_desc->cp_cvlen;
	    MEcopy((PTR)&ln_val, sizeof(i4), (PTR)data);
	    data += sizeof(i4);

	    n_val = (i4)cpdom_desc->cp_withnull;
	    MEcopy((PTR)&n_val, sizeof(i4), (PTR)data);
	    data += sizeof(i4);

	    if (scb->scb_cscb.cscb_version >= GCA_PROTOCOL_LEVEL_50)
	    {
		/* if cp_nullen > 0 then null value descriptor is given */

		n_val = (cpdom_desc->cp_nullen ? (i4)TRUE : (i4)FALSE);
		MEcopy((PTR)&n_val, sizeof(i4), (PTR)data);
		data += sizeof(i4);
		    
		if( cpdom_desc->cp_nullen)
		{
		    /* 
		    ** The data type for the NULL descriptor is the
		    ** same as the row descriptor.
		    */

		    gdv.gca_type = cpdom_desc->cp_type; 
		    gdv.gca_precision = (i4)cpdom_desc->cp_nulprec; 
		    gdv.gca_l_value = cpdom_desc->cp_nullen;

		    if ((sqlda->gca_result_modifier & GCA_LO_MASK) != 0) 
		    {
			status = adi_dtinfo((ADF_CB *) scb->scb_sscb.sscb_adscb,
					    gdv.gca_type,
					    &n_val);
			if (DB_FAILURE_MACRO(status))
			{
			    sc0e_0_put(((ADF_CB *)
			       scb->scb_sscb.sscb_adscb)->adf_errcb.ad_errcode,
				     0);
			    sc0e_0_uput(((ADF_CB *)
			       scb->scb_sscb.sscb_adscb)->adf_errcb.ad_errcode,
				     0);
			    sc0e_0_uput(E_SC0206_CANNOT_PROCESS, 0);
			    return (E_DB_ERROR);
			}

			if (n_val & AD_PERIPHERAL)
			{
			    i4       res_len;
			    i4       res_prec;

			    status = scs_redeem(scb,
						0,
						gdv.gca_type,
						gdv.gca_precision,
						DB_MAXTUP,
						cpdom_desc->cp_nuldata,
						&res_prec,
						&res_len,
						data
						    + sizeof(gdv.gca_type)
						    + sizeof(gdv.gca_precision)
						    + sizeof(gdv.gca_l_value));
			    if (status)
			    {
				/*
				** Error -- null value too long.
				*/

				sc0e_0_put(((ADF_CB *)
			  scb->scb_sscb.sscb_adscb)->adf_errcb.ad_errcode,
					 0);
				sc0e_0_uput(((ADF_CB *)
			   scb->scb_sscb.sscb_adscb)->adf_errcb.ad_errcode,
					  0);
				sc0e_0_uput(E_SC0206_CANNOT_PROCESS, 0);
				return (E_DB_ERROR);
			    }
			    gdv.gca_l_value = res_len;
			    gdv.gca_precision = res_prec;
			}
		    }

		    MEcopy((PTR)&gdv.gca_type, 
			   sizeof(gdv.gca_type), (PTR)data);
		    data += sizeof(gdv.gca_type);

		    MEcopy((PTR)&gdv.gca_precision, 
			   sizeof(gdv.gca_precision), (PTR)data);
		    data += sizeof(gdv.gca_precision);

		    MEcopy((PTR)&gdv.gca_l_value, 
			   sizeof(gdv.gca_l_value), (PTR)data);
		    data += sizeof(gdv.gca_l_value);

		    if ((n_val & AD_PERIPHERAL) == 0)

		    {
			/*
			** If a large object, then it was materialized
			** above.  It's length was placed in the
			** gca_l_value area already.
			*/

			MEcopy((PTR)cpdom_desc->cp_nuldata, 
			       gdv.gca_l_value, (PTR)data);
		    }
		    data += gdv.gca_l_value;
		}
	    }
	    else
	    {
                ln_val = (i4)cpdom_desc->cp_nullen;
		MEcopy((PTR)&ln_val, sizeof(i4), (PTR)data);
		data += sizeof(i4);

		if (cpdom_desc->cp_nullen)
		{
		    MEcopy((PTR)cpdom_desc->cp_nuldata, cpdom_desc->cp_nullen,
			   (PTR)data);
		    data += cpdom_desc->cp_nullen;
		}
	    } 
	}
	if (data_size_suspect)
	    message->scg_msize = data - msg_start;
    }

    return (E_DB_OK);
}

/*
** History:
**	2-Jul-1993 (daveb)
**	    prototyped.
*/
VOID
scs_copy_error(SCD_SCB	*scb,
                QEF_RCB	*qe_ccb,
                i4	copy_done )
{
    QEU_COPY	*qe_copy;
    QSF_RCB	*qs_ccb;
    i4		status = E_DB_OK;
    i4		i;

    qe_copy = qe_ccb->qeu_copy;
    qe_copy->qeu_error = TRUE;

    /*
    ** If ROLLBACK = DISABLED was specified, don't backout copy statement.
    */
    if ((qe_copy->qeu_stat & CPY_NOABORT) == 0)
	qe_copy->qeu_stat |= CPY_FAIL;

    /*
    ** Call QEF to end copy statement
    */
    status = qef_call(QEU_E_COPY, ( PTR ) qe_ccb);
    if (DB_FAILURE_MACRO(status))
    {
	if ((qe_ccb->error.err_code == E_QE0002_INTERNAL_ERROR)
	    || (qe_ccb->error.err_code == E_QE001D_FAIL))
	{
	    sc0e_0_put(E_SC0251_COPY_CLOSE_ERROR, 0);
	    sc0e_0_put(qe_ccb->error.err_code, 0);
	    scs_cpsave_err(qe_copy, E_SC0251_COPY_CLOSE_ERROR);
	}
	else /* rest are scs' fault */
	{
	    sc0e_0_put(E_SC0251_COPY_CLOSE_ERROR, 0);
	    sc0e_0_put(E_SC0024_INTERNAL_ERROR, 0);
	    sc0e_0_put(qe_ccb->error.err_code, 0);
	    sc0e_0_put(E_SC0210_SCS_SQNCR_ERROR, 0);
	    scs_cpsave_err(qe_copy, E_SC0251_COPY_CLOSE_ERROR);
	}
    }

    scb->scb_sscb.sscb_cquery.cur_row_count = qe_copy->qeu_count;

    /*
    ** If COPY is over, write appropriate error message depending on whether
    ** the COPY has been aborted or not.  Clean up memory allocated for COPY
    ** sequencing and destroy control blocks allocated in PSF.
    */
    if (copy_done)
    {
	for (i = 0; i < 5; i++)
	{
	    if (qe_copy->qeu_uputerr[i] != 0)
		sc0e_0_uput(qe_copy->qeu_uputerr[i], 0);
	}

	if (qe_copy->qeu_stat & CPY_FAIL)
	{
	    /* COPY: Copy has been aborted. */
	    sc0e_0_uput(5842L /* user error */, 0);
	}
	else
	{
	    char	buf[16];
	    /* COPY: Copy terminated abnormally.  %d rows successfully copied */

	    CVla(qe_copy->qeu_count, buf);
	    sc0e_uput(5841L /* user error */, 0, 1,
		      STlength(buf), (PTR) buf,
		      0, (PTR)0,
		      0, (PTR)0,
		      0, (PTR)0,
		      0, (PTR)0,
		      0, (PTR)0);		
	}

	if (qe_copy->qeu_sbuf != NULL)
	{
	    sc0m_deallocate(0, &qe_copy->qeu_sbuf);
	}
	if (scb->scb_sscb.sscb_cquery.cur_amem != NULL)
	{
	    sc0m_deallocate(0, &scb->scb_sscb.sscb_cquery.cur_amem);
	}

	qs_ccb = (QSF_RCB *) scb->scb_sscb.sscb_qsccb;
	qs_ccb->qsf_obj_id.qso_handle =
	    scb->scb_sscb.sscb_cquery.cur_qp.qso_handle;
	qs_ccb->qsf_lk_id =
	    scb->scb_sscb.sscb_cquery.cur_lk_id;
	status = qsf_call(QSO_DESTROY, qs_ccb);
	if (DB_FAILURE_MACRO(status))
	{
	    scs_qsf_error(status, qs_ccb->qsf_error.err_code,
		E_SC0210_SCS_SQNCR_ERROR);
	}
	scb->scb_sscb.sscb_cquery.cur_qe_cb = (PTR) 0;
	scb->scb_sscb.sscb_cquery.cur_qp.qso_handle = 0;
    }
}

/*
** History:  
**	2-Jul-1993 (daveb)
**	    prototyped.
**	24-jan-1994 (gmanning)
**	    Change errno to local_errno because of NT macro problems.
*/
VOID
scs_cpsave_err(QEU_COPY	*qe_copy,
                i4	local_errno )
{
    i4		i;

    for (i = 0; i < 5; i++)
    {
	if (qe_copy->qeu_uputerr[i] == 0)
	{
	    qe_copy->qeu_uputerr[i] = local_errno;
	    return;
	}
    }
}

/*{
** Name: scs_cpxmit_errors	- Transmit a batch of copy errors
**
** Description:
**      This routine is called from COPY FROM processing after 
**      the FE has indicated (by responding to the DBMS originated 
**      interrupt) that it is ready to hear about errors.  The error 
**      blocks are sent, followed by a response block which indicates 
**      that the copy is to continue.  If the copy is to be terminated, 
**      the response block is not sent, as it will be sent from the 
**      sequencer.
**
** Inputs:
**      scb                             Session control block
**	end_stream			!FALSE if this routine should
**					terminate the error stream with
**					a response block.
**
** Outputs:
**      None explicitly.   Errors are sent to the FE via GCF
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      01-Sep-1987 (fred)
**          Created.
**	2-Jul-1993 (daveb)
**	    prototyped, use proper PTR for allocation.
**      05-nov-1993 (smc)
**          Bug #58635
**          Made l_reserved & owner elements PTR for consistency with DM_SVCB.
**          This results in owner within sc0m_allocate having to be PTR.
**      22-mar-1996 (stial01)
**          Cast length passed to sc0m_allocate to i4.
[@history_template@]...
*/
VOID
scs_cpxmit_errors(SCD_SCB *scb,
		  i4  end_stream )
{
    i4		i;
    DB_STATUS	status;
    DB_STATUS	error;
    SCC_GCMSG	*message;
    QEU_COPY	*qe_copy =
		((QEF_RCB *) scb->scb_sscb.sscb_cquery.cur_qe_cb)->qeu_copy;
    PTR		block;

    for (i = 0; i < 5; i++)
    {
	if (qe_copy->qeu_uputerr[i] != 0)
	    sc0e_0_uput(qe_copy->qeu_uputerr[i], 0);
    }
    if (end_stream)
    {
	status = sc0m_allocate(0,
		    (i4)(sizeof(SCC_GCMSG)
			+ sizeof(GCA_RE_DATA)
			+ Sc_main_cb->sc_gchdr_size),
 		    DB_SCF_ID,
		    (PTR)SCS_MEM,
		    SCG_TAG,
		    &block);
	message = (SCC_GCMSG *)block;
	if (status)
	{
	    sc0e_0_put(status, 0);
	    sc0e_0_uput(status, 0);
	    scd_note(E_DB_SEVERE, DB_SCF_ID);
	}
	message->scg_next = 
	    (SCC_GCMSG *) &scb->scb_cscb.cscb_mnext;
	message->scg_prev = 
	    scb->scb_cscb.cscb_mnext.scg_prev;
	scb->scb_cscb.cscb_mnext.scg_prev->scg_next = 
	    message;
	scb->scb_cscb.cscb_mnext.scg_prev = 
	    message;
	message->scg_mask = 0;
	message->scg_mtype = GCA_RESPONSE;
	message->scg_msize = sizeof(GCA_RE_DATA);

	scb->scb_cscb.cscb_gcp.gca_fo_parms.gca_buffer =
	    message->scg_marea =
		((char *) message + sizeof(SCC_GCMSG));
	scb->scb_cscb.cscb_gcp.gca_fo_parms.gca_b_length =
	    Sc_main_cb->sc_gchdr_size + sizeof(GCA_RE_DATA);

	status = IIGCa_call(GCA_FORMAT,
			    &scb->scb_cscb.cscb_gcp,
			    GCA_SYNC_FLAG,
			    (PTR)0,
			    0,
			    &error);
	if (status)
	{
	    sc0e_0_put(error, 0);
	    scd_note(E_DB_SEVERE, DB_GCF_ID);
	}
	if (scb->scb_sscb.sscb_state != SCS_CP_ERROR)
	{
	    ((GCA_RE_DATA *)
		scb->scb_cscb.cscb_gcp.gca_fo_parms.gca_data_area)->gca_rqstatus
		    = GCA_CONTINUE_MASK;
	}
	else
	{
	    ((GCA_RE_DATA *)
		scb->scb_cscb.cscb_gcp.gca_fo_parms.gca_data_area)->gca_rqstatus
		    = GCA_FAIL_MASK;
	}
	
    }
}

/*{
** Name: scs_cpinterrupt()	- Send interrupt to the FE.
**
** Description:
**      This routine sends a COPY_ERROR interrupt to the FE 
**      to inform the FE that some errors have been encountered 
**      and that the protocol should be "turned around" so 
**      that the errors can be sent.
**
** Inputs:
**      scb                             Session control block
**
** Outputs:
**      None
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      01-Sep-1987 (fred)
**          Created.
**	2-Jul-1993 (daveb)
**	    prototyped.
*/
VOID
scs_cpinterrupt(SCD_SCB *scb )
{
    DB_STATUS           status;
    DB_STATUS		error;
    char		b[100];
    i4		cur_error;

    scb->scb_cscb.cscb_gcp.gca_fo_parms.gca_buffer = b;
    scb->scb_cscb.cscb_gcp.gca_fo_parms.gca_b_length =
	Sc_main_cb->sc_gchdr_size + sizeof(GCA_AT_DATA);

    status = IIGCa_call(GCA_FORMAT,
			&scb->scb_cscb.cscb_gcp,
			GCA_SYNC_FLAG,
			(PTR)0,
			0,
			&error);
    if (status)
    {
	sc0e_0_put(error, 0);
	scd_note(E_DB_SEVERE, DB_GCF_ID);
	return;
    }
    ((GCA_AT_DATA *) scb->scb_cscb.cscb_gcp.gca_fo_parms.gca_data_area)->
			gca_itype = GCA_COPY_ERROR;
    scb->scb_cscb.cscb_gco.gca_association_id =
	    scb->scb_cscb.cscb_assoc_id;
    scb->scb_cscb.cscb_gco.gca_buffer = b;
    scb->scb_cscb.cscb_gco.gca_msg_length = 
	Sc_main_cb->sc_gchdr_size + sizeof(GCA_AT_DATA);
    scb->scb_cscb.cscb_gco.gca_message_type = GCA_NP_INTERRUPT;
    scb->scb_cscb.cscb_gco.gca_end_of_data = TRUE;
    /*
    **	Flush SXF audit data prior to communicating with the user
    */
    status = sc0a_flush_audit(scb, &cur_error);

    if (status != E_DB_OK)
    {
	/*
	**	Write an error
	*/
	sc0e_0_put(status = cur_error, 0);
	scd_note(E_DB_SEVERE, DB_SXF_ID);
	return ;
    }

    status = IIGCa_call(GCA_SEND,
			(GCA_PARMLIST *)&scb->scb_cscb.cscb_gco,
			GCA_NO_ASYNC_EXIT,
			(PTR)0,
			-1,
			&error);
    scb->scb_cscb.cscb_gco.gca_status = OK;	/* Don't really care about  */
						/* error		    */
    if (status)
    {
	sc0e_0_put(status = error, 0);
    }
}

/*{
** Name:	scs_cp_cpnify - Turn large objects into coupons during copy
**
** Description:
**	This routine is used during a copy from a file to a table (a
**	COPY FROM).  It scans through the tuple descriptor looking for
**	attributes which are peripheral objects (aka large objects).
**	For each such attribute, it will change that data into a
**	coupon, replacing the inline peripheral data with the coupon
**	itself.
**
** Re-entrancy:
**	Yes.
**
** Inputs:
**	<param name>	<input value description>
**
** Outputs:
**	<param name>	<output value description>
**
** Returns:
**	<function return values>
**
** Exceptions: (if any)
**	None.
**
** Side Effects:
**  	None.
**
** History:
**	23-Feb-1993 (fred)
**  	    Created.
**	21-Dec-2001 (gordy)
**	    GCA_COL_ATT no longer defined with DB_DATA_VALUE: use DB_COPY macro.
**       6-Jul-2007 (hanal04) Bug 117209
**          Whilst crossing the fix for 117209 to main I noticed the same
**          buffer overrun is possible in the couponify.
*/
DB_STATUS
scs_cp_cpnify(SCD_SCB *scb,
	      char    *inbuffer,
	      i4       insize,
	      char    *outbuffer,
	      i4       outsize)
{
    DB_STATUS	    status = E_DB_OK;
    char    	    *ibp;
    char            *obp;
    DB_DATA_VALUE   att;
    GCA_TD_DATA	    *sqlda =
	(GCA_TD_DATA *) scb->scb_sscb.sscb_cquery.cur_rdesc.rd_tdesc;
    i4              current_get;
    i4              old_insize;
    DB_DATA_VALUE   local_dv;
    char            *att_ptr;
    union {
        ADP_PERIPHERAL	    periph;
	char		    buf[sizeof(ADP_PERIPHERAL) + 1];
    }		    pdv;

    /* If at beginning of tuple, say it correctly */
    if (scb->scb_sscb.sscb_copy.cp_cur_att < 0)
    {
	scb->scb_sscb.sscb_copy.cp_cur_att = 0;
	scb->scb_sscb.sscb_copy.cp_out_offset = 0;
	DB_COPY_ATT_TO_DV( &sqlda->gca_col_desc[0], &att );
	scb->scb_sscb.sscb_copy.cp_att_need = att.db_length;
    }
    obp = outbuffer + scb->scb_sscb.sscb_copy.cp_out_offset;

    if (scb->scb_sscb.sscb_copy.cp_in_offset < 0)
    {
	scb->scb_sscb.sscb_copy.cp_in_offset = 0;
    }
    ibp = inbuffer + scb->scb_sscb.sscb_copy.cp_in_offset;
    insize -= scb->scb_sscb.sscb_copy.cp_in_offset;

    /*
    ** Note that this MEcopy() is necessary since the sqlda part of
    ** the actual GCA message may not be aligned.  It depends on
    ** things like the copyfile name, log name, etc.
    */
    

    att_ptr = (char *) sqlda->gca_col_desc;
    att_ptr += (scb->scb_sscb.sscb_copy.cp_cur_att *
		(sizeof(sqlda->gca_col_desc[0].gca_attdbv)
		 + sizeof(sqlda->gca_col_desc[0].gca_l_attname)));
    
    DB_COPY_ATT_TO_DV( att_ptr, &att );

    while ((status == E_DB_OK) && (insize > 0))
    {
	if (att.db_length != ADE_LEN_UNKNOWN)
	{
	    current_get = min(insize, scb->scb_sscb.sscb_copy.cp_att_need);
            current_get = min(outsize, current_get);
	    MEcopy((PTR) ibp, current_get, (PTR) obp);
	    insize -= current_get;
	    ibp += current_get;
	    obp += current_get;
	    scb->scb_sscb.sscb_copy.cp_in_offset += current_get;
	    scb->scb_sscb.sscb_copy.cp_out_offset += current_get;
	    scb->scb_sscb.sscb_copy.cp_att_need -= current_get;
	}
	else
	{
	    STRUCT_ASSIGN_MACRO(att, local_dv);
	    current_get = sizeof(ADP_PERIPHERAL);
	    if (local_dv.db_datatype < 0)
		current_get += 1;

	    MEcopy((PTR) obp, current_get, (PTR) pdv.buf);
	    local_dv.db_length = current_get;
	    local_dv.db_data = pdv.buf;

	    scb->scb_sscb.sscb_gcadv.gca_type = local_dv.db_datatype;
	    scb->scb_sscb.sscb_gcadv.gca_precision = local_dv.db_prec;
	    scb->scb_sscb.sscb_dmm =
		scb->scb_sscb.sscb_copy.cp_in_lobj;
	    old_insize = insize;
	    status = scs_blob_fetch(scb,
				    &local_dv,
				    &insize,
				    &ibp,
				    1 /* Called from a copy */ );
	    MEcopy((PTR) pdv.buf, current_get, (PTR) obp);
	    scb->scb_sscb.sscb_copy.cp_in_lobj = scb->scb_sscb.sscb_dmm;
	    scb->scb_sscb.sscb_dmm = 0;
	    scb->scb_sscb.sscb_copy.cp_in_offset += (old_insize - insize);
	    if (status == E_DB_OK)
	    {
		scb->scb_sscb.sscb_copy.cp_in_lobj = 0;
		scb->scb_sscb.sscb_copy.cp_att_need = 0;
		obp += current_get;
		scb->scb_sscb.sscb_copy.cp_out_offset += current_get;
	    }
	    else
	    {
		if (DB_FAILURE_MACRO(status))
		{
		    scb->scb_sscb.sscb_copy.cp_in_lobj = 0;
		    /* reset after faking out scs_blob_fetch() */
		}
		else
		{
		    /*
		    ** The couponify worked, but consumed all the
		    ** input.  Mark the buffer used so our caller
		    ** will get us some more...
		    */

		    scb->scb_sscb.sscb_copy.cp_in_offset = -1;
		}
		break;
	    }
	}
	if (scb->scb_sscb.sscb_copy.cp_att_need == 0)
	{
	    scb->scb_sscb.sscb_copy.cp_cur_att += 1;
	    if (scb->scb_sscb.sscb_copy.cp_cur_att >=
		    scb->scb_sscb.sscb_copy.cp_att_count)
	    {
		scb->scb_sscb.sscb_copy.cp_cur_att = -1;
		break;
	    }
	    /*
	    ** Note that this MEcopy() is necessary since the sqlda part of
	    ** the actual GCA message may not be aligned.  It depends on
	    ** things like the copyfile name, log name, etc.
	    */
	    
	    att_ptr = (char *) sqlda->gca_col_desc;
	    att_ptr += (scb->scb_sscb.sscb_copy.cp_cur_att *
			(sizeof(sqlda->gca_col_desc[0].gca_attdbv)
			    + sizeof(sqlda->gca_col_desc[0].gca_l_attname)));

	    DB_COPY_ATT_TO_DV( att_ptr, &att );
	    scb->scb_sscb.sscb_copy.cp_att_need = att.db_length;
	}
    }
    if (insize == 0)
	scb->scb_sscb.sscb_copy.cp_in_offset = -1;

    if ((status != E_DB_OK) || (scb->scb_sscb.sscb_copy.cp_cur_att > 0))
    {
	if (status == E_DB_OK)
	{
	    status = E_DB_INFO;
	}
    }
    return(status);
}

/*{
** Name:	scs_cp_redeem - Expand large objects during copy
**
** Description:
**	This routine is used during a copy into a file from a table (a
**	COPY INTO).  It scans through the tuple descriptor looking for
**	attributes which are peripheral objects (aka large objects).
**	For each such attribute, it will expand that coupon into data,
**	replacing the inline coupon with the peripheral data itself.
**
** Re-entrancy:
**	Yes.
**
** Inputs:
**	<param name>	<input value description>
**
** Outputs:
**	<param name>	<output value description>
**
** Returns:
**	<function return values>
**
** Exceptions: (if any)
**	None.
**
** Side Effects:
**  	None.
**
** History:
**	24-Feb-1993 (fred)
**  	    Created.
**	19-nov-1996 (kch)
**	    We now calculate the number of non LO bytes in the row and
**	    decrement this total by the number of non LO bytes copied.
**	    When we come to an LO column, we call scs_redeem() with the
**	    remaining length of the copy buffer minus the number of non LO
**	    bytes still to copy, thus preventing scs_redeem() from leaving
**	    an insufficient amount of space in the buffer for the remaining
**	    non LO data. This change fixes bug 79287.
**	21-Dec-2001 (gordy)
**	    GCA_COL_ATT no longer defined with DB_DATA_VALUE: use DB_COPY macro.
**       5-Jul-2007 (hanal04) Bug 117209
**          Rewrite the fix for Bug 79287. When we are writing a non-LO
**          column we need to make sure current_get is not greater than
**          the remaining space in the output buffer.
*/
DB_STATUS
scs_cp_redeem(SCD_SCB *scb,
	      char    *inbuffer,
	      i4       insize,
	      char    *outbuffer,
	      i4       outsize)
{
    DB_STATUS	    status = E_DB_OK;
    char    	    *ibp;
    char            *obp;
    DB_DATA_VALUE   att;
    i4              current_get;
    i4              res_prec;
    i4              res_len;
    char            *att_ptr;
    GCA_TD_DATA	    *sqlda =
	(GCA_TD_DATA *) scb->scb_sscb.sscb_cquery.cur_rdesc.rd_tdesc;
    i4		    i;
    i4		    outsize_save;

    /* If at beginning of tuple, say it correctly */

    if (scb->scb_sscb.sscb_copy.cp_cur_att < 0)
    {
	scb->scb_sscb.sscb_copy.cp_cur_att = 0;
	DB_COPY_ATT_TO_DV( &sqlda->gca_col_desc[0], &att );
	scb->scb_sscb.sscb_copy.cp_att_need = att.db_length;
    }
    scb->scb_sscb.sscb_copy.cp_out_offset = 0;
    obp = outbuffer;

    if (scb->scb_sscb.sscb_copy.cp_in_offset < 0)
    {
	scb->scb_sscb.sscb_copy.cp_in_offset = 0;
    }
    ibp = inbuffer + scb->scb_sscb.sscb_copy.cp_in_offset;

    /*
    ** Note that this MEcopy() is necessary since the sqlda part of
    ** the actual GCA message may not be aligned.  It depends on
    ** things like the copyfile name, log name, etc.
    */

    att_ptr = (char *) sqlda->gca_col_desc;
    att_ptr += (scb->scb_sscb.sscb_copy.cp_cur_att *
		(sizeof(sqlda->gca_col_desc[0].gca_attdbv)
		 + sizeof(sqlda->gca_col_desc[0].gca_l_attname)));
    
    DB_COPY_ATT_TO_DV( att_ptr, &att );

    while ((status == E_DB_OK) && (outsize > 0))
    {
	if (att.db_length != ADE_LEN_UNKNOWN)
	{
	    current_get = min(insize, scb->scb_sscb.sscb_copy.cp_att_need);
            current_get = min(outsize, current_get);
	    MEcopy((PTR) ibp, current_get, (PTR) obp);
	    insize -= current_get;
	    outsize -= current_get;
	    ibp += current_get;
	    obp += current_get;
	    scb->scb_sscb.sscb_copy.cp_in_offset += current_get;
	    scb->scb_sscb.sscb_copy.cp_out_offset += current_get;
	    scb->scb_sscb.sscb_copy.cp_att_need -= current_get;
	    /* Calculate number of non LO bytes remaining */
	}
	else
	{
	    current_get = sizeof(ADP_PERIPHERAL);
	    if (att.db_datatype < 0)
		current_get += 1;

	    /*
	    ** Bug 79287 - save outsize and call scs_redeem() with the
	    ** number of bytes left in the copy buffer after we have
	    ** accounted for the remaining non LO bytes.
	    */
	    status = scs_redeem(scb,
				scb->scb_sscb.sscb_copy.cp_in_lobj,
				att.db_datatype,
				att.db_prec,
				outsize,
				(PTR) ibp,
				&res_prec,
				&res_len,
				(PTR) obp);

	    scb->scb_sscb.sscb_copy.cp_out_offset += res_len;
	    if (status == E_DB_OK)
	    {
		scb->scb_sscb.sscb_copy.cp_in_lobj = 0;
		scb->scb_sscb.sscb_copy.cp_att_need = 0;

		ibp += current_get;
		scb->scb_sscb.sscb_copy.cp_in_offset += current_get;
		insize -= current_get;

		outsize -= res_len;
		obp += res_len;
	    }
	    else
	    {
		scb->scb_sscb.sscb_copy.cp_in_lobj = 1;
		outsize = 0;
		if (DB_FAILURE_MACRO(status))
		{
		    scb->scb_sscb.sscb_copy.cp_in_lobj = 0;
		    /* No longer in object since we'll bail out now */
		}
		break;
	    }
	}
	if (scb->scb_sscb.sscb_copy.cp_att_need == 0)
	{
	    scb->scb_sscb.sscb_copy.cp_cur_att += 1;
	    if (scb->scb_sscb.sscb_copy.cp_cur_att >=
		    scb->scb_sscb.sscb_copy.cp_att_count)
	    {
		scb->scb_sscb.sscb_copy.cp_cur_att = -1;
		scb->scb_sscb.sscb_copy.cp_in_offset = -1;
		status = E_DB_OK;
		break;
	    }
	    /*
	    ** Note that this MEcopy() is necessary since the sqlda part of
	    ** the actual GCA message may not be aligned.  It depends on
	    ** things like the copyfile name, log name, etc.
	    */
	    
	    att_ptr = (char *) sqlda->gca_col_desc;
	    att_ptr += (scb->scb_sscb.sscb_copy.cp_cur_att *
			(sizeof(sqlda->gca_col_desc[0].gca_attdbv)
			    + sizeof(sqlda->gca_col_desc[0].gca_l_attname)));

	    DB_COPY_ATT_TO_DV( att_ptr, &att );
	    scb->scb_sscb.sscb_copy.cp_att_need = att.db_length;
	}
    }

    if ((status != E_DB_OK) || (scb->scb_sscb.sscb_copy.cp_cur_att > 0))
    {
	if (status == E_DB_OK)
	{
	    status = E_DB_INFO;
	}
    }
	
    return(status);
}

/*
** {
** Name:	scs_redeem - Expand large objects
**
** Description:
**	This routine is a utility used during a copy to expand
**      It is used both to expand coupons when doing a copy 
**      into a file from a table (a COPY INTO) as well as during copy
**      map processing to expand large object null indicator values
**      (copy...(... colx = long varchar(0) with null ('faceplant'))).
**      
**      This routine simply sets up to call adu_redeem() to do all the
**      work, returning its status without comment.  It is up to the
**      caller of this routine to correctly interpret and log said
**      status if necessary.
**
** Re-entrancy:
**	Yes.
**
** Inputs:
**	<param name>	<input value description>
**
** Outputs:
**	<param name>	<output value description>
**
** Returns:
**	<function return values>
**
** Exceptions: (if any)
**	None.
**
** Side Effects:
**  	None.
**
** History:
**	23-Jun-1993 (fred)
**  	    Created.
**      10-Nov-1993 (fred)
**          Added parameter to scs_lowksp_alloc() routine to indicate
**          that the call is from copy processing.
**	11-May-2004 (schka24)
**	    Remove same.
*/
static DB_STATUS
scs_redeem(SCD_SCB  	*scb,
	   i4           continuation,
	   DB_DT_ID     datatype,
	   i4           prec,
	   i4      out_size,
	   PTR          cpn_ptr,
	   i4           *res_prec,
	   i4           *res_len,
	   PTR          out_area)
{
    DB_STATUS       status;
    ADP_LO_WKSP	    *lowksp;
    DB_DATA_VALUE   coupon_dv;
    DB_DATA_VALUE   result_dv;
    DB_DATA_VALUE   wksp_dv;
    union {
        ADP_PERIPHERAL	    periph;
	char		    buf[sizeof(ADP_PERIPHERAL) + 1];
    }		    pdv;

    for (;;)
    {
	coupon_dv.db_datatype = datatype;
	coupon_dv.db_prec = prec;
	coupon_dv.db_length = sizeof(ADP_PERIPHERAL);
	
	if (coupon_dv.db_datatype < 0)
	    coupon_dv.db_length += 1;
	
	STRUCT_ASSIGN_MACRO(coupon_dv, result_dv);
	
	coupon_dv.db_data = (char *) pdv.buf;
	
	/*
	 ** Must correctly set the length here, since in the tuple
	 ** descriptor (from which we are constructing the DBDV),
	 ** the length is specified as unknown.
	 */
	
	MEcopy((PTR) cpn_ptr, coupon_dv.db_length, (PTR) pdv.buf);
	result_dv.db_data = out_area;
	result_dv.db_length = out_size;

	if (result_dv.db_length > MAXI2)
	{
	    result_dv.db_length = MAXI2;
	}
	
	status = scs_lowksp_alloc(scb, (PTR *) &lowksp);
	if (status)
	    break;
	
	wksp_dv.db_data = (char *) lowksp;
	wksp_dv.db_length = sizeof(*lowksp);
	
	status = adu_redeem((ADF_CB *) scb->scb_sscb.sscb_adscb,
			    &result_dv,
			    &coupon_dv,
			    &wksp_dv,
			    continuation);
	MEcopy((PTR) pdv.buf, coupon_dv.db_length, (PTR) cpn_ptr);
	*res_len = result_dv.db_length;
	*res_prec = result_dv.db_prec;
	break;
    }
    return(status);
}
