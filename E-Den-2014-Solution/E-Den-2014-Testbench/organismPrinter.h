// organismPrinter.h
// by Franz Koehler 2009

#ifndef _E_DEN_ORGANISMPRINTER_HEADER_
#define _E_DEN_ORGANISMPRINTER_HEADER_

#include "organism.h"
#include "runtimeManager.h"
#include "SDL.h"
#include "SDL_draw.h"
#include "SDL_DirectPixelManipulation.h"

#include <Gwen/Gwen.h>
#include <Gwen/Skins/Simple.h>
#include <Gwen/Skins/TexturedBase.h>
#include <Gwen/UnitTest/UnitTest.h>
#include <Gwen/Input/SDL2.h>
#include <Gwen/Renderers/SDL2.h>

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
	struct ResourceButtonEventHandler :public Gwen::Event::Handler
	{
		Groundpart* gp;
		std::string resourceName;
		bool increment;

		ResourceButtonEventHandler(Groundpart* gp, std::string resourceName, bool increment)
			:gp(gp),
			resourceName(resourceName),
			increment(increment)
		{
		}

		void onClick(Gwen::Controls::Base* pControl);
	};

	struct OrganismCountButtonEventHandler :public Gwen::Event::Handler
	{
		Groundpart* gp;
		RuntimeManager* runtime;
		Gwen::Controls::Label* label;
		bool increment;

		OrganismCountButtonEventHandler(Groundpart* gp, RuntimeManager* runtime, Gwen::Controls::Label* label, bool increment)
			:gp(gp),
			runtime(runtime),
			increment(increment),
			label(label)
		{
		}

		void onClick(Gwen::Controls::Base* pControl);
	};

	SDL_Renderer *renderer;
	Gwen::Renderer::SDL2 *gwenRenderer;
	Gwen::Controls::Canvas* pCanvas;
	Gwen::Input::SDL2* GwenInput;

	RuntimeManager* runtime;
    Groundpart* groundpart;
	SingleDimensionHeightmapGroundpart* extendedGroundpart;
    SDL_SunlightProvider* sun;
	SDL_ShadowAccumulator* shadows;

    double scale;
	double renderOffeset;
	double moveMomentum;
	bool moveRight;
    bool moveLeft;
	bool needToScaleY;
	bool scaleDownLeft;
	bool scaleDownRight;
	bool scaleDown;

	bool drawLightDebug;

    bool cleanupDeadOrganisms();
    int req_print(Bodypart* param_bp, int param_x, int param_y, float p_ang1, float p_ang2, float p_ang3, bool relevantForScaling, bool marked);
	void printHeigtmap();

	Gwen::Controls::Button* waterPlusButton;
	ResourceButtonEventHandler* waterPlusEventHandler;
	Gwen::Controls::Button* waterMinusButton;
	ResourceButtonEventHandler* waterMinusEventHandler;
	
	Gwen::Controls::Button* gooPlusButton;
	ResourceButtonEventHandler* gooPlusEventHandler;
	Gwen::Controls::Button* gooMinusButton;
	ResourceButtonEventHandler* gooMinusEventHandler;
	
	Gwen::Controls::Button* organismsPlusButton;
	OrganismCountButtonEventHandler* organismsPlusButtonEventHandler;
	Gwen::Controls::Button* organismsMinusButton;
	OrganismCountButtonEventHandler* organismsMinusButtonEventHandler;
	
	Gwen::Controls::Label* organismsNumberLabel;
	int numOrganisms, numEmptySpaces,cps;

  protected:
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

    int getDimX();
    int getDimY();
	int getOffX();
	int getOffY();

	void setDrawLightDebug(bool lightDebug);
	bool getDrawLightDebug();

	void setOffset(int param_offX, int param_offY);

	void processEvent(SDL_Event* evt);
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
		enum ShadowState {
			BRIGHT_LIT = 8,
			LIT = 4,
			DARK = 2,
			BLACK = 1
		};

		SDL_ShadowAccumulator(int x, int y, int w, int h, SDL_SunlightProvider* provider, SDL_Renderer *renderer);
		void add(Bodypart* bodypart, int x1, int y1, int x2, int y2);
		void distribute();
		void draw();
		void clear();
		void setSize(int dimx, int dimy);

		ShadowState getShadowStateAt(int x, int y);
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