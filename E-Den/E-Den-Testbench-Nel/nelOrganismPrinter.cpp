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
  NELOrganismPrinter::NELOrganismPrinter(NL3D::UScene *Scene, RuntimeManager* p_runtime) {
    scene = Scene; 
    runtime = p_runtime;
    sun = new NEL_SunlightProvider();
    if(runtime) runtime->add(sun);


    CPath::addSearchPath(CFile::getPath(pathToCylinderShape), true, false);
  };

  NELOrganismPrinter::~NELOrganismPrinter() {
    if(runtime) runtime->remove(sun);
    delete sun;
  };

  bool NELOrganismPrinter::print() {
    if(runtime) organisms = runtime->getOrganisms();
    else cleanupDeadOrganisms();
    

    Organism* org;

    int counter = 1;
    int max = organisms.size() + 1;

    for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
      org = *it;
      if( org->getState() != BSP_dead) {
        req_print(org->getRootBodypart(),(0.5f/max)*counter,0.0f,0.0f,0.0f,0.0f,0.0f);
      };
      counter++;
    };

    return true;
  };

  bool NELOrganismPrinter::req_print(Bodypart* bp, float offset_x, float offset_y, float offset_z, float rot1, float rot2, float rot3) {
    NELBodypartInformation* info = dynamic_cast<NELBodypartInformation*>(getInformation(bp));
    UInstance entity = info->entity;
    
    if(!entity.empty()) {
      entity.setPos(offset_x,offset_y,offset_z);
      entity.setRotEuler(rot1,rot2,rot3);
    };

    CVector myVector;
    myVector.sphericToCartesian(rot1,rot2,rot3);
    myVector.normalize();
    myVector *= bp->getSize();

    offset_x += myVector.x;
    offset_y += myVector.y;
    offset_z += myVector.z;

    SpawnpointInformationList bpSpawnpoints = bp->getSpawnpoints();
    for(SpawnpointInformationListIterator it = bpSpawnpoints.begin(); it != bpSpawnpoints.end(); it++) {
      // position 0 is reserved for the backwards spawnpoint
      SpawnpointInformation* sp;
      if(((*it)->occupied) && ((*it)->position != 0)) { 
        sp = (*it)->connectedBodypart->getSpawnpointInformationForBodypart(bp);
        float partner_ang1 = sp->ang2d;
        float partner_ang2 = sp->ang2;
        float partner_ang3 = sp->rot;
        req_print((*it)->connectedBodypart,offset_x,offset_y,offset_z,rot1 + 180.0f + partner_ang1 + (*it)->ang2d,rot2 + partner_ang2 + (*it)->ang2,rot3 + partner_ang3 + (*it)->rot);
      };
    };
    return true;
  };

  UInstance NELOrganismPrinter::createEntity(Bodypart* bp) {
    UInstance Entity = scene->createInstance(pathToCylinderShape);

    if (!Entity.empty()) {
		  Entity.setTransformMode(UTransformable::RotEuler);
      Entity.setScale(0.2f * bp->getSize(),0.2f * bp->getSize(),1.f * bp->getSize());
    }
    else {
      scene->deleteInstance(Entity);
      Entity = 0;
    };
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

  bool NELOrganismPrinter::cleanupDeadOrganisms() {
    Organism* org;
    std::list<Organism*> new_orgs;

    while(!organisms.empty()) {
      org = organisms.back();
      organisms.pop_back();
      if(org)
          if(org->getState() != BSP_dead)
              new_orgs.push_front(org);
    };

    organisms.clear();
    organisms.swap(new_orgs);

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