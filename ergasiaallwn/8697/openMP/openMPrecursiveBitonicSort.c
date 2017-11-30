/* Filename:openMPrecursiveBitonicSort.c
 *
 * Author: Stavros Papadopoulos
 *
 * Summary:This file contains the implementation of recursive version of Bitonic Sort
 * with the the usage and techincs of openMP.
 *
 * It is actually a modified "parallelized" version of the "serial" algorithm of bitonic  Sort ,written by
 * professor Nikos Pitsianis.
 *
 *filesource: https://elearning.auth.gr/pluginfile.php/740503/mod_workshop/instructauthors/bitonic.c
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>


struct timeval startwtime, endwtime;
double seq_time;


int N;          // data array size
int *a;         // data array to be sorted

omp_lock_t key;
const int ASCENDING  = 1;
const int DESCENDING = 0;
int active_threads=0;

void init(void);
void print(void);
void sort(void);
void test(void);
inline void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void impBitonicSort(void);
void PbitonicMerge(int lo, int cnt, int dir);
void PrecBitonicSort(int lo, int cnt, int dir);
int num_of_threads;

/** the main program **/ 
int main(int argc, char **argv) {

  if (argc != 2) {
    printf("Usage: %s q\n  where n=2^q is problem size (power of two)\n", 
	   argv[0]);
    exit(1);
  }
  
  

  N = 1<<atoi(argv[1]);
  a = (int *) malloc(N * sizeof(int));
  omp_init_lock(&key);
 num_of_threads=1<<atoi(argv[2]);
  //printf("THREADS %d\n",num_of_threads); 
  

  init();
  
  gettimeofday (&startwtime, NULL);
  sort();
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("%f", seq_time);

  test();
  omp_destroy_lock(&key);
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

 // printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
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


void PbitonicMerge(int lo, int cnt, int dir) {
    
  if (cnt>1) {
   if(active_threads<num_of_threads){
   
   
    int k=cnt/2;
    int i;
	
	
	
	for (i=lo; i<lo+k; i++){
      compare(i, i+k, dir);
	 
	  }
	 
	
	
      omp_set_lock(&key);
      active_threads=active_threads+2;
      omp_unset_lock(&key);
	
	
	#pragma omp parallel
            {

            #pragma omp sections
            {

            #pragma omp section
            PbitonicMerge(lo, k, dir);

            #pragma omp section
            PbitonicMerge(lo+k, k, dir);
            }
            #pragma omp barrier


            }

            omp_set_lock(&key);
             active_threads=active_threads-2;
            omp_unset_lock(&key);			 
           
    
	
	
    
    
   }
   
   else bitonicMerge(lo,cnt,dir);
  }
 }


void PrecBitonicSort(int lo, int cnt, int dir) {
    if (cnt>1){
        int k=cnt/2;

        if (active_threads<num_of_threads){

            omp_set_lock(&key);
             active_threads=active_threads+2;
            omp_unset_lock(&key);
            #pragma omp parallel
            {

            #pragma omp sections
            {

            #pragma omp section
            PrecBitonicSort(lo, k, ASCENDING);

            #pragma omp section
            PrecBitonicSort(lo+k, k, DESCENDING);
            }
            #pragma omp barrier


            }
            PbitonicMerge(lo, cnt, dir);

        omp_set_lock(&key);
        active_threads=active_threads-2;
		omp_unset_lock(&key);
        }
        else {
            recBitonicSort(lo, cnt, dir);

        }
    }
}

	
 



/** function sort() 
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/
void sort() {
  recBitonicSort(0, N, ASCENDING);
}



