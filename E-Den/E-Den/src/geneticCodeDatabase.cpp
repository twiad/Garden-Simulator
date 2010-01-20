// geneticCodeDatabase.cpp 
// by Franz Koehler 2010

#include "geneticCodeDatabase.h"

namespace EDen {

  bool GeneticCodeDatabase::empty() {
	  return orgs.empty();
  };

  int GeneticCodeDatabase::size() {
    return orgs.size();
  };

  int GeneticCodeDatabase::load(std::string pFilename) {
    filename = pFilename;
    
    doc = new TiXmlDocument(filename);
	  
    bool loadOkay = doc->LoadFile();
	  if (loadOkay)
	  {
      TiXmlElement* element,*e2,*e3;
      element = doc->FirstChildElement("E-DEN-CodeDefinition");
      element = element->FirstChildElement("Database");
      //e2 = element;
      element = element->FirstChildElement("Organism");
      //e3 = element;
      //Organism* org = new Organism(element);
      return 2;
	  }
	  else
	  {
      doc = new TiXmlDocument();

      TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
      TiXmlElement * element1 = new TiXmlElement("E-DEN-CodeDefinition");
      TiXmlElement * element2 = new TiXmlElement("Version");
      TiXmlElement * element3 = new TiXmlElement("Database");
      TiXmlText * versionText = new TiXmlText("0.1.0");
      
      element1->LinkEndChild(element2);
      element1->LinkEndChild(element3);
      element2->LinkEndChild(versionText);
      doc->LinkEndChild(decl);
      doc->LinkEndChild(element1);
      return (int)doc->SaveFile(filename);
	  }
  };

  int GeneticCodeDatabase::save() {
    TiXmlElement* database = doc->FirstChildElement("E-DEN-CodeDefinition")->FirstChildElement("Database");
    database->Clear();

    for( std::list<Organism*>::iterator it = orgs.begin(); it != orgs.end(); it++) {
      database->LinkEndChild((*it)->getXmlElement());
    };

    return 1; //(int)doc->SaveFile(filename);
  };

  void GeneticCodeDatabase::push(Organism* org) {
    orgs.push_back(org);
  };

  Organism* GeneticCodeDatabase::pull() {
    Organism* org = 0;
    
    if(orgs.size() > 0) {
      org = orgs.front();
      orgs.pop_front();
    };

    return org;
  };

}; // namespace