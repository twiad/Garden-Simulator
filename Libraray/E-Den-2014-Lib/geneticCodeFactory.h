#pragma once

#include "geneticCode.h"

namespace EDen {
	class GeneticCodeFactory
	{
	protected:
		static void addLeafEnergyProduction(GeneticClauseList* clauses, std::string primaryResource, std::string secondaryResource = "");
		static void addEnergyConsumptionAndDead(GeneticClauseList* clauses, float idleEnergyUsage, unsigned int lifetime);
		static void addGrowth(GeneticClauseList* clauses, float energyUseAmount);
		static void addHeal(GeneticClauseList* clauses);
		
		static void addBranch(GeneticClauseList* clauses, float energyCost, float primaryResourceCost, float maxSize, Color color, unsigned int numLeafSpawnpoints, unsigned int numStickSpawnpoints, unsigned int numBranchSpawnPoints, unsigned int numStickOrBranchSpawnpoints, float branchMutation, std::string primaryResource, float primaryResourceStorageSize, float energyStorageSize, std::string secondaryResource = "", float secondaryResourceStorageSize = 100.0f);
		static void addStick(GeneticClauseList* clauses, float energyCost, float primaryResourceCost, float maxSize, Color color, unsigned int numLeafSpawnpoints, unsigned int numStickSpawnpoints, unsigned int numBranchSpawnPoints, unsigned int numStickOrBranchSpawnpoints, unsigned int numSeedSpawnpoints, std::string primaryResource, float primaryResourceStorageSize, float energyStorageSize, std::string secondaryResource = "", float secondaryResourceStorageSize = 50.0f);
		static void addLeaf(GeneticClauseList* clauses, float energyCost, float primaryResourceCost, float maxSize, Color color, std::string primaryResource, unsigned int primaryResourceStorageSize, float energyStorageSize, std::string secondaryResource = "", unsigned int secondaryResourceStorageSize = 10.0f);
		static void addSeed(GeneticClauseList* clauses, float energyCost, float primaryResourceCost, float maxSize, Color color, std::string primaryResource, unsigned int primaryResourceStorageSize, float energyStorageSize, std::string secondaryResource = "", unsigned int secondaryResourceStorageSize = 500.0f);
		static void addSeedDrop(GeneticClauseList* clauses, float energyPercentage, float energyAmount, float mutation);
		static void addLeafDrop(GeneticClauseList* clauses, float energyPercentage, float sunAmount);

		static void addMaxAmountMutations(GeneticMutationList* mutations, float probability, std::string primaryResource, std::string secondaryResource = "");
		static void addMaxSizeMutations(GeneticMutationList* mutations, float probability);
		static void addColorMutation(GeneticMutationList* mutations, float probability);
		static void addSpawnpointMutations(GeneticMutationList* mutations, float probabilityAngle, float probabilityScale, float probabilityActive);
	public:
		static GeneticCode* generateSimplePlant(long speciesID, unsigned int lifetime, float baseEnergyConsumption = 0.6f, float seedDropEnergyPercentage = 0.95f, std::string primaryResource = "Wasser", std::string secondaryResource = "");
		static GeneticCode* generateRandomSimplePlant();
	};
};

