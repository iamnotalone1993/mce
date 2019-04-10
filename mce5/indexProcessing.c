/*
 * indexProcess.c
 *
 *  Created on: Apr 2, 2019
 *      Author: kimloai
 */

#include "indexProcessing.h"
int nextIndex(IndexQueue * anIndexQueue, int aCurrentIndex, int status){
	if (isEmptyIndexQueue(anIndexQueue)){
		if (anIndexQueue -> lastIndexBeforeGotoQueue == -1){
			if ((status & END_OF_ONE_FILE) == END_OF_ONE_FILE){
				return aCurrentIndex + 1;
			} else { //Default TODO: handle other situation
				return 0;
			}
		} else {
			int _returnValue = anIndexQueue -> lastIndexBeforeGotoQueue;
			anIndexQueue -> lastIndexBeforeGotoQueue = -1;
			return _returnValue;
		}

	} else { //queue is not empty
		Index * _index = popFromIndexQueue(anIndexQueue);
		int _returnIndex = _index -> index;
		assert(_index -> eventCode != POST);
		free(_index);
		return _returnIndex;
	}
	return 0;
}

bool isEmptyIndexQueue (IndexQueue *anIndexQueue){
	if (anIndexQueue -> front == NULL)
		return true;
	else
		return false;
}

void pushToIndexQueue(IndexQueue * anIndexQueue, int aCurrentProcess, int anIndex, int anEventCode){
	Index * _index = malloc(sizeof(Index));
	_index -> eventCode = anEventCode;
	_index -> index = anIndex;
	if (anIndexQueue -> front == NULL && anIndexQueue -> rear == NULL){// Queue is empty
		_index -> next = NULL;
		anIndexQueue -> front = anIndexQueue -> rear = _index;
		//Save the last index to restore
		anIndexQueue -> lastIndexBeforeGotoQueue = aCurrentProcess;
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
