// geneticCode.cpp
// by Franz Koehler 2009

#include "geneticCode.h"

namespace EDen {

  GeneticCode::GeneticCode(std::string dummyCodeIdentifier) {
    generateDummyGenecode(dummyCodeIdentifier); 
  };

  GeneticCode::GeneticCode(GeneticClauseList param_clauseList, unsigned int param_speciesIdentifier) {
    speciesIdentifier = param_speciesIdentifier;
    clauses = param_clauseList;
  };

  GeneticCode::~GeneticCode() {
    GeneticClause* clause;
    while(!clauses.empty()) {
      clause = clauses.back();
      clauses.pop_back();
      delete clause;
    }; 
  };

  GeneticCode* GeneticCode::copy() {
    GeneticClauseList newClauses;
    for(GeneticClauseListIterator it = clauses.begin(); it != clauses.end(); it++) {
      newClauses.push_back((*it)->copy());
    };

    return new GeneticCode(newClauses,speciesIdentifier);
  };

  bool GeneticCode::setBodypart(Bodypart* p_bp) {
    for(GeneticClauseListIterator it = clauses.begin(); it != clauses.end(); it++) {
      (*it)->setBodypart(p_bp);
    };
    return true;
  };

  bool GeneticCode::addClause(GeneticClause* newClause) {
    clauses.push_back(newClause);
    return true;
  };

  GeneticClauseList GeneticCode::getClauseList() {
    return clauses; 
  };

  unsigned int GeneticCode::getSpeciesIdentifier() {
    return speciesIdentifier; 
  };

  bool GeneticCode::mutate() {
    return false;
  };
};