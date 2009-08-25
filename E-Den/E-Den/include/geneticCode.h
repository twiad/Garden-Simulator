// geneticProcessor.h
// by Franz Koehler 2009

#ifndef _E_DEN_GENETIC_CODE_HEADER_
#define _E_DEN_GENETIC_CODE_HEADER_

#include "globals.h"
#include "geneticSystemCore.h"

namespace EDen {
  class GeneticCode {
    bool generateDummyGenecode(std::string dummyCodeIdentifier);
  protected:
    GeneticClauseList clauses;

    unsigned int speciesIdentifier;
    
    bool addClause(GeneticClause* clause);
  public:
    GeneticCode(std::string dummyCodeIdentifier = "TESTPART3");
    GeneticCode(GeneticClauseList param_clauseList);
    ~GeneticCode();

    unsigned int getSpeciesIdentifier();
    bool setBodypart(Bodypart* p_bp);

    bool mutate();
    GeneticClauseList getClauseList();
  };

} // namespace

#endif