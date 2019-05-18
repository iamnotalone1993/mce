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
			if (status & END_OF_ONE_FILE){
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
		// If duty completes then pop the first event in IndexQueue
		// else continue to return current index until the duty complete
		Index * _index = anIndexQueue -> front;
		if (_index -> duty & status){
			_index = popFromIndexQueue(anIndexQueue);
			int _returnIndex = _index -> index;
			free(_index);
			return _returnIndex;
		} else {
			// if the duty is to detect mce -> return a streng index to trigger detectMCE
			if (_index -> duty == DETECT_MCE) return TO_DETECT_MCE;
			else return _index -> index;
		}
	}
	return 0;
}

bool isEmptyIndexQueue (IndexQueue *anIndexQueue){
	if (anIndexQueue -> front == NULL)
		return true;
	else
		return false;
}

int getNumOfItemInIndexQueue(IndexQueue * anIndexQueue){
	Index * _iterIndex = anIndexQueue -> front;
	int _count = 0;
	while (_iterIndex != NULL){
		_count ++;
		_iterIndex = _iterIndex -> next;
	}
	return _count;
}

void pushToIndexQueue(IndexQueue * anIndexQueue, int aCurrentProcess, int anIndex, int anEventCode, int aDuty){
	if (anIndexQueue -> front == NULL && anIndexQueue -> rear == NULL){// Queue is empty
		Index * _index = malloc(sizeof(Index));
		_index -> duty = aDuty;
		_index -> eventCode = anEventCode;
		_index -> index = anIndex;
		_index -> next = NULL;
		anIndexQueue -> front = anIndexQueue -> rear = _index;
		//Save the last index to restore
		anIndexQueue -> lastIndexBeforeGotoQueue = aCurrentProcess;
	} else { // queue not empty
		//check if the duty has already in the queue
		Index * _iterIndex = anIndexQueue -> front;
		while (_iterIndex != NULL){
			if ((_iterIndex -> duty == aDuty && _iterIndex -> index == anIndex)\
					|| _iterIndex -> duty == DETECT_MCE){
				return;
			}
			_iterIndex = _iterIndex -> next;
		}
		Index * _index = malloc(sizeof(Index));
		_index -> duty = aDuty;
		_index -> eventCode = anEventCode;
		_index -> index = anIndex;

		_index -> next = NULL;
		anIndexQueue -> rear -> next = _index;
		anIndexQueue -> rear = _index;
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
