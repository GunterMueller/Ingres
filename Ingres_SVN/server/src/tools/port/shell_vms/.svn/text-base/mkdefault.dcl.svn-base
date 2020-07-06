$! Copyright (c) 2005 Ingres Corporation
$!
$!
$!	creates "default.h" in the current directory, defining
$!	symbols that may be used to determine the machine environment.
$!
$!! History:
$!!	28-jan-2005 (abbjo03)
$!!	    Created from mkdefault.sh.
$!!
$ create default.h
$ open /append out default.h
$ @ING_TOOLS:[bin]readvers
$ i=0
$ opts=f$edit(IIVERS_opts, "trim")
$optsloop:
$	o=f$elem(i, " ", opts)
$	if o .eqs. " " then goto endopts
$	write out "#define conf_''o' TRUE"
$	i=i+1
$	goto optsloop
$endopts:
$!
$ vers=IIVERS_config
$!
$ write out "VERS = ''vers'"
$ write out "#define ''vers'"
$!
$! CPP specials
$! CPP should be set to the name of the C pre-processor on the system.
$ write out "CPP = cc /preproc"
$!
$! Which C compiler should we use?
$ compiler="cc"
$ write out "CC = ''compiler'"
$!
$! Set optimizer flag
$	optimflag="/OPTIM"
$!
$ write out "OPTIM = ''optimflag'"
$!
$! C++ and Xerces defines for buliding ICE and other XML tools
$!
$ if f$extr(3, 4, vers) .eqs. "_vms"
$ then	write out "CXX=cxx"
$ endif
$ close out
$ exit
