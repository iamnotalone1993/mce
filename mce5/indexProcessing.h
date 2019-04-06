/*
 * indexProcess.h
 *
 *  Created on: Apr 2, 2019
 *      Author: kimloai
 */

#ifndef INDEXPROCESSING_H_
#define INDEXPROCESSING_H_

#include <stdbool.h>
#include <stdlib.h>
#include "const.h"

typedef struct Index{
	int index;
	int eventCode;
	struct Index * next;
} Index;

typedef struct IndexQueue{
	Index * front;
	Index * rear;
} IndexQueue;

int nextIndex(IndexQueue *, int, int);
bool isEmptyEventQueue(IndexQueue *);
void pushToIndexQueue(IndexQueue *, int, int);
Index *popFromIndexQueue(IndexQueue *);


#endif /* INDEXPROCESSINT_H_ */
