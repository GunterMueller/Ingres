:
#
# Copyright (c) 2004 Ingres Corporation
#
#	creates "default.h" in the current directory, defining
#	symbols that may be used to determine the machine environment.
#
## History:
##	10-dec-93 (dianeh)
##		Created (branched) from tools!port!ming!mkdefault;
##		removed code for obsolete machines/ports.
##	04-jan-94 (dianeh)
##		-lnsl flag belongs in INGNETLIB symbol, not "suffix", (which
##		accumulates to LDLIB) (NOTE: Only fixed for su4_u42, not for
##		usl_us5 or su4_us5, but these should get fixed as well.)
##	07-jan-94 (tomm)
##		CHanged Solaris Optimizer flag from -O2 to -O.  The Solaris
##		C compiler doesn't really have an option -O2.  This just
##		happened to work with cc -c.  If you try to use this to 
##		compile something and link, ld will complain about illegal
##		flag -2.  This will generate a bad entry in iisysdep.
##	20-jan-94 (dianeh)
##		Removed reference to RPC library -- this library isn't
##		currently referenced in in INGRES code other than rl.c,
##		where it doesn't need to be (and will soon be taken out).
##		We are waiting to hear back from Sun as to whether it's
##		still even needed at all, and if it is, at that point,
##		references to it will be put into the INGRES code where
##		it actually should be; until then, I'm just taking it out.
##		If it is determined that it's still needed, though, a new
##		way of dealing with the symbols here will have to be devised
##		and synchronized with mksysdep to avoid having the reference
##		to it end up in iisysdep.
##	12-jan-94 (johnst)
##		Bug #58876
##		Added XTERMLIB, XNETLIB entries for axp_osf.
##		Added librt.a for memlk on axp_osf.
##	01-feb-94 (johnst)
##		Added -ltermcap to XTERMLIB for dr6_us5 to link wview.
##		Integrate XTERMLIB for usl_us5 from 6.4/04: use shared 
##		libraries since wview had problems with the static versions
##		under SVR4.2.
##	25-jan-94 (arc)
##		Support Sun CMW (B1 Secure O/S).
##	11-feb-94 (tomm)
##		Sun Solaris needs to use libraries in /opt/SUNWspro/SC2.0.1
##		not /opt/SUNWspro/lib.  If we use the latter, ABF applications
##		will not link because of fortran library bugs.  Bleck.
##      23-mar-94 (ajc)
##              Support hp8_bls (B1 Secure O/S). In line with hp8* where 
##		possible.
##      2-may-94 (andrewc)
##              Use default Solaris X include files and libraries for
##              su4_us5
##  	04-May-94 (cmorris)
##  	    	Added INGNETLIB for HP-UX SNA LU6.2 support.
##	11-jul-94 (ajc)
##		HP-UX SNA LU6.2 is NOT supported on hp8_bls. Removed INGNETLIB
##		entry for hp8_bls.
##	20_dec-94 (GordonW)
##		over-hauled hp8_us5.
##		for su4_u42 default CC=acc, or if option=gcc is in VERS use
##		'gcc"
##      04-jan-95 (canor01, chech02)
##              Added rs4_us5 for AIX 4.1. 
##      24-feb-95 (brucel)
##              Added "INGNETLIB = /opt/SUNWconn/lib/libappc_api.a" to Solaris
##              (su4_us5) defines for LU62 support.
##      28-Nov-94 (nive/walro03)
##              Added support for dg8_us5 during OpenINGRES FCS2/FCS3 port.
##	12-Apr-95 (georgeb)
##		Changed the compile flags for Unixware to use the pentium
##		flag (-K pentium).
##	13-Apr-95 (smiba01)
##		ICL DRS6000 ES+ (dr6_ues) entries.
##		Also fixed dr6_uvl to dr6_uv1 in a couple of places.
##		Note: these changes are duplicates of those made in the
##		ingres63p library for 6.4 port.
##		Changes: (28-apr-92 (sbull))
##			 (30-sep-92 (swm))
##			 (01-dec-92 (abowler))
##			 (01-nov-93 (ajc))
##	01-may-95 (wadag01)
##		Added odt_us5 changes from 6.4/05.
##	20-jun-95 (hanch04)
##		New c89 from hp does not use -Ae, added -D_HPUX_SOURCE
##	22-jun-95 (gmcquary)
##		sqs_ptx port specific changes.
##	22-jun-95 (chimi03)
##		Remove references to XLIB libraries from the parameters
##		generated for hp8_us5.
##	07-jul-95 (kosma01)
##		Removed Xlib references for ris_us5 for OpIng 1.1/04 port.
##	15-jul-95 (popri01)
##		Added nc4_us5 to LIBC, CCMACH, and LDMACH "specials" lists
##		Add libucb.a for nc4_us5 suffix.
##              Add nc4_us5 special ldlibpath & syslibs.
##		Add special LOW_OPTIM fro nc4 because cc default is -O3.
##	20-jul-95 (morayf)
##		Added some odt_us5 changes for sos_us5. Didn't include X stuff
##		as this is not required for OpenIngres.
##		Using COFF format object files (and hence COFF linker) which
##		is the default format produced by the compiler.
##	25-jul-95 (allmi01)
##		Added support for dgi_us5 (DG-UX on Intel based platforms).
##	04-aug-95 (kosma01)
##		Changed CCMACH for ris_us5. Added option -qmaxmem=-1 removes
##		the limit on the amount of memory the compiler can use to 
##		optimize a file. This was done to correct a C2 secure 
##		problem on AIX. Table alarms were not triggered. The failure
##		was tracked to rdu_qopen of back/rdf/rdf/rdfgetinfo.c.
##      07-aug-1995 (pursch)
##              Add in changes by gmcquary from 6405 17-dec-92.
##              Add pym_us5 specific code.
##              Add in changes by erickson from 6405 3-dec-94.
##              pym_us5: add "-K Olimit:4000" to let the optimizer work
##              on bigger blocks; compiler is fast, we can afford it.
##              Supply RROC.
##              Use -lbsocket instead of -lsocket: native BSD sockets on
##              Pyramid (for speed and to bypass Nile OS bug).
##              Add prefix and suffix assignments for pym_us5.
##	 4-sep-95 (peeje01)
##		Remove "INGNETLIB = /opt/SUNWconn/lib/libappc_api.a" from
##		Double Byte Solaris (su4_us5)DBL as we do not support LU62 
##              in DBCS version.
##	 30_oct-95 (allmi01)
##		dgi_us5 final port changes and corrections
##	10-nov-1995 (murf)
##		Adding sui_us5 as the port configuration string for SOLARIS x86 
##		2.4 . The su4_us5 options will be appended with sui_us5.
##		INGNETLIB is not defined within the setup, as SNA (LU6.2)
##		support is not being catered for.
##       22-jun-95 (matbe01)
##              Added references to XLIB libraries back in for the parameters
##              generated for hp8_us5. They are needed for WindowView.
##       18-nov-95 (allst01)
##		Corrected definition for su4_cmw
##       27-dec-95 (thoro01)
##              Added libs to XTERMLIB and added lowsys.exp to CCMACH for AIX.
##	 09-jan-96 (hanch04)
##		Added D_SVID_GETTOD for su4_us5 CCMACH
##	 15-jan-96 (stephenb)
##		Added above change also for sui_us5
##	 01-Nov-95 (spepa01)
##		Add libucb.a to ldlibpath/syslibs as vdba uses BSD strcasecmp().
##	 21-Dec-95 (spepa01)
##		wview needs to use shared Xlibs.
##	 05-jan-95 (wadag01)
##		Changed CCMACH entry for sos_us5 to "-b elf -a ods30" to use
##		ELF object file format (the default on SCO is COFF).
##		The option '-a ods30' fixes the problem with naming of members
##		in the structure in the stdio.h (the defaults stdio.h on SCO
##		does not permit __ptr names, it uses _ptr, etc).
##		The -a ods30 option causes the inclusion of the backward
##		compatable version of stdio.h residing in ods_30_compat/stdio.h.
##		Defined CCPICFLAG for odt_us5.
##		Added sos_us5 to odt_us5 entry defining XTERMLIB, XNETLIB
##		and XTERMHDR.
##		Also removed 2 false comments relating to usl_us5 and Solaris
##		dated 1 & 2 Nov (murf integrated them by mistake on 10 Nov 95).
##	06-dec-95 (morayf)
##		Added rmx_us5 code for SNI RMx00 port, along with some pym_us5
##		changes to mirror these.
##	16-jan-1996 (toumi01; from 1.1 axp_osf port) (schte01)
##		Added axp_osf changes: added -ldnet_stub and /usr/ccs/lib
##	22-Feb-1996 (allmi01)
##		Added libresolv for vdba build to correct link errors.
##	23-feb-96 (morayf)
##		Updated pym_us5 for Pyramid port.
##	09-sep-96 (muhpa01)
##		Removed '-non_shared' flag from CCLDMACH setting.
##	03-jun-1996 (canor01)
##		Add threads library for operating system threads.
##	11-nov-1996 (hanch04)
##		Add threads library for operating system threads. sui_us5
##	09-jan-1997 (hanch04)
##		Removed os threading.
##	18-jan-1997 (hanch04)
##		OS thread build for su4_us5,sui_us5 needs REENTRANT defined
##		Added -lthread,-lpthread for su4_us5,sui_su5
##	18-feb-1997 (muhpa01)
##		Changed optimflag from +O3 to -O for hp8_us5.  
##		Also, removed '-lstr -lV3' from INGNETLIB as these are
##		now incorporated into libc on HP-UX as of 10.10
##	27-mar-1997 (canor01)
##		With change to PCfork to use fork1() instead of fork(),
##		-lpthread is not needed for su4_us5 or sui_us5.
##  01-May-1997 (merja01)
##      Made following changes to axp_osf to build with pthreads and
##      async IO.
##       CCMACH    - added    -oldc -pthread     
##       ldlibpath - added    /usr/shrlib
##       syslibs   - removed  dnet  <-- dnet_stub is all we need for
##                                      decnet.  dnet cause unresolves
##                                      on boxes without decnet.
##       LDLIBMACH - added    -lpthread -laio -lmach -lexc
##	15-may-1997 (muhpa01)
##		Added _REENTRANT compile flag and -ldce library settings
##		for hp8_us5.
##  15-Aug-1997 (merja01)
##       Put the lrt lib before lc as per the recommendation of the
##       Digital Unix Realtime Programming Guide section 1.6.2.
##	10-sep-1997/04-apr-1997 (popri01)
##		x-integ some 1.2/01 changes for Unixware (usl_us5) for
##		wview (XTERMLIB). (ucblib, for achilles, NOT x-integrated.)
##	10-sep-1997/24-apr-1997 (popri01/hweho01)
##		Added threads library (-lthread) and compiler options   
##		(-D_REENTRANT and -K thread ) for UnixWare (usl_us5).
##	21-feb-1997 (walro03)
##		Reapply linke01's OpIng 1.2/01 change of 28-Oct-96:
##		change  libs = "-lsna -ltli" to
##		        libs = "-lsna -ltli -liconv" for rs4_us5 & ris_us5
##		which includes link editor library liconv to include symbols
##		.liconv_open etc.
##      20-may-1997 (musro02)
##              For sqs_ptx, 
##                  removed references to /usr/local/lib, sqsppstub.a,
##                     sqsppslib.a and SQSPPSLIB
##                  Removed the CCMACH options "-DGCF64 -Wc,-seq,-Y -Wp,-Ns6000"
##                  Set prefix to _L/usr/lib
##                  Add -lsocket -lnsl and -lelf to suffix
##	23-jun-1997 (walro03)
##		Remove "-Qy -B full" from CCLDMACH for ICL DRS 6000 (dr6_us5)
##		to do dynamic linking for shared library support.  Also added
##		debug flags.
##	11-Jul-1997 (allmi01)
##		Updated Solaris x/86 port.
##	28-jul-1997 (walro03)
##		Updated for Tandem port (ts2_us5).
##	29-jul-1997 (walro03)
##		For Siemens rmx_us5 platform, show where X11 stuff.
##		Set HDRMACH to /opt/X/include for rmx_us5 X11 hdrs.
##		Set XTRMLIB and ldlibpath to /opt/X/lib for X11 archive libs.
##	23-sep-1997 (hanch04)
##	    Remove typo
##      24-sep-1997 (matbe01)
##              Added XTERMLIB entry for nc4_us5 so that windowview will link
##              on NCR.
##      24-sep-1997 (allmi01)
##              Added -Xt cc flag to CCMACH (needed to set ANSI conformance
##              to transitional for csphil and cltest).  For suffix, added -L
##              with specific references to
##              /opt/K/SCO/unixds/5.0.0b/lib and to
##              /opt/K/SCO/Unix/5.0.2Dp/lib
##              These are an x-integ from 1.2/01.  Did this for sos_us5.
##              NB 5.0.0b and 5.0.2Dp don't exist.  Use 5.0.0a and 5.0.0Cl
##              Further, all members of /lib and /usr/lib are actually links
##              to /opt/K/SCO/... so these "hard coded" references aren't
##              even needed.  Took them out again.
##	04-Sep-1997 (bonro01)
##		Add thread library for dg8_us5 compile options.
##      24-sep-1997/28-Feb-1997 (mosjo01/linke01)
##              In CCMACH, use -Xa option instead of -Xt, so that datatype
##              const and signed are acceptable when compile.
##              Added threads library for operating system threads for dgi_us5
##	06-Nov-1997 (merja01)
##		Added -lSM -lXext and -lICE to XTERMLIB for axp_osf to resolve 
##		errors building wview.
##	11-mar-1998 (walro03)
##		For ICL (dr6_us5), changed -YP to -L in LDLIBMACH.
##	13-nov-97 (toumi01)
##		Modified hp8.us5 to support DBL port to use X11R5.
##	19-nov-1997 (walro03)
##		For AIX 3.2.x (ris_us5), removed "-qchars=signed" from CCMACH
##		to remain consistent with 1.2/01.  Changing the usage of signed
##		vs. unsigned char fields causes upgradedb problems.
##	06-Oct-1997 (hanch04)
##	    Added -lposix4 for aio on su4_us5
##	27-Jan-1998 (muhpa01)
##		Removed -DREENTRANT & -ldce, added -lc - turning off 
##		support for POSIX_DRAFT_4 threads.
##	16-feb-1998 (toumi01)
##		Add entries for new Linux (lnx_us5) port.
##      12-Mar-1997 (allmi01/linke01)
##              Updated pym_us5 for Pyramid port.    
##	26-Mar-1998 (muhpa01)
##		Added settings for new HP config string, hpb_us5.  This
##		represents HP-UX 11.0 32-bit.  Note: hpb_us5 also defines
##		hp8_us5 in order to enable current HP settings.
##	31-Mar-98 (gordy)
##		Added dl library for Sun, needed for GCS.
##	20-Apr-1998 (merja01)
##		Modified LD_LIB flags for axp_osf.  Removed -lc
##		because it is added automaticaly when compiling.
##		Removed -laio because we will not be using async
##		IO libraries in 2.5.  Moved the -lrt flag before
##		the -lm flag as recommended by DEC axpdeveloper
##		support.  Also, removed the -oldc flag from
##		CCMACH.
##	6-may-1998/1-apr-1998 (popri01)
##		For Unixware, explicitly specifying "-lthread" is not recommended
##		and in fact creates problems as of UW 2.1.1 due to the introduction 
##		of UNIX 95 elements (e.g., a weak "lseek" in libsocket).
##	01-Jun-1998 (hanch04)
##		Added -ldl for su4_us5 for dynamic library call of new
##		CA license
##      08-jun-1998 (musro02)
##              For sqs_ptx, remove an extraneous blank following a "\"
##	09-Jun-1998 (merja01)
##      Modified LD_LIB flags for axp_osf.  Removed -lc
##      because it is added automaticaly when compiling.
##      Moved the -lrt flag before the -lm flag as recommended
##      by DEC axpdeveloper support.
##
##	23-jun-1998 (walro03)
##		For AIX 4 (rs4_us5), include CA licensing library.
##	10-Jul-1998 (allmi01)
##		Added entries for Silicon Graphics port (sgi_us5)
##	27-jul-1998 (ocoro02)
##		Bugs 78338/82023 - added '-lc' to suffix portion for rmx_us5
##		to avoid accept errors in system error log.
##      02-sep-1998 (musro02)
##              For sqs_ptx, add -KPIC
##              For sqs_ptx, ensure use of bsd sockets rather than api sockets:
##                  change CCMACH, and prefix. 
##                  -D_SOCKET_VERSION=11 and -L/usr/lib/bsd-socket for bsd
##                  -D_SOCKET_VERSION=1  for api  
##	09-sep-1998 (muhpa01)
##		Additional porting change for hpb_us5: remove hpb_us5
##		from case statement which inhibited setting of HASRANLIB.
##	03-sep-1998 (toumi01)
##		For Linux libc6 we need -lcrypt in LDLIB and LDLIBMACH.
##      13-sep-1998 (mosjo01)
##              Added WINSYSLIB to sos_us5(SCO OpenServer).
##      09-Jun-1998 (linke01)
##              Added -ldl for rmx_us5 for dynamic library call of new
##              CA license. Also, added CCPICFLAG for rmx_us5.
##      23-Jun-1998 (linke01)
##             Added -ldl for pym_us5 for dynamic library call of new CA license
##  14-Oct-1998 (merja01)
##      Add -lmld to LDLIBMACH for axp_osf to access mld routines for
##      diagnostic stack trace.
##   6-Nov-1998 (hayke02)
##	Replace -D_REENTRANT and -lthread with -mt for su4_us5 and sui_us5
##	on the advice of SUN support. 
##  10-Nov-1998 (linke01)
##      Added -ldl for rmx_us5 for dynamic library call of new CA license.
##      Also, added CCPICFLAG for rmx_us5.
##  10-Nov-1998 (linke01)
##      Added -ldl fo rpym_us5 fpr dynamic library call of new CA license.
##   11-Nov-1998 (muhpa01)
##	Modify LDLIBMACH for both hp8_us5 & hpb_us5 to include the
##	unwind library, libcl.sl, used for stack dump routine.  In
##	the case of hp8_us5, changed -lc to -lcl since -lc is included
##	automatically & was redundant.
##  03-Dec-1998 (muhpa01)
##	Modified +DA compiler setting for hp8_us5 build on 10.20, and
##	added +e needed for 'long long' datatype.  Removed XTERMLIB
##	settings for hp8_us5 & hpb_us5 and added -lmgr lib for SNAplus2,
##	now used by both hp8_us5 & hpb_us5.
##  15-dec-98 (stephenb)
##	Add new compiler optimization flag LEVEL1_OPTIM to allow lower levels 
##	of optimization. Set the flag to -xO1 for Solaris, the default for 
##	all other plaforms will be -O for the time being.
##  04-Dec-1998 (schte01)
##      Added NOROC to get around a DEC Unix compiler error.
##  07-jan-1999 (canor01)
##      Use -lposix4 for all Solaris builds.
##  19-jan-1999 (matbe01)
##      Changed XTERMLIB, CCMACH, ldlibpath, and added prefix for NCR
##      (nc4_us5).
##  05-Feb-1999 (peeje01)
##      Set NETSCAPE_API_FLAG for netscape server extension (ICE) build
##  11-jan-1999 (matbe01)
##      Removed -lsna and added -ldl for AIX (rs4_us5).
##  09-mar-1999 (muhpa01)
##      Set NETSCAPE_API_FLAG for netscape server extension for HP-UX.
##	16-mar-1999 (popri01)
##		As of Unixware 7.01, remove explicit libc ("-lc") references,
##		as the order and inclusion of libc should be handled by by the 
##		compiler/linker. This also eliminates a compiler warning. Also
##		remove explicit specification of system libraries (/lib /ccs/lib)
##		via default path override (-YP) - they are searched by default.
##		Finally, add Unixware NETSCAPE_API_FLAG.
##  10-mar-1999 (walro03)
##	Sequent (sqs_ptx) changes:
##	1. Set NETSCAPE_API_FLAG.
##	2. Set link library directory.
##	3. Set CCMACH_DBG and OPTIM_DBG.
##  22-Mar-1999 (kosma01)
##	More entries for Silicon Graphics port (sgi_us5)
##	sgi_us5 can be a pure 32bit platform, IRIX, or a 64 bit platform that
##	suports 32 bit also, IRIX64. It may have a MIPS 3 or a MIPS 4 processor.
##	sgi_us5 supports two 32 bit runtime environments ELF(new) and COFF(old).
##	I am compiling for the MIPS 3, ELF 32 bit environment. Thus -mips3 -n32 is 
##	in CCMACH and every system library should be found in /usr/lib32/mips3, 
##	or /usr/lib32.
##	By default the compiler defines char as unsigned, Ingres expects them to
##	be signed, -signed. I added -lelf to syslibs to get nlist and nlist64 for
##	syscheck. I had to give full path for libs used in XTERMLIB and WINSYSLIB.
##	Otherwise when they were used in compiling front!wview!xtermr4, they were
##	retrieved from /usr/lib, the old 32 bit libraries.
##	I turned off warning message 84 from ld (library not used). 
##	I turned off warning message 1116 from cc (function should return a value).
##	added -lcurses to sgi_us5 suffix for vsar.c in tools/port/others.
##	14-oct-1998 (bobmart)
##	    Changing hp8_us5 PIC flag from +z to +Z to accommodate
##	    customers that link large applications. (Bug 77830)
##  17-Mar-1999 (schte01)
##      Added NETSCAPE_API_FLAG for axp.
##  28-apr-1999 (toumi01)
##      Add option "-fno-strength-reduce" for Linux.
##  05-May-1999 (peeje01)
##	Add CGISFX for ICE cgi program suffixes (eg: 'cgi' on Unix, 'exe' on NT)
##	10-may-1999 (walro03)
##	    Remove obsolete version string bu2_us5, bu3_us5, dr5_us5, dr6_ues,
##	    dr6_uv1, dra_us5, ds3_osf, ib1_us5, mip_us5, odt_us5, pyr_u42.
##  13-May-1999 (allmi01)
##      Revamped all build options for rmx_us5 for building with
##      -Kthread to get POSIX threads. syslibs, prefix, suffix, libs,
##      CCMACH, CCLDMACH all changed to get proper thread safe options.
##  20-May-1999 (ahaal01)
##	Set NETSCAPE_API_FLAG for netscape server extension for AIX (rs4_us5).
##  22-jun-1999 (musro02)
##      For nc4_us5, correct definition of XTERMLIB (remove spurious newline)
##          redo reference to ucblib
##          Add NETSCAPE_API_FLAG, etc.
##          remove library names from suffix and remove prefix
##          (in iisysdep, expr segv's when LIBMACH LDLIB or
##          LDLIBMACH contains anything other that a -l... value)
##          when running newbuild ensure LD_LIBRARY_PATH contains all needed
##          libraries      
##	01-jul-1999 (toumi01)
##		Add option "-D_GNU_SOURCE" for Linux after encountering
##		problems referencing fds_bits in cl/clf/handy/clpoll.c.
##  03-jul-1999 (podni01)
##      Added support for ReliantUNIX V 5.44 - rux_us5 (same as rmx_us5)
##  09-Sep-1999 (bonro01)
##	Add -D_POSIX4_DRAFT_SOURCE to enable nano-second clock realtime
##	functions.
##	Add -lrte to resolve use of clock_gettime() in ABF ImageApp
##	applications.
##	Switch order of nsl and socket library.  I found a man page
##	intro(3) that lists the proper library order and it shows that
##	libsocket needs libnsl so socket must preceed nsl in the lib order.
##      Set NETSCAPE_API_FLAG for netscape server extension (ICE) build
##	01-oct-1999 (toumi01)
##		Add link parameter -ldl for Linux (lnx_us5) to resolve
##		dlopen and dlsym for CA licensing.
##	06-oct-1999 (toumi01)
##		Change Linux config string from lnx_us5 to int_lnx.
##		Add lnx as a valid suffix.
##  27-oct-1999 (somsa01)
##	Changed CCLDMACH for HP to include the "-E" flag. This allows symbols
##	from a target executable to be exported to any shared libraries that
##	it may load.
##  08-Dec-1999 (podni01)
##      Added NETSCAPE_API_FLAG for Siemens (rux_us5).
##     10-Jan-2000 (hanal04) Bug 99965 INGSRV1090.
##        Correct rux_us5 changes that were applied using the rmx_us5
##        label.
##  21-jan-2000 (somsa01)
##	Added APACHE_API_FLAG for building the Apache exetensions on
##	Solaris and HP.
##	08-feb-2000 (somsa01)
##	    Added MAINWIN_EX_LOC, which is set to the location of any build
##	    through MainWin.
##	24-feb-2000 (somsa01)
##	    For HP, if we're defining LARGEFILE64, then add
##	    '-D_LARGEFILE64_SOURCE to the compiler flag list.
##  29-feb-2000 (toumi01)
##	Add pthreads library for OS threads: int_lnx (Linux).
##  13-Mar-2000 (hweho01)
##      Added APACHE_API_FLAG for DEC/UX (axp_osf).
##	08-may-2000 (hanch04)
##	    Changed LDLIB for su4_us5
##  22-May-2000 (ahaal01)
##	Removed static link to lic98.0 file for AIX (rs4_us5) to enable
##	dynamic linking to the CA license code.
##  23-may-2000 (somsa01)
##	Set up CCMACH_DBG and LEVEL1_OPTIM for HP.
##  24-may-2000 (hweho01)
##      Added support for AIX 64-bit platform (ris_u64).
##  26-jun-2000 (toumi01)
##	Add _REENTRANT for Linux (required if the pthread library is used).
##  14-Jul-2000 (hanje04)
##      Added APACHE_API_FLAG and NETSCAPE_API_FLAG for building Apache and
##      Netscape extensions on int_lnx; and added -rdynamic to CCLDMACH.
##      This allows symbols from the target executable to be exported to
##      any shared libraries that use them
##     27-Feb-2000 (linke01)
##              changed suffix of LDLIB and LDLIBMACH for usl_us5
##  22-May-2000 (ahaal01)
##	Removed static link to lic98.0 file for AIX (rs4_us5) to enable
##	dynamic linking to the CA license code.
##     05-June-2000 (yeema01) Sir# 100007
##		Modified the settings for W4GL for hp8_us5 OpenROAD
##		4.1 port.
##	08-aug-2000 (hayke02)
##	    Change ingnetlib define and chip architecture compiler directive
##	    from DA1.0 to DA1.1 for hp8_us5.
##     09-Aug-2000 (hanal04) Bug 102289 INGSRV 1242.
##          Partially backed out change 444935 as it introduced build
##          errors for rs4_us5.
##  17-Mar-1999 (schte01)
##      Added NETSCAPE_API_FLAG for axp.
##     14-Jul-2000 (hanal04)
##          Removed the -oldc flag from CCMACH for axp_osf.
##  15-aug-2000 (somsa01)
##	Added support for ibm_lnx.
##     04-Dec-1998 (schte01)
##          Added NOROC to get around a DEC Unix compiler error.
##  17-Mar-1999 (schte01)
##      Added NETSCAPE_API_FLAG for axp.
##     25-Oct-2000 (bonro01)
##          Modified generation of conf_ defines to define them as TRUE
##          instead of giving them a null value.
##          The problem was that default.h is parsed to generate iisysdep
##          which is used in an installed ingres to generate script shell
##          variables for testing.  These shell variables need to have a
##          value in order to test for their existence.
##          rpmksvr.sh was modified to check for conf_INGRESII.
##	12-jan-2001 (hayke02)
##	    /usr/include/X11R4 does not exist on HP 10.20 nor 11.00 (used
##	    for HDRMACH) - use /usr/contrib/X11R[4,5]/include and
##	    /usr/include/X11R6 instead. This change fixes bug 103697.
##	17-Feb-2001 (bolke01)
##	    Changed the usl_us5 id to not include the libc library. This
##	    is included automatically.  Added -L/usr/ccs/lib to search
##	    path. (bug 104158)
##	    
##	2-feb-99 (stephenb)
##		Add 64-bit compile and assembler flags for Solaris if 
##		LP64 option is in VERS, also add 64-bit library path, this
##		should be ignored on 32-bit systems
##  11-Sep-2000 (hanje04)
##	Added support for Alpha Linux (axp_lnx)
##  19-Dec-2000 (hanje04)
##	Added CCMACH_DBG and LEVEL1_OPTIM for Alpha Linux
##  10-Oct-2000 (hweho01)
##      1)Added CCLDSERVER to hold the special linker options for dbms
##        server only. The flag will be referened in iilink.
##      2)Set CCLDSERVER to "-bmaxdata:0x30000000" for AIX platforms    
##        (ris_u64 and rs4_us5).
##  13-Feb-2001 (ahaal01)
##	Added MAINWIN_EX_LOC for AIX (rs4_us5), which is set to the location
##	of any build through MainWin.
##  22-Feb-2001 (ahaal01)
##	Added variable "MWSLSFX" for the MainWin file suffix and initialized
##	it to the value of the SLSFX variable.  The value of MWSLSFX can be
##	changed if the MainWin file suffix does not match the Ingres file
##	suffix.
##  17-Apr-2001 (ahaal01)
##	Added APACHE_API_FLAG for building Apache on AIX (rs4_us5) and
##	corrected directory paths for included system files.
##  09-May-2001 (ahaal01)
##	Added "-D_BSD=43" to CCMACH for AIX (rs4_us5).
##  01-dec-2000 (toumi01)
##	For now, remove "-lpthread" ld suffix for ibm_lnx, until POSIX
##	threading issues of dmfacp looping and stack limits being hit
##	during run_all xa testing are resolved.
##  11-dec-2000 (toumi01)
##	For now, remove "-lpthread" ld suffix for int_lnx, until POSIX
##	threading issues of lar12 hanging and stack limits being hit
##	during run_all xa testing are resolved:
##		< suffix="-lpthread -lm -lc -lcrypt -ldl"
##		> suffix="-lm -lc -lcrypt -ldl"
##  13-Dec-2000 (hweho01)
##	Set up LEVEL1_OPTIM for Compaq 64-bit (axp_osf).
##  22-dec-2000 (toumi01)
##	Replace "-lpthread" to enable *_lnx OS threading.
##	In doing so, remove test for $conf_libc6, since we are no longer
##	building Ingres for Linux based on libc5.
##	26-Jan-2001 (hanje04)
##	    Added -fwritable-string for Linux (Intel and S390)
##  18-Apr-2001 (bonro01)
##      Added APACHE_API_FLAG for building Apache.
##  25-Apr-2000 (wansh01)
##      Added NETSCAPE_API_FLAG for sgi_us5.
##  08-May-2001 (hweho01)
##      Added "-qchars=signed" to CCMACH list for ris_u64 platform.  
##      AIX compiler defined char as unsigned by default, Ingres expects 
##      them to be signed. 
##  11-june-2001 (legru01)
##      Added NETSCAPE_API_FLAG for sos_us5
##     22-Mar-2001 (linke01)
##     Added APACHE_API_FLAG for sui_us5  
##     11-Mar-2001 (linke01)
##         APACHE_API_FLAG was added to usl_us5
##  22-Jun-2001 (linke01)
##     Added NETSCAPE_API_FLAG for sui_us5
##  18-Jul-2001 (hanje04)
##	Added platform-specific information for building Xerces-C stuff.
##  18-Jul-2001 (somsa01)
##	For HP, we need to add '-c' to CXX_DEFS.
##  26-Jul-2001 (somsa01)
##	Modified the setting of XERC_DEFS for HP.
##  26-Jul-2001 (somsa01)
##	Once again modified the setting of CXX_DEFS and XERC_LIBS for HP.
##  28-Jul-2001 (hanje04)
##	Removed line breaks from XERC_DEFS and added /usr/lib/libc.so.1 for
##	Solaris.
##  18-jul-2001 (stephenb)
##	From allmi01 originals, add support for i64_aix
##  19-jul-2001 (stephenb)
##	Define ansi for i64_aix
##  18-aug-2001 (somsa01)
##	Corrected setting of MWSLSFX for HP.
##  08-Aug-2001 (devjo01)
##	Add CXX for axp_osf (Tru64).
##   02-Nov-2001 (loera01) Bug 106266 
##     Create internal SFX variable to set default value of MWSLSFX.
##  16-nov-2001 (somsa01)
##	Modified the setting of CXX_DEFS for HP to use '+Z'.
##  21-nov-2001 (devjo01)
##	Add -ieee for Tru64 CCMACH settings.
##  11-dec-2001 (somsa01)
##	Porting changes for hp2_us5.
##  14-Jan-2002 (hweho01)
##      Added settings to build ICE and other XML tools for rs4_us5 platform.
##  05-Feb-2002 (hanje04)
##	Corrected CXX info for ibm_lnx
##  19-Feb-2002 (bonro01)
##	Added usl_us5 specific information for building Xerces-C stuff.
##  04-mar-2002 (somsa01)
##	Changes for 64-bit add-on package for Solaris.
##  1-Mar-2002 (bonro01)
##	Added special link options for icesvr with LD_ICE_FLAGS.
##	This fixes a problem that caused user authentication errors in ICE.
##	Lib ddf was linked with both icesvr and oiddi, and this caused the
##	problem.  The solution was to remove -lddf from the oiddi shared
##	library and add link options to export the symbols from the icesvr
##	executable.
##  03-Dec-2001 (hanje04)
##	Add support for IA64 Linux (i64_lnx), as other linuxs except CCMACH
##	which requires -fPIC to relsove shared library linker problems.
##  15-Mar-2002 (hweho01)
##      Added -lC_r for rs4_us5 XERC_LIBS setting.
##  5-Jun-2002  (hweho01)
##      Added -lc_r for rs4_us5 XERC_LIBS setting.
##  15-Oct-2002 (somsa01)
##	Added define of CXXLDMACH, used only on HP-UX for now. This define
##	denotes link flags for C++ programs.
##  17-Oct-2002 (bonro01)
##	Changes for 64-bit add-on package for SGI.
##	02-Nov-2002 (hanje04)
##	    Added support for AMD x86-64 Linux, genericise Linux defines
##	    where ever possible.
##  08-Nov-2002 (somsa01)
##	If config64 is set, set VERS64 to it.
##  21-Oct-2002 (hweho01)
##      Added changes for AIX 32/64 hybrid build.
##  02-Dec-2002 (somsa01)
##	For HP 11.x, corrected setting of APACHE_API_FLAG and MAINWIN_EX_LOC.
##	01-May-2003 (yeema01)
##	    Added the -xCC flag for su4_us5 for conf_W4GL (OpenROAD) MAINWIN
##	    build.
##  13-May-2003 (bonro01)
##	Added changes for IA64 HP (i64_hpu)
##  28-Feb-03 (gordy)
##	Added HP linker flag to recursively search libraries to build IIGCD.
##  10-Apr-2003 (hanal04) 109389 INGSRV 2041
##      Added -ieee to CCMACH for axp_osf. This prevents use of
##      CMPTLT which traps on a denorm float. Instead -ieee directs
##      the compiler to use FBGE which will not trap denorm floats.
##  25-Jun-2003 (bonro01)
##      Changed shared library suffix to .sl
##  31-Oct-2003 (bonro01)
##	SGI changes for 32/64 bit builds, and Xerces support.
##  25-Sep-2003 (hanje04)
##	Add large file support for Linux
##  03-dec-2003 (devjo01)
##	Specify -xarch=v8plus for su4_us5 ASMACH to allow 'cas' opcode.
##	Support for Sparc8 & below processors no longer required.
##  05-jan-2004 (devjo01)
##	Add int_lnx specific define for HDRMACH, so OpenDLM header files
##	are included.
##  30-Jan-2004 (hanje04)
##	If HA_ROOT is defined use it to add the OpenDLM header files 
##	location to HDRMACH. Otherwise use $ING_SRC/ha
##	04-mar-2004 (somsa01)
##	    Added linker libraries specific to i64_lnx.
##	02-apr-2004 (somsa01)
##	    Added CXX64_DEFS defines for su4_us5, hp8_us5.
##      01-Aug-2004 (hanch04)
##          First line of a shell script needs to be a ":" and
##          then the copyright should be next.
##	29-Aug-2004 (hanje04)
##	    Add -lgcc_s so link flags for Linux
##	25-Dec-2004 (shaha03)
##	    SIR #113754 Changed compilation options for HP-UX itanium 
##	    for hybrid build.
##	04-Mar-2005 (hanje04)
## 	    SIR 114034
##	    Add support for reverse hybrid builds.
##	15-Mar-2005 (bonro01)
##	    Add support for Solaris AMD64 a64_sol
##	28-Mar-2005 (shaha03)
##	    Revert back the changes done for Hybrid build, and add support
##	    for reverse hybrid build for HP-UX itanium.
##      18-Apr-2005 (hanje04)
##          Add support for Max OS X (mg5_osx).
##          Based on initial changes by utlia01 and monda07.
##	29-Apr-2005 (kodse01)
##	    Variable vers should be $config64 on reverse-hybrid platforms.
##	23-Aug-2005 (schka24)
##	    Fix libc defn for Solaris 10, works on earlier ones too.
##	09-May-2005 (hanal04) Bug 114470
##          Submission of open files on nc4.us5 build. Change ld options to
##          prevent build errors.
##	25-Oct-2005 (hanje04)
##	    Remove -fwritable-strings from CCMACH on Linux. Not supported 
##	    against GCC 4.0
##	26-Jul-2006 (bonro01)
##	    Remove unrecognized 64-bit link option
##       6-Nov-2006 (hanal04) SIR 117044
##          Add int.rpl for Intel Rpath Linux build.
##	23-Mar-2007 (hanje04)
##	    SIR 117985
##	    Add support for 64bit PowerPC Linux (ppc_lnx)
##	31-Jul-2007 (hanje04)
##	    BUG 118869
##	    Reference libgcc_s.so.1 directly for int_lnx in LDLIBMACH etc to
##	    avoid a linker bug when linking on x86_64 Linuxes.


