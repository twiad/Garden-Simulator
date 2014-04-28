#include "SDL_WindowGroundpart.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

#define SDL_SCALE 40
#define SCALE_FACTORX 0.981f
#define SCALE_FACTORY 0.98f
#define MOVE_AMOUNT 0.5f
#define MOVE_MAX_AMOUNT 500.0f
#define MOVE_SLOWDOWN_FACTOR 0.5f
#define DOWN_SCALE_FACTOR 1.03f
#define SCALE_DOWN_HYST 50

namespace EDen {
	SDL_WindowGroundpart::~SDL_WindowGroundpart() {
		runtime->remove(sun);
		runtime->remove(this);
		SDL_DestroyWindow(window);
	};

  SDL_WindowGroundpart::SDL_WindowGroundpart(std::string name, int width, int height, float maxWater, float maxGoo, int emptySpaces, RuntimeManager* runtimeManager) : SingleDimensionHeightmapGroundpart(name, width * 2, maxWater, maxGoo, emptySpaces) {
	window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);
	if (!window) {
    	return;
	}

	primaryMarkedOrganism = 0;

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

	gwenRenderer = new Gwen::Renderer::SDL2(renderer);
	Gwen::Skin::TexturedBase* skin = new Gwen::Skin::TexturedBase(gwenRenderer);
	skin->SetRender(gwenRenderer);
	skin->Init("DefaultSkin.png");
	skin->SetDefaultFont("OpenSans.ttf", 8);

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

	orgInsprector = new SDLGwenOrgnismInspector(pCanvas);

	waterPlusButton->SetBounds(115,0,15,10);
	waterMinusButton->SetBounds(0,0,15,10);
	gooPlusButton->SetBounds(115,10,15,10);
	gooMinusButton->SetBounds(0,10,15,10);
	organismsPlusButton->SetBounds(115,20,15,10);
	organismsNumberLabel->SetBounds(16,20,98,10);
	organismsMinusButton->SetBounds(0,20,15,10);
	orgInsprector->setBounds(dimx - 100,0,100,92);

    scale = SDL_SCALE;
	renderOffeset = dimx/2;
	moveMomentum = 0.0f;
    moveLeft = false;
	moveRight = false;
	needToScaleY = false;
	scaleDownLeft = true;
	scaleDownRight = true;
	scaleDown = true;
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

  bool SDL_WindowGroundpart::print() {
    gwenRenderer->BeginContext(NULL);

	if(orgInsprector->getOrganism() != primaryMarkedOrganism) {
		orgInsprector->setOrganism(primaryMarkedOrganism);
	};

	orgInsprector->update();

	SDL_Rect statusBarRect;
	//draw Background 
	statusBarRect.x = 0;
	statusBarRect.y = 0;
	statusBarRect.w = dimx;
	statusBarRect.h = dimy;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer,&statusBarRect);
	//draw status bar
	statusBarRect.x = 15;
	statusBarRect.h = 10;
	statusBarRect.y = 0;
	statusBarRect.w = 100 * chemStorage->getCurrentPercentage("Wasser") / 100.0f;
	SDL_SetRenderDrawColor(renderer, 0, 0, 140, 255);
	SDL_RenderFillRect(renderer,&statusBarRect);
	statusBarRect.y = 10;
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

    moveLeft = false;
	moveRight = false;
	needToScaleY = false;
	scaleDownLeft = true;
	scaleDownRight = true;
	scaleDown = true;
    Organism* org;

    int counter = 1;
    int max = organisms.size() + 1;
	int padding = dimx / 10;

