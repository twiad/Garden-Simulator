#include "geneticCodeFactory.h"

#define RANDOM_SIMPLE_PLANT_MAX_LIFETIME 3000
#define RANDOM_SIMPLE_PLANT_MIN_LIFETIME 1500
 
namespace EDen {
	GeneticCode* GeneticCodeFactory::generateRandomSimplePlant() {
		int speciesIDConstant1 = 100;
		int speciesIDConstant2 = 25;

		int lifetime = Randomizer::value(RANDOM_SIMPLE_PLANT_MIN_LIFETIME,RANDOM_SIMPLE_PLANT_MAX_LIFETIME);
		float baseEnergyConsumption = 0.60f + ((lifetime/3000) * 0.1f);
		float dropSeedEnergyPercentage = mini(97.0f, 84.5f  + ((lifetime/1500) * 1.0f));

		bool gooCreature = (Randomizer::value(0,3) > 2.0f);
		std::string neededResource = "Wasser";
		std::string producedResource = "Goo";
		
		unsigned int speciesIdentifier = (int)Randomizer::value(speciesIDConstant1 + ((int)gooCreature * 100),speciesIDConstant1 + 5.0f + ((int)gooCreature * 100)) + speciesIDConstant2;

		return generateSimplePlant(speciesIdentifier + (Randomizer::value(1,9999) * 1000), lifetime, baseEnergyConsumption, dropSeedEnergyPercentage, neededResource, producedResource, gooCreature);
	};

