// organism.cpp
// by Franz Koehler 2009

#include "organism.h"
#include <stdio.h>
#include <iostream>

namespace EDen {
  Organism::Organism(std::string param_name, Bodypart* param_rootBodypart, RuntimeManager* param_runtimeManager): 
      name(param_name), groundpart(0) 
  {
    runtimeManager = param_runtimeManager;
    rootBodypart = param_rootBodypart;
    registerBodypart(param_rootBodypart);

    if(runtimeManager) {
      runtimeManager->registerBodypart(rootBodypart);
    };
  };
  
  Organism::~Organism() {
    if(connectedToGoundpart()) {
      groundpart->unregisterOrganism(this);
      removeChemStorageLinksWithGroundpart(groundpart);
      groundpart = 0;
    };

    geneticProcessors.clear();

    updateDelete();
    rootBodypart->destroy();
    delete rootBodypart;
    rootBodypart = 0;

    {
      ChemicalStorageLink* lastLink;
      while(!(storageLinks.empty())) {
        lastLink = storageLinks.back();
        storageLinks.pop_back();
        delete lastLink;
      };
    }
  };

  std::string Organism::getName() {
    return name;
  };

  bool Organism::registerBodypart(Bodypart* param_bodypart) {
    bodyparts.push_back(param_bodypart);
    geneticProcessors.push_back(param_bodypart->getGeneticProcessor());
    param_bodypart->setParentOrganism(this);
    if(runtimeManager) {
      runtimeManager->registerBodypart(param_bodypart);
    };
    return true;
  };

  bool Organism::unregisterBodypart(Bodypart* param_bodypart) {
    bool foundSomething = false;

    removeChemStorageLinksWithBodypart(param_bodypart);
    removeGeneticProcessorWithBodypart(param_bodypart);

    for(BodypartListIterator it = bodyparts.begin(); it != bodyparts.end(); it++) {
      if((*it) == param_bodypart) {
        bodyparts.erase(it);
        param_bodypart->setParentOrganism(0);
        
        if(runtimeManager) {
          runtimeManager->unregisterBodypart(param_bodypart);
        };

        foundSomething = true;
        break;
      };
    };

    return foundSomething;
  };

  int Organism::getBodypartCount() {
    return bodyparts.size();
  };

  OrganismState Organism::getState() {
    if(rootBodypart) return rootBodypart->getBodypartState();
    else return BSP_dead; // Or BSP_Dead maybe?
  };

  bool Organism::addChemicalStorageLink(ChemicalStorageLink* param_chemLink) {
    storageLinks.push_back(param_chemLink);
    return true;
  };

  bool Organism::removeChemStorageLinksWithBodypart(Bodypart* param_bodypart) {
    bool foundSomething = false;
    ChemicalStorage* chemStorage = param_bodypart->getChemicalStorage();
    do { 
      foundSomething = false;
      for(ChemicalStorageLinkListIterator it = storageLinks.begin(); it != storageLinks.end(); it++) {
        if(((*it)->getS1() == chemStorage) || ((*it)->getS2() == chemStorage)) {
          storageLinks.erase(it);
          foundSomething = true;
          break;
        };
      };
    } while(foundSomething == true);
    return true;
  };

  bool Organism::removeChemStorageLinksWithGroundpart(Groundpart* param_groundpart) {
    bool foundSomething = false;
    ChemicalStorage* chemStorage = param_groundpart->getChemicalStorage();
    do { 
      foundSomething = false;
      for(ChemicalStorageLinkListIterator it = storageLinks.begin(); it != storageLinks.end(); it++) {
        if(((*it)->getS1() == chemStorage) || ((*it)->getS2() == chemStorage)) {
          storageLinks.erase(it);
          foundSomething = true;
          break;
        };
      };
    } while(foundSomething == true);
    return true;
  };

  bool Organism::addGeneticProcessor(GeneticProcessor* param_geneticProcessor) {
    geneticProcessors.push_back(param_geneticProcessor);
    return true;
  };
  
  bool Organism::removeGeneticProcessorWithBodypart(Bodypart* param_bodypart) {
    bool foundSomething = false;
    do { 
      foundSomething = false;
      for(GeneticProcessorListIterator it = geneticProcessors.begin(); it != geneticProcessors.end(); it++) {
        if((*it)->getBodypart() == param_bodypart) {
          geneticProcessors.erase(it);
          foundSomething = true;
          break;
        };
      };
    } while(foundSomething == true);

    if(runtimeManager) {
      runtimeManager->unregisterBodypart(param_bodypart);
    };
    return true;
  };

  bool Organism::connectedToGoundpart() {
    if(groundpart == 0) return false;
    else return true;
  };

  bool Organism::connectToGoundpart(Groundpart* param_groundpart) {
    if(connectedToGoundpart()) {
      groundpart->unregisterOrganism(this);
      removeChemStorageLinksWithGroundpart(groundpart);
    };

    groundpart = param_groundpart;
    
    if(groundpart != 0) {
      groundpart->registerOrganism(this);
      storageLinks.push_front(new ChemicalStorageLink(groundpart->getChemicalStorage(),rootBodypart->getChemicalStorage()));
    };
    return true;
  };

  bool Organism::updateChemicalStorageLinks() {
    for(ChemicalStorageLinkListIterator it = storageLinks.begin(); it != storageLinks.end(); it++) {
      (*it)->update();
    };

    storageLinks.push_back(storageLinks.front());
    storageLinks.pop_front();
    return true;
  };

  bool Organism::updateGeneticProcessors() {
    GeneticProcessorList newProcs;
    GeneticProcessor* lastProc;
    
    //while(!geneticProcessors.empty()) {
    //  lastProc = geneticProcessors.back();
    //  do {
    //    lastProc->executeRelevantClauses();
    //  } 
    //  while(lastProc != geneticProcessors.back());
    //  geneticProcessors.pop_back();
    //  newProcs.push_back(lastProc);
    //};

    //geneticProcessors.swap(newProcs);
    
    // old version .. faster, but crashes :P
    for(GeneticProcessorListIterator it = geneticProcessors.begin(); it != geneticProcessors.end(); it++) {
      (*it)->executeRelevantClauses();
    };
    return true;
  };

  bool Organism::update() {
    updateChemicalStorageLinks();
    updateGeneticProcessors();  
    updateDelete();
    return true;
  };

  Bodypart* Organism::getRootBodypart() {
    return rootBodypart; 
  };

  bool Organism::updateDelete() {
    Bodypart* bp;
    while(!bodypartsToDelete.empty()) {
      bp = bodypartsToDelete.front();
      bodypartsToDelete.pop_front();

      if(bp->getParentBodypart() != 0) {
        bp->getParentBodypart()->unregisterChildBodypart(bp);
  //      bp->setParentBodypart(0);
      };

      if(bp == rootBodypart) rootBodypart = 0;

      if(bp->getParentOrganism() != 0) {
        bp->detachFromOrganism();
      };
      
      delete bp;
    }
    
    

    return true;  
  };

  bool Organism::deleteBodypart(Bodypart* param_bodypart) {
    bool foundOne = false;
    for(BodypartListIterator it = bodypartsToDelete.begin(); it != bodypartsToDelete.end(); it++) {
      if( (*it) == param_bodypart ) {
        foundOne = true;
        break;
      }
    };
    if( !foundOne ) bodypartsToDelete.push_back(param_bodypart);
    return true;
  };

  RuntimeManager* Organism::getRuntimeManager() {
    return runtimeManager; 
  };

};