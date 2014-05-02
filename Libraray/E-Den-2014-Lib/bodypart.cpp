// bodypart.cpp
// by Franz Koehler 2009

#include "globals.h"
#include "bodypart.h"

namespace EDen {
  unsigned int Bodypart::bodypartCount = 0;

  SpawnpointInformation::~SpawnpointInformation() {
    supportedBpTypes.clear();
  };

  SpawnpointInformation* SpawnpointInformation::copy() {
    SpawnpointInformation* spnew = new SpawnpointInformation();
    spnew->ang2d = ang2d;
    spnew->ang2 = ang2;
    spnew->rot = rot;
    spnew->scaleModifier = scaleModifier;
    spnew->position = position;
    spnew->supportedBpTypes = supportedBpTypes;

    return spnew;
  };

  Bodypart::Bodypart(BodypartType bodypartType, std::string dummyGenecodeIdentifier, Organism* param_parentOrganism, Bodypart* param_parentBodypart) {
    healthpoints = 100.0f;
    size = 1.0f;
    maxSize = 50.0f;
    scaleModifier = 1.0f;
    
    bpType = bodypartType;
    geneCode = new GeneticCode(dummyGenecodeIdentifier);
    bpState = BSP_creation;
    
    chemStorage = new ChemicalStorage();
    genProcessor = new GeneticProcessor(this);
    
    parentOrganism = param_parentOrganism;
    parentBodypart = param_parentBodypart;
    
    init();      
  };

  Bodypart::Bodypart(BodypartType bodypartType, GeneticCode* param_geneticCode, Organism* param_parentOrganism, Bodypart* param_parentBodypart) {
    healthpoints = 100.0f;
    size = 1.0f;
    maxSize = 50.0f;
    scaleModifier = 1.0f;
    
    bpType = bodypartType;
    if(param_geneticCode) geneCode = param_geneticCode;
    else geneCode = new GeneticCode();
    geneCode->setBodypart(this);
    bpState = BSP_creation;
    
    chemStorage = new ChemicalStorage();
    genProcessor = new GeneticProcessor(this);
    
    parentOrganism = param_parentOrganism;
    parentBodypart = param_parentBodypart;
    
    init();      
  };

  Bodypart::Bodypart(TiXmlElement* description, Organism* param_parentOrganism, Bodypart* param_parentBodypart) {
    TiXmlElement *chemStorageDescription,*geneCodeDescription,*childBodypartDescription,*childsIterator;
    TiXmlElement *spawnpointsDescription,*spawnpointIterator;

    parentOrganism = param_parentOrganism;
    parentBodypart = param_parentBodypart;
    

    description->QueryIntAttribute("Type",(int*)&bpType);
    description->QueryIntAttribute("State",(int*)&bpState);
    description->QueryFloatAttribute("Healthpoints",&healthpoints);
    description->QueryFloatAttribute("Size",&size);
    description->QueryFloatAttribute("ScaleModifier",&scaleModifier);

    chemStorageDescription = description->FirstChildElement("Storage");
    chemStorage = new ChemicalStorage(chemStorageDescription);

    geneCodeDescription = description->FirstChildElement("GeneticCode");
    geneCode = new GeneticCode(geneCodeDescription);

    spawnpointsDescription = description->FirstChildElement("Spawnpoints");
    spawnpointIterator = spawnpointsDescription->FirstChildElement("Spawnpoint");
    while(spawnpointIterator != 0) {
      SpawnpointInformation* sp = new SpawnpointInformation();
      xmlElementToSpawnpoint(spawnpointIterator,sp);
      addSpawnpoint(sp);
      spawnpointIterator = spawnpointIterator->NextSiblingElement();
    };

    childBodypartDescription = description->FirstChildElement("Childs");
    childsIterator = childBodypartDescription->FirstChildElement("Bodypart");
    while(childsIterator != 0) {
      spawnBodypart(new Bodypart(childsIterator,parentOrganism,this));
      childsIterator = childsIterator->NextSiblingElement();
    };

    genProcessor = new GeneticProcessor(this);

    init();
  };
  
