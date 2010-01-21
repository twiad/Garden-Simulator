// geneticCode.cpp
// by Franz Koehler 2009

#include "geneticCode.h"
#include "geneticMutation.h"

namespace EDen {

  GeneticCode::GeneticCode(std::string dummyCodeIdentifier) {
    generateDummyGenecode(dummyCodeIdentifier); 
  };

  GeneticCode::GeneticCode(GeneticClauseList param_clauseList, GeneticMutationList p_mutations, int param_speciesIdentifier, int param_subSpeciesIdentifier) {
    speciesIdentifier = param_speciesIdentifier;
    clauses = param_clauseList;
    possibleMutations = p_mutations;
  };

  GeneticCode::GeneticCode(TiXmlElement* description) {
    TiXmlElement* it;
    
    description->QueryIntAttribute("SpeciesID",&speciesIdentifier);
    description->QueryIntAttribute("SubSpeciesID",&subSpeciesIdentifier);
    
    it = description->FirstChildElement("Clauses");
    for(it = it->FirstChildElement("Clause"); it != 0; it = it->NextSiblingElement()) {
      addClause(new GeneticClause(it));
    };

    it = description->FirstChildElement("Mutations");
    for(it = it->FirstChildElement(); it != 0; it = it->NextSiblingElement()) {
      addMutation(GeneticMutation::parseXmlElement(it));
    };
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

  bool GeneticCode::addMutation(GeneticMutation* newMutation) {
    possibleMutations.push_back(newMutation);
    return true;
  };

  GeneticClauseList GeneticCode::getClauseList() {
    return clauses; 
  };

  int GeneticCode::getSpeciesIdentifier() {
    return speciesIdentifier; 
  };

  int GeneticCode::getSubSpeciesIdentifier() {
    return subSpeciesIdentifier; 
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

  TiXmlElement* GeneticCode::toXmlElement() {
    TiXmlElement* element,*clausesEl,*mutations;

    element = new TiXmlElement("GeneticCode");
    element->SetAttribute("SpeciesID",speciesIdentifier);
    element->SetAttribute("SubSpeciesID",subSpeciesIdentifier);
    
    clausesEl = new TiXmlElement("Clauses");
    for(GeneticClauseListIterator it = clauses.begin(); it != clauses.end(); it++) {
      clausesEl->LinkEndChild((*it)->toXmlElement());
    };
    element->LinkEndChild(clausesEl);

    mutations = new TiXmlElement("Mutations");
    for(GeneticMutationListIterator it = possibleMutations.begin(); it != possibleMutations.end(); it++) {
      mutations->LinkEndChild((*it)->toXmlElement());
    };
    element->LinkEndChild(mutations);

    return element;
  };

};