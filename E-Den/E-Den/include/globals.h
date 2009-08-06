// globals.h
// by Franz Koehler 2009

#ifndef _E_DEN_GLOBALS_HEADER_
#define _E_DEN_GLOBALS_HEADER_

#include <list>

namespace EDen {
  class Organism;
  class Bodypart;
  class SpawnpointInformation;

  typedef std::list<SpawnpointInformation*> SpawnpointInformationList;
  typedef std::list<SpawnpointInformation*>::iterator SpawnpointInformationListIterator;

  enum GeneticChemicalConditionType {
    GCC_current_value_more = 1,
    GCC_current_value_less,
    GCC_current_value_equal,
    GCC_space_left_more,
    GCC_space_left_less,
    GCC_space_left_equal,
    GCC_percentage_more,
    GCC_percentage_less,
    GCC_percentage_equal,
    GCC_unknown
  };

  enum GeneticStdConditionType {
    GBT_equal = 1,
    GBT_notEqual,
    GBT_more,
    GBT_less,
    GBT_UNKNOWN
  };

  enum GeneticConditionType {
    GCT_Compound = 1,
    GCT_Chemical,
    GCT_BodypartType,
    GCT_BodypartState,
    GCT_BodypartCreation,
    GCT_BodypartHP,
    GCT_BodypartSize,
    GCT_SpawnpointPresent,
    GCT_UNKNOWN
  };

  enum GeneticActionType {
    GAT_Compound = 1,
    GAT_SimpleConvert,
    GAT_ChemicalConsume,
    GAT_DualConvert,
    GAT_SplitConvert,
    GAT_Hurt,
    GAT_Heal,
    GAT_Grow,
    GAT_Shrink,
    GAT_EmptyStorage,
    
// This actions should be used during creation (or maybe growth action combinations)   
    GAT_ChangeMaxChemAmount,
    GAT_ChangeMaxSize,
    GAT_AddSpawnpoint,
    GAT_SpawnBP,

// and the last and least
    GAT_Die,
    GAT_UNKNOWN
  };

  enum BodypartType {
    BPT_Stick = 1,
    BPT_Leaf,
    BPT_Branch,
    BPT_Seed,
    BPT_UNKNOWN
  };

  enum BodypartState {
    BSP_creation = 1,
    BSP_alive,
    BSP_dead,
    BSP_UNKNOWN
  };

  typedef BodypartState OrganismState;
};

#endif
