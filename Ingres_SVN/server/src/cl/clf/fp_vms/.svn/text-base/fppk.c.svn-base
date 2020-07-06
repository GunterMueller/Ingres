/*
**    Copyright (c) 1988, 2000 Ingres Corporation
*/
 
#include    <compat.h>
#include    <gl.h>
#include    <me.h>
#include    <fp.h>
 
/**
**
**  Name: FPPK.C - FP routines for packed decimal.
**
**  Description:
**      This file contains all of the FP routines necessary to perform 
**      mathematical operations on packed decimal values.
**
**      This file defines the following routines:
**
**          FPpkint()	  - Take the integer part of a packed decimal.
**          FPpkceil()    - Take the ceiling of a packed decimal.
**          FPpkabs()     - Take the absolute value of a packed decimal.
**          FPpkneg()     - Negate a packed decimal.
**
**          FPpkadd_sim() - Add two packed decimals via software (this
**			    routine is executed only via calls from FPpkadd;
**			    it is not an external CL interface)
**          FPpksub_sim() - Subtract two packed decimals via software (this
**			    routine is executed only via calls from FPpksub;
**			    it is not an external CL interface)
**          FPpkmul_sim() - Multiply two packed decimals via software (this
**			    routine is executed only via calls from FPpkmul;
**			    it is not an external CL interface)
**          FPpkdiv()     - Divide two packed decimals.
**
**
**  History:
**      03-may-88 (thurston)
**          Initial creation.
**	03-sep-89 (jrb)
**	    Changed name of many of the functions in this file because the main
**	    implementation is now coded in MACRO.
**	13-jan-1993 (rog)
**	    Created fppk.c from mhpk.c.
**      11-aug-93 (ed)
**          added missing includes
**	19-jul-2000 (kinte01)
**	   Add missing forward function references
**	01-dec-2000	(kinte01)
**	    Bug 103393 - removed nat, longnat, u_nat, & u_longnat
**	    from VMS CL as the use is no longer allowed
**	14-feb-2007 (dougi)
**	    Replaced hard constant 31 with CL_MAX_DECPREC.
*/

STATUS FPpkadd_sim(); 
STATUS FPpksub_sim(); 


/* Global definitions so we can pass #define'd values to fpdec.mar.  */
globalvalue	IIFP_DECOVF = FP_DECOVF;
globalvalue	IIFP_DECDIV = FP_DECDIV;


/*{
** Name: FPpkint() - Take the integer part of a packed decimal.
**
** Description:
**      Takes the integer part of a packed decimal and returns it as another
**      packed decimal with the same precision and scale.
**
** Inputs:
**      pkin                            Pointer to the input decimal value.
**      prec                            Its precision:  1 <= prec <= 39.
**      scale                           Its scale:  0 <= scale <= prec.
**
** Outputs:
**      pkout                           The resulting packed decimal value is
**                                      placed here.  It will have the same
**                                      precision and scale as the input.
**
**      Returns:
**          VOID
**
**      Exceptions:
**          none
**
** Side Effects:
**          none
**
** History:
**      16-may-88 (jrb)
**          Initial creation and coding.
**      04-may-1993 (rog)
**	    Converted to FPpkint().
*/
 
VOID
FPpkint( PTR pkin, i4 prec, i4 scale, PTR pkout )
{
    u_char      *po;            /* pointer to output */
    i4		sc = scale;	/* scale counter */
 
    MEcopy(pkin, (u_i2)(prec/2 + 1), pkout);
            
    /* make po point to byte containing leftmost digit of fractional part */
    po = (u_char *)pkout + (prec / 2) - (scale / 2);
 
    /* now zero out all nibbles in fractional part */
    while (sc > 0)
        if (sc-- & 1)
            *po &= (u_char)0xf;
        else
            *po++ &= (u_char)0xf0;
}
 

/*{
** Name: FPpkceil() - Take the ceiling of a packed decimal.
**
** Description:
**      Takes the ceiling of a packed decimal and returns it as another
**      packed decimal with the same precision and scale.  The ceiling is
**      the smallest integer not less than the input.
**
** Inputs:
**      pkin                            Pointer to the input decimal value.
**      prec                            Its precision:  1 <= prec <= 39.
**      scale                           Its scale:  0 <= scale <= prec.
**
** Outputs:
**      pkout                           The resulting packed decimal value is
**                                      placed here.  It will have the same
**                                      precision and scale as the input.
**
**      Returns:
**          OK				The operation succeeded.
**
**          FP_DECOVF                   Result has too many non-fractional
**                                      digits; i.e. more than (prec - scale)
**                                      of them.
**
**      Exceptions:
**	    None
**
** Side Effects:
**          none
**
** History:
**      20-may-88 (jrb)
**          Initial creation and coding.
**      04-may-1993 (rog)
**	    Converted to FPpkceil().
*/
 
