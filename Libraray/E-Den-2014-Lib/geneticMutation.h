// geneticMutation.h
// by Franz Koehler 2009

#ifndef _E_DEN_GENETIC_MUTATION_HEADER_
#define _E_DEN_GENETIC_MUTATION_HEADER_

#include <list>

namespace EDen {
  class GeneticMutation;
  class Randomizer;
  typedef std::list<GeneticMutation*> GeneticMutationList;
  typedef std::list<GeneticMutation*>::iterator GeneticMutationListIterator; 
};

#include "globals.h"
#include "geneticSystemCore.h"
#include "geneticSystemActions.h"
#include "tinyxml.h"

namespace EDen {
  //  namespace Mutations {
  class GeneticMutation {
  protected:
    float prob; // probability
    std::string description;
    Randomizer* randomizer; // should point to a more central unit in future versions
    // TODO: possible performance improvement if mutations remember their clauses ... management overhead for now (and mutations do not happen that often ...)

    bool crawl(GeneticAction* p_act,float strength);
    bool crawl(GeneticCondition* p_cond,float strength);
    virtual bool execute(GeneticAction* p_act,float strength) {return false;};
    virtual bool execute(GeneticCondition* p_cond,float strength) {return false;};
  public:
    GeneticMutation(float param_probability = 0.1f, std::string p_description = "No Description ...");
    ~GeneticMutation();

    float getProbability();
    bool setProbability(float param_prob);
    bool execute(GeneticClause* clause,float strength);
    virtual GeneticMutation* copy() = 0;
    virtual TiXmlElement* toXmlElement() = 0;
    static GeneticMutation* parseXmlElement(TiXmlElement* description);
  };

  class GeneticSpawnpoint2DAngleMutation : public GeneticMutation {
    friend class GeneticAddSpawnpointAction;
    float min,max,maxstep;
  protected:
    virtual bool execute(GeneticAction* p_act,float strength);
  public:
    GeneticSpawnpoint2DAngleMutation(float p_min, float p_max, float p_maxstep, float p_prob, std::string p_desciption = "2DAngleMutation");
    GeneticSpawnpoint2DAngleMutation(TiXmlElement* descript);
    virtual GeneticMutation* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticSpawnpointScaleModifierMutation : public GeneticMutation {
    friend class GeneticAddSpawnpointAction;
    float min,max,maxstep;
  protected:
    virtual bool execute(GeneticAction* p_act,float strength);
  public:
    GeneticSpawnpointScaleModifierMutation(float p_min, float p_max, float p_maxstep, float p_prob, std::string p_desciption = "ScaleModifierMutation");
    GeneticSpawnpointScaleModifierMutation(TiXmlElement* descript);
    virtual GeneticMutation* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticMaxSizeMutation : public GeneticMutation {
    friend class GeneticChangeMaxSizeAction;

    float min,max,maxstep;
  protected:
    virtual bool execute(GeneticAction* p_act,float strength);
  public:
    GeneticMaxSizeMutation(float p_min, float p_max, float p_maxstep, float p_prob, std::string p_desciption = "MaxSizeMutation");
    GeneticMaxSizeMutation(TiXmlElement* descript);
    virtual GeneticMutation* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticMaxAmountMutation : public GeneticMutation {
    friend class GeneticChangeMaxChemicalAmountAction;

    float min,max,maxstep;
    std::string chemName;
  protected:
    virtual bool execute(GeneticAction* p_act,float strength);
  public:
    GeneticMaxAmountMutation(float p_min, float p_max, float p_maxstep, float p_prob, std::string p_chemicalName, std::string p_desciption = "MaxSizeMutation");
    GeneticMaxAmountMutation(TiXmlElement* descript);
    virtual GeneticMutation* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticColorMutation : public GeneticMutation {
    friend class GeneticSetColorAction;

    float min,max,maxstep;
  protected:
    virtual bool execute(GeneticAction* p_act,float strength);
  public:
    GeneticColorMutation(float p_min, float p_max, float p_maxstep, float p_prob, std::string p_desciption = "ColorMutation");
    GeneticColorMutation(TiXmlElement* descript);
    virtual GeneticColorMutation* copy();
    virtual TiXmlElement* toXmlElement();
  };

  class GeneticSpawnpointActiveMutation : public GeneticMutation {
    friend class GeneticAddSpawnpointAction;
  protected:
    virtual bool execute(GeneticAction* p_act,float strength);
  public:
    GeneticSpawnpointActiveMutation(float p_prob, std::string p_desciption = "SpawnpointActiveMutation");
    GeneticSpawnpointActiveMutation(TiXmlElement* descript);
    virtual GeneticMutation* copy();
    virtual TiXmlElement* toXmlElement();
  };
  //  } // namespace Mutations

} // namespace EDen

#endif