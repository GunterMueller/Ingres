CCFLAGS=-Dint_lnx -O
CC=cc
LDFLAGS=-L/$(II_SYSTEM)/ingres/lib -lingres -lpthread -lrt -lm -lc -lcrypt -ldl /lib/libgcc_s.so.1
LDLIBS=-L/$(II_SYSTEM)/ingres/lib -lingres -lpthread -lrt -lm -lc -lcrypt -ldl /lib/libgcc_s.so.1
MESG="This source file is not delivered in the INGRES distribution."
SHELL=/bin/sh
INC=$(II_SYSTEM)/ingres/files
CFLAGS=$(CCFLAGS) -I$(INC)
APIAOBJ=apiademo.o apiaclnt.o apiasvr.o apiautil.o
all: apiademo apis2ph1 apis2ph2 apisauto apisblob apiscdel apiscomm apisconn apiscopy apiscupd apisdata apiserr apisinit apisname apisparm apisprbr apisprgt apisproc apisprrp apisrept apisroll apisselc apissell apisscrl
apiademo: $(APIAOBJ)
	$(CC) $(CFLAGS) -o apiademo $(APIAOBJ) $(LDFLAGS)
