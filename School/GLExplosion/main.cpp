#include "GLfunctions.h"
#include "World_class.h"



void cut(_3Dc &space, int x, int y, int z, int r) //This will cut a hole rxrxr
{

    for(int i = x; i < x+r; i++)
    {
        for(int j = y; j < y+r; j++)
        {
            for(int k = z; k < z+r; k++)
            {
                space[i][j][k][0] = 0;
            };
        };
    };
};

void cube(_3Dc &space, int x, int y, int z, int r) //This will place a cube rxrxr
{

    for(int i = x; i < x+r; i++)
    {
        for(int j = y; j < y+r; j++)
        {
            for(int k = z; k < z+r; k++)
            {
                space[i][j][k][0] = 1;
            };
        };
    };
};

_3Dc draw_shape(int size)
{
    _3Dc my_shape;
    my_shape.resize(size); //We're initializing shape here
    for(int i = 0; i < size; i++)
    {
        my_shape[i].resize(size);
        for(int j = 0; j < size; j++)
        {
            my_shape[i][j].resize(size);
            for(int k = 0; k < size; k++)
            {
                my_shape[i][j][k].resize(5);
                my_shape[i][j][k][0] = 0;
            };
        };
    };

    cube(my_shape, 0, 0, 4, 8);
    cube(my_shape, 7, 7, 7, 3);

    cube(my_shape, 8, 0, 16, 5);
    cube(my_shape, 7, 9, 12, 10);
    cube(my_shape, 2, 4, 20, 7);
    //cut(my_shape, 1, 0, 20, 10);
    //cut(my_shape, 7, 9, 14, 15);
    return my_shape;
};


int main(int argc, char** argv) {
    glutInit(&argc, argv);

    std::cout<<"It is better to start with (10, 7, 16) coordinate and 50 explosion strength\non this example. ";
    std::cout<<"Use right mouse button to rotate, 'SPACE' to pause and 'esc' to exit.\n\n";

    _3Dc shape = draw_shape(30);
    std::vector<std::vector<double> > Vshape;

    World world(shape);
    world.assign_velocities(shape);
    world.convert(shape, Vshape);



    OpenGL opengl(Vshape, int(shape.size()), world.expl_str);

    //10 7 16

    std::cout<<"OpenGL explosion\n";

    opengl.gl_start();
    return 0; //This line is never reached
}




//Митрохин Антон 11-1
