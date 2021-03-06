// geneticCodeDatabase.h
// by Franz Koehler 2010

#ifndef _E_DEN_SPECIES_DATABASE_HEADER_
#define _E_DEN_SPECIES_DATABASE_HEADER_

#include "globals.h"
#include "geneticCode.h"
#include "organism.h"
#include "tinyxml.h"
#include "runtimeManager.h"
#include <boost/thread.hpp>
#include <map>

namespace EDen {

  class OneSpeciesDatabase {
  protected:
    std::list<Organism*> orgs;
    std::list<Organism*> candidates;
    TiXmlDocument* doc;
    std::string path;
    std::string name;
    RuntimeManager* runtime;
    boost::mutex mutex;

    bool inited;
    void initEmptyFile(std::string filename);

    int treshold;
    int maxCandidates;
	int maxSize;
    void setTreshold(int p_trehold);

    bool changedSinceLastUpdate;

	void removeLeastPerformantOrganism();

  public:
    OneSpeciesDatabase(RuntimeManager* p_runtime);
    bool empty();
    int size();
    void clear();

    int load(std::string pFilename);
    int save(std::string p_filename = "");
    void setName(std::string p_name);
    std::string getName();
    void setApplicationSettingsPath(std::string appSettingsPath);

    void push(Organism* org);
    Organism* pull(bool random = true, bool del = true);

	int getMaxSize();
	void setMaxSize(int newMaxSize);

    int getTreshold();
    void updateTreshold();
    void setMaxCandidates(int p_maxCandidates);
    int getMaxCandidates();

    std::string getDebugOut();
  };

  class SpeciesDatabase {
  protected:
    std::map<unsigned int,OneSpeciesDatabase*> species;
    TiXmlDocument* doc;
    std::string path;
    RuntimeManager* runtime;
    boost::mutex mutex;

    bool inited;
    void initEmptyFile(std::string filename);
    
    int getSpeciesIdWithLowestCount();
    int getSpeciesIdWithHighestCount();
    
    bool speciesSelectionAlternater;
    bool speciesSelectionAlternater2;

    int getNextSpeciesId();
    int lastSpeciesId;

  public:
    SpeciesDatabase(RuntimeManager* p_runtime);
    bool empty();
    int size();
    void clear();

    int load(std::string pFilename);
    int save(std::string p_filename = "");
    void setApplicationSettingsPath(std::string appSettingsPath);

    void push(Organism* org);
    Organism* pull(int speciesId = 0, bool random = true, bool del = true);

    int getSpeciesCount();
    void update();

    std::string getDebugOut();
	void getDebugOut(std::map<unsigned int,std::string>* singleSpeciesOutputs);
  };

} // namespace

#endif