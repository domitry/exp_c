#include <stdio.h>
#include <GL/glut.h>
#include "common.h"
#define WD 256
#define HT 256
#define THR 159

/***********************************************:::
    edge
:::***********************************************/

uchar *im, *binarized, *diffed;

void display(){
  glDrawPixels(WD, HT, GL_LUMINANCE, GL_UNSIGNED_BYTE, diffed);
  glutSwapBuffers();
}

void dispose(){
  free(im);
  free(binarized);
  free(diffed);
}

void keyboard(uchar key, int x, int y){
  if(key == 'q'){
    dispose();
    exit(0);
  }
}

uchar* binarize(uchar *buff, uchar thr, int size){
  int i;
  uchar *ret = (uchar *)malloc(sizeof(uchar)*size);
  for(i=0; i<size; i++)
    ret[i] = (buff[i] < thr ? 0 : 511);
  return ret;
}

// return value: number of taken neigbors ( <= 4)
int neighbor4(uchar *ret, uchar *buff, int x, int y, int width, int height){
  int dx, dy, nx, ny, cnt=0;
  uchar *seek = ret;
  
  for(dx=-1, nx=x+dx; dx<=1; nx=x+(++dx))
    for(dy=-1, ny=y+dy; dy<=1; ny=y+(++dy)){
      if(dx*dx == dy*dy)continue;
      if(nx<0 || nx>=width || ny<0 || ny>=height)continue;
      *(seek++) = buff[ny*width + nx];
      cnt++;
    }

  return cnt;
}

uchar* differentiate(uchar *buff, int width, int height){
  uchar *ret = (uchar*)malloc(sizeof(uchar)*width*height);
  uchar n4[4];
  int x, y;
  
  for(x=0; x<width; x++)
    for(y=0; y<height; y++){
      int seek = y*width + x;
      uchar self = buff[seek];
      int i, n = neighbor4(n4, buff, x, y, width, height);
      for(i=0; i<n; i++)if(self != n4[i])break;
      ret[seek] = (i==n ? 0 : 511);
    }
  return ret;
}

int main(int argc, char **argv){
  im = image_from_bin("sample.bin", WD*HT);
  binarized = binarize(im, THR, WD*HT);
  diffed = differentiate(binarized, WD, HT);
  image_to_bin(diffed, "64diffed.bin", WD*HT);
  
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
