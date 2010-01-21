// geneticCodeDatabase.h
// by Franz Koehler 2010

#ifndef _E_DEN_GENETIC_CODE_DATABASE_HEADER_
#define _E_DEN_GENETIC_CODE_DATABASE_HEADER_

#include "globals.h"
#include "geneticCode.h"
#include "organism.h"
#include "tinyxml.h"
#include "runtimeManager.h"
#include <map>

namespace EDen {

  class GeneticCodeDatabase {
  protected:
    std::list<Organism*> orgs;
    TiXmlDocument* doc;
    std::string filename;
    RuntimeManager* runtime;

  public:
    GeneticCodeDatabase(RuntimeManager* p_runtime);
    bool empty();
    int size();

    int load(std::string pFilename);
    int save();

    void push(Organism* org);
    Organism* pull();

  };

} // namespace

#endif