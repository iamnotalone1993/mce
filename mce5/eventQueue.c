#include "eventQueue.h"

Event * initEvent(int eventCode) {
	Event *anEvent = (Event * ) malloc(sizeof(Event));
	anEvent -> code = eventCode;
	anEvent -> processList = initProcessList();
	anEvent -> next = NULL;
	return anEvent;
}

void freeEvent(Event * anEvent) {
	free(anEvent -> processList);
	free(anEvent);
}

Queue * initQueue() {
	Queue * _queue = (Queue * ) malloc(sizeof(Queue));
	_queue -> front = NULL;
	_queue -> rear = NULL;
	return _queue;
}

void enqueue(Queue * aQueue, Event * anEvent) {
	if (aQueue -> front == NULL) {
		aQueue -> front = anEvent;
	} else //if (aQueue->front != NULL)
	{
		aQueue -> rear -> next = anEvent;
	}
	aQueue -> rear = anEvent;
}

Event * dequeue(Queue * aQueue) {
	if (aQueue -> front == NULL) {
		return NULL;
	} else {//if (aQueue->front != NULL)
		Event * anEvent = aQueue -> front;
		if (aQueue -> front == aQueue -> rear) {
			aQueue -> front = aQueue -> rear = NULL;
		} else {//if (aQueue->front != aQueue->rear)
			aQueue -> front = aQueue -> front -> next;
		}
		return anEvent;
	}
}

void push(Queue * aQueue, Event * anEvent) {
	if (aQueue -> front == NULL) {
		aQueue -> front = anEvent;
		aQueue -> rear = anEvent;
	} else //if (aQueue->front != NULL)
	{
		anEvent -> next = aQueue -> front;
		aQueue -> front = anEvent;
	}
}

Event * retrieve(Queue * aQueue, int eventCode) {
	Event * anEvent = aQueue -> front;
	while (anEvent != NULL) {
		if (anEvent -> code == eventCode) {
			//TODO
			break;
		}
		anEvent = anEvent -> next;
	}
	return anEvent;
}

Event * findAnEventFromQueue(Queue * aQueue, int anEvent){
	/* Find the event from the Queue*/
	Event * _pEvent = aQueue -> front;
	while (_pEvent != NULL) {
		if (_pEvent -> code == anEvent) {
			return _pEvent;
			break;
		}
		_pEvent = _pEvent -> next;
	}
	return NULL;
}

bool isEmpty(Queue * aQueue) {
	if (aQueue -> front == NULL)
		return true;
	return false;
}

void freeQueue(Queue * aQueue) {
	free(aQueue);
}
