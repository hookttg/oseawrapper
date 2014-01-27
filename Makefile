CC = gcc

ETOBJS = analbeat.o bdac.o classify.o match.o noisechk.o postclas.o qrsfilt.o rythmchk.o

all:		oseawrapper

oseawrapper:	$(ETOBJS) qrsdet.o oseawrapper.o
	$(CC) -o oseawrapper oseawrapper.o $(ETOBJS) qrsdet.o

oseawrapper.o:	oseawrapper.c qrsdet.h
	$(CC) -c oseawrapper.c

bdac.o:		bdac.c bdac.h qrsdet.h
	$(CC) -c bdac.c

classify.o:	classify.c qrsdet.h bdac.h match.h rythmchk.h analbeat.h postclas.h
	$(CC) -c classify.c

rythmchk.o:	rythmchk.c qrsdet.h
	$(CC) -c rythmchk.c

noisechk.o:	noisechk.c qrsdet.h
	$(CC) -c noisechk.c

match.o:	match.c bdac.h
	$(CC) -c match.c

postclas.o:	postclas.c bdac.h
	$(CC) -c postclas.c

analbeat.o:	analbeat.c bdac.h
	$(CC) -c analbeat.c

qrsfilt.o:	qrsfilt.c qrsdet.h
	$(CC) -c qrsfilt.c

qrsdet.o:	qrsdet.c qrsdet.h
	$(CC) -c qrsdet.c

clean:
	rm -f *.o *~ oseawrapper
