:
#
# Copyright (c) 2004 Ingres Corporation
#
# construct generic.h file
#
# History:
#	10-dec-93 (dianeh)
#		Created (branched) from tools!port!ming!mkgeneric;
#		removed code for obsolete machines/ports.
#	7-jan-93 (tomm)
#		test using egrep failed on Solaris.  Switch back to
#		if egrep ... syntax from previous version.
#	28-jan-94 (johnst)
#		Bug #56449
#		Added test for printf %p format option.
#	28-feb-94 (vijay)
#		Fix the way we grep nm symbols,	taken from mksecret.
#		Don't declare system functions, include	the headers instead.
#	28-Feb-1994 (fredv)
#		Sun Solaris need it's own $pre; otherwise, it will incorrectly
#		define symbols in generic.h.
#       01-jan-1995 (chech02)
#               Added rs4_us5 for AIX 4.1.
#       11-Apr-1995 (georgeb)
#               Added change for INDEX_PROVIDED from dr6_ues to include
#               dr6_us5.
#	11-Apr-95 (smiba01)
#		Added support for dr6_ues (& dr6_uv1)
#		Also, changes INDEX_PROVIDED check for dr6_ues only as this
#		platform currently does NOT have an index function, but does
#		have an index_to_name function in libc, which confused this
#		script.
#	 7-jun-95 (peeje01)
#		Add check for DoubleByte Character Set (DBCS) enabled port
#		(ie opt=DBL in VERS) and if required #define DOUBLEBYTE
#	15-jul-95 (popri01)
#		Add nc4_us5 to /ccs/lib/LIBC list.
#		Added special pre for nc4_us5.
#	22-jul-95 (morayf)
#		Added pre entry for sos_us5 (as per odt_us5).
#       24-jul-1995 (pursch)
#               Add pym_us5 to pre-existing branches for libc.a & cpp.
#               Add pym_us5 define DMAKEPP (cpp).
#               Add pym_us5 to the case statement for $pre requirements.
#	13-nov-1995 (murf)
#		Added support for sui_us5 and also placed the INDEX_PROVIDED
#		check as mentioned in change by smiba01 11-apr-95, above.
#	01-Nov-95 (allmi01)
#		Added support for Intel based DG-UX machines (dgi_us5).
#	06-dec-95 (morayf)
#		Added support for SNI RM400/600 machines with SINIX (rmx_us5).
#		Ensured Pyramid (pym_us5) has similiar entries.
#	28-jul-1997 (walro03)
#		Updated for Tandem (ts2_us5).  Grep of the nm output for 'index'
#		was getting false positives due to other names that begin with
#		'index'.  Adapted the ${pre} logic to ${suf} and defined that
#		suffix as '$' for Tandem and '' for other platforms.
#	14-Aug-97 (merja01)
#               Set TYPESIG to void fot axp_osf.
#	02-oct-1997 (hanch04)
#	    su4_us5 does not have libjobs.a but has killpg
#	16-feb-98 (toumi01)
#		Added support for Linux (lnx_us5).
#	03-Jun-1998 (hanch04)
#		Added INGRESII
#	10-Jul-1998 (allmi01)
#		Added entries for Silicon Graphics (sgi_us5) to bypass
#		local replacements for valid system functions.
#	10-Mar-1999 (popri01)
#		Unixware (usl_us5) may, depending on conformance level,
#		redefine "strrchr" as "rindex" (see strings.h). This 
#		causes a preprocessor loop ("macro nesting too deep") 
#		when combined with the reverse function definition here.
#		So avoid defining strrchr here.
#	12-Mar-1999 (kosma01)
#		Change libc library to 32 bit library for sgi_us5.
#	10-may-1999 (walro03)
#		Remove obsolete version string apl_us5, bul_us5, dr3_us5,
#		dr4_us5, dr6_ues, dr6_uv1, dra_us5, odt_us5, nc5_us5, ps2_us5,
#		rtp_us5, 3bx_us5.
#       12-May-1999 (allmi01)
#               Added entries to not replace killpg with kill via macro
#               on rmx_us5.
#       22-jun-1999 (musro02)
#               Added NCR (nc4_us5) (as of 3.02) to platforms that
#               don't need to define killpg (its in /usr/ccs/lib/libc89.a)
#       03-jul-1999 (podni01)
#               Added support for ReliantUNIX V 5.44 - rux_us5 (same as rmx_us5)
#	06-oct-1999 (toumi01)
#		Change Linux config string from lnx_us5 to int_lnx.
#       10-Jan-2000 (hanal04) Bug 99965 INGSRV1090.
#               Correct rux_us5 changes that were applied using the rmx_us5
#               label.
#       20-Feb-2000 (linke01)
#              usl_us5 get fatal compiler error "nesting too deep" 
#              on files compress.c and io.c, don't define strchr and strrchr
#              to resolve it.
#	14-Jun-2000 (hanje04)
#		Added support for OS/390 Linux (ibm_lnx)
#	11-Sep-2000 (hanje04)
#		 Added support for Alpha Linux (axp_lnx).
#	18-jul-2001 (stephenb)
#		From allmi01 originals, add support for i64_aix
#	03-Dec-2001 (hanje04)
#		Added support for IA64 Linux (i64_lnx)
#	05-Nov-2002 (hanje04)
#		Added support for AMD x86-64 Linux, genericise Linux defines
#		where possible
#	07-Feb-2003 (bonro01)
#		Added support for IA64 HP (i64_hpu)
##      01-Aug-2004 (hanch04)
##          First line of a shell script needs to be a ":" and
##          then the copyright should be next.
##	12-Jan-2005 (hanje04)
##	    Define TYPESIG as void on Linux
##	7-Mar-2004 (hanje04)
## 	    SIR 114034
##	    Add support for reverse hybrid builds.
#	15-Mar-2005 (bonro01)
#	    Add support for Solaris AMD64 a64_sol
#       18-Apr-2005 (hanje04)
#           Add support for Max OS X (mac_osx).
#	23-Aug-2005 (schka24)
#	    Solaris 10 doesn't ship libc.a, use the .so - works for older
#	    Solarae as well.
#           Based on initial changes by utlia01 and monda07.
##       6-Nov-2006 (hanal04) SIR 117044
##          Add int.rpl for Intel Rpath Linux build.
#	23-Mar-2007
#	    SIR 117985
#	    Add support for 64bit PowerPC Linux (ppc_lnx)
#	08-Mar-2008 (hanje04)
#	    SIR S119978
#	    Add support for Intel OS X
#

