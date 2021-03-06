// organism.cpp
// by Franz Koehler 2009

#include "globals.h"
#include "organism.h"
#include <stdio.h>
#include <iostream>

namespace EDen {
  Organism::Organism(std::string param_name, Bodypart* param_rootBodypart, RuntimeManager* param_runtimeManager): 
      name(param_name), groundpart(0), lifetime(0) 
  {
    runtimeManager = param_runtimeManager;
    rootBodypart = param_rootBodypart;
    registerBodypart(param_rootBodypart);
  };

  Organism::Organism(TiXmlElement* description, RuntimeManager* param_runtimeManager): groundpart(0), lifetime(0) {
    TiXmlElement* bodypartElement = description->FirstChildElement("Bodypart");
    description->QueryIntAttribute("Lifetime",&lifetime);
    name = description->Attribute("Name");

    runtimeManager = param_runtimeManager;
    rootBodypart = new Bodypart(bodypartElement,this);
    registerBodypart(rootBodypart);
  };
  
  Organism::~Organism() {
    bool allreadyUnregistered = false;

    if(runtimeManager) {
		std::list<Groundpart*> groundparts = runtimeManager->getGroundparts();

		for(std::list<Groundpart*>::iterator it = groundparts.begin(); it != groundparts.end(); it++) {
		  if(*it) {
		    if((*it) == groundpart) {
			  allreadyUnregistered = true;
		    };
		  }
		  (*it)->unregisterOrganism(this);
		};
	}

    if(connectedToGoundpart()) {
	  if(!allreadyUnregistered) {
		groundpart->unregisterOrganism(this);
	  }
      removeChemStorageLinksWithGroundpart(groundpart);
      groundpart = 0;
    };

    geneticProcessors.clear();

    updateDelete();
    //rootBodypart->destroy();
    delete rootBodypart; // !!!!!?!?!?!?!!!! CRASHES WITH DATABASE LOADED 
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

  int Organism::getLifetime() {
    return lifetime;
  };

  bool Organism::incLifetime() {
    lifetime++;
    return true;
  };

  bool Organism::registerBodypart(Bodypart* param_bodypart) {
	bool foundOne = false;
	for(BodypartListIterator it = bodyparts.begin(); it != bodyparts.end(); it++) {
		if((*it) == param_bodypart) {
			foundOne = true;
			break;
		}
	}
	
	if(!foundOne) {
		bodyparts.push_back(param_bodypart);
	}

	foundOne = false;
	GeneticProcessor* proc = param_bodypart->getGeneticProcessor();
	for(GeneticProcessorListIterator it = geneticProcessors.begin(); it != geneticProcessors.end(); it++) {
		if((*it) == proc) {
			foundOne = true;
			break;
		}
	}

	if(!foundOne) {
		geneticProcessors.push_back(proc);
	}
    
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
    if(runtimeManager) {
      runtimeManager->unregisterBodypart(param_bodypart);
    };

    for(BodypartListIterator it = bodyparts.begin(); it != bodyparts.end(); it++) {
      if((*it) == param_bodypart) {
        bodyparts.erase(it);
        param_bodypart->setParentOrganism(0);
        
        foundSomething = true;
        break;
      };
    };

    return foundSomething;
  };

  int Organism::getBodypartCount() {
    return bodyparts.size();
  };

  int Organism::getFitnessValue() {
	  float value = 0;
	  for(BodypartListIterator it = bodyparts.begin(); it != bodyparts.end(); it++) {
		  if((*it)->getBodypartType() == BodypartType::BPT_Leaf) value += 5;
		  else if((*it)->getBodypartType() == BodypartType::BPT_Seed) value += 0.5f;
		  else if((*it)->getBodypartType() == BodypartType::BPT_Branch) value -= 0.5;
		  else if((*it)->getBodypartType() == BodypartType::BPT_Stick) value += 0.3;
	  }
	  return (int)value;
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
//    bool foundSomething = false;
//    do { 
//      foundSomething = false;
      for(GeneticProcessorListIterator it = geneticProcessors.begin(); it != geneticProcessors.end(); it++) {
        if((*it)->getBodypart() == param_bodypart) {
          geneticProcessors.erase(it);
//          foundSomething = true;
          break;
        };
      };
//    } while(foundSomething == true);

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

  Groundpart* Organism::getGroundpart() {
	  return groundpart;
  };

#ifdef USE_CL
  bool Organism::updateChemicalStorageLinks(CLDriver* driver) {
    ChemicalStorageLinkList newStoragLinks;
    ChemicalStorageLink* lastLink;
    
    while(!storageLinks.empty()) {
      //do {
        lastLink = storageLinks.back();
        lastLink->update(driver);
      //} 
      //while(lastLink != storageLinks.back());
      storageLinks.pop_back();
      newStoragLinks.push_back(lastLink);
    };

    if(!newStoragLinks.empty()) {
      newStoragLinks.push_back(newStoragLinks.front());
      newStoragLinks.pop_front();
    };

    storageLinks.swap(newStoragLinks);

    return true;
  };

  bool Organism::updateGeneticProcessors(CLDriver* driver) {
    GeneticProcessorList newProcs;
    GeneticProcessor* lastProc;
    
    while(!geneticProcessors.empty()) {
      do {
        lastProc = geneticProcessors.back();
        lastProc->executeRelevantClauses();
      } 
      while((!geneticProcessors.empty()) && (lastProc != geneticProcessors.back()));
      if(!geneticProcessors.empty()) geneticProcessors.pop_back();
      newProcs.push_back(lastProc);
    };

    geneticProcessors.swap(newProcs);
    
    // old version .. faster, but crashes :P
    //for(GeneticProcessorListIterator it = geneticProcessors.begin(); it != geneticProcessors.end(); it++) {
    //  (*it)->executeRelevantClauses();
    //};
    return true;
  };

  bool Organism::update(CLDriver* driver) {
    incLifetime();
    updateChemicalStorageLinks(driver);
    updateDelete();
    updateGeneticProcessors(driver);  
    return true;
  };
#else
    bool Organism::updateChemicalStorageLinks() {
    ChemicalStorageLinkList newStoragLinks;
    ChemicalStorageLink* lastLink;
    
    while(!storageLinks.empty()) {
      //do {
        lastLink = storageLinks.back();
        lastLink->update();
      //} 
      //while(lastLink != storageLinks.back());
      storageLinks.pop_back();
      newStoragLinks.push_back(lastLink);
    };

    if(!newStoragLinks.empty()) {
      newStoragLinks.push_back(newStoragLinks.front());
      newStoragLinks.pop_front();
    };

    storageLinks.swap(newStoragLinks);

    // last version
    //for(ChemicalStorageLinkListIterator it = storageLinks.begin(); it != storageLinks.end(); it++) {
    //  (*it)->update();
    //};

    //storageLinks.push_back(storageLinks.front());
    //storageLinks.pop_front();
    return true;
  };

  bool Organism::updateGeneticProcessors() {
    GeneticProcessorList newProcs;
    GeneticProcessor* lastProc;
    
//	geneticProcessors.unique();
	while(!geneticProcessors.empty()) {
//      do {
		lastProc = geneticProcessors.front();
        lastProc->executeRelevantClauses();
		geneticProcessors.remove(lastProc);
//      } 
//      while((!geneticProcessors.empty()) && (lastProc != geneticProcessors.back()));
//      if(!geneticProcessors.empty()) 
		if(lastProc->getBodypart()->getParentOrganism() == this) {
			newProcs.push_back(lastProc);
		}
    };

    geneticProcessors.swap(newProcs);
    
    // old version .. faster, but crashes :P
    //for(GeneticProcessorListIterator it = geneticProcessors.begin(); it != geneticProcessors.end(); it++) {
    //  (*it)->executeRelevantClauses();
    //};
    return true;
  };

  bool Organism::update() {
    incLifetime();
    updateChemicalStorageLinks();
    updateDelete();
    updateGeneticProcessors();  
    return true;
  };
#endif

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
      }
	  else {
		removeChemStorageLinksWithBodypart(bp);
		removeGeneticProcessorWithBodypart(bp);
	  }
      
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
    if( !foundOne ) {
      bodypartsToDelete.push_back(param_bodypart);
      //runtimeManager->unregisterBodypart(param_bodypart);
    };
    return true;
  };

  RuntimeManager* Organism::getRuntimeManager() {
    return runtimeManager; 
  };

  TiXmlElement* Organism::getXmlElement() {
    TiXmlElement* element;

    element = new TiXmlElement("Organism");
    element->SetAttribute("Name",getName());
    element->SetAttribute("State",(int)getState());
    element->SetAttribute("Lifetime",getLifetime());
    element->LinkEndChild(rootBodypart->toXmlElement());

    return element;
  };
};