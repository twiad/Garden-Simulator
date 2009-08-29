// geneticProcessor.h
// by Franz Koehler 2009

#ifndef _E_DEN_GENETIC_PROCESSOR_HEADER_
#define _E_DEN_GENETIC_PROCESSOR_HEADER_

#include "geneticSystemCore.h"
#include "geneticSystemActions.h"
#include "geneticSystemConditions.h"
#include "chemicalSystem.h"
#include "chemicalDistributionSystem.h"
#include "bodypart.h"
#include "globals.h"

namespace EDen {
  class Bodypart;
  
  class GeneticProcessor {
    friend class GeneticClause;
  protected:
    ChemicalStorage* chemStorage;
    Bodypart* bodypart;
    // BodypartTypeInheritageMap bodypartTypeInheritage;
    GeneticClauseList relevantClauses;

    GeneticClauseList getDummyGenecode(std::string geneticCode);
    bool addClause(GeneticClause* newClause);
    
    bool mutate();
    //bool removeClauseFromRelevantClauses(GeneticClause* clause);

    //bool removeCreationClauses();
  public:
    GeneticProcessor(Bodypart* nBodypart);

    ~GeneticProcessor();

    Bodypart* getBodypart();

    bool executeRelevantClauses();
    bool updateGeneticCode();
  };
} // namespace

#endif