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

	size = atoi(argv[1]);
	index = 0;
	pFile = (FILE ** ) malloc(size * sizeof(FILE * ));
	indexQueue = malloc(sizeof(IndexQueue));
	indexQueue -> lastIndexBeforeGotoQueue = -1;

	primeArr = (mpz_t **) malloc(size * sizeof(mpz_t *));

	queueArr = (Queue ** ) malloc(size * sizeof(Queue * ));

	detectQueue = (DetectQueue **) malloc(size * sizeof(DetectEvent));

	for (i = 0; i < size; ++i) {
		sprintf(fileName, "./traces/trace%d", i);
		pFile[i] = fopen(fileName, "r");
		if (pFile[i] == NULL) {
			perror("Error opening file");
		}

		// Init global arrays
		queueArr[i] = initQueue();
		detectQueue[i] = initDetectQueue();

		primeArr[i] = (mpz_t *) malloc(sizeof(mpz_t));
		mpz_init(*primeArr[i]);

		if (i >= 1) mpz_nextprime(*(primeArr[i]), *(primeArr[i - 1]));
		else mpz_set_ui(*(primeArr[i]), 2);
	}

	// Init the initial event for the detectQueue
	for (i = 0; i < size; i++){
		mpz_t * _tmpMpz = malloc(sizeof(mpz_t));
		mpz_init(*(_tmpMpz));
		if (i > 0) mpz_nextprime(* (_tmpMpz), *(detectQueue[i - 1] -> rear -> clock));
		else mpz_set_ui(*(_tmpMpz), 2);
 		DetectEvent * _tmpEvent = malloc(sizeof(DetectEvent));
 		_tmpEvent -> clock = _tmpMpz;
 		_tmpEvent -> code = DEFAULT;
 		_tmpEvent -> next = NULL;
 		pushToDetectQueue(detectQueue[i], _tmpEvent);
 		//PRINT_CLOCK("detectQueue initial clock: %s\n", *(detectQueue[i] -> rear -> clock));
	}


	while ((status & END_OF_ALL_FILE) != END_OF_ALL_FILE) {
		//getchar();
		index = nextIndex(indexQueue, index, status);
		DEBUG_PRINT("Index: %d, status: 0x%x, ItemInIndexQueue: %d lastIndex: %d\n"\
				, index, status, \
				getNumOfItemInIndexQueue(indexQueue), indexQueue -> lastIndexBeforeGotoQueue);

		// If all files have been read detect MCE for the last epoch and end the program
		if (index >= size) {
			status = status | END_OF_ALL_FILE;
			continue;
		}
		DEBUG_PRINT("ItemIn queueArr[%d]: %d\n", index, getNumberOfItemInEventQueue(queueArr[index]));

		status = 0; //reset status for the next processing


		if (isEmpty(queueArr[index]) == true) {
			/*  Get an event from file and add event to the queue  */
			/*  If there is no event in the file (EOF) change to the next file  */

			buffer = (char * ) malloc(BUFFER_SIZE * sizeof(char));
			if (readOneLine(buffer, pFile[index]) != NULL) {
				DEBUG_PRINT("Readline file: %d, contain: %s", index, buffer);
				addEvent2Queue(buffer, queueArr[index]);
			} else { //if (fgets(buffer, BUFFER_SIZE, pFile[index]) == NULL)
				status = status | END_OF_ONE_FILE;
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

		free(queueArr[i]);
		free(detectQueue[i]);
	}
	free(pFile);
	free(queueArr);
	free(indexQueue);
	free(detectQueue);
	return 0;
}

char * readOneLine(char * aBuffer, FILE * aFileHandle){
	status = status | READ_ONE_LINE;
	if (!fgets(aBuffer, BUFFER_SIZE, aFileHandle)){
		status = status | END_OF_ONE_FILE;
		return NULL;
	} else {
		return aBuffer;
	}
	return fgets(aBuffer, BUFFER_SIZE, aFileHandle);
}

Event * addEvent2Queue(char * anEventLine, Queue * aQueue){
	int eventCode = getEventCode(anEventLine);
	Event * _event = initEvent(eventCode);
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
		status = status | POST_IS_PROCESSED;
		/*Case POST: readfile to find WAIT, execute the process list of POST and send clock to START*/
		_event = dequeue(aQueue[aCurrentProcess]);

		/*Find WAIT from the queue if not found read file to find WAIT*/
		if (findAnEventFromQueue(queueArr[aCurrentProcess], WAIT) == NULL){
			do{
				assert(readOneLine(_buffer, pFile[aCurrentProcess]));
				DEBUG_PRINT("    Readline file: %d, contain: %s", aCurrentProcess, _buffer);
			} while(addEvent2Queue(_buffer, queueArr[aCurrentProcess]) -> code != WAIT);
		}

		while (!isProcessListEmpty(_event -> processList)){
			Process *_startProcess = getProcessfromProcessList(_event->processList);

			/*Find corresponding START from corresponding Queue*/
			Event * _startEvent = findAnEventFromQueue(queueArr[_startProcess -> num], START);
			if (_startEvent == NULL){
				do{
					assert(readOneLine(_buffer, pFile[_startProcess -> num]));
					DEBUG_PRINT("    Readline file: %d, contain: %s", _startProcess -> num, _buffer);
					status = status | READ_ONE_LINE_FROM_OTHER_FILE;
					_startEvent = addEvent2Queue(_buffer, queueArr[_startProcess -> num]);

					/*If meet another POST event from other processes add that POST
					 * (which is from other process) to index queue for the next processing */
					if (_startEvent -> code == POST){
						pushToIndexQueue(indexQueue, aCurrentProcess, _startProcess -> num, POST, POST_IS_PROCESSED);
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
			 * send  the clock to this START process
			 * and add that process to COMPLETE's processList*/
			int _returnCode = removeAprocessFromProcessList(_startEvent -> processList, aCurrentProcess);
			assert (_returnCode != -1);
			// Save the clock that the POST sent
			//	The procedure include: read current clock and send that clock to the START process
			//  in this contest "send" mean make a LCM (START'clock, POST's clock)
			mpz_t * _currentClock = getCurrentClock(detectQueue[aCurrentProcess], aCurrentProcess);
			assert(_currentClock != NULL);
			mpz_lcm(*(_startEvent -> savedClock), \
					*(_startEvent -> savedClock), *_currentClock);

			PRINT_CLOCK("      START's initial clock [%d]: %s\n", _startProcess -> num, *(_startEvent ->savedClock));

			//find the COMPLETE in the same queue of start
			Event * _completeEvent = findAnEventFromQueue(queueArr[_startProcess -> num], COMPLETE);
			//if found nothing, readfile
			if (_completeEvent == NULL){
				do{
					assert(readOneLine(_buffer, pFile[_startProcess -> num]));
					DEBUG_PRINT("    Readline file: %d, contain: %s", _startProcess -> num, _buffer);
					status = status | READ_ONE_LINE_FROM_OTHER_FILE;
					_completeEvent = addEvent2Queue(_buffer, queueArr[_startProcess -> num]);
				} while(_completeEvent -> code != COMPLETE);
			}
			assert(_completeEvent != NULL);
			//add the process from START to COMPLETE
			Process *_completeTmpProcess = malloc(sizeof(Process));
			_completeTmpProcess -> num = aCurrentProcess;
			insertProcess2ProcessList(_completeEvent -> processList, _completeTmpProcess);
		}

		// Add POST to epoch to detect MCE
		assert(mpz_cmp_ui(*(_event -> savedClock), 1) == 0);
		DetectEvent * _tmpDetectEvent = initDetectEvent(_event -> code, aCurrentProcess, _event -> savedClock);
		pushToDetectQueue(detectQueue[aCurrentProcess], _tmpDetectEvent);
		PRINT_CLOCK("******POST final clock [%d]: %s\n", aCurrentProcess, *(_event -> savedClock));

		assert(isProcessListEmpty(_event -> processList));
		freeEvent(_event);
		break;
	}
	case START:{
		DEBUG_PRINT("  START\n");
		/*Wait for post to call all start in the START's process list*/
		/*If processList of START is empty. It means all POSTs visited this
		 * start -> dequeue the START and read file until found COMPLETE*/
		if (isProcessListEmpty(_event -> processList)){
			_event = dequeue(aQueue[aCurrentProcess]);
			/*Find and read COMPLETE to queue*/
			if (findAnEventFromQueue(queueArr[aCurrentProcess], COMPLETE) == NULL){
				do{
					assert(readOneLine(_buffer, pFile[aCurrentProcess]));
					DEBUG_PRINT("    Readline file: %d, contain: %s", aCurrentProcess, _buffer);
				} while (addEvent2Queue(_buffer, queueArr[aCurrentProcess]) -> code != COMPLETE);
			}
			// TODO: Maybe need to find POST from other process
			//Add START to epoch to detect MCE
			DetectEvent * _tmpDetectEvent = initDetectEvent(_event -> code, aCurrentProcess, _event -> savedClock);
			pushToDetectQueue(detectQueue[aCurrentProcess], _tmpDetectEvent);
			PRINT_CLOCK("******START final clock [%d]: %s\n", aCurrentProcess, *(_event -> savedClock));

			assert(isProcessListEmpty(_event -> processList));
			freeEvent(_event);
		}

		break;
	}
	case COMPLETE:{
		DEBUG_PRINT("  COMPLETE\n");
		status = status | COMPLETE_IS_PROCESSED;
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
					assert(readOneLine(_buffer, pFile[_tmpProcess -> num]));
					DEBUG_PRINT("    Readline file: %d, contain: %s", _tmpProcess -> num, _buffer);
					_waitEvent = addEvent2Queue(_buffer, queueArr[_tmpProcess -> num]);
				} while(_waitEvent -> code != WAIT);
			}
			assert(_waitEvent -> code == WAIT);

			// Add the COMPLETE's process to the check queue of wait
			Process * _toAddProcess = initProcess(aCurrentProcess);

			// TODO: Send the COMPLETE's clock to wait

			insertProcess2ProcessList(_waitEvent -> checkProcessList, _toAddProcess);
			free(_tmpProcess);
		}

		// TODO: Add complete to queue to detect MCE
		DetectEvent * _tmpDetectEvent = initDetectEvent(_event -> code, aCurrentProcess, _event -> savedClock);
		pushToDetectQueue(detectQueue[aCurrentProcess], _tmpDetectEvent);
		PRINT_CLOCK("******COMPLETE final clock [%d]: %s\n", aCurrentProcess, *(_event -> savedClock));

		freeEvent(_event);
		break;
	}
	case WAIT:{
		DEBUG_PRINT("  WAIT\n");
		/* If all COMPLETEs have sent the message to this WAIT -> dequeue the event
		 * and at the event to queue to detect MCE
		 * else keep WAIT in queueArr and add direction to IndexQueue*/
		if (isTheItemEqual(_event -> processList, _event -> checkProcessList)){
			_event = dequeue(queueArr[aCurrentProcess]);
			freeEvent(_event);


			//TODO: add WAIT to queue to detect MCE
			DetectEvent * _tmpDetectEvent = initDetectEvent(_event -> code, aCurrentProcess, _event -> savedClock);
			pushToDetectQueue(detectQueue[aCurrentProcess], _tmpDetectEvent);
			PRINT_CLOCK("******WAIT final clock [%d]: %s\n", aCurrentProcess, *(_event -> savedClock));
		} else {
			/* Program reach this condition means that COMPLETEs from other processes
			 * haven't completed
			 * -> Add direction to IndexQueue to read, until all process form WAIT's
			 * processList complete (have been checked)*/
			Process * _iterProcess = _event -> processList -> head;
			while (_iterProcess != NULL){
				// if this _iterProcess does not present in checkProcessList it means the COMPLETE
				// does not visit this WAIT
				// -> add a direction to IndexQueue to read the COMPLETE
				if (!findProcessFromProcessList(_event -> checkProcessList, _iterProcess)){
					pushToIndexQueue(indexQueue, aCurrentProcess, _iterProcess -> num, WAIT, COMPLETE_IS_PROCESSED);
				}
				_iterProcess = _iterProcess -> next;
			}
		}

		break;
	}
	default:

		break;
	}
	free(_buffer);
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

