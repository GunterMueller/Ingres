:
## Copyright (c) 2004 Ingres Corporation 
## Name:
##	iirpmbuild.sh
##
## Usage:
##	iirpmbuild
##
## Description:
##
##	Builds RPM packages from existing Ingres build area.
##
## Depends on:
##
##  II_RPM_BUILDROOT = directory to unpack install image, destination 
##		      for built RPMs
##  II_RPM_PREFIX    = default target directory for RPM install
##                     must match prefix in spec file!
##  II_RELEASE_DIR   = buildrel output directory (to locate install.pgz)
##
## History
##
##	24-oct-2003 (fanch01)
##	    Created.
##	27-Oct-2003 (hanje04)
##	    Added header and comments etc.
##	29-Jan-2004 (hanje04)
##	    Sign rpm packages
##	06-Mar-2004 (hanje04)
##	    Add RPMBLD variable to denote build command so that script will 
##	    work against rpm 3.0.6 as well as 4.1.
##	    Also remove rpmPackage function as it is no longer used.
##	27-Mar-2004 (hanje04)
##	    Make sure RPM are always built with defined architecture.
##	    For now, i386
##	28-Jun-2004 (hanje04)
##	    Add functionality to build Embedded saveset.
##	15-Jul-2004 (hanje04)
##	    Make sure libioutil.1.so is copied over to the RPM build area
##	    as it's NOT in the regular manifest.
##	17-Jul-2004 (hanje04)
##	    Make sure mdb.tar.gz is copied to the RPM build area.
##	13-Aug-2004 (hanje04)
##	    Make sure files/mdb dir exists. Exclude ice, spatial, secure
##	    packages from embedded saveset.
##	30-Jul-2004 (hanje04)
##	    Don't build Embedded saveset if II_RPM_EMB_PREFIX isn't set.
##	12-Aug-2004 (hanje04)
##	    Update for replacement of tar archives with pax archives.
##	29-Aug-2004 (hanje04)
##	    Only create documentation area if we need to.
##	21-Sep-2004 (hanje04)
##	    SIR 113101
##	    Remove all trace of EMBEDDED or MDB setup. Moving to separate 
##	    package.
##	22-Sep-2004 (hanje04)
##	    Update doc link to reflect new version.
##	25-Oct-2004 (hanje04)
##	    Add support for MDB package.
##	22-Nov-2004 (hanje04)
##	   Get build number from VERS file
##	05-Jan-2005 (hanje04)
##	   Get Ingres version from CONFIG file.
##	18-Jan-2005 (hanje04)
##	   Add support for other Linuxes.
##	19-Jan-2004 (kodse01)
##	   Corrected the comments to work for i64_lnx.
##	08-Mar-2005 (hanje04)
##         SIR 114034
##	   Add support for reverse hybrid builds.
##      04-Jan-2006 (hanje04)
##	    SIR 115662
##	    Virtually a complete re-write:
##          Change method such that only the archives that are required for
##          a particular packages are actually unpacked. Then remove them
##          once we're done. This allows RPM to package much more efficiently.
##          Allow packages for building to be specified on the command line.
##	24-Jan-2006 (hanje04)
##	    Move setting of -unhide flag so that pkglist is set correctly
##	31-Jan-2006 (hanje04)
##	    SIR 115688
##	    Enable building of licesing RPM.
##	06-Feb-2006 (hanje04)
##	    Exclude spatial by default and add -p flag to include it
##	13-Feb-2006 (hanje04)
##	    Remove don't abort if flags aren't matched.
##	14-Feb-2006 (hanje04)
##	    Make sure pkglst is set correctly when -p or -u are specified.
##	16-Jun-2006 (hanje04)
##	    BUG 115239
##	    spatial is now just a standalone package so add -sol flag
##	    to just create this package.
##       6-Nov-2006 (hanal04) SIR 117044
##          Add int.rpl for Intel Rpath Linux build.
##	23-Mar-2007
##	    SIR 117985
##	    Add support for 64bit PowerPC Linux (ppc_lnx)
##	2-Nov-2007 (bonro01)
##	    Fix -unhide parameter to allow building RPM with documentation.
##

self=`basename $0`
ARSFX=pgz

# Check VERS file
. readvers

# Reverse hybrid?
if [ "$conf_ADD_ON32" ] ;then
    vers=$config64
else
    vers=$config
fi

