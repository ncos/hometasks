#include "World_class.h"

int World::sign(double x)
{
 if (x>0) return 1;
    if (x<0) return -1;
    return 0;
};

double World::srange(int x1, int y1, int z1) //Squared distance to the explosion
{
    return (pow((x1-expl_coord[0]),2.0) + pow((y1-expl_coord[1]),2.0) + pow((z1-expl_coord[2]),2.0));
};

void World::explosion_coordinates(_3Dc &space)
{
    std::cout<<"Please input the coordinate of explosion\n";
    std::cout<<"Input x coordinate:";
    std::cin>>expl_coord[0];
    std::cout<<"\nInput y coordinate:";
    std::cin>>expl_coord[1];
    std::cout<<"\nInput z coordinate:";
    std::cin>>expl_coord[2];
    std::cout<<"\nInput explosion strength:";
    std::cin>>expl_str;
    std::cout<<"\n";

    //expl_coord[0] = 10;
    //expl_coord[1] = 7;
    //expl_coord[2] = 16;
    //expl_str = 50;

    space[expl_coord[0]][expl_coord[1]][expl_coord[2]][0] = 78;
    space[expl_coord[0]][expl_coord[1]][expl_coord[2]][1] = 1;
};



//-----------------------------------------------------------------------------------
void World::jump_out(_3Dc &space, int x, int y, int z)
{


    for(int i = x-1; i <= x+1; i++)
    {
        for(int j = y-1; j <= y+1; j++)
        {
            for(int k = z-1; k <= z+1; k++)
            {
                if((i<int(space.size()))&&(i>=0)&&(j<int(space.size()))&&(j>=0)&&(k<int(space.size()))&&(k>=0))
                {//std::cout<<i<<j<<k<<"\n";

                    if (space[i][j][k][1] == 0) 
                    {
                        space[i][j][k][2] = 3;
                        //point_strength(space, primary + 1, i, j, k);
                    };
                };
            };
        };
    };

    space[x][y][z][2] = 0;

    for(int i = x-1; i <= x+1; i++)
    {
        for(int j = y-1; j <= y+1; j++)
        {
            for(int k = z-1; k <= z+1; k++)
            {
                if((i<int(space.size()))&&(i>=0)&&(j<int(space.size()))&&(j>=0)&&(k<int(space.size()))&&(k>=0))
                {//std::cout<<i<<j<<k<<"\n";

                    if ((space[i][j][k][1] != 0)&&(space[i][j][k][2] ==3))
                    {
                        jump_out(space, i, j, k);
                    };
                };
            };
        };
    };

};

void World::assign_weakness(_3Dc &space, int &x, int &y, int &z)
{
    //std::cout<<"REached1."<<x<<" "<<y<<" "<<z<<"\n";
    space[x][y][z][3] = 1; //Let us know that we've visited this cell

    int N = 0; //Number of previous-level neighbours
    for(int i = x-1; i <= x+1; i++)
    {
        for(int j = y-1; j <= y+1; j++)
        {
            for(int k = z-1; k <= z+1; k++)
            {
                if((i<int(space.size()))&&(i>=0)&&(j<int(space.size()))&&(j>=0)&&(k<int(space.size()))&&(k>=0))
                {
                    if ((space[i][j][k][1] != 0)&&(space[i][j][k][2] !=3)) //Scanning previous level
                    {
                        int dam = 1; //Damping coefficient
                        if(space[i][j][k][0] != 0){dam = 2;};
                        N = N+1;
                        space[x][y][z][1] = space[x][y][z][1] + space[i][j][k][1]*dam;

                        //The bigger is a number - the stronger is a point
                    };
                };
            };
        };
    };

    space[x][y][z][1] = space[x][y][z][1]/N;


    for(int i = x-1; i <= x+1; i++)
    {
        for(int j = y-1; j <= y+1; j++)
        {
            for(int k = z-1; k <= z+1; k++)
            {
                if((i<int(space.size()))&&(i>=0)&&(j<int(space.size()))&&(j>=0)&&(k<int(space.size()))&&(k>=0))
                {
                    if (space[i][j][k][2] == 3) //If there is a blue cell nearby
                    {
                        if (space[i][j][k][3] != 1)
                        {
                            assign_weakness(space, i, j, k);
                            x = i;
                            y = j;
                            z = k;
                        };
                    };
                };
            };
        };
    };
};
//-----------------------------------------------------------------------------------


std::vector<double> World::mass_point(_3Dc &space, int x, int y, int z)
{
    space[x][y][z][0] = 2; //Coloring point blue
    std::vector<double> ms_pnt (4, 0);

    ms_pnt[0] = x;
    ms_pnt[1] = y;
    ms_pnt[2] = z;
    ms_pnt[3] = 1;  //Number of cells


    //And now we'll call this function recursively to all black(non-weak, non-empty) points
    for(int i = x-1; i <= x+1; i++)        //which are situated nearby
    {
        for(int j = y-1; j <= y+1; j++)
        {
            for(int k = z-1; k <= z+1; k++)
            {
                if((i<int(space.size()))&&(i>=0)&&(j<int(space.size()))&&(j>=0)&&(k<int(space.size()))&&(k>=0))
                {
                    if(space[i][j][k][0] == 1)
                    {
                        //std::cout<<new_point[0]<<new_point[1]<<new_point[2]<<"\n";
                        if (space[i][j][k][1] >= expl_str)
                        {
                            std::vector<double> ms_pnt_ (4, 0);

                            ms_pnt_ = mass_point(space, i, j, k);
                            ms_pnt[0] = ms_pnt[0] + ms_pnt_[0];
                            ms_pnt[1] = ms_pnt[1] + ms_pnt_[1];
                            ms_pnt[2] = ms_pnt[2] + ms_pnt_[2];
                            ms_pnt[3] = ms_pnt[3] + ms_pnt_[3];

                        };
                    };
                };
            };
        };
    };

    return ms_pnt;
};


