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
#include <boost/thread/thread.hpp>



namespace EDen {

  class RuntimeManager {
  protected:
    std::list<ResourceProvider*> resourceProviders;
    std::list<Groundpart*> groundparts;

    static boost::mutex orgsToProcessMutex;
    static std::list<Organism*> orgsToProcess;

    static unsigned long cycles;
    static int cps;
    
    std::list<Organism*> organisms;
    unsigned int preferedOrganismCount;
    bool enforcePreferedOrganismCount;

    boost::mutex bodypartsMutex;

    SpeciesDatabase* database;

    bool cleanupDeadOrganisms();
    bool deleteAll();
    bool deleteAllOfSpecies(int speciesID);
    int candidatesTreshold;

    Organism* getNextSeed();

    boost::thread* cpsWaiter;
    static void oneSecondTimer();
    static void processOrgs();
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
    int getCps();
    unsigned getPreferedOrganismCount();
    void setPreferedOrganismCount(unsigned pPreferedOrganismCount, bool killIfToMany = false);

    bool orgsAlive();

    int initDatabase(std::string appSettingsPath);
    int saveDatabase(std::string filename = "database.xml");
    int loadDatabase(std::string filename = "database.xml");

    std::list<Organism*> getOrganisms();

    std::string getDebugOut(bool shortInfo = false);
  };
};

#endif