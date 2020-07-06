:
#
#  Copyright (c) 2004 Ingres Corporation
#
#  Name: (PROG1PRFX)suodbc -- setup script for (PROD1NAME) ODBC Driver
#  Usage:
#	(PROG1PRFX)suodbc
#
#  Description:
#	This script should only be called by the (PROD1NAME) installation
#	utility.  It sets up the (PROD1NAME) ODBC Driver.
#
#  Exit Status:
#	0	setup procedure completed.
#	1	setup procedure did not complete.
#
## History:
##	21-Mar-2003 (loera01) SIR 109643
##	    Created.
##      01-Apr-2003 (loera01) SIR 109643
##         Fixed tracing section of odbcinst.ini and added "dsn=drivername"
##         for odbc.ini.
##	28-Jan-2004 (bonro01)
##	   Remove ingbuild call for PIF installations.
##      29-jan-2004 (loera01)
##         Run iiodbcinst with "-batch" argument if iisuodbc has a
##         batch argument.
##      09-feb-2004 (loera01)
##         Added more informative error message if iiodbcinst fails.
##      17-feb-2004 (loera01) Bugs 111820 and 111821 
##         For batch mode only: remove extraneous comments in batch mode;  
##         Attempt to write odbcinst.ini to $II_SYSTEM/ingres/files if
##         iiodbcinst fails.
##      24-feb-2004 (loera01) 
##         Don't call iisulock, since iisuodbc doesn't write
##         config.dat and iiodbcinst handles multiple users 
##         automatically.
##      25-feb-2004 (loera01)
##         Iisuodbc was executing some things in batch mode that should
##         have been restricted to interactive, and vice versa.
##	7-Mar-2004 (bonro01)
##	   Add -rmpkg flag for PIF un-installs
##	15-Mar-2004 (bonro01)
##	   Add -exresponse flag to support PIF installs.  No parms are
##	   passed yet.  The parms default to the internal batch defaults.
##      05-Apr-2004 (loera01) Bug 112098
##         Made "-rmpkg" functional and added prompt for alternate 
##         installation paths.
##      16-Apr-2004 (loera01)
##         Added a reference to the ODBC driver in config.dat.
##      29-jun-2004 (loera01)
##         Added reference to the ODBC tracing library in config.dat.
##      17-Aug-2004 (loera01)
##         Made error message more legible if the attempted write to
##         /usr/local/etc fails.
##      03-Sep-2004 (Ralph.Loen@ca.com) Bug 112975
##         Added cli module name to config.dat.
##      14-Oct-2004 (Ralph.Loen@ca.com) Bug 113242
##         Write $CONFIG_HOST instead of $HOSTNAME to config.dat.
##      22-Dec-2004 (nansa02)
##         Changed all trap 0 to trap : 0 for bug (113670).
##      24-Feb-2005 (Ralph.Loen@ca.com) Bug 113943
##         Make sure the odbcinst.ini file permissions are set to
##         read/write at all levels.
##      24-Feb-2005 (Ralph.Loen@ca.com) Bug 113943
##         If running in batch mode and ODBCSYSINI is defined, run chmod
##         against odbcinst.ini in the ODBCSYSINI path.
##      03-Mar-2004 (Ralph.Loen@ca.com) Bug 114012
##         Backing out the change for 113943.  Leave odbcinst.ini at the
##         default access permission.
##	30-Nov-2005 (hanje04)
##	    Use head -1 instead of cat to read version.rel to
##	    stop patch ID's being printed when present.
##	05-Jan-2007 (bonro01)
##	    Change Getting Started Guide to Installation Guide.
##      13-Jul-2007 (Ralph Loen) Bug 118746
##         Change prompt for input to use echo rather than iiechonn, so 
##         that the user has a full line to input the path.  Input may be
##         truncated when iisudobc is executed from ingbuild.
##
#  PROGRAM = (PROG1PRFX)suodbc
#
#  DEST = utility
#----------------------------------------------------------------------------


BATCH=false
NOTBATCH=true
INSTLOG="2>&1 | tee -a $(PRODLOC)/(PROD2NAME)/files/install.log"
PARM1="$1"

# check for batch flag
if [ "$1" = "-batch" ] ; then
   BATCH=true
   NOTBATCH=false
   INSTLOG="2>&1 | cat >> $(PRODLOC)/(PROD2NAME)/files/install.log"
   shift ;
elif [ "$1" = "-exresponse" ] ; then
    READ_RESPONSE=true
    BATCH=true      #run as if running -batch flag.
    NOTBATCH=false
    shift
    [ "$1" ] && { RESINFILE="$1" ; shift ; }
elif [ "$1" = "-vbatch" ] ; then
   BATCH=true
   NOTBATCH=false
   shift;
fi
. (PROG1PRFX)sysdep

. (PROG1PRFX)shlib

if [ "$1" = "-rmpkg" ] ; then
NOT_DONE=true
newPATH="/usr/local/etc"
while $NOT_DONE ; do
    {
        echo "Enter the default ODBC configuration path [ $newPATH ]: "
        $BATCH || read ALTPATH junk
        [ -z "$ALTPATH" ] && ALTPATH=$newPATH

        echo ""
        echo "The default ODBC configuration path is $ALTPATH"
        echo ""
    }
    prompt "Is the path information correct? " y && NOT_DONE=false
    echo ""
done
   
   (PROG1PRFX)odbcinst -rmpkg -p $ALTPATH 
   cat << !
  The (PROD1NAME) ODBC Driver has been removed

!
exit 0
fi

