:
#  Copyright (c) 2005 Ingres Corporation
#
# Name: iisuspat.sh
#
# Purpose:
#	Setup program Ingres Spatial Object Library
#
## History
##	20-Sep-2005 (hanje04)
##	    SIR 115239
##	    Created.
##	26-Oct-2005 (hanje04)
##	    SIR 115239
##	    Spatial Objects Library needs to be shared library on
##	    Linux.
#

# DEST = utility

# Initialize variables

. iishlib
. iisysdep

WRITE_RESPONSE=false
READ_RESPONSE=false
RESOUTFILE=ingrsp.rsp
RESINFILE=ingrsp.rsp
RPM=false
REMOVE=false

config_host=`iipmhost`

# Process command line args
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
elif [ "$1" = "-rmpkg" ] ; then
   REMOVE=true
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

#
# do_iisuspat - Setup package
#
do_iisuspat()
{
    echo "Setting up Ingres Spatial Object Library"

    check_response_file

    if $WRITE_RESPONSE ; then
   	mkresponse_msg
	rc=$?
    else ## **** Skip everything if we are in WRITE_RESPONSE mode ****
 	## Check library is installed
	if [ -f "$II_SYSTEM/ingres/lib/libspat.a" ] || \
		[ -f "$II_SYSTEM/ingres/lib/libspat.1.$SLSFX" ] ; then
	    iisetres ii.$config_host.config.spatial ON
	    rc=$?
	else
	    echo "Ingres Spatial Object Library is not installed"
	    rc=1
	fi
    fi #end WRITE_RESPONSE mode 

}

#
# do_rmspat - Remove package
#
do_rmspat()
{
    echo "Removing Ingres Spatial Object Library"
    iisetres ii.$config_host.config.spatial OFF
}

if $REMOVE ; then
    eval do_rmspat $INSTLOG
    rc=$?
else
    eval do_iisuspat $INSTLOG
    rc=$?
fi

exit $rc