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
  class SDL_ShadowAccumulator;

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
	SDL_Renderer *renderer;

	RuntimeManager* runtime;
    Groundpart* groundpart;
	SingleDimensionHeightmapGroundpart* extendedGroundpart;
    SDL_SunlightProvider* sun;
	SDL_ShadowAccumulator* shadows;

    double scale;
    bool needToScaleX;
	bool needToScaleY;

	bool drawLightDebug;

    bool cleanupDeadOrganisms();
    int req_print(Bodypart* param_bp, int param_x, int param_y, float p_ang1, float p_ang2, float p_ang3);
	void printHeigtmap();
    int percentagePrinterCounter;
  protected:
	//GroundpartList groundparts;
    std::list<Organism*> organisms;
    int dimx;
    int dimy;
	int offX;
	int offY;
  public:
    SDLOrganismPrinter(SDL_Renderer *renderer, int param_dimx = 500, int param_dimy = 800, int param_offx = 0, int param_offy = 0, Groundpart* param_groundPart = 0, RuntimeManager* runtimeManager = 0);
    ~SDLOrganismPrinter();

    virtual bool print();

    bool add(Organism* param_organism);
	//bool add(Groundpart* param_groundpart);
    std::list<Organism*> getOrganisms();
	//GroundpartList getGroundparts();

    bool orgsAlive();
    bool printOutPercentage(float value);

    int getDimX();
    int getDimY();
	int getOffX();
	int getOffY();

	void setDrawLightDebug(bool lightDebug);
	bool getDrawLightDebug();

	void setOffset(int param_offX, int param_offY);
  };

  class SDL_ShadowAccumulator {
	private:
		struct line {
			int x1;
			int y1;
			int x2;
			int y2;

			line(int px1, int py1, int px2, int py2): x1(px1),y1(py1),x2(px2),y2(py2) {};
		};

		struct color {
			float r,g,b;

			color(float pr, float pg, float pb): r(pr),g(pg),b(pb) {};
			color(EDen::Color* c): r(c->r * c->a), g(c->g * c->a), b(c->b * c->a) {};
		};

		struct shadowInformation {
			int y;
			color c;

			shadowInformation(int py, color pc): y(py),c(pc) {};
		};

		std::map<int,std::list<shadowInformation>*> shadowMap; // list of shadowcastinformation per X ordered by Y

		std::list<std::pair<Bodypart*, line>*>  lightConsumer;

		BodypartType lightConsumerType;
		BodypartType shadowcasterType;

		void add(int x1, int y1, int x2, int y2, color c);
		void insertOrdered(std::list<shadowInformation>* list, shadowInformation info);
		color getShadowAt(int x, int y);

		SDL_SunlightProvider* provider;

		SDL_Renderer* renderer;
		int x,y,w,h;
	public:
		SDL_ShadowAccumulator(int x, int y, int w, int h, SDL_SunlightProvider* provider, SDL_Renderer *renderer);
		void add(Bodypart* bodypart, int x1, int y1, int x2, int y2);
		void distribute();
		void draw();
		void clear();
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