// CLDriver.cpp
// by Franz Koehler 2009

#include "CLDriver.h"
#include <stdio.h>
#include <iostream>

#define FILEPATH "e:\Programme\NVIDIA GPU Computing Toolkit\CUDA\v3.2\src\oclVectorAdd"

namespace EDen {
  inline void checkCLErr(cl_int err, const char * name) {
    if (err != CL_SUCCESS) {
      std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
      exit(EXIT_FAILURE);
    }
  };

  char* oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength)
  {
      // locals 
      FILE* pFileStream = NULL;
      size_t szSourceLength;

      // open the OpenCL source code file
      #ifdef _WIN32   // Windows version
          if(fopen_s(&pFileStream, cFilename, "rb") != 0) 
          {       
              return NULL;
          }
      #else           // Linux version
          pFileStream = fopen(cFilename, "rb");
          if(pFileStream == 0) 
          {       
              return NULL;
          }
      #endif

      size_t szPreambleLength = strlen(cPreamble);

      // get the length of the source code
      fseek(pFileStream, 0, SEEK_END); 
      szSourceLength = ftell(pFileStream);
      fseek(pFileStream, 0, SEEK_SET); 

      // allocate a buffer for the source code string and read it in
      char* cSourceString = (char *)malloc(szSourceLength + szPreambleLength + 1); 
      memcpy(cSourceString, cPreamble, szPreambleLength);
      if (fread((cSourceString) + szPreambleLength, szSourceLength, 1, pFileStream) != 1)
      {
          fclose(pFileStream);
          free(cSourceString);
          return 0;
      }

      // close the file and return the total length of the combined (preamble + source) string
      fclose(pFileStream);
      if(szFinalLength != 0)
      {
          *szFinalLength = szSourceLength + szPreambleLength;
      }
      cSourceString[szSourceLength + szPreambleLength] = '\0';

      return cSourceString;
  }


  CLDriver::CLDriver() {
    // constant initialization values;
    cSourceFile = "VectorAdd.cl";
    cPathAndName = NULL;
    cSourceCL = NULL;
    iNumElements = 11444777;
    bNoPrompt = shrFALSE;
    
    // set and log Global and Local work size dimensions
    szLocalWorkSize = 256;
    szGlobalWorkSize = iNumElements + (szLocalWorkSize - (iNumElements % szLocalWorkSize));  // rounded up to the nearest multiple of the LocalWorkSize
// -1?


    // Allocate and initialize host arrays 
    srcA = new cl_float[szGlobalWorkSize];
    srcB = new cl_float[szGlobalWorkSize];
    dst = new cl_float[szGlobalWorkSize];

    //Get an OpenCL platform
    ciErr1 = clGetPlatformIDs(1, &cpPlatform, NULL);

    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }

    //Get the devices
    ciErr1 = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &cdDevice, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }

    //Create the context
    cxGPUContext = clCreateContext(0, 1, &cdDevice, NULL, NULL, &ciErr1);
    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }

    // Create a command-queue
    cqCommandQueue = clCreateCommandQueue(cxGPUContext, cdDevice, 0, &ciErr1);
    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }

    // Allocate the OpenCL buffer memory objects for source and result on the device GMEM
    cmDevSrcA = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_float) * szGlobalWorkSize, NULL, &ciErr1);
    cmDevSrcB = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_float) * szGlobalWorkSize, NULL, &ciErr2);
    ciErr1 |= ciErr2;
    cmDevDst = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, sizeof(cl_float) * szGlobalWorkSize, NULL, &ciErr2);
    ciErr1 |= ciErr2;
    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }
    
    // Read the OpenCL kernel in from source file
    cSourceCL = oclLoadProgSource("VectorAdd.cl", "", &szKernelLength);

    // Create the program
    cpProgram = clCreateProgramWithSource(cxGPUContext, 1, (const char **)&cSourceCL, &szKernelLength,&ciErr1);
    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }

    // Build the program with 'mad' Optimization option
    #ifdef MAC
        char* flags = "-cl-fast-relaxed-math -DMAC";
    #else
        char* flags = "-cl-fast-relaxed-math";
    #endif
    ciErr1 = clBuildProgram(cpProgram, 0, NULL, NULL, NULL, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
      Cleanup(EXIT_FAILURE);
    }

    // Create the kernel
    ckKernel = clCreateKernel(cpProgram, "VectorAdd", &ciErr1);
    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }

    // Set the Argument values
    ciErr1 = clSetKernelArg(ckKernel, 0, sizeof(cl_mem), (void*)&cmDevSrcA);
    ciErr1 |= clSetKernelArg(ckKernel, 1, sizeof(cl_mem), (void*)&cmDevSrcB);
    ciErr1 |= clSetKernelArg(ckKernel, 2, sizeof(cl_mem), (void*)&cmDevDst);
    ciErr1 |= clSetKernelArg(ckKernel, 3, sizeof(cl_int), (void*)&iNumElements);
    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }
  };

  void CLDriver::execute() {
    // Asynchronous write of data to GPU device
    ciErr1 = clEnqueueWriteBuffer(cqCommandQueue, cmDevSrcA, CL_FALSE, 0, sizeof(cl_float) * szGlobalWorkSize, srcA, 0, NULL, NULL);
    ciErr1 |= clEnqueueWriteBuffer(cqCommandQueue, cmDevSrcB, CL_FALSE, 0, sizeof(cl_float) * szGlobalWorkSize, srcB, 0, NULL, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }

    // Launch kernel
    ciErr1 = clEnqueueNDRangeKernel(cqCommandQueue, ckKernel, 1, NULL, &szGlobalWorkSize, &szLocalWorkSize, 0, NULL, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }

    // Synchronous/blocking read of results, and check accumulated errors
    ciErr1 = clEnqueueReadBuffer(cqCommandQueue, cmDevDst, CL_TRUE, 0, sizeof(cl_float) * szGlobalWorkSize, dst, 0, NULL, NULL);
    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }
  };

  void CLDriver::Cleanup (int iExitCode)
  {
      // Cleanup allocated objects
      if(cPathAndName)free(cPathAndName);
      if(cSourceCL)free(cSourceCL);
	  if(ckKernel)clReleaseKernel(ckKernel);  
      if(cpProgram)clReleaseProgram(cpProgram);
      if(cqCommandQueue)clReleaseCommandQueue(cqCommandQueue);
      if(cxGPUContext)clReleaseContext(cxGPUContext);
      if(cmDevSrcA)clReleaseMemObject(cmDevSrcA);
      if(cmDevSrcB)clReleaseMemObject(cmDevSrcB);
      if(cmDevDst)clReleaseMemObject(cmDevDst);

      // Free host memory
      free(srcA); 
      free(srcB);
      free (dst);
    
      // finalize logs and leave
      if (bNoPrompt)
      {
      }
      else 
      {
          getchar();
      }
      exit (iExitCode);
  }
};