STATUS
FPpkceil( PTR pkin, i4 prec, i4 scale, PTR pkout )
{
    u_char      *spo;                   /* walks thru scale digits */
    u_char      *ppo;                   /* for adding one to pkout */
    u_char      record = (u_char)0;     /* for finding any non-zero bits */
    i4         digcnt = scale;
 
    MEcopy(pkin, (u_i2)(prec/2 + 1), pkout);
            
    /* make po's point to byte containing leftmost digit of fractional part */
    spo = ppo = (u_char *)pkout + (prec / 2) - (digcnt / 2);
 
    /* now zero out all nybbles in fractional part */
    while (digcnt > 0)
    {
        if (digcnt-- & 1)
        {
            record |= *ppo & 0xf0;
            *ppo &= (u_char)0xf;
        }
        else
        {
            record |= *ppo & 0xf;
            *ppo++ &= (u_char)0xf0;
        }
    }
 
    /* Now ppo points to last byte of pkout.  If sign is positive and there */
    /* were some non-zero digits, we add 1.  Otherwise we're done.          */
 
    if (((*ppo & 0xf) != FP_PK_MINUS  &&  (*ppo & 0xf) != FP_PK_AMINUS)
	&& record != (u_char)0)
    {
        i4     carry = 1;
 
        /* first set pointers for integral part of pkout */
        if (((digcnt = scale + 1) & 1) == 0)
            spo--;
 
	/* now loop through integral part and add one until carry goes away */
        while (digcnt <= prec  &&  carry--)
            if (digcnt++ & 1)
                if ((*spo & 0xf0) == (u_char)0x90)
                {
                    *spo-- &= (u_char)0xf;      /* zero out high nybble */
                    carry = 1;
                }
                else
                {
                    *spo-- += (u_char)0x10;
                }
            else
                if ((*spo & 0xf) == (u_char)0x9)
                {
                    *spo &= (u_char)0xf0;       /* zero out low nybble */
                    carry = 1;
                }
                else
                {
                    (*spo)++;
                }
 
	/* if there is still a carry then we overflowed */
        if (carry > 0)
	    return(FP_DECOVF);
    }
    return(OK);
}
 

/*{
** Name: FPpkabs() - Take the absolute value of a packed decimal.
**
** Description:
**      Takes the absolute value of a packed decimal and returns it as another
**      packed decimal with the same precision and scale.
**
** Inputs:
**      pkin                            Pointer to the input decimal value.
**      prec                            Its precision:  1 <= prec <= 39.
**      scale                           Its scale:  0 <= scale <= prec.
**
** Outputs:
**      pkout                           The resulting packed decimal value is
**                                      placed here.  It will have the same
**                                      precision and scale as the input, but
**                                      a different sign.
**
**      Returns:
**          VOID
**
**      Exceptions:
**          none
**
** Side Effects:
**          none
**
** History:
**      16-may-88 (jrb)
**          Initial creation and coding.
**	04-may-1993 (rog)
**	    Converted to FPpkabs().
*/
 
VOID
FPpkabs( PTR pkin, i4 prec, i4 scale, PTR pkout )
{
    u_char      *signptr = (u_char *)pkout + prec / 2;

    /* copy the number and make the sign positive */
    MEcopy(pkin, (u_i2)(prec/2 + 1), pkout);
    *signptr = (*signptr & (u_char)0xf0) | FP_PK_PLUS;
}
 

/*{
** Name: FPpkneg() - Negate a packed decimal.
**
** Description:
**      Changes the sign of a packed decimal and returns it as another
**      packed decimal with the same precision and scale.
**
** Inputs:
**      pkin                            Pointer to the input decimal value.
**      prec                            Its precision:  1 <= prec <= 39.
**      scale                           Its scale:  0 <= scale <= prec.
**
** Outputs:
**      pkout                           The resulting packed decimal value is
**                                      placed here.  It will have the same
**                                      precision and scale as the input, but
**                                      a different sign.
**
**      Returns:
**          VOID
**
**      Exceptions:
**          none
**
** Side Effects:
**          none
**
** History:
**      16-may-88 (jrb)
**          Initial creation and coding.
*/
 
VOID
FPpkneg( PTR pkin, i4 prec, i4 scale, PTR pkout )
{
    u_char      *signptr = (u_char *)pkout + prec / 2;
 
    /* make a copy and complement the sign */
    MEcopy(pkin, (u_i2)(prec/2 + 1), pkout);
 
    /* NOTE: This may cause negative 0, but we should always be ready to    */
    /* handle it in all routines which know about the internals of decimal  */
    if ((*signptr & 0xf) == FP_PK_MINUS || (*signptr & 0xf) == FP_PK_AMINUS)
        *signptr = (*signptr & (u_char)0xf0) | FP_PK_PLUS;
    else
        *signptr = (*signptr & (u_char)0xf0) | FP_PK_MINUS;
}
 

/*{
** Name: FPpkadd_sim() - Add two packed decimals.
**
** Description:
**	This routine is called by FPpkadd which is written in MACRO.  The
**	sequence is: try using the VAX instruction to add; if that
**	overflows, use this code; if that overflows then it's really an
**	overflow.
**
** Inputs:
**      pk1                             Pointer to the 1st addend.
**      p1                              Its precision:  1 <= p1 <= 39.
**      s1                              Its scale:  0 <= s1 <= p1.
**      pk2                             Pointer to the 2nd addend.
**      p2                              Its precision:  1 <= p2 <= 39.
**      s2                              Its scale:  0 <= s2 <= p2.
**	pout				Result precision
**	sout				Result scale
**
** Outputs:
**      pkout                           The resulting packed decimal value is
**                                      placed here.
**
**      Returns:
**          OK				The operation succeeded.
**
**          FP_DECOVF                   Sum has too many non-fractional
**                                      digits; i.e. more than (pout - sout)
**                                      of them.
**
**      Exceptions:
**	    None.
**
** Side Effects:
**          none
**
** History:
**      25-may-88 (jrb)
**          Initial creation and coding.
**	15-sep-89 (jrb)
**	    Changed for use in VMS CL.
**	04-may-1993 (rog)
**	    Converted to FPpkadd_sim().
**	14-feb-2007 (dougi)
**	    Replaced hard constant 31 with CL_MAX_DECPREC.
*/
 
