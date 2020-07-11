:
#
## Copyright Ingres Corporation 2007. All Rights Reserved.
##
##
##
##  Name: iigenpreinst -- pre installation setup script for RPM/DEB packaging
##
##  Usage:
##       iigenpreinst [ -r release name ] [ -v version ] [ -u ]
##	    -u upgrade mode
##
##  Description:
##      This is a generic pre-install script to only to be called from
##	DEB for RPM packages
##
##  Exit Status:
##       0       setup procedure completed.
##       1       setup procedure did not complete.
##
##  DEST = utility
## History:
##	01-Jun-2007 (hanje04)
##	    SIR 118420
##	    Created from SPEC file templates.

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
relname=""
vers=""
upgrade=false

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
	    majvers=`echo $vers | cut -d- -f1`
	    minvers=`echo $vers | cut -d- -f2`
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
[ -z "$relname" ] || [ -z "$vers" ] && exit 1

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

[ ! -r $II_SYSTEM/ingres/version.rel ] || \
[ "$majvers" != `head -1 $II_SYSTEM/ingres/version.rel | cut -d' ' -f2` ] &&
{
    echo "${relname} ${vers} is not installed under $II_SYSTEM"
    echo "Aborting installation..."
    rc=1
    exit $rc
}

if [ ! -x $II_SYSTEM/ingres/utility/iipmhost -o \
     ! -x $II_SYSTEM/ingres/utility/iigetres ] ; then
    echo "${relname} base package files are missing"
    echo "Aborting installation..."
    exit 1
fi

#Check for install userid, groupid
CONFIG_HOST=`iipmhost`
[ "$CONFIG_HOST" ] && 
{
    II_USERID=`iigetres ii.${CONFIG_HOST}.setup.owner.user`
    II_GROUPID=`iigetres ii.${CONFIG_HOST}.setup.owner.group`
    export II_USERID II_GROUPID
}
if [ ! "$II_USERID" -o ! "$II_GROUPID" ] ; then
    echo "${relname} base package install was invalid"
    echo "Aborting installation..."
    exit 1
fi

. iisysdep
inst_id=`ingprenv II_INSTALLATION`
rcfile=$ETCRCFILES/ingres${inst_id}
if which invoke-rc.d > /dev/null 2>&1 ; then
    rccmd="invoke-rc.d ingres${inst_id}"
else
    rccmd=$rcfile
fi
inst_log="2>&1 | cat >> $II_SYSTEM/ingres/files/install.log"

#Check to see if instance is running and try to shut it down
#Abort the install is we can't
[ -x $rcfile ] && [ -f $II_SYSTEM/ingres/files/config.dat ] && 
{
    eval ${rccmd} status >> /dev/null 2>&1
    if [ $? = 0 ]
    then
	eval ${rccmd} stop $inst_log
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

# Abort if response file is set but invalid
[ "$II_RESPONSE_FILE" ] && [ ! -f "$II_RESPONSE_FILE" ] && {
rc=3
cat << !
Cannot locate response file.

	II_RESPONSE_FILE=$II_RESPONSE_FILE
!
exit $rc
}

[ "$II_RESPONSE_FILE" ] && {

# Check response file is readable by specified installation owner.
# If not abort the install before it starts.
    if su -c "[ ! -r \"$II_RESPONSE_FILE\" ]" $II_USERID ; then
	rc=4
        cat << !
Response file is not readable by user $II_USERID

        II_RESPONSE_FILE=$II_RESPONSE_FILE

If user $II_USERID does not exists, the response file should be
globally readable.
!
        exit $rc
    fi
}

exit $rc
