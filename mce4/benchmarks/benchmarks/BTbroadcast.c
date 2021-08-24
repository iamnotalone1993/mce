#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>

int ishft(int i, int shift)
{
	if (shift == 0) return i;
	if (shift > 0) return (i << shift);
	return (i >> -shift);
}

int btest(int i, int pos)
{
	return (i & (1 << pos));
}

int ibclr(int i, int pos)
{
	return (i & (~(1 << pos)));
}

int ibset(int i, int pos)
{
	return (i | (1 << pos));
}

int main(int argc, char **argv)
{
	int myrank, sharedBuffer, a, n, target, pe, position, j;
	float syn, check;
	MPI_Win win;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &pe);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	sharedBuffer = myrank;
	n = 1;
	MPI_Win_create(&sharedBuffer, n * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Win_fence(0, win);
	if (myrank == 0)
	{
		target = 1;
		while (target < pe)
		{
			target = ishft(target, 1);
		}
		target = ishft(target, -1);
		syn = 1.0;
		while (target > 0)
		{
			MPI_Send(&syn, 1, MPI_FLOAT, target, 1, MPI_COMM_WORLD);
			target = ishft(target, -1);
		}
	}
	else
	{
		if ((btest(myrank, 0) == true) || (myrank == pe))
		{
			position = 0;
			while (btest(myrank, position) == false)
			{
				position++;
			}
			MPI_Recv(&syn, 1, MPI_FLOAT, ibclr(myrank, position), 1, MPI_COMM_WORLD, &status);
			MPI_Get(&a, n, MPI_FLOAT, ibclr(myrank, position), 0, n, MPI_FLOAT, win);
			check = 0.0;
			while (check == 0.0)
			{
				MPI_Get(&check, 1, MPI_FLOAT, myrank, n-1, 1, MPI_FLOAT, win);
			}
		}
		else
		{
			position = 1;
			while (btest(myrank, position) == false)
			{
				position++;
			}
			j = position - 1;
			target = ibset(myrank, j);
			while (target > pe-1)
			{
				j--;
				target = ibset(myrank, j);
			}
			MPI_Recv(&syn, 1, MPI_FLOAT, ibclr(myrank, position), 1, MPI_COMM_WORLD, &status);
			MPI_Get(&a, n, MPI_FLOAT, ibclr(myrank, position), 0, n, MPI_FLOAT, win);
			check = 0.0;
			while (check == 0.0)
			{
				MPI_Get(&check, 1, MPI_FLOAT, myrank, n-1, 1, MPI_FLOAT, win);
			}
			syn = 1.0;
			while (j >= 0)
			{
				MPI_Send(&syn, 1, MPI_FLOAT, ibset(myrank, j), 1, MPI_COMM_WORLD);
				j--;
			}
		}
	}	
	MPI_Win_fence(0, win);
	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();
	return 0;
}
