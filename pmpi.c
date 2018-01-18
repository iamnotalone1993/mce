#include "pmpi.h"
void printClock(int a[]){
	int size = sizeof(a)/sizeof(a[0]);
	fprintf(fp,"[");
	for (int i = 0; i < size -1; i++){
		fprintf(fp,"%d,",a[i]);
	}
	fprintf(fp,"%d]",a[size-1]);
}
int MPI_Init( int *argc, char ***argv ){
	PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
	char srank[50];
	sprintf(srank, "%d", rank);
	strcat(fname,srank);
	fp = fopen(fname,r+);
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
	}
	else {
		clock[rank] = clock[rank] + 1;
		printClock(clock);
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
	}
	else {
		clock[rank] = clock[rank] + 1;
		printClock(clock);
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
	}
	else {
		clock[rank] = clock[rank] + 1;
		printClock(clock);
	}
	return PMPI_Accumulate(origin_addr, origin_count, origin_datatype,target_rank,
                   target_disp, target_count, target_datatype, op, win);
}
int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, 
                  MPI_Comm comm, MPI_Win *win){
				  }
int MPI_Win_fence(int assert, MPI_Win win){
					clock[rank] = clock[rank] + 1;
					printClock(clock);
				  }
int MPI_Win_post(MPI_Group group,int assert,MPI_Win win){
					clock[rank] = clock[rank] + 1;
					printClock(clock);
				  }
int MPI_Win_start(MPI_Group group,int assert,MPI_Win win){
					clock[rank] = clock[rank] + 1;
					printClock(clock);
				  }
int MPI_Win_complete(MPI_Win win){
					clock[rank] = clock[rank] + 1;
					printClock(clock);
				  }
int MPI_Win_wait(MPI_Win win){
					clock[rank] = clock[rank] + 1;
					printClock(clock);
				  }
int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win){
					clock[rank] = clock[rank] + 1;
					printClock(clock);
				  }
int MPI_Win_lock_all(int assert, MPI_Win win){
					clock[rank] = clock[rank] + 1;
					printClock(clock);
				  }
int MPI_Win_unlock(int rank, MPI_Win win){
					clock[rank] = clock[rank] + 1;
					printClock(clock);
				  }
int MPI_Win_unlock_all(MPI_Win win){
					clock[rank] = clock[rank] + 1;
					printClock(clock);
				  }
