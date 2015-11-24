#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define PI 3.14159265
#define T 2*PI
#define N 10

/***********************************************:::
      Impulse
:::***********************************************/

double func(double x){
  double ret = 2./T;
  int i;
  for(i=1; i<N; i++){
    ret += 2./T*cos(x*2*PI*i/T);
  };
  return ret;
}

int main(){
  int i, SAMPLE_N=512;
  FILE *fd;

  fd = fopen("37impulse.dat", "w");
  for(i=0;i<SAMPLE_N;i++){
    double x = i*2*PI/SAMPLE_N;
    fprintf(fd, "%f %f\n", x, func(x));
  }
  fclose(fd);
  return 0;
}
