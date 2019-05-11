/*
 * main.h
 *
 *  Created on: Mar 26, 2019
 *      Author: kimloai
 */

#ifndef MAIN_H_
#define MAIN_H_

#define DEBUG

#include <assert.h>

#include "eventQueue.h"
#include "processList.h"
#include "indexProcessing.h"
#include "const.h"
#include "debug.h"

int getEventCode(char *);
char *getData(char **);
Event * addEvent2Queue(char *, Queue*);
int processTheFirstEventFromQueue(Queue **, int);
char * readOneLine(char *, FILE *);

#endif /* MAIN_H_ */
