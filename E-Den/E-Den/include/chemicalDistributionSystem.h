// chemicalDistributionSystem.h
// intended to be used for cyclic resource distribution depending on the structure of the surrounding system, like sunlight distribution depending on angle and shadow of 3D models
// by Franz Koehler 2009

#ifndef _E_DEN_CHEMICALDISTRIBUTIONSYSTEM_HEADER_
#define _E_DEN_CHEMICALDISTRIBUTIONSYSTEM_HEADER_

namespace EDen {
  class ResourceProvider;
};

#include "globals.h"
#include "chemicalSystem.h"
#include "organism.h"

namespace EDen {
  class ResourceProvider {
  protected:  
    std::string chemicalName;
    float amount;

    BodypartList bodyparts;
    BodypartType reactiveBodypartType;

    virtual bool singleDistributionStep(Bodypart* param_bodypart);
  public:
    ResourceProvider(std::string param_chemicalName, BodypartType param_bodypartType, float param_amount);
    ~ResourceProvider();

    bool addBodypart(Bodypart* param_bodypart);
    bool removeBodypart(Bodypart* param_bodypart);
    
    bool distibute();
  };
};

#endif