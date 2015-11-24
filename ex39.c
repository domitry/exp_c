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

double func_(double x){
  if(x<PI)return 0;
  if(x<(3*PI)/2)return x-PI;
  return -x+2*PI;
}

int main(){
  int i, SAMPLE_N=512, num;
  FILE *fd;
  fd = fopen("39error.dat", "w");
  
  for(num=10; num<1000; num+=10){
    double sum=0;
    for(i=0;i<SAMPLE_N;i++){
      double x=i*2*PI/SAMPLE_N + 1.0;
      sum += abs(func(x, num) - func_(x));
    }
    sum = func(PI/4, num) - func_(PI/4);
    fprintf(fd, "%d %f\n", num, sum);
  }
  fclose(fd);
  return 0;
}