TMP=/tmp/libc.nm
trap 'rm -f $TMP' 0 1 2 13 15

exec 1> ${1-generic.h}

cat <<!
/*
 * generic.h -- allows portable use of the following functions:
 *
 *	dup2(cur,new)		dups fd cur into fd new
 *	killpg(pg,sig)		sends sig to process group pg
 *	signal(s,f)		set function f to catch signal s -- return old f
 *	freesignal(s)		unblock signal s
 *	memcpy(b1,b2,n)		copy n bytes from b2 to b1 -- return b1
 *	memzero(b,n)		zero n bytes starting at b -- return b
 *	strchr(s,c)		return ptr to c in s
 *	strrchr(s,c)		return ptr to last c in s
 *	vfork()			data share version of fork()
 */

union				/* handles side effects in macros */
{
	char	*ustring;
} __GeN__;

!

. readvers
vers=$config
[ "$conf_ADD_ON32" ] && vers=$config64

case $vers in
   [0-9]*) echo "#define x$vers"
           echo "#define DMAKEPP \"/lib/cpp\""
           ;;
  su4_us5|\
  sui_us5|\
  rmx_us5|\
  rux_us5|\
  pym_us5) echo "#define $vers"
           echo "#define DMAKEPP \"/usr/ccs/lib/cpp\""
           ;;
  sgi_us5) echo "#define $vers"
	   echo "#define DMAKEPP \"/usr/lib32/cmplrs/cpp\""
	   ;;
  i64_aix) echo "#define $vers"
           echo "#define DMAKEPP \"/usr/ccs/lib/cpp\""
           ;;
    *_lnx|\
  int_rpl) [ -z "$conf_ADD_ON32" ] && echo "#define $vers"
	   echo "#define DMAKEPP \"/lib/cpp\""
	   ;;
  *_osx) echo "#ifndef $vers"
	   echo "#define $vers"
	   echo "#endif"
	   ;;
        *) echo "#define $vers"
           echo "#define DMAKEPP \"/lib/cpp\""
           ;;
