/*
 * processProcessList.c
 *
 *  Created on: Mar 26, 2019
 *      Author: kimloai
 */
#include "processList.h"
Process * initProcess(Process * aProcess, int num) {
	aProcess = (Process * ) malloc(sizeof(Process));
	aProcess -> num = num;
	aProcess -> next = NULL;
	return aProcess;
}

void freeProcess(Process * aProcess) {
	free(aProcess);
}

ProcessList * initProcessList(ProcessList * aProcessList) {
	aProcessList = (ProcessList * ) malloc(sizeof(ProcessList));
	aProcessList -> head = NULL;
	aProcessList -> tail = NULL;
}

void insertProcess2ProcessList(ProcessList * aProcessList, Process * aProcess) {
	if (aProcessList -> head == NULL) {
		aProcessList -> head = aProcess;
	} else //if (aProcessList->head != NULL)
	{
		aProcessList -> tail -> next = aProcess;
	}
	aProcessList -> tail = aProcess;
}

Process * getProcessfromProcessList(ProcessList * aProcessList) {
	if (aProcessList -> head == NULL) {
		return NULL;
	} else //if (aProcessList->head != NULL)
	{
		Process * tmpProcess = aProcessList -> head;
		aProcessList -> head = aProcessList -> head -> next;
		if (aProcessList -> head == aProcessList -> tail) {
			aProcessList -> tail = NULL;
		} else //if (aProcessList->head != aProcessList->tail)
		{
			tmpProcess -> next = NULL;
		}
		return tmpProcess;
	}
}

void freeProcessList(ProcessList * aProcessList) {
	free(aProcessList);
}
