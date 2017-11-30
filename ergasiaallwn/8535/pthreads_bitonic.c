/*
 pthreads_bitonic.c
 Based on bitonic.c by Nikos Pitsianis, Duke CS

 This file contains two different implementations of the bitonic sort
        serial recursive  version :  recBitonicSort()
        parallel imperative version :  pImpBitonicSort()
 It also contains qsort for comparisson purposes

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
         My Notes
-----------------------------
Parallel Imperative
  Dividing the N=2^q repetitions (for every j of every k) to the existing threads.
  For example if N=256 and NUM_THREADS=128, every thread will have to do 2 repetitions.
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

struct timeval startwtime, endwtime;
double seq_time;

struct thread_data // Data container fo a thread
{
   int thread_id;
   int step;
   int j;
	 int k;
};

int NUM_THREADS; // Number of threads to be created used
struct thread_data *thread_data_array; // Array containing data for each thread

int N;          // problem size
int *a;         // problem sized array to be sorted

const int ASCENDING  = 1;
const int DESCENDING = 0;


void init(void);
void print(void);
void sort(void);
void test(void);
// inline void exchange(int i, int j);
void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void pImpBitonicSort(void);
void *insidepImpBitonicSort(void *threadarg);

int desc( const void *a, const void *b );
int asc( const void *a, const void *b );

/** the main program **/
/*
  When starting the program you give 2 arguements :
        q : where n=2^q is the problem size (power of two)
        t : where m=2^t is the number of threads to be used (power of two)
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
	thread_data_array = (struct thread_data *) malloc(NUM_THREADS * sizeof(struct thread_data));

  printf("----------------------------------\n");
  printf("| size = %d | threads = %d |\n", N, NUM_THREADS);
  printf("----------------------------------\n");

  init();
  gettimeofday( &startwtime, NULL );
  qsort( a, N, sizeof( int ), asc );
  gettimeofday( &endwtime, NULL );
  seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
  printf( "Qsort : %f sec\n", seq_time );
  // test();

  init();
  gettimeofday (&startwtime, NULL);
  pImpBitonicSort();
	gettimeofday (&endwtime, NULL);
  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);
  printf("(pthreads) Parallel imperative : %f sec\n", seq_time);
  // test();

  // init();
  // gettimeofday (&startwtime, NULL);
  // sort();
  // gettimeofday (&endwtime, NULL);
  // seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
	// 	      + endwtime.tv_sec - startwtime.tv_sec);
  // printf("Serial Recursive : %f sec\n", seq_time);
  // test();

	pthread_exit(NULL);
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



/** function recBitonicSort()
    first produces a bitonic sequence by recursively sorting
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order
 **/
void recBitonicSort(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    recBitonicSort(lo, k, ASCENDING);
    recBitonicSort(lo+k, k, DESCENDING);
    bitonicMerge(lo, cnt, dir);
  }
}


/** function sort()
   Caller of recBitonicSort for sorting the entire array of length N
   in ASCENDING order
**/
void sort() {
  recBitonicSort(0, N, ASCENDING);
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

void pImpBitonicSort() {
	pthread_t threads[NUM_THREADS];
	int *taskids[NUM_THREADS];
  int i,j,k,rc,step,limit;

	step = N/NUM_THREADS;
	limit = NUM_THREADS;
	if (step == 0) // threads more than elements
	{
		limit = N;
		step = 1;
	}

	for (k=2; k<=N; k=2*k)
	{
		for (j=k>>1; j>0; j=j>>1)
		{
      //Here is where the parallelization happens
			for (i=0; i<limit; i++)
			{
				//Threads are created here
				thread_data_array[i].thread_id = i;
				thread_data_array[i].step = step;
				thread_data_array[i].j = j;
				thread_data_array[i].k = k;
				rc = pthread_create(&threads[i], NULL, insidepImpBitonicSort, (void *)
			       &thread_data_array[i]);
			  if (rc) {
			    printf("ERROR; return code from pthread_create() is %d\n", rc);
			    exit(-1);
				}
			}

			for (i=0; i<limit; i++)
			{
				//Join threads Here
				pthread_join(threads[i],NULL);
			}
		}
  }
	//pthread_exit(NULL);
}

//Create function for the threads
void *insidepImpBitonicSort(void *threadarg)
{
	int i,step,j,k,threadid,ij,start;
	struct thread_data *my_data;

	my_data = (struct thread_data *) threadarg;
	threadid = my_data->thread_id;
	step = my_data->step;
	j = my_data->j;
	k = my_data->k;

	start = threadid*step;
  //each thread has to do work of size "step"
	for (i=start; i<start+step; i++)
	{
		ij=i^j;
		if ((ij)>i)
		{
			if ((i&k)==0 && a[i] > a[ij])
			exchange(i,ij);
			if ((i&k)!=0 && a[i] < a[ij])
			exchange(i,ij);
		}
	}
	pthread_exit(NULL);
}