	GeneticCode* GeneticCodeFactory::generateSimplePlant(long speciesID, unsigned int lifetime, float baseEnergyConsumption, float seedDropEnergyPercentage, std::string primaryResource, std::string secondaryResource, bool switchResources) {
		if(primaryResource == "Energie" || primaryResource == "Sonne" || secondaryResource == "Energie" || secondaryResource == "Sonne") {
			return 0;
		}

		if(switchResources) {
			std::string tmp = primaryResource;
			primaryResource = secondaryResource;
			secondaryResource = tmp;
		}

		float growthEnergyUseAmount = 0.5f;
		float seedDropEnergyAmount = 600.0f;
		float seedMutation = 1.0f;
		float branchMutation = 0.1f;

		float branchMaxSize = Randomizer::value(5.0f,15.0f);
		int branchNumLeafSpawnpoints = 2;
		int branchNumStickOrBranchSpawnpoints = 2;
		int branchNumBranchSpawnpoints = 0;
		int branchNumStickSpawnpoints = 0;
		float branchSpawnEnergyCost = 20.0f;
		float branchSpawnPrimaryResourceCost = 350.0f;
		float branchPrimaryResourceStorageSize = 200.0f;
		float branchSecondaryResourceStorageSize = 80.0f;
		float branchEnergyStorageSize = 3.0f;
		Color branchColor(0.37f,0.37f,0.18f,1.0f);
		//Color branchColor(1.00f,0.00f,0.00f,1.0f);

		float stickMaxSize = Randomizer::value(5.0f,15.0f);
		int stickNumLeafSpawnpoints = 4;
		int stickNumStickOrBranchSpawnpoints = 2;
		int stickNumStickSpawnpoints = 0;
		int stickNumBranchSpawnpoints = 0;
		int stickNumSeedSpawnpoints = 1;
		float stickSpawnEnergyCost = 5.0f;
		float stickSpawnPrimaryResourceCost = 450.0f;
		float stickPrimaryResourceStorageSize = 500.0f;
		float stickSecondaryResourceStorageSize = 120.0f;
		float stickEnergyStorageSize = 35.0f;
		Color stickColor(0.62f,0.43f,0.18f,1.0f);
		//Color stickColor(0.00f,0.00f,1.0f,1.0f);
		
		float leafMaxSize = Randomizer::value(1.0f,5.0f);;
		float leafSpawnEnergyCost = 2.0f;
		float leafSpawnPrimaryResourceCost = 10.0f;
		float leafPrimaryResourceStorageSize = 100.0f;
		float leafSecondaryResourceStorageSize = 100.0f;
		float leafEnergyStorageSize = 175.0f;
		Color leafColor(0.43f,0.9f - (((int)switchResources) * 0.8f),0.08f  + (((int)switchResources) * 0.8f),Randomizer::value(0.8f,1.0f)  - (((int)switchResources) * 0.2f));
		//Color leafColor(0.0f,1.0f,0.00f,1.0f);

		float seedMaxSize = 1.3f;
		float seedSpawnEnergyCost = 1.0f;
		float seedSpawnPrimaryResourceCost = 5.0f;
		float seedPrimaryResourceStorageSize = 500.0f;
		float seedSecondaryResourceStorageSize = 0.0f;
		float seedEnergyStorageSize = 500.0f;
		Color seedColor(0.31f,0.31f,0.31f,1.0f);

		float leafDropSunAmount = 0.5f;
		float leafDropEnergyPercentage = 10.0f;

		float maxAmountMutationProbability = 0.3f;
		float maxSizeMutationsProbability = 0.04f;
		float colorMutationProbability = 0.25f;
		float spawnpointScaleModifierMutationProbability = 0.2f;
		float spawnpointAngleMutationProbability = 0.03f;
		float spawnpointActiveMutationProbability = 0.01f;

		GeneticClauseList* clauses = new GeneticClauseList();
		GeneticMutationList* mutations = new GeneticMutationList();

		addLeafEnergyProduction(clauses, primaryResource, secondaryResource);
		addEnergyConsumptionAndDead(clauses, baseEnergyConsumption, lifetime);
		addGrowth(clauses, growthEnergyUseAmount);
		addHeal(clauses);
		addSeedDrop(clauses, seedDropEnergyPercentage, seedDropEnergyAmount, seedMutation);
		addLeafDrop(clauses, leafDropEnergyPercentage, leafDropSunAmount);

		addBranch(clauses, branchSpawnEnergyCost, branchSpawnPrimaryResourceCost, branchMaxSize, branchColor, branchNumLeafSpawnpoints, branchNumStickSpawnpoints, branchNumBranchSpawnpoints, branchNumStickOrBranchSpawnpoints, branchMutation, primaryResource, branchPrimaryResourceStorageSize, branchEnergyStorageSize, secondaryResource, branchSecondaryResourceStorageSize);
		addStick(clauses, stickSpawnEnergyCost, stickSpawnPrimaryResourceCost, stickMaxSize, stickColor, stickNumLeafSpawnpoints, stickNumStickSpawnpoints, stickNumBranchSpawnpoints, stickNumStickOrBranchSpawnpoints, stickNumSeedSpawnpoints, primaryResource, stickPrimaryResourceStorageSize, stickEnergyStorageSize, secondaryResource, stickSecondaryResourceStorageSize);
		addLeaf(clauses, leafSpawnEnergyCost, leafSpawnPrimaryResourceCost, leafMaxSize, leafColor, primaryResource, leafPrimaryResourceStorageSize, leafEnergyStorageSize, secondaryResource, leafSecondaryResourceStorageSize);
		addSeed(clauses, seedSpawnEnergyCost, seedSpawnPrimaryResourceCost, seedMaxSize, seedColor, primaryResource, seedPrimaryResourceStorageSize, seedEnergyStorageSize, secondaryResource, seedSecondaryResourceStorageSize);

		addMaxAmountMutations(mutations, maxAmountMutationProbability, primaryResource, secondaryResource);
		addMaxSizeMutations(mutations, maxSizeMutationsProbability);
		addColorMutation(mutations, colorMutationProbability);
		addSpawnpointMutations(mutations,spawnpointAngleMutationProbability, spawnpointScaleModifierMutationProbability, spawnpointActiveMutationProbability);

		return new GeneticCode(*clauses,*mutations,0,speciesID,0);
	};

