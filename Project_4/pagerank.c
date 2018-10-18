/*
	Parallel computing
  Project 4,HMMY July 2018
  Despina-Ekaterini Argiropoulos        8491
  Pagerank Gauss-Seidel method Serial
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>
#include<math.h>
#include<float.h>
#define DAMP 0.85 //google uses 0.85, else might be 0.7
#define AE 1e-8  //acceptable error

int N;
int *nin, *nout, **d;
float *a;
float CONST; 
double seq_time;
struct timeval startwtime, endwtime;
void init();
void matrix_data();
void save_results();
int gauss();
void free_all();

/*main*/
int main(int argc, char **argv){
  if (argc != 2) {
    printf("Error in num of args\n");
    exit(1);
  }
  N = atoi(argv[1]);
  CONST = (1-DAMP)/N;
  init();	//initiallizations
  printf("init DONE\n");
  matrix_data();	//read from file, save to matrix
  printf("matrix_data DONE\n");
  gettimeofday (&startwtime, NULL);
  gauss();	//Gauss Seidel method for pagerank
  gettimeofday (&endwtime, NULL);

  seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6 + endwtime.tv_sec - startwtime.tv_sec);
  printf("Pagerank Gauss-Seidel serial all clock time = %f\n", seq_time);

  save_results(); //print results to file
  free_all();	
  return 0;
}

void init(){
  int i,j;
  nin = (int *) malloc(N * sizeof(int));
  nout = (int *) malloc(N * sizeof(int));
  d = (int **) malloc(N * sizeof(int*));      
  for(i = 0; i < N; i++) {
    d[i] = (int *) malloc( sizeof(int));
    nin[i] = 0;	
    nout[i] = 0;
  }
  a = (float *) malloc(N * sizeof(float));
  for(i = 0; i < N; i++) {
    a[i] = 0;
  }
}

void matrix_data(){
  FILE *fp = fopen("hollins.dat","r");
  int i,j;
  int from,to;
  char line[1000];

  if (fp == NULL){
    printf("Error opening data file\n"); 
    exit(1);}
  while(!feof(fp)){
   fgets(line, sizeof(line), fp);
   if(!feof(fp)) {
     if (strncmp(line, "#", 1) != 0) {
       sscanf(line,"%d\t%d\n", &from,&to);
       nin[to]++;	//count num of in connections
       nout[from]++;	//count num of out connections
       d[to] = (int *)realloc(d[to],sizeof(int)*nin[to]);	
       d[to][nin[to]-1] = from;	//save node id "from" that is connected to "to"
     }
   }
  }
  printf("End of reading file\n");
 /*
   Adjust in case that same node has no connections to other
   Equal probability for each on
   all zeros --> 1/N
 */
  for(i = 0; i < N; i++){
    if (nout[i] == 0) {
      nout[i] = N;
      for(j = 0; j < N; j++){
        nin[j]++;
        d[j] = (int *)realloc(d[j],sizeof(int)*nin[j]);
        d[j][nin[j]-1] = i;
      }
    }
    printf("checkofnout line=%d\n",i);
  }
  fclose(fp);
}
int gauss()
{
    double  t,s, e = DBL_MAX;
    int i,j,r = 0;
    while(e>AE){	//stop when error is acceptable
      r++;	//num of iterations
      printf("r=%d\n",r);
      for(i=0;i<N;i++){
          s=0;
          for(j=0;j<nin[i];j++){
             s+= -DAMP*a[d[i][j]]/nout[d[i][j]] ;	//fix const of matrix
          }
          t=(CONST-s);	// divited with fact of i, here is always 1
          e=fabs(a[i]-t);	//compute error
          a[i]=t;	//store new value
      }
    }
    printf(" Converses in %3d iteration\n", r);
}

void save_results(){
  FILE *fp = fopen( "pagerankResultsSerial.txt", "w+");
  int i;

  for (i = 0; i < N; i++) {             
    fprintf(fp, "%lf\n",a[i]);
  }
  fprintf(fp, "\n");
  fclose(fp);
}

void free_all(){
  int i;
  for(i = 0; i < N; i++){
    free(d[i]);
  }
  free(a);
  free(d);
  free(nin);
  free(nout);
}