STATUS
FPpkadd_sim( PTR pk1, i4 p1, i4 s1, PTR pk2, i4 p2, i4 s2, PTR pkout,
	     i4 pout, i4 sout )
{
    u_char      *ptr1 = (u_char *)pk1 + p1 / 2;    /* ptr's walk thru addends */
    u_char      *ptr2 = (u_char *)pk2 + p2 / 2;    /* and result from end to  */
    u_char      *ptr3;				   /* start */
    u_char      **gptr;    /* generic ptr for pointing at ptr1 or ptr2 */
    i4         *gcnt;     /* generic cnt for counting digits in pk1 or pk2 */
    i4         gp;        /* generic precision counter */
    i4         neg1;
    i4         neg2;
    i4         p3;            /* precision of result */
    i4         s3;            /* scale of result */
    i4         sum;           /* sum of two digits */
    i4         xdigs;         /* nonoverlapping-digits counter */
    i4         p1cnt = 1;     /* digit counters for addends and result */
    i4         p2cnt = 1;
    i4         p3cnt = 1;
    i4         carry = 0;

    /* find out if either addend is negative */
    neg1 = ((*ptr1 & 0xf) == FP_PK_MINUS  ||  (*ptr1 & 0xf) == FP_PK_AMINUS);
    neg2 = ((*ptr2 & 0xf) == FP_PK_MINUS  ||  (*ptr2 & 0xf) == FP_PK_AMINUS);
 
    /* if signs are different we make a temporary copy of one addend, change */
    /* its sign, and call FPpksub with proper arguments			    */
    if (neg1 != neg2)
    {
	u_char	pktmp[(CL_MAX_DECPREC+1)/2];

        if (neg1)
        {
	    MEcopy(pk1, (u_i2)(p1 / 2 + 1), (PTR)pktmp);
            pktmp[p1 / 2] = (pktmp[p1 / 2] & (u_char)0xf0) | FP_PK_PLUS;
            return(FPpksub_sim(pk2, p2, s2, pktmp, p1, s1, pkout, pout, sout));
        }
        else
        {
	    MEcopy(pk2, (u_i2)(p2 / 2 + 1), (PTR)pktmp);
            pktmp[p2 / 2] = (pktmp[p2 / 2] & (u_char)0xf0) | FP_PK_PLUS;
            return(FPpksub_sim(pk1, p1, s1, pktmp, p2, s2, pkout, pout, sout));
        }
    }
 
    p3 = pout;
    s3 = sout;

    /* if we got this far, the addends' signs were the same */
    /* set sign on result to whatever addends' signs were */
    ptr3 = (u_char *)pkout + p3 / 2;
    if (neg1)
        *ptr3 = FP_PK_MINUS;
    else
        *ptr3 = FP_PK_PLUS;
 
    /* set generic pointer and counter to number with larger scale */
    if (s1 > s2)
    {
        gptr  = &ptr1;
        gcnt  = &p1cnt;
        xdigs = s3 - s2;
    }
    else
    {
        gptr  = &ptr2;
        gcnt  = &p2cnt;
        xdigs = s3 - s1;
    }
 
    /* now copy any excess scale digits to the result */
    while (xdigs--)
        if (p3cnt++ & 1)
            *ptr3-- |=
                (u_char)((((*gcnt)++ & 1) ? *(*gptr)-- : **gptr << 4) & 0xf0);
        else
            *ptr3 =
                (u_char)((((*gcnt)++ & 1) ? *(*gptr)-- >> 4 : **gptr) & 0x0f);
 
    /* here is the heart of this routine: it adds the two addends' digits */
    /* until there are no more digits in either or both of them (or there */
    /* is no more room in the result)                                     */
    while (p1cnt <= p1  &&  p2cnt <= p2  &&  p3cnt <= p3)
    {
        carry = (
            (sum = (((p1cnt++ & 1)  ?  *ptr1-- >> 4  :  *ptr1) & 0xf)
                 + (((p2cnt++ & 1)  ?  *ptr2-- >> 4  :  *ptr2) & 0xf)
                 + carry)
        > 9);
 
        if (p3cnt++ & 1)
            *ptr3-- |= (u_char)(((carry  ?  sum - 10  :  sum) << 4) & 0xf0);
        else
            *ptr3    = (u_char)(carry  ?  sum - 10  :  sum);
    }
 
    /* now set generic pointer, counter, and precision holder to number with */
    /* the larger number of integral digits                                  */
    if (p1-p1cnt > p2-p2cnt)
    {
        gptr = &ptr1;
        gcnt = &p1cnt;
        gp   = p1;
    }
    else
    {
        gptr = &ptr2;
        gcnt = &p2cnt;
        gp   = p2;
    }
 
    /* then copy excess precision digits (if any) from number with larger */
    /* precision into result and add in carry from above addition loop    */
    while (*gcnt <= gp  &&  p3cnt <= p3)
    {
        carry = (
            (sum = ((((*gcnt)++ & 1)  ?  *(*gptr)-- >> 4  :  **gptr) & 0xf)
                 + carry)
        > 9);
 
        if (p3cnt++ & 1)
            *ptr3-- |= (u_char)(((carry  ?  sum - 10  :  sum) << 4) & 0xf0);
        else
            *ptr3    = (u_char)(carry  ?  sum - 10  :  sum);
    }    
 
    /* now check for overflow; if any preceeding loop was terminated by the */
    /* result's becoming too large, control should end up here; now check   */
    /* if either there is an outstanding carry or if there are any non-zero */
    /* digits left in the excess precision digits we are copying            */
    if (p3cnt > p3)
        if (carry)
	{
	    return(FP_DECOVF);
	}
        else
	{
            while (*gcnt <= gp)
                if (((((*gcnt)++ & 1) ? *(*gptr)-- >> 4 : **gptr) & 0xf) != 0)
                {
		    return(FP_DECOVF);
                }
	}
 
    /* finally, we put zeros into all remaining positions (if any) in the  */
    /* result, taking into account a possible carry left over from above   */
    while (p3cnt <= p3)
    {
        if (p3cnt++ & 1)
            *ptr3-- |= carry ? (u_char)0x10 : (u_char)0;
        else
            *ptr3    = carry ? (u_char)0x01 : (u_char)0;
        carry = 0;
    }
    return(OK);
}
 

