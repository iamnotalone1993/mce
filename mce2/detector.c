#include "detector.h"

int getEventCode(char *str)
{
	if (strstr(str, "Fence") != NULL)
		return FENCE;
	if (strstr(str, "Barrier") != NULL)
		return BARRIER;
	if (strstr(str, "Put") != NULL)
		return PUT;
	if (strstr(str, "Get") != NULL)
		return GET;
	if (strstr(str, "Accumulate") != NULL)
		return ACCUMULATE;
	if (strstr(str, "Post") != NULL)
		return POST;
	if (strstr(str, "Start") != NULL)
		return START;
	if (strstr(str, "Complete") != NULL)
		return COMPLETE;
	if (strstr(str, "Wait") != NULL)
		return WAIT;
	if (strstr(str, "Lock") != NULL)
		return LOCK;
	if (strstr(str, "Unlock") != NULL)
		return UNLOCK;
	if (strstr(str, "Send") != NULL)
		return SEND;
	if (strstr(str, "Recv") != NULL)
		return RECV;
	if (strstr(str, "Load") != NULL)
		return LOAD;
	if (strstr(str, "Store") != NULL)
		return STORE;
	if (strstr(str, "Create") != NULL)
		return CREATE;
}

char *convertCode2Name(int code)
{
	char *tmp;
	switch (code)
	{
		case FENCE:
			tmp = malloc(6 * sizeof(char));
			strcpy(tmp, "FENCE\0");
			return tmp;
		case BARRIER:
                        tmp = malloc(8 * sizeof(char));
                        strcpy(tmp, "BARRIER\0");
                        return tmp;
		case GET:
                        tmp = malloc(4 * sizeof(char));
                        strcpy(tmp, "GET\0");
                        return tmp;
		case PUT:
                        tmp = malloc(4 * sizeof(char));
                        strcpy(tmp, "PUT\0");
                        return tmp;
		case ACCUMULATE:
                        tmp = malloc(11 * sizeof(char));
                        strcpy(tmp, "ACCUMULATE\0");
                        return tmp;
        	case POST:
                        tmp = malloc(5 * sizeof(char));
                        strcpy(tmp, "POST\0");
                        return tmp;
        	case START:
                        tmp = malloc(6 * sizeof(char));
                        strcpy(tmp, "START\0");
                        return tmp;
		case COMPLETE:
                        tmp = malloc(9 * sizeof(char));
                        strcpy(tmp, "COMPLETE\0");
                        return tmp;
		case WAIT:
                        tmp = malloc(5 * sizeof(char));
                        strcpy(tmp, "WAIT\0");
                        return tmp;
        	case LOCK:
                        tmp = malloc(5 * sizeof(char));
                        strcpy(tmp, "LOCK\0");
                        return tmp;
        	case UNLOCK:
                        tmp = malloc(7 * sizeof(char));
                        strcpy(tmp, "UNLOCK\0");
                        return tmp;
		case SEND:
                        tmp = malloc(5 * sizeof(char));
                        strcpy(tmp, "SEND\0");
                        return tmp;
		case RECV:
                        tmp = malloc(5 * sizeof(char));
                        strcpy(tmp, "RECV\0");
                        return tmp;
        	case LOAD:
                        tmp = malloc(5 * sizeof(char));
                        strcpy(tmp, "LOAD\0");
                        return tmp;
        	case STORE:
                        tmp = malloc(6 * sizeof(char));
                        strcpy(tmp, "STORE\0");
                        return tmp;
        	case CREATE:
                        tmp = malloc(7 * sizeof(char));
                        strcpy(tmp, "CREATE\0");
                        return tmp;
        }
}

int *getClock(char *buffer, int size)
{
	char *clockStr = (char *) malloc((size * 2 + 2) * sizeof(char));
      	memcpy(clockStr, strstr(buffer, "["), size * 2 + 1);
        clockStr[size * 2 + 1] = '\0';
	int *clock = (int *) malloc(size * sizeof(int));
	int i;
	for (i = 0; i < size; i++)
	{
		clock[i] = clockStr[i * 2 + 1] - '0';
	}
	return clock;
}

bool equalClock(int *clock1, int *clock2, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		if (clock1[i] != clock2[i])
		{
			return false;
		}
	}
	return true;
}

void printClock(int *clock, int size)
{
	int i;
	printf("clock=[");
	for (i = 0; i < size - 1; i++)
	{
		printf("%d|", clock[i]);
	}
	printf("%d]\n", clock[i]);
}

