#include "SDL_WindowGroundpart.h"
#include "math.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

#define SDL_SCALE 10
#define SCALE_FACTORX 0.981f
#define SCALE_FACTORY 0.98f
#define MOVE_AMOUNT 0.45f
#define MOVE_MAX_AMOUNT 500.0f
#define MOVE_SLOWDOWN_FACTOR 0.2f
#define DOWN_SCALE_FACTOR 1.03f
#define SCALE_DOWN_HYST_FACTOR 0.1f

namespace EDen {
	SDL_WindowGroundpart::~SDL_WindowGroundpart() {
		runtime->remove(sun);
		runtime->remove(this);
		if(statsWindow) delete statsWindow;
		if(statusWindow) delete statusWindow;
		if(shadows) {
			shadows->clear();
			delete shadows;
		}
		SDL_DestroyWindow(window);
	};

  SDL_WindowGroundpart::SDL_WindowGroundpart(std::string name, int width, int height, float maxWater, float maxGoo, int emptySpaces, RuntimeManager* runtimeManager) : SingleDimensionHeightmapGroundpart(name, width * 3, maxWater, maxGoo, emptySpaces) {
	window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);
	if (!window) {
    	return;
	}

	primaryMarkedOrganism = 0;
	statusWindow = 0;
	statsWindow = 0;
	lastStatusWindowPosition = 0;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	runtime = runtimeManager;
	runtime->add(this);

    sun = new SDL_SunlightProvider();
    runtime->add(sun);
	
	speciesDatabase = new SpeciesDatabase(runtime);

    dimx = width;
    dimy = height;

	shadows = new SDL_ShadowAccumulator(0,0,dimx,dimy,sun,renderer);
	drawLightDebug = false;

	//renderer skin and layout from here
	gwenRenderer = new Gwen::Renderer::SDL2(renderer);
	Gwen::Skin::TexturedBase* skin = new Gwen::Skin::TexturedBase(gwenRenderer);
	skin->SetRender(gwenRenderer);
	skin->Init("DefaultSkin.png");
	skin->SetDefaultFont("OpenSans.ttf", 10);

	pCanvas = new Gwen::Controls::Canvas(skin);
	pCanvas->SetSize(dimx, dimy);
	pCanvas->SetBounds(0,0,dimx,dimy);
	pCanvas->SetDrawBackground(false);
	//pCanvas->SetBackgroundColor(Gwen::Color(0, 0, 0, 255));

	GwenInput = new Gwen::Input::SDL2();
	GwenInput->Initialize(pCanvas);

