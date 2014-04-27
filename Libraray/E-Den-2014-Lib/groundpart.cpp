// groundpart.cpp
// by Franz Koehler 2009

#include "globals.h"
#include "groundpart.h"

namespace EDen {
  Groundpart::Groundpart(std::string param_name, float maxWater, float maxGoo, int emptySpaces) {
    chemStorage = new ChemicalStorage();
    chemStorage->setMaxAmount("Energie",0.0f);
    chemStorage->setMaxAmount("Sonne",0.0f);
    chemStorage->setMaxAmount("Wasser",maxWater);
    chemStorage->setMaxAmount("Goo",maxGoo);
	this->emptySpaces = emptySpaces;
	name = param_name;
	speciesDatabase = 0;
  };

  Groundpart::~Groundpart() {
	if(speciesDatabase != 0) {
		std::string filename = name;
	    filename.append(".xml");
		speciesDatabase->save(filename);
		delete speciesDatabase;
	};
    delete chemStorage;
  };

  bool Groundpart::hasEmptySpaces() {
	boost::mutex::scoped_lock lock(mutex);
	return emptySpaces > 0;
  };

  bool Groundpart::incEmptySpaces() {
	 boost::mutex::scoped_lock lock(mutex);
	 emptySpaces += 1;
	 return true;
  };

  bool Groundpart::decEmptySpaces() {
	 boost::mutex::scoped_lock lock(mutex);
	 if((emptySpaces + organisms.size()) > 1) {
		emptySpaces -= 1;
		return true;
	 }
	 else {
		return false;
	 }
  };

  void Groundpart::getNumOrganismsAndEmptySpaces(int* numOrganisms, int* numEmptySpaces) {
	  *numOrganisms = organisms.size();
	  *numEmptySpaces = emptySpaces;
  };

  std::string Groundpart::getName() {
	return name;
  };

  ChemicalStorage* Groundpart::getChemicalStorage() {
    return chemStorage;
  };

  SpeciesDatabase* Groundpart::getSpeciesDatabase() {
    return speciesDatabase;
  };

  void Groundpart::setSpeciesDatabase(SpeciesDatabase* param_database) {
	speciesDatabase = param_database;
  };

  Organism* Groundpart::addSeedFromDb() {
	  if(!hasEmptySpaces()) return 0;

	  if(speciesDatabase != 0) {
		  Organism* seed = speciesDatabase->pull();
		  if(seed != 0) {
			  seed->connectToGoundpart(this);
		  };
		  return seed;
	  }
	  else {
		  return 0;
	  };
  };

  bool Groundpart::saveDatabase() {
	  if(speciesDatabase != 0) {
		std::string filename = name;
		filename.append(".xml");
		speciesDatabase->save(filename);
	  };

	  return true;
  };

  std::list<Organism*> Groundpart::getOrganisms() {
	return organisms;
  };

  bool Groundpart::registerOrganism(Organism* param_organism) {
	  boost::mutex::scoped_lock lock(mutex);
	  organisms.push_back(param_organism);
	  emptySpaces -= 1;
	  return true;
  };

  bool Groundpart::unregisterOrganism(Organism* param_organism) {
	  boost::mutex::scoped_lock lock(mutex);
	  
	  for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
		if((*it) == param_organism) {
			emptySpaces += 1;
		};
	  }
	  organisms.remove(param_organism);
	  
