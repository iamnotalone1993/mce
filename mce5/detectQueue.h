/*
 * detectQueue.h
 *
 *  Created on: May 11, 2019
 *      Author: kimloai
 */

#ifndef DETECTQUEUE_H_
#define DETECTQUEUE_H_

#include "debug.h"
#include "const.h"

#include <assert.h>
#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct DetectEvent{
	int code;
	mpz_t * clock;
	struct DetectEvent *next;
} DetectEvent;

typedef struct DetectQueue{
	DetectEvent *front, *rear;
} DetectQueue;

extern mpz_t ** primeArr;
extern DetectQueue ** detectQueue;
extern int size;

DetectQueue * initDetectQueue(void);
DetectEvent * initDetectEvent(int, int, mpz_t *);
bool isDetectQueueEmpty(DetectQueue *);
mpz_t * getPreviousClock(DetectQueue *, int);
mpz_t * getCurrentClock(DetectQueue *, int);
void pushToDetectQueue(DetectQueue *, DetectEvent *);
DetectEvent * popFromDetectQueue(DetectQueue *);
int detectMCE(DetectQueue **);


#endif /* DETECTQUEUE_H_ */
