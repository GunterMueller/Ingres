/*****************************************************************************************
//                                                                                      //
//  Copyright (C) 2005-2006 Ingres Corporation. All Rights Reserved.		        //
//                                                                                      //
//    Source   : CellIntC.cpp, implementation file                                      //
//                                                                                      //
//                                                                                      //
//    Project  : CA-OpenIngres/Visual DBA.                                              //
//    Author   : Emmanuel Blattes                                                       //			
//                                                                                      //
//                                                                                      //
//    Purpose  : C interface for Cellular Control to display page usage of Ingres Table //
//                                                                                      //
*****************************************************************************************/

#include "stdafx.h"
#include "cellintc.h"

BYTE ConvertPageCharToByte(char ch)
{
  switch (ch) {
    case 'H': return (BYTE) PAGE_HEADER;
    case 'M': return (BYTE) PAGE_MAP;
    case 'F': return (BYTE) PAGE_FREE;
    case 'r': return (BYTE) PAGE_ROOT;
    case 'i': return (BYTE) PAGE_INDEX;
    case 's': return (BYTE) PAGE_SPRIG;
    case 'L': return (BYTE) PAGE_LEAF;
    case 'O': return (BYTE) PAGE_OVERFLOW_LEAF;
    case 'o': return (BYTE) PAGE_OVERFLOW_DATA;
    case 'A': return (BYTE) PAGE_ASSOCIATED;
    case 'D': return (BYTE) PAGE_DATA;
    case 'U': return (BYTE) PAGE_UNUSED;
    // hash specific section (ingres >= 2.5)
    case 'd': return (BYTE) PAGE_EMPTY_DATA_NO_OVF;
    case 'e': return (BYTE) PAGE_EMPTY_OVERFLOW;
    case 'x': return (BYTE) PAGE_DATA_WITH_OVF;
    case 'y': return (BYTE) PAGE_EMPTY_DATA_WITH_OVF;
    // end of hash specific section (ingres >=2.5)
    default:
      ASSERT (FALSE);
      return (BYTE)PAGE_ERROR_UNKNOWN;
  }
}

static BOOL IsPageInUse(BYTE by)
{
  pageType type = (pageType)by;
  switch (type) {
    case PAGE_FREE:
    case PAGE_UNUSED:
      return FALSE;
    default:
      return TRUE;
  }
  ASSERT (FALSE);
  return FALSE;
}

static BOOL IsOverflowPage(BYTE by)
{
  pageType type = (pageType)by;
  switch (type) {
    case PAGE_OVERFLOW_LEAF:
    case PAGE_OVERFLOW_DATA:
    // hash specific section (ingres >= 2.5)
    case PAGE_EMPTY_OVERFLOW:
//    case PAGE_DATA_WITH_OVF:
//    case PAGE_EMPTY_DATA_WITH_OVF:
      return TRUE;
    default:
      return FALSE;
  }
  ASSERT (FALSE);
  return FALSE;
}

int GetFillFactor(BYTE by, int itemsPerByte)
{
  if (itemsPerByte > 1) {
    return by & 0x3F;   // 6 low-order bits
  }
  else {
    if (IsPageInUse(by))
      return (64-1);
    else
      return 0;
  }
}

int GetOverflowFactor(BYTE by, int itemsPerByte)
{
  if (itemsPerByte > 1) {
    return by >> 6 & 0x3;   // 2 hi-order bits
  }
  else {
    if (IsOverflowPage(by))
      return 3;
    else
      return 0;
  }
}

