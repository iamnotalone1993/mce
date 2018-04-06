#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct list
{
	char *varName;
	struct list *next;
} list;

bool charInVarName(char c)
{
        if (c == '_')  
                return true; 
        if (c >= '0' && c <= '9')
                return true; 
        if (c >= 'a' && c <= 'z')
                return true; 
        if (c >= 'A' && c <= 'Z')
                return true;
        return false;
}

void insertList(char *tmp, int num, list **head)
{
	list *nodeTmp;
	int i, begin;
	char *nameTmp;
	
	for (i = 0; i < strlen(tmp + num); ++i)
	{
		if (charInVarName(tmp[i + num]) == true)
		{
			begin = i;
			break;
		}
	}
	for (; i < strlen(tmp + num); ++i)
	{
		if (charInVarName(tmp[i + num]) == false)
		{
			break;
		}
	}
	i -= begin;
	begin += num;
	nameTmp = (char *) malloc((i + 1) * sizeof(char));
	memcpy(nameTmp, tmp + begin, i);
	nameTmp[i] = '\0';

	nodeTmp = *head;
	if (nodeTmp != NULL)
	{
		while (nodeTmp != NULL)
		{
			if (strcmp(nodeTmp->varName, nameTmp) == 0)
			{
				break;
			}
			nodeTmp = nodeTmp->next;
		}
	}

	if (nodeTmp == NULL)
	{
		nodeTmp = (list *) malloc(sizeof(list));
		nodeTmp->varName = nameTmp;
		nodeTmp->next = *head;
		*head = nodeTmp;
       	}
	else //if (nodeTmp == NULL)
	{
		free(nameTmp);
	}
}

void printList(list *head)
{
	printf("Printing a list...\n");
	list *tmp = head;
	while (tmp != NULL)
	{
		printf("%s\n", tmp->varName);
		tmp = tmp->next;
	}
	printf("Ending.\n");
}

char *fgets2(char *dst, int max, FILE *fp)
{
	int c;
	char *p;
	
	/* get max bytes or upto new line */
	for (p = dst, max--; max > 0; max--)
	{
		if ((c = fgetc(fp)) == EOF)
			break;
		*p++ = c;
		if (c == ';')
			break;
	}
	*p = 0;
	if (p == dst || c == EOF)
		return NULL;
	return p;
}

void processRelevantVar(char *buf, list **head, FILE **pFileOut)
{
	char *tmp, *extraCode;
	int i;

	if ((tmp = strstr(buf, "MPI_Win_create")) != NULL)
	{
		printf("CP1\n");
		insertList(tmp, 14, head);
		//printList(*head);
	}
	else if ((tmp = strstr(buf, "MPI_Put")) != NULL)
	{
		printf("CP2\n");
		insertList(tmp, 7, head);
		//printList(*head);
	}
	else if ((tmp = strstr(buf, "MPI_Get")) != NULL)
	{
		printf("CP3\n");
		insertList(tmp, 7, head);
		//printList(*head);
	}
	else if ((tmp = strstr(buf, "MPI_Accumulate")) != NULL)
	{
		printf("CP4\n");
		insertList(tmp, 14, head);
		//printList(*head);
	}
	else
	{
		printf("CP5\n");
		list *varTmp = *head;
		while (varTmp != NULL)
		{
			tmp = strstr(buf, varTmp->varName);
			if (tmp != NULL)
			{
                        	if (tmp == buf)
                                {
                                	if (charInVarName((tmp + strlen(varTmp->varName))[0]) == false)
                                        {
                                        	extraCode = (char *) malloc(20 + strlen(varTmp->varName));
                                                sprintf(extraCode, "tracels(false, &%s);\n", varTmp->varName);
                                                fputs(extraCode, *pFileOut);
                                                free(extraCode);
						break;
                                        }
                                }
                                else //if (tmp != buf)
                                {
                                        if (charInVarName((tmp - 1)[0]) == false && 
							charInVarName((tmp + strlen(varTmp->varName))[0]) == false)
                                        {
                                        	extraCode = (char *) malloc(20 + strlen(varTmp->varName));
                                                sprintf(extraCode, "tracels(false, &%s);\n", varTmp->varName);
                                                fputs(extraCode, *pFileOut);
                                                free(extraCode);
						break;
                                        }
                               	}
                                tmp = strstr(tmp + 1, varTmp->varName);
				break;
			}
			varTmp = varTmp->next;
		}
	}
}

int main(int argc, char **argv)
{
	if (argc == 3)
	{
		FILE *pFileIn, *pFileOut;
		char *buf;
		list *head = NULL;
		pFileIn = fopen(argv[1], "r");
		if (pFileIn == NULL) 
		{
			perror ("Error opening file");
		}
		else //if (pFile != NULL)
		{
			pFileOut = fopen(argv[2], "w");
			buf = (char*) malloc(100);
			while (fgets(buf, 100, pFileIn) != NULL)
			{
				printf("%s", buf);
				getchar();
				fputs(buf, pFileOut);
				processRelevantVar(buf, &head, &pFileOut);
				free(buf);
				buf = (char*) malloc(100);
			}
			free(buf);
			fclose(pFileIn);
			fclose(pFileOut);
		}
	}
	else 
	{
		//do nothing
	}
	return 0;
}
