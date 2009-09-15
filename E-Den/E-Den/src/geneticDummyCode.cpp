#include "geneticProcessor.h"

#define GRUNDVERBRAUCH 0.045f
#define LIFETIME 1500
#define SEED_DROP_ENERGIE_PERCENTAGE 96.0f
#define BRANCHE_MUTATION 0.1f
#define SEED_MUTATION 1.0f

namespace EDen {
  bool GeneticCode::generateDummyGenecode(std::string code) {
    if(code == "TESTPART1") {
      speciesIdentifier = 500;
      GeneticANDCondition* andCond = new GeneticANDCondition();
      andCond->add(new GeneticChemicalCondition(GCC_current_value_more,0.05f,"Wasser"));
      andCond->add(new GeneticChemicalCondition(GCC_space_left_more,0.005f,"Gold"));
      
      GeneticCompoundAction* compAct = new GeneticCompoundAction();
      compAct->add(new GeneticSimpleChemicalConvertAction("Wasser","Gold",0.05f,0.1f));

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
      speciesIdentifier = 501;
      GeneticANDCondition* andCond = new GeneticANDCondition();
      andCond->add(new GeneticChemicalCondition(GCC_space_left_more,0.1f,"BLAK"));
      andCond->add(new GeneticChemicalCondition(GCC_current_value_more,0.01f,"Gold"));
      
      GeneticCompoundAction* compAct = new GeneticCompoundAction();
      compAct->add(new GeneticSimpleChemicalConvertAction("Gold","BLAK",0.01f,10.0f));

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
      speciesIdentifier = 502;
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
      // IF Type = Seed
      //  AND Creation
      // THEN
      //  Add Stick position 0 at 180°
      //  Change maximum amount of "Wasser" to 15
      //  Change maximum amount of "Energie" to 150
      //  Change maximum size to 2.0f + (100.0f - parent "Energie" percentage) * 0.05
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();

      cond = new GeneticBodypartTypeCondition(BPT_Seed, GBT_equal);
      gAndCond->add(cond);
      gAndCond->add(new GeneticBodypartCreationCondition());
      compAct->add(new GeneticAddSpawnpointAction(BPT_Stick, 0, 0.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Wasser",500.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Energie",500.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Sonne",100.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Goo", 0.0f));
      
      compAct->add(new GeneticChangeMaxSizeAction(2.0f));
      addClause(new GeneticClause(gAndCond, compAct, "Seed Creation"));

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

      cond = new GeneticBodypartTypeCondition(BPT_Leaf, GBT_equal);
      gAndCond->add(cond);
      gAndCond->add(new GeneticBodypartCreationCondition());
      compAct->add(new GeneticAddSpawnpointAction(bpts, 0, 180.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Wasser",15.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Energie",150.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Sonne",120.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Goo", 10.0f));
      
      compAct->add(new GeneticChangeMaxSizeAction(1.5f));
      addClause(new GeneticClause(gAndCond, compAct, "Leaf Creation"));
    
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
      //  Change maximum amount of "Energie" to 35
      //  Change maximum size to 5.0 + parent water percantage * 50
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();

      cond = new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal);
      gAndCond->add(cond);
      gAndCond->add(new GeneticBodypartCreationCondition());
      compAct->add(new GeneticAddSpawnpointAction(bpts, 0, 180.0f));
      compAct->add(new GeneticAddSpawnpointAction(bpts, 1, 0.0f));
      compAct->add(new GeneticAddSpawnpointAction(BPT_Leaf, 2, 145.0f));
      compAct->add(new GeneticAddSpawnpointAction(BPT_Leaf, 3, -145.0f));
      compAct->add(new GeneticAddSpawnpointAction(BPT_Leaf, 4, 125.0f));
      compAct->add(new GeneticAddSpawnpointAction(BPT_Leaf, 5, -125.0f));
      compAct->add(new GeneticAddSpawnpointAction(BPT_Seed, 6, 0.0f));
      //compAct->add(new GeneticAddSpawnpointAction(BPT_Leaf, 7));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Wasser", 500.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Energie", 35.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Sonne", 0.01f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Goo", 10.0f));
      
      compAct->add(new GeneticChangeMaxSizeAction(5.0f));

      addClause(new GeneticClause(gAndCond, compAct, "Stick Creation"));
 
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

      cond = new GeneticBodypartTypeCondition(BPT_Branch, GBT_equal);
      gAndCond->add(cond);
      gAndCond->add(new GeneticBodypartCreationCondition());
      compAct->add(new GeneticAddSpawnpointAction(bpts, 0, 180.0f));
      compAct->add(new GeneticAddSpawnpointAction(bpts, 1, -13.0f));
      compAct->add(new GeneticAddSpawnpointAction(bpts, 2, 13.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Wasser",150.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Energie",3.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Sonne", 0.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Goo", 3.0f));
      
      compAct->add(new GeneticChangeMaxSizeAction(10.0f));

      addClause(new GeneticClause(gAndCond, compAct, "Branch Creation"));
     
      ////////////////////////// Creation Rules End Here ////////////////////

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND Type = Leaf
      //  AND "Energie" space_left_more than 10.0
      //  AND "Wasser" current_value_more than 15.0
      //  AND "Sonne" current_value_more than 1.0
      // THEN
      //  Consume 1.0 "Sonne"
      //  Convert 10.0 "Wasser" to 20.0 "Energie"
      //  Convert 0.1 "Wasser" to 5.0 "Goo"
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
      
      cond = new GeneticBodypartTypeCondition(BPT_Leaf, GBT_equal);

      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(cond);
      gAndCond->add(new GeneticChemicalCondition(GCC_space_left_more,10.0f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,15.0f,"Wasser"));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,1.0f,"Sonne"));

      compAct->add(new GeneticChemicalConsumeAction("Sonne",1.0f));
      compAct->add(new GeneticSimpleChemicalConvertAction("Wasser","Energie",10.0f,2.0f));
      compAct->add(new GeneticSimpleChemicalConvertAction("Wasser","Goo",0.1f,50.0f));

      addClause(new GeneticClause(gAndCond, compAct, "Leaf Energie Production"));
      
      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      // THEN
      //  Consume GRUNDVERBRAUCH "Energie"
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
   
      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      
      compAct->add(new GeneticChemicalConsumeAction("Energie",GRUNDVERBRAUCH));
    
      addClause(new GeneticClause(gAndCond, compAct, "Energie consumption"));

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND Type = Seed
      //  AND HasParent
      //  AND "Energie" more than 97%
      // THEN
      //  DetatchSeed
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
   
      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(new GeneticBodypartTypeCondition(BPT_Seed,GBT_equal));
      gAndCond->add(new GeneticHasParentCondition());
      gAndCond->add(new GeneticChemicalCondition(GCC_percentage_more,SEED_DROP_ENERGIE_PERCENTAGE,"Energie"));
      
      compAct->add(new GeneticSimpleMutateAction(0,SEED_MUTATION));
      compAct->add(new GeneticDropSeedAction());
    
      addClause(new GeneticClause(gAndCond, compAct, "Drop Seed"));
            
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
      
      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(new GeneticCanGrowCondition());
      gAndCond->add(new GeneticChemicalCondition(GCC_percentage_more,25.0f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,1.375f,"Energie"));
      
      compAct->add(new GeneticChemicalConsumeAction("Energie",0.50f));
      compAct->add(new GeneticGrowAction(0.1f));

      addClause(new GeneticClause(gAndCond, compAct, "Energie consumption and grow"));
    
      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  Type = Stick
      //  AND "Wasser" current_value_more than 10.00
      //  AND "Energie" current_value_more than 2.0
      //  AND "Energie" space_left_more than 4.0
      //  AND Spawnpoint Seed present
      // THEN
      //  Spawn Seed
      //  Consume "Energie" 2.0
      //  Consume "Wasser" 10.0
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
      
      cond = new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal);

      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(cond);
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,10.0f,"Wasser"));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,2.0f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(GCC_space_left_more,4.0f,"Energie"));
      gAndCond->add(new GeneticSpawnpointPresentCondition(BPT_Seed));

      compAct->add(new GeneticSpawnBodypartAction(BPT_Seed));
      compAct->add(new GeneticChemicalConsumeAction("Energie",2.0f));
      compAct->add(new GeneticChemicalConsumeAction("Wasser",10.0f));

      addClause(new GeneticClause(gAndCond, compAct, "Spawn Leaf"));

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
      
      cond = new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal);

      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(cond);
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,10.0f,"Wasser"));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,2.0f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(GCC_space_left_more,4.0f,"Energie"));
      gAndCond->add(new GeneticSpawnpointPresentCondition(BPT_Leaf));

      compAct->add(new GeneticSpawnBodypartAction(BPT_Leaf));
      compAct->add(new GeneticChemicalConsumeAction("Energie",2.0f));
      compAct->add(new GeneticChemicalConsumeAction("Wasser",10.0f));

      addClause(new GeneticClause(gAndCond, compAct, "Spawn Leaf"));

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  Type = Stick OR Branch
      //  AND Spawnpoint Stick present
      //  AND Size > 1.5
      //  AND "Energie" current_value_more than 0.01
      //  AND "Wasser" current_value_more than 20.0
      // THEN
      //  //Mutate
      //  Spawn Stick
      //  Consume 0.01 "Energie"
      //  Consume 20.0 "Wasser"
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      gOrCond = new GeneticORCondition();
      compAct = new GeneticCompoundAction();
      
      gOrCond->add(new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal));
      gOrCond->add(new GeneticBodypartTypeCondition(BPT_Branch, GBT_equal));
        
      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(gOrCond);
      gAndCond->add(new GeneticSpawnpointPresentCondition(BPT_Stick));
      gAndCond->add(new GeneticBodypartSizeCondition(GBT_more,4.5f));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,0.01f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,450.0f,"Wasser"));
