// geneticSystemConditions.h
// by Franz Koehler 2009

#include "geneticSystemCore.h"
#include "chemicalSystem.h"
#include "bodypart.h"
#include "globals.h"
#include "tinyxml.h"

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
    GeneticChemicalCondition(GeneticChemicalConditionType ntype, 
                             float ntriggerValue, 
                             std::string nchemName,
                             Bodypart * param_bp = 0);
    
    virtual ~GeneticChemicalCondition();

    virtual bool fullfilled();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticCondition* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticBodypartCreationCondition: public GeneticCondition {
  protected:  
    Bodypart* bp;
  public:
    GeneticBodypartCreationCondition(Bodypart* param_bodypart = 0): GeneticCondition(GCT_BodypartCreation), bp(param_bodypart) {};
    virtual ~GeneticBodypartCreationCondition();

    virtual bool fullfilled();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticCondition* copy();
    virtual TiXmlElement* toXmlElement();
  };
  
  class GeneticBodypartTypeCondition: public GeneticCondition {
  protected:
    Bodypart* bp;
    BodypartType bpType;
    GeneticStdConditionType condType;
  public:
    GeneticBodypartTypeCondition( 
      BodypartType bodypartType, GeneticStdConditionType conditionType, Bodypart * param_bp = 0): 
          GeneticCondition(GCT_BodypartType), 
          bp(param_bp),
          bpType(bodypartType),
          condType(conditionType) {};
    virtual ~GeneticBodypartTypeCondition();

    BodypartType getBodypartType();

    virtual bool fullfilled();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticCondition* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticBodypartStateCondition: public GeneticCondition {
  protected:
    Bodypart* bp;
    BodypartState bpState;
    GeneticStdConditionType condType;
  public:
    GeneticBodypartStateCondition( 
      BodypartState bodypartState, GeneticStdConditionType conditionType, Bodypart* bodypart = 0): 
          GeneticCondition(GCT_BodypartState), 
          bp(bodypart),
          bpState(bodypartState),
          condType(conditionType) {};
    virtual ~GeneticBodypartStateCondition();

    BodypartState getBodypartState();

    virtual bool fullfilled();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticCondition* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticSpawnpointPresentCondition: public GeneticCondition {
  protected:  
    Bodypart* bp;
    BodypartType bpType;
  public:
    GeneticSpawnpointPresentCondition(BodypartType param_bpType, Bodypart* param_bodypart = 0): 
        GeneticCondition(GCT_SpawnpointPresent), bp(param_bodypart), bpType(param_bpType) {};
    virtual ~GeneticSpawnpointPresentCondition();

    virtual bool fullfilled();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticCondition* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticHealthpointsCondition: public GeneticCondition {
  protected:
    Bodypart* bp;
    float amount;
    GeneticStdConditionType condType;
  public:
    GeneticHealthpointsCondition(GeneticStdConditionType param_condType, float param_amount, Bodypart* param_bodypart = 0);
    virtual ~GeneticHealthpointsCondition();

    virtual bool fullfilled();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticCondition* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticParentHealthpointsCondition: public GeneticCondition {
  protected:
    Bodypart* bp;
    float amount;
    GeneticStdConditionType condType;
  public:
    GeneticParentHealthpointsCondition(GeneticStdConditionType param_condType, float param_amount, Bodypart* param_bodypart = 0);
    virtual ~GeneticParentHealthpointsCondition();

    virtual bool fullfilled();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticCondition* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticParentOrganismLifetimeCondition: public GeneticCondition {
  protected:
    Bodypart* bp;
    int amount;
    GeneticStdConditionType condType;
  public:
    GeneticParentOrganismLifetimeCondition(GeneticStdConditionType param_condType, int param_amount, Bodypart* param_bodypart = 0);
    virtual ~GeneticParentOrganismLifetimeCondition();

    virtual bool fullfilled();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticCondition* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticBodypartSizeCondition: public GeneticCondition { 
  protected:
    Bodypart* bp;
    float amount;
    GeneticStdConditionType condType;
  public:
    GeneticBodypartSizeCondition(GeneticStdConditionType param_condType, float param_amount, Bodypart* param_bodypart = 0);
    virtual ~GeneticBodypartSizeCondition();

    virtual bool fullfilled();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticCondition* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticCanGrowCondition: public GeneticCondition {
  protected:
    Bodypart* bp;
  public:
    GeneticCanGrowCondition(Bodypart* param_bodypart = 0);
    virtual ~GeneticCanGrowCondition();

    virtual bool fullfilled();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticCondition* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticHasParentCondition: public GeneticCondition {
  protected:
    Bodypart* bp;
  public:
    GeneticHasParentCondition(Bodypart* param_bodypart = 0);
    virtual ~GeneticHasParentCondition();

    virtual bool fullfilled();
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual GeneticCondition* copy();
    virtual TiXmlElement* toXmlElement();
  };
} // namespace

#endif