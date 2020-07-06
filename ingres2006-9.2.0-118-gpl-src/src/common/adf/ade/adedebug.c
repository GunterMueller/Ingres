/*
** Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cv.h>
#include    <tr.h>
#include    <si.h>
#include    <st.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <adf.h>
#include    <ulf.h>
#include    <adfint.h>
#include    <ade.h>
#include    <adefebe.h>
#include    <adeint.h>
#include    <adumoney.h>
#include    <adudate.h>

/*
[@#include@]...
*/

/**
**
**  Name: ADEDEBUG.C - Debugging aids for ADE.
**
**  Description:
**      This file contains routines that will assist in the debugging of the 
**      ADE module of ADF.
**
**	This file contains the following externally visible routines:
**	------------------------------------------------------------
**	    ade_cx_print() - Print the CX.
**	    ade_cxbrief_print() -   Print the CX in a compact form.
**
**	This file also contains the following static routines:
**	-----------------------------------------------------
**	    ade_klist_print() - Print a list of CX constants.
**	    ade_ilist_print() - Print a list of CX instructions.
**	    ade_kbrief_print() - Same as ade_klist_print() only smaller output.
**	    ade_ibrief_print() - same as ade_ilist_print() only smaller output.
**
**
**  History:
**      19-jul-86 (thurston)
**          Initial creation.  Split off from ADEEXECUTE.C.
**      06-dec-86 (eric)
**          added ade_cxbrief_print(), ade_kbrief_print(), 
**	    and ade_ibrief_print().
**	24-feb-87 (thurston)
**	    Moved definitions of forward static functions out of the routines
**	    that use them to the top of the file.
**	06-may-87 (thurston & wong)
**	    Integrated FE changes.
**	13-may-87 (thurston)
**	    Changed all end of offset list terminaters to be ADE_END_OFFSET_LIST
**	    instead of zero, since zero is a legal instruction offset for the
**	    FE's.
**	19-may-87 (thurston)
**	    Added extra argument cxbases to ade_cxbrief_print(), and used it to
**	    call ade_ibrief_print(), which now requires it for the FE's.
**	07-jul-87 (thurston)
**	    In ade_ilist_print(), get the next instruction offset through use
**	    of the proper MACRO.
**	10-feb-88 (thurston)
**	    Added code to both ade_cx_print() and ade_cxbrief_print() to display
**	    the new VIRGIN segment of a CX.
**	10-mar-88 (thurston)
**	    Added missing #ifdef ADE_BACKEND around the #include <scf.h>.
**	08-sep-88 (thurston)
**	    Added code to the ade_prinstr() routine to detect `coercions' and
**	    `copy_coercions' and print that out instead of `<unknown operator>'.
**	06-nov-88 (thurston)
**	    Removed some `READONLY' and `WSCREADONLY' stuff from statics.
**	28-feb-1989 (fred)
**	    Changed references to Adi_* global variables to be thru the server
**	    control block (Adf_globs) for support of user-defined abstract data
**	    types.
**	24-jul-89 (jrb)
**	    Added decimal and logical key types to table.  Also, removed code
**	    support for AD_BIT_TYPE since it is obsolete.
**      22-mar-91 (jrb)
**          Added #includes of st.h and si.h.
**      15-oct-91 (anitap)
**          Added support for ADE_ESC_CHAR, ADE_PNAME and ADE_DBDV.
**	17-jan-1992 (fred)
**	    Added large object support, specifically ADU_REDEEM support in the
**	    printout.
**	09-mar-92 (stec, merged by stevet)
**          Modified code in ade_2prvalue().
**	26-aug-92 (pearl, merged by stevet)
**	    Remove width argument for call to adf_printf() in case of
**	    DB_MNY_TYPE.  Since it is a string, the width isn't necessary;
**	    besides it causes SIdofrmt to SIGSEGV, as it expects pointers.
**      25-sep-92 (stevet)
**          Use FEXI function table to print adf trace message and 
**          remove reference to scf.h file.
**      29-dec-1992 (stevet)
**          Added function prototype.
**      17-mar-1993 (stevet)
**          Added missing precision passing in ade_kbrief_print() and 
**	14-may-1993 (robf)
**	    Add handling for DB_SECID_TYPE
**          ade_klist_print().  Print precision and scale for DB_DEC_TYPE.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	25-aug-93 (ed)
**	    remove dbdbms.h
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	21-june-01 (inkdo01)
**	    Added a few more data type names.
**	15-Feb-2005 (schka24)
**	    Use generic seg info in cxhead.
**	24-apr-06 (dougi)
**	    Added new date/time types.
**      16-oct-2006 (stial01)
**          Changes for locator datatypes.
**/

#define		ADE_PDTSMALL	0   /* Minimum datatype in this table */
#define		ADE_PDTBIG     99   /* Maximum datatype in this table */

/*
**  Definition of static variables and forward static functions.
*/

static VOID        ade_klist_print(PTR      base,
				   i4  koff);

static VOID        ade_ilist_print(PTR	    cxbases[],
				   PTR      base,
				   i4  ioff,
				   i4  loff);

static VOID        ade_kbrief_print(PTR      base,
				    i4  koff);

static VOID        ade_ibrief_print(PTR	     cxbases[],
				    PTR      base,
				    i4  ioff,
				    i4  loff);

/* Variable argument function, not prototype */
static VOID	   adf_printf();

static VOID	   ade_prinstr(ADI_FI_ID  *instr);

static VOID	   ade_2prvalue(VOID          (*fcn)(),
				DB_DATA_VALUE  *db_dv);

static VOID	   ade_prvalue(DB_DATA_VALUE  *db_dv);

static VOID	   ade_prtype(DB_DATA_VALUE  *db_dv);

static VOID	   ade_prdv(DB_DATA_VALUE  *db_dv);


static char	*type_name[] = {
/* 0 - 19 */
    NULL,	    "DB_BOO_TYPE",  NULL,	    "DB_DTE_TYPE",
    "DB_ADTE_TYPE", "DB_MNY_TYPE",  "DB_TMWO_TYPE", "DB_TMW_TYPE",
    "DB_TME_TYPE",  "DB_TSWO_TYPE", "DB_DEC_TYPE",  "DB_LOGKEY_TYPE",
    "DB_TABKEY_TYPE",NULL,	    "DB_BIT_TYPE",  "DB_VBIT_TYPE",
    NULL,	    NULL,	    "DB_TSW_TYPE",  "DB_TSTMP_TYPE",
/* 20 - 39 */
    "DB_CHA_TYPE",  "DB_VCH_TYPE",  "DB_LVCH_TYPE", "DB_BYTE_TYPE",
    "DB_VBYTE_TYPE","DB_LBYTE_TYPE","DB_NCHR_TYPE", "DB_NVCHR_TYPE",
    "DB_LNVCHR_TYPE","DB_LNLOC_TYPE","DB_INT_TYPE",  "DB_FLT_TYPE",
    "DB_CHR_TYPE",  "DB_INYM_TYPE", "DB_INDS_TYPE", "DB_LBLOC_TYPE",
    "DB_LCLOC_TYPE","DB_TXT_TYPE",  NULL,	    "DB_ALL_TYPE",
/* 40 - 59 */
    NULL,	    "DB_LTXT_TYPE", NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
/* 60 - 79 */
    "DB_SEC_TYPE", "DB_SECID_TYPE",	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
/* 80 - 99 */
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL
};



