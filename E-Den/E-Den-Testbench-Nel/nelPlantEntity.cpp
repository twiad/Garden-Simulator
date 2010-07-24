// nelPlantEntity.cpp by Franz Koehler @ 2010

#include "nelPlantEntity.h"

using namespace NLMISC;
using namespace NL3D;
using namespace EDen;

namespace EDen {
  PlantEntity::PlantEntity(Organism* organism, CVector position) {
    org = organism;
    pos = position;
  };

  PlantEntity::~PlantEntity() {
    
  };

  Organism* PlantEntity::getOrganism() {
    return org;
  };

  CVector PlantEntity::getPosition() {
    return pos; 
  };

  PlantEntityManager::PlantEntityManager(CVector position, float p_dimx, float p_dimy, float maxWater, float maxGoo) : Groundpart(maxWater,maxGoo) {
    dimx = p_dimx;
    dimy = p_dimy;
    pos = position;
  };

  PlantEntityManager::~PlantEntityManager() {
    clear();
  };

  bool PlantEntityManager::registerOrganism(Organism* organism) {
    // check if plant exists
    for(std::list<PlantEntity*>::iterator it = plants.begin(); it != plants.end(); it++) {
      if((*it)->getOrganism() == organism) {
        return false;
      };
    };

    PlantEntity* plant = new PlantEntity(organism,CVector(pos.x + rnd.value(0.0f,dimx), pos.y + rnd.value(0.0f,dimy), pos.z));
    plants.push_back(plant);
    return true;
  };

  bool PlantEntityManager::unregisterOrganism(Organism* organism) {
    for(std::list<PlantEntity*>::iterator it = plants.begin(); it != plants.end(); it++) {
      if((*it)->getOrganism() == organism) {
        PlantEntity* plant = (*it);    
        plants.erase(it);
        delete plant;
        break;
        return true;
      };
    };
    return false;
  };

  std::list<PlantEntity*> PlantEntityManager::getPlants() {
    return plants; 
  };

  void PlantEntityManager::clear() {
    while(!plants.empty()) {
      delete plants.back();
      plants.pop_back();
    };
  };
};