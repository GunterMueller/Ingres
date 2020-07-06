/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <sl.h>
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
#define             OPB_MBFK             TRUE
#include    <opxlint.h> /* EXTERNAL DEFINITION */ 

/**
**
**  Name: OPBMBF.C - find the matching boolean factors for a keylist
**
**  Description:
**      This file contains routines which find matching boolean factors
**      associated with a keylist of joinop attributes
**
**  History:    
**      3-may-86 (seputis)    
**          initial creation
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**      14-sep-93 (smc)
**          Moved <cs.h> for CS_SID.
**      21-jan-1999 (hanch04)
**          replace nat and longnat with i4
**	26-oct-00 (inkdo01)
**	    Tidied by removing "ifdef 0"ed code and one commented bit. Changed
**	    to permit multi-att, non-hash keys to have multiple values 
**	    (IN clause) on 2nd or lower attribute.
[@history_line@]...
**/

/*}
** Name: OPB_MBFSTATE - state determined by type of limiting predicate analyzed
**
** Description:
**      Used to determine the useability of limiting
**      predicates with respect to keys.  Each constant represents a
**      state which is determined by the type of limiting predicates
**      which have been analyzed.  A new limiting predicate could cause
**      a state transition.
**
** History:
**      21-jun-86 (seputis)
**          initial creation 
[@history_line@]...
*/
typedef i4          OPB_MBFSTATE;

#define                 OPB_SALL        1
/* an isam, btree, or sorted heap was found so all types of keys would be
** useful for the first attribute (this is true in the case of single
** and multi attribute keys)
*/

#define                 OPB_SSINGLEALL  2
/* in a multi attribute key for isam, btree, or sorted heap ... the second
** and succeeding attributes can have "ranges" or exactly "one value".  In
** other words the second attribute cannot use a predicate of the form
** (r.a = 5) OR (r.a = b) ... why?
**
** Indeed, why?! (inkdo01 26/10/00). Even multi-attr hash keys can have 
** multiple values on a low order key. Code which assigns this value in 
** opb_mbf has now been removed, thus permitting IN-lists on low order
** columns of all index structures.
*/

#define                 OPB_SEQ         4
/* a multi attribute hash key was found so allow a boolean factor to have
** keys with multiple values
*/


/*}
** Name: OPB_BFPRIORITY - class of key being analyzed
**
** Description:
**      These constants describe the class of key which will be used
**      with the limiting boolean factor.  Thus, predicates which have
**      exactly one equality value will be preferred over those that have
**      more than one equality value, which are preferred over those that
**      contain a range key.
**
** History:
**      21-jun-86 (seputis)
**          initial creation
[@history_line@]...
*/
typedef i4          OPB_BFPRIORITY;

#define                 OPB_CEXACT      1
/* an ADC_KEXACTKEY predicate with exactly one value 
*/

#define                 OPB_CMULTI	2
/* an ADC_KEXACTKEY predicate with one or more values
*/

#define                 OPB_CRANGE      3
/* an ADC_KRANGEKEY, ADC_KLOWKEY, or ADC_KHIGHKEY was found boolean factor
** was found
*/

#define                 OPB_CNOTUSABLE  4
/* initial state prior to finding any limiting predicates for the key 
** - this constant must be the highest value and as a result the "lowest
** preference"; i.e. not boolean factors are usable when building key
*/


