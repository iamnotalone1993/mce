/*
 * main.c
 *
 *  Created on: Mar 26, 2019
 *      Author: kimloai
 */

#include "main.h"

int main(int argc, char ** argv) {
	int size, index, eventCode, i;
	char fileName[25], * buffer, * tmpBuffer, * tmpStr;
	FILE ** pFile;
	Event * currentEvent;
	Queue ** queueArr;

	size = atoi(argv[1]);
	index = 0;
	pFile = (FILE ** ) malloc(size * sizeof(FILE * ));
	currentEvent = NULL;
	queueArr = (Queue ** ) malloc(size * sizeof(Queue * ));

	for (i = 0; i < size; ++i) {
		sprintf(fileName, "./traces/trace%d", i);
		pFile[i] = fopen(fileName, "r");
		if (pFile[i] == NULL) {
			perror("Error opening file");
		}

		queueArr[i] = initQueue(queueArr[i]);
	}

	while (index < size) {
		printf("HERE\n");
		//getchar();
		if (currentEvent == NULL) {
			if (isEmpty(queueArr[index]) == true) {
				printf("CP1\n");
				/*  Get an event from file and add event to the queue  */
				/*  If there is no event in the file (EOF) change to the next file  */

				buffer = (char * ) malloc(BUFFER_SIZE * sizeof(char));
				if (fgets(buffer, BUFFER_SIZE, pFile[index]) != NULL) {
					printf("CP11\n");

					addEvent2Queue(&buffer, &queueArr[index]);

				} else //if (fgets(buffer, BUFFER_SIZE, pFile[index]) == NULL)
				{
					++index;
				}
				free(buffer);
			} else //if (isEmpty(queueArr[index]) == false)
			{
				printf("CP2\n");
				Event * anEvent = dequeue(queueArr[index]);
				if (anEvent -> code == POST) {
					Process * aProcess = getProcessfromProcessList(anEvent -> processList);
					if (aProcess == NULL) { /* do nothing */ } else //if (aProcess != NULL)
					{
						if (anEvent -> processList -> head == NULL) {
							freeEvent(anEvent);
						} else //if (anEvent->processList->head != NULL)
						{
							push(queueArr[index], anEvent);
						}

						currentEvent = initEvent(POST);
						insertProcess2ProcessList(currentEvent -> processList, aProcess);
					}
				} else if (anEvent -> code == START) {
					//TODO
				} else if (anEvent -> code == COMPLETE) {
					//TODO
				} else if (anEvent -> code == WAIT) {
					//TODO
				} else { /* TODO */ }
			}
		} else //if (currentEvent != NULL)
		{
			//TODO
		}
	}

	for (i = 0; i < size; ++i) {
		fclose(pFile[i]);
		freeQueue(queueArr[i]);
	}
	free(pFile);
	free(queueArr);

	return 0;
}

int getEventCode(char * str) {
	int i = 0;
	switch (str[i]) {
	case 'L':
		i = i + 2;
		if (str[i] == 'a')
			return LOAD;
		else //if (str[i] == 'c')
			return LOCK;
	case 'S':
		i++;
		if (str[i] == 'e')
			return SEND;
		else //if (str[i] == 't')
		{
			i++;
			if (str[i] == 'o')
				return STORE;
			else //if (str[i] == 'a')
				return START;
		}
	case 'G':
		return GET;
	case 'P':
		i++;
		if (str[i] == 'u')
			return PUT;
		else //if (str[i] == 'o')
			return POST;
	case 'A':
		return ACCUMULATE;
	case 'B':
		return BARRIER;
	case 'C':
		i++;
		if (str[i] == 'o')
			return COMPLETE;
		else //if (str[i] == 'r');
			return CREATE;
	case 'F':
		return FENCE;
	case 'R':
		return RECV;
	case 'U':
		return UNLOCK;
	case 'W':
		return WAIT;
	default:
		return DEFAULT;
	}
}

void addEvent2Queue(char **anEventLine, Queue **aQueue){
	int eventCode = getEventCode(*anEventLine);
	Event * anEvent = initEvent(eventCode);
	// TODO: Post Start are different from Complete Wait -> switch case
	char *pch = strtok(*anEventLine, "\t\n");//get event code
	//printf("%s\n", pch);// output Post
	while (pch = strtok(NULL, "\t\n")){
		int aProcessNumber = atoi(pch);
		//printf("%d\n", a); //out put 0 1 2 3
		Process * aProcess = initProcess(aProcessNumber);
		insertProcess2ProcessList(anEvent -> processList, aProcess);
	}

	enqueue(*aQueue, anEvent);
	return;
}
