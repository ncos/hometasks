#include "CLfunctions.h"

//Helper functions
void oclErrorString(cl_int error)
{
    static const std::string errorString[] = {
        "CL_SUCCESS",
        "CL_DEVICE_NOT_FOUND",
        "CL_DEVICE_NOT_AVAILABLE",
        "CL_COMPILER_NOT_AVAILABLE",
        "CL_MEM_OBJECT_ALLOCATION_FAILURE",
        "CL_OUT_OF_RESOURCES",
        "CL_OUT_OF_HOST_MEMORY",
        "CL_PROFILING_INFO_NOT_AVAILABLE",
        "CL_MEM_COPY_OVERLAP",
        "CL_IMAGE_FORMAT_MISMATCH",
        "CL_IMAGE_FORMAT_NOT_SUPPORTED",
        "CL_BUILD_PROGRAM_FAILURE",
        "CL_MAP_FAILURE",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "CL_INVALID_VALUE",
        "CL_INVALID_DEVICE_TYPE",
        "CL_INVALID_PLATFORM",
        "CL_INVALID_DEVICE",
        "CL_INVALID_CONTEXT",
        "CL_INVALID_QUEUE_PROPERTIES",
        "CL_INVALID_COMMAND_QUEUE",
        "CL_INVALID_HOST_PTR",
        "CL_INVALID_MEM_OBJECT",
        "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
        "CL_INVALID_IMAGE_SIZE",
        "CL_INVALID_SAMPLER",
        "CL_INVALID_BINARY",
        "CL_INVALID_BUILD_OPTIONS",
        "CL_INVALID_PROGRAM",
        "CL_INVALID_PROGRAM_EXECUTABLE",
        "CL_INVALID_KERNEL_NAME",
        "CL_INVALID_KERNEL_DEFINITION",
        "CL_INVALID_KERNEL",
        "CL_INVALID_ARG_INDEX",
        "CL_INVALID_ARG_VALUE",
        "CL_INVALID_ARG_SIZE",
        "CL_INVALID_KERNEL_ARGS",
        "CL_INVALID_WORK_DIMENSION",
        "CL_INVALID_WORK_GROUP_SIZE",
        "CL_INVALID_WORK_ITEM_SIZE",
        "CL_INVALID_GLOBAL_OFFSET",
        "CL_INVALID_EVENT_WAIT_LIST",
        "CL_INVALID_EVENT",
        "CL_INVALID_OPERATION",
        "CL_INVALID_GL_OBJECT",
        "CL_INVALID_BUFFER_SIZE",
        "CL_INVALID_MIP_LEVEL",
        "CL_INVALID_GLOBAL_WORK_SIZE",
    };

    const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

    const int index = -error;

    std::string err = (index >= 0 && index < errorCount) ? errorString[index] : "";

	if (err != "CL_SUCCESS")
	{
		std::cout<<"WARNING: "<<err<<"\n";
	};

};


GLuint CreateVBO(const void* data, int dataSize, GLenum target, GLenum usage)
{
    GLuint id = 0;  // 0 is reserved, glGenBuffersARB() will return non-zero id if success

    glGenBuffers(1, &id);                        // create a vbo
    glBindBuffer(target, id);                    // activate vbo id to use
    glBufferData(target, dataSize, data, usage); // upload data to video card

    // check data size in VBO is same as input array, if not return 0 and delete VBO
    int bufferSize = 0;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
    if(dataSize != bufferSize)
    {
        glDeleteBuffers(1, &id);
        id = 0;
        std::cout << "[createVBO()] Data size is mismatch with input array\n";
        //printf("[createVBO()] Data size is mismatch with input array\n");
    }
    //this was important for working inside blender!
    glBindBuffer(target, 0);
    return id;      // return VBO id
};


void getDevInfo(cl::Device device)
{
	std::string output_str;
	std::vector<size_t> WIS;


	device.getInfo(CL_DEVICE_NAME, &output_str);
	std::cout<<"Device name is "<<output_str<<"\n";

	device.getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &WIS);
	std::cout<<"Device maximum work item sizes: ["<<WIS[0]<<", "<<WIS[0]<<", "<<WIS[0]<<"]\n";

	device.getInfo(CL_DEVICE_VENDOR, &output_str);
	std::cout<<"Device vendor is "<<output_str<<"\n";

	device.getInfo(CL_DEVICE_PROFILE, &output_str);
	std::cout<<"Device profile is "<<output_str<<"\n";

	device.getInfo(CL_DEVICE_VERSION, &output_str);
	std::cout<<"Device version is "<<output_str<<"\n";

	device.getInfo(CL_DRIVER_VERSION, &output_str);
	std::cout<<"Driver version is "<<output_str<<"\n\n";
};



