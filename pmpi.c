#include "pmpi.h"
#define getVarName(var) #var
void printClock(int a[]){
	int size = sizeof(a)/sizeof(a[0]);
	fprintf(fp,"[");
	for (int i = 0; i < size -1; i++){
		fprintf(fp,"%d,",a[i]);
	}
	fprintf(fp,"%d]",a[size-1]);
}

void tracels(bool isLoad, void *var_add){
	if (isLoad){
		clock[pmpi_rank] = clock[pmpi_rank] + 1;
		printClock(clock);
		fprintf("/tLoad/t%p",var_add);
		lastOp = LS;
	} 
	else {
		clock[pmpi_rank] = clock[pmpi_rank] + 1;
		printClock(clock);
		fprintf("/tStore/t%p",var_add);
		lastOp = LS;
	}
}

int MPI_Init( int *argc, char ***argv ){
	PMPI_Comm_rank(MPI_COMM_WORLD, &pmpi_rank);
	char srank[50];
	sprintf(srank, "%d", pmpi_rank);
	strcat(fname,srank);
	printf("name: %s",fname);
	fp = fopen(fname,"w");
	/*
	ifp = fopen("interested.txt","r");
	int n;
	char line [ 128 ];
	fgets ( line, sizeof line, ifp );
	n = atoi(line);
	interested = malloc(n * sizeof(char*));
	for (int i =0; i< n; i++){
	   interested[i] = malloc(128);
	}
	int i = 0;
	while ( fgets ( line, sizeof line, ifp ) != NULL )
    {
		line[strcspn(line, "\r\n")] = 0;
		strcpy(interested[i],line);
		i++;
    }
	*/
	PMPI_Comm_size(MPI_COMM_WORLD, &size);
	clock = (int*) malloc(size);
	for (int i=0; i < size; i++) clock[i] = 0;
	return PMPI_Init(argc, argv);	
}
int MPI_Finalize(){
	fclose(fp);
	free(clock);
	return PMPI_Finalize();
}
int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype
            origin_datatype, int target_rank, MPI_Aint target_disp,
            int target_count, MPI_Datatype target_datatype, MPI_Win
            win){
	if (lastOp == RMA){
		printClock(clock);
		fprintf(fp,"\tPut\torigin_addr\torigin_count\ttarget_rank\ttarget_disp\ttarget_count\n");
	}
	else {
		lastOp = RMA;
		clock[pmpi_rank] = clock[pmpi_rank] + 1;
		printClock(clock);
		fprintf(fp,"\tPut\torigin_addr\torigin_count\ttarget_rank\ttarget_disp\ttarget_count\n");
	}
	
	return PMPI_Put(origin_addr, origin_count,
            origin_datatype, target_rank, target_disp,
            target_count, target_datatype, win);
}
int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype
            origin_datatype, int target_rank, MPI_Aint target_disp,
            int target_count, MPI_Datatype target_datatype, MPI_Win
            win){
	if (lastOp == RMA){
		printClock(clock);
		fprintf(fp,"\tGet\torigin_addr\torigin_count\ttarget_rank\ttarget_disp\ttarget_count\n");
	}
	else {
		lastOp = RMA;
		clock[pmpi_rank] = clock[pmpi_rank] + 1;
		printClock(clock);
		fprintf(fp,"\tGet\torigin_addr\torigin_count\ttarget_rank\ttarget_disp\ttarget_count\n");
	}
	return PMPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp,
             target_count, target_datatype, win);
}
int MPI_Accumulate(const void *origin_addr, int origin_count, MPI_Datatype
                   origin_datatype, int target_rank, MPI_Aint
                   target_disp, int target_count, MPI_Datatype
                   target_datatype, MPI_Op op, MPI_Win win){
	if (lastOp == RMA){
		printClock(clock);
		fprintf(fp,"\tAccumulate\torigin_addr\torigin_count\ttarget_rank\ttarget_disp\ttarget_count\n");
	}
	else {
		lastOp = RMA;
		clock[pmpi_rank] = clock[pmpi_rank] + 1;
		printClock(clock);
		fprintf(fp,"\tAccumulate\torigin_addr\torigin_count\ttarget_rank\ttarget_disp\ttarget_count\n");
	}
	return PMPI_Accumulate(origin_addr, origin_count, origin_datatype,target_rank,
                   target_disp, target_count, target_datatype, op, win);
}
/*int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, 
                  MPI_Comm comm, MPI_Win *win){
				  }*/
int MPI_Win_fence(int assert, MPI_Win win){
					lastOp = SYN;
					clock[pmpi_rank] = clock[pmpi_rank] + 1;
					printClock(clock);
					fprintf(fp,"\tFence");
					MPI_Allgather(&clock[pmpi_rank], 1, MPI_INT, 
                           clock, size, MPI_INT, MPI_COMM_WORLD);
					return PMPI_Win_fence(assert, win);
				  }
