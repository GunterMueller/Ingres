:
#
# Copyright (c) 2006 Ingres Corporation
#
# Name: ingres_install.sh -- startup script for Ingres GUI installer on
#			     Linux
#
# Usage:
#	ingres_install
#
# Description:
#	This script is run by users to install Ingres. It is a wrapper script
#	for the Ingres GUI installer which needs to be run as root.
#
# Exit Status:
#	0	Success
#	1	Failure
#
## History:
##	26-Sep-2006 (hanje04)
##	    SIR 116877
##	    Created.
##	13-Nov-2006 (hanje04)
##	    Create dummy lib directory with links to RPM libraries before
##	    attempting to start the GUI. This is currently the only way
##	    to get the installer to work against versions of RPM other than
##	    then one it was built against. (Currently 4.1)
##	    Also remove use of GUI su tools has setting LD_LIBRARY_PATH
##	    for them is proving problematic.
##	16-Nov-2006 (hanje04)
##	    SD 111979
##	    If we're root we don't need to SU.
##	01-Dec-2006 (hanje04)
##	    SIR 116877
##	    Echo note to user that we are searching RPM database as this
##	    can take a while.
##	    Make script executable from anywhere.
##	    Remove "inginstgui" from error message.
##	22-Dec-2006 (hanje04)
##	    Fix up library locations to work on x86_64 Linux.
##	01-Mar-2007 (hanje04)
##	    BUG 117812
##	    Only print exit message on error. Clean exit doesn't
##	    always been we've installed.


# useful variables
unamep=`uname -p`
self=`basename $0`
instloc=`dirname $0`
instexe=${instloc}/bin/inginstgui
rpmlibdir=/tmp/ingrpmlibs.$$
uid=`id -u`

# set umask
umask 022

# library directory
case "$unamep" in
    x86_64)
            libdir=/usr/lib64
            ;;
        *)
            libdir=/usr/lib
            ;;
esac

#
# localsu() - wrapper function for calling /bin/su if one of the GUI
#		equivalents cannot be found.
#
localsu()
{
    ROOTMSG="Installation of Ingres requires 'root' access.
Enter the password for the 'root' account to continue"

    cat << EOF
$ROOTMSG

EOF
    su -c "${1}" root

    return $?
}

fix_rpm_deps()
{

    # get library names from RPM version
    rpmlibvers=`rpm -q --queryformat "%{VERSION}\n" rpm|cut -d. -f1,2`
    rpmlib=${libdir}/librpm-${rpmlibvers}.so
    rpmdblib=${libdir}/librpmdb-${rpmlibvers}.so
    rpmiolib=${libdir}/librpmio-${rpmlibvers}.so

    # Check libraries exist
    [ ! -r ${rpmlib} ] || [ ! -r ${rpmdblib} ] || [ ! -r ${rpmiolib} ] &&
    {
	cat << EOF
ERROR:
Cannot locate RPM runtime libraries.
Aborting...

EOF
	return 1
    }

    # create temp rpmlibdir
    mkdir -p ${rpmlibdir} || return 1
    
    # create links
    for lib in `ldd ${instexe} | grep librpm | awk '{print $1}'`
    do
	case ${lib} in
	    librpm-*)
		ln -s ${rpmlib} ${rpmlibdir}/${lib} || return 1
		;;
	    librpmdb-*)
		ln -s ${rpmdblib} ${rpmlibdir}/${lib} || return 1
		;;
	    librpmio-*)
		ln -s ${rpmiolib} ${rpmlibdir}/${lib} || return 1
		;;
	    *)
		return 1
		;;
	esac
    done

    return 0

}

#
# Main - body of program
#

# su executable
# FIXME! Only use localsu for now, GUI su tools are causing problems
#
# if [ -x /opt/gnome/bin/gnomesu ]
# then
#     sucmd="exec /opt/gnome/bin/gnomesu"
# elif [ -x /opt/kde3/bin/kdesu ]
# then
#     sucmd="exec /opt/kde3/bin/kdesu"
# else
    sucmd=localsu
# fi

if [ ! -x ${instexe} ] 
then
    cat << EOF
Error: $self

Cannot locate the Ingres installer executable:

	${instexe}

EOF
    exit 1
fi

fix_rpm_deps || exit 1

# Launch installer as root, using sucmd if we need to
if [ $uid != 0 ]
then
    ${sucmd} " (export LD_LIBRARY_PATH=${rpmlibdir}:${LD_LIBRARY_PATH}; echo 'Searching RPM database, please wait...' ; ${instexe} )"
else
    export LD_LIBRARY_PATH=${rpmlibdir}:${LD_LIBRARY_PATH}
    echo "Searching RPM database, please wait..."
    ${instexe}
fi

rc=$?

if [ ${rc} = 0 ]
then
    cat << EOF

Done.
EOF
else
    cat << EOF

Installation failed with error code ${rc}
EOF
fi

# remove temp dir
[ -w ${rpmlibdir} ] && rm -rf ${rpmlibdir}

exit ${rc}

