#include "profiler.h"

void printClock(int *a)
{
	int size, i;

	PMPI_Comm_size(MPI_COMM_WORLD, &size);
	fprintf(fp, "[");
	for (i = 0; i < size - 1; i++)
	{
		fprintf(fp, "%d|", a[i]);
	}
	fprintf(fp, "%d]\n", a[size-1]);
}

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
	int result, rank, size, i;
	char fileName[15];

	result = PMPI_Init(argc, argv);
	execTime = PMPI_Wtime();

	PMPI_Comm_size(MPI_COMM_WORLD, &size);
        PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
	sprintf(fileName, "trace%d", rank);
	fp = fopen(fileName, "w");
	clock = (int *) malloc(size * sizeof(int));
	for (i = 0; i < size; i++) 
	{
		clock[i] = 0;
	}
	//fprintf(fp, "Init\t");
	//printClock(clock);

	return result;
}

int MPI_Finalize()
{
	int result, rank;
	double maxTime;

	PMPI_Comm_rank(MPI_COMM_WORLD, &rank);

	execTime = MPI_Wtime() - execTime;
	PMPI_Reduce(&execTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	if (rank == 0)
	{
		printf("Execution Time = %f seconds.\n", maxTime);
	}
	result = PMPI_Finalize();

	fclose(fp);
	free(clock);

	return result;
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win) //TODO
{
	int result;

	result = PMPI_Win_create(base, size, disp_unit, info, comm, win);

	fprintf(fp, "Create\t%p\t%d\t%d\n", base, (int) size, disp_unit);

	return result;
}

int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank,
				MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) //TODO
{	
	int result;

	result = PMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, 
                                target_datatype, win);

	fprintf(fp, "Put\t%p\t%d\t%d\t%d\t%d\n", origin_addr, origin_count, target_rank, (int) target_disp, target_count);

	return result;
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, 
			MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win) //TODO
{
	int result;

	result = PMPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, 
                                target_datatype, win);

	fprintf(fp, "Get\t%p\t%d\t%d\t%d\t%d\n", origin_addr, origin_count, target_rank, (int) target_disp, target_count);

	return result;
}

int MPI_Accumulate(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, 
					MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win) //TODO
{
	int result;

	result = PMPI_Accumulate(origin_addr, origin_count, origin_datatype,target_rank, target_disp, target_count,
                                        target_datatype, op, win);

	fprintf(fp, "Accumulate\t%p\t%d\t%d\t%d\t%d\n", origin_addr, origin_count, target_rank, (int) target_disp, target_count);

	return result;
}

