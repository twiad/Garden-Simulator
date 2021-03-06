// globals.h
// by Franz Koehler 2009

#ifndef _E_DEN_GLOBALS_HEADER_
#define _E_DEN_GLOBALS_HEADER_

#include "stdafx.h"
#include <list>
#include <map>
#include <boost/thread.hpp> 

template <class T> const T& maxi ( const T& a, const T& b ) {
  return (b<a)?a:b;     // or: return comp(b,a)?a:b; for the comp version
}

template <class T> const T& mini ( const T& a, const T& b ) {
  return (b>a)?a:b;     
}

namespace EDen {
  class Organism;
  class Bodypart;
  class Groundpart;
  class SpawnpointInformation;
  class SpeciesDatabase;
  class GeneticCode;
  class RuntimeManager;

  typedef std::list<SpawnpointInformation*> SpawnpointInformationList;
  typedef std::list<SpawnpointInformation*>::iterator SpawnpointInformationListIterator;
  
  typedef std::list<Bodypart*> BodypartList;
  typedef std::list<Bodypart*>::iterator BodypartListIterator;

  typedef std::list<Groundpart*> GroundpartList;
  typedef std::list<Groundpart*>::iterator GroundpartListIterator;
  
  class Randomizer {
  private: 
	static bool inited;
	static boost::mutex randomizerMutex;
  public:
    Randomizer();

    static float value(); // returns a value betwien 0 and 1 (possibly including both of them)
    static float value(float min, float max); 
  };

  template<class T>
  inline void list_swap(std::list<T>& li, unsigned a, unsigned b)
  {
	if (a && b && a != b && a <= li.size() && b <= li.size())
	{
		std::list<T>::iterator it_a, it_b = it_a = li.begin();

		std::advance(it_a, --a);
		std::advance(it_b, --b);

		std::iter_swap(it_a, it_b);
	}
  }

  template<class T>
  inline void list_softShuffle(std::list<T>& li)
  {
	unsigned size = li.size();
	if(size == 2) {
		list_swap(li,0,1);
	}
	else if(size > 2 && size < 10) {
		int a, b;
		for(int i = size / 2; i >= 0; i--) {
			a = Randomizer::value(0,size-1);
			b = Randomizer::value(0,size-1);
			if(a != b) {
				list_swap(li,a,b);
			}
		};
	}
	else if(size > 10) {
		int a, b;
		for(int i = (int)sqrt(size); i >= 0; i--) {
			a = Randomizer::value(0,size-1);
			b = Randomizer::value(0,size-1);
			if(a != b) {
				list_swap(li,a,b);
			}
		};
	}
  }

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
	GBT_percentage_more,
	GBT_percentage_less,
    GBT_UNKNOWN
  };

  enum GeneticConditionType {
    GCT_Compound = 1,
    GCT_Chemical,
    GCT_BodypartType,
	GCT_ParentBodypartType,
    GCT_BodypartState,
    GCT_BodypartCreation,
    GCT_BodypartHP,
    GCT_BodypartSize,
    GCT_SpawnpointPresent,
	GCT_ParentSpawnpointPresent,
    GCT_HasParent,
    GCT_OrganismLifetime,
    GCT_UNKNOWN
  };

  enum GeneticActionType {
    GAT_Compound = 10,
    GAT_SimpleConvert = 20,
    GAT_ChemicalConsume = 30,
    GAT_DualConvert = 40,
    GAT_SplitConvert = 50,
    GAT_Hurt = 60,
    GAT_Heal = 70,
    GAT_Grow = 80,
    GAT_Shrink = 90,
    GAT_EmptyStorage = 100,
    GAT_DropSeed = 110,
	GAT_DropLeaf = 111,
    GAT_Mutate = 120,
    
// This actions should be used during creation (or maybe growth action combinations)   
    GAT_ChangeMaxChemAmount = 500,
    GAT_ChangeMaxSize = 510,
    GAT_AddSpawnpoint = 520,
    GAT_SpawnBP = 530,
	GAT_SpawnParentBP = 531,
    GAT_SetColor = 540,

// and the last and least
    GAT_Die = 5000,
    GAT_UNKNOWN = 0
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
    BSP_seed,
    BSP_UNKNOWN
  };

  enum RuntimeManagerState {
    RMS_Normal = 1,
    RMS_Slow,
    RMS_Pause,
    RMS_Unknown
  };

  typedef BodypartState OrganismState;
  typedef std::list<BodypartType> BodypartTypeList;
  typedef std::list<BodypartType>::iterator BodypartTypeListIterator;

  class GeneticClause;
  typedef std::list<GeneticClause*> GeneticClauseList;
  typedef std::list<GeneticClause*>::iterator GeneticClauseListIterator;

  typedef std::map<BodypartType,BodypartType> BodypartTypeInheritageMap;
  typedef std::map<BodypartType,BodypartType>::iterator BodypartTypeInheritageMapIterator;

  class Color {
  public:
    Color(float red = 0.5f, float green = 0.5f, float blue = 0.5f, float alpha = 1.0f): r(red), g(green), b(blue), a(alpha) {};
    float r,g,b,a;
  };

  void wait(int seconds);
};

#endif