/*{
** Name: FPpksub_sim() - Subtract two packed decimals.
**
** Description:
**	This routine is called by FPpksub which is written in MACRO.  The
**	sequence is: try using the VAX instruction to subtract; if that
**	overflows, use this code; if that overflows then it's really an
**	overflow.
**
**
** Inputs:
**      pk1                             Pointer to the minuend.
**      p1                              Its precision:  1 <= p1 <= 39.
**      s1                              Its scale:  0 <= s1 <= p1.
**      pk2                             Pointer to the subtrahend.
**      p2                              Its precision:  1 <= p2 <= 39.
**      s2                              Its scale:  0 <= s2 <= p2.
**	pout				Result precision
**	sout				Result scale
**
** Outputs:
**      pkout                           The resulting packed decimal value is
**                                      placed here.
**
**      Returns:
**          OK				The operation succeeded.
**
**          FP_DECOVF                    Result has too many non-fractional
**                                      digits; i.e. more than (pout - sout)
**                                      of them.
**
**      Exceptions:
**          None
**
** Side Effects:
**          none
**
** History:
**      01-jun-88 (jrb)
**          Initial creation and coding.
**	15-sep-89 (jrb)
**	    Adapted this routine for the VMS CL.
**	04-may-1993 (rog)
**	    Converted to FPpksub_sim().
**	14-feb-2007 (dougi)
**	    Replaced hard constant 31 with CL_MAX_DECPREC.
*/
 
