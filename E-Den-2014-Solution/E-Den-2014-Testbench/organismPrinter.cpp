// organismPrinter.cpp
// by Franz Koehler 2009

#include "organismPrinter.h"
#include "stdlib.h"
#include "math.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

#define SDL_SCALE 40
//#define SCALE_FACTORX 0.99991f
#define SCALE_FACTORX 0.995f
#define SCALE_FACTORY 0.99f
#define MOVE_AMOUNT 0.05f
#define MOVE_MAX_AMOUNT 15.0f
#define MOVE_SLOWDOWN_FACTOR 0.7f
#define DOWN_SCALE_FACTOR 1.0001f

namespace EDen {

  bool OrganismPrinter::print() { 
    std::cout << "\n--[" << org->getName() << "]-[" << org->getBodypartCount() << "]--" << std::endl;
    printLetter(org->getRootBodypart(),true,true,false);
    int i = 0;
    while(true == print(org->getRootBodypart(),true,true,false,0,i)) 
    {
      i++;
    };
    
    return true;
  };

  bool OrganismPrinter::printLetter(Bodypart* bp, bool linebreak, bool lastelement, bool tab) {
    
    switch(bp->getBodypartState()) {
      case BSP_creation:
        std::cout << ".";
        break;
      case BSP_alive:
        std::cout << " ";
        break;
      case BSP_dead:
        std::cout << "#";
        break;
      case BSP_UNKNOWN:
        std::cout << "?";
        break;
      default:
        std::cout << "??";
    };
    
    switch(bp->getBodypartType()) {
      case BPT_Leaf:
        std::cout << "L";
        break;
      case BPT_Stick:
        std::cout << "S";
        break;
      case BPT_Branch:
        std::cout << "B";
        break;
      default:
        std::cout << "?";
    };

    if(tab && lastelement) std::cout << "\t";
    if(linebreak && lastelement) std::cout << std::endl << "-- ";

    return true;
  };

  bool OrganismPrinter::print(Bodypart* bp, bool linebreak, bool lastelement, bool tab, int lvl, int targetlvl) {
    BodypartList list = bp->getChildBodyparts();
    BodypartList bpsWithChilds;
    BodypartList bpsWithoutChilds;

    Bodypart* lastBPWithChilds = 0;

    if(list.empty()) return false;
    else if(lvl > targetlvl) return true;
    else {
      bool returnvalue = true;
      
      for(BodypartListIterator it = list.begin(); it != list.end(); it++) {
        if(!(*it)->getChildBodyparts().empty()) {
          bpsWithChilds.push_back((*it));
          lastBPWithChilds = (*it);
        }
        else bpsWithoutChilds.push_back((*it));
      };


      int count;
      int size;

      if(lvl == targetlvl) {
        count = 0;
        size = bpsWithoutChilds.size();

        for(BodypartListIterator it = bpsWithoutChilds.begin(); it != bpsWithoutChilds.end(); it++) {
          if(count < (size - 1)) printLetter(*it,false,false);
          else printLetter(*it,linebreak, bpsWithChilds.empty(), tab);
          count++;
        };

        count = 0;
        size = bpsWithChilds.size();

        for(BodypartListIterator it = bpsWithChilds.begin(); it != bpsWithChilds.end(); it++) {
          if(count < (size - 1)) printLetter(*it,false,true,true);
          else printLetter(*it, linebreak, true, tab);
          count++;
        };
      };

      if(lvl < targetlvl) {
        count = 0;
        size = bpsWithChilds.size();

        bool childWithChildsInLvlrangeFound = false;

        for(BodypartListIterator it = bpsWithChilds.begin(); it != bpsWithChilds.end(); it++) {
          if(count < (size - 1)) childWithChildsInLvlrangeFound = print(*it,false,true,true, lvl + 1, targetlvl) || childWithChildsInLvlrangeFound;
          else childWithChildsInLvlrangeFound = print(*it, linebreak, true, tab, lvl + 1, targetlvl) || childWithChildsInLvlrangeFound;
          count++;
        };
        
        returnvalue = returnvalue && childWithChildsInLvlrangeFound;

      };

      if(returnvalue)
        return returnvalue;
      else
        return returnvalue;
    };
  };

