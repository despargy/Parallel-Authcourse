/*Task 1
Paschos Nikolaos 8360
Bekos Christophoros 8311*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cilk/cilk.h>

int *a;		// array to be sorted
int N;		// number of elements

struct timeval startwtime, endwtime;
double seq_time;

const int ASCENDING  = 1;
const int DESCENDING = 0;
const int SELECTION_SORT_NUM = 32;

void init();
void test();
void print();

void qqsort();
void qqsort_recur(int low, int high);
void swap(int* a, int* b);
int partition(int low, int high);
void selection_sort(int low, int high);


int main(int argc, char **argv) {
  
	if (argc != 2) {
		printf("Usage: %s q p\n  where n=2^q is problem size (power of two) and p is num=2^p threads\n", 
		argv[0]);
		exit(1);
	}

	N = 1<<atoi(argv[1]);
	a = (int *) malloc(N * sizeof(int));

	init();
	gettimeofday (&startwtime, NULL);
	qqsort();
	gettimeofday (&endwtime, NULL);
	seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
	        + endwtime.tv_sec - startwtime.tv_sec);
	// fprintf(f,"%f\n",seq_time);
	printf("QQsort wall clock time = %f\n", seq_time);
	test();
}

void init() {
  int i;
  for (i = 0; i < N; i++) {
    a[i] = rand() % N; // (N - i);
  }
}

void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]);
  }

  printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
}


void qqsort() {
	qqsort_recur(0, N-1);
}

void swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

int partition(int low, int high) {
	int pivot = a[high];
	int lt_index = low;

	for (int i = low; i != high; ++i) {
		int cmp = a[i]<pivot;
		if (a[i] < pivot) {
			swap(a + lt_index, a + i);
			++lt_index;
		}
	}
	
	swap(a + lt_index, a + high);

	return lt_index;
}

void selection_sort(int low, int high) {
	for (int i = low; i != high; ++i) {
		int* min_ptr = a + i;
		for (int j = i+1; j != high+1; ++j) {
			if (*min_ptr > a[j]) {
				min_ptr = a + j;
			}
		}

		swap(min_ptr, a + i);
	}
}

void qqsort_recur(int low, int high) {
	if (low > high) {
		return;
	} else if (high - low < SELECTION_SORT_NUM) {
		selection_sort(low, high);
	} else {
		int partition_index = partition(low, high);

		cilk_spawn qqsort_recur(low, partition_index - 1);
		qqsort_recur(partition_index + 1, high);
		cilk_sync;
    }
}