  Bodypart::~Bodypart() {
    if(parentOrganism != 0) parentOrganism->unregisterBodypart(this);
    destroy();
    delete genProcessor;
    genProcessor = 0;
    delete chemStorage;
    chemStorage = 0;
    childBodyparts.clear();
    spawnpoints.clear();
    delete geneCode;
    //bodypartCount--;
  };

  bool Bodypart::init() {
    geneCode->setBodypart(this);
    genProcessor->updateGeneticCode();
    genProcessor->executeRelevantClauses();
    bpState = BSP_alive;
    genProcessor->updateGeneticCode();
    //bodypartCount++;
    return true;
  };

  bool Bodypart::unregisterChildBodypart(Bodypart* param_childBodypart) {
    bool foundOne = false;
    
    // remove bp from childs list
    for(BodypartListIterator it = childBodyparts.begin(); it != childBodyparts.end(); it++) {
      if((*it) == param_childBodypart) {
        childBodyparts.erase(it);
        foundOne = true;
        break;
      };
    };

    // free own spawnpoint
    for(SpawnpointInformationListIterator it = spawnpoints.begin(); it != spawnpoints.end(); it++) {
      if((*it)->connectedBodypart == param_childBodypart) {
        (*it)->connectedBodypart = 0;
        (*it)->occupied = false;
      };
    };

    // free childBPs spawnpoint
    for(SpawnpointInformationListIterator it = param_childBodypart->spawnpoints.begin(); it != param_childBodypart->spawnpoints.end(); it++) {
      if((*it)->connectedBodypart == this) {
        (*it)->connectedBodypart = 0;
        (*it)->occupied = false;
      };
    };

    // reset Childs ParentBodypart
    param_childBodypart->parentBodypart = 0;

    return foundOne;
  };

  GeneticCode* Bodypart::getGeneticCode() {
    return geneCode;
  };

  BodypartType Bodypart::getBodypartType() {
    return bpType; 
  };

  bool Bodypart::setBodypartState(BodypartState param_bpState) {
    bpState = param_bpState;
    return true;
  };

  BodypartState Bodypart::getBodypartState() {
    return bpState;
  };

  bool Bodypart::update() {
    genProcessor->executeRelevantClauses();
    return true;
  };

  bool Bodypart::destroy() {
    //Bodypart* lastBP;
    if(parentOrganism) {
      if(parentOrganism->getRuntimeManager()) {
        parentOrganism->getRuntimeManager()->unregisterBodypart(this);
      };
    };
    
    {
      Bodypart* bp;
      while(!childBodyparts.empty()) {
        bp = childBodyparts.back();
        bp->destroy();
        childBodyparts.pop_back();
        delete bp;
      };
    }
    
    {
      SpawnpointInformation* sp;
      while(!spawnpoints.empty()) {
        sp = spawnpoints.back();
        spawnpoints.pop_back();
        delete sp;
      };
    }

    return true;
  };

  bool Bodypart::occupieSpawnpoint(Bodypart* param_bodypart) {
    bool foundSomething = false;
    for(SpawnpointInformationListIterator it = spawnpoints.begin(); it != spawnpoints.end(); it++) {
      if(((*it)->isSupportedType(param_bodypart->getBodypartType())) && ((*it)->occupied == false)) {
        (*it)->occupied = true;
        (*it)->connectedBodypart = param_bodypart;
        foundSomething = true;
        break;
      }
    };
    return foundSomething;
  };

  bool Bodypart::addSpawnpoint(BodypartType param_bpType, int param_position) {
    SpawnpointInformation* sp = new SpawnpointInformation();
    sp->position = param_position;
    sp->addSupportedType(param_bpType);
    sp->occupied = false;
    spawnpoints.push_back(sp);
    return true;
  };

