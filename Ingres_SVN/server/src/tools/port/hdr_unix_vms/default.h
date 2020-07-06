#define conf_INGRESII TRUE
#define conf_libc6 TRUE
#define conf_SVR_AR TRUE
#define conf_DBL TRUE
#define conf_B64 TRUE
#define conf_CAS_ENABLED TRUE
#define conf_BUILD_SPATIAL TRUE
XTERMLIB = /usr/X11R6/lib/libXaw.so /usr/X11R6/lib/libXmu.so /usr/X11R6/lib/libXt.so /usr/X11R6/lib/libX11.so /usr/lib/libtermcap.so
WINSYSLIB = /usr/X11R6/lib/libXmu.so /usr/X11R6/lib/libXt.so /usr/X11R6/lib/libX11.so
XNETLIB =
NETSCAPE_API_FLAG = -DXP_UNIX
APACHE_API_FLAG = -DLINUX=2
VERS = int_lnx
#define int_lnx
HDRMACH = /home/ingres/SANDBOX/Ingres_SVN/server/src/ha/opendlm/src/include
CPP = /lib/cpp
CCMACH = -fsigned-char -fno-strength-reduce -D_REENTRANT -DLINUX -D_GNU_SOURCE -DXLIB_ILLEGAL_ACCESS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
CCLDMACH = -rdynamic 
CCLDSERVER = 
CCPICFLAG = -fPIC
SLSFX = so
MWSLSFX = so
CGISFX = cgi
LDLIBPATH = /lib /usr/lib /usr/local/lib
LDLIBPATH64 = 
LIBMACH = 
#define LDLIB -lpthread -lrt -lm -lc -lcrypt -ldl /lib/libgcc_s.so.1
LDLIBMACH =   -lpthread -lrt -lm -lc -lcrypt -ldl /lib/libgcc_s.so.1
LDLIBMACH64 =   -lpthread -lrt -lm -lc -lcrypt -ldl /lib/libgcc_s.so.1
LD_ICE_FLAGS = 
CC = cc
OPTIM = -O
LEVEL1_OPTIM = -O1
SHELLHEADER = "#!/bin/sh"
#define HASRANLIB 1
#define XARGS 1
SYS5=1
DIRFUNC=1
DIRENT=1
INGRES6=1
SYSGRAPHLIB = 
SYMLINK=test -h
CXX=g++
CXX_DEFS= 
XERC_DEFS=-O -DXML_USE_NATIVE_TRANSCODER -DXML_USE_INMEM_MESSAGELOADER -DXML_USE_NETACCESSOR_SOCKET 
XERC_LIBS=
XERCVERS=27