/*
**  Array of hex characters used when display a bit field:
*/

static char	ad0_hexchars[16] = { '0','1','2','3','4','5','6','7',
				     '8','9','A','B','C','D','E','F' };
/*
[@static_variable_or_function_definition@]...
*/


/*{
** Name: ade_cx_print() - Print the CX.
**
** Description:
**      This routine prints CX header, followed by the constants in the CX,
**	then the INIT instructions, then the MAIN instructions, and finally
**      the FINIT instructions.
**
** Inputs:
**	cxbases				Array of base addresses.  This is only
**					needed for the FRONTEND version
**					since they will need to look up the
**					corresponding DB_DATA_VALUE in their
**					DB_DATA_VALUEs array pointed to by
**					cxbases[ADE_DVBASE].
**      cx                              Ptr to the CX.
**
** Outputs:
**	none
**
**	Returns:
**	    VOID
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      16-jul-86 (thurston)
**          Initial creation.
**	10-feb-88 (thurston)
**	    Added code to display the new VIRGIN segment of a CX.
*/

# ifdef ADF_BUILD_WITH_PROTOS
VOID
ade_cx_print(
PTR                cxbases[],
PTR                cx)
# else
VOID
ade_cx_print( cxbases, cx)
PTR                cxbases[];
PTR                cx;
# endif
{
    ADE_CXHEAD          *cxhead = (ADE_CXHEAD *)cx;
    ADE_CXSEG		*v_seg = &cxhead->cx_seg[ADE_SVIRGIN];
    ADE_CXSEG		*i_seg = &cxhead->cx_seg[ADE_SINIT];
    ADE_CXSEG		*m_seg = &cxhead->cx_seg[ADE_SMAIN];
    ADE_CXSEG		*f_seg = &cxhead->cx_seg[ADE_SFINIT];
    i4             cxk_off = cxhead->cx_k_last_offset;
    char                v[7];
    i2			rw;
    f4			f4tmp;


    adf_printf("\f\n===============================================================================\n");
    adf_printf("\t\t\tS T A R T   O F   C X   D U M P\n");
    adf_printf("===============================================================================\n\n\n\n");


    /* Print the CX header */
    /* ------------------- */

    adf_printf("\n\n");
    adf_printf("\t\t\t\t+-------------+\n");
    adf_printf("\t\t\t\t|  CX HEADER  |\n");
    adf_printf("\t\t\t\t+-------------+\n");
    adf_printf("\t\t\t\taddress = %x\n\n\n", cxhead);
    f4tmp = (f4)cxhead->cx_hi_version + ((f4)cxhead->cx_lo_version / 100.0);
    CVfa(f4tmp, (i4) 7, (i4) 2, 'f', '.', &v[0], &rw);
    adf_printf("version = %6s\t  allocated bytes = %d\t  used bytes = %d\n",
		v,
		cxhead->cx_allocated,
		cxhead->cx_bytes_used);
    adf_printf("\n# VIRGIN instrs = %d\t  1st offset = %d\t  last offset = %d\n",
		v_seg->seg_n,
		v_seg->seg_1st_offset,
		v_seg->seg_last_offset);
    adf_printf("# INIT instrs   = %d\t  1st offset = %d\t  last offset = %d\n",
		i_seg->seg_n,
		i_seg->seg_1st_offset,
		i_seg->seg_last_offset);
    adf_printf("# MAIN instrs   = %d\t  1st offset = %d\t  last offset = %d\n",
		m_seg->seg_n,
		m_seg->seg_1st_offset,
		m_seg->seg_last_offset);
    adf_printf("# FINIT instrs  = %d\t  1st offset = %d\t  last offset = %d\n",
		f_seg->seg_n,
		f_seg->seg_1st_offset,
		f_seg->seg_last_offset);
    adf_printf("# constants    = %d\t\t\t\t  last offset = %d\n",
		cxhead->cx_n_k,
		cxhead->cx_k_last_offset);
    adf_printf("\n# VLTs = %d\t\t  highest base = %d\t  alignment used = %d\n",
		cxhead->cx_n_vlts,
		cxhead->cx_hi_base,
		cxhead->cx_alignment);
    adf_printf("\n-------------------------------------------------------------------------------\n");


    /* Now loop through all of the CX's constants and print them */
    /* --------------------------------------------------------- */

    if (cxhead->cx_n_k)
	adf_printf("\f\n>>>  Now the list of constants ...  <<<\n");
    else
	adf_printf("\f\n>>>  There are no constants in this CX  <<<\n");

    ade_klist_print(cx, cxk_off);


    /* Now loop through the VIRGIN segment, printing the instructions */
    /* -------------------------------------------------------------- */

    if (v_seg->seg_n)
    {
	adf_printf("\f\n>>>  Now the list of VIRGIN instructions ...  <<<\n");
	ade_ilist_print(cxbases, cx, v_seg->seg_1st_offset, v_seg->seg_last_offset);
    }
    else
	adf_printf("\f\n>>>  There are no VIRGIN instructions in this CX  <<<\n");


    /* Now loop through the INIT segment, printing the instructions */
    /* ------------------------------------------------------------ */

    if (i_seg->seg_n)
    {
	adf_printf("\f\n>>>  Now the list of INIT instructions ...  <<<\n");
	ade_ilist_print(cxbases, cx, i_seg->seg_1st_offset, i_seg->seg_last_offset);
    }
    else
	adf_printf("\f\n>>>  There are no INIT instructions in this CX  <<<\n");


    /* Now loop through the MAIN segment, printing the instructions */
    /* ------------------------------------------------------------ */

    if (m_seg->seg_n)
    {
	adf_printf("\f\n>>>  Now the list of MAIN instructions ...  <<<\n");
	ade_ilist_print(cxbases, cx, m_seg->seg_1st_offset, m_seg->seg_last_offset);
    }
    else
	adf_printf("\f\n>>>  There are no MAIN instructions in this CX  <<<\n");


    /* Now loop through the FINIT segment, printing the instructions */
    /* ------------------------------------------------------------- */

    if (f_seg->seg_n)
    {
	adf_printf("\f\n>>>  Now the list of FINIT instructions ...  <<<\n");
	ade_ilist_print(cxbases, cx, f_seg->seg_1st_offset, f_seg->seg_last_offset);
    }
    else
	adf_printf("\f\n>>>  There are no FINIT instructions in this CX  <<<\n");


    adf_printf("\n\n\n\n===============================================================================\n");
    adf_printf("\t\t\tE N D   O F   C X   D U M P\n");
    adf_printf("===============================================================================\n\f\n");
}


