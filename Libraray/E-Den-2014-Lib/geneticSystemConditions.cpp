// geneticSystemConditions.cpp
// by Franz Koehler 2009

#include "globals.h"
#include "geneticSystemConditions.h"

namespace EDen {
  GeneticChemicalCondition::GeneticChemicalCondition(
                             GeneticChemicalConditionType ntype, 
                             float ntriggerValue, 
                             std::string nchemName,
                             Bodypart * param_bp): GeneticCondition(GCT_Chemical),
                                                     chemCondType(ntype),
                                                     triggerValue(ntriggerValue),
                                                     chemName(nchemName) {
    if(param_bp) storage = param_bp->getChemicalStorage();
    else storage = 0;
  };

  GeneticChemicalCondition::GeneticChemicalCondition(TiXmlElement* desc, Bodypart* param_bp): GeneticCondition(GCT_Chemical) {
    chemName = desc->Attribute("ChemicalName");
    desc->QueryIntAttribute("Type",(int*)&chemCondType);
    desc->QueryFloatAttribute("TriggerValue",&triggerValue);
    
    setBodypart(param_bp);
  };
  
  GeneticChemicalCondition::~GeneticChemicalCondition() {
    // GeneticCondition::~GeneticCondition();
  };

  GeneticCondition* GeneticChemicalCondition::copy() {
    return new GeneticChemicalCondition(chemCondType,triggerValue,chemName); 
  };

  TiXmlElement* GeneticChemicalCondition::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("ChemicalCondition");

    element->SetAttribute("ChemicalName",chemName);
    element->SetDoubleAttribute("TriggerValue",triggerValue);
    element->SetAttribute("Type",(int)chemCondType);

