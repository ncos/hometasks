#include "GLfunctions.h"

int old_x = 0;
int dx = 0;
double _dx = 0.0;

void handleKeypress(unsigned char key, int x, int y) {    //The current mouse coordinates
    switch (key) {
        case 27: //Escape key
        exit(0); //Exit the program
    }
}

void Sphere(int x, int y, int z, int r)
{
    glTranslatef(x, y, z);
    glutSolidSphere(r, 100, 100);
    glTranslatef(-x, -y, -z);
};

void initRendering()
{
    glClearColor(0.9, 0.9, 0.9, 1.0);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0,0,-400);
};

void handleResize(int w, int h) {
    //Tell OpenGL how to convert from coordinates to pixel values
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective

    //Set the camera perspective
    glLoadIdentity(); //Reset the camera
    gluPerspective(45.0,                  //The camera angle
                   (double)w / (double)h, //The width-to-height ratio
                   1.0,                   //The near z clipping coordinate
                   800.0);                //The far z clipping coordinate
                   glMatrixMode(GL_MODELVIEW);
};

void motionFunc(int x, int y)
{
    dx = x - old_x;
    glRotated((360.0*dx/2513.0), 0, 1, 0);
    old_x = x;
};

void mouseFunc(int button, int state, int x, int y)
{
    if ((button == GLUT_RIGHT_BUTTON)||(button == GLUT_LEFT_BUTTON))
    {
        if (state == GLUT_DOWN)
        {
            old_x = x;
            _dx = 0.0;
            glutSetCursor(GLUT_CURSOR_SPRAY);
        }
        else
        {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
            if ((dx <= 3)&&(dx >= -3)) dx = 0;
            _dx = dx;
            dx = 0;
        };
    };
};

void drawScene()
{
    //Clear information from last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Acceleration after rotation
    glRotated((360.0*_dx/2513.0), 0, 1, 0);
    _dx = _dx/1.05;

    //Sphere
    //glColor3f(1.0, 1.0, 1.0);
    //Sphere(20, 0, 0, 2);


    //Wire cube
    glColor3f(1.0, 1.0, 1.0);
    glutWireCube(150);

    ////OpenCL objects
    glPointSize(5.);

    glColor3f(0.0, 0.0, 1.0);
    //world.StartApplication();
    //printf("vertex buffer\n");
    //glBindBuffer(GL_ARRAY_BUFFER, world.p_vbo);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    //printf("enable client state\n");
    glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_COLOR_ARRAY);

    //Need to disable these for blender
    glDisableClientState(GL_NORMAL_ARRAY);

    //printf("draw arrays\n");
    //glDrawArrays(GL_POINTS, 0, world.num);

    //printf("disable stuff\n");
    //glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glutSwapBuffers(); //Send the 3D scene to the screen
};

