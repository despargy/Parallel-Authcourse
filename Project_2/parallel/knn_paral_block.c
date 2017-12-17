/*
	Parallel computing
  Project 2,HMMY Nov 2017
  Despina-Ekaterini Argiropoulos        8491
  BLOCK- Parallel
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <float.h>
#include <mpi.h>
int k;
int N;
int chunk;
int D;
double **mainMatrix;	//main matrix
double **compMatrix;	//matrix to help us compaire
double **bufferMatrix;
double **bufferkDist;
double **kDist;	//array distance of k closer points
int **kId;	//array id of k closer points
void init(void);
void knn(void);
int binarySearch(double item, int low, int high, int j);
void shift(int p, int j);
int id_p;
int num_p;
struct timeval startwtime, endwtime;
double seq_time;
void store_to_file();
void validation();
/* main */
int main(int argc, char **argv) {

  if (argc != 4) {
    printf("Problem in args\n");
    exit(1);
  }
  MPI_Init(NULL, NULL); 
  k = atoi(argv[1]); 
  N = atoi(argv[2]);
  D = atoi(argv[3]);

  init();	//initialazation
  MPI_Barrier(MPI_COMM_WORLD);
  if( id_p == 0) gettimeofday (&startwtime, NULL);
  knn();	//knn function
  MPI_Barrier(MPI_COMM_WORLD);
  if (id_p == 0) { 
    gettimeofday (&endwtime, NULL);
    seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6 + endwtime.tv_sec - startwtime.tv_sec);
  printf("Knn mpi-block wall clock time = %f\n", seq_time);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  store_to_file();
  MPI_Barrier(MPI_COMM_WORLD);
  if (id_p == (num_p -1)) validation();
  MPI_Barrier(MPI_COMM_WORLD);


MPI_Finalize();

}

