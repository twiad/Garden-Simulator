// E-Den-Testbench.cpp : Defines the entry point for the console application.
// by Franz Koehler 2009

#include "bodypart.h"
#include "organismPrinter.h"
#include "runtimeManager.h"
#include "SDL.h"
#include "SDL_draw.h"

#include "shlobj.h"
#include "Shlwapi.h"
#include <string.h>
#include <direct.h>

#define CYCLES_TO_RUN 3000
#define PRINT_OUT_ALL_N_CYCLES 3000
#define SND_RUN_MULTIPLYER 2
// STARTING_WATER_max: 2.0e9
#define MAX_WATER 2.0e9
#define STARTING_WATER 2.0e9
#define MAX_GOO 2.0e8
#define STARTING_GOO 1.5e8
#define CHEM_SYSTEM_CLK_DEVIDER 1
#define SDL_RUN_FACTOR 2
#define SDL_IDEL_CYCLES 1
#define SDL_DIMX 1270
#define SDL_DIMY 500

using namespace EDen; 
using namespace std;

SDL_Window* window = 0;
SDL_Renderer* renderer = 0;

Organism* organism = 0;
Organism* organism2 = 0;
Bodypart* bp3 = 0;
Groundpart* gp = 0;
Groundpart* gp2 = 0;
Groundpart* gp3 = 0;
SDLOrganismPrinter* op1 = 0;
SDLOrganismPrinter* op3 = 0;
SDLOrganismPrinter* op4 = 0;
SDLOrganismPrinter* activePrinter = 0;
ChemicalStorageLink* chemLink1 = 0;
ChemicalStorageLink* chemLink2 = 0;
ChemicalStorageLink* chemLink3 = 0;

OrganismPrinter* op2 = 0;
RuntimeManager* runtime;
RuntimeManager* runtime2;
int cyclecount = 0;

TCHAR appSettingsPath[MAX_PATH];
char appSettingsPathP[MAX_PATH];

bool printall;
bool pause = false;
bool slowMode = false;

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
  if(runtime) {
    for(int i = 0; i < cycles; i++) {
        runtime->update();
		if(chemLink1) chemLink1->update();
		if(chemLink2) chemLink2->update();
		if(chemLink3) chemLink3->update();
	}
  };
  
};

bool updateCaption() {
    std::string newCaption = "";
    char str[255];
    
    //if(runtimeManager) {
    //  sprintf(str,"[%d|%lu]-",runtimeManager->getCps(), runtimeManager->getCycleCount());
    //  newCaption += str;
    //}

    if(runtime) {
		newCaption.append(runtime->getDebugOut(true)).append("-");
		//std::list<Organism*> organisms = runtime->getOrganisms();

		//for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
		//  sprintf(str,"(%d|%d|%d)",(*it)->getRootBodypart()->getGeneticCode()->getSpeciesIdentifier(),(*it)->getBodypartCount(),(*it)->getLifetime());
		//  newCaption += str;
		//};

		//newCaption.append(runtime->getDebugOut());
	}

	if(gp) {
		newCaption.append(gp->getSpeciesDatabase()->getDebugOut());
	}

    SDL_SetWindowTitle(window,newCaption.c_str());
    return true;
};

void sdl_run(int cycles) {
  run(cycles);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);

  op1->print();
  op1->printOutPercentage(gp->getChemicalStorage()->getCurrentPercentage("Wasser"));
  op1->printOutPercentage(gp->getChemicalStorage()->getCurrentPercentage("Goo"));

  if(op3) {
	  op3->print();
	  op3->printOutPercentage(gp3->getChemicalStorage()->getCurrentPercentage("Wasser"));
	  op3->printOutPercentage(gp3->getChemicalStorage()->getCurrentPercentage("Goo"));
  }

  if(op4) {
	  op4->print();
	  op4->printOutPercentage(gp2->getChemicalStorage()->getCurrentPercentage("Wasser"));
	  op4->printOutPercentage(gp2->getChemicalStorage()->getCurrentPercentage("Goo"));
  }

  SDL_RenderPresent(renderer);
  
  if(runtime) {
	if(runtime->getState() == RMS_Slow)
	    updateCaption();
	else if(runtime->getCycleCount() % 10 == 0)
		updateCaption();
	if(runtime->getCycleCount() % 9990 == 0) if(gp) gp->saveDatabase();
  }
//  printf("bp3.maxSize: %f\n", bp3->getMaxSize());
//  printOrgs();
}

void updateRuntimeState() {
  if(slowMode && !pause) runtime->setState(RMS_Slow);
  else if(pause) runtime->setState(RMS_Pause);
  else runtime->setState(RMS_Normal);
};

