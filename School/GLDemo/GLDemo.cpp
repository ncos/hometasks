#include <utility>
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

#include "GLfunctions.h"

int main(int argc, char** argv) 
{ 
    //Initialize GLUT
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(955, 0);
    glutInitWindowSize(960, 1000); //Set the window size

    //Create the window
    glutCreateWindow("OpenGL Explosion");
    initRendering(); //Initialize rendering

    //Adjust properties
    //glutFullScreen(); //Make window as big as it possible


    //Set handler functions for drawing, keypresses, window resizes, etc.
    glutDisplayFunc(drawScene);
    glutIdleFunc(drawScene);
    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);

    glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
    return 0; //This line is never reached
}
