#ifndef PANDATYPE_H
#define PANDATYPE_H


struct coordinate
{
	double x;
	double y;
	double z;
};


struct rgb
{
	float R;
	float G;
	float B;
	rgb()
	{
		R = 0;
		G = 0;
		B = 0;
	};
};


struct panda
{
	coordinate position;
	coordinate speed;
	float weight;
	float size;
	float glamor;
	rgb color;
	panda()
	{
		weight = 1;
		size = 1;
		glamor = 0;
	};
};


#endif // PANDATYPE_H
