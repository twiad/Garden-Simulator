// geneticSystemConditions.h
// by Franz Koehler 2009

#include "geneticSystemCore.h"
#include "chemicalSystem.h"
#include "bodypart.h"
#include "globals.h"

#ifndef _E_DEN_CHEMICAL_SYSTEM_CONDITIONS_HEADER_
#define _E_DEN_CHEMICAL_SYSTEM_CONDITIONS_HEADER_

namespace EDen {
  class Bodypart;
  enum BodypartType;
  
  class GeneticChemicalCondition: public GeneticCondition {
  protected:
    ChemicalStorage* storage;
    GeneticChemicalConditionType chemCondType;
    float triggerValue;
    std::string chemName;
  public:
    GeneticChemicalCondition(ChemicalStorage* nstorage, 
                             GeneticChemicalConditionType ntype, 
                             float ntriggerValue, 
                             std::string nchemName): GeneticCondition(GCT_Chemical),
                                                     storage(nstorage),
                                                     chemCondType(ntype),
                                                     triggerValue(ntriggerValue),
                                                     chemName(nchemName) {};
    
    virtual ~GeneticChemicalCondition();

    virtual bool fullfilled();

  };

  class GeneticBodypartCreationCondition: public GeneticCondition {
  protected:  
    Bodypart* bp;
  public:
    GeneticBodypartCreationCondition(Bodypart* param_bodypart): GeneticCondition(GCT_BodypartCreation), bp(param_bodypart) {};
    virtual ~GeneticBodypartCreationCondition();

    virtual bool fullfilled();
  };
  
  class GeneticBodypartTypeCondition: public GeneticCondition {
  protected:
    Bodypart* bp;
    BodypartType bpType;
    GeneticStdConditionType condType;
  public:
    GeneticBodypartTypeCondition(Bodypart* bodypart, 
      BodypartType bodypartType, GeneticStdConditionType conditionType): 
          GeneticCondition(GCT_BodypartType), 
          bp(bodypart),
          bpType(bodypartType),
          condType(conditionType) {};
    virtual ~GeneticBodypartTypeCondition();

    BodypartType getBodypartType();

    virtual bool fullfilled();
  };

  class GeneticBodypartStateCondition: public GeneticCondition {
  protected:
    Bodypart* bp;
    BodypartState bpState;
    GeneticStdConditionType condType;
  public:
    GeneticBodypartStateCondition(Bodypart* bodypart, 
      BodypartState bodypartState, GeneticStdConditionType conditionType): 
          GeneticCondition(GCT_BodypartState), 
          bp(bodypart),
          bpState(bodypartState),
          condType(conditionType) {};
    virtual ~GeneticBodypartStateCondition();

    BodypartState getBodypartState();

    virtual bool fullfilled();
  };

  class GeneticSpawnpointPresentCondition: public GeneticCondition {
  protected:  
    Bodypart* bp;
    BodypartType bpType;
  public:
    GeneticSpawnpointPresentCondition(Bodypart* param_bodypart, BodypartType param_bpType): 
        GeneticCondition(GCT_SpawnpointPresent), bp(param_bodypart), bpType(param_bpType) {};
    virtual ~GeneticSpawnpointPresentCondition();

    virtual bool fullfilled();
  };

  class GeneticHealthpointsCondition: public GeneticCondition {
  protected:
    Bodypart* bp;
    float amount;
    GeneticStdConditionType condType;
  public:
    GeneticHealthpointsCondition(Bodypart* param_bodypart, GeneticStdConditionType param_condType, float param_amount);
    virtual ~GeneticHealthpointsCondition();

    virtual bool fullfilled();
  };

  class GeneticParentHealthpointsCondition: public GeneticCondition {
  protected:
    Bodypart* bp;
    float amount;
    GeneticStdConditionType condType;
  public:
    GeneticParentHealthpointsCondition(Bodypart* param_bodypart, GeneticStdConditionType param_condType, float param_amount);
    virtual ~GeneticParentHealthpointsCondition();

    virtual bool fullfilled();
  };

  class GeneticBodypartSizeCondition: public GeneticCondition { 
  protected:
    Bodypart* bp;
    float amount;
    GeneticStdConditionType condType;
  public:
    GeneticBodypartSizeCondition(Bodypart* param_bodypart, GeneticStdConditionType param_condType, float param_amount);
    virtual ~GeneticBodypartSizeCondition();

    virtual bool fullfilled();
  };

  class GeneticCanGrowCondition: public GeneticCondition {
  protected:
    Bodypart* bp;
  public:
    GeneticCanGrowCondition(Bodypart* param_bodypart);
    virtual ~GeneticCanGrowCondition();

    virtual bool fullfilled();
  };
} // namespace

#endif