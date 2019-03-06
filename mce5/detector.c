#include "detector.h"

Int *initInt(Int *anInt, int num)
{
	anInt = (Int *) malloc(sizeof(Int));
	anInt->num = num;
	anInt->next = NULL;
	return anInt;
}

void freeInt(Int *anInt)
{
	free(anInt);
}

List *initList(List *aList)
{
	aList = (List *) malloc(sizeof(List));
	aList->head = NULL;
	aList->tail = NULL;
}

void insertInt2List(List *aList, Int *anInt)
{
	if (aList->head == NULL)
	{
		aList->head = anInt;
	}
	else //if (aList->head != NULL)
	{
		aList->tail->next = anInt;
	}
	aList->tail = anInt;
}

Int *getIntfromList(List *aList)
{
	if (aList->head == NULL)
        {
                return NULL;
        }
        else //if (aList->head != NULL)
        {
                Int *tmpInt = aList->head;
                aList->head = aList->head->next;
                if (aList->head == aList->tail)
                {
                        aList->tail = NULL;
                }
                else //if (aList->head != aList->tail)
                {
                        tmpInt->next = NULL;
                }
                return tmpInt;
        }
}

void freeList(List *aList)
{
	free(aList);
}

Event *initEvent(Event *anEvent, int eventCode)
{
	anEvent = (Event *) malloc(sizeof(Event));
	anEvent->code = eventCode;
	anEvent->intList = initList(anEvent->intList);
	anEvent->next = NULL;
	return anEvent;
}

void freeEvent(Event *anEvent)
{
	free(anEvent->intList);
	free(anEvent);
}

Queue *initQueue(Queue *aQueue)
{
	aQueue = (Queue *) malloc(sizeof(Queue));
	aQueue->front = NULL;
	aQueue->rear = NULL;
	return aQueue;
}

void enqueue(Queue *aQueue, Event *anEvent)
{
	if (aQueue->front == NULL)
	{
		aQueue->front = anEvent;
	}
	else //if (aQueue->front != NULL)
	{
		aQueue->rear->next = anEvent;
	}
	aQueue->rear = anEvent;
}

Event *dequeue(Queue *aQueue)
{
	if (aQueue->front == NULL)
	{
		return NULL;
	}
	else //if (aQueue->front != NULL)
	{
		Event *anEvent = aQueue->front;
		aQueue->front = aQueue->front->next;
		if (aQueue->front == aQueue->rear)
		{
			aQueue->rear = NULL;
		}
		else //if (aQueue->front != aQueue->rear)
		{
			anEvent->next = NULL;
		}
		return anEvent;
	}
}

void push(Queue *aQueue, Event *anEvent)
{
	if (aQueue->front == NULL)
	{
		aQueue->front = anEvent;
		aQueue->rear = anEvent;
	}
	else //if (aQueue->front != NULL)
	{
		anEvent->next = aQueue->front;
		aQueue->front = anEvent;
	}
}

Event *retrieve(Queue *aQueue, int eventCode)
{
	Event *anEvent = aQueue->front;
	while (anEvent != NULL)
	{
		if (anEvent->code == eventCode)
		{
			//TODO
			break;
		}
		anEvent = anEvent->next;
	}
	return anEvent;
}

bool isEmpty(Queue *aQueue)
{
	if (aQueue->front == NULL)
		return true;
	return false;
}

void freeQueue(Queue *aQueue)
{
	free(aQueue);
}

int getEventCode(char *str)
{
	int i = 0;
	switch (str[i])
	{
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

char *getData(char **buffer)
{
        char *tmpBuffer, *tmpStr = NULL;
        int tmpInt;

        tmpBuffer = *buffer;
        *buffer = strchr(*buffer, '\t');
        if (*buffer != NULL)
        {
                (*buffer)++;
        }
        tmpInt = strcspn(tmpBuffer, "\t\n");
        tmpStr = (char *) malloc((tmpInt + 1) * sizeof(char));
        memcpy(tmpStr, tmpBuffer, tmpInt);
        tmpStr[tmpInt] = '\0';

        return tmpStr;
}

int main(int argc, char **argv)
{
	int size, index, eventCode, i;
	char fileName[25], *buffer, *tmpBuffer, *tmpStr;
	FILE **pFile;
	Event *currentEvent;
	Queue **queueArr;

	size = atoi(argv[1]);
	index = 0;
	pFile = (FILE **) malloc(size * sizeof(FILE *));
	currentEvent = NULL;
	queueArr = (Queue **) malloc(size * sizeof(Queue *));

	for (i = 0; i < size; ++i)
	{
                sprintf(fileName, "./traces/trace%d", i);
                pFile[i] = fopen(fileName, "r");
                if (pFile[i] == NULL)
                {
                        perror("Error opening file");
                }

		queueArr[i] = initQueue(queueArr[i]);
	}

	while (index < size)
	{
		printf("HERE\n");
		getchar();
		if (currentEvent == NULL)
		{
			if (isEmpty(queueArr[index]) == true)
			{
				printf("CP1\n");
				buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
		        	if (fgets(buffer, BUFFER_SIZE, pFile[index]) != NULL)
		        	{
					printf("CP11\n");
		        		eventCode = getEventCode(buffer);
		                	if (eventCode == POST)
		                	{
						printf("CP111\n");
						Event *anEvent = initEvent(anEvent, eventCode);

						tmpBuffer = buffer;
		                        	tmpStr = getData(&tmpBuffer);
		                       		free(tmpStr);

						while (tmpBuffer != NULL)
						{
							printf("CP112\n");
							tmpStr = getData(&tmpBuffer);
		                                	Int *anInt = initInt(anInt, atoi(tmpStr));
							free(tmpStr);
		                                	insertInt2List(anEvent->intList, anInt);
						}

						enqueue(queueArr[index], anEvent);
					}
					else if (eventCode == START)
					{
						//TODO
					}
					else if (eventCode == COMPLETE)
					{
						//TODO
					}
					else if (eventCode == WAIT)
					{
						//TODO
					}
					else { /* TODO */ }
				}
				else //if (fgets(buffer, BUFFER_SIZE, pFile[index]) == NULL)
				{
					++index;
				}
				free(buffer);
			}
		        else //if (isEmpty(queueArr[index]) == false)
			{
				printf("CP2\n");
				Event *anEvent = dequeue(queueArr[index]);
				if (anEvent->code == POST)
				{
					Int *anInt = getIntfromList(anEvent->intList);
					if (anInt == NULL) { /* do nothing */ }
					else //if (anInt != NULL)
					{
						if (anEvent->intList->head == NULL)
						{
							freeEvent(anEvent);
						}	
						else //if (anEvent->intList->head != NULL)
						{
							push(queueArr[index], anEvent);
						}

						currentEvent = initEvent(currentEvent, POST);
						insertInt2List(currentEvent->intList, anInt);
					}
				}
				else if (anEvent->code == START)
				{
					//TODO
				}
				else if (anEvent->code == COMPLETE)
				{
					//TODO
				}
				else if (anEvent->code == WAIT)
				{
					//TODO
				}
				else { /* TODO */ }
			}
		}
		else //if (currentEvent != NULL)
		{
			//TODO
		}
	}

	for (i = 0; i < size; ++i)
        {
                fclose(pFile[i]);
		freeQueue(queueArr[i]);
        }
        free(pFile);
	free(queueArr);

	return 0;
}
