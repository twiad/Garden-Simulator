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
      boost::mutex::scoped_lock lock(bodypartsMutex);

      if(!bodyparts[param_bodypart]) {
        if(param_bodypart->getBodypartType() == reactiveBodypartType) {
          updateBodypartInformation(param_bodypart,new ExtendedBodypartInformation());  
        };
      };
      return true;
    } else return false; // if param was zeropointer
  };

  ExtendedBodypartInformation* ResourceProvider::getInformation(Bodypart* param_bodypart) {
    boost::mutex::scoped_lock lock(bodypartsMutex);

    return bodyparts[param_bodypart];
  };

  bool ResourceProvider::removeBodypart(Bodypart* param_bodypart) {
    boost::mutex::scoped_lock lock(bodypartsMutex);

    ExtendedBodypartInformation* info = bodyparts[param_bodypart];
    if(info) delete info;
    bodyparts.erase(param_bodypart);

    return true;
  };

  bool ResourceProvider::updateBodypartInformation(Bodypart* param_bodypart, ExtendedBodypartInformation* param_info) {
    if((param_bodypart) && (param_info)) {
      if(param_bodypart->getBodypartType() == reactiveBodypartType) {
        boost::mutex::scoped_lock lock(bodypartsMutex);

        ExtendedBodypartInformation* info = bodyparts[param_bodypart];
        if(info) delete info;
        bodyparts[param_bodypart] = param_info;
      } else delete param_info;

      return true;
    } else return false; // if param was zeropointer
  };

  bool ResourceProvider::singleDistributionStep(Bodypart* param_bodypart, ExtendedBodypartInformation* param_info) {
    if(param_info && param_bodypart && (param_bodypart->getBodypartState() != BSP_dead)) {
      ChemicalStorage* chemStorage = param_bodypart->getChemicalStorage();
      if(chemStorage)
        return chemStorage->add(chemicalName,amount * param_info->factor);
    }
    return false; //param_bodypart->getChemicalStorage()->add(chemicalName,amount);
  };

  bool ResourceProvider::distibute() {
    boost::mutex::scoped_lock lock(bodypartsMutex);

    for(std::map<Bodypart*,ExtendedBodypartInformation*>::iterator it = bodyparts.begin(); it != bodyparts.end(); it++) {
      singleDistributionStep((*it).first,(*it).second);
    };

    return true;
  };
};