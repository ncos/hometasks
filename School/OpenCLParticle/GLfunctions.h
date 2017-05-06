#ifndef GLFUNCTIONS_H
#define GLFUNCTIONS_H

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>




void handleKeypress(unsigned char key, int x, int y);

void Sphere(int x, int y, int z, int r);

void initRendering();

void handleResize(int w, int h);

extern int old_x;
extern int dx;
extern double _dx;
void motionFunc(int x, int y);

void mouseFunc(int button, int state, int x, int y);



#endif // GLFUNCTIONS_H
