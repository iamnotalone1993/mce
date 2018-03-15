#ifndef PMPI_H
#define PMPI_H

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>

int *clock;
double execTime;
FILE *fp;
MPI_Group startGroup, postGroup;

void printClock(int *);
void tracels(bool, void *);

int MPI_Init(int *, char ***);
int MPI_Finalize(void);
int MPI_Win_create(void *, MPI_Aint, int, MPI_Info, MPI_Comm, MPI_Win *);
int MPI_Put(const void *, int, MPI_Datatype, int, MPI_Aint, int, MPI_Datatype, MPI_Win);
int MPI_Get(void *, int, MPI_Datatype, int, MPI_Aint, int, MPI_Datatype, MPI_Win);
int MPI_Accumulate(const void *, int, MPI_Datatype, int, MPI_Aint, int, MPI_Datatype, MPI_Op, MPI_Win);
int MPI_Win_fence(int, MPI_Win);
int MPI_Win_post(MPI_Group, int, MPI_Win);
int MPI_Win_start(MPI_Group, int, MPI_Win);
int MPI_Win_complete(MPI_Win);
int MPI_Win_wait(MPI_Win);
int MPI_Win_lock(int, int, int, MPI_Win);
int MPI_Win_unlock(int, MPI_Win);
int MPI_Send(const void *, int, MPI_Datatype, int, int, MPI_Comm);
int MPI_Recv(void *, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status *);
int MPI_Barrier(MPI_Comm);

#endif
