// E-Den-Testbench.cpp : Defines the entry point for the console application.
// by Franz Koehler 2009

#include "stdafx.h"
#include "bodypart.h"
#include "organismPrinter.h"
#include "SDL.h"

#define CYCLES_TO_RUN 3000
#define PRINT_OUT_ALL_N_CYCLES 3000
#define SND_RUN_MULTIPLYER 2
// STARTING_WATER_max: 2.0e7
#define STARTING_WATER 1.0e7
#define CHEM_SYSTEM_CLK_DEVIDER 1
#define SDL_RUN_FACTOR 2
#define SDL_IDEL_CYCLES 1

using namespace EDen; 
using namespace std;

Organism* organism = 0;
Organism* organism2 = 0;
Bodypart* bp3 = 0;
Groundpart* gp = 0;
SDLOrganismPrinter* op1 = 0;
OrganismPrinter* op2 = 0;
int cyclecount = 0;

bool printall;

class SDL_SunlightProvider : public ResourceProvider {
private:
  SDLOrganismPrinter* printer;
public:
  SDL_SunlightProvider(SDLOrganismPrinter * param_printer) : ResourceProvider("Sonne",BPT_Leaf,1.0f) {
    printer = param_printer;
    amount = 0.9f;
    reactiveBodypartType = BPT_Leaf;
    chemicalName = "Sonne";
  };

  ~SDL_SunlightProvider() {};
};

SDL_SunlightProvider* sun;

void outputWaterAndGold(ChemicalStorage* cs) {
  if(cs) {
    float w = cs->getCurrentAmount("Wasser");
    float wp = cs->getCurrentPercentage("Wasser");
    float wm = cs->getMaxAmount("Wasser");
    float e = cs->getCurrentAmount("Energie");
    float ep = cs->getCurrentPercentage("Energie");
    float em = cs->getMaxAmount("Energie");
    float g = cs->getCurrentAmount("Gold");
    cout << "--[\t" << w << "/" << wm << "'" << wp << "w\t" << 
      e << "/" << em << "'" << ep <<"e\t" << 
      g << "g\t" << 
      "]--" << endl;
  }
};

void outputWaterAndGold(Bodypart* bp) {
  if(bp) {
    cout << "|" << bp->getHealthpoints() << "|" << bp->getSize() << "|\t";
    outputWaterAndGold(bp->getChemicalStorage());
  }
};

void run(int cycles = 1, Organism* org = organism) {
  if(op1 != 0) {
    Organism* org;
    std::list<Organism*> orgs = op1->getOrganisms();
    cyclecount += cycles;
    sun->distibute();
    for(int i = 0; i < cycles; i++) {
      for(std::list<Organism*>::iterator it = orgs.begin(); it != orgs.end(); it++) {
        org = *it;
        if((org != 0) && (org->getState() != BSP_dead)) {
          //if(organism->getState() != BSP_dead) 
            org->updateGeneticProcessors();
          //else std::cout << "#";
            if((i%CHEM_SYSTEM_CLK_DEVIDER) == 0) {
              org->updateDelete();
              org->updateChemicalStorageLinks();
            };
         
        };
      };
    };
  };
};

void printOrgs() {
  if(op1 != 0) {
    op1->print();
  };
};

void sdl_run(int cycles) {
  printOrgs();
  //outputWaterAndGold(gp->getChemicalStorage());
  //outputWaterAndGold(organism->getRootBodypart()->getChemicalStorage());
  //printf("-[%d]-----------------------------------------------",cyclecount);
  //printf("\t\t\t[running %d cycles]\n",cycles);

  run(cycles);
//  printf("bp3.maxSize: %f\n", bp3->getMaxSize());
//  printOrgs();
}

bool wait_for_events()
{
  SDL_Event event;
  char *key;
  bool quit = false;

  printf("waiting for events, press 'q' or 'ESC' to quit\n");
  while ( !quit ) {				
    if(SDL_PollEvent(&event)) {	
	    switch (event.type) {		//check the event type
		    case SDL_KEYDOWN:			//if a key has been pressed
          key = SDL_GetKeyName(event.key.keysym.sym);
		      printf("The %s key was pressed!\n", key );
          if ( event.key.keysym.sym == SDLK_ESCAPE )	//quit if 'ESC' pressed
	          quit = true;
		      else if ( key[0] == 'q'  )	//quit if 'q'  pressed
	          quit = true;			//same as "if ( event.key.keysym.sym == SDLK_q )"
		    break;
		     case SDL_MOUSEMOTION:             //mouse moved
			     printf("Mouse motion x:%d, y:%d\n", event.motion.x, event.motion.y );
           sdl_run(1);
			     break;
		     case SDL_MOUSEBUTTONUP:           //mouse button pressed
			     printf("Mouse button %d pressed x:%d, y:%d\n", event.button.button, event.button.x, event.button.y );
           if(event.button.button == 1) 
             gp->getChemicalStorage()->add("Wasser",event.button.x*100.0f);
           else
             gp->getChemicalStorage()->add("Wasser",-event.button.x*100.0f);
			     break; 
	       case SDL_QUIT:		
	         exit ( 1 );
		     break;
	    }
    }
    else {
      sdl_run(SDL_IDEL_CYCLES);
      if(organism->getState() == BSP_dead) exit(1);
    }
  } //while
  return true;
}

