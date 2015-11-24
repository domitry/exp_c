#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include "common.h"

#define PI 3.14159265
#define WD 64
#define HT 64
#define THR 25

/***********************************************:::
    LOW pass & HIGH pass
:::***********************************************/

typedef unsigned char uchar;
uchar *im, *result;

double dft(ImMat *input, ImMat *output){
  int u, v, len=input->width;
  output->width = input->width;
  output->height = input->height;

  for(u=0; u<input->width; u++){
    for(v=0; v<input->height; v++){
      int x, y, i = u+v*input->width;
      output->real[i] = 0;
      output->im[i] = 0;
      for(x=0; x<input->width; x++){
        for(y=0; y<input->height; y++){
          int j = x+y*input->width;
          double a = input->real[j];
          double b = input->im[j];
          double X = (-2*PI*(u*x+v*y))/len;
          output->real[i] += (a*cos(X) - b*sin(X));
          output->im[i] += (a*sin(X) + b*cos(X));
        }
      }
      output->real[i] /= len;
      output->im[i]  /= len;
    }
  }
}

double dft_rev(ImMat *input, ImMat *output){
  int u, v, len=input->width;
  output->width = input->width;
  output->height = input->height;

  for(u=0; u<input->width; u++){
    for(v=0; v<input->height; v++){
      int x, y, i = u+v*input->width;
      output->real[i] = 0;
      output->im[i] = 0;
      for(x=0; x<input->width; x++){
        for(y=0; y<input->height; y++){
          int j = x+y*input->width;
          double a = input->real[j];
          double b = input->im[j];
          double X = (2*PI*(u*x+v*y))/len;
          output->real[i] += (a*cos(X) - b*sin(X));
          output->im[i] += (a*sin(X) + b*cos(X));
        } 
     }
      output->real[i] /= len;
      output->im[i]  /= len;
    }
  }
}

void dot(ImMat *A, ImMat *B, ImMat *output){
  int i, len=A->width*A->height;
  output->width = A->width;
  output->height = A->height;

  for(i=0;i<len;i++){
    double a=A->real[i], b=A->im[i];
    double c=B->real[i], d=B->im[i];
    output->real[i] = a*c - b*d;
    output->im[i] = a*d + b*c;
  }
}

void display(){
  glDrawPixels(WD, HT, GL_LUMINANCE, GL_UNSIGNED_BYTE, result);
  glutSwapBuffers();
}

void dispose(){
  free(im);
  free(result);
}

double max_double(double *buff, int size){
  double ret=-1e30;
  int i;
  for(i=0;i<size;i++)ret=(ret < buff[i] ? buff[i] : ret);
  return ret;
}

double min_double(double *buff, int size){
  double ret=1e30;
  int i;
  for(i=0;i<size;i++)ret=(ret > buff[i] ? buff[i] : ret);
  return ret;
}

void save_as_dat(char *fname, double *buff, int size){
  int i;
  FILE *fd;
  fd = fopen(fname, "w");
  for(i=0;i<size;i++){
    fprintf(fd, "%d %f\n", i, buff[i]);
  }
  fclose(fd);
}

int main(int argc, char **argv){
  ImMat image, Im, LowPass, HighPass, HighResult, LowResult, result_high, result_low;
  uchar *abs_result;
  int i; double ABS[HT*WD];
  im = image_from_bin("resized_sample.bin", WD*HT);
  
  for(i=0; i<WD*HT; i++){
    int x=i%WD, y=i/WD;
    image.real[i] = (double)im[i];
    LowPass.real[i] = ((x>THR && x<WD-THR && y>THR && y<HT-THR) ? 0 : 1);
    HighPass.real[i] = ((x>THR && x<WD-THR && y>THR && y<HT-THR) ? 1 : 0);
    image.im[i] = 0;
    LowPass.im[i] = 0;
    HighPass.im[i] = 0;

    image.width=WD;image.height=HT;
    Im.width=WD;Im.height=HT;
    LowPass.width=WD;LowPass.height=HT;
    HighPass.width=WD;HighPass.height=HT;
  }

  dft(&image, &Im);
  dft_rev(&Im, &image);

  dot(&Im, &LowPass, &LowResult);
  dft_rev(&LowResult, &result_low);

  dot(&Im, &HighPass, &HighResult);
  dft_rev(&HighResult, &result_high);

  result = (uchar*)malloc(sizeof(uchar)*WD*HT);
  low_result = (uchar*)malloc(sizeof(uchar)*WD*HT);
  high_result = (uchar*)malloc(sizeof(uchar)*WD*HT);
  abs_result = (uchar*)malloc(sizeof(uchar)*WD*HT);


  for(i=0; i<WD*HT; i++){
    ABS[i] = sqrt(pow(Im.real[i], 2) + pow(Im.im[i], 2));
  }

  for(i=0; i<WD*HT; i++){
    low_result[i] = (uchar)result_low.real[i];
    high_result[i] = (uchar)result_low.real[i];
    
    double max_ = max_double(ABS, WD*HT);
    double min_ = min_double(ABS, WD*HT);
    abs_result[i] = (uchar)((511*(ABS[i]-min_))/(max_-min_));
  }

  image_to_bin(abs_result, "65abs.bin", WD*HT);
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WD, HT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  
  glClearColor(0, 0, 0, 0);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();

  return 0;
}
