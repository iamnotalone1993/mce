#ifndef __PMPI_H__
#define __PMPI_H__
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
int *clock;
char fname[50] = "trace_";
//char **interested;
FILE *fp;
//FILE *ifp;
enum opType {LS = 1, SYN = 2, RMA = 3};
enum opType lastOp;
int rank;
int size;
MPI_Group startGroup, postGroup;
extern int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype
            origin_datatype, int target_rank, MPI_Aint target_disp,
            int target_count, MPI_Datatype target_datatype, MPI_Win
            win);
extern int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype
            origin_datatype, int target_rank, MPI_Aint target_disp,
            int target_count, MPI_Datatype target_datatype, MPI_Win
            win);
extern int MPI_Accumulate(const void *origin_addr, int origin_count, MPI_Datatype
                   origin_datatype, int target_rank, MPI_Aint
                   target_disp, int target_count, MPI_Datatype
                   target_datatype, MPI_Op op, MPI_Win win);
extern int MPI_Init( int *argc, char ***argv );
extern int MPI_Finalize( void );
extern int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, 
                  MPI_Comm comm, MPI_Win *win);
extern int MPI_Win_fence(int assert, MPI_Win win);
extern int MPI_Win_post(MPI_Group group,int assert,MPI_Win win);
extern int MPI_Win_start(MPI_Group group,int assert,MPI_Win win);
extern int MPI_Win_complete(MPI_Win win);
extern int MPI_Win_wait(MPI_Win win);
extern int MPI_Win_lock(int lock_type, int rank, int assert, MPI_Win win);
extern int MPI_Win_lock_all(int assert, MPI_Win win);
extern int MPI_Win_unlock(int rank, MPI_Win win);
extern int MPI_Win_unlock_all(MPI_Win win);

#endif
