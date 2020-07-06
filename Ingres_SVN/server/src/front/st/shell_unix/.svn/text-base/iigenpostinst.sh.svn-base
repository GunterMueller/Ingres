:
#
## Copyright Ingres Corporation 2007. All Rights Reserved.
##
##
##
##  Name: iigenpostinst -- post installation setup script for RPM/DEB packaging
##
##  Usage:
##       iigenpostinst [ -r release name ] [ -v version ] [ -p package ] [ -u ]
##          -u upgrade mode
##
##  Description:
##      This is a generic post-install script to only to be called from
##      DEB or RPM packages
##
##  Exit Status:
##       0       setup procedure completed.
##       1       setup procedure did not complete.
##
##  DEST = utility
### History:
##      04-Jun-2007 (hanje04)
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
relname=""
vers=""
pkg=""
upgrade=false
rc=0 # initialize return code
. iishlib

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

# If we're not installing using a response file we
# won't be doing any setup so skip the re-install checks
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

# Use response file if one if provided
IISUFLAG="-rpm"
[ "$II_RESPONSE_FILE" ] && IISUFLAG="$IISUFLAG -exresponse $II_RESPONSE_FILE"
export IISUFLAG

#Check for install userid, groupid
CONFIG_HOST=`iipmhost`
II_USERID=`iigetres ii.${CONFIG_HOST}.setup.owner.user`
II_GROUPID=`iigetres ii.${CONFIG_HOST}.setup.owner.group`
export II_USERID II_GROUPID

# correct dir ownership
chown $II_USERID:$II_GROUPID ${II_SYSTEM}/ingres/.
for dir in bin demo files ice install lib sig utility vdba
do
    [ -d "${II_SYSTEM}/ingres/${dir}/." ] && \
    chown -R $II_USERID:$II_GROUPID ${II_SYSTEM}/ingres/${dir}/.
done

# and version.rel
chown $II_USERID:$II_GROUPID ${II_SYSTEM}/ingres/version.rel


# Run setup
ifssave=$IFS
IFS=:
for script in ${suscr}
do
su -m -c "${script} $IISUFLAG || ( echo 'Setup script ${iisuscr} failed.' && echo 'See $II_SYSTEM/ingres/files/install.log for more info.' )" $II_USERID ||
{
    rc=1
    break
}
done
IFS=$ifssave

# fix setuid
set_setuid
exit $rc

