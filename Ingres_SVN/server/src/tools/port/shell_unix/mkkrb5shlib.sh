:
#
# Copyright (c) 2004 Ingres Corporation
#
# Name:	
#	mkkrb5shlib.sh
#
# Usage:
#	     mkkrb5shlib
#
# Description:
#
#	Creates the shared library for the Kerberos driver.
#
#       NOTE:  Support for AIX has been dropped from this script due to
#       version problems in the GSS API shared library archive.  It is
#       still possible to provide a workable version of the Kerberos driver
#       using the following scheme:
#    
#       If the existing Kerberos installation includes a GSS API library 
#       libgssapi_krb5.a.2.2 containing the shared archive member shr.o.2.2:
#
#       ls -1 gssapi_krb5.o | xargs -n50 /usr/bin/nm -B | \
#          awk ' $2=="D"||$2=="B" { print $3 } ' | sort | uniq > expsym_list
#
#       $krblink_cmd gssapi_krb5.o $krblink_opts \
#	    -o shr.o.2.2
# 
#       ar cq $ING_BUILD/lib/libgssapi_krb5.a shr.o.2.2
#
#       ls -1 gcskrb.o | xargs -n50 /usr/bin/nm -B | \
#          awk ' $2=="D"||$2=="B" { print $3 } ' | sort | uniq > expsym_list
#
#       $krblink_cmd gcskrb.o $krblink_opts -L $ING_BUILD/lib \
#	-ledbccompat.1 -lgssapi_krb5 \
#       -o $ING_BUILD/lib/lib$GCSKRB_LIBS.$SLSFX.2.0
#
#       rm shr.o.2.2 $ING_BUILD/lib/libgssapi_krb5.a
#
#       If the Kerberos installation has a different major or minor ID
#       for the archived object, such as libgssapi_krb5.a.2.1 containing
#       shr.o.2.1, then "2.1" should be referenced in linking instead of 
#       "2.2".  Further updates will require re-linking.
#
# DEST = utility
#
# History
#
#	10-May-99 (rajus01)
#		Created.
#	17-May-99 (rajus01)
#		Changed libgcskrb.$SLSFX.2.0 to lib$GCSKRB_LIBS.$SLSFX.2.0 for
#		HP-UX. Changed $GCSKRB_LIBS.$SLSFX.2.0 to 
#		lib$GCSKRB_LIBS.$SLSFX.2.0. for solaris.
#	26-apr-2000 (somsa01)
#		Added support for multiple product builds.
#	01-May-2000 (hanch04)
#		Updated crypto to k5crypto.
#	19-may-2000 (somsa01)
#		Added hpb_us5, and changed naming convention for gcskrb on HP.
#	22-Jul-2000 (hanje04)
#		Added support for Intel Linux, int_lnx
#	22-aug-2000 (somsa01)
#		Added ibm_lnx.
#	09-jan-2001 (somsa01)
#		Added the capability of running in an alternate product build
#		environment.
#	09-Feb-2001 (ahaal01)
#		Added AIX (rs4_us5).
#	21-Feb-2001 (wansh01)
#		Added sgi_us5.
#	18-Apr-2001 (bonro01)
#		Added rmx_us5 kerberos support.
#	23-mar-2001 (hweho01)
#		Added support for axp_osf platform.
#       23-Apr-2001 (hweho01)
#		Added support for ris_u64 platform.
#       12-Mar-2001 (linke01)
#               Added sui_us5 configure string for building kerberos		
#	03-Dec-2001 (hanje04)
# 		Added support for IA64 Linux (int_lnx)
#	11-dec-2001 (somsa01)
#		Porting changes for hp2_us5.
#       04-Jan-2002 (loera01) SIR 106743
#               Dropped support for AIX--see notes above.  Removed 
#               dependency on Kerberos libraries from the treasure library.
#               Instead, a stub gssapi shared library is built from the 
#               /drivers/krb5 directory.  Kerberos will resolve the actual
#               shared library from the shared library path at runtime.
#               Simplified the script commands.
#	12-Apr-2002 (inifa01)
#		Modified for baroque builds.
#	05-Nov-2002 (hanje04)
#	    Added support for AMD x86-64 Linux, genericise Linux defines 
#	    where apropriate.
#	19-Nov-2002 (bonro01)
#		Added support for sgi_us5 platform.
#       08-Mar-2004 (loera01)
#               Added support for i64_hpu.
#	11-Jun-2004 (somsa01)
#		Cleaned up code for Open Source.
#	15-Jul-2004 (hanje04)
#		Define DEST=utility so file is correctly pre-processed
#      01-Aug-2004 (hanch04)
#          First line of a shell script needs to be a ":" and
#          then the copyright should be next.
#	28-Oct-2004 (hanje04)
#	   Object location is now common/gcf/drivers_unix_vms NOT drivers.
#	22-Mar-2005 (hanje04)
#	   Add support for reverse hybrid builds
#	01-Apr-2005 (bonro01)
#		Porting changes for Solaris AMD64 a64_sol.
#	25-Apr-2005 (sheco02)
#	   Because of change 476512, object location is now common/gcf/drivers
#	   NOT common/gcf/drivers_unix_vms.
#	06-Apr-2005 (hanje04)
#	   Replace drivers_unix_vms with drivers.
#	16-May-2005 (hanje04)
#	    Add support for Mac OS X.
#	01-Jul-2005 (hweho01) 
#	   Corrected the gssapi stub library name for axp_osf platform.
#	04-Aug-2005 (zicdo01)
#	   Remove check to build as ingres
#       12-June-2006 (Lance Larsh) Bug 116237
#          Use the -soname qualifier on Linux systems to avoid hard-coded
#          references to the library path.
#	10-Aug-2006  (hweho01) 
#          For axp_osf platform, modify the reference to gssapi_krb5 
#          shared library.
#       6-Nov-2006 (hanal04) SIR 117044
#          Add int.rpl for Intel Rpath Linux build.
#	08-Feb-2008 (hanje04)
#	   SIR S119978
#	   Add support for Intel OS X

