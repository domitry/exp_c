#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define PI 3.14159265
#define T 2*PI
#define SAMPLE_N 512

typedef struct ImaginaryArray{
  double real[SAMPLE_N];
  double im[SAMPLE_N];
}ImArray;

void save_as_dat(char *fname, double *buff, int size){
  int i;
  FILE *fd;
  fd = fopen(fname, "w");
  for(i=0;i<size;i++){
    fprintf(fd, "%d %f\n", i, buff[i]);
  }
  fclose(fd);
}

double func(double x){
  int i;
  if(x<PI)return 0;
  if(x<(3*PI)/2)return x-PI;
  return -x+2*PI;
}

double dft(ImArray *input, ImArray *output, int len){
  int k,n;
  for(k=0; k<len; k++){
    output->real[k] = 0;
    output->im[k] = 0;
    for(n=0; n<len; n++){
      double a = input->real[n];
      double b = input->im[n];
      double x = (-2*PI*k*n)/len;

      output->real[k] += (a*cos(x) - b*sin(x));
      output->im[k] += (a*sin(x) + b*cos(x));
    }
  }
}

double dft_rev(ImArray *input, ImArray *output, int len){
  int k,n;
  for(k=0; k<len; k++){
    output->real[k] = 0;
    output->im[k] = 0;
    for(n=0; n<len; n++){
      double a = input->real[n];
      double b = input->im[n];
      double x = (2*PI*k*n)/len;

      output->real[k] += (a*cos(x) - b*sin(x));
      output->im[k] += (a*sin(x) + b*cos(x));
    }
    output->real[k] /= len;
    output->im[k] /= len;
  }
}

void dot(ImArray *A, ImArray *B, ImArray *out, int len){
  int i;
  for(i=0;i<len;i++){
    double a=A->real[i], b=A->im[i];
    double c=B->real[i], d=B->im[i];
    out->real[i] = a*c - b*d;
    out->im[i] = a*d + b*c;
  }
}

int main(){
  ImArray f, F, LOW_PASS, HIGH_PASS, LOW_DOT, HIGH_DOT, result_low, result_high;
  int i, WINDOW=10, UPPER_THR=3;

  // init f and h
  for(i=0;i<SAMPLE_N;i++){
    f.real[i] = func(i*2*PI/SAMPLE_N) + (((double)rand()/RAND_MAX)-0.5)/3;
    HIGH_PASS.real[i] = ((i<UPPER_THR || i>SAMPLE_N-UPPER_THR) ? 0 : 1);
    LOW_PASS.real[i] = ((i<UPPER_THR || i>SAMPLE_N-UPPER_THR) ? 1 : 0);
    f.im[i] = 0;
    HIGH_PASS.im[i] = 0;
    LOW_PASS.im[i] = 0;
  }

  dft(&f, &F, SAMPLE_N);

  dot(&F, &LOW_PASS, &LOW_DOT, SAMPLE_N);
  dot(&F, &HIGH_PASS, &HIGH_DOT, SAMPLE_N);
  dft_rev(&LOW_DOT, &result_low, SAMPLE_N);
  dft_rev(&HIGH_DOT, &result_high, SAMPLE_N);

  save_as_dat("53given_noise.dat", f.real, SAMPLE_N);
  save_as_dat("53low_pass3.dat", result_low.real, SAMPLE_N);
  save_as_dat("53high_pass3.dat", result_high.real, SAMPLE_N);

  return 0;
}