/*{
** Name: ade_cxbrief_print() - Print the CX in a compact form.
**
** Description:
**	This routine does the same thing as ade_cx_print(), only the output
**	is smaller, possibly at the expense of some detail. If you
**	are interested in finding bugs in ADE/ADF, then ade_cx_print() is the
**	routine to use. If you are interested in debugging code that calls
**	ADE/ADF, then ade_cxbrief_print() is the one to use.
**
** Inputs:
**	cxbases				Array of base addresses.  This is only
**					needed for the FRONTEND version
**					since they will need to look up the
**					corresponding DB_DATA_VALUE in their
**					DB_DATA_VALUEs array pointed to by
**					cxbases[ADE_DVBASE].
**      cx	    Ptr to the CX.
**	cxname	    Name of the CX, for outputing, or NULL.
**
** Outputs:
**	none
**
**	Returns:
**	    VOID
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      6-dec-86 (eric)
**          Initial creation, mostly stolen from ade_cx_print.
**	19-may-87 (thurston)
**	    Added extra argument cxbases to ade_cxbrief_print(), and used it to
**	    call ade_ibrief_print(), which now requires it for the FE's.
**	10-feb-88 (thurston)
**	    Added code to display the new VIRGIN segment of a CX.
*/

# ifdef ADF_BUILD_WITH_PROTOS
VOID
ade_cxbrief_print(
PTR		    cxbases[],
PTR		    cx,
char		    *cxname)
# else
VOID
ade_cxbrief_print( cxbases, cx, cxname)
PTR		    cxbases[];
PTR		    cx;
char		    *cxname;
# endif
{
    ADE_CXHEAD          *cxhead = (ADE_CXHEAD *)cx;
    ADE_CXSEG		*v_seg = &cxhead->cx_seg[ADE_SVIRGIN];
    ADE_CXSEG		*i_seg = &cxhead->cx_seg[ADE_SINIT];
    ADE_CXSEG		*m_seg = &cxhead->cx_seg[ADE_SMAIN];
    ADE_CXSEG		*f_seg = &cxhead->cx_seg[ADE_SFINIT];
    i4             cxk_off = cxhead->cx_k_last_offset;
    char                v[7];
    i2			rw;


    adf_printf("===============================================================================\n");
    if (cxname == NULL)
    {
	adf_printf("\t\tC X   D U M P\n");
    }
    else
    {
	adf_printf("\t\tC X   D U M P: %s\n", cxname);
    }
    adf_printf("===============================================================================\n");


    /* Now loop through all of the CX's constants and print them */
    /* --------------------------------------------------------- */

    if (cxhead->cx_n_k)
    {
	adf_printf(">>>  Now the list of constants ...  <<<\n");
	ade_kbrief_print(cx, cxk_off);
    }
    else
    {
	adf_printf(">>>  There are no constants in this CX  <<<\n");
    }


    /* Now loop through the VIRGIN segment, printing the instructions */
    /* -------------------------------------------------------------- */

    if (v_seg->seg_n)
    {
	adf_printf("\n>>>  Now the list of VIRGIN instructions ...  <<<\n");
	if (v_seg->seg_flags & ADE_CXSEG_PMQUEL)
	    adf_printf("[segment starts in pm_quel initially]\n");
	if (v_seg->seg_flags & ADE_CXSEG_FANCY)
	    adf_printf("[segment needs fancy handling]\n");
	ade_ibrief_print(cxbases, cx, v_seg->seg_1st_offset, v_seg->seg_last_offset);
    }
    else
	adf_printf(">>>  There are no VIRGIN instructions in this CX  <<<\n");


    /* Now loop through the INIT segment, printing the instructions */
    /* ------------------------------------------------------------ */

    if (i_seg->seg_n)
    {
	adf_printf("\n>>>  Now the list of INIT instructions ...  <<<\n");
	if (i_seg->seg_flags & ADE_CXSEG_PMQUEL)
	    adf_printf("[segment starts in pm_quel initially]\n");
	if (i_seg->seg_flags & ADE_CXSEG_FANCY)
	    adf_printf("[segment needs fancy handling]\n");
	ade_ibrief_print(cxbases, cx, i_seg->seg_1st_offset, i_seg->seg_last_offset);
    }
    else
	adf_printf(">>>  There are no INIT instructions in this CX  <<<\n");


    /* Now loop through the MAIN segment, printing the instructions */
    /* ------------------------------------------------------------ */

    if (m_seg->seg_n)
    {
	adf_printf("\n>>>  Now the list of MAIN instructions ...  <<<\n");
	if (m_seg->seg_flags & ADE_CXSEG_PMQUEL)
	    adf_printf("[segment starts in pm_quel initially]\n");
	if (m_seg->seg_flags & ADE_CXSEG_FANCY)
	    adf_printf("[segment needs fancy handling]\n");
	ade_ibrief_print(cxbases, cx, m_seg->seg_1st_offset, m_seg->seg_last_offset);
    }
    else
	adf_printf(">>>  There are no MAIN instructions in this CX  <<<\n");


    /* Now loop through the FINIT segment, printing the instructions */
    /* ------------------------------------------------------------- */

    if (f_seg->seg_n)
    {
	adf_printf("\n>>>  Now the list of FINIT instructions ...  <<<\n");
	if (f_seg->seg_flags & ADE_CXSEG_PMQUEL)
	    adf_printf("[segment starts in pm_quel initially]\n");
	if (f_seg->seg_flags & ADE_CXSEG_FANCY)
	    adf_printf("[segment needs fancy handling]\n");
	ade_ibrief_print(cxbases, cx, f_seg->seg_1st_offset, f_seg->seg_last_offset);
    }
    else
	adf_printf(">>>  There are no FINIT instructions in this CX  <<<\n");


    adf_printf("===============================================================================\n");
    adf_printf("\t\tE N D   O F   C X   D U M P\n");
    adf_printf("===============================================================================\n");
}


/*
** Name: ade_klist_print() - Print a list of CX constants.
**
** Description:
**      Routine to format and print the list of CX constants starting with 
**      the supplied constant.
**
** Inputs:
**      base                            Base address of constants.
**      koff                            Offset to first constant in list.
**
** Outputs:
**      none
**
**	Returns:
**	    VOID
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      16-jul-86 (thurston)
**          Initial creation.
**	13-may-87 (thurston)
**	    Changed all end of offset list terminaters to be ADE_END_OFFSET_LIST
**	    instead of zero, since zero is a legal instruction offset for the
**	    FE's.
*/

