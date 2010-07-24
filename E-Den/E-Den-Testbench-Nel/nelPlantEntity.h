// nelPlantEntity.h
// by Franz Koehler 2010

#ifndef _E_DEN_NEL_PLANTENTITY_HEADER_
#define _E_DEN_NEL_PLANTENTITY_HEADER_

#include <nel/misc/vector.h>
#include "organism.h"
#include "runtimeManager.h"

namespace EDen {
  class PlantEntity {
  protected:
    Organism* org;
    NLMISC::CVector pos;

  public:
    PlantEntity(Organism* organism, NLMISC::CVector Position);
    ~PlantEntity();

    Organism* getOrganism();
    NLMISC::CVector getPosition();
  };

  class PlantEntityManager : public Groundpart {
  protected:
    std::list<PlantEntity*> plants;  
    float dimx, dimy;
    NLMISC::CVector pos;

    Randomizer rnd;

    void clear();
  public:
    PlantEntityManager(NLMISC::CVector position = NLMISC::CVector(0.f,0.f,0.f), float p_dimx = 100.0f, float p_dimy = 100.0f, float maxWater = 2.0e8, float maxGoo = 2.0e8);
    ~PlantEntityManager();

    virtual bool registerOrganism(Organism* param_organism);
    virtual bool unregisterOrganism(Organism* param_organism);

    std::list<PlantEntity*> getPlants();
  };
}

#endif