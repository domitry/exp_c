#include <stdio.h>
#include <math.h>
#define PI 3.14159265

/***********************************************:::
      Sin curve
:::***********************************************/

int main(){
  int i, N=512; FILE *fd;
  fd = fopen("34sin.dat", "w");
  for(i=0;i<N;i++){
    double x=i*2*PI/N;
    fprintf(fd, "%f %f\n", x, sin(x));
  }
  fclose(fd);
  return 0;
}
