/*
 * main.c
 *
 *  Created on: Mar 26, 2019
 *      Author: kimloai
 */

#include "main.h"

FILE ** pFile;
Queue ** queueArr;
IndexQueue *indexQueue;
int status;
int main(int argc, char ** argv) {
	int size, index, eventCode, i;
	char fileName[25], * buffer, * tmpBuffer, * tmpStr;

	size = atoi(argv[1]);
	index = 0;
	pFile = (FILE ** ) malloc(size * sizeof(FILE * ));
	indexQueue = malloc(sizeof(IndexQueue));
	indexQueue -> lastIndexBeforeGotoQueue = -1;

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
		//getchar();
		index = nextIndex(indexQueue, index, status);
		DEBUG_PRINT("Index: %d, status: 0x%x\n", index, status);
		status = 0; //reset status for the next processing
		if (isEmpty(queueArr[index]) == true) {
			/*  Get an event from file and add event to the queue  */
			/*  If there is no event in the file (EOF) change to the next file  */

			buffer = (char * ) malloc(BUFFER_SIZE * sizeof(char));
			if (fgets(buffer, BUFFER_SIZE, pFile[index]) != NULL) {
				DEBUG_PRINT("Readline file: %d, contain: %s", index, buffer);
				addEvent2Queue(buffer, queueArr[index]);
				status = status || READ_ONE_LINE;

			} else { //if (fgets(buffer, BUFFER_SIZE, pFile[index]) == NULL)
				status = status || END_OF_ONE_FILE;
			}
			free(buffer);
		} else { //if (isEmpty(queueArr[index]) == false)
			/*Dequeue the event in the queue and process it*/
			DEBUG_PRINT("Process the first event, code: %d\n", queueArr[index] -> front -> code);
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
	char * _buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
	switch (_event->code){
	case POST:{
		DEBUG_PRINT("  POST\n");
		/*Case POST: readfile to find WAIT, execute the process list of POST*/
		_event = dequeue(aQueue[aCurrentProcess]);

		/*Find WAIT from the queue if not found read file to find WAIT*/
		if (findAnEventFromQueue(queueArr[aCurrentProcess], WAIT) == NULL){
			do{
				assert(fgets(_buffer, BUFFER_SIZE, pFile[aCurrentProcess]));
				DEBUG_PRINT("    Readline file: %d, contain: %s", aCurrentProcess, _buffer);
			} while(addEvent2Queue(_buffer, queueArr[aCurrentProcess]) -> code != WAIT);
		}

		while (!isProcessListEmpty(_event -> processList)){
			Process *_startProcess = getProcessfromProcessList(_event->processList);

			/*Find corresponding START from corresponding Queue*/
			Event * _startEvent = findAnEventFromQueue(queueArr[_startProcess -> num], START);
			if (_startEvent == NULL){
				do{
					assert(fgets(_buffer, BUFFER_SIZE, pFile[_startProcess -> num]));
					DEBUG_PRINT("    Readline file: %d, contain: %s", _startProcess -> num, _buffer);
					status = status || READ_ONE_LINE_FROM_OTHER_FILE;
					_startEvent = addEvent2Queue(_buffer, queueArr[_startProcess -> num]);

					/*If meet another POST event from other processes add that POST
					 * (which is from other process) to index queue for the next processing */
					if (_startEvent -> code == POST){
						pushToIndexQueue(indexQueue, aCurrentProcess, _startProcess -> num, POST);
					}
				} while(_startEvent -> code != START);
			}

			/*Add the process (which is processed) of POST's processList to WAIT for later processing*/
			//find WAIT
			Event * _waitEvent = findAnEventFromQueue(queueArr[aCurrentProcess], WAIT);
			assert(_waitEvent != NULL);

			//add the POST's process to WAIT's processList
			insertProcess2ProcessList(_waitEvent -> processList, _startProcess);
			assert(_startEvent != NULL);

			/*Remove the corresponding START process from the START's process list
			 * and add that process to COMPLETE's processList*/
			int _returnCode = removeAprocessFromProcessList(_startEvent -> processList, aCurrentProcess);
			assert (_returnCode != -1);

			//find the COMPLETE in the same queue of start
			Event * _completeEvent = findAnEventFromQueue(queueArr[_startProcess -> num], COMPLETE);
			//if found nothing, readfile
			if (_completeEvent == NULL){
				do{
					assert(fgets(_buffer, BUFFER_SIZE, pFile[_startProcess -> num]));
					DEBUG_PRINT("    Readline file: %d, contain: %s", _startProcess -> num, _buffer);
					status = status || READ_ONE_LINE_FROM_OTHER_FILE;
					_completeEvent = addEvent2Queue(_buffer, queueArr[_startProcess -> num]);
				} while(_completeEvent -> code != COMPLETE);
			}
			assert(_completeEvent != NULL);
			//add the process from START to COMPLETE
			Process *_completeTmpProcess = malloc(sizeof(Process));
			_completeTmpProcess -> num = aCurrentProcess;
			insertProcess2ProcessList(_completeEvent -> processList, _completeTmpProcess);
		}

		//TODO: Add POST to epoch to detect MCE

		free(_event);
		break;
	}
	case START:{
		DEBUG_PRINT("  START\n");
		/*Wait for post to call all start in the START's process list*/
		/*If processList of START is empty. It means all POSTs visited this
		 * start -> dequeue the START and readfile until found COMPLETE*/
		if (isProcessListEmpty(_event -> processList)){
			_event = dequeue(aQueue[aCurrentProcess]);
			/*Find and read COMPLETE to queue*/
			if (findAnEventFromQueue(queueArr[aCurrentProcess], COMPLETE) == NULL){
				do{
					assert(fgets(_buffer, BUFFER_SIZE, pFile[aCurrentProcess]));
					DEBUG_PRINT("    Readline file: %d, contain: %s", aCurrentProcess, _buffer);
				} while (addEvent2Queue(_buffer, queueArr[aCurrentProcess]) -> code != COMPLETE);
			}
			// TODO: Maybe need to find POST from other process
			// TODO: Add START to epoch to detect MCE
			free(_event);
		}
		assert(isProcessListEmpty(_event -> processList));
		break;
	}
	case COMPLETE:{
		DEBUG_PRINT("  COMPLETE\n");
		assert(aQueue[aCurrentProcess] -> front -> code == COMPLETE);
		/* Send complete message to corresponding WAIT
		 * if if the WAIT event hasn't had its process list, leave a sign to it
		 * The sign is -processID*/
		// Go to each queue to find WAIT and add a sign to that process
		_event = dequeue(aQueue[aCurrentProcess]);
		while (!isProcessListEmpty(_event -> processList)){
			// Get a process from the processList
			Process * _tmpProcess = getProcessfromProcessList(_event -> processList);
			assert(_tmpProcess != NULL);

			// Find WAIT in corresponding queue (_tmpProcess index) if found nothing readfile
			Event * _waitEvent = findAnEventFromQueue(queueArr[_tmpProcess -> num], WAIT);
			if (_waitEvent == NULL){
				do{
					assert(fgets(_buffer, BUFFER_SIZE, pFile[_tmpProcess -> num]));
					DEBUG_PRINT("    Readline file: %d, contain: %s", _tmpProcess -> num, _buffer);
					_waitEvent = addEvent2Queue(_buffer, queueArr[_tmpProcess -> num]);
				} while(_waitEvent -> code != WAIT);
			}
			assert(_waitEvent -> code == WAIT);

			// Add the COMPLETE's process to the check queue of wait
			Process * _toAddProcess = initProcess(aCurrentProcess);

			insertProcess2ProcessList(_waitEvent -> checkProcessList, _toAddProcess);
			free(_tmpProcess);
		}

		// TODO: Add complete to queue to detect MCE
		free(_event);
		break;
	}
	case WAIT:{
		DEBUG_PRINT("  WAIT\n");
		assert(false);
		/* If all COMPLETEs have sent the message to this WAIT -> dequeue the event
		 * and at the event to queue to detect MCE
		 * else keep WAIT in queueArr and add direction to IndexQueue*/
		if (isTheNumberOfItemInTwoProcessListEqual(_event -> processList, _event -> checkProcessList)){
			_event = dequeue(queueArr[aCurrentProcess]);
			free(_event);
			//TODO: add WAIT to queue to detect MCE
		} else {

		}

		break;
	}
	default:

		break;
	}
	free(_buffer);
	return 0;
}
