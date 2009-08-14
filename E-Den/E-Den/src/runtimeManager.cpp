// runtimeManager.cpp
// by Franz Koehler 2009

#include "runtimeManager.h"

namespace EDen {
  RuntimeManager::RuntimeManager() {
    
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
  };

  bool RuntimeManager::reset() {
    cycles = 0;
    deleteAll();
    return true;
  };

  bool RuntimeManager::add(Organism* param_org) {
    if(param_org) {
      organisms.push_front(param_org);
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
    for(std::list<ResourceProvider*>::iterator it = resourceProviders.begin(); it != resourceProviders.end(); it++) {
      if(*it) {
        (*it)->distibute();
      };
    };

    Organism* org;
    for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
      org = *it;
      if((org) && (org->getState() != BSP_dead)) {
        // Modulo something for each subsystem?  
        org->updateGeneticProcessors();
        org->updateDelete();
        org->updateChemicalStorageLinks();
      };
    };

    cleanupDeadOrganisms();

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

    organisms.clear();
    organisms.swap(new_orgs);

    return true;
  };

  unsigned long RuntimeManager::getCycleCount() {
    return cycles;
  };
};