static VOID
ade_klist_print(
PTR                base,
i4            koff)
{
    ADE_CONSTANT        *konst;
    DB_DATA_VALUE       dv;

    while (koff != ADE_END_OFFSET_LIST)
    {
	konst = (ADE_CONSTANT *) ((char *)base + koff);

	adf_printf("\n\n");
	adf_printf("\t\t\t\t+---------------+\n");
	adf_printf("\t\t\t\t|  CX CONSTANT  |\n");
	adf_printf("\t\t\t\t+---------------+\n");
	adf_printf("\t\t\t\t  offset = %d\n\n\n", koff);
	dv.db_datatype = konst->con_dt;
	dv.db_length = konst->con_len;
	dv.db_prec = konst->con_prec;
	dv.db_data = (PTR) ((char *)konst + sizeof(ADE_CONSTANT));
	adf_printf("data");
	ade_prdv(&dv);
	adf_printf("\ndata offset will be = %d\t\t  next const offset = %d\n",
		    koff + sizeof(ADE_CONSTANT),
		    konst->con_offset_next);
	adf_printf("\n-------------------------------------------------------------------------------\n");

	koff = konst->con_offset_next;
    }
}


/*
** Name: ade_ilist_print() - Print a list of CX instructions.
**
** Description:
**      Routine to format and print the list of CX instructions starting with 
**      the supplied instruction.
**
** Inputs:
**	cxbases				Array of base addresses.  This is only
**					needed for the FRONTEND version
**					since they will need to look up the
**					corresponding DB_DATA_VALUE in their
**					DB_DATA_VALUEs array pointed to by
**					cxbases[ADE_DVBASE].
**      base                            Base address of instructions.
**      ioff                            Offset to first instruction in list.
**	loff				Offset to last instruction in list.
**
** Outputs:
**      none
**
**	Returns:
**	    VOID
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      16-jul-86 (thurston)
**          Initial creation.
**	06-may-87 (thurston & wong)
**	    Added `loff' argument.
**	13-may-87 (thurston)
**	    Changed all end of offset list terminaters to be ADE_END_OFFSET_LIST
**	    instead of zero, since zero is a legal instruction offset for the
**	    FE's.
**	07-jul-87 (thurston)
**	    Get the next instruction offset through use of the proper MACRO.
**	13-Jan-2005 (jenjo02)
**	    Change ADE_GET_OPERAND_MACRO to return pointer to 
**	    ADE_OPERAND rather than copying it to the stack.
*/

static VOID
/*ARGSUSED*/
ade_ilist_print(
PTR		   cxbases[],
PTR                base,
i4            ioff,
i4            loff)
{
    ADE_INSTRUCTION     *instr;
    ADE_I_OPERAND       *opint;
    ADE_OPERAND         op;
    ADE_OPERAND         *opP;
    i4                  onum;
    i4		noff;

    while (ioff != ADE_END_OFFSET_LIST)
    {
	instr = (ADE_INSTRUCTION *) ((char *)base + ioff);
	noff = ADE_GET_OFFSET_MACRO(instr, noff, loff);

	adf_printf("\n\n");
	adf_printf("\t\t\t\t+------------------+\n");
	adf_printf("\t\t\t\t|  CX INSTRUCTION  |\n");
	adf_printf("\t\t\t\t+------------------+\n");
	adf_printf("\t\t\t\t   offset = %d\n\n\n", ioff);
        adf_printf("instr code = %d\t\t  # oprs = %d",
		    instr->ins_icode,
		    instr->ins_nops);
        adf_printf("\t  next instr offset = %d\n\n", noff);

	opint = (ADE_I_OPERAND *) ((char *)instr + sizeof(ADE_INSTRUCTION));
	for (onum = 0; onum < instr->ins_nops; onum++)
	{
	    adf_printf("Operand #%2d:\n", onum);
	    ADE_GET_OPERAND_MACRO(cxbases, opint, &op, &opP);
	    adf_printf("\tdatatype = %d\t  length = %d",
		    opP->opr_dt,
		    opP->opr_len);
	    adf_printf("\t  (base, offset) = (%d,%d)\n",
		    opP->opr_base,
		    (i4)opP->opr_offset);
	    opint++;
	}
	adf_printf("\n-------------------------------------------------------------------------------\n");

	ioff = noff;
    }
}

/*
** Name: ade_kbrief_print() - Same as ade_klist_print() only smaller output.
**
** Description:
**      Routine to format and print the list of CX constants starting with 
**      the supplied constant. This does the same thing as ade_klist_print()
**	only the output is smaller to support ade_cxbrief_print().
**
** Inputs:
**      base                            Base address of constants.
**      koff                            Offset to first constant in list.
**
** Outputs:
**      none
**
**	Returns:
**	    VOID
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      6-dec-86 (eric)
**          Initial creation, stolen from ade_klist_print() mostly.
**	13-may-87 (thurston)
**	    Changed all end of offset list terminaters to be ADE_END_OFFSET_LIST
**	    instead of zero, since zero is a legal instruction offset for the
**	    FE's.
**      17-mar-1993 (stevet)
**          Added precision for decimal.
**	28-sep-05 (inkdo01)
**	    Display hex offset of constant, too.
*/
static VOID
ade_kbrief_print(
PTR                base,
i4            koff)
{
    ADE_CONSTANT        *konst;
    DB_DATA_VALUE       dv;
    i4			baseno = 0;

    while (koff != ADE_END_OFFSET_LIST)
    {
	konst = (ADE_CONSTANT *) ((char *)base + koff);
	dv.db_datatype = konst->con_dt;
	dv.db_length = konst->con_len;
	dv.db_prec = konst->con_prec;
	dv.db_collID = konst->con_collID;
	dv.db_data = (PTR) ((char *)konst + sizeof(ADE_CONSTANT));

	adf_printf("%x: ", koff);	/* const offset */
	adf_printf("%d[%d]{", baseno, koff + sizeof (ADE_CONSTANT));
	ade_prtype(&dv);
	ade_prvalue(&dv);
	adf_printf("\n");

	koff = konst->con_offset_next;
    }
}


/*
** Name: ade_ibrief_print() - Same as ade_ilist_print() only smaller output.
**
** Description:
**      Routine to format and print the list of CX instructions starting with 
**      the supplied instructions. This does the same thing as ade_ilist_print()
**	only the output is smaller to support ade_cxbrief_print().
**
** Inputs:
**	cxbases				Array of base addresses.  This is only
**					needed for the FRONTEND version
**					since they will need to look up the
**					corresponding DB_DATA_VALUE in their
**					DB_DATA_VALUEs array pointed to by
**					cxbases[ADE_DVBASE].
**      base                            Base address of instructions.
**      ioff                            Offset to first instruction in list.
**      loff                            Offset to last instruction in list.
**
** Outputs:
**      none
**
**	Returns:
**	    VOID
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      8-dec-86 (eric)
**          initial creation, taken mostly from ade_ilist_print();
**	06-may-87 (thurston & wong)
**	    Added `loff' argument.
**	13-may-87 (thurston)
**	    Changed all end of offset list terminaters to be ADE_END_OFFSET_LIST
**	    instead of zero, since zero is a legal instruction offset for the
**	    FE's.
**	13-Jan-2005 (jenjo02)
**	    Change ADE_GET_OPERAND_MACRO to return pointer to 
**	    ADE_OPERAND rather than copying it to the stack.
**	25-Mar-2005 (schka24)
**	    Suppress data type for mecopy's, not useful information.
*/

