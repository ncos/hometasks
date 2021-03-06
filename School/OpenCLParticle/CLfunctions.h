#ifndef CLFUNCTIONS_H
#define CLFUNCTIONS_H

#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <stdlib.h>

#include <CL/cl2.hpp>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/gl.h>


#include "pandatype.h"


//Helper functions
void oclErrorString(cl_int error);


GLuint CreateVBO(const void* data, int dataSize, GLenum target, GLenum usage);


void getDevInfo(cl::Device device);


class World
{
private:
	//list of devices, context object, queue and kernel
	std::vector<cl::Device> devices;
	cl::Context context;
	cl::CommandQueue queue;
	cl::Kernel kernel;

public:
	size_t num;
	float dt;
	GLuint gl_p_vbo;
	GLuint gl_v_vbo;
	GLuint gl_r_vbo;
	GLuint gl_m_vbo;
	GLuint gl_q_vbo;
	cl::Buffer cl_a;
	float a[10];
	
private:
	std::vector<cl::Memory> cl_vbos;
	std::vector<float> pos;
	std::vector<float> vel;
	std::vector<float> r;
	std::vector<float> m;
	std::vector<float> q;

public:
	void WorldInit();
	
private:
	cl::Kernel CreateKernel(std::string kernel_path);
	
public:
	void AppendNewObject(panda Panda);
	void LoadEverybody(cl::Kernel kernel,
					   std::vector<float> pos, 
					   std::vector<float> vel,
					   std::vector<float> r,
					   std::vector<float> m,
					   std::vector<float> q);

	void ConfigureApplication(std::string kernel_path);
	void StartApplication();
};


#endif // CLFUNCTIONS_H
