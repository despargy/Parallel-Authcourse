/* Filename:pThreadsRecursiveBitonicSort.c
 *
 * Author: Stavros Papadopoulos
 *
 * Summary:This file contains the implementation of recursive version of Bitonic Sort
 * with the the usage and techincs of pThreads.
 *
 * It is actually a modified "parallelized" version of the "serial" algorithm of bitonic Sort written by
 * professor Nikos Pitsianis.
 *
 *filesource: https://elearning.auth.gr/pluginfile.php/740503/mod_workshop/instructauthors/bitonic.c
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

struct timeval startwtime, endwtime;
double seq_time;

struct Data {

    int lo,cnt,dir;

};



int N;          // data array size
int *a;         // data array to be sorted

int num_of_threads;
int active_threads;

const int ASCENDING  = 1;
const int DESCENDING = 0;


void init(void);
void print(void);
//void sort(void);
void test(void);
inline void exchange(int i, int j);
void compare(int i, int j, int dir);
void bitonicMerge(int lo, int cnt, int dir);
void recBitonicSort(int lo, int cnt, int dir);
void PrecBitonicSort1();
void *PrecBitonicSort(void *td);
void *PbitonicMerge(void *td);
pthread_mutex_t mutexsum;






/** the main program **/
int main(int argc, char **argv){




    if (argc != 3) {
        printf("Usage: %s q\n  where n=2^q is problem size (power of two)\n",
               argv[0]);
        exit(1);
    }

    num_of_threads=1<<atoi(argv[2]);
    N = 1<<atoi(argv[1]);
    a = (int *) malloc(N * sizeof(int));



    init();
    gettimeofday (&startwtime, NULL);

	PrecBitonicSort1();

    gettimeofday (&endwtime, NULL);

    seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
                        + endwtime.tv_sec - startwtime.tv_sec);

    printf("%f\n", seq_time);

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
        recBitonicSort(lo, k, ASCENDING);
        recBitonicSort(lo+k, k, DESCENDING);
        bitonicMerge(lo, cnt, dir);
    }
}




void *PrecBitonicSort(void *td) {


    struct Data *temp;

    temp=(struct Data *)td;
    if ((temp->cnt) > 1 ){
        if (active_threads<num_of_threads) {
            pthread_mutex_lock (&mutexsum);

            active_threads=active_threads+2;

            pthread_mutex_unlock (&mutexsum);

    struct Data td1;


    struct Data td2;


    int k=temp->cnt/2;


    td1.lo=temp->lo;
    td1.cnt=k;
    td1.dir=ASCENDING;
    td2.lo=temp->lo+k;
    td2.cnt=k;
    td2.dir=DESCENDING;
    pthread_t thread1, thread2, thread3;



        pthread_create(&thread1,NULL, PrecBitonicSort, (void *)&td1);
        pthread_create(&thread2,NULL, PrecBitonicSort, (void *)&td2);

            pthread_join(thread1,NULL);
            pthread_join(thread2,NULL);

            pthread_mutex_lock (&mutexsum);

            active_threads=active_threads-2;

           pthread_mutex_unlock (&mutexsum);


           /* pthread_create(&thread3, NULL, bitonicMerge1,(void *)domi);

            pthread_join(thread3,NULL);

            pthread_mutex_lock (&mutexsum);

            counter=counter-1;

            pthread_mutex_unlock (&mutexsum);
			*/

            PbitonicMerge(temp);
        }




    else{
            recBitonicSort(temp->lo, temp->cnt,temp->dir);
        }

    }
return 0;
}

void *PbitonicMerge(void *td ) {
    struct Data *temp;

    temp=(struct Data *)td;

    if ((temp->cnt)>1) {
        if (active_threads<num_of_threads) {

        pthread_mutex_lock (&mutexsum);

        active_threads=active_threads+2;

        pthread_mutex_unlock (&mutexsum);

        struct Data td1,td2;

        int k=temp->cnt/2;


        pthread_t thread1, thread2;




        int i;
        for (i=temp->lo; i<temp->lo+k; i++)
            compare(i, i+k, temp->dir);

        td1.lo=temp->lo;
        td1.cnt=k;
        td1.dir=temp->dir;
        td2.lo=temp->lo+k;
        td2.cnt=k;
        td2.dir=temp->dir;


        pthread_create(&thread1, NULL, PbitonicMerge, (void *)&td1);
        pthread_create(&thread2, NULL, PbitonicMerge, (void *)&td2);
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);

       pthread_mutex_lock (&mutexsum);

        active_threads=active_threads-2;

       pthread_mutex_unlock (&mutexsum);

        }

        else {
                bitonicMerge(temp->lo, temp->cnt, temp->dir);


        }
    }

return 0;
}

void PrecBitonicSort1(){

  pthread_mutex_init(&mutexsum, NULL);
  active_threads=0;
  struct Data data;
    data.lo = 0;
    data.cnt = N;
    data.dir = ASCENDING;

  PrecBitonicSort(&data);

}










