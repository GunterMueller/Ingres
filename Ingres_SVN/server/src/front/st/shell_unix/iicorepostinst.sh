:
#
## Copyright Ingres Corporation 2007. All Rights Reserved.
##
##
##
##  Name: iicorepostinst -- post installation setup script for RPM/DEB
##	 core package
##
##  Usage:
##       iicorepostinst [ -r release name ] [ -v version ]
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
##      12-Jun-2007 (hanje04)
##	    SIR 118420
##          Created.

## Need II_SYSTEM to be set
[ -z "$II_SYSTEM" ] &&
{
    echo "II_SYSTEM is not set"
    exit 1
}

PATH=$II_SYSTEM/ingres/bin:$II_SYSTEM/ingres/utility:$PATH
LD_LIBRARY_PATH=/lib:/usr/lib:$II_SYSTEM/ingres/lib
export II_SYSTEM PATH LD_LIBRARY_PATH
relname=""
vers=""
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
[ -z "$relname" ] || [ -z "$vers" ] && 
{
    echo "Missing argument"
    exit 1
}

#Silent install?
[ -r "$II_RESPONSE_FILE" ] && \
        silent=`iiread_response SILENT_INSTALL $II_RESPONSE_FILE`
[ "$silent" ] &&
{
    ECHO=true
    CAT=true
}
export ECHO CAT

parse_response()
{

# Read response file and set environment variables accordingly
    [ "$II_RESPONSE_FILE" ] && [ ! -f "$II_RESPONSE_FILE" ] && {
    rc=1
    $CAT << !
Cannot locate response file.

        II_RESPONSE_FILE=$II_RESPONSE_FILE
!
    exit $rc
    }

    for var in `cut -s -d= -f1 $II_RESPONSE_FILE | grep -v ^#`
    do
        export ${var}=`iiread_response ${var} $II_RESPONSE_FILE`
    done
}

check_env()
{
    [ "$II_RESPONSE_FILE" ] && parse_response
    # Use response file if one if provided
    IISUFLAG=-batch
    [ "$II_RESPONSE_FILE" ] && IISUFLAG="$IISUFLAG -exresponse $II_RESPONSE_FILE"
    export II_SUFLAG

    #Check for install userid, groupid; default to ingres
    II_USERID=${II_USERID:-ingres}
    II_GROUPID=${II_GROUPID:-ingres}

    #Check for user defined installation ID otherwise default to II
    [ -z "$II_INSTALLATION" ] && II_INSTALLATION=`ingprenv II_INSTALLATION`
    [ -z "$II_INSTALLATION" ] && II_INSTALLATION=II

    ingsetenv II_INSTALLATION $II_INSTALLATION
    $CAT << !
II_INSTALLATION configured as $II_INSTALLATION.
!

}

genenv()
{
    # Pick up term type from response file
    [ "$II_TERMINAL" ] && export TERM_INGRES=$II_TERMINAL
    [ "$TERM_INGRES" ] || TERM_INGRES=konsolel

    cat << ! > $II_SYSTEM/.ing${II_INSTALLATION}bash
# ${relname} environment for $II_INSTALLATION installation
# Generated at installation time

export II_SYSTEM=$II_SYSTEM
export PATH=\$II_SYSTEM/ingres/bin:\$II_SYSTEM/ingres/utility:\$PATH
if [ "\$LD_LIBRARY_PATH" ] ; then
    LD_LIBRARY_PATH=/usr/local/lib:\$II_SYSTEM/ingres/lib:\$II_SYSTEM/ingres/lib/lp32:\$LD_LIBRARY_PATH
else
    LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib:\$II_SYSTEM/ingres/lib:\$II_SYSTEM/ingres/lib/lp32
fi
export LD_LIBRARY_PATH
export TERM_INGRES=$TERM_INGRES
!

    cat << ! > $II_SYSTEM/.ing${II_INSTALLATION}tcsh
# ${relname} environment for $II_INSTALLATION installation
# Generated at installation time

setenv II_SYSTEM $II_SYSTEM
set path=(\$II_SYSTEM/ingres/{bin,utility} \$path)
if ( \$?LD_LIBRARY_PATH ) then
    setenv LD_LIBRARY_PATH /usr/local/lib:\$II_SYSTEM/ingres/lib:\$LD_LIBRARY_PATH
else
    setenv LD_LIBRARY_PATH /lib:/usr/lib:/usr/local/lib:\$II_SYSTEM/ingres/lib
endif
setenv TERM_INGRES $TERM_INGRES
!

} # genenv

do_setup()
{
    # correct dir ownership
    chown $II_USERID:$II_GROUPID ${II_SYSTEM}/ingres/.
    for dir in bin demo files ice install lib sig utility vdba
    do
        [ -d "${II_SYSTEM}/ingres/${dir}/." ] && \
            chown -R $II_USERID:$II_GROUPID ${II_SYSTEM}/ingres/${dir}/.
    done

    # and version.rel
    chown $II_USERID:$II_GROUPID ${II_SYSTEM}/ingres/version.rel

    # If ingvalidpw exists then it needs to be owned by root
    # and have SUID set.
    [ -x $II_SYSTEM/ingres/bin/ingvalidpw ] && {
        chown root $II_SYSTEM/ingres/bin/ingvalidpw
        chmod 4755 $II_SYSTEM/ingres/bin/ingvalidpw
    }

    # Create config.dat and set installation owner and group
    touch $II_SYSTEM/ingres/files/config.dat
    CONFIG_HOST=`iipmhost`
    iisetres "ii.${CONFIG_HOST}.setup.owner.user" $II_USERID
    iisetres "ii.${CONFIG_HOST}.setup.owner.group" $II_GROUPID

    # Remove release.dat if it exists
    [ -f $II_SYSTEM/ingres/install/release.dat ] && \
        rm -f $II_SYSTEM/ingres/install/release.dat

    # Run setup
    su -m -c "iisutm $IISUFLAG || ( echo 'Setup of ${relname} Base Package failed.' && echo 'See $II_SYSTEM/ingres/files/install.log for more info.' )" $II_USERID || rc=2

    # Generate environment script
    genenv

    eval homedir="~"$II_USERID
    [ -d "$homedir" ] && [ -w "$homedir" ] ||
    {
	$CAT << EOF
The home directory for the user ${II_USERID}:

        $homedir

does not exist or is not writeable. The environment setup script for
this instance of Ingres r3 will be written to:

        $II_SYSTEM

EOF
        eval homedir=$II_SYSTEM
    }

    # Copy it to home directory if we can
    [ "$homedir" != "$II_SYSTEM" ] && \
	su $II_USERID -c "cp -f $II_SYSTEM/.ing*sh $homedir" >> /dev/null 2>&1

    # Install startup scripts under /etc/rc.d
    [ -x $II_SYSTEM/ingres/utility/mkrc ] &&
    {
	inst_id=`ingprenv II_INSTALLATION`
	mkrc -r
	mkrc && mkrc -i
	( [ "$II_START_ON_BOOT" = "NO" ] || [ "$START_ON_BOOT" = "NO" ] ) &&
	{
	    if [ -x /usr/sbin/update-rc.d ] ; then
		/usr/sbin/update-rc.d -f ingres${inst_id} remove > \
							/dev/null 2>&1
	    else
		/sbin/chkconfig --del ingres${inst_id} > /dev/null 2>&1
	    fi
	}
    }
} # do_setup

check_env
do_setup

exit $rc
