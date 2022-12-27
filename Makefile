IDIR=include/
CC=g++
OUTFILE=reddan
CFLAGS=-I $(IDIR) -I remote/include -o $(OUTFILE) -std=c++17

EXTLIBS=-lreadline
LIBS = libs/*

SRCS=src/*

build:
	$(CC) $(SRCS) $(LIBS) $(CFLAGS) $(EXTLIBS)



