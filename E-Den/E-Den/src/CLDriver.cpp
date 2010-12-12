// CLDriver.cpp
// by Franz Koehler 2009

//#include "CL\cl.h"
#include "CLDriver.h"
#include <stdio.h>
#include <iostream>


namespace EDen {
  //inline void checkCLErr(cl_int err, const char * name) {
  //  if (err != CL_SUCCESS) {
  //    std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
  //    exit(EXIT_FAILURE);
  //  }
  //};

  CLDriver::CLDriver() {
    anyvalue = 1;
  };

  void CLDriver::execute() {

  };
};