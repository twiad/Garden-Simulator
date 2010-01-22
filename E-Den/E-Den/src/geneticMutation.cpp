// geneticProcessor.cpp
// by Franz Koehler 2009

#include "geneticMutation.h"
#include <iostream>
#include <ctime>

namespace EDen {

  GeneticMutation::GeneticMutation(float param_probability, std::string p_description ) {
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

  bool GeneticMutation::crawl(GeneticAction* p_act,float strength) {
    if(p_act->getActionType() == GAT_Compound) {
      bool retval = false;
      GeneticActionsList actions = ((GeneticCompoundAction*)(p_act))->getChildActions();
      for(GeneticActionsListIterator it = actions.begin(); it != actions.end(); it++) {
        crawl(*it,strength);
      };
      return true;
    } 
    else  {
      return execute(p_act,strength);
    };
  };

  bool GeneticMutation::crawl(GeneticCondition* p_cond,float strength) {
    if(p_cond->getConditionType() == GCT_Compound) {
      bool retval = false;
      GeneticConditionsList conditions = ((GeneticCompoundCondition*)(p_cond))->getChildConditions();
      for(GeneticConditionsListIterator it = conditions.begin(); it != conditions.end(); it++) {
        crawl(*it,strength);
      };
      return true;
    } 
    else  {
      return execute(p_cond,strength);
    };
  };

  bool GeneticMutation::execute(GeneticClause* clause,float strength) {
    bool a = crawl(clause->act,strength);
    bool b = crawl(clause->cond,strength);
    return a && b;
  };

  GeneticMutation* GeneticMutation::parseXmlElement(TiXmlElement* descript) {
    std::string type = descript->ValueStr();
    if(type == "SpawnpointAngle1Mutation") return new GeneticSpawnpoint2DAngleMutation(descript);
    else if(type == "MaxSizeMutation") return new GeneticMaxSizeMutation(descript);
    else if(type == "MaxAmountMutation") return new GeneticMaxAmountMutation(descript);
    else if(type == "SpawnpointActiveMutation") return new GeneticSpawnpointActiveMutation(descript);
    else return 0;
  };

/////////////////////////////////////////////////////////////////////////////////////////

  Randomizer::Randomizer() {
    std::srand((unsigned) time(NULL));
  };

  float Randomizer::value() {
    
    
    float xmpf = ((float)(std::rand() % 100)) / 99.0f;
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
  
  GeneticSpawnpoint2DAngleMutation::GeneticSpawnpoint2DAngleMutation(TiXmlElement* descript) {
    description = descript->Attribute("Description");
    
    descript->QueryFloatAttribute("Min",&min);
    descript->QueryFloatAttribute("Max",&max);
    descript->QueryFloatAttribute("Maxstep",&maxstep);
    descript->QueryFloatAttribute("Probability",&prob);
  };

  bool GeneticSpawnpoint2DAngleMutation::execute(GeneticAction* p_act,float strength) {
    if(randomizer->value() < prob*strength) {
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

  TiXmlElement* GeneticSpawnpoint2DAngleMutation::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("SpawnpointAngle1Mutation");

    element->SetDoubleAttribute("Min",min);
    element->SetDoubleAttribute("Max",max);
    element->SetDoubleAttribute("Maxstep",maxstep);
    element->SetDoubleAttribute("Probability",prob);
    element->SetAttribute("Description",description);

    return element;
  };

  //

  GeneticMaxSizeMutation::GeneticMaxSizeMutation(float p_min, float p_max, float p_maxstep, float p_prob, std::string p_desciption) : GeneticMutation(p_prob,p_desciption) {
    min = p_min;
    max = p_max;
    maxstep = p_maxstep;
  };
  
  GeneticMaxSizeMutation::GeneticMaxSizeMutation(TiXmlElement* descript)  {
    description = descript->Attribute("Description");
    
    descript->QueryFloatAttribute("Min",&min);
    descript->QueryFloatAttribute("Max",&max);
    descript->QueryFloatAttribute("Maxstep",&maxstep);
    descript->QueryFloatAttribute("Probability",&prob);
  };

  bool GeneticMaxSizeMutation::execute(GeneticAction* p_act,float strength) {
    if(randomizer->value() < prob*strength) {
      if(p_act->getActionType() == GAT_ChangeMaxSize) {
        float oldsize = ((GeneticChangeMaxSizeAction*)(p_act))->amount;
        ((GeneticChangeMaxSizeAction*)(p_act))->amount = randomizer->value(maxi<float>(min,oldsize - maxstep),mini<float>(max,oldsize + maxstep));
        return true; 
      };
    }
    return false;
  };

  GeneticMutation* GeneticMaxSizeMutation::copy() {
    return new GeneticMaxSizeMutation(min,max,maxstep,prob);
  };

  TiXmlElement* GeneticMaxSizeMutation::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("MaxSizeMutation");

    element->SetDoubleAttribute("Min",min);
    element->SetDoubleAttribute("Max",max);
    element->SetDoubleAttribute("Maxstep",maxstep);
    element->SetDoubleAttribute("Probability",prob);
    element->SetAttribute("Description",description);

    return element;
  };

   
  GeneticMaxAmountMutation::GeneticMaxAmountMutation(float p_min, float p_max, float p_maxstep, float p_prob, std::string p_chemicalName, std::string p_desciption): GeneticMutation(p_prob,p_desciption) {
    min = p_min;
    max = p_max;
    maxstep = p_maxstep;
    chemName = p_chemicalName;
  };

  GeneticMaxAmountMutation::GeneticMaxAmountMutation(TiXmlElement* descript) {
    description = descript->Attribute("Description");
    chemName = descript->Attribute("ChemicalName");
    
    descript->QueryFloatAttribute("Min",&min);
    descript->QueryFloatAttribute("Max",&max);
    descript->QueryFloatAttribute("Maxstep",&maxstep);
    descript->QueryFloatAttribute("Probability",&prob);
  };

  GeneticMutation* GeneticMaxAmountMutation::copy() {
    return new GeneticMaxAmountMutation(min,max,maxstep,prob,chemName);
  };

  TiXmlElement* GeneticMaxAmountMutation::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("MaxAmountMutation");

    element->SetDoubleAttribute("Min",min);
    element->SetDoubleAttribute("Max",max);
    element->SetDoubleAttribute("Maxstep",maxstep);
    element->SetDoubleAttribute("Probability",prob);
    element->SetAttribute("Description",description);
    element->SetAttribute("ChemicalName",chemName);

    return element;
  };

  bool GeneticMaxAmountMutation::execute(GeneticAction* p_act, float strength) {
    if(randomizer->value() < prob*strength) {
      if(p_act->getActionType() == GAT_ChangeMaxChemAmount) {
        if(((GeneticChangeMaxChemicalAmountAction*)(p_act))->chemName == chemName) {
          float oldsize = ((GeneticChangeMaxChemicalAmountAction*)(p_act))->value;
          ((GeneticChangeMaxChemicalAmountAction*)(p_act))->value = randomizer->value(maxi<float>(min,oldsize - maxstep),mini<float>(max,oldsize + maxstep));
          return true; 
        };
      };
    }
    return false;
  };

  GeneticColorMutation::GeneticColorMutation(float p_min, float p_max, float p_maxstep, float p_prob, std::string p_desciption): GeneticMutation(p_prob,p_desciption) {
    min = p_min;
    max = p_max;
    maxstep = p_maxstep;
  };

  GeneticColorMutation::GeneticColorMutation(TiXmlElement* descript) {
    description = descript->Attribute("Description");
    
    descript->QueryFloatAttribute("Min",&min);
    descript->QueryFloatAttribute("Max",&max);
    descript->QueryFloatAttribute("Maxstep",&maxstep);
    descript->QueryFloatAttribute("Probability",&prob);
  };

  GeneticColorMutation* GeneticColorMutation::copy() {
    return new GeneticColorMutation(min,max,maxstep,prob,description);
  };

  TiXmlElement* GeneticColorMutation::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("ColorMutation");

    element->SetDoubleAttribute("Min",min);
    element->SetDoubleAttribute("Max",max);
    element->SetDoubleAttribute("Maxstep",maxstep);
    element->SetDoubleAttribute("Probability",prob);
    element->SetAttribute("Description",description);

    return element;
  };

