// chemicalDistributionSystem.cpp
// by Franz Koehler 2009

#include "chemicalDistributionSystem.h"
#include "chemicalSystem.h"
#include "organism.h"
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

    for(std::map<Bodypart*,BodypartInformation*>::iterator it = bodyparts.begin(); it != bodyparts.end(); it++) {
      singleDistributionStep((*it).first,dynamic_cast<ExtendedBodypartInformation*>((*it).second));
    };

    return true;
  };

  bool ResourceProvider::bodypartAdded(Bodypart* bodypart, BodypartInformation* information) {
    if(!(bodypart->getBodypartType() == reactiveBodypartType)) {
      return false;
    };
    return true;
  };

  bool ResourceProvider::bodypartRemoved(Bodypart* bodypart, BodypartInformation* information) {
    return true;
  };
};