// geneticCodeDatabase.cpp 
// by Franz Koehler 2010

#include "speciesDatabase.h"
#include <iostream>
#include <stdio.h>

#define ORGS_TO_SAVE 100

namespace EDen {

  OneSpeciesDatabase::OneSpeciesDatabase(RuntimeManager* p_runtime): inited(false), name("") {
    runtime = p_runtime;
  };

  bool OneSpeciesDatabase::empty() {
	  return orgs.empty();
  };

  int OneSpeciesDatabase::size() {
    return orgs.size();
  };

  void OneSpeciesDatabase::clear() {
    Organism* org;
    
    while(!orgs.empty()) {
      org = pull();
      delete org;
    };
  };

  void OneSpeciesDatabase::initEmptyFile(std::string filename)
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

  int OneSpeciesDatabase::load(std::string pFilename) {
    std::string filename = path;
    filename.append("\\").append(name).append(".").append(pFilename);
    
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

  int OneSpeciesDatabase::save(std::string pFilename) {
    if(orgs.size() > 0) {
      std::string filename = path;
      filename.append("\\").append(name).append(".").append(pFilename);
      
      doc = new TiXmlDocument(filename);
      bool loadOkay = doc->LoadFile();
	    if (!loadOkay) 
        initEmptyFile(filename);

      
      TiXmlElement* database = doc->FirstChildElement("E-DEN-CodeDefinition")->FirstChildElement("Database");
      database->Clear();

      if(orgs.size() >= ORGS_TO_SAVE) {
            std::list<Organism*> orgsToSave;

        for(int i = 0; i < ORGS_TO_SAVE; i++)
          orgsToSave.push_back(pull(true,false));

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

  void OneSpeciesDatabase::push(Organism* org) {
    orgs.push_back(org);
  };

  Organism* OneSpeciesDatabase::pull(bool random, bool del) {
    Organism* org = 0;
    
    std::list<Organism*>::iterator it = orgs.begin();
      
    if(random) {
      int pos = (int)runtime->randomizer->value(0.0f,(float)orgs.size() - 1);
      for(int i = 0; i < pos; i++) {
        it++;
      };
    };

    org = *it;
    if(del) orgs.erase(it);
    return org;
  };

  void OneSpeciesDatabase::setApplicationSettingsPath(std::string appSettingsPath) {
    path = appSettingsPath;
  };

  void OneSpeciesDatabase::setName(std::string p_name) {
    name = p_name;
  };

  std::string OneSpeciesDatabase::getName() {
    return name; 
  };

  //////////////////////////////////////////////////////////////////////////////////////////////////

  SpeciesDatabase::SpeciesDatabase(RuntimeManager* p_runtime): inited(false) {
    runtime = p_runtime;
  };

  bool SpeciesDatabase::empty() {
    bool foundOne = false;
    for(std::map<int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
      if(!(*it).second->empty()) {
        foundOne = true;
        break;
      };
    };

    return !foundOne;
  };

  int SpeciesDatabase::size() {
    int size = 0;
    for(std::map<int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
      size += (*it).second->size();
    };
    return size;
  };

  void SpeciesDatabase::clear() {
    std::map<int,OneSpeciesDatabase*>::iterator dbit;
    while(!species.empty()) {
      dbit = species.begin();
      (*dbit).second->clear();
      delete (*dbit).second;
      species.erase(dbit);
    };
  };

  int SpeciesDatabase::load(std::string pFilename) {
    for(std::map<int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
      (*it).second->load(pFilename);
    };
    return true;
  };

  int SpeciesDatabase::save(std::string p_filename) {
    for(std::map<int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
      (*it).second->save(p_filename);
    };
    return true;
  };
 
  void SpeciesDatabase::setApplicationSettingsPath(std::string appSettingsPath) {
    path = appSettingsPath;
    for(std::map<int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
      (*it).second->setApplicationSettingsPath(appSettingsPath);
    };
  };

  void SpeciesDatabase::push(Organism* org) {
    int speciesId = org->getRootBodypart()->getGeneticCode()->getSpeciesIdentifier();
   
    if(species.count(speciesId) == 0) {
      std::string name = "";
      char str[64];

      sprintf(str,"species%d",speciesId);
      name += str;

      OneSpeciesDatabase* db = new OneSpeciesDatabase(runtime);
      db->setName(name);
      db->setApplicationSettingsPath(path);
      db->push(org);

      species[speciesId] = db;
    }
    else {
      species[speciesId]->push(org);
    };
  };

  Organism* SpeciesDatabase::pull(int speciesId, bool random, bool del) {
    OneSpeciesDatabase* db;
    Organism* org = 0;
    
    if(empty()) return 0;

    while(species.count(speciesId) == 0) {
      // TODO: pick random species
      speciesId = (*species.begin()).first;
    };
    
    db = species[speciesId];
    org = db->pull(random,del);

    if(del) {
      if(db->empty()) {
        species[speciesId]->clear();
        delete species[speciesId];
        species.erase(speciesId);
      };
    };

    return org;
  };

  int SpeciesDatabase::getSpeciesCount() {
    return species.size();
  };

}; // namespace