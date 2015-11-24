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

int abs_int(int a){
  return a<0?-a:a;
}

int guard(int x, int min, int max){
  if(x<min)return min;
  if(x>max)return max;
  return x;
}

// *** Handle only the real part of the given array ***
void fold(ImArray *a, ImArray *b, ImArray *output, int len, int window){
  int i, n, dx;
  if(window<0)window=len;
  for(n=0; n<len; n++){
    output->real[n] = 0;
    for(i=-window; i<=window; i++){
      output->real[n] += b->real[abs_int(i)]*a->real[guard(n-i, 0, len-1)];
    }
  }
}

int main(){
  ImArray f, h, F, H, FH, result_folding, result_dft;
  int i, WINDOW=10;

  for(i=0;i<SAMPLE_N;i++){
    f.real[i] = func(i*2*PI/SAMPLE_N) + (((double)rand()/RAND_MAX)-0.5)/3;
    h.real[i] = (i<WINDOW ? 1./(2*WINDOW) : 0);
    f.im[i] = 0;
    h.im[i]= 0;
  }

  save_as_dat("52series_given_noise.dat", f.real, SAMPLE_N);
  
  fold(&f, &h, &result_folding, SAMPLE_N, WINDOW);
  save_as_dat("52result_folding.dat", result_folding.real, SAMPLE_N);
  
  for(i=0;i<SAMPLE_N;i++)h.real[i]*=2; //dirty hack

  dft(&h, &H, SAMPLE_N);
  dft(&f, &F, SAMPLE_N);

  for(i=0;i<SAMPLE_N;i++){
    double a=H.real[i], b=H.im[i];
    double c=F.real[i], d=F.im[i];
    FH.real[i] = a*c - b*d;
    FH.im[i] = a*d + b*c;
  }
  
  dft_rev(&FH, &result_dft, SAMPLE_N);
  
  save_as_dat("52result_dft.dat", result_dft.real, SAMPLE_N);

  return 0;
}
