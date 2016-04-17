#include "GLfunctions.h"
#include "CLfunctions.h"
#include "pandatype.h"




//Creating our cute little pandas (:
panda Pinky, Blacky, Pawy, Cuddly;
//And a nice place for them to live
World world;


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
	world.StartApplication();
    //printf("vertex buffer\n");
    glBindBuffer(GL_ARRAY_BUFFER, world.gl_p_vbo);
    glVertexPointer(3, GL_FLOAT, 0, 0);



    //printf("enable client state\n");
    glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_COLOR_ARRAY);

    //Need to disable these for blender
    glDisableClientState(GL_NORMAL_ARRAY);






    //printf("draw arrays\n");
    glDrawArrays(GL_POINTS, 0, world.num);

    //printf("disable stuff\n");
    //glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);


	glutSwapBuffers(); //Send the 3D scene to the screen
};


void create_pandas()
{
	//Pinky
	Pinky.position.x = 0;
	Pinky.position.y = 0;
	Pinky.position.z = 0;

	Pinky.speed.x = 0;
	Pinky.speed.y = 0;
	Pinky.speed.z = 0;

	Pinky.weight = 2;
	Pinky.glamor = 1;

	Pinky.color.R = 0.176;
	Pinky.color.G = 0.48;
	Pinky.color.B = 0.96;


	//Blacky
	Blacky.position.x = 50;
	Blacky.position.y = -20;
	Blacky.position.z = 0;

	Blacky.speed.x = -50;
	Blacky.speed.y = 20;
	Blacky.speed.z = 0;

	Blacky.weight = 2;
	Blacky.glamor = 1;

	Blacky.color.R = 0;
	Blacky.color.G = 0;
	Blacky.color.B = 0;


	//Pawy
	Pawy.position.x = -40;
	Pawy.position.y = 0;
	Pawy.position.z = 0;

	Pawy.speed.x = 10;
	Pawy.speed.y = 0;
	Pawy.speed.z = 0;

	Pawy.weight = 10;
	Pawy.glamor = 0.001;

	Pawy.color.R = 0.184;
	Pawy.color.G = 0.134;
	Pawy.color.B = 0.11;



	//Cuddly
	Cuddly.position.x = 50;
	Cuddly.position.y = 40;
	Cuddly.position.z = -40;

	Cuddly.speed.x = 3;
	Cuddly.speed.y = 5;
	Cuddly.speed.z = 1;

	Cuddly.weight = 3;
	Cuddly.glamor = 0;

	Cuddly.color.R = 0.376;
	Cuddly.color.G = 0.48;
	Cuddly.color.B = 0.96;
};


void random_panda(int num)
{
	for (int i = 0; i < num; i++)
	{
		panda Panda;
		Panda.position.x=rand() % 120 -60;
		Panda.position.y=rand() % 120 -60;
		Panda.position.z=rand() % 120 -60;

		Panda.speed.x=float(rand() % 1400 -700)/10.0;
		Panda.speed.y=float(rand() % 1400 -700)/10.0;
		Panda.speed.z=float(rand() % 1400 -700)/10.0;

		world.AppendNewObject(Panda);
	};
};


int main(int argc, char** argv) {
	srand ( time(NULL) );

	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(960, 1000); //Set the window size
	
	//Create the window
	glutCreateWindow("OpenCLPanda");
	initRendering(); //Initialize rendering

	//Adjust properties
	//glutFullScreen(); //Make window as big as it possible
	
	//Set up OpenCL
	
	world.WorldInit();



	create_pandas();
	world.AppendNewObject(Pinky); //Configuring new object
	world.AppendNewObject(Blacky); //Configuring new object
	//world.AppendNewObject(Pawy); //Configuring new object
	//world.AppendNewObject(Cuddly); //Configuring new object
	//random_panda(100);


	world.ConfigureApplication("My_kernel.cl");

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

