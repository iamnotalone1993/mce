#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE *pFile;
	pFile = fopen("trace2", "w");
	if (pFile == NULL)
		printf("NULL\n");
	return 0;
}
