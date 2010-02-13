// organismPrinter.h
// by Franz Koehler 2009

#ifndef _E_DEN_ORGANISMPRINTER_HEADER_
#define _E_DEN_ORGANISMPRINTER_HEADER_

#include "organism.h"
#include "runtimeManager.h"
#include "SDL.h"
#include "SDL_draw.h"
#include "SDL_DirectPixelManipulation.h"

namespace EDen {

  class SDL_SunlightProvider;

  class OrganismPrinter {
  private:
    bool print(Bodypart* bp, bool linebreak = true, bool lastelement = true, bool branch = false, int lvl = 0, int targetlvl = 0);
    bool printLetter(Bodypart* bp, bool linebreak = true, bool lastelement = true, bool tab = false);
  protected:
    Organism* org;
  public:
    OrganismPrinter(Organism* param_organism): org(param_organism) {};
    ~OrganismPrinter() {};

    virtual bool print();
  };

  class SDLOrganismPrinter {
  private:
    SDL_Surface *screen;
    SDL_Surface *pscreen;

    RuntimeManager* runtimeManager;
    SDL_SunlightProvider* sun;

    float scale;
    bool needToScale;

    bool updateCaption();
    bool resetScreen();
    bool cleanupDeadOrganisms();
    int req_print(Bodypart* param_bp, int param_x, int param_y, float p_ang1, float p_ang2, float p_ang3);
    int percentagePrinterCounter;
  protected:
    std::list<Organism*> organisms;

    int dimx;
    int dimy;
  public:
    SDLOrganismPrinter(int param_dimx = 500, int param_dimy = 800, RuntimeManager* param_runtimeUnit = 0);
    ~SDLOrganismPrinter();

    virtual bool print();

    bool add(Organism* param_organism);
    std::list<Organism*> getOrganisms();

    bool orgsAlive();
    bool redrawScreen();
    bool printOutPercentage(float value);
  };

  class SDL_SunlightProvider : public ResourceProvider {
  private:
//    SDLOrganismPrinter* printer;
  public:
    SDL_SunlightProvider() : ResourceProvider("Sonne",BPT_Leaf,0.9f) {
      // printer = param_printer;
      amount = 0.9f;
      reactiveBodypartType = BPT_Leaf;
      chemicalName = "Sonne";
    };

    ~SDL_SunlightProvider() {};

    bool setFactor(Bodypart* param_bodypart ,float param_factor);
  };
}

#endif