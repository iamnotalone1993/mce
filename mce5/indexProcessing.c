/*
 * indexProcess.c
 *
 *  Created on: Apr 2, 2019
 *      Author: kimloai
 */

#include "indexProcessing.h"
int nextIndex(IndexQueue * anIndexQueue, int aCurrentIndex, int status){

	return 0;
}

bool isEventQueueEmpty (IndexQueue *anIndexQueue){
	if (anIndexQueue -> front == NULL)
		return true;
	else
		return false;
}

void pushToIndexQueue(IndexQueue * anIndexQueue, int anIndex, int anEventCode){
	Index * _index = malloc(sizeof(Index));
	_index -> eventCode = anEventCode;
	_index -> index = anIndex;
	if (anIndexQueue -> front == NULL && anIndexQueue -> rear == NULL){// Queue is empty
		_index -> next = NULL;
		anIndexQueue -> front = anIndexQueue -> rear = _index;
	} else { // queue not empty
		_index -> next = anIndexQueue -> front;
		anIndexQueue -> front = _index;
	}
}

Index *popFromIndexQueue(IndexQueue * anIndexQueue){
	if (anIndexQueue -> front == NULL && anIndexQueue -> rear == NULL){// Queue is empty
		return NULL;
	} else {
		Index * _pIndex = anIndexQueue -> front;
		if (anIndexQueue -> front == anIndexQueue -> rear) {// There is one node in the queue
			anIndexQueue -> front = anIndexQueue -> rear = NULL;
		} else {
			anIndexQueue -> front = anIndexQueue -> front -> next;
		}
		return _pIndex;
	}
}
