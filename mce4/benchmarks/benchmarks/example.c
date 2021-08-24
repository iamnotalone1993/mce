#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char **argv)
{
	int numtasks, rank, sharedBuffer, localBuffer;
	MPI_Win win;
	MPI_Group worldGroup, primeGroup;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);

	const int ranks[3] = {0, 1, 2};

	sharedBuffer = rank;
	MPI_Win_create(&sharedBuffer, sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

	MPI_Win_fence(0, win);
	if (rank == 0)
	{	
		MPI_Get(&localBuffer, 1, MPI_INT, 1, 0, 1, MPI_INT, win);
	}
	else if (rank == 1)
	{
		MPI_Get(&localBuffer, 1, MPI_INT, 2, 0, 1, MPI_INT, win);
		if (localBuffer == 1)
		{
			//do something
		}
	}
	else if (rank == 2)
	{
		localBuffer = 3;
		MPI_Put(&localBuffer, 1, MPI_INT, 0, 0, 1, MPI_INT, win);
	}
	else //if (rank > 2)
	{
		//do nothing
	}
	MPI_Win_fence(0, win);
	/**/
	
	if (rank == 0)
	{
		MPI_Win_lock(MPI_LOCK_SHARED, 1, 0, win);
		localBuffer = 6;
		MPI_Put(&localBuffer, 1, MPI_INT, 1, 0, 1, MPI_INT, win);
		MPI_Win_unlock(1, win);
		/**/

		MPI_Group_incl(worldGroup, 1, ranks + 1, &primeGroup);
		MPI_Win_start(primeGroup, 0, win);
                MPI_Get(&localBuffer, 1, MPI_INT, 1, 0, 1, MPI_INT, win);
		sharedBuffer = 5;
		MPI_Win_complete(win);
		/**/
		
		MPI_Win_lock(MPI_LOCK_SHARED, 1, 0, win);
                localBuffer = 7;
                MPI_Put(&localBuffer, 1, MPI_INT, 1, 0, 1, MPI_INT, win);
                MPI_Win_unlock(1, win);
		/**/
	}
	else if (rank == 1)
	{
		MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);
		localBuffer = 6;
		MPI_Put(&localBuffer, 1, MPI_INT, 0, 0, 1, MPI_INT, win);
		MPI_Win_unlock(0, win);
		/**/

        	MPI_Group_incl(worldGroup, 1, ranks, &primeGroup);
		MPI_Win_post(primeGroup, 0, win);
		sharedBuffer = 3;
		MPI_Win_wait(win);
		/**/

		MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);
                localBuffer = 7;
                MPI_Put(&localBuffer, 1, MPI_INT, 0, 0, 1, MPI_INT, win);
                MPI_Win_unlock(0, win);
		/**/
	}
	else if (rank == 2)
	{	
		MPI_Win_lock(MPI_LOCK_SHARED, 1, 0, win);
		localBuffer = 4;
		MPI_Put(&localBuffer, 1, MPI_INT, 1, 0, 1, MPI_INT, win);
		MPI_Win_unlock(1, win);
		/**/
	}
	else //if (rank > 2)
	{
		printf("rank = %d\n",rank);
		//do nothing
	}
	/**/

	MPI_Finalize();
	return 0;
}
