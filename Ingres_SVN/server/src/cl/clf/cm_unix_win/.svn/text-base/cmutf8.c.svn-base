/*
**Copyright (c) 2007 Ingres Corporation
*/

#include    <compat.h>
#include    <clconfig.h>
#include    <gl.h>
#include    <cs.h>
#include    <cm.h>
#include    <nm.h>
#include    <st.h>
#include    <lo.h>
#include    <me.h>
#include    <meprivate.h>
#include    <mu.h>
#include    <errno.h>

#ifdef	UNIX
#include    <ernames.h>
#endif

#ifdef	VMS
#include    <si.h>
#include    <cs.h>
#include    <lnmdef.h>
#include    <iodef.h>
#include    <fab.h>
#include    <rab.h>
#include    <ssdef.h>
#endif

#ifdef DESKTOP
#include    <er.h>
#endif /* DESKTOP */

/*
**  External variables
*/
GLOBALREF CM_UTF8ATTR *CM_UTF8AttrTab; 
GLOBALREF CM_UTF8CASE *CM_UTF8CaseTab;

i4 CM_UTF8toUTF32 (char *instr, i4 incnt, u_i2 *outval);
i4 CM_UTF32toUTF8 (int inval, char *resstr);

/**
**
**  Name: CMUTF8.C - Routines to manipulate unicode strings encoded in UTF8
**	             format. These routines are equivalent to other CM 
**		     routines.
**
**  Description:
**      This file contains Routines to manipulate unicode strings encoded 
**	in UTF8 format. These routines are equivalent to other similar CM 
**	routines for multibyte characters and will be called from the macro for
**	other CM routines. However they will they take into account 
**	that the UTF8 strings can be precomposed to form unicode characters.
**
**  History:
**      05-Apr-2007 (gupsh01)
**	    Created.
*/

/* Name: cmupct - calculates the position count of the
**		  previous start character in a UTF8 
**		  stream.
** History:
**
**	05-may-2007 (gupsh01)
**	    Added History section.
*/
i4
cmupct (u_char *str, u_char *startpos)
{
  unsigned char	*p = str;
  int		rescnt = 1;
  if (str && startpos)
  {
    rescnt = 0;
    while (p >= startpos)
    {
        p--;
        rescnt++;
        if (((*p & 0xFF) & 0x80) == 0)
          break;
        else if (((*p & 0xFF) & 0xC0) != 0)
          break;
    }
  }
  return rescnt;
}

/* Name: cmu_getutf8property - Gets the property
**			   for UTF8 charactersets
** Description:
**
** Input
**	key	UTF8 character bytes.
**	keycnt	count of input characters.
**
** Output
**	property if ok else returns 0.
**
** History:
**	20-may-2007 (gupsh01)
**	    Created.
*/
u_i2
cmu_getutf8property(u_char *key, i4 keycnt)
{
    u_i2 index;
    if (CM_UTF8toUTF32(key, keycnt, &index) != OK)
      return (0);
    else
      return (CM_UTF8AttrTab[index].property);
}

/* Name: cmu_getutf8casecnt - Gets the casecount 
**			   for UTF8 charactersets
** Description:
** Input
** Output
**
** History:
**	20-may-2007 (gupsh01)
**	    Created.
*/
u_i2
cmu_getutf8casecnt(u_char *key, i4 keycnt)
{
    u_i2 index;
    u_i2 caseindex;
    u_i2 casecnt = 0;

    if (CM_UTF8toUTF32(key, keycnt, &index) != OK)
      return (0); 

    caseindex = CM_UTF8AttrTab[index].caseindex;
    if (caseindex != 0)
    {
	casecnt = CM_UTF8CaseTab[caseindex].casecnt;
    }
    return (casecnt);
}

