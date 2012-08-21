// test.c


#include "chardb.h"
#include "msnd.h"

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>

//extern int msnd_push(double duration, unsigned int frequency);


// forward declarations
static void sndstring(char *s);


int main() {

	msnd_init();

	msnd_push3(500000, 600);
	msnd_push3(500000, 0);
	msnd_push3(500000, 600);
	msnd_push3(500000, 0);
	msnd_push3(500000, 600);
	msnd_push3(500000, 0);
	msnd_flush();

	msnd_deinit();

	return 0;
}
static void sndstring(char *s) {
	int i;
	for(i = 0; s[i] != '\0'; i++) {
		switch(s[i]) {
		case '.':
			break;
		case '-':
			break;
		case ' ':
			break;
		case '\t':
			break;
		}
	}
}




