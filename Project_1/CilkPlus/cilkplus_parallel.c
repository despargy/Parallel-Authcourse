/*
 Parallel computing: CilkPlus method
   based in the serial programme bitonic.c
   
  Project 1,HMMY Nov 2017
  Despina-Ekaterini Argiropoulos	8491
 
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cilk/cilk.h>		//file reference for cilk
#include <cilk/cilk_api.h>	//API to interact with Cilkplus runtime
struct timeval startwtime, endwtime;
double seq_time;


int N;          // data array size
int nofT;	// number of Threads
int *a;         // data array to be sorted

const int ASCENDING  = 1;
const int DESCENDING = 0;


void init(void);
void print(void);
void sort(void);
void test(void);
void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void impBitonicSort(void);


/** the main program **/ 
int main(int argc, char **argv) {

  if (argc != 3) {
    printf("Usage: %s q p\n  where n=2^q is problem size (power of two) [12-24]\n", 
	   argv[0]);
    printf("  where n=2^p is num of threads [1-8]\n");
    exit(1);
  }

  N = 1<<atoi(argv[1]);
  nofT = atoi(argv[2]);

  a = (int *) malloc(N * sizeof(int));

__cilkrts_set_param("nworkers", argv[2]);	//Specifies the number of worker threads to be created
  init();
  gettimeofday (&startwtime, NULL);
  impBitonicSort();
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Imperative wall clock time = %f\n", seq_time);

  test();

  init();
  gettimeofday (&startwtime, NULL);
  sort();
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Recursive wall clock time = %f\n", seq_time);

  test();

  // print();
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
    parallel edition
    first produces a bitonic sequence by recursively sorting 
    its two halves(wich run in parallel) in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order 
    if can open one more (except the master), calls the recursively sorting in parallel
    else in serial as bitonic.c
 **/
void recBitonicSort(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    cilk_spawn recBitonicSort(lo, k, ASCENDING);	//the function may (but is not required to) run in parallel with the caller
    cilk_spawn recBitonicSort(lo+k, k, DESCENDING);	//the function may (but is not required to) run in parallel with the caller
    cilk_sync;	//wait for spawned tasks to complete
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



/*
  imperative version of bitonic sort
  parallel edition
*/
void impBitonicSort() {

  int i,j,k;
  
  for (k=2; k<=N; k=2*k) {
    for (j=k>>1; j>0; j=j>>1) {
      cilk_for (i=0; i<N; i++) {	//permits loop iterations to run in parallel
	int ij=i^j;
	if ((ij)>i) {
	  if ((i&k)==0 && a[i] > a[ij]) 
	      exchange(i,ij);
	  if ((i&k)!=0 && a[i] < a[ij])
	      exchange(i,ij);
	}
      }
    }
  }
}