/* Name: cmu_utf8_tolower - Converts a UTF8 string to lowercase
**	 This routine expects a dstlen and srclen.
**	 it will check the dstlen if it is nonzero
**
** Input 
**
** Output
**
**
** History:
**	20-may-2007 (gupsh01)
**	    Created.
*/
u_i2
cmu_getutf8_tolower(u_char *src, i4 srclen, u_char *dst, i4 dstlen)
{
  u_i2		index = 0;
  u_i2		caseindex = 0;
  u_char	temp[8] = {0};
  bool		tempused = FALSE;
  u_char	*pdst;
  u_i2		plen = 0;

  /* Check for inplace replace */
  if (src == dst)
  {
    plen = srclen;
    pdst = temp;
    tempused = TRUE;
  }
  else 
    pdst = dst;

  if (srclen != 1)
  {
    /* We don't fail right now and just copy the source
    ** to destination.
    */
    if (CM_UTF8toUTF32(src, srclen, &index) != OK)
    {
      if (!(tempused))
      {
        MEcopy (src, srclen, dst);	/* copy the source to dest */
      }
      return (OK); 
    }
  }
  else 
    index = *src;

  caseindex = CM_UTF8AttrTab[index].caseindex;
  if (caseindex != 0)
  {
    u_i2	property;
    char	*casechars;
    i4		casecnt;

    property = CM_UTF8AttrTab[index].property;
    if (property & CM_A_UPPER)
    {
      casecnt = CM_UTF8CaseTab[caseindex].casecnt;
      if (dstlen && (dstlen < casecnt))
        return (FAIL);
      casechars = CM_UTF8CaseTab[caseindex].casechars;
      MEcopy (casechars, casecnt, pdst);
      plen = casecnt;
    }
    else 
      MEcopy (src, srclen, pdst);
  }
  else 
    MEcopy (src, srclen, pdst);

  if (tempused)
    MEcopy (pdst, plen, dst);

  return (OK);
}

/* Name: cmu_utf8_toupper - Converts a UTF8 string to lowercase
**	 This routine expects a dstlen and srclen.
**	 it will check the dstlen if it is nonzero
**
**	 This routine does not fail right nor for illegal input
**	 but returns.
**
** Input 
**
** Output
**
** History:
**	20-may-2007 (gupsh01)
**	    Created.
*/
u_i2
cmu_getutf8_toupper(u_char *src, i4 srclen, u_char *dst, i4 dstlen)
{
  u_i2		index = 0;
  u_i2		caseindex = 0;
  u_char	temp[8] = {0};
  bool		tempused = FALSE;
  u_char	*pdst;
  u_i2		plen = 0;

  /* Check for inplace replace */
  if (src == dst)
  {
    plen = srclen;
    pdst = temp;
    tempused = TRUE;
  }
  else 
    pdst = dst;

  if (srclen != 1)
  {
    if (CM_UTF8toUTF32(src, srclen, &index) != OK)
    {
    /* We don't fail right now and just copy the source
    ** to destination.
    */
      if (!(tempused))
      {
        MEcopy (src, srclen, dst);	/* copy the source to dest */
      }
      return (OK); 
    }
  } 
  else 
    index = *src;

  caseindex = CM_UTF8AttrTab[index].caseindex;
  if (caseindex != 0)
  {
    u_i2	property;
    char	*casechars;
    i4		casecnt;

    property = CM_UTF8AttrTab[index].property;
    if (property & CM_A_LOWER)
    {
      casecnt = CM_UTF8CaseTab[caseindex].casecnt;
      if (dstlen && (dstlen < casecnt))
        return (FAIL);
      casechars = CM_UTF8CaseTab[caseindex].casechars;
      MEcopy (casechars, casecnt, pdst);
      plen = casecnt;
    }
    else 
      MEcopy (src, srclen, pdst);
  }
  else 
    MEcopy (src, srclen, pdst);

  if (tempused)
    MEcopy (pdst, plen, dst);

  return (OK);
}

/* The routines below are used for conversion between UTF8 and UTF32 
** adopted from Unicode.org, and modified for our requirements. 
*
** Copyright 2001-2004 Unicode, Inc.
**
** Disclaimer
**
** This source code is provided as is by Unicode, Inc. No claims are
** made as to fitness for any particular purpose. No warranties of any
** kind are expressed or implied. The recipient agrees to determine
** applicability of information provided. If this file has been
** purchased on magnetic or optical media from Unicode, Inc., the
** sole remedy for any claim will be exchange of defective media
** within 90 days of receipt.
**
** Limitations on Rights to Redistribute This Code
**
** Unicode, Inc. hereby grants the right to freely use the information
** supplied in this file in the creation of products supporting the
** Unicode Standard, and to make copies of this file in any form
** for internal or external distribution as long as this notice
** remains attached.
*/

typedef unsigned long   UTF32;  /* at least 32 bits */
typedef unsigned char   UTF8;   /* typically 8 bits */

#define UNI_REPLACEMENT_CHAR (UTF32)0x0000FFFD
#define UNI_MAX_LEGAL_UTF32 (UTF32)0x0010FFFF
#define UNI_SUR_HIGH_START  (UTF32)0xD800
#define UNI_SUR_HIGH_END    (UTF32)0xDBFF
#define UNI_SUR_LOW_START   (UTF32)0xDC00
#define UNI_SUR_LOW_END     (UTF32)0xDFFF