void sdl_test() {
  gp = new Groundpart();
  Bodypart* bp,* bp2;
  
  op1 = new SDLOrganismPrinter(1024,800);
  sun = new SDL_SunlightProvider(op1);
  bp = new Bodypart(BPT_Stick,"TESTPART3");
  organism = new Organism("TestOrganism", bp, sun);
  organism->connectToGoundpart(gp);
  op1->add(organism);
//  op2 = new OrganismPrinter(organism);
  bp2 = new Bodypart(BPT_Branch,"TESTPART3",organism);
  bp->occupieSpawnpoint(bp2);
  bp3 = new Bodypart(BPT_Leaf,"TESTPART3",organism);
  if(!(bp->spawnBodypart(bp2))) cout << "[!1] bp2 not spawned" << endl;
  if(!(bp->spawnBodypart(bp3))) cout << "[!1] bp3 not spawned" << endl;
  bp3 = new Bodypart(BPT_Leaf,"TESTPART3",organism);
  if(!(bp->spawnBodypart(bp3))) cout << "[!1] bp4 not spawned" << endl;
  bp3->getChemicalStorage()->add("Energie",20.0f);

/////////////////////////////////////////////////////////////////////////////

  bp = new Bodypart(BPT_Stick,"TESTPART4");
  organism = new Organism("TestOrganism2", bp, sun);
  organism->connectToGoundpart(gp);
  op1->add(organism);
  bp2 = new Bodypart(BPT_Stick,"TESTPART4",organism);
  bp->occupieSpawnpoint(bp2);
  bp3 = new Bodypart(BPT_Leaf,"TESTPART4",organism);
  if(!(bp->spawnBodypart(bp2))) cout << "[!2] bp2 not spawned" << endl;
  if(!(bp->spawnBodypart(bp3))) cout << "[!2] bp3 not spawned" << endl;
  //bp3 = new Bodypart(BPT_Leaf,"TESTPART4",organism);
  //if(!(bp->spawnBodypart(bp3))) cout << "[!2] bp4 not spawned" << endl;
  bp3->getChemicalStorage()->add("Energie",10.0f);

//////////////////////////////////////////////////////////////////////////////

  //bp = new Bodypart(BPT_Stick,"TESTPART4");
  //organism = new Organism("TestOrganism2", bp, sun);
  //organism->connectToGoundpart(gp);
  //op1->add(organism);
  //bp2 = new Bodypart(BPT_Branch,"TESTPART4",organism);
  //bp->occupieSpawnpoint(bp2);
  //bp3 = new Bodypart(BPT_Stick,"TESTPART4",organism);
  //if(!(bp->spawnBodypart(bp2))) cout << "[!2] bp2 not spawned" << endl;
  //if(!(bp2->spawnBodypart(bp3))) cout << "[!2] bp3 not spawned" << endl;
  //bp3 = new Bodypart(BPT_Leaf,"TESTPART4",organism);
  //if(!(bp->spawnBodypart(bp3))) cout << "[!2] bp4 not spawned" << endl;
  //bp3->getChemicalStorage()->add("Energie",10.0f);

//////////////////////////////////////////////////////////////////////////////

  cout << "Adding some initial Water ..." << endl;
  gp->getChemicalStorage()->add("Wasser",STARTING_WATER);
  gp->getChemicalStorage()->add("Goo",10.0f);
  cout << endl;

  printOrgs();
  run(1);
  wait_for_events();
};