TMP=/tmp/libc.nm
trap 'rm -f $TMP' 0 1 2 13 15

exec 1> default.h

. readvers
for o in $opts
{
  echo "#define conf_$o TRUE"
}

vers=$config
[ "$config64" ] && vers64=$config64
[ "$conf_ADD_ON32" ] && vers=$config64

case $vers in
  [a-z][a-z0-9][a-z0-9]_us5) ;;
  [0-9][a-z0-9][a-z0-9]_us5) vers=x$vers;;
  [a-z][a-z0-9][a-z0-9]_u42) ;;
  [0-9][a-z0-9][a-z0-9]_u42) vers=x$vers;;
  [a-z][a-z0-9][a-z0-9]_u43) ;;
  [0-9][a-z0-9][a-z0-9]_u43) vers=x$vers;;
  [a-z][a-z0-9][a-z0-9]_osf) ;;
  [0-9][a-z0-9][a-z0-9]_osf) vers=x$vers;;
  [a-z][a-z0-9][a-z0-9]_ulx) ;;
  [0-9][a-z0-9][a-z0-9]_ulx) vers=x$vers;;
  [a-z][a-z0-9][a-z0-9]_ul5) ;;
  [0-9][a-z0-9][a-z0-9]_ul5) vers=x$vers;;
  [a-z][a-z0-9][a-z0-9]_lnx) ;;
  [0-9][a-z0-9][a-z0-9]_lnx) vers=x$vers;;
  [a-z][a-z0-9][a-z0-9]_rpl) ;;
  [0-9][a-z0-9][a-z0-9]_rpl) vers=x$vers;;
  [a-z][a-z0-9][a-z0-9]_u64) ;;
  [0-9][a-z0-9][a-z0-9]_u64) vers=x$vers;;
  [a-z][a-z0-9][a-z0-9]_aos) ;;
  [a-z][a-z0-9][a-z0-9]_prm) ;;
  [a-z][a-z0-9][a-z0-9]_aix) ;;
  [0-9][a-z0-9][a-z0-9]_aix) vers=x$vers;;
  [a-z][a-z0-9][a-z0-9]_hpu) ;;
  [a-z][a-z0-9][a-z0-9]_sol) ;;

  [a-z][a-z0-9][a-z0-9]_cmw) ;;
  [a-z][a-z0-9][a-z0-9]_bls) ;;
  [a-z][a-z0-9][a-z0-9]_ues) ;;
  [a-z][a-z0-9][a-z0-9]_osx) ;;
                      *_kit) ;;
                    sqs_ptx) ;;
                          *) echo "$0: bad config, should be xxx_uyy >&2"
                             exit 1 ;;
