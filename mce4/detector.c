#include "detector.h"

int getMemory()
{
        char *tmp, *tmp1, *fileName;
        int result, pid; 
        fileName = (char *) malloc(25 * sizeof(char));
        pid = getpid();
        sprintf(fileName, "/proc/%d/status", pid);
        FILE *pFile = fopen(fileName, "r");
        free(fileName);
        char *buffer = (char *) malloc(50 * sizeof(char));
        while (fgets(buffer, 50, pFile) != NULL)
        {
                if (strstr(buffer, "VmRSS:") != NULL)
                {
                        tmp = strpbrk(buffer, "0123456789");
                        result = strcspn(tmp, " \t");
                        tmp1 = (char *) malloc((result + 1) * sizeof(char));
                        memcpy(tmp1, tmp, result);
                        tmp1[result] = '\0';
                        result = atoi(tmp1);
                        free(tmp1);
                        break;
                }
                else
                {
                        free(buffer);
                        buffer = (char *) malloc(50 * sizeof(char));
                }
        }
        free(buffer);
        fclose(pFile);
        return result;
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

void printClock(FILE *fp, EVC *EVCList, int size)
{
	int i;

	fprintf(fp, "clockBase=[");
	for (i = 0; i < size - 1; i++)
	{
		gmp_fprintf(fp, "%Zd|", EVCList[i].clockBase);
	}
	gmp_fprintf(fp, "%Zd]\n", EVCList[i].clockBase);

	fprintf(fp, "clock=[");
	for (i = 0; i < size - 1; i++)
	{
		gmp_fprintf(fp, "%Zd|", EVCList[i].clock);
	}
	gmp_fprintf(fp, "%Zd]\n", EVCList[i].clock);
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

int isConcurrent(mpz_t clock1, mpz_t clock2)
{
	if (mpz_divisible_p(clock1, clock2) != 0)
		return 1;
	if (mpz_divisible_p(clock2, clock1) != 0)
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

void printComm(FILE *fp, Comm *aComm)
{
	char *code = convertCode2Name(aComm->code);
	fprintf(fp, "code=%s target_rank=%d target_addr=%s\n", code, aComm->target_rank, aComm->target_addr);
	free(code);
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

void printLoca(FILE *fp, Loca *aLoca)
{
	char *code = convertCode2Name(aLoca->code);
	fprintf(fp, "code=%s varAddr=%s\n", code, aLoca->varAddr);
	free(code);
}

Node *initNode(mpz_t aClock)
{
	Node *aNode = (Node *) malloc(sizeof(Node));
	mpz_init_set(aNode->clock, aClock);
	aNode->commHead = NULL;
	aNode->commTail = NULL;
	aNode->locaHead = NULL;
	aNode->locaTail = NULL;
	aNode->next = NULL;
	return aNode;
}

void freeNode(Node *aNode)
{
        mpz_clear(aNode->clock);

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

void printNode(FILE *fp, Node *aNode, int size)
{
	fprintf(fp, "Printing a Node...\n");
	int i;
	gmp_fprintf(fp, "clock=%Zd\n", aNode->clock);
	
	Comm *commTmp = aNode->commHead;
	while (commTmp != NULL)
	{
		printComm(fp, commTmp);
		commTmp = commTmp->next;
	}

	Loca *locaTmp = aNode->locaHead;
	while (locaTmp != NULL)
	{
		printLoca(fp, locaTmp);
		locaTmp = locaTmp->next;
	}
}

List* initList(char *base, int size)
{
	List *aList = (List *) malloc(size * sizeof(List));
	aList->base = base;
	aList->size = size;
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
		freeNode(aList->tail);
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

void printList(FILE *fp, List *aList, int size)
{
	fprintf(fp, "Printing a List...\n");
	fprintf(fp, "base=%s size=%d\n", aList->base, aList->size);

	Node *tmp = aList->head;
	while (tmp != NULL)
	{
		printNode(fp, tmp, size);
		tmp = tmp->next;
	}
}

void printAllList(FILE *fp, List **aList, int size)
{
	int i;
	fprintf(fp, "Printing all Lists...\n");
	for (i = 0; i < size; i++)
	{
		printList(fp, aList[i], size);
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
	char *code;
	printf("\nPrinting a Chain...\n");
	printf("rank=%d\n", aChain->rank);
	Loca *tmp = aChain->head;
	while (tmp != NULL)
	{
		code = convertCode2Name(tmp->code);
		printf("code=%s varAddr=%s\n", code, tmp->varAddr);
		free(code);
		tmp = tmp->next;
	}
}

Int *initInt(int num)
{
	Int *aInt = (Int *) malloc(sizeof(Int));
	aInt->num = num;
	aInt->next = NULL;
	return aInt;
}

void freeInt(Int *aInt)
{
	free(aInt);
}

IntList *initIntList()
{
	IntList *aIntList = (IntList *) malloc(sizeof(IntList));
	aIntList->head = NULL;
	aIntList->tail = NULL;
	return aIntList;
}

bool isIntListEmpty(IntList *aIntList)
{
	if (aIntList->head == NULL) return true;
	else false;
}

void insertIntList(IntList *aIntList, Int *aInt)
{
        if (aIntList->head == NULL && aIntList->tail == NULL)
        {
                aIntList->tail = aInt;
		aIntList->head = aIntList->tail;
        }
        else
        {
                aIntList->tail->next = aInt;
                aIntList->tail = aIntList->tail->next;
        }
}

Int *removeIntList(IntList *aIntList)
{
	if (aIntList->head == NULL)
	{
		return NULL;
	}
	else //if (aIntList->head != NULL)
	{
		Int *tmpInt = aIntList->head;
		aIntList->head = aIntList->head->next;
		if (aIntList->head == aIntList->tail)
		{
			aIntList->tail = NULL;
		}
		return tmpInt;
	}
}

void removeAnIntList(IntList *aIntList, int num)
{
	Int *preInt = NULL;
	Int *tmpInt = aIntList->head;
	while (tmpInt != NULL)
	{
		if (tmpInt->num == num)
		{
			if (tmpInt == aIntList->head)
			{
				aIntList->head = aIntList->head->next;
				if (tmpInt == aIntList->tail)
				{
					aIntList->tail = NULL;
				}
				else {/* do nothing */}
			}
			else //if (tmpInt != aIntList->head)
			{
				if (tmpInt == aIntList->tail)
				{
					aIntList->tail = preInt;
				}
				else {/* do nothing */}
				preInt->next = tmpInt->next;
			}
			freeInt(tmpInt);
			break;
		}
		else {/* do nothing */}
		preInt = tmpInt;
		tmpInt = tmpInt->next;
	}
}

void readEventWithinEpoch(FILE **pFile, int index, EVC *EVCList, int size, List **aList, Chai *aChain, int endEvent)
{
	while (true)
        {
        	char *buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
        	if (fgets(buffer, BUFFER_SIZE, pFile[index]) != NULL)
                {
			//printf("CP3: %s", buffer);
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
				else if (eventCode == CREATE || eventCode == BARRIER || eventCode == FENCE) 
				{
					//skip
					//printf("CP4: %s", buffer);
					//getchar();
					free(buffer);
					break;
				}
				else {/* do nothing */}
                    	}
                        else //if (eventCode == endEvent)
                        {
				if (eventCode != COMPLETE && eventCode != WAIT)
				{
					mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);
				}
				free(buffer);
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
							char *code1 = convertCode2Name(aChain->head->code);
							char *code2 = convertCode2Name(tmp->code);
                                                	printf("MCE within an epoch on %s between %s and %s in P%d\n",
											tmp->varAddr, code1, code2, aChain->rank);
							free(code1);
							free(code2);
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
								char *code1 = convertCode2Name(aChain->head->code);
								char *code2 = convertCode2Name(tmp->code);
                                                        	printf("MCE within an epoch on %s between %s and %s in P%d\n", 
												tmp->varAddr, code1, code2, aChain->rank);
								free(code1);
								free(code2);
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
			freeLoca(aChain->tail);
		}
		free(aChain);
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
					if (isConcurrent(nodeTmp1->clock, nodeTmp2->clock) == -1)
					{
						break;
					}
					else if (isConcurrent(nodeTmp1->clock, nodeTmp2->clock) == 1)
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
									char *code1 = convertCode2Name(commTmp1->code);
									char *code2 = convertCode2Name(commTmp2->code);
                                        				printf("MCE across processes on %s in P%d between %s in P%d and %s in P%d\n", commTmp1->target_addr, commTmp1->target_rank, code1, i, code2, j);
									free(code1);
									free(code2);
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
									char *code1 = convertCode2Name(commTmp1->code);
									char *code2 = convertCode2Name(locaTmp2->code);
									printf("MCE across processes on %s in P%d between %s in P%d and %s in P%d\n", commTmp1->target_addr, j, code1, i, code2, j);
									free(code1);
									free(code2);
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
									char *code1 = convertCode2Name(locaTmp1->code);
									char *code2 = convertCode2Name(commTmp2->code);
                                                                        printf("MCE across processes on %s in P%d between %s in P%d and %s in P%d\n",
                                                                                	commTmp2->target_addr, i, code1, i, code2, j);
									free(code1);
									free(code2);
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
	clock_t begin = clock();
	int size, i, index, eventCode, post, start, send, dest;
	char fileName[25];
	char *buffer, *tmpBuffer, *tmpStr;

	char *pscw;
	/* 
		P: POST 
		S: START
		C: COMPLETE
		W: WAIT 
		A: SEND
		R: RECEIVE
		N: NORMAL
	*/

	bool *barrier;
	IntList *aIntListPost, *aIntListStart, *aIntListComplete, *aIntListWait, *aIntListBarrier;
	EVC *EVCList;

	//unsigned long long int count = 0;
	
	/* DEBUGGING ZONE */
	FILE *fp = fopen("./out", "w");
	if (fp == NULL)
        {
        	perror("Error opening file");
	}
	/**/

	post = -1;
	start = -1;
	send = -1;
	dest = -1;
	size = atoi(argv[1]);
	pscw = (char *) malloc(size * sizeof(char));
	barrier = (bool *) malloc(size * sizeof(bool));
	FILE **pFile = (FILE **) malloc(size * sizeof(FILE *));
	List **aList = (List **) malloc(size * sizeof(List *));
	EVCList = (EVC *) malloc(size * sizeof(EVC));
	for (i = 0; i < size; i++)
	{	
		pscw[i] = 'N';
		barrier[i] = false;
		sprintf(fileName, "./traces/trace%d", i);
		pFile[i] = fopen(fileName, "r");
		if (pFile[i] == NULL)
		{
			perror("Error opening file");
		}

		if (i == 0)
		{
			mpz_init_set_str(EVCList[i].clockBase, "2", BASE);
		}
		else //if (i > 0)
		{
			mpz_init(EVCList[i].clockBase);
			mpz_nextprime(EVCList[i].clockBase, EVCList[i-1].clockBase);
			mpz_init_set(EVCList[i].clock, EVCList[i].clockBase);
		}

		aList[i] = initList(NULL, size);
	}

	index = 0;
	while (index < size)
	{
		/* DEBUGGING ZONE */
		//printf("CP1: index=%d pscw=%c post=%d start=%d\n", index, pscw[index], post, start);
		/**/

		if (pscw[index] == 'N')
		{	
			buffer = (char *) malloc(BUFFER_SIZE * sizeof(char));
			if (fgets(buffer, BUFFER_SIZE, pFile[index]) != NULL)
			{              

				/* DEBUGGING ZONE */
				//printf("CP2: %s", buffer);
				//count++;
				//printf("%d\n", count);
				//getchar();
				/**/

				eventCode = getEventCode(buffer);
				if (eventCode == CREATE)
				{	
                                        tmpBuffer = buffer;
                                        tmpStr = getData(&tmpBuffer);
                                        free(tmpStr);

                                        tmpStr = getData(&tmpBuffer);
                                        aList[index] = initList(tmpStr, size);
				}
				else if (eventCode == FENCE)
				{

					/* DEBUGGING ZONE */
                                        //for (i = 0; i < size; i++)
                                        //{
                                        //        printf("%d\t", barrier[i]);
                                        //}
                                        //printf("\n");
                                        //getchar();
					/**/
				
					barrier[index] = true;
					for (i = 0; i < size; i++)
					{
						if (i != index && barrier[i] != true)
						{
							index = i;
							break;
						}
					}
						
					if (i == size)
					{
						/* detect MCE across processes */

						/* DEBUGGING ZONE */
						fprintf(fp, "\nSfence\n");
						printAllList(fp, aList, size);
						/**/
			
						int tmpMem = getMemory();
						memUsage = (tmpMem > memUsage) ? tmpMem : memUsage;
						detectMCEAcrossProc(aList, size);
						freeAllList(aList, size);

						for (i = 0; i < size; i++)
                                                {
                                                        mpz_set(EVCList[i].clock, EVCList[i].clockBase);

							Node *aNode = initNode(EVCList[i].clock);
							insertList(aList[i], aNode);
							Chai *aChain = initChain(i);	
		                                        readEventWithinEpoch(pFile, i, EVCList, size, aList, aChain, FENCE);

		                                        /* detect MCE within an epoch */
		                                        //printChain(aChain);
		                                        detectMCEInProc(aChain);
						}
			
						/* detect MCE across processes */

						/* DEBUGGING ZONE */
						fprintf(fp, "\nEfence\n");
						printAllList(fp, aList, size);
						/**/

						tmpMem = getMemory();
						memUsage = (tmpMem > memUsage) ? tmpMem : memUsage;
						detectMCEAcrossProc(aList, size);
						freeAllList(aList, size);
				
						for (i = 0; i < size; i++)
                                                {
                                                        mpz_set(EVCList[i].clock, EVCList[i].clockBase);
							barrier[i] = false;
						}
						index = 0;
					}					
				}
				else if (eventCode == POST)
				{
		                	tmpBuffer = buffer;
		                        tmpStr = getData(&tmpBuffer);
		                        free(tmpStr);
		                        aIntListPost = initIntList();
					aIntListWait = initIntList();
		                        while (tmpBuffer != NULL)
		                        {
		                                tmpStr = getData(&tmpBuffer);
		                                Int *aInt = initInt(atoi(tmpStr));
		                                insertIntList(aIntListPost, aInt);
		                                free(tmpStr);
					}

					pscw[index] = 'P';
				}
				else if (eventCode == START)
				{
                                	tmpBuffer = buffer;
                                	tmpStr = getData(&tmpBuffer);
                                        free(tmpStr);
					if (tmpBuffer == NULL)
					{
						fgets(buffer, BUFFER_SIZE, pFile[index]);
					}
					else //if (tmpBuffer != NULL)
					{
						aIntListStart = initIntList();
						aIntListComplete = initIntList();
                                        	while (tmpBuffer != NULL)
                                        	{
                                        		tmpStr = getData(&tmpBuffer);
                                                	Int *aInt = initInt(atoi(tmpStr));
                                                	insertIntList(aIntListStart, aInt);
							free(tmpStr);
                                        	}

						pscw[index] = 'S';
					}
				}
				else if (eventCode == LOCK)
				{
					mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);
					Node *aNode = initNode(EVCList[index].clock);
					insertList(aList[index], aNode);
					Chai *aChain = initChain(index);
			                readEventWithinEpoch(pFile, index, EVCList, size, aList, aChain, UNLOCK);

			                /* detect MCE within an epoch */
			                detectMCEInProc(aChain);
				}
				else if (eventCode == BARRIER)
				{
					
					/* DEBUGGING ZONE */
	                                //for (i = 0; i < size; i++)
                                        //{
                                        //        printf("%d\t", barrier[i]);
                                        //}
                                        //printf("\n");
					//getchar();
					/**/

					tmpBuffer = buffer;
                                        tmpStr = getData(&tmpBuffer);
                                        free(tmpStr);
					if (tmpBuffer == NULL)	//Case: currentComm == MPI_COMM_WORLD
					{
						barrier[index] = true;
						for (i = 0; i < size; i++)
						{
							if (i != index && barrier[i] == false)
							{
								index = i;
								break;
							}
						}
						
						if (i == size)
						{
							/* detect MCE across processes */
	
							/* DEBUGGING ZONE */
							fprintf(fp, "\nBARRIER\n");
							printAllList(fp, aList, size);
							/**/

							int tmpMem = getMemory();
							memUsage = (tmpMem > memUsage) ? tmpMem : memUsage;
							detectMCEAcrossProc(aList, size);
							freeAllList(aList, size);
				
							for (i = 0; i < size; i++)
							{
                						mpz_set(EVCList[i].clock, EVCList[i].clockBase);
								barrier[i] = false;
							}
							index = 0;
						}
						else { /* do nothing */ }
					}
					else 
					{
						Int *tmpInt;
						aIntListBarrier = initIntList();
                                        	while (tmpBuffer != NULL)
                                        	{
                                                	tmpStr = getData(&tmpBuffer);
							i = atoi(tmpStr);
							if (i != index)
							{
                                                		Int *aInt = initInt(i);
                                                		insertIntList(aIntListBarrier, aInt);
							}
                                                	free(tmpStr);
                                        	}

						tmpInt = aIntListBarrier->head;
						while (tmpInt != NULL)
						{
							fgets(buffer, BUFFER_SIZE, pFile[tmpInt->num]);
							mpz_lcm(EVCList[index].clock, EVCList[index].clock, EVCList[tmpInt->num].clock);
							tmpInt = tmpInt->next;
						}
						tmpInt = removeIntList(aIntListBarrier);
						while (tmpInt != NULL)
						{
							mpz_set(EVCList[tmpInt->num].clock, EVCList[index].clock);
							free(tmpInt);
							tmpInt = removeIntList(aIntListBarrier);
						}
					}
				}
				else if (eventCode == SEND)
				{
					if (dest < 0)
					{
                                		tmpBuffer = buffer;
                                        	tmpStr = getData(&tmpBuffer);
                                        	free(tmpStr);
						tmpStr = getData(&tmpBuffer);
						dest = atoi(tmpStr);
						free(tmpStr);

						pscw[index] = 'A';
						send = index;
						index = dest;
					}
					else //if (dest >= 0)
					{
                        			mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);
                        			Node *aNode = initNode(EVCList[index].clock);
                        			insertList(aList[index], aNode);
                        			Chai *aChain = initChain(index);
                        			//readEventWithinEpoch(pFile, index, EVCList, size, aList, aChain, COMPLETE);

                        			pscw[index] = 'N';				
						index = dest;		
					}
				}
				else if (eventCode == RECV)
				{
					if (send < 0)
					{
						tmpBuffer = buffer;
                                                tmpStr = getData(&tmpBuffer);
                                                free(tmpStr);
                                                tmpStr = getData(&tmpBuffer);
                                                send = atoi(tmpStr);
                                                free(tmpStr);

						mpz_lcm(EVCList[index].clock, EVCList[index].clock, EVCList[send].clock);
                                                mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);

                                                Node *aNode = initNode(EVCList[index].clock);
                                                insertList(aList[index], aNode);
                                                //Chai *aChain = initChain(index);
                                                //readEventWithinEpoch(pFile, index, EVCList, size, aList, aChain, COMPLETE);

						pscw[index] = 'R';
						dest = index;
						index = send;
					}
					else //if (send >= 0)
					{
						mpz_lcm(EVCList[index].clock, EVCList[index].clock, EVCList[send].clock);
			                        mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);

                        			Node *aNode = initNode(EVCList[index].clock);
                        			insertList(aList[index], aNode);
						//Chai *aChain = initChain(index);
                        			//readEventWithinEpoch(pFile, index, EVCList, size, aList, aChain, COMPLETE);

						index = send;
					}
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
		else if (pscw[index] == 'P')
		{
			/* DEBUGGING ZONE */
                        //printf("POST: index=%d post=%d start=%d\n", index, post, start);
			/**/

			if (start < 0)
			{
                		Int *aInt = removeIntList(aIntListPost);
                		if (aInt != NULL)
                		{
                                	insertIntList(aIntListWait, aInt);
					post = index;
					index = aInt->num;
                		}
				else //if (aInt == NULL)
				{
					mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);
                			Node *aNode = initNode(EVCList[index].clock);
                			insertList(aList[index], aNode);
                			Chai *aChain = initChain(index);
                			readEventWithinEpoch(pFile, index, EVCList, size, aList, aChain, WAIT);

                			/* detect MCE within an epoch */
					detectMCEInProc(aChain);

					pscw[index] = 'W';
					post = -1;
				}
			}
			else //if (start >= 0)
			{
				removeAnIntList(aIntListPost, start);
				if (isIntListEmpty(aIntListPost) == false) {/* do nothing */} 
				else //if (isIntListEmpty(aIntListPost) == true)
				{
                                        mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);
                                        Node *aNode = initNode(EVCList[index].clock);
                                        insertList(aList[index], aNode);
                                        Chai *aChain = initChain(index);
                                        readEventWithinEpoch(pFile, index, EVCList, size, aList, aChain, WAIT);

                                        /* detect MCE within an epoch */
                                        detectMCEInProc(aChain);

                                        pscw[index] = 'W';
				}
				index = start;
			}
		}
		else if (pscw[index] == 'S')
		{
			/* DEBUGGING ZONE */
			//printf("START: index=%d post=%d start=%d\n", index, post, start);
			/**/

			if (post < 0)
			{
				Int *aInt = removeIntList(aIntListStart);
				if (aInt == NULL)
				{
                                        mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);
                                        Node *aNode = initNode(EVCList[index].clock);
                                        insertList(aList[index], aNode);
                                        Chai *aChain = initChain(index);
                                        readEventWithinEpoch(pFile, index, EVCList, size, aList, aChain, COMPLETE);

                                        /* detect MCE within an epoch */
                                        detectMCEInProc(aChain);

                                        pscw[index] = 'C';
					start = -1;
				}
				else //if (aInt != NULL)
				{
                                        insertIntList(aIntListComplete, aInt);
                                	mpz_lcm(EVCList[index].clock, EVCList[index].clock, EVCList[aInt->num].clock);

					start = index;
					index = aInt->num;
				}
			}
			else //if (post >= 0)
			{
				mpz_lcm(EVCList[index].clock, EVCList[index].clock, EVCList[post].clock);
				removeAnIntList(aIntListStart, post);
				if (isIntListEmpty(aIntListStart) == true)
				{
                        		mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);
                        		Node *aNode = initNode(EVCList[index].clock);
                        		insertList(aList[index], aNode);
                        		Chai *aChain = initChain(index);
                        		readEventWithinEpoch(pFile, index, EVCList, size, aList, aChain, COMPLETE);

                        		/* detect MCE within an epoch */
                        		detectMCEInProc(aChain);

					pscw[index] = 'C';
					index = post;
				}
				else 
				{
					pscw[index] = 'S';
					start = -1;
					index = post;
				}
			}
		}
		else if (pscw[index] == 'C')
		{
			if (post < 0)
			{
				Int *aInt = removeIntList(aIntListComplete);
				if (aInt != NULL)
				{	start = index;
					index = aInt->num;
					freeInt(aInt);
				}
				else //if (aInt == NULL)
				{
                                        mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);
                                        Node *aNode = initNode(EVCList[index].clock);
                                        insertList(aList[index], aNode);

					pscw[index] = 'N';
					start = -1;	
				}
			}
			else //if (post >= 0)
			{
				removeAnIntList(aIntListComplete, post);
				if (isIntListEmpty(aIntListComplete) == true)
				{
                                	mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);
                                        Node *aNode = initNode(EVCList[index].clock);
                                        insertList(aList[index], aNode);
					
					pscw[index] = 'N';
				}
				else {/* do nothing */}
				index = post;
			}
		}
		else if (pscw[index] == 'W')
		{
			if (start < 0)
			{
                                Int *aInt = removeIntList(aIntListWait);
                                if (aInt == NULL)
                                {
                                        mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);
                                        Node *aNode = initNode(EVCList[index].clock);
                                        insertList(aList[index], aNode);

					pscw[index] = 'N';	
					post = -1;	
				}
				else //if (aInt != NULL)
				{
                                	mpz_lcm(EVCList[index].clock, EVCList[index].clock, EVCList[aInt->num].clock);
                                 
					post = index;
					index = aInt->num;
					freeInt(aInt);
				}
			}
			else //if (start >= 0)
			{
				removeAnIntList(aIntListWait, start);
				mpz_lcm(EVCList[index].clock, EVCList[index].clock, EVCList[start].clock);
				if (isIntListEmpty(aIntListWait) == true)		
				{
                                        mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);
                                        Node *aNode = initNode(EVCList[index].clock);
                                        insertList(aList[index], aNode);
					
					pscw[index] = 'N';
				}
				index = start;
			}
		}
		else if (pscw[index] == 'A')
		{
                       	mpz_mul(EVCList[index].clock, EVCList[index].clock, EVCList[index].clockBase);
                        Node *aNode = initNode(EVCList[index].clock);
                        insertList(aList[index], aNode);
			//Chain *aChain = initChain(index);
                        //readEventWithinEpoch(pFile, index, EVCList, size, aList, aChain, COMPLETE);

			pscw[index] = 'N';
			send = -1;
			dest = -1;
		}
		else if (pscw[index] == 'R')
		{
			pscw[index] = 'N';
			send = -1;
			dest = -1;
		}
		else
		{
			//do nothing
		}
	}

	int tmpMem = getMemory();
	memUsage = (tmpMem > memUsage) ? tmpMem : memUsage;
	for (i = 0; i < size; i++)
	{
		fclose(pFile[i]);
		mpz_clear(EVCList[i].clock);
		mpz_clear(EVCList[i].clockBase);
	}
	free(pFile);
	free(EVCList);
	free(pscw);
	free(barrier);

	//End Of File
	//detect MCE across processes

	/* DEBUGGING ZONE */
	fprintf(fp, "\nEOF\n");
	printAllList(fp, aList, size);
	fclose(fp);
	/**/

	detectMCEAcrossProc(aList, size);
	freeAllList(aList, size);
	free(aList);
	printf("Memory Usage: %dkB.\n", memUsage);
	clock_t end = clock();
	double elapsedTime = (double) (end - begin) / CLOCKS_PER_SEC;
	printf("Execution Time: %fs.\n", elapsedTime);

	return 0;
}
