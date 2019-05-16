#ifndef DETECTOR_H
#define DETECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <gmp.h>

#include "processList.h"
#include "const.h"

typedef struct Event {
	int code;
	mpz_t * savedClock;
	ProcessList *processList;
	ProcessList *checkProcessList; // for WAIT only
	struct Event *next;
} Event;

Event * initEvent(int);
void freeEvent(Event *);

typedef struct Queue {
	Event *front;
	Event *rear;
} Queue;

Queue * initQueue();
void enqueue(Queue *, Event *);
Event * dequeue(Queue *);
void push(Queue *, Event *);
Event * findAnEventFromQueue(Queue *, int);
bool isEmpty(Queue *);
void freeQueue(Queue *);
int getNumberOfItemInEventQueue(Queue *);

#endif
