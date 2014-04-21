#pragma once

#include <Gwen/Gwen.h>
#include <Gwen/Skins/Simple.h>
#include <Gwen/Skins/TexturedBase.h>
#include <Gwen/UnitTest/UnitTest.h>
#include <Gwen/Input/SDL2.h>
#include <Gwen/Renderers/SDL2.h>

#include "runtimeManager.h"

namespace EDen {
	class SDLGwenStatusWindow
	{
	private:
		SDL_Window *window;
		Gwen::Renderer::SDL2 *pRenderer;
		Gwen::Controls::Canvas* pCanvas;
		Gwen::Input::SDL2* GwenInput;

		RuntimeManager* runtime;

		Gwen::Controls::Button* testButton;
		Gwen::Controls::ListBox* listBox;
	public:
		SDLGwenStatusWindow(RuntimeManager* runtime);
		~SDLGwenStatusWindow();

		void processEvent(SDL_Event* evt);
		void update();
	};
};