char *getData(char **buffer)
{
	char *tmpBuffer, *tmpStr;
	int tmpInt;
	
	tmpBuffer = *buffer;
	*buffer = strchr(*buffer, '\t') + 1;
        tmpInt = strcspn(tmpBuffer, "\t\n");
        tmpStr = (char *) malloc((tmpInt + 1) * sizeof(char));
        memcpy(tmpStr, tmpBuffer, tmpInt);
	tmpStr[tmpInt] = '\0';

	return tmpStr;
}

int isConcurrent(int *clock1, int rank1, int *clock2, int rank2)
{
	if (clock1[rank2] >= clock2[rank2])
		return 1;
	if (clock2[rank1] >= clock1[rank1])
		return -1;
	return 0;
}

Comm *initComm(int code, int target_rank, char *target_addr)
{
	Comm *aComm = (Comm *) malloc(sizeof(Comm));
	aComm->code = code;
	aComm->target_rank = target_rank;
	aComm->target_addr = target_addr;
	aComm->next = NULL;
	return aComm;
}

void freeComm(Comm *aComm)
{
	free(aComm->target_addr);
	free(aComm);
}

void insertCommNode(Node *aNode, Comm* aComm)
{
	if (aNode->commHead == NULL && aNode->commTail == NULL)
	{
		aNode->commTail = aComm;
		aNode->commHead = aNode->commTail;
	}
	else
	{
		aNode->commTail->next = aComm;
		aNode->commTail = aNode->commTail->next;
	}
}

void printComm(Comm *aComm)
{
	printf("code=%s target_rank=%d target_addr=%s\n", 
			convertCode2Name(aComm->code), aComm->target_rank, aComm->target_addr);
}

Loca *initLoca(int code, char *varAddr)
{
	Loca *aLoca = (Loca *) malloc(sizeof(Loca));
	aLoca->code = code;
	aLoca->varAddr = varAddr;
	aLoca->next = NULL;
	return aLoca;
}

void freeLoca(Loca *aLoca)
{
	free(aLoca->varAddr);
	free(aLoca);
}

void insertLocaNode(Node *aNode, Loca *aLoca)
{
	if (aNode->locaHead == NULL && aNode->locaTail == NULL)
        {
                aNode->locaTail = aLoca;
                aNode->locaHead = aNode->locaTail;
        }
        else
        {
                aNode->locaTail->next = aLoca;
                aNode->locaTail = aNode->locaTail->next;
        }
}

void printLoca(Loca *aLoca)
{
	printf("code=%s varAddr=%s\n", convertCode2Name(aLoca->code), aLoca->varAddr);
}

Node *initNode(int *clock)
{
	Node *aNode = (Node *) malloc(sizeof(Node));
	aNode->clock = clock;
	aNode->commHead = NULL;
	aNode->commTail = NULL;
	aNode->locaHead = NULL;
	aNode->locaTail = NULL;
	aNode->next = NULL;
	return aNode;
}

void freeNode(Node *aNode)
{
        free(aNode->clock);

        while (aNode->commHead != NULL)
        {
                aNode->commTail = aNode->commHead;
                aNode->commHead = aNode->commHead->next;
                freeComm(aNode->commTail);
        }
        aNode->commTail = NULL;

        while (aNode->locaHead != NULL)
        {
                aNode->locaTail = aNode->locaHead;
                aNode->locaHead = aNode->locaHead->next;
                freeLoca(aNode->locaTail);
        }
        free(aNode);
}

void printNode(Node *aNode, int size)
{
	printf("Printing a Node...\n");
	int i;
	printClock(aNode->clock, size);
	
	Comm *commTmp = aNode->commHead;
	while (commTmp != NULL)
	{
		printComm(commTmp);
		commTmp = commTmp->next;
	}

	Loca *locaTmp = aNode->locaHead;
	while (locaTmp != NULL)
	{
		printLoca(locaTmp);
		locaTmp = locaTmp->next;
	}
}

List* initList(char *base, int size, int disp_unit)
{
	List *aList = (List *) malloc(size * sizeof(List));
	aList->base = base;
	aList->size = size;
	aList->disp_unit = disp_unit;
	aList->head = NULL;
	aList->tail = NULL;
	return aList;
}

void insertList(List *aList, Node *aNode)
{
        if (aList->head == NULL && aList->tail == NULL)
        {
                aList->tail = aNode;
                aList->head = aList->tail;
        }
        else
        {
                aList->tail->next = aNode; 
                aList->tail = aList->tail->next;
        }
}

void freeList(List *aList)
{
	while (aList->head != NULL)
	{
		aList->tail = aList->head;
		aList->head = aList->head->next;
		free(aList->tail);
	}
	aList->tail = NULL;
}

