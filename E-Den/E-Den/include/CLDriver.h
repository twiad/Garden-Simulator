// geneticMutation.h
// by Franz Koehler 2009

#ifndef _E_DEN_CL_DRIVER_HEADER_
#define _E_DEN_CL_DRIVER_HEADER_

#include <list>
#include <oclUtils.h>
#include "chemicalSystem.h"
#include <boost/thread/mutex.hpp>
//#define __NO_STD_VECTOR
#define CHUNKSIZE 1000

namespace EDen {
  class CLDriver;
};

#include "globals.h"

namespace EDen {
  class ChemicalStorage;
  class Chemical;

  class CLDriver {
  protected:
    struct StorageSyncData {
      StorageSyncData() {
        current = 0; busy = false;
      };
      float* aMax;
      float* aCurrent;
      float* bMax;
      float* bCurrent;
      float* aCurrentOut;
      float* bCurrentOut;
      ChemicalStorage** a;
      ChemicalStorage** b;
      Chemical** chemical;
      int current;
      bool busy;
      boost::mutex chunkMutex;
    };

    std::list<StorageSyncData*> fullStorageSyncDataChunksToProcess;
    boost::mutex fullStorageSyncDataChunksToProcessMutex;
    std::list<StorageSyncData*> storageSyncDataChunksToProcess;
    boost::mutex storageSyncDataChunksToProcessMutex;
    std::list<StorageSyncData*> storageSyncDataChunksProcessed;
    boost::mutex storageSyncDataChunksProcessedMutex;
    std::list<StorageSyncData*> emptyStorageSyncDataChunks;
    boost::mutex emptyStorageSyncDataChunksMutex;

  private:
    // Name of the file with the source code for the computation kernel
    // *********************************************************************
    const char* cSourceFile;

    // OpenCL Vars
    cl_context cxGPUContext;        // OpenCL context
    cl_command_queue cqCommandQueue;// OpenCL command que
    cl_platform_id cpPlatform;      // OpenCL platform
    cl_device_id cdDevice;          // OpenCL device
    cl_program cpProgram;           // OpenCL program
    cl_kernel ckKernel;             // OpenCL kernel
    cl_mem cmDevSrcAMax;            // OpenCL device source buffer A
    cl_mem cmDevSrcACurrent;        // OpenCL device source buffer B 
    cl_mem cmDevSrcBMax;            // OpenCL device source buffer A
    cl_mem cmDevSrcBCurrent;        // OpenCL device source buffer B 
    cl_mem cmDevDstACurrent;        // OpenCL device destination buffer
    cl_mem cmDevDstBCurrent;        // OpenCL device destination buffer
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

    void Cleanup (int iExitCode, const char* msg = "");
  public:
    CLDriver();

    void execute();
    void enqueueStorageSync(ChemicalStorage* A, ChemicalStorage* B, Chemical* C);
  };
} // namespace EDen

#endif