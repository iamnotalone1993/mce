#ifndef DETECTOR_H
#define DETECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define	BUFFER_SIZE		30

#define	FENCE			1
#define	BARRIER			2
#define	GET			3
#define	PUT			4
#define	ACCUMULATE		5
#define	POST			6
#define	START			7
#define	COMPLETE		8
#define	WAIT			9
#define	LOCK			10
#define	UNLOCK			11
#define	SEND			12
#define	RECV			13
#define	LOAD			14
#define	STORE			15
#define CREATE			16

/*typedef struct {
        int code;
        int *clock;
        void *next;
} Sync;*/

typedef struct Comm {
        int code;
	char *origin_addr;
	int target_rank;
	char *target_addr;
	//int *clock;
        struct Comm *next;
} Comm;

typedef struct Loca {
        int code;
        char *varAddr;
	//int *clock;
        struct Loca *next;
} Loca;

typedef struct List {
        char *base;
        int size;
        int disp_unit;
        struct Comm *commHead;
	struct Comm *commTail;
        struct Loca *locaHead;
	struct Loca *locaTail;
} List;

typedef struct Chai {
	int rank;
	struct Loca *head;
	struct Loca *tail;
} Chai;

List insertCommNode(List, int, char *, int, char * /*, int *, int*/);
List insertLocaNode(List, int, char *);
Chai insertChain(Chai, int, char *);

int getEventCode(char *);
char *convertCode2Name(int);
int *getClock(char *);
bool equalClock(int *, int *, int);

void detectMCEInProc(Chai);

#endif
