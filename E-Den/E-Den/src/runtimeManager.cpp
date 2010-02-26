// runtimeManager.cpp
// by Franz Koehler 2009

#include "runtimeManager.h"
#define MAX_PLANT_COUNT 3
#define CANDIDATES_LEVEL (150 / 25)

#define NUM_THREADS 2

namespace EDen {

  boost::mutex RuntimeManager::orgsToProcessMutex;
  std::list<Organism*> RuntimeManager::orgsToProcess;

  unsigned long RuntimeManager::cycles = 0;
  int RuntimeManager::cps = 0;

  void RuntimeManager::processOrgs() {
    Organism* org = 0;

    do {
      boost::mutex::scoped_lock lock(orgsToProcessMutex);
      if(!orgsToProcess.empty()) {
        org = orgsToProcess.front();
        orgsToProcess.pop_front();
      } else org = 0;

      lock.unlock();

      if((org) && (org->getState() != BSP_dead)) {
        org->updateGeneticProcessors();
        org->updateDelete();
        org->updateChemicalStorageLinks();
        org->incLifetime();
      };
    } while (org);
  };

  void RuntimeManager::oneSecondTimer() {
    static long lastCycleCount = 0;
    while(true) {
      int currentCycle = cycles;
      cps = currentCycle - lastCycleCount;
      lastCycleCount = currentCycle;

      wait(1);
    };
  };

  RuntimeManager::RuntimeManager() {
    randomizer = new Randomizer();
    database = new SpeciesDatabase(this);
    reset();
    
    cpsWaiter = new boost::thread(&oneSecondTimer);
  };

  RuntimeManager::~RuntimeManager() {
    deleteAll();
  };

  bool RuntimeManager::deleteAll() {
    while(! organisms.empty()) {
      delete organisms.back();
      organisms.pop_back();
    };

    while(! groundparts.empty()) {
      delete groundparts.back();
      groundparts.pop_back();
    };

    while(! resourceProviders.empty()) {
      delete resourceProviders.back();
      resourceProviders.pop_back();
    };

    database->clear();

    return true;
  };

  bool RuntimeManager::deleteAllOfSpecies(int speciesID) {
    std::list<Organism*> oldOrgs;
    Organism* org = 0;
    while(!organisms.empty() && (organisms.front() != org)) {
      org = organisms.front();
      organisms.pop_front();
      if(org->getRootBodypart()->getGeneticCode()->getSpeciesIdentifier() != speciesID) {
        oldOrgs.push_back(org);
      }
      else {
        delete org;
      };
    };

    organisms.clear();
    organisms.swap(oldOrgs);

    return true;
  };

  Organism* RuntimeManager::getNextSeed() {
	  return database->pull();
  };

  bool RuntimeManager::reset() {
    preferedOrganismCount = MAX_PLANT_COUNT ;
    candidatesTreshold = CANDIDATES_LEVEL ;
    state = RMS_Normal;
    
    cycles = 0;

    deleteAll();

    return true;
  };

  bool RuntimeManager::add(Organism* param_org, bool p_connectToGroundpart) {
    if(param_org) {
      if(organisms.size() < preferedOrganismCount) {
        organisms.push_front(param_org);
        if(p_connectToGroundpart) {
          param_org->connectToGoundpart(groundparts.front());
        };
      }
      else {
        database->push(param_org);
      };

      
      return true; 
    } else return false;
  };
  
  bool RuntimeManager::add(ResourceProvider* param_resourceProvider) {
    if(param_resourceProvider) {
      resourceProviders.push_front(param_resourceProvider);
      return true;
    } else return false;
  };

  bool RuntimeManager::add(Groundpart* param_groundpart) {
    if(param_groundpart) {
      groundparts.push_front(param_groundpart);
      return true;
    } else return false;
  };

  bool RuntimeManager::registerBodypart(Bodypart* param_bodypart) {
    //boost::mutex::scoped_lock lock(bodypartsMutex);
    for(std::list<ResourceProvider*>::iterator it = resourceProviders.begin(); it != resourceProviders.end(); it++) {
      if(*it) (*it)->addBodypart(param_bodypart);
    };

    return true;
  };

  bool RuntimeManager::unregisterBodypart(Bodypart* param_bodypart) {
    //boost::mutex::scoped_lock lock(bodypartsMutex);
    for(std::list<ResourceProvider*>::iterator it = resourceProviders.begin(); it != resourceProviders.end(); it++) {
      if(*it) (*it)->removeBodypart(param_bodypart);
    };
    return true;
  };

