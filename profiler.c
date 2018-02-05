#include "profiler.h"
#define getVarName(var) #var

void printClock(int *a)
{
	int size, i;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	//fprintf(fp, "[");
	for (i = 0; i < size - 1; i++)
	{
		fprintf(fp, "%d|", a[i]);
	}
	fprintf(fp, "%d\n", a[size-1]);
}

void tracels(bool isLoad, void *varaddr)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (lastOp != RMA)
        {
		clock[rank] = clock[rank] + 1;
        }
	else {}//if (lastOP == RMA) do nothing
	
	if (isLoad == true)
	{
		fprintf(fp, "Load\t%p\t", varaddr);
	} 
	else //if (isLoad == false)
	{
		fprintf(fp, "Store\t%p\t", varaddr);
	}
	printClock(clock);
    lastOp = LS;
}

int MPI_Init(int *argc, char ***argv)
{
	int rank, size, result, i;
	result = PMPI_Init(argc, argv);
	PMPI_Comm_size(MPI_COMM_WORLD, &size);
	PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
	char srank[50];
	sprintf(srank, "%d", rank);
	strcat(fname, srank);
	fp = fopen(fname, "w");
	PMPI_Comm_size(MPI_COMM_WORLD, &size);
	clock = (int*) malloc(size);
	for (i = 0; i < size; i++) 
	{
		clock[i] = 0;
	}
	fprintf(fp, "Init\t");
	printClock(clock);
	return result;	
}

int MPI_Finalize()
{
	fclose(fp);
	free(clock);
	return PMPI_Finalize();
}

int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win)
{
	return PMPI_Win_create(base, size, disp_unit, info, comm, win);
}

int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank,
				MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{	
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (lastOp == RMA)
	{
		fprintf(fp, "Put\t%p\t%d\t%d\t%ld\t%d\t",origin_addr,origin_count,target_rank,target_disp,target_count);
		printClock(clock);
	}
	else 
	{
		lastOp = RMA;
		clock[rank] = clock[rank] + 1;
		fprintf(fp, "Put\t%p\t%d\t%d\t%ld\t%d\t",origin_addr,origin_count,target_rank,target_disp,target_count);
        printClock(clock);
	}
	return PMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, 
				target_datatype, win);
}

int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, 
			MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (lastOp == RMA)
	{
		fprintf(fp, "Get\t%p\t%d\t%d\t%ld\t%d\t",origin_addr,origin_count,target_rank,target_disp,target_count);
		printClock(clock);
	}
	else 
	{
		lastOp = RMA;
		clock[rank] = clock[rank] + 1;
		fprintf(fp, "Get\t%p\t%d\t%d\t%ld\t%d\t",origin_addr,origin_count,target_rank,target_disp,target_count);
                printClock(clock);
	}
	return PMPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, 
				target_datatype, win);
}

int MPI_Accumulate(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, 
					MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (lastOp == RMA)
	{
		fprintf(fp, "Accumulate\t%p\t%d\t%d\t%ld\t%d\t",origin_addr,origin_count,target_rank,target_disp,target_count);
		printClock(clock);
	}
	else 
	{
		lastOp = RMA;
		clock[rank] = clock[rank] + 1;
		fprintf(fp, "Accumulate\t%p\t%d\t%d\t%ld\t%d\t",origin_addr,origin_count,target_rank,target_disp,target_count);
                printClock(clock);
	}
	return PMPI_Accumulate(origin_addr, origin_count, origin_datatype,target_rank, target_disp, target_count,
					target_datatype, op, win);
}

int MPI_Win_fence(int assert, MPI_Win win)
{
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	lastOp = SYN;
	clock[rank] = clock[rank] + 1;
	MPI_Allgather(clock + rank, 1, MPI_INT, clock, 1, MPI_INT, MPI_COMM_WORLD);
	fprintf(fp, "Fence\t");
	printClock(clock);
	return PMPI_Win_fence(assert, win);
}

int MPI_Win_post(MPI_Group group, int assert, MPI_Win win)
{
	int size, rank, groupSize, i;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	lastOp = SYN;
	clock[rank] = clock[rank] + 1;
	//get group members and send clock from each member
	MPI_Group worldGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
	MPI_Group_size(group, &groupSize);
	int worldRanks[groupSize], groupRanks[groupSize];
	for (i = 0; i < groupSize; i++)
	{
		groupRanks[i] = i;
	}
	postGroup = group;
	MPI_Group_translate_ranks(group, groupSize, groupRanks, worldGroup, worldRanks);
	for (i = 0; i < groupSize; i++)
	{
		MPI_Send(clock, size, MPI_INT, worldRanks[i], 0, MPI_COMM_WORLD);
	}
	fprintf(fp, "Post");
	for (i = 0; i < groupSize; i++)
	{
		fprintf(fp, "\t%d",groupRanks[i]);
	}
	fprintf(fp, "\t");
	printClock(clock);
	return PMPI_Win_post(group, assert, win);
}