int MPI_Win_post(MPI_Group group,int assert,MPI_Win win){
					lastOp = SYN;
					clock[pmpi_rank] = clock[pmpi_rank] + 1;
					printClock(clock);
					fprintf(fp,"\tPost");
					//get group members and send clock from each member
					MPI_Group worldGroup;
					MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
					int groupSize;
					MPI_Group_size(group, &groupSize);
					int worldRanks[groupSize],groupRanks[groupSize];
					for (int i = 0; i < groupSize; i++){
						groupRanks[i] = i;
					}
					postGroup = group;
					MPI_Group_translate_ranks(group, groupSize, groupRanks, worldGroup, worldRanks);
					for (int i = 0; i < groupSize; i++){
						MPI_Send(clock, size, MPI_INT, worldRanks[i], 0, MPI_COMM_WORLD);
					}
					return PMPI_Win_post(group, assert, win);
				  }
int MPI_Win_start(MPI_Group group,int assert,MPI_Win win){
					lastOp = SYN;
					clock[pmpi_rank] = clock[pmpi_rank] + 1;
					printClock(clock);
					fprintf(fp,"\tStart");
					startGroup = group;
					//get group members and receive clock to each member
					MPI_Group worldGroup;
					MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
					int groupSize;
					MPI_Group_size(group, &groupSize);
					int worldRanks[groupSize],groupRanks[groupSize];
					for (int i = 0; i < groupSize; i++){
						groupRanks[i] = i;
					}
					int tmpClock[size];
					MPI_Group_translate_ranks(group, groupSize, groupRanks, worldGroup, worldRanks);
					for (int i = 0; i < groupSize; i++){
						MPI_Recv(tmpClock, size, MPI_INT, worldRanks[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						for (int j = 0; j < size; j++){
							if (tmpClock[j] > clock[j]) clock[j] = tmpClock[j];
						}
					}
					return PMPI_Win_start(win);
				  }
int MPI_Win_complete(MPI_Win win){
					lastOp = SYN;
					clock[pmpi_rank] = clock[pmpi_rank] + 1;
					printClock(clock);
					fprintf(fp,"\tComplete");
					//get group members and send clock from each member
					MPI_Group worldGroup;
					MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
					int groupSize;
					MPI_Group_size(startGroup, &groupSize);
					int worldRanks[groupSize],groupRanks[groupSize];
					for (int i = 0; i < groupSize; i++){
						groupRanks[i] = i;
					}
					MPI_Group_translate_ranks(group, groupSize, groupRanks, worldGroup, worldRanks);
					for (int i = 0; i < groupSize; i++){
						MPI_Send(clock, size, MPI_INT, worldRanks[i], 0, MPI_COMM_WORLD);
					}
					return PMPI_Win_complete(win);
				  }
int MPI_Win_wait(MPI_Win win){
					lastOp = SYN;
					clock[pmpi_rank] = clock[pmpi_rank] + 1;
					printClock(clock);
					fprintf(fp,"\tWait");
					//get group members and receive clock to each member
					MPI_Group worldGroup;
					MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
					int groupSize;
					MPI_Group_size(postGroup, &groupSize);
					int worldRanks[groupSize],groupRanks[groupSize];
					for (int i = 0; i < groupSize; i++){
						groupRanks[i] = i;
					}
					int tmpClock[size];
					MPI_Group_translate_ranks(group, groupSize, groupRanks, worldGroup, worldRanks);
					for (int i = 0; i < groupSize; i++){
						MPI_Recv(tmpClock, size, MPI_INT, worldRanks[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						for (int j = 0; j < size; j++){
							if (tmpClock[j] > clock[j]) clock[j] = tmpClock[j];
						}
					}
					return PMPI_Win_wait(win);
				  }
int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win){
					lastOp = SYN;
					clock[pmpi_rank] = clock[pmpi_rank] + 1;
					printClock(clock);
					fprintf(fp,"\tLock");
					return PMPI_Win_lock(lock_type, rank, assert, win);
				  }
int MPI_Win_lock_all(int assert, MPI_Win win){
					lastOp = SYN;
					clock[pmpi_rank] = clock[pmpi_rank] + 1;
					printClock(clock);
					fprintf(fp,"\tLockAll");
					return PMPI_Win_lock_all(assert,win);
				  }
int MPI_Win_unlock(int rank, MPI_Win win){
					lastOp = SYN;
					clock[pmpi_rank] = clock[pmpi_rank] + 1;
					printClock(clock);
					fprintf(fp,"\tUnlock");
					return PMPI_Win_unlock(win);
				  }
int MPI_Win_unlock_all(MPI_Win win){
					lastOp = SYN;
					clock[pmpi_rank] = clock[pmpi_rank] + 1;
					printClock(clock);
					fprintf(fp,"\tUnlockAll");
					return PMPI_Win_unlock_all(win);
				  }
