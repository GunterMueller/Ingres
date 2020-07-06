
/*
**Copyright (c) 1989, 2004 Ingres Corporation
*/

#include	<compat.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<fdesc.h>
#include	<abfrts.h>

/**
** Name:	ilsystab.roc -	Application Run-Time System Routines Table.
**
** Description:
**	Contains the interpreter application system function routines table.
**	These tables map system function names to the ABF run-time routines
**	that implement their functionality.
**
**	Note:  The internal system function component objects must be defined
**	in "iaom!iamsystm.c" while the ABF run-time routines are defined in the
**	"abfrt" directory.
**
**	Defines:
**
**	iiITsyProcs[]		application system procedures function table.
**	iiITsyFrames[]		application system frames function table.
**
** History:
**	Revision 6.4
**	03/13/91 (emerson)
**		Added commandlineparameters to iiITsyProcs.
**
**	Revision 6.3/03/00  89/07  wong
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/

/*:
** Name:	iiITsyProcs[] -	ABF System Procedures Function Tables.
**
** Description:
**	Application system procedures function table used by the interpreter.
**	This links and maps the system procedures into the interpreter.
**
**	Note:  System procedures must also be defined in "iaom!iamsystm.c".
**
** History:
**	07/89 (jhw) -- Written.
**	03/13/91 (emerson)
**		Added commandlineparameters (for Topaz release).
**	17-jun-93 (essi)
**		Added support for External File I/O.
*/

i4	IIARsequence_value();
i4	IIARfnd_row();
i4	IIARbeep();
i4	IIARhelp();
i4	IIARhlpMenu();
i4	IIARfieldHelp();
i4	IIARgtFrmName();

i4	IIARaxAllRows();
i4	IIARaxLastRow();
i4	IIARaxFirstRow();
i4	IIARaxInsertRow();
i4	IIARaxSetRowDeleted();
i4	IIARaxRemoveRow();
i4	IIARaxClear();

i4	IIARclpCmdLineParms();

i4	IIARofOpenFile();
i4	IIARwfWriteFile();
i4	IIARrfReadFile();
i4	IIARcfCloseFile();
i4	IIARfrRewindFile();
i4	IIARpfPositionFile();
i4	IIARffFlushFile();
i4	IIARifInquireFile();

GLOBALDEF ABRTSFUNCS	iiITsyProcs[] =
{
	{ERx("sequence_value"), IIARsequence_value},
	{ERx("find_record"), IIARfnd_row},
	{ERx("beep"), IIARbeep},
	{ERx("help"), IIARhelp},
	{ERx("help_menu"), IIARhlpMenu},
	{ERx("help_field"), IIARfieldHelp},
	{ERx("ii_frame_name"), IIARgtFrmName},
	{ERx("arrayallrows"), IIARaxAllRows},
	{ERx("arraylastrow"), IIARaxLastRow},
	{ERx("arrayfirstrow"), IIARaxFirstRow},
	{ERx("arrayinsertrow"), IIARaxInsertRow},
	{ERx("arraysetrowdeleted"),IIARaxSetRowDeleted},
	{ERx("arrayremoverow"), IIARaxRemoveRow},
	{ERx("arrayclear"), IIARaxClear},
	{ERx("commandlineparameters"), IIARclpCmdLineParms},
	{ERx("openfile"), IIARofOpenFile},
	{ERx("writefile"), IIARwfWriteFile},
	{ERx("readfile"), IIARrfReadFile},
	{ERx("closefile"), IIARcfCloseFile},
	{ERx("rewindfile"), IIARfrRewindFile},
	{ERx("positionfile"), IIARpfPositionFile},
	{ERx("flushfile"), IIARffFlushFile},
	{ERx("inquirefile"), IIARifInquireFile},
	NULL
};

/*:
** Name:	iiITsyFrames[] -	ABF System Frames Function Table.
**
** Description:
**	Application system frames function table used by the interpreter.
**	This links and maps the system frames into the interpreter.
**
**	Note:  System frames must also be defined in "iaom!iamsystm.c".
**
** History:
**	07/89 (jhw) -- Written.
*/

i4	IIARlookup();

GLOBALDEF ABRTSFUNCS	iiITsyFrames[] =
{
	{ERx("look_up"), IIARlookup},
	NULL
};
