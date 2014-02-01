// geneticSystem.cpp
// by Franz Köhler 2009

#include "globals.h"
#include "geneticSystemCore.h"
#include "geneticSystemActions.h"
#include "geneticSystemConditions.h"

namespace EDen {
  GeneticCondition* GeneticCondition::parseXmlElement(TiXmlElement* description) {
    std::string type = description->ValueStr();
    if(type == "ConditionOR") return new GeneticORCondition(description);
    else if(type == "ConditionAND") return new GeneticANDCondition(description);
    else if(type == "ChemicalCondition") return new GeneticChemicalCondition(description);
    else if(type == "BodypartTypeCondition") return new GeneticBodypartTypeCondition(description);
    else if(type == "BodypartStateCondition") return new GeneticBodypartStateCondition(description);
    else if(type == "HealthpointsCondition") return new GeneticHealthpointsCondition(description);
    else if(type == "ParentHealthpointsCondition") return new GeneticParentHealthpointsCondition(description);
    else if(type == "OrganismLifetimeCondition") return new GeneticParentOrganismLifetimeCondition(description);
    else if(type == "SizeCondition") return new GeneticBodypartSizeCondition(description);
    else if(type == "CreationCondition") return new GeneticBodypartCreationCondition(description);
    else if(type == "SpawnpointPresentCondition") return new GeneticSpawnpointPresentCondition(description);
    else if(type == "CanGrowCondition") return new GeneticCanGrowCondition(description);
    else if(type == "HasParentCondition") return new GeneticHasParentCondition(description);
    else return 0;
  };

  GeneticAction* GeneticAction::parseXmlElement(TiXmlElement* description) {
    std::string type = description->ValueStr();
    if(type == "CompoundAction") return new GeneticCompoundAction(description);
    else if(type == "ChemicalConvertAction") return new GeneticSimpleChemicalConvertAction(description);
    else if(type == "RegularChemicalConsumeAction") return new GeneticRegularChemicalConsumeAction(description);
    else if(type == "ChemicalConsumeAction") return new GeneticChemicalConsumeAction(description);
    else if(type == "SetColorAction") return new GeneticSetColorAction(description);
    else if(type == "SpawnBodypartAction") return new GeneticSpawnBodypartAction(description);
    else if(type == "ChangeMaxChemicalAmountAction") return new GeneticChangeMaxChemicalAmountAction(description);
    else if(type == "AddSpawnpointAction") return new GeneticAddSpawnpointAction(description);
    else if(type == "GrowAction") return new GeneticGrowAction(description);
    else if(type == "ShrinkAction") return new GeneticShrinkAction(description);
    else if(type == "HurtAction") return new GeneticHurtAction(description);
    else if(type == "MutateAction") return new GeneticSimpleMutateAction(description);
    else if(type == "HealParentAction") return new GeneticHealParentAction(description);
    else if(type == "HealAction") return new GeneticHealAction(description);
    else if(type == "DieAction") return new GeneticDieAction(description);
    else if(type == "EmptyChemicalStorageAction") return new GeneticEmptyChemicalStorageAction(description);
    else if(type == "ChangeMaxSizeAction") return new GeneticChangeMaxSizeAction(description);
    else if(type == "DropSeedAction") return new GeneticDropSeedAction(description);
	else if(type == "DropLeafAction") return new GeneticDropLeafAction(description);
    else if(type == "SetColorAction") return new GeneticSetColorAction(description);
    else return 0;
  };
};