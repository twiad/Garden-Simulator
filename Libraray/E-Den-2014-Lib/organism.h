// organism.h
// by Franz Koehler 2009

#ifndef _E_DEN_ORGANISM_HEADER_
#define _E_DEN_ORGANISM_HEADER_

#include "bodypart.h"
#include "groundpart.h"
#include "runtimeManager.h"

#include "tinyxml.h"

namespace EDen {
  class Bodypart;
  class GeneticProcessor;
  class Groundpart;
  class RuntimeManager;

  typedef std::list<ChemicalStorageLink*> ChemicalStorageLinkList;
  typedef std::list<ChemicalStorageLink*>::iterator ChemicalStorageLinkListIterator;

  typedef std::list<GeneticProcessor*> GeneticProcessorList;
  typedef std::list<GeneticProcessor*>::iterator GeneticProcessorListIterator;

  typedef std::list<Bodypart*> BodypartList;
  typedef std::list<Bodypart*>::iterator BodypartListIterator;

  class Organism {
  protected:
    std::string name;
    int lifetime;

    Bodypart* rootBodypart;
    Groundpart* groundpart;
    RuntimeManager* runtimeManager;


    BodypartList bodyparts;
    BodypartList bodypartsToDelete;
    ChemicalStorageLinkList storageLinks;
    GeneticProcessorList geneticProcessors;

    bool removeChemStorageLinksWithBodypart(Bodypart* param_bodypart);
    bool removeChemStorageLinksWithGroundpart(Groundpart* param_groundpart);
    bool addGeneticProcessor(GeneticProcessor* param_geneticProcessor);
    bool removeGeneticProcessorWithBodypart(Bodypart* param_bodypart);

  public:
    Organism(std::string param_name, Bodypart* param_rootBodypart, RuntimeManager* param_runtimeManager = 0);
    Organism(TiXmlElement* description, RuntimeManager* param_runtimeManager = 0);
    ~Organism();

    std::string getName();
    OrganismState getState();
    int getLifetime();
    bool incLifetime();

#ifdef USE_CL
    bool updateChemicalStorageLinks(CLDriver* driver);
    bool updateGeneticProcessors(CLDriver* driver);
    bool updateDelete();
    bool update(CLDriver* driver);
#else
    bool updateChemicalStorageLinks();
    bool updateGeneticProcessors();
    bool updateDelete();
    bool update();
#endif
    
    bool registerBodypart(Bodypart* param_bodypart);
    bool unregisterBodypart(Bodypart* param_bodypart);
    bool deleteBodypart(Bodypart* param_bodypart);
    int getBodypartCount();
    RuntimeManager* getRuntimeManager();
    bool addChemicalStorageLink(ChemicalStorageLink* param_chemLink);
    
    bool connectToGoundpart(Groundpart* param_groundpart);
    bool connectedToGoundpart();
	Groundpart* getGroundpart();

    Bodypart* getRootBodypart();

	int getFitnessValue();

    TiXmlElement* getXmlElement();
  };
}

#endif