void init() {		//initialize the data array about distance

  MPI_Comm_size(MPI_COMM_WORLD, &num_p);
  MPI_Comm_rank(MPI_COMM_WORLD, &id_p);
  printf("From rank %d" " out of %d processors\n", id_p, num_p);
  chunk = N/num_p;
  mainMatrix = (double **) malloc(chunk * sizeof(double*));	//malloc a[][]
  for(int i = 0; i < chunk; i++) {
    mainMatrix[i] = (double *) malloc(D * sizeof(double));
  }
  compMatrix = (double **) malloc(chunk * sizeof(double*));      //malloc a[][]
  for(int i = 0; i < chunk; i++) {
    compMatrix[i] = (double *) malloc(D * sizeof(double));
  }
  bufferMatrix = (double **) malloc(chunk * sizeof(double*));     //malloc a[][]
  for(int i = 0; i < chunk; i++) {
    bufferMatrix[i] = (double *) malloc(D * sizeof(double));
  }
  kDist = (double **) malloc(k * sizeof(double*));	//malloc kDist[][]
  for(int i = 0; i < k; i++) {
    kDist[i] = (double *) malloc(chunk * sizeof(double));
  }
  kId = (int **) malloc(k * sizeof(int*));	//malloc kId[][]
  for(int i = 0; i < k; i++) {
    kId[i] = (int *) malloc(chunk * sizeof(int));
  }
  bufferkDist = (double **) malloc(k * sizeof(double*));      //malloc kDist[][]
  for(int i = 0; i < k; i++) {
    bufferkDist[i] = (double *) malloc(chunk * sizeof(double));
  }



  FILE *infileptr;
  int x;
  int i,j,z;
  MPI_Status status;
  if(id_p == (num_p -1)){
    infileptr = fopen("corpus.txt","r");
    for(i = 0; i < num_p - 1; i++){
      //Read from file to buffer instead of freadf()
      for(z = 0; z<chunk; z++) {
        for(j = 0; j<D; j++) {
          x = fscanf(infileptr,"%lf\t",&bufferMatrix[z][j]);
        }
        x = fscanf(infileptr,"\n");
      }
      /////////////
      MPI_Send(&bufferMatrix[0][0], chunk*(D+2), MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
    }

   //Read from file
    for(z = 0; z<chunk; z++) {
      for(j = 0; j<D; j++) {
        x = fscanf(infileptr,"%lf\t",&mainMatrix[z][j]);
      }
      x = fscanf(infileptr,"\n");
    }
    fclose(infileptr);
   ////////////
  }  
  else {
    MPI_Recv(&mainMatrix[0][0], chunk*(D+2), MPI_DOUBLE, num_p-1, 1, MPI_COMM_WORLD, &status );
  }

  //initialazation kDist array
  for (j = 0; j < chunk; j++) {
   for (i = 0; i < k; i++) {
     kDist[i][j] = DBL_MAX;     //max double = DBL_MAX
     kId[i][j] = -1;
   }
  }
  ///////////////


  //initialazation compMatrix
  for(z = 0; z<chunk; z++) {
    for(j = 0; j<D; j++) {
     compMatrix[z][j] = mainMatrix[z][j];
    } 
  }

  /////////////////////////
  
}

void knn() {
  int i, j, c, e, p, t;
  int next, prev;
  MPI_Status status;
  for(t = 0; t < num_p ; t++){
    for(j = 0; j < chunk; j++){
      for(c= 0; c < chunk; c++){
        double dis = 0; 
        for(e = 0; e < D; e++){	//calculate distance in D space
          dis = dis + pow((compMatrix[c][e] - mainMatrix[j][e]),2); 
        }
        if(j==c && !t) continue;
        if (dis < kDist[k-1][j]){		//sorter than the other k points or not
          p = binarySearch( dis, 0, k, j);	//where to place it
          shift(p,j);	//make space
          kDist[p][j] = dis;	//set its dist
          kId[p][j] = c;		//set its id
        }
      }
    }
    if(t == (num_p-1)) continue;
    next = id_p + 1;
    prev = id_p - 1;
    if (id_p == 0) prev = (num_p-1);
    if (id_p == (num_p-1)) next = 0;
    if (id_p == 0){
      MPI_Send(&compMatrix[0][0], chunk*(D+2), MPI_DOUBLE, next, 1, MPI_COMM_WORLD);
      MPI_Recv(&bufferMatrix[0][0], chunk*(D+2), MPI_DOUBLE, prev, 1, MPI_COMM_WORLD, &status) ;
    }
    else{
      MPI_Recv(&bufferMatrix[0][0], chunk*(D+2), MPI_DOUBLE, prev, 1, MPI_COMM_WORLD, &status) ;
      MPI_Send(&compMatrix[0][0], chunk*(D+2), MPI_DOUBLE, next, 1, MPI_COMM_WORLD);
    }
    for(i = 0; i<chunk; i++) {
      for(j = 0; j<D; j++) {
       compMatrix[i][j] = bufferMatrix[i][j];
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


void store_to_file(){
  int x;
  int i,j,z;
  FILE *fp;
  MPI_Status status;
  if(id_p == (num_p -1)){
    fp = fopen("nkResultsBlock.txt","w+");
    for(i = 0; i < num_p - 1; i++){
      MPI_Recv(&bufferkDist[0][0], (chunk+1)*(k)+3, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &status );
      for(z = 0; z<chunk; z++) {
        for(j = 0; j<k; j++) {
          x = fprintf(fp,"%lf\t",bufferkDist[j][z]);
        }
        x = fprintf(fp,"\n");
      }
    }
    for(z = 0; z<chunk; z++) {
      for(j = 0; j<k; j++) {
        x = fprintf(fp,"%lf\t",kDist[j][z]);
       }
      x = fprintf(fp,"\n");
    }
    fclose(fp);
  }
  else {
    MPI_Send(&kDist[0][0], (chunk+1)*(k)+3, MPI_DOUBLE, (num_p-1), 1, MPI_COMM_WORLD);
  }

}


void validation(){
  FILE *fp1 = fopen( "validated.txt", "r" );
  FILE *fp2 = fopen( "nkResultsBlock.txt", "r");
  double d1,d2,dif;
  int count=0;
  double er=0.00001;
  int i,x,y,ok=1;
  printf("ok\n");

  for(i = 0; i < k*N; i++){
    x = fscanf(fp1,"%lf", &d1);
    y = fscanf(fp2,"%lf", &d2);
    dif=(d1 - d2);
    if (dif < 0) dif=dif*(-1);
    if(dif>er){
      ok = 0;
 //     break;
      count++;
      printf("%d\n",i);
    }
  }
  if(ok){
    printf("Validation done: PASSed\n");
  }
  else {
    printf("Validation done: FAILed\n");
    printf("%d\n",count);
  }

  fclose(fp1);
  fclose(fp2);


}

