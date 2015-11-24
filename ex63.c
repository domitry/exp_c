#include <stdio.h>
#include <GL/glut.h>
#define WD 256
#define HT 256
#define NEIGHBOR 3

/***********************************************:::
    average
:::***********************************************/

typedef unsigned char uchar;
uchar *im, *averaged;

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
  glDrawPixels(WD, HT, GL_LUMINANCE, GL_UNSIGNED_BYTE, averaged);
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
  free(averaged);
}

void keyboard(uchar key, int x, int y){
  if(key == 'q'){
    dispose();
    exit(0);
  }
}

// return value: number of taken neigbors
int neighbors(uchar *ret, uchar *buff, int x, int y, int range, int width, int height){
  int nx, ny, cnt=0;
  uchar *seek = ret;
  
  for(nx=x-range; nx<=x+range; nx++)
    for(ny=y-range; ny<=y+range; ny++){
      if(nx<0 || nx>=width || ny<0 || ny>=height)continue;
      *(seek++) = buff[ny*width + nx];
      cnt++;
    }
  return cnt;
}

uchar* average(uchar* buff, int range, int width, int height){
  int x, y;
  uchar ns[(2*range+1)*(2*range+1)];
  uchar *ret = (uchar*)malloc(sizeof(uchar)*width*height);
  
  for(x=0; x<width; x++)
    for(y=0; y<height; y++){
      int sum=0, i;
      int n = neighbors(ns, buff, x, y, range, width, height);
      for(i=0; i<n; i++)sum+=ns[i];
      ret[y*width + x] = (uchar)((double)sum/n);
    }
  
  return ret;
}

int main(int argc, char **argv){
  im = image_from_bin("sample.bin", WD*HT);
  averaged = average(im, NEIGHBOR, WD, HT);
  image_to_bin(averaged, "63averaged.bin", WD*HT);
  
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