STATUS
FPpksub_sim( PTR pk1, i4 p1, i4 s1, PTR pk2, i4 p2, i4 s2, PTR pkout,
	     i4 pout, i4 sout )
{
    u_char      *ptr1 = (u_char *)pk1 + p1 / 2;    /* ptr's walk thru numbers */
    u_char      *ptr2 = (u_char *)pk2 + p2 / 2;	   /* and result from end to  */
    u_char      *ptr3;				   /* start */
    u_char      *tmpptr;	/* used for swapping */
    i4         temp;
    i4         neg1;
    i4         neg2;
    i4         p3;             /* precision of result */
    i4         s3;             /* scale of result */
    i4         diff;           /* difference of two digits */
    i4		xdigs;
    i4         p1cnt = 1;      /* digit counters for addends and result */
    i4         p2cnt = 1;
    i4         p3cnt = 1;
    i4         borrow = 0;
 
    /* find out if either number is negative */
    neg1 = ((*ptr1 & 0xf) == FP_PK_MINUS  ||  (*ptr1 & 0xf) == FP_PK_AMINUS);
    neg2 = ((*ptr2 & 0xf) == FP_PK_MINUS  ||  (*ptr2 & 0xf) == FP_PK_AMINUS);
 
    /* if signs are different we make a temporary copy of one number, change */
    /* its sign, and call FPpkadd with proper arguments			    */
    if (neg1 != neg2)
    {
	u_char	pktmp[(CL_MAX_DECPREC+1)/2];

        if (neg1)
        {
	    MEcopy(pk2, (u_i2)(p2 / 2 + 1), (PTR)pktmp);
            pktmp[p2 / 2] = (pktmp[p2 / 2] & (u_char)0xf0) | FP_PK_MINUS;
            return(FPpkadd_sim(pk1, p1, s1, (PTR)pktmp, p2, s2, pkout, pout, sout));
        }
        else
        {
	    MEcopy(pk2, (u_i2)(p2 / 2 + 1), (PTR)pktmp);
            pktmp[p2 / 2] = (pktmp[p2 / 2] & (u_char)0xf0) | FP_PK_PLUS;
            return(FPpkadd_sim(pk1, p1, s1, (PTR)pktmp, p2, s2, pkout, pout, sout));
        }
    }

    /* calcuate result's precision and scale using proper formulas */
    p3 = pout;
    s3 = sout;
 
    /* set result pointer to end of its space so we can set sign next */
    ptr3 = (u_char *)pkout + p3 / 2;

    /* find out which number is larger in absolute value; swap ptr's and     */
    /* associated precision  and scale if necessary to ensure ptr1 is larger */
    /* in absolute value; also, set result's sign depending on neg flag and  */
    /* which number was larger in absolute value */
    if (FPpkcmp(pk1, p1, s1, pk2, p2, s2) == (neg1 ? 1 : -1))
    {
        tmpptr = ptr1;
        ptr1   = ptr2;
        ptr2   = tmpptr;

        temp   = p1;
        p1     = p2;
        p2     = temp;

        temp   = s1;
        s1     = s2;
        s2     = temp;

        *ptr3 = (neg1  ?  FP_PK_PLUS  :  FP_PK_MINUS);
    }
    else
    {
        *ptr3 = (neg1  ?  FP_PK_MINUS  :  FP_PK_PLUS);
    }
 
    /* loop through any excess scale digits; if excess digits belong to */
    /* minuend we just copy them--if they belong to the subtrahend we   */
    /* complement them by 9 (10 for the rightmost) and copy them        */
    if (s1 >= s2)
    {
        xdigs = s3 - s2;
        while (xdigs--)
            if (p3cnt++ & 1)
                *ptr3-- |=
                    (u_char)(((p1cnt++ & 1) ? *ptr1-- : *ptr1 << 4) & 0xf0);
            else
                *ptr3 =
                    (u_char)(((p1cnt++ & 1) ? *ptr1-- >> 4 : *ptr1) & 0x0f);
    }
    else
    {
        xdigs = s3 - s1;
        while (xdigs--)
        {
            borrow = (
                (diff = - (((p2cnt++ & 1)  ?  *ptr2-- >> 4  : *ptr2) & 0xf)
                        - borrow)
            < 0);

            if (p3cnt++ & 1)
                *ptr3-- |=
                    (u_char)(((borrow  ?  diff + 10  :  diff) << 4) & 0xf0);
            else
                *ptr3    = (u_char)(borrow  ?  diff + 10  :  diff);
        }
    }
 
    /* and this is the heart of the subtraction routine: we subtract one   */
    /* digit from another and record the borrow;  this continues until any */
    /* of the numbers exceeds its precision 				   */
    while (p1cnt <= p1  &&  p2cnt <= p2  &&  p3cnt <= p3)
    {
        borrow = (
            (diff = (((p1cnt++ & 1)  ?  *ptr1-- >> 4  :  *ptr1) & 0xf)
                  - (((p2cnt++ & 1)  ?  *ptr2-- >> 4  :  *ptr2) & 0xf)
                  - borrow)
        < 0);
 
        if (p3cnt++ & 1)
            *ptr3-- |= (u_char)(((borrow  ?  diff + 10  :  diff) << 4) & 0xf0);
        else
            *ptr3    = (u_char)(borrow  ?  diff + 10  :  diff);
    }
 
    /* since we guaranteed that ptr1 is larger than ptr2, we know that if */
    /* there is anything left it must be in ptr1 (it is not too hard to   */
    /* convince yourself that if p3cnt > p3 then there cannot be digits   */
    /* remaining in BOTH ptr1 and ptr2); now we copy any remaining digits */
    /* from ptr1 to ptr3 (allowing for a borrow from above)		  */
    while (p1cnt <= p1  &&  p3cnt <= p3)
    {
        borrow = (
            (diff = (((p1cnt++ & 1)  ?  *ptr1-- >> 4  :  *ptr1) & 0xf)
                  - borrow)
        < 0);
 
        if (p3cnt++ & 1)
            *ptr3-- |= (u_char)(((borrow  ?  diff + 10  :  diff) << 4) & 0xf0);
        else
            *ptr3    = (u_char)(borrow  ?  diff + 10  :  diff);
    }    

    /* now check for overflow;  if there is an outstanding borrow we must */
    /* have a 1 in ptr1 right now (anything larger will mean more digits  */
    /* are necessary in the result, and a 0 means that there are non-zero */
    /* digits somewhere to the left in ptr1 (because ptr1 is greater than */
    /* ptr2) so there can't be a borrow if all digits remaining in ptr1   */
    /* are zeros!)							  */
    if (p3cnt > p3)
    {
	/* a borrow is ok as long as the next ptr1 digit is a one (in */
	/* which case it cancels out and everthing is peachy)	      */
        if (borrow)
	    if ((((p1cnt++ & 1) ? *ptr1-- >> 4  :  *ptr1) & 0xf) != 1)
	    {
		return(FP_DECOVF);
	    }

	while (p1cnt <= p1)
	    if ((((p1cnt++ & 1) ? *ptr1-- >> 4 : *ptr1) & 0xf) != 0)
	    {
		return(FP_DECOVF);
	    }
    }

    /* finally, we put zeros into all remaining positions (if any) */
    while (p3cnt <= p3)
        if (p3cnt++ & 1)
            ptr3--;
        else
            *ptr3 = (u_char)0;
    
    return(OK);
}

 

