// bodypartObserver.cpp
// by Franz Koehler 2010

#include "bodypartObserver.h"
#include "bodypart.h"
#include <stdio.h>
#include <iostream>

namespace EDen {

  BodypartInformation* updateInformation(Bodypart* bodypart, BodypartInformation* information = 0) {
    return 0;
  };

  bool BodypartObserver::updateBodypartInformation(Bodypart* param_bodypart, BodypartInformation* param_info) {
    if((param_bodypart) && (param_info)) {
      // boost::mutex::scoped_lock lock(bodypartsMutex);
      BodypartInformation* info = bodyparts[param_bodypart];
      if(info) delete info;
      bodyparts[param_bodypart] = param_info;
      return true;
    } else return false; // if param was zeropointer
  };

  bool BodypartObserver::addBodypart(Bodypart* param_bodypart) {
    if(param_bodypart) {
      BodypartInformation* info;
      boost::mutex::scoped_lock lock(bodypartsMutex);
      if(!bodyparts[param_bodypart]) {
        info = updateInformation(param_bodypart);
        if(!info) info = new BodypartInformation();
        bodypartAdded(param_bodypart,info);
        updateBodypartInformation(param_bodypart,info);  
      } 
      else {
        info = updateInformation(param_bodypart,info);
        bodypartAdded(param_bodypart,info);
      }
      return true;
    } else return false; // if param was zeropointer
  };

  bool BodypartObserver::removeBodypart(Bodypart* param_bodypart) {
    if(param_bodypart) {
      boost::mutex::scoped_lock lock(bodypartsMutex);
      BodypartInformation* info = bodyparts[param_bodypart];
      bodypartRemoved(param_bodypart,info);
      if(info) delete info;
      bodyparts.erase(param_bodypart);
    };
    return true;
  };
};