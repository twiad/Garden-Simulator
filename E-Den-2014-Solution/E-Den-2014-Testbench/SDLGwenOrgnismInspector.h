#pragma once

#include "organism.h"
#include <Gwen/Gwen.h>
#include <Gwen/Skins/Simple.h>
#include <Gwen/Skins/TexturedBase.h>
#include <Gwen/UnitTest/UnitTest.h>
#include <Gwen/Input/SDL2.h>
#include <Gwen/Renderers/SDL2.h>

#include "runtimeManager.h"

namespace EDen {
	class SDLGwenOrgnismInspector
	{
	private:
		Gwen::Controls::Canvas* parentCanvas;
		Gwen::Input::SDL2* GwenInput;

		Gwen::Controls::ListBox* listBox;

		int posX, posY;
		int dimX, dimY;

		void show(Organism* org);
		void hide();
	protected:
		Organism* org;
	public:
		SDLGwenOrgnismInspector(Gwen::Controls::Canvas* parentCanvas);
		~SDLGwenOrgnismInspector();

		bool isShown();

		void processEvent(SDL_Event* evt);
		void update();
		void setOrganism(Organism* org);
		Organism* getOrganism();

		void setBounds(int posX, int posY, int dimX, int dimY);
		int getPosX();
		int getPosY();
		int getWidth();
		int getHeight();
	};
}

