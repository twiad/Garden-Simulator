// bodypartObserver.cpp
// by Franz Koehler 2010

#include "bodypartObserver.h"
#include "bodypart.h"
#include <stdio.h>
#include <iostream>

namespace EDen {

  BodypartObserver::BodypartObserver() {};
  BodypartObserver::~BodypartObserver() {
    removeAllBodyparts();
  };

  BodypartInformation* BodypartObserver::updateInformation(Bodypart* bodypart, BodypartInformation* information) {
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
        if(bodypartAdded(param_bodypart,info)) {
          updateBodypartInformation(param_bodypart,info);  
        }
        else {
          delete info;
          return false;
        };
      } 
      else {
        info = updateInformation(param_bodypart,bodyparts[param_bodypart]);
        if(bodypartAdded(param_bodypart,info)) {
          
        } else {
          removeBodypart(param_bodypart);
          return false;
        }
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

  bool BodypartObserver::removeAllBodyparts() {
    std::map<Bodypart*, BodypartInformation*>::iterator it;
    while(!bodyparts.empty()) {
      it = bodyparts.begin();
      bodypartRemoved(it->first,it->second);
      delete it->second;
      bodyparts.erase(it);
    };
    return true;
  };

  BodypartInformation* BodypartObserver::getInformation(Bodypart* param_bodypart) {
    boost::mutex::scoped_lock lock(bodypartsMutex);
    return bodyparts[param_bodypart];
  };
};