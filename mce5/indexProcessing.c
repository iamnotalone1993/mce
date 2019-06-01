/*
 * indexProcess.c
 *
 *  Created on: Apr 2, 2019
 *      Author: kimloai
 */

#include "indexProcessing.h"
int nextIndex(IndexQueue * anIndexQueue, int aCurrentIndex, int status){
	if (isEmptyIndexQueue(anIndexQueue)){
		if (anIndexQueue -> lastIndexBeforeGotoQueue == -10){
			if (status & END_OF_ONE_FILE){
				return aCurrentIndex + 1;
			} else { //Default TODO: handle other situation
				return 0;
			}
		} else {
			int _returnValue = anIndexQueue -> lastIndexBeforeGotoQueue;
			anIndexQueue -> lastIndexBeforeGotoQueue = -10;
			return _returnValue;
		}
	} else { //queue is not empty
		// If duty completes then pop the first event in IndexQueue
		// else continue to return current index until the duty complete
		Index * _index = anIndexQueue -> front;
		if (((_index -> duty & status) && aCurrentIndex == _index -> index)  /*Duty complete: normal case*/
				|| ((_index -> duty & status) && _index -> duty == DETECT_MCE)){// duty complete: Detect MCE
			_index = popFromIndexQueue(anIndexQueue);
			if (anIndexQueue -> front == NULL){
				int _returnValue = anIndexQueue -> lastIndexBeforeGotoQueue;
				anIndexQueue -> lastIndexBeforeGotoQueue = -10;
				return _returnValue;
			} else {
				return anIndexQueue -> front -> index;
			}
			free(_index);
		} else { // the duty is uncompleted
			if (_index -> duty == DETECT_MCE){
				return -1;
			} else {
				return anIndexQueue -> front -> index;
			}
		}
	}
	return 0;
}

int getCurrentPriority(IndexQueue * anIndexQueue){
	return anIndexQueue -> currentPriority;
}

bool isFindDetectMCE(IndexQueue * anIndexQueue){
	return anIndexQueue -> findDetectMCE;
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

void pushToIndexQueue(IndexQueue * anIndexQueue, int aCurrentProcess, \
						int anIndex, int anEventCode, int aDuty, int aPriority){
	// update the new priority for the queue
	if (aPriority != INT_MAX){ // aPriority == INT_MAX means detectMCE
		if (anIndexQueue -> currentPriority < aPriority){
			anIndexQueue -> currentPriority = aPriority;
		}
	} else {
		anIndexQueue -> findDetectMCE = true;
	}

	// add a new Indes to the Queue
	if (anIndexQueue -> front == NULL){// Queue is empty
		Index * _index = malloc(sizeof(Index));
		_index -> duty = aDuty;
		_index -> eventCode = anEventCode;
		_index -> index = anIndex;
		_index -> priority = aPriority;
		_index -> next = NULL;
		anIndexQueue -> front = anIndexQueue -> rear = _index;
		//Save the last index to restore
		anIndexQueue -> lastIndexBeforeGotoQueue = aCurrentProcess;
	} else { // queue not empty
		//check if the duty has already in the queue
		// and find the position to push the new Index
		Index * _iterIndex = anIndexQueue -> front;
		while (_iterIndex != NULL){
			if ((_iterIndex -> duty == aDuty && _iterIndex -> index == anIndex)){
				return;
			}
			_iterIndex = _iterIndex -> next;
		}

		// the initiation of the new Index
		Index * _index = malloc(sizeof(Index));
		_index -> duty = aDuty;
		_index -> eventCode = anEventCode;
		_index -> index = anIndex;
		_index -> priority = aPriority;

		_index -> next = NULL;
		if (anIndexQueue -> front -> priority > aPriority) {// insert first
			_index -> next = anIndexQueue -> front;
			anIndexQueue -> front = _index;
		} else { //insert other position
			_iterIndex = anIndexQueue -> front;
			while (_iterIndex != NULL){ // find the position
				if (_iterIndex -> next != NULL && _iterIndex -> next -> priority > aPriority) {
					break;
				}
				_iterIndex = _iterIndex -> next;
			}
			if (_iterIndex == NULL){// this duty is the lowest priority, so insert last
				anIndexQueue -> rear -> next = _index;
				anIndexQueue -> rear = _index;
			} else { // insert to the _savedPosition
				_index -> next = _iterIndex -> next;
				_iterIndex -> next = _index;
			}
		}
	}
	//DEBUG_PRINT("#@#@##@#");
	//printIndexQueue(indexQueue);
}

void resetTheIndexQueue(IndexQueue * anIndexQueue){
	// reset the indexQueue
	anIndexQueue -> front = anIndexQueue -> rear = NULL;
	// reset the currentPriority
	anIndexQueue -> currentPriority = 0;
	anIndexQueue -> findDetectMCE = false;
}

Index *popFromIndexQueue(IndexQueue * anIndexQueue){
	if (anIndexQueue -> front == NULL && anIndexQueue -> rear == NULL){// Queue is empty
		return NULL;
	} else {
		Index * _pIndex = anIndexQueue -> front;
		if (anIndexQueue -> front == anIndexQueue -> rear) {// There is only one node in the queue
			resetTheIndexQueue(anIndexQueue);
		} else {
			anIndexQueue -> front = anIndexQueue -> front -> next;
		}
		return _pIndex;
	}
}

void printIndexQueue(IndexQueue * anIndexQueue){
	Index * _iter = anIndexQueue -> front;
	fprintf(stderr, "#**#Contain of indexQueue (index duty priority): ");
	while (_iter != NULL){
		fprintf(stderr, "%d %d %d;", _iter -> index, _iter -> duty, _iter -> priority);
		_iter = _iter -> next;
	}
	fprintf(stderr, "\n");
}