void freeAllList(List **aList, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		freeList(aList[i]);
	}
}

void printList(List *aList, int size)
{
	printf("\nPrinting a List...\n");
	printf("base=%s size=%d disp_unit=%d\n", aList->base, aList->size, aList->disp_unit);

	Node *tmp = aList->head;
	while (tmp != NULL)
	{
		printNode(tmp, size);
		tmp = tmp->next;
	}
}

void printAllList(List **aList, int size)
{
	int i;
	printf("\nPrinting all Lists...\n");
	for (i = 0; i < size; i++)
	{
		printList(aList[i], size);
	}
}

Chai *initChain(int rank)
{
	Chai *aChain = (Chai *) malloc(sizeof(Chai));
	aChain->rank = rank;
	aChain->head = NULL;
	aChain->tail = NULL;
	return aChain;
}

void insertChain(Chai *aChain, Loca *aLoca)
{
	if (aChain->head == NULL && aChain->tail == NULL)
	{
		aChain->tail = aLoca;
		aChain->head = aChain->tail;
	}
	else
	{
		aChain->tail->next = aLoca;
		aChain->tail = aChain->tail->next;
	}
}

void printChain(Chai *aChain)
{
	printf("\nPrinting a Chain...\n");
	printf("rank=%d\n", aChain->rank);
	Loca *tmp = aChain->head;
	while (tmp != NULL)
	{
		printf("code=%s varAddr=%s\n", convertCode2Name(tmp->code), tmp->varAddr);
		tmp = tmp->next;
	}
}

void readEventWithinEpoch(FILE **pFile, int index, List **aList, Chai *aChain, int endEvent)
{
	while (true)
        {
        	char *buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
        	if (fgets(buffer, BUFFER_SIZE, pFile[index]) != NULL)
                {
			//printf("C3: %s", buffer);
			//getchar();
                	int eventCode = getEventCode(buffer);
                       	if (eventCode != endEvent)
                        {
				if (eventCode >= GET && eventCode <= ACCUMULATE)
				{	
					char *tmpBuffer = buffer;
					char *tmpStr = getData(&tmpBuffer);
					free(tmpStr);
					char *origin_addr = getData(&tmpBuffer);
					tmpStr = getData(&tmpBuffer);
					free(tmpStr);
					tmpStr = getData(&tmpBuffer);
					int target_rank = atoi(tmpStr);
					free(tmpStr);
					char *target_addr = (char *) malloc((strlen(aList[target_rank]->base) + 1) * sizeof(char));
					strcpy(target_addr, aList[target_rank]->base);
					Comm *aComm = initComm(eventCode, target_rank, target_addr);
					insertCommNode(aList[index]->tail, aComm);
					Loca *aLoca = initLoca(eventCode, origin_addr);
					insertChain(aChain, aLoca);
				}
				else if (eventCode >= LOAD && eventCode <= STORE)
				{
					char *tmpBuffer = buffer;
					char *tmpStr = getData(&tmpBuffer);
					free(tmpStr);
					char *varAddr = getData(&tmpBuffer);
					char *varAddr2 = (char *) malloc((strlen(varAddr) + 1) * sizeof(char));
					strcpy(varAddr2, varAddr);
					Loca *aLoca = initLoca(eventCode, varAddr);
					insertLocaNode(aList[index]->tail, aLoca);
					Loca *aLoca2 = initLoca(eventCode, varAddr2);
					insertChain(aChain, aLoca2);
				}
				else 
				{ 
					//do nothing
				}
                    	}
                        else //if (eventCode == endEvent)
                        {
                        	break;
                        }
          	}
		else //if (fgets(buffer, BUFFER_SIZE, pFile[i]) != NULL)
                {
                	//do nothing
                }
		free(buffer);
	}
}

