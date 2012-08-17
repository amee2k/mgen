// mgen.c

/*

tloch@beagle:~/tmp/mgen$ gcc -o mgen mgen.c 
tloch@beagle:~/tmp/mgen$ ./mgen
Charmap (database is 25 bytes):
	A: bitmap 2, length 2
	B: bitmap 1, length 4
	C: bitmap 5, length 4
	D: bitmap 1, length 3
	E: bitmap 0, length 1
	Q: bitmap 11, length 4
	R: bitmap 6, length 3
	1: bitmap 30, length 5

Symbols:
	.            Short mark (Dit!)
	-            Long mark (Dah!)
	(1 blank)    Inter-element separation
	(3 blanks)   Letter separation
	(7 blanks)   Word separation
	+            End of message

Input:
	a-z/A-Z/0-9  Characters
	#            Prosign mark
	(blank)      Word separation
	(newline)    End of message

Morse> CQ CQ CQ DE A1AA A1AA A#R
- . - .   - - . -          - . - .   - - . -          - . - .   - - . -          - . .   .          . -   . - - - -   . -   . -          . -   . - - - -   . -   . -          . - . - - +

Morse> 

*/

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

// input buffer
static char buf[100];


// constants for implicit string concatenation
// (redefine as empty strings to disable colors)
#define HILIT "\e[1m"
#define LOLIT "\e[0m"



// forward declarations
static int8_t is_in(char needle, char *haystack);
static void process_line(char *l);
static void process_char(uint8_t x);
static void charmap_info();


int main() {

	printf("\n" HILIT "Symbols:" LOLIT "\n");
	printf("\t.            Short mark (Dit!)\n");
	printf("\t-            Long mark (Dah!)\n");
	printf("\t(1 blank)    Inter-element separation\n");
	printf("\t(3 blanks)   Letter separation\n");
	printf("\t(7 blanks)   Word separation\n");
	printf("\t+            End of message\n");

	printf("\n" HILIT "Input:" LOLIT "\n");
	printf("\ta-z/A-Z/0-9  Characters\n");
	printf("\t#            Prosign mark\n");
	printf("\t(blank)      Word separation\n");
	printf("\t(newline)    End of message\n");


	while(1) {

		// read next line from the terminal
		printf("\n" HILIT "Morse>" LOLIT " "); fflush(stdout);
		if(fgets(buf, sizeof(buf), stdin) == NULL) break;

		process_line(buf);

		// emit end of message
		printf(" +\n");

	}

	printf("\n");
	return 0;
}







// morse code character map

#define DIT(tail) (0 | ((tail) << 1))
#define DAH(tail) (1 | ((tail) << 1))

static char charmap[] = "ABCDEQR1";

static uint8_t charlens[] = {
	2, 4, 4, 3, 1, 4, 3, 5,
};

static uint8_t charsigns[] = {
	// this probably looks better if you experiment a bit with spacing...
	DIT(DAH(0)),
	DAH(DIT(DIT(DIT(0)))),
	DAH(DIT(DAH(DIT(0)))),
	DAH(DIT(DIT(0))),
	DIT(0),
	DAH(DAH(DIT(DAH(0)))),
	DIT(DAH(DAH(0))),
	DIT(DAH(DAH(DAH(DAH(0))))),
};





static void process_line(char *l) {
	uint8_t i, j;		// loop variables
	char c;				// current character;
	int8_t x;			// character index in charmap
	uint8_t ps;			// prosign flag

	// prosign flag always reset at the beginning of a line
	// Note: the flag is not used  _iif_  i == 0  holds, so this is really redundant ;)
	ps = 0;

	for(i = 0; buf[i] != '\0'; i++) {
		// loop over it char by char, normalize case
		c = buf[i];
		c = toupper(c);

		// find char in charmap
		x = is_in(c, charmap);
		if(x == -1) {
			// not in charmap

			// check if we have a space
			if(c == ' ') printf("       "); // 7 spaces

			// check if we have a prosign mark
			if(c == '#') ps = 1;

			// continue with next char
			continue;
		}

		// emit letter separator unless first letter
		if(i != 0) {
			if(ps == 0) {
				// letter spearator
				printf("   "); // 3 spaces
			} else {
				// inter-element separator (prosign)
				printf(" "); // 1 space
			}
		}

		process_char(x);

		// reset prosign flag
		ps = 0;
	}
}

static void process_char(uint8_t x) {
	uint8_t j;
	uint8_t s;

	// get encoded morse code sign from charmap
	s = charsigns[x];

	// loop over each element in the sign
	for(j = 0; j < charlens[x]; j++) {

		// emit inter-element separator unless first element
		if(j != 0) printf(" "); // single space

		// emit actual code element
		if( (s & 0x01) == DAH(0) ) printf("-"); else printf(".");

		// shift temporary variable to next element
		s >>= 1;
	}
}


// codeflow

static void charmap_info() {
	uint8_t i;

	printf(HILIT "Charmap" LOLIT " (database is %d bytes):\n", (int)(sizeof(charmap) + sizeof(charsigns) + sizeof(charlens)));
	for(i = 0; charmap[i] != '\0'; i++) {
		printf("\t%c: bitmap %d, length %d\n", charmap[i], charsigns[i], charlens[i]);
	}
}

static int8_t is_in(char needle, char *haystack) {
	int i;
	for(i = 0; haystack[i] != '\0'; i++) {
		if(haystack[i] == needle) return i;
	}
	return -1;
}



