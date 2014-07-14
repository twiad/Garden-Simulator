#include "SDLGwenStatusWindow.h"

namespace EDen {
	SDLGwenStatusWindow;
	SDLGwenStatusWindow::SDLGwenStatusWindow(SDL_WindowGroundpart* p_gp, int p_width)
	{
		gp = p_gp;
		width = p_width;
		listBox = 0;

		hoverHandler = new ListItemHoverEventHandler(gp);
	}

	SDLGwenStatusWindow::~SDLGwenStatusWindow()
	{
		if(hoverHandler->currentSpeciesID != 0) {
			hoverHandler->currentSpeciesID = 0;
			gp->clearScaleToOrganisms();
		};

		delete hoverHandler;
	}

	void SDLGwenStatusWindow::processEvent(SDL_Event* evt) {
		if(isShown()) {
			if(evt->type == SDL_MOUSEBUTTONUP) {
				if(evt->button.button == 3 && evt->button.x > gp->getDimX() - width && evt->button.y < listBox->GetBounds().h) {
					listBox->UnselectAll();
					gp->clearScaleToOrganisms();
				};
			}
			
			if(evt->window.event == SDL_WINDOWEVENT_LEAVE) {
				if(hoverHandler->currentSpeciesID != 0) {
					hoverHandler->currentSpeciesID = 0;
					gp->clearScaleToOrganisms();
				}
			}
		}
	}

	void SDLGwenStatusWindow::update() {
		if(isShown()) {
			std::map<unsigned int,std::string> outputs;
			gp->getDebugOut(&outputs);

			Gwen::Controls::Layout::TableRow* item;
			unsigned int speciesID;
			unsigned int numItems = 0;
			bool foundOne = false;

			for(int i = listBox->GetTable()->RowCount() - 1; i >= 0 ; i--) {
				item = listBox->GetTable()->GetRow(i);
				speciesID = item->UserData.Get<unsigned int>("speciesID");
				foundOne = false;
				for(std::map<unsigned int,std::string>::iterator itOutputs = outputs.begin(); itOutputs != outputs.end(); itOutputs++) {
					if(speciesID == itOutputs->first) {
						item->SetCellText(0,itOutputs->second.append("(").append(Gwen::Utility::ToString(gp->getAliveOrganismsOfSpecies(itOutputs->first)).append(")")));
						foundOne = true;
						numItems++;
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
					numItems++;
				}
			}

			listBox->SetHeight(mini((int)(numItems)*20 + 4,gp->getDimY()));
		
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
	}
	
	bool SDLGwenStatusWindow::isShown() {
		return listBox != 0;
	};

	void SDLGwenStatusWindow::show(bool beShown) {
		if(isShown() && !beShown) {
			Gwen::Controls::ListBox* oldListBox = listBox;
			listBox = 0;
			delete oldListBox;
			
			if(hoverHandler->currentSpeciesID != 0) {
				hoverHandler->currentSpeciesID = 0;
				gp->clearScaleToOrganisms();
			};
		}
		else if(!isShown() && beShown) {
			listBox = new Gwen::Controls::ListBox(gp->getCanvas());
			listBox->SetBounds(gp->getDimX() - width, 0, width, gp->getDimY());
			listBox->SetMouseInputEnabled(false);
			listBox->SetKeyboardInputEnabled(false);
			update();
		}
	};

	void SDLGwenStatusWindow::resizeWindow(int dimx, int dimy) {
		if(isShown()) {
			listBox->SetBounds(dimx - width, 0, width, dimy);
		}
	};

	void SDLGwenStatusWindow::getSize(int* w, int* h) {
		if(isShown()) {
			Gwen::Rect tmpRect = listBox->GetBounds();
			*w = tmpRect.w;
			*h = tmpRect.h;
		}
		else {
			*w = 0;
			*h = 0;
		}
	};

	void SDLGwenStatusWindow::getPosition(int* x, int* y) {
		if(isShown()) {
			Gwen::Rect tmpRect = listBox->GetBounds();
			*x = tmpRect.x;
			*y = tmpRect.y;
		}
		else {
			*x = 0;
			*y = 0;
		}
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