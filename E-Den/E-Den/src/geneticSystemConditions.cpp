// geneticSystemConditions.cpp
// by Franz Koehler 2009

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
  
  GeneticChemicalCondition::~GeneticChemicalCondition() {
    // GeneticCondition::~GeneticCondition();
  };

  GeneticCondition* GeneticChemicalCondition::copy() {
    return new GeneticChemicalCondition(chemCondType,triggerValue,chemName); 
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

  GeneticBodypartTypeCondition::~GeneticBodypartTypeCondition() {
    // GeneticCondition::~GeneticCondition();
  };

  GeneticCondition* GeneticBodypartTypeCondition::copy() {
    return new GeneticBodypartTypeCondition(bpType,condType);
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

  GeneticBodypartStateCondition::~GeneticBodypartStateCondition() {
    // GeneticCondition::~GeneticCondition();
  };

  GeneticCondition* GeneticBodypartStateCondition::copy() {
    return new GeneticBodypartStateCondition(bpState,condType);
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

  GeneticHealthpointsCondition::~GeneticHealthpointsCondition() {
    
  };

  GeneticCondition* GeneticHealthpointsCondition::copy() {
    return new GeneticHealthpointsCondition(condType,amount);
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

  GeneticParentHealthpointsCondition::~GeneticParentHealthpointsCondition() {
      
  };

  GeneticCondition* GeneticParentHealthpointsCondition::copy() {
    return new GeneticParentHealthpointsCondition(condType,amount);
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
        case GBT_UNKNOWN:
          return false;
          break;
        default:
          return false;
      };
    }
    else return false;
  };

  GeneticBodypartSizeCondition::GeneticBodypartSizeCondition(GeneticStdConditionType param_condType, float param_amount, Bodypart* param_bodypart):
    GeneticCondition(GCT_BodypartSize) {
    setBodypart(param_bodypart);
    condType = param_condType;
    amount = param_amount;
  };

  GeneticBodypartSizeCondition::~GeneticBodypartSizeCondition() {
      
  };

  GeneticCondition* GeneticBodypartSizeCondition::copy() {
    return new GeneticBodypartSizeCondition(condType,amount);
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

  GeneticCondition* GeneticBodypartCreationCondition::copy() {
    return new GeneticBodypartCreationCondition();
  };

  bool GeneticBodypartCreationCondition::fullfilled() {
    if(bp->getBodypartState() == BSP_creation) return true;
    else return false;
  };

  bool GeneticBodypartCreationCondition::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart; 
    return true;
  };

  GeneticSpawnpointPresentCondition::~GeneticSpawnpointPresentCondition() {
    // GeneticCondition::~GeneticCondition();       
  };

  GeneticCondition* GeneticSpawnpointPresentCondition::copy() {
    return new GeneticSpawnpointPresentCondition(bpType);
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

  GeneticCanGrowCondition::~GeneticCanGrowCondition() {

  };

  GeneticCondition* GeneticCanGrowCondition::copy() {
    return new GeneticCanGrowCondition();
  };

  bool GeneticCanGrowCondition::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart; 
    return true;
  };

  bool GeneticCanGrowCondition::fullfilled() {
    if(bp->getSize() < bp->getMaxSize()) return true;
    else return false;
  };
} // namespace