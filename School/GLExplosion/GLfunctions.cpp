#include "GLfunctions.h"


double old_x;
double dx;
double _dx;
int pause = 1;
int size = 0;
int e_strength = 0;

std::vector<std::vector<double> > space;


OpenGL::OpenGL(std::vector<std::vector<double> > &shape, int sz, int e_str)
{
    space = shape;
    size = sz;
    e_strength = e_str;
};

void OpenGL::handleKeypress(unsigned char key, int x, int y) {    //The current mouse coordinates
    switch (key) {
        case 27: //Escape key
            exit(0); //Exit the program
        case 32:
            pause = -pause;
            //std::cout<<pause<<"\n";
    }
}

void OpenGL::Cube(double x, double y, double z, double r)
{
    glTranslatef(x, y, z);
    glutSolidCube(r);
    glTranslatef(-x, -y, -z);
};

void OpenGL::initRendering()
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

void OpenGL::handleResize(int w, int h) {
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

void OpenGL::motionFunc(int x, int y)
{
    dx = x - old_x;
    glRotated((360.0*dx/2513.0), 0, 1, 0);
    old_x = x;
};

void OpenGL::mouseFunc(int button, int state, int x, int y)
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

void OpenGL::set_color(int col)
{
    glColor3f(0.9, 0.9, 0.9);
    if (col == 1) glColor4f(0.6, 0.3, 0.3, 0.9);//Orange
    if (col == 78) glColor3f(1.0, 0.0, 0.0);//Red
    if (col == 2) glColor4f(0.0, 0.0, 1.0, 0.1);//Blue
    if (col == 3) glColor3f(0.0, 1.0, 0.0);//Green
    if (col == 69) glColor3f(1.0, 1.0, 0.0);//Weak points
};

void OpenGL::weakness_out(int wk)
{
    float R = 0;
    float G = 0;
    float B = 0;


    //int col = 999*wk/max_wk;

    //B = (col%10)/10.0;
    //G = (int(col/10)%10)/10.0;
    //R = (int(col/100)%10)/10.0;

    if (wk>=e_strength){R = 1;};
    if ((wk<e_strength)&&(wk>=e_strength/2)){B = 1;};
    if (wk<e_strength/2){G = 1;};

    //std::cout<<R<<G<<B;
    glColor3f(R, G, B);
};

void OpenGL::drawScene() 
{
    //Clear information from last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Acceleration after rotation
    glRotated((360.0*_dx/2513.0), 0, 1, 0);
    _dx = _dx/1.05; //1.05 is a very important constant. Don't change it

    //Cube
    for (int i = 0; i<int(space.size()); i++)
    {
        //set_color(space[i][j][k][0]);
        weakness_out(space[i][6]);
        double expander = 75.0/size;
        //std::cout<<expander;
        Cube(space[i][0]*expander, space[i][1]*expander, space[i][2]*expander, expander);

        if (pause == -1){
            space[i][0] = space[i][0] + space[i][3];
            space[i][1] = space[i][1] + space[i][4];
            space[i][2] = space[i][2] + space[i][5];



            //Deleting...

            if((space[i][0]>300)||(space[i][0]<-300)||(space[i][1]>300)||(space[i][1]<-300)||(space[i][2]>300)||(space[i][2]<-300))
            {
                space.erase(space.begin()+i);
            };
        };
        //std::cout<<space[i][0]<<"\n";
        //std::cout<<space[i][3]<<"\n";
        //std::cout<<"\n";

    };

    //Wire cube
    glColor3f(1.0, 1.0, 1.0);
    glutWireCube(150);

    ////OpenCL objects


    glutSwapBuffers(); //Send the 3D scene to the screen
};

void OpenGL::gl_start()
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(955, 0);
    glutInitWindowSize(960, 1000); //Set the window size

    //Create the window
    glutCreateWindow("OpenGL Explosion");
    initRendering(); //Initialize rendering

    //Adjust properties
    glutFullScreen(); //Make window as big as it possible


    //Set handler functions for drawing, keypresses, window resizes, etc.
    glutDisplayFunc(drawScene);
    glutIdleFunc(drawScene);


    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);

    std::cout<<"Press SPACE to continue\n";
    glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
};

