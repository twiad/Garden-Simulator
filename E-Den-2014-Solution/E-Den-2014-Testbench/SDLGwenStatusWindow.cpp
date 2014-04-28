#include "SDLGwenStatusWindow.h"

#define STATUS_WINDOW_DIM_X 200
#define STATUS_WINDOW_DIM_Y 400

namespace EDen {
	SDLGwenStatusWindow::SDLGwenStatusWindow(SDL_WindowGroundpart* p_gp)
	{
		gp = p_gp;

		window = SDL_CreateWindow("Species", 10, 10, STATUS_WINDOW_DIM_X, STATUS_WINDOW_DIM_Y, SDL_WINDOW_SHOWN);

		if (!window) {
			return;
		}

		pRenderer = new Gwen::Renderer::SDL2(window);

		Gwen::Skin::TexturedBase* skin = new Gwen::Skin::TexturedBase(pRenderer);
		skin->SetRender(pRenderer);
		skin->Init("DefaultSkin.png");
		skin->SetDefaultFont("OpenSans.ttf", 11);

		pCanvas = new Gwen::Controls::Canvas(skin);
		pCanvas->SetSize(STATUS_WINDOW_DIM_X, STATUS_WINDOW_DIM_Y);
		pCanvas->SetDrawBackground(true);
		pCanvas->SetBackgroundColor(Gwen::Color(200, 100, 20, 255));

		GwenInput = new Gwen::Input::SDL2();
		GwenInput->Initialize(pCanvas);

		hoverHandler = new ListItemHoverEventHandler(gp);
		listBox = new Gwen::Controls::ListBox(pCanvas);
		listBox->SetBounds(3, 3, STATUS_WINDOW_DIM_X - 6, STATUS_WINDOW_DIM_Y - 6);
		listBox->AddItem("No Items Yet");

		update();
	}

	SDLGwenStatusWindow::~SDLGwenStatusWindow()
	{
		SDL_DestroyWindow(window);
		delete hoverHandler;
	}

	void SDLGwenStatusWindow::processEvent(SDL_Event* evt) {
		GwenInput->ProcessEvent(evt);
		if ((evt->type == SDL_WINDOWEVENT)) {
			if(evt->window.windowID == SDL_GetWindowID(window)) {
				if(evt->window.event == SDL_WINDOWEVENT_LEAVE) {
					gp->clearScaleToOrganisms();
					hoverHandler->currentSpeciesID = 0;
				}
			}
		}
	}

	void SDLGwenStatusWindow::update() {
		listBox->Clear();

		//listBox->AddItem(runtime->getDebugOut(true));

		std::list<Groundpart*> groundparts = gp->getRuntimeManager()->getGroundparts();
		for(std::list<Groundpart* >::iterator it = groundparts.begin(); it != groundparts.end(); it++) {
			//listBox->AddItem((*it)->getName());

			std::map<int,std::string> outputs;
			(*it)->getSpeciesDatabase()->getDebugOut(&outputs);

			Gwen::Controls::Layout::TableRow* item;

			for(std::map<int,std::string>::iterator itOutputs = outputs.begin(); itOutputs != outputs.end(); itOutputs++) {
				item = listBox->AddItem(itOutputs->second);
				item->UserData.Set("speciesID",itOutputs->first);
				item->onHoverEnter.Add(hoverHandler,&ListItemHoverEventHandler::onHoverIn);
				//item->onHoverLeave.Add(hoverHandler,&ListItemHoverEventHandler::onHoverOut);
			}
		};

		pRenderer->BeginContext(NULL);
		pCanvas->RenderCanvas();
		pRenderer->PresentContext(NULL);
		pRenderer->EndContext(NULL);

		if(hoverHandler->currentSpeciesID != 0) {
			int currentSpeciesID = hoverHandler->currentSpeciesID;
			gp->clearScaleToOrganisms();
			for(std::list<Organism*>::iterator it = gp->getOrganisms()->begin(); it != gp->getOrganisms()->end(); it++) {
				if((*it)->getRootBodypart()->getGeneticCode()->getSpeciesIdentifier() == currentSpeciesID) {
					gp->addScaleToOrganism(*it);
				}
			};
		};
	}

	void SDLGwenStatusWindow::ListItemHoverEventHandler::onHoverIn(Gwen::Controls::Base* pControl) {
		if(currentSpeciesID != pControl->UserData.Get<int>("speciesID")) {
			currentSpeciesID = pControl->UserData.Get<int>("speciesID");
			gp->clearScaleToOrganisms();
			for(std::list<Organism*>::iterator it = gp->getOrganisms()->begin(); it != gp->getOrganisms()->end(); it++) {
				if((*it)->getRootBodypart()->getGeneticCode()->getSpeciesIdentifier() == currentSpeciesID) {
					gp->addScaleToOrganism(*it);
				}
			};
		}
	};

	//void SDLGwenStatusWindow::ListItemHoverEventHandler::onHoverOut(Gwen::Controls::Base* pControl) {
	//	if(pControl->UserData.Get<int>("speciesID") == currentSpeciesID) {
	//		gp->clearScaleToOrganisms();
	//		currentSpeciesID = 0;
	//	}
	//};
}