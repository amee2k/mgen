.PHONY: all default clean
default: all
CC=gcc
CCOPT=-W -Wall
LDOPT=-W -Wall -lSDL -lm

all: mgen test

clean:
	-rm mgen test
	-rm *.o
	-rm chardb.c


mgen: mgen.o msnd.o chardb.o
	$(CC) $(LDOPT) -o mgen  mgen.o msnd.o chardb.o

mgen.o: mgen.c msnd.h chardb.h
	$(CC) $(CCOPT) -o mgen.o -c mgen.c


test: test.o msnd.o chardb.o
	$(CC) $(LDOPT) -o test  test.o msnd.o chardb.o

test.o: test.c msnd.h chardb.h
	$(CC) $(CCOPT) -o test.o -c test.c


msnd.o: msnd.c
	$(CC) $(CCOPT) -o msnd.o -c msnd.c


chardb.c: chardb dbformat.pl
	./dbformat.pl  <chardb >chardb.c

chardb.o: chardb.c chardb.h
	$(CC) $(CCOPT) -o chardb.o -c chardb.c


