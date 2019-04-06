/*
 * main.h
 *
 *  Created on: Mar 26, 2019
 *      Author: kimloai
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <assert.h>

#include "eventQueue.h"
#include "processList.h"
#include "indexProcessing.h"
#include "const.h"

int getEventCode(char *);
char *getData(char **);
Event * addEvent2Queue(char *, Queue*);
int processTheFirstEventFromQueue(Queue **, int);

#endif /* MAIN_H_ */