/*{
** Name: FPpkmul_sim() - Multiply two packed decimals.
**
** Description:
**	This routine is called by FPpkmul which is written in MACRO.  The
**	sequence is: try using the VAX instruction to multiply; if that
**	overflows, use this code; if that overflows then it's really an
**	overflow.
**
** Inputs:
**      pk1                             Pointer to the multiplicand.
**      p1                              Its precision:  1 <= p1 <= 39.
**      s1                              Its scale:  0 <= s1 <= p1.
**      pk2                             Pointer to the multiplier.
**      p2                              Its precision:  1 <= p2 <= 39.
**      s2                              Its scale:  0 <= s2 <= p2.
**	pout				Result precision
**	sout				Result scale
**
** Outputs:
**      pkout                           The resulting packed decimal value is
**                                      placed here.
**
**      Returns:
**          VOID
**
**      Exceptions:
**          EXDECOVF                    Result has too many non-fractional
**                                      digits; i.e. more than (pout - sout)
**                                      of them.
**
** Side Effects:
**          none
**
** History:
**      13-sep-88 (jrb)
**          Initial creation.
**	16-feb-89 (jrb)
**	    Rewrote using base-10000 digits to improve performance.
**	20-feb-89 (jrb)
**	    Corrected overflow handling by not generating overflow when excess
**	    fractional digits resulted; just truncate the digits.
**	01-sep-89 (jrb)
**	    Made this into a subordinate routine to be called from FPpkmul when
**	    it finds that it cannot do the multiplication with the hardware
**	    instruction.
**	04-may-1993 (rog)
**	    Converted to FPpkmul_sim().
**	14-feb-2007 (dougi)
**	    Replaced hard constant 31 with CL_MAX_DECPREC.
*/

STATUS
FPpkmul_sim( PTR pk1, i4 p1, i4 s1, PTR pk2, i4 p2, i4 s2, PTR pkout,
	     i4 pout, i4 sout )
{
    u_char	*ptr1 = (u_char *)pk1 + p1 / 2;	/* end of multiplicand */
    u_char	*ptr2 = (u_char *)pk2 + p2 / 2;	/* end of multiplier */
    u_char	*ptr3;				/* product pointer   */
    i4		pka1[(CL_MAX_DECPREC + 3) / 4];	/* temp buffers */
    i4		pka3[(2*CL_MAX_DECPREC + 3) / 4];
    i4		pkc1;				/* indexes into buffers */
    i4		pkc3 = 0;
    i4		pkc3sav = 0;			/* holder for pkc3	    */
    i4		pc2 = 0;			/* precision counter	    */
    i4		pka1used = (p1 + 3) / 4;	/* space needed in pka1	    */
						/* buffer for pk1 (storing  */
						/* 4 digits per i4)	    */
    i4		pka3used;
    i4		dig;
    i4		carry;
    i4		tens;				/* for converting into and  */
						/* out of base 10000	    */
    i4		skip = 0;			/* # of digits to truncate  */
    i4	tmp;				/* temp product of two digits */
    i4		t;
    i4		i;

    /* set ptr3 to end of result buffer */
    ptr3 = (u_char *)pkout + pout / 2;

    /* set result's sign;  if signs are same then set result pos else neg */
    *ptr3 = ((*ptr1 & 0xf) == FP_PK_MINUS  ||  (*ptr1 & 0xf) == FP_PK_AMINUS)
         == ((*ptr2 & 0xf) == FP_PK_MINUS  ||  (*ptr2 & 0xf) == FP_PK_AMINUS)
	 ?  FP_PK_PLUS  :  FP_PK_MINUS;

    /* Algorithm: 
    **	We will put the multiplicand (pk1) into a temp array of i4s (pka1)
    **	four digits at a time.  Then we multiply using base 10000 digits; this
    **	cuts down on the number of iterations on the inner loop.  The number
    **	10000 was chosen because we are guaranteed to be able to hold at least
    **	10 digits using a i4 (i4s are at least 32 bits) and we know
    **	that the product of two 4 digit numbers can never be longer than 10
    **	digits.  So base 10000 means we are dealing with 4 digits at a time.
    **	The (x + 3) / 4 that you see all over this code finds the number of
    **	base-10000 digits necessary to hold x base-10 digits.
    */

    /* zero out temp result buffer */
    pka3used = (p1 + p2 + 3) / 4;
    for (i=0; i < pka3used; i++)
	pka3[i] = 0;

    /* move pk1 into temp multiplicand buffer four digits at a time */
    t = -1;
    for (i=0; i < p1; i++)
    {
	if (i % 4 == 0)
	{
	    pka1[++t] = (*ptr1-- >> 4) & 0xf;
	    tens = 10;
	}
	else
	{
	    pka1[t] = pka1[t] +
			tens * ((i & 1  ?  *ptr1  :  (*ptr1-- >> 4)) & 0xf);
	    tens *= 10;
	}
    }
    

    /* now get a digit from the multiplier and if it's non-zero then multiply */
    /* it by each digit of the multiplicand and add the product to the result */
    while (pc2++ < p2)
    {
	/* convert four digits (or less) of multiplier into dig */
	tens = 10;
	dig = (*ptr2-- >> 4) & 0xf;

	for (i=0; i < 3  &&  pc2 < p2; i++, tens *= 10)
	    dig = dig + tens * ((pc2++ & 1  ?  *ptr2  :  (*ptr2-- >> 4)) & 0xf);

	if (dig != 0)
	{
	    /* now multiply pka1 digits by dig and store in pka3 */
	    pkc1 = 0;
	    carry = 0;
	    while (pkc1 < pka1used)
	    {
		tmp = pka1[pkc1++] * dig + pka3[pkc3] + carry;
		pka3[pkc3++] = tmp % 10000L;
		carry = tmp / 10000L;
	    }

	    pka3[pkc3] = carry;
	}
	pkc3  = ++pkc3sav;
    }

    /* if there are more than 39 fractional digits we truncate some of them */
    t = -1;
    if (sout == CL_MAX_DECPREC)
    {
	t = (skip = s1 + s2 - CL_MAX_DECPREC) / 4;
	if (skip % 4 == 0)
	{
	    t--;
	}
	else
	{
	    dig = pka3[skip / 4];
	    for (i = skip % 4; i > 0; i--)
		dig /= 10;
	}
    }

    /* now move answer from temp result buffer into pout */
    for (i = 0; i < pout; i++)
    {
	if ((i + skip) % 4 == 0)
	    dig = pka3[++t];

	if (i & 1)
	    *ptr3 = (u_char)(dig % 10);
	else
	    *ptr3-- |= (u_char)((dig % 10) << 4);

	dig /= 10;
    }

    if (dig != 0)
    {
	return(FP_DECOVF);
    }

    while (++t < pka3used)
    {
	if (pka3[t] != 0)
	{
	    return(FP_DECOVF);
	}
    }
    return(OK);
}
 