void growthTest() {
  printall = false;
  Groundpart* gp = new Groundpart();
  Bodypart* bp = new Bodypart(BPT_Stick,"TESTPART3");

  organism = new Organism("TestOrganism", bp);
  organism->connectToGoundpart(gp);

  op1 = new SDLOrganismPrinter();
  op1->add(organism);

  Bodypart* bp2 = new Bodypart(BPT_Branch,"TESTPART3",organism);
  Bodypart* bp3 = new Bodypart(BPT_Leaf,"TESTPART3",organism);

  bp->occupieSpawnpoint(bp2);

  if(!(bp->spawnBodypart(bp2))) cout << "[!!] bp2 not spawned" << endl;
  if(!(bp->spawnBodypart(bp3))) cout << "[!!] bp3 not spawned" << endl;

  cout << "Adding some initial Water ..." << endl;
  gp->getChemicalStorage()->add("Wasser",STARTING_WATER);
  outputWaterAndGold(gp->getChemicalStorage());
  cout << endl;

  printOrgs();

  int i = 0;
  while(i <= CYCLES_TO_RUN)
  {
    run(1);
    //if(bp3->getBodypartState() == BSP_dead) printall = true;

    //////if(bp.getChemicalStorage()->getCurrentAmount("Energie") > 0.01)

    //if(i%10999 == 0)  
    //  bp->getChemicalStorage()->add("Wasser",99.0f);

    //////bp->getChemicalStorage()->add("Wasser",0.55f);

    //bp->getChemicalStorage()->add("Wasser",0.6f);
    
    //{ float currentWater = bp->getChemicalStorage()->getCurrentAmount("Wasser");
    //  if(currentWater < 50.0) bp->getChemicalStorage()->add("Wasser",13.0f);// - currentWater);
    //}

    if((i%PRINT_OUT_ALL_N_CYCLES == 0) || printall) {
      cout << " - - - - - - - - - - - - - [" << i/1000 << "k]" << endl;
      outputWaterAndGold(gp->getChemicalStorage());
      
      if((organism != 0) && (organism->getState() != BSP_dead))
        outputWaterAndGold(organism->getRootBodypart()->getChemicalStorage());
      
      if((organism2 != 0) && (organism2->getState() != BSP_dead))
        outputWaterAndGold(organism2->getRootBodypart()->getChemicalStorage());

      printOrgs();
      cout << endl; 
    };


    i++;
  };

  organism2 = bp2->detachToNewOrganism();
  organism2->connectToGoundpart(gp);
  op2 = new OrganismPrinter(organism2);

  i = 0;
  while(i <= CYCLES_TO_RUN)
  {

    run(SND_RUN_MULTIPLYER);
    //if(bp3->getBodypartState() == BSP_dead) printall = true;

    //////if(bp.getChemicalStorage()->getCurrentAmount("Energie") > 0.01)

    //if(i%10999 == 0)  
    //  bp->getChemicalStorage()->add("Wasser",99.0f);

    //////bp->getChemicalStorage()->add("Wasser",0.55f);

    //bp->getChemicalStorage()->add("Wasser",0.6f);
    
    //{ float currentWater = bp->getChemicalStorage()->getCurrentAmount("Wasser");
    //  if(currentWater < 50.0) bp->getChemicalStorage()->add("Wasser",13.0f);// - currentWater);
    //}

    if((i%PRINT_OUT_ALL_N_CYCLES == 0) || printall) {
      cout << " - - - - - - - - - - - - - [" << (i/1000)* SND_RUN_MULTIPLYER << "k]" << endl;
      outputWaterAndGold(gp->getChemicalStorage());
      
      if((organism != 0) && (organism->getState() != BSP_dead))
        outputWaterAndGold(organism->getRootBodypart()->getChemicalStorage());
      
      if((organism2 != 0) && (organism2->getState() != BSP_dead))
        outputWaterAndGold(organism2->getRootBodypart()->getChemicalStorage());
      
      printOrgs();
      cout << endl; 
    };

    i++;
  };

  delete op1;
  delete op2;
  delete organism;
  delete organism2;
};

void destructTest() {
  Bodypart* bp;
  Organism* org;
  //ChemicalStorage* cs;
  //GeneticProcessor* gp;
  for(int i = 0; i < 100000; i++) {
    bp = new Bodypart(BPT_Stick,"TESTPART3");
    org = new Organism("TestOrganism",bp);
    // std::cout << " " << bp->getGeneticCode() << ",";
    delete org;
    
    //cs = new ChemicalStorage();
    //cs->addNew("TestChem",100.0f,100.0f);
    //delete cs;

    //gp = new GeneticProcessor(bp);
    //gp->parseGeneticCode();
    //delete gp;

    std::cout << ".";
  };
  cout << "RGR" <<std::endl;

};

int _tmain(int argc, _TCHAR* argv[])
{
  cout << "\n\t\tThis is the E-Den-Testbench\n" << endl;

  sdl_test();
  //growthTest();
  //destructTest();
    
  cout << "~-[" << "Finished" << "]-~" << endl;

	return 0;
}

