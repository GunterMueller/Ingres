#!/bin/sh
# This is a simple script to help you run the build with less overhead.
# It will automatically set up your environment and run the build.
# It will log all logs to a timestamped log file.

#TODO
# - add online help
# - add automatic execution of sep tests
# - add automatic packaging 
# - add cleanup and forced rebuild options (jam clean with -c) (jam force with -a)
# - add some additional sanity checks as issues are uncovered

NOW=`date +"%m_%d_%y_%H%M"`
PWD=`pwd`
LOGDIR="${PWD}/logs"
LOGFILE="${LOGDIR}/jam_${NOW}.log"

# Set a return code to success, we'll toggle it if anything fails
RETURN=0

if [ ! -d ${LOGDIR} ]
then
  mkdir -p ${LOGDIR}
fi

# if we're starting fresh, set up the environment
if [ -f set_env.sh ]
then
  . ./set_env.sh
else
  echo "Error, I cannot find the set_env.sh script"
  echo "This is required to use runbuild.sh"
  echo "Please run runbuild.sh from the root directory of the Ingres source"
  exit 1
fi

date > ${LOGFILE}
echo " "
echo "Starting Ingres build."
echo "This will take roughly 20 minutes or more."
echo "(depending on how fast your computer is)"
echo " "
echo "Use \"tail -f ${LOGFILE}\""
echo "in another terminal window to follow progress."
echo " "
echo "RESULTS:"
cd $ING_ROOT/src/tools/port/jam/; jam >> ${LOGFILE} 2>&1
cd $ING_SRC; mkjams >> ${LOGFILE} 2>&1 
jam >> ${LOGFILE} 2>&1

# Check if the build succeeded or failed by the presence of
# skipped or failed targets
grep "failed updating" ${LOGFILE} > /dev/null 2>&1
if [ $? -eq 0 ]
then
  echo "Some compile targets failed."
  RETURN=1
else
  echo "No compile targets failed."
fi
grep "skipped" ${LOGFILE} > /dev/null 2>&1
if [ $? -eq 0 ]
then
  echo "Some compile targets were skipped."
  RETURN=1
else
  echo "No compile targets were skipped."
fi

buildrel -a >> ${LOGFILE} 2>&1
if [ $? -eq 0 ]
then
  echo "buildrel -a succeeded."
else
  echo "buildrel -a failed."
  RETURN=1
fi

echo " "
date >> ${LOGFILE}

if [ $RETURN -eq 0 ]
then
  echo "The build succeeded."
else
  echo "The build failed. Check the logs to determine why."
fi

echo "Logs are in: ${LOGFILE}"

exit ${RETURN}
