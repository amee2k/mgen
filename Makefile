.PHONY: all default clean
default: all

all: mgen

clean:
	-rm mgen
	-rm *.o
	-rm chardb.c


mgen: mgen.o msnd.o chardb.o
	gcc -o mgen -lSDL -lm  mgen.o msnd.o chardb.o

mgen.o: mgen.c msnd.h chardb.h
	gcc -o mgen.o -c mgen.c

msnd.o: msnd.c
	gcc -o msnd.o -c msnd.c


chardb.c: chardb dbformat.pl
	./dbformat.pl  <chardb >chardb.c

chardb.o: chardb.c 
	gcc -o chardb.o -c chardb.c


