// runtimeManager.cpp
// by Franz Koehler 2009

#include "globals.h"
#include "runtimeManager.h"

#define NUM_CORES 4
#define MAX_PLANT_COUNT (NUM_CORES + (NUM_CORES / 2))

#define CANDIDATES_LEVEL (150 / 25)
#define DATABASE_UPDATE_CYCLES 100

namespace EDen {
  boost::mutex RuntimeManager::orgsToProcessMutex;
  boost::mutex RuntimeManager::orgsToDeleteMutex;
  std::list<Organism*> RuntimeManager::orgsToProcess;

  unsigned long RuntimeManager::cycles = 0;
  int RuntimeManager::cps = 0;
  unsigned int RuntimeManager::bodypartCount = 0;
  unsigned long RuntimeManager::lastPlantAddedAt = 0;

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

  Groundpart* RuntimeManager::getGroundpartWithEmptySpace() {
	  if(groundparts.empty()) 
		  return 0;

	  for(GroundpartListIterator it = groundparts.begin(); it != groundparts.end(); it++) {
		  if((*it)->hasEmptySpaces()) {
			  return *it;
		  };
	  };

	  return 0;
  };

  Groundpart* RuntimeManager::getGroundpartWithEmptySpaceAndSpecies() {
	  if(groundparts.empty()) 
		  return 0;

	  for(GroundpartListIterator it = groundparts.begin(); it != groundparts.end(); it++) {
		  if((*it)->hasEmptySpaces()) {
			  if((*it)->getSpeciesDatabase()->size() > 0) {
				  return *it;
			  };
		  };
	  };

	  return 0;
  };

  bool RuntimeManager::reset() {
    preferedOrganismCount = MAX_PLANT_COUNT ;
    candidatesTreshold = CANDIDATES_LEVEL ;
    numthreads = (int)(NUM_CORES * 1.6f);
    state = RMS_Normal;
    
    cycles = 0;
    bodypartCount = 0;

    deleteAll();

	baadfoodHappened = false;

    return true;
  };

  bool RuntimeManager::add(Organism* param_org) {
    if(param_org) {
	  if(param_org->getState() != BSP_dead) {
		  if(!param_org->connectedToGoundpart()) {
			  database->push(param_org);
		  }
	  }
	  else {
		orgsToDelete.push_back(param_org);
	  }
      return true; 
    } else return false;
  };
  
  bool RuntimeManager::add(ResourceProvider* param_resourceProvider) {
    if(param_resourceProvider) {
      resourceProviders.push_front(param_resourceProvider);
      observers.push_front(param_resourceProvider);
      return true;
    } else return false;
  };

  bool RuntimeManager::add(BodypartObserver* observer) {
    if(observer) {
      observers.push_front(observer);
      return true;
    } else return false;
  };

  bool RuntimeManager::add(Groundpart* param_groundpart) {
    if(param_groundpart) {
      groundparts.push_back(param_groundpart);
      return true;
    } else return false;
  };

  bool RuntimeManager::registerBodypart(Bodypart* param_bodypart) {
    //boost::mutex::scoped_lock lock(bodypartsMutex);
    //bodypartCount++; // does not work well, TODO: track down why
    if(param_bodypart) {
      for(std::list<BodypartObserver*>::iterator it = observers.begin(); it != observers.end(); it++) {
        if(*it) (*it)->addBodypart(param_bodypart);
      };
    }
    return true;
  };

