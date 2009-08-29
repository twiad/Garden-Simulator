// geneticProcessor.cpp
// by Franz Koehler 2009

#include "geneticMutation.h"

namespace EDen {

  GeneticMutation::GeneticMutation(float param_probability) {
    prob = param_probability; 
  };

  GeneticMutation::~GeneticMutation() {
    
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

}; // namespace