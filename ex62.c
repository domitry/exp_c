#include <stdio.h>
#include <GL/glut.h>
#define WD 256
#define HT 256
#define THR 159

/***********************************************:::
    binarize image and show it
:::***********************************************/

typedef unsigned char uchar;
uchar *im, *binarized;

uchar* image_from_bin(char *fname, int size){
  FILE *fd;
  uchar *ret = (uchar*)malloc(sizeof(uchar)*size);
  fd = fopen(fname, "rb");
  fread(ret, 1, size, fd);
  fclose(fd);
  return ret;
}

void image_to_bin(uint* buff, char *fname, int size){
  FILE *fd;
  fd = fopen(fname, "wb");
  fwrite(buff, 1, size, fd);
  fclose(fd);
}

void display(){
  glDrawPixels(WD, HT, GL_LUMINANCE, GL_UNSIGNED_BYTE, binarized);
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
  free(binarized);
}

void keyboard(uchar key, int x, int y){
  if(key == 'q'){
    dispose();
    exit(0);
  }
}

uchar* binarize(uchar* buff, uchar thr, int size){
  int i;
  uchar *ret = (uchar *)malloc(sizeof(uchar)*size);
  for(i=0; i<size; i++)
    ret[i] = (buff[i] < thr ? 0 : 511);
  return ret;
}

int main(int argc, char **argv){
  im = image_from_bin("sample.bin", WD*HT);
  binarized = binarize(im, THR, WD*HT);
  image_to_bin(binarized, "62binarized.bin", WD*HT);
  
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
