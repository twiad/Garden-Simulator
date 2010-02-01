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
#include "geneticMutation.h"
#include "speciesDatabase.h"

namespace EDen {
  class RuntimeManager {
  protected:
    std::list<ResourceProvider*> resourceProviders;
    std::list<Organism*> organisms;
    std::list<Groundpart*> groundparts;

    SpeciesDatabase* database;

    bool cleanupDeadOrganisms();
    bool deleteAll();
    bool deleteAllOfSpecies(int speciesID);
    unsigned long cycles;
    int candidatesTreshold;

    Organism* getNextSeed();

    int clock_frac_resources_provider;
    int clock_frac_genproc;
    int clock_frac_delete;
    int clock_frac_chemlinks;
  public:
    RuntimeManager();
    ~RuntimeManager();

    Randomizer* randomizer;

    bool reset();

    bool add(Organism* param_org, bool connectToGroundpart = false);
    bool add(ResourceProvider* param_resourceProvider);
    bool add(Groundpart* param_groudnpart);

    bool registerBodypart(Bodypart* param_bodypart);
    bool unregisterBodypart(Bodypart* param_bodypart);
    bool remove(Organism* param_org);
    bool remove(ResourceProvider* param_resourceProvider);
    bool remove(Groundpart* paramGroundpart);

    bool update();

    unsigned long getCycleCount();
    int getOrganismCount();
    int getSeedCount();
    bool orgsAlive();

    int initDatabase(std::string appSettingsPath);
    int saveDatabase(std::string filename = "database.xml");
    int loadDatabase(std::string filename = "database.xml");

    std::list<Organism*> getOrganisms();

    std::string getDebugOut();
  };
};

#endif