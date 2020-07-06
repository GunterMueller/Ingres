# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
/**
** Name:
**	oosymbol.qh -	Symbol table extern declarations
**
** Description:
**	Generated automatically by classout.
**
** History:
**	30-Aug-91 (Sanjive)
**		Integrate a change from ingres6302p :-
**      	11-Feb-90 (wojtek)
**              	#defined '_init' to '_iiOOinit' to avoid clash with
**              	standard objects.
**      20-jul-92 (fraser)
**              Changed _flush, etc. to ii_flush because of conflict with
**              Microsoft C library.
**
**              Note:  All identifiers beginning with an underscore should
**                     be changed.  Identifiers beginning with underscore
**                     are reserved in ANSI C.
**/
#define _ _iiOOempty
  GLOBALCONSTREF char _[];
  GLOBALCONSTREF char _iiOOempty[];
#define _alloc _iiOOalloc
GLOBALCONSTREF char	_alloc[];
#define _at iiOOat
GLOBALCONSTREF char	_at[];
  GLOBALCONSTREF char _atEnd[];
  GLOBALCONSTREF char _atPut[];
#define _authorized iiOOauthorized
GLOBALCONSTREF char	_authorized[];
  GLOBALCONSTREF char _catForm[];
  GLOBALCONSTREF char _catInit[];
#define _confirmName iiOOconfirmName
GLOBALCONSTREF char	_confirmName[];
  GLOBALCONSTREF char _class[];
  GLOBALCONSTREF char _copy[];
  GLOBALCONSTREF char _copyId[];
  GLOBALCONSTREF char _currentName[];
  GLOBALCONSTREF char _decode[];
  GLOBALCONSTREF char _destroy[];
  GLOBALCONSTREF char _do[];
  GLOBALCONSTREF char _edit[];
  GLOBALCONSTREF char _encode[];
  GLOBALCONSTREF char _fetch[];
  GLOBALCONSTREF char _fetch0[];
  GLOBALCONSTREF char _fetchAll[];
  GLOBALCONSTREF char _fetchDb[];
  GLOBALCONSTREF char _first[];
  GLOBALCONSTREF char ii_flush[];
  GLOBALCONSTREF char ii_flush0[];
  GLOBALCONSTREF char ii_flushAll[];
#define _init   _iiOOinit
  GLOBALCONSTREF char _init[];
  GLOBALCONSTREF char _init0[];
  GLOBALCONSTREF char _initDb[];
  GLOBALCONSTREF char _insertrow[];
  GLOBALCONSTREF char _isEmpty[];
  GLOBALCONSTREF char _isNil[];
  GLOBALCONSTREF char _markChange[];
  GLOBALCONSTREF char _mlook[];
  GLOBALCONSTREF char _name[];
  GLOBALCONSTREF char _nameOk[];
#define _new iiOOnew
GLOBALCONSTREF char	_new[];
  GLOBALCONSTREF char _new0[];
  GLOBALCONSTREF char _newDb[];
#define _next iiOOnext
GLOBALCONSTREF char	_next[];
  GLOBALCONSTREF char _noMethod[];
  GLOBALCONSTREF char _perform[];
  GLOBALCONSTREF char _print[];
#define _readfile iiOOreadfile
GLOBALCONSTREF char	_readfile[];
#define _rename	iiOOrename
GLOBALCONSTREF char	_rename[];
  GLOBALCONSTREF char _setPermit[];
  GLOBALCONSTREF char _setRefNull[];
  GLOBALCONSTREF char _subClass[];
  GLOBALCONSTREF char _subResp[];
#define _time	iiOOtime
GLOBALCONSTREF char	_time[];
  GLOBALCONSTREF char _vig[];
  GLOBALCONSTREF char _withName[];
#define _writefile	iiOOwritefile
GLOBALCONSTREF char	_writefile[];