  bool Bodypart::addSpawnpoint(SpawnpointInformation* sp) {
    sp->occupied = false;
    spawnpoints.push_back(sp);
    return true;
  };

  bool Bodypart::resetSpawnpoints() {
    SpawnpointInformation* sp;
    while(!spawnpoints.empty()) {
      sp = spawnpoints.back();
      spawnpoints.pop_back();
      delete sp;
    };

    return true;
  };

  bool Bodypart::spawnBodypart(Bodypart* bp) { 
    if((!spawnPointAvailable(bp->getBodypartType())) || (!bp->spawnPointAvailable(bpType))) {
      return false;
    }
    else {
      if((!occupieSpawnpoint(bp)) || (!bp->occupieSpawnpoint(this))) {
        return false;
      }
      else {
        if(parentOrganism != 0) {
          parentOrganism->registerBodypart(bp);
          parentOrganism->addChemicalStorageLink(connectStorageToBodypart(bp));
        };
        childBodyparts.push_back(bp);
        bp->parentBodypart = this;
        bp->setScaleModifier(getSpawnpointInformationForBodypart(bp)->scaleModifier);
        bp->init();
        return true;
      }
    };
  };

  bool Bodypart::spawnPointAvailable(BodypartType bpType) {
    bool foundSomething = false;
    
    for(SpawnpointInformationListIterator it = spawnpoints.begin(); it != spawnpoints.end(); it++) {
      if(((*it)->isSupportedType(bpType)) && ((*it)->occupied == false)) {
        foundSomething = true;
        break;
      }
    }
    
    return foundSomething; 
  };

  ChemicalStorage* Bodypart::getChemicalStorage() {
    return chemStorage;  
  };
  
  GeneticProcessor* Bodypart::getGeneticProcessor() {
    return genProcessor;
  };

  ChemicalStorageLink* Bodypart::connectStorageToBodypart(Bodypart* bp) {
    return new ChemicalStorageLink(chemStorage,bp->getChemicalStorage());
  };

  bool Bodypart::setParentOrganism(Organism* param_organism) {
    parentOrganism = param_organism;
    return true;
  };

  Organism* Bodypart::getParentOrganism() {
    return parentOrganism;
  };

  Bodypart* Bodypart::getParentBodypart() {
    return parentBodypart;
  };

  BodypartList Bodypart::getChildBodyparts() {
    return childBodyparts; 
  };

  SpawnpointInformationList* Bodypart::getSpawnpoints() {
    return &spawnpoints;
  };

  SpawnpointInformation* Bodypart::getSpawnpointInformationForBodypart(Bodypart* param_bp) {
    SpawnpointInformation* returnvalue;
    for(SpawnpointInformationListIterator it = spawnpoints.begin(); it != spawnpoints.end(); it++) {
      if((*it)->connectedBodypart == param_bp) {
        returnvalue = (*it);
        break;
      }
    };
    return returnvalue;
  };

  bool Bodypart::attachToOrganism(Organism* org) {
    parentOrganism = org;
    
    if(parentOrganism != 0) {
      for(BodypartListIterator it = childBodyparts.begin(); it != childBodyparts.end(); it++) {
        parentOrganism->addChemicalStorageLink(new ChemicalStorageLink(chemStorage,(*it)->getChemicalStorage()));
        (*it)->attachToOrganism(parentOrganism);
      };
      
      if(parentOrganism->getRootBodypart() != this) {
        parentOrganism->registerBodypart(this);
      };

    };

    return true;
  };

  bool Bodypart::detachFromOrganism() {
    if(parentOrganism != 0) {
      for(BodypartListIterator it = childBodyparts.begin(); it != childBodyparts.end(); it++) {
        (*it)->detachFromOrganism();
      };
      
      parentOrganism->unregisterBodypart(this);
      parentOrganism = 0;
    };

    return true;
  };

