#include "SDLGwenStatusWindow.h"

namespace EDen {
	SDLGwenStatusWindow::SDLGwenStatusWindow(SDL_WindowGroundpart* p_gp, Gwen::Rect pos)
	{
		gp = p_gp;

		window = SDL_CreateWindow(gp->getName().append(" Species").c_str(), pos.x, pos.y, pos.w, pos.h, SDL_WINDOW_RESIZABLE);

		if (!window) {
			return;
		}

		pRenderer = new Gwen::Renderer::SDL2(window);

		Gwen::Skin::TexturedBase* skin = new Gwen::Skin::TexturedBase(pRenderer);
		skin->SetRender(pRenderer);
		skin->Init("DefaultSkin.png");
		skin->SetDefaultFont("OpenSans.ttf", 11);

		pCanvas = new Gwen::Controls::Canvas(skin);
		pCanvas->SetSize(pos.w, pos.h);
		pCanvas->SetDrawBackground(true);
		pCanvas->SetBackgroundColor(Gwen::Color(200, 100, 20, 255));

		GwenInput = new Gwen::Input::SDL2();
		GwenInput->Initialize(pCanvas);

		hoverHandler = new ListItemHoverEventHandler(gp);
		listBox = new Gwen::Controls::ListBox(pCanvas);
		listBox->SetBounds(3, 3, pos.w - 6, pos.h - 6);
		listBox->SetMouseInputEnabled(false);
		listBox->SetKeyboardInputEnabled(false);
		update();
	}

	SDLGwenStatusWindow::~SDLGwenStatusWindow()
	{
		SDL_DestroyWindow(window);
		if(hoverHandler->currentSpeciesID != 0) {
			hoverHandler->currentSpeciesID = 0;
			gp->clearScaleToOrganisms();
		}
		delete hoverHandler;
	}

	int SDLGwenStatusWindow::getSDLWindowID() {
		return SDL_GetWindowID(window);
	};

	void SDLGwenStatusWindow::processEvent(SDL_Event* evt) {
		if(evt->window.windowID == SDL_GetWindowID(window)) {
			GwenInput->ProcessEvent(evt);
			if(evt->type == SDL_MOUSEBUTTONUP) {
				if(evt->button.button == 3) {
					listBox->UnselectAll();
					gp->clearScaleToOrganisms();
				};
			}
			
			if(evt->window.event == SDL_WINDOWEVENT_RESIZED) {
				resizeWindow(evt->window.data1, evt->window.data2);
			}
			else if(evt->window.event == SDL_WINDOWEVENT_LEAVE) {
				if(hoverHandler->currentSpeciesID != 0) {
					hoverHandler->currentSpeciesID = 0;
					gp->clearScaleToOrganisms();
				}
			}
		}
	}

	void SDLGwenStatusWindow::update() {
		std::map<unsigned int,std::string> outputs;
		gp->getDebugOut(&outputs);

		Gwen::Controls::Layout::TableRow* item;
		unsigned int speciesID;
		bool foundOne = false;

		for(int i = listBox->GetTable()->RowCount() - 1; i >= 0 ; i--) {
			item = listBox->GetTable()->GetRow(i);
			speciesID = item->UserData.Get<unsigned int>("speciesID");
			foundOne = false;
			for(std::map<unsigned int,std::string>::iterator itOutputs = outputs.begin(); itOutputs != outputs.end(); itOutputs++) {
				if(speciesID == itOutputs->first) {
					item->SetCellText(0,itOutputs->second.append("(").append(Gwen::Utility::ToString(gp->getAliveOrganismsOfSpecies(itOutputs->first)).append(")")));
					foundOne = true;
					continue;
				};
			};
			if(!foundOne) {
				if(listBox->GetSelectedRow() == item) {
					listBox->UnselectAll();
					gp->clearScaleToOrganisms();
				}
				listBox->GetTable()->Remove(item);
			};
		};

		for(std::map<unsigned int,std::string>::iterator itOutputs = outputs.begin(); itOutputs != outputs.end(); itOutputs++) {
			foundOne = false;
			for(int i = listBox->GetTable()->RowCount() - 1; i >= 0 ; i--) {
				item = listBox->GetTable()->GetRow(i);
				speciesID = item->UserData.Get<unsigned int>("speciesID");
				if(speciesID == itOutputs->first) {
					foundOne = true;
					continue;
				};
			};

			if(!foundOne) {
				item = listBox->AddItem(itOutputs->second.append("(").append(Gwen::Utility::ToString(gp->getAliveOrganismsOfSpecies(itOutputs->first)).append(")")));
				item->UserData.Set<unsigned int>("speciesID",itOutputs->first);
				item->onHoverEnter.Add(hoverHandler,&ListItemHoverEventHandler::onHoverIn);
				item->onHoverLeave.Add(hoverHandler,&ListItemHoverEventHandler::onHoverOut);
			}
		}
		

		pRenderer->BeginContext(NULL);
		pCanvas->RenderCanvas();
		pRenderer->PresentContext(NULL);
		pRenderer->EndContext(NULL);

		if(hoverHandler->currentSpeciesID != 0 || listBox->GetSelectedRow() != 0) {
			int currentSpeciesID = hoverHandler->currentSpeciesID;
			if(currentSpeciesID == 0) {
				currentSpeciesID = listBox->GetSelectedRow()->UserData.Get<unsigned int>("speciesID");
			}
			gp->clearScaleToOrganisms();
			for(std::list<Organism*>::iterator it = gp->getOrganisms()->begin(); it != gp->getOrganisms()->end(); it++) {
				if((*it)->getRootBodypart()->getGeneticCode()->getSpeciesIdentifier() == currentSpeciesID) {
					gp->addScaleToOrganism(*it);
				}
			};
		};
	}
	
	void SDLGwenStatusWindow::resizeWindow(int dimx, int dimy) {
		pCanvas->SetSize(dimx, dimy);
		listBox->SetSize(dimx - 6, dimy - 6);
	};

	void SDLGwenStatusWindow::getSize(int* w, int* h) {
		SDL_GetWindowSize(window,w,h);
	};

	void SDLGwenStatusWindow::getPosition(int* x, int* y) {
		SDL_GetWindowPosition(window,x,y);
	};

	void SDLGwenStatusWindow::ListItemHoverEventHandler::onHoverIn(Gwen::Controls::Base* pControl) {
		if(pControl->IsHovered()) {
			if(currentSpeciesID != pControl->UserData.Get<unsigned int>("speciesID")) {
				currentSpeciesID = pControl->UserData.Get<unsigned int>("speciesID");
				gp->clearScaleToOrganisms();
				for(std::list<Organism*>::iterator it = gp->getOrganisms()->begin(); it != gp->getOrganisms()->end(); it++) {
					if((*it)->getRootBodypart()->getGeneticCode()->getSpeciesIdentifier() == currentSpeciesID) {
						gp->addScaleToOrganism(*it);
					}
				};
			}
		}
	};

	void SDLGwenStatusWindow::ListItemHoverEventHandler::onHoverOut(Gwen::Controls::Base* pControl) {
		if(pControl->UserData.Get<unsigned int>("speciesID") == currentSpeciesID) {
			gp->clearScaleToOrganisms();
			currentSpeciesID = 0;
		}
	};
}