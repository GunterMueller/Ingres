:
#
## Copyright Ingres Corporation 2007. All Rights Reserved.
##
##
##
##  Name: iigenpreun -- pre removal script for RPM/DEB packaging
##
##  Usage:
##       iigenpreun [ -r release name ] [ -v version ] [ -p package ] [ -u ]
##          -u upgrade mode
##
##  Description:
##      This is a generic pre-removal script, only to be called from
##      DEB or RPM packages
##
##  Exit Status:
##       0       setup procedure completed.
##       1       setup procedure did not complete.
##
##  DEST = utility
### History:
##      12-Jun-2007 (hanje04)
##	    SIR 118420
##          Created from SPEC file templates.

## Need II_SYSTEM to be set
[ -z "$II_SYSTEM" ] &&
{
    echo "II_SYSTEM is not set"
    exit 1
}

PATH=$II_SYSTEM/ingres/bin:$II_SYSTEM/ingres/utility:$PATH
LD_LIBRARY_PATH=/lib:/usr/lib:$II_SYSTEM/ingres/lib
export II_SYSTEM PATH LD_LIBRARY_PATH
unset BASH_ENV

rc=0 # initialize return code

# Parse arguments
while [ "$1" ]
do
    case "$1" in
        -r)
            relname=$2
            shift ; shift
            ;;
        -v)
            vers=$2
            shift ; shift
            ;;
        -s)
            suscr=$2
            shift ; shift
            ;;
        -p)
            pkgname=$2
            shift ; shift
            ;;
        -u)
            upgrade=true
            shift
            ;;
         *)
            exit 1
            ;;
    esac
done

# need version and release name
[ -z "$relname" ] || [ -z "$vers" ] || [ -z "$suscr" ] || [ -z "$pkgname" ] && 
{
    echo "Missing argument"
    exit 1
}

# Mesg cmds.
ECHO=true
CAT=true

# Keep silent unless explicitly told not to
[ -x $II_SYSTEM/ingres/utility/iiread_response ] && \
        [ -r "$II_RESPONSE_FILE" ] &&
{
    silent=`iiread_response SILENT_INSTALL $II_RESPONSE_FILE`
    case "$silent" in
        [Oo][No]|\
        [Yy][Ee][Ss])
            ECHO=echo
            CAT=cat
                ;;
    esac
}
export ECHO CAT

    if [ -x $II_SYSTEM/ingres/utility/iisysdep ] ; then
	. iisysdep
	inst_id=`ingprenv II_INSTALLATION`
	rcfile=$ETCRCFILES/ingres${inst_id}
	if which invoke-rc.d > /dev/null 2>&1 ; then
	    rccmd="invoke-rc.d ingres${inst_id}"
	else
	    rccmd=$rcfile
	fi
	inst_log="2>&1 | cat >> $II_SYSTEM/ingres/files/install.log"

	#Check for install userid, groupid
	CONFIG_HOST=`iipmhost`
	II_USERID=`iigetres ii.${CONFIG_HOST}.setup.owner.user`
	II_GROUPID=`iigetres ii.${CONFIG_HOST}.setup.owner.group`
	export II_USERID II_GROUPID

	#Check to see if instance is running and try to shut it down
	#Abort the install is we can't
	[ -x $rcfile ] && [ -f $II_SYSTEM/ingres/files/config.dat ] &&
	{
	    eval $rccmd status >& /dev/null
	    if [ $? = 0 ]
	    then
		eval $rccmd stop $inst_log
		if [ $? != 0 ] ; then
		    rc=2
		    cat << EOF
${relname} installation $inst_id is running and could not be cleanly shutdown.
Aborting installation...
EOF
		    exit $rc
		fi
	    fi
    }


    fi

if $upgrade ; then
    # Script should not be run on upgrade as nothing is actually
    # being removed.
    exit 0;
else
    # Remove config
    ifssave=$IFS
    IFS=:
    for script in ${suscr}
    do  
	su -m -c "${script} -rmpkg" $II_USERID >> /dev/null
    done 
    IFS=$ifssave

    $ECHO "Removing ${relname} ${pkgname}..."
    sleep 5

    exit $rc

fi

