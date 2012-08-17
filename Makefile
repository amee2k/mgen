.PHONY: all default clean
default: all

all: mgen

clean:
	-rm mgen
	-rm *.o


mgen: mgen.o msnd.o
	gcc -o mgen -lSDL -lm  mgen.o msnd.o

mgen.o: mgen.c msnd.h
	gcc -o mgen.o -c mgen.c

msnd.o: msnd.c
	gcc -o msnd.o -c msnd.c



