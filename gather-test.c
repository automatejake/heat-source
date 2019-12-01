#include <stdio.h>
#include <stdlib.h>
#include<time.h>

#include <sys/utsname.h>
#include <mpi.h>

int main(int argc, char** argv){
    int i, my_rank, rank_sum, size, buf, fromOne;
    int tag = 0;


    MPI_Status status;
    MPI_Init(&argc, &argv); //initialize mpi
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    float new_array[(my_rank + 1) * 2];
    float old_array[2] = {1.0,2.0};
    int process_height = 2;
    MPI_Gather(&old_array, process_height, MPI_FLOAT, &new_array, process_height, MPI_FLOAT, 0, MPI_COMM_WORLD);
    if(my_rank == 0){
        printf("\n");
        int i;
        for(i = 0; i < sizeof(old_array); i++ ){
            printf("%f, ", new_array[i]);
        }
    }
}