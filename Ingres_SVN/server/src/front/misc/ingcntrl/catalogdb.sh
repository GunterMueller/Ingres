:
#
#	Copyright (c) 2004 Ingres Corporation
#	All rights reserved.
#

#
# Name:    catalogdb -	Ingres DBMS Access Control Program.
#
# Description:
#	A UNIX only shell to call IngCntrl in its catalogdb mode.
#
umask 1

ingcntrl $*