static VOID
/*ARGSUSED*/
ade_ibrief_print(
PTR		   cxbases[],
PTR                base,
i4            ioff,
i4            loff)
{
    ADE_INSTRUCTION     *instr;
    ADE_I_OPERAND       *opint;
    ADE_OPERAND         op;
    ADE_OPERAND         *opP;
    i4                  onum;
    DB_DATA_VALUE	dv;
    ADI_FI_ID		fiid;

    while (ioff != ADE_END_OFFSET_LIST)
    {
	instr = (ADE_INSTRUCTION *) ((char *)base + ioff);

	fiid = instr->ins_icode;
	adf_printf("%x: ", ioff);	/* instr offset */
	ade_prinstr(&fiid);
	adf_printf("(");

	dv.db_data = NULL;
	opint = (ADE_I_OPERAND *) ((char *)instr + sizeof(ADE_INSTRUCTION));
	for (onum = 0; onum < instr->ins_nops; onum++)
	{
	    if (onum == 3)
		adf_printf("\n\t\t");

	    ADE_GET_OPERAND_MACRO(cxbases, opint, &op, &opP);
	    if (opP->opr_base == ADE_LABBASE)  /* is this a label operand? */
		adf_printf("%d[%x]", opP->opr_base, (i4)opP->opr_offset);
	    else if (instr->ins_icode == ADE_MECOPY || instr->ins_icode == ADE_MECOPYN
		  || instr->ins_icode == ADE_MECOPYQA)
	    {
		adf_printf("%d[%d](%d)", opP->opr_base, (i4)opP->opr_offset,
				opP->opr_len);
	    }
	    else
	    {
		dv.db_datatype = opP->opr_dt;
		dv.db_length = opP->opr_len;
		dv.db_prec = opP->opr_prec;
		dv.db_collID = opP->opr_collID;

		adf_printf("%d[%d]{", opP->opr_base, (i4)opP->opr_offset);
		ade_prtype(&dv);
	    }

	    if (onum < (instr->ins_nops - 1))
		adf_printf(", ");

	    opint++;
	}
	adf_printf(");\n");

	ioff = ADE_GET_OFFSET_MACRO(instr, ioff, loff);
    }
}

/*
[@function_definition@]...
*/


/*
** Name: adf_printf() - Perform a printf through SCC_TRACE
**
** Description:
**      This routine performs the printf function by returning the info to 
**      be displayed through SCC_TRACE. 
[@comment_line@]...
**
** Inputs:
**	str -
**	    The control string
**	p1 - p8
**	    parameters 1 to 8 for the control string
**
** Outputs:
**  none
**
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      17-nov-87 (eric)
**          written
**	21-may-89 (jrb)
**	    renamed scf_enumber to scf_local_error
**      25-sep-92 (stevet)
**          Use FEXI function table to print adf trace message and 
**          remove reference to scf.h file.
[@history_template@]...
*/
static VOID
adf_printf( str, p1, p2, p3, p4, p5, p6, p7, p8)
char	    *str;
char	    *p1;
char	    *p2;
char	    *p3;
char	    *p4;
char	    *p5;
char	    *p6;
char	    *p7;
char	    *p8;
{
    char	cbuf[500];
#ifdef    ADE_BACKEND
    DB_STATUS   db_stat=E_DB_ERROR;
    DB_STATUS   (*func)();
#endif /* ADE_BACKEND */

#ifdef    ADE_FRONTEND
    SIprintf(cbuf, str, p1, p2, p3, p4, p5, p6, p7, p8);
#endif /* ADE_FRONTEND */

#ifdef    ADE_BACKEND
    STprintf(cbuf, str, p1, p2, p3, p4, p5, p6, p7, p8);

    if ((func = Adf_globs->Adi_fexi[ADI_02ADF_PRINTF_FEXI].adi_fcn_fexi) 
	!= NULL)
	db_stat = (*func)(cbuf);

    if(db_stat != E_DB_OK)
    {
	TRdisplay("SCF error displaying an ADF message to user\n");
	TRdisplay("The ADF message is :%s",cbuf);
    }
#endif /* ADE_BACKEND */
}

/*
** Name: ade_prdv() - Print a DB_DATA_VALUE struct for debugging.
**
** Description:
**        This routine formats and prints a DB_DATA_VALUE for debugging.
**
** Inputs:
**      db_dv                           pointer to the DB_DATA_VALUE to be
**					formated and printed.
**
** Outputs:
**	none
**
**	Returns:
**	    none
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	23-apr-86 (ericj)
**          Initially written.
**	26-sep-86 (thurston)
**	    Made to work for char, varchar, and longtext.
**	31-oct-86 (thurston)
**	    Made a little more robust when handed bad data.
**	18-nov-86 (thurston)
**	    Made the display of datatype names more readable.
**      8-dec-86 (eric)
**          changed to use adu_prtype(), and adu_prvalue().
**	24-feb-87 (thurston)
**	    Added support for nullable datatypes.
**	26-feb-88 (eric)
**	    Taken from aduprint.c and renamed to ade_*() because of
**	    FE stuff.
**	14-jul-89 (jrb)
**	    Added decimal support.
**	17-jul-2006 (gupsh01)
**	    Added ANSI datetime support.
*/