	waterPlusButton = new Gwen::Controls::Button(pCanvas);
    waterPlusButton->SetText("W+");
	waterPlusEventHandler = new ResourceButtonEventHandler(this,"Wasser",true);
	waterPlusButton->onPress.Add(waterPlusEventHandler, &ResourceButtonEventHandler::onClick);
	waterMinusButton = new Gwen::Controls::Button(pCanvas);
    waterMinusButton->SetText("W-");
	waterMinusEventHandler = new ResourceButtonEventHandler(this,"Wasser",false);
	waterMinusButton->onPress.Add(waterMinusEventHandler, &ResourceButtonEventHandler::onClick);
	gooPlusButton = new Gwen::Controls::Button(pCanvas);
    gooPlusButton->SetText("G+");
	gooPlusEventHandler = new ResourceButtonEventHandler(this,"Goo",true);
	gooPlusButton->onPress.Add(gooPlusEventHandler, &ResourceButtonEventHandler::onClick);
	gooMinusButton = new Gwen::Controls::Button(pCanvas);
    gooMinusButton->SetText("G-");
	gooMinusEventHandler = new ResourceButtonEventHandler(this,"Goo",false);
	gooMinusButton->onPress.Add(gooMinusEventHandler, &ResourceButtonEventHandler::onClick);
	waterPercentageLabel = new Gwen::Controls::Label(pCanvas);
	waterPercentageLabel->SetText("0%");
	waterPercentageLabel->SetAlignment(3);
	waterPercentageLabel->SetTextColor(Gwen::Color(250,200,0,255));
	gooPercentageLabel = new Gwen::Controls::Label(pCanvas);
	gooPercentageLabel->SetText("0%");
	gooPercentageLabel->SetAlignment(3);
	gooPercentageLabel->SetTextColor(Gwen::Color(250,200,0,255));
	organismsNumberLabel = new Gwen::Controls::Label(pCanvas);
	organismsNumberLabel->SetText("0/0");
	organismsNumberLabel->SetAlignment(3);
	organismsNumberLabel->SetTextColor(Gwen::Color(250,200,0,255));
	organismsPlusButton = new Gwen::Controls::Button(pCanvas);
	organismsPlusButton->SetText("O+");
	organismsPlusButtonEventHandler = new OrganismCountButtonEventHandler(this,runtime,organismsNumberLabel,true);
	organismsPlusButton->onPress.Add(organismsPlusButtonEventHandler, &OrganismCountButtonEventHandler::onClick);
	organismsMinusButton = new Gwen::Controls::Button(pCanvas);
	organismsMinusButton->SetText("O-");
	organismsMinusButtonEventHandler = new OrganismCountButtonEventHandler(this,runtime,organismsNumberLabel,false);
	organismsMinusButton->onPress.Add(organismsMinusButtonEventHandler, &OrganismCountButtonEventHandler::onClick);	
	statusWindowButton = new Gwen::Controls::Button(pCanvas);
	statusWindowButton->SetText("S");
	statusWindowButtonEventHandler = new StatusWindowButtonEventHandler(this);
	statusWindowButton->onPress.Add(statusWindowButtonEventHandler, &StatusWindowButtonEventHandler::onClick);

	orgInsprector = new SDLGwenOrgnismInspector(pCanvas);

	waterPlusButton->SetBounds(115,0,15,15);
	waterPercentageLabel->SetBounds(16,1,98,15);
	waterMinusButton->SetBounds(0,0,15,15);
	gooPlusButton->SetBounds(115,15,15,15);
	gooPercentageLabel->SetBounds(16,16,98,15);
	gooMinusButton->SetBounds(0,15,15,15);
	organismsPlusButton->SetBounds(115,30,15,15);
	organismsNumberLabel->SetBounds(16,31,98,15);
	organismsMinusButton->SetBounds(0,30,15,15);
	statusWindowButton->SetBounds(130,30,15,15);
	orgInsprector->setBounds(dimx - 175,0,175,102);

