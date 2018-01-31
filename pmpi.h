#ifndef __PMPI_H__
#define __PMPI_H__
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h>

int *clock;
char fname[50] = "trace_";
FILE *fp;
enum opType {LS = 1, SYN = 2, RMA = 3};
enum opType lastOp;
MPI_Group startGroup, postGroup;

void printClock(int *);
void tracels(bool, char *);

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
int MPI_Win_lock_all(int, MPI_Win);
int MPI_Win_unlock(int, MPI_Win);
int MPI_Win_unlock_all(MPI_Win);

#endif
