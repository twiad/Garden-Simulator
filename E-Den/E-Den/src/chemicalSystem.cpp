#include "chemicalSystem.h"
#include <list>

namespace EDen {
	std::string Chemical::getName() {
		return _name;
	}

	ChemicalStorage::~ChemicalStorage() {
		storage.clear();
	}

  bool ChemicalStorage::isPresent(std::string chemName) {
    if(storage.find(chemName) == storage.end())
        return false;
    else
        return true; 
  };

  bool ChemicalStorage::add(std::string chemName, float amount) {
    if(isPresent(chemName)) {
      float newAmount = storage[chemName].current + amount;
      
      if(newAmount < 0.0) newAmount = 0.0;
      else if(newAmount > storage[chemName].max*size) newAmount = storage[chemName].max*size;

      setCurrentAmount(chemName, newAmount);
      return true;
    }
    else return addNew(chemName,amount);
  };

  bool ChemicalStorage::addNew(std::string chemName, float amount, float maxAmount) {
    ChemicalStorageMapIterator it = storage.find(chemName);
    if(it == storage.end()) {
      ChemicalStorageEntry entry;
      Chemical chem(chemName);

      entry.chemical = chem;
      entry.current = amount;
      entry.max = maxAmount;
      
      storage[chemName] = entry;
      return true;
    }
    else {
      storage[chemName].max = maxAmount;
      return setCurrentAmount(chemName, amount);
    }
	};

  float ChemicalStorage::getMaxAmount(std::string chemName) {
    if(isPresent(chemName)) {
      return storage[chemName].max*size;
    }
    else return STD_CHEM_STORAGE_MAX_AMOUNT * size;
  };

  bool ChemicalStorage::setMaxAmount(std::string chemName, float maxAmount) {
    if(maxAmount < 0.0) maxAmount = 0;

    if(isPresent(chemName)) {
      storage[chemName].max = maxAmount;
    }
    else addNew(chemName,0.0,maxAmount);

    return true;
  };
  
  float ChemicalStorage::getCurrentAmount(std::string chemName) {
    if(isPresent(chemName)) {
      float current = storage[chemName].current;
      float max = storage[chemName].max*size;
      if(max < current) return max;
      else return current;
    }
    else return 0.0; 
  };

  bool ChemicalStorage::setCurrentAmount(std::string chemName, float newAmount) {
    if(newAmount < 0.0) newAmount = 0.0;
        
    if(isPresent(chemName)) {
      if(newAmount > storage[chemName].max*size)
          storage[chemName].current = storage[chemName].max*size;
      else
          storage[chemName].current = newAmount;
    }
    else {
      addNew(chemName,newAmount);      
    }
    return true;
  };

  float ChemicalStorage::getSpaceLeft(std::string chemName) {
    return getMaxAmount(chemName) - getCurrentAmount(chemName);
  };

  float ChemicalStorage::getCurrentPercentage(std::string chemName) {
    float max = getMaxAmount(chemName);
    if(max <= 0) return 0.0;
    else 
      return 100*(getCurrentAmount(chemName)/max);
  };

	bool ChemicalStorage::isPresent(Chemical chem) {
		return isPresent(chem.getName());
	};

	bool ChemicalStorage::addNew(Chemical chem, float amount, float maxAmount) {
    return addNew(chem.getName(),amount,maxAmount);    
  };

	bool ChemicalStorage::add(Chemical chem, float amount) {
    return add(chem.getName(),amount);
	};


  bool ChemicalStorage::setMaxAmount(Chemical chem, float maxAmount) {
    return setMaxAmount(chem.getName(),maxAmount);
  };
  
  float ChemicalStorage::getMaxAmount(Chemical chem) {
    return getMaxAmount(chem.getName());
  };

  float ChemicalStorage::getCurrentAmount(Chemical chem) {
    return getCurrentAmount(chem.getName());
  };

  bool ChemicalStorage::setCurrentAmount(Chemical chem, float newAmount) {
    return setCurrentAmount(chem.getName(), newAmount);
  };

  float ChemicalStorage::getSpaceLeft(Chemical chem) {
    return getSpaceLeft(chem.getName()); 
  };

  float ChemicalStorage::getSize() {
    return size;  
  };

