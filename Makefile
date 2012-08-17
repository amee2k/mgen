.PHONY: all default clean
default: all

all: mgen

clean:
	-rm mgen
	-rm *.o


mgen: mgen.o
	gcc -o mgen mgen.o

mgen.o: mgen.c
	gcc -o mgen.o -c mgen.c



