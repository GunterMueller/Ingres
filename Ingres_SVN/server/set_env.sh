#!/bin/sh

# This script eliminates a few manual steps when building from Ingres's subversion repository
# It does this by setting up the build environment variables automatically

# Check for the presence of things that likely mean the src directory
# is in front of us so we know where we are
if [ -f src/INSTALL ] && [ -d src/back ]
then
  # First, clear the known environment variables used for Ingres builds
  . ./clear_env.sh
  export ING_ROOT=`pwd`

  # Then, set up the environment using bldenv
  . $ING_ROOT/src/tools/port/jam/bldenv
else
  # Otherwise give up
  echo " "
  echo "This file depends on releative locations of other source files."
  echo "Please source this file from the directory it is in."
  echo "This is typically the root of the Ingres source tree."
  echo " "
fi
