/*
 * detectQueue.c
 *
 *  Created on: May 11, 2019
 *      Author: kimloai
 */

#include "detectQueue.h"

void testGMP(void){
	mpz_t n;
	mpz_init(n);
	mpz_set_ui(n,1234);
	mpz_mul(n, n, n);

	printf ("n = ");
	mpz_out_str(stdout,10,n);
	printf ("\n");
}

DetectQueue * initDetectQueue(void){
	DetectQueue * _returnPtr = (DetectQueue *)malloc(sizeof(DetectQueue));
	_returnPtr -> front = _returnPtr -> rear = NULL;
	return _returnPtr;
}

DetectEvent * initDetectEvent(int anEventCode, int anIndex, mpz_t * aInitialClock){
	DetectEvent * _returnEvent = malloc(sizeof(DetectEvent));
	/* If the event is an internal event (it does not receive clock from other process)
	 * 		do a local tick
	 * else it receives some clock from other process (aClock != 1)
	 * 		do a LCM of the initialClock and current clock*/
	mpz_t * _currentClock = getCurrentClock(detectQueue[anIndex], anIndex);
	mpz_t * _previousClock = getPreviousClock(detectQueue[anIndex], anIndex);
	if (mpz_cmp_ui(*(aInitialClock), 1) == 0 || mpz_cmp(*(aInitialClock), *(_previousClock)) == 0 ){
		mpz_mul(*aInitialClock, * _currentClock, *(primeArr[anIndex]));
	} else {
		mpz_lcm(*(aInitialClock), *(aInitialClock), *(_currentClock));
	}

	_returnEvent -> code = anEventCode;
	_returnEvent -> clock = aInitialClock;
	_returnEvent -> next = NULL;
	return _returnEvent;
}

mpz_t * getPreviousClock(DetectQueue * aDetectQueue, int aProcess){
	assert(aDetectQueue -> front != NULL);
	if (aDetectQueue -> front == aDetectQueue -> rear){
		return aDetectQueue -> front -> clock;
	} else {
		DetectEvent * _iterDetectEvent = aDetectQueue -> front;
		while (_iterDetectEvent != NULL){
			if (_iterDetectEvent -> next == aDetectQueue -> rear){
				break;
			}
			_iterDetectEvent = _iterDetectEvent -> next;
		}
		return _iterDetectEvent -> clock;
	}
}

mpz_t * getCurrentClock(DetectQueue * aDetectQueue, int aProcess){
	if (isDetectQueueEmpty(aDetectQueue)){
		return primeArr[aProcess];
	} else {
		return aDetectQueue -> rear -> clock;
	}
}

bool isDetectQueueEmpty(DetectQueue * aDetectQueue){
	if (aDetectQueue -> front == NULL) return true;
	else return false;
}

void pushToDetectQueue(DetectQueue * aDetectQueue, DetectEvent * aDetectEvent){
	if (aDetectQueue -> front == NULL){
		aDetectQueue -> front = aDetectQueue -> rear = aDetectEvent;
	} else {
		aDetectQueue -> rear -> next = aDetectEvent;
		aDetectQueue -> rear = aDetectEvent;
		assert(aDetectEvent -> next == NULL);
	}
}
DetectEvent * popFromDetectQueue(DetectQueue * aDetectQueue){
	if (aDetectQueue -> front == NULL){
		return NULL;
	} else if (aDetectQueue -> front == aDetectQueue -> rear){
		DetectEvent * _tmpDetectEvent = aDetectQueue -> front;
		aDetectQueue -> front = aDetectQueue -> rear = NULL;
		return _tmpDetectEvent;
	} else {
		DetectEvent * _tmpDetectEvent = aDetectQueue -> front;
		aDetectQueue -> front = aDetectQueue -> front -> next;
		return _tmpDetectEvent;
	}
}