esac

# Only do these nm's once; just grep the output.
case $vers in
  dr6_us5|\
  nc4_us5|\
  rmx_us5|\
  pym_us5|\
   rux_us5) LIBC=/usr/ccs/lib/libc.a ;;
   usl_us5) LIBC=/usr/ccs/lib/libc.so ;;
   sgi_us5) LIBC=/usr/lib32/mips3/libc.so ;;
  int_lnx|\
  int_rpl|\
  ibm_lnx|\
  i64_lnx|\
  axp_lnx) LIBC=/usr/lib/libc.a ;;
  a64_lnx) LIBC=/usr/lib64/libc.a ;;
  ppc_lnx) LIBC=/usr/lib64/libc.a ;;
  i64_aix) LIBC=/usr/lib/ia64l64/libc.so ;;
  i64_hpu) LIBC=/usr/lib/hpux64/libc.so ;;
  su9_us5|\
  su4_us5|\
  a64_sol) LIBC=/usr/lib/libc.so ;;
  mg5_osx) LIBC=/usr/lib/libc.dylib ;;
         *) LIBC=/lib/libc.a ;;
esac

case $vers in
        *) nm $LIBC > $TMP ;;
esac

[ "$ING_VERS" ] && { echo "BAROQUE = 1" ; echo "#define BAROQUE" ; }

