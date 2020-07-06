$! Copyright (c) 2004, 2005  Ingres Corporation
$!
$!
$! inglink.dcl - Links Ingres executables
$!
$! Supports the linking of Ingres executables based on a linker options
$! template (.lot) file.
$!
$! Parameters:
$!	p1	path to executable to be linked, e.g.
$!		ING_BUILD:[bin]ercompile.exe
$!	p2	path to main object, usually in relative form, e.g.,
$!		[.cl.clf.er_vms]ercomp.obj
$!
$! A search is made in the main object directory, e.g., ING_SRC:[cl.clf.er_vms],
$! for a .lot file with the name of the executable, e.g., ercompile.lot. If
$! no .lot file is found, a basic link is made. If a .lot file is found, it
$! it preprocessed into a .opt file using sed. The following substitutions are
$! made:
$!
$!	INGLIB:		replaced by the path to the main "library" directory,
$!			e.g., ING_BUILD:[lib]
$!	INGTOOLSLIB:	replaced by the path the tools library directory, e.g.,
$!			ING_TOOLS:[lib]
$!	MAINOBJ		replaced by the path to the main object, e.g.,
$!			ING_SRC:[cl.clf.er_vms]
$!
$! The substitutions do not include the suffixes (e.g., .obj, .olb) or any
$! applicable linker options (e.g., /library, /include=(gv)). A sample
$! linker options template file looks as follows:
$!
$!	INGLIB:frontmain.obj, -
$!	MAINOBJ.obj, -
$!	INGLIB:libcompat.olb/library
$!
$!!  History:
$!!	24-nov-2004 (abbjo03)
$!!	    Created.
$!!	16-dec-2004 (abbjo03)
$!!	    Create a linker map file when using linker options templates.
$!!	10-jan-2005 (abbjo03)
$!!	    Exit with an error if linker gives warnings in ALL cases.
$!!	08-feb-2005 (abbjo03)
$!!	    Link septool and setuser with /SYSEXE.
$!!	17-feb-2005 (abbjo03)
$!!	    Create symbol table files for bin and utility executables.
$!!     26-apr-2004 (loera01)
$!!          Broke sed command into three separate commands, because
$!!          long directory paths exceed the 256-character command limit.
$!!          This change should be removed when the build environment
$!!          is upgraded to VMS 7.3-2 or later.
$!!	22-jul-2005 (abbjo03)
$!!	     Unless LINKFLAGS is defined, set it to /NOTRACEBACK so that
$!!	     privileged images can be installed.
$!!	09-apr-2007 (abbjo03)
$!!	     Support ING_SRC defined in terms of a concealed logical.
$!!
$ exe = p1
$ mainobj = p2
$ exename = f$parse(exe,,,"name")
$ exepath = f$parse(exe,,,"device")+f$parse(exe,,,"directory")
$ objpath = f$parse(mainobj,,,"directory")
$ objname = f$parse(mainobj,,,"name")
$ opttempl = f$search("''objpath'''exename'.lot")
$ if exepath .eqs. "ING_BUILD:[bin]" .or. exepath .eqs. "ING_BUILD:[utility]" -
	then stb = "/SYMB=ING_BUILD:[debug]''exename'.stb"
$ if opttempl .eqs. ""
$ then	on warning then goto err_exit
$	link /exec='exe' 'stb' 'mainobj'
$	exit
$ endif
$ if "''LINKFLAGS'" .eqs. "" then LINKFLAGS = "/NOTRACE"
$ if exename .eqs. "SEPTOOL" .or. exename .eqs. "SETUSER" then -
	LINKFLAGS = "/SYSEXE /NOTRACE"
$ ingsrcpath=f$parse(f$trnlnm("ING_SRC")-".]"+"]",,,"directory")-"]"+"."
$ objpath = f$parse(mainobj,,,"directory","no_conceal") - "]["
$ objpath="ING_SRC:["+(objpath-ingsrcpath)
$ mainobj=objpath+objname
$ pipe sed "-e s/INGLIB:/ING_BUILD:[lib]/g" 'opttempl' > 'objpath''exename'.sed1
$ pipe sed "-e s/INGTOOLSLIB:/ING_TOOLS:[lib]/g" 'objpath''exename.sed1 > -
'objpath''exename'.sed2
$ pipe sed "-e s/MAINOBJ/''mainobj'/" 'objpath''exename.sed2 > -
'objpath''exename'.opt
$ del 'objpath''exename.sed*;*/nol
$ on warning then goto err_exit
$ link 'LINKFLAGS' /map='objpath''exename'.map /exec='exe' 'stb' -
	'objpath''exename'.opt/options
$ del 'objpath''exename'.opt;*
$ exit
$err_exit:
$ exit %x2c
