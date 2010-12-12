// geneticMutation.h
// by Franz Koehler 2009

#ifndef _E_DEN_CL_DRIVER_HEADER_
#define _E_DEN_CL_DRIVER_HEADER_

#include <list>
#include <oclUtils.h>
//#define __NO_STD_VECTOR

namespace EDen {
  //class CLDriver;
};

#include "globals.h"

namespace EDen {
  class CLDriver {
  private:
    //inline void checkErr(cl_int err, const char * name);
    int anyvalue;

    // Name of the file with the source code for the computation kernel
    // *********************************************************************
    const char* cSourceFile;

    // Host buffers for demo
    // *********************************************************************
    void *srcA, *srcB, *dst;        // Host buffers for OpenCL test
    void* Golden;                   // Host buffer for host golden processing cross check

    // OpenCL Vars
    cl_context cxGPUContext;        // OpenCL context
    cl_command_queue cqCommandQueue;// OpenCL command que
    cl_platform_id cpPlatform;      // OpenCL platform
    cl_device_id cdDevice;          // OpenCL device
    cl_program cpProgram;           // OpenCL program
    cl_kernel ckKernel;             // OpenCL kernel
    cl_mem cmDevSrcA;               // OpenCL device source buffer A
    cl_mem cmDevSrcB;               // OpenCL device source buffer B 
    cl_mem cmDevDst;                // OpenCL device destination buffer 
    size_t szGlobalWorkSize;        // 1D var for Total # of work items
    size_t szLocalWorkSize;		      // 1D var for # of work items in the work group	
    size_t szParmDataBytes;			    // Byte size of context information
    size_t szKernelLength;			    // Byte size of kernel code
    cl_int ciErr1, ciErr2;			    // Error code var
    char* cPathAndName;      // var for full paths to data, src, etc.
    char* cSourceCL;         // Buffer to hold source for compilation 

    // demo config vars
    int iNumElements;	// Length of float arrays to process (odd # for illustration)
    shrBOOL bNoPrompt;  

    void Cleanup (int iExitCode);
  public:
    CLDriver();

    void execute();
  };
} // namespace EDen

#endif