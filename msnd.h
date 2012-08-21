// msnd.h

#ifndef _MSND_H
#define _MSND_H 1

struct msnd_item {
	unsigned int duration;
	unsigned int frequency;

	struct msnd_item *next;
};

#ifndef _MSND_INTERNAL
extern int msnd_init();
extern void msnd_deinit();


// duration in samples
extern int msnd_push(unsigned int duration, unsigned int frequency);

// duration in fractional seconds
extern int msnd_push2(double duration, unsigned int frequency);

// duration in microseconds
extern int msnd_push3(long duration, unsigned int frequency);


extern void msnd_flush();

#endif

#endif

