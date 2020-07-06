# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      Copyright (c) 2004 Ingres Corporation
**      All rights reserved.
*/
/*
** includes and defines
*/
#include <compat.h>
#include <st.h>
#include <dbms.h>
#include <fe.h>
#include <cv.h>
#include "msgs.h"
#include "dba.h"
#define INTEGER_COLUMN 1
#define FLOAT_OR_MONEY_COLUMN 2
#define CHAR_COLUMN 3
/*
**
**	File:		tablefield_find
**	Routine:	tablefield_find
**
**	Author:		Tom Turchioe.  Relational Technology.  V1.0
**	Purpose:	This utility routine will unload a specified
**		        tablefield on a specified form searching a specified
**			column for an inputted value
**
**	Parameters:
**		Input:	none
**	       Output:  none
**	      Returns:  none
**
**	History
**		9/1/88 - (tomt) forward wrap around logic added
**	       9/15/88 - (tomt) search from current position logic added
**	      10/27/88 - (tomt) added memory for last value entered
**	       3/14/89 - (tomt) converted for use with R6.
**	      10/01/89 - (tomt) use CL routines.
**	26-Aug-1993 (fredv)
**		Included <st.h>.
**	27-aug-1997 (walro03)
**		Correct compile errors when CVaf was called with "." which
**		should be '.'
**	       
*/
VOID
tablefield_find()
{
  char form[FE_MAXNAME + 1], tablefield[FE_MAXNAME + 1], col[FE_MAXNAME + 1];
  char prompt_value[40], column_content[40], msg[80];
  static char old_prompt_value[40];
  int row_num, field_type;
  bool success, isatblfld;
  STATUS status;
  int int_prompt_value, int_column_content;
  int cursor_row, numrows;
  bool around_once, finished_search;
  float float_column_content, float_prompt_value;
    success = FALSE;		/* SET TO FALSE so logic works */
    finished_search = FALSE;	/* used for wrap around search */
    around_once = FALSE;	/* used for wrap around search */
    /* where am I */
/* # line 67 "tblfldfn.sc" */	/* inquire_frs */
  {
    if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,form,19,(char *)0,0);
    } /* IIiqset */
  }
/* # line 68 "tblfldfn.sc" */	/* inquire_frs */
  {
    if (IIiqset(2,0,form,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,tablefield,19,(char *)0,0);
      IIiqfsio((short *)0,1,30,1,&isatblfld,28,(char *)0,0);
    } /* IIiqset */
  }
/* # line 70 "tblfldfn.sc" */	/* host code */
    if (isatblfld == FALSE) 
    {		
	/* are we on a tablefield */
/* # line 73 "tblfldfn.sc" */	/* message */
  {
    IImessage((char *)"You are not on a scrolling area...");
  }
/* # line 74 "tblfldfn.sc" */	/* sleep */
  {
    IIsleep(2);
  }
/* # line 75 "tblfldfn.sc" */	/* host code */
	return;	/* not on a tablefield.  routine works on tblfld only */
    }
    /* what row am I on? */
/* # line 78 "tblfldfn.sc" */	/* getrow */
  {
    if (IItbsetio(2,form,tablefield,-2) != 0) {
      IItcogetio((short *)0,1,30,4,&cursor_row,(char *)"_RECORD");
      IITBceColEnd();
    } /* IItbsetio */
  }
/* # line 79 "tblfldfn.sc" */	/* inquire_frs */
  {
    if (IIiqset(4,0,form,tablefield,(char *)0) != 0) {
      IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,col,19,(char *)0,0);
      IIiqfsio((short *)0,1,30,4,&field_type,25,col,0);
    } /* IIiqset */
  }
/* # line 81 "tblfldfn.sc" */	/* host code */
    /* Make sure that tablefield is NOT empty */
/* # line 82 "tblfldfn.sc" */	/* inquire_frs */
  {
    if (IIiqset(3,0,form,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&numrows,32,tablefield,0);
    } /* IIiqset */
  }
/* # line 83 "tblfldfn.sc" */	/* host code */
    if (numrows == 0) 
    {
/* # line 85 "tblfldfn.sc" */	/* message */
  {
    IImessage((char *)"Nothing to find, the scrolling area is empty");
  }
/* # line 86 "tblfldfn.sc" */	/* sleep */
  {
    IIsleep(2);
  }
/* # line 87 "tblfldfn.sc" */	/* host code */
	return;
    }
/* Get Value from user.  Step through table field to find match. */
/* # line 90 "tblfldfn.sc" */	/* prompt */
  {
    IIprmptio(0,(char *)"Enter string to find: ",(short *)0,1,32,39,
    prompt_value);
  }
