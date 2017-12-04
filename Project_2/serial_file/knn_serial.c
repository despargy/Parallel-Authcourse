/*
	Parallel computing
  Project 2,HMMY Nov 2017
  Despina-Ekaterini Argiropoulos        8491

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
//char *name;
void init(void);
void knn(void);
int binarySearch(int item, int low, int high, int j);
void shift(int p, int j);
/* main */
int main(int argc, char **argv) {

  if (argc != 4) {
    printf("Problem in args\n");
    exit(1);
  }
 
  k = atoi(argv[1]); 
  N = atoi(argv[2]);
  D = atoi(argv[3]);
//  name ="corpus.txt";
//  printf("%s",name);
  a = (double **) malloc(N * sizeof(double*));	//malloc a[][]
  for(int i = 0; i < N; i++) {
    a[i] = (double *) malloc(D * sizeof(double));
  }

  kDist = (double **) malloc(k * sizeof(double*));	//malloc kDist[][]
  for(int i = 0; i < k; i++) {
    kDist[i] = (double *) malloc(N * sizeof(double));
  }
  kId = (int **) malloc(k * sizeof(int*));	//malloc kId[][]
  for(int i = 0; i < k; i++) {
    kId[i] = (int *) malloc(N * sizeof(int));
  }


  init();	//instead of data file

  knn();




/*  printf("\n");
  for (int i = 0; i < k; i++) {             //check printf
    for (int j = 0; j < N; j++) {
     printf("%lf\t", kDist[i][j]);
    }
    printf("\n");
  }
  printf("\n");
   for (int i = 0; i < k; i++) {             //check printf
    for (int j = 0; j < N; j++) {
     printf("%d\t", kId[i][j]);
    }
    printf("\n");
  }
  printf("\n");

*/
int i,j;
FILE *printfile = fopen( "kDresultsArray.txt", "w+");
  for (i = 0; i < N; i++) {             //check printf
    for (j = 0; j < k; j++) {
    fprintf(printfile, "%lf\t",kDist[j][i]);
    }
    fprintf(printfile, "\n");
  }




}

void init() {		//initialize the data array about distance
  int i,j;
  FILE *file = fopen( "corpus.txt", "r" );
  for (i = 0; i < N; i++) { 
    for (j = 0; j < D; j++) {
     fscanf(file, "%lf", &a[i][j]);
    }
  }
  fclose(file);
/*FILE *printfile = fopen( name, "w");
  for (i = 0; i < N; i++) {		//check printf
    for (j = 0; j < D; j++) {
    fprintf(printfile, "%lf\t",&a[i][j]);
    }
    fprintf(printfile, "\n");
  }*/
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


int binarySearch(int item, int low, int high, int j)
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
