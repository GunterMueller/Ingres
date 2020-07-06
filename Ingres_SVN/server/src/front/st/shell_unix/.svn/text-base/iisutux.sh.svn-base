:
#  Copyright (c) 2004 Ingres Corporation
#
#  Setup program for the Tuxedo Product
#
## History
##	21-Apr-94 (larrym)
##		Created.
##	22-nov-95 (hanch04)
##		add batchmode and log of install
##	05-nov-98 (toumi01)
##		Modify INSTLOG for -batch to an equivalent that avoids a
##		nasty bug in the Linux GNU bash shell version 1.
##	23-oct-2001 (gupsh01)
##		Added code for handling -exresponse and -mkresponse.
##	29-jan-2002 (devjo01)
## 		Set permissions for tuxedo directory.
##      15-Jan-2004 (hanje04)
##              Added support for running from RPM based install.
##              Invoking with -rpm runs in batch mode and uses
##              version.rel to check version instead of ingbuild.
##	7-Mar-2004 (bonro01)
##		Add -rmpkg flag for PIF un-installs
#

if [ "$1" = "-rmpkg" ] ; then
   cat << !
   DTP for Tuxedo has been removed

!
exit 0
fi

# DEST = utility

WRITE_RESPONSE=false
READ_RESPONSE=false
RESOUTFILE=ingrsp.rsp
RESINFILE=ingrsp.rsp
RPM=false

# check for batch flag
if [ "$1" = "-batch" ] ; then
   BATCH=true
   NOTBATCH=false
   INSTLOG="2>&1 | cat >> $II_SYSTEM/ingres/files/install.log"
elif [ "$1" = "-mkresponse" ] ; then
   WRITE_RESPONSE=true
   BATCH=false
   NOTBATCH=true
   INSTLOG="2>&1 | tee -a $II_SYSTEM/ingres/files/install.log"
   if [ "$2" ] ; then
        RESOUTFILE="$2"
   fi
elif [ "$1" = "-exresponse" ] ; then
   READ_RESPONSE=true
   BATCH=true
   NOTBATCH=false
   INSTLOG="2>&1 | tee -a $II_SYSTEM/ingres/files/install.log"
   if [ "$2" ] ; then
        RESINFILE="$2"
   fi
elif [ "$1" = "-rpm" ] ; then
   RPM=true
   BATCH=true
   NOTBATCH=false
   INSTLOG="2>&1 | cat >> $II_SYSTEM/ingres/files/install.log"
else
   BATCH=false
   NOTBATCH=true
   INSTLOG="2>&1 | tee -a $II_SYSTEM/ingres/files/install.log"
fi

export WRITE_RESPONSE
export READ_RESPONSE
export RESOUTFILE
export RESINFILE

do_iisutux()
{
echo "Setting up DTP for Tuxedo..."

. iisysdep

. iishlib

check_response_file

if [ "$WRITE_RESPONSE" = "true" ] ; then
        mkresponse_msg
else ## **** Skip everything if we are in WRITE_RESPONSE mode ****

if [ -d $II_SYSTEM/ingres/files/tuxedo ]
then
    echo \$II_SYSTEM/ingres/files/tuxedo/ exists.
else
    echo Creating \$II_SYSTEM/ingres/files/tuxedo/
    mkdir $II_SYSTEM/ingres/files/tuxedo
    chmod 755 $II_SYSTEM/ingres/files/tuxedo
fi
fi #end WRITE_RESPONSE mode 

exit 0
}
eval do_iisutux $INSTLOG
