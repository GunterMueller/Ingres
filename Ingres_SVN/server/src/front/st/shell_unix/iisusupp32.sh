:
#  Copyright (c) 2005 Ingres Corporation
#
# Name: iisusup32.sh
#
# Purpose:
#	Setup program for 32bit support for 64bit Ingres
#
## History
##	09-Mar-2005 (hanje04)
##	    Created.
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
# do_iisusupp32 - Setup package
#
do_iisu32supp()
{
    echo "Setting up 32bit support for 64bit Ingres"

    check_response_file

    if $WRITE_RESPONSE ; then
   	mkresponse_msg
	rc=$?
    else ## **** Skip everything if we are in WRITE_RESPONSE mode ****
 	## Check files are installed
	if [ -d "$II_SYSTEM/ingres/lib/lp32" ] && \
	   [ -d "$II_SYSTEM/ingres/files/collation/lp32" ] && \
	   [ -d "$II_SYSTEM/ingres/files/ucharmaps/lp32" ] ; then 
	    echo "Setting II_LP32_ENABLED=TRUE"
            ingsetenv II_LP32_ENABLED TRUE
	    rc=$?
	else
	    echo "32bit support for 64bit Ingres is not installed"
	    rc=1
	fi
    fi #end WRITE_RESPONSE mode 

}

#
# do_rmsupp32 - Remove package
#
do_rmsupp32()
{
    echo "Removing 32bit support for 64bit Ingres"
    ingunset II_LP32_ENABLED
}

if $REMOVE ; then
    eval do_rmsupp32 $INSTLOG
    rc=$?
else
    eval do_iisu32supp $INSTLOG
    rc=$?
fi

exit $rc