typedef enum {
        conversionOK,           /* conversion successful */
        sourceExhausted,        /* partial character in source, but hit end */
        targetExhausted,        /* insuff. room in target for conversion */
        sourceIllegal           /* source sequence is illegal/malformed */
} ConversionResult;

/*
 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
 * for *legal* UTF-8 will be 4 or fewer bytes total.
 */
static const UTF8 firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

static const char trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/*
 * Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence.
 */
static const UTF32 offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL,
                     0x03C82080UL, 0xFA082080UL, 0x82082080UL };

/*
 * Utility routine to tell whether a sequence of bytes is legal UTF-8.
 * This must be called with the length pre-determined by the first byte.
 * If not calling this from ConvertUTF8to*, then the length can be set by:
 *  length = trailingBytesForUTF8[*source]+1;
 * and the sequence is illegal right away if there aren't that many bytes
 * available.
 * If presented with a length > 4, this returns FALSE.  The Unicode
 * definition of UTF-8 goes up to 4-byte sequences.
 */

static bool isLegalUTF8(const UTF8 *source, int length) {
    UTF8 a;
    const UTF8 *srcptr = source+length;
    switch (length) {
    default: return FALSE;
        /* Everything else falls through when "true"... */
    case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return FALSE;
    case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return FALSE;
    case 2: if ((a = (*--srcptr)) > 0xBF) return FALSE;

        switch (*source) {
            /* no fall-through in this inner switch */
            case 0xE0: if (a < 0xA0) return FALSE; break;
            case 0xED: if (a > 0x9F) return FALSE; break;
            case 0xF0: if (a < 0x90) return FALSE; break;
            case 0xF4: if (a > 0x8F) return FALSE; break;
            default:   if (a < 0x80) return FALSE;
        }

    case 1: if (*source >= 0x80 && *source < 0xC2) return FALSE;
    }
    if (*source > 0xF4) return FALSE;
    return TRUE;
}

ConversionResult
CM_ConvertUTF8toUTF32(
	const UTF8** sourceStart, const UTF8* sourceEnd, 
	UTF32** targetStart, UTF32* targetEnd, i4 flags) 
{
    ConversionResult result = conversionOK;
    const UTF8* source = *sourceStart;
    UTF32* target = *targetStart;
    while (source < sourceEnd) {
	UTF32 ch = 0;
	unsigned short extraBytesToRead = trailingBytesForUTF8[*source]; 
	if (source + extraBytesToRead >= sourceEnd) {
	    result = sourceExhausted; break;
	}
	/* Do this check whether lenient or strict */
	if (! isLegalUTF8(source, extraBytesToRead+1)) {
	    result = sourceIllegal;
	    break;
	}
	/*
	 * The cases all fall through. See "Note A" below.
	 */
	switch (extraBytesToRead) {
	    case 5: ch += *source++; ch <<= 6;
	    case 4: ch += *source++; ch <<= 6;
	    case 3: ch += *source++; ch <<= 6;
	    case 2: ch += *source++; ch <<= 6;
	    case 1: ch += *source++; ch <<= 6;
	    case 0: ch += *source++;
	}
	ch -= offsetsFromUTF8[extraBytesToRead];

	if (target >= targetEnd) {
	    source -= (extraBytesToRead+1); /* Back up the source pointer! */
	    result = targetExhausted; break;
	}
	if (ch <= UNI_MAX_LEGAL_UTF32) {
	    /*
	     * UTF-16 surrogate values are illegal in UTF-32, and anything
	     * over Plane 17 (> 0x10FFFF) is illegal.
	     */
	    if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
		if (flags == 1) {
		    source -= (extraBytesToRead+1); /* return to the illegal value itself */
		    result = sourceIllegal;
		    break;
		} else {
		    *target++ = UNI_REPLACEMENT_CHAR;
		}
	    } else {
		*target++ = ch;
	    }
	} else { /* i.e., ch > UNI_MAX_LEGAL_UTF32 */
	    result = sourceIllegal;
	    *target++ = UNI_REPLACEMENT_CHAR;
	}
    }
    return result;
}

