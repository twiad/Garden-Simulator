// geneticSystem.cpp
// by Franz Köhler 2009

#include "globals.h"
#include "geneticSystemCore.h"

namespace EDen {
  GeneticConditionType GeneticCondition::getConditionType() {
    return condType; 
  };

  bool GeneticCondition::dependsOnUnfullfilledConditionType(GeneticConditionType param_conditionType) {
    if((param_conditionType == condType) && (! (fullfilled()))) return true;
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

  GeneticConditionsList GeneticCompoundCondition::getChildConditions() {
    return childConditions; 
  };

  bool GeneticCompoundCondition::setBodypart(Bodypart* param_bodypart) {
    for(GeneticConditionsListIterator it = childConditions.begin(); it != childConditions.end(); it++) {
      (*it)->setBodypart(param_bodypart);
    };
    return true;
  };

  GeneticCompoundAction::GeneticCompoundAction(TiXmlElement* desc): GeneticAction(GAT_Compound) {
    for(TiXmlElement* it = desc->FirstChildElement(); it != 0; it = it->NextSiblingElement()) {
      add(GeneticAction::parseXmlElement(it)); 
    };
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

  GeneticAction* GeneticCompoundAction::copy() {
    GeneticCompoundAction* act = new GeneticCompoundAction();
    for(GeneticActionsListIterator it = childActions.begin(); it != childActions.end(); it++) {
      act->add((*it)->copy());
    };
    return act;
  };

  TiXmlElement* GeneticCompoundAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("CompoundAction");
    for(GeneticActionsListIterator it = childActions.begin(); it != childActions.end(); it++) {
      element->LinkEndChild((*it)->toXmlElement());
    };
    return element;
  };

  bool GeneticCompoundAction::setBodypart(Bodypart* param_bodypart) {
    for(GeneticActionsListIterator it = childActions.begin(); it != childActions.end(); it++) {
      (*it)->setBodypart(param_bodypart);
    };
    return true;
  };

  bool GeneticCompoundAction::add(GeneticAction* newAction) {
    childActions.push_back(newAction);
    return true;
  };
  
  GeneticActionsList GeneticCompoundAction::getChildActions() {
    return childActions; 
  };

  bool GeneticCompoundAction::execute() {
    bool result = true;
    
    for(GeneticActionsListIterator it = childActions.begin(); it != childActions.end(); it++) {
      result = result && (*it)->execute();
    };

    return result;
  };

  GeneticANDCondition::GeneticANDCondition(TiXmlElement* desc): GeneticCompoundCondition() {
    for(TiXmlElement* it = desc->FirstChildElement(); it != 0; it = it->NextSiblingElement()) {
      add(GeneticCondition::parseXmlElement(it)); 
    };
  };

  GeneticANDCondition::~GeneticANDCondition() {
    //GeneticCompoundCondition::~GeneticCompoundCondition(); 
  };

  GeneticCondition* GeneticANDCondition::copy() {
    GeneticANDCondition* cond = new GeneticANDCondition();
    for(GeneticConditionsListIterator it = childConditions.begin(); it != childConditions.end(); it++) {
      cond->add((*it)->copy());
    };
    return cond;
  };

  TiXmlElement* GeneticANDCondition::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("ConditionAND");
    for(GeneticConditionsListIterator it = childConditions.begin(); it != childConditions.end(); it++) {
      element->LinkEndChild((*it)->toXmlElement());
    };
    return element;
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

  bool GeneticANDCondition::dependsOnUnfullfilledConditionType(GeneticConditionType param_conditionType) {
    bool result = false;
    
    for(GeneticConditionsListIterator it = childConditions.begin(); it != childConditions.end(); it++) {
      if ((*it)->dependsOnUnfullfilledConditionType(param_conditionType)) {
        result = true;
        break; 
      };
    };

    return result;
  };

  GeneticORCondition::GeneticORCondition(TiXmlElement* desc): GeneticCompoundCondition() {
    for(TiXmlElement* it = desc->FirstChildElement(); it != 0; it = it->NextSiblingElement()) {
      add(GeneticCondition::parseXmlElement(it)); 
    };
  };

  GeneticORCondition::~GeneticORCondition() {
	  //GeneticCompoundCondition::~GeneticCompoundCondition(); 
  };

  GeneticCondition* GeneticORCondition::copy() {
    GeneticORCondition* cond = new GeneticORCondition();
    for(GeneticConditionsListIterator it = childConditions.begin(); it != childConditions.end(); it++) {
      cond->add((*it)->copy());
    };
    return cond;
  };

  TiXmlElement* GeneticORCondition::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("ConditionOR");
    for(GeneticConditionsListIterator it = childConditions.begin(); it != childConditions.end(); it++) {
      element->LinkEndChild((*it)->toXmlElement());
    };
    return element;
  };

  bool GeneticORCondition::fullfilled() {
    bool result = false;
    
    for(GeneticConditionsListIterator it = childConditions.begin(); it != childConditions.end(); it++) {
      result = result || (*it)->fullfilled();
    };
    return result;
  };

  bool GeneticORCondition::dependsOnUnfullfilledConditionType(GeneticConditionType param_conditionType) {
    bool result = true;
    bool notEmpty = false;
    
    for(GeneticConditionsListIterator it = childConditions.begin(); it != childConditions.end(); it++) {
      notEmpty = true;
      if (!((*it)->dependsOnUnfullfilledConditionType(param_conditionType))) {
        result = false;
        break; 
      };
    };

    return result && notEmpty;
  };

  GeneticActionType GeneticAction::getActionType() {
    return actType;  
  };

  bool GeneticAction::getActive() {
    return active;
  };

  GeneticClause::~GeneticClause() {
    if(description != "Branch Creation") {
      int i = 0 + 100;
    };
    
    delete cond;
    delete act;
  };

  GeneticClause* GeneticClause::copy() {
    return new GeneticClause(cond->copy(),act->copy(),description);
  };

  bool GeneticClause::run() {
    if(cond->fullfilled()) {
      act->execute();
      
      return true; 
    }
    else {
      return false;
    };
  };

  bool GeneticClause::setBodypart(Bodypart* param_bodypart) {
    cond->setBodypart(param_bodypart);
    act->setBodypart(param_bodypart);
    return true;
  };

  bool GeneticClause::dependsOnUnfullfilledConditionType(GeneticConditionType param_conditionType) {
    return cond->dependsOnUnfullfilledConditionType(param_conditionType);
  };

  TiXmlElement* GeneticClause::toXmlElement() {
    TiXmlElement* element = new TiXmlElement("Clause");
    TiXmlElement* actions = new TiXmlElement("Actions");
    TiXmlElement* conditions = new TiXmlElement("Conditions");

    actions->LinkEndChild(act->toXmlElement());
    conditions->LinkEndChild(cond->toXmlElement());
    element->SetAttribute("Description",description);
    
    element->LinkEndChild(conditions);
    element->LinkEndChild(actions);

    return element;
  };

  GeneticClause::GeneticClause(TiXmlElement* desc) {
    TiXmlElement* it;
    
    description = desc->Attribute("Description");

    it = desc->FirstChildElement("Conditions")->FirstChildElement();
    cond = GeneticCondition::parseXmlElement(it);
    it = desc->FirstChildElement("Actions")->FirstChildElement();
    act = GeneticAction::parseXmlElement(it);
  };
};