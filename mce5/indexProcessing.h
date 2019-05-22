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
	struct Index * next;
} Index;

typedef struct IndexQueue{
	Index * front;
	Index * rear;
	int lastIndexBeforeGotoQueue;
} IndexQueue;

int nextIndex(IndexQueue *, int, int);
bool isEmptyIndexQueue(IndexQueue *);
int getNumOfItemInIndexQueue(IndexQueue *);
void pushToIndexQueue(IndexQueue *, int, int, int, int);
Index *popFromIndexQueue(IndexQueue *);
void printIndexQueue(IndexQueue *);

#endif /* INDEXPROCESSINT_H_ */
