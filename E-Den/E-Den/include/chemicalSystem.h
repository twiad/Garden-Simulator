// chemicalSystem.h
// by Franz Koehler 2009

#ifndef _E_DEN_CHEMICAL_SYSTEM_CORE_HEADER_
#define _E_DEN_CHEMICAL_SYSTEM_CORE_HEADER_

#include <map>
#include <iostream>
#include <string>
#include "tinyxml.h"

#define STD_CHEM_STORAGE_MAX_AMOUNT 1.0f

namespace EDen {

	class Chemical {
		
	protected:
		std::string _name;
	public:
		Chemical(std::string name = "DUMMY_RESOURCE"): _name(name) { };
		~Chemical() { };
		
		inline std::string getName();
	};

	class ChemicalStorage {
	protected:	
		struct  ChemicalStorageEntry {
      ChemicalStorageEntry() {max = 0.0; current = 0.0;};
			Chemical chemical;
			float max;
			float current;
		};

    // tell the map how to sort strings -.-
    struct ltstr
    {
      bool operator()(const std::string s1, const std::string s2) const
        {
          return s1.compare(s2) < 0;
        }
    };

		typedef std::map<std::string,ChemicalStorageEntry,ltstr> ChemicalStorageMap;
		typedef std::map<std::string,ChemicalStorageEntry,ltstr>::iterator ChemicalStorageMapIterator;

		ChemicalStorageMap storage;
    float size;

	public:
    ChemicalStorage():size(1.0f) {};
		~ChemicalStorage();

		bool isPresent(Chemical chem);
		bool addNew(Chemical chem, float amount, float maxAmount = STD_CHEM_STORAGE_MAX_AMOUNT);
		bool add(Chemical chem, float amount);
    bool setMaxAmount(Chemical chem, float maxAmount);
    float getMaxAmount(Chemical chem);
    bool setCurrentAmount(Chemical chem, float newAmount);
    float getCurrentAmount(Chemical chem);
    float getSpaceLeft(Chemical chem);

    bool isPresent(std::string chemName);
    bool add(std::string chemName, float amount);
    bool addNew(std::string chemName, float amount, float maxAmount = STD_CHEM_STORAGE_MAX_AMOUNT);
    bool setMaxAmount(std::string chemName, float maxAmount);
    float getMaxAmount(std::string chemName);
    bool setCurrentAmount(std::string chemName, float newAmount);
    float getCurrentAmount(std::string chemName);
    float getSpaceLeft(std::string chemName);
    float getCurrentPercentage(std::string chemName);

    float getSize();
    bool setSize(float param_size);

    bool sync(ChemicalStorage* other);
    bool moveAllToStorage(ChemicalStorage* other);

    TiXmlElement* toXmlElement();
	};

  class ChemicalStorageLink {
  protected:
    ChemicalStorage* _s1;
    ChemicalStorage* _s2;
  public:
    ChemicalStorageLink(ChemicalStorage* s1, ChemicalStorage* s2): _s1(s1),_s2(s2) {};
    ~ChemicalStorageLink() {};

    ChemicalStorage* getS1();
    ChemicalStorage* getS2();

    void update();
  };
} // namespace EDen

#endif