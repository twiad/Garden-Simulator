// nelOrganismPrinter.cpp by Franz Koehler @ 2010

#include "nelOrganismPrinter.h"

namespace EDen {
  NELOrganismPrinter::NELOrganismPrinter(NL3D::UScene *Scene) {
    scene = Scene; 
  };

  NELOrganismPrinter::~NELOrganismPrinter() {
    
  };

  bool NELOrganismPrinter::print() {
    // use the path of the shape to find its textures
		CPath::addSearchPath(CFile::getPath(pathToCylinderShape), true, false);

    // add an entity to the scene
		UInstance Entity = Scene->createInstance(pathToCylinderShape);

		// if we can't create entity, skip it
    if (!Entity.empty()) {

		  // we will rotate it later so use Euler rotation transform mode
		  Entity.setTransformMode(UTransformable::RotEuler);

      Entity.setScale(0.2f,0.2f,1.f);

		  // add entity to the vector
			Entities.push_back(Entity);
    }

    Entity = Scene->createInstance(pathToPlaneShape);
    if (!Entity.empty()) {
		  Entity.setTransformMode(UTransformable::RotEuler);
			Entities.push_back(Entity);
    }

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