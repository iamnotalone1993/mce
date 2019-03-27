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

Queue * initQueue(Queue * aQueue) {
	aQueue = (Queue * ) malloc(sizeof(Queue));
	aQueue -> front = NULL;
	aQueue -> rear = NULL;
	return aQueue;
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
	} else //if (aQueue->front != NULL)
	{
		Event * anEvent = aQueue -> front;
		aQueue -> front = aQueue -> front -> next;
		if (aQueue -> front == aQueue -> rear) {
			aQueue -> rear = NULL;
		} else //if (aQueue->front != aQueue->rear)
		{
			anEvent -> next = NULL;
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

bool isEmpty(Queue * aQueue) {
	if (aQueue -> front == NULL)
		return true;
	return false;
}

void freeQueue(Queue * aQueue) {
	free(aQueue);
}