    scale = SDL_SCALE;
	renderOffeset = dimx/2;
	moveMomentum = 0.0f;

  };

  bool SDL_WindowGroundpart::add(Organism* param_organism) {
    static bool alternate;
    
    if(alternate) organisms.push_front(param_organism);
    else organisms.push_back(param_organism);

    alternate = ! alternate;
    return true;
  };

  std::list<Organism*>* SDL_WindowGroundpart::getOrganisms() {
    return &organisms;
  };

  void SDL_WindowGroundpart::clearScaleToOrganisms() {
	  scaleToOrganisms.clear();
  };

  void SDL_WindowGroundpart::addScaleToOrganism(Organism* org) {
	  scaleToOrganisms.push_back(org);
  };

  bool SDL_WindowGroundpart::print(bool fast) {
	SDL_Rect statusBarRect;
	if(!fast) {
		gwenRenderer->BeginContext(NULL);

		//update childobjects data
		if(orgInsprector->getOrganism() != primaryMarkedOrganism) {
			orgInsprector->setOrganism(primaryMarkedOrganism);
		};

		orgInsprector->update();
		if(statsWindow) statsWindow->update();
		if(statusWindow) statusWindow->update();
	
		//draw Background 
		statusBarRect.x = 0;
		statusBarRect.y = 0;
		statusBarRect.w = dimx;
		statusBarRect.h = dimy;
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		//SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer,&statusBarRect);
	}

	shadows->clear();

	//draw organisms
    clip.reset();
    Organism* org;

    for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
      org = *it;
      if( org->getState() != BSP_dead) {
		if(scaleToOrganisms.size() != 0) {
			req_print(org->getRootBodypart(), (scale * getOrganismX(org)) + (dimx / 2) - ((getWidth() /  2) * scale) + renderOffeset, scale * getHeightAt(getOrganismX(org)) - 1, 0.0f, 0.0f, 0.0f, isScaleToOrganism(org), org == primaryMarkedOrganism, fast);
		}
		else {
			req_print(org->getRootBodypart(), (scale * getOrganismX(org)) + (dimx / 2) - ((getWidth() /  2) * scale) + renderOffeset, scale * getHeightAt(getOrganismX(org)) - 1, 0.0f, 0.0f, 0.0f, true , org == primaryMarkedOrganism, fast);
		}
      };
    };
    
	shadows->distribute();

	if(!fast) {
		printHeigtmap();
		if(drawLightDebug) {
			shadows->draw();
		}

		//draw status bar
		statusBarRect.x = 15;
		statusBarRect.h = 15;
		statusBarRect.y = 0;
		statusBarRect.w = 100 * chemStorage->getCurrentPercentage("Wasser") / 100.0f;
		SDL_SetRenderDrawColor(renderer, 0, 0, 140, 255);
		SDL_RenderFillRect(renderer,&statusBarRect);
		statusBarRect.y = 15;
		statusBarRect.w = 100 * chemStorage->getCurrentPercentage("Goo") / 100.0f;
		SDL_SetRenderDrawColor(renderer, 140, 30, 0, 255);
		SDL_RenderFillRect(renderer,&statusBarRect);

		if(organismsNumberLabel) {
		  int new_numOrganisms, new_numEmptySpaces,new_cps;
		  getNumOrganismsAndEmptySpaces(&new_numOrganisms,&new_numEmptySpaces);
		  new_cps = runtime->getCps();

		  if((new_numOrganisms != numOrganisms) || (new_numEmptySpaces != numEmptySpaces) || (new_cps != cps)) {
			numOrganisms = new_numOrganisms;
			numEmptySpaces = new_numEmptySpaces;
			cps = new_cps;

			organismsNumberLabel->SetText(Gwen::Utility::ToString(numOrganisms).append("/").append(Gwen::Utility::ToString(numOrganisms + numEmptySpaces)).append("@").append(Gwen::Utility::ToString(cps)));
		  }
		}

		if(waterPercentageLabel) {
		  float newWaterPercentage = ceilf(chemStorage->getCurrentPercentage("Wasser") * 100) / 100;
		  if(waterPercentage != newWaterPercentage) {
			  waterPercentageLabel->SetText(Gwen::Utility::ToString(newWaterPercentage).append("%"));
			  waterPercentage = newWaterPercentage;
		  }
		}

		if(gooPercentageLabel) {
		  float newGooPercentage = ceilf(chemStorage->getCurrentPercentage("Goo") * 100) / 100;
		  if(gooPercentage != newGooPercentage) {
			  gooPercentageLabel->SetText(Gwen::Utility::ToString(newGooPercentage).append("%"));
			  gooPercentage = newGooPercentage;
		  }
		}

		//render canvas and flip to window
		pCanvas->RenderCanvas();
		gwenRenderer->PresentContext(NULL);
		gwenRenderer->EndContext(NULL);
	}

	adjustViewport();

    return true;
  };

  void SDL_WindowGroundpart::adjustViewport() {
	clip.scaleDown = clip.scaleDown && (clip.scaleDownLeft || clip.scaleDownRight);

	float newScale = scale;

	if(clip.needToScaleY) {
		newScale = scale * SCALE_FACTORY;
		moveMomentum = moveMomentum * MOVE_SLOWDOWN_FACTOR;
	}
	else if(clip.moveLeft && clip.moveRight) {
		newScale = scale * SCALE_FACTORX;
		//moveMomentum = moveMomentum * MOVE_SLOWDOWN_FACTOR;
	}
	else if(clip.moveLeft) {
		if(moveMomentum < 0) {
			moveMomentum = 0.0f;
			newScale = scale * SCALE_FACTORX;
		}
		else {
			moveMomentum += MOVE_AMOUNT * scale * 0.9f;
		}

		if(moveMomentum > (MOVE_MAX_AMOUNT * scale)) {
			moveMomentum = (MOVE_MAX_AMOUNT * scale);
		}
		if(clip.scaleDown) {
			newScale = scale * DOWN_SCALE_FACTOR;
		}
	}
	else if(clip.moveRight) {
		if(moveMomentum > 0) {
			moveMomentum = 0.0f;
			newScale = scale * SCALE_FACTORX;
		}
		else {
			moveMomentum -= MOVE_AMOUNT * scale;
		}

		if(moveMomentum < -(MOVE_MAX_AMOUNT * scale)) {
			moveMomentum = -(MOVE_MAX_AMOUNT * scale);
		}
		if(clip.scaleDown) {
			newScale = scale * DOWN_SCALE_FACTOR;
		}
	}
    else {
		if(clip.scaleDown) {
			newScale = scale * DOWN_SCALE_FACTOR;
		}
		moveMomentum = moveMomentum * MOVE_SLOWDOWN_FACTOR;
	}

	if(scale != newScale) {
		renderOffeset = renderOffeset/scale*newScale;
		scale = newScale;
	}
	
	renderOffeset += moveMomentum;
  };

  void SDL_WindowGroundpart::processEvent(SDL_Event* evt) {
	if(statsWindow) statsWindow->processEvent(evt);
	if(statusWindow) statusWindow->processEvent(evt);

	if(evt->window.windowID == SDL_GetWindowID(window) || ((statsWindow) && statsWindow->getSDLWindowID() == evt->window.windowID) || ((statusWindow) && statusWindow->getSDLWindowID() == evt->window.windowID)) {
		if(evt->type == SDL_KEYDOWN) {
			const char *key = SDL_GetKeyName(evt->key.keysym.sym);
			if ( key[0] == 'S'  )  {
				if(key[1] != 'p') {
					toggleStatusWindow();
				};
			}
			else if ( key[0] == 'V'  ) {
				if(evt->key.keysym.mod & KMOD_CTRL) {
					if(statsWindow) {
						SDLGwenStatisticsWindow* tmpPointer = statsWindow;
						statsWindow = 0;
						delete tmpPointer;
					}
					else {
						statsWindow = new SDLGwenStatisticsWindow(runtime);
						statsWindow->show();
					}
				}
				else if(statsWindow) {
					if(statsWindow->isShown()) {
						statsWindow->hide();
					}
					else {
						statsWindow->show();
					};
				};
			};
		};
	};

	if(evt->window.windowID == SDL_GetWindowID(window)) {
		bool eventDigested = GwenInput->ProcessEvent(evt);
		if ((evt->type == SDL_WINDOWEVENT)) {
			if(evt->window.event == SDL_WINDOWEVENT_RESIZED) {
				resizeWindow(evt->window.data1, evt->window.data2);
			}
		}
		else if(!eventDigested && evt->type == SDL_MOUSEBUTTONUP) {
			if(evt->button.button == 1) {
				int x = evt->button.x - renderOffeset;
				int clickedX = (width / 2) + ((x - (dimx / 2)) * (1.0f/scale));

				primaryMarkedOrganism = getOrganismNearX(clickedX);
			}
			else if(evt->button.button == 3) {
				//int x = evt->button.x - renderOffeset;
				//int clickedX = (width / 2) + ((x - (dimx / 2)) * (1.0f/scale));

				primaryMarkedOrganism = 0;
			}
		};
	};

	if ((evt->type == SDL_WINDOWEVENT)) {
		if(evt->window.event == SDL_WINDOWEVENT_CLOSE) {
			if((statusWindow) && statusWindow->getSDLWindowID() == evt->window.windowID) {
				hideStatusWindow();
			};

			if((statsWindow) && statsWindow->getSDLWindowID() == evt->window.windowID) {
				if(statsWindow->isShown()) {
					statsWindow->hide();
				};
			};
		};
	};
  };


  void SDL_WindowGroundpart::resizeWindow(int pDimx, int pDimy) {
	  dimx = pDimx;
	  dimy = pDimy;

	  pCanvas->SetSize(dimx,dimy);
	  orgInsprector->setBounds(dimx - 175,0,orgInsprector->getWidth(),orgInsprector->getHeight());
	  shadows->setSize(dimx,dimy);
  };

  int SDL_WindowGroundpart::req_print(Bodypart *param_bp, int param_x, int param_y, float p_ang1, float p_ang2, float p_ang3, bool relevantForScaling, bool marked, bool fast) {
	int returnvalue = 0;
    
    if(param_bp) {
      returnvalue = 1;
	  int x2,y2;
      
      float a1,a2,a3;
      a1 = p_ang1;
      a2 = p_ang2;
      a3 = p_ang3;
      
	  {
          int x1,y1;  
		  x1 = param_x;
		  y1 = param_y;
		  {
		  float radi1,radi2,radi3,deltax,deltay;
			radi1 = 2.0f * M_PI * (p_ang1/360.0f);
			radi2 = 2.0f * M_PI * (p_ang2/360.0f);
			radi3 = 2.0f * M_PI * (p_ang3/360.0f);

			//an absolut rotation?
			deltax = sin(radi1);//*(cos(rad3));
			deltay = cos(radi1);//*cos(rad2);

			x2 = (int)(param_x + (deltax * param_bp->getSize() * scale));
			y2 = (int)(param_y + (deltay * param_bp->getSize() * scale));
		  }

		  if(shadows) {
			  shadows->add(param_bp,x1,y1,x2,y2);
		  }

		  //std::cout << "(" << x1 << "\t" << y1 << ")\t(" << x2 << "\t" << y2 << ")\n";

		  if(relevantForScaling) {
			  if((x1 <= 0) || (x2 <= 0)) {
				  clip.moveLeft = true;
			  }
			  if((x1 > dimx) || (x2 > dimx)) {
				  clip.moveRight = true;
			  }
			  if((y1 > dimy) || (y2 > dimy)) {
				  clip.needToScaleY = true;
			  };
			  if((x1 <= (dimx * SCALE_DOWN_HYST_FACTOR)) || (x2 <= (dimx * SCALE_DOWN_HYST_FACTOR))) {
				  clip.scaleDownLeft = false;
			  }
			  if((x1 > (dimx - (dimx * SCALE_DOWN_HYST_FACTOR))) || (x2 > (dimx - (dimx * SCALE_DOWN_HYST_FACTOR)))) {
				  clip.scaleDownRight = false;
			  }
			  if((y1 > (dimy - (dimy * SCALE_DOWN_HYST_FACTOR))) || (y2 > (dimy - (dimy * SCALE_DOWN_HYST_FACTOR)))) {
				  clip.scaleDown = false;
			  }
		  }

		  if(!fast) {
			if(renderer == 0) {
				std::cout << "renderer is null\n";
			  }

			  if(marked) {
				SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
			  }
			  else {
				  if(relevantForScaling) {
					  SDL_SetRenderDrawColor(renderer, param_bp->color.r * 255, param_bp->color.g * 255, param_bp->color.b * 255, param_bp->color.a * 255);
				  }
				  else {
					  SDL_SetRenderDrawColor(renderer, param_bp->color.r * 255, param_bp->color.g * 255, param_bp->color.b * 255, param_bp->color.a * 40 + 10);
				  }
			  }
			  SDL_RenderDrawLine(renderer,x1,dimy-(y1+1),x2,dimy-(y2+1));
		  }
	  }

      SpawnpointInformationList* bpSpawnpoints = param_bp->getSpawnpoints();
      for(SpawnpointInformationListIterator it = bpSpawnpoints->begin(); it != bpSpawnpoints->end(); it++) {
        // position 0 is reserved for the backwards spawnpoint
        SpawnpointInformation* sp;
        if(((*it)->occupied) && ((*it)->position != 0)) { 
          sp = (*it)->connectedBodypart->getSpawnpointInformationForBodypart(param_bp);
          returnvalue += req_print((*it)->connectedBodypart,x2,y2,a1 + 180.0f + sp->ang2d + (*it)->ang2d,a2 + sp->ang2 + (*it)->ang2,a3 + sp->rot + (*it)->rot, relevantForScaling, marked, fast);
        };
      };
      //std::cout << returnvalue << std::endl;
      return returnvalue;
    }
    else return 0;
  };

  void SDL_WindowGroundpart::ResourceButtonEventHandler::onClick(Gwen::Controls::Base* pControl) {
    ChemicalStorage* storage = gp->getChemicalStorage();
    if(increment) {
      storage->add(resourceName,storage->getMaxAmount(resourceName) * 0.02f);
    }
    else {
      storage->add(resourceName,-storage->getMaxAmount(resourceName) * 0.02f);
    }
  };

  void SDL_WindowGroundpart::OrganismCountButtonEventHandler::onClick(Gwen::Controls::Base* pControl) {
    if(increment) {
	  if(gp->incEmptySpaces()) if(runtime) runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() + 1);
    }
    else {
      if(gp->decEmptySpaces()) if(runtime) runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() - 1);
    }

	if(label) {
	  int numOrganisms, numEmptySpaces,cps;
      gp->getNumOrganismsAndEmptySpaces(&numOrganisms,&numEmptySpaces);
	  if(runtime) {
		  cps = runtime->getCps();
	  }
	  else {
		  cps = 0;
	  }

	  label->SetText(Gwen::Utility::ToString(numOrganisms).append("/").append(Gwen::Utility::ToString(numOrganisms + numEmptySpaces)).append("@").append(Gwen::Utility::ToString(cps)));
	}
  };

  void SDL_WindowGroundpart::StatusWindowButtonEventHandler::onClick(Gwen::Controls::Base* pControl) {
	  gp->toggleStatusWindow();
  };

  SDL_WindowGroundpart::ViewportClippingInformation::ViewportClippingInformation() 
  {
	  reset();
  };

  void SDL_WindowGroundpart::ViewportClippingInformation::reset() {
	moveLeft = false;
	moveRight = false;
	needToScaleY = false;
	scaleDownLeft = true;
	scaleDownRight = true;
	scaleDown = true;
  };

  void SDL_WindowGroundpart::printHeigtmap() {
	  int height;
	  int halfGroundpartWidth = getWidth() /  2;
	  int halfDimX = dimx / 2;
	  SDL_SetRenderDrawColor(renderer, 100, chemStorage->getCurrentPercentage("Wasser")/100.0f * 100 + 100, chemStorage->getCurrentPercentage("Goo")/100.0f * 100, 255);
	  int ix;
	  for(int i = 0; i < dimx; i++) {
		ix = i - renderOffeset;
		height = getHeightAt(halfGroundpartWidth + ((ix - halfDimX) * (1.0f/scale))) * scale;
		SDL_RenderDrawLine(renderer, i,dimy - (1 + height), i,dimy - 1);
	  };
  };

  int SDL_WindowGroundpart::getDimX() {
    return dimx;  
  };

  int SDL_WindowGroundpart::getDimY() {
    return dimy; 
  };

  void SDL_WindowGroundpart::setDrawLightDebug(bool param_lightDebug) {
	  drawLightDebug = param_lightDebug;
  };

  bool SDL_WindowGroundpart::getDrawLightDebug() {
	  return drawLightDebug;
  };

  bool SDL_WindowGroundpart::orgsAlive() {
    if(organisms.size() > 0) return true;
    return false;
  };

  unsigned int SDL_WindowGroundpart::getNumSpecies() {
	  std::map<unsigned int, std::string> speciesMap;
	  speciesDatabase->getDebugOut(&speciesMap);

	  std::map<unsigned int, unsigned int> countMap;
	   unsigned int speciesID;
	  for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
		  speciesID = (*it)->getRootBodypart()->getGeneticCode()->getSpeciesIdentifier();
		  if(speciesMap.count(speciesID) == 0) {
			  if(countMap.count(speciesID) == 0) {
				  countMap[speciesID] = 0;
			  }
//			  countMap[speciesID] += 1;
		  }
	  }

	  return speciesMap.size() + countMap.size();
  };

  void SDL_WindowGroundpart::getDebugOut(std::map<unsigned int, std::string>* outList) {
	  speciesDatabase->getDebugOut(outList);

	  std::map<unsigned int, unsigned int> countMap;
	  unsigned int speciesID;
	  for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
		  speciesID = (*it)->getRootBodypart()->getGeneticCode()->getSpeciesIdentifier();
		  if(outList->count(speciesID) == 0) {
			  if(countMap.count(speciesID) == 0) {
				  countMap[speciesID] = 0;
			  }
			  //countMap[speciesID] += 1;
		  }
	  }

	  for(std::map<unsigned int, unsigned int>::iterator it = countMap.begin(); it != countMap.end(); it++) {
		  outList->insert(std::pair<unsigned int,std::string>((it->first),std::string("species").append(Gwen::Utility::ToString(it->first)).append("[0|0|0]")));
	  }
  };

  RuntimeManager* SDL_WindowGroundpart::getRuntimeManager() {
	  return runtime;
  };

  bool SDL_WindowGroundpart::registerOrganism(Organism* param_organism) {
	  int orgPosX = getOrganismX(param_organism);
	  int posX = orgPosX;
	  int windowPosX = (posX - (width / 2)) * scale + (dimx / 2) + renderOffeset;
	  bool emptySlotFound = false;
	  int i = 0;
	  int newPosX;

	  while(!emptySlotFound) {
		posX = orgPosX + (i * 3);
		windowPosX = (posX - (width / 2)) * scale + (dimx / 2) + renderOffeset;

		if((!isOccupiedByAlivePlant(posX)) && (shadows->getShadowStateAt(windowPosX, 0) >= SDL_ShadowAccumulator::ShadowState::LIT)) {
			newPosX = posX;
			emptySlotFound = true;
		}

		posX = orgPosX - (i * 3);
		windowPosX = (posX - (width / 2)) * scale + (dimx / 2) + renderOffeset;

		if((!isOccupiedByAlivePlant(posX)) && (shadows->getShadowStateAt(windowPosX, 0) >= SDL_ShadowAccumulator::ShadowState::LIT)) {
			newPosX = posX;
			emptySlotFound = true;
		}

		i++;
	  }

	  if(orgPosX != newPosX) {
		  boost::mutex::scoped_lock lock(mutex);
		  plantPositionMemory[param_organism] = newPosX;
	  }

	  bool retval = SingleDimensionHeightmapGroundpart::registerOrganism(param_organism);
	  return retval;
  };

  bool SDL_WindowGroundpart::unregisterOrganism(Organism* param_organism) {
	  if(primaryMarkedOrganism == param_organism) {
		  primaryMarkedOrganism = 0;
	  };

	  return SingleDimensionHeightmapGroundpart::unregisterOrganism(param_organism);
  }

  bool SDL_WindowGroundpart::isScaleToOrganism(Organism* org) {
	  for(std::list<Organism*>::iterator it = scaleToOrganisms.begin(); it != scaleToOrganisms.end(); it++) {
		  if((*it) == org) {
			  return true;
		  }
	  }

	  return false;
  }

  void SDL_WindowGroundpart::toggleStatusWindow() {
	  if(!statusWindow) {
		  if(lastStatusWindowPosition == 0) {
			statusWindow = new SDLGwenStatusWindow(this);
		  }
		  else {
			statusWindow = new SDLGwenStatusWindow(this,*lastStatusWindowPosition);
		  }
	  }
	  else {
		  hideStatusWindow();
	  };
  }

  void SDL_WindowGroundpart::hideStatusWindow() {
	SDLGwenStatusWindow* tmpPointer = statusWindow;
	statusWindow = 0;
	if(lastStatusWindowPosition == 0) {
		lastStatusWindowPosition = new Gwen::Rect();
	}
	tmpPointer->getSize(&lastStatusWindowPosition->w,&lastStatusWindowPosition->h);
	tmpPointer->getPosition(&lastStatusWindowPosition->x,&lastStatusWindowPosition->y);

	delete tmpPointer;
	clearScaleToOrganisms();
  }
}