/*{
** Name: FPpkdiv() - Divide two packed decimals.
**
** Description:
**	Divides two packed decimal numbers.  This function does not use
**	a corresponding MACRO routine because it is easier to catch the
**	divide-by-zero error and map it to FP_DECDIV in C.
**
** Inputs:
**      pk1                             Pointer to the dividend.
**      p1                              Its precision:  1 <= p1 <= 39.
**      s1                              Its scale:  0 <= s1 <= p1.
**      pk2                             Pointer to the divisor.
**      p2                              Its precision:  1 <= p2 <= 39.
**      s2                              Its scale:  0 <= s2 <= p2.
**	pout				Result precision
**	sout				Result scale
**
** Outputs:
**      pkout                           The resulting packed decimal value is
**                                      placed here.
**
**      Returns:
**          OK				The operation succeeded.
**
**          FP_DECOVF                    Result has too many non-fractional
**                                      digits; i.e. more than (pout - sout)
**                                      of them.
**          FP_DECDIV                    Attempt to divide by zero.  The
**                                      denominator (2nd input decimal value)
**                                      is equal to zero. 
**
**      Exceptions:
**          none
**
** Side Effects:
**          none
**
** History:
**	20-feb-89 (jrb)
**	    Initial creation and coding.
**	01-sep-89 (jrb)
**	    Made this into a subordinate routine to be called from FPpkdiv when
**	    it finds that it cannot do the division with the hardware
**	    instruction.
**	04-may-1993 (rog)
**	    Converted from MHpkdiv_sim().
**	14-feb-2007 (dougi)
**	    Replaced hard constant 31 with CL_MAX_DECPREC.
*/
 
