/*
** Copyright (c) 2007 Ingres Corporation. All rights reserved.
*/

/*
**  Name: genpkgdata.h
**
**  Purpose: 
**	Header for GLOBALREFs for SPEC/Control file generation process.
** 
**  History:
**	21-Jun-2007 (hanje04)
**	    SIR 118420
**	    Created from genrpm.c
*/

/**** external references ****/
GLOBALREF char *pkgBuildroot;
GLOBALREF LIST distPkgList;
GLOBALREF char *license_type;

/**** Data definitions ****/
/**** spec and substitution list maintenance ****/
GLOBALREF SPECS *specsFirst;
GLOBALREF SPECS *specsLast;
GLOBALREF SUBS *subsFirst;
GLOBALREF SUBS *subsLast;

/**** required config file substitutions ****/
GLOBALREF SUBS *subsBaseName;
GLOBALREF SUBS *subsVersion;
GLOBALREF SUBS *subsRelease;
GLOBALREF SUBS *subsPrefix;
GLOBALREF SUBS *subsEmbPrefix;
GLOBALREF SUBS *subsMDBPrefix;
GLOBALREF SUBS *subsMDBVersion;
GLOBALREF SUBS *subsMDBRelease;
GLOBALREF SUBS *subsDocPrefix;