#  Get shared libraries info.
. shlibinfo

[ "$ING_VERS" ] && { vers_src="/$ING_VERS/src" ; vers="/$ING_VERS" ; }

if $use_shared_libs
then
	:
else
	echo 	"$0: Build not set up for using shared libraries"
	exit
fi

: ${ING_BUILD?} ${ING_SRC?} ${krblink_cmd?} 

workdir=$ING_BUILD/lib/shared

#
# Set hybrid variables. If conf_ADD_ON32 is set then it's reverse hybrid
#
HB=64 ## hybrid build
RB=32 ## regular build
[ "$conf_ADD_ON32" ] && 
{
    HB=32
    RB=64
}

do_reg=true do_hyb=false

param=$#

while [ $# -gt 0 ]
do
    case $1 in
	-lp${HB})	do_hyb=true; do_reg=false; shift;;
	-lp${RB})	do_reg=true; do_hyb=false; shift ;;
    esac
done

if ($do_hyb && $do_reg)
then
        echo    "$0: 32 and 64 bit shared libraries can not be build at the same time."
        exit
fi

if $do_hyb
then
	LPHB=lp${HB}
	INGLIB=$ING_BUILD/lib/${LPHB}
	if [ "$krblinkHB_cmd" ]; then
		krblink_cmd="$krblinkHB_cmd"
	fi
	if [ "$krblinkHB_opts" ]; then
		krblink_opts="$krblinkHB_opts"
	fi
        if [ "$config" = "rs4_us5" ] ; then
           OBJECT_MODE="64" ; export OBJECT_MODE
           BITSFX="64"
        fi
else
	LPHB=
	INGLIB=$ING_BUILD/lib
fi