void World::assign_v(_3Dc &space, int x, int y, int z, std::vector<double> V)
{
    space[x][y][z][0] = 78; //Coloring point

    for(int i = x-1; i <= x+1; i++)
    {
        for(int j = y-1; j <= y+1; j++)
        {
            for(int k = z-1; k <= z+1; k++)
            {
                if((i<int(space.size()))&&(i>=0)&&(j<int(space.size()))&&(j>=0)&&(k<int(space.size()))&&(k>=0))
                {//std::cout<<i<<j<<k<<"\n";
                    if(space[i][j][k][0] == 2)
                    {
                        //std::cout<<new_point[0]<<new_point[1]<<new_point[2]<<"\n";
                        if (space[i][j][k][1] >= expl_str)
                        {
                            space[i][j][k][2] = V[0];
                            space[i][j][k][3] = V[1];
                            space[i][j][k][4] = V[2];
                            assign_v(space, i, j, k, V);
                        };
                    };
                };
            };
        };
    };
};

void World::clean_v(_3Dc &space)
{
    for (int i = 0; i<int(space.size()); i++)
    {
        for (int j = 0; j<int(space.size()); j++)
        {
            for (int k = 0; k<int(space.size()); k++)
            {
                space[i][j][k][2] = 0;
                space[i][j][k][3] = 0;
                space[i][j][k][4] = 0;
            };
        };
    };
};


World::World(_3Dc &shape)
{
    expl_str = 0;
    expl_coord.resize(3, 0);

    explosion_coordinates(shape);

    point_strength(shape, expl_coord[0], expl_coord[1], expl_coord[2]);
    clean_v(shape);
};  


void World::point_strength(_3Dc &space, int x, int y, int z)
{
    while(x < int(space.size()-1))
    {
        jump_out(space, x, y, z);
        assign_weakness(space, x, y, z);
    };
};


void World::assign_velocities(_3Dc &space)
{
    for (int i = 0; i<int(space.size()); i++)
    {
        for (int j = 0; j<int(space.size()); j++)
        {
            for (int k = 0; k<int(space.size()); k++)
            {
                if (space[i][j][k][0] == 1)
                {

                    if (space[i][j][k][1] <= expl_str)
                    {
                        std::vector<double> V (3, 0);
                        std::vector<double> r (3, 0);
                        std::vector<double> V_ (4, 0);

                        V_ = mass_point(space, i,j,k); //Highlighting a particular object with a blue color and returning it's masspoint

                        for (int t = 0; t < 3; t++){
                            if (V_[t]/V_[3] - expl_coord[t]){r[t] = (V_[t]/V_[3] - expl_coord[t]);}
                            else {r[t] = 0.0001;};
                        };

                        V[0] = pow(expl_str, 0.2)/(r[0]*pow(V_[3], 0.1)*90);
                        V[1] = pow(expl_str, 0.2)/(r[1]*pow(V_[3], 0.1)*90);
                        V[2] = pow(expl_str, 0.2)/(r[2]*pow(V_[3], 0.1)*90);


                        if (V_[3] != 1){
                            std::cout<<"Object...\n";
                            std::cout<<"Masspoint:"<<V_[0]/V_[3]<<";"<<V_[1]/V_[3]<<";"<<V_[2]/V_[3]<<"\n";
                            std::cout<<"Velocity:"<<V[0]<<";"<<V[1]<<";"<<V[2]<<"\n";
                            std::cout<<"N = "<<V_[3]<<"\n\n";
                        };


                        //The first object will not be processed by the assign_v function; hence, we are to do it
                        space[i][j][k][2] = -V[0]*space[i][j][k][1];
                        space[i][j][k][3] = -V[1]*space[i][j][k][1];
                        space[i][j][k][4] = -V[2]*space[i][j][k][1];



                        V[0] = V[0]*20;
                        V[1] = V[1]*20;
                        V[2] = V[2]*20;
                        assign_v(space, i,j,k, V);
                    };
                };          
            };
        };
    };
};

void World::convert(_3Dc &space, std::vector<std::vector<double> >& out)
{
    //std::cout<<"CONVERTING...\n";
    for (int i = 0; i<int(space.size()); i++)
    {
        for (int j = 0; j<int(space.size()); j++)
        {
            for (int k = 0; k<int(space.size()); k++)
            {
                if (space[i][j][k][0] != 0)
                {
                    std::vector<double> point (7, 0);

                    //Coordinates:
                    point[0] = i;
                    point[1] = j;
                    point[2] = k;

                    //Velocity:
                    point[3] = space[i][j][k][2];
                    point[4] = space[i][j][k][3];
                    point[5] = space[i][j][k][4];

                    //Strength(color)
                    point[6] = space[i][j][k][1];
                    out.push_back(point);

                };
            };
        };
    };
};

