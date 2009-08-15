// geneticProcessor.cpp
// by Franz Koehler 2009

#include "geneticProcessor.h"

namespace EDen {
  GeneticProcessor::GeneticProcessor(Bodypart* nBodypart){
    bodypart = nBodypart;
    chemStorage = bodypart->getChemicalStorage();
  };

  GeneticProcessor::~GeneticProcessor() {
    GeneticClause* clause;
    while(!relevantClauses.empty()) {
      clause = relevantClauses.back();
      relevantClauses.pop_back();
      delete clause;
    };
    
    // relevantClauses.clear();
  };

  bool GeneticProcessor::addClause(GeneticClause* newClause) {
    relevantClauses.push_back(newClause);
    return true;
  };

  //bool GeneticProcessor::removeClauseFromRelevantClauses(GeneticClause* clause) {
  //  relevantClauses.remove(clause);
  //  return true; 
  //};

  Bodypart* GeneticProcessor::getBodypart() {
    return bodypart; 
  };

  bool GeneticProcessor::executeRelevantClauses() {
    if(bodypart->getBodypartState() != BSP_dead) {  
      GeneticClauseList nextRelevantClauses;
      
      while(!relevantClauses.empty()) {
        relevantClauses.back()->run();
        if(!relevantClauses.back()->deleteMe)
          nextRelevantClauses.push_front(relevantClauses.back());
        else 
          delete relevantClauses.back();

        relevantClauses.pop_back();
      }

      relevantClauses.clear(); //pointer ... hmm .. who deletes them?
      relevantClauses.swap(nextRelevantClauses);

      return true;
    }
    else { // BP is dead :_(
      if(bodypart->getChildBodyparts().empty())
        bodypart->getParentOrganism()->deleteBodypart(bodypart);
      return false; 
    }
  };

  bool GeneticProcessor::addDummyGenecode() {
    std::string code = bodypart->getGeneticCode();

    if(code == "TESTPART1") {
      GeneticANDCondition* andCond = new GeneticANDCondition();
      andCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,0.05f,"Wasser"));
      andCond->add(new GeneticChemicalCondition(chemStorage,GCC_space_left_more,0.005f,"Gold"));
      
      GeneticCompoundAction* compAct = new GeneticCompoundAction();
      compAct->add(new GeneticSimpleChemicalConvertAction(chemStorage,"Wasser","Gold",0.05f,0.1f));

