// geneticProcessor.cpp
// by Franz Koehler 2009

#include "geneticMutation.h"
#include <iostream>
#include <ctime>

namespace EDen {

  GeneticMutation::GeneticMutation(float param_probability, std::string p_description) {
    prob = param_probability; 
    description = p_description;
    randomizer = new Randomizer();
  };

  GeneticMutation::~GeneticMutation() {
    delete randomizer;
  };

  float GeneticMutation::getProbability() {
    return prob;
  };

  bool GeneticMutation::setProbability(float param_prob) {
    if((param_prob >= 0) && (param_prob <= 1.0)) {
      prob = param_prob;
      return true;
    } else return false;
  };

  bool GeneticMutation::crawl(GeneticAction* p_act) {
    if(p_act->getActionType() == GAT_Compound) {
      bool retval = false;
      GeneticActionsList actions = ((GeneticCompoundAction*)(p_act))->getChildActions();
      for(GeneticActionsListIterator it = actions.begin(); it != actions.end(); it++) {
        crawl(*it);
      };
      return true;
    } 
    else  {
      return execute(p_act);
    };
  };

  bool GeneticMutation::crawl(GeneticCondition* p_cond) {
    if(p_cond->getConditionType() == GCT_Compound) {
      bool retval = false;
      GeneticConditionsList conditions = ((GeneticCompoundCondition*)(p_cond))->getChildConditions();
      for(GeneticConditionsListIterator it = conditions.begin(); it != conditions.end(); it++) {
        crawl(*it);
      };
      return true;
    } 
    else  {
      return execute(p_cond);
    };
  };

  bool GeneticMutation::execute(GeneticClause* clause) {
    bool a = crawl(clause->act);
    bool b = crawl(clause->cond);
    return a && b;
  };

/////////////////////////////////////////////////////////////////////////////////////////

  Randomizer::Randomizer() {
    std::srand((unsigned) time(NULL));
  };

  float Randomizer::value() {
    
    
    float xmpf = ((float)(std::rand() % 1000)) / 999.0f;
    // std::cout << " rnd: " << xmpf << std::endl;
    return xmpf;
  }; // returns a value betwien 0 and 1 (possibly including both of them)
  
  float Randomizer::value(float min, float max) {
    return min + (value()*(max-min));
  }; 


  //////////////////////////////////////////////////////////////////

  GeneticSpawnpoint2DAngleMutation::GeneticSpawnpoint2DAngleMutation(float p_min, float p_max, float p_maxstep, float p_prob, std::string p_desciption) : GeneticMutation(p_prob,p_desciption) {
    min = p_min;
    max = p_max;
    maxstep = p_maxstep;
  };
  

  bool GeneticSpawnpoint2DAngleMutation::execute(GeneticAction* p_act) {
    if(randomizer->value() < prob) {
      if(p_act->getActionType() == GAT_AddSpawnpoint) {
        SpawnpointInformation* sp = ((GeneticAddSpawnpointAction*)(p_act))->sp;
        if(sp->position != 0) {
          float oldang = sp->ang2d;
          sp->ang2d = randomizer->value(maxi<float>(min,oldang - maxstep),mini<float>(max,oldang + maxstep));
        };
      };
      return true; 
    }
    return false;
  };

  GeneticMutation* GeneticSpawnpoint2DAngleMutation::copy() {
    return new GeneticSpawnpoint2DAngleMutation(min,max,maxstep,prob);
  };

}; // namespace