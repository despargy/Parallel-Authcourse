/*
 --INITIAL COMMENTS--
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <omp.h>

struct timeval startwtime, endwtime;
double seq_time;


int NUM_THR;

int N;          // data array size
int M;		// number of threads
int *a;         // data array to be sorted
int chunk;      // CHUNK for OpenMP Imp Bitonic

int *b;		// data array to be copied to

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
int cmpfunc (const void * a, const void * b);  //cmp function for qsort


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


/** the main program **/ 
int main(int argc, char **argv) {

  if (argc != 3) {
    printf("Usage: %s q p\n  where n=2^q is problem size (power of two) and m=2^p (power of two) is the number of threads.\n", 
	   argv[0]);
    exit(1);

   
  }


  
  
  
  

  N = 1<<atoi(argv[1]);			//raises to the power of two
  a = (int *) malloc(N * sizeof(int));	//allocates enough memory

  b = (int *) malloc(N * sizeof(int));	//allocates enough memory for copy function

  int M = 1<<atoi(argv[2]);		

  
  
  init();				//initialiazes the random array

 // print();				//prints the random array

  gettimeofday (&startwtime, NULL);
  impBitonicSort();			// runs imp bitonic
  gettimeofday (&endwtime, NULL);


  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Imperative wall clock time = %f\n", seq_time);

 // print();		//prints the now sorted array

  test();		// tests if truly in ascending order

  init();
  gettimeofday (&startwtime, NULL);
  sort();				//runs rec bitonic
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Recursive wall clock time = %f\n", seq_time);

  test();

 // print();


/////////////////////////////////////////// Rec bitonic OpenMP

init();

   

  gettimeofday (&startwtime, NULL);
  sortOpenMP(M);				//runs rec bitonic OpenMP
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Recursive OpenMP wall clock time = %f\n", seq_time);
  printf("Using %d threads", M);

  test();

 // print();


///////////////////////////////////////////// Imp bitonic OpenMP

init();				//initialiazes the random array

 // print();				//prints the random array

  chunk = (int) N / M; 

  omp_set_num_threads(M);

  gettimeofday (&startwtime, NULL);
  impBitonicSortOpenMP();			// runs imp bitonic OpenMP
  gettimeofday (&endwtime, NULL);


  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Imperative OpenMP wall clock time = %f\n", seq_time);
  printf("Using %d threads", M);

 // print();		//prints the now sorted array

  test();		// tests if truly in ascending order



////////////////////////////////////////////qsort() ordering

init();				//initialiazes the random array



  gettimeofday (&startwtime, NULL);
  qsort(a, N, sizeof(int), asc);	// runs qsort()
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("qsort() wall clock time = %f\n", seq_time);
  

 // print();		//prints the now sorted array

  test();		// tests if truly in ascending order

 

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

/** function copy()
   Makes a copy of the random array so that all comparissons are
   made with the same random elements (I added it)
**/
void copy(){
 int i;
  for (i = 0; i < N; i++) {
    b[i] = a[i]; 
  }
}


/** function bitonicMergeOpenMP()
   Uses OpenMP to parallelize the process
**/
void bitonicMergeOpenMP(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    for (i=lo; i<lo+k; i++)		
      compare(i, i+k, dir);
    bitonicMergeOpenMP(lo, k, dir);		
    bitonicMergeOpenMP(lo+k, k, dir);
    
  }
}


/** function recBitonicSortOpenMP()
   Uses OpenMP to parallelize the process
**/
void recBitonicSortOpenMP(int lo, int cnt, int dir, int threads) {
  if (cnt>1) {
    int k=cnt/2;

 omp_set_num_threads(threads); 
 
  if(threads == 1){
    
         
         qsort( a + lo, k, sizeof( int ), asc );              // --EXTRA CREDIT--
         qsort( a + ( lo + k ) , k, sizeof( int ), desc );

         //recBitonicSort(lo, cnt, dir); or this which is slower

  }
  else{ 
     
    #pragma omp parallel sections 
    {
    
    #pragma omp section 
    recBitonicSortOpenMP(lo, k, ASCENDING, threads/2);
	
    #pragma omp section	
    recBitonicSortOpenMP(lo+k, k, DESCENDING, threads - threads/2);
    
    
    }
   
  }
     
    bitonicMergeOpenMP(lo, cnt, dir);
  }
}

/** function sortOpenMP() 
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/
void sortOpenMP(int M) {
  recBitonicSortOpenMP(0, N, ASCENDING, M);
}


/*
  imperative OpenMP version of bitonic sort
*/ 
void impBitonicSortOpenMP() {

  

  int i,j,k;
 
  
 for (k=2; k<=N; k=2*k) {
    for (j=k>>1; j>0; j=j>>1) {
      #pragma omp parallel for \   
         shared(j,k) private(i) \
         schedule(static,chunk)   
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






/////////////////////////////qsort() compare function

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

