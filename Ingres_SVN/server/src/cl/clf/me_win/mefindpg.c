/******************************************************************************
**
** Copyright (c) 1988, 2001 Ingres Corporation
**
******************************************************************************/

#include <compat.h>
#include <clconfig.h>
#include <meprivate.h>
#include <me.h>

/******************************************************************************
**
**  Name: MEFINDPAGES.ROC - Find a contigious region of free memory
**
**  Description:
**	This file contains a table used to aid searching for contigious
**	cleared bits.
**
LIBRARY = IMPCOMPATLIBDATA
**
**  History:
**	06-aug-2001 (somsa01)
**	    Use meprivate.h instead of melocal.h .
**
******************************************************************************/

GLOBALDEF READONLY i2 MEconttab[] =
{
	 /* 00 */ 02000, 00770, 00661, 00660, 00552, 00550, 00551, 00550,
	 /* 01 */ 00443, 00440, 00441, 00440, 00442, 00440, 00441, 00440,
	 /* 02 */ 00344, 00330, 00331, 00330, 00332, 00330, 00331, 00330,
	 /* 03 */ 00333, 00330, 00331, 00330, 00332, 00330, 00331, 00330,
	 /* 04 */ 00255, 00240, 00231, 00230, 00222, 00220, 00221, 00220,
	 /* 05 */ 00233, 00220, 00221, 00220, 00222, 00220, 00221, 00220,
	 /* 06 */ 00244, 00230, 00221, 00220, 00222, 00220, 00221, 00220,
	 /* 07 */ 00233, 00220, 00221, 00220, 00222, 00220, 00221, 00220,
	 /* 08 */ 00166, 00150, 00141, 00140, 00132, 00130, 00131, 00130,
	 /* 09 */ 00133, 00120, 00121, 00120, 00122, 00120, 00121, 00120,
	 /* 0a */ 00144, 00130, 00121, 00120, 00122, 00110, 00111, 00110,
	 /* 0b */ 00133, 00120, 00111, 00110, 00122, 00110, 00111, 00110,
	 /* 0c */ 00155, 00140, 00131, 00130, 00122, 00120, 00121, 00120,
	 /* 0d */ 00133, 00120, 00111, 00110, 00122, 00110, 00111, 00110,
	 /* 0e */ 00144, 00130, 00121, 00120, 00122, 00110, 00111, 00110,
	 /* 0f */ 00133, 00120, 00111, 00110, 00122, 00110, 00111, 00110,
	 /* 10 */ 00077, 00060, 00051, 00050, 00042, 00040, 00041, 00040,
	 /* 11 */ 00033, 00030, 00031, 00030, 00032, 00030, 00031, 00030,
	 /* 12 */ 00044, 00030, 00021, 00020, 00022, 00020, 00021, 00020,
	 /* 13 */ 00033, 00020, 00021, 00020, 00022, 00020, 00021, 00020,
	 /* 14 */ 00055, 00040, 00031, 00030, 00022, 00020, 00021, 00020,
	 /* 15 */ 00033, 00020, 00011, 00010, 00022, 00010, 00011, 00010,
	 /* 16 */ 00044, 00030, 00021, 00020, 00022, 00010, 00011, 00010,
	 /* 17 */ 00033, 00020, 00011, 00010, 00022, 00010, 00011, 00010,
	 /* 18 */ 00066, 00050, 00041, 00040, 00032, 00030, 00031, 00030,
	 /* 19 */ 00033, 00020, 00021, 00020, 00022, 00020, 00021, 00020,
	 /* 1a */ 00044, 00030, 00021, 00020, 00022, 00010, 00011, 00010,
	 /* 1b */ 00033, 00020, 00011, 00010, 00022, 00010, 00011, 00010,
	 /* 1c */ 00055, 00040, 00031, 00030, 00022, 00020, 00021, 00020,
	 /* 1d */ 00033, 00020, 00011, 00010, 00022, 00010, 00011, 00010,
	 /* 1e */ 00044, 00030, 00021, 00020, 00022, 00010, 00011, 00010,
	 /* 1f */ 00033, 00020, 00011, 00010, 00022, 00010, 00011, 01000
};