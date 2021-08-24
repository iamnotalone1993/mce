#include <mpi.h>
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char **argv)
{
	int size, rank, sharedBuffer, localBuffer, i;
	MPI_Win win;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	localBuffer = rank;
	sharedBuffer = rank;
	MPI_Win_create(&sharedBuffer, sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
	
	if (rank == 0)
	{
		MPI_Win_lock(MPI_LOCK_SHARED, 1, 0, win); 
		MPI_Put(&localBuffer, 1, MPI_INT, 1, 0, 1, MPI_INT, win);
		MPI_Win_unlock(1, win);
		
		MPI_Send(&localBuffer, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
	}
	else if (rank == 1)
	{
		MPI_Recv(&localBuffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		MPI_Win_lock(MPI_LOCK_SHARED, 1, 0, win);
		MPI_Put(&localBuffer, 1, MPI_INT, 1, 0, 1, MPI_INT, win);
		MPI_Win_unlock(1, win);

		for (i = rank + 1; i < size; i++)
		{
			MPI_Send(&localBuffer, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}
	else //if (rank >= 2)
	{
		MPI_Recv(&localBuffer, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
		MPI_Win_lock(MPI_LOCK_SHARED, 1, 0, win);
		MPI_Get(&localBuffer, 1, MPI_INT, 1, 0, 1, MPI_INT, win);
		MPI_Win_unlock(1, win);
	}

	MPI_Finalize();	
	return 0;
}
