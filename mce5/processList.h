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
void freeProcessList(ProcessList *);

#endif /* PROCESSLIST_H_ */
