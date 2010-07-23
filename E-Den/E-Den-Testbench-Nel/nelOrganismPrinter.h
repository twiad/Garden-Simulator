// nelOrganismPrinter.h
// by Franz Koehler 2010

#ifndef _E_DEN_NEL_ORGANISMPRINTER_HEADER_
#define _E_DEN_NEL_ORGANISMPRINTER_HEADER_

#include "organism.h"
#include "runtimeManager.h"
#include <nel/3d/u_scene.h>

namespace EDen {

  class NEL_SunlightProvider;

  class NELOrganismPrinter : public BodypartObserver {
  private:

  protected:
    NL3D::UScene *scene;
    std::list<Organism*> organisms;
  public:
    NELOrganismPrinter(NL3D::UScene *Scene);
    ~NELOrganismPrinter();

    virtual bool print();
  };

  class NEL_SunlightProvider : public ResourceProvider {
  public:
    NEL_SunlightProvider() : ResourceProvider("Sonne",BPT_Leaf,0.9f) {
      amount = 0.9f;
      reactiveBodypartType = BPT_Leaf;
      chemicalName = "Sonne";
    };

    ~NEL_SunlightProvider() {};

    bool setFactor(Bodypart* param_bodypart ,float param_factor);
  };
}

#endif