  bool RuntimeManager::unregisterBodypart(Bodypart* param_bodypart) {
    //boost::mutex::scoped_lock lock(bodypartsMutex);
    for(std::list<BodypartObserver*>::iterator it = observers.begin(); it != observers.end(); it++) {
      if(*it) (*it)->removeBodypart(param_bodypart);
    };
    //bodypartCount--;
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
      };
    };

    for(std::list<BodypartObserver*>::iterator it = observers.end(); it != observers.begin(); it--) {
      if(*it == param_resourceProvider) {
        observers.erase(it);
        break;
      };
    };
    return true;
  };

  bool RuntimeManager::remove(BodypartObserver* observer) {
    for(std::list<BodypartObserver*>::iterator it = observers.end(); it != observers.begin(); it--) {
      if(*it == observer) {
        observers.erase(it);
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

  bool RuntimeManager::addDelete(Bodypart* bodypart) {
	  boost::mutex::scoped_lock lock(orgsToDeleteMutex);
	  bodypartsToDelete.push_back(bodypart);
	  return true;
  };

  bool RuntimeManager::addDelete(Organism* org) {
	  boost::mutex::scoped_lock lock(orgsToDeleteMutex);
	  orgsToDelete.push_back(org);
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

    for(int i = 0; i < numthreads; i++) {
      threadpool.create_thread(&processOrgs);
    };
    threadpool.join_all();

    cleanupDeadOrganisms();

	deleteOrgsToDelete();

    if((cycles % 10000) == 9999) {
      saveDatabase("autosave.xml");
    };

    if((cycles % DATABASE_UPDATE_CYCLES) == 0) {
      database->update();
	  for(std::list<Groundpart* >::iterator it = groundparts.begin(); it != groundparts.end(); it++) {
		  SpeciesDatabase* db = (*it)->getSpeciesDatabase();
		  if(db) db->update();
	  };
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
	  if(org) {
          if(org->getState() != BSP_dead) {
              new_orgs.push_front(org);
		  }
          else {
			  addLifetimeToAvgAccumulator(org->getLifetime());
			  orgsToDelete.push_back(org);
		  }
	  }
    };

	// Adding new organisms
	bool noSeeds = false;
	Groundpart *gp = getGroundpartWithEmptySpaceAndSpecies();
	if(gp == 0) gp = getGroundpartWithEmptySpace();

	//unsigned long debugTemp = lastPlantAddedAt + (getAvgLifetime() / preferedOrganismCount);

    while(gp != 0 && (lastPlantAddedAt + ((getAvgLifetime() * 0.95f) / preferedOrganismCount) <= cycles) && (noSeeds == false)) {
		Organism* seed = gp->addSeedFromDb();
		if(seed == 0) {
			seed = getNextSeed();
			if(seed != 0) {
				new_orgs.push_back(seed);
				seed->connectToGoundpart(gp);
				gp = getGroundpartWithEmptySpace();
				lastPlantAddedAt = cycles;
			}
			else {
				noSeeds = true;
			}
		}
		else {
			new_orgs.push_back(seed);
			lastPlantAddedAt = cycles;
		};
		gp = getGroundpartWithEmptySpace();
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

  void RuntimeManager::addLifetimeToAvgAccumulator(unsigned int lifetime) {
	unsigned int preferedAvgListSize = maxi(20, mini(getOrganismCount() * 3,500));
	while(avgLifetimAccumulator.size() > preferedAvgListSize - 1) {
		avgLifetimAccumulator.pop_back();
	}
	avgLifetimAccumulator.push_front(lifetime);
  };

  unsigned int RuntimeManager::getAvgLifetime() {
	unsigned long sum = 0;
	unsigned int num = 0;
	for(std::list<unsigned int>::iterator it = avgLifetimAccumulator.begin(); it != avgLifetimAccumulator.end(); it++) {
		sum += *it;
		num++;
	};
	if(num > 0) {
		return sum/num;
	}
	else {
		return 1;
	}
  };

  bool RuntimeManager::orgsAlive() {
    if((organisms.size() > 0) || (database->size() > 0)) return true;
	for(std::list<Groundpart*>::iterator it = groundparts.begin(); it != groundparts.end(); it++) {
		SpeciesDatabase* database = (*it)->getSpeciesDatabase();
		if(database != 0) {
			if(database->size() > 0) return true;
		};
	};

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

  void RuntimeManager::deleteOrgsToDelete() {
	  Organism* org;
	  Bodypart* bp;
	  orgsToDelete.sort();
	  orgsToDelete.unique();
	  while(!orgsToDelete.empty()) {
		  org = orgsToDelete.front();
		  orgsToDelete.pop_front();
		  //if((int)org == 0xbaadf00d) baadfoodHappened = true; else // caused by concurrent acces to lists
		  {delete org;}
	  };

	  while(!bodypartsToDelete.empty()) {
		  bp = bodypartsToDelete.front();
		  bodypartsToDelete.pop_front();
		  //if((int)bp == 0xbaadf00d) baadfoodHappened = true; else // caused by concurrent acces to lists
		  delete bp;
	  };
  };

  GroundpartList RuntimeManager::getGroundparts() {
	return groundparts;
  };

  std::string RuntimeManager::getDebugOut(bool shortInfo) {
    //return database->getDebugOut();
    std::string out = "";
    char str[64];

	if(baadfoodHappened) out += "!";

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

      sprintf(str,"[%d/%d@%d|%d]",getOrganismCount(),getPreferedOrganismCount(), getCps(),getCycleCount());
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

  unsigned int RuntimeManager::getBodypartCount() {
	int count = 0;

	for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
		count += (*it)->getBodypartCount();
    };

    return count;
  };
  
};