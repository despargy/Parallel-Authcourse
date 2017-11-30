/* Filename:bitonicCilkRecursive.c
 *
 * Author: Stavros Papadopoulos
 *
 * Summary:This file contains the implementation of recursive version of Bitonic Sort
 * with the the usage and techincs of Cilk Plus.
 *
 * It is actually a modified "parallelized" version of the "serial" algorithm of bitonic Sort ,written by
 * professor Nikos Pitsianis.
 *
 *filesource: https://elearning.auth.gr/pluginfile.php/740503/mod_workshop/instructauthors/bitonic.c
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <cilc/cilc.h>
#include <sys/time.h>


struct timeval startwtime, endwtime;
double seq_time;

/*struct Data {

    int lo,cnt,dir;

};*/



int N;          // data array size
int *a;         // data array to be sorted

/*int num_of_threads;
int active_threads;*/

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







/** the main program **/
int main(int argc, char **argv){
   if (argc != 2) {
        printf("Usage: %s q\n  where n=2^q is problem size (power of two)\n",
               argv[0]);
        exit(1);
    }

    //num_of_threads=1<<atoi(argv[2]);
    N = 1<<atoi(argv[1]);
    a = (int *) malloc(N * sizeof(int));



    init();
    gettimeofday (&startwtime, NULL);)
    sort();

	//PrecBitonicSort1();

    gettimeofday (&endwtime, NULL);

    seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
                        + endwtime.tv_sec - startwtime.tv_sec);

    printf("recursive wall clok time= %f\n", seq_time);

    //print();

    test();




   // pthread_exit(NULL);

}


/** -------------- SUB-PROCEDURES  ----------------- **/

/** procedure test() : verify sort results **/
void test() {
    int pass = 1;
    int i;
    for (i = 1; i < N; i++) {
        pass &= (a[i-1] <= a[i]);
    }

    //printf(" TEST %s\n",(pass) ? "PASSed" : "FAILed");
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
        cilk_spawn recBitonicSort(lo, k, ASCENDING);
        recBitonicSort(lo+k, k, DESCENDING);
        cilk_sync;
        bitonicMerge(lo, cnt, dir);
    }
}


















