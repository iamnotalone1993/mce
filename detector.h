#ifndef DETECTOR_H
#define DETECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define	BUFFER_SIZE	30

#define	FENCE		1
#define	BARRIER		2
#define	PUT		3
#define	GET		4
#define	ACCUMULATE	5
#define	POST		6
#define	START		7
#define	COMPLETE	8
#define	WAIT		9
#define	LOCK		10
#define	UNLOCK		11
#define	SEND		12
#define	RECV		13
#define	LOAD		14
#define	STORE		15
#define CREATE		16

typedef struct {
	void *base;
	int size;
	int disp_unit;
	void *next;
} List;

typedef struct {
        int code;
        void *next;
} Comm;

typedef struct {
        int code;
        int *clock;
        void *next;
} Sync;

typedef struct {
        int code;
        void *varAddr;
        void *next;
} Loca;

int isSynCall(char *);
int *getClock(char *);
bool equalClock(int *, int *, int);

#endif