void World::WorldInit()
{
    cl_int err;
    dt = .01f;
    std::vector< cl::Platform > platformList;

    std::string platformVendor;

    cl::Platform::get(&platformList); //Got all the available platforms

    std::cout << "You've got " << platformList.size() << " platform(s):\n";

    for (size_t i = 0; i < platformList.size(); i++)
    {
        platformList[i].getInfo((cl_platform_info)CL_PLATFORM_NAME, &platformVendor);
        std::cout << i <<": "<<platformVendor << "\n";
    };

    //Choosing a platform
    size_t platf_num = 9999999;
    //while ((platf_num >= platformList.size())||(platf_num < 0))
    //{
        //std::cout<<"Choose platform number: ";
        //std::cin>>platf_num;
        //if ((platf_num >= platformList.size())||(platf_num < 0))
        //{
            //std::cout<<"Incorrect. Please, try again.\n";
        //};
    //};

    //platf_num = 0;

    //cl_context_properties cprops[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[platf_num])(), 0};
    //cl_context_properties props[] = {CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
    //                                 CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
    //                                 CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[platf_num])(),
    //                                 0}; 

    std::vector<cl::Device> all_devices;
    platformList[0].getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    cl::Device default_device = all_devices[0];
    std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";
    
      
    context = cl::Context({default_device});
    queue = cl::CommandQueue(context,default_device);


    //context = cl::Context(CL_DEVICE_TYPE_GPU, cprops, NULL, NULL, &err);
    //context = cl::Context(CL_DEVICE_TYPE_GPU||CL_DEVICE_TYPE_CPU, props,  NULL, NULL, &err);
    //oclErrorString(err); //Printing out error (if any)
    //We'd created a context
    //devices = context.getInfo<CL_CONTEXT_DEVICES>();

    //Creating a queue:
    //queue = cl::CommandQueue(context, devices[0], 0, &err);
    //oclErrorString(err); //Printing out error (if any)


    getDevInfo(devices[0]); //Printing out device information
    glewInit();
};


cl::Kernel World::CreateKernel(std::string kernel_path)
{
    //Loading kernel source code from file
    std::ifstream file(kernel_path.c_str());
    if(file.is_open() ){std::cout<<"Kernel file successfully opened.\n";}
    else {std::cout<<"Warning! Unable to open kernel file. Please, try again.\n";};
    //Stringify data
    std::string kernel_source(std::istreambuf_iterator<char>(file),(std::istreambuf_iterator<char>()));
    std::cout<<"("<<kernel_path<<")\n\n";
    int pl = kernel_source.size();
    //Program setup
    // cl::Program::Sources source(1, std::make_pair(kernel_source.c_str(), pl));
    cl::Program::Sources sources;    
    sources.push_back({kernel_source.c_str(),kernel_source.length()});
    cl::Program program(context, sources);
    //This will build the kernel code
    cl_int err = program.build(devices, "");
    oclErrorString(err);
    //Kernel to pass data in
    kernel = cl::Kernel(program, "OpenCLPandA", &err);
    oclErrorString(err); //Printing out error (if any)
    return kernel;
};


void World::AppendNewObject(panda Panda)
//Converts data from float arrays to vectors
{	
    //Positions:
    pos.push_back(Panda.position.x);
    pos.push_back(Panda.position.y);
    pos.push_back(Panda.position.z);
    
    //Velocities:
    vel.push_back(Panda.speed.x);
    vel.push_back(Panda.speed.y);
    vel.push_back(Panda.speed.z);
    
    //Sizes:
    r.push_back(Panda.size);

    //Masses:
    m.push_back(Panda.weight);

    //Q:
    q.push_back(Panda.glamor);
    //std::cout<<pos.size()<<" "<<pos.max_size()<<" "<<pos.capacity()<<"\n";
    //Renewing num (number of elements to process)
    num = pos.size()/3; 
};


