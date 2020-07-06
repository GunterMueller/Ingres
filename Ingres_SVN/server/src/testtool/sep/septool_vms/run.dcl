$!-----------------------------------------------------------------------------      
$! History:
$!---------
$! Created  ?
$!
$! Modified 
$!    12-06-2001  (bolke01)
$!                Added increased functionality as described in the Usage notes"
$!
$! Inputs
$!-------
$!    P1        valid filename or symbol
$!    P2 - P8   parameters to P1
$!
$! Outputs
$!--------
$!     None
$!
$! Description:
$!-------------
$! 
$! This function will run anything that is passed in that exists in
$! either the local directory, is accessed via a symbol, or is in
$! a remote directory that is fully specified.
$!      
$! A check is made on the file type and only STMLF files are treated 
$! as command files.  All others are executed by adding a '$' to the 
$! passed in file name     
$!      
$!-----------------------------------------------------------------------------      
$ def_ver = f$verify(0)
$ if f$type(test).nes."INTEGER" then $ test == 0
$ t = f$verify(test)
$ def_dir = f$environment("DEFAULT")
$! 
$ runfile = p1 
$!
$ if runfile - "HELP" -"?" .nes. runfile .or. "''p1'" .eqs. ""
$ then 
$  write sys$output " "
$  write sys$output "Usage run.com"
$  write sys$output " "
$  write sys$output "   @run.com [?|HELP|runfile[.COM|.EXE] p2 [p3] [p4] ... [p8] "
$  write sys$output "   where runfile  is one of:"
$  write sys$output "       1   symbol with or without embedded parameters"
$  write sys$output " "
$  write sys$output "       2   local command file - normal extension = .COM "
$  write sys$output "               ( no extension = .COM or .EXE with precidence on .COM)"
$  write sys$output " "
$  write sys$output "       3   local executable - normal extension .EXE"
$  write sys$output " "
$  write sys$output "       4   executable from another location - normal extension .EXE"
$  write sys$output " "
$  write sys$output "   NOTE: The file attribute RFM is checked for correct type :"
$  write sys$output "          RFM = STMLF is treated as a command file 
$  write sys$output "          all others are considered to be executables"
$  write sys$output " "
$  write sys$output " "
$  exit
$ endif
$!
$ have_dollar = 0			     	! assume no dollar in position 0 to start with 
$ tmpparam = ""					! reset var for holding stripped embedded params
$!
$ gosub eval_parameters
$!
$ if nr_of_p.eq.0 then $ exit
$!
$ tmpcom = 0                                 	! assume its not a command file
$!
$! ** check to see if it is, and it exists
$   tmprun = f$parse(runfile,"''def_dir'.COM")
$   if f$search(tmprun) .eqs. ""
$   then
$!
$! ** does not exist so check for EXE
$     tmprun = f$parse(runfile,"''def_dir'.EXE")
$     if f$search(tmprun) .eqs. "" 
$     then
$!                                             not an EXE either so check for a dollar in pos 0
$        if f$search(tmprun) .eqs. "" .and. f$extract(0,1,"''runfile'").nes."$"
$        then
$           tmpcom = 0			     	! symbol - do nothing
$!
$! extract a potential EXE from a symbol
$ 	    set mess/nofac/notext/noid/nosev  	! disable warning of nonexistant symbols
$           symfile = "''runfile'" 
$           symfile = 'symfile' 
$           tmpsym := 'symfile'
$ 	    set mess/fac/text/id/sev		! re-enable messages
$!
$! ** check for embedded params in symbol
$           have_param = f$locate (" ",tmpsym)
$           have_dollar = 0
$           if  f$extract(0,1,"''tmpsym'").eqs."$" then have_dollar = 1
$! ** remove params so we can check existane of EXE in symbol
$           tmprun = f$extract (have_dollar,have_param,tmpsym)
$           tmpparam = f$extract (have_param,255,tmpsym)
$        else
$!
$           tmpcom = 0			     ! remote file 
$! ** dollar exists on front of runfile, but double check it and record this
$           if  f$extract(0,1,"''runfile'").eqs."$" then have_dollar = 1
$!
$! ** remove dollar from front of runfile
$           tmprun = f$extract(have_dollar,255,runfile)
$        endif    
$      endif    
$   endif    
$!
$! ** Final check to see if after all of the manipulations there exists a file to run.
$ tmprun2 = f$search(tmprun)  
$ if tmprun2 .eqs. "" 
$ then 
$   write sys$output "Not found [''P1'] as ''tmprun'"
$   exit
$ endif
$!
$! ** Ensure that we do the correct run action on the file - even if the naming convention is wrong.
$ if f$file (tmprun2,"RFM") .EQS. "STMLF" THEN  tmpcom = 1      ! detected a text file, treat as a command file
$ if f$file (tmprun2,"RFM") .EQS. "VAR" THEN  tmpcom = 1        ! detected a text file, treat as a command file
$!
$! ** add dollar to front of runfile if it existed previously.
$ if  have_dollar .eq. 1
$  then 
$    tmprun = "$"+tmprun2
$ else
$    tmprun = tmprun2
$ endif
$!
$ tmplogset = 0
$!
$ if tmpcom .eq. 0 
$ then
$   if f$extract(0,1,tmprun).eqs."$"
$   then
$	tmp_dev = f$parse(tmprun,,,"DEVICE",   "SYNTAX_ONLY")
$	tmp_dir = f$parse(tmprun,,,"DIRECTORY","SYNTAX_ONLY")
$	tmp_nam = f$parse(tmprun,,,"NAME",     "SYNTAX_ONLY")
$	tmp_typ = f$parse(tmprun,,,"TYPE",     "SYNTAX_ONLY")
$	if f$extract(0,1,tmp_dev).eqs."$" then tmp_dev = f$extract(1,255,tmp_dev) 
$	define/nolog tmplog "''tmp_dev'''tmp_dir'"
$!
$! Add a dollar and add back parameters stripped out of symbol in previous evaluation
$	tmprun = "$tmplog:''tmp_nam'''tmp_typ'" + " ''tmpparam'"
$	tmplogset = 1
$    else
$	tmprun = "$''tmprun'"
$   endif
$ else
$	tmprun = "@''tmprun'"   
$ endif
$!
$ tmprun 'p2' 'p3' 'p4' 'p5' 'p6' 'p7' 'p8'
$!
$ if tmplogset then $ deassign tmplog
$!
$ exit
$!*********************************************************************!
$eval_parameters:
$!**********************************************************************!
$!
$ nr_of_p = 0
$ i = 1
$ j = 1
$_start_eval_loop:
$ if i.eq.9   then $ goto _end_eval_loop
$ p = p'i'
$ if p.eqs."" then $ goto _cont_eval_loop
$!
$ if i.ne.j then $ p'j' = p
$ nr_of_p = j
$ j = j + 1
$!
$_cont_eval_loop:
$ i = i + 1
$ goto _start_eval_loop
$_end_eval_loop:
$!
$ i = nr_of_p + 1
$_start_eval_loop2:
$ if i.eq.9 then $ goto _end_eval_loop2
$!
$ p'i' = ""
$!
$ i = i + 1
$ goto _start_eval_loop2
$_end_eval_loop2:
$!
$ return
$!****************************************************!
$ exit
