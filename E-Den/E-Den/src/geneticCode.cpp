// geneticCode.cpp
// by Franz Koehler 2009

#include "geneticCode.h"

namespace EDen {

  GeneticCode::GeneticCode(std::string dummyCodeIdentifier) {
    generateDummyGenecode(dummyCodeIdentifier); 
  };

  GeneticCode::GeneticCode(GeneticClauseList param_clauseList, GeneticMutationList p_mutations, unsigned int param_speciesIdentifier) {
    speciesIdentifier = param_speciesIdentifier;
    clauses = param_clauseList;
    possibleMutations = p_mutations;
  };

  GeneticCode::~GeneticCode() {
    GeneticClause* clause;
    while(!clauses.empty()) {
      clause = clauses.back();
      clauses.pop_back();
      delete clause;
    }; 

    GeneticMutation* mutation;
    while(!possibleMutations.empty()) {
      mutation = possibleMutations.back();
      possibleMutations.pop_back();
      delete mutation;
    };
  };

  GeneticCode* GeneticCode::copy() {
    GeneticClauseList newClauses;
    for(GeneticClauseListIterator it = clauses.begin(); it != clauses.end(); it++) {
      newClauses.push_back((*it)->copy());
    };
  
    GeneticMutationList newMutations;
    for(GeneticMutationListIterator it = possibleMutations.begin(); it != possibleMutations.end(); it++) {
      newMutations.push_back((*it)->copy());
    };

    return new GeneticCode(newClauses,newMutations,speciesIdentifier);
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

  bool GeneticCode::mutate(float strength) {
    // for all clauses, for all possible mutations do mutation.execute(clause)
    for(GeneticClauseListIterator clause = clauses.begin(); clause != clauses.end(); clause++) {
      for(GeneticMutationListIterator mutation = possibleMutations.begin(); mutation != possibleMutations.end(); mutation++) {
        (*mutation)->execute(*clause,strength);
      };
    };
    
    return false;
  };
};