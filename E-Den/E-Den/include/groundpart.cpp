// groundpart.cpp
// by Franz Koehler 2009

#include "groundpart.h"

namespace EDen {
  Groundpart::Groundpart(float maxWater, float maxGoo) {
    chemStorage = new ChemicalStorage();
    chemStorage->setMaxAmount("Energie",0.0f);
    chemStorage->setMaxAmount("Sonne",0.0f);
    chemStorage->setMaxAmount("Wasser",maxWater);
    chemStorage->setMaxAmount("Goo",maxGoo);
  };

  Groundpart::~Groundpart() {
    delete chemStorage;
  };

  ChemicalStorage* Groundpart::getChemicalStorage() {
    return chemStorage;
  };
};