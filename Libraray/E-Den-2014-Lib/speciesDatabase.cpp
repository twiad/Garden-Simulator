// geneticCodeDatabase.cpp 
// by Franz Koehler 2010

#include "globals.h"
#include "speciesDatabase.h"
#include <iostream>
#include <stdio.h>
#include <limits>

#define ORGS_TO_SAVE 100
#define CANDIDATES_COUNT 200
#define MAX_SIZE 2000
#define LOWEST_SPECIES_SELECTION_COUNT 2
#define XML_VERSION_STRING "0.1.0.3"

namespace EDen {

  OneSpeciesDatabase::OneSpeciesDatabase(RuntimeManager* p_runtime): inited(false), name("NONAME"), maxCandidates(20),changedSinceLastUpdate(true) {
    treshold = 2;
	maxSize = MAX_SIZE - CANDIDATES_COUNT;
    runtime = p_runtime;
  };

  bool OneSpeciesDatabase::empty() {
	  boost::mutex::scoped_lock lock(mutex);
	  return orgs.empty() && candidates.empty();
  };

  int OneSpeciesDatabase::size() {
    boost::mutex::scoped_lock lock(mutex);
    return orgs.size() + candidates.size();
  };

  void OneSpeciesDatabase::clear() {
    Organism* org;
    
	boost::mutex::scoped_lock lock(mutex);
    while(!(orgs.empty() && candidates.empty())) {
      org = pull(false);
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
    TiXmlText * versionText = new TiXmlText(XML_VERSION_STRING);

    element1->LinkEndChild(element2);
    element1->LinkEndChild(element3);
    element2->LinkEndChild(versionText);
    doc->LinkEndChild(decl);
    doc->LinkEndChild(element1);
    doc->SaveFile(filename);
  };

  int OneSpeciesDatabase::load(std::string pFilename) {
    changedSinceLastUpdate = true;
    
    std::string filename = path;
    filename.append("/").append(name).append(".").append(pFilename);
    
    doc = new TiXmlDocument(filename);
	  
    bool loadOkay = doc->LoadFile();
	  if (loadOkay)
	  {
      TiXmlElement* element;
      int tmpTreshold = 2;
      element = doc->FirstChildElement("E-DEN-CodeDefinition");
      
      element = element->FirstChildElement("Database");
      element->QueryIntAttribute("Treshold",&tmpTreshold);
      setTreshold(tmpTreshold);
      element->QueryIntAttribute("MaxCandidates",&maxCandidates);

      element = element->FirstChildElement("Organism");
      //clear();
      Organism* org;
      while(element != 0) {
        org = new Organism(element,runtime);
        //runtime->add(org);
		push(org);
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
    if(size() > 0) {
      std::string filename = path;
      filename.append("/").append(name).append(".").append(pFilename);
      
      doc = new TiXmlDocument(filename);
      bool loadOkay = doc->LoadFile();
	    if (!loadOkay) 
        initEmptyFile(filename);

      
      TiXmlElement* database = doc->FirstChildElement("E-DEN-CodeDefinition")->FirstChildElement("Database");
      database->Clear();

      if(size() >= ORGS_TO_SAVE) {
            std::list<Organism*> orgsToSave;

        for(int i = 0; i < ORGS_TO_SAVE; i++)
          orgsToSave.push_back(pull());

        for( std::list<Organism*>::iterator it = orgsToSave.begin(); it != orgsToSave.end(); it++) {
          database->LinkEndChild((*it)->getXmlElement());
          push((*it));
        };

        database->SetAttribute("OrganismCount",orgsToSave.size());
      } else {
        for( std::list<Organism*>::iterator it = candidates.begin(); it != candidates.end(); it++) {
          database->LinkEndChild((*it)->getXmlElement());
        };

        for( std::list<Organism*>::iterator it = orgs.begin(); it != orgs.end(); it++) {
          database->LinkEndChild((*it)->getXmlElement());
        };

        database->SetAttribute("OrganismCount",size());
      };

      database->SetAttribute("Treshold",treshold);
      database->SetAttribute("MaxCandidates",maxCandidates);

      return (int)doc->SaveFile(filename);
    } else return false;
  };

  void OneSpeciesDatabase::push(Organism* org) {
    boost::mutex::scoped_lock lock(mutex);
    
	org->getRootBodypart()->setScaleModifier(1.0f);

    changedSinceLastUpdate = true;
    if((candidates.size() < (unsigned)maxCandidates) && (org->getRootBodypart()->getGeneticCode()->getSubSpeciesIdentifier() >= treshold)) {
      candidates.push_back(org);
	}
    else {
	  if(orgs.size() >= maxSize) {
		  removeLeastPerformantOrganism();
	  };
      orgs.push_back(org);
	}
  };

  Organism* OneSpeciesDatabase::pull(bool random, bool del) {
    boost::mutex::scoped_lock lock(mutex);
    Organism* org = 0;
    
    std::list<Organism*>* orgsp;
    if(candidates.size() > 0) orgsp = &candidates;
    else orgsp = &orgs;

	if(orgsp->empty()) return 0;

    std::list<Organism*>::iterator it = orgsp->begin();
      
    if(random) {
      int pos = (int)runtime->randomizer->value(0.0f,(float)orgsp->size() - 1);
      for(int i = 0; i < pos; i++) {
        it++;
      };
    };

    org = *it;
    if(del) {
      orgsp->erase(it);
      changedSinceLastUpdate = true;
    };

	return org;
  };

  void OneSpeciesDatabase::removeLeastPerformantOrganism() {
	  int lowestValue = std::numeric_limits<int>::max();
	  int current;
	  Organism* org;
	  for(std::list<Organism* >::iterator it = orgs.begin(); it != orgs.end(); it++) {
		  current = (*it)->getRootBodypart()->getGeneticCode()->getSubSpeciesIdentifier();
		  if(current < lowestValue) {
			  org = (*it);
			  lowestValue = current;
		  };
	  };
	  orgs.remove(org);
	  runtime->addDelete(org);
  };

  int OneSpeciesDatabase::getMaxSize() {
	  return maxSize;
  };

  void OneSpeciesDatabase::setMaxSize(int newMaxSize) {
	  boost::mutex::scoped_lock lock(mutex);
	  int diff = maxSize - newMaxSize;
	  
      for(int i = 0; i < diff; i++) {
		removeLeastPerformantOrganism();
	  }
	  maxSize = newMaxSize;
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

  int OneSpeciesDatabase::getMaxCandidates() {
    return maxCandidates;
  };

  void OneSpeciesDatabase::setMaxCandidates(int p_maxCandidates) {
    maxCandidates = p_maxCandidates;
  };

  void OneSpeciesDatabase::updateTreshold() {
	boost::mutex::scoped_lock lock(mutex);
	if(changedSinceLastUpdate) {
      while(candidates.size() >= 0.9f * getMaxCandidates() || candidates.size() >= 0.9f * orgs.size()) {
		  setTreshold(treshold + 1);
	  };

	  if(candidates.size() == 0) {
		  int newNumberOfCandidates = getMaxCandidates();
		  if(orgs.size() < newNumberOfCandidates) {
			  newNumberOfCandidates = orgs.size();		
		  };
		  newNumberOfCandidates = floor(newNumberOfCandidates / 2.0f);
		  while(candidates.size() < newNumberOfCandidates) {
			  setTreshold(treshold - 1);
		  };
	  }
    };

    changedSinceLastUpdate = false;
  };

  void OneSpeciesDatabase::setTreshold(int p_treshold) {
    if(p_treshold == treshold) return;
    else {
      Organism* org;
      if(p_treshold > treshold) {
        std::list<Organism*> newCandidates;
        while(!candidates.empty()) {
          org = candidates.front();
          if((candidates.size() < (unsigned)maxCandidates) && (org->getRootBodypart()->getGeneticCode()->getSubSpeciesIdentifier() >= p_treshold))
              newCandidates.push_back(org);
          else orgs.push_back(org);
          candidates.pop_front();
        };
        candidates.swap(newCandidates);
      }
      else {
        std::list<Organism*> newOrgs;
        while(!orgs.empty()) {
          org = orgs.front();
          if((candidates.size() < (unsigned)maxCandidates) && (org->getRootBodypart()->getGeneticCode()->getSubSpeciesIdentifier() >= p_treshold))
              candidates.push_back(org);
          else newOrgs.push_back(org);
          orgs.pop_front();
        };
        orgs.swap(newOrgs);
      };
    };

	int diff = orgs.size() - maxSize;
	for(int i = 0; i < diff; i++) {
		removeLeastPerformantOrganism();
	};

    treshold = p_treshold;
  };

  int OneSpeciesDatabase::getTreshold() {
    return treshold;
  };

  std::string OneSpeciesDatabase::getDebugOut() {
    std::string out = "";
    char str[64];

    sprintf(str,"[%d|%d|%d]",size(),candidates.size(),treshold);

    out += str;
    out.insert(0,name);
    return out;
  };
  //////////////////////////////////////////////////////////////////////////////////////////////////

  SpeciesDatabase::SpeciesDatabase(RuntimeManager* p_runtime): inited(false) {
    runtime = p_runtime;
    speciesSelectionAlternater = true;
    speciesSelectionAlternater2 = true;
  };

  bool SpeciesDatabase::empty() {
    bool foundOne = false;
    for(std::map<unsigned int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
      if(!(*it).second->empty()) {
        foundOne = true;
        break;
      };
    };

    return !foundOne;
  };

  int SpeciesDatabase::size() {
    int size = 0;
    for(std::map<unsigned int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
      size += (*it).second->size();
    };
    return size;
  };

  void SpeciesDatabase::clear() {
    std::map<unsigned int,OneSpeciesDatabase*>::iterator dbit;
    while(!species.empty()) {
      dbit = species.begin();
      (*dbit).second->clear();
      delete (*dbit).second;
      species.erase(dbit);
    };
  };

  int SpeciesDatabase::load(std::string pFilename) {
    std::string filename = path;
    filename.append("/").append("species").append(".").append(pFilename);
    
    doc = new TiXmlDocument(filename);
	  
    bool loadOkay = doc->LoadFile();
	  if (loadOkay)
	  {
      TiXmlElement* element;
      int speciesId;
      element = doc->FirstChildElement("E-DEN-CodeDefinition");
      element = element->FirstChildElement("Database");
      element = element->FirstChildElement("Species");
      while(element != 0) {
        element->QueryIntAttribute("ID",&speciesId);
        if(species.count(speciesId) == 0) {
          OneSpeciesDatabase* db = new OneSpeciesDatabase(runtime);
          std::string name = "";
          char str[64];

          sprintf(str,"species%d",speciesId);
          name += str;
          db->setName(name);
          db->setApplicationSettingsPath(path);

          species[speciesId] = db;
        };
        element = element->NextSiblingElement("Species");
      };
	  }
	  else
	  {
      initEmptyFile(filename);
	  }
    
    for(std::map<unsigned int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
      (*it).second->load(pFilename);
    };
    return true;
  };

  int SpeciesDatabase::save(std::string p_filename) {
    if(size() > 0) {
      std::string filename = path;
      filename.append("/").append("species").append(".").append(p_filename);
      
      doc = new TiXmlDocument(filename);
      bool loadOkay = doc->LoadFile();
	    if (!loadOkay) 
        initEmptyFile(filename);

      
      TiXmlElement* database = doc->FirstChildElement("E-DEN-CodeDefinition")->FirstChildElement("Database");
      database->Clear();

      TiXmlElement* spec;
      for(std::map<unsigned int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
        spec = new TiXmlElement("Species");
        spec->SetAttribute("ID",(*it).first);
        database->LinkEndChild(spec);
      };
      database->SetAttribute("speciesCount",getSpeciesCount());

      doc->SaveFile(filename);

      for(std::map<unsigned int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
        (*it).second->save(p_filename);
      };

      return true;
    } else return false;
  };
 
  void SpeciesDatabase::setApplicationSettingsPath(std::string appSettingsPath) {
    path = appSettingsPath;
    for(std::map<unsigned int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
      (*it).second->setApplicationSettingsPath(appSettingsPath);
    };
  };

  void SpeciesDatabase::push(Organism* org) {
	int speciesId = org->getRootBodypart()->getGeneticCode()->getSpeciesIdentifier();
   
    boost::mutex::scoped_lock lock(mutex);
    if(species.count(speciesId) == 0) {
      std::string name = "";
      char str[64];

      sprintf(str,"species%d",speciesId);
      name += str;

      OneSpeciesDatabase* db = new OneSpeciesDatabase(runtime);
      db->setMaxCandidates(CANDIDATES_COUNT);
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

    boost::mutex::scoped_lock lock(mutex);
    if(empty()) return 0;

    if(speciesId == 0) {
      speciesId = getNextSpeciesId();
    };

	while(species[speciesId]->empty()) {
	  if(del) {
		species[speciesId]->clear();
		delete species[speciesId];
		species.erase(speciesId);
	  }
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

  int SpeciesDatabase::getNextSpeciesId() {
    int speciesId;
    static int lowest_species_count = LOWEST_SPECIES_SELECTION_COUNT;

    if(!speciesSelectionAlternater2) {
      //if(speciesSelectionAlternater) {
        speciesId = getSpeciesIdWithLowestCount();
        speciesSelectionAlternater = !speciesSelectionAlternater;
      //}
  //    else {
  //      speciesId = getSpeciesIdWithHighestCount();
		//speciesSelectionAlternater = !speciesSelectionAlternater;
		//
  //    };

      if(lowest_species_count++ >= LOWEST_SPECIES_SELECTION_COUNT) {
		speciesSelectionAlternater2 = !speciesSelectionAlternater2;
		lowest_species_count = 0;
	  }	
    }
    else {
      std::map<unsigned int,OneSpeciesDatabase*>::iterator it;
      for(it = species.begin(); it != species.end(); it++) {
        if((*it).first == lastSpeciesId) {
          it++;
          if(it != species.end()) {
            speciesId = (*it).first;
            lastSpeciesId = speciesId;
            break;
          }
          else {
            it = species.begin();
            speciesId = (*it).first;
            lastSpeciesId = speciesId;
            speciesSelectionAlternater2 = !speciesSelectionAlternater2;
            break;
          };
        };
      };
      if(it == species.end()) {
        it = species.begin();
        speciesId = (*it).first;
        lastSpeciesId = speciesId;
        speciesSelectionAlternater2 = !speciesSelectionAlternater2;
      };
    };
    return speciesId;
  };

  int SpeciesDatabase::getSpeciesIdWithLowestCount() {
    int lowestCount = 1000000, lowestCountId = 0;

    for(std::map<unsigned int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
      if((*it).second->size() < lowestCount) {
        lowestCountId = (*it).first;
        lowestCount = (*it).second->size();
      };
    };

    return lowestCountId;
  };

  int SpeciesDatabase::getSpeciesIdWithHighestCount() {
    int highest = 0, highestCountId = 0;

    for(std::map<unsigned int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
      if((*it).second->size() > highest) {
        highestCountId = (*it).first;
        highest = (*it).second->size();
      };
    };

    return highestCountId;
  };

  int SpeciesDatabase::getSpeciesCount() {
    return species.size();
  };

  void SpeciesDatabase::initEmptyFile(std::string filename)
  {
    doc = new TiXmlDocument();

    TiXmlDeclaration * decl = new TiXmlDeclaration("1.01", "", "");
    TiXmlElement * element1 = new TiXmlElement("E-DEN-CodeDefinition");
    TiXmlElement * element2 = new TiXmlElement("Version");
    TiXmlElement * element3 = new TiXmlElement("Database");
    TiXmlText * versionText = new TiXmlText(XML_VERSION_STRING);

    element1->LinkEndChild(element2);
    element1->LinkEndChild(element3);
    element2->LinkEndChild(versionText);
    doc->LinkEndChild(decl);
    doc->LinkEndChild(element1);
    doc->SaveFile(filename);
  };

  void SpeciesDatabase::update() {
    for(std::map<unsigned int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
      (*it).second->updateTreshold();
    };
  };

  std::string SpeciesDatabase::getDebugOut() {
    std::string out = "-";

    for(std::map<unsigned int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
      out.append((*it).second->getDebugOut());
    };

    return out;
  };

  void SpeciesDatabase::getDebugOut(std::map<unsigned int, std::string>* outList) {
    for(std::map<unsigned int,OneSpeciesDatabase*>::iterator it = species.begin(); it != species.end(); it++) {
	  outList->insert(std::pair<unsigned int,std::string>((it->first),it->second->getDebugOut()));
    };
  };

}; // namespace
