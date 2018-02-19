#include "detector.h"

List insertCommNode(List aList, int code, char *origin_addr, int target_rank, char *target_addr/*, int *clock, int size*/)
{
	if (aList.commHead == NULL && aList.commTail == NULL)
	{
		aList.commTail = (Comm *) malloc(sizeof(Comm));
		aList.commHead = aList.commTail;
	}
	else
	{
		aList.commTail->next = (Comm *) malloc(sizeof(Comm));
		aList.commTail = aList.commTail->next;
	}
	aList.commTail->code = code;
	aList.commTail->origin_addr = origin_addr;
	aList.commTail->target_rank = target_rank;
	aList.commTail->target_addr = target_addr;
	//aList.commTail->clock = (int *) malloc (size * sizeof(int));
	aList.commTail->next = NULL;
	return aList;	
}

List insertLocaNode(List aList, int code, char *varAddr)
{
	if (aList.locaHead == NULL && aList.locaTail == NULL)
        {
                aList.locaTail = (Loca *) malloc(sizeof(Loca));
                aList.locaHead = aList.locaTail;
        }
        else
        {
                aList.locaTail->next = (Loca *) malloc(sizeof(Loca));
                aList.locaTail = aList.locaTail->next;
        }
	aList.locaTail->code = code;
	aList.locaTail->varAddr = varAddr;
	aList.locaTail->next = NULL;
	return aList;
}

Chai insertChainNode(Chai aChain, int code, char *varAddr)
{
	if (aChain.head == NULL && aChain.tail == NULL)
	{
		aChain.tail = (Loca *) malloc(sizeof(Loca));
		aChain.head = aChain.tail;
	}
	else
	{
		aChain.tail->next = (Loca *) malloc(sizeof(Loca));
		aChain.tail = aChain.tail->next;
	}
	aChain.tail->code = code;
	aChain.tail->varAddr = (char *) malloc((strlen(varAddr) + 1) * sizeof (char));
	strcpy(aChain.tail->varAddr, varAddr);
	aChain.tail->next = NULL;
	return aChain;
}

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

void detectMCEInProc(Chai aChain)
{
	if (aChain.head == NULL && aChain.tail == NULL) 
	{ 
		/* do nothing */ 
	}
	else 
	{
		Loca *tmp;
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

int main(int argc, char **argv)
{
	int size, i, index, *clock, *tmpClock, tmpInt, eventCode, j;
	char fileName[15];
	char *buffer, *clockStr, *tmpBuffer, *tmpStr;
	FILE **pFile;
	List *aList;

	size = atoi(argv[1]);
	pFile = (FILE **) malloc(size * sizeof(FILE *));
	aList = (List *) malloc(size * sizeof(List));
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

	buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
	index = 0;
	while (index < size)
	{
		if (fgets(buffer, BUFFER_SIZE, pFile[index]) != NULL)
		{              
			printf("C1: %s", buffer);
                        getchar();
			if (getEventCode(buffer) == FENCE)
			{
				clockStr = (char *) malloc((size * 2 + 2) * sizeof(char));
				memcpy(clockStr, strstr(buffer, "["), size * 2 + 1);
				clockStr[size * 2 + 1] = '\0';
				clock = getClock(clockStr);
				
				for (i = index + 1; i < size; i++)
				{
					while (true)
					{
						if (fgets(buffer, BUFFER_SIZE, pFile[i]) != NULL)
						{
							printf("C2: %s",buffer);
							getchar();
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
					}
				}
	
				for (i = index; i < size; i++)
                        	{
					Chai aChain;
					aChain.rank = i;
					aChain.head = NULL;
					aChain.tail = NULL;
					while (true)
					{
                                		if (fgets(buffer, BUFFER_SIZE, pFile[i]) != NULL)
                                		{
							printf("C3: %s", buffer);
							getchar();
							eventCode = getEventCode(buffer);
                                        		if (eventCode != FENCE)
							{
								if (eventCode >= GET && eventCode <= ACCUMULATE)
								{
									tmpBuffer = buffer;
									tmpStr = getData(&tmpBuffer);
									free(tmpStr);
									char *origin_addr = getData(&tmpBuffer);
									tmpStr = getData(&tmpBuffer);
									int target_rank = atoi(tmpStr);
									free(tmpStr);
									aList[i] = insertCommNode(aList[i], eventCode, origin_addr, 
													target_rank, aList[target_rank].base);
									
									aChain = insertChainNode(aChain, eventCode, origin_addr);
								}
								else if (eventCode >= LOAD && eventCode <= STORE)
								{
									tmpBuffer = buffer;
									tmpStr = getData(&tmpBuffer);
									free(tmpStr);
									char *varAddr = getData(&tmpBuffer);
									aList[i] = insertLocaNode(aList[i], eventCode, varAddr);

									aChain = insertChainNode(aChain, eventCode, varAddr);
								}
								else 
								{ 
									/* do nothing */ 
								}
							}
							else //if (eventCode == FENCE)
							{
								break;
							}
						}
                                	}
					
					/* detect MCE within an epoch */
					detectMCEInProc(aChain);

                        	}
				
				//dectect MCE accross processes

			}
			
			else if (getEventCode(buffer) == BARRIER)
			{
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
	}

	for (i = 0; i < size; i++)
	{
		fclose(pFile[i]);
	}
	free(pFile);

	//End Of File

	free(aList);

	return 0;
}
