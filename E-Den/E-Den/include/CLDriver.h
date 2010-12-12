// geneticMutation.h
// by Franz Koehler 2009

#ifndef _E_DEN_CL_DRIVER_HEADER_
#define _E_DEN_CL_DRIVER_HEADER_

#include <list>
//#include "CL\cl.h"
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

  public:
    CLDriver();

    void execute();
  };
} // namespace EDen

#endif