/* # line 91 "tblfldfn.sc" */	/* host code */
    if (STlength(prompt_value) == 0) 
    {
	return;		/* hitting <CR> means don't look */
    } 
    else if (prompt_value[0] == '.') 
    {    
	/* use last value */
	if (STlength(old_prompt_value) == 0) 
	{  
	    /* no previous string entered */
/* # line 101 "tblfldfn.sc" */	/* message */
  {
    IImessage((char *)"No previous string entered...");
  }
/* # line 102 "tblfldfn.sc" */	/* sleep */
  {
    IIsleep(2);
  }
/* # line 103 "tblfldfn.sc" */	/* host code */
	    return;
	}
	STcopy(old_prompt_value, prompt_value);
    } 
    else 
    {				     
	/* save original for later */
	STcopy(prompt_value, old_prompt_value);  
    }
    /*
    **  Convert the inputted string to the datatype of the field that the
    **  cursor is on.
    */
    switch (field_type) 
    {
	case INTEGER_COLUMN:
	    status = CVan(prompt_value, &int_prompt_value); /* cvt to int */
	    if (status != OK)
	    {
/* # line 122 "tblfldfn.sc" */	/* message */
  {
    IImessage((char *)
"This is a numeric field... Please enter 0 - 9 only...");
  }
/* # line 124 "tblfldfn.sc" */	/* sleep */
  {
    IIsleep(2);
  }
/* # line 125 "tblfldfn.sc" */	/* host code */
	        return;		/* error in input... make user redo */
	    }
	    break;
	case FLOAT_OR_MONEY_COLUMN:
	    /* convert to an float */
	    status = CVaf( prompt_value, '.', &float_prompt_value);
	    if (status != OK)
	    {
		POP_MSG("This is a numeric field.  Please enter 0 - 9\
 and \".\" only");
		return;		/* error in input... make user redo */
	    }
	    break;
	case CHAR_COLUMN:
	    CVupper(prompt_value);	/* uppercase */
	    break;			/* do nothing, get out */
	default:
	    POP_MSG("Internal error: undetermined datatype (FIND)");
            return;	/* error: don't go any furthur: return */
    }
    /*
    **  Depending upon datatype, different unloadtable loops must be used.
    **  If an integer column, then unloadtable must have integer variable.
    **  Likewise for float.
    **  If character, unloadtable must unload into a string variable.
    **
    **  Perform wrap around search.  Look from current point forward.  If we
    **  get to end of tblfld data set, we start from the beginning (repeat the
    **  while loop) until we get to the current position.
    */
    while (!finished_search) 
    {
	switch (field_type) 
	{
	    case INTEGER_COLUMN:
/* # line 160 "tblfldfn.sc" */	/* unloadtable */
  {
    if (IItbact(form,tablefield,0) == 0) goto IItbE1;
    while (IItunload() != 0) {
      IItcogetio((short *)0,1,30,4,&row_num,(char *)"_RECORD");
      IItcogetio((short *)0,1,30,4,&int_column_content,col);
      IITBceColEnd();
/* # line 163 "tblfldfn.sc" */	/* host code */
		    if (int_column_content == int_prompt_value) 
		    { 
			/* there is a match */
			if (around_once && (row_num < cursor_row)) 
			{
			    finished_search = TRUE;
			    success = TRUE;
			} 
			else if (!around_once && row_num == cursor_row) 
			{
			    finished_search = FALSE;
			    success = TRUE;
			} 
			else if (!around_once && (row_num > cursor_row)) 
			{
			    finished_search = TRUE;
			    success = TRUE;
			}
			if (success && finished_search) 
			{
/* # line 183 "tblfldfn.sc" */	/* endloop */
      {
        if (1) goto IItbE1;
      }
/* # line 184 "tblfldfn.sc" */	/* host code */
			}
		    }
		    /* must break out at bottom or else we end on next row */
		    if ((around_once && (row_num >= cursor_row)) 
		      || (success && finished_search)) 
		    {
			finished_search = TRUE;	/* once around is enough */
/* # line 191 "tblfldfn.sc" */	/* endloop */
      {
        if (1) goto IItbE1;
      }
/* # line 192 "tblfldfn.sc" */	/* host code */
		    }
    } /* IItunload */
IItbE1:
    IItunend();
  }
		break;
	    case FLOAT_OR_MONEY_COLUMN:
/* # line 196 "tblfldfn.sc" */	/* unloadtable */
  {
    if (IItbact(form,tablefield,0) == 0) goto IItbE2;
    while (IItunload() != 0) {
      IItcogetio((short *)0,1,30,4,&row_num,(char *)"_RECORD");
      IItcogetio((short *)0,1,31,4,&float_column_content,col);
      IITBceColEnd();
/* # line 199 "tblfldfn.sc" */	/* host code */
		    if (float_column_content == float_prompt_value)
		    {
			if (around_once && (row_num < cursor_row))
			{
			    finished_search = TRUE;
			    success = TRUE;
			} 
			else if (!around_once && row_num == cursor_row) 
			{
			    finished_search = FALSE;
			    success = TRUE;
			} 
			else if (!around_once && (row_num > cursor_row)) 
			{
			    finished_search = TRUE;
			    success = TRUE;
			}
			if (success && finished_search) 
			{
/* # line 218 "tblfldfn.sc" */	/* endloop */
      {
        if (1) goto IItbE2;
      }
/* # line 219 "tblfldfn.sc" */	/* host code */
			}
		    }
		    /* must break out at bottom or else we end on next row */
		    if ((around_once && (row_num >= cursor_row)) 
		      || (success && finished_search)) 
		    {
			finished_search = TRUE;	/* once around is enough */
/* # line 226 "tblfldfn.sc" */	/* endloop */
      {
        if (1) goto IItbE2;
      }
/* # line 227 "tblfldfn.sc" */	/* host code */
		    }
    } /* IItunload */
IItbE2:
    IItunend();
  }
		break;
	    case CHAR_COLUMN:
/* # line 231 "tblfldfn.sc" */	/* unloadtable */
  {
    if (IItbact(form,tablefield,0) == 0) goto IItbE3;
    while (IItunload() != 0) {
      IItcogetio((short *)0,1,30,4,&row_num,(char *)"_RECORD");
      IItcogetio((short *)0,1,32,39,column_content,col);
      IITBceColEnd();
/* # line 234 "tblfldfn.sc" */	/* host code */
		    CVupper(column_content);		/* uppercase */
		    if (isamatch(prompt_value, column_content) == 0) 
		    {
			if (around_once && (row_num < cursor_row)) 
			{
			    finished_search = TRUE;
			    success = TRUE;
			} 
			else if (!around_once && row_num == cursor_row) 
			{
			    finished_search = FALSE;
			    success = TRUE;
			} 
			else if (!around_once && (row_num > cursor_row)) 
			{
			    finished_search = TRUE;
			    success = TRUE;
			}
			if (success && finished_search) 
			{
/* # line 254 "tblfldfn.sc" */	/* endloop */
      {
        if (1) goto IItbE3;
      }
/* # line 255 "tblfldfn.sc" */	/* host code */
			}
		    }
		    /* must break out at bottom or else we end on next row */
		    if ((around_once && (row_num >= cursor_row)) 
		      || (success && finished_search)) 
		    {
			finished_search = TRUE;	/* once around is enough */
/* # line 262 "tblfldfn.sc" */	/* endloop */
      {
        if (1) goto IItbE3;
      }
/* # line 263 "tblfldfn.sc" */	/* host code */
		    }
    } /* IItunload */
IItbE3:
    IItunend();
  }
		break;
	}
	around_once = TRUE;
    }	/* end of WHILE loop */
    /*
    **  If no match was found, tell user.  If one was, scroll to it.
    */
    if (success != TRUE) 
    {
	STprintf(msg, "%s not found.", old_prompt_value);
	POP_MSG(msg);
    } 
    else 
    {			
	/* success: scroll to matched row */
/* # line 280 "tblfldfn.sc" */	/* scroll */
  {
    if (IItbsetio(1,form,tablefield,-3) != 0) {
      IItbsmode((char *)"to");
      if (IItscroll(0,row_num) != 0) {
      } /* IItscroll */
    } /* IItbsetio */
  }
/* # line 281 "tblfldfn.sc" */	/* host code */
    }
    return;
}
/*
** isamatch()
**
** find occurance of substr in targstr.
** This routine will exit when the length of the substring is
** exceeded or when a mismatch is found.  If a mismatch is found, routine
** will return failure.  If the routine exits because of end of substr, routine
** will return success because all preceding characters matched in order to
** reach the end of string.
**
** Returns:
**	OK if success
**	FAIL if failure
**
*/
isamatch (substr, targstr)
char	*substr, *targstr;
{
    /*
    **  Variables
    */
    bool	match_found;
    char	*substr_save;	/* used to remember starting position */
    /*
    **  Initialize
    */
    match_found = FALSE;	/* indicates if there was a character match */
    substr_save = substr;	/* remember starting position */
    /*
    **  Determine if target string is null.  If it is, return FAIL.
    */
    if (STlength( targstr) == 0)
	return( FAIL);
    /*
    **  Perform essentially what is a substring function.  If the substring
    **  occurs anywhere in the target string, OK is returned.
    */
    while (*substr != '\0' && *targstr != '\0') 
    {
	if (*substr != *targstr) 
	{	
	    /* no match at current position */
	    match_found = FALSE;
	    substr = substr_save;	/* start at beginning of substr */
	    targstr++;			/* walk to next position in targstr */
	} 
	else 
	{			
	    /* found a match */
	    substr++;			/* walk to next character in */
	    targstr++;			/*   both strings */
	    match_found = TRUE;		/* indicate a character match */
	}
    }
    /*
    **  Hack - In case the target string end was encountered while checking
    **  the substring against it, we check to ensure that the ENTIRE substring
    **  was matched and not part of it.  If the entire substring was matched, 
    **  current character in the substring should be the null.  As a safety, we
    **  check to see that a successful match was the last thing to occur.
    */
    if (match_found && (*substr != '\0'))
	return( FAIL);
    if (match_found)
	return( OK);			/* match found - OK */
    return( FAIL);			/* else return fail */
}
