/*
 * processList.h
 *
 *  Created on: Mar 26, 2019
 *      Author: kimloai
 */

#ifndef PROCESSLIST_H_
#define PROCESSLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "const.h"

typedef struct Process {
	int num;
	struct Process *next;
} Process;

Process *initProcess(int);
void freeProcess(Process *);

typedef struct ProcessList {
	Process *head;
	Process *tail;
} ProcessList;

ProcessList *initProcessList();
void insertProcess2ProcessList(ProcessList *, Process *);
Process *getProcessfromProcessList(ProcessList *);
bool findProcessFromProcessList(ProcessList *, Process*);
int removeAprocessFromProcessList(ProcessList *, int);
void freeProcessList(ProcessList *);
bool isProcessListEmpty(ProcessList *);
int getNumberOfItemInProcessList(ProcessList *);
bool isTheItemEqual(ProcessList *, ProcessList *);
void printProcessList(ProcessList *, char *);

#endif /* PROCESSLIST_H_ */
