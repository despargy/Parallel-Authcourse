/*
  
 Parallel computing: pThreads method
   based in the serial programme bitonic.c
   
  Project 1,HMMY Nov 2017
  Despina-Ekaterini Argiropoulos        8491
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>	//include pthread
struct timeval startwtime, endwtime;
double seq_time;


int N;          // data array size
int nofT;	// number of Threads
int *a;         // data array to be sorted

int T=1;	//counter of opened pthreads, starts from the master one
struct param{	//struct so to pass args to recBitonicSort
  pthread_t thread;
  int arg1; 
  int arg2;
  int arg3; 
};

const int ASCENDING  = 1;
const int DESCENDING = 0;


void init(void);
void print(void);
void sort(void);
void test(void);
void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(struct param *incParam);

/** the main program **/ 
int main(int argc, char **argv) {

  if (argc != 3) {
    printf("Usage: %s q\n  where n=2^q is problem size (power of two)\n", 
	   argv[0]);
    printf("  where n=2^p is num of threads [1-8]\n");

    exit(1);
  }

  N = 1<<atoi(argv[1]);
  nofT = atoi(argv[2]);
  a = (int *) malloc(N * sizeof(int));

  init();
  gettimeofday (&startwtime, NULL);
  sort();
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Recursive wall clock time = %f\n", seq_time);

  test();

  pthread_exit(NULL);
  free(a);	
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
    its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them en the same order 
    if can open one more (except the master), calls the recursively sorting in parallel
    else in serial as bitonic.c
 **/
void recBitonicSort(struct param *incParam) {
  if ((*incParam).arg2>1) {
    int k=((*incParam).arg2)/2;
    struct param call1,call2;
    call1.arg1 = (*incParam).arg1;
    call1.arg2 = k;
    call1.arg3 = ASCENDING;
    call2.arg1 = (*incParam).arg1+k;
    call2.arg2 = k;
    call2.arg3 = DESCENDING;

    if((T+1)>nofT){	// check if can open another thread: limit nofT, counter of threads T 
      recBitonicSort(&call1);
      recBitonicSort(&call2);
      bitonicMerge((*incParam).arg1, (*incParam).arg2, (*incParam).arg3);
    }
    else{
     T++;
     int rc1,rc2;
     (void)  pthread_create(&call1.thread, NULL,(void *) recBitonicSort, (void *) &call1);
     (void)  pthread_create(&call2.thread, NULL,(void *) recBitonicSort,(void *) &call2);
     (void)  pthread_join(call1.thread, NULL);
     (void)  pthread_join(call2.thread, NULL);
     bitonicMerge((*incParam).arg1, (*incParam).arg2, (*incParam).arg3);

    } 
  }

}


/** function sort() 
   Caller of recBitonicSort for sorting the entire array of length N 
   in ASCENDING order
 **/
void sort() {
 struct param firstcall;
 firstcall.arg1=0;
 firstcall.arg2=N;
 firstcall.arg3= ASCENDING;
 recBitonicSort(&firstcall);
}


