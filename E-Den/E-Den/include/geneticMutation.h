// geneticMutation.h
// by Franz Koehler 2009

#ifndef _E_DEN_GENETIC_MUTATION_HEADER_
#define _E_DEN_GENETIC_MUTATION_HEADER_

#include <list>

namespace EDen {
  class GeneticMutation;
  typedef std::list<GeneticMutation*> GeneticMutationList;
  typedef std::list<GeneticMutation*>::iterator GeneticMutationListIterator; 
};

#include "globals.h"
#include "geneticSystemCore.h"
#include "geneticSystemActions.h"

namespace EDen {
  class Randomizer {
  public:
    Randomizer();

    float value(); // returns a value betwien 0 and 1 (possibly including both of them)
    float value(float min, float max); 
  };

//  namespace Mutations {
    class GeneticMutation {
    protected:
      float prob; // probability
      std::string description;
      Randomizer* randomizer; // should point to a more central unit in future versions
      // TODO: possible performance improvement if mutations remember their clauses ... management overhead for now (and mutations do not happen that often ...)

      bool crawl(GeneticAction* p_act);
      bool crawl(GeneticCondition* p_cond);
      virtual bool execute(GeneticAction* p_act) {return false;};
      virtual bool execute(GeneticCondition* p_cond) {return false;};
    public:
      GeneticMutation(float param_probability, std::string p_description = "No Description ...");
      ~GeneticMutation();

      float getProbability();
      bool setProbability(float param_prob);
      bool execute(GeneticClause* clause);
      virtual GeneticMutation* copy() = 0;
    };

    class GeneticSpawnpoint2DAngleMutation : public GeneticMutation {
      friend class GeneticAddSpawnpointAction;
      float min,max,maxstep;
    protected:
      virtual bool execute(GeneticAction* p_act);
    public:
      GeneticSpawnpoint2DAngleMutation(float p_min, float p_max, float p_maxstep, float p_prob, std::string p_desciption = "2DAngleMutation");
      virtual GeneticMutation* copy();
    };

    class GeneticMaxSizeMutation : public GeneticMutation {
    friend class GeneticChangeMaxSizeAction;
    
      float min,max,maxstep;
    protected:
      virtual bool execute(GeneticAction* p_act);
    public:
      GeneticMaxSizeMutation(float p_min, float p_max, float p_maxstep, float p_prob, std::string p_desciption = "MaxSizeMutation");
      virtual GeneticMutation* copy();
    };


//  } // namespace Mutations

} // namespace EDen

#endif