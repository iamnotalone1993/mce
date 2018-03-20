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

typedef struct Comm {
        int code;
	int target_rank;
	char *target_addr;
        struct Comm *next;
} Comm;

typedef struct Loca {
        int code;
        char *varAddr;
        struct Loca *next;
} Loca;

typedef struct Node {
	int *clock;
	struct Comm *commHead;
	struct Comm *commTail;
	struct Loca *locaHead;
	struct Loca *locaTail;
	struct Node *next;
} Node;

typedef struct List {
        char *base;
        int size;
        int disp_unit;
        struct Node *head;
	struct Node *tail;
} List;

typedef struct Chai {
	int rank;
	struct Loca *head;
	struct Loca *tail;
} Chai;

int getEventCode(char *);
char *convertCode2Name(int);
int *getClock(char *, int);
bool equalClock(int *, int *, int);
char *getData(char **);
void printClock(int *, int);
int isConcurrent(int *, int, int *, int);

Comm *initComm(int, int, char *);
void freeComm(Comm *);
void insertCommNode(Node *, Comm *);
void printComm(Comm *);

Loca *initLoca(int, char *);
void freeLoca(Loca *);
void insertLocaNode(Node *, Loca *);
void printLoca(Loca *);

Node *initNode(int *);
void freeNode(Node *);
void printNode(Node *, int);

List *initList(char *, int, int);
void insertList(List *, Node *);
void freeList(List *);
void freeAllList(List **, int);
void printList(List *, int);
void printAllList(List **, int);

Chai *initChain(int);
void insertChain(Chai *, Loca *);
void printChain(Chai *);

void readEventWithinEpoch(FILE **, int, List **, Chai *, int);
void detectMCEInProc(Chai *);
void detectMCEAcrossProc(List **, int);

#endif