[ -n "$PRIVATE_TOOLS" ] &&
{ echo "PRIVATETOOLS = 1" ; echo "#define PRIVATETOOLS" ; }

# Set XTERMLIB, WINSYSLIB, XNETLIB, and INGNETLIB.
# All these are related to the use of X11 and X11's network interface.
#
# XTERMLIB and XNETLIB must be set for WindowView.  Use the full pathname
# if you are using your own X11 libraries.  The order of the libraries must
# be Xaw, Xmu, Xt and X11.
#
# WINSYSLIB must be set for Windows 4GL.  Use the full pathname if you
# are using your own X11 libraries.  The order of the libraries must be
# Xm, Xt and X11.
#
# INGNETLIB lists libraries needed for platform specific protocol drivers
# used when linking the INGRES/NET Comm Server (iigcc).
#
# CCPICFLAG must be set to the compiler options needed for generating code
# for shared libraries ("-PIC" on sun4).
#
# NETSCAPE_API_FLAG must be set for platforms that the netscape enterprise
# server supports
# (eg "-DNET_SSL -DSOLARIS -D_REENTRANT -DMCC_HTTPD -DXP_UNIX -DSPAPI20" on
# solaris) Check the netscape makefile in the netscape nsapi examples directory
# on your netscape installation
#
# APACHE_API_FLAG must be set for platforms that the Apache web server
# supports ("-DHPUX10 +z" on HP-UX 10.x). Check the Apache Configuration file
# in the Apache web server source for your particular platform's appropriate
# settings.
#

case $vers in
  dg8_us5 | dgi_us5)
        echo "# define WINSYSLIB_SET"
        echo "XTERMLIB = /usr/opt/X11/lib/libXaw.a /usr/opt/X11/lib/libXmu.a /usr/opt/X11/lib/libXt.a /usr/opt/X11/lib/libX11.a -ltermcap"
        echo "WINSYSLIB = /usr/opt/X11/lib/libXaw.a /usr/opt/X11/lib/libXext.a /usr/opt/X11/lib/libXm.a /usr/opt/X11/lib/libXt.a /usr/opt/X11/lib/libX11.a /usr/lib/libPW.a"
        echo "XTERMHDR = /usr/include/X11 /usr/include/X11/Xaw /usr/include/X11/Xmu"
        echo 'XNETLIB = /usr/opt/X11/lib/libXext.a'
        echo 'XMLIB =  /usr/opt/X11/lib/libXm.a'
        echo 'XAWLIB = /usr/opt/X11/lib/libXaw.a'
        echo 'XMULIB = /usr/opt/X11/lib/libXmu.a'
        echo 'XTLIB  = /usr/opt/X11/lib/libXt.a'
        echo 'XLIB   = /usr/opt/X11/lib/libX11.a'
        echo 'XPW    =/usr/lib/libPW.a'
        echo 'LIBDL  =/usr/lib/libdl.a'
        echo 'TERMCAPLIB = -ltermcap'
        echo "CCPICFLAG = -K PIC"
        echo "NETSCAPE_API_FLAG = -DXP_UNIX"
        ;;
 hp8_us5|\
 hpb_us5|\
 i64_hpu|\
 hp2_us5)
           echo "XTERMLIB ="
           echo "XNETLIB ="
	   [ -d /usr/include/sna ] && ingnetlib="-L/opt/sna/lib -lappc.sl -lcsv.sl -lmgr.sl"
           echo "INGNETLIB = $ingnetlib"
           echo "NETSCAPE_API_FLAG = -DNET_SSL -DHPUX -DXP_UNIX +z"
	   if [ "$vers" = "hp8_us5" ]
	   then
	       echo "APACHE_API_FLAG = -DHPUX10 +z"
	       echo "MAINWIN_EX_LOC = run/ux10"
	   else
	       echo "APACHE_API_FLAG = -DHPUX11 +z"
	       echo "MAINWIN_EX_LOC = run/ux11"
	   fi
	
           ;;
 hp8_bls)
	   echo "XTERMLIB = /usr/lib/X11R4/libXaw.sl /usr/lib/X11R4/libXmu.sl /usr/lib/X11R4/libXt.a /usr/lib/X11R4/libX11.a -lcurses"
           echo "XNETLIB ="
           echo '#define R_INGRES_XLIB'
           ;;
  nc4_us5) echo "XTERMLIB = /usr/lib/libXaw.so /usr/lib/libXmu.so /usr/lib/libXt.so /usr/lib/libX11.so /usr/lib/libXext.so -lcurses"
           echo "XNETLIB ="
           echo "NETSCAPE_API_FLAG = -DNET_SSL -DXP_UNIX"
           ;;
  sos_us5)
           echo "XTERMLIB = -lXaw -lXmu -lXt -lX11 -ltermcap -lPW"
           echo "XNETLIB ="
	   echo "XTERMHDR = /usr/include/X11"
           echo "WINSYSLIB = /usr/lib/libXm.so /usr/lib/libXt.so /usr/lib/libX11.so"
            echo "NETSCAPE_API_FLAG =  -DXP_UNIX"
           ;;
  ris_us5)
        echo "XTERMLIB = /usr/lpp/X11/Xamples/lib/Xaw/libXaw.a /usr/lpp/X11/lib/libXmu.a /usr/lpp/X11/lib/R5/libXt.a /usr/lpp/X11/lib/R5/libX11.a -lcur"
	;;
  rs4_us5) echo "XTERMLIB = /usr/lpp/X11/Xamples/lib/Xaw/libXaw.a /usr/lpp/X11/Xamples/lib/Xmu/libXmu.a /usr/lpp/X11/lib/R6/libXt.a /usr/lpp/X11/lib/R6/libXext.a /usr/lpp/X11/lib/R6/libX11.a /usr/lib/libcurses.a /usr/lib/libcur.a"
           echo "INGNETLIB = -ltli_r"
           echo "XNETLIB ="
           echo "MAINWIN_EX_LOC = run/aix4"
           [ "$conf_X11R5" -o "$conf_X11R4" ] && echo '#define R_INGRES_XLIB'
           echo "NETSCAPE_API_FLAG = -DNET_SSL -DAIX -D_REENTRANT -DMCC_HTTPD -DXP_UNIX -DSPAPI20"
           echo "APACHE_API_FLAG = -DAIX=43 -U__STR__"
           ;;
  ris_u64) echo "XTERMLIB = /usr/lpp/X11/Xamples/lib/Xaw/libXaw.a /usr/lpp/X11/Xamples/lib/Xmu/libXmu.a /usr/lpp/X11/lib/R6/libXt.a /usr/lpp/X11/lib/R6/libXext.a /usr/lpp/X11/lib/R6/libX11.a /usr/lib/libcurses.a /usr/lib/libcur.a"
           echo "INGNETLIB = -ltli_r"
           echo "XNETLIB ="
           [ "$conf_X11R5" -o "$conf_X11R4" ] && echo '#define R_INGRES_XLIB'
           echo "NETSCAPE_API_FLAG = -DNET_SSL -DAIX -D_REENTRANT -DMCC_HTTPD -DXP_UNIX -DSPAPI20"
           echo "APACHE_API_FLAG = -DAIX=43 -U__STR__"
           ;;
  i64_aix) echo "XTERMLIB = /usr/lpp/X11/Xamples/lib/Xaw/libXaw.a /usr/lpp/X11/Xamples/lib/Xmu/libXmu.a /usr/lpp/X11/lib/R6/libXt.a /usr/lpp/X11/lib/R6/libXext.a /usr/lpp/X11/lib/R6/libX11.a /usr/lib/ia64l64/libcurses.so /usr/lib/libcur.a"
           echo "INGNETLIB ="
           echo "XNETLIB ="
           [ "$conf_X11R5" -o "$conf_X11R4" ] && echo '#define R_INGRES_XLIB'
           echo "NETSCAPE_API_FLAG = -DNET_SSL -DAIX -D_REENTRANT -DMCC_HTTPD -DXP_UNIX -DSPAPI20"
           echo "APACHE_API_FLAG = -DAIX=43 -U__STR__"
           ;;
  sgi_us5) echo "XTERMLIB = /usr/lib32/libXaw.so /usr/lib32/libXmu.so /usr/lib32/libXt.so /usr/lib32/libX11.so /usr/lib32/libcurses.so "
           echo "XNETLIB ="
  	   echo "WINSYSLIB = /usr/lib32/libXm.so /usr/lib32/libXt.so /usr/lib32/libX11.so "
           echo "NETSCAPE_API_FLAG = -DXP_UNIX"
           ;;

  su4_u42) echo "XTERMLIB = /usr/openwin/lib/libXaw.a /usr/openwin/lib/libXmu.a /usr/openwin/lib/libXt.a /usr/openwin/lib/libX11.a -ltermcap"
           echo "XNETLIB ="
           [ ! "$conf_OL" ] && echo '#define R_INGRES_XLIB'
           echo "INGNETLIB = /usr/sunlink/sunlink.a -lnsl"
           ;;
