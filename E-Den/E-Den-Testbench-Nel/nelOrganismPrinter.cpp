// nelOrganismPrinter.cpp by Franz Koehler @ 2010

#include "nelOrganismPrinter.h"

#include <nel/misc/path.h>
#include <nel/misc/file.h>
#include <nel/misc/common.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>

using namespace NLMISC;
using namespace NL3D;
using namespace EDen;

namespace EDen {
  NELOrganismPrinter::NELOrganismPrinter(NL3D::UScene *Scene) {
    scene = Scene; 
    
    CPath::addSearchPath(CFile::getPath(pathToCylinderShape), true, false);
  };

  NELOrganismPrinter::~NELOrganismPrinter() {
    
  };

  bool NELOrganismPrinter::print() {
    return true;
  };

  UInstance NELOrganismPrinter::createEntity(Bodypart* bp) {
    UInstance Entity = scene->createInstance(pathToCylinderShape);

		// if we can't create entity, skip it
    if (!Entity.empty()) {
		  Entity.setTransformMode(UTransformable::RotEuler);
      Entity.setScale(0.2f * bp->getSize(),0.2f * bp->getSize(),1.f * bp->getSize());
    }
    return Entity;
  };

  BodypartInformation* NELOrganismPrinter::updateInformation(Bodypart* bodypart, BodypartInformation* information) {
    if(information) {
      return information; // could chec for valid entity here
    }
    else {
      NELBodypartInformation* info = new NELBodypartInformation();
      info->entity = createEntity(bodypart);
      return info;
    };
  };

  bool NELOrganismPrinter::bodypartAdded(Bodypart* bodypart, BodypartInformation* information) {
    if(information) {
      return true;
    }
    else {
      return false;
    };
  };

  bool NELOrganismPrinter::bodypartRemoved(Bodypart* bodypart, BodypartInformation* information) {
    UInstance entity = dynamic_cast<NELBodypartInformation*>(information)->entity;
    scene->deleteInstance(entity);
    dynamic_cast<NELBodypartInformation*>(information)->entity = 0;
    return true;
  };
};



/*
			// increase the angle
			angle += 0.1f;

			if (angle >= NLMISC::Pi*2) angle = 0.f;

			// rotate all entities
			for(size_t i = 0; i < Entities.size(); ++i)
			{
				Entities[i].setRotEuler(0.f, angle, 0.f);
			}*/