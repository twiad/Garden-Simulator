// bodypart.h
// by Franz Koehler 2009

#ifndef _E_DEN_BODYPART_HEADER_
#define _E_DEN_BODYPART_HEADER_

#include "chemicalSystem.h"
#include "geneticCode.h"
#include "geneticProcessor.h"
#include "organism.h"
#include "globals.h"
#include "tinyxml.h"

namespace EDen {
  class GeneticCode;
  class GeneticProcessor;
  class Organism;
  
  class SpawnpointInformation {
  public:    
    SpawnpointInformation(): occupied(false),position(0),connectedBodypart(0),ang2d(0.0f),ang2(0.0f),rot(0.0f),scaleModifier(1.0f) {};
    ~SpawnpointInformation();
    BodypartTypeList supportedBpTypes;
    bool occupied;
    int position;
    Bodypart* connectedBodypart;

	bool removeSupportedType(BodypartType param_bpType);
    bool addSupportedType(BodypartType param_bpType);
    bool isSupportedType(BodypartType param_bpType);

    SpawnpointInformation* copy();
    // we will need more information like position and orientation here later
    float ang2d;
    float ang2;
    float rot;
    float scaleModifier;
  };

  class Bodypart {
  protected:
    static unsigned int bodypartCount;
    ChemicalStorage* chemStorage;
    GeneticProcessor* genProcessor;

    BodypartType bpType;
    BodypartState bpState;
    GeneticCode* geneCode;

    SpawnpointInformationList spawnpoints;

    Bodypart* parentBodypart;
    BodypartList childBodyparts;

    Organism* parentOrganism;
    
    bool attachToOrganism(Organism* org);
    
    float healthpoints;
    float size, maxSize, scaleModifier;
    
  public:
    Bodypart(BodypartType bodypartType, std::string geneticCode, Organism* param_parentOrganism = 0, Bodypart* param_parentBodypart = 0);
    Bodypart(BodypartType bodypartType = BPT_UNKNOWN, GeneticCode* param_geneticCode = 0, Organism* param_parentOrganism = 0, Bodypart* param_parentBodypart = 0);
    Bodypart(TiXmlElement* description, Organism* param_parentOrganism = 0, Bodypart* param_parentBodypart = 0);
    ~Bodypart();

    ChemicalStorage* getChemicalStorage();
    GeneticProcessor* getGeneticProcessor();

    bool init();

    Color color;

    GeneticCode* getGeneticCode();
    BodypartType getBodypartType();

    bool setBodypartState(BodypartState param_bpState);
    BodypartState getBodypartState();

    bool update();
    bool destroy();

    bool resetSpawnpoints();
    bool addSpawnpoint(BodypartType param_bpType, int param_position = 0);
    bool addSpawnpoint(SpawnpointInformation* sp);

    virtual bool spawnBodypart(Bodypart* bp, SpawnpointInformation* spawnpointSuggestion = 0, SpawnpointInformation* partnerSpawnpointSuggestion = 0);
    virtual bool spawnPointAvailable(BodypartType bpType = BPT_UNKNOWN);

    ChemicalStorageLink* connectStorageToBodypart(Bodypart* bp);

    bool setParentOrganism(Organism* param_organism);
    Organism* getParentOrganism();
    Bodypart* getParentBodypart();
    BodypartList getChildBodyparts();
    SpawnpointInformationList* getSpawnpoints();
    SpawnpointInformation* getSpawnpointInformationForBodypart(Bodypart* param_bp);

    bool occupieSpawnpoint(Bodypart* param_odypart, SpawnpointInformation* spawnpointSuggestion = 0);

    bool detachFromOrganism();
    bool unregisterChildBodypart(Bodypart* param_childBodypart, bool blockSpawnpointForBodypartType = false);
    Organism* detachToNewOrganism(bool blockSpawnpointForBodypartType = false);
	bool detachToNowhere(bool blockSpawnpointForBodypartType = true);

    float getHealthpoints();
    bool hurt(float damage);
    bool heal(float health);
	bool die();

    float getMaxSize();
    bool setMaxSize(float param_maxSize);
    float getScaleModifier();
    bool setScaleModifier(float param_scaleModifier);
    float getSize();
    bool grow(float amount);
    bool shrink(float amount);

    TiXmlElement* toXmlElement();
    static void spawnpointToXmlElement(SpawnpointInformation* sp, TiXmlElement* element);
    static void xmlElementToSpawnpoint(TiXmlElement* description, SpawnpointInformation* sp);

    static unsigned int getBodypartCount();
  };
} // namespace

#endif