/* Name: CM_UTF8toUTF32 - Converts a UTF8 string to UTF32 for 
** lookup in the properties table.
** Input 
** 	instr - input UTF8 string.
**	incnt - byte length of the input string. 
** 	outval - output UTF32 value
** Output
**	Return OK   - If success.
**	       FAIL - if conversion failed.
** History
**	24-may-2007 (gupsh01)
**	    Added
*/
i4
CM_UTF8toUTF32 (char *instr, i4 incnt, u_i2 *outval)
{
        i4 	i, n;
        ConversionResult result;
        UTF32 	utf32_buf[2];
        UTF8 	utf8_buf[5];
        UTF32 	*utf8SourceStart;
        UTF8 	*utf32TargetStart;

        fflush(stdout);
        /*ILLEGAL       i = 0x10FFFF + 21;  */

        utf32_buf[0] = 0; utf32_buf[1] = 0;

        for (n = 0; n < incnt; n++) 
	  utf8_buf[n] = instr[n];

        utf8SourceStart = utf8_buf;
        utf32TargetStart = utf32_buf;

        result = CM_ConvertUTF8toUTF32((const UTF8 **) &utf8SourceStart,
                    &(utf8_buf[incnt]), (const UTF32 **) &utf32TargetStart,
                    &(utf32_buf[1]), 0);
        if (result != conversionOK) 
	{
	  *outval = 0;
	  return (FAIL);
        }
        *outval = utf32_buf[0]; 
	return (OK);
}

ConversionResult
CM_ConvertUTF32toUTF8 (
        const UTF32** sourceStart, const UTF32* sourceEnd,
        UTF8** targetStart, UTF8* targetEnd, i2 flags, i2 *rescnt) 
{
    i4 result = conversionOK;
    const UTF32* source = *sourceStart;
    UTF8* target = *targetStart;
    while (source < sourceEnd) {
        UTF32 ch;
        unsigned short bytesToWrite = 0;
        const UTF32 byteMask = 0xBF;
        const UTF32 byteMark = 0x80;
        ch = *source++;
        if (flags == 1) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                --source; /* return to the illegal value itself */
                result = sourceIllegal;
                break;
            }
        }
        /*
         * Figure out how many bytes the result will require. Turn any
         * illegally large UTF32 things (> Plane 17) into replacement chars.
         */
        if (ch < (UTF32)0x80) {      bytesToWrite = 1;
        } else if (ch < (UTF32)0x800) {     bytesToWrite = 2;
        } else if (ch < (UTF32)0x10000) {   bytesToWrite = 3;
        } else if (ch <= UNI_MAX_LEGAL_UTF32) {  bytesToWrite = 4;
        } else {                            bytesToWrite = 3;
                                            ch = UNI_REPLACEMENT_CHAR;
                                            result = sourceIllegal;
        }

        target += bytesToWrite;
        if (target > targetEnd) {
            --source; /* Back up source pointer! */
            target -= bytesToWrite; result = targetExhausted; break;
        }
        switch (bytesToWrite) { /* note: everything falls through. */
            case 4: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
            case 3: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
            case 2: *--target = (UTF8)((ch | byteMark) & byteMask); ch >>= 6;
            case 1: *--target = (UTF8) (ch | firstByteMark[bytesToWrite]);
        }
        target += bytesToWrite;
	*rescnt = bytesToWrite;
    }
    return result;
}

i4
CM_UTF32toUTF8 (int inval, char *resstr)
{
        i4 i, n;
        ConversionResult result;
        UTF32 utf32_buf[2];
        UTF8 utf8_buf[15];
        UTF32 *utf32SourceStart;
        UTF8 *utf8TargetStart;
        u_i2 rescnt = 0;

        fflush(stdout);
        /*ILLEGAL       i = 0x10FFFF + 21;  */

        utf32_buf[0] = inval; utf32_buf[1] = 0;
        for (n = 0; n < 8; n++) utf8_buf[n] = 0;

        utf32SourceStart = utf32_buf;
        utf8TargetStart = utf8_buf;

        result = CM_ConvertUTF32toUTF8((const UTF32 **) &utf32SourceStart,
                                     &(utf32_buf[1]), &utf8TargetStart,
                                     &(utf8_buf[14]),
                                     0, &rescnt);
        if (result != conversionOK) {
        fprintf(stderr,
            "CM_ConvertUTF32toUTF8 : fatal error: result %d for input %08x\n", result, utf32_buf[0]); exit(1);
        }
        for (i=0; i < rescnt; i++)
        {
          resstr[i] = utf8_buf[i];
        }
        return rescnt;
}