STATUS
FPpkdiv_sim( PTR pk1, i4 p1, i4 s1, PTR pk2, i4 p2, i4 s2, PTR pkout,
	     i4 pout, i4 sout )
{
    u_char	*ptr1 = (u_char *)pk1 + p1 / 2;	/* end of dividend */
    u_char	*ptr2 = (u_char *)pk2 + p2 / 2;	/* end of divisor */
    u_char	*ptr3;
    i4		pka1[(2*CL_MAX_DECPREC + 3) / 4 + 1]; /* temp buffers */
    i4		pka2[(CL_MAX_DECPREC + 3) / 4];
    i4		pka3[(CL_MAX_DECPREC + 3) / 4 + 1];
    i4		pk1frac;
    i4		pka1used;
    i4		pka2used = 0;
    i4		carry;
    i4		borrow;
    i4		offset;
    i4		tens;				/* for converting into and  */
						/* out of base 10000	    */
    i4	tmp;				/* temp product of two digits */
    i4		dig;
    i4		t;
    i4		i;
    i4		j;

    /* calculate scale of temp dividend value */
    pk1frac = sout + s2;

    /* set ptr3 to end of result buffer */
    ptr3 = (u_char *)pkout + pout / 2;

    /* set result's sign;  if signs are same then set result pos else neg */
    *ptr3 = ((*ptr1 & 0xf) == FP_PK_MINUS  ||  (*ptr1 & 0xf) == FP_PK_AMINUS)
         == ((*ptr2 & 0xf) == FP_PK_MINUS  ||  (*ptr2 & 0xf) == FP_PK_AMINUS)
	 ?  FP_PK_PLUS  :  FP_PK_MINUS;

    /* now put ptr2 into pka2 using base-10000 digits */
    t = -1;
    for (i=0; i < p2; i++)
    {
	if (i % 4 == 0)
	{
	    tens = 10;
	    pka2[++t] = (*ptr2-- >> 4) & 0xf;
	    if (t > 0  &&  pka2[t-1] != 0)
		pka2used = t;
	}
	else
	{
	    pka2[t] += tens * (((i & 1)  ?  *ptr2  :  (*ptr2-- >> 4)) & 0xf);
	    tens *= 10;
	}
    }
    if (pka2[t] != 0)
        pka2used = t+1;

    /* if pka2used never moved then the divisor was 0 */
    if (pka2used == 0)
    {
	return(FP_DECDIV);
    }

    /* now we have to set up the dividend so that is has at least (39 + p2) */
    /* base-10 digits (so that the quotient will have 39 digits) and scale  */
    /* pk1frac.  First set up the scale part (fractional part).		    */
    if (s1 <= pk1frac)
    {
	i4	pad = pk1frac - s1;

	offset = pad % 4;
	for (t=0; t < (pad + 3) / 4; t++)
	    pka1[t] = 0;
	t--;

	tens = 1;
	for (i=0; i < offset; i++)
	    tens *= 10;

	i=0;
    }
    else
    {
	offset = 0;
	t = -1;
	i = s1 - pk1frac;
	ptr1 -= (i + 1) / 2;
    }

    /* now move dividend into pka1 buffer using base-10000 digits */
    for (; i < p1; i++)
    {
	if ((i + offset) % 4 == 0)
	{
	    tens = 10;
	    pka1[++t] = ((i & 1)  ?  *ptr1  :  (*ptr1-- >> 4)) & 0xf;
	}
	else
	{
	    pka1[t] += tens * (((i & 1)  ?  *ptr1  :  (*ptr1-- >> 4)) & 0xf);
	    tens *= 10;
	}
    }

    /* now we've filled (p1 - s1 + pk1frac) base-10 places in pka1; zero    */
    /* fill (if necessary) until we have (10 + pka2used) base-10000 digits   */
    /* so that when we divide by pka2 we will have 10 base-10000 digits left */
    /* which will give us the 39 base-10 digits we need to return.	    */
    while (++t < 10 + pka2used)
	pka1[t] = 0;
    pka1used = t;

    /* now we're ready to divide; we fast-path the case where pka2used == 1 */
    /* and use the normalization method when it's larger; note that these   */
    /* algorithms are just doing division of nonnegative integers--all	    */
    /* accounting for signs and decimal point placement has been done above */
    carry = 0;
    if (pka2used == 1)
    {
	i = pka2[0];
	
	/* this is easy... just divide in one pass */
	for (t = pka1used-1; t >= 0; t--)
	{
	    tmp = pka1[t] + carry * 10000L;
	    pka3[t] = tmp / i;
	    carry = tmp % i;
	}
    }
    else
    {
	/* the following algorithm is based on Knuth Vol. II, pp. 255-260   */
	/* and the associated exercises					    */
	i4	    d = 10000L / (pka2[pka2used-1] + 1);
	i4	    v1, v2;
	i4	    q;

	/* first we normalize pka1 and pka2 by multiplying by d; this will  */
	/* ensure that the leading digit of the divisor (pka2) is at least  */
	/* 5000 which is necessary for the algorithm's later assumptions.   */
	if (d == 1)
	{
	    pka1[pka1used] = 0;
	}
	else
	{
	    /* multiply divisor by d */
	    for (i=0; i < pka2used; i++)
	    {
		tmp = (i4)pka2[i] * d + carry;
		pka2[i] = tmp % 10000L;
		carry = tmp / 10000L;
	    }

	    /* multiply dividend by d */
	    for (i=0; i < pka1used; i++)
	    {
		tmp = (i4)pka1[i] * d + carry;
		pka1[i] = tmp % 10000L;
		carry = tmp / 10000L;
	    }
	    pka1[pka1used] = carry;
	}

	/* we used an additional position in pka1 here so account for it */
	pka1used++;

	v1 = pka2[pka2used-1];
	v2 = pka2[pka2used-2];
	for (t = pka1used-1; t >= pka2used; t--)
	{
	    /* first we try to get an approximation for our quotient digit; */
	    /* this approximation either be correct or one too large	    */
	    if (pka1[t] == v1)
	    {
		q = 9999;
	    }
	    else
	    {
		i4	    f = pka1[t] * 10000L + pka1[t-1];

		q = f / v1;

		/* this will loop at most twice */
		while ((i4)v2*q > (f-q*(i4)v1)*10000L+pka1[t-2])
		    q--;
	    }

	    /* now subtract pka2*q from pka1[t] through pka1[t-pka2used] */
	    borrow = 0;
	    for (i = t-pka2used, j=0; i < t; i++, j++)
	    {
		tmp = pka1[i] - (i4)pka2[j] * q + borrow;

		if (tmp < 0L)
		{
		    pka1[i] = tmp % 10000L;
		    borrow = tmp / 10000L;
		    if (pka1[i] < 0)
		    {
			pka1[i] += 10000;
			borrow--;
		    }
		}
		else
		{
		    pka1[i] = tmp;
		    borrow = 0;
		}
	    }
	    pka1[t] += borrow;
	    pka3[t-pka2used] = q;

	    /* the next test will be true with probability on the order of  */
	    /* one in 5000 (Knuth, 4.3.1, exercise 21).  Note that Knuth's  */
	    /* solution to this exercise is base 10, and therefore will not */
	    /* work for this routine.  An example of input that will cause  */
	    /* the following code to execute is:			    */
	    /*       4121633300000000 [31,0] / 588888888888 [16,0]	    */
	    /* In this case q will be 6999, which is one too large.	    */
	    /* Note: This code will never be executed if pka2used < 3.	    */
	    if (pka1[t] < 0)
	    {
		pka3[t-pka2used]--;	/* was one too many */

		/* add divisor back once */
		carry = 0;
		for (i = t-pka2used, j=0; i < t; i++, j++)
		{
		    carry = (pka1[i] += pka2[j] + carry) > 10000;

		    if (carry)
			pka1[i] -= 10000;
		}
		pka1[t]++;  /* there will always be a carry left over here  */
	    }
	}
    }

    /* put result into pkout */
    t = -1;
    for (i=0; i < pout; i++)
    {
	if (i % 4 == 0)
	    dig = pka3[++t];

	if (i & 1)
	    *ptr3 = (u_char)(dig % 10);
	else
	    *ptr3-- |= (u_char)((dig % 10) << 4);

	dig /= 10;
    }

    if (dig != 0)
    {
	return(FP_DECOVF);
    }

    while (++t < pka1used-pka2used)
    {
	if (pka3[t] != 0)
	{
	    return(FP_DECOVF);
	}
    }
    return(OK);
}
