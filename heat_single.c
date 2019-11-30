#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include <sys/utsname.h>
#include <mpi.h>

#define COLS 1000
#define ROWS 1000

#define WHITE    "15 15 15 "
#define RED      "15 00 00 "
#define ORANGE   "15 05 00 "
#define YELLOW   "15 10 00 "
#define LTGREEN  "00 13 00 "
#define GREEN    "05 10 00 "
#define LTBLUE   "00 05 10 "
#define BLUE     "00 00 10 "
#define DARKTEAL "00 05 05 "
#define BROWN    "03 03 00 "
#define BLACK    "00 00 00 "

// copy new grid to the old grid
// handled by process 1
void copyNewToOld(float grid_a[ROWS][COLS], float grid_b[ROWS][COLS]) {
  int x, y;
  for (x = 0; x < ROWS; ++x) {
    for (y = 0; y < COLS; ++y) {
      grid_b[x][y] = grid_a[x][y];
    }
  }
}

int iterations = 0;

// update the new grid based on the old grid 
// distributed
void calculateNew(float grid_a[ROWS][COLS], float grid_b[ROWS][COLS]) {
  iterations++;
  printf("Iterations: %d\n",iterations);
  int x, y;
  for (x = 1; x < ROWS - 1; ++x) {
    for (y = 1; y < COLS - 1; ++y) {
      // each points temperature is based on the average of the four surrounding points
      grid_a[x][y] = 0.25 * (grid_b[x-1][y] + grid_b[x+1][y] + grid_b[x][y-1] + grid_b[x][y+1]);
    }
  }
}

// print the grid to a bitmap file
void printGridtoFile(float grid[ROWS][COLS]) {
  int x, y;
  FILE * fp;
  fp = fopen("c.pnm", "w");

  /* Print the P3 format header */
  //handled by process 1
  fprintf(fp, "P3\n%d %d\n15\n", COLS, ROWS);
  for (x = 0; x < ROWS; ++x) {
    for (y = 0; y < COLS; ++y) {
      if (grid[x][y] > 250) {
	fprintf(fp, "%s ", RED );
      } else if (grid[x][y] > 180) {
	fprintf(fp, "%s ", ORANGE );
      } else if (grid[x][y] > 120) {
	fprintf(fp, "%s ", YELLOW );
      } else if (grid[x][y] > 80) {
	fprintf(fp, "%s ", LTGREEN );
      } else if (grid[x][y] > 60) {
	fprintf(fp, "%s ", GREEN );
      } else if (grid[x][y] > 50) {
	fprintf(fp, "%s ", LTBLUE );
      } else if (grid[x][y] > 40) {
	fprintf(fp, "%s ", BLUE );
      } else if (grid[x][y] > 30) {
	fprintf(fp, "%s ", DARKTEAL );
      } else if (grid[x][y] > 20) {
	fprintf(fp, "%s ", BROWN );
      } else {
	fprintf(fp, "%s ", BLACK );
      }
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}

int main(int argc, char **argv) {
  int h, w, cycles, heat;
  int i, my_rank, rank_sum, size, buf, fromOne;
  int tag = 0;

  float grid_a[ROWS][COLS];
  float grid_b[ROWS][COLS];

  MPI_Status status;
  MPI_Init(&argc, &argv); //initialize mpi
  
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);


  if(my_rank == 0){


    if (argc != 2) {
      printf("Usage: ./program <number of timestamps>\n");
      exit(0);
    }
    cycles = atoi(argv[1]);

    // initialize two-dimensional array
    for (h = 0; h < ROWS; ++h) {
      for (w = 0; w < COLS; ++w) {
        //default is 20 celcius
        grid_a[w][h] = 20;
      }
    }

    // initialize a heat source
    for (heat = 299; heat < 700; ++heat) {
      //40% of top of room (299-699)
      grid_a[0][heat] = 300;
    }

    for (cycles; cycles > 0; --cycles) {
      copyNewToOld(grid_a, grid_b);
      calculateNew(grid_a, grid_b);
    }

  }

  if(my_rank == 0){
    printGridtoFile(grid_a);
    system("convert c.pnm c.png");
    return 0;
  }

  MPI_Finalize();
  return 0;

}
