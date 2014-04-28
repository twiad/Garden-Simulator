#pragma once

#include <Gwen/Gwen.h>
#include <Gwen/Skins/Simple.h>
#include <Gwen/Skins/TexturedBase.h>
#include <Gwen/UnitTest/UnitTest.h>
#include <Gwen/Input/SDL2.h>
#include <Gwen/Renderers/SDL2.h>

#include "runtimeManager.h"

#include "SDL_WindowGroundpart.h"

namespace EDen {
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

		SDL_Window *window;
		Gwen::Renderer::SDL2 *pRenderer;
		Gwen::Controls::Canvas* pCanvas;
		Gwen::Input::SDL2* GwenInput;

		SDL_WindowGroundpart* gp;

		Gwen::Controls::Button* testButton;
		Gwen::Controls::ListBox* listBox;
		ListItemHoverEventHandler* hoverHandler;
	public:
		SDLGwenStatusWindow(SDL_WindowGroundpart* groundpart);
		~SDLGwenStatusWindow();

		void processEvent(SDL_Event* evt);
		void update();
	};
};