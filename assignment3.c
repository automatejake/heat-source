// CPSC 3620 - Assignment 3: Part 2
// Dr. Amy Apon, Dr. Linh Ngo
// ***** - 9/30/2014
// I worked on this assignment alone, using only course materials.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void copyNewToOld(float *new, float *old, int width, int height);
void calculateNew(float *new, float *old, int width, int height);
void writeGrid(char *filename, float *grid, int width, int height);

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    
    // Get process rank and size
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Read in the number of iterations and the room width/height
    int steps = atoi(argv[1]);
    int room_width = atoi(argv[2]);
    int room_height = room_width;
    
    // Actual width of the array includes the walls
    int width = room_width+2;
    // Calculate height of the slice for this process
    // This is done in such a way to allow uneven distribution (off by 1)
    int slice_height = room_height*(rank+1)/size - room_height*rank/size;
    // Allocate two arrays to hold the slice temperatures (plus ghost rows)
    float *temps = (float *)malloc(width*(slice_height+2)*sizeof(float));
    float *old = (float *)malloc(width*(slice_height+2)*sizeof(float));
    
    // Fill the entire slice with 20 degrees
    int i;
    for (i = 0; i < width*(slice_height+2); i++)
        temps[i] = 20.0;
    // If this is the top slice, insert the 300-degree fireplace
    if (rank == 0)
        for (i = width*3/10+1; i < width*7/10+1; i++)
            temps[i] = 300.0;

    // Loop over the specified number of iterations
    for (; steps > 0; steps--)
    {
        // Send bottom row down
        if (rank < size-1)
            MPI_Send(temps + width*slice_height, width, MPI_FLOAT, rank+1, 0, MPI_COMM_WORLD);
        if (rank > 0)
        {
            // Receive ghost row from above
            MPI_Recv(temps, width, MPI_FLOAT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // Send top row up
            MPI_Send(temps + width, width, MPI_FLOAT, rank-1, 1, MPI_COMM_WORLD);
        }
        // Receive ghost row from below
        if (rank < size-1)
            MPI_Recv(temps + width*(slice_height+1), width, MPI_FLOAT, rank+1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        // Copy the current slice to the old slice
        copyNewToOld(temps, old, width, slice_height+2);
        // Generate a new current slice based on the old slice
        calculateNew(temps, old, width, slice_height+2);
    }
    
    // Initialize these pointers to NULL so they can be freed safely
    float *results = NULL;
    int *sizes = NULL;
    int *disps = NULL;
    if (rank == 0)
    {
        // On the root process, allocate space for the full room
        results = (float *)malloc(width*room_height*sizeof(float));
        // Allocate arrays to store the displacements and sizes for each slice
        disps = (int *)malloc(size*sizeof(int));
        sizes = (int *)malloc(size*sizeof(int));
        // Calculate displacements and sizes
        disps[0] = 0;
        for (i = 1; i <= size; i++)
        {
            int next = (room_height*i/size) * width;
            if (i < size)
                disps[i] = next;
            sizes[i-1] = next - disps[i-1];
        }
    }
    
    // Gather the slices back into the full room
    MPI_Gatherv(temps + width, slice_height*width, MPI_FLOAT, results, sizes, disps, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    // Process 0 now writes the room to a file
    if (rank == 0)
        writeGrid("room.pnm", results, width, room_height);
    
    // Free all allocated arrays
    free(temps);
    free(old);
    free(sizes);
    free(disps);
    free(results);

    MPI_Finalize();
    return 0;
}

void copyNewToOld(float *new, float *old, int width, int height)
{
    // Simply copy the entire new array into the old array
    memcpy(old, new, width*height*sizeof(float));
}

void calculateNew(float *new, float *old, int width, int height)
{
    int i,j;
    // Loop over all but the first and last rows
    for (j = 1; j < height-1; j++)
    {
        // Advance to the next row
        new += width;
        old += width;
        // Loop over all but the first and last columns
        for (i = 1; i < width-1; i++)
        {
            // Calculate average of left/right/up/down from the old slice
            new[i] = (old[i-1] + old[i+1] + old[i-width] + old[i+width]) / 4;
        }
    }
}

void writeGrid(char *filename, float *grid, int width, int height)
{
    // Open file and write header
    FILE *fout = fopen(filename, "w");
    fprintf(fout, "P3\n%d %d\n15\n", width-2, height);
    
    // Loop over the interior of the room
    int i,j;
    for (j = 0; j < height; j++)
    {
        for (i = 1; i < width-1; i++)
        {
            // Read temperature value
            float c = grid[i];
            // Print out the appropriate color
            if (c > 250.0)
                fprintf(fout, "15 00 00 ");
            else if (c > 180.0)
                fprintf(fout, "15 05 00 ");
            else if (c > 120.0)
                fprintf(fout, "15 10 00 ");
            else if (c > 80.0)
                fprintf(fout, "00 13 00 ");
            else if (c > 60.0)
                fprintf(fout, "05 10 00 ");
            else if (c > 50.0)
                fprintf(fout, "00 05 10 ");
            else if (c > 40.0)
                fprintf(fout, "00 00 10 ");
            else if (c > 30.0)
                fprintf(fout, "00 05 05 ");
            else if (c > 20.0)
                fprintf(fout, "03 03 00 ");
            else
                fprintf(fout, "00 00 00 ");
        }
        fprintf(fout, "\n");
        // Advance to the next row
        grid += width;
    }
    fclose(fout);
}