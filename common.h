#include <stdio.h>
#include <GL/glut.h>
typedef unsigned char uchar;

typedef struct ImaginaryMatrix{
  double real[WD*HT];
  double im[WD*HT];
  int width;
  int height;
}ImMat;

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

void reshape(int w, int h){
  glViewport(0,0,(GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}
