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

  class SpeciesDatabase {
  protected:
    std::list<Organism*> orgs;
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
    Organism* pull(bool del = true);
  };

} // namespace

#endif