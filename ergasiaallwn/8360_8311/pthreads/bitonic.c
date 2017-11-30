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
#include <pthread.h>
#include <math.h>

struct timeval startwtime, endwtime;
double seq_time;


int N;          // data array size
int *a;         // data array to be sorted
int p;          // 2^p threads
int batch_size;

const int ASCENDING  = 1;
const int DESCENDING = 0;

struct pthread_data{
  int k,j,i;
};

struct pthread_rec_data{
  int id, size;
};

void init(void);
void print(void);
void sort(void);
void test(void);
inline void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void impBitonicSort(void);
int asc(const void *a, const void *b);
int desc(const void *a, const void *b);
void* pImpBSort(void* data);
void pimpBitonicSort();
void pSort();
void* pRecBitonicSort(void* data);
void* pBitonicMerge(void* data);

/** the main program **/ 
int main(int argc, char **argv) {

  if (argc != 3) {
    printf("Usage: %s q p\n  where n=2^q is problem size (power of two) and p is 2^p threads\n", 
     argv[0]);
    exit(1);
  }

  N = 1<<atoi(argv[1]);
  p = 1<<atoi(argv[2]);
  batch_size = N/p;
  printf("Running for %d size matrix and %d threads. . .\n", N,p);

  a = (int *) malloc(N * sizeof(int));

  init();
  gettimeofday( &startwtime, NULL );
  qsort( a, N, sizeof( int ), asc );
  gettimeofday( &endwtime, NULL );
  seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
  printf( "Qsort wall clock time = %f\n", seq_time );
  test();


  init();
  gettimeofday( &startwtime, NULL );
  pSort();
  gettimeofday( &endwtime, NULL );
  seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
  printf( "Recursive Pthreads Qsort wall clock time = %f\n", seq_time );
  test();

  init();
  gettimeofday (&startwtime, NULL);
  pimpBitonicSort();
  gettimeofday (&endwtime, NULL);
  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
          + endwtime.tv_sec - startwtime.tv_sec);
  printf("Parallel Imperative wall clock time = %f\n", seq_time);
  test();

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
   the parameter cnt is the number of elements to be sorted. 
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

void pSort() {
  pthread_t threads[p];
  struct pthread_rec_data prec_data[p];
  int rec_batch_size = N/p;
  // printf("Rec batch size = %d\n",rec_batch_size);
  for(int i=0; i<p; i++){
    prec_data[i].id = i;
    prec_data[i].size = rec_batch_size;
    int thread_create_status = pthread_create(&threads[i], NULL, pRecBitonicSort, (void*)&prec_data[i]);
    if(thread_create_status){
      printf("Error creating thread %d with error %d\n", i,thread_create_status);
    }
  }
  for(int i=0; i<p; i++){
    int thread_join_status = pthread_join(threads[i],NULL);
    if(thread_join_status){
      printf("Error joining thread %d with error: %d\n", i, thread_join_status);
    }
  }

  // now the array alternates between asc/desc for every N/p data
  // now bitonicMerge is called to merge the list two at a time
  for(int i=p/2; i>0; i=i>>1){
    // printf("i = %d\n",i);
    for(int j=0; j<i; j++){
      // printf("j = %d\n",j);
      prec_data[j].id = j;
      prec_data[j].size = N / i;
      // printf("size = %d\n", prec_data[j].size);
      int thread_create_status = pthread_create(&threads[j], NULL, pBitonicMerge, (void*)&prec_data[j]);
      if(thread_create_status){
        printf("Error creating thread %d with error %d\n", j,thread_create_status);
      }
    }
    for(int j=0; j<i; j++){
    int thread_join_status = pthread_join(threads[j],NULL);
      if(thread_join_status){
        printf("Error joining thread %d with error: %d\n", j, thread_join_status);
      }
    }
  }

}

void* pBitonicMerge(void* data){
  struct pthread_rec_data *pdata = (struct pthread_rec_data*) data;
  int lo = pdata->id * pdata->size;
  // printf("lo = %d\n",lo);
  bitonicMerge(lo,pdata->size, !(pdata->id % 2));
}

void* pRecBitonicSort(void* data){
  struct pthread_rec_data *pdata = (struct pthread_rec_data*) data;
  int lo = pdata->id * pdata->size;
  // printf("%d\n",lo);
  if(pdata->id % 2 == 0){
    // printf("pdataid %d\n",pdata->id);
    qsort(a+lo, pdata->size, sizeof( int ), asc );
  }
  else{
    // printf("pdataid %d\n",pdata->id);
    qsort(a+lo,pdata->size,sizeof(int),desc);
  }

}


/*
  imperative version of bitonic sort
*/
void impBitonicSort() {

  int i,j,k;
  
  for (k=2; k<=N; k=2*k) {
    for (j=k>>1; j>0; j=j>>1) {       // (j=k/2; j>0; j=j/2)
      for (i=0; i<N; i++) {           // 
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

void pimpBitonicSort() {

  int j,k,i;
  
  for (k=2; k<=N; k=2*k) {
    for (j=k>>1; j>0; j=j>>1) {       // (j=k/2; j>0; j=j/2)

      pthread_t threads[p];
      struct pthread_data data_str[p];
      for (i=0; i<p; i++) {

        //create threads with proper data
        data_str[i].k = k;
        data_str[i].i = i*batch_size;
        data_str[i].j = j;

        if((data_str[i].i+batch_size) <= N){
          int thread_create_status = pthread_create(&threads[i], NULL, pImpBSort,(void*) &data_str[i]);
          if(thread_create_status){
            printf("Error creating thread %d with error: %d\n", i,thread_create_status);
          }
        } 
        // printf("thread created with id: %d\n", i);
      }

      //join threads
      for(i=0; i<p; i++){
        int thread_join_status = pthread_join(threads[i],NULL);
        if(thread_join_status){
          printf("Error joining thread %d with error: %d\n", i, thread_join_status);
        }
      }

    }
  }
}

void* pImpBSort(void* data){
  struct pthread_data *pdata = (struct pthread_data*) data;
  for(int i=0; i<batch_size; i++){
    int pos = i + pdata->i;
    int ij=pos^pdata->j;
    if ((ij)>pos) {
      if ((pos&pdata->k)==0 && a[pos] > a[ij]) 
         exchange(pos,ij);
      if ((pos&pdata->k)!=0 && a[pos] < a[ij])
         exchange(pos,ij);
    }
  }
  pthread_exit(NULL);
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