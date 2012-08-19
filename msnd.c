// msnd.c

#include "msnd.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL_audio.h>
#include <stdint.h>
#include <math.h>

#define SRATE 22050
#define BFO 1200
#define WPM 30
#define BSIZE 1024
#define VOLUME 3000
#define PI 3.14159265

static unsigned long stime = 0; // time in samples

static SDL_AudioSpec fmt;

// forward declaration
static void mixaudio(void *unused, Uint8 *stream, int len);

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

static void mixaudio(void *userdata, Uint8 *stream, int len) {
	int i;
	int16_t *buffer = stream;
	printf("Buffer size: %d bytes (%d samples)\n", len, fmt.samples);

	for(i = 0; i < fmt.samples; i++) {
		buffer[i] = sin((double)stime * 2 * PI * BFO / (fmt.freq)) * VOLUME;
		stime++;
	}
}