	  return true;
  };

  bool Groundpart::dropOrganism(Organism* droppedOrganism, Bodypart* parentBodypart) {
	if(speciesDatabase != 0) {
		if(droppedOrganism->getState() == BSP_seed) {
			if(Randomizer::value(0.0f,1.0f) >= 0.5f) {
				speciesDatabase->push(droppedOrganism);
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		};
	}
	else {
		return false;
	};

	return false;
  };

  SingleDimensionHeightmapGroundpart::SingleDimensionHeightmapGroundpart(std::string name, int param_width, float maxWater, float maxGoo, int emptySpaces) : Groundpart(name, maxWater, maxGoo, emptySpaces) {
	  width = param_width;
	  heightmap = new int[width];
	  randomizeHeights();
  };

  SingleDimensionHeightmapGroundpart::~SingleDimensionHeightmapGroundpart() {
	  delete heightmap;
  };

  void SingleDimensionHeightmapGroundpart::randomizeHeights() {
	  int currentHeight = 5;
	  
	  for(int i = 0; i < width; i++) {
		  float randomValue = Randomizer::value(0.0f,1.0f);
		  if(randomValue < 0.1f) {
			  currentHeight -= 2;
		  }
		  else if(randomValue < 0.4f) {
			  currentHeight -= 1;
		  }
		  else if(randomValue < 0.6f) {
			  currentHeight = currentHeight;
		  }
		  else if(randomValue < 0.9f) {
			  currentHeight += 1;
		  }
		  else {
			  currentHeight += 2;
		  }

		  if(currentHeight < 1) currentHeight = 1;
		  if(currentHeight > 100) currentHeight = 100;

		  heightmap[i] = currentHeight;
	  };
  };

  bool SingleDimensionHeightmapGroundpart::registerOrganism(Organism* param_organism) {
	  bool retval = Groundpart::registerOrganism(param_organism);

	  int plantPosX = getOrganismX(param_organism);
	  alivePlantPositions[plantPosX].push_back(param_organism);
	  
	  return retval;
  };

  bool SingleDimensionHeightmapGroundpart::unregisterOrganism(Organism* param_organism) {
	  bool retval = Groundpart::unregisterOrganism(param_organism);

	  int plantPosX = getOrganismX(param_organism);

	  boost::mutex::scoped_lock lock(mutex);
	  alivePlantPositions[plantPosX].remove(param_organism);
	  if(alivePlantPositions[plantPosX].size() == 0) {
		  alivePlantPositions.erase(plantPosX);
	  }

      plantPositionMemoryIterator it = plantPositionMemory.find(param_organism);
	  if(it != plantPositionMemory.end()) {
         plantPositionMemory.erase(it);
	  };

	  return true;
  };

  bool SingleDimensionHeightmapGroundpart::dropOrganism(Organism* droppedOrganism, Bodypart* parentBodypart) {
	if(speciesDatabase != 0) {
		/*if(droppedOrganism->getState() == BSP_seed) {
			speciesDatabase->push(droppedOrganism);
			return true;
		}
		else {
			return false;
		};*/
		if(droppedOrganism->getState() == BSP_seed) {
			if(parentBodypart == 0) {
				return false;
			};
			int bodypartPosX = getBodypartPosX(parentBodypart);
			if((bodypartPosX >= 0) && (bodypartPosX < width)) {
				speciesDatabase->push(droppedOrganism);
				{
					boost::mutex::scoped_lock lock(mutex);
					plantPositionMemory.insert(std::pair<Organism* , int>(droppedOrganism, bodypartPosX));
				}
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		};
	}
	else {
		return false;
	};

	return false;
  };

  int SingleDimensionHeightmapGroundpart::getOrganismX(Organism* param_organism) {
	  boost::mutex::scoped_lock lock(mutex);
	  plantPositionMemoryIterator it = plantPositionMemory.find(param_organism);
	  if(it == plantPositionMemory.end()) {
		int numOrgs = organisms.size();
		if(numOrgs <= 1) {
			return width/2;
		}
		else if(numOrgs <= 10) {
			int newPosX = (int)Randomizer::value(width/3.0f,width/3.0f*2.0f);
			plantPositionMemory.insert(std::pair<Organism* , int>(param_organism, newPosX));
			return newPosX;
		}
		else {
			int newPosX = (int)Randomizer::value(width/6.0f,width/6.0f*5.0f);
			plantPositionMemory.insert(std::pair<Organism* , int>(param_organism, newPosX));
			return newPosX;
		};
	  }
	  else {
		return (it->second);
	  }
  };

  int SingleDimensionHeightmapGroundpart::getBodypartPosX(Bodypart* param_bodypart) {
	  Organism * org = param_bodypart->getParentOrganism();
	  int orgX = getOrganismX(org);
	  if(Randomizer::value(0.0f,1.0f) > 0.5) {
		  return orgX + Randomizer::value(3.0f,width / 50.0f);
	  }
	  else {
		  return orgX - Randomizer::value(3.0f,width / 50.0f);
	  }
  };

  Organism* SingleDimensionHeightmapGroundpart::getOrganismNearX(int posX) {
	  Organism* orgOut = 0;
		
	  bool leftBoundHit = false;
	  bool rightBoundHit = false;

	  int i = 0;

	  while(orgOut == 0 && (!leftBoundHit) && (!rightBoundHit)) {
		if((posX + i) > width) {
			rightBoundHit = true;
		}
		if((posX - i) < 0) {
			leftBoundHit = true;
		}

		if((!rightBoundHit) && alivePlantPositions.count(posX + i) > 0) {
			orgOut = alivePlantPositions[posX + i].back();
		}
		else if((!leftBoundHit) && alivePlantPositions.count(posX - i) > 0) {
			orgOut = alivePlantPositions[posX - i].back();
		}

		i++;
	  }
	  
	  return orgOut;
  };

  bool SingleDimensionHeightmapGroundpart::isOccupiedByAlivePlant(int posX) {
	  return alivePlantPositions.count(posX) > 0;
  };

  float SingleDimensionHeightmapGroundpart::getHeightAt(int posX) {
	  if(posX < 0) return 0;
	  if(posX >= width) return 0;
	  return ((float)heightmap[posX]) / 5.0f;
  };

  int SingleDimensionHeightmapGroundpart::getWidth() {
	return width;
  };
};