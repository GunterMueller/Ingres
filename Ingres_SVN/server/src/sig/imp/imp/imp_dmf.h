# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name: imp_dmf.incl
**
## History:
##      02-Jul-2002 (fanra01)
##          Sir 108159
##          Added to distribution in the sig directory.
##      06-Apr-2004 (srisu02)
##          Sir 108159
##	    Add 3 wait columns to the buffer structure
##          Integrated changes made by flomi02
*/
/* Description of table imp_dmf_cache_stats from database imadmf */
/* # line 56 "imp_dmf.incl" */	/* host code */
  struct dmfbuf_ {
	char	server[65];
	long	page_size;
	long	force_count;
	long	io_wait_count;
	long	group_buffer_read_count;
	long	group_buffer_write_count;
	long	fix_count;
	long	unfix_count;
	long	read_count;
	long	write_count;
	long	hit_count;
	long	dirty_unfix_count;
	long	pages_still_valid;
	long	pages_invalid;
	long	buffer_count;
	long	page_buffer_count;
	long	flimit;
	long	mlimit;
	long	wbstart;
	long	wbend;
	long	hash_bucket_count;
	long	group_buffer_count;
	long	group_buffer_size;
	long	cache_status;
	long	free_buffer_count;
	long	free_buffer_waiters;
	long	fixed_buffer_count;
	long	modified_buffer_count;
	long	free_group_buffer_count;
	long	fixed_group_buffer_count;
	long	modified_group_buffer_count;
        long    fc_wait;
        long    bm_gwait ;
        long    bm_mwait ;
  } dmfbuf;
