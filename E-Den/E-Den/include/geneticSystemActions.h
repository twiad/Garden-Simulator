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
    GeneticSimpleChemicalConvertAction(std::string nFromChemName,
                                       std::string nToChemName,
                                       float nAmount,
                                       float nRatio = 1.0f, 
                                       Bodypart* p_bp = 0);

    virtual ~GeneticSimpleChemicalConvertAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticChemicalConsumeAction: public GeneticAction {
  private:
    virtual void verify();
  protected:
    ChemicalStorage* storage;
    std::string chemName;
    float amount;
  public:
    GeneticChemicalConsumeAction(std::string nChemName, float nAmount, Bodypart* p_bp = 0);
    
    virtual ~GeneticChemicalConsumeAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticSpawnBodypartAction: public GeneticAction {
  private:
    Bodypart* parentBodypart;
    BodypartType childBodypartType;
  public:
    GeneticSpawnBodypartAction(BodypartType param_childBodypartType, Bodypart* param_parentBodypart = 0);
    virtual ~GeneticSpawnBodypartAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticChangeMaxChemicalAmountAction: public GeneticAction {
  friend class GeneticMaxAmountMutation;
  private:
    Bodypart* bp;
    float value;
    std::string chemName;
  public:
    GeneticChangeMaxChemicalAmountAction(std::string param_chemicalName, float param_value, Bodypart* param_bodypart = 0);

    virtual ~GeneticChangeMaxChemicalAmountAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticAddSpawnpointAction: public GeneticAction {
  friend class GeneticSpawnpoint2DAngleMutation;
  friend class GeneticSpawnpointActiveMutation;
  private:
    Bodypart* bp;
    SpawnpointInformation* sp;
    bool spawnpointAdded;
    bool active;
  public:
    GeneticAddSpawnpointAction(BodypartType param_bodypartType, int param_position, float param_ang2d, bool p_active = true, Bodypart* param_bodypart = 0);
    GeneticAddSpawnpointAction(std::list<BodypartType> param_bodypartTypes, int param_position, float param_ang2d, bool p_active = true, Bodypart* param_bodypart = 0);
    
    virtual ~GeneticAddSpawnpointAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticHurtAction: public GeneticAction {
  protected:
    Bodypart* bp;
    float amount;
  public:
    GeneticHurtAction(float param_amount, Bodypart* p_bp = 0);
    virtual ~GeneticHurtAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticSimpleMutateAction: public GeneticAction {
  protected:
    Bodypart* bp;
    float strength;
  public:
    GeneticSimpleMutateAction(Bodypart* p_bp = 0, float p_strength = 1.0f);
    virtual ~GeneticSimpleMutateAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticHealAction: public GeneticAction {
  protected:
    Bodypart* bp;
    float amount;
  public:
    GeneticHealAction(float param_amount, Bodypart* p_bp = 0);
    ~GeneticHealAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticHealParentAction: public GeneticAction {
  protected:
    Bodypart* bp;
    float amount;
  public:
    GeneticHealParentAction(float param_amount, Bodypart* p_bp = 0);
    ~GeneticHealParentAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticDieAction: public GeneticAction {
  private:
    Bodypart* bp;
  public:
    GeneticDieAction(Bodypart* param_bodypart = 0);
    virtual ~GeneticDieAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart = 0);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticGrowAction: public GeneticAction {
  protected:
    Bodypart* bp;
    float amount;
  public:
    GeneticGrowAction(float param_amount, Bodypart* p_bp = 0);
    virtual ~GeneticGrowAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart = 0);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticShrinkAction: public GeneticAction {
  protected:
    Bodypart* bp;
    float amount;
  public:
    GeneticShrinkAction(float param_amount, Bodypart* p_bp = 0);
    virtual ~GeneticShrinkAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticEmptyChemicalStorageAction: public GeneticAction {
  protected:
    Bodypart* bp;
  public:
    GeneticEmptyChemicalStorageAction(Bodypart* param_bodypart = 0);
    virtual ~GeneticEmptyChemicalStorageAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticChangeMaxSizeAction: public GeneticAction {
  friend class GeneticMaxSizeMutation;
  protected:
    Bodypart* bp;
    float amount;
  public:
    GeneticChangeMaxSizeAction(float param_amount, Bodypart* p_bp = 0);
    virtual ~GeneticChangeMaxSizeAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticDropSeedAction: public GeneticAction {
  protected:
    Bodypart* bp;
  public:
    GeneticDropSeedAction(Bodypart* p_bp = 0);
    virtual ~GeneticDropSeedAction();

    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticAction* copy();
    virtual TiXmlElement* toXmlElement();
  };

} // namespace

#endif