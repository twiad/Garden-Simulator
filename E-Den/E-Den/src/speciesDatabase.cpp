// geneticCodeDatabase.cpp 
// by Franz Koehler 2010

#include "speciesDatabase.h"
#include <iostream>
#include <stdio.h>

#define ORGS_TO_SAVE 100

namespace EDen {

  SpeciesDatabase::SpeciesDatabase(RuntimeManager* p_runtime): inited(false) {
    runtime = p_runtime;
  };

  bool SpeciesDatabase::empty() {
	  return orgs.empty();
  };

  int SpeciesDatabase::size() {
    return orgs.size();
  };

  void SpeciesDatabase::clear() {
    Organism* org;
    
    while(!orgs.empty()) {
      org = pull();
      delete org;
    };
  };

  void SpeciesDatabase::initEmptyFile(std::string filename)
  {
    doc = new TiXmlDocument();

    TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
    TiXmlElement * element1 = new TiXmlElement("E-DEN-CodeDefinition");
    TiXmlElement * element2 = new TiXmlElement("Version");
    TiXmlElement * element3 = new TiXmlElement("Database");
    TiXmlText * versionText = new TiXmlText("0.1.0.1");

    element1->LinkEndChild(element2);
    element1->LinkEndChild(element3);
    element2->LinkEndChild(versionText);
    doc->LinkEndChild(decl);
    doc->LinkEndChild(element1);
    doc->SaveFile(filename);
  }
  int SpeciesDatabase::load(std::string pFilename) {
    std::string filename = path;
    filename.append("\\").append(pFilename);
    
    doc = new TiXmlDocument(filename);
	  
    bool loadOkay = doc->LoadFile();
	  if (loadOkay)
	  {
      TiXmlElement* element;
      element = doc->FirstChildElement("E-DEN-CodeDefinition");
      element = element->FirstChildElement("Database");
      element = element->FirstChildElement("Organism");
      //clear();
      Organism* org;
      while(element != 0) {
        org = new Organism(element,runtime);
        runtime->add(org,true);
        element = element->NextSiblingElement("Organism");
      };
      return 2;
	  }
	  else
	  {
      initEmptyFile(filename);
      return true;
	  }
  };

  int SpeciesDatabase::save(std::string pFilename) {
    if(orgs.size() > 0) {
      std::string filename = path;
      filename.append("\\").append(pFilename);
      
      doc = new TiXmlDocument(filename);
      bool loadOkay = doc->LoadFile();
	    if (!loadOkay) 
        initEmptyFile(filename);

      
      TiXmlElement* database = doc->FirstChildElement("E-DEN-CodeDefinition")->FirstChildElement("Database");
      database->Clear();

      if(orgs.size() >= ORGS_TO_SAVE) {
            std::list<Organism*> orgsToSave;

        for(int i = 0; i < ORGS_TO_SAVE; i++)
          orgsToSave.push_back(pull(false));

        for( std::list<Organism*>::iterator it = orgsToSave.begin(); it != orgsToSave.end(); it++) {
          database->LinkEndChild((*it)->getXmlElement());
        };
        database->SetAttribute("OrganismCount",orgsToSave.size());
      } else {
        for( std::list<Organism*>::iterator it = orgs.begin(); it != orgs.end(); it++) {
          database->LinkEndChild((*it)->getXmlElement());
        };
        database->SetAttribute("OrganismCount",orgs.size());
      };

      return (int)doc->SaveFile(filename);
    } else return false;
  };

  void SpeciesDatabase::push(Organism* org) {
    orgs.push_back(org);
  };

  Organism* SpeciesDatabase::pull(bool del) {
    Organism* org = 0;
    
    int pos = (int)runtime->randomizer->value(0.0f,(float)orgs.size() - 1);
    std::list<Organism*>::iterator it = orgs.begin();
    for(int i = 0; i < pos; i++) {
      it++;
    };
    
    org = *it;
    if(del) orgs.erase(it);
    return org;
  };

  void SpeciesDatabase::setApplicationSettingsPath(std::string appSettingsPath) {
    path = appSettingsPath;
  };
}; // namespace