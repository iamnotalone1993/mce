#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int toInt(char a[]) {
  int c, sign, offset, n;
 
  if (a[0] == '-') {  // Handle negative integers
    sign = -1;
  }
 
  if (sign == -1) {  // Set starting position to convert
    offset = 1;
  }
  else {
    offset = 0;
  }
 
  n = 0;
 
  for (c = offset; a[c] != '\0'; c++) {
    n = n * 10 + a[c] - '0';
  }
 
  if (sign == -1) {
    n = -n;
  }
 
  return n;
}

int main( int argc, char *argv[] )  {
	int numfile;
	int i;
	FILE **fps;
	//open trace files
	if( argc == 2 ) {
		printf("The number of trace files is %s\n", argv[1]);
		numfile = toInt(argv[1]); 
		fps = malloc(sizeof(FILE*) * numfile);
		int i = 0;
		char fname[20],num[10];
		for (i = 0; i < numfile; i++){
			strcpy(fname,  "trace_");
			sprintf(num, "%d", i);
			strcat(fname,num);
			fps[i] = fopen(fname,"r");
	  }
	}
	else if( argc > 2 ) {
		printf("Too many arguments supplied.\n");
		return 1;
	}
	else {
		printf("Number of trace files expected.\n");
		return 1;
	}
	//read first lines for later processing
	char **lines;
	lines = malloc(sizeof(char*) * numfile);
	for (i = 0; i < numfile; i++){
		size_t len = 0;
		ssize_t read;
		read = getline(&lines[i], &len, fp);
	}
	
	//code for detecting errors in fence epoch
	for (i = 0;i < numfile; i++){
		size_t len = 0;
		ssize_t read;
		
		while (read = getline(&lines[i], &len, fps[i]) != -1){
			if (strstr(lines[i],"Fence")) break;
			else{
				//check inside process
				char *tmpline;
				char fname[20],num[10];
				strcpy(fname,  "trace_");
				sprintf(num, "%d", i);
				strcat(fname,num);
				FILE *tmpfile = fopen(fname,"r");
				//find position of lines[i]
				while (read = getline(&tmpline, &len, tmpfile) != -1){
					if (strcmp(lines[i],tmpline) == 0) break;
				}
				//check whether lines[i] and tmpline are conflict
				while (read = getline(&tmpline, &len, tmpfile) != -1){
					if (strstr(lines[i],"Fence")) break;
					else {
						//check conflict and report
					}
				}
				fclose(tmpfile);
				//check other process events
				int j;
				for (j = i + 1; j < numfile; j++){
					char *tmpline;
					char fname[20],num[10];
					strcpy(fname,  "trace_");
					sprintf(num, "%d", j);
					strcat(fname,num);
					FILE *tmpfile = fopen(fname,"r");
					
					//find position of lines[j]
					while (read = getline(&tmpline, &len, tmpfile) != -1){
						if (strcmp(lines[j],tmpline) == 0) break;
					}
					//check whether lines[i] and tmpline are conflict
					while (read = getline(&tmpline, &len, tmpfile) != -1){
						if (strstr(lines[j],"Fence")) break;
						else {
							//check conflict and report
						}
					}
					fclose(tmpfile);
				}
			}
		}
	}
	return 0;
}