  SDLOrganismPrinter::SDLOrganismPrinter(SDL_Renderer *param_renderer, int param_dimx, int param_dimy, int param_offx, int param_offy, Groundpart* param_groundpart, RuntimeManager* param_runtimemanager) {
    static bool firstInit = true;
	//static SDL_Window *window = 0;
	renderer = param_renderer;
	runtime = param_runtimemanager;
    groundpart = param_groundpart;
	extendedGroundpart = dynamic_cast<SingleDimensionHeightmapGroundpart*>(groundpart);
    sun = new SDL_SunlightProvider();
    runtime->add(sun);
	
    dimx = param_dimx;
    dimy = param_dimy;
	offX = param_offx;
	offY = param_offy;

	shadows = new SDL_ShadowAccumulator(offX,offY,dimx,dimy,sun,renderer);
	drawLightDebug = false;

	gwenRenderer = new Gwen::Renderer::SDL2(renderer);
	Gwen::Skin::TexturedBase* skin = new Gwen::Skin::TexturedBase(gwenRenderer);
	skin->SetRender(gwenRenderer);
	skin->Init("DefaultSkin.png");
	skin->SetDefaultFont("OpenSans.ttf", 8);

	pCanvas = new Gwen::Controls::Canvas(skin);
	pCanvas->SetSize(dimx, dimy);
	pCanvas->SetBounds(offX,offY,dimx,dimy);
	pCanvas->SetDrawBackground(true);
	pCanvas->SetBackgroundColor(Gwen::Color(0, 0, 0, 255));

	GwenInput = new Gwen::Input::SDL2();
	GwenInput->Initialize(pCanvas);

	waterPlusButton = new Gwen::Controls::Button(pCanvas);
    waterPlusButton->SetText("W+");
	waterPlusEventHandler = new ResourceButtonEventHandler(groundpart,"Wasser",true);
	waterPlusButton->onPress.Add(waterPlusEventHandler, &ResourceButtonEventHandler::onClick);
	waterMinusButton = new Gwen::Controls::Button(pCanvas);
    waterMinusButton->SetText("W-");
	waterMinusEventHandler = new ResourceButtonEventHandler(groundpart,"Wasser",false);
	waterMinusButton->onPress.Add(waterMinusEventHandler, &ResourceButtonEventHandler::onClick);
	gooPlusButton = new Gwen::Controls::Button(pCanvas);
    gooPlusButton->SetText("G+");
	gooPlusEventHandler = new ResourceButtonEventHandler(groundpart,"Goo",true);
	gooPlusButton->onPress.Add(gooPlusEventHandler, &ResourceButtonEventHandler::onClick);
	gooMinusButton = new Gwen::Controls::Button(pCanvas);
    gooMinusButton->SetText("G-");
	gooMinusEventHandler = new ResourceButtonEventHandler(groundpart,"Goo",false);
	gooMinusButton->onPress.Add(gooMinusEventHandler, &ResourceButtonEventHandler::onClick);
	organismsNumberLabel = new Gwen::Controls::Label(pCanvas);
	organismsNumberLabel->SetText("0/0");
	organismsNumberLabel->SetAlignment(3);
	organismsNumberLabel->SetTextColor(Gwen::Color(250,200,0,255));
	organismsPlusButton = new Gwen::Controls::Button(pCanvas);
	organismsPlusButton->SetText("O+");
	organismsPlusButtonEventHandler = new OrganismCountButtonEventHandler(groundpart,runtime,organismsNumberLabel,true);
	organismsPlusButton->onPress.Add(organismsPlusButtonEventHandler, &OrganismCountButtonEventHandler::onClick);
	organismsMinusButton = new Gwen::Controls::Button(pCanvas);
	organismsMinusButton->SetText("O-");
	organismsMinusButtonEventHandler = new OrganismCountButtonEventHandler(groundpart,runtime,organismsNumberLabel,false);
	organismsMinusButton->onPress.Add(organismsMinusButtonEventHandler, &OrganismCountButtonEventHandler::onClick);	

	waterPlusButton->SetBounds(115,0,15,10);
	waterMinusButton->SetBounds(0,0,15,10);
	gooPlusButton->SetBounds(115,10,15,10);
	gooMinusButton->SetBounds(0,10,15,10);
	organismsPlusButton->SetBounds(115,20,15,10);
	organismsNumberLabel->SetBounds(16,20,98,10);
	organismsMinusButton->SetBounds(0,20,15,10);

//    updateCaption();
    scale = SDL_SCALE;
	renderOffeset = dimx/2;
	moveMomentum = 0.0f;
    moveLeft = false;
	moveRight = false;
	needToScaleY = false;
//    resetScreen();
  };