static VOID
ade_prdv(
DB_DATA_VALUE      *db_dv)
{
    DB_DT_ID	dt;
    DB_DT_ID	bdt;
    i4		prec;
    i4		len;
    i4		blen;
    PTR		data;

    if (db_dv == NULL)
    {
	adf_printf("< Pointer to DB_DATA_VALUE is NULL >\n");
	return;
    }

    dt   = bdt  = db_dv->db_datatype;
    prec	= db_dv->db_prec;
    len  = blen = db_dv->db_length;
    data        = db_dv->db_data;
    if (dt < 0)
    {
	bdt = -dt;
	blen--;
    }

    adf_printf("type: %d (", (i4) dt);
    ade_prtype(db_dv);

    if (data == NULL)
    {
	adf_printf("< pointer to data is NULL >\n");
    }
    else if (ADI_ISNULL_MACRO(db_dv))
    {
	adf_printf("value: < NULL >\n");
    }
    else
    {
	switch (bdt)
	{
	  case DB_CHA_TYPE:
	  case DB_CHR_TYPE:
	  case DB_FLT_TYPE:
	  case DB_DEC_TYPE:
	  case DB_INT_TYPE:
	  case DB_MNY_TYPE:
	  case DB_BOO_TYPE:
	    adf_printf("value: ");
	    break;

	  case DB_VCH_TYPE:
	    adf_printf("varchar count: %d\n", 
				((DB_TEXT_STRING *) data)->db_t_count);
	    break;

	  case DB_TXT_TYPE:
	    adf_printf("text count: %d\n",
				((DB_TEXT_STRING *) data)->db_t_count);
	    break;

	  case DB_LTXT_TYPE:
	    adf_printf("longtext count: %d\n", 
				((DB_TEXT_STRING *) data)->db_t_count);
	    break;

	  case DB_DTE_TYPE:
	  case DB_ADTE_TYPE:
	  case DB_TMWO_TYPE:
	  case DB_TMW_TYPE:
	  case DB_TME_TYPE:
	  case DB_TSWO_TYPE:
	  case DB_TSW_TYPE:
	  case DB_TSTMP_TYPE:
	  case DB_INYM_TYPE:
	  case DB_INDS_TYPE:
	  default:
	    break;
	}

	ade_prvalue(db_dv);
	adf_printf("\n");
    }

    return;
}

/*
** Name: ade_prtype() - Print the data type of a DB_DATA_VALUE for debugging.
**
** Description:
**        This routine formats and prints the type of a DB_DATA_VALUE 
**	for debugging.
**
** Inputs:
**      db_dv                           pointer to the DB_DATA_VALUE to be
**					formated and printed.
**
** Outputs:
**	none
**
**	Returns:
**	    none
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      8-dec-86 (eric)
**          created, from adu_prdv mostly.
**	24-feb-87 (thurston)
**	    Added support for nullable datatypes.
**	26-feb-88 (eric)
**	    Taken from aduprint.c and renamed to ade_*() because of
**	    FE stuff.
**	16-dec-04 (inkdo01)
**	    Add collation ID to string type displays.
*/

static VOID
ade_prtype(
DB_DATA_VALUE      *db_dv)
{
    DB_DT_ID		dt  = db_dv->db_datatype;
    DB_DT_ID		bdt = abs(dt);

    if (bdt < ADE_PDTSMALL || bdt > ADE_PDTBIG || type_name[bdt] == NULL) 
    {
	adf_printf("<dt %d ln %d>", (i4) dt, (i4)db_dv->db_length);
    }
    else
    {
	if (dt < 0)
	    adf_printf("Nullable ");
	if (bdt == DB_DEC_TYPE)
	    adf_printf("DB_DEC_TYPE(%d,%d,%d)} ", 
		       (i4)DB_P_DECODE_MACRO(db_dv->db_prec),
		       (i4)DB_S_DECODE_MACRO(db_dv->db_prec),
		       db_dv->db_length);
	else if (db_dv->db_collID >= 0 && (bdt == DB_CHA_TYPE ||
	    bdt == DB_CHR_TYPE || bdt == DB_VCH_TYPE ||
	    bdt == DB_TXT_TYPE || bdt == DB_NCHR_TYPE ||
	    bdt == DB_NVCHR_TYPE))
	    adf_printf("%s(%d, %d)} ", 
		       type_name[bdt],
		       db_dv->db_length,
		       db_dv->db_collID);
	else if (bdt == DB_TME_TYPE || bdt == DB_TMW_TYPE ||
	    bdt == DB_TMWO_TYPE || bdt == DB_TSW_TYPE ||
	    bdt == DB_TSWO_TYPE || bdt == DB_TSTMP_TYPE)
	    adf_printf("%s(%d, %d)} ", 
		       type_name[bdt],
		       db_dv->db_length,
		       db_dv->db_prec);
	else
	    adf_printf("%s(%d)} ", 
		       type_name[bdt],
		       db_dv->db_length);
    }
    return;
}


/*
** Name: ade_prvalue() - Print out just the data value of a DB_DATA_VALUE
**
** Description:
**        This routine formats and prints just the data value of a 
**	DB_DATA_VALUE for debugging. No extra blanks, tabs, new lines, etc.
**	will be added, except for a leading and trailing ' as a delimiter as
**	needed.
**
** Inputs:
**      db_dv                           pointer to the DB_DATA_VALUE to be
**					formated and printed.
**
** Outputs:
**	none
**
**	Returns:
**	    none
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      8-dec-86 (eric)
**          created, from adu_prdv mostly.
**	24-feb-87 (thurston)
**	    Added support for nullable datatypes.
**	12-mar-87 (thurston)
**	    Added support for `bit fields'.  These are currently only used
**	    by ADF to reference the null byte on `nullable pre-instructions'.
**	4-aug-87 (daved)
**	    make this routine use adu_2prvalue.
**	26-feb-88 (eric)
**	    Taken from aduprint.c and renamed to ade_*() because of
**	    FE stuff.
*/

static VOID
ade_prvalue(
DB_DATA_VALUE      *db_dv)
{
    ade_2prvalue(adf_printf, db_dv);
    return;
}


/*
** Name: ade_2prvalue() - Print out just the data value of a DB_DATA_VALUE
**
** Description:
**        This routine formats and prints just the data value of a 
**	DB_DATA_VALUE for debugging. No extra blanks, tabs, new lines, etc.
**	will be added, except for a leading and trailing ' as a delimiter as
**	needed.
**
** Inputs:
**	fcn				function to print strings
**      db_dv                           pointer to the DB_DATA_VALUE to be
**					formated and printed.
**
** Outputs:
**	none
**
**	Returns:
**	    none
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	4-aug-87 (daved)
**	    written
**	26-feb-88 (eric)
**	    Taken from aduprint.c and renamed to ade_*() because of
**	    FE stuff.
**	14-jul-89 (jrb)
**	    Added decimal support.
**	09-mar-92 (stec, merged by stevet)
**	    Improved printout of text constants. Fixed the bug that
**	    caused garbage to be printed out when data was not null
**	    terminated.
**	22-jun-93 (robf)
**	    Added cases for DB_SEC_TYPE and DB_SECID_TYPE
**	25-nov-02 (inkdo01)
**	    Added Unicode support (NCHR/NVCHR).
*/

