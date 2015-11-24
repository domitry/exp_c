#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

#define PI 3.14159265
#define WD 256
#define HT 256

#define THR 128
#define REV 1.0
#define NON_REV -1.0
//#define FILTER_SQUARE

/***********************************************:::
    LOW pass & HIGH pass -- reduced order
:::***********************************************/

typedef unsigned char uchar;

typedef struct ImaginaryMatrix{
  double *real;
  double *im;
  int width;
  int height;
}ImMat;

void init_mat(ImMat *mat, int w, int h){
  mat->real = (double *)malloc(sizeof(double)*w*h);
  mat->im = (double *)malloc(sizeof(double)*w*h);
  mat->width = w;
  mat->height = h;
}

uchar *im, *result;

void transpose(ImMat *in, ImMat *out){
  int w=in->width, h=in->height, x, y;
  for(y=0; y<h; y++)
    for(x=0; x<w; x++){
      int src=y*w+x, dst=x*w+y;
      out->real[dst]=in->real[src];
      out->im[dst]=in->im[src];
    }
}

void dft(double *in_real, double *in_im, double *out_real, double *out_im, int len, double sign){
  int k,n;
  for(k=0; k<len; k++){
    out_real[k] = 0.;
    out_im[k] = 0.;
    for(n=0; n<len; n++){
      double a = in_real[n], b = in_im[n];
      double x = sign*(2*PI*k*n)/len;
      double cos_ = cos(x), sin_ = sin(x);
      out_real[k] += (a*cos_ - b*sin_);
      out_im[k] += (a*sin_ + b*cos_);
    }
  }
}

void dft2(ImMat *in, ImMat *out){
  ImMat W, tW, tOut;
  int w=in->width, h=in->height, y;
  init_mat(&W, w, h); init_mat(&tW, w, h); init_mat(&tOut, w, h);
  
  for(y=0; y<h; y++)
    dft(&(in->real[y*w]), &(in->im[y*w]), &(W.real[y*w]), &(W.im[y*w]), w, NON_REV);
  transpose(&W, &tW);

  for(y=0; y<h; y++)
    dft(&(tW.real[y*w]), &(tW.im[y*w]), &(tOut.real[y*w]), &(tOut.im[y*w]), w, NON_REV);
  transpose(&tOut, out);
}

void dft2_rev(ImMat *in, ImMat *out){
  ImMat W, tW, tOut;
  int w=in->width, h=in->height, x, y;
  init_mat(&W, w, h); init_mat(&tW, w, h); init_mat(&tOut, w, h);
  
  for(y=0; y<h; y++)
    dft(&(in->real[y*w]), &(in->im[y*w]), &(W.real[y*w]), &(W.im[y*w]), w, REV);
  transpose(&W, &tW);
  for(y=0; y<h; y++)
    dft(&(tW.real[y*w]), &(tW.im[y*w]), &(tOut.real[y*w]), &(tOut.im[y*w]), w, REV);
  transpose(&tOut, out);

  double wh=w*h;
  for(y=0; y<h; y++)
    for(x=0; x<w; x++){
      out->real[y*w+x]/=wh;
      out->im[y*w+x]/=wh;
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

uchar* image_from_bin(char *fname, int size){
  FILE *fd;
  uchar *ret = (uchar*)malloc(sizeof(uchar)*size);
  fd = fopen(fname, "rb");
  fread(ret, 1, size, fd);
  fclose(fd);
  return ret;
}

void image_to_bin(uchar* buff, char *fname, int size){
  FILE *fd;
  fd = fopen(fname, "wb");
  fwrite(buff, 1, size, fd);
  fclose(fd);
}

void display(){
  glDrawPixels(WD, HT, GL_LUMINANCE, GL_UNSIGNED_BYTE, result);
  glutSwapBuffers();
}

void reshape(int w, int h){
  glViewport(0,0,(GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void dispose(){
  free(im);
  free(result);
}

void keyboard(uchar key, int x, int y){
  if(key == 'q'){
    dispose();
    exit(0);
  }
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

void export_mat(ImMat *mat, char *fname, int size){
  int i; uchar arr[size];
  for(i=0; i<size; i++)arr[i] = (uchar)mat->real[i];
  image_to_bin(arr, fname, size);
}

int main(int argc, char **argv){
  ImMat image, mat, LowPass, HighPass;
  ImMat HighResult, LowResult, high_result, low_result;
  int i;
  double ABS[HT*WD];
  
  result = (uchar*)malloc(sizeof(uchar)*WD*HT);
  im = image_from_bin("sample.bin", WD*HT);

  init_mat(&image, WD, HT); init_mat(&mat, WD, HT);
  init_mat(&LowPass, WD, HT); init_mat(&HighPass, WD, HT);
  init_mat(&LowResult, WD, HT); init_mat(&HighResult, WD, HT);
  init_mat(&low_result, WD, HT); init_mat(&high_result, WD, HT);

  for(i=0; i<WD*HT; i++){
    int x=i%WD, y=i/WD;
    image.real[i] = (double)im[i];
    image.im[i] = 0.0;
    #ifdef FILTER_SQUARE
    LowPass.real[i] = ((x>THR && x<WD-THR && y>THR && y<HT-THR) ? 0 : 1);
    #else
    LowPass.real[i] = (sqrt(pow(x-WD/2,2) + pow(y-HT/2, 2)) > THR ? 0 : 1);
    #endif
    HighPass.real[i] = (LowPass.real[i] == 0 ? 1 : 0);
  } 
  
  dft2(&image, &mat);
  dft2_rev(&mat, &image);
  dot(&mat, &LowPass, &LowResult);
  dft2_rev(&LowResult, &low_result);
  
  dot(&mat, &HighPass, &HighResult);
  dft2_rev(&HighResult, &high_result);

  for(i=0; i<WD*HT; i++){
    ABS[i] = log(sqrt(pow(mat.real[i], 2) + pow(mat.im[i], 2)));
  }

  export_mat(&LowPass, "66low_filter.bin", WD*HT);
  export_mat(&HighPass, "66high_filter.bin", WD*HT);
  export_mat(&image, "66inv.bin", WD*HT);
  export_mat(&low_result, "66low.bin", WD*HT);
  export_mat(&high_result, "66high.bin", WD*HT);

  for(i=0; i<WD*HT; i++){
    result[i] = (uchar)image.real[i];
    //double max_ = max_double(ABS, WD*HT);
    //double min_ = min_double(ABS, WD*HT);
    //result[i] = (uchar)((255*(ABS[i]-min_))/(max_-min_));
  }
  image_to_bin(result, "66idtfed.bin", HT*WD);
  
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
