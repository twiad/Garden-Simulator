// geneticCodeDatabase.h
// by Franz Koehler 2010

#ifndef _E_DEN_GENETIC_CODE_DATABASE_HEADER_
#define _E_DEN_GENETIC_CODE_DATABASE_HEADER_

#include "globals.h"
#include "geneticCode.h"
#include "tinyxml.h"
#include <map>

namespace EDen {
  
  class GeneticCodeDatabase {
  protected:
    std::map<int,GeneticCode*> codes;
    TiXmlDocument* doc;
    std::string filename;

  public:
    GeneticCodeDatabase();

    int load(std::string pFilename);
    int save();

    void push(GeneticCode* code);
    GeneticCode* pull();

  };

} // namespace

#endif