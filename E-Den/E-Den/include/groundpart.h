// groundpart.h
// by Franz Koehler 2009

#ifndef _E_DEN_GROUNDPART_HEADER_
#define _E_DEN_GROUNDPART_HEADER_

namespace EDen {
  class Groundpart;
};

#include "globals.h"
#include "chemicalSystem.h"
#include "organism.h"

namespace EDen {

  class Groundpart {
  protected:
    ChemicalStorage* chemStorage;
  public:
    Groundpart(float maxWater = 2.0e8, float maxGoo = 2.0e8);
    ~Groundpart();

    ChemicalStorage* getChemicalStorage();

    bool registerOrganism(Organism* param_organism) {return true;};
    bool unregisterOrganism(Organism* param_organism) {return true;};
  };

};

#endif