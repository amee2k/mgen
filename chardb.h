// chardb.h

#ifndef _CHARDB_H
#define _CHARDB_H 1

#include <stdint.h>


/*

C: - . - .

Code:			0 0 0 0  0 1 0 1		(4 (length) bits right aligned)


Padding:		1 1 1 1  1 1 1 1		(last encoded element is a dit = 0)
Mask:			1 1 1 1  0 0 0 0		(4 (length) bits right aligned, then inverted)
				----------------
Padding & Mask: 1 1 1 1  0 0 0 0


Code | PM:		1 1 1 1  0 1 0 1		( -> 245 )

*/

 
/* Encode character (letter -> table entry):
 *
 * - dit == 0; dah == 1
 * - start of code is always at the LSB. the code is right-aligned
 * - pad to 8 bits using the opposite of the last encoded bit for padding
 *
 */
 
/* Decode character (table entry -> morse code):
 *
 * - start of code is always at the LSB. the code is right-aligned
 * - then shift and produce code elements until the buffer == code >>> 7
 * - dit == 0; dah == 1
 *
 */

#define DIT(tail) (0 | ((tail) << 1))
#define DAH(tail) (1 | ((tail) << 1))
#define FLIP 1
#define NOFLIP 0
#define CODEPOINT(tail, length) ( (tail & (1<<(length-1))) == 0 ? 0xFF : 0x00 )  &  ~( (1<<(length)) - 1 )  |  tail

#ifndef _CHARDB_INTERNAL
extern char charmap[];
extern uint8_t charsigns[];
extern int chardbsize;
#endif

#endif

