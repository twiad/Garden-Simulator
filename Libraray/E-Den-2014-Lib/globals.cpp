// globals.cpp
// by FranzKoehler 2009

#include "globals.h"

namespace EDen {
  void wait(int seconds) 
  { 
    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC_);
    xt.sec += 1;

    boost::thread::sleep(xt);
  }; 
} // namespace