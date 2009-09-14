// groundpart.cpp
// by Franz Koehler 2009

#include "groundpart.h"

namespace EDen {
  Groundpart::Groundpart() {
    chemStorage = new ChemicalStorage();
    chemStorage->setMaxAmount("Energie",0.0f);
    chemStorage->setMaxAmount("Sonne",0.0f);
    chemStorage->setMaxAmount("Wasser",2.0e9);
    chemStorage->setMaxAmount("Goo",3000.0f);
  };

  Groundpart::~Groundpart() {
    delete chemStorage;
  };

  ChemicalStorage* Groundpart::getChemicalStorage() {
    return chemStorage;
  };
};