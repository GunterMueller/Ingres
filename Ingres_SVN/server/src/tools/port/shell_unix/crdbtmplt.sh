:
#
# Copyright (c) 2004 Ingres Corporation
#
# crdbtmplt - Sets up the initial database templates for INGRES 6.x
#             We basically have two sets of templates. Which one you
#             you want to use depends on whether your machine is byte
#             swapped or not. The two sets of templates reside in 
#             $ING_SRC/admin/install/dbtmplt{bs}
#
# History:
#	oct-23-1989 (owen)
#		Initial revision
#	oct-25-1989 (owen)
#		Database templates should be mode 600 and all template
#		files begin wih prefix 'aaaaaa'
##      01-Aug-2004 (hanch04)
##          First line of a shell script needs to be a ":" and
##          then the copyright should be next
##	4-Oct-2004 (drivi01)
##		Due to merge of jam build with windows, some directories
##		needed to be updated to reflect most recent map.

ensure ingres || ensure rtingres || exit 1

: ${ING_BUILD?} ${ING_SRC?} 
#
# Make sure that the dbtmplt area exists in ING_BUILD to receive
# the database templates.
#
[ ! -d "$ING_BUILD/dbtmplt" ] && mkdir $ING_BUILD/dbtmplt && chmod 700 $ING_BUILD/dbtmplt 

#
# Need to make sure bzarch.h exists as this tells whether
# or not the porting machine is byte swapped.
#
[ ! -f "$ING_SRC/cl/hdr/hdr_unix/bzarch.h" ] && \
    echo "WARNING: Need \$ING_SRC/cl/hdr/hdr_unix/bzarch.h before proceeding." && \
    exit 1

#
# If bzarch.h defines 'BYTE_SWAP', then you want to copy the
# database templates from the ING_SRC/admin/install/dbtmpltbs
# directory instead of the ING_SRC/admin/install/dbtmplt (not
# byte swapped) directory.
#
if grep BYTE_SWAP $ING_SRC/cl/hdr/hdr_unix/bzarch.h >/dev/null 
then
    DBTMPLTDIR=dbtmpltbs
else
    DBTMPLTDIR=dbtmplt
fi

#
# Copy the templates into ING_BUILD/dbtmplt. First, remove any
# previously existing templates files.
#
rm -f $ING_BUILD/dbtmplt/aaaaaa*
if cp $ING_SRC/admin/install/$DBTMPLTDIR/aaaaaa* $ING_BUILD/dbtmplt
then
    chmod 600 $ING_BUILD/dbtmplt/aaaaaa*
    echo "Loaded initial database templates successfully"
else
    echo "Database templates were not set up successfuly"
    rm -f $ING_BUILD/dbtmplt/aaaaaa*
    exit 1
fi
exit 
