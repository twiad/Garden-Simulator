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
    cSourceFile = "StorageSync.cl";
    cPathAndName = NULL;
    cSourceCL = NULL;
    iNumElements = 11444777;
    bNoPrompt = shrFALSE;
    
    // set and log Global and Local work size dimensions
    szLocalWorkSize = 256;
    szGlobalWorkSize = CHUNKSIZE + (szLocalWorkSize - (CHUNKSIZE % szLocalWorkSize));  // rounded up to the nearest multiple of the LocalWorkSize
// -1?


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
    cmDevSrcAMax = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_float) * CHUNKSIZE, NULL, &ciErr1);
    ciErr1 |= ciErr2;
    cmDevSrcACurrent = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_float) * CHUNKSIZE, NULL, &ciErr2);
    ciErr1 |= ciErr2;
    cmDevSrcBMax = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_float) * CHUNKSIZE, NULL, &ciErr2);
    ciErr1 |= ciErr2;
    cmDevSrcBCurrent = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_float) * CHUNKSIZE, NULL, &ciErr2);
    ciErr1 |= ciErr2;
    cmDevDstACurrent = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, sizeof(cl_float) * CHUNKSIZE, NULL, &ciErr2);
    ciErr1 |= ciErr2;
    cmDevDstBCurrent = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, sizeof(cl_float) * CHUNKSIZE, NULL, &ciErr2);
    ciErr1 |= ciErr2;
    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }
    
    // Read the OpenCL kernel in from source file
    cSourceCL = oclLoadProgSource("StorageSync.cl", "", &szKernelLength);

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
    ckKernel = clCreateKernel(cpProgram, "StorageSync", &ciErr1);
    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }

    // Set the Argument values
    ciErr1  = clSetKernelArg(ckKernel, 0, sizeof(cl_mem), (void*)&cmDevSrcAMax);
    ciErr1 |= clSetKernelArg(ckKernel, 1, sizeof(cl_mem), (void*)&cmDevSrcACurrent);
    ciErr1 |= clSetKernelArg(ckKernel, 2, sizeof(cl_mem), (void*)&cmDevSrcBMax);
    ciErr1 |= clSetKernelArg(ckKernel, 3, sizeof(cl_mem), (void*)&cmDevSrcBCurrent);
    ciErr1 |= clSetKernelArg(ckKernel, 4, sizeof(cl_mem), (void*)&cmDevDstACurrent);
    ciErr1 |= clSetKernelArg(ckKernel, 5, sizeof(cl_mem), (void*)&cmDevDstBCurrent);
    ciErr1 |= clSetKernelArg(ckKernel, 6, sizeof(cl_int), (void*)&iNumElements);
    if (ciErr1 != CL_SUCCESS)
    {
        Cleanup(EXIT_FAILURE);
    }
  };

  void CLDriver::execute() {
    StorageSyncData* data;

    do {
      data = 0;
      {
        boost::mutex::scoped_lock listLock(fullStorageSyncDataChunksToProcessMutex);
        for(std::list<StorageSyncData*>::iterator it = fullStorageSyncDataChunksToProcess.begin(); it != fullStorageSyncDataChunksToProcess.end(); it++) {
          if(!(*it)->busy) {
            boost::mutex::scoped_lock chunkLock((*it)->chunkMutex);
            if(!(*it)->busy) {
              (*it)->busy = true;
              data = (*it);           
              continue;
            }
          };
        };
        fullStorageSyncDataChunksToProcess.remove(data);
      }

      if(data == 0) {
        boost::mutex::scoped_lock listLock(storageSyncDataChunksToProcessMutex);
        for(std::list<StorageSyncData*>::iterator it = storageSyncDataChunksToProcess.begin(); it != storageSyncDataChunksToProcess.end(); it++) {
          if(!(*it)->busy) {
            boost::mutex::scoped_lock chunkLock((*it)->chunkMutex);
            if(!(*it)->busy) {
              (*it)->busy = true;
              data = (*it);           
              continue;
            }
          };
        };
        storageSyncDataChunksToProcess.remove(data);
      };

      if(data != 0) {
        ciErr1  = clEnqueueWriteBuffer(cqCommandQueue, cmDevSrcAMax, CL_FALSE, 0, sizeof(cl_float) * CHUNKSIZE, data->aMax, 0, NULL, NULL);
        ciErr1 |= clEnqueueWriteBuffer(cqCommandQueue, cmDevSrcACurrent, CL_FALSE, 0, sizeof(cl_float) * CHUNKSIZE, data->aCurrent, 0, NULL, NULL);
        ciErr1 |= clEnqueueWriteBuffer(cqCommandQueue, cmDevSrcBMax, CL_FALSE, 0, sizeof(cl_float) * CHUNKSIZE, data->bMax, 0, NULL, NULL);
        ciErr1 |= clEnqueueWriteBuffer(cqCommandQueue, cmDevSrcBCurrent, CL_FALSE, 0, sizeof(cl_float) * CHUNKSIZE, data->bCurrent, 0, NULL, NULL);

        size_t chunksize = CHUNKSIZE;
        size_t chunkcount = data->current;
        ciErr1 = clEnqueueNDRangeKernel(cqCommandQueue, ckKernel, 1, NULL, &chunksize, &chunkcount, 0, NULL, NULL);
        if (ciErr1 != CL_SUCCESS)
        {
            Cleanup(EXIT_FAILURE);
        }

        ciErr1  = clEnqueueReadBuffer(cqCommandQueue, cmDevDstACurrent, CL_TRUE, 0, sizeof(cl_float) * CHUNKSIZE, data->aCurrentOut, 0, NULL, NULL);
        ciErr1 |= clEnqueueReadBuffer(cqCommandQueue, cmDevDstBCurrent, CL_TRUE, 0, sizeof(cl_float) * CHUNKSIZE, data->bCurrentOut, 0, NULL, NULL);
        if (ciErr1 != CL_SUCCESS)
        {
          Cleanup(EXIT_FAILURE);
        }

        // copy data back to chem storages
        for(int row = 0; row < data->current; row++) {
          data->a[row]->setCurrentAmount(*(data->chemical[row]),data->aCurrentOut[row]);
          data->b[row]->setCurrentAmount(*(data->chemical[row]),data->bCurrentOut[row]);
        };

        // empty chunk and put to empty list
        data->current = 0;
        boost::mutex::scoped_lock listLock(emptyStorageSyncDataChunksMutex);
        emptyStorageSyncDataChunks.push_back(data);
        data->busy = false;
      };
    } while (data != 0);
  };

  void CLDriver::enqueueStorageSync(ChemicalStorage* A, ChemicalStorage* B, Chemical* Chem) {
    StorageSyncData* data = 0;
    std::list<StorageSyncData*> fullChunks;

    {
      boost::mutex::scoped_lock listLock(storageSyncDataChunksToProcessMutex);
      for(std::list<StorageSyncData*>::iterator it = storageSyncDataChunksToProcess.begin(); it != storageSyncDataChunksToProcess.end(); it++) {
        if(!(*it)->busy) {
          boost::mutex::scoped_lock chunkLock((*it)->chunkMutex);
          if(!(*it)->busy) {
            (*it)->busy = true;
            if((*it)->current >= CHUNKSIZE) {
              fullChunks.push_back(*it);
            }
            else {
              data = (*it);           
              continue;
            };
          }
        };
      };

      // move full chunks to full chunks list
      if(!fullChunks.empty()) {
        for(std::list<StorageSyncData*>::iterator it = fullChunks.begin(); it != fullChunks.end(); it++) {
          storageSyncDataChunksToProcess.remove(*it);
        };
        listLock.unlock();

        boost::mutex::scoped_lock listLock2(fullStorageSyncDataChunksToProcessMutex);
        for(std::list<StorageSyncData*>::iterator it = fullChunks.begin(); it != fullChunks.end(); it++) {
          fullStorageSyncDataChunksToProcess.push_back(*it);
          (*it)->busy = false;
        };
  
        fullChunks.clear();
      };
    }

    if(data == 0) { // no free/writable slot in current data packages found
      boost::mutex::scoped_lock listLock(emptyStorageSyncDataChunksMutex);
      for(std::list<StorageSyncData*>::iterator it = emptyStorageSyncDataChunks.begin(); it != emptyStorageSyncDataChunks.end(); it++) {
        if(!(*it)->busy) {
          boost::mutex::scoped_lock chunkLock((*it)->chunkMutex);
          if(!(*it)->busy) {
            data = (*it);           
            (*it)->busy = true;
            continue;
          }
        };
      };
    }

    if(data == 0) { // no empty chunk present, create a new one
      data = new StorageSyncData();
      data->busy = true;
      boost::mutex::scoped_lock listLock(storageSyncDataChunksToProcessMutex);
      storageSyncDataChunksToProcess.push_back(data);
    };
    
    // data should now point to a valid and writable package with busy set to true by this
    int currentRow = data->current;
    data->a[currentRow] = A;
    data->b[currentRow] = B;
    data->chemical[currentRow] = Chem;
    data->aCurrent[currentRow] = A->getCurrentAmount(*Chem);
    data->aMax[currentRow] = A->getMaxAmount(*Chem);
    data->bCurrent[currentRow] = B->getCurrentAmount(*Chem);
    data->bMax[currentRow] = B->getMaxAmount(*Chem);
    data->current += 1;
  };

  void CLDriver::Cleanup (int iExitCode)
  {
      std::cerr << "[!!!] CLDriver->Error : " << iExitCode << std::endl;
      // Cleanup allocated objects
      if(cPathAndName)free(cPathAndName);
      if(cSourceCL)free(cSourceCL);
	    if(ckKernel)clReleaseKernel(ckKernel);  
      if(cpProgram)clReleaseProgram(cpProgram);
      if(cqCommandQueue)clReleaseCommandQueue(cqCommandQueue);
      if(cxGPUContext)clReleaseContext(cxGPUContext);
      if(cmDevSrcAMax)clReleaseMemObject(cmDevSrcAMax);
      if(cmDevSrcACurrent)clReleaseMemObject(cmDevSrcACurrent);
      if(cmDevSrcBMax)clReleaseMemObject(cmDevSrcBMax);
      if(cmDevSrcBCurrent)clReleaseMemObject(cmDevSrcBCurrent);
      if(cmDevDstACurrent)clReleaseMemObject(cmDevDstACurrent);
      if(cmDevDstBCurrent)clReleaseMemObject(cmDevDstBCurrent);

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