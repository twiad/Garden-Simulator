// geneticSystem.cpp
// by Franz Köhler 2009

#include "geneticSystemCore.h"

namespace EDen {
  GeneticConditionType GeneticCondition::getConditionType() {
    return condType; 
  };

  bool GeneticCondition::dependsOnConditionType(GeneticConditionType param_conditionType) {
    if(param_conditionType == condType) return true;
    else return false;
  };

  GeneticCompoundCondition::~GeneticCompoundCondition() {
    GeneticCondition* cond;
    while(!childConditions.empty()) {
      cond = childConditions.back();
      childConditions.pop_back();
      delete cond;
    }
    // GeneticCondition::~GeneticCondition();
  };

  bool GeneticCompoundCondition::add(GeneticCondition* newCondition) {
    childConditions.push_back(newCondition);
    return true;
  };

  GeneticCompoundAction::~GeneticCompoundAction() {
    GeneticAction* act;
    while(!childActions.empty()) {
      act = childActions.back();
      childActions.pop_back();
      delete act;
    }
    // GeneticAction::~GeneticAction(); 
  };

  bool GeneticCompoundAction::add(GeneticAction* newAction) {
    childActions.push_back(newAction);
    return true;
  };

  bool GeneticCompoundAction::execute() {
    bool result = true;
    
    for(GeneticActionsListIterator it = childActions.begin(); it != childActions.end(); it++) {
      result = result && (*it)->execute();
    };

    return result;
  };

  GeneticANDCondition::~GeneticANDCondition() {
    // GeneticCompoundCondition::~GeneticCompoundCondition(); 
  };

  bool GeneticANDCondition::fullfilled() {
    bool result = true;
    bool notEmpty = false;
    
    for(GeneticConditionsListIterator it = childConditions.begin(); it != childConditions.end(); it++) {
      result = result && (*it)->fullfilled();
      notEmpty = true;
    };
    return result && notEmpty;
  };

  bool GeneticANDCondition::dependsOnConditionType(GeneticConditionType param_conditionType) {
    bool result = false;
    
    for(GeneticConditionsListIterator it = childConditions.begin(); it != childConditions.end(); it++) {
      if ((*it)->dependsOnConditionType(param_conditionType)) {
        result = true;
        break; 
      };
    };

    return result;
  };

  GeneticORCondition::~GeneticORCondition() {

  };

  bool GeneticORCondition::fullfilled() {
    bool result = false;
    
    for(GeneticConditionsListIterator it = childConditions.begin(); it != childConditions.end(); it++) {
      result = result || (*it)->fullfilled();
    };
    return result;
  };

  bool GeneticORCondition::dependsOnConditionType(GeneticConditionType param_conditionType) {
    bool result = true;
    bool notEmpty = false;
    
    for(GeneticConditionsListIterator it = childConditions.begin(); it != childConditions.end(); it++) {
      notEmpty = true;
      if (!((*it)->dependsOnConditionType(param_conditionType))) {
        result = false;
        break; 
      };
    };

    return result && notEmpty;
  };

  GeneticActionType GeneticAction::getActionType() {
    return actType;  
  };

  GeneticClause::~GeneticClause() {
    delete cond;
    delete act;
  };

  bool GeneticClause::run() {
    if(cond->fullfilled()) {
      act->execute();
      if(cond->dependsOnConditionType(GCT_BodypartCreation)) {
        deleteMe = true;
      };
      return true; 
    }
    else {
      return false;
    };
  };
};