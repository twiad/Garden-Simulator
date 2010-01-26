// geneticProcessor.h
// by Franz Koehler 2009

#ifndef _E_DEN_GENETIC_CODE_HEADER_
#define _E_DEN_GENETIC_CODE_HEADER_

#include "globals.h"
#include "geneticSystemCore.h"
#include "geneticMutation.h"
#include "tinyxml.h"

namespace EDen {
  class GeneticCode {
    bool generateDummyGenecode(std::string dummyCodeIdentifier);
  protected:
    GeneticClauseList clauses;
    GeneticMutationList possibleMutations;

    int speciesIdentifier;
    int subSpeciesIdentifier;
    int generation;
    
    bool addClause(GeneticClause* clause);
    bool addMutation(GeneticMutation* newMutation);
  public:
    GeneticCode(std::string dummyCodeIdentifier = "TESTPART3");
    GeneticCode(GeneticClauseList param_clauseList, GeneticMutationList possibleMutations, int init_generation = 0, int param_speciesIdentifier = 0, int param_subSpeciesIdentifier = 0);
    GeneticCode(TiXmlElement* description);
    ~GeneticCode();

    int getSpeciesIdentifier();
    int getSubSpeciesIdentifier();
    int getGeneration();
    int incGeneration();
    bool setBodypart(Bodypart* p_bp);

    bool mutate(float strength = 1.0f);
    GeneticClauseList getClauseList();
    GeneticCode* copy();
    TiXmlElement* toXmlElement();
  };

} // namespace

#endif