static VOID
ade_2prvalue(
VOID	           (*fcn)(),
DB_DATA_VALUE      *db_dv)
{
    char		num_buf[64];
    i2			flt_wid;
    f8			f8_tmp;
    AD_DATENTRNL	*datep;
    AD_NEWDTNTRNL	newdt;
    AD_NEWDTNTRNL	*ndatep;
    DB_DT_ID		bdt;
    i4			blen;
    PTR			data;
    i4			prec;
    i4			res_wid;

    if (db_dv == NULL)
    {
	(*fcn)("< Pointer to DB_DATA_VALUE is NULL >");
	return;
    }

    bdt  = abs(db_dv->db_datatype);
    prec = db_dv->db_prec;
    blen = db_dv->db_length - ((db_dv->db_datatype < 0) ? 1 : 0);
    data = db_dv->db_data;

    if (data == NULL)
    {
	(*fcn)("< pointer to data is NULL >\n");
    }
    else if (ADI_ISNULL_MACRO(db_dv))
    {
	(*fcn)("< NULL >\n");
    }
    else
    {
	switch (bdt)
	{
	  case DB_CHA_TYPE:
	  case DB_CHR_TYPE:
	    (*fcn)("'%24s'", (u_char *) data);
	    break;

	  case DB_NCHR_TYPE:
	  case DB_NVCHR_TYPE:
	    (*fcn)("'%8x'", *((i4 *)data));
	    break;

	  case DB_DTE_TYPE:
	    datep = (AD_DATENTRNL *) data;
	    (*fcn)("dn_status = %x\tdn_highday = %d\tdn_time = %d\n",
		      datep->dn_status, datep->dn_highday, datep->dn_time);
	    (*fcn)("dn_year = %d\tdn_month = %d\tdn_lowday = %d",
		      datep->dn_year, datep->dn_month, datep->dn_lowday);
	    break;

	  case DB_ADTE_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)("dn_year = %d\tdn_month = %d\tdn_day = %d\n",
		      ndatep->dn_year, ndatep->dn_month, ndatep->dn_day);
	    break;

	  case DB_TMWO_TYPE:
	  case DB_TMW_TYPE:
	  case DB_TME_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)("dn_time = %d\tdn_nsecond = %d\n",
		      ndatep->dn_time, ndatep->dn_nsecond );
	    (*fcn)("dn_tzhour\tdn_tzminute = %d\n",
		      ndatep->dn_tzhour, ndatep->dn_tzminute);
	    break;

	  case DB_INYM_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)("dn_year = %d\tdn_month = %d\n",
		      ndatep->dn_year, ndatep->dn_month);
	    break;

	  case DB_INDS_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)("dn_day = %d\tdn_time = %d\tdn_nsecond = %d\n",
		      ndatep->dn_day, ndatep->dn_time, ndatep->dn_nsecond);
	    break;

	  case DB_TSWO_TYPE:
	  case DB_TSW_TYPE:
	  case DB_TSTMP_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)("dn_year = %d\tdn_month = %d\tdn_day = %d\n",
		      ndatep->dn_year, ndatep->dn_month, ndatep->dn_day);
	    (*fcn)("dn_time = %d\tdn_nsecond = %d\n",
		      ndatep->dn_time, ndatep->dn_nsecond );
	    (*fcn)("dn_tzhour\tdn_tzminute = %d\n",
		      ndatep->dn_tzhour, ndatep->dn_tzminute);
	    break;

	  case DB_FLT_TYPE:
	    if (blen == 4)
		f8_tmp = *(f4 *) data;
	    else
		f8_tmp = *(f8 *) data;
	    CVfa(f8_tmp, (i4) 32, (i4) 4, 'f', '.', num_buf, &flt_wid);
	    (*fcn)("%10s", num_buf);
	    break;

	  case DB_DEC_TYPE:
	    CVpka(data,
		    (i4)DB_P_DECODE_MACRO(prec),
		    (i4)DB_S_DECODE_MACRO(prec),
		    '.',
		    (i4)DB_P_DECODE_MACRO(prec) + 5,
		    (i4)DB_S_DECODE_MACRO(prec),
		    CV_PKLEFTJUST,
		    num_buf,
		    &res_wid);
		    
	    (*fcn)("%10s", num_buf);
	    break;

	  case DB_INT_TYPE:
	    if (blen == 8)
#ifdef LP64
		(*fcn)("%ld", *(i8 *) data);
#else
		(*fcn)("%lld", *(i8 *) data);
#endif
	    else if (blen == 4)
		(*fcn)("%d", *(i4 *) data);
	    else if (blen == 2)
		(*fcn)("%d", *(i2 *) data);
	    else
		(*fcn)("%d", *(i1 *) data);
	    break;

	  case DB_MNY_TYPE:
	    f8_tmp = ((AD_MONEYNTRNL *) data)->mny_cents / 100.0;
	    CVfa(f8_tmp, 32, 2, 'f', '.', num_buf, &flt_wid);
	    (*fcn)("$ %10s", num_buf);
	    break;

	  case DB_VCH_TYPE:
	  case DB_TXT_TYPE:
	  case DB_LTXT_TYPE:
	    {
		char	fmt[10];
		i4	len;

		len = ((DB_TEXT_STRING *) data)->db_t_count;

		len = (len > 24) ? 24 : len;
		(VOID)STprintf(fmt, "'%%.%ds'", len);

		(*fcn)(fmt,
  			((DB_TEXT_STRING *) data)->db_t_text);

	    }
	    break;

	  case DB_BOO_TYPE:
	    if (*((bool *) data))
		(*fcn)("TRUE");
	    else
		(*fcn)("FALSE");
	    break;

	  default:
	    (*fcn)("< unknown type: don't know how to interpret data >");
	    break;
	}
    }

    return;
}


/*
** Name: ade_prinstr()	- Print out the name of an instruction
**
** Description:
**      This routine takes a ADE_INSTRUCTION and prints out the name of the
**      instruction for debugging. For an example, see ade_ibrief_print() 
**      in adedebug.c
**
** Inputs:
**      instr                           The instruction to print.
**
** Outputs:
**	none.
**
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    Trace output is produced.
**
** History:
**      8-dec-86 (eric)
**          initially created.
**	12-mar-87 (thurston)
**	    Added code to print out new CX special instructions that deal with
**	    nullable datatype.
**	10-feb-88 (thurston)
**	    Added code to display several of the newer (now old) CX special
**	    instruction names.
**	26-feb-88 (eric)
**	    Taken from aduprint.c and renamed to ade_*() because of
**	    FE stuff.
**	08-sep-88 (thurston)
**	    Added code to detect `coercions' and `copy_coercions' and print that
**	    out instead of `< unknown operator >'.
**      16-may-89 (eric)
**          added support for the new instruction ADE_TMCVT.
**      02-feb-1991 (jennifer)
**          Added support for ADE_ESC_CHAR for bug B34259.
**	3-jul-96 (inkdo01)
**	    Added support for ADE_4CXI_CMP_SET_SKIP.
**	26-mar-97 (inkdo01)
**	    Added new abelled AND/OR ops.
**	8-sep-99 (inkdo01)
**	    Added BRANCH & BRANCHF ops.
**	24-nov-00 (inkdo01)
**	    Added MECOPYN and 5CXI_CLR_SKIP.
**	15-dec-00 (inkdo01)
**	    Added ADE_AVG for inline aggregates.
**	3-jan-01 (inkdo01)
**	    Added ADE_NULLCHECK, ADE_MECOPYQA, ADE_AVGQ for inline aggregates.
**	9-apr-01 (inkdo01)
**	    Added ADE_UNORM to normalize Unicode input values.
**	30-aug-01 (inkdo01)
**	    Re-add AGBGN, NAGEND and add OLAGBGN, OLAGEND and AGNEXT.
**	5-nov-04 (inkdo01)
**	    Add ADE_NOOP (for no-op'ed instructions).
**	28-Mar-2005 (schka24)
**	    Added 6CXI, 7CXI instructions.  This thing cries out for a
**	    lookup table.
**	25-Jul-2005 (schka24)
**	    Added SETTRUE.
*/