int MPI_Win_fence(int assert, MPI_Win win)
{
	int result, rank;

	result = PMPI_Win_fence(assert, win);

	PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
	clock[rank] = clock[rank] + 1;
	PMPI_Allgather(clock + rank, 1, MPI_INT, clock, 1, MPI_INT, MPI_COMM_WORLD);
	fprintf(fp, "Fence\t");
	printClock(clock);

	return result;
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	int result, size, rank, groupSize, i, *worldRanks, *groupRanks;
	MPI_Group worldGroup;

	result = PMPI_Win_post(group, assert, win);

	PMPI_Comm_size(MPI_COMM_WORLD, &size);
	PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
	clock[rank] = clock[rank] + 1;
	//get group members and send clock from each member
	PMPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
	PMPI_Group_size(group, &groupSize);
	worldRanks = (int *) malloc(groupSize * sizeof(int));
	groupRanks = (int *) malloc(groupSize * sizeof(int));
	for (i = 0; i < groupSize; i++)
	{
		groupRanks[i] = i;
	}
	postGroup = group;
	PMPI_Group_translate_ranks(group, groupSize, groupRanks, worldGroup, worldRanks);
	for (i = 0; i < groupSize; i++)
	{
		PMPI_Send(clock, size, MPI_INT, worldRanks[i], 0, MPI_COMM_WORLD);
	}
	fprintf(fp, "Post\t");
	/*for (i = 0; i < groupSize; i++)
	{
		fprintf(fp, "\t%d",groupRanks[i]);
	}*/
	printClock(clock);
	free(worldRanks);
	free(groupRanks);

	return result;
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	int result, size, rank, groupSize, i, j, *worldRanks, *groupRanks, *tmpClock;
	MPI_Group worldGroup;

	result = PMPI_Win_start(group, assert, win);

	PMPI_Comm_size(MPI_COMM_WORLD, &size);
	PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
	clock[rank] = clock[rank] + 1;
	startGroup = group;
	//get group members and receive clock to each member
	PMPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
	PMPI_Group_size(group, &groupSize);
	worldRanks = (int *) malloc(groupSize * sizeof(int));
        groupRanks = (int *) malloc(groupSize * sizeof(int));
	for (i = 0; i < groupSize; i++)
	{
		groupRanks[i] = i;
	}
	tmpClock = (int *) malloc(size * sizeof(int));
	PMPI_Group_translate_ranks(group, groupSize, groupRanks, worldGroup, worldRanks);
	for (i = 0; i < groupSize; i++)
	{
		PMPI_Recv(tmpClock, size, MPI_INT, worldRanks[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (j = 0; j < size; j++)
		{
			if (tmpClock[j] > clock[j]) clock[j] = tmpClock[j];
		}
	}
	fprintf(fp, "Start\t");
	printClock(clock);
	/*for (i = 0; i < groupSize; i++)
	{
		fprintf(fp, "\t%d", groupRanks[i]);
	}*/
	free(worldRanks);
        free(groupRanks);
	free(tmpClock);

	return result;
}

int MPI_Win_complete(MPI_Win win)
{
	int result, size, rank, groupSize, i, *worldRanks, *groupRanks;
	MPI_Group worldGroup;
	
	result = PMPI_Win_complete(win);

	PMPI_Comm_size(MPI_COMM_WORLD, &size);
	PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
	clock[rank] = clock[rank] + 1;
	//get group members and send clock from each member
	PMPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
	PMPI_Group_size(startGroup, &groupSize);
	worldRanks = (int *) malloc(groupSize * sizeof(int));
        groupRanks = (int *) malloc(groupSize * sizeof(int));
	for (i = 0; i < groupSize; i++)
	{
		groupRanks[i] = i;
	}
	PMPI_Group_translate_ranks(startGroup, groupSize, groupRanks, worldGroup, worldRanks);
	for (i = 0; i < groupSize; i++)
	{
		PMPI_Send(clock, size, MPI_INT, worldRanks[i], 0, MPI_COMM_WORLD);
	}
	fprintf(fp, "Complete\t");
	/*for (i = 0; i < groupSize; i++)
	{
		fprintf(fp, "\t%d",groupRanks[i]);
	}*/
	printClock(clock);
	free(worldRanks);
        free(groupRanks);

	return result;
}

int MPI_Win_wait(MPI_Win win)
{
	int result, size, rank, groupSize, i, j, *worldRanks, *groupRanks, *tmpClock;
	MPI_Group worldGroup;

	result = PMPI_Win_wait(win);

	PMPI_Comm_size(MPI_COMM_WORLD, &size);
	PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
	clock[rank] = clock[rank] + 1;
	//get group members and receive clock to each member
	PMPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
	PMPI_Group_size(postGroup, &groupSize);
	worldRanks = (int *) malloc(groupSize * sizeof(int));
        groupRanks = (int *) malloc(groupSize * sizeof(int));
	for (i = 0; i < groupSize; i++)
	{
		groupRanks[i] = i;
	}
	tmpClock = (int *) malloc(size * sizeof(int));
	PMPI_Group_translate_ranks(postGroup, groupSize, groupRanks, worldGroup, worldRanks);
	for (i = 0; i < groupSize; i++)
	{
		PMPI_Recv(tmpClock, size, MPI_INT, worldRanks[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (j = 0; j < size; j++)
		{
			if (tmpClock[j] > clock[j]) clock[j] = tmpClock[j];
		}
	}
    	fprintf(fp, "Wait\t");
    	/*for (i = 0; i < groupSize; i++)
    	{
		fprintf(fp, "\t%d",groupRanks[i]);
	}*/
	printClock(clock);
	free(worldRanks);
        free(groupRanks);
	free(tmpClock);

	return result;
}

int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win)
{
	int result, pmpiRank;

	result = PMPI_Win_lock(lock_type, rank, assert, win);

	PMPI_Comm_rank(MPI_COMM_WORLD, &pmpiRank);
	clock[pmpiRank] = clock[pmpiRank] + 1;
	fprintf(fp, "Lock\t");
	printClock(clock);

	return result;
}

int MPI_Win_unlock(int rank, MPI_Win win)
{
	int result, pmpiRank;

	result = PMPI_Win_unlock(rank, win);

	PMPI_Comm_rank(MPI_COMM_WORLD, &pmpiRank);
	clock[pmpiRank] = clock[pmpiRank] + 1;
	fprintf(fp, "Unlock\t");
	printClock(clock);

	return result;
}

int MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
	int result, rank, size;

	result = PMPI_Send(buf, count, datatype, dest, tag, comm);

        PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
	PMPI_Comm_size(MPI_COMM_WORLD, &size);
        clock[rank] = clock[rank] + 1;
	PMPI_Send(clock, size, MPI_INT, dest, tag, comm);

	return result;	
}

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	int result, rank, size, *tmpClock, i;

	result = PMPI_Recv(buf, count, datatype, source, tag, comm, status);
	
	PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
	PMPI_Comm_size(MPI_COMM_WORLD, &size);
	tmpClock = (int *) malloc(size * sizeof(int));
        clock[rank] = clock[rank] + 1;
	PMPI_Recv(tmpClock, size, MPI_INT, source, tag, comm, status);
	for (i = 0; i < size; i++)
	{
		if (tmpClock[i] > clock[i])
		{
			clock[i] = tmpClock[i];
		}
	}
	free(tmpClock);

	return result;
}

int MPI_Barrier(MPI_Comm comm)
{
	int result, rank;

	result = PMPI_Barrier(comm);

        PMPI_Comm_rank(comm, &rank);
        clock[rank] = clock[rank] + 1;
        PMPI_Allgather(clock + rank, 1, MPI_INT, clock, 1, MPI_INT, comm);
        fprintf(fp, "Barrier\t");
        printClock(clock);

	return result;
}