# Set rpm architecture
case $vers in
    #IA32 Linux
    int_lnx|\
    int_rpl)    ARCH=i386
		;;
    #IA64 Linux | AMD64 LINUX | S390 Linux
   i64_lnx|\
   a64_lnx|\
   ppc_lnx|\
    ibm_lnx)
		ARCH=`uname -m`
		;;
    *)	    echo "$config is not a recognised Linux platorm"
	    exit 1
		;;
esac

if [ -x /usr/bin/rpmbuild ] ; then
    RPMBLD=rpmbuild
else
    RPMBLD=rpm
fi

shopt -s nullglob

usage()
{
    args="[-unhide"
    for a in $pkglst
    do
        args="${args}] [${a}"
    done

    args="${args}]"
	
    # FIXME!! format the argument list
    echo "Usage:"
    cat << EOF 
$self $args

EOF

    exit 1
}
	   
#
# -[ error() ] - Wrapper for echoing errors
#
error()
{
    msg="Unspecified error"
    [ "$1" ] && msg="$1"

    cat << EOF
Error:
$msg

EOF

    return 0
}
# error()

#
#-[ clean_exit() ] - Clean up and exit with specified error or zero-----------
#
clean_exit()
{
    # Set return code
    rc=0
    [ $1 ] && rc=$1

    # Reset any traps and exit
    trap - 0
    exit $rc
}
# clean_exit()

#
#-[ get_pkg_list() ] - Get Valid rpm package list from manifest
#
get_pkg_list()
{
    [ -r "${rpmconfig}" ] ||
    {
	error "Cannot locate ${rpmconfig}"
	clean_exit 2
    }

    # Remove unwanted packages from the build list
    if ${unhide}
    then
        pkgmask=DUMMY
    else
        pkgmask="i18n|documentation"
    fi

    if ! ${spatial}
    then
	pkgmask="${pkgmask}|spatial"
    fi

    # Scan rpmconfig for package
    grep '^$spec' "${rpmconfig}" | awk '
	$2 == "" { printf "core\n" ; }
	$2 != "" { printf "%s\n",$2 ; }' | \
	egrep -v ${pkgmask}
}
# get_pkg_list()

envInit()
{
# check environment for proper variables
    rc=0
    if [[ "$II_RPM_BUILDROOT" == "" ]];  then
	error "Set II_RPM_BUILDROOT to location for RPMs and install image"
	(( rc++ ))
    fi
    if [[ "$II_RPM_PREFIX" == "" ]]; then
	error "Set II_RPM_PREFIX so that it matches the spec file prefix"
	(( rc++ ))
    elif (( $rc == 0 )); then
	mkdir -p "$II_RPM_BUILDROOT$II_RPM_PREFIX"
	if [[ ! -d "$II_RPM_BUILDROOT$II_RPM_PREFIX" ]]; then
	    error "$II_RPM_BUILDROOT$II_RPM_PREFIX does not exist and could not create"
	    (( rc++ ))
	fi
    fi
    
# get version info from rpmconfig
    rpm_basename=`grep '^$sub.*rpm_basename' ${rpmconfig} | awk '{print $3}'`
    rpm_version=`grep '^$sub.*rpm_version' ${rpmconfig} | awk '{print $3}'`
    rpm_release=`grep '^$sub.*rpm_release' ${rpmconfig} | awk '{print $3}'`

# Create dir tree for documentation
	if mkdir -p $II_RPM_BUILDROOT/usr/share/doc
	then
	    [ -L "$II_RPM_BUILDROOT/usr/share/doc/${rpm_basename}-${rpm_version}" ] || \
		ln -s $ING_SRC/tools/techpub/pdf \
		 $II_RPM_BUILDROOT/usr/share/doc/${rpm_basename}-${rpm_version}
	else
	     error "error: Could not create doc area under $II_RPM_BUILDROOT"
	     ((rc++))
	fi

    if [[ "$II_RELEASE_DIR" == "" || ! -d "$II_RELEASE_DIR"  ]]; then
	error "Set II_RELEASE_DIR to a valid directory containing buildrel output"
	(( rc++ ))
    fi

    export rpm_basename rpm_version rpm_release
    return $rc
}