su4_cmw)	echo "XTERMLIB = /usr/openwin/lib/libXaw.a /usr/openwin/lib/libXmu.a /usr/openwin/lib/libXt.a /usr/openwin/lib/libX11.a /lib/libtermcap.a"
		echo "XNETLIB ="
		echo '# define R_INGRES_XLIB'
		echo "INGNETLIB = /usr/sunlink/sunlink.a"
		;;
  su4_us5) echo "XTERMLIB = /usr/openwin/lib/libXaw.so /usr/openwin/lib/libXmu.so /usr/openwin/lib/libXt.so /usr/openwin/lib/libX11.so /usr/ucblib/libucb.a"
           echo "XTERMHDR = /usr/openwin/include"
           echo "NETSCAPE_API_FLAG = -DNET_SSL -DSOLARIS -D_REENTRANT -DMCC_HTTPD -DXP_UNIX -DSPAPI20"
           echo "APACHE_API_FLAG = -DSOLARIS2 -D_REENTRANT"
	   echo "MAINWIN_EX_LOC = run/sunos5"
           echo "XNETLIB ="
	   echo "INGNETLIB = /opt/SUNWconn/lib/libappc_api.a"
           ;;
  su9_us5) echo "XTERMLIB = /usr/openwin/lib/libXaw.so /usr/openwin/lib/libXmu.so /usr/openwin/lib/libXt.so /usr/openwin/lib/libX11.so /usr/ucblib/libucb.a"
           echo "XTERMHDR = /usr/openwin/include"
           echo "XNETLIB ="
	    echo "NETSCAPE_API_FLAG = -DNET_SSL -DSOLARIS -D_REENTRANT -DMCC_HTTPD -DXP_UNIX -DSPAPI20"
           ;;
  a64_sol) echo "XTERMLIB = /usr/openwin/lib/libXaw.so /usr/openwin/lib/libXmu.so /usr/openwin/lib/libXt.so /usr/openwin/lib/libX11.so /usr/ucblib/libucb.a"
           echo "XTERMHDR = /usr/openwin/include"
           echo "XNETLIB ="
	    echo "NETSCAPE_API_FLAG = -DNET_SSL -DSOLARIS -D_REENTRANT -DMCC_HTTPD -DXP_UNIX -DSPAPI20"
           ;;
  sui_us5) echo "XTERMLIB = /usr/openwin/lib/libXaw.so /usr/openwin/lib/libXmu.so /usr/openwin/lib/libXt.so /usr/openwin/lib/libX11.so /usr/ucblib/libucb.a"
           echo "XTERMHDR = /usr/openwin/include"
           echo "XNETLIB ="
	   echo "INGNETLIB ="
           echo "APACHE_API_FLAG = -DSOLARIS2=260 -DUSE_EXPAT"
           echo "NETSCAPE_API_FLAG = -DXP_UNIX"     
           ;;
  sqs_ptx) echo "XTERMLIB = -lXaw -lXmu -lXt -lX11 -ltermcap"
           echo "XNETLIB ="
           echo "CCPICFLAG = -K PIC"
           echo "NETSCAPE_API_FLAG = -DXP_UNIX"
           ;;
 dr6_us5) echo "XNETLIB = "
  	  echo "XTERMLIB = /usr/X/lib/libXaw.so /usr/X/lib/libXmu.so /usr/X/lib/libXt.so /usr/X/lib/libX11.so /usr/X/lib/libXext.so /usr/ucblib/libtermcap.a"
          # echo "XTERMLIB = -L/usr/X/lib -lXaw -lXmu -lXt -lX11 -lXext -ltermcap"
          echo "#define WINSYSLIB_SET"
          echo "WINSYSLIB = /usr/X/lib/libXm.a /usr/X/lib/libXt.a /usr/X/lib/libX11.a"
           ;;
 usl_us5) echo "XNETLIB ="
	  echo "XTERMLIB = -lcurses -lXaw -lXext -lXmu -lXt -lX11 -lSM -lICE"
	  echo "# define WINSYSLIB_SET"
	  echo "WINSYSLIB = -lXm -lXt -lX11"
	  echo "CCPICFLAG = -K PIC"
          echo "APACHE_API_FLAG = -DUW=700 -DUSE_HSREGEX -DUSE_EXPAT"
	  echo "NETSCAPE_API_FLAG = -DNET_SSL -DUNIXWARE -K PIC -DUnixWare -b elf -D_REENTRANT -DMCC_HTTPD -DXP_UNIX -DSPAPI20"
	  ;;
 axp_osf) echo "XTERMLIB = -lXaw -lXmu -lXt -lX11 -ltermcap -lSM -lXext -lICE"
	  echo "XNETLIB = -ldnet_stub"
	  echo "WINSYSLIB = /usr/lib/libXm.a \
/usr/lib/libXt.a /usr/lib/libX11.a"
          echo "NETSCAPE_API_FLAG = -DIS_64 -ieee_with_inexact -DXP_UNIX -DOSF1"
          echo "APACHE_API_FLAG = -DOSF1"
	  ;;
 rux_us5|\
 rmx_us5) echo "XTERMLIB = /opt/X/lib/libXaw.a /opt/X/lib/libXmu.a /opt/X/lib/libXt.a /opt/X/lib/libX11.a /opt/X/lib/libXext.a -ltermcap -lcmd"
          echo "XNETLIB ="
          echo "WINSYSLIB = -lXm -lXaw -lXt -lX11"
	  echo "NETSCAPE_API_FLAG = -DXP_UNIX"
          echo "APACHE_API_FLAG = -DSVR4 -D_XPG_IV -DHAS_DLFCN -DUSE_MMAP_FILES -DUSE_SYSVSEM_SERIALIZED_ACCEPT -DNEED_UNION_SEMUN -DUSE_HSREGEX -DUSE_EXPAT"
          ;;
 pym_us5) echo "XTERMLIB = -lXaw -lXmu -lXt -lX11 -ltermcap -lcmd"
          echo "XNETLIB ="
          echo "WINSYSLIB = -lXm -lXaw -lXt -lX11"
          ;;
 ts2_us5) echo "XTERMLIB = -lXaw -lXmu -lXt -lX11 -ltermcap"
          echo "XNETLIB ="
          echo "WINSYSLIB = -lXm -lXaw -lXt -lX11"
          ;;
 int_lnx|\
 int_rpl|\
 ibm_lnx|\
 i64_lnx|\
 axp_lnx) echo "XTERMLIB = /usr/X11R6/lib/libXaw.so /usr/X11R6/lib/libXmu.so /usr/X11R6/lib/libXt.so /usr/X11R6/lib/libX11.so /usr/lib/libtermcap.so"
          echo "WINSYSLIB = /usr/X11R6/lib/libXmu.so /usr/X11R6/lib/libXt.so /usr/X11R6/lib/libX11.so"
          echo "XNETLIB ="
	  echo "NETSCAPE_API_FLAG = -DXP_UNIX"
	  echo "APACHE_API_FLAG = -DLINUX=2"
          ;;
 a64_lnx|\
 ppc_lnx) echo "XTERMLIB = /usr/X11R6/lib64/libXaw.so /usr/X11R6/lib64/libXmu.so /usr/X11R6/lib64/libXt.so /usr/X11R6/lib64/libX11.so /usr/lib64/libtermcap.so"
          echo "XNETLIB ="
	  echo "NETSCAPE_API_FLAG = -DXP_UNIX"
	  echo "APACHE_API_FLAG = -DLINUX=2"
	  ;;
esac

echo "VERS = $vers"
[ "$vers64" ] && echo "VERS64 = $vers64"
echo "#define $vers"

# hpb_us5 is the new config string for HP-UX 11.0 (32-bit).  The following
# also defines hp8_us5 to enable most HP specific settings.  Care must be
# taken when the need arises to differentiate them

[ $vers = "hp2_us5" ] && echo "#define hpb_us5"
[ $vers = "hpb_us5" ] && echo "#define hp8_us5"

# HDRMACH specials

case $vers in
  dg8_us5 | dgi_us5) echo "HDRMACH = /usr/include/X11 /usr/include/X11/Xmu /usr/include/X11/Xaw /usr/include/netinet";;
  i64_hpu) echo "HDRMACH = /usr/include/X11R6";;
  hpb_us5|\
  hp2_us5) echo "HDRMACH = /usr/include/sna";;
  hp8_us5) if [ "$conf_DBL" ]
	   then
		HDRMACH="/usr/include/X11R5 /usr/include/Motif1.2"
	   else
		HDRMACH="/usr/contrib/X11R4/include /usr/contrib/X11R5/include /usr/include/X11R6"
	   fi
	   [ -d /usr/include/sna ] && HDRMACH="$HDRMACH /usr/include/sna"
	   echo "HDRMACH = $HDRMACH";;
  hp8_bls) echo "HDRMACH = /usr/include/X11R4";;
  pyr_us5) echo "HDRMACH = /usr/.attinclude /usr/.ucbinclude";;
  rux_us5|\
  rmx_us5) echo "HDRMACH = /opt/X/include";;
  sgi_us5) echo "HDRMACH = /usr/include/X11";;
  axp_osf) echo "HDRMACH = /usr/sys/include /usr/sys/BINARY";;
  int_lnx|\
  int_rpl) if [ "$HA_ROOT" ]; then
	   echo "HDRMACH = $HA_ROOT/opendlm/src/include"
	   else
	   echo "HDRMACH = $ING_SRC/ha/opendlm/src/include"
	   fi
	   ;;
    su4_*) if [ "$conf_OL" ] ; then
             incs='$(OPENWINHOME)/include/Xol $(OPENWINHOME)/include'
             echo "HDRMACH = $incs /usr/include/netinet"
           else
             echo "HDRMACH = /usr/include/netinet"
           fi
           ;;
esac

# N.B.: avoid adding generic entries to HDRMACH.
# Sun compiler currently imposes a limit on -I options.
[ -d /usr/netinclude ] && echo "HDRMACH += /usr/netinclude"

# CPP specials
# CPP should be set to the name of the C pre-processor on the system.
case $vers in 
  rmx_us5|\
  rux_us5|\
  pym_us5) echo "CPP = /usr/ccs/lib/cpp";;
  su4_us5|\
  su9_us5|\
  sui_us5) echo "CPP = /usr/ccs/lib/cpp";;
  sgi_us5) echo "CPP = /usr/lib32/cmplrs/cpp";;
  dr6_us5) echo "CPP = /usr/ccs/lib/cpp";;
  i64_aix) echo "CPP = /usr/ccs/lib/cpp";;
  mg5_osx) echo "CPP = /usr/bin/cpp";;
        *) echo "CPP = /lib/cpp";;
esac

