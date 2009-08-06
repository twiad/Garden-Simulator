// geneticProcessor.h
// by Franz Koehler 2009

#ifndef _E_DEN_GENETIC_PROCESSOR_HEADER_
#define _E_DEN_GENETIC_PROCESSOR_HEADER_

#include "geneticSystemCore.h"
#include "geneticSystemActions.h"
#include "geneticSystemConditions.h"
#include "chemicalSystem.h"
#include "bodypart.h"
#include "globals.h"

namespace EDen {
  class Bodypart;
  
  class GeneticProcessor {
    friend class GeneticClause;
  protected:
    typedef std::list<GeneticClause*> GeneticClauseList;
    typedef std::list<GeneticClause*>::iterator GeneticClauseListIterator;

    typedef std::map<BodypartType,BodypartType> BodypartTypeInheritageMap;
    typedef std::map<BodypartType,BodypartType>::iterator BodypartTypeInheritageMapIterator;

    ChemicalStorage* chemStorage;
    Bodypart* bodypart;
    // BodypartTypeInheritageMap bodypartTypeInheritage;
    GeneticClauseList relevantClauses;

    bool addClause(GeneticClause* newClause);
    //bool removeClauseFromRelevantClauses(GeneticClause* clause);
  public:
    GeneticProcessor(Bodypart* nBodypart);

    ~GeneticProcessor();

    Bodypart* getBodypart();

    bool executeRelevantClauses();
    bool parseGeneticCode();
  };
} // namespace

#endif