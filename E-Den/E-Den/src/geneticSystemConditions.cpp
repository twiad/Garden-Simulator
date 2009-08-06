// geneticSystemConditions.cpp
// by Franz Koehler 2009

#include "geneticSystemConditions.h"

namespace EDen {
  GeneticChemicalCondition::~GeneticChemicalCondition() {
    // GeneticCondition::~GeneticCondition();
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

  GeneticHealthpointsCondition::GeneticHealthpointsCondition(Bodypart* param_bodypart, GeneticStdConditionType param_condType, float param_amount):
    GeneticCondition(GCT_BodypartHP) {
    bp = param_bodypart;
    condType = param_condType;
    amount = param_amount;
  };

  GeneticHealthpointsCondition::~GeneticHealthpointsCondition() {
    
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

  GeneticParentHealthpointsCondition::GeneticParentHealthpointsCondition(Bodypart* param_bodypart, GeneticStdConditionType param_condType, float param_amount):
    GeneticCondition(GCT_BodypartHP) {
    bp = param_bodypart;
    condType = param_condType;
    amount = param_amount;
  };

  GeneticParentHealthpointsCondition::~GeneticParentHealthpointsCondition() {
      
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

  GeneticBodypartSizeCondition::GeneticBodypartSizeCondition(Bodypart* param_bodypart, GeneticStdConditionType param_condType, float param_amount):
    GeneticCondition(GCT_BodypartSize) {
    bp = param_bodypart;
    condType = param_condType;
    amount = param_amount;
  };

  GeneticBodypartSizeCondition::~GeneticBodypartSizeCondition() {
      
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

  bool GeneticBodypartCreationCondition::fullfilled() {
    if(bp->getBodypartState() == BSP_creation) return true;
    else return false;
  };

  GeneticSpawnpointPresentCondition::~GeneticSpawnpointPresentCondition() {
    // GeneticCondition::~GeneticCondition();       
  };

  bool GeneticSpawnpointPresentCondition::fullfilled() {
    if(bp->spawnPointAvailable(bpType)) return true;
    else return false;
  };

  GeneticCanGrowCondition::GeneticCanGrowCondition(Bodypart* param_bp): GeneticCondition(GCT_BodypartSize) {
    bp = param_bp;
  };

  GeneticCanGrowCondition::~GeneticCanGrowCondition() {

  };

  bool GeneticCanGrowCondition::fullfilled() {
    if(bp->getSize() < bp->getMaxSize()) return true;
    else return false;
  };
} // namespace