/*{
** Name: opb_bfclass	- find classification of boolean factor
**
** Description:
**      This routine is used to determine whether a boolean factor is 
**      useful in building a key.  The state variable determines whether
**      the class of the boolean factor is usable.  If the boolean factor
**      is usable then the classification is returned, otherwise a classication
**      indicating the boolean factor is not useable is returned.
**
** Inputs:
**      bfi                             boolean factor number which contains
**                                      the OPB_BFGLOBAL structure
**      bfkeyp                          ptr to keyinfo structure which 
**                                      contains the OPB_BFGLOBAL information
**      operator                        type of boolean factor being
**                                      analyzed
**      state                           state variable used to determine
**                                      if boolean factor type can be used in 
**                                      key
**
** Outputs:
**	Returns:
**	    class of boolean factor
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	4-may-86 (seputis)
**          initial creation from checktype in checktype.c
[@history_line@]...
*/
static OPB_BFPRIORITY
opb_bestbf(
	OPB_IBF            bfi,
	OPB_BFKEYINFO      *bfkeyp,
	OPB_MBFSTATE	   state,
	OPB_ATBFLST        *keydesc)
{
    OPB_BFPRIORITY      class;      /* class to be returned */

    /* look at cases in priority order */
    if ( (bfkeyp->opb_global->opb_single != OPB_NOBF)) /* was there a single
                                    ** value sargable boolean factor found? */
    {
	class = OPB_CEXACT;
        keydesc->opb_gbf = bfkeyp->opb_global->opb_multiple; /* save index of
                                            ** boolean factor which has single
                                            ** key */
    }
    else if (
	     (bfkeyp->opb_global->opb_multiple != OPB_NOBF) /* was there a multi
                                            ** value sargable boolean factor
                                            ** found */
             &&
             (state != OPB_SSINGLEALL)
            )
    {
	class = OPB_CMULTI;
        keydesc->opb_gbf = bfkeyp->opb_global->opb_multiple; /* save index of
                                            ** boolean factor which has multiple
                                            ** keys */
    }
    else if (
             (bfkeyp->opb_global->opb_range != OPB_NOBF) /* was there a range 
                                            ** type boolean factor found */
             &&
             (state != OPB_SEQ)
            )
    {
	class = OPB_CRANGE;
        keydesc->opb_gbf = bfi;		    /* save the boolean factor index 
                                            ** which contains OPB_BFGLOBAL
                                            ** structure */
    }
    else 
    {
	class = OPB_CNOTUSABLE;             /* no useful boolean factor
                                            ** exists for the storage structure
                                            */
    }

    return (class);

}

