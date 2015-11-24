#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define PI 3.14159265

/***********************************************:::
      Sin curve with random noise
:::***********************************************/

int main(){
  int i, N=512; FILE *fd;
  fd = fopen("36sin_with_noise.dat", "w");
  for(i=0;i<N;i++){
    double x = i*2*PI/N;
    fprintf(fd, "%f %f\n", x, sin(x) + ((double)rand()/RAND_MAX-0.5)/3);
  }
  fclose(fd);
  return 0;
}
