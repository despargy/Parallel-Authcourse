/*
 cilk_bitonic.c
 Based on bitonic.c by Nikos Pitsianis, Duke CS

 This file contains two different implementations of the bitonic sort
        recursive  version :  cRecBitonicSort()
        imperative version :  cImpBitonicSort()
 It also contains qsort for comparisson and optimization purposes

 The bitonic sort is also known as Batcher Sort.
 For a reference of the algorithm, see the article titled
 Sorting networks and their applications by K. E. Batcher in 1968


 The following codes take references to the codes avaiable at

 http://www.cag.lcs.mit.edu/streamit/results/bitonic/code/c/bitonic.c

 http://www.tools-of-computing.com/tc/CS/Sorts/bitonic_sort.htm

 http://www.iti.fh-flensburg.de/lang/algorithmen/sortieren/bitonic/bitonicen.htm
 */

/*
------------------------------
 My Notes - Polizois Siois
------------------------------
Parallel Imperative
  Using _Cilk_for to parallelise the inner loop of the serial Imperative implementation.
  We choose the inner loop because it performs comparissons(and exchanges) which
  are independent to one another.Cilk decides how to divide and assign the work to
  each available thread.
Parallel Recursive
  We are able to execute the two parts(ascending and descending) of the recursion
  in parallel because they operate independently and on different parts of the array.
  We declare wich parts can be parallelised but cilk decides how to divide and assign
  the work to each available thread.
Parallel Recursive & qsort
  When recursion spawns the last available thread, we can switch to serial qsort
  just to make the program a bit faster.(qsort is faster than serial recursive bitonic)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cilk/cilk_api.h>
#include <cilk/cilk.h>

struct timeval startwtime, endwtime;
double seq_time;

int NUM_THREADS; // Number of threads to be created used
int N;          // data array size
int *a;         // data array to be sorted

const int ASCENDING  = 1;
const int DESCENDING = 0;


void init(void);
void print(void);
void sort(void);
void test(void);
inline void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void cRecBitonicSort(int lo, int cnt, int dir);
void cImpBitonicSort(void);

int setWorkers(void);

int desc( const void *a, const void *b );
int asc( const void *a, const void *b );

/** the main program **/
/*
  When starting the program you give one arguement :
        The number of ints to be sorted
*/
int main(int argc, char **argv) {

  if (argc != 3 || atoi(argv[2]) > 8) {
    printf("Usage: %s q t\nwhere n=2^q is the problem size (power of two)\nand m=2^t is the number of threads (power of two)\n"
		,argv[0]);
    exit(1);
  }

  N = 1<<atoi(argv[1]); //Converting the arguement to int and calculating 2^q
  NUM_THREADS = 1<<atoi(argv[2]); //Converting the arguement to int and calculating 2^t
  a = (int *) malloc(N * sizeof(int)); //Allocating size for the array that stores the intergers

  printf("----------------------------------\n");
  printf("| size = %d | threads = %d |\n", N, NUM_THREADS);
  printf("----------------------------------\n");

  /*Creating the user-demanded threads*/
  if(setWorkers()==2) printf("Could not create %d workers, resetting to %d\n"
  , NUM_THREADS, __cilkrts_get_nworkers());

  init();
  gettimeofday( &startwtime, NULL );
  qsort( a, N, sizeof( int ), asc );
  gettimeofday( &endwtime, NULL );
  seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
  printf( "Qsort : %f sec\n", seq_time );
  //test();

  init();
  gettimeofday (&startwtime, NULL);
  cImpBitonicSort();
	gettimeofday (&endwtime, NULL);
  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);
  printf("(cilk) Parallel Imperative : %f sec\n", seq_time);
  //test();

  init();
  gettimeofday (&startwtime, NULL);
  sort();
  gettimeofday (&endwtime, NULL);
  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);
  printf("(cilk) Parallel Recursive with qsort : %f sec\n", seq_time);
  //test();

}

/** -------------- SUB-PROCEDURES  ----------------- **/

/** procedure test() : verify sort results **/
void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]);
  }

  printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
}


/** procedure init() : initialize array "a" with data **/
void init() {
  int i;
  for (i = 0; i < N; i++) {
    a[i] = rand() % N; // (N - i);
  }
}

/** procedure  print() : print array elements **/
void print() {
  int i;
  for (i = 0; i < N; i++) {
    printf("%d\n", a[i]);
  }
  printf("\n");
}


/** INLINE procedure exchange() : pair swap **/
inline void exchange(int i, int j) {
  int t;
  t = a[i];
  a[i] = a[j];
  a[j] = t;
}