do_(PROG1PRFX)suodbc()
{

$BATCH || echo "Setting up the (PROD1NAME) ODBC Driver..."

if [ -f $II_SYSTEM/ingres/install/release.dat ]; then
    VERSION=`$(PRODLOC)/(PROD2NAME)/install/(PROG2PRFX)build -version` ||
    {
	$BATCH || cat << !

$VERSION

!
	exit 1
    }
else
    VERSION=`head -1 $II_SYSTEM/ingres/version.rel` ||
    {
	$BATCH || cat << !

Missing file $II_SYSTEM/ingres/version.rel

!
	exit 1
    }
fi

RELEASE_ID=`echo $VERSION | sed "s/[ ().\/]//g"`

$BATCH || cat << !

This procedure will set up the following version of (PROD1NAME) ODBC driver:

        $VERSION

to run on local host:

        $HOSTNAME

!

$BATCH || prompt "Do you want to continue this setup procedure?" y  || exit 1

NOT_DONE=true

if [ -n "$ODBC_HOME" ]
then
    $BATCH || echo "ODBC_HOME is defined.  Value is $ODBC_HOME"
	newPATH=$ODBC_HOME
elif [ -n "$ODBCSYSINI" ]
then
    $BATCH || echo "ODBCSYSINI is defined.  Value is $ODBCSYSINI"
	newPATH=$ODBCSYSINI
else
    newPATH="/usr/local/etc"
fi

while $NOT_DONE ; do
    $BATCH || 
    {
        echo "Enter the default ODBC configuration path [ $newPATH ]: "
        $BATCH || read ALTPATH junk
        [ -z "$ALTPATH" ] && ALTPATH=$newPATH

        echo ""
        echo "The default ODBC configuration path is $ALTPATH"
        echo ""
    }
    $BATCH || prompt "Is the path information correct? " y && NOT_DONE=false
    echo ""
done

NOT_DONE=true
while $NOT_DONE ; do
    dmFlag=""
    READ_ONLY=false
    $BATCH || 
    {
        if prompt "Does your ODBC installation use a CAI/PT driver manager? " n 
        then
            echo "Your installation is using a CAI/PT driver manager"
            dmFlag="-m CAI/PT"
        else
            echo "Your installation is using a unixODBC driver manager"
            dmFlag=""
        fi
    }
echo ""
$BATCH || prompt "Is this information correct?" y && NOT_DONE=false
echo ""
done

NOT_DONE=true
while $NOT_DONE ; do
    rFlag=""
    READ_ONLY=false
    $BATCH || 
    {
        if prompt "Is this always a read-only driver? " n 
        then
            echo Your driver will be read-only.
            rFlag="-r"
        else
            echo Your driver will allow updates.
        fi
    }
echo ""
$BATCH || prompt "Is this information correct?" y && NOT_DONE=false
echo ""
done

if [ "$rFlag" = "-r" ] ; then
(PROG1PRFX)setres (PROG4PRFX).$CONFIG_HOST.odbc.module_name lib(PROG1PRFX)odbcdriverro.1.$SLSFX
else
(PROG1PRFX)setres (PROG4PRFX).$CONFIG_HOST.odbc.module_name lib(PROG1PRFX)odbcdriver.1.$SLSFX
fi

(PROG1PRFX)setres (PROG4PRFX).$CONFIG_HOST.odbc.trace_module_name lib(PROG1PRFX)odbctrace.1.$SLSFX
(PROG1PRFX)setres (PROG4PRFX).$CONFIG_HOST.odbc.cli_module_name lib(PROG1PRFX)odbc.1.$SLSFX

if $BATCH; then
    (PROG1PRFX)odbcinst -batch > /dev/null 2>&1
    if [ $? -ne 0 ];then
        echo "Cannot write to default ODBC configuraton path /usr/local/etc."
        echo "Writing instead to "$II_SYSTEM"/ingres/files/odbcinst.ini."
        (PROG1PRFX)odbcinst -p $II_SYSTEM/ingres/files -batch 
        if [ $? -ne 0 ];then
           cat << !
(PROG1PRFX)suodbc failed.  Please resolve the problem and re-execute 
(PROG1PRFX)suodbc or execute (PROG1PRFX)odbcinst.

!
           exit 0
        else
            echo "Successfully wrote ODBC configuration files"     
        fi
    else
        echo "Successfully wrote ODBC configuration files"     
    fi
    else
    if [ "$ALTPATH" = "/usr/local/etc" ]
    then
        (PROG1PRFX)odbcinst $rFlag $dmFlag
    elif [ "$ALTPATH" = "$ODBCSYSINI" ]
    then
        (PROG1PRFX)odbcinst $rFlag $dmFlag
    else
        (PROG1PRFX)odbcinst $rFlag $dmFlag -p $ALTPATH
    fi
    if [ $? -ne 0 ];then
        echo "Cannot write to default ODBC configuraton path /usr/local/etc."
        echo "Writing instead to "$II_SYSTEM"/ingres/files/odbcinst.ini."
        ALTPATH="$II_SYSTEM/ingres/files"
        (PROG1PRFX)odbcinst $rFlag $dmFlag -p $II_SYSTEM/ingres/files 
        if [ $? -ne 0 ];then
           cat << !
(PROG1PRFX)suodbc failed.  Please resolve the problem and re-execute 
(PROG1PRFX)suodbc or execute (PROG1PRFX)odbcinst.

!
           exit 0
        else
            echo "Successfully wrote ODBC configuration files"     
        fi
    else
        echo "Successfully wrote ODBC configuration files"     
    fi
    
$BATCH || cat << !

An odbcinst.ini file has been created in the directory "$ALTPATH".  
You may use the utility (PROG1PRFX)odbcadmn to create and manage
ODBC data sources.

See the (PROD1NAME) Installation Guide for more information.
  
!
fi # if $BATCH

$BATCH || pause
trap : 0
exit 0
}

eval do_(PROG1PRFX)suodbc $INSTLOG
trap : 0
exit 0
