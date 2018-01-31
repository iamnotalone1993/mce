#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc == 3)
	{
		printf("%s\n", argv[0]);
		printf("%s\n", argv[1]);
		printf("%s\n", argv[2]);
	}
	else 
	{
		//do nothing
	}
	return 0;
}
