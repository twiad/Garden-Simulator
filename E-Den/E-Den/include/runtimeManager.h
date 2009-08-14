// runtimeManager.h
// holds Organisms, manages their updates, 
// holds the resourceSystem manages the resource system updates
// holds a Universe-Representation (Resource-Provider)
// by Franz Koehler 2009

#ifndef _E_DEN_RUNTIMEMANAGER_HEADER_
#define _E_DEN_RUNTIMEMANAGER_HEADER_

#include "globals.h"
#include "organism.h"
#include "groundpart.h"
#include "chemicalDistributionSystem.h"

namespace EDen {
  class RuntimeManager {
  protected:
    std::list<ResourceProvider*> resourceProviders;
    std::list<Organism*> organisms;
    std::list<Groundpart*> groundparts;

    bool cleanupDeadOrganisms();
    bool deleteAll();
    unsigned long cycles;
  public:
    RuntimeManager();
    ~RuntimeManager();

    bool reset();

    bool add(Organism* param_org);
    bool add(ResourceProvider* param_resourceProvider);
    bool add(Groundpart* param_groudnpart);

    bool registerBodypart(Bodypart* param_bodypart);
    bool unregisterBodypart(Bodypart* param_bodypart);
    bool remove(Organism* param_org);
    bool remove(ResourceProvider* param_resourceProvider);
    bool remove(Groundpart* paramGroundpart);

    bool update();

    unsigned long getCycleCount();
  };
};

#endif