  bool ChemicalStorage::setSize(float param_size) {
    size = param_size; 
    return true;
  };

  bool ChemicalStorage::sync(ChemicalStorage* other) {
    /// TODO: UNDBEDINGT NOCH HYSTERESE CHECKEN!!! PERFORMANCE!
    for(ChemicalStorageMapIterator it = storage.begin(); it != storage.end(); it++) {
//  I:  apost/amax = bpost/bmax
// II: apost+bpost = a+b

      ChemicalStorageEntry storageEntry = (*it).second;
      if(storageEntry.max*size > 0.0) {
        float bamaxquot = (other->getMaxAmount(storageEntry.chemical))/(storageEntry.max*size);
        float basum = (other->getCurrentAmount(storageEntry.chemical))+(storageEntry.current);
        float newb = basum*(bamaxquot/(bamaxquot+1));
        other->setCurrentAmount(storageEntry.chemical,newb);
        (*it).second.current = basum - newb;
      }
    };
    return true;
  };

  //bool ChemicalStorage::sync(ChemicalStorage* other) {
  //
  //  std::list<std::string> chemNames;
  //  for(ChemicalStorageMapIterator it = storage.begin(); it != storage.end(); it++) {
  //    chemNames.push_back((*it).first);
  //  };

  //  for(ChemicalStorageMapIterator it = other->storage.begin(); it != other->storage.end(); it++) {
  //    chemNames.push_back((*it).first);
  //  };

  //  {
  //    std::string name;
  //    while(!chemNames.empty()) {
  //      name = chemNames.back();
  //      chemNames.pop_back();
  //      if(getMaxAmount(name) > 0.0) {
  //        float bamaxquot = (other->getMaxAmount(name)/(getMaxAmount(name)));
  //        float basum = (other->getCurrentAmount(name)+(getCurrentAmount(name)));
  //        float newb = basum*(bamaxquot/(bamaxquot+1));
  //        other->setCurrentAmount(name,newb);
  //        setCurrentAmount(name,basum - newb);
  //      }; // if > 0
  //    }; // while !empty() 
  //  } // std::string
  //  return true;
  //};
  
  bool ChemicalStorage::moveAllToStorage(ChemicalStorage* other) {
    for(ChemicalStorageMapIterator it = storage.begin(); it != storage.end(); it++) {
      other->add((*it).first,(*it).second.current);
      (*it).second.current = 0.0f;
    };

    return true;
  };

  ChemicalStorage::ChemicalStorage(TiXmlElement* description) {
    TiXmlElement* it;
    double tmpCurrent, tmpMax;
    
    description->QueryFloatAttribute("Size",&size);
    it = description->FirstChildElement("Chemical");
    std::string name;
        
    while(it != 0) {
      name = it->Attribute("Name");
      it->QueryDoubleAttribute("Current",&tmpCurrent);
      it->QueryDoubleAttribute("Max",&tmpMax);

      addNew(name,(float)tmpCurrent,(float)tmpMax);

      it = it->NextSiblingElement();
    };
  };

  TiXmlElement* ChemicalStorage::toXmlElement() {
    TiXmlElement* element,*chemicalElement;

    element = new TiXmlElement("Storage");
    for(ChemicalStorageMapIterator it = storage.begin(); it != storage.end(); it++) {
      chemicalElement = new TiXmlElement("Chemical");
      chemicalElement->SetAttribute("Name",(*it).second.chemical.getName());
      chemicalElement->SetDoubleAttribute("Current",(double)(*it).second.current);
      chemicalElement->SetDoubleAttribute("Max",(double)(*it).second.max);
      element->LinkEndChild(chemicalElement);
    };

    element->SetDoubleAttribute("Size",size);
    return element;
  };

  void ChemicalStorageLink::update() {
    // TODO: Validate that _s1,_s2 are ready, port to CoAJnR maybe ;)
    _s1->sync(_s2);
    //_s2->sync(_s1); // depends ... should not be needed :P
  };

  ChemicalStorage* ChemicalStorageLink::getS1() {
    return _s1; 
  };

  ChemicalStorage* ChemicalStorageLink::getS2() {
    return _s2;
  };
} // namespace EDen