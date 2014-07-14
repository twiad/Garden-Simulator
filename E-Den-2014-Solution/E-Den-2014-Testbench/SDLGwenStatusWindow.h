#pragma once

#include <Gwen/Gwen.h>
#include <Gwen/Skins/Simple.h>
#include <Gwen/Skins/TexturedBase.h>
#include <Gwen/UnitTest/UnitTest.h>
#include <Gwen/Input/SDL2.h>
#include <Gwen/Renderers/SDL2.h>

#include "runtimeManager.h"

#include "SDL_WindowGroundpart.h"

#define STATUS_WINDOW_DIM_X 200
#define STATUS_WINDOW_DIM_Y 400

namespace EDen {
	class SDL_WindowGroundpart;

	class SDLGwenStatusWindow
	{
	private:
		struct ListItemHoverEventHandler :public Gwen::Event::Handler
		{
			SDL_WindowGroundpart* gp;
			int currentSpeciesID;

			ListItemHoverEventHandler(SDL_WindowGroundpart* gp) :gp(gp), currentSpeciesID(0)
			{
			}

			void onHoverIn(Gwen::Controls::Base* pControl);
			void onHoverOut(Gwen::Controls::Base* pControl);
		};

		SDL_WindowGroundpart* gp;

		int width;

		Gwen::Controls::ListBox* listBox;
		ListItemHoverEventHandler* hoverHandler;
	public:
		SDLGwenStatusWindow(SDL_WindowGroundpart* groundpart, int width = STATUS_WINDOW_DIM_X);
		~SDLGwenStatusWindow();

		bool isShown();
		void show(bool shown);
		void resizeWindow(int dimx, int dimy);

		void processEvent(SDL_Event* evt);
		void update();
		void getSize(int* w, int* h);
		void getPosition(int* x, int* y);
	};
};