/** procedure compare()
   The parameter dir indicates the sorting direction, ASCENDING
   or DESCENDING; if (a[i] > a[j]) agrees with the direction,
   then a[i] and a[j] are interchanged.
**/
inline void compare(int i, int j, int dir) {
  if (dir==(a[i]>a[j]))
    exchange(i,j);
}




/** Procedure bitonicMerge()
   It recursively sorts a bitonic sequence in ascending order,
   if dir = ASCENDING, and in descending order otherwise.
   The sequence to be sorted starts at index position lo,
   the parameter cbt is the number of elements to be sorted.
 **/
void bitonicMerge(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);
    bitonicMerge(lo, k, dir);
    bitonicMerge(lo+k, k, dir);
  }
}



/** function cRecBitonicSort()
    first produces a bitonic sequence by recursively sorting
    its two halves(in parallel) in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order
 **/
void cRecBitonicSort(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    if(__cilkrts_get_worker_number()+1 < NUM_THREADS) //available threads, so parallel execution
    {
        _Cilk_spawn cRecBitonicSort(lo, k, ASCENDING);
        cRecBitonicSort(lo+k, k, DESCENDING);
        _Cilk_sync;
        bitonicMerge(lo, cnt, dir);
    }else //no more threads available so switch to quicksort to serial execution
    {
      qsort( a + lo, k, sizeof( int ), asc );
      qsort( a + ( lo + k ) , k, sizeof( int ), desc );
      bitonicMerge(lo, cnt, dir);
    }
  }
}

/** function setWorkers()
   Responsible for telling cilk to create the threads demanded by the user
   !Note!
   Cilk may not always permit the creation of the demanded threads and reset
   to the default threads
**/
int setWorkers()
{
  char workers[sizeof(long double)];
  sprintf(workers,"%d",NUM_THREADS);
  __cilkrts_end_cilk();
  int rc = __cilkrts_set_param("nworkers",workers);
  return rc;
}

/** function sort()
   Caller of cRecBitonicSort for sorting the entire array of length N
   in ASCENDING order
**/
void sort() {
  cRecBitonicSort(0, N, ASCENDING);
}

/** compare for qsort **/
int desc( const void *a, const void *b ){
    int* arg1 = (int *)a;
    int* arg2 = (int *)b;
    if( *arg1 > *arg2 ) return -1;
    else if( *arg1 == *arg2 ) return 0;
    return 1;
}
int asc( const void *a, const void *b ){
    int* arg1 = (int *)a;
    int* arg2 = (int *)b;
    if( *arg1 < *arg2 ) return -1;
    else if( *arg1 == *arg2 ) return 0;
    return 1;
}

/*
  serial imperative version of bitonic sort (example)
*/
/*
	Lets say we give 4 as an arguement so we have N=2^4(power of 2) = 16 elements.
	In the OUTER for loop the values of k are : 2, 4, 8, 16
	In the MIDDLE for loop :
		when k=2 we get j=1
			So in the INNER for we compare elements whose distance is 1 ,ex. 0-1,2-3...
		when k=4 we get j=2,1 so in the INNER for loop
			when j=2 we compare elements whose distance is 2 ,ex. 0-2,1-3...
			when j=1 we compare elements whose distance is 1 ,ex. 0-1,2-3...
		when k=8 we get j=4,2,1 so in the INNER for loop
			when j=4 we compare elements whose distance is 4 ,ex. 0-4,1-5...,8-12
			when j=2 we compare elements whose distance is 2 ,ex. 0-2,1-3...
			when j=1 we compare elements whose distance is 1 ,ex. 0-1,2-3...
		when k=16 we get j=8,4,2,1 so in the INNER for loop
			when j=8 we compare elements whose distance is 8 ,ex. 0-8,1-9...,7-15
			when j=4 we compare elements whose distance is 4 ,ex. 0-4,1-5...,8-12
			when j=2 we compare elements whose distance is 2 ,ex. 0-2,1-3...
			when j=1 we compare elements whose distance is 1 ,ex. 0-1,2-3...
*/

void cImpBitonicSort() {
  int i,j,k;

	for (k=2; k<=N; k=2*k)
	{
		for (j=k>>1; j>0; j=j>>1)
		{
      //Here is where the parallelization happens
      //Cilk divides the work and assigns it to the available threads by herself
      _Cilk_for (i=0; i<N; i++)
			{
				int ij=i^j;
				if ((ij)>i)
				{
					if ((i&k)==0 && a[i] > a[ij])
					exchange(i,ij);
					if ((i&k)!=0 && a[i] < a[ij])
					exchange(i,ij);
				}

			}
		}
  }
}