int MPI_Win_start(MPI_Group group, int assert, MPI_Win win)
{
	int size, rank, groupSize, i, j;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	lastOp = SYN;
	clock[rank] = clock[rank] + 1;
	startGroup = group;
	//get group members and receive clock to each member
	MPI_Group worldGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
	MPI_Group_size(group, &groupSize);
	int worldRanks[groupSize], groupRanks[groupSize];
	for (i = 0; i < groupSize; i++)
	{
		groupRanks[i] = i;
	}
	int tmpClock[size];
	MPI_Group_translate_ranks(group, groupSize, groupRanks, worldGroup, worldRanks);
	for (i = 0; i < groupSize; i++)
	{
		MPI_Recv(tmpClock, size, MPI_INT, worldRanks[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (j = 0; j < size; j++)
		{
			if (tmpClock[j] > clock[j]) clock[j] = tmpClock[j];
		}
	}
	fprintf(fp, "Start");
	for (i = 0; i < groupSize; i++)
	{
		fprintf(fp, "\t%d",groupRanks[i]);
	}
	fprintf(fp, "\t");
    printClock(clock);
	return PMPI_Win_start(group, assert, win);
}

int MPI_Win_complete(MPI_Win win)
{
	int size, rank, groupSize, i;
	MPI_Group worldGroup;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	lastOp = SYN;
	clock[rank] = clock[rank] + 1;
	//get group members and send clock from each member
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
	MPI_Group_size(startGroup, &groupSize);
	int worldRanks[groupSize], groupRanks[groupSize];
	for (i = 0; i < groupSize; i++)
	{
		groupRanks[i] = i;
	}
	MPI_Group_translate_ranks(startGroup, groupSize, groupRanks, worldGroup, worldRanks);
	for (i = 0; i < groupSize; i++)
	{
		MPI_Send(clock, size, MPI_INT, worldRanks[i], 0, MPI_COMM_WORLD);
	}
	fprintf(fp, "Complete");
	for (i = 0; i < groupSize; i++)
	{
		fprintf(fp, "\t%d",groupRanks[i]);
	}
	fprintf(fp, "\t");
	printClock(clock);
	return PMPI_Win_complete(win);
}

int MPI_Win_wait(MPI_Win win)
{
	int size, rank, groupSize, i, j;
	MPI_Group worldGroup;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	lastOp = SYN;
	clock[rank] = clock[rank] + 1;
	//get group members and receive clock to each member
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
	MPI_Group_size(postGroup, &groupSize);
	int worldRanks[groupSize], groupRanks[groupSize];
	for (i = 0; i < groupSize; i++)
	{
		groupRanks[i] = i;
	}
	int tmpClock[size];
	MPI_Group_translate_ranks(postGroup, groupSize, groupRanks, worldGroup, worldRanks);
	for (i = 0; i < groupSize; i++)
	{
		MPI_Recv(tmpClock, size, MPI_INT, worldRanks[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (j = 0; j < size; j++)
		{
			if (tmpClock[j] > clock[j]) clock[j] = tmpClock[j];
		}
	}
    fprintf(fp, "Wait");
    for (i = 0; i < groupSize; i++)
    {
		fprintf(fp, "\t%d",groupRanks[i]);
	}
	fprintf(fp, "\t");
	printClock(clock);
	return PMPI_Win_wait(win);
}

int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win)
{
	int pmpiRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &pmpiRank);
	lastOp = SYN;
	clock[pmpiRank] = clock[pmpiRank] + 1;
	fprintf(fp, "Lock\t");
	printClock(clock);
	return PMPI_Win_lock(lock_type, rank, assert, win);
}

int MPI_Win_lock_all(int assert, MPI_Win win)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	lastOp = SYN;
	clock[rank] = clock[rank] + 1;
	fprintf(fp, "LockAll\t");
	printClock(clock);
	return PMPI_Win_lock_all(assert, win);
}

int MPI_Win_unlock(int rank, MPI_Win win)
{
	int pmpiRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &pmpiRank);
	lastOp = SYN;
	clock[pmpiRank] = clock[pmpiRank] + 1;
	fprintf(fp, "Unlock\t");
	printClock(clock);
	return PMPI_Win_unlock(rank, win);
}

int MPI_Win_unlock_all(MPI_Win win)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	lastOp = SYN;
	clock[rank] = clock[rank] + 1;
	fprintf(fp, "UnlockAll\t");
	printClock(clock);
	return PMPI_Win_unlock_all(win);
}