    return element;
  };

  bool GeneticChemicalCondition::setBodypart(Bodypart* param_bodypart) {
    if(param_bodypart) storage = param_bodypart->getChemicalStorage();
    return true;
  };
  
  bool GeneticChemicalCondition::fullfilled() {
    switch(chemCondType) {
      case GCC_current_value_more:
        if(storage->getCurrentAmount(chemName) > triggerValue)
          return true;
        else return false;
        break;
      case GCC_current_value_less:
        if(storage->getCurrentAmount(chemName) < triggerValue)
          return true;
        else return false;
        break;
      case GCC_current_value_equal:
        if(storage->getCurrentAmount(chemName) == triggerValue)
          return true;
        else return false;
        break;
      case GCC_space_left_more:
        if(storage->getSpaceLeft(chemName) > triggerValue)
          return true;
        else return false;
        break;
      case GCC_space_left_less:
        if(storage->getSpaceLeft(chemName) < triggerValue)
          return true;
        else return false;
        break;
      case GCC_space_left_equal:
        if(storage->getSpaceLeft(chemName) == triggerValue)
          return true;
        else return false;
        break;
      case GCC_percentage_more:
        if(storage->getCurrentPercentage(chemName) > triggerValue)
          return true;
        else return false;
        break;
      case GCC_percentage_less:
        if(storage->getCurrentPercentage(chemName) < triggerValue)
          return true;
        else return false;
        break;
      case GCC_percentage_equal:
        if(storage->getCurrentPercentage(chemName) == triggerValue)
          return true;
        else return false;
        break;
      case GCC_unknown:
        return false;
        break;
      default: return false;
    };
  };

  GeneticBodypartTypeCondition::GeneticBodypartTypeCondition(TiXmlElement* desc, Bodypart* param_bp): GeneticCondition(GCT_BodypartType) {
    desc->QueryIntAttribute("BodypartType",(int*)&bpType);
    desc->QueryIntAttribute("ConditionType",(int*)&condType);
    
    setBodypart(param_bp);
  };

  GeneticBodypartTypeCondition::~GeneticBodypartTypeCondition() {
    // GeneticCondition::~GeneticCondition();
  };

  GeneticCondition* GeneticBodypartTypeCondition::copy() {
    return new GeneticBodypartTypeCondition(bpType,condType);
  };

  TiXmlElement* GeneticBodypartTypeCondition::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("BodypartTypeCondition");

    element->SetAttribute("BodypartType",(int)bpType);
    element->SetAttribute("ConditionType",(int)condType);

    return element;
  };

  bool GeneticBodypartTypeCondition::setBodypart(Bodypart* p_bp) {
    bp = p_bp;
    return true;
  };

  bool GeneticBodypartTypeCondition::fullfilled() {
    switch(condType) {
      case GBT_equal:
        if(bp->getBodypartType() == bpType) return true;
        else return false;
        break;
      case GBT_notEqual:
        if(bp->getBodypartType() != bpType) return true;
        else return false;
        break;
      case GBT_UNKNOWN:
        return false;
        break;
      default: return false;
    };
  };

  GeneticBodypartStateCondition::GeneticBodypartStateCondition(TiXmlElement* desc, Bodypart* param_bp): GeneticCondition(GCT_BodypartState) {
    desc->QueryIntAttribute("State",(int*)&bpState);
    desc->QueryIntAttribute("Type",(int*)&condType);
    
    setBodypart(param_bp);
  };

  GeneticBodypartStateCondition::~GeneticBodypartStateCondition() {
    // GeneticCondition::~GeneticCondition();
  };

  GeneticCondition* GeneticBodypartStateCondition::copy() {
    return new GeneticBodypartStateCondition(bpState,condType);
  };

  TiXmlElement* GeneticBodypartStateCondition::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("BodypartStateCondition");

    element->SetAttribute("State",(int)bpState);
    element->SetAttribute("Type",(int)condType);

    return element;
  };

  bool GeneticBodypartStateCondition::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart;
    return true;
  };

  bool GeneticBodypartStateCondition::fullfilled() {
    switch(condType) {
      case GBT_equal:
        if(bp->getBodypartState() == bpState) return true;
        else return false;
        break;
      case GBT_notEqual:
        if(bp->getBodypartState() != bpState) return true;
        else return false;
        break;
      case GBT_UNKNOWN:
        return false;
        break;
      default: return false;
    };
  };

  GeneticHealthpointsCondition::GeneticHealthpointsCondition(GeneticStdConditionType param_condType, float param_amount, Bodypart* param_bodypart):
    GeneticCondition(GCT_BodypartHP) {
    setBodypart(param_bodypart);
    condType = param_condType;
    amount = param_amount;
  };

  GeneticHealthpointsCondition::GeneticHealthpointsCondition(TiXmlElement* desc, Bodypart* param_bp): GeneticCondition(GCT_BodypartHP) {
    desc->QueryFloatAttribute("Amount",&amount);
    desc->QueryIntAttribute("Type",(int*)&condType);
    
    setBodypart(param_bp);
  };

  GeneticHealthpointsCondition::~GeneticHealthpointsCondition() {
    
  };

  GeneticCondition* GeneticHealthpointsCondition::copy() {
    return new GeneticHealthpointsCondition(condType,amount);
  };

  TiXmlElement* GeneticHealthpointsCondition::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("HealthpointsCondition");

    element->SetDoubleAttribute("Amount",amount);
    element->SetAttribute("Type",(int)condType);

    return element;
  };

  bool GeneticHealthpointsCondition::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart; 
    return true;
  };

  bool GeneticHealthpointsCondition::fullfilled() {
    switch(condType) {
      case GBT_equal:
        if(bp->getHealthpoints() == amount) return true;
        else return false;
        break;
      case GBT_less:
        if(bp->getHealthpoints() < amount) return true;
        else return false;
        break;
      case GBT_more:
        if(bp->getHealthpoints() > amount) return true;
        else return false;
        break;
      case GBT_notEqual:
        if(bp->getHealthpoints() != amount) return true;
        else return false;
        break;
	  //case GBT_percentage_more:
   //     if(bp->getHealthpoints() / bp->getMaxHealpoints() * 100.0f > amount) return true;
   //     else return false;
   //     break;
	  //case GBT_percentage_less:
   //     if(bp->getHealthpoints() / bp->getMaxHealpoints() * 100.0f < amount) return true;
   //     else return false;
   //     break;
      case GBT_UNKNOWN:
        return false;
        break;
      default:
        return false;
    };
  };

  GeneticParentHealthpointsCondition::GeneticParentHealthpointsCondition(GeneticStdConditionType param_condType, float param_amount, Bodypart* param_bodypart):
    GeneticCondition(GCT_BodypartHP) {
    setBodypart(param_bodypart);
    condType = param_condType;
    amount = param_amount;
  };

  GeneticParentHealthpointsCondition::GeneticParentHealthpointsCondition(TiXmlElement* desc, Bodypart* param_bp): GeneticCondition(GCT_BodypartHP) {
    desc->QueryFloatAttribute("Amount",&amount);
    desc->QueryIntAttribute("Type",(int*)&condType);
    
    setBodypart(param_bp);
  };

  GeneticParentHealthpointsCondition::~GeneticParentHealthpointsCondition() {
      
  };

  GeneticCondition* GeneticParentHealthpointsCondition::copy() {
    return new GeneticParentHealthpointsCondition(condType,amount);
  };

  TiXmlElement* GeneticParentHealthpointsCondition::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("ParentHealthpointsCondition");

    element->SetDoubleAttribute("Amount",amount);
    element->SetAttribute("Type",(int)condType);

    return element;
  };

  bool GeneticParentHealthpointsCondition::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart; 
    return true;
  };

  bool GeneticParentHealthpointsCondition::fullfilled() {
    if(bp->getParentBodypart()) {
      switch(condType) {
        case GBT_equal:
          if(bp->getParentBodypart()->getHealthpoints() == amount) return true;
          else return false;
          break;
        case GBT_less:
          if(bp->getParentBodypart()->getHealthpoints() < amount) return true;
          else return false;
          break;
        case GBT_more:
          if(bp->getParentBodypart()->getHealthpoints() > amount) return true;
          else return false;
          break;
        case GBT_notEqual:
          if(bp->getParentBodypart()->getHealthpoints() != amount) return true;
          else return false;
          break;
		//case GBT_percentage_more:
  //        if(bp->getParentBodypart()->getHealthpoints() / bp->getParentBodypart()->getMaxHealthpoints() * 100.0f  > amount) return true;
  //        else return false;
  //        break;
		//case GBT_percentage_less:
  //        if(bp->getParentBodypart()->getHealthpoints() / bp->getParentBodypart()->getMaxHealthpoints() * 100.0f < amount) return true;
  //        else return false;
  //        break;
        case GBT_UNKNOWN:
          return false;
          break;
        default:
          return false;
      };
    }
    else return false;
  };

  GeneticParentOrganismLifetimeCondition::GeneticParentOrganismLifetimeCondition(GeneticStdConditionType param_condType, int param_amount, Bodypart* param_bodypart): GeneticCondition(GCT_OrganismLifetime) {
    condType = param_condType;
    bp = param_bodypart;
    amount = param_amount;
  };

  GeneticParentOrganismLifetimeCondition::GeneticParentOrganismLifetimeCondition(TiXmlElement* desc, Bodypart* param_bp): GeneticCondition(GCT_OrganismLifetime) {
    desc->QueryIntAttribute("Amount",&amount);
    desc->QueryIntAttribute("Type",(int*)&condType);
    
    setBodypart(param_bp);
  };

  GeneticParentOrganismLifetimeCondition::~GeneticParentOrganismLifetimeCondition() {};

  bool GeneticParentOrganismLifetimeCondition::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart; 
    return true;
  };

  GeneticCondition* GeneticParentOrganismLifetimeCondition::copy() {
    return new GeneticParentOrganismLifetimeCondition(condType, amount);
  };

  TiXmlElement* GeneticParentOrganismLifetimeCondition::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("OrganismLifetimeCondition");

    element->SetAttribute("Amount",amount);
    element->SetAttribute("Type",(int)condType);

    return element;
  };

  bool GeneticParentOrganismLifetimeCondition::fullfilled() {
    if(bp->getParentOrganism()) {
      switch(condType) {
        case GBT_equal:
          if(bp->getParentOrganism()->getLifetime() == amount) return true;
          else return false;
          break;
        case GBT_less:
          if(bp->getParentOrganism()->getLifetime() < amount) return true;
          else return false;
          break;
        case GBT_more:
          if(bp->getParentOrganism()->getLifetime() > amount) return true;
          else return false;
          break;
        case GBT_notEqual:
          if(bp->getParentOrganism()->getLifetime() != amount) return true;
          else return false;
          break;
        case GBT_UNKNOWN:
          return false;
          break;
        default:
          return false;
      };
    } else return false;
  };


  GeneticBodypartSizeCondition::GeneticBodypartSizeCondition(GeneticStdConditionType param_condType, float param_amount, Bodypart* param_bodypart):
    GeneticCondition(GCT_BodypartSize) {
    setBodypart(param_bodypart);
    condType = param_condType;
    amount = param_amount;
  };

  GeneticBodypartSizeCondition::GeneticBodypartSizeCondition(TiXmlElement* desc, Bodypart* param_bp): GeneticCondition(GCT_BodypartSize) {
    desc->QueryFloatAttribute("Amount",&amount);
    desc->QueryIntAttribute("Type",(int*)&condType);
    
    setBodypart(param_bp);
  };

  GeneticBodypartSizeCondition::~GeneticBodypartSizeCondition() {
      
  };

  GeneticCondition* GeneticBodypartSizeCondition::copy() {
    return new GeneticBodypartSizeCondition(condType,amount);
  };

  TiXmlElement* GeneticBodypartSizeCondition::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("SizeCondition");

    element->SetDoubleAttribute("Amount",amount);
    element->SetAttribute("Type",(int)condType);

    return element;
  };

  bool GeneticBodypartSizeCondition::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart; 
    return true;
  };

  bool GeneticBodypartSizeCondition::fullfilled() {
    switch(condType) {
      case GBT_equal:
        if(bp->getSize() == amount) return true;
        else return false;
        break;
      case GBT_less:
        if(bp->getSize() < amount) return true;
        else return false;
        break;
      case GBT_more:
        if(bp->getSize() > amount) return true;
        else return false;
        break;
      case GBT_notEqual:
        if(bp->getSize() != amount) return true;
        else return false;
        break;
	  case GBT_percentage_more:
		if((bp->getSize() / bp->getMaxSize() * 100.0f) > amount) return true;
		else return false;
		break;
	  case GBT_percentage_less:
		if((bp->getSize() / bp->getMaxSize() * 100.0f) < amount) return true;
		else return false;
		break;
      case GBT_UNKNOWN:
        return false;
        break;
      default:
        return false;
    };
  };

  GeneticBodypartCreationCondition::~GeneticBodypartCreationCondition() {
    // GeneticCondition::~GeneticCondition(); 
  };

  GeneticBodypartCreationCondition::GeneticBodypartCreationCondition(TiXmlElement* desc, Bodypart* param_bp): GeneticCondition(GCT_BodypartCreation) {
    setBodypart(param_bp);
  };

  GeneticCondition* GeneticBodypartCreationCondition::copy() {
    return new GeneticBodypartCreationCondition();
  };

  TiXmlElement* GeneticBodypartCreationCondition::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("CreationCondition");
    return element;
  };

  bool GeneticBodypartCreationCondition::fullfilled() {
    if(bp->getBodypartState() == BSP_creation) return true;
    else return false;
  };

  bool GeneticBodypartCreationCondition::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart; 
    return true;
  };

  GeneticSpawnpointPresentCondition::GeneticSpawnpointPresentCondition(TiXmlElement* desc, Bodypart* param_bp): GeneticCondition(GCT_SpawnpointPresent) {
    desc->QueryIntAttribute("Type",(int*)&bpType);
    
    setBodypart(param_bp);
  };

  GeneticSpawnpointPresentCondition::~GeneticSpawnpointPresentCondition() {
    // GeneticCondition::~GeneticCondition();       
  };

  GeneticCondition* GeneticSpawnpointPresentCondition::copy() {
    return new GeneticSpawnpointPresentCondition(bpType);
  };

  TiXmlElement* GeneticSpawnpointPresentCondition::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("SpawnpointPresentCondition");

    element->SetAttribute("Type",(int)bpType);

    return element;
  };

  bool GeneticSpawnpointPresentCondition::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart; 
    return true;
  };

  bool GeneticSpawnpointPresentCondition::fullfilled() {
    if(bp->spawnPointAvailable(bpType)) return true;
    else return false;
  };

  GeneticCanGrowCondition::GeneticCanGrowCondition(Bodypart* param_bp): GeneticCondition(GCT_BodypartSize) {
    setBodypart(param_bp);
  };

  GeneticCanGrowCondition::GeneticCanGrowCondition(TiXmlElement* desc, Bodypart* param_bp): GeneticCondition(GCT_BodypartSize) {
    setBodypart(param_bp);
  };

  GeneticCanGrowCondition::~GeneticCanGrowCondition() {

  };

  GeneticCondition* GeneticCanGrowCondition::copy() {
    return new GeneticCanGrowCondition();
  };

  TiXmlElement* GeneticCanGrowCondition::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("CanGrowCondition");

    return element;
  };

  bool GeneticCanGrowCondition::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart; 
    return true;
  };

  bool GeneticCanGrowCondition::fullfilled() {
    if(bp->getSize() < bp->getMaxSize()) return true;
    else return false;
  };

  GeneticHasParentCondition::GeneticHasParentCondition(Bodypart* param_bodypart) : GeneticCondition(GCT_HasParent) {
    bp = param_bodypart;
  };

  GeneticHasParentCondition::GeneticHasParentCondition(TiXmlElement* desc, Bodypart* param_bp): GeneticCondition(GCT_HasParent) {
    setBodypart(param_bp);
  };

  GeneticHasParentCondition::~GeneticHasParentCondition() {
    
  };

  bool GeneticHasParentCondition::fullfilled() {
    if(bp->getParentBodypart()) {
		return true;
	}
    else return false;
  };
  
  bool GeneticHasParentCondition::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart;
    return true;
  };
  
  GeneticCondition* GeneticHasParentCondition::copy() {
    return new GeneticHasParentCondition();  
  };

  TiXmlElement* GeneticHasParentCondition::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("HasParentCondition");

    return element;
  };
} // namespace