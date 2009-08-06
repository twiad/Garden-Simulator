// geneticSystemActions.h
// by Franz Koehler 2009

#include "globals.h"
#include "geneticSystemCore.h"
#include "chemicalSystem.h"


#ifndef _E_DEN_CHEMICAL_SYSTEM_ACTIONS_HEADER_
#define _E_DEN_CHEMICAL_SYSTEM_ACTIONS_HEADER_

#include "bodypart.h"

namespace EDen {
  class SpawnpointInformation;

  class GeneticSimpleChemicalConvertAction: public GeneticAction {
  private:
    void verify();
  
  protected:
    ChemicalStorage* storage;
    std::string fromChemName;
    std::string toChemName;
    float amount;
    float ratio;

  public:
    GeneticSimpleChemicalConvertAction(ChemicalStorage* nStorage,
                                       std::string nFromChemName,
                                       std::string nToChemName,
                                       float nAmount,
                                       float nRatio = 1.0f): GeneticAction(GAT_SimpleConvert),
                                                             storage(nStorage),
                                                             fromChemName(nFromChemName),
                                                             toChemName(nToChemName),
                                                             amount(nAmount),
                                                             ratio(nRatio) { verify(); };

    virtual ~GeneticSimpleChemicalConvertAction();

    virtual bool execute();
  };

  class GeneticChemicalConsumeAction: public GeneticAction {
  private:
    virtual void verify();
  protected:
    ChemicalStorage* storage;
    std::string chemName;
    float amount;
  public:
    GeneticChemicalConsumeAction(ChemicalStorage* nStorage, std::string nChemName, float nAmount):
        GeneticAction(GAT_ChemicalConsume), storage(nStorage), chemName(nChemName), amount(nAmount) {verify();};
    
    virtual ~GeneticChemicalConsumeAction();

    virtual bool execute();
  };

  class GeneticSpawnBodypartAction: public GeneticAction {
  private:
    Bodypart* parentBodypart;
    BodypartType childBodypartType;
  public:
    GeneticSpawnBodypartAction(Bodypart* param_parentBodypart, BodypartType param_childBodypartType):
        GeneticAction(GAT_SpawnBP), parentBodypart(param_parentBodypart), childBodypartType(param_childBodypartType) {};
    virtual ~GeneticSpawnBodypartAction();

    virtual bool execute();
  };

  class GeneticChangeMaxChemicalAmountAction: public GeneticAction {
  private:
    Bodypart* bp;
    float value;
    std::string chemName;
  public:
    GeneticChangeMaxChemicalAmountAction(Bodypart* param_bodypart, std::string param_chemicalName, float param_value):
        GeneticAction(GAT_ChangeMaxChemAmount), bp(param_bodypart), chemName(param_chemicalName), value(param_value) {};

    virtual ~GeneticChangeMaxChemicalAmountAction();

    virtual bool execute();
  };

  class GeneticAddSpawnpointAction: public GeneticAction {
  private:
    Bodypart* bp;
    SpawnpointInformation* sp;
    bool spawnpointAdded;
  public:
    GeneticAddSpawnpointAction(Bodypart* param_bodypart, BodypartType param_bodypartType, int param_position, float param_ang2d);
    GeneticAddSpawnpointAction(Bodypart* param_bodypart, std::list<BodypartType> param_bodypartTypes, int param_position, float param_ang2d);
    
    virtual ~GeneticAddSpawnpointAction();

    virtual bool execute();
  };

  class GeneticHurtAction: public GeneticAction {
  protected:
    Bodypart* bp;
    float amount;
  public:
    GeneticHurtAction(Bodypart* param_bodypart, float param_amount);
    virtual ~GeneticHurtAction();

    virtual bool execute();
  };

  class GeneticHealAction: public GeneticAction {
  protected:
    Bodypart* bp;
    float amount;
  public:
    GeneticHealAction(Bodypart* param_bodypart, float param_amount);
    ~GeneticHealAction();

    virtual bool execute();
  };

  class GeneticHealParentAction: public GeneticAction {
  protected:
    Bodypart* bp;
    float amount;
  public:
    GeneticHealParentAction(Bodypart* param_bodypart, float param_amount);
    ~GeneticHealParentAction();

    virtual bool execute();
  };

  class GeneticDieAction: public GeneticAction {
  private:
    Bodypart* bp;
  public:
    GeneticDieAction(Bodypart* param_bodypart);
    virtual ~GeneticDieAction();

    virtual bool execute();
  };

  class GeneticGrowAction: public GeneticAction {
  protected:
    Bodypart* bp;
    float amount;
  public:
    GeneticGrowAction(Bodypart* param_bodypart, float param_amount);
    virtual ~GeneticGrowAction();

    virtual bool execute();
  };

  class GeneticShrinkAction: public GeneticAction {
  protected:
    Bodypart* bp;
    float amount;
  public:
    GeneticShrinkAction(Bodypart* param_bodypart, float param_amount);
    virtual ~GeneticShrinkAction();

    virtual bool execute();
  };

  class GeneticEmptyChemicalStorageAction: public GeneticAction {
  protected:
    Bodypart* bp;
  public:
    GeneticEmptyChemicalStorageAction(Bodypart* param_bodypart);
    virtual ~GeneticEmptyChemicalStorageAction();

    virtual bool execute();
  };

  class GeneticChangeMaxSizeAction: public GeneticAction {
  protected:
    Bodypart* bp;
    float amount;
  public:
    GeneticChangeMaxSizeAction(Bodypart* param_bodypart, float param_amount);
    virtual ~GeneticChangeMaxSizeAction();

    virtual bool execute();
  };

} // namespace

#endif