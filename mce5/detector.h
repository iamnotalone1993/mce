#ifndef DETECTOR_H
#define DETECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE             512
#define BASE                    10

#define DEFAULT                 0
#define FENCE                   1
#define BARRIER                 2
#define GET                     3
#define PUT                     4
#define ACCUMULATE              5
#define POST                    6
#define START                   7
#define COMPLETE                8
#define WAIT                    9
#define LOCK                    10
#define UNLOCK                  11
#define SEND                    12
#define RECV                    13
#define LOAD                    14
#define STORE                   15
#define CREATE                  16

typedef struct Int {
	int num;
	struct Int *next;
} Int;

Int *initInt(Int *, int);
void freeInt(Int *);

typedef struct List {
	Int *head;
	Int *tail;
} List;

List *initList(List *);
void insertInt2List(List *, Int *);
Int *getIntfromList(List *);
void freeList(List *);

typedef struct Event {
	int code;
	List *intList;
	struct Event *next;
} Event;

Event *initEvent(Event *, int);
void freeEvent(Event *);

typedef struct Queue {
	Event *front;
	Event *rear;
} Queue;

Queue *initQueue(Queue *);
void enqueue(Queue *, Event *);
Event *dequeue(Queue *);
void push(Queue *, Event *);
Event *retrieve(Queue *, int);
bool isEmpty(Queue *);
void freeQueue(Queue *);

int getEventCode(char *);
char *getData(char **);

#endif