esac

case $vers in
    xe3_us5) LIBC=/lib/386/Slibc.a ;;
    dr6_us5) LIBC=/usr/ccs/lib/libc.a ;;
    usl_us5) LIBC=/usr/ccs/lib/libc.a ;;
    nc4_us5) LIBC=/usr/ccs/lib/libc.a ;;
    pym_us5) LIBC=/usr/ccs/lib/libc.so ;;
    rux_us5) LIBC=/usr/ccs/lib/libc.so ;;
    rmx_us5) LIBC=/usr/ccs/lib/libc.so ;;
    sui_us5) LIBC=/usr/lib/libc.a;;
    sgi_us5) LIBC=/usr/lib32/mips3/libc.so ;;
    int_lnx|\
    int_rpl|\
    ibm_lnx|\
    i64_lnx|\
    axp_lnx) LIBC=/usr/lib/libc.a;;
    a64_lnx) LIBC=/usr/lib64/libc.a;;
    ppc_lnx) LIBC=/usr/lib64/libc.a;;
    i64_aix) LIBC=/usr/lib/ia64l64/libc.so;;
    i64_hpu) LIBC=/usr/lib/hpux64/libc.so;;
    su9_us5|\
    su4_us5|\
    a64_sol) LIBC=/usr/lib/libc.so;;
    *_osx) LIBC=/usr/lib/libc.dylib;;
    *)       LIBC=/lib/libc.a ;;
esac

case $vers in
    ris_us5|axp_osf|rs4_us5|ris_u64) pre='^' ;;
    dr6_us5|nc4_us5|\
    usl_us5|sos_us5|rmx_us5|pym_us5|rux_us5) pre='^.*\|' ;;
    su4_us5) pre='\|_';;
    i64_aix) pre='\|';;
    su4_us5|sui_us5) pre='\|_';;
    ts2_us5) pre='\| ';;
    *)       pre='_*[^A-Za-z]' ;;
esac

case $vers in
    ts2_us5) suf='$';;
    *)       suf='' ;;
esac

case $vers in
  dr6_us5) CPP="/usr/ccs/lib/cpp"
           ;;
  su4_us5|\
  sui_us5) CPP="/usr/ccs/lib/cpp"
           ;;
  usl_us5) CPP="/usr/ccs/lib/cpp"
           ;;
  rmx_us5|\
  rux_us5|\
  pym_us5) CPP="/usr/ccs/lib/cpp"
           ;;
  sgi_us5) CPP="/usr/lib32/cmplrs/cpp"
	   ;;
  i64_aix) CPP="/usr/ccs/lib/cpp"
           ;;
  *_osx) CPP="/usr/bin/cpp"
           ;;
        *) CPP="/lib/cpp"
           ;;
esac

#
#
# 	Define DOUBLEBYTE if this is a DBCS enabled port.
#
if [ "$conf_DBL" ]
then
    echo '# define DOUBLEBYTE'
fi

#
#
# 	Define INGRESII if this is Ingres II
#
    echo '# define INGRESII'

case $vers in
    ris_us5|rs4_us5|ris_u64|i64_aix)        nm=/bin/nm  ;;
    mip_us5|sgi_us5)	    nm="nm -B" ;;
    su4_us5|sui_us5)	    nm="/usr/ccs/bin/nm" ;;
    *)              nm=nm   ;;
esac

$nm $LIBC > $TMP

#
# dup2(x,y)
#
if [ `egrep -c "${pre}dup2" $TMP || true` -lt 1 ] ; then
  if [ -f /usr/include/fcntl.h ] ; then
    F_DUPFD=`( cat /usr/include/fcntl.h; echo F_DUPFD ) | $CPP -P |
               sed -n '$p'`
    echo "#define dup2(x,y) fcntl(x,`echo $F_DUPFD`,y)"
  else echo "#define dup2(x,y) dup((x)|0100,y)"
  fi
fi

#
# lstat(x,y)
#
if egrep -c "${pre}lstat" $TMP  >/dev/null
then
	echo "#define LSTAT_PROVIDED"
fi

