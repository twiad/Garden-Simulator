// geneticSystemActions.cpp
// by Franz Koehler 2009

#include "geneticSystemActions.h"

namespace EDen {
  GeneticSimpleChemicalConvertAction::~GeneticSimpleChemicalConvertAction() {
    //GeneticAction::~GeneticAction();
  };

  void GeneticSimpleChemicalConvertAction::verify() {
    if(amount < 0.0) amount = 0.0;
    if(ratio < 0.0) ratio = 0.0;
  };

  bool GeneticSimpleChemicalConvertAction::execute() {
    return(storage->add(fromChemName, -amount) && storage->add(toChemName, amount*ratio));
  };

  GeneticChemicalConsumeAction::~GeneticChemicalConsumeAction() {
    //GeneticAction::~GeneticAction();
  };

  void GeneticChemicalConsumeAction::verify() {
    if(amount < 0.0) amount = 0.0;
  };

  bool GeneticChemicalConsumeAction::execute() {
    return storage->add(chemName,-amount);
    //return storage->add(chemName,-amount*storage->getSize());
  };

  GeneticSpawnBodypartAction::~GeneticSpawnBodypartAction() {
    //GeneticAction::~GeneticAction();
  };

  bool GeneticSpawnBodypartAction::execute() {
    Bodypart* bp = new Bodypart(childBodypartType,parentBodypart->getGeneticCode(),parentBodypart->getParentOrganism());
    
    if(parentBodypart->spawnBodypart(bp)) {
      return true;
    }
    else {
      bp->destroy();
      delete bp;
      return false;
    }
  };

  GeneticChangeMaxChemicalAmountAction::~GeneticChangeMaxChemicalAmountAction() {
    //GeneticAction::~GeneticAction();
  };

  bool GeneticChangeMaxChemicalAmountAction::execute() {
    bp->getChemicalStorage()->setMaxAmount(chemName,value);
    return true;
  };

  GeneticAddSpawnpointAction::GeneticAddSpawnpointAction(Bodypart* param_bodypart, BodypartType param_bodypartType, int param_position, float param_ang2d):
    GeneticAction(GAT_AddSpawnpoint), spawnpointAdded(false) {
    bp = param_bodypart;
    sp = new SpawnpointInformation();
    sp->addSupportedType(param_bodypartType);
    sp->position = param_position;
    sp->occupied = false;
    sp->ang2d = param_ang2d;
  };

  GeneticAddSpawnpointAction::GeneticAddSpawnpointAction(Bodypart* param_bodypart, std::list<BodypartType> param_bodypartTypes, int param_position, float param_ang2d):
    GeneticAction(GAT_AddSpawnpoint), spawnpointAdded(false) {
    bp = param_bodypart;
    sp = new SpawnpointInformation();
    sp->supportedBpTypes = param_bodypartTypes;
    sp->position = param_position;
    sp->occupied = false;
    sp->ang2d = param_ang2d;
  };

  GeneticAddSpawnpointAction::~GeneticAddSpawnpointAction() {
    if(!spawnpointAdded) delete sp;
  };

  bool GeneticAddSpawnpointAction::execute() {
    bp->addSpawnpoint(sp);
    spawnpointAdded = true;
    return true;
  };

  GeneticGrowAction::GeneticGrowAction(Bodypart* param_bodypart, float param_amount):
    GeneticAction(GAT_Grow) {
    bp = param_bodypart;
    amount = param_amount;
  };

  GeneticGrowAction::~GeneticGrowAction() {
      
  };

  bool GeneticGrowAction::execute() {
    bp->grow(amount);
    return true;
  };

  GeneticShrinkAction::GeneticShrinkAction(Bodypart* param_bodypart, float param_amount): 
    GeneticAction(GAT_Shrink) {
    bp = param_bodypart;
    amount = param_amount;
  };
  
  GeneticShrinkAction::~GeneticShrinkAction() {};

  bool GeneticShrinkAction::execute() {
    bp->shrink(amount);
    return true;
  };

  GeneticHurtAction::GeneticHurtAction(Bodypart* param_bodypart, float param_amount):
    GeneticAction(GAT_Hurt) {
    bp = param_bodypart;
    amount = param_amount;
  };

  GeneticHurtAction::~GeneticHurtAction() {
      
  };

  bool GeneticHurtAction::execute() {
    bp->hurt(amount);
    return true;
  };

  GeneticHealParentAction::GeneticHealParentAction(Bodypart* param_bodypart, float param_amount):
    GeneticAction(GAT_Hurt) {
    bp = param_bodypart;
    amount = param_amount;
  };

  GeneticHealParentAction::~GeneticHealParentAction() {
    
  };

  bool GeneticHealParentAction::execute() {
    if(bp->getParentBodypart()) {
      bp->getParentBodypart()->heal(amount);
      return true;
    }
    else return false;
  };

  GeneticHealAction::GeneticHealAction(Bodypart* param_bodypart, float param_amount):
    GeneticAction(GAT_Heal) {
    bp = param_bodypart;
    amount = param_amount;
  };

  GeneticHealAction::~GeneticHealAction() {
      
  };

  bool GeneticHealAction::execute() {
    bp->heal(amount);
    return true;
  };

  GeneticDieAction::GeneticDieAction(Bodypart* param_bodypart):GeneticAction(GAT_Die) {
    bp = param_bodypart;
  };

  GeneticDieAction::~GeneticDieAction() {
  
  };
  
  bool GeneticDieAction::execute() {
    bp->setMaxSize(0.0f);
    bp->shrink(0.0f);
    bp->setBodypartState(BSP_dead);
    return true;
  };

  GeneticEmptyChemicalStorageAction::GeneticEmptyChemicalStorageAction(Bodypart* param_bodypart):GeneticAction(GAT_ChemicalConsume) {
    bp = param_bodypart;
  };

  GeneticEmptyChemicalStorageAction::~GeneticEmptyChemicalStorageAction() {
    
  };

  bool GeneticEmptyChemicalStorageAction::execute() {
    Bodypart* pbp = bp->getParentBodypart();
    if(pbp) {
      bp->getChemicalStorage()->moveAllToStorage(bp->getParentBodypart()->getChemicalStorage());
      return true;
    }
    else return false;
  };

  GeneticChangeMaxSizeAction::GeneticChangeMaxSizeAction(Bodypart* param_bodypart, float param_amount):GeneticAction(GAT_ChangeMaxSize) {
     bp = param_bodypart;
     amount = param_amount;
  };

  GeneticChangeMaxSizeAction::~GeneticChangeMaxSizeAction() {
    
  };

  bool GeneticChangeMaxSizeAction::execute() {
    bp->setMaxSize(amount);
    return true;
  };

} // namespace