CCFLAGS=-Dint_lnx -O
CC=cc
MESG="This source file is not delivered in the INGRES distribution."
SHELL=/bin/sh
INC=$(II_SYSTEM)/ingres/files
CFLAGS=$(CCFLAGS) -I$(INC)
OBJS        = iceapp.o
iceapp.cgi :	$(OBJS)
	$(CC) $(CFLAGS) -o$@ $(OBJS) -L$II_SYSTEM/ingres/lib -lingres -lpthread -lrt -lm -lc -lcrypt -ldl -lgcc_s