void switchActivePrinter() {
  if(activePrinter == 0)  {
    if(op1 != 0) activePrinter = op1;
    else activePrinter = op3;
  } 
  else if(activePrinter == op1) {
    if(op3 != 0) activePrinter = op3;
  }
  else if(activePrinter == op3) {
    if(op1 != 0) activePrinter = op1;
  };
};

bool wait_for_events()
{
  SDL_Event event;
  const char *key;
  SDL_Keymod modifier;
  bool quit = false;

  printf("waiting for events, press 'q' or 'ESC' to quit\n");
  while ( !quit ) {				
    if(SDL_PollEvent(&event)) {	
	    switch (event.type) {		//check the event type
		    case SDL_KEYDOWN:			//if a key has been pressed
          key = SDL_GetKeyName(event.key.keysym.sym);
          modifier = SDL_GetModState();
		      printf("The %s key was pressed!\n", key );
          if ( event.key.keysym.sym == SDLK_ESCAPE )	//quit if 'ESC' pressed
	          quit = true;
		  else if ( key[0] == 'Q'  )	//quit if 'q'  pressed
	          quit = true;			
          else if ( key[0] == 'S'  )  {//save if 's' pressed
            //runtime->saveDatabase();
		    if(gp) gp->saveDatabase();
          //else if ( key[0] == 'L'  )  //load if 'l' is pressed
            //runtime->loadDatabase();
		  }
          else if ( key[0] == 'Z'  ) {  //slow if 'z' is pressed
            slowMode = !slowMode;
            updateRuntimeState();
          }
		  else if ( key[0] == 'D'  ) {  //debug displays
			  if(op1) op1->setDrawLightDebug(!op1->getDrawLightDebug());
			  if(op3) op3->setDrawLightDebug(!op3->getDrawLightDebug());
			  if(op4) op4->setDrawLightDebug(!op4->getDrawLightDebug());
          }
          else if ( key[0] == 'P'  ) { //pause if 'p' is pressed
            pause = !pause;
            updateRuntimeState();
          }
          else if ( key[0] == 'M'  ) {
			  if(gp != 0) if(gp->incEmptySpaces()) if(runtime) runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() + 1);
			  if(gp2 != 0) if(gp2->incEmptySpaces()) if(runtime) runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() + 1);
		      if(gp3 != 0) if(gp3->incEmptySpaces()) if(runtime) runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() + 1);

		  }
          else if ( key[0] == 'N'  )
			if((runtime->getPreferedOrganismCount() - 2) > 0) {
				if(modifier == KMOD_RCTRL) {
                  if(gp != 0) if(gp->decEmptySpaces()) if(runtime) runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() - 1);
				  if(gp2 != 0) if(gp2->decEmptySpaces()) if(runtime) runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() - 1);
				  if(gp3 != 0) if(gp3->decEmptySpaces()) if(runtime) runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() - 1);
				}
				else {
				  if(gp != 0) if(gp->decEmptySpaces()) if(runtime) runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() - 1);
				  if(gp2 != 0) if(gp2->decEmptySpaces()) if(runtime) runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() - 1);
				  if(gp3 != 0) if(gp3->decEmptySpaces()) if(runtime) runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() - 1);
				}
			}
          else if ( key[0] == 'j'  ) { //testmode if 'p' is pressed
          //  switchActivePrinter();
          //  printOrgs();
          }
		    break;
		     case SDL_MOUSEMOTION:             //mouse moved
			     printf("Mouse motion x:%d, y:%d\n", event.motion.x, event.motion.y );
           if(! pause) 
             sdl_run(SDL_IDEL_CYCLES);
           else
             SleepEx(500,true);
			     break;
		     case SDL_MOUSEBUTTONUP:           //mouse button pressed
			     printf("Mouse button %d pressed x:%d, y:%d\n", event.button.button, event.button.x, event.button.y );
           if(event.button.button == 1) {
             gp->getChemicalStorage()->add("Wasser",((float)event.button.x / (float)op1->getDimX()) * ((float)STARTING_WATER / 7.0f));
           }
           else
             gp->getChemicalStorage()->add("Wasser",-((float)event.button.x / (float)op1->getDimX()) * ((float)STARTING_WATER / 7.0f));
			     break; 
	       case SDL_QUIT:
			 SDL_Quit();
	         exit ( 1 );
		     break;
	    }
    }
    else {
      if(! pause) 
        sdl_run(SDL_IDEL_CYCLES);
      else
        SleepEx(500,true);

      if(! runtime->orgsAlive()) {
		SDL_Quit();
        exit(1);
	  }

      if(slowMode) 
        SleepEx(250,true);
    }
  } //while
  if(gp) delete gp;
  if(gp2) delete gp2;
  if(gp3) delete gp3;
  SDL_Quit();
  return true;
}

