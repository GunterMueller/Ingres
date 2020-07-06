/*
** Copyright (c) 2004 Ingres Corporation
*/

/**
** Name: ADU1DATES.H - define vector for time components.
**
** Description:
**        This file defines a vector for time components.
**
** History: $Log-for RCS$
**      14-apr-86 (ericj)
**	    Adapted from 4.0/02 code for Jupiter.
**	09-feb-1998 (hanch04)
**	    Added DV_YEAR0 to support years 0000-9999
**	17-jan-02 (inkdo01)
**	    Changed constants to hex as they were clearly intended 
**	    to be.
**	17-jul-2006 (gupsh01)
**	    Modified the DV_MILLISECOND TO DV_NANOSECOND.
**	22-nov-2006 (gupsh01)
**	    Added DV_YEARLT4.
**/

#define    DV_ABSOLUTE 0x01
#define    DV_LENGTH   0x02
#define    DV_OFFSET   0x03

#define    DV_YEAR     0x01
#define    DV_MONTH    0x02
#define    DV_WEEK     0x04
#define    DV_DAY      0x10
#define    DV_DOW      0x20
#define    DV_HOUR     0x40
#define    DV_MINUTE   0x0100
#define    DV_SECOND   0x0200
#define    DV_NANOSECOND  0x0400
#define    DV_AMPM     0x1000
#define    DV_TIMEZONE 0x2000
#define    DV_YEAR0    0x4000
#define    DV_YEARLT4  0x8000

typedef struct
{
    i4  dv_year;
    i4  dv_month;
    i4  dv_week;
    i4  dv_day;
    i4  dv_hour;
    i4  dv_minute;
    i4  dv_second;
    i4  dv_nanosecond;
    i2  dv_dow;
    i2  dv_ampm;
    i2  dv_timezone;
    i2  dv_status;
    i2  dv_datetype;
} DATEVECT;
