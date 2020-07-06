/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <ulm.h>
#include    <ulf.h>
#include    <adf.h>
#include    <dmf.h>
#include    <dmtcb.h>
#include    <scf.h>
#include    <qsf.h>
#include    <qefrcb.h>
#include    <rdf.h>
#include    <psfparse.h>
#include    <qefnode.h>
#include    <qefact.h>
#include    <qefqp.h>
/* beginning of optimizer header files */
#include    <opglobal.h>
#include    <opdstrib.h>
#include    <opfcb.h>
#include    <opgcb.h>
#include    <opscb.h>
#include    <ophisto.h>
#include    <opboolfact.h>
#include    <oplouter.h>
#include    <opeqclass.h>
#include    <opcotree.h>
#include    <opvariable.h>
#include    <opattr.h>
#include    <openum.h>
#include    <opagg.h>
#include    <opmflat.h>
#include    <opcsubqry.h>
#include    <opsubquery.h>
#include    <opcstate.h>
#include    <opstate.h>

/* external routine declarations definitions */
#define        OPV_PARSER      TRUE
#include    <me.h>
#include    <bt.h>
#include    <opxlint.h>

/**
**
**  Name: OPVPARSER.C - init global range table element from parser's table
**
**  Description:
**      Routine to initialize the global range table element from the parser's 
**      table 
**
**  History:    
**      1-jul-86 (seputis)    
**          initial creation
**      16-aug-91 (seputis)
**          add CL include files for PC group
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**      16-sep-93 (smc)
**          Moved <cs.h> for CS_SID.
**      10-aug-98 (stial01)
**          opv_parser() Remove code to invalidate indexes. The invalidate
**          table that follows will do the job. Invalidate indexes by table id
**          can get E_OP008E_RDF_INVALIDATE, E_RD0045_ULH_ACCESS
[@history_line@]...
**/