void sdl_test() {
  SpeciesDatabase* gpDatabase;
  std::string gpdbFilename;
  
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) 
  {
    /*exit with SDL_Init error*/
  }

  runtime = new RuntimeManager();
  gp = new SingleDimensionHeightmapGroundpart("GOO1",SDL_DIMX,MAX_WATER,MAX_GOO*2,1);
  runtime->add(gp);

  gpDatabase = new SpeciesDatabase(runtime);
  gpDatabase->setApplicationSettingsPath(appSettingsPathP);
  gp->setSpeciesDatabase(gpDatabase);
  gp->incEmptySpaces();
  gp->incEmptySpaces();
  //gpdbFilename = gp->getName().append(".xml");
  //gpDatabase->load(gpdbFilename);

  Bodypart* bp,* bp2;
  sun = new SDL_SunlightProvider();

  window = SDL_CreateWindow("An SDL2 window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SDL_DIMX, SDL_DIMY, SDL_WINDOW_OPENGL);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  //int Buffers, Samples;
  //SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &Buffers );
  //SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &Samples );
  //cout << "buf = " << Buffers << ", samples = " << Samples << ".";

  op1 = new SDLOrganismPrinter(renderer, SDL_DIMX, SDL_DIMY, 0, 0, gp, runtime);

  activePrinter = op1;
//  op3 = new SDLOrganismPrinter(&window, SDL_DIMX,SDL_DIMY,runtime);




//////////////////////////////////////////////////////////////////////////////
  //bp = new Bodypart(BPT_Stick,"TESTPART4");
  //organism = new Organism("TestOrganism", bp, runtime);
  ////organism->connectToGoundpart(gp);
  ////op1->add(organism); 
  //runtime->add(organism);
  //bp2 = new Bodypart(BPT_Branch,"TESTPART4",organism);
  //bp->occupieSpawnpoint(bp2);
  //bp3 = new Bodypart(BPT_Leaf,"TESTPART4",organism);
  //if(!(bp->spawnBodypart(bp2))) cout << "[!1] bp2 not spawned" << endl;
  //if(!(bp->spawnBodypart(bp3))) cout << "[!1] bp3 not spawned" << endl;
  //bp3 = new Bodypart(BPT_Leaf,"TESTPART4",organism);
  //if(!(bp->spawnBodypart(bp3))) cout << "[!1] bp4 not spawned" << endl;
  //bp3->getChemicalStorage()->add("Energie",20.0f);

/////////////////////////////////////////////////////////////////////////////

  //bp = new Bodypart(BPT_Stick,"TESTPART4");
  //organism = new Organism("TestOrganism3", bp, runtime);
  //organism->connectToGoundpart(gp);
  //// op1->add(organism); 
  //runtime->add(organism);
  //bp2 = new Bodypart(BPT_Branch,"TESTPART4",organism);
  //bp->occupieSpawnpoint(bp2);
  //bp3 = new Bodypart(BPT_Stick,"TESTPART4",organism);
  //if(!(bp->spawnBodypart(bp2))) cout << "[!2] bp2 not spawned" << endl;
  //if(!(bp2->spawnBodypart(bp3))) cout << "[!2] bp3 not spawned" << endl;
  //bp3 = new Bodypart(BPT_Leaf,"TESTPART4",organism);
  //if(!(bp->spawnBodypart(bp3))) cout << "[!2] bp4 not spawned" << endl;
  //bp3->getChemicalStorage()->add("Energie",10.0f);
  //bp3->getChemicalStorage()->add("Sonne",200.0f);

//////////////////////////////////////////////////////////////////////////////

  gp->getChemicalStorage()->add("Wasser",STARTING_WATER);
  gp->getChemicalStorage()->add("Goo",STARTING_GOO*2);

  runtime->initDatabase(appSettingsPathP);
  //runtime->initDatabase("f:\\tmp\\eden\\");
  run(1);

  bp = new Bodypart(BPT_Seed,"TESTPART3");
  organism = new Organism("TestOrganism2", bp, runtime);
  //organism->connectToGoundpart(gp);
  //op1->add(organism); 
  bp->setBodypartState(BSP_seed);
  runtime->add(organism);
  bp->setScaleModifier(1.0f);
  //bp2 = new Bodypart(BPT_Stick,"TESTPART3",organism);
  //bp->occupieSpawnpoint(bp2);
  //bp3 = new Bodypart(BPT_Leaf,"TESTPART3",organism);
  //if(!(bp->spawnBodypart(bp2))) cout << "[!2] bp2 not spawned" << endl;
  //if(!(bp->spawnBodypart(bp3))) cout << "[!2] bp3 not spawned" << endl;
  //bp3 = new Bodypart(BPT_Leaf,"TESTPART3",organism);
  //if(!(bp->spawnBodypart(bp3))) cout << "[!2] bp4 not spawned" << endl;
  bp->getChemicalStorage()->add("Energie",100.0f);
  //bp2->getChemicalStorage()->add("Energie",100.0f);
  //bp3->getChemicalStorage()->add("Energie",10.0f);
  //bp3->getChemicalStorage()->add("Sonne",200.0f);

  run(1);

  runtime->loadDatabase("autosave.xml");
  gpdbFilename = gp->getName().append(".xml");
  gpDatabase->load(gpdbFilename);
  /*gp2 = new SingleDimensionHeightmapGroundpart("WATER1", 200, MAX_WATER*2, MAX_GOO/2, 1);
  runtime->add(gp2);
  
  gpDatabase = new SpeciesDatabase(runtime);
  gpDatabase->setApplicationSettingsPath(appSettingsPathP);
  gp2->setSpeciesDatabase(gpDatabase);
  gpdbFilename = gp2->getName().append(".xml");
  gpDatabase->load(gpdbFilename);

  gp3 = new SingleDimensionHeightmapGroundpart("WATER2", 200, MAX_WATER*2, MAX_GOO/2, 1);
  runtime->add(gp3);

  gpDatabase = new SpeciesDatabase(runtime);
  gpDatabase->setApplicationSettingsPath(appSettingsPathP);
  gp3->setSpeciesDatabase(gpDatabase);
  gpdbFilename = gp3->getName().append(".xml");
  gpDatabase->load(gpdbFilename);

  gp2->getChemicalStorage()->add("Wasser",STARTING_WATER*2);
  gp2->getChemicalStorage()->add("Goo",STARTING_GOO/2);
  gp3->getChemicalStorage()->add("Wasser",STARTING_WATER*2);
  gp3->getChemicalStorage()->add("Goo",STARTING_GOO/2);

  chemLink1 = new ChemicalStorageLink(gp->getChemicalStorage(),gp2->getChemicalStorage());
  chemLink2 = new ChemicalStorageLink(gp2->getChemicalStorage(),gp3->getChemicalStorage());
  chemLink3 = new ChemicalStorageLink(gp3->getChemicalStorage(),gp->getChemicalStorage());

  op3 = new SDLOrganismPrinter(renderer, (SDL_DIMX/5)*2,SDL_DIMY,0, 0, gp3, runtime);
  op4 = new SDLOrganismPrinter(renderer, (SDL_DIMX/5)*2,SDL_DIMY,(SDL_DIMX/5)*3, 0, gp2, runtime);*/

  run(1);

  wait_for_events();
};

