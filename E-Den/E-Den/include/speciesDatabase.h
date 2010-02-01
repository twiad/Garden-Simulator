// geneticCodeDatabase.h
// by Franz Koehler 2010

#ifndef _E_DEN_SPECIES_DATABASE_HEADER_
#define _E_DEN_SPECIES_DATABASE_HEADER_

#include "globals.h"
#include "geneticCode.h"
#include "organism.h"
#include "tinyxml.h"
#include "runtimeManager.h"
#include <map>

namespace EDen {

  class OneSpeciesDatabase {
  protected:
    std::list<Organism*> orgs;
    TiXmlDocument* doc;
    std::string path;
    std::string name;
    RuntimeManager* runtime;

    bool inited;
    void initEmptyFile(std::string filename);

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
  };

  class SpeciesDatabase {
  protected:
    std::map<int,OneSpeciesDatabase*> species;
    TiXmlDocument* doc;
    std::string path;
    RuntimeManager* runtime;

    bool inited;
    void initEmptyFile(std::string filename);

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
  };

} // namespace

#endif