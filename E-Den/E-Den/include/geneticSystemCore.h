// geneticSystemCore.h
// by Franz Koehler 2009

#ifndef _E_DEN_GENETIC_SYSTEM_HEADER_
#define _E_DEN_GENETIC_SYSTEM_HEADER_

#include <list>
#include "chemicalSystem.h"
#include "globals.h"

namespace EDen {
  
  class GeneticCondition {
  protected:
    GeneticConditionType condType;
  public:  
    GeneticCondition(GeneticConditionType conditionType = GCT_UNKNOWN): condType(conditionType) {};
    virtual ~GeneticCondition() {};

    GeneticConditionType getConditionType();
    virtual bool fullfilled() = 0;
    virtual bool setBodypart(Bodypart* param_bodypart) = 0;
    virtual bool dependsOnConditionType(GeneticConditionType param_conditionType);
  };

  class GeneticCompoundCondition: public GeneticCondition {
  protected:
    typedef std::list<GeneticCondition*> GeneticConditionsList;
    typedef std::list<GeneticCondition*>::iterator GeneticConditionsListIterator;
    GeneticConditionsList childConditions;
  public:
    GeneticCompoundCondition(): GeneticCondition(GCT_Compound) {};
    virtual ~GeneticCompoundCondition();

    bool add(GeneticCondition* newCondition);
    
    virtual bool fullfilled() = 0;
    virtual bool setBodypart(Bodypart* param_bodypart);
    virtual bool dependsOnConditionType(GeneticConditionType param_conditionType) = 0;
  };

  class GeneticANDCondition: public GeneticCompoundCondition {
  public:
    GeneticANDCondition(): GeneticCompoundCondition() {};
    virtual ~GeneticANDCondition();

    virtual bool fullfilled();
    virtual bool dependsOnConditionType(GeneticConditionType param_conditionType);
  };

  class GeneticORCondition: public GeneticCompoundCondition {
  public:
    GeneticORCondition(): GeneticCompoundCondition() {};
    virtual ~GeneticORCondition();

    virtual bool fullfilled();
    virtual bool dependsOnConditionType(GeneticConditionType param_conditionType);
  };

  class GeneticAction {
  protected:
    GeneticActionType actType;

  public:
    GeneticAction(GeneticActionType param_actType = GAT_UNKNOWN): actType(param_actType) {};
    virtual ~GeneticAction() {};

    GeneticActionType getActionType();

    virtual bool execute() = 0;
    virtual bool setBodypart(Bodypart* param_bodypart) = 0;
  };

  class GeneticCompoundAction: public GeneticAction {
  protected:
    typedef std::list<GeneticAction*> GeneticActionsList;
    typedef std::list<GeneticAction*>::iterator GeneticActionsListIterator;

    GeneticActionsList childActions;
  public:
    GeneticCompoundAction(): GeneticAction(GAT_Compound) {};
    virtual ~GeneticCompoundAction();

    bool add(GeneticAction* newAction);
    virtual bool execute();
    virtual bool setBodypart(Bodypart* param_bodypart);
  };

  class GeneticClause {
    friend class GeneticProcessor;
  public:
    GeneticCondition* cond;
    GeneticAction* act;
    std::string description;

    bool deleteMe;

    GeneticClause(GeneticCondition* condition, GeneticAction* action, std::string param_description = ""): cond(condition), act(action), description(param_description), deleteMe(false) {};
    virtual ~GeneticClause();

    bool run();
    bool setBodypart(Bodypart* param_bodypart);
  };
} // namespace

#endif