//void growthTest() {
//  printall = false;
//  Groundpart* gp = new Groundpart(MAX_WATER,MAX_GOO);
//  Bodypart* bp = new Bodypart(BPT_Stick,"TESTPART3");
//
//  organism = new Organism("TestOrganism", bp);
//  organism->connectToGoundpart(gp);
//
//  op1 = new SDLOrganismPrinter(&window);
//  op1->add(organism);
//
//  Bodypart* bp2 = new Bodypart(BPT_Branch,"TESTPART3",organism);
//  Bodypart* bp3 = new Bodypart(BPT_Leaf,"TESTPART3",organism);
//
//  bp->occupieSpawnpoint(bp2);
//
//  if(!(bp->spawnBodypart(bp2))) cout << "[!!] bp2 not spawned" << endl;
//  if(!(bp->spawnBodypart(bp3))) cout << "[!!] bp3 not spawned" << endl;
//
//  cout << "Adding some initial Water ..." << endl;
//  gp->getChemicalStorage()->add("Wasser",STARTING_WATER);
//  outputWaterAndGold(gp->getChemicalStorage());
//  cout << endl;
//
//  printOrgs();
//
//  int i = 0;
//  while(i <= CYCLES_TO_RUN)
//  {
//    run(1);
//    //if(bp3->getBodypartState() == BSP_dead) printall = true;
//
//    //////if(bp.getChemicalStorage()->getCurrentAmount("Energie") > 0.01)
//
//    //if(i%10999 == 0)  
//    //  bp->getChemicalStorage()->add("Wasser",99.0f);
//
//    //////bp->getChemicalStorage()->add("Wasser",0.55f);
//
//    //bp->getChemicalStorage()->add("Wasser",0.6f);
//    
//    //{ float currentWater = bp->getChemicalStorage()->getCurrentAmount("Wasser");
//    //  if(currentWater < 50.0) bp->getChemicalStorage()->add("Wasser",13.0f);// - currentWater);
//    //}
//
//    if((i%PRINT_OUT_ALL_N_CYCLES == 0) || printall) {
//      cout << " - - - - - - - - - - - - - [" << i/1000 << "k]" << endl;
//      outputWaterAndGold(gp->getChemicalStorage());
//      
//      if((organism != 0) && (organism->getState() != BSP_dead))
//        outputWaterAndGold(organism->getRootBodypart()->getChemicalStorage());
//      
//      if((organism2 != 0) && (organism2->getState() != BSP_dead))
//        outputWaterAndGold(organism2->getRootBodypart()->getChemicalStorage());
//
//      printOrgs();
//      cout << endl; 
//    };
//
//
//    i++;
//  };
//
//  organism2 = bp2->detachToNewOrganism();
//  organism2->connectToGoundpart(gp);
//  op2 = new OrganismPrinter(organism2);
//
//  i = 0;
//  while(i <= CYCLES_TO_RUN)
//  {
//
//    run(SND_RUN_MULTIPLYER);
//    //if(bp3->getBodypartState() == BSP_dead) printall = true;
//
//    //////if(bp.getChemicalStorage()->getCurrentAmount("Energie") > 0.01)
//
//    //if(i%10999 == 0)  
//    //  bp->getChemicalStorage()->add("Wasser",99.0f);
//
//    //////bp->getChemicalStorage()->add("Wasser",0.55f);
//
//    //bp->getChemicalStorage()->add("Wasser",0.6f);
//    
//    //{ float currentWater = bp->getChemicalStorage()->getCurrentAmount("Wasser");
//    //  if(currentWater < 50.0) bp->getChemicalStorage()->add("Wasser",13.0f);// - currentWater);
//    //}
//
//    if((i%PRINT_OUT_ALL_N_CYCLES == 0) || printall) {
//      cout << " - - - - - - - - - - - - - [" << (i/1000)* SND_RUN_MULTIPLYER << "k]" << endl;
//      outputWaterAndGold(gp->getChemicalStorage());
//      
//      if((organism != 0) && (organism->getState() != BSP_dead))
//        outputWaterAndGold(organism->getRootBodypart()->getChemicalStorage());
//      
//      if((organism2 != 0) && (organism2->getState() != BSP_dead))
//        outputWaterAndGold(organism2->getRootBodypart()->getChemicalStorage());
//      
//      printOrgs();
//      cout << endl; 
//    };
//
//    i++;
//  };
//
//  delete op1;
//  delete op2;
//  delete organism;
//  delete organism2;
//};

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