  SDLOrganismPrinter::~SDLOrganismPrinter() {
    if(runtime) runtime->remove(sun);
  };

 // bool SDLOrganismPrinter::resetScreen() {
	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	//SDL_RenderClear(renderer);
 //   
 //   percentagePrinterCounter = 0;

 //   return true;
 // };

//  bool SDLOrganismPrinter::redrawScreen() {
////	SDL_RenderCopy(renderer, screen, NULL, NULL);
//	SDL_RenderPresent(renderer);
//
//	//SDL_UpdateWindowSurface(window);
//    return true;
//  };

  bool SDLOrganismPrinter::add(Organism* param_organism) {
    static bool alternate;
    
    if(alternate) organisms.push_front(param_organism);
    else organisms.push_back(param_organism);

    alternate = ! alternate;
    return true;
  };

  std::list<Organism*> SDLOrganismPrinter::getOrganisms() {
    return organisms;
  };

  bool SDLOrganismPrinter::cleanupDeadOrganisms() {
    Organism* org;
    std::list<Organism*> new_orgs;

    while(!organisms.empty()) {
      org = organisms.back();
      organisms.pop_back();
      if(org)
          if(org->getState() != BSP_dead)
              new_orgs.push_front(org);
    };

    organisms.clear();
    organisms.swap(new_orgs);

    return true;
  };

  //bool SDLOrganismPrinter::updateCaption() {
  //  std::string newCaption = "";
  //  char str[255];
  //  
  //  //if(runtimeManager) {
  //  //  sprintf(str,"[%d|%lu]-",runtimeManager->getCps(), runtimeManager->getCycleCount());
  //  //  newCaption += str;
  //  //}

  //  if(runtimeManager) newCaption.append(runtimeManager->getDebugOut(true)).append("-");

  //  for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
  //    sprintf(str,"(%d|%d|%d)",(*it)->getRootBodypart()->getGeneticCode()->getSpeciesIdentifier(),(*it)->getBodypartCount(),(*it)->getLifetime());
  //    newCaption += str;
  //  };

  //  if(runtimeManager) newCaption.append(runtimeManager->getDebugOut());

  //  SDL_SetWindowTitle(window,newCaption.c_str());
  //  return true;
  //};