  bool GeneticColorMutation::execute(GeneticAction* p_act,float strength) {
    if(randomizer->value() < prob*strength) {
      if(p_act->getActionType() == GAT_SetColor) {
        float colorValue;
        int colorSwitcher = (int)randomizer->value(1,4);
        switch(colorSwitcher) {
          case 1:  colorValue = ((GeneticSetColorAction*)(p_act))->color.r; break;
          case 2:  colorValue = ((GeneticSetColorAction*)(p_act))->color.g; break;
          case 3:  colorValue = ((GeneticSetColorAction*)(p_act))->color.b; break;
          case 4:  colorValue = ((GeneticSetColorAction*)(p_act))->color.a; break;
          default: colorValue = ((GeneticSetColorAction*)(p_act))->color.r;
        };

        float oldvalue = colorValue;
        colorValue = randomizer->value(maxi<float>(min,oldvalue - maxstep),mini<float>(max,oldvalue + maxstep));

        switch(colorSwitcher) {
          case 1:  ((GeneticSetColorAction*)(p_act))->color.r = colorValue; break;
          case 2:  ((GeneticSetColorAction*)(p_act))->color.g = colorValue; break;
          case 3:  ((GeneticSetColorAction*)(p_act))->color.b = colorValue; break;
          case 4:  ((GeneticSetColorAction*)(p_act))->color.a = colorValue; break;
          default: ((GeneticSetColorAction*)(p_act))->color.r = colorValue;
        };

        return true; 
      };
    }
    return false;
  };
  
  GeneticSpawnpointActiveMutation::GeneticSpawnpointActiveMutation(float p_prob, std::string p_desciption): GeneticMutation(p_prob,p_desciption) {
    
  };

  GeneticSpawnpointActiveMutation::GeneticSpawnpointActiveMutation(TiXmlElement* descript) {
    description = descript->Attribute("Description");
    descript->QueryFloatAttribute("Probability",&prob);
  };

  GeneticMutation* GeneticSpawnpointActiveMutation::copy() {
    return new GeneticSpawnpointActiveMutation(prob,description);
  };

  TiXmlElement* GeneticSpawnpointActiveMutation::toXmlElement() {
    TiXmlElement* element;
    element = new TiXmlElement("SpawnpointActiveMutation");

    element->SetDoubleAttribute("Probability",prob);
    element->SetAttribute("Description",description);

    return element;
  };

  bool GeneticSpawnpointActiveMutation::execute(GeneticAction* p_act,float strength) {
    if(randomizer->value() < prob*strength) {
      if(p_act->getActionType() == GAT_AddSpawnpoint) {
        if(((GeneticAddSpawnpointAction*)(p_act))->sp->position != 0) {
          ((GeneticAddSpawnpointAction*)(p_act))->active = !(((GeneticAddSpawnpointAction*)(p_act))->active);
          return true; 
        };
      };
    }
    return false;
  };

}; // namespace