void World::LoadEverybody(cl::Kernel kernel,
                   std::vector<float> pos, 
                   std::vector<float> vel,
                   std::vector<float> r,
                   std::vector<float> m,
                   std::vector<float> q)
{
    //Store the number of particles and the size in bytes of our arrays
    cl_int err;
    int array_size = pos.size() * sizeof(float);
    //Create VBOs (defined in util.cpp)
    gl_p_vbo = CreateVBO(&pos, array_size, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
    gl_v_vbo = CreateVBO(&vel, array_size, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
    gl_r_vbo = CreateVBO(&r,   array_size, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
    gl_m_vbo = CreateVBO(&m,   array_size, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
    gl_q_vbo = CreateVBO(&q,   array_size, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
    //Make sure OpenGL is finished before we proceed
    glFinish();
    //Create OpenCL buffer from GL VBO
    cl::Memory cl_p_vbo = cl::BufferGL(context, CL_MEM_READ_WRITE, gl_p_vbo, &err);
    cl::Memory cl_v_vbo = cl::BufferGL(context, CL_MEM_READ_WRITE, gl_v_vbo, &err);
    cl::Memory cl_r_vbo = cl::BufferGL(context, CL_MEM_READ_WRITE, gl_r_vbo, &err);
    cl::Memory cl_m_vbo = cl::BufferGL(context, CL_MEM_READ_WRITE, gl_m_vbo, &err);
    cl::Memory cl_q_vbo = cl::BufferGL(context, CL_MEM_READ_WRITE, gl_q_vbo, &err);
    //... and put data to the main array
    cl_vbos.push_back(cl_p_vbo);
    cl_vbos.push_back(cl_v_vbo);
    cl_vbos.push_back(cl_r_vbo);
    cl_vbos.push_back(cl_m_vbo);
    cl_vbos.push_back(cl_q_vbo);
    //Make sure the queue is finished
    queue.finish();
    //Transferring data on device
    err = kernel.setArg(0, cl_vbos[0]); //position vbo
    oclErrorString(err); //Printing out error (if any)
    err = kernel.setArg(1, cl_vbos[1]); //velocity vbo
    oclErrorString(err); //Printing out error (if any)
    err = kernel.setArg(2, cl_vbos[2]); //sizes    vbo
    oclErrorString(err); //Printing out error (if any)
    err = kernel.setArg(3, cl_vbos[3]); //masses   vbo
    oclErrorString(err); //Printing out error (if any)
    err = kernel.setArg(4, cl_vbos[4]); //q        vbo
    oclErrorString(err); //Printing out error (if any)
    err = kernel.setArg(5, dt); //Pass in the timestep
    oclErrorString(err); //Printing out error (if any)


    cl_a = cl::Buffer(context, CL_MEM_READ_WRITE, array_size, NULL, &err);
    cl::Event event;
    err = queue.enqueueWriteBuffer(cl_a, CL_TRUE, 0, sizeof(float) * 10, a, NULL, &event);
    err = kernel.setArg(6, cl_a);


    oclErrorString(err); //Printing out error (if any)
    queue.finish();
};


void World::ConfigureApplication(std::string kernel_path)
{
    cl::Kernel kernel = CreateKernel(kernel_path);
    LoadEverybody(kernel, pos, vel, r, m, q);
};


void World::StartApplication()
{
    cl_int err;
    cl::Event event_;
    glFinish();
    //Map OpenGL buffer object for writing from OpenCL
    //this passes in the vector of VBO buffer objects
    err = queue.enqueueAcquireGLObjects(&cl_vbos, NULL, &event_);
    oclErrorString(err); //Printing out error (if any)
    queue.finish();
    //Execute the kernel
    err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(num), cl::NullRange, NULL, &event_);
    oclErrorString(err); //Printing out error (if any)
    queue.finish();
    //Release the VBOs so OpenGL can play with them
    err = queue.enqueueReleaseGLObjects(&cl_vbos, NULL, &event_);
    oclErrorString(err); //Printing out error (if any)

    float c_done[10];
    err = queue.enqueueReadBuffer(cl_a, CL_TRUE, 0, sizeof(float) * num, &c_done, NULL, &event_);

    std::cout<<c_done[0]<<" -- "<<c_done[1]<<"\n";

    queue.finish();
};