cleanup()
{
    echo Cleaning up ...
    rm $INGLIB/libgssapi_krb5.*
    cd $workdir; rm -rf gcskrb 
}
build_stub()    
{
    echo ""
    echo " Building temporary gssapi stub library "
    echo ""

    case $config in
    *_lnx|int_rpl)
         # we must specify the proper SONAME on Linux to preserve compatibility
         $krblink_cmd gssapi_krb5.o $krblink_opts \
            -o $INGLIB/libgssapi_krb5.$SLSFX -soname=libgssapi_krb5.$SLSFX.2
     ;;
    *)
    $krblink_cmd gssapi_krb5.o $krblink_opts  -o $INGLIB/libgssapi_krb5.$SLSFX
    ;;
    esac

    if [ $? != 0 -o ! -r $INGLIB/libgssapi_krb5.$SLSFX ]; then
        echo $0 Failed,  aborting !!
	cleanup
        exit 4
    fi
}
check_result()
{
    if [ $? != 0 -o ! -r $INGLIB/lib$GCSKRB_LIBS.$SLSFX.2.0 ]; then
        echo $0 Failed,  aborting !!
	cleanup
        exit 4
    else
        chmod 755 $INGLIB/lib$GCSKRB_LIBS.$SLSFX.2.0
        ls -l $INGLIB/lib$GCSKRB_LIBS.$SLSFX.2.0
	cleanup
    fi
}
mkdir $workdir 2>/dev/null
cd $workdir

trap 'cd $workdir; rm -rf $workdir/*; exit 6' 1 2 3 15

[ `(PROG1PRFX)dsfree` -le 100 ] && echo "** WARNING !! You may run out of diskspace **"

#
#	Making libgcskrb.$SLSFX.2.0
#

    echo ""
    echo " Building Kerberos Shared Library "
    echo ""

    # check whether libcompat.1.$SLSFX exists, since kerberos needs it.
    # If it exists, continue processing. Otherwise exit.
    [ -r $INGLIB/lib(PROG0PRFX)compat.1.$SLSFX ] ||
	{ echo Need to build lib(PROG0PRFX)compat.1.$SLSFX first, exiting ...;  exit 5 ;}

    # Create a temporary working directory. cd to the dir. Exit, if it fails.
    mkdir $workdir/gcskrb 2> /dev/null ; cd $workdir/gcskrb
	 [ $? != 0 ] &&  { echo Aborting ...; exit 3 ;}

    # Clean up the working directory.
    rm -f *

    # Copy the objects from the source directory.

    [ ! -r $ING_SRC/common/gcf$vers_src/drivers/krb5/${LPHB}/gcskrb.o ] &&   
	{ echo Missing object gcskrb.o ...; echo "Aborting !!"; exit 2; }
    [ ! -r $ING_SRC/common/gcf$vers_src/drivers/krb5/${LPHB}/gssapi_krb5.o ] &&   
	{ echo Missing object gssapi_krb5.o ...; echo "Aborting !!"; exit 2; }

    cp $ING_SRC/common/gcf$vers_src/drivers/krb5/${LPHB}/gcskrb.o .
    cp $ING_SRC/common/gcf$vers_src/drivers/krb5/${LPHB}/gssapi_krb5.o .

    case $config in
    i64_hpu |\
    hp8_us5 |\
    hpb_us5 |\
    hp2_us5 |\
      *_lnx |\
    int_rpl |\
    rmx_us5 |\
    sui_us5 |\
    sgi_us5 |\
    a64_sol |\
      *_osx |\
    su4_us5)
    
        build_stub

    	echo ""
    	echo " Building libgcskrb.$SLSFX.2.0 "
    	echo ""

        $krblink_cmd gcskrb.o $krblink_opts -L$INGLIB \
	-l(PROG0PRFX)compat.1 -lgssapi_krb5 \
	-o $INGLIB/lib$GCSKRB_LIBS.$SLSFX.2.0

        check_result
	;;

    axp_osf)

        build_stub

    	echo ""
    	echo " Building libgcskrb.$SLSFX.2.0 "
    	echo ""

        $krblink_cmd -o $INGLIB/lib$GCSKRB_LIBS.$SLSFX.2.0 gcskrb.o \
        $INGLIB/lib(PROG0PRFX)compat.1.$SLSFX \
        -L$INGLIB -lgssapi_krb5 \
        $krblink_opts

        check_result
	;;

    *)
	echo "Unsupported Platform......"
	cleanup
        ;;
    esac

exit 0