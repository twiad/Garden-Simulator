// runtimeManager.cpp
// by Franz Koehler 2009

#include "runtimeManager.h"
#define MAX_PLANT_COUNT 3

namespace EDen {
  RuntimeManager::RuntimeManager() {
    randomizer = new Randomizer();
    database = new GeneticCodeDatabase(this);
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
    //int pos = (int)randomizer->value(0.0f,(float)seeds.size() - 1);
    //std::list<Organism*>::iterator it = seeds.begin();
    //for(int i = 0; i < pos; i++) {
    //  it++;
    //};
    //return *it;

	  return database->pull();
  };

  bool RuntimeManager::reset() {
    clock_frac_resources_provider = 1;
    clock_frac_genproc = 1;
    clock_frac_delete = 200;
    clock_frac_chemlinks = 1;
    
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
    for(std::list<ResourceProvider*>::iterator it = resourceProviders.begin(); it != resourceProviders.end(); it++) {
      if(*it) (*it)->addBodypart(param_bodypart);
    };

    return true;
  };

  bool RuntimeManager::unregisterBodypart(Bodypart* param_bodypart) {
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

// some Modulo for every provider?
    //if(cycles % clock_frac_resources_provider == 0) {
    {
      for(std::list<ResourceProvider*>::iterator it = resourceProviders.begin(); it != resourceProviders.end(); it++) {
        if(*it) {
          (*it)->distibute();
        };
      };
    };

    Organism* org;
    for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
      org = *it;
      if((org) && (org->getState() != BSP_dead)) {
        //if((cycles % clock_frac_genproc) == 0) org->updateGeneticProcessors();
        //if((cycles % clock_frac_delete) == 0) org->updateDelete();
        //if((cycles % clock_frac_chemlinks) == 0) org->updateChemicalStorageLinks();
        org->updateGeneticProcessors();
        org->updateDelete();
        org->updateChemicalStorageLinks();
        org->incLifetime();
      };
    };

    cleanupDeadOrganisms();

    if((cycles % 100000) == 0) {
      database->save("autosave.xml");
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

    while(!database->empty() && (new_orgs.size() < MAX_PLANT_COUNT)) {
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
    if(organisms.size() > 0) return true;
    return false;
  };

  int RuntimeManager::saveDatabase() {
    return database->save();
  };

  int RuntimeManager::loadDatabase() {
    return database->load("database.xml");
  };

  std::list<Organism*> RuntimeManager::getOrganisms() {
    return organisms; 
  };
};