  Organism* Bodypart::detachToNewOrganism() {
    Organism* org;
    RuntimeManager* runtime;
	Groundpart* groundpart = 0;
	Bodypart* oldParentBodypart = 0;

    std::string name = "Parentless Organism";
	if(parentOrganism != 0) {
      name = parentOrganism->getName();
      runtime = parentOrganism->getRuntimeManager();
	  if(parentOrganism->connectedToGoundpart()) {
		  groundpart = parentOrganism->getGroundpart();
	  };
	};

    if(parentBodypart != 0) {
	  oldParentBodypart = parentBodypart;
      parentBodypart->unregisterChildBodypart(this);
      parentBodypart = 0;
    };
    
    if(parentOrganism != 0) {
      detachFromOrganism();
    };

    org = new Organism(name,this,runtime);
    attachToOrganism(org);

	bool addToRuntimedb = true;

	if(!(groundpart == 0)) {
	  addToRuntimedb = !(groundpart->dropOrganism(org, oldParentBodypart));
    };

	if(addToRuntimedb && (runtime)) {
	  runtime->add(org);
	}

    return org;
  };

  bool Bodypart::detachToNowhere() {
  RuntimeManager* runtime;

    std::string name = "Parentless Organism";
    if(parentBodypart != 0) {
      parentBodypart->unregisterChildBodypart(this);
      parentBodypart = 0;
    };
    
    if(parentOrganism != 0) {
      name = parentOrganism->getName();
      runtime = parentOrganism->getRuntimeManager();
	  destroy();
	  detachFromOrganism();
    };

    if(runtime) {
	  runtime->unregisterBodypart(this);
	  return runtime->addDelete(this);
    };

	return false;
  };

  float Bodypart::getHealthpoints() {
    return healthpoints;
  };

  bool Bodypart::hurt(float damage) {
    if(damage < 0) damage = 0.0f;
    healthpoints -= damage;
    if(healthpoints < 0) healthpoints = 0.0f;

    return true;
  };

  float Bodypart::getMaxSize() {
    return maxSize; 
  };
  
  bool Bodypart::setMaxSize(float param_maxSize) {
    if(param_maxSize != 0.0f)
      maxSize = param_maxSize * getScaleModifier();
    else
      maxSize = 0.0f;
    return true;
  };

  bool Bodypart::setScaleModifier(float param_scaleModifier) {
    maxSize = maxSize / getScaleModifier();
    scaleModifier = param_scaleModifier;
    maxSize = maxSize * getScaleModifier();
    return true;
  };

  float Bodypart::getScaleModifier() {
    if (parentBodypart == 0)
      return scaleModifier;
    else return scaleModifier * parentBodypart->getScaleModifier();
  };

  float Bodypart::getSize() {
    return size;
  };

  bool Bodypart::grow(float amount) {
    if(amount < 0) amount = 0;
    size = amount + size;
    if(size > maxSize) size = maxSize;
    chemStorage->setSize(size);
    return true;  
  };

  bool Bodypart::shrink(float amount) {
    if(amount < 0) amount = 0;
    else if (amount > 1.0) amount = 1.0;
    size = amount * size;
    if (size < 0) size = 0;
    chemStorage->setSize(size);
    return true;
  };

  bool Bodypart::heal(float health) {
    if(health < 0) health = 0.0f;
    healthpoints += health;
    if(healthpoints > 100) healthpoints = 100.0f;

    return true;
  };

  bool SpawnpointInformation::isSupportedType(BodypartType param_bpType) {
    bool foundOne = false;
    for(BodypartTypeListIterator it = supportedBpTypes.begin(); it != supportedBpTypes.end(); it++) {
      if((*it) == param_bpType) {
        foundOne = true;
        break;
      };
    };
    return foundOne;
  };

  bool SpawnpointInformation::addSupportedType(BodypartType param_bpType) {
    if(!(isSupportedType(param_bpType))) // check for redundancy?
        supportedBpTypes.push_back(param_bpType);
    
    return true;    
  };