  bool SDLOrganismPrinter::print() {
    gwenRenderer->BeginContext(NULL);
	pCanvas->RenderCanvas();

	if(groundpart) organisms = groundpart->getOrganisms();
    else cleanupDeadOrganisms();
    
	SDL_Rect statusBarRect;
	statusBarRect.x = 15;
	statusBarRect.h = 10;
	statusBarRect.y = 0;
	statusBarRect.w = 100 * groundpart->getChemicalStorage()->getCurrentPercentage("Wasser") / 100.0f;
	SDL_SetRenderDrawColor(renderer, 0, 0, 140, 255);
	SDL_RenderFillRect(renderer,&statusBarRect);
	statusBarRect.y = 10;
	statusBarRect.w = 100 * groundpart->getChemicalStorage()->getCurrentPercentage("Goo") / 100.0f;
	SDL_SetRenderDrawColor(renderer, 140, 30, 0, 255);
	SDL_RenderFillRect(renderer,&statusBarRect);

	if(organismsNumberLabel) {
      int new_numOrganisms, new_numEmptySpaces,new_cps;
      groundpart->getNumOrganismsAndEmptySpaces(&new_numOrganisms,&new_numEmptySpaces);
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
    Organism* org;

    int counter = 1;
    int max = organisms.size() + 1;
	int padding = dimx / 10;

    for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
      org = *it;
      if( org->getState() != BSP_dead) {
	    int orgX = ((dimx - padding)/max)*counter + padding / 2 + renderOffeset;
		int orgY = 0;

		if(extendedGroundpart != 0) {
		  int halfGroundpartWidth = extendedGroundpart->getWidth() /  2;
		  int halfDimX = dimx / 2;

		  orgX = extendedGroundpart->getOrganismX(org);
		  orgY = scale * extendedGroundpart->getHeightAt(orgX) - 1;
		  orgX = (scale * orgX) + halfDimX - (halfGroundpartWidth * scale) + renderOffeset;
		};

        req_print(org->getRootBodypart(), orgX, orgY, 0.0f, 0.0f, 0.0f);
      };
      counter++;
    };
    
	printHeigtmap();

	shadows->distribute();
	if(drawLightDebug) {
		shadows->draw();
	}
	shadows->clear();

	gwenRenderer->PresentContext(NULL);
	gwenRenderer->EndContext(NULL);

	if(needToScaleY) {
		scale = scale * SCALE_FACTORY;
		moveMomentum = moveMomentum * MOVE_SLOWDOWN_FACTOR;
	}
	else if(moveLeft && moveRight) {
		scale = scale * SCALE_FACTORX;
		moveMomentum = moveMomentum * MOVE_SLOWDOWN_FACTOR;
	}
	else if(moveLeft) {
		moveMomentum += MOVE_AMOUNT * scale;
		if(moveMomentum > (MOVE_MAX_AMOUNT * scale)) {
			moveMomentum = (MOVE_MAX_AMOUNT * scale);
		}
		scale = scale * DOWN_SCALE_FACTOR;
	}
	else if(moveRight) {
		moveMomentum -= MOVE_AMOUNT * scale;
		if(moveMomentum < -(MOVE_MAX_AMOUNT * scale)) {
			moveMomentum = -(MOVE_MAX_AMOUNT * scale);
		}
		scale = scale * DOWN_SCALE_FACTOR;
	}
    else {
		scale = scale * DOWN_SCALE_FACTOR;
		moveMomentum = moveMomentum * MOVE_SLOWDOWN_FACTOR;
	}
    //if(runtimeManager->getState() == RMS_Slow)
    //    updateCaption();
    //else if(runtimeManager->getCycleCount() % 10 == 0)
    //    updateCaption();

	renderOffeset += moveMomentum;

    return true;
  };

  void SDLOrganismPrinter::processEvent(SDL_Event* evt) {
	GwenInput->ProcessEvent(evt);
  }

