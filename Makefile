IDIR=include/
CC=g++
OUTFILE=reddan
CFLAGS=-I $(IDIR) -o $(OUTFILE)

EXTLIBS=-lreadline
LIBS = libs/*

SRCS=src/*

build:
	$(CC) $(SRCS) $(LIBS) $(CFLAGS) $(EXTLIBS)