	void GeneticCodeFactory::addLeafEnergyProduction(GeneticClauseList* clauses, std::string primaryResource, std::string secondaryResource) {
			// much energy
		GeneticANDCondition* gAndCond = new GeneticANDCondition();
		GeneticCompoundAction* compAct = new GeneticCompoundAction();
      
		GeneticBodypartTypeCondition* cond = new GeneticBodypartTypeCondition(BPT_Leaf, GBT_equal);

		gAndCond->add(new GeneticBodypartStateCondition(BSP_alive, GBT_equal));
		gAndCond->add(cond);
		gAndCond->add(new GeneticChemicalCondition(GCC_space_left_more, 75.0f, "Energie"));
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more, 5.0f, primaryResource));
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more, 1.5f, "Sonne"));

		compAct->add(new GeneticChemicalConsumeAction("Sonne",1.0f));
		compAct->add(new GeneticSimpleChemicalConvertAction(primaryResource, "Energie", 5.0f, 15.0f));
		if(secondaryResource != "") {
			compAct->add(new GeneticSimpleChemicalConvertAction(primaryResource, secondaryResource, 1.0f, 100.0f));
		}

		clauses->push_back(new GeneticClause(gAndCond, compAct, "Leaf Energie Production High Efficiency"));
			// low energy
		//gAndCond = new GeneticANDCondition();
		//compAct = new GeneticCompoundAction();
  //    
		//cond = new GeneticBodypartTypeCondition(BPT_Leaf, GBT_equal);

		//gAndCond->add(new GeneticBodypartStateCondition(BSP_alive, GBT_equal));
		//gAndCond->add(cond);
		//gAndCond->add(new GeneticChemicalCondition(GCC_space_left_more, 4.0f, "Energie"));
		//gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more, 0.4f, primaryResource));
		//gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more, 0.1f, "Sonne"));
		//gAndCond->add(new GeneticChemicalCondition(GCC_current_value_less, 1.5f, "Sonne"));

		//compAct->add(new GeneticChemicalConsumeAction("Sonne",0.1f));
		//compAct->add(new GeneticSimpleChemicalConvertAction(primaryResource, "Energie", 0.4f, 10.0f));
		//if(secondaryResource != "") {
		//	compAct->add(new GeneticSimpleChemicalConvertAction(primaryResource, secondaryResource, 1.0f, 100.0f));
		//}

		//clauses->push_back(new GeneticClause(gAndCond, compAct, "Leaf Energie Production Low Efficiency"));
	};

	void GeneticCodeFactory::addEnergyConsumptionAndDead(GeneticClauseList* clauses, float idleEnergyUsage, unsigned int lifetime) {
		// energy consumption
		GeneticANDCondition* gAndCond = new GeneticANDCondition();
		GeneticCompoundAction* compAct = new GeneticCompoundAction();
   
		gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
      
		compAct->add(new GeneticRegularChemicalConsumeAction("Energie",idleEnergyUsage,(int)(lifetime * (Randomizer::value(0.9f,1.1f) / 2))));
    
		clauses->push_back(new GeneticClause(gAndCond, compAct, "Energie consumption"));

		// pain
		gAndCond = new GeneticANDCondition();
		compAct = new GeneticCompoundAction();

		gAndCond->add(new GeneticChemicalCondition(GCC_percentage_less,0.0005f,"Energie"));
		gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
		
		compAct->add(new GeneticHurtAction(1.0f));
		compAct->add(new GeneticShrinkAction(0.98f));
      
		clauses->push_back(new GeneticClause(gAndCond, compAct, "lose HP on low Energie"));

		// dead
		gAndCond = new GeneticANDCondition();
		GeneticORCondition* gOrCond = new GeneticORCondition();
		compAct = new GeneticCompoundAction();
      
		gOrCond->add(new GeneticParentOrganismLifetimeCondition(GBT_more,(int)(lifetime)));
		gOrCond->add(new GeneticHealthpointsCondition(GBT_less,0.01f));
		gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
		gAndCond->add(gOrCond);
      
		compAct->add(new GeneticEmptyChemicalStorageAction());
		compAct->add(new GeneticDieAction());
      
		clauses->push_back(new GeneticClause(gAndCond, compAct, "Die if not enough HP or to old"));
	};

	void GeneticCodeFactory::addGrowth(GeneticClauseList* clauses, float energyUseAmount) {
		GeneticANDCondition* gAndCond = new GeneticANDCondition();
		GeneticCompoundAction* compAct = new GeneticCompoundAction();
      
		gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
		gAndCond->add(new GeneticCanGrowCondition());
		gAndCond->add(new GeneticChemicalCondition(GCC_percentage_more,25.0f,"Energie"));
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,energyUseAmount * 2,"Energie"));
      
		compAct->add(new GeneticChemicalConsumeAction("Energie",energyUseAmount));
		compAct->add(new GeneticGrowAction(energyUseAmount));

		clauses->push_back(new GeneticClause(gAndCond, compAct, "Energie consumption and grow"));
	};

	void GeneticCodeFactory::addHeal(GeneticClauseList* clauses) {
		
				// Heal Self
		GeneticANDCondition* gAndCond = new GeneticANDCondition();
		GeneticCompoundAction* compAct = new GeneticCompoundAction();
        
		gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
		gAndCond->add(new GeneticHealthpointsCondition(GBT_less,100.0f));
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more,1.5f,"Energie"));
      
		compAct->add(new GeneticHealAction(1.0f));
