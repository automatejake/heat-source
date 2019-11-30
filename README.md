# heat-source
Solution to a CSC496 Parallel Programming Assignment using MPI
Problem Description

The temperature of the interior of an area will depend on the temperatures around it. Consider a square area that has known temperatures along each of its edges, and a single heat source at one location. The scientific question that we would like to answer is, “What is the distribution of heat in a region given a fixed heat source?” 

The approach to solving this problem is to divide the area into a fine mesh of points. Temperature at an inside point are taken to be the average of temperatures of four neighboring points. The figure at the top of this page shows the graphical result of a calculation with a square that is 30 units by 30 units and a heat source of 1000 degrees Celsius located 3 units from the top and the left. This solution was obtained after 50,000 iterations, which takes just a few seconds on a desktop computer. The figure to the left shows the model of the area as a square mesh of points. One of the points is enlarged.





Programming Assignment

A sequential heat-source.c program has been developed to solve this problem. The design of this program is as follows:
The program declares a mesh of size of 1000 by 1000. 
The temperature of the edges is fixed at 20 degrees Celsius. 
The interior of the mesh is initialized to 20 degrees Celsius, but these will change as the program
executes. 
The program defines a fireplace that is about 40% of the room width and placed at the top of the room.
The temperature of the fireplace is set to 300 degrees Celsius. 
The program runs iteratively to calculate the temperature of each coordinate in the mesh and produces a graphical output, which is a bitmap file. The coloring range of the bitmap file is defined as follows:
If the temperature is above 250, print in RED
If the temperature is between 180 and 250 print in ORANGE
If the temperature is between 120 and 180 print in YELLOW
If the temperature is between 80 and 120 print in LTGREEN
If the temperature is between 60 and 80 print in GREEN
If the temperature is between 50 and 60 print in LTBLUE
If the temperature is between 40 and 50 print in BLUE
If the temperature is between 30 and 40 print in DARKTEAL
If the temperature is between 20 and 30 print in BROWN
If the temperature is 20 or less print in BLACK

Task 1: You are to modify this sequential program into one program that uses MPI and a data parallel technique to solve the heat distribution problem. The technique used in the lecture tiled the room using small boxes. It is recommended that you divide the area into strips of equal size, but you can use any partitioning technique that you like. You should use ghost points to hold data points that are used by an MPI process as input to the calculation, but are not modified in that process. The ghost points must be communicated in each iteration along with the set of data calculated by each process.

Task 2: Using a single batch submission script, you are to run and time your program on the Bridges supercomputer using 1, 2, 4, 8, 16, and 20 processes. To time your program, change the mpirun call to as follows:
Normal call: mpirun -np 8 …
Timing call: time mpirun -np 8 ...