static VOID
ade_prinstr(
ADI_FI_ID	    *instr)
{
    ADI_OPRATION        *op;
    ADI_OP_ID           adi_oid;
    ADI_FI_DESC         *fi;
    ADI_FI_ID		adi_fid = *instr;

    if (adi_fid < 0)
    {
	/* print out the special CX instruction. */
	switch (adi_fid)
	{
	 case ADE_AND:
	    adf_printf("ADE_AND");
	    break;

	 case ADE_OR:
	    adf_printf("ADE_OR");
	    break;

	 case ADE_ANDL:
	    adf_printf("ADE_ANDL");
	    break;

	 case ADE_ORL:
	    adf_printf("ADE_ORL");
	    break;

	 case ADE_BRANCH:
	    adf_printf("ADE_BRANCH");
	    break;

	 case ADE_BRANCHF:
	    adf_printf("ADE_BRANCHF");
	    break;

	 case ADE_NOT:
	    adf_printf("ADE_NOT");
	    break;

	 case ADE_MECOPY:
	    adf_printf("ADE_MECOPY");
	    break;

	 case ADE_MECOPYN:
	    adf_printf("ADE_MECOPYN");
	    break;

	 case ADE_MECOPYQA:
	    adf_printf("ADE_MECOPYQA");
	    break;

	 case ADE_AVG:
	    adf_printf("ADE_AVG");
	    break;

	 case ADE_AVGQ:
	    adf_printf("ADE_AVGQ");
	    break;

	 case ADE_NULLCHECK:
	    adf_printf("ADE_NULLCHECK");
	    break;

	 case ADE_OLAP:
	    adf_printf("ADE_OLAP");
	    break;

	 case ADE_UNORM:
	    adf_printf("ADE_UNORM");
	    break;

	 case ADE_AGBGN:
	    adf_printf("ADE_AGBGN");
	    break;

	 case ADE_AGNEXT:
	    adf_printf("ADE_AGNEXT");
	    break;

	 case ADE_NAGEND:
	    adf_printf("ADE_NAGEND");
	    break;

	 case ADE_NOOP:
	    adf_printf("ADE_NOOP");
	    break;

	 case ADE_OLAGBGN:
	    adf_printf("ADE_OLAGBGN");
	    break;

	 case ADE_OLAGEND:
	    adf_printf("ADE_OLAGEND");
	    break;

	 case ADE_KEYBLD:
	    adf_printf("ADE_KEYBLD");
	    break;

	 case ADE_CALCLEN:
	    adf_printf("ADE_CALCLEN");
	    break;

	 case ADE_COMPARE:
	    adf_printf("ADE_COMPARE");
	    break;

	 case ADE_PMQUEL:
	    adf_printf("ADE_PMQUEL");
	    break;

	 case ADE_NO_PMQUEL:
	    adf_printf("ADE_NO_PMQUEL");
	    break;

	 case ADE_PMFLIPFLOP:
	    adf_printf("ADE_PMFLIPFLOP");
	    break;

	 case ADE_PMENCODE:
	    adf_printf("ADE_PMENCODE");
	    break;

	 case ADE_BYCOMPARE:
	    adf_printf("ADE_BYCOMPARE");
	    break;

	 case ADE_PNAME:
            adf_printf("ADE_PNAME");
            break;

         case ADE_DBDV:
            adf_printf("ADE_DBDV");
            break;

	 case ADE_0CXI_IGNORE:
	    adf_printf("ADE_0CXI_IGNORE");
	    break;

	 case ADE_1CXI_SET_SKIP:
	    adf_printf("ADE_1CXI_SET_SKIP");
	    break;

	 case ADE_2CXI_SKIP:
	    adf_printf("ADE_2CXI_SKIP");
	    break;

	 case ADE_3CXI_CMP_SET_SKIP:
	    adf_printf("ADE_3CXI_CMP_SET_SKIP");
	    break;

	 case ADE_4CXI_CMP_SET_SKIP:
	    adf_printf("ADE_4CXI_CMP_SET_SKIP");
	    break;

	 case ADE_5CXI_CLR_SKIP:
	    adf_printf("ADE_5CXI_CLR_SKIP");
	    break;

	 case ADE_6CXI_CLR_OP0NULL:
	    adf_printf("ADE_6CXI_CLR_OP0NULL");
	    break;

	 case ADE_7CXI_CKONLY:
	    adf_printf("ADE_7CXI_CKONLY");
	    break;

         case ADE_ESC_CHAR:
            adf_printf("ADE_ESC_CHAR");
            break;

	 case ADE_TMCVT:
	    adf_printf("ADE_TMCVT");
	    break;

	 case ADE_REDEEM:
	    adf_printf("ADE_REDEEM");
	    break;

	 case ADE_SETTRUE:
	    adf_printf("ADE_SETTRUE");
	    break;

	  default:
	    adf_printf("< unknown CX instr %d >", (i4)adi_fid);
	    break;
	}
	return;
    }

    /* get the fi_desc for this fiid */
    fi = (ADI_FI_DESC *) NULL;
    if (adi_fid >= (ADI_FI_ID) 0 && adi_fid <= Adf_globs->Adi_fi_biggest)
    {
	fi = (ADI_FI_DESC *)
	    Adf_globs->Adi_fi_lkup[(i4) (ADI_LK_MAP_MACRO(adi_fid))].adi_fi;
    }

    if (fi == NULL)
    {
	adf_printf("< unknown fiid %d >", (i4) adi_fid);
	return;
    }

    /* is this fi a COERCION or a COPY_COERCION?  If so, no op ID. */
    if (fi->adi_fitype == ADI_COERCION)
    {
	adf_printf("< coercion >");
	return;
    }

    if (fi->adi_fitype == ADI_COPY_COERCION)
    {
	adf_printf("< copy coercion >");
	return;
    }
    
    /* get the op id from the fi_desc */
    adi_oid = fi->adi_fiopid;

    /* get the opname from the opid */
    for (op = &Adf_globs->Adi_operations[0]; op->adi_opid != ADI_NOOP; op++)
    {
	if (op->adi_opid == adi_oid)
	{
	    adf_printf(op->adi_opname.adi_opname);
	    return;
	}
    }

    adf_printf("< unknown operator %d>", adi_oid);
    return;
}
