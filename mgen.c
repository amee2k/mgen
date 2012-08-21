// mgen.c


#include "chardb.h"
#include "msnd.h"

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>



// constants for implicit string concatenation
// (redefine as empty strings to disable colors)
#define HILIT "\e[1m"
#define LOLIT "\e[0m"

#define BFO 1200
#define WPM 20

// paris standard
static unsigned int cu = 1200000L / WPM; // duration of a code unit in microseconds

#define KEYDN(u) msnd_push3(cu * (u), BFO)
#define KEYUP(u) msnd_push3(cu * (u), 0)

// forward declarations
static void process_line(char *line);
static void process_char(uint8_t x);
static void charmap_info();

// codeflow
static int8_t is_in(char needle, char *haystack);
static uint8_t arithmetic_right_shift(uint8_t x, uint8_t b);




int main() {
	static char buf[100]; 	// input buffer

	charmap_info();

	printf("\n" HILIT "Timing:" LOLIT "\n");
	printf("\tCode speed:  %u WPM\n", WPM);
	printf("\tTiming unit: %u microseconds\n", cu);
	printf("\tBFO:         %u Hz\n", BFO);

	printf("\n" HILIT "Symbols:" LOLIT "\n");
	printf("\t.            Short mark (Dit!)\n");
	printf("\t-            Long mark (Dah!)\n");
	printf("\t(1 blank)    Inter-element separation\n");
	printf("\t(3 blanks)   Letter separation\n");
	printf("\t(7 blanks)   Word separation\n");
	printf("\t+            End of message\n");

	printf("\n" HILIT "Input:" LOLIT "\n");
	printf("\ta-z/A-Z/0-9  Characters (case insensitive)\n");
	printf("\t#            Ligature mark (for prosign)\n");
	printf("\t(blank)      Word separation\n");
	printf("\t(newline)    End of message\n");
	printf("\t^D           Exit program (close stdin)\n");

	if(msnd_init() != 0) {
		printf("Failed to initialize audio device. Terminating.");
		return -1;
	}

	KEYDN(10);

	while(1) {

		// read next line from the terminal
		printf("\n" HILIT "Morse>" LOLIT " "); fflush(stdout);
		if(fgets(buf, sizeof(buf), stdin) == NULL) break;

		process_line(buf);

		// emit end of message
		printf(" +\n");
		msnd_flush();

	}

	msnd_deinit();

	printf("\n");
	return 0;
}






static void process_line(char *line) {
	uint8_t i;			// loop variables
	char c;				// current character;
	int8_t x;			// character index in charmap
	uint8_t ps;			// prosign flag

	// prosign flag always reset at the beginning of a line
	// Note: the flag is not used  _iif_  i == 0  holds, so this is really redundant ;)
	ps = 0;

	for(i = 0; line[i] != '\0'; i++) {
		// loop over it char by char, normalize case
		c = line[i];
		c = toupper(c);

		// find char in charmap
		x = is_in(c, charmap);
		if(x == -1) {
			// not in charmap

			// check if we have a space
			if(c == ' ') {
				printf("       "); // 7 spaces
				KEYUP(7);
			}

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
				KEYUP(3);
			} else {
				// inter-element separator (prosign)
				printf(" "); // 1 space
				KEYUP(1);
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
	uint8_t marker;

	// get encoded morse code sign from charmap
	s = charsigns[x];

	// calculate mask (if the buffer looks like this, we are done with the character)
	marker = arithmetic_right_shift(s, 7);

	// loop over each element
	while(s != marker) {

		// emit inter-element separator unless first element
		if(j != 0) {
			printf(" "); // single space
			KEYUP(1);
		}

		// emit actual code element
		if( s & 0x01 ) {
			printf("-");
			KEYDN(3);
		} else {
			printf(".");
			KEYDN(1);
		}

		// shift temporary variable to next element
		s = arithmetic_right_shift(s, 1);
		j++;
	}
}





// codeflow

static uint8_t arithmetic_right_shift(uint8_t x, uint8_t b) {
	while(b != 0) {
		x = (x & 0x80) | (x >> 1);
		b--;
	}
	return x;
}

static void charmap_info() {
	int i;

	printf(HILIT "Charmap" LOLIT " (database is %d bytes):\n", chardbsize);
	for(i = 0; charmap[i] != '\0'; i++) {
		printf("\t%c: bitmap %d\n", charmap[i], charsigns[i]);
	}
}

static int8_t is_in(char needle, char *haystack) {
	int i;
	for(i = 0; haystack[i] != '\0'; i++) {
		if(haystack[i] == needle) return i;
	}
	return -1;
}



