// geneticCodeDatabase.cpp 
// by Franz Koehler 2010

#include "geneticCodeDatabase.h"

namespace EDen {

  GeneticCodeDatabase::GeneticCodeDatabase() {
    
  };

  int GeneticCodeDatabase::load(std::string pFilename) {
    filename = pFilename;
    
    doc = new TiXmlDocument(filename);
	  
    bool loadOkay = doc->LoadFile();
	  if (loadOkay)
	  {
		  //printf("\n%s loaded ...\n", filename);
      return 1;
	  }
	  else
	  {
		  //printf("Failed to load file \"%s\"\nCreating File ...", filename);

      doc = new TiXmlDocument();

      TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
      TiXmlElement * element1 = new TiXmlElement("E-DEN-CodeDefinition");
      TiXmlElement * element2 = new TiXmlElement("Version");
      TiXmlText * versionText = new TiXmlText("0.1.0");
      
      element1->LinkEndChild(element2);

      element2->LinkEndChild(versionText);
      doc->LinkEndChild(decl);
      doc->LinkEndChild(element1);
      return (int)doc->SaveFile(filename);
	  }
  };

  int GeneticCodeDatabase::save() {
    
  };

  void GeneticCodeDatabase::push(GeneticCode* code) {
    
  };

  GeneticCode* GeneticCodeDatabase::pull() {
    
  };

}; // namespace