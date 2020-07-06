/*
**    Copyright (c) 1987, 2000 Ingres Corporation
*/
/*
**	01-dec-2000	(kinte01)
**	    Bug 103393 - removed nat, longnat, u_nat, & u_longnat
**	    from VMS CL as the use is no longer allowed
*/
# include	<compat.h>
# include	<gl.h>
# include	<si.h>
# include	<ds.h>

GLOBALDEF readonly DSTEMPLATE	DSnat = {
	/* i4	ds_id	*/		DS_NAT,
	/* i4	ds_size	*/		sizeof (i4),
	/* i4	ds_cnt	*/		0,
	/* i4	dstemp_type */		DS_IN_CORE,
	/* DSPTRS *ds_ptrs */		0,
	/* char	*dstemp_file */		0,
	/* i4 	(*dstemp_func)() */	0
};
