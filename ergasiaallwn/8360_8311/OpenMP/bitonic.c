/*
 bitonic.c 

 This file contains two different implementations of the bitonic sort
        recursive  version :  recBitonicSort()
        imperative version :  impBitonicSort() 
 

 The bitonic sort is also known as Batcher Sort. 
 For a reference of the algorithm, see the article titled 
 Sorting networks and their applications by K. E. Batcher in 1968 


 The following codes take references to the codes avaiable at 

 http://www.cag.lcs.mit.edu/streamit/results/bitonic/code/c/bitonic.c

 http://www.tools-of-computing.com/tc/CS/Sorts/bitonic_sort.htm

 http://www.iti.fh-flensburg.de/lang/algorithmen/sortieren/bitonic/bitonicen.htm 
 */

/* 
------- ---------------------- 
   Nikos Pitsianis, Duke CS 
-----------------------------
*/

/*Task 1
Paschos Nikolaos 8360
Bekos Christophoros 8311*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include <string.h>

struct timeval startwtime, endwtime;
double seq_time;


int N;          // data array size
int *a;         // data array to be sorted
int num_of_threads;
int threads_left;

const int ASCENDING  = 1;
const int DESCENDING = 0;


void init(void);
void print(void);
void sort(void);
void test(void);
inline void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void impBitonicSort(void);
void pImpBitonicSort();
void pRecBitonicSort(int lo, int cnt, int dir);
void pSort();
int asc(const void *a, const void *b);
int desc(const void *a, const void *b); 


/** the main program **/ 
int main(int argc, char **argv) {

  if (argc != 3) {
    printf("Usage: %s q\n  where n=2^q is problem size (power of two)\n", 
	   argv[0]);
    exit(1);
  }

  N = 1<<atoi(argv[1]);
  num_of_threads = 1  <<atoi(argv[2]);
  // printf("num of threads = %d\n", num_of_threads);
  a = (int *) malloc(N * sizeof(int));
  // threads_left = num_of_threads;

  init();
  gettimeofday( &startwtime, NULL );
  qsort( a, N, sizeof( int ), asc );
  gettimeofday( &endwtime, NULL );
  seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
  printf( "Qsort wall clock time = %f\n", seq_time );
  test();

  init();
  gettimeofday( &startwtime, NULL );
  pSort( a, N, sizeof( int ), asc );
  gettimeofday( &endwtime, NULL );
  seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
  printf( "Parallel Imperative wall clock time = %f\n", seq_time);
  test();

  init();
  gettimeofday( &startwtime, NULL );
  pImpBitonicSort( a, N, sizeof( int ), asc );
  gettimeofday( &endwtime, NULL );
  seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
  printf("Parallel Recursive wall clock time = %f\n", seq_time);
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

// void pRecBitonicSort(int lo, int cnt, int dir) {
//     if (cnt>1) {
//         int k=cnt/2;
// #pragma omp task shared(k)
//         {recBitonicSort(lo, k, ASCENDING);}
//         recBitonicSort(lo+k, k, DESCENDING);
// #pragma omp taskwait
//         bitonicMerge(lo, cnt, dir);
//     }
// }

void pRecBitonicSort(int lo, int cnt, int dir) {
    if (cnt>1) {
        int k=cnt/2;
#pragma omp task shared(k)
        {recBitonicSort(lo, k, ASCENDING);}
        recBitonicSort(lo+k, k, DESCENDING);
#pragma omp taskwait
        bitonicMerge(lo, cnt, dir);
    }
}

void pSort() {
#pragma omp parallel
    {
#pragma omp single nowait
        { pRecBitonicSort(0, N, ASCENDING); }
    }
}


/*
  imperative version of bitonic sort
*/
void impBitonicSort() {

  int i,j,k;
  
  for (k=2; k<=N; k=2*k) {
    for (j=k>>1; j>0; j=j>>1) {
      for (i=0; i<N; i++) {
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

void pImpBitonicSort() {
  
  int i,j,k;
  for (k=2; k<=N; k=2*k) {
    for (j=k>>1; j>0; j=j>>1) {
      #pragma omp parallel for num_threads(num_of_threads) default(shared) private(i)
      for (i=0; i<N; i++) {
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

int asc( const void *a, const void *b ){
    int* arg1 = (int *)a;
    int* arg2 = (int *)b;
    if( *arg1 < *arg2 ) return -1;
    else if( *arg1 == *arg2 ) return 0;
    return 1;
}

int desc( const void *a, const void *b ){
    int* arg1 = (int *)a;
    int* arg2 = (int *)b;
    if( *arg1 > *arg2 ) return -1;
    else if( *arg1 == *arg2 ) return 0;
    return 1;
}