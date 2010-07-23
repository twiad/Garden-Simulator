// chemicalDistributionSystem.h
// intended to be used for cyclic resource distribution depending on the structure of the surrounding system, like sunlight distribution depending on angle and shadow of 3D models
// by Franz Koehler 2009

#ifndef _E_DEN_CHEMICALDISTRIBUTIONSYSTEM_HEADER_
#define _E_DEN_CHEMICALDISTRIBUTIONSYSTEM_HEADER_

namespace EDen {
  class ResourceProvider;
};

#include "globals.h"
#include "bodypartObserver.h"
#include <boost/thread/mutex.hpp>

namespace EDen {
  class ExtendedBodypartInformation : public BodypartInformation {
  public:
    float factor;

    ExtendedBodypartInformation() {factor = 1.0f;};
    virtual ~ExtendedBodypartInformation() {};
  };

  class ResourceProvider {
  protected:  
    std::string chemicalName;
    float amount;

    std::map<Bodypart*, ExtendedBodypartInformation*> bodyparts;
    boost::mutex bodypartsMutex;
    BodypartType reactiveBodypartType;

    ExtendedBodypartInformation* getInformation(Bodypart* param_bodypart);
    bool updateBodypartInformation(Bodypart* param_bodypart, ExtendedBodypartInformation* param_info);
    
    virtual bool singleDistributionStep(Bodypart* param_bodypart, ExtendedBodypartInformation* param_info);

  public:
    ResourceProvider(std::string param_chemicalName, BodypartType param_bodypartType, float param_amount);
    ~ResourceProvider();

    bool addBodypart(Bodypart* param_bodypart);
    bool removeBodypart(Bodypart* param_bodypart);
    
    bool distibute();
  };
};

#endif