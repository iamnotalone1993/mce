#include "profiler.h"

void tracels(bool isLoad, void *varaddr) //TODO
{	
	if (isLoad == true)
	{
		fprintf(fp, "Load\t%p\n", varaddr);
	} 
	else //if (isLoad == false)
	{
		fprintf(fp, "Store\t%p\n", varaddr);
	}
}

int MPI_Init(int *argc, char ***argv)
{
	int rank;
	char fileName[15];

	int result = PMPI_Init(argc, argv);
	execTime = PMPI_Wtime();

        PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
	sprintf(fileName, "trace%d", rank);
	fp = fopen(fileName, "w");

	return result;
}

int MPI_Finalize()
{
	int rank;
	double maxTime;

	PMPI_Comm_rank(MPI_COMM_WORLD, &rank);

	execTime = MPI_Wtime() - execTime;
	PMPI_Reduce(&execTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	if (rank == 0)
	{
		printf("Execution Time = %f seconds.\n", maxTime);
	}
	int result = PMPI_Finalize();

	fclose(fp);

	return result;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win) //TODO
{
	int result = PMPI_Win_create(base, size, disp_unit, info, comm, win);
	fprintf(fp, "Create\t%p\t%d\t%d\n", base, (int) size, disp_unit);
	return result;
}

int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank,
				MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) //TODO
{	
	int result = PMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, 
                                target_datatype, win);
	fprintf(fp, "Put\t%p\t%d\t%d\t%d\t%d\n", origin_addr, origin_count, target_rank, (int) target_disp, target_count);
	return result;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, 
			MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) //TODO
{
	int result = PMPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, 
                                target_datatype, win);
	fprintf(fp, "Get\t%p\t%d\t%d\t%d\t%d\n", origin_addr, origin_count, target_rank, (int) target_disp, target_count);
	return result;
}

int MPI_Accumulate(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, 
					MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win) //TODO
{
	int result = PMPI_Accumulate(origin_addr, origin_count, origin_datatype,target_rank, target_disp, target_count,
                                        target_datatype, op, win);
	fprintf(fp, "Accumulate\t%p\t%d\t%d\t%d\t%d\n", origin_addr, origin_count, target_rank, (int) target_disp, target_count);
	return result;
}

int MPI_Win_fence(int assert, MPI_Win win)
{
	int result = PMPI_Win_fence(assert, win);
	fprintf(fp, "Fence\n");
	return result;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	int result = PMPI_Win_post(group, assert, win);
	fprintf(fp, "Post\n");
	return result;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	int result = PMPI_Win_start(group, assert, win);
	fprintf(fp, "Start\n");
	return result;
}

int MPI_Win_complete(MPI_Win win)
{
	int result = PMPI_Win_complete(win);
	fprintf(fp, "Complete\n");
	return result;
}

int MPI_Win_wait(MPI_Win win)
{
	int result = PMPI_Win_wait(win);
    	fprintf(fp, "Wait\n");
	return result;
}

int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win)
{
	int result = PMPI_Win_lock(lock_type, rank, assert, win);
	fprintf(fp, "Lock\n");
	return result;
}

int MPI_Win_unlock(int rank, MPI_Win win)
{
	int result = PMPI_Win_unlock(rank, win);
	fprintf(fp, "Unlock\n");
	return result;
}

int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	int result = PMPI_Send(buf, count, datatype, dest, tag, comm);
	fprintf(fp, "Send\t%d\t%d\n", dest, tag);
	return result;	
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	int result = PMPI_Recv(buf, count, datatype, source, tag, comm, status);
	fprintf(fp, "Recv\t%d\t%d\n", status->MPI_SOURCE, status->MPI_TAG);
	return result;
}

int MPI_Barrier(MPI_Comm comm)
{
	int result = PMPI_Barrier(comm);
        fprintf(fp, "Barrier\n");
	return result;
}