void detectMCEInProc(Chai *aChain)
{
	if (aChain->head == NULL && aChain->tail == NULL) 
	{ 
		//do nothing
	}
	else 
	{
		Loca *tmp;
		while (aChain->head != NULL)
		{
			aChain->tail = aChain->head;
			if (aChain->head->code != LOAD && aChain->head->code != STORE)
                        {
				tmp = aChain->head->next;
                        	while (tmp != NULL)
                        	{
                                	if (aChain->head->code == GET)
                                	{
						if (strcmp(aChain->head->varAddr, tmp->varAddr) == 0)
						{
                                                	printf("MCE within an epoch on %s between %s and %s in P%d\n",
									tmp->varAddr, convertCode2Name(aChain->head->code),
										convertCode2Name(tmp->code), aChain->rank);
						}
						else //if (strcmp(aChain.tail->varAddr, tmp->varAddr) != 0)
						{
							//do nothing
						}
                                	}
                                	else if (aChain->head->code == PUT || aChain->head->code == ACCUMULATE)
                                	{
						if (tmp->code == GET || tmp->code == STORE)
						{
							if (strcmp(aChain->head->varAddr, tmp->varAddr) == 0)
                                                	{
                                                        	printf("MCE within an epoch on %s between %s and %s in P%d\n", 
										tmp->varAddr, convertCode2Name(aChain->head->code), 
											convertCode2Name(tmp->code), aChain->rank);
                                                	}
                                                	else //if (strcmp(aChain.tail->varAddr, tmp->varAddr) != 0)
                                                	{
                                                        	//do nothing
                                                	}
						}
						else //if (tmp->varAddr->code != GET && tmp->varAddr->code != STORE)
						{
							//do nothing
						}
                                	}
                                	else //if (aChain.tail->code == LOAD || aChain.tail->code == STORE) 
                                	{
                                        	//do nothing
                                	}
                                	tmp = tmp->next;
                        	}
                        }
                        else //if (aChain.tail->code == LOAD || aChain.tail->code == STORE) 
                        {
                                        //do nothing
                        }
			aChain->head = aChain->head->next;
			free(aChain->tail->varAddr);
			aChain->tail->next = NULL;
                	free(aChain->tail);
		}
	}
}

void detectMCEAcrossProc(List **aList, int size)
{
	int i, j;
	Node *nodeTmp1, *nodeTmp2;
	Comm *commTmp1, *commTmp2;
	Loca *locaTmp1, *locaTmp2;

	for (i = 0; i < size; i++)
	{
		nodeTmp1 = aList[i]->head;
		while (nodeTmp1 != NULL)
		{
			for (j = i + 1; j < size; j++)
			{
				nodeTmp2 = aList[j]->head;
				while (nodeTmp2 != NULL)
				{
					if (isConcurrent(nodeTmp1->clock, i, nodeTmp2->clock, j) == -1)
					{
						break;
					}
					else if (isConcurrent(nodeTmp1->clock, i, nodeTmp2->clock, j) == 1)
					{
						//do nothing
					}
					else //if (isConcurrent(nodeTmp1->clock, i, nodeTmp2->clock, j) == 0)
					{
						commTmp1 = nodeTmp1->commHead;
						while (commTmp1 != NULL)
						{
							commTmp2 = nodeTmp2->commHead;
							while (commTmp2 != NULL)
							{
								if (commTmp1->target_rank == commTmp2->target_rank &&
									strcmp(commTmp1->target_addr, commTmp2->target_addr) == 0)
								{
                                        				printf("MCE across processes on %s in P%d between %s in P%d and %s in P%d\n",
                                                        			commTmp1->target_addr, commTmp1->target_rank,
										convertCode2Name(commTmp1->code), i,
										convertCode2Name(commTmp2->code), j);
								}
								else
								{
									//do nothing
								}
								commTmp2 = commTmp2->next;
							}
							
							locaTmp2 = nodeTmp2->locaHead;
							while (locaTmp2 != NULL)
							{
								if (commTmp1->target_rank == j &&
									strcmp(commTmp1->target_addr, locaTmp2->varAddr) == 0)
								{
									printf("MCE across processes on %s in P%d between %s in P%d and %s in P%d\n",
                                                        			commTmp1->target_addr, j,
										convertCode2Name(commTmp1->code), i,
                                                                        	convertCode2Name(locaTmp2->code), j);
								}
								else
								{
									//do nothing
								}
								locaTmp2 = locaTmp2->next;
							}
							commTmp1 = commTmp1->next;
						}

						locaTmp1 = nodeTmp1->locaHead;
						while (locaTmp1 != NULL)
						{
							commTmp2 = nodeTmp2->commHead;
							while (commTmp2 != NULL)
							{
								if (commTmp2->target_rank == i &&
                                                                        strcmp(commTmp2->target_addr, locaTmp1->varAddr) == 0)
								{
                                                                        printf("MCE across processes on %s in P%d between %s in P%d and %s in P%d\n",
                                                                                commTmp2->target_addr, i,
										convertCode2Name(locaTmp1->code), i,
                                                                                convertCode2Name(commTmp2->code), j);
								}
								else
								{
									//do nothing
								}
								commTmp2 = commTmp2->next;
							}
							locaTmp1 = locaTmp1->next;
						}
					}
					nodeTmp2 = nodeTmp2->next;
				}
			}
			nodeTmp1 = nodeTmp1->next;
		}
	}
}