      addClause(new GeneticClause(
        // Conditions
        andCond
        ,
        // Actions
        compAct
        ,
        "Water to Gold"        
        ));
    }
    else if(code == "TESTPART2") {
      GeneticANDCondition* andCond = new GeneticANDCondition();
      andCond->add(new GeneticChemicalCondition(chemStorage,GCC_space_left_more,0.1f,"BLAK"));
      andCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,0.01f,"Gold"));
      
      GeneticCompoundAction* compAct = new GeneticCompoundAction();
      compAct->add(new GeneticSimpleChemicalConvertAction(chemStorage,"Gold","BLAK",0.01f,10.0f));

      addClause(new GeneticClause(
        // Conditions
        andCond
        ,
        // Actions
        compAct
        , "Gold to BLAK"
        ));
    }
    else if(code == "TESTPART3") {
      bool unfullfilledBPTConditionFound = false;
      GeneticANDCondition* gAndCond = 0;
      GeneticORCondition* gOrCond = 0;
      GeneticCompoundAction* compAct = 0;
      GeneticCondition* cond = 0;
      
      // Strick and Branch BPTList for creation of Multiple Choice Spawnpoints
      std::list<BodypartType> bpts;
      bpts.push_back(BPT_Branch);
      bpts.push_back(BPT_Stick);

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF Type = Leaf
      //  AND Creation
      // THEN
      //  Add Stick OR Branch Spawnpoint to position 0 at 180°
      //  Change maximum amount of "Wasser" to 15
      //  Change maximum amount of "Energie" to 150
      //  Change maximum size to 2.0f + (100.0f - parent "Energie" percentage) * 0.05
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();

      cond = new GeneticBodypartTypeCondition(bodypart, BPT_Leaf, GBT_equal);
      unfullfilledBPTConditionFound = !(cond->fullfilled());
      gAndCond->add(cond);
      gAndCond->add(new GeneticBodypartCreationCondition(bodypart));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 0, 180.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Wasser",15.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Energie",150.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Sonne",120.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Goo", 10.0f));
      
      float pbp_water_percentage = 0.0f;
      Bodypart* parent_bodypart = bodypart->getParentBodypart();
      if (parent_bodypart) {
        pbp_water_percentage = parent_bodypart->getChemicalStorage()->getCurrentPercentage("Energie");
      };

      compAct->add(new GeneticChangeMaxSizeAction(bodypart,1.0f + ((100.0f - pbp_water_percentage) * 0.015f)));
      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Leaf Creation"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF Type = Stick
      //  AND Creation
      // THEN
      //  Add Stick OR Branch Spawnpoint to position 0 at 180.0°
      //  Add Stick OR Branch Spawnpoint to position 1 at 0.0°
      //  Add Leaf Spawnpoint to position 2
      //  Add Leaf Spawnpoint to position 3
      //  Add some more leafs ;)
      //  Change maximum amount of "Wasser" to 100
      //  Change maximum amount of "Energie" to 15
      //  Change maximum size to 5.0 + parent water percantage * 50
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();

      cond = new GeneticBodypartTypeCondition(bodypart, BPT_Stick, GBT_equal);
      unfullfilledBPTConditionFound = !(cond->fullfilled());
      gAndCond->add(cond);
      gAndCond->add(new GeneticBodypartCreationCondition(bodypart));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 0, 180.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 1, 0.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, BPT_Leaf, 2, 155.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, BPT_Leaf, 3, -155.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, BPT_Leaf, 4, 125.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, BPT_Leaf, 5, -125.0f));
      //compAct->add(new GeneticAddSpawnpointAction(bodypart, BPT_Leaf, 6));
      //compAct->add(new GeneticAddSpawnpointAction(bodypart, BPT_Leaf, 7));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Wasser", 100.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Energie", 15.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Sonne", 0.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Goo", 10.0f));
      
      pbp_water_percentage = 0.0f;
      parent_bodypart = bodypart->getParentBodypart();
      if (parent_bodypart) {
        pbp_water_percentage = parent_bodypart->getChemicalStorage()->getCurrentPercentage("Wasser");
      };

      compAct->add(new GeneticChangeMaxSizeAction(bodypart,5.0f + (pbp_water_percentage * 50)));

      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Stick Creation"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF Type = Branch
      //  AND Creation
      // THEN
      //  Add Stick OR Branch Spawnpoint to position 0 at 180.0°
      //  Add Stick OR Branch Spawnpoint to position 1 at -13.0 °
      //  Add Stick OR Branch Spawnpoint to position 2 at 13.0°
      //  Change maximum amount of "Wasser" to 150
      //  Change maximum amount of "Energie" to 10
      //  Change maximum size to 10.0 + parent water percentage * 100
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();

      cond = new GeneticBodypartTypeCondition(bodypart, BPT_Branch, GBT_equal);
      unfullfilledBPTConditionFound = !(cond->fullfilled());
      gAndCond->add(cond);
      gAndCond->add(new GeneticBodypartCreationCondition(bodypart));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 0, 180.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 1, -13.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 2, 13.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Wasser",150.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Energie",10.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Sonne", 0.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Goo", 3.0f));
      
      pbp_water_percentage = 0.0f;
      parent_bodypart = bodypart->getParentBodypart();
      if (parent_bodypart) {
        pbp_water_percentage = parent_bodypart->getChemicalStorage()->getCurrentPercentage("Wasser");
      };

      compAct->add(new GeneticChangeMaxSizeAction(bodypart,10.0f + (pbp_water_percentage * 100)));

      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Branch Creation"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ////////////////////////// Creation Rules End Here ////////////////////

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND Type = Leaf
      //  AND "Energie" space_left_more than 10.0
      //  AND "Wasser" current_value_more than 15.0
      //  AND "Sonne" current_value_more than 5.0
      // THEN
      //  Consume 5.0 "Sonne"
      //  Convert 10.0 "Wasser" to 10.0 "Energie"
      //  Convert 0.1 "Wasser" to 5.0 "Goo"
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
      
      cond = new GeneticBodypartTypeCondition(bodypart, BPT_Leaf, GBT_equal);
      unfullfilledBPTConditionFound = !(cond->fullfilled());

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(cond);
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_space_left_more,10.0f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,15.0f,"Wasser"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,1.0f,"Sonne"));

      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Sonne",1.0f));
      compAct->add(new GeneticSimpleChemicalConvertAction(chemStorage,"Wasser","Energie",10.0f,2.0f));
      compAct->add(new GeneticSimpleChemicalConvertAction(chemStorage,"Wasser","Goo",0.1f,50.0f));

      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Leaf Energie Production"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      // THEN
      //  Consume 0.0013 "Energie"
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
      
      unfullfilledBPTConditionFound = false;
      
      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Energie",0.0013f));


      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Energie consumption"));
      else {
        delete gAndCond;
        delete compAct;
      };

            
      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND CanGrow
      //  #AND has more than 25.0% energy
      //  AND has more than 2.1 energy
      // THEN
      //  Consume 0.500 "Energie"
      //  Grow 0.1
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
      
      unfullfilledBPTConditionFound = false;
      
      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(new GeneticCanGrowCondition(bodypart));
      //gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_percentage_more,25.0f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,1.375f,"Energie"));
      
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Energie",0.50f));
      compAct->add(new GeneticGrowAction(bodypart,0.1f));


      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Energie consumption and grow"));
      else {
        delete gAndCond;
        delete compAct;
      };      
      
      


      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  Type = Stick
      //  AND "Wasser" current_value_more than 10.00
      //  AND "Energie" current_value_more than 2.0
      //  AND "Energie" space_left_more than 4.0
      //  AND Spawnpoint Leaf present
      // THEN
      //  Spawn Leaf
      //  Consume "Energie" 2.0
      //  Consume "Wasser" 10.0
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
      
      cond = new GeneticBodypartTypeCondition(bodypart, BPT_Stick, GBT_equal);
      unfullfilledBPTConditionFound = !(cond->fullfilled());

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(cond);
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,10.0f,"Wasser"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,2.0f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_space_left_more,4.0f,"Energie"));
      gAndCond->add(new GeneticSpawnpointPresentCondition(bodypart,BPT_Leaf));

      compAct->add(new GeneticSpawnBodypartAction(bodypart,BPT_Leaf));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Energie",2.0f));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Wasser",10.0f));

      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Spawn Leaf"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  Type = Stick OR Branch
      //  AND Spawnpoint Stick present
      //  AND Size > 1
      //  AND "Energie" current_value_more than 0.01
      //  AND "Wasser" current_value_more than 20.0
      // THEN
      //  Spawn Stick
      //  Consume 0.01 "Energie"
      //  Consume 20.0 "Wasser"
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      gOrCond = new GeneticORCondition();
      compAct = new GeneticCompoundAction();
      
      gOrCond->add(new GeneticBodypartTypeCondition(bodypart, BPT_Stick, GBT_equal));
      gOrCond->add(new GeneticBodypartTypeCondition(bodypart, BPT_Branch, GBT_equal));
        
      unfullfilledBPTConditionFound = !(gOrCond->fullfilled());

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(gOrCond);
      gAndCond->add(new GeneticSpawnpointPresentCondition(bodypart,BPT_Stick));
      gAndCond->add(new GeneticBodypartSizeCondition(bodypart,GBT_more,1.0f));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,0.01f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,20.0f,"Wasser"));
      

      compAct->add(new GeneticSpawnBodypartAction(bodypart,BPT_Stick));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Energie",0.01f));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Wasser",20.0f));

      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Spawn Stick"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  Type = Stick OR Branch
      //  Size > 1.5
      //  AND "Energie" current_value_more than 0.30
      //  AND "Wasser" current_value_more than 0.30
      //  AND Spawnpoint Branch present
      // THEN
      //  Spawn Branch
      //  Consume 0.3 "Energie"
      //  Consume 0.3 "Wasser"
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      gOrCond = new GeneticORCondition();
      compAct = new GeneticCompoundAction();
      
      gOrCond->add(new GeneticBodypartTypeCondition(bodypart, BPT_Stick, GBT_equal));
      gOrCond->add(new GeneticBodypartTypeCondition(bodypart, BPT_Branch, GBT_equal));
        
      unfullfilledBPTConditionFound = !(gOrCond->fullfilled());

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(gOrCond);
      gAndCond->add(new GeneticSpawnpointPresentCondition(bodypart,BPT_Branch));
      gAndCond->add(new GeneticBodypartSizeCondition(bodypart,GBT_more,1.5f));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,0.3f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,0.3f,"Wasser"));
      
      compAct->add(new GeneticSpawnBodypartAction(bodypart,BPT_Branch));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Energie",0.1f));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Wasser",0.1f));

      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Spawn Branche"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND "Energie" percentage_less than 0.0001
      // THEN
      //  Hurt 1.0
      //  Shrink 0.97
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
        
      unfullfilledBPTConditionFound = false;

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_percentage_less,0.0001f,"Energie"));
      
      compAct->add(new GeneticHurtAction(bodypart,1.0f));
      compAct->add(new GeneticShrinkAction(bodypart,0.97f));
      
      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "lose HP on low Energie"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND Healthpoints less_than 0.01
      // THEN
      //  EmptyChemicalStorage
      //  Die
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
        
      unfullfilledBPTConditionFound = false;

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(new GeneticHealthpointsCondition(bodypart,GBT_less,0.01f));
      
      compAct->add(new GeneticEmptyChemicalStorageAction(bodypart));
      compAct->add(new GeneticDieAction(bodypart));

      
      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Die if not enough HP"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND Healthpoints less_than 100.0
      //  AND "Energie" current_value_more than 1.5
      // THEN
      //  Heal 1.0
      //  Grow 1.0
      //  Consume Energy 1.0
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
        
      unfullfilledBPTConditionFound = false;

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(new GeneticHealthpointsCondition(bodypart,GBT_less,100.0f));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,1.5f,"Energie"));
      
      compAct->add(new GeneticHealAction(bodypart,1.0f));
      compAct->add(new GeneticGrowAction(bodypart,1.0f));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Energie",1.0f));
      
      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Convert Energie to HP"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND Parent_Healthpoints less_than 100.0
      // THEN
      //  HealParent 1.0
      //  Hurt 1.0
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
        
      unfullfilledBPTConditionFound = false;

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(new GeneticParentHealthpointsCondition(bodypart,GBT_less,100.0f));
      
      compAct->add(new GeneticHealParentAction(bodypart,1.0f));
      compAct->add(new GeneticHurtAction(bodypart,1.0f));
      
      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Heal Parent"));
      else {
        delete gAndCond;
        delete compAct;
      };

    }
    else if(code == "TESTPART4") {
      bool unfullfilledBPTConditionFound = false;
      GeneticANDCondition* gAndCond = 0;
      GeneticORCondition* gOrCond = 0;
      GeneticCompoundAction* compAct = 0;
      GeneticCondition* cond = 0;
      
      // Strick and Branch BPTList for creation of Multiple Choice Spawnpoints
      std::list<BodypartType> bpts;
      bpts.push_back(BPT_Branch);
      bpts.push_back(BPT_Stick);

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF Type = Leaf
      //  AND Creation
      // THEN
      //  Add Stick OR Branch Spawnpoint to position 0 at 180°
      //  Change maximum amount of "Wasser" to 15
      //  Change maximum amount of "Energie" to 150
      //  Change maximum amount of "Goo" to 3
      //  Change maximum size to 1.0
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();

      cond = new GeneticBodypartTypeCondition(bodypart, BPT_Leaf, GBT_equal);
      unfullfilledBPTConditionFound = !(cond->fullfilled());
      gAndCond->add(cond);
      gAndCond->add(new GeneticBodypartCreationCondition(bodypart));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 0, 180.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Wasser",15.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Energie",150.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Goo",3.0f));
      
      float pbp_water_percentage = 0.0f;
      Bodypart* parent_bodypart = bodypart->getParentBodypart();
      if (parent_bodypart) {
        pbp_water_percentage = parent_bodypart->getChemicalStorage()->getCurrentPercentage("Energie");
      };

      compAct->add(new GeneticChangeMaxSizeAction(bodypart,2.5f));
      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Leaf Creation"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF Type = Stick
      //  AND Creation
      // THEN
      //  Add Stick OR Branch Spawnpoint to position 0 at 180.0°
      //  Add Stick OR Branch Spawnpoint to position 1 at 3.25°
      //  Add Leaf Spawnpoint to position 2
      //  Add Leaf Spawnpoint to position 3
      //  Add some more leafs ;)
      //  Add Stick OR Branch Spawnpoint to position 6 at -3.25°
      //  Change maximum amount of "Wasser" to 100
      //  Change maximum amount of "Energie" to 15
      //  Change maximum amount of "Goo" to 5
      //  Change maximum size to 10.0
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();

      cond = new GeneticBodypartTypeCondition(bodypart, BPT_Stick, GBT_equal);
      unfullfilledBPTConditionFound = !(cond->fullfilled());
      gAndCond->add(cond);
      gAndCond->add(new GeneticBodypartCreationCondition(bodypart));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 0, 180.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 1, 3.25f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, BPT_Leaf, 2, 165.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, BPT_Leaf, 3, -165.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, BPT_Leaf, 4, 175.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, BPT_Leaf, 5, -175.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 6, -3.25f));
      //compAct->add(new GeneticAddSpawnpointAction(bodypart, BPT_Leaf, 6));
      //compAct->add(new GeneticAddSpawnpointAction(bodypart, BPT_Leaf, 7));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Wasser", 100.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Energie", 15.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Goo", 5.0f));
      
      pbp_water_percentage = 0.0f;
      parent_bodypart = bodypart->getParentBodypart();
      if (parent_bodypart) {
        pbp_water_percentage = parent_bodypart->getChemicalStorage()->getCurrentPercentage("Wasser");
      };

      compAct->add(new GeneticChangeMaxSizeAction(bodypart,10.0f));

      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Stick Creation"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF Type = Branch
      //  AND Creation
      // THEN
      //  Add Stick OR Branch Spawnpoint to position 0 at 180.0°
      //  Add Stick OR Branch Spawnpoint to position 1 at -9.0 °
      //  Add Stick OR Branch Spawnpoint to position 2 at 9.0°
      //  Add Stick OR Branch Spawnpoint to position 1 at -18.0 °
      //  Add Stick OR Branch Spawnpoint to position 2 at 18.0°
      //  Change maximum amount of "Wasser" to 75
      //  Change maximum amount of "Energie" to 10
      //  Change maximum amount of "Goo" to 50
      //  Change maximum size to 5.0
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();

      cond = new GeneticBodypartTypeCondition(bodypart, BPT_Branch, GBT_equal);
      unfullfilledBPTConditionFound = !(cond->fullfilled());
      gAndCond->add(cond);
      gAndCond->add(new GeneticBodypartCreationCondition(bodypart));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 0, 180.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 1, -9.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 2, 9.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 3, -18.0f));
      compAct->add(new GeneticAddSpawnpointAction(bodypart, bpts, 4, 18.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Wasser",75.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Energie",15.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction(bodypart,"Goo",50.0f));
      
      pbp_water_percentage = 0.0f;
      parent_bodypart = bodypart->getParentBodypart();
      if (parent_bodypart) {
        pbp_water_percentage = parent_bodypart->getChemicalStorage()->getCurrentPercentage("Wasser");
      };

      compAct->add(new GeneticChangeMaxSizeAction(bodypart,5.0f));

      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Branch Creation"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ////////////////////////// Creation Rules End Here ////////////////////

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND Type = Leaf
      //  AND "Energie" space_left_more than 10.0
      //  AND "Wasser" current_value_more than 15.0
      //  AND "Goo" current_value_more than 0.5
      // THEN
      //  Consume 1.0 "Goo"
      //  Convert 1.0 "Wasser" to 10.0 "Energie"
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
      
      cond = new GeneticBodypartTypeCondition(bodypart, BPT_Leaf, GBT_equal);
      unfullfilledBPTConditionFound = !(cond->fullfilled());

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(cond);
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_space_left_more,10.0f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,15.0f,"Wasser"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,1.0f,"Goo"));

      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Goo",1.0f));
      compAct->add(new GeneticSimpleChemicalConvertAction(chemStorage,"Wasser","Energie",1.0f,10.0f));

      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Leaf Energie Production"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      // THEN
      //  Consume 0.0013 "Energie"
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
      
      unfullfilledBPTConditionFound = false;
      
      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Energie",0.0013f));


      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Energie consumption"));
      else {
        delete gAndCond;
        delete compAct;
      };

            
      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND CanGrow
      //  #AND has more than 25.0% energy
      //  AND has more than 2.1 energy
      // THEN
      //  Consume 0.500 "Energie"
      //  Grow 0.1
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
      
      unfullfilledBPTConditionFound = false;
      
      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(new GeneticCanGrowCondition(bodypart));
      //gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_percentage_more,25.0f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,1.375f,"Energie"));
      
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Energie",0.50f));
      compAct->add(new GeneticGrowAction(bodypart,0.1f));


      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Energie consumption and grow"));
      else {
        delete gAndCond;
        delete compAct;
      };      
      
      


      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  Type = Stick
      //  AND "Wasser" current_value_more than 10.00
      //  AND "Energie" current_value_more than 2.0
      //  AND Spawnpoint Leaf present
      // THEN
      //  Spawn Leaf
      //  Consume "Energie" 2.0
      //  Consume "Wasser" 10.0
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
      
      cond = new GeneticBodypartTypeCondition(bodypart, BPT_Stick, GBT_equal);
      unfullfilledBPTConditionFound = !(cond->fullfilled());

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(cond);
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,10.0f,"Wasser"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,2.0f,"Energie"));

      gAndCond->add(new GeneticSpawnpointPresentCondition(bodypart,BPT_Leaf));

      compAct->add(new GeneticSpawnBodypartAction(bodypart,BPT_Leaf));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Energie",2.0f));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Wasser",10.0f));

      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Spawn Leaf"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  Type = Stick OR Branch
      //  AND Spawnpoint Stick present
      //  AND Size > 1
      //  AND "Energie" current_value_more than 0.01
      //  AND "Wasser" current_value_more than 20.0
      //  AND "Energie" percentage less than 85%
      // THEN
      //  Spawn Stick
      //  Consume 0.01 "Energie"
      //  Consume 20.0 "Wasser"
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      gOrCond = new GeneticORCondition();
      compAct = new GeneticCompoundAction();
      
      gOrCond->add(new GeneticBodypartTypeCondition(bodypart, BPT_Stick, GBT_equal));
      gOrCond->add(new GeneticBodypartTypeCondition(bodypart, BPT_Branch, GBT_equal));
        
      unfullfilledBPTConditionFound = !(gOrCond->fullfilled());

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(gOrCond);
      gAndCond->add(new GeneticSpawnpointPresentCondition(bodypart,BPT_Stick));
      gAndCond->add(new GeneticBodypartSizeCondition(bodypart,GBT_more,1.0f));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,0.01f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,20.0f,"Wasser"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_percentage_less,15.0f,"Energie"));
      

      compAct->add(new GeneticSpawnBodypartAction(bodypart,BPT_Stick));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Energie",0.01f));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Wasser",20.0f));

      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Spawn Stick"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  Type = Stick OR Branch
      //  Size > 1.5
      //  AND "Energie" current_value_more than 0.30
      //  AND "Wasser" current_value_more than 0.30
      //  AND Spawnpoint Branch present
      //  AND "Energy" percentage more than 7%
      // THEN
      //  Spawn Branch
      //  Consume 0.3 "Energie"
      //  Consume 0.3 "Wasser"
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      gOrCond = new GeneticORCondition();
      compAct = new GeneticCompoundAction();
      
      gOrCond->add(new GeneticBodypartTypeCondition(bodypart, BPT_Stick, GBT_equal));
      gOrCond->add(new GeneticBodypartTypeCondition(bodypart, BPT_Branch, GBT_equal));
        
      unfullfilledBPTConditionFound = !(gOrCond->fullfilled());

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(gOrCond);
      gAndCond->add(new GeneticSpawnpointPresentCondition(bodypart,BPT_Branch));
      gAndCond->add(new GeneticBodypartSizeCondition(bodypart,GBT_more,1.5f));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,0.3f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,0.3f,"Wasser"));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_percentage_more,1.5f,"Energie"));

      compAct->add(new GeneticSpawnBodypartAction(bodypart,BPT_Branch));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Energie",0.1f));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Wasser",0.1f));

      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Spawn Branche"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND "Energie" percentage_less than 0.0001
      // THEN
      //  Hurt 1.0
      //  Shrink 0.97
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
        
      unfullfilledBPTConditionFound = false;

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_percentage_less,0.0001f,"Energie"));
      
      compAct->add(new GeneticHurtAction(bodypart,1.0f));
      compAct->add(new GeneticShrinkAction(bodypart,0.97f));
      
      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "lose HP on low Energie"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND Healthpoints less_than 0.01
      // THEN
      //  EmptyChemicalStorage
      //  Die
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
        
      unfullfilledBPTConditionFound = false;

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(new GeneticHealthpointsCondition(bodypart,GBT_less,0.01f));
      
      compAct->add(new GeneticEmptyChemicalStorageAction(bodypart));
      compAct->add(new GeneticDieAction(bodypart));

      
      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Die if not enough HP"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND Healthpoints less_than 100.0
      //  AND "Energie" current_value_more than 1.5
      // THEN
      //  Heal 1.0
      //  Grow 1.0
      //  Consume Energy 1.0
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
        
      unfullfilledBPTConditionFound = false;

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(new GeneticHealthpointsCondition(bodypart,GBT_less,100.0f));
      gAndCond->add(new GeneticChemicalCondition(chemStorage,GCC_current_value_more,1.5f,"Energie"));
      
      compAct->add(new GeneticHealAction(bodypart,1.0f));
      compAct->add(new GeneticGrowAction(bodypart,1.0f));
      compAct->add(new GeneticChemicalConsumeAction(chemStorage,"Energie",1.0f));
      
      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Convert Energie to HP"));
      else {
        delete gAndCond;
        delete compAct;
      };

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND Parent_Healthpoints less_than 100.0
      // THEN
      //  HealParent 1.0
      //  Hurt 1.0
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
        
      unfullfilledBPTConditionFound = false;

      gAndCond->add(new GeneticBodypartStateCondition(bodypart,BSP_alive,GBT_equal));
      gAndCond->add(new GeneticParentHealthpointsCondition(bodypart,GBT_less,100.0f));
      
      compAct->add(new GeneticHealParentAction(bodypart,1.0f));
      compAct->add(new GeneticHurtAction(bodypart,1.0f));
      
      if(!unfullfilledBPTConditionFound) 
        addClause(new GeneticClause(gAndCond, compAct, "Heal Parent"));
      else {
        delete gAndCond;
        delete compAct;
      };

    };
    
    return true;
  };

  bool GeneticProcessor::parseGeneticCode() {
    addDummyGenecode();
    
    
    return true;
  };
} // namespace