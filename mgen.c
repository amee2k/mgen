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


// constants for implicit string concatenation
// (redefine as empty strings to disable colors)
#define HILIT "\e[1m"
#define LOLIT "\e[0m"



// forward declarations
static void process_line(char *l);
static void process_char(uint8_t x);
static void charmap_info();

// codeflow
static int8_t is_in(char needle, char *haystack);
static uint8_t arithmetic_right_shift(uint8_t x, uint8_t b);




int main() {
	static char buf[100]; 	// input buffer

	charmap_info();

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
/*
 * A	2	.-
 * B	4	-...
 * C	4	-.-.
 * D	3	-..
 * E	1	.
 * Q	4	--.-
 * R	3	.--
 * 1	5	.----
 * $	7	...-..-
 *
 */

/* Encode character (letter -> table entry):
 *
 *	- dit == 0; dah == 1
 *	- start of code is always at the LSB. the code is right-aligned
 *	- pad to 8 bits using the opposite of the last encoded bit for padding
 *
 */

/* Decode character (table entry -> morse code):
 *
 *	- start of code is always at the LSB. the code is right-aligned
 *	- then shift and produce code elements until the buffer == code >>> 7
 *	- dit == 0; dah == 1
 *
 */

#define DIT(tail) (0 | ((tail) << 1))
#define DAH(tail) (1 | ((tail) << 1))
#define FLIP 1
#define NOFLIP 0
#define CODEPOINT(tail, length) ( (tail & (1<<(length-1))) == 0 ? 0xFF : 0x00 )  &  ~( (1<<(length)) - 1 )  |  tail

static char charmap[] = "ABCDEQR1$";

/*

C: - . - .

Code:			0 0 0 0  0 1 0 1		(4 (length) bits right aligned)


Padding:		1 1 1 1  1 1 1 1		(last encoded element is a dit = 0)
Mask:			1 1 1 1  0 0 0 0		(4 (length) bits right aligned, then inverted)
				----------------
Padding & Mask: 1 1 1 1  0 0 0 0


Code | PM:		1 1 1 1  0 1 0 1		( -> 245 )



*/


static uint8_t charsigns[] = {
	// this probably looks better if you experiment a bit with spacing...
	CODEPOINT( DIT(DAH(0)),								2),	// A
	CODEPOINT( DAH(DIT(DIT(DIT(0)))),					4),	// B
	CODEPOINT( DAH(DIT(DAH(DIT(0)))),					4),	// C
	CODEPOINT( DAH(DIT(DIT(0))),						3),	// D
	CODEPOINT( DIT(0),									1),	// E
	CODEPOINT( DAH(DAH(DIT(DAH(0)))),					4),	// Q
	CODEPOINT( DIT(DAH(DAH(0))),						3),	// R
	CODEPOINT( DIT(DAH(DAH(DAH(DAH(0))))),				5),	// 1
	CODEPOINT( DIT(DIT(DIT(DAH(DIT(DIT(DAH(0))))))),	5),	// $
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
	uint8_t marker;

	// get encoded morse code sign from charmap
	s = charsigns[x];

	// calculate mask (if the buffer looks like this, we are done with the character)
	marker = arithmetic_right_shift(s, 7);

	// loop over each element
	while(s != marker) {

		// emit inter-element separator unless first element
		if(j != 0) printf(" "); // single space

		// emit actual code element
		if( s & 0x01 ) printf("-"); else printf(".");

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
	uint8_t i;

	printf(HILIT "Charmap" LOLIT " (database is %d bytes):\n", (int)(sizeof(charmap) + sizeof(charsigns)));
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



