/*
 * processProcessList.c
 *
 *  Created on: Mar 26, 2019
 *      Author: kimloai
 */
#include "processList.h"
Process * initProcess(int num) {
	Process *aProcess = (Process * ) malloc(sizeof(Process));
	aProcess -> num = num;
	aProcess -> next = NULL;
	return aProcess;
}

void freeProcess(Process * aProcess) {
	free(aProcess);
}

ProcessList * initProcessList() {
	ProcessList *aProcessList = (ProcessList * ) malloc(sizeof(ProcessList));
	aProcessList -> head = NULL;
	aProcessList -> tail = NULL;
	return aProcessList;
}

void insertProcess2ProcessList(ProcessList * aProcessList, Process * aProcess) {
	if (aProcessList -> head == NULL) {
		aProcessList -> head = aProcess;
	} else { //if (aProcessList->head != NULL)
		aProcessList -> tail -> next = aProcess;
	}
	aProcessList -> tail = aProcess;
}

Process * getProcessfromProcessList(ProcessList * aProcessList) {
	if (aProcessList -> head == NULL) {
		return NULL;
	} else {//if (aProcessList->head != NULL)
		Process * tmpProcess = aProcessList -> head;
		if (aProcessList -> head == aProcessList -> tail) {
			aProcessList -> tail = aProcessList -> head = NULL;
		} else {//if (aProcessList->head != aProcessList->tail)
			aProcessList -> head = aProcessList -> head -> next;
		}
		return tmpProcess;
	}
}

bool findProcessFromProcessList(ProcessList * aProcessList, Process * aProcess){
	Process * _iterProcess = aProcessList -> head;
	while (_iterProcess != NULL){
		if (_iterProcess -> num == aProcess -> num){
			return true;
		}
		_iterProcess = _iterProcess -> next;
	}
	return false;
}

int removeAprocessFromProcessList(ProcessList *aProcessList, int aProcess){
	if (aProcessList -> head == NULL) {
		return -1;
	} else {//if (aProcessList->head != NULL)
		Process * _pProcess = aProcessList -> head;
		Process * _pPreviousProcess = aProcessList -> head;

		//the process is in the start of list
		if (aProcessList -> head -> num == aProcess){
			aProcessList -> head = aProcessList -> head -> next;
			free(_pProcess);
			return 0;
		}

		//the process is not in the start of list
		while (_pProcess != NULL){
			if(_pProcess -> num == aProcess){
				_pPreviousProcess -> next = _pProcess -> next;
				if (_pProcess -> next == NULL) aProcessList -> tail = _pPreviousProcess;
				free(_pProcess);
				return 0;
			}

			_pPreviousProcess = _pProcess;
			_pProcess = _pProcess -> next;
		}
	}
	return -1;
}

void freeProcessList(ProcessList * aProcessList) {
	free(aProcessList);
}

bool isProcessListEmpty(ProcessList *aProcessList){
	if (aProcessList->head == NULL) return true;
	else return false;
}

int getNumberOfItemInProcessList(ProcessList * aProcessList){
	int _result = 0;
	Process * _iterProcess = aProcessList -> head;
	while (_iterProcess != NULL){
		_result ++;
		_iterProcess = _iterProcess -> next;
	}
	return _result;
}

bool isTheItemEqual(ProcessList * aProcessList, ProcessList * anotherProcessList){
	if (getNumberOfItemInProcessList(aProcessList) == getNumberOfItemInProcessList(anotherProcessList)){
		return true;
	} else {
		return false;
	}
}

void printProcessList(ProcessList * aProcessList, char * aMessage){
	Process * _iterProcess = aProcessList -> head;
	printf("%s: ", aMessage);
	while (_iterProcess != NULL){
		printf("%d, ", _iterProcess -> num);
		_iterProcess = _iterProcess -> next;
	}
	printf("\n");
}