void simpleSdl_test() {
	SDL_Window* window = SDL_CreateWindow("An SDL2 window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 400, 400, SDL_WINDOW_OPENGL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,0);
	SDL_Surface* surface = SDL_GetWindowSurface(window);
	SDL_Texture* texture = SDL_CreateTexture(renderer, surface->format->BitsPerPixel, SDL_TEXTUREACCESS_STREAMING, surface->w, surface->h);

	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); 
	for(int i = 1; i < 399; i = i + 5) {
		SDL_RenderDrawLine(renderer,i,0,400-i,400);
	}
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
	SDL_RenderPresent(renderer);

	SleepEx(1500,true);

	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	SleepEx(1500,true);
}

void TCharToChar(const wchar_t* Src, char* Dest, int Size)
{
	WideCharToMultiByte(CP_ACP, 0, Src, wcslen(Src)+1, Dest , Size, NULL, NULL);
}

int main(int argc, char **argv)
{
  SHGetSpecialFolderPath(0,appSettingsPath,CSIDL_APPDATA,1);
  TCharToChar(appSettingsPath, appSettingsPathP, MAX_PATH);
  char * subdir = "\\eden";
  strcat(appSettingsPathP,subdir);
  mkdir(appSettingsPathP);

  cout << "\n\t\tThis is the E-Den-Testbench\n" << endl;
  cout << "loading autosave from " << appSettingsPathP << endl;

  sdl_test();
  //simpleSdl_test();
  //growthTest();
  //destructTest();
    
  cout << "~-[" << "Finished" << "]-~" << endl;

  return 0;
}

