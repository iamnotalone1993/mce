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

int *getClock(char *str)
{
	int *clock, size, i;
	size = strlen(str) / 2;
	clock = (int *) malloc(size * sizeof(int));
	for (i = 0; i < size; i++)
	{
		clock[i] = str[i * 2 + 1] - '0';
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

bool isConcurrent(int *clock1, int rank1, int *clock2, int rank2)
{
	if (clock1[rank2] >= clock2[rank2])
		return false;
	if (clock2[rank1] >= clock1[rank1])
		return false;
	return true;
}

void insertCommNode(List *aList, int code, int target_rank, char *target_addr/*, int *clock, int size*/)
{
	if (aList->commHead == NULL && aList->commTail == NULL)
	{
		aList->commTail = (Comm *) malloc(sizeof(Comm));
		aList->commHead = aList->commTail;
	}
	else
	{
		aList->commTail->next = (Comm *) malloc(sizeof(Comm));
		aList->commTail = aList->commTail->next;
	}
	aList->commTail->code = code;
	aList->commTail->target_rank = target_rank;
	aList->commTail->target_addr = target_addr;
	//aList.commTail->clock = (int *) malloc (size * sizeof(int));
	aList->commTail->next = NULL;
}

void insertLocaNode(List *aList, int code, char *varAddr)
{
	if (aList->locaHead == NULL && aList->locaTail == NULL)
        {
                aList->locaTail = (Loca *) malloc(sizeof(Loca));
                aList->locaHead = aList->locaTail;
        }
        else
        {
                aList->locaTail->next = (Loca *) malloc(sizeof(Loca));
                aList->locaTail = aList->locaTail->next;
        }
	aList->locaTail->code = code;
	aList->locaTail->varAddr = varAddr;
	aList->locaTail->next = NULL;
}

void freeList(List *aList, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		while (aList[i].commHead != NULL)
		{
			aList[i].commTail = aList[i].commHead;
			aList[i].commHead = aList[i].commHead->next;
			free(aList[i].commTail->target_addr);
			free(aList[i].commTail);
		}
		aList[i].commTail = NULL;

		while (aList[i].locaHead != NULL)
		{
			aList[i].locaTail = aList[i].locaHead;
			aList[i].locaHead = aList[i].locaHead->next;
			free(aList[i].locaTail->varAddr);
			free(aList[i].locaTail);
		}
		aList[i].locaTail = NULL;
	}
}

void printList(List *aList, int size)
{
	int i;
	printf("\nPrinting a List...\n");
	for (i = 0; i < size; i++)
	{
		printf("rank=%d base=%s size=%d disp_unit=%d\n", i, aList[i].base, aList[i].size, aList[i].disp_unit);
		Comm *tmp1 = aList[i].commHead;
		while (tmp1 != NULL)
		{
			printf("code=%s target_rank=%d\n", convertCode2Name(tmp1->code), tmp1->target_rank);
			tmp1 = tmp1->next;
		}
		Loca *tmp2 = aList[i].locaHead;
		while (tmp2 != NULL)
		{
			printf("code=%s\n", convertCode2Name(tmp2->code));
			tmp2 = tmp2->next;
		}
	}
}

void insertChainNode(Chai *aChain, int code, char *varAddr)
{
	if (aChain->head == NULL && aChain->tail == NULL)
	{
		aChain->tail = (Unif *) malloc(sizeof(Unif));
		aChain->head = aChain->tail;
	}
	else
	{
		aChain->tail->next = (Unif *) malloc(sizeof(Unif));
		aChain->tail = aChain->tail->next;
	}
	aChain->tail->code = code;
	aChain->tail->varAddr = (char *) malloc((strlen(varAddr) + 1) * sizeof (char));
	strcpy(aChain->tail->varAddr, varAddr);
	aChain->tail->next = NULL;
}

void printChain(Chai aChain)
{
	printf("\nPrinting a Chain...\n");
	printf("rank=%d\n", aChain.rank);
	Unif *tmp = aChain.head;
	while (tmp != NULL)
	{
		printf("code=%s varAddr=%s\n", convertCode2Name(tmp->code), tmp->varAddr);
		tmp = tmp->next;
	}
}

void readEventWithinEpoch(FILE **pFile, int index, List *aList, Chai *aChain, int endEvent)
{
	while (true)
        {
        	char *buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
        	if (fgets(buffer, BUFFER_SIZE, pFile[index]) != NULL)
                {
			printf("C9: %s", buffer);
			getchar();
                	int eventCode = getEventCode(buffer);
                       	if (eventCode != endEvent)
                        {
				if (eventCode >= GET && eventCode <= ACCUMULATE)
				{
					char *tmpBuffer = buffer;
					char *tmpStr = getData(&tmpBuffer);
					free(tmpStr);
					tmpStr = getData(&tmpBuffer);
					free(tmpStr);
					char *origin_addr = getData(&tmpBuffer);
					tmpStr = getData(&tmpBuffer);
					int target_rank = atoi(tmpStr);
					free(tmpStr);
					char *target_addr = 
					(char *) malloc((strlen(aList[target_rank].base) + 1) * sizeof(char));
					strcpy(target_addr, aList[target_rank].base);
					insertCommNode(&aList[target_rank], eventCode, target_rank, target_addr);
					insertChainNode(aChain, eventCode, origin_addr);
				}
				else if (eventCode >= LOAD && eventCode <= STORE)
				{
					char *tmpBuffer = buffer;
					char *tmpStr = getData(&tmpBuffer);
					free(tmpStr);
					char *varAddr = getData(&tmpBuffer);
					insertLocaNode(&aList[index], eventCode, varAddr);
					insertChainNode(aChain, eventCode, varAddr);
				}
				else 
				{ 
					/* do nothing */ 
				}
                    	}
                        else //if (eventCode == endEvent)
                        {
                        	break;
                        }
			free(buffer);
          	}
		else //if (fgets(buffer, BUFFER_SIZE, pFile[i]) != NULL)
                {
                	/* do nothing */
                }
	}
}

void detectMCEInProc(Chai aChain)
{
	if (aChain.head == NULL && aChain.tail == NULL) 
	{ 
		/* do nothing */ 
	}
	else 
	{
		Unif *tmp;
		while (aChain.head != NULL)
		{
			aChain.tail = aChain.head;
			if (aChain.head->code != LOAD && aChain.head->code != STORE)
                        {
				tmp = aChain.head->next;
                        	while (tmp != NULL)
                        	{
                                	if (aChain.head->code == GET)
                                	{
						if (strcmp(aChain.head->varAddr, tmp->varAddr) == 0)
						{
                                                	printf("MCE within an epoch on %s between %s and %s in process %d\n", tmp->varAddr, 
                                                        	convertCode2Name(aChain.head->code), convertCode2Name(tmp->code), aChain.rank);
						}
						else //if (strcmp(aChain.tail->varAddr, tmp->varAddr) != 0)
						{
							/* do nothing */
						}
                                	}
                                	else if (aChain.head->code == PUT || aChain.head->code == ACCUMULATE)
                                	{
						if (tmp->code == GET || tmp->code == STORE)
						{
							if (strcmp(aChain.head->varAddr, tmp->varAddr) == 0)
                                                	{
                                                        	printf("MCE within an epoch on %s between %s and %s in process %d\n", 
									tmp->varAddr, convertCode2Name(aChain.head->code), 
										convertCode2Name(tmp->code), aChain.rank);
                                                	}
                                                	else //if (strcmp(aChain.tail->varAddr, tmp->varAddr) != 0)
                                                	{
                                                        	/* do nothing */
                                                	}
						}
						else //if (tmp->varAddr->code != GET && tmp->varAddr->code != STORE)
						{
							/* do nothing */
						}
                                	}
                                	else //if (aChain.tail->code == LOAD || aChain.tail->code == STORE) 
                                	{
                                        	/* do nothing */
                                	}
                                	tmp = tmp->next;
                        	}
                        }
                        else //if (aChain.tail->code == LOAD || aChain.tail->code == STORE) 
                        {
                                        /* do nothing */
                        }
			aChain.head = aChain.head->next;
			free(aChain.tail->varAddr);
			aChain.tail->next = NULL;
                	free(aChain.tail);
		}
	}
}

void detectMCEAcrossProc(List *aList, int size)
{
	int i;
	Comm *commTmp1, *commTmp2;
	Loca *locaTmp3;
	for (i = 0; i < size; i++)
	{
		commTmp1 = aList[i]->commHead;
		while (commTmp1 != NULL)
		{
			commTmp2 = commTmp2->next;
			while (commTmp2 != NULL)
			{
				if (strcmp(commTmp1->target_addr, commTmp2->target_addr) == 0 &&
					isConcurrent(commTmp1->clock, commTmp1->origin_rank, commTmp2->clock, commTmp2->origin_rank) == true)
				{
					printf("MCE across processes on %s in %d between %s in process %d and %s in process %d\n",
							commTmp1->target_addr, i, convertCode2Name(commTmp1->code), commTmp1->origin_rank,
									convertCode2Name(commTmp2->code), commTmp2->origin_rank);
				}
				else 
				{
					/* do nothing */
				}
				commTmp2 = commTmp2->next;
			}
			commTmp1 = commTmp1->next;
			
			locaTmp3 = aList[i]->locaHead;	
			while (locaTmp3 != NULL)
			{
				if (strcmp(commTmp1->target_addr, locaTmp3->varAddr) == 0 &&
					isConcurrent(commTmp1->clock, commTmp1->origin_rank, locaTmp3->clock, i) == true)
				{
					printf("MCE across processes on %s in %d between %s in process %d and %s\n",
                                                        commTmp1->target_addr, i, convertCode2Name(commTmp1->code), commTmp1->origin_rank,
                                                                        convertCode2Name(locaTmp3->code));
				}
				else 
				{
					/* do nothing */
				}
				locaTmp3 = locaTmp3->next;
			}
		}
	}
}

int main(int argc, char **argv)
{
	int size, i, index, *clock, *tmpClock, tmpInt, eventCode, j;
	char fileName[15];
	char *buffer, *clockStr, *tmpBuffer, *tmpStr;

	size = atoi(argv[1]);
	FILE **pFile = (FILE **) malloc(size * sizeof(FILE *));
	List *aList = (List *) malloc(size * sizeof(List));
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
		aList[i].base = tmpStr;

		tmpStr = getData(&tmpBuffer);
		aList[i].size = atoi(tmpStr);
		free(tmpStr);

		tmpStr = getData(&tmpBuffer);
		aList[i].disp_unit = atoi(tmpStr);
		free(tmpStr);

		aList[i].commHead = NULL;
		aList[i].commTail = NULL;
		aList[i].locaHead = NULL;
		aList[i].locaTail = NULL;
		free(buffer);
	}

	index = 0;
	while (index < size)
	{
		buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
		if (fgets(buffer, BUFFER_SIZE, pFile[index]) != NULL)
		{              
			printf("C1: %s", buffer);
			if (getEventCode(buffer) == FENCE)
			{
				/* get Clock */
				clockStr = (char *) malloc((size * 2 + 2) * sizeof(char));
				memcpy(clockStr, strstr(buffer, "["), size * 2 + 1);
				clockStr[size * 2 + 1] = '\0';
				clock = getClock(clockStr);
				free(buffer);
				
				for (i = index + 1; i < size; i++)
				{
					while (true)
					{
						buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
						if (fgets(buffer, BUFFER_SIZE, pFile[i]) != NULL)
						{
							printf("C2: %s",buffer);
							if (getEventCode(buffer) == FENCE)
							{
								clockStr = (char *) malloc((size * 2 + 2) * sizeof(char));
                               					memcpy(clockStr, strstr(buffer, "["), size * 2 + 1);
                               					clockStr[size * 2 + 1] = '\0';
								if (equalClock(clock, getClock(clockStr), size) == true)
								{
									break;
								}
							}
						}
						free(buffer);
					}
				}
	
				for (i = index; i < size; i++)
                        	{
					Chai aChain;
					aChain.rank = i;
					aChain.head = NULL;
					aChain.tail = NULL;
					readEventWithinEpoch(pFile, i, aList, &aChain, FENCE);
					
					/* detect MCE within an epoch */
					detectMCEInProc(aChain);

                        	}
				
				/* dectect MCE across processes */
				freeList(aList, size);
			}
			
			else if (getEventCode(buffer) == POST)
			{
				/* get Clock */
                                clockStr = (char *) malloc((size * 2 + 2) * sizeof(char));
                                memcpy(clockStr, strstr(buffer, "["), size * 2 + 1);
                                clockStr[size * 2 + 1] = '\0';
                                clock = getClock(clockStr);
				
				Chai aChain;
                                aChain.rank = index;
                                aChain.head = NULL;
                                aChain.tail = NULL;
				readEventWithinEpoch(pFile, index, aList, &aChain, WAIT);

				/* detect MCE within an epoch */
                                detectMCEInProc(aChain);
			}
			else if (getEventCode(buffer) == START)
			{
				/* get Clock */
                                clockStr = (char *) malloc((size * 2 + 2) * sizeof(char));
                                memcpy(clockStr, strstr(buffer, "["), size * 2 + 1);
                                clockStr[size * 2 + 1] = '\0';
                                clock = getClock(clockStr);

                                Chai aChain;
                                aChain.rank = index;
                                aChain.head = NULL;
                                aChain.tail = NULL;
                                readEventWithinEpoch(pFile, index, aList, &aChain, COMPLETE);

                                /* detect MCE within an epoch */
                                detectMCEInProc(aChain);
			}
			else if (getEventCode(buffer) == LOCK)
			{
                                /* get Clock */
                                clockStr = (char *) malloc((size * 2 + 2) * sizeof(char));
                                memcpy(clockStr, strstr(buffer, "["), size * 2 + 1);
                                clockStr[size * 2 + 1] = '\0';
                                clock = getClock(clockStr);

                                Chai aChain;
                                aChain.rank = index;
                                aChain.head = NULL;
                                aChain.tail = NULL;
                                readEventWithinEpoch(pFile, index, aList, &aChain, UNLOCK);

                                /* detect MCE within an epoch */
                                detectMCEInProc(aChain);
			}
			else if (getEventCode(buffer) == BARRIER)
			{
				//detect MCE across processes
			}
			else 
			{ 
				/* do nothing */ 
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
	printList(aList, size);
	freeList(aList, size);

	return 0;
}
