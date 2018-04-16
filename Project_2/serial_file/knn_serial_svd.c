/*
	Parallel computing
  Project 2,HMMY Nov 2017
  Despina-Ekaterini Argiropoulos        8491
	Serial - SVD txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <float.h>
int k;
int N;
int D;
double **a;	//array random distance
double **kDist;	//array distance of k closer points
int **kId;	//array id of k closer points
void init(void);
void knn(void);
int binarySearch(double item, int low, int high, int j);
void shift(int p, int j);
struct timeval startwtime, endwtime;
double seq_time;
void validation();
/* main */
int main(int argc, char **argv) {

  if (argc != 4) {
    printf("Problem in args\n");
    exit(1);
  }
 
  k = atoi(argv[1]); 
  N = atoi(argv[2]);
  D = atoi(argv[3]);

  init();	//initialize
  gettimeofday (&startwtime, NULL);
  knn();
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6 + endwtime.tv_sec - startwtime.tv_sec);
  printf("Knn serial all clock time = %f\n", seq_time);


  int i,j;
  FILE *printfile = fopen( "nkResultsSerial_svd.txt", "w+");
  for (i = 0; i < N; i++) {             //check printf
    for (j = 0; j < k; j++) {
    fprintf(printfile, "%lf\t",kDist[j][i]);
    }
    fprintf(printfile, "\n");
  }

  printf("\n");
  validation();

}

void init() {		//initialize the data array about distance
  int i,j,x;
  a = (double **) malloc(N * sizeof(double*));  //malloc a[][]
  for(i = 0; i < N; i++) {
    a[i] = (double *) malloc(D * sizeof(double));
  }

  kDist = (double **) malloc(k * sizeof(double*));      //malloc kDist[][]
  for( i = 0; i < k; i++) {
    kDist[i] = (double *) malloc(N * sizeof(double));
  }
  kId = (int **) malloc(k * sizeof(int*));      //malloc kId[][]
  for(i = 0; i < k; i++) {
    kId[i] = (int *) malloc(N * sizeof(int));
  }
  FILE *file = fopen( "mnist_train_svd.txt", "r" );
  for (i = 0; i < N; i++) { 
    for (j = 0; j < D; j++) {
     x = fscanf(file, "%lf", &a[i][j]);
    }
  }
  fclose(file);

}

void knn() {
  int i, j, c, e, p;
  for (j = 0; j < N; j++) { 	//initialazation kDist array
   for (i = 0; i < k; i++) {
     kDist[i][j] = DBL_MAX;	//max double = DBL_MAX
     kId[i][j] = -1;
   }
  }
  for(j = 0; j < N; j++){
    for(c= 0; c < N; c++){
      if (c == j) continue;	//so not to check itself
      double dis = 0; 
      for(e = 0; e < D; e++){	//calculate distance in D space
        dis = dis + pow((a[c][e] - a[j][e]),2); 
      }
      if (dis < kDist[k-1][j]){		//sorter than the other k points or not
        p = binarySearch( dis, 0, k, j);	//where to place it
        shift(p,j);	//make space
        kDist[p][j] = dis;	//set its dist
        kId[p][j] = c;		//set its id
      }
    }
  }

}


int binarySearch(double item, int low, int high, int j)
{
    if (high <= low)
      return (item > kDist[low][j])?  (low + 1): low;
    int mid = (low + high)/2;
    if(item == kDist[mid][j])
      return mid+1;
    if(item > kDist[mid][j])
      return binarySearch(item, mid+1, high, j);
    return binarySearch(item, low, mid-1, j);
}


void shift(int p,int j){
 int i;
 for(i = k-1; i != p; i--){
   kDist[i][j] = kDist[i-1][j];
   kId[i][j] = kId[i-1][j];
 }
}


void validation(){
  FILE *fp1 = fopen( "validation_mnist_train_svd.txt", "r" );
  FILE *fp2 = fopen( "nkResultsSerial_svd.txt", "r");
  double d1,d2,dif;
  double er=0.00001;
  int i,x,y,ok=1;
//  printf("ok\n");

  for(i = 0; i < k*D; i++){
    x = fscanf(fp1,"%lf", &d1);
    y = fscanf(fp2,"%lf", &d2);
    dif=(d1 - d2);
    if (dif < 0) dif=dif*(-1);
    if(dif>er){
      ok = 0;
      break;
    } 
  }
  if(ok){
    printf("Validation done: PASSed\n");
  }
  else {
    printf("Validation done: FAILed\n");
  }
 
  fclose(fp1);
  fclose(fp2);


}


