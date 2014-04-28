// groundpart.h
// by Franz Koehler 2009 - 2014


#ifndef _E_DEN_GROUNDPART_HEADER_
#define _E_DEN_GROUNDPART_HEADER_

namespace EDen {
  class Groundpart;
};

#include "globals.h"
#include "chemicalSystem.h"
#include "organism.h"
#include "speciesDatabase.h"

namespace EDen {

  class Groundpart {
  protected:
    ChemicalStorage* chemStorage;
	SpeciesDatabase* speciesDatabase;
	int emptySpaces;
	std::list<Organism*> organisms;

	std::string name;

	boost::mutex mutex;
  public:
    Groundpart(std::string name, float maxWater = 2.0e8, float maxGoo = 2.0e8, int emptySpaces = 1);
    ~Groundpart();

	std::string getName();

    ChemicalStorage* getChemicalStorage();
	SpeciesDatabase* getSpeciesDatabase();
	void setSpeciesDatabase(SpeciesDatabase* database);
	Organism* addSeedFromDb();
	bool saveDatabase();

	virtual bool hasEmptySpaces();
	virtual bool incEmptySpaces();
	virtual bool decEmptySpaces();
	virtual void getNumOrganismsAndEmptySpaces(int* numOrganisms, int* numEmptySpaces);
	virtual int getAliveOrganismsOfSpecies(int speciesID);

	std::list<Organism*> getOrganisms();

    virtual bool registerOrganism(Organism* param_organism);
    virtual bool unregisterOrganism(Organism* param_organism);

	virtual bool dropOrganism(Organism* param_organism, Bodypart* param_parentBodypart);
  };

  class SingleDimensionHeightmapGroundpart : public Groundpart {
  protected:
	  int width;
	  int* heightmap;
	  
	  std::map<Organism* ,int> plantPositionMemory;
	  std::map<int, std::list<Organism*>> alivePlantPositions;
	  typedef std::map<Organism* ,int>::iterator plantPositionMemoryIterator;

	  void randomizeHeights();
	  int getBodypartPosX(Bodypart* param_bodypart);
  public:
	  SingleDimensionHeightmapGroundpart(std::string name, int width, float maxWater = 2.0e8, float maxGoo = 2.0e8, int emptySpaces = 1);
	  ~SingleDimensionHeightmapGroundpart();

	  virtual bool registerOrganism(Organism* param_organism);
      virtual bool unregisterOrganism(Organism* param_organism);

	  virtual bool dropOrganism(Organism* param_organism, Bodypart* param_parentBodypart);
	  
	  int getOrganismX(Organism* param_organism);
	  Organism* SingleDimensionHeightmapGroundpart::getOrganismNearX(int posX);
	  bool isOccupiedByAlivePlant(int posX);

	  float getHeightAt(int posX);
	  int getWidth();
  };
};

#endif