//      gAndCond->add(new GeneticChemicalCondition(GCC_percentage_less,46.0f,"Sonne"));
      

      //compAct->add(new GeneticSimpleMutateAction());
      compAct->add(new GeneticSpawnBodypartAction(BPT_Stick));
      compAct->add(new GeneticChemicalConsumeAction("Energie",0.01f));
      compAct->add(new GeneticChemicalConsumeAction("Wasser",450.0f));

      addClause(new GeneticClause(gAndCond, compAct, "Spawn Stick"));

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  Type = Stick OR Branch
      //  Size > 1.5
      //  AND "Energie" current_value_more than 7.0
      //  AND "Wasser" current_value_more than 250.0
      //  AND Spawnpoint Branch present
      // THEN
      //  Mutate
      //  Spawn Branch
      //  Consume 6.0 "Energie"
      //  Consume 250.0 "Wasser"
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      gOrCond = new GeneticORCondition();
      compAct = new GeneticCompoundAction();
      
      gOrCond->add(new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal));
      gOrCond->add(new GeneticBodypartTypeCondition(BPT_Branch, GBT_equal));
        
      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(gOrCond);
      gAndCond->add(new GeneticSpawnpointPresentCondition(BPT_Branch));
      gAndCond->add(new GeneticBodypartSizeCondition(GBT_more,4.5f));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,20.00f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,350.00f,"Wasser"));
      
      compAct->add(new GeneticSimpleMutateAction(0,BRANCHE_MUTATION));
      compAct->add(new GeneticSpawnBodypartAction(BPT_Branch));
      compAct->add(new GeneticChemicalConsumeAction("Energie",20.0f));
      compAct->add(new GeneticChemicalConsumeAction("Wasser",350.0f));
      
      addClause(new GeneticClause(gAndCond, compAct, "Spawn Branche"));

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND "Energie" percentage_less than 0.0001
      // THEN
      //  Hurt 1.0
      //  Shrink 0.97
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      //gOrCond = new GeneticORCondition();
      compAct = new GeneticCompoundAction();

      //gOrCond->add(new GeneticParentOrganismLifetimeCondition(GBT_more,140));
      gAndCond->add(new GeneticChemicalCondition(GCC_percentage_less,0.0001f,"Energie"));
      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      //gAndCond->add(gOrCond);
      
      compAct->add(new GeneticHurtAction(1.0f));
      compAct->add(new GeneticShrinkAction(0.97f));
      
      addClause(new GeneticClause(gAndCond, compAct, "lose HP on low Energie"));

      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      //  AND Healthpoints less_than 0.1 OR ParentOrganismLifetime > 2000
      // THEN
      //  EmptyChemicalStorage
      //  Die
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      gOrCond = new GeneticORCondition();
      compAct = new GeneticCompoundAction();
      
      gOrCond->add(new GeneticParentOrganismLifetimeCondition(GBT_more,LIFETIME));
      gOrCond->add(new GeneticHealthpointsCondition(GBT_less,0.1f));
      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(gOrCond);
      
      
      compAct->add(new GeneticEmptyChemicalStorageAction());
      compAct->add(new GeneticDieAction());
      
      addClause(new GeneticClause(gAndCond, compAct, "Die if not enough HP or to old"));

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
        
      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(new GeneticHealthpointsCondition(GBT_less,100.0f));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,1.5f,"Energie"));
      
      compAct->add(new GeneticHealAction(1.0f));
      compAct->add(new GeneticGrowAction(1.0f));
      compAct->add(new GeneticChemicalConsumeAction("Energie",1.0f));

      addClause(new GeneticClause(gAndCond, compAct, "Convert Energie to HP"));

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
        
      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(new GeneticParentHealthpointsCondition(GBT_less,100.0f));
      
      compAct->add(new GeneticHealParentAction(1.0f));
      compAct->add(new GeneticHurtAction(1.0f));
      
      addClause(new GeneticClause(gAndCond, compAct, "Heal Parent"));

    }
    else if(code == "TESTPART4") {
      speciesIdentifier = 503;
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
      //  Change maximum size to 2.5
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();

      cond = new GeneticBodypartTypeCondition(BPT_Leaf, GBT_equal);

      gAndCond->add(cond);
      gAndCond->add(new GeneticBodypartCreationCondition());
      compAct->add(new GeneticAddSpawnpointAction(bpts, 0, 180.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Wasser",15.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Energie",150.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Goo",3.0f));
      
      compAct->add(new GeneticChangeMaxSizeAction(2.5f));
      
      addClause(new GeneticClause(gAndCond, compAct, "Leaf Creation"));
      
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
      //  Change maximum size to 5.0
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();

      cond = new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal);
      gAndCond->add(cond);
      gAndCond->add(new GeneticBodypartCreationCondition());
      compAct->add(new GeneticAddSpawnpointAction(bpts, 0, 180.0f));
      compAct->add(new GeneticAddSpawnpointAction(bpts, 1, 3.25f));
      compAct->add(new GeneticAddSpawnpointAction(BPT_Leaf, 2, 165.0f));
      compAct->add(new GeneticAddSpawnpointAction(BPT_Leaf, 3, -165.0f));
      compAct->add(new GeneticAddSpawnpointAction(BPT_Leaf, 4, 175.0f));
      compAct->add(new GeneticAddSpawnpointAction(BPT_Leaf, 5, -175.0f));
      compAct->add(new GeneticAddSpawnpointAction(bpts, 6, -3.25f));
      //compAct->add(new GeneticAddSpawnpointAction(BPT_Leaf, 6));
      //compAct->add(new GeneticAddSpawnpointAction(BPT_Leaf, 7));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Wasser", 100.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Energie", 15.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Goo", 5.0f));
      
      compAct->add(new GeneticChangeMaxSizeAction(5.0f));

      addClause(new GeneticClause(gAndCond, compAct, "Stick Creation"));
      
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
      //  Change maximum size to 80.0
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();

      cond = new GeneticBodypartTypeCondition(BPT_Branch, GBT_equal);
      gAndCond->add(cond);
      gAndCond->add(new GeneticBodypartCreationCondition());
      compAct->add(new GeneticAddSpawnpointAction(bpts, 0, 180.0f));
      compAct->add(new GeneticAddSpawnpointAction(bpts, 1, -9.0f));
      compAct->add(new GeneticAddSpawnpointAction(bpts, 2, 9.0f));
      compAct->add(new GeneticAddSpawnpointAction(bpts, 3, -18.0f));
      compAct->add(new GeneticAddSpawnpointAction(bpts, 4, 18.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Wasser",75.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Energie",5.0f));
      compAct->add(new GeneticChangeMaxChemicalAmountAction("Goo",10.0f));
      
      compAct->add(new GeneticChangeMaxSizeAction(30.0f));

      addClause(new GeneticClause(gAndCond, compAct, "Branch Creation"));
      
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
      
      cond = new GeneticBodypartTypeCondition(BPT_Leaf, GBT_equal);

      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(cond);
      gAndCond->add(new GeneticChemicalCondition(GCC_space_left_more,10.0f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,15.0f,"Wasser"));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,1.0f,"Goo"));

      compAct->add(new GeneticChemicalConsumeAction("Goo",1.0f));
      compAct->add(new GeneticSimpleChemicalConvertAction("Wasser","Energie",1.0f,10.0f));

      addClause(new GeneticClause(gAndCond, compAct, "Leaf Energie Production"));
     
      ///////////////////////////////////////////////////////////////////////
      // Rule:
      // IF State = Alive
      // THEN
      //  Consume 0.0013 "Energie"
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
      
      unfullfilledBPTConditionFound = false;
      
      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      
      compAct->add(new GeneticChemicalConsumeAction("Energie",GRUNDVERBRAUCH));


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
      //  Mutate
      ///////////////////////////////////////////////////////////////////////

      gAndCond = new GeneticANDCondition();
      compAct = new GeneticCompoundAction();
      
      unfullfilledBPTConditionFound = false;
      
      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(new GeneticCanGrowCondition());
      //gAndCond->add(new GeneticChemicalCondition(GCC_percentage_more,25.0f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,1.375f,"Energie"));
      
      compAct->add(new GeneticChemicalConsumeAction("Energie",0.50f));
      compAct->add(new GeneticGrowAction(0.1f));
      // compAct->add(new GeneticSimpleMutateAction());

      addClause(new GeneticClause(gAndCond, compAct, "Energie consumption and grow"));

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
      
      cond = new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal);

      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(cond);
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,10.0f,"Wasser"));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,2.0f,"Energie"));

      gAndCond->add(new GeneticSpawnpointPresentCondition(BPT_Leaf));

      compAct->add(new GeneticSpawnBodypartAction(BPT_Leaf));
      compAct->add(new GeneticChemicalConsumeAction("Energie",2.0f));
      compAct->add(new GeneticChemicalConsumeAction("Wasser",10.0f));

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
      
      gOrCond->add(new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal));
      gOrCond->add(new GeneticBodypartTypeCondition(BPT_Branch, GBT_equal));
        
      //unfullfilledBPTConditionFound = !(gOrCond->fullfilled());

      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(gOrCond);
      gAndCond->add(new GeneticSpawnpointPresentCondition(BPT_Stick));
      gAndCond->add(new GeneticBodypartSizeCondition(GBT_more,1.0f));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,0.01f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,20.0f,"Wasser"));
      gAndCond->add(new GeneticChemicalCondition(GCC_percentage_less,15.0f,"Energie"));
      

      compAct->add(new GeneticSpawnBodypartAction(BPT_Stick));
      compAct->add(new GeneticChemicalConsumeAction("Energie",0.01f));
      compAct->add(new GeneticChemicalConsumeAction("Wasser",20.0f));

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
      //  Mutate
      //  Spawn Branch
      //  Consume 0.3 "Energie"
      //  Consume 0.3 "Wasser"
      ///////////////////////////////////////////////////////////////////////
      
      gAndCond = new GeneticANDCondition();
      gOrCond = new GeneticORCondition();
      compAct = new GeneticCompoundAction();
      
      gOrCond->add(new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal));
      gOrCond->add(new GeneticBodypartTypeCondition(BPT_Branch, GBT_equal));
        
      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(gOrCond);
      gAndCond->add(new GeneticSpawnpointPresentCondition(BPT_Branch));
      gAndCond->add(new GeneticBodypartSizeCondition(GBT_more,1.5f));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,0.3f,"Energie"));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,0.3f,"Wasser"));
      gAndCond->add(new GeneticChemicalCondition(GCC_percentage_more,1.5f,"Energie"));

      compAct->add(new GeneticSimpleMutateAction());
      compAct->add(new GeneticSpawnBodypartAction(BPT_Branch));
      compAct->add(new GeneticChemicalConsumeAction("Energie",0.1f));
      compAct->add(new GeneticChemicalConsumeAction("Wasser",0.1f));

 
      addClause(new GeneticClause(gAndCond, compAct, "Spawn Branche"));

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
        
      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(new GeneticChemicalCondition(GCC_percentage_less,0.0001f,"Energie"));
      
      compAct->add(new GeneticHurtAction(1.0f));
      compAct->add(new GeneticShrinkAction(0.97f));
      
      addClause(new GeneticClause(gAndCond, compAct, "lose HP on low Energie"));
  
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

      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(new GeneticHealthpointsCondition(GBT_less,0.01f));
      
      compAct->add(new GeneticEmptyChemicalStorageAction());
      compAct->add(new GeneticDieAction());

      
      addClause(new GeneticClause(gAndCond, compAct, "Die if not enough HP"));
    
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

      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(new GeneticHealthpointsCondition(GBT_less,100.0f));
      gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,1.5f,"Energie"));
      
      compAct->add(new GeneticHealAction(1.0f));
      compAct->add(new GeneticGrowAction(1.0f));
      compAct->add(new GeneticChemicalConsumeAction("Energie",1.0f));
      
      addClause(new GeneticClause(gAndCond, compAct, "Convert Energie to HP"));
 
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
        


      gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      gAndCond->add(new GeneticParentHealthpointsCondition(GBT_less,100.0f));
      
      compAct->add(new GeneticHealParentAction(1.0f));
      compAct->add(new GeneticHurtAction(1.0f));

      addClause(new GeneticClause(gAndCond, compAct, "Heal Parent"));

    };
    
    possibleMutations.push_back(new GeneticSpawnpoint2DAngleMutation(-179.0f,179.0f,20.0f,0.1f,"Silli Anglular Mutation"));
    possibleMutations.push_back(new GeneticMaxSizeMutation(1.0f,1000.0f,2.0f,0.05f,"Silli MaxSize Mutation"));
    
    return true;
  };
};