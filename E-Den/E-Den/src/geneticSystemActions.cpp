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
  
  GeneticSimpleChemicalConvertAction::GeneticSimpleChemicalConvertAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_SimpleConvert) {
    fromChemName = description->Attribute("FromName");
    toChemName = description->Attribute("ToName");
    description->QueryFloatAttribute("Amount",&amount);
    description->QueryFloatAttribute("Ratio",&ratio);

    verify();
    setBodypart(p_bp);
  };

  GeneticSimpleChemicalConvertAction::~GeneticSimpleChemicalConvertAction() {
    //GeneticAction::~GeneticAction();
  };

  GeneticAction* GeneticSimpleChemicalConvertAction::copy() {
    return new GeneticSimpleChemicalConvertAction(fromChemName,toChemName,amount,ratio);
  };

  TiXmlElement* GeneticSimpleChemicalConvertAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("ChemicalConvertAction");

    element->SetAttribute("FromName",fromChemName);
    element->SetAttribute("ToName",toChemName);
    element->SetDoubleAttribute("Amount",amount);
    element->SetDoubleAttribute("Ratio",ratio);

    return element;
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
    storage->add(fromChemName, -amount*storage->getSize());
    storage->add(toChemName, amount*ratio*storage->getSize());
    return true;
  };

  GeneticChemicalConsumeAction::GeneticChemicalConsumeAction(std::string nChemName, float nAmount, Bodypart* p_bp):
    GeneticAction(GAT_ChemicalConsume), chemName(nChemName), amount(nAmount) {
    verify();
    setBodypart(p_bp);
  };

  GeneticChemicalConsumeAction::GeneticChemicalConsumeAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_ChemicalConsume) {
    chemName = description->Attribute("Name");
    description->QueryFloatAttribute("Amount",&amount);

    verify();
    setBodypart(p_bp);
  };

  GeneticChemicalConsumeAction::~GeneticChemicalConsumeAction() {
    //GeneticAction::~GeneticAction();
  };

  GeneticAction* GeneticChemicalConsumeAction::copy() {
    return new GeneticChemicalConsumeAction(chemName,amount);
  };

  TiXmlElement* GeneticChemicalConsumeAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("ChemicalConsumeAction");

    element->SetAttribute("Name",chemName);
    element->SetDoubleAttribute("Amount",amount);

    return element;
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

  //

  GeneticRegularChemicalConsumeAction::GeneticRegularChemicalConsumeAction(std::string nChemName, float nAmount, int p_maxLifeTime, Bodypart* p_bp):
    GeneticAction(GAT_ChemicalConsume), chemName(nChemName), amount(nAmount), maxLifeTime(p_maxLifeTime) {
    verify();
    setBodypart(p_bp);
  };

  GeneticRegularChemicalConsumeAction::GeneticRegularChemicalConsumeAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_ChemicalConsume) {
    chemName = description->Attribute("Name");
    description->QueryFloatAttribute("Amount",&amount);
    description->QueryIntAttribute("Lifetime",&maxLifeTime);

    verify();
    setBodypart(p_bp);
  };

  GeneticRegularChemicalConsumeAction::~GeneticRegularChemicalConsumeAction() {
    //GeneticAction::~GeneticAction();
  };

  GeneticAction* GeneticRegularChemicalConsumeAction::copy() {
    return new GeneticRegularChemicalConsumeAction(chemName,amount,maxLifeTime);
  };

  TiXmlElement* GeneticRegularChemicalConsumeAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("RegularChemicalConsumeAction");

    element->SetAttribute("Name",chemName);
    element->SetDoubleAttribute("Amount",amount);
    element->SetAttribute("Lifetime",maxLifeTime);

    return element;
  };

  bool GeneticRegularChemicalConsumeAction::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart;
    if(bp) storage = bp->getChemicalStorage();
    return true;
  };

  void GeneticRegularChemicalConsumeAction::verify() {
    if(amount < 0.0) amount = 0.0;
  };

  bool GeneticRegularChemicalConsumeAction::execute() {
    float localamount = amount;
    Organism* org = bp->getParentOrganism();
    if(org != 0) {
      int lifetime = bp->getParentOrganism()->getLifetime();
      float delta = (float)(maxLifeTime - lifetime);
      if(delta > 0) {
        localamount = ((float)(maxLifeTime - delta)*0.9f)/((float)maxLifeTime);
        if(localamount > 1.0f)
          localamount = localamount * localamount;
        localamount = localamount * amount * storage->getSize();
      }
      else if(delta == 0)
        localamount = storage->getCurrentAmount(chemName);
      else
        localamount = storage->getCurrentAmount(chemName);
    };
    return storage->add(chemName,-localamount);
  };

  GeneticSpawnBodypartAction::GeneticSpawnBodypartAction(BodypartType param_childBodypartType, Bodypart* param_parentBodypart):
        GeneticAction(GAT_SpawnBP), childBodypartType(param_childBodypartType) { setBodypart(param_parentBodypart); };

  GeneticSpawnBodypartAction::GeneticSpawnBodypartAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_SpawnBP) {
    int type;
    description->QueryIntAttribute("Type",&type);
    childBodypartType = (BodypartType)type;

    setBodypart(p_bp);
  };

  GeneticSpawnBodypartAction::~GeneticSpawnBodypartAction() {
    //GeneticAction::~GeneticAction();
  };

  GeneticAction* GeneticSpawnBodypartAction::copy() {
    return new GeneticSpawnBodypartAction(childBodypartType);
  };

  TiXmlElement* GeneticSpawnBodypartAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("SpawnBodypartAction");

    element->SetAttribute("Type",(int)childBodypartType);

    return element;
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

  GeneticChangeMaxChemicalAmountAction::GeneticChangeMaxChemicalAmountAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_ChangeMaxChemAmount) {
    chemName = description->Attribute("Name");
    description->QueryFloatAttribute("Value",&value);

    setBodypart(p_bp);
  };

  GeneticChangeMaxChemicalAmountAction::~GeneticChangeMaxChemicalAmountAction() {
    //GeneticAction::~GeneticAction();
  };

  GeneticAction* GeneticChangeMaxChemicalAmountAction::copy() {
    return new GeneticChangeMaxChemicalAmountAction(chemName,value);
  };

  TiXmlElement* GeneticChangeMaxChemicalAmountAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("ChangeMaxChemicalAmountAction");

    element->SetAttribute("Name",chemName);
    element->SetDoubleAttribute("Value",value);

    return element;
  };

  bool GeneticChangeMaxChemicalAmountAction::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart;
    return true;
  };

  bool GeneticChangeMaxChemicalAmountAction::execute() {
    bp->getChemicalStorage()->setMaxAmount(chemName,value);
    return true;
  };

  GeneticAddSpawnpointAction::GeneticAddSpawnpointAction(BodypartType param_bodypartType, int param_position, float param_ang2d, float p_ang2, float p_rot, bool p_symetric, bool p_active, Bodypart* param_bodypart):
    GeneticAction(GAT_AddSpawnpoint), spawnpointAdded(false) {
    setBodypart(param_bodypart);
    sp = new SpawnpointInformation();
    sp->addSupportedType(param_bodypartType);
    sp->position = param_position;
    sp->occupied = false;
    sp->ang2d = param_ang2d;
    sp->ang2 = p_ang2;
    sp->rot = p_rot;
    symetric = p_symetric;
    active = p_active;
  };

  GeneticAddSpawnpointAction::GeneticAddSpawnpointAction(std::list<BodypartType> param_bodypartTypes, int param_position, float param_ang2d, float p_ang2, float p_rot, bool p_symetric, bool p_active, Bodypart* param_bodypart):
    GeneticAction(GAT_AddSpawnpoint), spawnpointAdded(false) {
    setBodypart(param_bodypart);
    sp = new SpawnpointInformation();
    sp->supportedBpTypes = param_bodypartTypes;
    sp->position = param_position;
    sp->occupied = false;
    sp->ang2d = param_ang2d;
    sp->ang2 = p_ang2;
    sp->rot = p_rot;
    symetric = p_symetric;
    active = p_active;
  };

  GeneticAddSpawnpointAction::GeneticAddSpawnpointAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_AddSpawnpoint) {
    description->QueryValueAttribute("Active",&active);
    description->QueryValueAttribute("Symetric",&symetric);
    sp = new SpawnpointInformation();
    Bodypart::xmlElementToSpawnpoint(description->FirstChildElement("Spawnpoint"),sp);
    setBodypart(p_bp);
  };

  GeneticAddSpawnpointAction::~GeneticAddSpawnpointAction() {
    delete sp;
  };

  GeneticAction* GeneticAddSpawnpointAction::copy() {
    SpawnpointInformation deb;

    deb.position = sp->position;
    deb.ang2d = sp->ang2d;
    deb.ang2 = sp->ang2;
    deb.rot =sp->rot;
    deb.supportedBpTypes = sp->supportedBpTypes;

    return new GeneticAddSpawnpointAction(sp->supportedBpTypes,sp->position,sp->ang2d,sp->ang2,sp->rot,symetric,active);
  };

  TiXmlElement* GeneticAddSpawnpointAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("AddSpawnpointAction");
    
    element->SetAttribute("Active",(int)active);
    element->SetAttribute("Symetric",(int)symetric);
    TiXmlElement* element2 = new TiXmlElement("Spawnpoint");
    Bodypart::spawnpointToXmlElement(sp,element2);
    element->LinkEndChild(element2);

    return element;
  };

  bool GeneticAddSpawnpointAction::setBodypart(Bodypart* p_bp) {
    bp = p_bp;
    return true;
  };

  bool GeneticAddSpawnpointAction::execute() {
    if(active) {
      bp->addSpawnpoint(sp->copy());
      if(symetric) {
        sp->ang2d = -1 * sp->ang2d;
        bp->addSpawnpoint(sp->copy());
        sp->ang2d = -1 * sp->ang2d;
      };
      spawnpointAdded = true;
    };
    return true;
  };

  GeneticSetColorAction::GeneticSetColorAction(float red, float green, float blue, float alpha, Bodypart* param_bodypart): GeneticAction(GAT_SetColor) {
    color.r = red;
    color.g = green;
    color.b = blue;
    color.a = alpha;
    bp = param_bodypart;
  };

  GeneticSetColorAction::GeneticSetColorAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_SetColor) {
    description->QueryFloatAttribute("Red",&(color.r));
    description->QueryFloatAttribute("Green",&(color.g));
    description->QueryFloatAttribute("Blue",&(color.b));
    description->QueryFloatAttribute("Alpha",&(color.a));
    bp = p_bp;
  };

  GeneticSetColorAction::~GeneticSetColorAction() {
    
  };

  bool GeneticSetColorAction::execute() {
    bp->color.r = color.r;  
    bp->color.g = color.g;
    bp->color.b = color.b;
    bp->color.a = color.a;
    return true;
  };

  bool GeneticSetColorAction::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart;
    return true;
  };

  GeneticAction* GeneticSetColorAction::copy() {
    return new GeneticSetColorAction(color.r,color.g,color.b,color.a);  
  };

  TiXmlElement* GeneticSetColorAction::toXmlElement() {
    TiXmlElement* element = new TiXmlElement("SetColorAction");

    element->SetDoubleAttribute("Red", color.r);
    element->SetDoubleAttribute("Green", color.g);
    element->SetDoubleAttribute("Blue", color.b);
    element->SetDoubleAttribute("Alpha", color.a);

    return element;
  };

  GeneticGrowAction::GeneticGrowAction(float param_amount, Bodypart* param_bodypart):
    GeneticAction(GAT_Grow) {
    setBodypart(param_bodypart);
    amount = param_amount;
  };

  GeneticGrowAction::GeneticGrowAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_Grow) {
    description->QueryFloatAttribute("Amount",&amount);

    setBodypart(p_bp);
  };

  GeneticGrowAction::~GeneticGrowAction() {
      
  };

  GeneticAction* GeneticGrowAction::copy() {
    return new GeneticGrowAction(amount);
  };

  TiXmlElement* GeneticGrowAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("GrowAction");
    
    element->SetDoubleAttribute("Amount",amount);

    return element;
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

  GeneticShrinkAction::GeneticShrinkAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_Shrink) {
    description->QueryFloatAttribute("Amount",&amount);

    setBodypart(p_bp);
  };
  
  GeneticShrinkAction::~GeneticShrinkAction() {};

  GeneticAction* GeneticShrinkAction::copy() {
    return new GeneticShrinkAction(amount);
  };

  TiXmlElement* GeneticShrinkAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("ShrinkAction");
    
    element->SetDoubleAttribute("Amount",amount);

    return element;
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

  GeneticHurtAction::GeneticHurtAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_Hurt) {
    description->QueryFloatAttribute("Amount",&amount);

    setBodypart(p_bp);
  };

  GeneticHurtAction::~GeneticHurtAction() {
      
  };

  GeneticAction* GeneticHurtAction::copy() {
    return new GeneticHurtAction(amount);
  };

  TiXmlElement* GeneticHurtAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("HurtAction");
    
    element->SetDoubleAttribute("Amount",amount);

    return element;
  };

  bool GeneticHurtAction::setBodypart(Bodypart* p_bp) {
    bp = p_bp;
    return true;
  };

  bool GeneticHurtAction::execute() {
    bp->hurt(amount);
    return true;
  };

  GeneticSimpleMutateAction::GeneticSimpleMutateAction(Bodypart* p_bp, float p_strength): GeneticAction(GAT_Mutate) {
    strength = p_strength;
    bp = p_bp;
  };

  GeneticSimpleMutateAction::GeneticSimpleMutateAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_Mutate) {
    description->QueryFloatAttribute("Strength",&strength);

    setBodypart(p_bp);
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

  TiXmlElement* GeneticSimpleMutateAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("MutateAction");
    
    element->SetDoubleAttribute("Strength",strength);

    return element;
  };

  GeneticHealParentAction::GeneticHealParentAction(float param_amount, Bodypart* p_bp):
    GeneticAction(GAT_Heal) {
    setBodypart(p_bp);
    amount = param_amount;
  };

  GeneticHealParentAction::GeneticHealParentAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_Heal) {
    description->QueryFloatAttribute("Amount",&amount);

    setBodypart(p_bp);
  };

  GeneticHealParentAction::~GeneticHealParentAction() {
    
  };

  GeneticAction* GeneticHealParentAction::copy() {
    return new GeneticHealParentAction(amount);
  };

  TiXmlElement* GeneticHealParentAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("HealParentAction");
    
    element->SetDoubleAttribute("Amount",amount);

    return element;
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

  GeneticHealAction::GeneticHealAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_Heal) {
    description->QueryFloatAttribute("Amount",&amount);

    setBodypart(p_bp);
  };

  GeneticHealAction::~GeneticHealAction() {
      
  };

  GeneticAction* GeneticHealAction::copy() {
    return new GeneticHealAction(amount);
  };

  TiXmlElement* GeneticHealAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("HealAction");
    
    element->SetDoubleAttribute("Amount",amount);

    return element;
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

  GeneticDieAction::GeneticDieAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_Die) {
    setBodypart(p_bp);
  };

  GeneticDieAction::~GeneticDieAction() {
  
  };

  GeneticAction* GeneticDieAction::copy() {
    return new GeneticDieAction();
  };

  TiXmlElement* GeneticDieAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("DieAction");

    return element;
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

  GeneticEmptyChemicalStorageAction::GeneticEmptyChemicalStorageAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_ChemicalConsume) {
    setBodypart(p_bp);
  };

  GeneticEmptyChemicalStorageAction::~GeneticEmptyChemicalStorageAction() {
    
  };

  GeneticAction* GeneticEmptyChemicalStorageAction::copy() {
    return new GeneticEmptyChemicalStorageAction();
  };

  TiXmlElement* GeneticEmptyChemicalStorageAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("EmptyChemicalStorageAction");

    return element;
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

  GeneticChangeMaxSizeAction::GeneticChangeMaxSizeAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_ChangeMaxSize) {
    description->QueryFloatAttribute("Amount",&amount);

    setBodypart(p_bp);
  };

  GeneticChangeMaxSizeAction::~GeneticChangeMaxSizeAction() {
    
  };

  GeneticAction* GeneticChangeMaxSizeAction::copy() {
    return new GeneticChangeMaxSizeAction(amount);
  };

  TiXmlElement* GeneticChangeMaxSizeAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("ChangeMaxSizeAction");
    
    element->SetDoubleAttribute("Amount",amount);

    return element;
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

  GeneticDropSeedAction::GeneticDropSeedAction(TiXmlElement* description, Bodypart* p_bp): GeneticAction(GAT_DropSeed) {
    setBodypart(p_bp);
  };

  GeneticDropSeedAction::~GeneticDropSeedAction() {
    
  };

  bool GeneticDropSeedAction::execute() {
    Organism* parentOrganism = bp->getParentOrganism();
    GeneticCode* code = bp->getGeneticCode();
    bp->setBodypartState(BSP_seed);
    if(parentOrganism != 0) {
      int bpcount = parentOrganism->getBodypartCount();
      code->setSubSpeciesIdentifier((int)(bpcount / 25));
    };
    code->incGeneration();
    bp->detachToNewOrganism();
  return true;
  };

  bool GeneticDropSeedAction::setBodypart(Bodypart* param_bodypart) {
    bp = param_bodypart;
    return true;
  };
  
  GeneticAction* GeneticDropSeedAction::copy() {
    return new GeneticDropSeedAction();
  };

  TiXmlElement* GeneticDropSeedAction::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("DropSeedAction");

    return element;
  };

} // namespace