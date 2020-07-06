:
#  Copyright (c) 2004 Ingres Corporation
#
#  Name: iihostnm -- return the local host name
#
#  Usage:
#	iihostnm [ temp_file ]
#
#  Description:
#	Returns the name of the local host to the configuration rules
#	system during setup procedures via the named temporary file.
#
## History:
##	18-jan-93 (tyler)
##		Created.	
##	26-apr-2000 (somsa01)
##		Updated MING hint for program name to account for different
##		products.
##
#  PROGRAM = (PROG1PRFX)hostnm
#
#  DEST = utility
#----------------------------------------------------------------------------

echo `hostname` > "$1"