  TiXmlElement* Bodypart::toXmlElement() {
    TiXmlElement* element,*childpartsElement,*spawnpointsElement;

    element = new TiXmlElement("Bodypart");
    element->SetAttribute("State",getBodypartState());
    element->SetAttribute("Type",getBodypartType());
    element->SetDoubleAttribute("Healthpoints",getHealthpoints());
    element->SetDoubleAttribute("Size",getSize());
    element->SetDoubleAttribute("ScaleModifier",getScaleModifier());
    element->SetAttribute("IID",(int)this);
    
    childpartsElement = new TiXmlElement("Childs");
    for(BodypartListIterator it = childBodyparts.begin(); it != childBodyparts.end(); it++) {
      childpartsElement->LinkEndChild((*it)->toXmlElement());
    };
    element->LinkEndChild(childpartsElement);

    spawnpointsElement = new TiXmlElement("Spawnpoints");
    for(SpawnpointInformationListIterator it = spawnpoints.begin(); it != spawnpoints.end(); it++) {
      TiXmlElement* element = new TiXmlElement("Spawnpoint");
      spawnpointToXmlElement((*it),element);
      spawnpointsElement->LinkEndChild(element);
    };
    element->LinkEndChild(spawnpointsElement);

    element->LinkEndChild(chemStorage->toXmlElement());
    element->LinkEndChild(geneCode->toXmlElement());

    return element;
  };

  void Bodypart::xmlElementToSpawnpoint(TiXmlElement* description, SpawnpointInformation* sp) {
    //description->QueryValueAttribute("Occupied",&(sp->occupied));
    sp->occupied = false;
    description->QueryFloatAttribute("Ang1",&(sp->ang2d));
    description->QueryFloatAttribute("Ang2",&(sp->ang2));
    description->QueryFloatAttribute("Rot",&(sp->rot));
    description->QueryFloatAttribute("ScaleModifier",&(sp->scaleModifier));

    //if(sp->scaleModifier == 0.0f)
    //  sp->scaleModifier = 1.0f;

    description->QueryIntAttribute("PositionId",&(sp->position));
    sp->connectedBodypart = 0;
    // TODO: connected bodypart via IID?!?
    
    TiXmlElement* it = description->FirstChildElement("SupportedTypes")->FirstChildElement("Type");
    int type;
    while(it != 0) {
      it->QueryIntAttribute("ID",&type);
      sp->addSupportedType((BodypartType)type);
      it = it->NextSiblingElement();
    };
  };

  void Bodypart::spawnpointToXmlElement(SpawnpointInformation* sp, TiXmlElement* element) {
    TiXmlElement* supportedTypesElement,*supportedTypeElement;

    element->SetAttribute("Occupied",sp->occupied);
    element->SetDoubleAttribute("Ang1",sp->ang2d);
    element->SetDoubleAttribute("Ang2",sp->ang2);
    element->SetDoubleAttribute("Rot",sp->rot);
    element->SetDoubleAttribute("ScaleModifier",sp->scaleModifier);
    element->SetAttribute("PositionId",sp->position);
    element->SetAttribute("ConnectedBodypartID",(int)(sp->connectedBodypart));
    
    supportedTypesElement = new TiXmlElement("SupportedTypes");
    BodypartTypeList supportedTypes = sp->supportedBpTypes;

    for(BodypartTypeListIterator it = supportedTypes.begin(); it != supportedTypes.end(); it++) {
      supportedTypeElement = new TiXmlElement("Type");
      supportedTypeElement->SetAttribute("ID",(int)(*it));
      supportedTypesElement->LinkEndChild(supportedTypeElement);
    };
    element->LinkEndChild(supportedTypesElement);
  };

  unsigned int Bodypart::getBodypartCount() {
    return bodypartCount;
  };
} // namespace