  bool RuntimeManager::remove(Organism* param_org) {
    for(std::list<Organism*>::iterator it = organisms.end(); it != organisms.begin(); it--) {
      if(*it == param_org) {
        organisms.erase(it);
        break;
        return true;
      };
    };
    return true;
  };

  bool RuntimeManager::remove(ResourceProvider* param_resourceProvider) {
    for(std::list<ResourceProvider*>::iterator it = resourceProviders.end(); it != resourceProviders.begin(); it--) {
      if(*it == param_resourceProvider) {
        resourceProviders.erase(it);
        break;
        return true;
      };
    };
    return true;
  };

  bool RuntimeManager::remove(Groundpart* paramGroundpart) {
    for(std::list<Groundpart*>::iterator it = groundparts.end(); it != groundparts.begin(); it--) {
      if(*it == paramGroundpart) {
        groundparts.erase(it);
        break;
        return true;
      };
    };
    return true;
  };  

  bool RuntimeManager::update() {

    {
      for(std::list<ResourceProvider*>::iterator it = resourceProviders.begin(); it != resourceProviders.end(); it++) {
        if(*it) {
          (*it)->distibute();
        };
      };
    };

    for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
      orgsToProcess.push_back(*it);
    };

    boost::thread_group threadpool;

    for(int i = 0; i < NUM_THREADS; i++) {
      threadpool.create_thread(&processOrgs);
    };
    threadpool.join_all();

    cleanupDeadOrganisms();

    if((cycles % 10000) == 9999) {
      saveDatabase("autosave.xml");
    };

    if((cycles % 3000) == 0) {
      database->update();
    };

    cycles++;

    return true;
  };

  bool RuntimeManager::cleanupDeadOrganisms() {
    Organism* org;
    std::list<Organism*> new_orgs;

    while(!organisms.empty()) {
      org = organisms.back();
      organisms.pop_back();
      if(org)
          if((enforcePreferedOrganismCount) && (new_orgs.size() >= preferedOrganismCount))
            delete org;
          else if(org->getState() != BSP_dead)
              new_orgs.push_front(org);
          else
            delete org;
    };

    while(orgsAlive() && (new_orgs.size() < preferedOrganismCount)) {
      Organism* org = getNextSeed();
      new_orgs.push_back(org);
      org->connectToGoundpart(groundparts.front());
    };

    organisms.clear();
    organisms.swap(new_orgs);
    
    enforcePreferedOrganismCount = false;
    return true;
  };

  unsigned long RuntimeManager::getCycleCount() {
    return cycles;
  };

  int RuntimeManager::getOrganismCount() {
    return organisms.size();
  }

  int RuntimeManager::getSeedCount() {
    return database->size();
  };

  int RuntimeManager::getCps() {
    return cps;
  };

  bool RuntimeManager::orgsAlive() {
    if((organisms.size() > 0) || (database->size() > 0)) return true;
    return false;
  };

  int RuntimeManager::saveDatabase(std::string filename) {
    database->save(filename);
    return true;
  };

  int RuntimeManager::loadDatabase(std::string filename) {
    database->load(filename);
    return true;
  };

  int RuntimeManager::initDatabase(std::string appSettingsPath) {
    database->setApplicationSettingsPath(appSettingsPath);
    return true; 
  };

  std::list<Organism*> RuntimeManager::getOrganisms() {
    return organisms; 
  };

  std::string RuntimeManager::getDebugOut(bool shortInfo) {
    //return database->getDebugOut();
    std::string out = "";
    char str[64];

    if(shortInfo) {
      switch (state) {		
		    case RMS_Normal:
          out += "N";
		      break;
		    case RMS_Slow:         
          out += "S";
			    break;
		    case RMS_Pause:      
          out += "P";
			    break; 
	    }

      sprintf(str,"[%d/%d@%d|%d]",getOrganismCount(),getPreferedOrganismCount(),getCps(),getCycleCount());
      out += str;
    } 
    else 
      out = out + database->getDebugOut();

    return out;
  };

  unsigned RuntimeManager::getPreferedOrganismCount() {
    return preferedOrganismCount;
  };

  void RuntimeManager::setPreferedOrganismCount(unsigned pPreferedOrganismCount, bool killIfToMany) {
    if(pPreferedOrganismCount < 1)
      pPreferedOrganismCount = 1;

    preferedOrganismCount = pPreferedOrganismCount;
    enforcePreferedOrganismCount = killIfToMany;
  };

  RuntimeManagerState RuntimeManager::getState() {
    return state;
  };

  void RuntimeManager::setState(RuntimeManagerState newState) {
    state = newState; 
  };
  
};