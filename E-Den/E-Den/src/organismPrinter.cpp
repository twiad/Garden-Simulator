// organismPrinter.cpp
// by Franz Koehler 2009

#include "organismPrinter.h"
#include "stdlib.h"
#include "math.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#define SDL_SCALE 10
#define SCALE_FACTOR 0.9f
#endif

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

  SDLOrganismPrinter::SDLOrganismPrinter(int param_dimx, int param_dimy, RuntimeManager* param_runtimeManager): 
    dimx(param_dimx), 
    dimy(param_dimy) {
    runtimeManager = param_runtimeManager;
    sun = new SDL_SunlightProvider();
    if(runtimeManager) runtimeManager->add(sun);
    screen = SDL_SetVideoMode(dimx,dimy,16,SDL_HWSURFACE|SDL_ANYFORMAT);	
    updateCaption();
    scale = SDL_SCALE;
    needToScale = false;
    resetScreen();
  };

  SDLOrganismPrinter::~SDLOrganismPrinter() {
    if(runtimeManager) runtimeManager->remove(sun);
  };

  bool SDLOrganismPrinter::resetScreen() {
    if(SDL_MUSTLOCK(screen))
		  SDL_LockSurface(screen);
    
    //for(int x = 0; x < dimx; x++)
    //  for(int y = 0; y < dimy; y++)
    //    // putpixel(screen,x,y,SDL_MapRGB(screen->format,x%65,y%255,(x+y)%255));
    //    putpixel(screen,x,y,SDL_MapRGB(screen->format,0,0,0));

    Draw_FillRect(screen,0,0,dimx,dimy,SDL_MapRGB(screen->format,0,0,0));
    
    percentagePrinterCounter = 0;

    return true;
  };

  bool SDLOrganismPrinter::redrawScreen() {
    SDL_UpdateRect(screen,0,0,dimx,dimy);

    return true;
  };

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

  bool SDLOrganismPrinter::updateCaption() {
    std::string newCaption = "";
    char str[64];
    if(runtimeManager) {
      sprintf(str,"[%lu]- [%d|%d",runtimeManager->getCycleCount(),runtimeManager->getSeedCount(),runtimeManager->getCandidatesCount());
      newCaption += str;
    }

    //for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
    //  newCaption += " " + (*it)->getName(); 
    //};
    
    sprintf(str,"|%d]",organisms.size());
    newCaption += str;

    SDL_WM_SetCaption(newCaption.c_str(),newCaption.c_str());
    return true;
  };

  bool SDLOrganismPrinter::print() {
    resetScreen();
    if(runtimeManager) organisms = runtimeManager->getOrganisms();
    else cleanupDeadOrganisms();
    
    needToScale = false;
    Organism* org;

    int counter = 1;
    int max = organisms.size() + 1;

    for(std::list<Organism*>::iterator it = organisms.begin(); it != organisms.end(); it++) {
      org = *it;
      if( org->getState() != BSP_dead) {
        req_print(org->getRootBodypart(),(dimx/max)*counter,0,0.0f);
      };
      counter++;
    };
    
    //redrawScreen();

    if(needToScale) scale = scale * SCALE_FACTOR;
    
    if(runtimeManager->getCycleCount() % 10 == 0)
        updateCaption();

    return true;
  };

  int SDLOrganismPrinter::req_print(Bodypart *param_bp, int param_x, int param_y, float param_angle) {
    int offsetx = 0;
    //int offsety = -dimy/2;
    int offsety = 0;
    
    if(param_bp) {
      int returnvalue = 1;
      int x1,y1,x2,y2;
      float ang,dx,dy,px;
      
      x1 = param_x;
      y1 = param_y;
      ang = param_angle;

      dx = sin(2.0f * M_PI * (param_angle/360.0f));
      dy = cos(2.0f * M_PI * (param_angle/360.0f));

      x2 = (int)(param_x + (dx * param_bp->getSize() * scale));
      y2 = (int)(param_y + (dy * param_bp->getSize() * scale));

      px = (float(y1) / (float)dimy) * 5;
      if(sun)
          sun->setFactor(param_bp,px / scale);

      //std::cout << "(" << x1 << "\t" << y1 << ")\t(" << x2 << "\t" << y2 << ")\n";

      if((x1-offsetx <= 0) || (x1-offsetx > dimx) || (x2-offsetx <= 0) || (x2-offsetx > dimx) || 
        (y1-offsety> dimy) || (y2-offsety > dimy)) {
        needToScale = true;
      };

      if((x1-offsetx >= 0) && (x1-offsetx < dimx) && (x2-offsetx >= 0) && (x2-offsetx < dimx) && 
        (y1-offsety >= 0) && (y1-offsety< dimy) && (y2-offsety >= 0) && (y2-offsety < dimy)) {
        Uint32 col = SDL_MapRGBA(screen->format,(int)(param_bp->color.r * 255),(int)(param_bp->color.g * 255),(int)(param_bp->color.b * 255),(int)(param_bp->color.a));
        Draw_Line(screen,x1-offsetx,dimy-(y1-offsety+1),x2-offsetx,dimy-(y2-offsety+1),col);
//        SDL_UpdateRect(screen,std::min(x1-offsetx,x2-offsetx) - 1,std::min(dimy-(y1-offsety+1) - 1,dimy-(y2-offsety+1)), fabs((float)(x1-x2)) + 3,fabs((float)(y1-y2)) + 3);
      }

      SpawnpointInformationList bpSpawnpoints = param_bp->getSpawnpoints();
      for(SpawnpointInformationListIterator it = bpSpawnpoints.begin(); it != bpSpawnpoints.end(); it++) {
        // position 0 is reserved for the backwards spawnpoint
        if(((*it)->occupied) && ((*it)->position != 0)) { 
          float partner_ang = (*it)->connectedBodypart->getSpawnpointInformationForBodypart(param_bp).ang2d;
          returnvalue += req_print((*it)->connectedBodypart,x2,y2,ang + 180.0f + partner_ang + (*it)->ang2d);
        };
      };
      //std::cout << returnvalue << std::endl;
      return returnvalue;
    }
    else return 0;
  };

  bool SDL_SunlightProvider::setFactor(Bodypart* param_bodypart ,float param_factor) {
    ExtendedBodypartInformation* info = getInformation(param_bodypart);
    if(info)
      info->factor = param_factor;

    else {
      info = new ExtendedBodypartInformation();
      info->factor = param_factor;
      updateBodypartInformation(param_bodypart,info);
    };

    return true;
  };

  bool SDLOrganismPrinter::orgsAlive() {
    if(organisms.size() > 0) return true;
    return false;
  };

  bool SDLOrganismPrinter::printOutPercentage(float value) {
    Uint32 col;
    int barheight = 2;
    if(percentagePrinterCounter % 2) col = SDL_MapRGB(screen->format,255,0,0);
    else col = SDL_MapRGB(screen->format,200,200,0);
    
    while(value > 1.0f) value = value * 0.01f;
    
    Draw_FillRect(screen,0,(barheight * percentagePrinterCounter),dimx*value,barheight,col);
    SDL_UpdateRect(screen,0,(barheight * percentagePrinterCounter),dimx*value,barheight);
    percentagePrinterCounter++;
    return true;
  };
}