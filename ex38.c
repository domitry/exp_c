#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define PI 3.14159265
#define T 2*PI

/***********************************************:::
      Fourier series
:::***********************************************/

double an(int n){
  int k=n/2;
  return ((n%2||!(k%2)) ? 0 : -4./(PI*n*n));
}

double bn(int n){
  int k=n/2;
  if(n%2)return (2.*(k%2 ? 1 : -1))/(PI*n*n);
  return 0;
}

double func(double x, int N){
  double ret = PI/8;
  int i;
  for(i=1; i<N; i++){
    ret += an(i)*cos(i*x);
    ret += bn(i)*sin(i*x);
  }
  return ret;
}

int main(){
  int i, j, SAMPLE_N=512, nums[] = {10, 100, 1000};
  double x; char fname[50]; FILE *fd;

  for(j=0; j<3; j++){
    int num = nums[j];
    sprintf(fname, "38series%d.dat", num);
    
    fd = fopen(fname, "w");
    for(i=0;i<SAMPLE_N;i++){
      x = i*2*PI/SAMPLE_N;
      fprintf(fd, "%f %f\n", x, func(x, num));
    }
    fclose(fd);
  }
  return 0;
}
