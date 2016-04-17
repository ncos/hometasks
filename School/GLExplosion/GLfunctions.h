#ifndef GLFUNCTIONS_H
#define GLFUNCTIONS_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

//x y z (color, strength, velocity)
typedef std::vector<std::vector<std::vector<std::vector<double> > > > _3Dc;

extern double old_x;
extern double dx;
extern double _dx;
extern int pause;
extern int size;
extern int e_strength;

extern std::vector<std::vector<double> > space;




class OpenGL
{
public:
    OpenGL(std::vector<std::vector<double> > &shape, int sz, int e_str);

    static void handleKeypress(unsigned char key, int x, int y);

    static void Cube(double x, double y, double z, double r);

    static void initRendering();

    static void handleResize(int w, int h);

    static void motionFunc(int x, int y);

    static void mouseFunc(int button, int state, int x, int y);

    static void set_color(int col);

    static void weakness_out(int wk);

    static void drawScene();

    static void gl_start();
};




#endif // GLFUNCTIONS_H
