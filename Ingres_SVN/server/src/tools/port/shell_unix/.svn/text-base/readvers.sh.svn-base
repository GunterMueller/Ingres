:
#
# Copyright (c) 2004 Ingres Corporation
#
# readvers.sh: reads the VERS file and set some variables
#
#   VERS is of the following (relatively free) format:
#
#       # comment line
#       config=<config>
#       config64=<64 bit port config>
#       build=<num>
#       option=<opt>
#	inc=<build increment>
#
#   <config> is the familiar config string. <num> is the build level.
#   <config64> is the familiar config string for the 64 bit port.
#   <opt> is a configuration keyword, such as W4GL, DBL, B64, etc.  There
#   may be multiple option lines.
#   <build increment> is the minor build version for any on build number.
#   i.e. if the build is r89e then the build increment is e.
#
#   Use with
#
#	. readvers
#
#   and then the following variables will be set:
#
#       $config         value of config line
#       $config64       value of config64 line
#       $build          value of build line
#       $optdef         -Dconf_<opt> ... (for all options)
#       $conf_<opt>     set to "true" for each option
#
#   Example:
#
#	VERS contains:		readvers.sh sets:
#	-------------		----------------
#	config=su4_us5		config=su4_us5
#	config=su9_us5		config64=su9_us5
#	build=02		build=02
#	option=W4GL		optdef=" -Dconf_W4GL -Dconf_DBL"
#	option=DBL		conf_W4GL=true
#				conf_DBL=true
#	option=B64		conf_B64=true
#	option=NO_SHL		conf_NO_SHL=true
#		If this option is present, the tools will not build FE
#		shared libraries.
#	option=SVR_SHL		conf_SVR_SHL=true
#		If present Ingres server (iimerge) will be built from 
#		shared libraries and not from just a iimerge.o containing
#		all the libraries
#	option=INGRESII		conf_INGRESII=true
#
##   History:
##	28-Sep-92 (seiwald)
##		Written.
##	15-oct-92 (lauraw)
##		Changed "patch" to "build" (as per SMUG).
##		Added exit for bad VERS file.	 
##		Added port$noise (i.e., $ING_VERS) support.
##		Added support for MPE build environment use of $CL.
##	10-nov-92 (lauraw)
##		Default "build" changed to 00. (01 would be PUR1.)
##	15-mar-1993 (dianeh)
##		Changed first line to a colon (":").
##	13-jul-1993 (lauraw)
##		VERS may now live in $ING_SRC, too (as per SMUG).
##	13-jul-93 (vijay)
##		Add comment about NO_SHL.
##	23-jul-93 (gillnh2o)
##		Integrating Jab's changes made in ingres63p 'readvers'.... 
##		We were collecting "opts" but never outputting it, and
##		there's a case for setting it in the output. Added that
##		print statement in END.
##		We were setting $noise as a shell variable that was left in
##		the "environment" (for lack of a better term) and thus anything
##		source'ing this would have $noise set. ("mkming.sh" broke as a
##		result.) Changing the name to make it a little more local...
##	04-nov-1997 (hanch04)
##	    Added comment for B64 Large File Support option
##	09-jun-1998 (walro03)
##		Corrected comments from previous change (INGRESII doc).
##	07-jan-1999 (canor01)
##		Added TNG option.
##	26-apr-2000 (hayke02)
##		Removed TNG (embedded Ingres). This change fixes bug 101325.
##	04-Feb-2001 (hanje04)
##		As part of fix for SIR 103876, added comment about SRV_SHL
##		for building iimerge with shared libraries.
##	22-Feb-2002 (hanje04)
##		If build increment <inc> is defined in VERS file then 
##		display it in version.rel
##	20-aug-2002 (hanch04)
##		Added config64 for 32/64 dual build.
##      01-Aug-2004 (hanch04)
##          First line of a shell script needs to be a ":" and
##          then the copyright should be next.

[ -n "$ING_VERS" ] && noise="/$ING_VERS/src"

if [ -r $ING_SRC/tools/port$noise/conf/VERS ]
  then VERS=$ING_SRC/tools/port$noise/conf/VERS
elif [ -r $CL/tools/port$noise/conf/VERS ]
  # This is where MPE development keeps their VERS file
  then VERS=$CL/tools/port$noise/conf/VERS
else VERS=$ING_SRC/VERS
fi

build=00	# default if not set in VERS
inc=""		# If not set don't print

eval `awk '
	BEGIN{FS="="}
	/^#/{next}
	/^[ 	]*$/{next}
	$1 == "config" { config=$2; print "config=" $2 }
	$1 == "config64" { config64=$2; print "config64=" $2 }
	$1 == "build" { build=$2; print "build=" $2 }
	$1 == "option" { opts=opts " " $2; print "conf_" $2 "=true" }
	$1 == "option" { dopts=dopts " -Dconf_" $2 }
	$1 == "inc" { inc=$2; print "inc=" $2 }
	END { print "optdef=\"" dopts "\"" 
		print "opts=\"" opts "\"" }
' $VERS` 

: ${config?"$VERS file is missing or uses obsolete format."}
