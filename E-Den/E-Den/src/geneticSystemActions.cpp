// geneticSystemActions.cpp
// by Franz Koehler 2009

#include "geneticSystemActions.h"

namespace EDen {
  GeneticSimpleChemicalConvertAction::GeneticSimpleChemicalConvertAction(
                                       std::string nFromChemName,
                                       std::string nToChemName,
                                       float nAmount,
                                       float nRatio, 
                                       Bodypart* p_bp): GeneticAction(GAT_SimpleConvert),
                                                             fromChemName(nFromChemName),
                                                             toChemName(nToChemName),
                                                             amount(nAmount),
                                                             ratio(nRatio) { verify(); setBodypart(p_bp); };
  
  GeneticSimpleChemicalConvertAction::~GeneticSimpleChemicalConvertAction() {
    //GeneticAction::~GeneticAction();
  };

  GeneticAction* GeneticSimpleChemicalConvertAction::copy() {
    return new GeneticSimpleChemicalConvertAction(fromChemName,toChemName,amount,ratio);
  };

  bool GeneticSimpleChemicalConvertAction::setBodypart(Bodypart* param_bodypart) {
    if(param_bodypart) storage = param_bodypart->getChemicalStorage();
    return true;
  };

  void GeneticSimpleChemicalConvertAction::verify() {
    if(amount < 0.0) amount = 0.0;
    if(ratio < 0.0) ratio = 0.0;
  };

  bool GeneticSimpleChemicalConvertAction::execute() {
    return(storage->add(fromChemName, -amount*storage->getSize()) && storage->add(toChemName, amount*ratio*storage->getSize()));
  };

  GeneticChemicalConsumeAction::GeneticChemicalConsumeAction(std::string nChemName, float nAmount, Bodypart* p_bp):
    GeneticAction(GAT_ChemicalConsume), chemName(nChemName), amount(nAmount) {
    verify();
    setBodypart(p_bp);
  };

  GeneticChemicalConsumeAction::~GeneticChemicalConsumeAction() {
    //GeneticAction::~GeneticAction();
  };

  GeneticAction* GeneticChemicalConsumeAction::copy() {
    return new GeneticChemicalConsumeAction(chemName,amount);
  };

  bool GeneticChemicalConsumeAction::setBodypart(Bodypart* param_bodypart) {
    if(param_bodypart) storage = param_bodypart->getChemicalStorage();
    return true;
  };

  void GeneticChemicalConsumeAction::verify() {
    if(amount < 0.0) amount = 0.0;
  };

  bool GeneticChemicalConsumeAction::execute() {
    //return storage->add(chemName,-amount);
    return storage->add(chemName,-amount*storage->getSize());
  };

  GeneticSpawnBodypartAction::GeneticSpawnBodypartAction(BodypartType param_childBodypartType, Bodypart* param_parentBodypart):
        GeneticAction(GAT_SpawnBP), childBodypartType(param_childBodypartType) { setBodypart(param_parentBodypart); };

  GeneticSpawnBodypartAction::~GeneticSpawnBodypartAction() {
    //GeneticAction::~GeneticAction();
  };

  GeneticAction* GeneticSpawnBodypartAction::copy() {
    return new GeneticSpawnBodypartAction(childBodypartType);
  };

  bool GeneticSpawnBodypartAction::setBodypart(Bodypart* p_bp) {
    parentBodypart = p_bp;
    return true;
  };

  bool GeneticSpawnBodypartAction::execute() {
    Bodypart* bp = new Bodypart(childBodypartType,parentBodypart->getGeneticCode()->copy(),parentBodypart->getParentOrganism());
    
    if(parentBodypart->spawnBodypart(bp)) {
      return true;
    }
    else {
      bp->destroy();
      delete bp;
      return false;
    }
  };

  GeneticChangeMaxChemicalAmountAction::GeneticChangeMaxChemicalAmountAction(std::string param_chemicalName, float param_value, Bodypart* param_bodypart):
        GeneticAction(GAT_ChangeMaxChemAmount), chemName(param_chemicalName), value(param_value) { setBodypart(param_bodypart); };

  GeneticChangeMaxChemicalAmountAction::~GeneticChangeMaxChemicalAmountAction() {
    //GeneticAction::~GeneticAction();
  };

  GeneticAction* GeneticChangeMaxChemicalAmountAction::copy() {
    return new GeneticChangeMaxChemicalAmountAction(chemName,value);
  };

