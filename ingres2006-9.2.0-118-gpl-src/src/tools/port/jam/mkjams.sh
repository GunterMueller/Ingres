:
#
#  Copyright (c) 2004 Ingres Corporation
#
#  Name: mkjams -- Generate multiple Jamfiles using mkjam
#
#  Usage:
#       mkjams
#
#  Description:
#       This script can be used to generate Jamfiles thoughout
#       the source tree. It ONLY generates Jamfiles in top levels
#	of source tree because there are static Jamfiles submitted in
#	the source code directories
#
#  Exit Status:
#       0       Jamfile successfully created
#       1       Creation of Jamfile failed
#
# Generate Jamfile in the current directory for Ingres r3 and beyond.
#
##
## History:
##	21-Jun-2004 (hanje04)
##	    Created from mkjams.
##	19-Jul-2004 (hanje04)
##	    Correct directories visited under front!ice
##	01-Aug-2004 (hanje04)
##	    Add . to MANIFEST find so it works when $ING_SRC is a link
##	15-Oct-2004 (bonro01)
##	    Remove old technique for building 64-bit hybrid build
##	10-Feb-2005 (bonro01)
##	    Follow symbolic links when looking for MANIFEST files.
##	06-Feb-2005 (hweho01)
##	    For AIX, need to find test tools MANIFEST files at 
##          $ING_TST, option "-follow" doesn't work at the location 
##          pointed by a link.
##	28-Feb-2006 (hanje04)
##	    Correct typo introduced by previous change
##	03-Mar-2006 (hanje04)
##	   BUG 115796
##	   Create all build related link here instead of mkidirs.sh
##	   so that mkjams can follow them when looking for MANIFEST and
##	   Jamfiles.
##	23-Mar-2006 (hweho01)
##         For AIX, need to find patch tool MANIFEST files at 
##         $PTOOLSROOT, option "-follow" doesn't work at the location 
##         pointed by a link.
##      28-Mar-2006 (hweho01)
##         For AIX, only search the MANIFEST files in $PTOOLSROOT 
##         and $ING_TST directoies if location variables are defined.
##  12-May-2006 (fanra01)
##      When using the cygwin shell from a command prompt the invocation of
##      mkjam from this script is not recognized.  Add explicit invocations.
##	19-Jun-2006 (bonro01)
##	    Fix previous change. mkjam is located at $ING_TOOLS/bin on
##	    unix instead of $ING_SRC/bin.  Create symbolic link for
##	    $ING_SRC/bin -> $ING_TOOLS/bin
##  02-Nov-2006 (clach04)
##      Updated (link) -L checks to be standard posix -h checks
##      -L checks fail with built-in test in bourne shell (work with /bin/test)
##	30-May-2007 (hanje04)
##	    BUG 119214
##	    Use $SHELL to invoke mkjam instead of sh to avoid problems with
##	    dash under Ubuntu.
##	    
##
CMD=`basename $0`
unames=`uname -s`

usage='[ -d <dir>[,<dir>[,<n>...]] ] [ -p <project>[,<project>[,<n...>]] ]

       where:

         <dir>         is the name of the source directory (e.g., back)
                       to rebuild Jamfile files in; if more than one
                       directory is specified, separate directory names
                       with commas (e.g., -d back,cl,gl).

         <project>     is the name of the project -- currently supported
                       is "ingres" (default is "ingres"); if more than
                       one project is specified, separate the project
                       names with commas (e.g., -p foo,bar,ingres).
'

: ${ING_SRC?"must be set"}


while [ $# -gt 0 ]
do
  case "$1" in
     -d) [ $# -gt 1 ] ||
         { echo "" ; echo "$CMD $usage" ; echo "" ; exit 1 ; }
         DIRS="`echo $2 | tr ',' ' '`"
         shift ; shift
         ;;
     -p) [ $# -gt 1 ] ||
         { echo "" ; echo "$CMD $usage" ; echo "" ; exit 1 ; }
         projects="`echo $2 | tr ',' ' '`"
         shift ; shift
         ;;
     *) echo "Illegal option: $1"
         echo "Usage:" ; echo "$CMD $usage" ; exit 1
         ;;
  esac