/*{
** Name: opv_parser	- init global range table element given parser varno
**
** Description:
**      This routine will initialize the global range table element in OPF
**      corresponding to the PSF range table element.
**
** Inputs:
**      global                          ptr to global range table
**      gvar                            element in parser range table
**                                      which is referenced in query
**
** Outputs:
**      global->ops_rangetab.opv_base[gvar] initialize corresponding element
**                                      in optimizer range table
**	Returns:
**	    VOID
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	2-jul-86 (seputis)
**          initial creation
**	6-nov-88 (seputis)
**          change RDF invalidation to include all indexes on base relation
**          since this is only notification that OPF gets that its' time
**          stamp is out of date
**      25-sep-89 (seputis)
**          - made addition to timestamp check, to refresh if this is a
**          multi-variable query and the tuple count is zero
**	9-nov-89 (seputis)
**          - added OPA_NOVID initialization for b8160, and corrected
**	    sep 25 fix for timestamps
**	12-jan-90 (seputis)
**	    - detect table ID's which are the same for different range vars
**	26-feb-91 (seputis)
**	    - add improved diagnostics for b35862
**      31-dec-91 (seputis)
**          - flush cache entry if tuple count is zero and more than one
**          range table entry, since aggregate queries and flattened queries
**          are not getting handled.
**	12-feb-93 (jhahn)
**	    Added support for statement level rules. (FIPS)
**	12-apr-93 (ed)
**	    - fix bug 50673, relax range variable check for larger OPF table
**      7-dec-93 (ed)
**          b56139 - add OPZ_TIDHISTO to mark tid attributes which
**          need histograms,... needed since target list is traversed
**          earlier than before
**      16-feb-95 (inkdo01)
**          b66907 - check for explicit refs to inconsistent tabs/idxes
**      23-feb-95 (wolf) 
**          Recent addition of MEcopy call should have been accompanied by
**	    an include of me.h
**      10-aug-98 (stial01)
**          opv_parser() Remove code to invalidate indexes. The invalidate
**          table that follows will do the job. Invalidate indexes by table id
**          can get E_OP008E_RDF_INVALIDATE, E_RD0045_ULH_ACCESS
**	31-oct-1998 (nanpr01)
**	    Reset the rdfcb->infoblk ptr before checking the error code.
**	18-june-99 (inkdo01)
**	    Init opv_ttmodel for temp table model histogram feature.
**	19-Jan-2000 (thaju02)
**	    relation descriptor may be out of date and contain a stale 
**	    secondary index count, use rdfcb->rdf_info_block->rdr_no_index 
**	    which reflects the number of index entries in the rdr_indx 
**	    array. (b100060)
**	17-Jan-2004 (schka24)
**	    Rename RDR_BLD_KEY to RDR_BLD_PHYS.
**	3-Sep-2005 (schka24)
**	    Remove if-0'ed out section that included a ref to a member
**	    that is going away (opv_ghist).
**	17-Nov-2005 (schka24)
**	    Don't propagate RDF invalidates that we cause.  Our RDF cache
**	    is out of date but that's not other servers' problem.
[@history_line@]...
*/
bool
opv_parser(
	OPS_STATE          *global,
	OPV_IGVARS	   gvar,
	OPS_SQTYPE         sqtype,
	bool		   rdfinfo,
	bool               psf_table,
	bool               abort)
{
    OPV_GRT             *gbase;	    /* ptr to base of global range table */

# ifdef E_OP0387_VARNO
    if ((gvar < 0) || ((gvar >= PST_NUMVARS) && (gvar >= OPV_MAXVAR)))
	opx_error(E_OP0387_VARNO ); /* range var out of range - 
                                    ** consistency check */
# endif

    gbase = global->ops_rangetab.opv_base; /* get base of global range table
                                    ** ptr to array of ptrs */
    if ( !gbase->opv_grv[gvar] )
    {
	/* if global range variable element has not been allocated */
	OPV_GRV             *grvp;	    /* ptr to new range var element */
	
        if (global->ops_rangetab.opv_gv <= gvar)
            global->ops_rangetab.opv_gv = gvar+1; /* update the largest range
                                            ** table element assigned so far
                                            */
        grvp = (OPV_GRV *) opu_memory(global, sizeof( OPV_GRV ) ); /* save
                                            ** and allocate ptr to global
                                            ** var */
	/* Explicitly zero out the grv entry */
	MEfill(sizeof(*grvp), (u_char)0, (PTR)grvp);
	grvp->opv_qrt = gvar;		    /* save index to
                                            ** parser range table element */
        grvp->opv_created = sqtype;         /* save global range table type */
	grvp->opv_gvid = OPA_NOVID;	    /* if this base table was implicitly
					    ** referenced then the view id of the
					    ** explicitly reference view will be
					    ** saved */
        grvp->opv_siteid = OPD_NOSITE;      /* initialize distributed site location */
	grvp->opv_same = OPV_NOGVAR;	    /* used to map tables with similiar
					    ** IDs */
	grvp->opv_compare = OPV_NOGVAR;	    /* used to map tables with similiar
					    ** IDs */
	grvp->opv_ttmodel = NULL;	    /* RDR_INFO ptr for temp table model
					    ** histograms */
	gbase->opv_grv[gvar] = grvp;	    /* place into table */

        /* get RDF information about the table */
        if (rdfinfo)
        {
	    RDF_CB             *rdfcb;	    /* ptr to RDF control block which
                                            ** has proper db_id and sessionid
                                            ** info */
	    DB_STATUS          status;      /* RDF return status */

	    i4	       ituple;

            rdfcb = &global->ops_rangetab.opv_rdfcb;
	    if (psf_table)
	    {
		STRUCT_ASSIGN_MACRO(global->ops_qheader->pst_rangetab[gvar]->
		    pst_rngvar, rdfcb->rdf_rb.rdr_tabid); /* need 
						** table id from parser's table */
#if 0
                if ((BTnext((i4)-1, (char *)&rangep->pst_outer_rel,
                    (i4)BITS_IN(rangep->pst_outer_rel)) >= 0)
                    ||
                    (BTnext((i4)-1, (char *)&rangep->pst_inner_rel,
                    (i4)BITS_IN(rangep->pst_outer_rel)) >= 0)
                    )
                    grvp->opv_gmask |= OPV_GOJVAR; /* mark whether this
                                            ** variable is within the scope of an
                                            ** outer join */
/* OPV_GOJVAR is not reliably set since the subquery bitmap should be tested 
** also for an aggregate temp, multi-to-one mappings may exist for global range
** variables
*/
#endif
		if (global->ops_qheader->pst_rangetab[gvar]->
		    pst_rgtype == PST_SETINPUT)
		    grvp->opv_gmask |= OPV_SETINPUT; /* is this the set input
						    ** parameter for a procedure
						    */
		rdfcb->rdf_rb.rdr_types_mask = RDR_RELATION | RDR_INDEXES |
		    RDR_ATTRIBUTES | RDR_BLD_PHYS; /* get relation info 
						** - The optimizer uses attribute
						** info in query tree directly 
						** but it is needed to be requested
						** since the RDF uses attr info to
						** build RDR_BLK_KEY info.  The
						** attribute info does not need to
						** requested if RDF is changed.
						** - ask for indexes so that
						** invalidating the cache can also
						** invalidate any indexes in the
						** cache
						*/
/* When we're ready to enable column comparison stats, uncomment the
   following statement. **
		rdfcb->rdf_rb.rdr_2types_mask = RDR2_COLCOMPARE; /* column 
						** comparison stats, too */
		
	    }
	    rdfcb->rdf_info_blk = NULL;     /* get new ptr to info
                                            ** associated with global var */
            status = rdf_call( RDF_GETDESC, (PTR)rdfcb);
	    grvp->opv_relation = rdfcb->rdf_info_blk; /* save ptr to
                                            ** new info block */
	    if ((status != E_RD0000_OK)
		&&
		(rdfcb->rdf_error.err_code != E_RD026A_OBJ_INDEXCOUNT) /* this
					    ** is a check for distributed
					    ** index information, which could
					    ** be inconsistent but should not
					    ** cause the query to be aborted
					    ** it will cause indexes to be
					    ** avoided */
		)
	    {
		if (abort)
		    opx_verror( status, E_OP0004_RDF_GETDESC,
			rdfcb->rdf_error.err_code);
		else
		{
		    gbase->opv_grv[gvar] = NULL; /* deallocate
				** variable if index cannot be found */
		    return (TRUE);	/* indicate failure to get RDF
				    ** descriptor */
		}
	    }
            BTset( (i4)gvar, (char *)&global->ops_rangetab.opv_mrdf); /* indicate
                                            ** that RDF information is fixed */

	    /* Check table and all associated indexes to see if there is a
	    ** statement level index associated with this variable
	    */
	    if (grvp->opv_relation->rdr_rel->tbl_2_status_mask
		& DMT_STATEMENT_LEVEL_UNIQUE)
		grvp->opv_gmask |= OPV_STATEMENT_LEVEL_UNIQUE;
	    for( ituple = grvp->opv_relation->rdr_no_index;
		ituple-- ;)
	    {
		if (grvp->opv_relation->rdr_indx[ituple]->idx_status
		    & DMT_I_STATEMENT_LEVEL_UNIQUE)
		    grvp->opv_gmask |= OPV_STATEMENT_LEVEL_UNIQUE;
	    }
	    if (psf_table)
	    {	/* check if timestamp matches and invalidate RDF cache
                ** date of last modify do not match, this is only done
		** for tables passed by PSF, the other tables should
		** be dependent on the PSF time stamp */
		DB_TAB_TIMESTAMP	*timeptr; /* ptr to last modify
                                            ** date that RDF has cached */
		DB_TAB_TIMESTAMP	*psftimeptr; /* ptr to last modify
					    ** date which parser used for the
                                            ** table */
		psftimeptr = &global->ops_qheader->pst_rangetab[gvar]->
		    pst_timestamp;
		timeptr = &grvp->opv_relation->rdr_rel->tbl_date_modified;
                if (timeptr->db_tab_high_time != psftimeptr->db_tab_high_time
                    ||
                    timeptr->db_tab_low_time != psftimeptr->db_tab_low_time
                    ||
                    (
                        !grvp->opv_relation->rdr_rel->tbl_record_count
                        &&
                        (global->ops_qheader->pst_rngvar_count
                            > 1)
		     )			    /* special zero tuple count
                                            ** case check to see if tuple count
                                            ** is way off, i.e. a table create
                                            ** followed by a number of appends
                                            ** will cause a 0 tuple count, check
					    ** for more than one variable since
					    ** refresh is only useful when joins occur
					    */
                    )
		{
		    PTR save_fcb = rdfcb->rdf_rb.rdr_fcb;

		    /* Don't propagate this invalidate to other DBMS servers.
		    ** There's no reason to think that they are as out of
		    ** date as we are.  (plus this might be a session temp
		    ** which is strictly local!)
		    */

		    rdfcb->rdf_rb.rdr_fcb = NULL;
		    status = rdf_call( RDF_INVALIDATE, (PTR)rdfcb);
		    rdfcb->rdf_rb.rdr_fcb = save_fcb;
# ifdef E_OP008E_RDF_INVALIDATE
		    if (status != E_RD0000_OK)
			opx_verror( E_DB_ERROR, E_OP008E_RDF_INVALIDATE,
			    rdfcb->rdf_error.err_code);
# endif
		    status = rdf_call( RDF_GETDESC, (PTR)rdfcb);
		    grvp->opv_relation = rdfcb->rdf_info_blk; /* save ptr to
					    ** new info block */
		    if (status != E_RD0000_OK)
			opx_verror( E_DB_ERROR, E_OP0004_RDF_GETDESC,
			    rdfcb->rdf_error.err_code);
		    timeptr = &grvp->opv_relation->rdr_rel->tbl_date_modified;
		    if (timeptr->db_tab_high_time != psftimeptr->db_tab_high_time
			||
			timeptr->db_tab_low_time != psftimeptr->db_tab_low_time
			)
			opx_vrecover( E_DB_ERROR, E_OP008F_RDF_MISMATCH,
			    rdfcb->rdf_error.err_code); /* PSF timestamp is
					    ** still out of date, so tell
                                            ** SCF to reparse the query */
		}
		{   /* search thru existing tables to discover if any
		    ** tables have the same table ID */
		    OPV_IGVARS	    gvno;
		    DB_TAB_ID	    *tabidp;
		    OPV_IGVARS	    target_vno;

		    tabidp = &global->ops_qheader->pst_rangetab[gvar]->pst_rngvar;
		    target_vno = OPV_NOGVAR;

		    for (gvno = 0; gvno < OPV_MAXVAR; gvno++)
		    {
			OPV_GRV	    *grv1p;
			grv1p = gbase->opv_grv[gvno];
			if (grv1p && grv1p->opv_relation)
			{
			    DB_TAB_ID   *gtabidp;

			    if (gvno == gvar)
				continue;
			    gtabidp = &grv1p->opv_relation->rdr_rel->tbl_id;
			    if ((tabidp->db_tab_base == gtabidp->db_tab_base)
				&&
				(tabidp->db_tab_index == gtabidp->db_tab_index)
				)
			    {	/* found 2 table ID's which are identical */
				global->ops_gmask |= OPS_IDSAME;
				if (target_vno == OPV_NOGVAR)
				{   /* map all table id's vars to the lowest
				    ** global range number of the group */
				    if (gvno > gvar)
					target_vno = gvar;
				    else
					target_vno = gvno;
				}
				grv1p->opv_same = target_vno;
				grvp->opv_same = target_vno;
				if (target_vno != gvar)
				    break;
			    }
			}
		    }
		}
	    }
	    if (global->ops_cb->ops_smask & OPS_MDISTRIBUTED)
		opd_addsite(global, grvp); /* add site information if a
					** distributed thread */
            if (grvp->opv_relation && grvp->opv_relation->rdr_rel->
                   tbl_2_status_mask & DMT_INCONSIST)
                                        /* check for inconsistent table
                                        ** (due to partial back/recov) */
            {
               OPT_NAME     tabname;
               i2           i;
               
               MEcopy(&grvp->opv_relation->rdr_rel->tbl_name, 
                         sizeof(grvp->opv_relation->rdr_rel->tbl_name),
                         &tabname);           /* table name is msg token */
               for (i = sizeof(grvp->opv_relation->rdr_rel->tbl_name);
                      i > 1 && tabname.buf[i-1] == ' '; i--);
               tabname.buf[i] = 0;            /* lop blanks off name */
	       opx_1perror(E_OP009A_INCONSISTENT_TAB,(PTR)&tabname);    
            }
        }
	else
	    grvp->opv_relation = NULL;          /* set to NULL if not used */
    }
    return(FALSE);
}