int main(int argc, char **argv)
{
	int size, i, index, *clock, *tmpClock, tmpInt, eventCode, j;
	char fileName[15];
	char *buffer, *tmpBuffer, *tmpStr;

	size = atoi(argv[1]);
	FILE **pFile = (FILE **) malloc(size * sizeof(FILE *));
	List **aList = (List **) malloc(size * sizeof(List *));
	for (i = 0; i < size; i++)
	{
		sprintf(fileName, "trace%d", i);
		pFile[i] = fopen(fileName, "r");
		if (pFile[i] == NULL)
		{
			perror("Error opening file");
		}

		buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
		fgets(buffer, BUFFER_SIZE, pFile[i]);

		tmpBuffer = buffer;
                tmpStr = getData(&tmpBuffer);
		free(tmpStr);

		tmpStr = getData(&tmpBuffer);
		char *base = tmpStr;

		tmpStr = getData(&tmpBuffer);
		int size = atoi(tmpStr);
		free(tmpStr);

		tmpStr = getData(&tmpBuffer);
		int disp_unit = atoi(tmpStr);
		free(tmpStr);

		aList[i] = initList(base, size, disp_unit);
		free(buffer);
	}

	index = 0;
	while (index < size)
	{
		buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
		if (fgets(buffer, BUFFER_SIZE, pFile[index]) != NULL)
		{              
			//printf("C1: %s", buffer);
			if (getEventCode(buffer) == FENCE)
			{
				clock = getClock(buffer, size);
				Node *aNode = initNode(clock);
				insertList(aList[index], aNode);
				free(buffer);
				
				for (i = index + 1; i < size; i++)
				{
					while (true)
					{
						buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
						if (fgets(buffer, BUFFER_SIZE, pFile[i]) != NULL)
						{
							//printf("C2: %s",buffer);
							if (getEventCode(buffer) == FENCE)
							{
								clock = getClock(buffer, size);
								if (equalClock(aList[index]->head->clock, clock, size) == true)
								{
									Node *aNode = initNode(clock);
									insertList(aList[i], aNode);
									break;
								}
								else //if (equalClock(aList[index].clock, clock, size) == false)
								{
									free(clock);
								}
							}
							else //if (getEventCode(buffer) != FENCE)
							{
								//do nothing
							}
						}
						else //if (fgets(buffer, BUFFER_SIZE, pFile[i]) == NULL)
						{
							//do nothing
						}
						free(buffer);
					}
				}
	
				for (i = index; i < size; i++)
                        	{
					Chai *aChain = initChain(i);	
					readEventWithinEpoch(pFile, i, aList, aChain, FENCE);
					
					/* detect MCE within an epoch */
					detectMCEInProc(aChain);
                        	}
				
				/* dectect MCE across processes */
				detectMCEAcrossProc(aList, size);
				freeAllList(aList, size);
			}
			
			else if (getEventCode(buffer) == POST)
			{
				clock = getClock(buffer, size);
				Node *aNode = initNode(clock);
				insertList(aList[index], aNode);
				Chai *aChain = initChain(index);
				readEventWithinEpoch(pFile, index, aList, aChain, WAIT);
				
				/* detect MCE within an epoch */
                                detectMCEInProc(aChain);
			}
			else if (getEventCode(buffer) == START)
			{
				clock = getClock(buffer, size);
				Node *aNode = initNode(clock);
				insertList(aList[index], aNode);
                                Chai *aChain = initChain(index);
                                readEventWithinEpoch(pFile, index, aList, aChain, COMPLETE);

                                /* detect MCE within an epoch */
                                detectMCEInProc(aChain);
			}
			else if (getEventCode(buffer) == LOCK)
			{
				clock = getClock(buffer, size);
				Node *aNode = initNode(clock);
				insertList(aList[index], aNode);
				Chai *aChain = initChain(index);
                                readEventWithinEpoch(pFile, index, aList, aChain, UNLOCK);

                                /* detect MCE within an epoch */
                                detectMCEInProc(aChain);
			}
			else if (getEventCode(buffer) == BARRIER)
			{
				//detect MCE across processes
				detectMCEAcrossProc(aList, size);
				freeAllList(aList, size);
			}
			else 
			{ 
				//do nothing
			}
		}
		else 
		{
			index++;
		}
		free(buffer);
	}

	for (i = 0; i < size; i++)
	{
		fclose(pFile[i]);
	}
	free(pFile);

	//End Of File
	//detect MCE across processes
	detectMCEAcrossProc(aList, size);
	freeAllList(aList, size);

	return 0;
}