//		compAct->add(new GeneticGrowAction(0.1f));
		compAct->add(new GeneticChemicalConsumeAction("Energie",1.0f));

		clauses->push_back(new GeneticClause(gAndCond, compAct, "Convert Energie to HP"));

				// Heal parent
		gAndCond = new GeneticANDCondition();
		compAct = new GeneticCompoundAction();
        
		gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
		gAndCond->add(new GeneticParentHealthpointsCondition(GBT_less,100.0f));
      
		compAct->add(new GeneticHealParentAction(1.0f));
		compAct->add(new GeneticHurtAction(1.0f));
      
		clauses->push_back(new GeneticClause(gAndCond, compAct, "Heal Parent"));
	};

	void GeneticCodeFactory::addSeedDrop(GeneticClauseList* clauses, float energyPercentage, float energyAmount, float seedMutation) {
		GeneticANDCondition* gAndCond = new GeneticANDCondition();
		GeneticCompoundAction* compAct = new GeneticCompoundAction();
   
		gAndCond->add(new GeneticBodypartStateCondition(BSP_alive, GBT_equal));
		gAndCond->add(new GeneticBodypartTypeCondition(BPT_Seed, GBT_equal));
		gAndCond->add(new GeneticHasParentCondition());
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more, energyAmount, "Energie"));
		gAndCond->add(new GeneticChemicalCondition(GCC_percentage_more, energyPercentage, "Energie"));
      
		compAct->add(new GeneticSimpleMutateAction(0, seedMutation));
		compAct->add(new GeneticDropSeedAction());
    
		clauses->push_back(new GeneticClause(gAndCond, compAct, "Drop Seed"));
	};

	void GeneticCodeFactory::addLeafDrop(GeneticClauseList* clauses, float energyPercentage, float sunAmount) {
		GeneticANDCondition* gAndCond = new GeneticANDCondition();
		GeneticCompoundAction* compAct = new GeneticCompoundAction();
   
		gAndCond->add(new GeneticBodypartStateCondition(BSP_alive,GBT_equal));
		gAndCond->add(new GeneticBodypartTypeCondition(BPT_Leaf,GBT_equal));
		gAndCond->add(new GeneticBodypartSizeCondition(GBT_percentage_more,50));
		gAndCond->add(new GeneticHasParentCondition());
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_less,sunAmount,"Sonne"));
		gAndCond->add(new GeneticChemicalCondition(GCC_percentage_less,energyPercentage,"Energie"));
      
		compAct->add(new GeneticEmptyChemicalStorageAction());
		compAct->add(new GeneticDropLeafAction());
    
		clauses->push_back(new GeneticClause(gAndCond, compAct, "Drop Leaf without sun"));
	};

	void GeneticCodeFactory::addBranch(GeneticClauseList* clauses, float energyCost, float primaryResourceCost, float maxSize, Color color, unsigned int numLeafSpawnpoints, unsigned int numStickSpawnpoints, unsigned int numBranchSpawnPoints, unsigned int numStickAndBranchSpawnpoints, float branchMutation, std::string primaryResource, float primaryResourceStorageSize, float energyStorageSize, std::string secondaryResource, float secondaryResourceStorageSize) {
				// Spawn Action
		GeneticANDCondition* gAndCond = new GeneticANDCondition();
		GeneticORCondition* gOrCond = new GeneticORCondition();
		GeneticCompoundAction* compAct = new GeneticCompoundAction();
      
		gOrCond->add(new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal));
		gOrCond->add(new GeneticBodypartTypeCondition(BPT_Branch, GBT_equal));
        
		gAndCond->add(new GeneticBodypartStateCondition(BSP_alive, GBT_equal));
		gAndCond->add(gOrCond);
		gAndCond->add(new GeneticSpawnpointPresentCondition(BPT_Branch));
		gAndCond->add(new GeneticBodypartSizeCondition(GBT_more, 4.5f));
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more, energyCost, "Energie"));
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more, primaryResourceCost, primaryResource));
      
		compAct->add(new GeneticSimpleMutateAction(0, branchMutation));
		compAct->add(new GeneticSpawnBodypartAction(BPT_Branch));
		compAct->add(new GeneticChemicalConsumeAction("Energie", energyCost));
		compAct->add(new GeneticChemicalConsumeAction(primaryResource, primaryResourceCost));
      
		clauses->push_back(new GeneticClause(gAndCond, compAct, "Spawn Branch"));

				// Creation Action
		gAndCond = new GeneticANDCondition();
		compAct = new GeneticCompoundAction();

		std::list<BodypartType> bpts;
		bpts.push_back(BPT_Branch);
		bpts.push_back(BPT_Stick);

		GeneticCondition* cond = new GeneticBodypartTypeCondition(BPT_Branch, GBT_equal);
		gAndCond->add(cond);
		gAndCond->add(new GeneticBodypartCreationCondition());

		//backwardSpawn
		compAct->add(new GeneticAddSpawnpointAction(bpts, 0, 1.0f, 180.0f));

		unsigned int sumSpawnpoints = numStickAndBranchSpawnpoints + numBranchSpawnPoints + numStickSpawnpoints + numLeafSpawnpoints;
		if(sumSpawnpoints > 0) {
			float singleScaleModifierStep = 0.5f / sumSpawnpoints;
			float currentScaleModifier = 0.75f;
			float singleAngularStep = 15.0f / sumSpawnpoints;
			float currentAngle = 1.5f;
			unsigned int spawnpointIndex = 1;

			for(unsigned int i = 0; i < numStickAndBranchSpawnpoints; i++) {
				compAct->add(new GeneticAddSpawnpointAction(bpts, spawnpointIndex, currentScaleModifier, (-1 + (((int)spawnpointIndex % 2) * 2)) * currentAngle, 0.0f, 0.0f, false, true));

				currentAngle += singleAngularStep;
				currentScaleModifier += singleScaleModifierStep;
				spawnpointIndex += 1;
			}

			for(unsigned int i = 0; i < numStickSpawnpoints; i++) {
				compAct->add(new GeneticAddSpawnpointAction(BPT_Stick, spawnpointIndex, currentScaleModifier, (-1 + (((int)spawnpointIndex % 2) * 2)) * currentAngle, 0.0f, 0.0f, false, true));

				currentAngle += singleAngularStep;
				currentScaleModifier += singleScaleModifierStep;
				spawnpointIndex += 1;
			}

			for(unsigned int i = 0; i < numBranchSpawnPoints; i++) {
				compAct->add(new GeneticAddSpawnpointAction(BPT_Branch, spawnpointIndex, currentScaleModifier, (-1 + (((int)spawnpointIndex % 2) * 2)) * currentAngle, 0.0f, 0.0f, false, true));

				currentAngle += singleAngularStep;
				currentScaleModifier += singleScaleModifierStep;
				spawnpointIndex += 1;
			}

			for(unsigned int i = 0; i < numLeafSpawnpoints; i++) {
				compAct->add(new GeneticAddSpawnpointAction(BPT_Leaf, spawnpointIndex, currentScaleModifier, (-1 + (((int)spawnpointIndex % 2) * 2)) * currentAngle, 0.0f, 0.0f, false, true));

				currentAngle += singleAngularStep;
				currentScaleModifier += singleScaleModifierStep;
				spawnpointIndex += 1;
			}
		}


		//resources
		compAct->add(new GeneticChangeMaxChemicalAmountAction(primaryResource, primaryResourceStorageSize));
		compAct->add(new GeneticChangeMaxChemicalAmountAction("Energie", energyStorageSize));
		compAct->add(new GeneticChangeMaxChemicalAmountAction("Sonne", 0.0f));
		if(secondaryResource != "") {
			compAct->add(new GeneticChangeMaxChemicalAmountAction(secondaryResource, secondaryResourceStorageSize));
		}
      
		compAct->add(new GeneticChangeMaxSizeAction(maxSize));
		compAct->add(new GeneticSetColorAction(color.r,color.g,color.b,color.a));

		clauses->push_back(new GeneticClause(gAndCond, compAct, "Branch Creation"));
	};

	void GeneticCodeFactory::addStick(GeneticClauseList* clauses, float energyCost, float primaryResourceCost, float maxSize, Color color, unsigned int numLeafSpawnpoints, unsigned int numStickSpawnpoints, unsigned int numBranchSpawnpoints, unsigned int numStickAndBranchSpawnpoints, unsigned int numSeedSpawnpoints, std::string primaryResource, float primaryResourceStorageSize, float energyStorageSize, std::string secondaryResource, float secondaryResourceStorageSize) {
				// Spawn Action
		GeneticANDCondition* gAndCond = new GeneticANDCondition();
		GeneticORCondition* gOrCond = new GeneticORCondition();
		GeneticCompoundAction* compAct = new GeneticCompoundAction();
      
		gOrCond->add(new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal));
		gOrCond->add(new GeneticBodypartTypeCondition(BPT_Branch, GBT_equal));
        
		gAndCond->add(new GeneticBodypartStateCondition(BSP_alive, GBT_equal));
		gAndCond->add(gOrCond);
		gAndCond->add(new GeneticSpawnpointPresentCondition(BPT_Stick));
		gAndCond->add(new GeneticBodypartSizeCondition(GBT_more, 4.5f));
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more, energyCost, "Energie"));
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more, primaryResourceCost, primaryResource));
      
		compAct->add(new GeneticSpawnBodypartAction(BPT_Stick));
		compAct->add(new GeneticChemicalConsumeAction("Energie", energyCost));
		compAct->add(new GeneticChemicalConsumeAction(primaryResource, primaryResourceCost));
      
		clauses->push_back(new GeneticClause(gAndCond, compAct, "Spawn Stick"));

				// Creation Action
		gAndCond = new GeneticANDCondition();
		compAct = new GeneticCompoundAction();

		GeneticCondition* cond = new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal);
		gAndCond->add(cond);
		gAndCond->add(new GeneticBodypartCreationCondition());

		//backwardSpawn
		std::list<BodypartType> backSpawnpoint;
		backSpawnpoint.push_back(BPT_Branch);
		backSpawnpoint.push_back(BPT_Stick);
		backSpawnpoint.push_back(BPT_Seed);

		std::list<BodypartType> bpts;
		bpts.push_back(BPT_Branch);
		bpts.push_back(BPT_Stick);

		compAct->add(new GeneticAddSpawnpointAction(bpts, 0, 1.0f, 180.0f));

		unsigned int sumSpawnpoints = numStickAndBranchSpawnpoints + numBranchSpawnpoints + numStickSpawnpoints + numLeafSpawnpoints + numSeedSpawnpoints;
		if(sumSpawnpoints > 0) {
			float singleScaleModifierStep = 0.5f / sumSpawnpoints;
			float currentScaleModifier = 0.75f;
			float singleAngularStep = 40.0f / sumSpawnpoints;
			float currentAngle = 5.0f;
			unsigned int spawnpointIndex = 1;

			for(unsigned int i = 0; i < numStickAndBranchSpawnpoints; i++) {
				compAct->add(new GeneticAddSpawnpointAction(bpts, spawnpointIndex, currentScaleModifier, (-1 + (((int)spawnpointIndex % 2) * 2)) * currentAngle, 0.0f, 0.0f, false, true));

				currentAngle += singleAngularStep;
				currentScaleModifier += singleScaleModifierStep;
				spawnpointIndex += 1;
			}

			for(unsigned int i = 0; i < numStickSpawnpoints; i++) {
				compAct->add(new GeneticAddSpawnpointAction(BPT_Stick, spawnpointIndex, currentScaleModifier, (-1 + (((int)spawnpointIndex % 2) * 2)) * currentAngle, 0.0f, 0.0f, false, true));

				currentAngle += singleAngularStep;
				currentScaleModifier += singleScaleModifierStep;
				spawnpointIndex += 1;
			}

			for(unsigned int i = 0; i < numBranchSpawnpoints; i++) {
				compAct->add(new GeneticAddSpawnpointAction(BPT_Branch, spawnpointIndex, currentScaleModifier, (-1 + (((int)spawnpointIndex % 2) * 2)) * currentAngle, 0.0f, 0.0f, false, true));

				currentAngle += singleAngularStep;
				currentScaleModifier += singleScaleModifierStep;
				spawnpointIndex += 1;
			}

			for(unsigned int i = 0; i < numLeafSpawnpoints; i++) {
				compAct->add(new GeneticAddSpawnpointAction(BPT_Leaf, spawnpointIndex, currentScaleModifier, (-1 + (((int)spawnpointIndex % 2) * 2)) * currentAngle, 0.0f, 0.0f, false, true));

				currentAngle += singleAngularStep;
				currentScaleModifier += singleScaleModifierStep;
				spawnpointIndex += 1;
			}

			for(unsigned int i = 0; i < numSeedSpawnpoints; i++) {
				compAct->add(new GeneticAddSpawnpointAction(BPT_Seed, spawnpointIndex, currentScaleModifier, (-1 + (((int)spawnpointIndex % 2) * 2)) * currentAngle, 0.0f, 0.0f, false, true));

				currentAngle += singleAngularStep;
				currentScaleModifier += singleScaleModifierStep;
				spawnpointIndex += 1;
			}

			compAct->add(new GeneticAddSpawnpointAction(BPT_Branch, spawnpointIndex, 1.0f, 0.0f, 0.0f, 0.0f, false, false));
		}


		//resources
		compAct->add(new GeneticChangeMaxChemicalAmountAction(primaryResource, primaryResourceStorageSize));
		compAct->add(new GeneticChangeMaxChemicalAmountAction("Energie", energyStorageSize));
		compAct->add(new GeneticChangeMaxChemicalAmountAction("Sonne", 0.0f));
		if(secondaryResource != "") {
			compAct->add(new GeneticChangeMaxChemicalAmountAction(secondaryResource, secondaryResourceStorageSize));
		}
      
		compAct->add(new GeneticChangeMaxSizeAction(maxSize));
		compAct->add(new GeneticSetColorAction(color.r,color.g,color.b,color.a));

		clauses->push_back(new GeneticClause(gAndCond, compAct, "Stick Creation"));
	};

	void GeneticCodeFactory::addLeaf(GeneticClauseList* clauses, float energyCost, float primaryResourceCost, float maxSize, Color color, std::string primaryResource, unsigned int primaryResourceStorageSize, float energyStorageSize, std::string secondaryResource, unsigned int secondaryResourceStorageSize) {
				// Spawn Action
		GeneticANDCondition* gAndCond = new GeneticANDCondition();
		GeneticORCondition* gOrCond = new GeneticORCondition();
		GeneticCompoundAction* compAct = new GeneticCompoundAction();
      
		gOrCond->add(new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal));
		gOrCond->add(new GeneticBodypartTypeCondition(BPT_Branch, GBT_equal));
        
		gAndCond->add(new GeneticBodypartStateCondition(BSP_alive, GBT_equal));
		gAndCond->add(gOrCond);
		gAndCond->add(new GeneticSpawnpointPresentCondition(BPT_Leaf));
		gAndCond->add(new GeneticBodypartSizeCondition(GBT_more, 4.5f));
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more, energyCost, "Energie"));
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more, primaryResourceCost, primaryResource));
      
		compAct->add(new GeneticSpawnBodypartAction(BPT_Leaf));
		compAct->add(new GeneticChemicalConsumeAction("Energie", energyCost));
		compAct->add(new GeneticChemicalConsumeAction(primaryResource, primaryResourceCost));
      
		clauses->push_back(new GeneticClause(gAndCond, compAct, "Spawn Leaf"));

				// Creation Action
		gAndCond = new GeneticANDCondition();
		compAct = new GeneticCompoundAction();

		GeneticCondition* cond = new GeneticBodypartTypeCondition(BPT_Leaf, GBT_equal);
		gAndCond->add(cond);
		gAndCond->add(new GeneticBodypartCreationCondition());

		//backwardSpawn
		std::list<BodypartType> bpts;
		bpts.push_back(BPT_Branch);
		bpts.push_back(BPT_Stick);

		compAct->add(new GeneticAddSpawnpointAction(bpts, 0, 1.0f, 180.0f,0.0f,0.0f));

		//resources
		compAct->add(new GeneticChangeMaxChemicalAmountAction(primaryResource, primaryResourceStorageSize));
		compAct->add(new GeneticChangeMaxChemicalAmountAction("Energie", energyStorageSize));
		compAct->add(new GeneticChangeMaxChemicalAmountAction("Sonne", 6.0f));
		if(secondaryResource != "") {
			compAct->add(new GeneticChangeMaxChemicalAmountAction(secondaryResource, secondaryResourceStorageSize));
		}
      
		compAct->add(new GeneticChangeMaxSizeAction(maxSize));
		compAct->add(new GeneticSetColorAction(color.r,color.g,color.b,color.a));

		clauses->push_back(new GeneticClause(gAndCond, compAct, "Leaf Creation"));
	};
	
	void GeneticCodeFactory::addSeed(GeneticClauseList* clauses, float energyCost, float primaryResourceCost, float maxSize, Color color, std::string primaryResource, unsigned int primaryResourceStorageSize, float energyStorageSize, std::string secondaryResource, unsigned int secondaryResourceStorageSize) {
				// Spawn Action
		GeneticANDCondition* gAndCond = new GeneticANDCondition();
		GeneticORCondition* gOrCond = new GeneticORCondition();
		GeneticCompoundAction* compAct = new GeneticCompoundAction();
      
		gOrCond->add(new GeneticBodypartTypeCondition(BPT_Stick, GBT_equal));
		gOrCond->add(new GeneticBodypartTypeCondition(BPT_Branch, GBT_equal));
        
		gAndCond->add(new GeneticBodypartStateCondition(BSP_alive, GBT_equal));
		gAndCond->add(gOrCond);
		gAndCond->add(new GeneticSpawnpointPresentCondition(BPT_Seed));
		gAndCond->add(new GeneticBodypartSizeCondition(GBT_more, 4.5f));
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more, energyCost, "Energie"));
		gAndCond->add(new GeneticChemicalCondition(GCC_current_value_more, primaryResourceCost, primaryResource));
      
		compAct->add(new GeneticSpawnBodypartAction(BPT_Seed));
		compAct->add(new GeneticChemicalConsumeAction("Energie", energyCost));
		compAct->add(new GeneticChemicalConsumeAction(primaryResource, primaryResourceCost));
      
		clauses->push_back(new GeneticClause(gAndCond, compAct, "Spawn Seed"));

				// Creation Action
		gAndCond = new GeneticANDCondition();
		compAct = new GeneticCompoundAction();

		GeneticCondition* cond = new GeneticBodypartTypeCondition(BPT_Seed, GBT_equal);
		gAndCond->add(cond);
		gAndCond->add(new GeneticBodypartCreationCondition());

		//backwardSpawn
		std::list<BodypartType> bpts;
		bpts.push_back(BPT_Branch);
		bpts.push_back(BPT_Stick);

		compAct->add(new GeneticAddSpawnpointAction(bpts, 0, 1.00f, 180.0f));

		//resources
		compAct->add(new GeneticChangeMaxChemicalAmountAction(primaryResource, primaryResourceStorageSize));
		compAct->add(new GeneticChangeMaxChemicalAmountAction("Energie", energyStorageSize));
		compAct->add(new GeneticChangeMaxChemicalAmountAction("Sonne", 0.0f));
		if(secondaryResource != "") {
			compAct->add(new GeneticChangeMaxChemicalAmountAction(secondaryResource, secondaryResourceStorageSize));
		}
      
		compAct->add(new GeneticChangeMaxSizeAction(maxSize));
		compAct->add(new GeneticSetColorAction(color.r,color.g,color.b,color.a));

		clauses->push_back(new GeneticClause(gAndCond, compAct, "Seed Creation"));
	};

	void GeneticCodeFactory::addMaxAmountMutations(GeneticMutationList* mutations, float prob, std::string primaryResource, std::string secondaryResource) {
		mutations->push_back(new GeneticMaxAmountMutation(1.0f,10000.0f,10.0f,prob, primaryResource, std::string("Max Amount Mutation: ").append(primaryResource)));
		if(secondaryResource != "") {
			mutations->push_back(new GeneticMaxAmountMutation(1.0f,10000.0f,10.0f,prob, secondaryResource, std::string("Max Amount Mutation: ").append(secondaryResource)));
		}
	};

	void GeneticCodeFactory::addMaxSizeMutations(GeneticMutationList* mutations, float prob) {
		mutations->push_back(new GeneticMaxSizeMutation(1.0f,1000.0f,2.0f,prob,"Max Size Mutation"));
	};

	void GeneticCodeFactory::addColorMutation(GeneticMutationList* mutations, float prob) {
		mutations->push_back(new GeneticColorMutation(0.1f,1.0f,0.08f,prob,"ColorMutation"));
	};

	void GeneticCodeFactory::addSpawnpointMutations(GeneticMutationList* mutations, float probAngle, float probScale, float probActive) {
		mutations->push_back(new GeneticSpawnpoint2DAngleMutation(-179.0f,179.0f,5.0f,probAngle,"Anglular Mutation"));
		mutations->push_back(new GeneticSpawnpointActiveMutation(probActive, "Spawnpoint Active Mutation"));
		mutations->push_back(new GeneticSpawnpointScaleModifierMutation(0.1f,1.1f,0.05f,probScale,"Scale Modifier Mutation"));
	};
}
