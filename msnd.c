// msnd.c

#define _MSND_INTERNAL 1
#include "msnd.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL_audio.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>

#define SRATE 22050
#define BSIZE 1024
#define VOLUME 3000
#define PI 3.14159265


static SDL_AudioSpec fmt;

static volatile struct msnd_item *queue = NULL; // head of queue (NULL _iif_ queue is empty)
static struct msnd_item *tail = NULL; // tail of queue (undefined _iif_ queue is empty)

// forward declaration
static void mixaudio(void *unused, Uint8 *stream, int len);
static struct msnd_item *msnd_pop();


int msnd_init() {
	SDL_AudioSpec req;

	req.freq = SRATE;
	req.format = AUDIO_S16;
	req.channels = 1;
	req.samples = BSIZE;
	req.callback = mixaudio;
	req.userdata = NULL;

	/* Open the audio device and start playing sound! */
	if ( SDL_OpenAudio(&req, &fmt) < 0 ) {
		fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
		return -1;
	}

	SDL_PauseAudio(0);
	return 0;
}

void msnd_deinit() {
	SDL_CloseAudio();
}



// queue management

int msnd_push(unsigned int duration, unsigned int frequency) {
	struct msnd_item *it = malloc(sizeof(struct msnd_item));
	if(it == NULL) return -1;

	// duration in samples
	it->duration = duration;
	it->frequency = frequency;
	it->next = NULL;

	// FIXME: does this require locking??
	SDL_LockAudio();
	if(queue == NULL) queue = it; else tail->next = it;
	tail = it;
	SDL_UnlockAudio();

	return 0;
}
int msnd_push2(double duration, unsigned int frequency) {
	// duration in fractional seconds
	return msnd_push(duration * fmt.freq, frequency);
}
int msnd_push3(long duration, unsigned int frequency) {
	// duration in microseconds
	return msnd_push(duration * fmt.freq / 1000000L, frequency);
}

void msnd_flush() {
	while(queue != NULL) usleep(5000);
}

static struct msnd_item *msnd_pop() {
	struct msnd_item *it = queue;
	if(it == NULL) return NULL;

	queue = it->next;

	return it;
}


// callback function

static struct msnd_item *ci = NULL; // current item (NULL _iif_ silence / nothing to play)
static unsigned int tx = 0; // time into current item
static unsigned long stime = 0; // absolute time in samples since program start


static int16_t next_sample() {
	int16_t s;

	//s = sin((double)stime * 2 * PI * BFO / (fmt.freq)) * VOLUME;
	//stime++;

	// have current item? if not, try get one from the queue
	if(ci == NULL) {
		ci = msnd_pop();

		// still no item? generate silence
		if(ci == NULL) return fmt.silence;

		// initialize variables
		tx = 0;
	}

	// generate sample
	if(ci->frequency == 0) {
		s = fmt.silence;
	} else {
		s = sin((double)tx * 2 * PI * ci->frequency / (fmt.freq)) * VOLUME;
	}
	tx++;

	// done playing this item?
	if(tx >= ci->duration) {
		free(ci);
		ci = NULL;
	}

	return s;
}

static void mixaudio(void *userdata, Uint8 *stream, int len) {
	int i;
	int16_t *buffer = stream;
	//printf("Buffer size: %d bytes (%d samples)\n", len, fmt.samples);

	for(i = 0; i < fmt.samples; i++) {
		//buffer[i] = sin((double)stime * 2 * PI * BFO / (fmt.freq)) * VOLUME;	stime++;
		buffer[i] = next_sample();
	}
}





