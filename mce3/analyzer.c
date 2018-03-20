#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct 
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

int main(int argc, char **argv)
{
	if (argc == 3)
	{
		FILE *pFileIn, *pFileOut;
		char *buf, *tmp, *extraCode;
		int i;
		bool flag;
		list *head;
		flag = true;
		head = NULL;
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
				if (flag == true)
				{
					tmp = strstr(buf, "MPI_Win_create");
					fputs(buf, pFileOut);
					if (tmp != NULL)
					{
						for (i = 0; i < strlen(tmp + 16); ++i)
						{
							if (tmp[16 + i] == ',')
							{
								head = (list *) malloc(sizeof(list));
								head->varName = (char *) malloc(i + 1);
								head->next = NULL;
								memcpy(head->varName, tmp + 16, i);
								head->varName[i] = '\0';
								break;
							}
						}
						flag = false;
					}
				}
				else //if (flag == false)
				{
					fputs(buf, pFileOut);
					tmp = strstr(buf, head->varName);
					while (tmp != NULL)
					{
						if (tmp == buf)
						{
							if (charInVarName((tmp + strlen(head->varName))[0]) == false)
							{
								extraCode = (char *) malloc(20 + strlen(head->varName));
                                                		sprintf(extraCode, "tracels(false, &%s);\n", head->varName);
                                                		fputs(extraCode, pFileOut);
                                                		free(extraCode);
								break;
							}
						}
						else //if (tmp != buf)
						{
							if (charInVarName((tmp - 1)[0]) == false && 
									charInVarName((tmp + strlen(head->varName))[0]) == false)
							{
								extraCode = (char *) malloc(20 + strlen(head->varName));
                                                		sprintf(extraCode, "tracels(false, &%s);\n", head->varName);
                                                		fputs(extraCode, pFileOut);
                                                		free(extraCode);
								break;
							}	 
						}
						tmp = strstr(tmp + 1, head->varName);
					}
				}
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
