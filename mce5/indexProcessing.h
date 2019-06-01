/*
 * indexProcess.h
 *
 *  Created on: Apr 2, 2019
 *      Author: kimloai
 */

#ifndef INDEXPROCESSING_H_
#define INDEXPROCESSING_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gmp.h>

#include "const.h"
#include "debug.h"


typedef struct Index{
	int index;
	int eventCode;
	int duty;
	int priority;
	struct Index * next;
} Index;

typedef struct IndexQueue{
	Index * front;
	Index * rear;
	bool findDetectMCE;
	int currentPriority;
	int lastIndexBeforeGotoQueue;
} IndexQueue;

extern IndexQueue *indexQueue;

int nextIndex(IndexQueue *, int, int);
bool isEmptyIndexQueue(IndexQueue *);
bool isFindDetectMCE(IndexQueue *);
void resetTheIndexQueue(IndexQueue *);
int getNumOfItemInIndexQueue(IndexQueue *);
int getCurrentPriority(IndexQueue *);
void pushToIndexQueue(IndexQueue *, int, int, int, int, int);
Index *popFromIndexQueue(IndexQueue *);
void printIndexQueue(IndexQueue *);

#endif /* INDEXPROCESSINT_H_ */
