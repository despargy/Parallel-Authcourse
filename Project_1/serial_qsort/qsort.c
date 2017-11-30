#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

struct timeval startwtime, endwtime;
double seq_time;


int N;          // data array size
int *a;         // data array to be sorted

void init(void);
void print(void);
void test(void);
int cmpfunc (const void * b, const void * c);

/**  main  **/ 
int main(int argc, char **argv) {

  if (argc != 2) {
    printf("Usage: %s q\n  where n=2^q is problem size (power of two)\n", 
	   argv[0]);
    exit(1);
  }

  N = 1<<atoi(argv[1]);
  a = (int *) malloc(N * sizeof(int));
  
  init();
  gettimeofday (&startwtime, NULL);
  qsort(a,N,sizeof(int),cmpfunc);
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

  printf("Wall clock time = %f\n", seq_time);

  test();

}

void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]);
  }

  printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
}


void init() {
  int i;
  for (i = 0; i < N; i++) {
    a[i] = rand() % N; // (N - i);
  }
}


void print() {
  int i;
  for (i = 0; i < N; i++) {
    printf("%d\n", a[i]);
  }
  printf("\n");
}
int cmpfunc (const void * b, const void * c) {
   return ( *(int*)b - *(int*)c );
}
