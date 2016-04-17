#ifndef WORLDCLASS_H
#define WORLDCLASS_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <vector>

//x y z (color, strength, velocity)
typedef std::vector<std::vector<std::vector<std::vector<double> > > > _3Dc;



class World
{
public:
    int expl_str; //The strength of explosion

private:
    std::vector<int> expl_coord; //Explosion coordinates

    int sign(double x);

    double srange(int x1, int y1, int z1);

    void explosion_coordinates(_3Dc &space);

    void jump_out(_3Dc &space, int x, int y, int z);

    void assign_weakness(_3Dc &space, int &x, int &y, int &z);

    std::vector<double> mass_point(_3Dc &space, int x, int y, int z);


    void assign_v(_3Dc &space, int x, int y, int z, std::vector<double> V);

    void clean_v(_3Dc &space);


public:
    World(_3Dc &shape);

    void point_strength(_3Dc &space, int x, int y, int z);

    void assign_velocities(_3Dc &space);

    void convert(_3Dc &space, std::vector<std::vector<double> >& out);
};

#endif // WORLDCLASS_H

