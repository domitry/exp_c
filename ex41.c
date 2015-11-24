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
  if(x<PI)return 0;
  if(x<(3*PI)/2)return x-PI;
  return -x+2*PI;
}

double func2(double x){
  return sin(x);
}

void dft(ImArray *input, ImArray *output, int len){
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

void dft_rev(ImArray *input, ImArray *output, int len){
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

int main(){
  ImArray input, output, reinput, redft;
  int i;

  // sample set
  for(i=0;i<SAMPLE_N;i++){
    input.real[i] = func2(i*2*PI/SAMPLE_N);// + (((double)rand()/RAND_MAX)-0.5)/3;
    input.im[i] = 0;
  }

  save_as_dat("42series_given_noise.dat", input.real, SAMPLE_N);

  dft(&input, &output, SAMPLE_N);
  save_as_dat("42_real_part.dat", output.real, SAMPLE_N);
  save_as_dat("42_imaginary_part.dat", output.im, SAMPLE_N);

  dft_rev(&output, &reinput, SAMPLE_N);
  save_as_dat("42rev_dfted.dat", reinput.real, SAMPLE_N);

  if(1){
    double error[SAMPLE_N];
    for(i=0;i<SAMPLE_N;i++){
      error[i] = reinput.real[i] - input.real[i];
    }
    save_as_dat("42error.dat", error, SAMPLE_N);
  }

  dft(&output, &redft, SAMPLE_N);
  save_as_dat("42re_dfted.dat", redft.real, SAMPLE_N);
  
  return 0;
}
