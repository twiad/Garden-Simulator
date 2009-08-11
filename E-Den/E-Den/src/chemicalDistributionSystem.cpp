// chemicalDistributionSystem.cpp
// by Franz Koehler 2009

#include "chemicalDistributionSystem.h"
#include <stdio.h>
#include <iostream>

namespace EDen {
  ResourceProvider::ResourceProvider(std::string param_chemicalName, BodypartType param_bodypartType, float param_amount) {
    amount = param_amount;
    chemicalName = param_chemicalName;
    reactiveBodypartType = param_bodypartType;
  };

  ResourceProvider::~ResourceProvider() {
    
  };

  bool ResourceProvider::addBodypart(Bodypart* param_bodypart) {
    if(param_bodypart) {
      if(param_bodypart->getBodypartType() == reactiveBodypartType)
          bodyparts.push_back(param_bodypart);

      return true;
    } else return false; // if param was zeropointer
  };

  bool ResourceProvider::removeBodypart(Bodypart* param_bodypart) {
    for(BodypartListIterator it = bodyparts.begin(); it != bodyparts.end(); it++) {
      if(*it == param_bodypart) {
        bodyparts.erase(it);
      };
    };

    return true;
  };

  bool ResourceProvider::singleDistributionStep(Bodypart* param_bodypart) {
    return param_bodypart->getChemicalStorage()->add(chemicalName,amount);
  };

  bool ResourceProvider::distibute() {
    for(BodypartListIterator it = bodyparts.begin(); it != bodyparts.end(); it++) {
      singleDistributionStep(*it);
    };

    return true;
  };
};