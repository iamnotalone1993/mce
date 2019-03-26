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

#endif /* PROCESSLIST_H_ */
