CCFLAGS=-DLNX -Dint_lnx -fPIC -O -o$@
LDFLAGS=-shared
SHLSFX=so
CC=cc
MESG="This source file is not delivered in the INGRES distribution."
SHELL=/bin/sh
INC=$(II_SYSTEM)/ingres/files
CFLAGS=$(CCFLAGS) -I$(INC)
all: libdemoudt.1.$(SHLSFX)
common.o:	common.c $(INC)/iiadd.h
	$(CC) $(CFLAGS) -c common.c
cpx.o:	cpx.c $(INC)/iiadd.h udt.h
	$(CC) $(CFLAGS) -c cpx.c
intlist.o:	intlist.c $(INC)/iiadd.h udt.h
	$(CC) $(CFLAGS) -c intlist.c
op.o:	op.c $(INC)/iiadd.h udt.h
	$(CC) $(CFLAGS) -c op.c
zchar.o:	zchar.c $(INC)/iiadd.h udt.h
	$(CC) $(CFLAGS) -c zchar.c
iimhpk.o:	$(INC)/iiadd.h iipk.h
	@[ -f iimhpk.c ] || echo $(MESG)
	@[ -f iimhpk.c ] && $(CC) $(CFLAGS) -c iimhpk.c
iicvpk.o:	$(INC)/iiadd.h iipk.h
	@[ -f iicvpk.c ] || echo $(MESG) 
	@[ -f iicvpk.c ] && $(CC) $(CFLAGS) -c iicvpk.c
numeric.o:	numeric.c $(INC)/iiadd.h iipk.h udt.h
	$(CC) $(CFLAGS) -c numeric.c
nvarchar.o:	nvarchar.c $(INC)/iiadd.h udt.h
	$(CC) $(CFLAGS) -c nvarchar.c
libdemoudt.1.$(SHLSFX) : common.o cpx.o iicvpk.o iimhpk.o intlist.o numeric.o op.o zchar.o nvarchar.o
	$(CC) $(LDFLAGS) -o libdemoudt.1.$(SHLSFX) common.o cpx.o iicvpk.o iimhpk.o intlist.o numeric.o op.o zchar.o nvarchar.o
