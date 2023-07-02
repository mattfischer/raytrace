#ifndef OPENCL_HPP
#define OPENCL_HPP

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

#include <string>
#include <vector>

namespace OpenCL {
    class Context {
    public:
        Context();
        ~Context();

        cl_platform_id clPlatform();
        cl_device_id clDevice();
        cl_context clContext();
        cl_command_queue clQueue();

    private:
        cl_platform_id mClPlatform;
        cl_device_id mClDevice;
        cl_context mClContext;
        cl_command_queue mClHostQueue;
        cl_command_queue mClDeviceQueue;
    };

    class Program {
    public:
        Program(Context &context, const std::string &filename);
        Program(Context &context, const std::vector<std::string> &filenames);
    
        cl_program &clProgram();

    private:
        std::string loadSourceFile(const std::string &filename);

        cl_program mClProgram;
    };

    class Allocator;
    class Kernel {
    public:
        Kernel(Program &program, const std::string &name, Allocator &allocator);

        void setArg(int arg, void *value);

        void enqueue(Context &context, int size);

    private:
        cl_kernel mClKernel;
        Allocator &mAllocator;
    };

    class Allocator {
    public:
        Allocator(Context &context);
    
        void *allocateBytes(size_t size);

        template<typename T, typename ...Args> T* allocate(Args&& ...args) {
            void *mem = allocateBytes(sizeof(T));
            return new(mem) T(std::forward<Args>(args)...);
        }

        void mapAreas();
        void unmapAreas();

        std::vector<void*> &areas();

    private:
        Context &mContext;
        std::vector<void*> mAreas;
        size_t mAreaFull;
        bool mMapped;
    };
}
#endif