#
#-[ unpack_archives() - Unpack archives to be packaged in RPM ]
#
unpack_archives()
{
    [ "$1" ] || return 1
    if [ "$1" = "core" ] ; then
	local rpm=""
    elif [ "$1" = "LICENSE" ] ; then
	# no archive for LICENSE RPM
	return 0
    else
	local rpm="$1"
    fi

    # get list of archives for RPM from rpmconfig using awk.
    # 	- scan through rpmconfig until we find the "$spec" value
    # 	  we're looking for.
    #   - When we do turn on printing and print out all the "$features"
    #	  associated with the "$spec"
    #   - When we find another "$spec" turn of printing and scan through to
    #	  the end.
    archlist=`awk -v spec=$rpm 'BEGIN{
	prnt=0 ;
	}
	prnt==0 && $1 == "$spec" && $2 == spec { prnt=1 ; }
	$1 == "$feature" && prnt == 1 { printf "%s\n", $2 ;} 
	prnt==1 && $1 == "$spec" && $2 != spec { prnt=0; } ' \
	"${rpmconfig}"`

    rc=$?

    # Check we have somesort of list
    [ -z "${archlist}" ] || [ $rc != 0 ] && 
    {
	error "Failed to create archive list"
	return 1
    }

    # Now unpack the archives
    cd $II_RPM_BUILDROOT$II_RPM_PREFIX
    for arch in $archlist
    do
	[ -r "${II_RELEASE_DIR}/${arch}/install.${ARSFX}" ] ||
	{
	    cat << EOF
Warning:
Cannot locate ${II_RELEASE_DIR}/${arch}/install.${ARSFX}
skipping...

EOF
	    return 0
	}

	echo "unpacking $arch..."
	pax -pp -rzf ${II_RELEASE_DIR}/${arch}/install.${ARSFX} ||
	{
	    error "Failed to unpack $arch"
	    return 3
	}
    done

    return 0

}

build_rpm()
{
    [ -z "$1" ] &&
    {
	error "No package specified for building"
	return 1
    }

    # generate spec file name and location
    local pkg=$1
    local bldroot=${II_RPM_BUILDROOT}${II_RPM_PREFIX}
    local specloc=ingres/files/rpmspec
    if [ "$pkg" = "core" ] ; then
	local specname=${rpm_basename}-${rpm_version}-${rpm_release}
    else
	local specname=${rpm_basename}-${rpm_version}-${pkg}-${rpm_release}
    fi
    local base=${specname}
    local log=$II_RPM_BUILDROOT/${base}.log
    
    specfile=${bldroot}/${specloc}/${specname}.spec

    # Build package
    echo "packaging $base ..."
    ${RPMBLD} -bb --define "_rpmdir ${II_RPM_BUILDROOT}" --target \
	${ARCH} ${specfile} >& ${log}

    rc=$?
    if (( $rc )); then
	error "packaging $base failed, see ${base}.log for details"
    fi
}

#
#-[ Main - main body of script ]----------------------------------------------
#

# Announce startup
cat << EOF

iirpmbuild

EOF

# Get valid package list from manifest
[ -z "$II_MANIFEST_DIR" ] &&
{
    error "II_MANIFEST_DIR is not set"
    clean_exit 1
}

unhide=false
spatial=false

# parse command line

export unhide
export spatial
export rpmconfig="${II_MANIFEST_DIR}/rpmconfig"

# If no arguments are given build everything
pkglst=`get_pkg_list`
[ -z "$1" ] && bldlst="${pkglst}"

# Parse command line
while [ "$1" ]
do
  case "$1" in
    -unhide) 
	unhide=true
	pkglst=`get_pkg_list`
	shift
	;;
   -s*) spatial=true
	pkglst=spatial
	bldlst=spatial
	[ -d "$SOL_RELEASE_DIR" ] ||
	{
	    error "SOL_RELEASE_DIR is not set or doesn't not exist"
	    clean_exit 6
	}
	export II_RELEASE_DIR=$SOL_RELEASE_DIR
	shift
	# only one spatial package so we're done
	break
	;;
     *)
	match=false
	for pkg in $pkglst
	do 
	    # Check args are valid packages
	    [ "$1" = "$pkg" ] &&
	    {
	    if [ "${bldlst}" ] ; then 
		    bldlst="${bldlst} $1"
	    else
		    bldlst="$1"
	    fi
	    # break out when we find a match
	    match=true
	    break
	    }

        done
	
        # if we found a match check the next arg
        ${match} && shift && continue

        # otherwise print usage message and exit
        error "$1 is not a valid package"
        usage
	;;
  esac # parsing arguments.

done

[ -z "$bldlst" ] && bldlst="${pkglst}"

export pkglst
# Check build environment
envInit || clean_exit 3

# Build packages
for p in $bldlst
do
	echo "Building $p RPM..."
	unpack_archives $p || clean_exit 4
	build_rpm $p || clean_exit 5
done

clean_exit
