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
        req_print(org->getRootBodypart(),0.0f,((100.0f/max)*counter) - 50.0f,0.0f,0.0f,0.0f,0.0f);
      };
      counter++;
    };

    return true;
  };

  bool NELOrganismPrinter::req_print(Bodypart* bp, float offset_x, float offset_y, float offset_z, float rot1, float rot2, float rot3) {
    if(bp == 0) 
      return false;
    if(bp->getBodypartState() == BSP_dead) 
      return false;

    NELBodypartInformation* info = dynamic_cast<NELBodypartInformation*>(getInformation(bp));
    if(info == 0) 
      return false;

    UInstance entity = info->entity;
    
    if(sun)
      sun->setFactor(bp,offset_z * 10.0f);

    entity.setPos(offset_x,offset_y,offset_z);
    entity.setRotEuler(rot1/360.0f*(NLMISC::Pi*2),rot2/360.0f*(NLMISC::Pi*2),rot3/360.0f*(NLMISC::Pi*2));
    
    switch(bp->getBodypartType())  {
      case BPT_Stick:
        entity.setScale(0.2f,0.2f,bp->getSize() * 0.2f);
        break;
      case BPT_Leaf:
        entity.setScale(bp->getSize() * 0.1f, 0.2f, bp->getSize() * 0.2f);
        break;
      case BPT_Branch:
        entity.setScale(0.3f,0.3f,bp->getSize() * 0.2f);
        break;
      case BPT_Seed:
        entity.setScale(1.0f,1.0f,bp->getSize()*0.1f);
        break;
      default:
        entity.setScale(0.2f,0.2f,bp->getSize() * 0.5f);
    };
    
    entity.getMaterial(0).setColor(CRGBA(bp->color.r * 255,bp->color.g * 255,bp->color.b * 255,bp->color.a * 255));
    entity.getMaterial(0).setDiffuse(CRGBA(bp->color.r * 255,bp->color.g * 255,bp->color.b * 255,bp->color.a * 255));

    CMatrix mat = entity.getMatrix();

    CVector myVector = mat.mulVector(CVector(0.0f,0.0f,1.0f));

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
        float resRot1 = rot1 + 180.0f + partner_ang1 + (*it)->ang2d;
        float resRot2 = rot2 + partner_ang2 + (*it)->ang2;
        float resRot3 = rot3 + partner_ang3 + (*it)->rot;
        req_print((*it)->connectedBodypart,offset_x,offset_y,offset_z,resRot1,resRot2,resRot3);
      };
    };
    return true;
  };

  UInstance NELOrganismPrinter::createEntity(Bodypart* bp) {
    UInstance Entity = scene->createInstance(pathToCylinderShape);

    static float debuggy = 0.01f;
    static float debuggz = 0.01f;

    if (!Entity.empty()) {
		  Entity.setTransformMode(UTransformable::RotEuler);
      Entity.setScale(0.1f,0.1f,0.3f);
      Entity.setPos(-5.0f,-50.0f + debuggy,80.0f - debuggz);
      debuggy += 0.2f; 
      if(debuggy > 100.0f) { 
        debuggy = 0.01f; debuggz += 0.4f;
        if(debuggz > 80.0f) debuggz = 0.01f;
      };
    }
    else {
      scene->deleteInstance(Entity);
      Entity = 0;
    };
    return Entity;
  };

  BodypartInformation* NELOrganismPrinter::updateInformation(Bodypart* bodypart, BodypartInformation* information) {
    if(information) {
      return information; // could check for valid entity here
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
      return true;//return false;
    };
  };

  bool NELOrganismPrinter::bodypartRemoved(Bodypart* bodypart, BodypartInformation* information) {
    if(information) {
      UInstance entity = dynamic_cast<NELBodypartInformation*>(information)->entity;
      scene->deleteInstance(entity);
      dynamic_cast<NELBodypartInformation*>(information)->entity = 0;
    }
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

  bool NEL_SunlightProvider::setFactor(Bodypart* param_bodypart ,float param_factor) {
    ExtendedBodypartInformation* info = dynamic_cast<ExtendedBodypartInformation*>(getInformation(param_bodypart));
    if(info)
      info->factor = param_factor;

    else {
      info = new ExtendedBodypartInformation();
      info->factor = param_factor;
      updateBodypartInformation(param_bodypart,info);
    };

    return true;
  };

};