  int SDLOrganismPrinter::req_print(Bodypart *param_bp, int param_x, int param_y, float p_ang1, float p_ang2, float p_ang3) {
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

      if((x1 <= 0) || (x2 <= 0)) {
		  moveLeft = true;
	  }
	  if((x1 > dimx) || (x2 > dimx)) {
		  moveRight = true;
	  }
	  if((y1> dimy) || (y2 > dimy)) {
		  needToScaleY = true;
      };

    //  if(
		  ////(x1-offsetx >= 0) && (x1-offsetx < dimx) && (x2-offsetx >= 0) && (x2-offsetx < dimx) && 
    //    (y1 >= 0) && (y1< dimy) && (y2 >= 0) && (y2 < dimy)) {
		
      if(renderer == 0) {
	    std::cout << "renderer is null\n";
      }
		//if(param_bp->getBodypartType() == BPT_Leaf) {
		//	SDL_SetRenderDrawColor(renderer, param_bp->color.r * 255, param_bp->color.g * 255, param_bp->color.b * 255, ((param_bp->getChemicalStorage()->getCurrentPercentage("Sonne") * 200) / 100) + 55);
		//}
		//else {
		//	SDL_SetRenderDrawColor(renderer, param_bp->color.r * 255, param_bp->color.g * 255, param_bp->color.b * 255, ((param_bp->getChemicalStorage()->getCurrentPercentage("Energie") * 200) / 100) + 55);
		//}
      SDL_SetRenderDrawColor(renderer, param_bp->color.r * 255, param_bp->color.g * 255, param_bp->color.b * 255, param_bp->color.a * 255);
      SDL_RenderDrawLine(renderer,x1+offX,dimy-(y1+1)+offY,x2+offX,dimy-(y2+1)+offY);

      SpawnpointInformationList bpSpawnpoints = param_bp->getSpawnpoints();
      for(SpawnpointInformationListIterator it = bpSpawnpoints.begin(); it != bpSpawnpoints.end(); it++) {
        // position 0 is reserved for the backwards spawnpoint
        SpawnpointInformation* sp;
        if(((*it)->occupied) && ((*it)->position != 0)) { 
          sp = (*it)->connectedBodypart->getSpawnpointInformationForBodypart(param_bp);
          float partner_ang1 = sp->ang2d;
          float partner_ang2 = sp->ang2;
          float partner_ang3 = sp->rot;
          returnvalue += req_print((*it)->connectedBodypart,x2,y2,a1 + 180.0f + partner_ang1 + (*it)->ang2d,a2 + partner_ang2 + (*it)->ang2,a3 + partner_ang3 + (*it)->rot);
        };
      };
      //std::cout << returnvalue << std::endl;
      return returnvalue;
    }
    else return 0;
  };

  void SDLOrganismPrinter::ResourceButtonEventHandler::onClick(Gwen::Controls::Base* pControl) {
    ChemicalStorage* storage = gp->getChemicalStorage();
    if(increment) {
      storage->add(resourceName,storage->getMaxAmount(resourceName) * 0.1f);
    }
    else {
      storage->add(resourceName,-storage->getMaxAmount(resourceName) * 0.1f);
    }
  };

  void SDLOrganismPrinter::OrganismCountButtonEventHandler::onClick(Gwen::Controls::Base* pControl) {
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

  void SDLOrganismPrinter::printHeigtmap() {
	  if(extendedGroundpart != 0) {
		  int height;
		  int halfGroundpartWidth = extendedGroundpart->getWidth() /  2;
		  int halfDimX = dimx / 2;
		  SDL_SetRenderDrawColor(renderer, 100, extendedGroundpart->getChemicalStorage()->getCurrentPercentage("Wasser")/100.0f * 100 + 100, extendedGroundpart->getChemicalStorage()->getCurrentPercentage("Goo")/100.0f * 100, 255);
		  int ix;
		  for(int i = 0; i < dimx; i++) {
			ix = i - renderOffeset;
			height = extendedGroundpart->getHeightAt(halfGroundpartWidth + ((ix - halfDimX) * (1.0f/scale))) * scale;
			SDL_RenderDrawLine(renderer, i + offX,dimy - (offY - 1 + height), i + offX,dimy - (offY - 1));
		  };
	  };
  };

  int SDLOrganismPrinter::getDimX() {
    return dimx;  
  };

  int SDLOrganismPrinter::getDimY() {
    return dimy; 
  };

  int SDLOrganismPrinter::getOffX() {
    return offX;  
  };

  int SDLOrganismPrinter::getOffY() {
    return offY; 
  };

  void SDLOrganismPrinter::setDrawLightDebug(bool param_lightDebug) {
	  drawLightDebug = param_lightDebug;
  };

  bool SDLOrganismPrinter::getDrawLightDebug() {
	  return drawLightDebug;
  };

  void SDLOrganismPrinter::setOffset(int param_offX, int param_offY) {
	  offX = param_offX;
	  offY = param_offY;
  };

  bool SDLOrganismPrinter::orgsAlive() {
    if(organisms.size() > 0) return true;
    return false;
  };

  bool SDL_SunlightProvider::setFactor(Bodypart* param_bodypart ,float param_factor) {
    ExtendedBodypartInformation* info = dynamic_cast<ExtendedBodypartInformation*>(getInformation(param_bodypart));
    if(info)
      info->factor = param_factor;

    else {
      info = new ExtendedBodypartInformation();
      info->factor = param_factor;
      updateBodypartInformation(param_bodypart,info);
    };

    return true;
  };

  SDL_ShadowAccumulator::SDL_ShadowAccumulator(int param_x, int param_y, int param_w, int param_h, SDL_SunlightProvider* param_provider, SDL_Renderer* param_renderer) {
	  x = param_x;
	  y = param_y;
	  h = param_h;
	  w = param_w;

	  provider = param_provider;
	  renderer = param_renderer;

	  lightConsumerType = BPT_Leaf;
	  shadowcasterType = BPT_Leaf;
  };

  void SDL_ShadowAccumulator::add(Bodypart* param_bodypart, int param_x1, int param_y1, int param_x2, int param_y2) {
	  BodypartType paramType = param_bodypart->getBodypartType();
	  
	  if(paramType == lightConsumerType) {
		  std::pair<Bodypart*,line>* newPair = new std::pair<Bodypart*,line>(param_bodypart,line(param_x1, h - param_y1, param_x2, h - param_y2));
		  lightConsumer.push_back(newPair);
	  }
	  if(paramType == shadowcasterType) {
		  add(param_x1, param_y1, param_x2, param_y2, color(&(param_bodypart->color)));
	  }
  };

  void SDL_ShadowAccumulator::distribute() {
	  for(std::list<std::pair<Bodypart*, line>*>::iterator it = lightConsumer.begin(); it != lightConsumer.end(); it++) {
		  color receptiveColor = color(&(*it)->first->color);
		  float a = (*it)->first->color.a;
		  int x = ((*it)->second.x1 + (*it)->second.x2) / 2;
		  int y = h - (((*it)->second.y1 + (*it)->second.y2) / 2);

		  float sunValue = 0.0f;
		  color shadowColor = getShadowAt(x,y);
		  if(y > 0) {
			  sunValue += (1.0f - abs(receptiveColor.r - (1.0f - shadowColor.r))) * (1.0f - shadowColor.r) * a;
			  sunValue += (1.0f - abs(receptiveColor.g - (1.0f - shadowColor.g))) * (1.0f - shadowColor.g) * a;
			  sunValue += (1.0f - abs(receptiveColor.b - (1.0f - shadowColor.b))) * (1.0f - shadowColor.b) * a;
			  //sunValue += receptiveColor.r * (1.0f - shadowColor.r);
			  //sunValue += receptiveColor.g * (1.0f - shadowColor.g);
			  //sunValue += receptiveColor.b * (1.0f - shadowColor.b);
		  }

		  x = (*it)->second.x1;
		  y = h - (*it)->second.y1;

		  if(y > 0) {
			  color shadowColor = getShadowAt(x,y);
			  sunValue += (1.0f - abs(receptiveColor.r - (1.0f - shadowColor.r))) * (1.0f - shadowColor.r) * a;
			  sunValue += (1.0f - abs(receptiveColor.g - (1.0f - shadowColor.g))) * (1.0f - shadowColor.g) * a;
			  sunValue += (1.0f - abs(receptiveColor.b - (1.0f - shadowColor.b))) * (1.0f - shadowColor.b) * a;
		  }

		  x = (*it)->second.x2;
		  y = h - (*it)->second.y2;

		  if(y > 0) {
			  color shadowColor = getShadowAt(x,y);
			  sunValue += (1.0f - abs(receptiveColor.r - (1.0f - shadowColor.r))) * (1.0f - shadowColor.r) * a;
			  sunValue += (1.0f - abs(receptiveColor.g - (1.0f - shadowColor.g))) * (1.0f - shadowColor.g) * a;
			  sunValue += (1.0f - abs(receptiveColor.b - (1.0f - shadowColor.b))) * (1.0f - shadowColor.b) * a;
		  }

		  provider->setFactor((*it)->first, sunValue / 9);
	  };
  };

  void SDL_ShadowAccumulator::draw() {
	  // draw the light
	  const int alphaBg = 50; 

	  for(int i = 0; i < w; i++) {
		  std::map<int,std::list<shadowInformation>*>::iterator it = shadowMap.find(i);
		  if(it == shadowMap.end()) {
			  SDL_SetRenderDrawColor(renderer, 255, 255, 255, alphaBg);
			  SDL_RenderDrawLine(renderer,i + x ,0 + y - 1,i + x ,h + y - 1);
		  }
		  else {
			  color accu = color(1.0f,1.0f,1.0f);
			  int lastY = h;

			  for(std::list<shadowInformation>::iterator it2 = it->second->begin(); it2 != it->second->end(); it2++) {
				  SDL_SetRenderDrawColor(renderer,accu.r * 255,accu.g * 255,accu.b * 255, alphaBg);
				  SDL_RenderDrawLine(renderer,i + x ,h - it2->y + y - 1,i + x ,h - lastY + y - 1);
				  lastY = it2->y;

				  accu.r *= 1.0f - it2->c.r;
				  accu.g *= 1.0f - it2->c.g;
				  accu.b *= 1.0f - it2->c.b;
			  };

			  SDL_SetRenderDrawColor(renderer,accu.r * 255,accu.g * 255,accu.b * 255, alphaBg);
			  SDL_RenderDrawLine(renderer,i + x,h + y - 1,i + x,h - lastY + y - 1);
		  };
	  };

	  // draw consumer indicators
	  int xl;
	  int yl;
	  float sunValue;
	  for(std::list<std::pair<Bodypart*, line>*>::iterator it = lightConsumer.begin(); it != lightConsumer.end(); it++) {
		  color receptiveColor = color(&(*it)->first->color);
		  float a = (*it)->first->color.a;
		  xl = ((*it)->second.x1 + (*it)->second.x2) / 2;
		  yl = h - (((*it)->second.y1 + (*it)->second.y2) / 2);

		  sunValue = 0.0f;
		  color shadowColor = getShadowAt(xl,yl);
		  if(yl > 0) {
							// (leafcolor - lightcolor) * lightcolor * alpha
			  sunValue += (1.0f - abs(receptiveColor.r - (1.0f - shadowColor.r))) * (1.0f - shadowColor.r) * a;
			  sunValue += (1.0f - abs(receptiveColor.g - (1.0f - shadowColor.g))) * (1.0f - shadowColor.g) * a;
			  sunValue += (1.0f - abs(receptiveColor.b - (1.0f - shadowColor.b))) * (1.0f - shadowColor.b) * a;
		  }

		  xl = (*it)->second.x1;
		  yl = h - (*it)->second.y1;

		  if(yl > 0) {
			  shadowColor = getShadowAt(xl,yl);
			  sunValue += (1.0f - abs(receptiveColor.r - (1.0f - shadowColor.r))) * (1.0f - shadowColor.r) * a;
			  sunValue += (1.0f - abs(receptiveColor.g - (1.0f - shadowColor.g))) * (1.0f - shadowColor.g) * a;
			  sunValue += (1.0f - abs(receptiveColor.b - (1.0f - shadowColor.b))) * (1.0f - shadowColor.b) * a;
		  }

		  xl = (*it)->second.x2;
		  yl = h - (*it)->second.y2;

		  if(yl > 0) {
			  shadowColor = getShadowAt(xl,yl);
			  sunValue += (1.0f - abs(receptiveColor.r - (1.0f - shadowColor.r))) * (1.0f - shadowColor.r) * a;
			  sunValue += (1.0f - abs(receptiveColor.g - (1.0f - shadowColor.g))) * (1.0f - shadowColor.g) * a;
			  sunValue += (1.0f - abs(receptiveColor.b - (1.0f - shadowColor.b))) * (1.0f - shadowColor.b) * a;
		  }

		  sunValue = sunValue / 9;

		  if(sunValue > 0.98f) {
			  SDL_SetRenderDrawColor(renderer,120 + (135 * (sunValue - 0.5f)), sunValue * 255, (sunValue - 0.98f) * 255, 255);
		  }
		  else if(sunValue > 0.5f) {
			  SDL_SetRenderDrawColor(renderer,120 + (135 * (sunValue - 0.5f)), sunValue * 255, 0, 255);
		  }
		  else if(sunValue > 0.1f) {
			  SDL_SetRenderDrawColor(renderer,120 + (135 * (0.5f - sunValue)) , sunValue * 255, 0, 255);
		  }
		  else {
			  SDL_SetRenderDrawColor(renderer,(120 * sunValue) + (135 * (0.5f - sunValue)) , sunValue * 255, 0, 255);
		  }
		  
		  SDL_RenderDrawLine(renderer,x + (*it)->second.x1, y + (*it)->second.y1 - 1, x + (*it)->second.x2, y + (*it)->second.y2 - 1);
	  };
  };

  void SDL_ShadowAccumulator::clear() {
	  std::map<int,std::list<shadowInformation>*>::iterator itShadowsMap = shadowMap.begin();
	  while(itShadowsMap != shadowMap.end()) {
		  itShadowsMap->second->clear();
		  delete itShadowsMap->second;
		  shadowMap.erase(itShadowsMap);
		  itShadowsMap = shadowMap.begin();
	  };

	  std::list<std::pair<Bodypart*, line>*>::iterator itConsumer = lightConsumer.begin();
	  while(itConsumer != lightConsumer.end()) {
		  delete (*itConsumer);
		  lightConsumer.erase(itConsumer);
		  itConsumer = lightConsumer.begin();
	  };
  };

  void SDL_ShadowAccumulator::add(int param_x1, int param_y1, int param_x2, int param_y2, color param_c) {
	int y;
	float m = 0;
	if(abs(param_x1 - param_x2) > 0) {
		m = ((float)param_y1 - (float)param_y2) / ((float)param_x1 - (float)param_x2);
	}
	
	//if(param_y1 < param_y2) {
	//	y = param_y1;
	//}
	//else {
	//	y = param_y2;
	//}

	if(param_x1 > param_x2) {
		for(int i = 0; i <= param_x1 - param_x2; i++) {
			std::map<int,std::list<shadowInformation>*>::iterator it = shadowMap.find(i + param_x2);
			if(it == shadowMap.end()) {
				std::list<shadowInformation>* newList = new std::list<shadowInformation>();
				newList->push_back(shadowInformation(i * m + param_y2,param_c));
				shadowMap.insert(std::pair<int,std::list<shadowInformation>*>(i + param_x2,newList));
			}
			else {
				insertOrdered(it->second,shadowInformation(i * m + param_y2,param_c));
			}
		}
	}
	else {
		for(int i = 0; i <= param_x2 - param_x1; i++) {
			std::map<int,std::list<shadowInformation>*>::iterator it = shadowMap.find(i + param_x1);
			if(it == shadowMap.end()) {
				std::list<shadowInformation>* newList = new std::list<shadowInformation>();
				newList->push_back(shadowInformation((i * m) + param_y1,param_c));
				shadowMap.insert(std::pair<int,std::list<shadowInformation>*>(i + param_x1,newList));
			}
			else {
				insertOrdered(it->second,shadowInformation((i * m) + param_y1,param_c));
			}
		}
	}
  };

  void SDL_ShadowAccumulator::insertOrdered(std::list<shadowInformation>* list, shadowInformation info) {
	  bool inserted = false;
	  for(std::list<shadowInformation>::iterator it = list->begin(); it != list->end(); it++) {
		  if( (*it).y < info.y)  {
			  list->emplace(it,info);
			  inserted = true;
			  break;
		  }
	  }
	  if(!inserted) {
		  list->push_back(info);
	  }
  };

  SDL_ShadowAccumulator::color SDL_ShadowAccumulator::getShadowAt(int x, int y) {
	  std::map<int,std::list<shadowInformation>*>::iterator it = shadowMap.find(x);
	  if(it == shadowMap.end()) {
		  return color(0.0f, 0.0f, 0.0f);
	  }
	  else {
		  color currentLight = color(1.0f,1.0f,1.0f);

		  for(std::list<shadowInformation>::iterator it2 = it->second->begin(); it2 != it->second->end(); it2++) {
			  if(it2->y > y) {
				  currentLight.r = currentLight.r * (1.0f - it2->c.r);
				  currentLight.g = currentLight.g * (1.0f - it2->c.g);
				  currentLight.b = currentLight.b * (1.0f - it2->c.b);
			  }
			  else {
				  break;
			  }
		  };

		  currentLight.r = 1.0f - currentLight.r;
		  currentLight.g = 1.0f - currentLight.g;
		  currentLight.b = 1.0f - currentLight.b;

		  return currentLight;
	  };
  };
}