  bool GeneticChangeMaxChemicalAmountAction::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart;
    return true;
  };

  bool GeneticChangeMaxChemicalAmountAction::execute() {
    bp->getChemicalStorage()->setMaxAmount(chemName,value);
    return true;
  };

  GeneticAddSpawnpointAction::GeneticAddSpawnpointAction(BodypartType param_bodypartType, int param_position, float param_ang2d, Bodypart* param_bodypart):
    GeneticAction(GAT_AddSpawnpoint), spawnpointAdded(false) {
    setBodypart(param_bodypart);
    sp = new SpawnpointInformation();
    sp->addSupportedType(param_bodypartType);
    sp->position = param_position;
    sp->occupied = false;
    sp->ang2d = param_ang2d;
  };

  GeneticAddSpawnpointAction::GeneticAddSpawnpointAction(std::list<BodypartType> param_bodypartTypes, int param_position, float param_ang2d, Bodypart* param_bodypart):
    GeneticAction(GAT_AddSpawnpoint), spawnpointAdded(false) {
    setBodypart(param_bodypart);
    sp = new SpawnpointInformation();
    sp->supportedBpTypes = param_bodypartTypes;
    sp->position = param_position;
    sp->occupied = false;
    sp->ang2d = param_ang2d;
  };

  GeneticAddSpawnpointAction::~GeneticAddSpawnpointAction() {
    delete sp;
  };

  GeneticAction* GeneticAddSpawnpointAction::copy() {
    return new GeneticAddSpawnpointAction(sp->supportedBpTypes,sp->position,sp->ang2d);
  };

  bool GeneticAddSpawnpointAction::setBodypart(Bodypart* p_bp) {
    bp = p_bp;
    return true;
  };

  bool GeneticAddSpawnpointAction::execute() {
    bp->addSpawnpoint(sp->copy());
    spawnpointAdded = true;
    return true;
  };

  GeneticGrowAction::GeneticGrowAction(float param_amount, Bodypart* param_bodypart):
    GeneticAction(GAT_Grow) {
    setBodypart(param_bodypart);
    amount = param_amount;
  };

  GeneticGrowAction::~GeneticGrowAction() {
      
  };

  GeneticAction* GeneticGrowAction::copy() {
    return new GeneticGrowAction(amount);
  };

  bool GeneticGrowAction::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart; 
    return true;
  };

  bool GeneticGrowAction::execute() {
    bp->grow(amount);
    return true;
  };

  GeneticShrinkAction::GeneticShrinkAction(float param_amount, Bodypart* param_bodypart): 
    GeneticAction(GAT_Shrink) {
    setBodypart(param_bodypart);
    amount = param_amount;
  };
  
  GeneticShrinkAction::~GeneticShrinkAction() {};

  GeneticAction* GeneticShrinkAction::copy() {
    return new GeneticShrinkAction(amount);
  };

  bool GeneticShrinkAction::setBodypart(Bodypart* p_bp) {
    bp = p_bp;
    return true;
  };

  bool GeneticShrinkAction::execute() {
    bp->shrink(amount);
    return true;
  };

  GeneticHurtAction::GeneticHurtAction(float param_amount, Bodypart* param_bodypart):
    GeneticAction(GAT_Hurt) {
    setBodypart(param_bodypart);
    amount = param_amount;
  };

  GeneticHurtAction::~GeneticHurtAction() {
      
  };

  GeneticAction* GeneticHurtAction::copy() {
    return new GeneticHurtAction(amount);
  };

  bool GeneticHurtAction::setBodypart(Bodypart* p_bp) {
    bp = p_bp;
    return true;
  };

  bool GeneticHurtAction::execute() {
    bp->hurt(amount);
    return true;
  };

  GeneticSimpleMutateAction::GeneticSimpleMutateAction(Bodypart* p_bp, float p_strength) {
    strength = p_strength;
    bp = p_bp;
  };
  
  GeneticSimpleMutateAction::~GeneticSimpleMutateAction() {
    
  };

  bool GeneticSimpleMutateAction::execute() {
    bp->getGeneticCode()->mutate(strength);
    return true;
  };

  bool GeneticSimpleMutateAction::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart;
    return true;
  };

  GeneticAction* GeneticSimpleMutateAction::copy() {
    return new GeneticSimpleMutateAction(0,strength); 
  };

  GeneticHealParentAction::GeneticHealParentAction(float param_amount, Bodypart* p_bp):
    GeneticAction(GAT_Hurt) {
    setBodypart(p_bp);
    amount = param_amount;
  };

  GeneticHealParentAction::~GeneticHealParentAction() {
    
  };

  GeneticAction* GeneticHealParentAction::copy() {
    return new GeneticHealParentAction(amount);
  };

  bool GeneticHealParentAction::setBodypart(Bodypart* p_bp) {
    bp = p_bp;
    return true;
  };

  bool GeneticHealParentAction::execute() {
    if(bp->getParentBodypart()) {
      bp->getParentBodypart()->heal(amount);
      return true;
    }
    else return false;
  };

  GeneticHealAction::GeneticHealAction(float param_amount, Bodypart* param_bodypart):
    GeneticAction(GAT_Heal) {
    setBodypart(param_bodypart);
    amount = param_amount;
  };

  GeneticHealAction::~GeneticHealAction() {
      
  };

  GeneticAction* GeneticHealAction::copy() {
    return new GeneticHealAction(amount);
  };

  bool GeneticHealAction::setBodypart(Bodypart* p_bp) {
    bp = p_bp;
    return true;
  };

  bool GeneticHealAction::execute() {
    bp->heal(amount);
    return true;
  };

  GeneticDieAction::GeneticDieAction(Bodypart* param_bodypart):GeneticAction(GAT_Die) {
    setBodypart(param_bodypart);
  };

  GeneticDieAction::~GeneticDieAction() {
  
  };

  GeneticAction* GeneticDieAction::copy() {
    return new GeneticDieAction();
  };

  bool GeneticDieAction::setBodypart(Bodypart* p_bp) {
    bp = p_bp;
    return true;
  };
  
  bool GeneticDieAction::execute() {
    bp->setMaxSize(0.0f);
    bp->shrink(0.0f);
    bp->setBodypartState(BSP_dead);

    Organism* org = bp->getParentOrganism();
    if(org) {
      RuntimeManager* rm = org->getRuntimeManager();
      if(rm) rm->unregisterBodypart(bp);
    };
    return true;
  };

  GeneticEmptyChemicalStorageAction::GeneticEmptyChemicalStorageAction(Bodypart* param_bodypart):GeneticAction(GAT_ChemicalConsume) {
    setBodypart(param_bodypart);
  };

  GeneticEmptyChemicalStorageAction::~GeneticEmptyChemicalStorageAction() {
    
  };

  GeneticAction* GeneticEmptyChemicalStorageAction::copy() {
    return new GeneticEmptyChemicalStorageAction();
  };

  bool GeneticEmptyChemicalStorageAction::setBodypart(Bodypart* p_bp) {
    bp = p_bp;
    return true;
  };

  bool GeneticEmptyChemicalStorageAction::execute() {
    Bodypart* pbp = bp->getParentBodypart();
    if(pbp) {
      bp->getChemicalStorage()->moveAllToStorage(bp->getParentBodypart()->getChemicalStorage());
      return true;
    }
    else return true;
  };

  GeneticChangeMaxSizeAction::GeneticChangeMaxSizeAction(float param_amount, Bodypart* param_bodypart):GeneticAction(GAT_ChangeMaxSize) {
     setBodypart(param_bodypart);
     amount = param_amount;
  };

  GeneticChangeMaxSizeAction::~GeneticChangeMaxSizeAction() {
    
  };

  GeneticAction* GeneticChangeMaxSizeAction::copy() {
    return new GeneticChangeMaxSizeAction(amount);
  };

  bool GeneticChangeMaxSizeAction::setBodypart(Bodypart* p_bp) {
    bp = p_bp;
    return true;
  };

  bool GeneticChangeMaxSizeAction::execute() {
    bp->setMaxSize(amount);
    return true;
  };

  GeneticDropSeedAction::GeneticDropSeedAction(Bodypart* p_bp):GeneticAction(GAT_DropSeed) {
    bp = p_bp;
  };

  GeneticDropSeedAction::~GeneticDropSeedAction() {
    
  };

  bool GeneticDropSeedAction::execute() {
    //Organism* org = bp->getParentOrganism();
    //if(org) {
    //  RuntimeManager* rtm = org->getRuntimeManager();
    //  if(rtm)
    //    if(rtm->getOrganismCount() < MAX_PLANT_COUNT) {
          bp->setBodypartState(BSP_seed);
          bp->detachToNewOrganism();
    //    }
    //};
    return true;
  };

  bool GeneticDropSeedAction::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart;
    return true;
  };
  
  GeneticAction* GeneticDropSeedAction::copy() {
    return new GeneticDropSeedAction();
  };

} // namespace