done

# Before we start creating anything we need to setup some links
# (Unix ONLY)
# ING_TST area
[ "$unames" != "Windows_NT" ] && [ "$ING_TST" ] && 
{
    [ -h "$ING_SRC/tst" ] ||
    { 
	echo 'Creating $ING_SRC/tst->$ING_TST  link...'
	ln -s "$ING_TST" $ING_SRC/tst ||
	{
	    echo "$CMD: Cannot create link $ING_SRC/tst. Check permissions."
	    exit 1
	}
    }
    [ -h "$ING_SRC/bin" ] ||
    { 
	echo 'Creating $ING_SRC/bin->$ING_TOOLS/bin  link...'
	ln -s "$ING_TOOLS/bin" $ING_SRC/bin ||
	{
	    echo "$CMD: Cannot create link $ING_SRC/bin. Check permissions."
	    exit 1
	}
    }
}

# Patchtools area
[ "$unames" != "Windows_NT" ] && [ "$PTOOLSROOT" ] &&
{
    [ -h "$ING_SRC/patchtools" ] ||
    {
	echo 'Creating $ING_SRC/patchtools->$PTOOLSROOT  link...'
	ln -s "$PTOOLSROOT" $ING_SRC/patchtools ||
	{
            echo "$CMD: Cannot create link $ING_SRC/patchtools. Check permissions."
            exit 1
	}
    }
}



[ "$DIRS" ] ||
DIRS="admin back cl common dbutil front gl ha sig tech testtool tools TOP"
if [ "$config_string" = "r64_us5" ] ; then
MANF=`find $ING_SRC/.  -name MANIFEST`
[ "$ING_TST" ] && MANF_TST=`find $ING_TST/.  -name MANIFEST`
[ "$PTOOLSROOT" ] && MANF_PTOOLS=`find $PTOOLSROOT/.  -name MANIFEST`
else
MANF=`find $ING_SRC/. -follow -name MANIFEST`
fi

[ "$projects" ] ||
{ 
  projects="ingres"
  echo "Building Jamfile files for: $projects"
}

# Call sh $ING_SRC/bin/mkjam with the subsystem-level directories, the facility-level
# directories, then the group-level directory for each project (except
# vec31, which only gets group-level).
cd $ING_SRC
for project in $projects
{
  case $project in
    w4gl|x11)   if [ -r $ING_SRC/bin/mk${project}jams ]
            then
                $ING_SRC/bin/mk${project}jams $mkjamargs
            else
                echo "Ignoring build of ${project} project (not installed?)"
            fi
            ;;

	ingres)
	    echo "Doing MANIFEST files"
	    for manf in $MANF
	    {
	        $SHELL $ING_SRC/bin/mkjam `dirname $manf`
	    }
            if [ "$config_string" = "r64_us5" ] ; then
                for manf_tst in $MANF_TST
                  {
                        $SHELL $ING_SRC/bin/mkjam `dirname $manf_tst`
                  }
                for manf_ptools in $MANF_PTOOLS
                  {
                        $SHELL $ING_SRC/bin/mkjam `dirname $manf_ptools`
                  }
            fi

    	    for dir in $DIRS
            {
              case $dir in
                  admin |\
                   back |\
                     cl |\
                 common |\
                 dbutil |\
                     gl |\
                     ha |\
                    sig |\
                   tech |\
               testtool |\
                  tools |\
		  front |\
		  w4gl ) [ -d "$dir" ] && if [ "$ING_VERS" ] 
			   then
                             $SHELL $ING_SRC/bin/mkjam $dir/ice/$ING_VERS/src/plays \
                                    $dir/ice/$ING_VERS/src/samples \
				    $dir/*/$ING_VERS/src \
                                    $dir/* \
                                    $dir
                           else
                              $SHELL $ING_SRC/bin/mkjam $dir/ice/plays $dir/ice/samples \
                                      $dir/* $dir
                           fi
                           ;;
		  TOP ) $SHELL $ING_SRC/bin/mkjam $ING_SRC ;;
              esac
            }
            ;;
         *) echo "$CMD: Don't know how to build project ${project}. Skipping..."
            continue
            ;;
  esac
}

exit 0