# CCMACH specials
case $vers in
   axp_osf) echo "CCMACH = -signed -D_BSD -pthread -ieee";;
   dg8_us5) echo "CCMACH = -Xa";;
   dgi_us5) echo "CCMACH = -Xa -DSYSV -DSVR4 -DSVR4_0 -D_POSIX4_DRAFT_SOURCE";;
   usl_us5) echo "CCMACH = -Xt -DSYSV -DSVR4 -DSVR4_0 -D_REENTRANT -K thread,pentium";;
   sos_us5) echo "CCMACH = -Xt -b elf -a ods30";;
   sgi_us5) echo "CCMACH = -n32 -mips3 -signed -common -D_SGI_SOURCE -D_SGI_MP_SOURCE -D_SGI_REENTRANT_FUNCTIONS -D_OLD_TERMIOS -woff 1116 -woff 1009 -woff 1174 -woff 1552"
   	    echo "CCMACH_DBG = -g -n32 -mips3 -signed -common -D_SGI_SOURCE -D_SGI_REENTRANT_FUNCTIONS -D_OLD_TERMIOS -woff 1116 -woff 1009 -woff 1174 -woff 1552"
            echo "CCMACH64 = -DADD_ON64 -64 -mips4 -G0 -signed -common -D_SGI_SOURCE -D_SGI_MP_SOURCE -D_SGI_REENTRANT_FUNCTIONS -D_OLD_TERMIOS -woff 1116 -woff 1009 -woff 1174 -woff 1552"
	    ;;
   dr6_us5) echo "CCMACH = -DSYSV -DSVR4 -DSVR4_0"
	    echo "CCMACH_DBG = -g -DSYSV -DSVR4 -DSVR4_0"
	    ;;
   hp8_bls) compflags="-D_INCLUDE_HPUX_SOURCE -D_INCLUDE_XOPEN_SOURCE -D_INCLUDE_POSIX_SOURCE -D_INCLUDE_AES_SOURCE -DSEC_MSIX_IPC"
	    echo "CCMACH = $compflags +DA1.0 +DS1.0 "
	    ;;
   hp2_us5) [ ! "$conf_ANSIC" ] && compflags=" -W  p,-H375000"
	    [ "$conf_B64" ] && compflags="$compflags -D_LARGEFILE64_SOURCE"
            compflags="+e +Olibcalls -D_HPUX_SOURCE -D_POSIX_C_SOURCE=199506L +DChpux +DD64 +M2 +DS2.0a $compflags"
            echo "CCMACH = $compflags"
            echo "CCMACH_DBG = $compflags -g"
            ;;
   hpb_us5) [ ! "$conf_ANSIC" ] && compflags=" -W  p,-H375000"
	    [ "$conf_B64" ] && compflags="$compflags -D_LARGEFILE64_SOURCE"
            compflags32="+e +Olibcalls -D_HPUX_SOURCE -D_POSIX_C_SOURCE=199506L +DAportable +DS2.0a $compflags"
            echo "CCMACH = $compflags32"
	    echo "CCMACH64 = -Dhp2_us5 -DADD_ON64 +e +Olibcalls -D_HPUX_SOURCE -D_POSIX_C_SOURCE=199506L +DChpux +DD64 +M2 +DS2.0a $compflags"
            echo "CCMACH_DBG = $compflags32 -g"
	    echo "CCMACH_DBG64 = -Dhp2_us5 -DADD_ON64 +e +Olibcalls -D_HPUX_SOURCE -D_POSIX_C_SOURCE=199506L +DChpux +DD64 +M2 +DS2.0a $compflags -g"
            ;;
   hp8_us5) [ ! "$conf_ANSIC" ] && compflags=" -W  p,-H375000"
	    [ "$conf_B64" ] && compflags="$compflags -D_LARGEFILE64_SOURCE"
            if [ "$conf_W4GL" ] ; then 
              compflags="-D_INCLUDE_HPUX_SOURCE -D_INCLUDE_XOPEN_SOURCE -D_INCLUDE_POSIX_SOURCE -D_INCLUDE_AES_SOURCE +DA1.0 +DS1.0 -J $compflags"
            else
	      compflags="+e -D_HPUX_SOURCE +DA1.1 +DS1.1b $compflags"
            fi
            echo "CCMACH = $compflags"
            echo "CCMACH_DBG = $compflags -g"
            ;;
   i64_hpu) [ ! "$conf_ANSIC" ] && compflags=" -W  p,-H375000"
            compflags="-D_HPUX_SOURCE -D_POSIX_C_SOURCE=199506L -ext $compflags"
            echo "CCMACH = +DD64 $compflags"
            echo "CCMACH_DBG = +DD64 $compflags -g"
	    echo "CCMACH32 = $compflags"
	    echo "CCMACH_DBG32 = $compflags -g"
            ;;
   ris_us5) echo "CCMACH = -D_ALL_SOURCE -qnoro -qhssngl -qpgmsize=l -qmaxmem=-1 -Q -bI:/usr/lib/FCM/lowsys.exp" 
	    echo "CCMACH_DBG = -g -D_ALL_SOURCE -qnoro -qhssngl -qpgmsize=l -qmaxmem=-1 -qchars=signed -Q -bI:/usr/lib/FCM/lowsys.exp" 
	    ;;
   sqs_ptx)
	echo "CCMACH = -DNO_XPG3_HDR -D_SOCKET_VERSION=11"
	echo "CCMACH_DBG = -g -DNO_XPG3_HDR -D_SOCKET_VERSION=11"
	;;
   rs4_us5) echo "CCMACH = -qnoro -qhssngl -qchars=signed -qmaxmem=-1 -Q -D_BSD=43"
            echo "CCMACH64 = -DADD_ON64 -q64 -qnoro -qhssngl -qchars=signed -qmaxmem=-1 -Q -D_BSD=43" ;;
   ris_u64) echo "CCMACH = -q64 -qnoro -qhssngl -qchars=signed -qmaxmem=-1 -Q -D_BSD=43" ;;
   i64_aix) echo "CCMACH = -q64 -qlanglvl=ansi -qnoro -qhssngl -qmaxmem=-1 -Q "
            echo "CCMACH_DBG = -g -q64 -qlanglvl=ansi -qnoro -qhssngl -qmaxmem=-1 -Q " ;;
   su4_us5| \
   sui_us5) if [ "$conf_W4GL" ] ; then
                 echo "CCMACH = -xCC -DSVR4 -D_SVID_GETTOD -mt"
            else
                 echo "CCMACH = -DSVR4 -D_SVID_GETTOD -mt"
                 echo "CCMACH64 = -Dsu9_us5 -DADD_ON64 -xarch=v9 -DSVR4 -D_SVID_GETTOD -mt"
            fi
            ;;
   su9_us5) echo "CCMACH = -xarch=v9 -DSVR4 -D_SVID_GETTOD -mt" ;;
   a64_sol)
	echo "CCMACH = -DSVR4 -D_SVID_GETTOD -mt -D_LARGEFILE64_SOURCE"
	echo "CCMACH64 = -DSVR4 -D_SVID_GETTOD -DADD_ON64 -xarch=generic64 -mt"
	;;
   su4_u42) [ "$conf_OL" ] && defs=-DXTK_OLIT
            [ "$conf_65TOOLS_64SOURCE" ] && compflags="-DGCF64"
            echo "CCMACH = -Dsparc -pipe $compflags $defs"
            ;;
   nc4_us5) echo "CCMACH = -w -DSVR4 -Hnocopyr" ;;
   su4_cmw) echo "CCMACH = -DSunOS_CMW" ;;
   rmx_us5) echo "CCMACH = -W0 -Xt -Ksigned -Kpic -KOlimit:4000 -Kr4000";;
   rux_us5) echo "CCMACH = -Kthread -Ksigned -Kmips3 -FOlimit,4000"
            echo "CCMACH_DBG = -g -Kthread -Ksigned -Kmips3 -FOlimit,4000";;
   ts2_us5) echo "CCMACH = -KPIC -Wx,-G0 -Olimit 4000 -signed -D_REENTRANT";;
   pym_us5) echo "CCMACH = -Xa -Ksigned";;
     *_lnx|\
    int_rpl) compflags=""
	    [ "$vers" = "ppc_lnx" ] && compflags="-m64"
	    [ "$conf_B64" ] && compflags="-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE"
	    echo "CCMACH = -fsigned-char -fno-strength-reduce -D_REENTRANT -DLINUX -D_GNU_SOURCE -DXLIB_ILLEGAL_ACCESS $compflags"
	    [ "$conf_ADD_ON32" ] && echo "CCMACH32 = -m32 $CCMACH" ;;
    mg5_osx) echo "CCMACH = -DDARWIN -fsigned-char -fno-strength-reduce" ;;
esac

# ASMACH specials (passed to assembler)
#  The ICL DRS 500 based machines must invoke m4 preprocessor
#  so that comments may be added to CS assembler sources !
case $vers in
  ts2_us5) echo "ASMACH = -O -G0 -KPIC" ;;
  sgi_us5) 
      echo "ASMACH = -n32 -mips3 -show -KPIC -Wb,-nosymregs -w"
      echo "ASMACH64 = -64 -mips4 -show -KPIC -Wb,-nosymregs -w -DADD_ON64"
      ;;
  su4_us5) [ "$conf_ADD_ON64" ] && echo "ASMACH64 = -xarch=v9"
      echo "ASMACH = -xarch=v8plus"
      ;;
  su9_us5) echo "ASMACH = -xarch=v9" ;;
  rs4_us5) [ "$conf_ADD_ON64" ] && echo "ASMACH64 = -a64" ;;
  ris_u64) echo "ASMACH = -a64" ;;
esac

# CCLDMACH specials--desired flags which aren't passed to ld by cc
case $vers in
   axp_osf) echo "CCLDMACH = " ;;
   dg8_us5 |\
   dgi_us5) echo "CCLDMACH = " ;;
   dr6_us5) echo "CCLDMACH = " 
	    echo "CCLDMACH_DBG = " 
	    ;;
   hp2_us5|\
   hpb_us5|\
   hp8_us5) if [ "$conf_W4GL" ] ; then
              echo "CCLDMACH = -z -Wl,-E -Wl,-O"
              echo "CXXLDMACH = -z -Wl,-E -Wl,-O"
            else
              echo "CCLDMACH = -Wl,-E -Wl,-O -Wl,+s -Wl,+n"
              echo "CXXLDMACH = -Wl,-E -Wl,-O -Wl,+s"
            fi
	    echo "CCLDMACH64 = +DD64 -Wl,-E -Wl,+s -Wl,+n"
	    echo "CXXLDMACH64 = +DD64 -Wl,-E -Wl,+s"
            ;;
   hp8_bls) echo "CCLDMACH = -Wl,-O" ;;
   i64_hpu)
	    echo "CCLDMACH = +DD64 -Wl,-E -Wl,+s"
	    echo "CXXLDMACH = +DD64 -Wl,-E -Wl,+s"
	    echo "CCLDMACH32 = -Wl,-E -Wl,+s"
	    echo "CXXLDMACH32 = -Wl,-E -Wl,+s"
	    ;;
   sos_us5) echo "CCLDMACH = " ;;
   ris_us5) echo "CCLDMACH = -x" 
            echo "CCLDMACH_DBG = -x" 
	    ;;
   rs4_us5) echo "CCLDMACH = "
            echo "CCLDMACH64 = -q64"
            ;;
   ris_u64) echo "CCLDMACH = " ;;
   i64_aix) echo "CCLDMACH = -f elf64"
            echo "CCLDMACH_DBG = -f elf64"
            ;;
   sqs_ptx) echo "CCLDMACH = " ;;
   nc4_us5) echo "CCLDMACH = " ;;
   su4_us5) echo "CCLDMACH = "
            echo "CCLDMACH64 = -xarch=v9"
            ;;
   su9_us5) echo "CCLDMACH = -xarch=v9" ;;
   a64_sol) echo "CCLDMACH = -mt"
            echo "CCLDMACH64 = -DADD_ON64 -xarch=generic64 -mt"
	    ;;
   sui_us5) echo "CCLDMACH = " ;;
   pym_us5) echo "CCLDMACH = " ;;
   su4_u42) CCLDMACH="-Qoption ld -dc -Qoption ld -dp"
            CCLDMACH="$CCLDMACH -Qoption ld -x -Qoption ld -X"
            echo "CCLDMACH = $CCLDMACH"
            ;;
   su4_cmw) CCLDMACH="-Qoption ld -dc -Qoption ld -dp"
            CCLDMACH="$CCLDMACH -Qoption ld -x -Qoption ld -X"
            echo "CCLDMACH = $CCLDMACH"
            ;;
   su4_cmwXX) CCLDMACH="-Xlinker -dc -Xlinker -dp"
            CCLDMACH="$CCLDMACH -Xlinker -x -Xlinker -X"
            echo "CCLDMACH = $CCLDMACH"
            ;;
   sgi_us5) echo "CCLDMACH = -mips3 -Wl,-woff,84 -Wl,-woff,85 -Wl,-woff,34"
            echo "CCLDMACH64 = -64 -mips4 -Wl,-woff,84 -Wl,-woff,85 -Wl,-woff,34"
            ;;
   ts2_us5) echo "CCLDMACH = " ;;
   usl_us5) echo "CCLDMACH = -Qy -K thread" ;;
     *_lnx|\
   int_rpl) if [ "$vers" = "ppc_lnx" ] ; then
		echo "CCLDMACH = -m64 -rdynamic " 
		echo "#define CCLDMACH -m64 -rdynamic " 
	    else
		echo "CCLDMACH = -rdynamic " 
	    fi
	    [ "$conf_ADD_ON32" ] && echo "CCLDMACH32 = -m32 $CCLDMACH"
	    ;;
   rux_us5) echo "CCLDMACH = -Kthread" ;;
   mg5_osx) echo "CCLDMACH = " ;;
         *) echo "CCLDMACH = -x" ;;
