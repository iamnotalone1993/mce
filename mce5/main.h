/*
 * main.h
 *
 *  Created on: Mar 26, 2019
 *      Author: kimloai
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "eventQueue.h"
#include "processList.h"

int getEventCode(char *);
char *getData(char **);
void addEvent2Queue(char **, Queue**);

#endif /* MAIN_H_ */
