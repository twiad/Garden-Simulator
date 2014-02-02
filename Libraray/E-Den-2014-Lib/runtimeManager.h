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

#ifdef USE_CL
#include "CLDriver.h"
#endif

namespace EDen {

  class RuntimeManager {
  protected:
    std::list<ResourceProvider*> resourceProviders;
    GroundpartList groundparts;
    std::list<BodypartObserver*> observers;

    static boost::mutex orgsToProcessMutex;
    static std::list<Organism*> orgsToProcess;
	
    static unsigned long cycles;
    static int cps;
    static unsigned int bodypartCount;

#ifdef USE_CL
    static CLDriver driver;
#endif

	bool baadfoodHappened;

    RuntimeManagerState state;

    std::list<Organism*> organisms;

	static boost::mutex orgsToDeleteMutex;
	std::list<Organism*> orgsToDelete;

	std::list<Bodypart*> bodypartsToDelete;
    unsigned int preferedOrganismCount;
    bool enforcePreferedOrganismCount;
    int numthreads;

    boost::mutex bodypartsMutex;

    SpeciesDatabase* database;

    bool cleanupDeadOrganisms();
    bool deleteAll();
    bool deleteAllOfSpecies(int speciesID);
    int candidatesTreshold;

    Organism* getNextSeed();
	Groundpart* getGroundpartWithEmptySpace();
	Groundpart* getGroundpartWithEmptySpaceAndSpecies();

    boost::thread* cpsWaiter;
    static void oneSecondTimer();
    static void processOrgs();
	void deleteOrgsToDelete();
  public:
    RuntimeManager();
    ~RuntimeManager();

    Randomizer* randomizer;

    bool reset();

    bool add(Organism* param_org);
    bool add(ResourceProvider* param_resourceProvider);
    bool add(BodypartObserver* observer);
    bool add(Groundpart* param_groudnpart);

    bool registerBodypart(Bodypart* param_bodypart);
    bool unregisterBodypart(Bodypart* param_bodypart);
    bool remove(Organism* param_org);
    bool remove(ResourceProvider* param_resourceProvider);
    bool remove(BodypartObserver* observer);
    bool remove(Groundpart* paramGroundpart);

	bool addDelete(Bodypart* bodypart);
	bool addDelete(Organism* org);
    
	bool update();

    unsigned long getCycleCount();
    unsigned int getBodypartCount();
    int getOrganismCount();
    int getSeedCount();
    int getCps();
    unsigned getPreferedOrganismCount();
    void setPreferedOrganismCount(unsigned pPreferedOrganismCount, bool killIfToMany = false);
    RuntimeManagerState getState();
    void setState(RuntimeManagerState newState);

    bool orgsAlive();

    int initDatabase(std::string appSettingsPath);
    int saveDatabase(std::string filename = "database.xml");
    int loadDatabase(std::string filename = "database.xml");

    std::list<Organism*> getOrganisms();
	GroundpartList getGroundparts();

    std::string getDebugOut(bool shortInfo = false);
  };
};

#endif