#
# index(x,y)
#
case $vers in
  dr6_us5|usl_us5|sui_us5) if egrep -c "${pre}index$" $TMP >/dev/null
           then
                echo "#define INDEX_PROVIDED"
           fi
           ;;
        *) if egrep -c "${pre}index" $TMP >/dev/null
           then
                echo "#define INDEX_PROVIDED"
           fi
           ;;
esac

#
# bcopy(x,y)
#
if  egrep -c "${pre}bcopy" $TMP >/dev/null
then
	echo "#define BCOPY_PROVIDED"
fi

#
# mkdir(x,y)
#
if egrep -c "${pre}mkdir" $TMP >/dev/null
then
	echo "#define MKDIR_PROVIDED" 
fi

#
# killpg
#
case $vers in
     su4_us5|sui_us5)        ;;
     i64_hpu)	;;
     nc4_us5)         if [ ! -f /usr/ccs/lib/libc89.a ] ; then
               if [ `egrep -c "${pre}killpg" $TMP || true` -lt 1 ] ; then
               echo "#define killpg(x,y) kill(-(x),y)"
               fi
               fi
                ;;
     i64_aix|\
     rux_us5)  echo "#define killpg(x,y) kill(x,y)"
     		;;
     *)              if [ ! -f /usr/lib/libjobs.a ] ; then
               if [ `egrep -c "${pre}killpg" $TMP || true` -lt 1 ] ; then
               echo "#define killpg(x,y) kill(-(x),y)"
               fi
               fi
		;;
esac                

#
# memory
#
[ -f /usr/include/memory.h ] && echo "#include <memory.h>"
[ -f /usr/include/string.h ] && echo "#include <string.h>"
[ -f /usr/include/strings.h ] && echo "#include <strings.h>"

if [ `egrep -c "${pre}memcpy" $TMP || true` -ge 1 ] ; then
  echo "#define memzero(b,n) memset(b,'\\\\0',n)"
elif [ `egrep -c "${pre}bcopy" $TMP || true` -ge 1 ] ; then
  echo "#define memcpy(b1,b2,n) (bcopy(__GeN__.ustring=b2,b1,n),__GeN__.ustring)"
  echo "#define memzero(b,n) (bzero(__GeN__.ustring=b,n),__GeN__.ustring)"
else
  echo "#define memzero(b,n) memset(b,'\\\\0',n)"
fi

#
# signal
#
[ -f /usr/lib/libjobs.a ] && echo "#define signal sigset"
if [ `egrep -c "${pre}sigblock" $TMP || true` -ge 1 ] ; then
  echo "#define freesignal(s) sigsetmask(sigblock(0)&~(1<<(s-1)))"
else
  echo "#define freesignal(s)"
fi

# Determine TYPESIG setting. TYPESIG is also associated with sigvec in the CL.
# Some systems define *signal differently.

case $vers in
  dg8_us5) echo '#define TYPESIG void'
           ;;
  dgi_us5|axp_osf) echo '#define TYPESIG void'
           ;;
	*_lnx|int_rpl) echo '#define TYPESIG void'
	   ;;
        *) if grep '*signal' /usr/include/signal.h /usr/include/sys/signal.h |
                grep void > /dev/null ; then
             echo '#define TYPESIG void'
           else
             echo '#define TYPESIG int'
           fi
           ;;
esac

#
# strchr(x,y), strrchr(x,y)
#
case $vers in
        usl_us5) ;;   
        *) if [ `egrep -c "${pre}index${suf}" $TMP || true` -ge 2 -a ! -f /xenix ]
           then
               echo "#ifndef strchr"
               echo "#define strchr index" ;  echo "#define strrchr rindex" 
               echo "#endif"
           fi
esac

#
# vfork
#
case $vers in
	      *)    [ `egrep -c "${pre}vfork" $TMP || true` -lt 1 ] &&
		    { echo "#define vfork fork" ; } ;;
esac

#
# printf %p format option.
#
(	cd /tmp
	echo '
main()
{
	printf("%p", 0xabcd);
}
' > /tmp/percentp.c
cc -o percentp percentp.c > /dev/null 2>&1 && \
	./percentp | egrep '[aA][bB][cC][dD]' > /dev/null 2>&1
) && echo '# define PRINTF_PERCENT_P'
rm -f /tmp/percentp*

exit 0
