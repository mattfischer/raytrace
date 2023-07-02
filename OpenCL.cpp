#include "OpenCL.hpp"

#include <stdio.h>

#include <fstream>

namespace OpenCL {
    Context::Context()
    {
        cl_uint num_platforms;

        clGetPlatformIDs(1, &mClPlatform, &num_platforms);
        printf("Found %i platforms\n", num_platforms);
        
        cl_uint num_devices;
        clGetDeviceIDs(mClPlatform, CL_DEVICE_TYPE_GPU, 1, &mClDevice, &num_devices);
        printf("Found %i devices\n", num_devices);

        cl_int errcode;

        cl_device_svm_capabilities svm_caps;
        errcode = clGetDeviceInfo(mClDevice, CL_DEVICE_SVM_CAPABILITIES, sizeof(svm_caps), &svm_caps, NULL);
        printf("Get device caps: %llx errcode: %i\n", svm_caps, errcode);

        cl_context_properties context_properties[] = {
            CL_CONTEXT_PLATFORM, (cl_context_properties)mClPlatform,
            0
        };
        mClContext = clCreateContext(context_properties, 1, &mClDevice, NULL, NULL, &errcode);
        printf("Context: %p errcode: %i\n", mClContext, errcode);

        mClHostQueue = clCreateCommandQueueWithProperties(mClContext, mClDevice, NULL, &errcode);
        printf("Command queue: %p errcode: %i\n", mClHostQueue, errcode);

        cl_queue_properties properties[] = {
            CL_QUEUE_PROPERTIES, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_ON_DEVICE | CL_QUEUE_ON_DEVICE_DEFAULT,
            (cl_queue_properties)0
        };
        mClDeviceQueue = clCreateCommandQueueWithProperties(mClContext, mClDevice, properties, &errcode);
        printf("Command queue: %p errcode: %i\n", mClDeviceQueue, errcode);
    }

    Context::~Context()
    {
    }

    cl_platform_id Context::clPlatform()
    {
        return mClPlatform;
    }

    cl_device_id Context::clDevice()
    {
        return mClDevice;
    }

    cl_context Context::clContext()
    {
        return mClContext;
    }

    cl_command_queue Context::clQueue()
    {
        return mClHostQueue;
    }

    Program::Program(Context &context, const std::string &filename)
    {
        std::string sourceStr = loadSourceFile(filename);

        const char *source = sourceStr.c_str();
        size_t size = sourceStr.size();
        cl_int errcode;
        mClProgram = clCreateProgramWithSource(context.clContext(), 1, &source, &size, &errcode);
        printf("Program: %p errcode: %i\n", mClProgram, errcode);

        errcode = clBuildProgram(mClProgram, 0, NULL, "-cl-std=CL2.0", NULL, NULL);
        printf("Build program: %i\n", errcode);

        char buffer[1024*10];
        size_t logSize;
        clGetProgramBuildInfo(mClProgram, context.clDevice(), CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &logSize);
        buffer[logSize] = '\0';
        printf("%s\n", buffer);
    }

    Program::Program(Context &context, const std::vector<std::string> &filenames)
    {
        std::vector<std::string> sourceStrs;
        std::vector<const char *> sources;
        std::vector<size_t> sizes;
        for(const std::string &filename : filenames) {
            std::string sourceStr = loadSourceFile(filename);
            sources.push_back(sourceStr.c_str());
            sizes.push_back(sourceStr.size());
            sourceStrs.push_back(std::move(sourceStr));
        }
        
        cl_int errcode;
        mClProgram = clCreateProgramWithSource(context.clContext(), sourceStrs.size(), &sources[0], &sizes[0], &errcode);
        printf("Program: %p errcode: %i\n", mClProgram, errcode);

        errcode = clBuildProgram(mClProgram, 0, NULL, "-cl-std=CL2.0", NULL, NULL);
        printf("Build program: %i\n", errcode);

        char buffer[1024*10];
        size_t logSize = 0;
        clGetProgramBuildInfo(mClProgram, context.clDevice(), CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &logSize);
        buffer[logSize] = '\0';
        printf("%s\n", buffer);
    }
    
    std::string Program::loadSourceFile(const std::string &filename)
    {
        std::string ret;
        std::ifstream ifstream(filename.c_str(), std::ios_base::in | std::ios_base::binary);
        if(ifstream.good()) {
            ifstream.seekg(0, std::ios_base::end);
            size_t size = ifstream.tellg();
            ifstream.seekg(0);
            ret.resize(size);
            ifstream.read(&ret[0], size);
        }

        return ret;
    }

    cl_program &Program::clProgram()
    {
        return mClProgram;
    }

    Kernel::Kernel(Program &program, const std::string &name, Allocator &allocator)
    : mAllocator(allocator)
    {
        cl_int errcode;
        mClKernel = clCreateKernel(program.clProgram(), name.c_str(), &errcode);
        printf("Kernel: %p errcode: %i\n", mClKernel, errcode);
    }

    void Kernel::setArg(int arg, void *value)
    {
        clSetKernelArgSVMPointer(mClKernel, arg, value);
    }

    void Kernel::enqueue(Context &context, int size)
    {
        std::vector<void*> &areas = mAllocator.areas();
        cl_int errcode = clSetKernelExecInfo(mClKernel, CL_KERNEL_EXEC_INFO_SVM_PTRS, areas.size() * sizeof(void*), &areas[0]);
        printf("Set exec info: %i\n", errcode);

        size_t global_size = size;
        errcode = clEnqueueNDRangeKernel(context.clQueue(), mClKernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
        printf("Enqueue kernel: %i\n", errcode);
    }

    static const int kAreaSize = 1024*1024*10;
    Allocator::Allocator(Context &context)
    : mContext(context)
    {
        mAreaFull = 0;
        mMapped = false;
    }

    void *Allocator::allocateBytes(size_t size)
    {
        if(mAreas.size() == 0 || kAreaSize - mAreaFull < size) {
            void *area = clSVMAlloc(mContext.clContext(), CL_MEM_READ_WRITE, kAreaSize, 0);
            if(mMapped) {
                clEnqueueSVMMap(mContext.clQueue(), CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, area, kAreaSize, 0, NULL, NULL);
            }
            mAreas.push_back(area);
            mAreaFull = 0;
        }

        uint8_t *ret = (uint8_t*)mAreas.back() + mAreaFull;
        mAreaFull += size;

        return ret;
    }

    std::vector<void*> &Allocator::areas()
    {
        return mAreas;
    }

    void Allocator::mapAreas()
    {
        for(void* ptr : mAreas) {
            clEnqueueSVMMap(mContext.clQueue(), CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, ptr, kAreaSize, 0, NULL, NULL);
        }
        mMapped = true;
    }

    void Allocator::unmapAreas()
    {
        for(void* ptr : mAreas) {
            clEnqueueSVMUnmap(mContext.clQueue(), ptr, 0, NULL, NULL);
        }
        mMapped = false;
    }
}