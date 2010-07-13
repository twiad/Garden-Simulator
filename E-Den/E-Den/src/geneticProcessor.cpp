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
    BodypartState state = bodypart->getBodypartState();
    
    if(state != BSP_dead) {  
      GeneticClauseList nextRelevantClauses;
      
      if(state == BSP_seed) {
        Bodypart* bp = new Bodypart(BPT_Stick,bodypart->getGeneticCode()->copy(),bodypart->getParentOrganism());

        bodypart->setBodypartState(BSP_creation);
        executeRelevantClauses();
        bodypart->resetSpawnpoints();
  
        SpawnpointInformation* sp = new SpawnpointInformation();
        sp->position = 1;
        sp->addSupportedType(BPT_Stick);
        sp->ang2d = 180.0f;
        bodypart->addSpawnpoint(sp);

        if(!bodypart->spawnBodypart(bp)) {
          delete bp; 
        } 
        else { 
          SpawnpointInformationList spi = bp->getSpawnpoints();
          for(SpawnpointInformationListIterator it = spi.begin(); it != spi.end(); it++) {
            if((*it)->connectedBodypart == bodypart) {
              (*it)->position = 0;
            }
          }
        }

        bodypart->setBodypartState(BSP_alive);
      };

      static int bla = 0; //debug
      while(!relevantClauses.empty()) {
        GeneticClause* clause = relevantClauses.back();
        //if(clause->description == "Spawn Seed")
        //  bla += 1;
        clause->run();
        //deleteMe is not used at the moment
        //if(!relevantClauses.back()->deleteMe)

        //if(clause == relevantClauses.back()) {
          nextRelevantClauses.push_front(clause);
          relevantClauses.pop_back();
        //}
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

    GeneticClauseList originalClauses = bodypart->getGeneticCode()->getClauseList();

    // here we want to sort out irrelevant clauses ... that are those ones which depend on a bodypartTypeCondition which is unfullfilled

    for(GeneticClauseListIterator it = originalClauses.begin(); it != originalClauses.end(); it++) {
      if(!( (*it)->dependsOnUnfullfilledConditionType(GCT_BodypartType) ||
            (*it)->dependsOnUnfullfilledConditionType(GCT_BodypartCreation) )) {
        relevantClauses.push_back(*it);
      };
    };

    // no filter:
    //    relevantClauses.assign(originalClauses.begin(),originalClauses.end());

    return true;
  };

  bool GeneticProcessor::mutate() {
    return bodypart->getGeneticCode()->mutate(); 
  };

  //bool GeneticProcessor::removeCreationClauses() {
  //  GeneticClauseList nextRelevantClauses;
  //    
  //  while(!relevantClauses.empty()) {
  //    relevantClauses.back()->run();
  //    if(!relevantClauses.back()->dependsOnUnfullfilledConditionType(GCT_BodypartCreation))
  //      nextRelevantClauses.push_front(relevantClauses.back());
  //    //else 
  //    //  delete relevantClauses.back();

  //    relevantClauses.pop_back();
  //  }

  //  relevantClauses.clear();
  //  relevantClauses.swap(nextRelevantClauses);

  //  return true;
  //};
} // namespace