/*
 * main.c
 *
 *  Created on: Mar 26, 2019
 *      Author: kimloai
 */

#include "main.h"

FILE ** pFile;
Queue ** queueArr;

int main(int argc, char ** argv) {
	int size, index, eventCode, i;
	char fileName[25], * buffer, * tmpBuffer, * tmpStr;

	size = atoi(argv[1]);
	index = 0;
	pFile = (FILE ** ) malloc(size * sizeof(FILE * ));

	queueArr = (Queue ** ) malloc(size * sizeof(Queue * ));

	for (i = 0; i < size; ++i) {
		sprintf(fileName, "./traces/trace%d", i);
		pFile[i] = fopen(fileName, "r");
		if (pFile[i] == NULL) {
			perror("Error opening file");
		}

		queueArr[i] = initQueue();
	}

	while (index < size) {
		printf("HERE\n");
		//getchar();
		if (isEmpty(queueArr[index]) == true) {
			printf("CP1\n");
			/*  Get an event from file and add event to the queue  */
			/*  If there is no event in the file (EOF) change to the next file  */

			buffer = (char * ) malloc(BUFFER_SIZE * sizeof(char));
			if (fgets(buffer, BUFFER_SIZE, pFile[index]) != NULL) {
				printf("CP11\n");

				addEvent2Queue(buffer, queueArr[index]);

			} else { //if (fgets(buffer, BUFFER_SIZE, pFile[index]) == NULL)
				++index;
			}
			free(buffer);
		} else { //if (isEmpty(queueArr[index]) == false)
			printf("CP2\n");
			/*Dequeue the event in the queue and process it*/
			int returnCode = processTheFirstEventFromQueue(queueArr, index);
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

Event * addEvent2Queue(char * anEventLine, Queue * aQueue){
	int eventCode = getEventCode(anEventLine);
	Event * _event = initEvent(eventCode);
	// TODO: Post Start are different from Complete Wait -> switch case
	char *pch = strtok(anEventLine, "\t\n");//get event code
	//printf("%s\n", pch);// output Post
	while ((pch = strtok(NULL, "\t\n")) != NULL){
		int aProcessNumber = atoi(pch);
		//printf("%d\n", a); //out put 0 1 2 3
		Process * aProcess = initProcess(aProcessNumber);
		insertProcess2ProcessList(_event -> processList, aProcess);
	}

	enqueue(aQueue, _event);
	return _event;
}

int processTheFirstEventFromQueue(Queue ** aQueue, int aCurrentProcess){
	Event * _event = aQueue[aCurrentProcess] -> front;
	switch (_event->code){
	case POST:{
		/*Case POST: execute the process list, readfile to find WAIT*/
		char * _buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
		_event = dequeue(aQueue[aCurrentProcess]);

		while (!isProcessListEmpty(_event -> processList)){
			Process *_startProcess = getProcessfromProcessList(_event->processList);
			/*Find corresponding START from corresponding Queue*/
			Event * _event = findAnEventFromQueue(queueArr[_startProcess -> num], START);
			if (_event == NULL){
				do{
					assert(fgets(_buffer, BUFFER_SIZE, pFile[_startProcess -> num]));
					_event = addEvent2Queue(_buffer, queueArr[_startProcess -> num]);
				} while(_event -> code != START);
			}
			free(_startProcess);
			assert(_event != NULL);

			/*Remove the corresponding START process from the START's process list*/
			int _returnCode = removeAprocessFromProcessList(_event -> processList, aCurrentProcess);
			assert (_returnCode != -1);
		}

		/*Find WAIT from the queue if not found read file to find WAIT*/
		if (findAnEventFromQueue(queueArr[aCurrentProcess], WAIT) == NULL){
			do{
				assert(fgets(_buffer, BUFFER_SIZE, pFile[aCurrentProcess]));
			} while(addEvent2Queue(_buffer, queueArr[aCurrentProcess]) -> code != WAIT);
		}
		//TODO: Add POST to epoch to detect MCE

		free(_buffer);
		free(_event);
		break;
	}
	case START:{
		/*Wait for post to call all start in the START's process list*/
		char * _buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
		/*If processList of START is empty. It means all POSTs visitted this
		 * start -> dequeue the START and readfile until found complete*/
		if (isProcessListEmpty(_event -> processList)){
			_event = dequeue(aQueue[aCurrentProcess]);
			/*Find and read COMPLETE to queue*/
			if (findAnEventFromQueue(queueArr[aCurrentProcess], COMPLETE) == NULL){
				do{
					assert(fgets(_buffer, BUFFER_SIZE, pFile[aCurrentProcess]));
				} while (addEvent2Queue(_buffer, queueArr[aCurrentProcess]) -> code != COMPLETE);
			}
			// TODO: Process next state
			// TODO: Add START to epoch to detect MCE
			free(_event);
		}
		assert(!isProcessListEmpty(_event -> processList));
		break;
	}
	default:

		break;
	}
	return 0;
}
