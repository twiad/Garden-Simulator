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

namespace EDen {
  class GeneticMutation {
  protected:
    float prob; // probability
  public:
    GeneticMutation(float param_probability);
    ~GeneticMutation();

    float getProbability();
    bool setProbability(float param_prob);
    virtual bool execute(GeneticClause* clause) = 0;
  };

} // namespace

#endif