esac

# CCLDSERVER specials--desired flags passed to ld by cc when linking dbms
#                    server. (this will be referenced in iilink)
case $vers in
    rs4_us5|\
    ris_u64) echo "CCLDSERVER = -bmaxdata:0x30000000" ;;
          *) echo "CCLDSERVER = " ;;
esac


# CCPICFLAG specials: must be set to the compiler options needed for generating
# position independent code for shared libraries ("-PIC" on sun4).
case $vers in
   dr6_us5) echo "CCPICFLAG = -K PIC" ;;
   su4_us5|\
   su9_us5|\
   sui_us5) echo "CCPICFLAG = -K PIC" ;;
   a64_sol) echo "CCPICFLAG = -K PIC" ;;
   pym_us5) echo "CCPICFLAG = -K PIC" ;;
   rux_us5|\
   rmx_us5) echo "CCPICFLAG = -K PIC" ;;
   su4_u42|su4_cmw) if [ "$conf_W4GL" ] ; then
              echo "CCPICFLAG = -PIC"
            else	
              echo "CCPICFLAG = "
            fi
            ;;
   rs4_us5|\
   i64_aix|\
   ris_u64|\
   ris_us5) echo "CCPICFLAG = " ;;
   hp2_us5|\
   hpb_us5|\
   hp8_us5) echo "CCPICFLAG = +Z" ;;
   sos_us5) echo "CCPICFLAG = -K pic" ;;
   sgi_us5) echo "CCPICFLAG = -KPIC" ;;
   axp_osf) echo "CCPICFLAG = " ;;
     *_lnx|\
   int_rpl|\
   mg5_osx) echo "CCPICFLAG = -fPIC" ;;
         *) ;;
esac

# Shared library suffix. You need this if you want to deliver ingres shared
# libs with the same suffix as other shared libs on the box. Default is "so".
case $vers in
  ris_u64|\
  rs4_us5|\
  ris_us5) echo "SLSFX = a" 
	   SFX=a
	   export SFX 
	   ;;
  i64_hpu|\
  hp2_us5|\
  hpb_us5|\
  hp8_us5|\
  hp8_bls) echo "SLSFX = sl" 
	SFX=sl
	export SFX
	;;
  mg5_osx) echo "SLSFX = dylib"
	   SFX=dylib
	   export SFX
	;;
        *) echo "SLSFX = so" 
	SFX=so
	export SFX
	;;
esac

# MainWin shared library suffix. You need this if you want to deliver MainWin
# shared libs with the same suffix as other shared libs on the box.
# Default is "so".
case $vers in
  rs4_us5) echo "MWSLSFX = so" ;;
        *) echo "MWSLSFX = $SFX" ;;
esac

# Common Gateway Interface suffix. This is needed for the ICE server.
# On non NT platforms this will be 'cgi' for NT it must be 'exe'
case $vers in
        *) echo "CGISFX = cgi" ;;
esac

# LDLIBPATH and LIBMACH specials.  The LDLIBPATH symbol is set to a list of
# directories where _system_ (not Ingres) libraries are expected to reside.
# The syslibs variable is set a list of system libraries which might
# potentially go into the LIBMACH list; the list should be in the correct
# scanning order.  LIBMACH is all the libraries in the list that actually
# exist in one of the directories in the LDLIBPATH list.
case $vers in
   su4_u42) ldlibpath="/usr/lang/SC1.0 /lib /usr/lib"
            syslibs="net inet bsdipc socket ld dnet tsi stcp nsl_s nsl"
            ;;
   su4_cmw) ldlibpath="/usr/lang/SC1.0 /usr/lang/SC0.0 /lib /usr/lib \
		/usr/local/lib"
	    syslibs="net inet bsdipc socket ld dnet tsi stcp nsl_s nsl cmw bsm"
	    ;;
   sui_us5) ldlibpath="/opt/SUNWspro/lib /usr/ccs/lib /usr/lib /usr/ucb/lib"
            syslibs="curses gen m"
            ;;
   su4_us5) ldlibpath="/opt/SUNWspro/lib /usr/ccs/lib /usr/lib"
            ldlibpath64="/opt/SUNWspro/lib/v9 /usr/ccs/lib/sparcv9 /usr/lib/sparcv9"
            syslibs="curses gen m"
            ;;
   su9_us5) ldlibpath="/opt/SUNWspro/lib/v9 /usr/ccs/lib/sparcv9 /usr/lib/sparcv9"
            syslibs="curses gen m"
            ;;
   a64_sol) ldlibpath="/opt/SUNWspro/lib /usr/ccs/lib /lib /usr/lib"
            ldlibpath64="/opt/SUNWspro/lib/amd64 /usr/ccs/lib/amd64 /lib/amd64 /usr/lib/amd64"
            syslibs="curses gen m"
            ;;
   i64_hpu) ldlibpath="/usr/lib/hpux64"
            syslibs=""
	    ;;
   hp2_us5|\
   hpb_us5|\
   hp8_us5) ldlibpath="/lib /usr/lib /usr/local/lib"
	    ldlibpath64=$ldlibpath
            syslibs=""
            ;;
   hp8_bls) ldlibpath="/lib /usr/lib /usr/local/lib"
            syslibs="m6 /lib/libsec.sl net bsdipc ld"
            ;;
   dgi_us5) ldlibpath="/usr/sde/ix86dgux/usr/lib /usr/lib"
	    syslibs="resolv socket nsl elf gen m l rte"
	    ;;
   dr6_us5) ldlibpath="/usr/ccs/lib /usr/lib /usr/ucblib"
            syslibs="nsl socket elf gen m c ucb"
            ;;
   usl_us5) ldlibpath="/usr/ccs/lib /usr/lib"
            syslibs="nsl socket elf gen m"
            ;;
   nc4_us5) ldlibpath="/usr/ucblib /usr/ccs/lib /usr/lib"
            syslibs="nsl socket elf gen dl"
            ;;
   sos_us5) ldlibpath="/lib /usr/ccs/lib /usr/lib /usr/local/lib"
            syslibs="net inet bsdipc ld dnet tsi stcp nsl_s nsl"
	    ;;
   axp_osf) ldlibpath="/lib /usr/lib /usr/ccs/lib /usr/shlib"
            syslibs="net inet bsdipc socket ld dnet_stub \
            tsi stcp nsl_s nsl"
	    ;;
   rmx_us5) ldlibpath="/usr/ccs/lib /usr/lib"
            syslibs=""
            ;;
   rux_us5) ldlibpath="/opt/thread/lib /usr/lib"
            syslibs="mproc dl elf socket_r nsl_r m_r"
            ;;
   pym_us5) ldlibpath="/usr/ccs/lib /usr/lib /usr/ucblib"
            syslibs=""
            ;;
   i64_aix) ldlibpath="/usr/ccs/lib/ia64l64 /usr/lib/ia64l64"
     	    syslibs=""
	    ;;
   ts2_us5) ldlibpath="/lib /usr/lib"
            syslibs="c m net inet bsdipc socket ld dnet tsi stcp"
            ;;
   sgi_us5) ldlibpath="/usr/lib32/mips3 /usr/lib32 /lib32"
            ldlibpath64="/usr/lib64/mips4 /usr/lib64 /lib64"
            syslibs="pthread fpe socket nsl elf "
#           syslibs="pthread fpe net inet bsdipc socket ld dnet tsi stcp nsl_s nsl elf "
            ;;
   int_lnx|\
   int_rpl|\
   ibm_lnx|\
   i64_lnx|\
   axp_lnx) ldlibpath="/lib /usr/lib /usr/local/lib"
            ;;
   a64_lnx|\
   ppc_lnx) ldlibpath="/lib64 /usr/lib64 /usr/local/lib64"
            ;;
   rs4_us5|\
   ris_u64) ldlibpath="/lib /usr/lib /usr/local/lib"
	    ldlibpath64=$ldlibpath
            syslibs="net inet bsdipc socket ld dnet tsi stcp nsl_s nsl curses"
            ;;
   mg5_osx) ldlibpath="/usr/lib /usr/local/lib"
	    ;;
         *) ldlibpath="/lib /usr/lib /usr/local/lib"
            syslibs="net inet bsdipc socket ld dnet tsi stcp nsl_s nsl curses"
            ;;
esac

libs=
for lib in $syslibs
{
  if [ "`expr $lib : '^\(.\).*$'`" = "/" ] ; then
    [ -f $lib ] && libs="$libs $lib"
  else
    lflag=false
    for lp in $ldlibpath
    {
      [ -f $lp/lib${lib}.a ] && lflag=true
    }
    $lflag && libs="$libs -l$lib"
  fi
}

echo "LDLIBPATH = $ldlibpath"
echo "LDLIBPATH64 = $ldlibpath64"
echo "LIBMACH = $libs"

# LDLIB and LDLIBMACH specials
prefix=
suffix="-lm -lc"

case $vers in
   axp_osf) suffix="-lrt -lm -lbsd -laio -lpthread -lmach -lexc -lmld";;
   i64_hpu|\
   hp2_us5|\
   hpb_us5) suffix="-lpthread -lm -lrt -lcl";;
   hp8_us5) if [ "$conf_W4GL" ] ; then
              libdld="-ldld" ; suffix="$libdld -lm -lc -lPW"
            else
              suffix="-lm -lcl"
            fi
            ;;
   hp8_bls) if [ "$conf_W4GL" ] ; then
              libdld="-ldld" ; suffix="$libdld -lm -lc -lPW"
            else
              suffix="-lm"
            fi
            ;;
   dr6_us5) libs= ; prefix="-L/usr/ccs/lib -L/usr/lib -L/usr/ucblib"
            suffix="-lnsl -lsocket -lgen -lm -lelf -ldl -lc -lucb"
            ;;
   usl_us5) libs= ; prefix="-YP,/usr/ccs/lib:/usr/lib"
            suffix="-lnsl -lsocket -lgen -lm -lelf -ldl -K thread"
            ;;
   dgi_us5) suffix="-lsocket -lnsl -lgen -lm -lelf -ldl -ll -lcurses -lthread -lrte"
            ;;
   dg8_us5) suffix="-lm -lc -lthread"
            ;;
   sos_us5) libs=
            if [ -r /lib/libcrypt_d.a ]
            then
                suffix="-lnsl_s -lsocket -lprot -lintl -lm -lld -lc -lc_s \
                -lcrypt_d /lib/crtn.o"
            else
                suffix="-lsuds -lnsl_s -lsocket -lprot -lx -lm -lld -lc -lc_s \
                -lintl \
                -lcurses -lcrypt_i /lib/crtn.o"
            fi
            ;;
   pyr_us5) suffix="-lm -lc /.ucblib/libc.a"
            ;;
   rmx_us5) prefix="-YP,/usr/ccs/lib:/usr/lib:/usr/ucblib"
            suffix="-lsocket -lnsl -lc -lmproc -lext -lcurses -lelf -ldl -lgen -lm -lc -lucb"
            ;;
   rux_us5) prefix="-L/opt/thread/lib"
            suffix="-lmproc -ldl -lelf -lsocket_r -lnsl_r -lm_r"; libs=
            ;;
   pym_us5) prefix="-YP,/usr/ccs/lib:/usr/lib:/usr/ucblib"
            suffix="-lsocket -lnsl -lc -lmproc -lext -lcurses -lelf -ldl -lgen -lm -lc -lucb"
	    ;;
   ris_us5) suffix="-lm -lbsd -lc"
            libs="-lsna -ltli -liconv"
	    ;;
   rs4_us5) suffix="-lm -lbsd -lc_r"
            libs="-ltli_r -liconv -lpthreads -ldl"
            ;;
   ris_u64) suffix="-ltli_r -lisode -liconv -lpthreads -ldl -lm_r -lbsd_r -lc_r"
            libs=
            ;;
   i64_aix) suffix="-lm -lc"
	    prefix="-L/usr/lib/ia64l64"
            libs="-ltli -liconv -lpthreads -ldl -lcurses"
            ;;
   sqs_ptx) prefix="-L/lib"
            suffix="-lseq -lsocket -lnsl -lelf -lm -ldl -lc";;
   sui_us5|\
   su4_us5) prefix="-L/usr/lib/sparc"
            prefix64="-L/usr/lib/sparcv9"
	    suffix="-ldl -lsocket -lelf -mt -lposix4 -ldl -lnsl -lm"
	    ;;
   su9_us5) prefix="-L/usr/lib/sparcv9"
	    suffix="-ldl -lsocket -lelf -mt -lposix4 -ldl -lnsl -lm"
	    ;;
   a64_sol) prefix="-L/usr/lib"
   	    prefix64="-L/usr/lib/amd64"
	    suffix="-ldl -lsocket -lelf -mt -lposix4 -ldl -lnsl -lm"
	    ;;
   su4_u42) suffix="-lm -lc -ldl" ; libs=
            ;;
   su4_cmw) suffix="-lm -lc -ldl" 
            ;;
   nc4_us5) suffix="-lm -lc -lc89"
            ;;
   ts2_us5) suffix="-lnsl -lthread /usr/ucblib/libucb.a"
	    ;;
   sgi_us5) prefix="-L/usr/lib32/mips3 -L/usr/lib32 -L/lib "
            prefix64="-L/usr/lib64/mips4 -L/usr/lib64 -L/lib64 "
	    suffix="-lpthread -lfpe -lmutex -lm -lc -lcurses "
	    ;;
    int_lnx) suffix="-lpthread -lm -lc -lcrypt -ldl /lib/libgcc_s.so.1"
	    ;; 
     *_lnx|\
    int_rpl) suffix="-lpthread -lm -lc -lcrypt -ldl -lgcc_s"
	    ;; 
   mg5_osx) suffix="-v -lm -lc -ldl -lgcc -lcc_dynamic -bind_at_load -framework CoreServices -framework DirectoryService -framework Security -prebind"
	    ;;
