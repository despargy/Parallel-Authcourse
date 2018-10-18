			Pagerank with Gauss-Seidel method
A serial and a parallel implementation of Pagerank algorithm in C.

How to use:
Save the data file in the same folder with source file.
The default data file is "hollins.dat", line 73 in parallel implementation (openMPpagerank.c source file) - line 69 in serial implementation (pagerank.c source file).

for Serial implementation
    arguments: number of nodes
example	compile: gcc -o pagerank pagerank.code
example	run: ./pagerank 6012

for Parallel implementation with OpenMP
	arguments: number of nodes, number of threads
	compile: make clean
			 make
	example run:	./openMPpagerank 6012 4