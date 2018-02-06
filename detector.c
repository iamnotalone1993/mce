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

char *toTokenArray(char *str){
	char *  p    = strtok (str, "\t");
	int size = 0;
	char ** res  = NULL;
	while (p) {
	res = realloc (res, sizeof (char*) * ++size);

	if (res == NULL) {
		printf("Memory allocation failed.Terminating..\n");
		exit (-1); /* memory allocation failed */
	}

	res[size-1] = p;

	p = strtok (NULL, " ");
	}
	return res;
};

int *getClock(char *str){
	char * p = strtok(str, "|");
	int *res, size = 0;
	while( p ) {
	 	res = realloc (res, sizeof (int) * ++size);

		if (res == NULL) {
			printf("Memory allocation failed.Terminating..\n");
			exit (-1); /* memory allocation failed */
		}

		res[size-1] = toInt(p);
    	p = strtok(NULL, "|");
	}
	return res;
};

void checkConflict(char *op1,int rank1, char *op2, int rank2, char **shared, bool isInside){
	char **op1_tokens = toTokenArray(op1);
	char **op2_tokens = toTokenArray(op2);
	size_t size;
	size = sizeof(op1_tokens) / sizeof(op1_tokens[0]);
	int *clock1 = getClock(op1_tokens[size-1]);
	size = sizeof(op2_tokens) / sizeof(op1_tokens[0]);
	int *clock2 = getClock(op2_tokens[size-1]);
	//RMA: op 	origin_addr		origin_count	target_rank		target_disp		target_count	clock
	//LS: op 	address		clock
	if (strstr(op1,"Put")) {
		//if 2 ops are not in the same process, then op1 is in origin and op2 is in target
		if (strstr(op2,"Store")) {
			if (isInside){
				if (strcmp(op1_tokens[1],op2_tokens[1]) == 0 && clock1[rank1] <= clock2[rank2]) { //compare address and clock
					printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
				}
			}
			else {
				//array are not considered
				if (toInt(op1_tokens[3]) == rank2) {
					printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
				}
			}
			
		}
		else if (strstr(op2,"Load")){
			if (!isInside && toInt(op1_tokens[3]) == rank2) {
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
		}
		else if (strstr(op2,"Put")) {
			if (strcmp(op1_tokens[3], op2_tokens[3]) == 0 && strcmp(op1_tokens[4], op2_tokens[4]) == 0) { //rank == rank && disp == disp -> same address
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
		}
		else if (strstr(op2,"Get")) {
			if (isInside && strcmp(op1_tokens[1], op2_tokens[1]) == 0) {
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
			if (strcmp(op1_tokens[3], op2_tokens[3]) == 0 && strcmp(op1_tokens[4], op2_tokens[4]) == 0) { //put to a, get from a (a = address)
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
		}
		else { //Accumulate
			if (strcmp(op1_tokens[3], op2_tokens[3]) == 0 && strcmp(op1_tokens[4], op2_tokens[4]) == 0) { //rank == rank && disp == disp -> same address
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
		}
		return;

	}

	if (strstr(op1,"Get")){
		if (strstr(op2,"Store")) {
			if (isInside){
				if (strcmp(op1_tokens[1],op2_tokens[1]) == 0 && clock1[rank1] <= clock2[rank2]) { //compare address and clock
					printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
				}
			}
			else {
				//array are not considered
				if (toInt(op1_tokens[3]) == rank2) {
					printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
				}
			}
			
		}
		else if (strstr(op2,"Load")){
			if (isInside && strcmp(op1_tokens[1],op2_tokens[1]) == 0) { //inside process and same address
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
		}
		else if (strstr(op2,"Put")) {
			if (isInside && strcmp(op1_tokens[1], op2_tokens[1]) == 0) { //get/put same origin address
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
			if (strcmp(op1_tokens[3], op2_tokens[3]) == 0 && strcmp(op1_tokens[4], op2_tokens[4]) == 0) { //rank == rank && disp == disp -> same address, get/put from/to same address
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
		}
		else if (strstr(op2,"Get")) {
			if (isInside && strcmp(op1_tokens[1], op2_tokens[1]) == 0) { //get/put same origin address
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
		}
		else { //Accumulate
			if (isInside && strcmp(op1_tokens[1], op2_tokens[1]) == 0) { //get/put same origin address
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
			if (strcmp(op1_tokens[3], op2_tokens[3]) == 0 && strcmp(op1_tokens[4], op2_tokens[4]) == 0) { //rank == rank && disp == disp -> same address, get/put from/to same address
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
		}
		return;
	}

	if (strstr(op1,"Accumulate")){
		if (strstr(op2,"Store")) {
			if (isInside){
				if (strcmp(op1_tokens[1],op2_tokens[1]) == 0 && clock1[rank1] <= clock2[rank2]) { //compare address and clock
					printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
				}
			}
			else {
				//array are not considered
				if (toInt(op1_tokens[3]) == rank2) {
					printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
				}
			}
			
		}
		else if (strstr(op2,"Load")){
			if (!isInside && toInt(op1_tokens[3]) == rank2) {
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
		}
		else if (strstr(op2,"Put")) {
			if (strcmp(op1_tokens[3], op2_tokens[3]) == 0 && strcmp(op1_tokens[4], op2_tokens[4]) == 0) { //rank == rank && disp == disp -> same address
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
		}
		else if (strstr(op2,"Get")) {
			if (isInside && strcmp(op1_tokens[1], op2_tokens[1]) == 0) {
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
			if (strcmp(op1_tokens[3], op2_tokens[3]) == 0 && strcmp(op1_tokens[4], op2_tokens[4]) == 0) { //put to a, get from a (a = address)
				printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			}
		}
		else { //Accumulate
			//if (strcmp(op1_tokens[3], op2_tokens[3]) == 0 && strcmp(op1_tokens[4], op2_tokens[4]) == 0) { //rank == rank && disp == disp -> same address
				//printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
			//}
		}
		return;
	}

	if (strstr(op1,"Load")){
		if ((strstr(op2,"Put") || strstr(op2,"Accumulate")) && !isInside && toInt(op2_tokens[3]) == rank1) {	
			printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
		}
		return;
	}

	if (strstr(op1,"Store")){
		if ((strstr(op2,"Put") || strstr(op2,"Accumulate") || strstr(op2,"Get")) && !isInside && toInt(op2_tokens[3]) == rank1) {
			printf("These ops are conflict:\n1: %s\n2: %s\n", op1, op2);
		}
		return;
	}
};


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
				if (!(strstr(lines[i],"Store") || strstr(lines[i],"Load")){
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
						if (strstr(tmpline[i],"Fence")) break;
						else {
							//check conflict inside and report
							if (strstr(lines[i],"Put")) {
								if (strstr(tmpline,"Store")) {
									//generate 2 arrays of token
									char **put_arr = toTokenArray(lines[i]);
									
									char **store_arr = toTokenArray(tmpline);
									if (strcmp(put_arr[1],store_arr[1]) == 0) { //same address
										//report
									}
								}
								else if (strstr(tmpline,"Put")) {
									//suppose generated 2 arrays of token
									if (put_arr1[3] == put_arr2[3] && put_arr1[4] == put_arr2[4]) { //rank == rank && disp == disp -> same address
										//report
									}
								}
								else if (strstr(tmpline,"Get")) {
									//suppose generated 2 arrays of token
									if (put_arr[1] == get_arr[1]) {
										//report
									}
									if (put_arr[3] == get_arr[3] && put_arr[4] == get_arr[4]) { //put to a, get from a (a = address)
										//report
									}
								}
								else { //Accumulate
									//similar to put
								}

							}
							else if (strstr(lines[i],"Get")){

							}
							else {//Accumulate

							}
						}
					}
					fclose(tmpfile);
				}
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
