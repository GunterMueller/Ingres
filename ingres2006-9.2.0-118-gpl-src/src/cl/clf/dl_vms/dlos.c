/*
** Copyright 1991, 2002 Ingres Corporation, Inc
**
*/
# include <compat.h>
# include <gl.h>
# include <descrip.h> 
# include <ssdef.h>
# include <lnmdef.h>
# include <libdef.h>
# include <cm.h>
# include <cv.h>
# include <er.h>
# include <lo.h>
# include <nm.h>
# include <si.h>
# include <st.h>

# include <dl.h>
# include "dlint.h"

/* Itemlist structure for system service calls */
typedef struct itmlist
{
	i2 buflen;
	i2 itmcode;
	PTR bufadr;
	i4 retadr;
} ITEMLIST;

#define PROCLNT		ERx("LNM$PROCESS_TABLE")
#define DEFAULTDIR	ERx("SYS$DISK:[]")

/*
** Check whether the error is LIB$_KEYNOTFOU, i.e. that we couldn't find the
** lookup function.
*/
static bool
is_err_keynotfou(err)
i4 err;
{
	return LIB$MATCH_COND(&err, &LIB$_KEYNOTFOU) != 0;
}

/*
** Condition handler that handles everything.  If this is an unexpected error
** (i.e. if it isn't LIB$_KEYNOTFOU, which means that we couldn't find the
** lookup function) it sends the error text to SYS$ERROR.
*/
static i4
handle_all(sig, mech)
i4 sig[];
i4 mech[];
{
	if (!is_err_keynotfou(sig[1]))
	{
		/*
		** We have to adjust the count of longwords which follow to 
		** omit the PC and PSL; otherwise, they'll be translated as 
		** message codes.
		*/
		sig[0] -= 2;
		sys$putmsg(sig);
		sig[0] += 2;
	}
	return SS$_CONTINUE;
}

/*
** Map and load the image using the supplied logical name and function string.
*/
STATUS find_symbol (lookupname, d_logname, d_lookupfcn, lookupfcn)
char *lookupname;
struct dsc$descriptor_s *d_logname;
struct dsc$descriptor_s *d_lookupfcn;
i4 *lookupfcn; 
{
    i4 status;

    LIB$ESTABLISH(handle_all);		

    CVupper(lookupname);
    d_lookupfcn->dsc$w_length = STlength(lookupname);
    status = LIB$FIND_IMAGE_SYMBOL(
    d_logname, d_lookupfcn, lookupfcn, NULL);
    if ((status&1) != 1)
    {
        if (! (is_err_keynotfou(status) && lookupname == NULL))
            return DL_OSLOAD_FAIL;
    }
    LIB$REVERT(); 
    return(OK);
}

/*
** Name:	DLosprepare - Load and map the DL module
**
** Description:
**	Given location, which we KNOW exists, or a logical name pointing
**      to the DL module, load into memory using
**	OS-specific DL mechanims.
**
** Inputs:
**	locp		Location of library/module to be attached to.  May
**                      be NULL.
**	syms		SYMBOL structs specifying names to be found in
**			attached file;
** Outputs:
**	dlhandle	May contain the logical name of the shared image
**                      and function mapping information.
**	errp		CL_ERR_DESC that will be filled in on error.
** Returns:
**	OK if found some file with that name was found, non-OK otherwise.
** Side effects:
**	Loads the DL module into memory.
**
** History:
**	4/92 (Mike S) Make VMS handler send message to SYS$ERROR when
**		      shareable image can't be loaded.  Otherise, it's
**		      impossible to tell what the problem was.
**      16-jul-93 (ed)
**	    added gl.h
**	26-oct-01 (kinte01)
**	    clean up compiler warnings.
**      08-Mar-02 (loera01)
**          Revamped for VMS.  This may now be called from DLbind as well
**          as DLprepare_loc.
**	10-oct-2002 (abbjo03)
**	    Fix assignment in find_symbol to use pointer notation.
*/
STATUS
DLosprepare(locp, syms, errp, dlhandle)
LOCATION *locp;
char *syms[];
CL_ERR_DESC *errp;
DLhandle *dlhandle;
{
	char *shlibname;
        i4 lookupfcn;
        STATUS ret;

	LOtos(locp, &shlibname);

	/*  We need to do the following:
	**	1. Convert what we were given to a full pathname.
	**  	2. Define a user-level logical name to point to the full
	**	   pathname.
        **      3. Map the symbol.
	*/
        {
		char fullshlibname[MAX_LOC+1];
		$DESCRIPTOR(d_fullshlibname, fullshlibname);
		$DESCRIPTOR(d_shlibname, shlibname);
		$DESCRIPTOR(d_default, DEFAULTDIR);
		$DESCRIPTOR(d_logname, dlhandle->logname); 
	    	$DESCRIPTOR(d_proclnt, PROCLNT);
		char lookupname[33];
		$DESCRIPTOR(d_lookupfcn, lookupname);
		i4 status;
		i4 context = 0;
		ITEMLIST itemlist[2];

                /*
                ** If a previous call to DLprepare or DLprepare_loc
                ** already assigned a logical to the DL module, just map
                ** the symbol without re-assigning.
                */
                if (NMgetenv(dlhandle->logname) != NULL) 
                {
		    STcopy(syms[0], lookupname);
  		    d_logname.dsc$w_length = STlength(dlhandle->logname);
                    ret = find_symbol(lookupname, &d_logname, &d_lookupfcn,
                        &lookupfcn);
                    if ( ret == OK )
                    {
                        dlhandle->func = lookupfcn;
                        if (dlhandle->sym != NULL)
                            MEfree(dlhandle->sym);
                        dlhandle->sym = STalloc(syms[0]);
	                DLdebug("Successfully loaded %s\n", shlibname);
                    }
                    return ret;
                }

		/* 
                ** Expand the library to a full pathname.
                */
		d_shlibname.dsc$w_length = STlength(shlibname);		
		status = LIB$FIND_FILE(&d_shlibname, &d_fullshlibname,
				       &context, &d_default);
		(VOID)LIB$FIND_FILE_END(&context);

		if ((status&1) != 1)
			return DL_MOD_NOT_FOUND;
		fullshlibname[MAX_LOC] = EOS;
		STtrmwhite(fullshlibname);
		d_fullshlibname.dsc$w_length = STlength(fullshlibname);		
				
		/* 
                ** Make the logical name.
                */
		itemlist[0].buflen = d_fullshlibname.dsc$w_length;
		itemlist[0].bufadr = d_fullshlibname.dsc$a_pointer;
		itemlist[0].retadr = (i4)d_fullshlibname.dsc$a_pointer;
		itemlist[0].itmcode = LNM$_STRING;
		itemlist[1].buflen = itemlist[1].itmcode = 0;

		d_logname.dsc$w_length = STlength(dlhandle->logname);		
		status = SYS$CRELNM(0, &d_proclnt, &d_logname, 0, itemlist);

		/* This shouldn't fail, ever */
		if ((status&1) != 1)
			return DL_OSLOAD_FAIL;
		STcopy(syms[0], lookupname);

                /*
                ** Map and load.
                */
                ret = find_symbol(lookupname, &d_logname, &d_lookupfcn,
                    &lookupfcn);
                if ( ret == OK )
                {
                        dlhandle->func = lookupfcn;
                        if (dlhandle->sym != NULL)
                            MEfree(dlhandle->sym);
                        dlhandle->sym = STalloc(syms[0]);
              	        DLdebug("Successfully loaded %s\n", shlibname);
                }
                return ret;
        }
}
