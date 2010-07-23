// nelOrganismPrinter.h
// by Franz Koehler 2010

#ifndef _E_DEN_NEL_ORGANISMPRINTER_HEADER_
#define _E_DEN_NEL_ORGANISMPRINTER_HEADER_

#include "organism.h"
#include "runtimeManager.h"
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>

namespace EDen {

  const std::string pathToCylinderShape = "h:/src/ryzom/code/nel/samples/pacs/shapes/cylinder.shape";
  const std::string pathToPlaneShape = "h:/src/ryzom/code/nel/samples/pacs/shapes/rectangle.shape";

  class NEL_SunlightProvider;

  class NELBodypartInformation : public BodypartInformation {
  public:
    NL3D::UInstance entity; 
  };

  class NELOrganismPrinter : public BodypartObserver {
  private:

  protected:
    NL3D::UScene *scene;
    std::list<Organism*> organisms;

    NL3D::UInstance createEntity(Bodypart* bp);
  public:
    NELOrganismPrinter(NL3D::UScene *Scene);
    ~NELOrganismPrinter();

    virtual BodypartInformation* updateInformation(Bodypart* bodypart, BodypartInformation* information = 0);

    virtual bool bodypartAdded(Bodypart* bodypart, BodypartInformation* information);
    virtual bool bodypartRemoved(Bodypart* bodypart, BodypartInformation* information);

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