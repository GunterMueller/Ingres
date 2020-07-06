:
#
# Copyright (c) 2004 Ingres Corporation
#
##      01-Aug-2004 (hanch04)
##          First line of a shell script needs to be a ":" and
##          then the copyright should be next.
##	10-Dec-2004 (hanje04)
##	    Make includes work with jam
##	14-Sep-2004 (hanje04)
##	    Add support for jam builds.
##      29-May-2007 (hanal04) Bug 118414
##          Double quote ^SubDir in grep to avoid grep failure on su9.us5

if [ "$ING_SRCRULES" ] ; then
    fgrist=`grep "^SubDir" Jamfile | awk '{ print "<" $3 "!" $4 "!" $5 ">"}'`
    fname=`echo $1 | cut -d. -f1`
    jam -an -d5 ${fgrist}${fname}.o  | awk '
 	$1 == "bind" { 
 		printf "%s\n", $4;
 	}'
 
else
    ming -o $* | awk '
	$1 == "make" { lev = lev + 1 }
	$1 == "done" { lev = lev - 1 }
	$1 == "file" {
		for( i = 1; i < lev; i++ )
			printf "    ";
		printf "%s\n", $2;
	}'
fi