esac

echo "#define LDLIB $suffix"
echo "LDLIBMACH = $prefix $libs $suffix"
echo "LDLIBMACH64 = $prefix64 $libs $suffix"

# LD_ICE_FLAGS special link options for icesvr
case $vers in
   ris_u64 | rs4_us5) 
	echo "LD_ICE_FLAGS = -brtl -bexpall"
	;;
   usl_us5) 
	echo "LD_ICE_FLAGS = -Wl,-Bexport"
	;;
   *)
	echo "LD_ICE_FLAGS = "
	;;
esac

[ "$conf_MWMWORRKAROUND" ] && echo 'MWM1_2LIB = $(INGLIB)/libmwm1_2.a'

# Which C compiler should we use?
# Make gcc an option for suncmw
case $vers in
  i64_hpu|\
  hp2_us5|\
  hpb_us5|\
  hp8_us5|\
  hp8_bls) compiler="c89" ;;
  rs4_us5) compiler="xlc_r" ;;
  ris_u64) compiler="xlc_r" ;;
  i64_aix) compiler="xlc_r" ;;
  ris_us5) compiler="c89" ;;
  su4_u42) compiler="acc"
	   [ "$conf_GCC" ]  && compiler="gcc";;
  su4_cmw) compiler="acc" 
	   [ "$conf_GCC" ]  && compiler="gcc";;
        *) compiler="cc" ;;
esac
echo "CC = $compiler"

# Set optimizer flag
case $vers in
  axp_osf) optimflag="-O -Olimit 5000" ;;
  dr6_us5) optimflag=-O 
	   echo 'OPTIM_DBG = '
	   ;;
  i64_hpu|\
  hp2_us5|\
  hpb_us5|\
  hp8_us5) optimflag=-O ;;
  hp8_bls) optimflag=+O3 ;;
  su4_us5|\
  su9_us5|\
  sui_us5) optimflag=-O ;;
  a64_sol) optimflag=-xO2 ;;
  sos_us5) optimflag=-O ;;
  pym_us5) optimflag="-O -KOlimit:6000 ";;
  ris_us5) optimflag=-O 
	   echo 'OPTIM_DBG = '
	   ;;
  rux_us5|\
  rmx_us5) optimflag=-O ;;
  sgi_us5) optimflag="-O3 ";;
  sqs_ptx)
	optimflag=-O
	echo 'OPTIM_DBG =-Wc,-O0'
	;;
  su4_u42|su4_cmw|dg8_us5|dgi_us5) optimflag=-O2 ;;
  su4_u42|su4_cmw|dg8_us5) optimflag=-O2 ;;
        *) optimflag=-O ;;
esac
echo "OPTIM = $optimflag"

# set optim level 1 flag (for low optimization)
case $vers in
  su4_us5|\
  su9_us5) optim1flag=-xO1 ;;
  a64_sol) optim1flag=-x01 ;;
  i64_hpu|\
  hp2_us5|\
  hpb_us5) optim1flag=+O1 ;;
  hp8_us5) optim1flag=+O1 ;;
  axp_osf) optim1flag=-O1 ;;
    *_lnx|\
  int_rpl) optim1flag=-O1 ;;
	*) optim1flag=-O ;;
esac
echo "LEVEL1_OPTIM = $optim1flag"

# First line of executable shell scripts
case $vers in
      *) echo 'SHELLHEADER = "#!/bin/sh"' ;;
esac

case $vers in
  dr6_us5|\
   usl_us5) echo '#define NOROC 1' ;;
   hp8_us5|\
   hpb_us5|\
   hp2_us5|\
   hp8_bls) echo '#define RROC +ESlit' ;;
   i64_hpu) echo '#define RROC +Olit=all' ;;
   sos_us5) echo '#define NOROC 1' ;;
   ris_u64|\
   i64_aix|\
   rs4_us5|\
   ris_us5) echo '#define NOROC 1' ;;
   sqs_ptx)\
            echo '#define NOROC 1'
            echo 'LOW_OPTIM=-Wc,-Og'
            ;;
   su4_u42|su4_cmw) echo '#define RROC -R' ;;
   nc4_us5) echo 'LOW_OPTIM=-O0' ;;
   pym_us5|rmx_us5|ts2_us5|sgi_us5|rux_us5) echo '#define NOROC 1'
            echo '#define RROC -Wf,-Cr,-Cs'
            ;;
   sgi_us5) echo '#define NOROC 1'
	    echo '#define RROC -Wf,-Cr,-Cs'
	    echo 'LOW_OPTIM=-O1' ;;
   axp_osf) echo '#define NOROC 1' ;;

esac

case $vers in
  sos_us5|\
  hp8_us5|\
   hp8_bls) ;;
   pyr_us5) echo '#define HASRANLIB 1' ;;
         *) [ -f /bin/ranlib -o -f /usr/bin/ranlib -o -f /usr/ucb/ranlib ] &&
            { echo '#define HASRANLIB 1' ; }
            ;;
esac

[ -f /usr/bin/xargs -o -f /bin/xargs ] && echo '#define XARGS 1'

[ -f /usr/lib/lint ] && echo '#define BSDLINT 1'

# Remainder sometimes dependencies of CCDEFS, to be passed to cc for
# compiling tools.
case $vers in
   dr6_us5|\
   rmx_us5|\
   rux_us5|\
   pym_us5|\
   usl_us5) echo "SYS5=1" ; echo 'DIRFUNC=1' ; echo 'DIRENT=1' ;;
  dg8_us5 | dgi_us5) echo "SYS5=1" ; echo 'DIRFUNC=1' ; echo 'INGRES6=1' ;;
   sgi_us5) echo 'DIRENT=1' ; echo 'DIRFUNC=1' ; echo 'INGRES6=1' ; echo 'SYS5=1' ;;
         *) if grep -s SIGTSTP /usr/include/signal.h > /dev/null ; then
              echo "BSD42=1"
            else
              echo "SYS5=1"
            fi
            if grep -s opendir $TMP > /dev/null ;then
              echo 'DIRFUNC=1'
              [ -f /usr/include/dirent.h ] && echo "DIRENT=1"
            else
              echo 'NO_DIRFUNC=1'
            fi
            echo 'INGRES6=1'
            ;;
esac

# Special libraries needed only by rungraph and vigraph...
case $vers in
  su4_u42) echo 'SYSGRAPHLIB = -lsuntool -lsunwindow -lpixrect' ;;
        *) echo 'SYSGRAPHLIB = '
esac

# Test flag to use to test for symbolic links, default to "-h" for
# platforms that have symbolic links (lstat exists) and "!" for
# platforms that do not
case $vers in
  axp_osf) echo 'SYMLINK=test -L' ;;
        *) if grep -s lstat $TMP > /dev/null ; then
             echo 'SYMLINK=test -h'
           else
             echo 'SYMLINK=false'
           fi
           ;;
esac

# C++ and Xerces defines for buliding ICE and other XML tools

case $vers in 
	su4_us5) echo 'CXX=CC'
	         echo 'CXX_DEFS=-DSOLARIS -DSVR4 -D_SVID_GETTOD -mt -w '
	         echo 'CXX64_DEFS=-Dsu9_us5 -DADD_ON64 -xarch=v9 -DSOLARIS -DSVR4 -D_SVID_GETTOD -mt -w '
		 echo 'XERC_DEFS=-DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER -DXML_USE_NETACCESSOR_SOCKET'
		 echo 'XERC_LIBS=/usr/lib/libc.so.1'
		;;
	a64_sol) echo 'CXX=CC'
	         echo 'CXX_DEFS=-DSOLARIS -DSVR4 -D_SVID_GETTOD -mt -w '
	         echo 'CXX64_DEFS=-DADD_ON64 -xarch=generic64 -DSOLARIS -DSVR4 -D_SVID_GETTOD -mt -w '
		 echo 'XERC_DEFS=-DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER -DXML_USE_NETACCESSOR_SOCKET'
		 echo 'XERC_LIBS=/usr/lib/libc.so.1'
		;;

	mg5_osx) echo 'CXX=g++'
		 echo 'CXX_DEFS= '
		 echo 'XERC_DEFS=-O -DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER -DXML_USE_NETACCESSOR_SOCKET '
		 echo 'XERC_LIBS='
		;;
	  *_lnx|\
        int_rpl) echo 'CXX=g++'
		 echo 'CXX_DEFS= '
		 echo 'XERC_DEFS=-O -DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER -DXML_USE_NETACCESSOR_SOCKET '
		 echo 'XERC_LIBS='
		;;

	i64_aix) echo 'CXX=xlc_r'
		 echo 'CXX_DEFS= '
		 echo 'XERC_DEFS=-O -DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER -DXML_USE_NETACCESSOR_SOCKET '
		 echo 'XERC_LIBS='
		;;
		
	hp2_us5) echo 'CXX=aCC'
	         echo 'CXX_DEFS=-D_HP_UX -DHPaCC -DHPUX11 +DA2.0W +Z'
		 echo 'XERC_DEFS=-DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER -DXML_USE_NETACCESSOR_SOCKET'
		 echo 'XERC_LIBS=/usr/lib/libc.sl'
		;;
	hp8_us5|\
	hpb_us5) echo 'CXX=aCC'
	         echo 'CXX_DEFS=-D_HP_UX -DHPaCC -DHPUX11 +DAportable +Z'
	         echo 'CXX64_DEFS=-Dhp2_us5 -DADD_ON64 -D_HP_UX -DHPaCC -DHPUX11 +DA2.0W +Z'
		 echo 'XERC_DEFS=-DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER -DXML_USE_NETACCESSOR_SOCKET'
		 echo 'XERC_LIBS=/usr/lib/libc.sl'
		;;

	i64_hpu) echo 'CXX=aCC'
	         echo 'CXX_DEFS=-D_HP_UX -DHPaCC -DHPUX11 +Z -D_HPUX_SOURCE -D_POSIX_C_SOURCE=199506L +DD64'
		 echo 'XERC_DEFS=-DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER -DXML_USE_NETACCESSOR_SOCKET'
		 echo 'XERC_LIBS=/usr/lib/hpux64/libc.so'
		;;

	axp_osf) echo 'CXX=cxx'
		 echo 'CXX_DEFS='
		 echo 'XERC_DEFS=-DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER -DXML_USE_NETACCESSOR_SOCKET'
		 echo 'XERC_LIBS='
		 ;;

        rs4_us5) echo 'CXX=xlc_r'
                 echo 'CXX_DEFS='
		 echo 'XERC_DEFS=-DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER -DXML_USE_NETACCESSOR_SOCKET'
                 echo 'XERC_LIBS=-lc_r -lC_r'
                ;;

        usl_us5) echo 'CXX=g++'
                 echo 'CXX_DEFS=-DUNIXWARE'
                 echo 'XERC_DEFS=-DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER -DXML_USE_NETACCESSOR_SOCKET'
                 echo 'XERC_LIBS='
                ;;
        sgi_us5) echo 'CXX=CC'
                 echo 'CXX_DEFS='
                 echo 'XERC_DEFS=-DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER -DXML_USE_NETACCESSOR_SOCKET'
                 echo 'XERC_LIBS='
                ;;
	*)	echo 'CXX=Add C++ defs in mkdefault.sh for you platform'
		;;
esac
exit 0
