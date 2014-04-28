#pragma once

#include <Gwen/Gwen.h>
#include <Gwen/Skins/Simple.h>
#include <Gwen/Skins/TexturedBase.h>
#include <Gwen/UnitTest/UnitTest.h>
#include <Gwen/Input/SDL2.h>
#include <Gwen/Renderers/SDL2.h>

#include "runtimeManager.h"

namespace EDen {
	class SDLGwenStatisticsWindow
	{
	private:
		SDL_Window *window;
		Gwen::Renderer::SDL2 *pRenderer;
		Gwen::Controls::Canvas* pCanvas;
		Gwen::Input::SDL2* GwenInput;

		RuntimeManager* runtime;

		struct DataStorageEntry
		{
			bool visible;
			Gwen::Color col;
			int maxValues;
			std::list<double> data;
		};

		std::map<std::string,DataStorageEntry> data;
	protected:
		void addDataPoint(std::string identifier,double value);
		void resizeWindow(int dimx, int dimy);
	public:
		SDLGwenStatisticsWindow(RuntimeManager* p_runtime);
		~SDLGwenStatisticsWindow();

		void show();
		void hide();
		bool isShown();

		void processEvent(SDL_Event* evt);
		void update();
	};
}
