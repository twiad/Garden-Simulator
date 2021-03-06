#pragma once
#include "groundpart.h"
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

#include "organismPrinter.h" // for now ... sunlight and shadow providers still in there

#include "SDLGwenOrgnismInspector.h"
#include "SDLGwenStatusWindow.h"
#include "SDLGwenStatisticsWindow.h"

namespace EDen {
	class SDLGwenStatusWindow;
	class SDLGwenStatisticsWindow;

	class SDL_WindowGroundpart : public EDen::SingleDimensionHeightmapGroundpart {
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

		struct StatusWindowButtonEventHandler :public Gwen::Event::Handler
		{
			SDL_WindowGroundpart* gp;
			
			StatusWindowButtonEventHandler(SDL_WindowGroundpart* gp)
				:gp(gp)
			{
			}

			void onClick(Gwen::Controls::Base* pControl);
		};

		struct ViewportClippingInformation {
			bool moveLeft;
			bool moveRight;
			bool needToScaleY;
			bool scaleDownLeft;
			bool scaleDownRight;
			bool scaleDown;

			ViewportClippingInformation();
			void reset();
		};

		SDL_Window* window;
		SDL_Renderer* renderer;
		Gwen::Renderer::Base* gwenRenderer;
		Gwen::Controls::Canvas* pCanvas;
		Gwen::Input::SDL2* GwenInput;

		SDL_Surface* softwareRendererSurface;
		SDL_Renderer* softwareRenderer;
		SDL_Renderer* hardwareRenderer;

		RuntimeManager* runtime;
		SDL_SunlightProvider* sun;
		SDL_ShadowAccumulator* shadows;

		double scale;
		double renderOffeset;
		double moveMomentum;
		ViewportClippingInformation clip;

		bool drawLightDebug;

		int req_print(Bodypart* param_bp, int param_x, int param_y, float p_ang1, float p_ang2, float p_ang3, bool relevantForScaling, bool marked, bool fast);
		void printHeigtmap();
		
		SDLGwenStatisticsWindow* statsWindow;

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
	
		Gwen::Controls::Label* waterPercentageLabel;
		Gwen::Controls::Label* gooPercentageLabel;
		Gwen::Controls::Label* organismsNumberLabel;

		Gwen::Controls::Button* statusWindowButton;
		StatusWindowButtonEventHandler* statusWindowButtonEventHandler;

		Gwen::Controls::Label* statusLabel;

		SDLGwenOrgnismInspector* orgInsprector;
		SDLGwenStatusWindow* statusWindow;

		int numOrganisms, numEmptySpaces,cps;
		float waterPercentage, gooPercentage;

		void adjustViewport();
	  protected:
		int dimx;
		int dimy;

		Organism* primaryMarkedOrganism;
		std::list<Organism*> scaleToOrganisms;
		bool isScaleToOrganism(Organism* org);

		void resizeWindow(int dimx, int dimy);

		void hideStatusWindow();
	  public:
		SDL_WindowGroundpart(std::string name, bool softwareRendered, int width, int height, float maxWater = 2.0e8, float maxGoo = 2.0e8, int emptySpaces = 1, RuntimeManager* runtimeManager = 0);
		~SDL_WindowGroundpart();

		RuntimeManager* getRuntimeManager();

		virtual bool registerOrganism(Organism* param_organism);
        virtual bool unregisterOrganism(Organism* param_organism);

		virtual bool print(bool fast = false);

		bool add(Organism* param_organism);
		std::list<Organism*>* getOrganisms();

		void clearScaleToOrganisms();
		void addScaleToOrganism(Organism* org);

		bool orgsAlive();
		unsigned int getNumSpecies();
		void getDebugOut(std::map<unsigned int, std::string>* outList);

		void setStatusLabelText(std::string text);

		int getDimX();
		int getDimY();

		void setDrawLightDebug(bool lightDebug);
		bool getDrawLightDebug();

		void processEvent(SDL_Event* evt);
		Gwen::Controls::Canvas* getCanvas();
		void toggleStatusWindow();
	};
} // namespace