    for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
      org = *it;
      if( org->getState() != BSP_dead) {
	    int orgX = ((dimx - padding)/max)*counter + padding / 2 + renderOffeset;
		int orgY = 0;

		int halfGroundpartWidth = getWidth() /  2;
		int halfDimX = dimx / 2;

		orgX = getOrganismX(org);
		orgY = scale * getHeightAt(orgX) - 1;
  	    orgX = (scale * orgX) + halfDimX - (halfGroundpartWidth * scale) + renderOffeset;

		if(scaleToOrganisms.size() != 0) {
			req_print(org->getRootBodypart(), orgX, orgY, 0.0f, 0.0f, 0.0f, isScaleToOrganism(org), org == primaryMarkedOrganism);
		}
		else {
			req_print(org->getRootBodypart(), orgX, orgY, 0.0f, 0.0f, 0.0f, true , org == primaryMarkedOrganism);
		}
        
      };
      counter++;
    };
    
	printHeigtmap();

	shadows->distribute();
	if(drawLightDebug) {
		shadows->draw();
	}
	shadows->clear();

    pCanvas->RenderCanvas();
	gwenRenderer->PresentContext(NULL);
	gwenRenderer->EndContext(NULL);

	scaleDown = scaleDown && (scaleDownLeft || scaleDownRight);

	float newScale = scale;

	if(needToScaleY) {
		newScale = scale * SCALE_FACTORY;
		moveMomentum = moveMomentum * MOVE_SLOWDOWN_FACTOR;
	}
	else if(moveLeft && moveRight) {
		newScale = scale * SCALE_FACTORX;
		moveMomentum = moveMomentum * MOVE_SLOWDOWN_FACTOR;
	}
	else if(moveLeft) {
		moveMomentum += MOVE_AMOUNT * scale;
		if(moveMomentum > (MOVE_MAX_AMOUNT * scale)) {
			moveMomentum = (MOVE_MAX_AMOUNT * scale);
		}
		if(scaleDown) {
			newScale = scale * DOWN_SCALE_FACTOR;
		}
	}
	else if(moveRight) {
		moveMomentum -= MOVE_AMOUNT * scale;
		if(moveMomentum < -(MOVE_MAX_AMOUNT * scale)) {
			moveMomentum = -(MOVE_MAX_AMOUNT * scale);
		}
		if(scaleDown) {
			newScale = scale * DOWN_SCALE_FACTOR;
		}
	}
    else {
		if(scaleDown) {
			newScale = scale * DOWN_SCALE_FACTOR;
		}
		moveMomentum = moveMomentum * MOVE_SLOWDOWN_FACTOR;
	}

	if(scale != newScale) {
		renderOffeset = renderOffeset/scale*newScale;
		scale = newScale;
	}
	
	renderOffeset += moveMomentum;

    return true;
  };

  void SDL_WindowGroundpart::processEvent(SDL_Event* evt) {
    if(evt->window.windowID == SDL_GetWindowID(window)) {
		GwenInput->ProcessEvent(evt);
	};

	if ((evt->type == SDL_WINDOWEVENT)) {
		if(evt->window.windowID == SDL_GetWindowID(window)) {
			if(evt->window.event == SDL_WINDOWEVENT_RESIZED) {
				resizeWindow(evt->window.data1, evt->window.data2);
			}
		}
	}
	else if(evt->type == SDL_MOUSEBUTTONUP) {
		if(evt->window.windowID == SDL_GetWindowID(window)) {
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
		}
	};
  }

  void SDL_WindowGroundpart::resizeWindow(int pDimx, int pDimy) {
	  dimx = pDimx;
	  dimy = pDimy;

	  pCanvas->SetSize(dimx,dimy);
	  orgInsprector->setBounds(dimx - 100,0,100,92);
	  shadows->setSize(dimx,dimy);
  };

  int SDL_WindowGroundpart::req_print(Bodypart *param_bp, int param_x, int param_y, float p_ang1, float p_ang2, float p_ang3, bool relevantForScaling, bool marked) {
	int returnvalue = 0;
    
    if(param_bp) {
      returnvalue = 1;
      int x1,y1,x2,y2;
      float a1,a2,a3,deltax,deltay,px,radi1,radi2,radi3;
      
      x1 = param_x;
      y1 = param_y;
      a1 = p_ang1;
      a2 = p_ang2;
      a3 = p_ang3;
      radi1 = 2.0f * M_PI * (p_ang1/360.0f);
      radi2 = 2.0f * M_PI * (p_ang2/360.0f);
      radi3 = 2.0f * M_PI * (p_ang3/360.0f);

      //an absolut rotation?
      deltax = sin(radi1);//*(cos(rad3));
      deltay = cos(radi1);//*cos(rad2);

      x2 = (int)(param_x + (deltax * param_bp->getSize() * scale));
      y2 = (int)(param_y + (deltay * param_bp->getSize() * scale));

	  if(shadows) {
		  shadows->add(param_bp,x1,y1,x2,y2);
	  }

      //std::cout << "(" << x1 << "\t" << y1 << ")\t(" << x2 << "\t" << y2 << ")\n";

	  if(relevantForScaling) {
		  if((x1 <= 0) || (x2 <= 0)) {
			  moveLeft = true;
		  }
		  if((x1 > dimx) || (x2 > dimx)) {
			  moveRight = true;
		  }
		  if((y1 > dimy) || (y2 > dimy)) {
			  needToScaleY = true;
		  };
		  if((x1 <= SCALE_DOWN_HYST) || (x2 <= SCALE_DOWN_HYST)) {
			  scaleDownLeft = false;
		  }
		  if((x1 > (dimx - SCALE_DOWN_HYST)) || (x2 > (dimx - SCALE_DOWN_HYST))) {
			  scaleDownRight = false;
		  }
		  if((y1 > (dimy - SCALE_DOWN_HYST)) || (y2 > (dimy - SCALE_DOWN_HYST))) {
			  scaleDown = false;
		  }
	  }

      if(renderer == 0) {
	    std::cout << "renderer is null\n";
      }

	  if(marked) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	  }
	  else {
		SDL_SetRenderDrawColor(renderer, param_bp->color.r * 255, param_bp->color.g * 255, param_bp->color.b * 255, param_bp->color.a * 255);
	  }
      SDL_RenderDrawLine(renderer,x1,dimy-(y1+1),x2,dimy-(y2+1));

      SpawnpointInformationList bpSpawnpoints = param_bp->getSpawnpoints();
      for(SpawnpointInformationListIterator it = bpSpawnpoints.begin(); it != bpSpawnpoints.end(); it++) {
        // position 0 is reserved for the backwards spawnpoint
        SpawnpointInformation* sp;
        if(((*it)->occupied) && ((*it)->position != 0)) { 
          sp = (*it)->connectedBodypart->getSpawnpointInformationForBodypart(param_bp);
          float partner_ang1 = sp->ang2d;
          float partner_ang2 = sp->ang2;
          float partner_ang3 = sp->rot;
          returnvalue += req_print((*it)->connectedBodypart,x2,y2,a1 + 180.0f + partner_ang1 + (*it)->ang2d,a2 + partner_ang2 + (*it)->ang2,a3 + partner_ang3 + (*it)->rot, relevantForScaling, marked);
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
      storage->add(resourceName,storage->getMaxAmount(resourceName) * 0.1f);
    }
    else {
      storage->add(resourceName,-storage->getMaxAmount(resourceName) * 0.1f);
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

  RuntimeManager* SDL_WindowGroundpart::getRuntimeManager() {
	  return runtime;
  };

  bool SDL_WindowGroundpart::registerOrganism(Organism* param_organism) {
	  int posX = getOrganismX(param_organism);
	  bool emptySlotFound = false;
	  int i = 0;
	  int newPosX;
	  while(!emptySlotFound) {
	    if(!isOccupiedByAlivePlant(posX + i)) {
			newPosX = posX + i;
			emptySlotFound = true;
		}
		else if(!isOccupiedByAlivePlant(posX - i)) {
			newPosX = posX - i;
			emptySlotFound = true;
		}

		i++;
	  }

	  if(posX != newPosX) {
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
}