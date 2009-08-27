// geneticProcessor.cpp
// by Franz Koehler 2009

#include "geneticProcessor.h"

namespace EDen {
  GeneticProcessor::GeneticProcessor(Bodypart* nBodypart){
    bodypart = nBodypart;
    chemStorage = bodypart->getChemicalStorage();
  };

  GeneticProcessor::~GeneticProcessor() {
    GeneticClause* clause;
    while(!relevantClauses.empty()) {
      clause = relevantClauses.back();
      relevantClauses.pop_back();
      // delete clause;
    };
    
    // relevantClauses.clear();
  };

  bool GeneticProcessor::addClause(GeneticClause* newClause) {
    relevantClauses.push_back(newClause);
    return true;
  };

  //bool GeneticProcessor::removeClauseFromRelevantClauses(GeneticClause* clause) {
  //  relevantClauses.remove(clause);
  //  return true; 
  //};

  Bodypart* GeneticProcessor::getBodypart() {
    return bodypart; 
  };

  bool GeneticProcessor::executeRelevantClauses() {
    if(bodypart->getBodypartState() != BSP_dead) {  
      GeneticClauseList nextRelevantClauses;
      
      while(!relevantClauses.empty()) {
        relevantClauses.back()->run();
        if(!relevantClauses.back()->deleteMe)
          nextRelevantClauses.push_front(relevantClauses.back());
        //else 
        //  delete relevantClauses.back();

        relevantClauses.pop_back();
      }

      relevantClauses.clear();
      relevantClauses.swap(nextRelevantClauses);

      return true;
    }
    else { // BP is dead :_(
      if(bodypart->getChildBodyparts().empty())
        bodypart->getParentOrganism()->deleteBodypart(bodypart);
      return false; 
    }
  };

  bool GeneticProcessor::updateGeneticCode() {
    
    GeneticClause* clause;
    while(!relevantClauses.empty()) {
      clause = relevantClauses.back();
      relevantClauses.pop_back();
      // delete clause;
    };

//    bodypart->getGeneticCode()->setBodypart(bodypart);
    GeneticClauseList originalClauses = bodypart->getGeneticCode()->getClauseList();

    relevantClauses.assign(originalClauses.begin(),originalClauses.end());

    return true;
  };
} // namespace