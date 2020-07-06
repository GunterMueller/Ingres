/* Description of table iiprocedure from database iidbdb */
  EXEC SQL DECLARE iiprocedure TABLE
	(dbp_name	char(32) not null,
	 dbp_owner	char(32) not null,
	 dbp_txtlen	integer not null,
	 dbp_txtid1	integer not null,
	 dbp_txtid2	integer not null,
	 dbp_mask1	integer not null,
	 dbp_mask2	integer not null,
	 dbp_id	integer not null,
	 dbp_idx	integer not null,
	 dbp_parameter_count	integer not null,
	 dbp_record_width	integer not null,
	 dbp_ubt_id	integer not null,
	 dbp_ubt_idx	integer not null,
	 dbp_create_date	integer not null,
	 dbp_free	char(16) not null);

  struct proc_tbl_ {
	char	dbp_name[33];
	char	dbp_owner[33];
	i4	dbp_txtlen;
	i4	dbp_txtid1;
	i4	dbp_txtid2;
	i4	dbp_mask1;
	i4	dbp_mask2;
	i4	dbp_id;
	i4	dbp_idx;
	i4	dbp_parameter_count;
	i4	dbp_record_width;
	i4	dbp_ubt_id;
	i4	dbp_ubt_idx;
	i4	dbp_create_date;
	char	dbp_free[17];
  } proc_tbl;