/*{
** Name: opb_mbf	- find matching boolean factors for keylist
**
** Description:
**	See if there are the right matching boolean factors for
**	the given attributes so that they can be used to key the
**	relation.
**
** Inputs:
**      subquery                        ptr to subquery being analyzed
**      storage                         storage structure associated with
**                                      keylist
**      mbf                             matching boolean factor list
**                                      - contains keylist on which the
**                                      relation is ordered, but the matching
**                                      boolean factors have not been found
**
** Outputs:
**      mbf                             ptr to structure which will contain
**                                      the matching boolean factors for the
**                                      attributes in the dmfkeylist
**	Returns:
**	    TRUE if a limiting boolean factor has been found
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	4-may-86 (seputis)
**
**	25-apr-94 (rganski)
**	    b59982 - added histdt parameter to call to opb_bfkget(). See
**	    comment in opb_bfkget() for more details.
**	26-oct-00 (inkdo01)
**	    Dropped use of OPB_SSINGLEALL since its sole purpose appeared
**	    to be to prevent IN-lists on lower order columns of multi-att,
**	    non-hash key structures. Just didn't make sense.
**	22-nov-01 (inkdo01)
**	    Check for "tid is null" and don't return key structure (these 
**	    may be outer join tricks and are useless for key access).
{@history_line@}...
*/
bool
opb_mbf(
	OPS_SUBQUERY       *subquery,
	OPO_STORAGE	   storage,
	OPB_MBF            *mbf)
{
    i4                  count;	/* count of number of active elements in the
                                ** the keylist
                                */
    OPB_MBFSTATE	state;  /* state transition variable which changes
                                ** depending on the class of the limiting
                                ** predicate which was found
                                */

    mbf->opb_maorder = NULL;
    if ( (count = mbf->opb_count) <= 0)
        return(FALSE);		/* keylist does not exist so this relation
                                ** does not have a useful ordering for this
                                ** query
                                */
    switch (storage)
    {

    case DB_HEAP_STORE:
        return(FALSE);		/* heap is not useful for limiting predicates*/
    case DB_HASH_STORE:
    {
	state = OPB_SEQ;
	break;
    }
    default:
	state = OPB_SALL;
    }
    
    {
	i4                    keyi;	/* offset in opb_keyorder array of
                                        ** of attribute being analyzed
                                        */
	OPZ_AT                 *abase;  /* base of joinop attributes array */
        OPB_BFT                *bfbase; /* base of boolean factor array */
	OPE_ET                 *ebase;  /* base of equivalence class array */

	abase = subquery->ops_attrs.opz_base; 
        bfbase = subquery->ops_bfs.opb_base;
        ebase = subquery->ops_eclass.ope_base;
	for (keyi = 0; keyi < count; keyi++)
	{
	    OPB_ATBFLST		*keydesc;/* ptr to key attribute being 
                                        ** analyzed */
	    OPZ_ATTS		*ap;	/* ptr to joinop attribute associated
                                        ** with key element
                                        */
	    OPE_IEQCLS		eqcls;	/* equivalence class of joinop
                                        ** attribute */
	    OPB_BFPRIORITY	bestclass; /* class of best limiting boolean 
                                        ** factor found so far */

	    bestclass = OPB_CNOTUSABLE; /* this is the least preferred class 
					** since it indicates that no 
                                        ** limiting predicates exist
					*/
	    keydesc = &mbf->opb_kbase->opb_keyorder[keyi];
	    ap = abase->opz_attnums[keydesc->opb_attno];
	    if ( (eqcls = ap->opz_equcls) >= 0) 
	    {	/* if equivalence class exists then check if a list of limiting
		** predicates is associated with the equivalence class
		*/
		OPB_IBF            bfi; /* boolean factor number which contains
                                        ** OPB_BFGLOBAL information */

		bfi = ebase->ope_eqclist[eqcls]->ope_nbf; /* get boolean factor
                                        ** which contains summary of information
                                        ** for this equivalence class */
                if (bfi != OPB_NOBF)	/* check if any boolean factors
                                        ** exist for this equivalence class */
		{
		    OPB_BOOLFACT    *bfp;/* ptr to boolean factor which
                                        ** contains the OPB_BFGLOBAL info
                                        ** for this equivalence class 
                                        */
		    OPB_BFKEYINFO   *bfinfop; /* ptr to bfkeyinfo for this
					** joinop attribute (type,length)
					*/
		    bfp = bfbase->opb_boolfact[bfi];

		    /* Check for "tid is null" - a predicate that's useless
		    ** for keying. */
		    if (count == 1 && 
			ebase->ope_eqclist[eqcls]->ope_eqctype == OPE_TID &&
			bfp->opb_qnode &&
			bfp->opb_qnode->pst_sym.pst_value.pst_s_op.pst_opno ==
			    subquery->ops_global->ops_cb->ops_server->opg_isnull)
		    {
			mbf->opb_bfcount = 0;
			return(FALSE);	/* show no key from TID */
		    }

		    bfinfop = opb_bfkget( subquery, bfp, eqcls, 
			&ap->opz_dataval, &ap->opz_histogram.oph_dataval, TRUE);
					/* get the keyinfo ptr
                                        ** for the type and length
                                        ** of the attribute, which
                                        ** contains the OPB_BFGLOBAL
                                        ** information for the
                                        ** attribute
                                        */
		    bestclass = opb_bestbf( bfi, bfinfop, state, keydesc);
		}
	    }
	    if (bestclass == OPB_CNOTUSABLE)
	    {   /* boolean factor for attribute not found so exit */
		if (storage == DB_HASH_STORE)
		{
		    mbf->opb_bfcount = 0;	/* boolean facts for
                                                ** all key attributes must exist
						** for hash key to be built
                                                */
		    return(FALSE);              /* key cannot be built */
		}
		mbf->opb_bfcount = keyi;        /* save the number of attributes
                                                ** which have useful keys */
		return( keyi > 0 );		/* partial key can be built */
	    }

	}
    }
    mbf->opb_bfcount = mbf->opb_count; /* all attributes have matching boolean
                                    ** factors */
    return( TRUE );		    /* boolean factors were found for key */
}

