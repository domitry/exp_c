#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define PI 3.14159265
#define T 2*PI
#define SAMPLE_N 512

double func(double x){
  int i;
  if(x<PI)return 0;
  if(x<(3*PI)/2)return x-PI;
  return -x+2*PI;
}

void average(double *out, double *arr, int range, int size){
  int i;
  for(i=0; i<size; i++){
    int cnt=0, nx;
    double sum = 0;
    for(nx=i-range; nx<=i+range; nx++){
      if(nx<0 || nx>=size)continue;
      sum+=arr[nx];
      cnt++;
    }
    out[i] = sum/cnt;
  }
}

void save_as_dat(char *fname, double *buff, int size){
  int i; FILE *fd;
  fd = fopen(fname, "w");
  for(i=0;i<size;i++){
    fprintf(fd, "%d %f\n", i, buff[i]);
  }
  fclose(fd);
}

int main(){
  int i,j, nums[]={4,10,20};
  double x[SAMPLE_N], out[SAMPLE_N];

  for(i=0;i<SAMPLE_N;i++){
    x[i] = func(i*2*PI/SAMPLE_N);
  }
  save_as_dat("51series_given.dat", x, SAMPLE_N);

  for(i=0;i<SAMPLE_N;i++){
    x[i] += (((double)rand()/RAND_MAX)-0.5)/3;
  }
  save_as_dat("51series_given_noise.dat", x, SAMPLE_N);

  for(j=0; j<3; j++){
    int num = nums[j]; char fname[100];
    sprintf(fname, "51averaged%d.dat", num);
    average(out, x, num, SAMPLE_N);
    save_as_dat(fname, out, SAMPLE_N);
  }
}
