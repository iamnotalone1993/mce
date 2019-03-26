/*
 * processList.c
 *
 *  Created on: Mar 26, 2019
 *      Author: kimloai
 */
#include "processList.h"
Int * initInt(Int * anInt, int num) {
	anInt = (Int * ) malloc(sizeof(Int));
	anInt -> num = num;
	anInt -> next = NULL;
	return anInt;
}

void freeInt(Int * anInt) {
	free(anInt);
}

List * initList(List * aList) {
	aList = (List * ) malloc(sizeof(List));
	aList -> head = NULL;
	aList -> tail = NULL;
}

void insertInt2List(List * aList, Int * anInt) {
	if (aList -> head == NULL) {
		aList -> head = anInt;
	} else //if (aList->head != NULL)
	{
		aList -> tail -> next = anInt;
	}
	aList -> tail = anInt;
}

Int * getIntfromList(List * aList) {
	if (aList -> head == NULL) {
		return NULL;
	} else //if (aList->head != NULL)
	{
		Int * tmpInt = aList -> head;
		aList -> head = aList -> head -> next;
		if (aList -> head == aList -> tail) {
			aList -> tail = NULL;
		} else //if (aList->head != aList->tail)
		{
			tmpInt -> next = NULL;
		}
		return tmpInt;
	}
}

void freeList(List * aList) {
	free(aList);
}
