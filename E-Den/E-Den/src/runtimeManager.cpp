// runtimeManager.cpp
// by Franz Koehler 2009

#include "runtimeManager.h"
#define MAX_PLANT_COUNT 3
#define CANDIDATES_COUNT 20
#define CANDIDATES_LEVEL (150 / 25)

#define NUM_THREADS 2

namespace EDen {

  boost::mutex eden_runtimeManager_orgsToProcessMutex;
  std::list<Organism*> eden_runtimeManager_orgsToProcess;

  RuntimeManager::RuntimeManager() {
    randomizer = new Randomizer();
    database = new SpeciesDatabase(this);
    reset();
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
    clock_frac_resources_provider = 1;
    clock_frac_genproc = 1;
    clock_frac_delete = 200;
    clock_frac_chemlinks = 1;

    candidatesTreshold = CANDIDATES_LEVEL ;
    
    cycles = 0;

    deleteAll();

    return true;
  };

  bool RuntimeManager::add(Organism* param_org, bool p_connectToGroundpart) {
    if(param_org) {
      if(organisms.size() < MAX_PLANT_COUNT) {
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

  void processOrgs() {
    Organism* org = 0;

    do {
      boost::mutex::scoped_lock lock(eden_runtimeManager_orgsToProcessMutex);
      if(!eden_runtimeManager_orgsToProcess.empty()) {
        org = eden_runtimeManager_orgsToProcess.front();
        eden_runtimeManager_orgsToProcess.pop_front();
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

  bool RuntimeManager::update() {

    {
      for(std::list<ResourceProvider*>::iterator it = resourceProviders.begin(); it != resourceProviders.end(); it++) {
        if(*it) {
          (*it)->distibute();
        };
      };
    };

    for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
      eden_runtimeManager_orgsToProcess.push_back(*it);
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
          if(org->getState() != BSP_dead)
              new_orgs.push_front(org);
          else {
            delete org;
          };
    };

    while(orgsAlive() && (new_orgs.size() < MAX_PLANT_COUNT)) {
      Organism* org = getNextSeed();
      new_orgs.push_back(org);
      org->connectToGoundpart(groundparts.front());
    };

    organisms.clear();
    organisms.swap(new_orgs);

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

  std::string RuntimeManager::getDebugOut() {
    return database->getDebugOut();
  };
};