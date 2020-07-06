#!/bin/sh

# This very simple script wipes ingres build environment variables
# - This ensures you don't have artifacts from other build areas
#   or any other unintended scenario

# TODO:
# - find an intelligent way to clean PATH and LD_LIBRARY_PATH

unset ING_BUILD
unset XERCESLOC
unset II_DEB_BUILDROOT
unset PAXLOC
unset ING_SRCRULES
unset II_RPM_BUILDROOT
unset LD_LIBRARY_PATH_64
unset ING_TST
unset II_RPM_PREFIX
unset GTKCCLD
unset KRB5HDR
unset ING_SRC
unset ING_TOOLS
unset HDRGTK
unset XERCESCROOT
unset II_SYSTEM
unset II_MANIFEST_DIR
unset ING_ROOT
unset TST_SEP

echo "Ingres build environment is cleared (to be safe)"
