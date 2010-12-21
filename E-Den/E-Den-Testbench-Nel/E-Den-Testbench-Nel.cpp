// E-Den-Testbench-Nel.cpp
// test the 3d-binding of e-den
// Partly copied from nel shapeviewer example which is under "GNU Affero General Public License"

#define MAX_WATER 2.0e9
#define STARTING_WATER 1.9e9
#define MAX_GOO 2.0e9
#define STARTING_GOO 1.7e9
#define INITIAL_MUTATION_COUNT 50
#define INITIAL_MUTATION_RATE 2.0f

// -.-
#define PI 3.1415926536

#include "stdafx.h"

#include <nel/misc/path.h>
#include <nel/misc/file.h>
#include <nel/misc/common.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_light.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_instance.h>
#include <math.h>

#include "nelOrganismPrinter.h"

#include "shlobj.h"
#include "Shlwapi.h"

using namespace NLMISC;
using namespace NL3D;
using namespace EDen;

TCHAR appSettingsPath[MAX_PATH];
char appSettingsPathP[MAX_PATH];

Organism* generateProtoplant(RuntimeManager* runtime, PlantEntityManager* gp)
{
  Bodypart* bp,* bp2;
  bp = new Bodypart(BPT_Stick,"TESTPART3");

  Organism* organism = new Organism("TestOrganism2", bp, runtime);
  organism->connectToGoundpart(gp);
  runtime->add(organism);
  //runtime->registerBodypart(bp);
  bp->setScaleModifier(1.0f);
  bp2 = new Bodypart(BPT_Stick,"TESTPART3",organism);
  for(int i = 0; i < INITIAL_MUTATION_COUNT; i++)
    bp2->getGeneticCode()->mutate(INITIAL_MUTATION_RATE);

  bp->occupieSpawnpoint(bp2);
  runtime->registerBodypart(bp2);
  //bp3 = new Bodypart(BPT_Leaf,"TESTPART3",organism);
  bp->getChemicalStorage()->add("Energie",100.0f);
  bp2->getChemicalStorage()->add("Energie",100.0f);
  //bp3->getChemicalStorage()->add("Energie",10.0f);
  //bp3->getChemicalStorage()->add("Sonne",200.0f);

  return organism;
}

int _tmain(int argc, _TCHAR* argv[])
{
  SHGetSpecialFolderPath(0,appSettingsPath,CSIDL_APPDATA,1);
  PathAppend(appSettingsPath,TEXT("EDen"));
  wcstombs(appSettingsPathP,appSettingsPath,MAX_PATH);

  try
	{
		// create driver
		NL3D::UDriver *Driver = UDriver::createDriver(0,false);
		if (!Driver) throw 2;
		Driver->setDisplay(UDriver::CMode(800, 600, 32, true,0,4));
		Driver->setWindowTitle(ucstring("E-Den-Testbench-NEL"));


		// can use both dds and tga textures for shapes
		CPath::remapExtension ("dds", "tga", true);

		// create the lights
		ULight *Light = ULight::createLight();
		if (!Light) throw 3;
		Light->setMode(ULight::DirectionalLight);
		Light->setPosition(CVector(60.f, 30.f, 40.f));
  	Light->setAmbiant(CRGBA(200, 200, 200));

    Driver->setLight(0, *Light);
		Driver->enableLight(0);

    Light = ULight::createLight();
		if (!Light) throw 3;
		Light->setMode(ULight::DirectionalLight);
		Light->setPosition(CVector(10.f, 40.f, 80.f));
  	Light->setAmbiant(CRGBA(180, 180, 180));

    Driver->setLight(1, *Light);
		Driver->enableLight(1);

    Light = ULight::createLight();
		if (!Light) throw 3;
		Light->setMode(ULight::DirectionalLight);
		Light->setPosition(CVector(60.f, 30.f, -40.f));
  	Light->setAmbiant(CRGBA(200, 200, 200));

    Driver->setLight(2, *Light);
		Driver->enableLight(2);

		// Create a scene
		NL3D::UScene *Scene = Driver->createScene(true);
		if (!Scene) throw 4;

		// create the camera
		UCamera Camera = Scene->getCam();
		if (Camera.empty()) throw 5;
		Camera.setTransformMode (UTransformable::DirectMatrix);
		Camera.setPerspective ((float)Pi/2.f, 1.33f, 0.1f, 1000);

    CVector cameraStart = CVector(130.f,0.f,50.f);
    CVector cameraPosition = cameraStart;
    CVector cameraLookAt = CVector(0.f,0.f,5.f);
		Camera.lookAt (cameraPosition, cameraLookAt);

    /////////////////////
    RuntimeManager* runtime = new RuntimeManager();
    
    PlantEntityManager* gp = new PlantEntityManager(CVector(-50.0f,-50.0f,0.0f),100.0f,100.0f,MAX_WATER,MAX_GOO);
    runtime->add(gp);

    NELOrganismPrinter* printer = new NELOrganismPrinter(Scene,runtime,gp);
    runtime->add(printer);

    generateProtoplant(runtime, gp);

    gp->getChemicalStorage()->add("Wasser",STARTING_WATER);
    gp->getChemicalStorage()->add("Goo",STARTING_GOO);

    runtime->initDatabase(appSettingsPathP);
    runtime->loadDatabase("autosave.xml");
    ////////////////////

    // camera rotation
    float angle = 1.f;
    float cameraRotationSpeed = 0.0007f;

    runtime->update();
    printer->print();

		// main loop
		while (Driver->isActive() && runtime->orgsAlive())
		{
			Driver->EventServer.pump();
      
      runtime->update();
      
      angle += cameraRotationSpeed;
      while(angle >= 2 * PI) angle -= 2 * PI;
      cameraPosition.x = cameraStart.x * sin(angle) + cameraLookAt.x;
      cameraPosition.y = cameraStart.x * cos(angle) + cameraLookAt.y;
      Camera.lookAt (cameraPosition, cameraLookAt);

      if(!runtime->orgsAlive())
        generateProtoplant(runtime, gp);
      if((runtime->getCycleCount() % 10) == 0) {
        Driver->setWindowTitle(ucstring(runtime->getDebugOut(true)));
      }
      if((runtime->getCycleCount() % 500) == 0) {
        if((runtime->getCps() > 40) && ((int)runtime->getPreferedOrganismCount() <= runtime->getOrganismCount()))
          runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() + 1);
        if((runtime->getCps() < 23) && ((int)runtime->getPreferedOrganismCount() >= runtime->getOrganismCount()) && (runtime->getPreferedOrganismCount() > 3))
          runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() - 1);
        if((runtime->getCps() < 10))
          runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount(),true);
        
      };
      

      printer->print();

      int cps = runtime->getCps();
      if((cps < 59) || 
        ((cps < 120) && ((runtime->getCycleCount() % 3) == 0)) ||
        ((cps < 300) && ((runtime->getCycleCount() % 10) == 0)) ||
        ((runtime->getCycleCount() % 25) == 0) ) {
        Driver->clearBuffers(CRGBA(0, 0, 0));
        
        // animate the scene  
			  Scene->animate(NLMISC::CTime::getLocalTime() / 1000.0);
			  Scene->render();
			  // show the scene
			  Driver->swapBuffers();
      }
			// escape will leave the program
			if (Driver->AsyncListener.isKeyPushed(KeyESCAPE))
			{
				break;
			}
			// F3 will change the render mode
			else if (Driver->AsyncListener.isKeyPushed(KeyF3))
			{
				UDriver::TPolygonMode p = Driver->getPolygonMode();
				p = UDriver::TPolygonMode(((int)p+1)%3);
				Driver->setPolygonMode(p);
			}
      else if (Driver->AsyncListener.isKeyPushed(KeyF5))
			{
				runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() - 1); 
			}
      else if (Driver->AsyncListener.isKeyPushed(KeyF6))
			{
				runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount() + 1); 
			}
      else if (Driver->AsyncListener.isKeyPushed(KeyF9))
			{
				cameraRotationSpeed = cameraRotationSpeed * 0.5f; 
			}
      else if (Driver->AsyncListener.isKeyPushed(KeyF10))
			{
				cameraRotationSpeed = cameraRotationSpeed * 2.0f; 
			}
      else if (Driver->AsyncListener.isKeyPushed(KeyC))
			{
				generateProtoplant(runtime, gp);
			}
      else if (Driver->AsyncListener.isKeyPushed(KeyX))
			{
        runtime->setPreferedOrganismCount(runtime->getPreferedOrganismCount(),true);
      }
			// F12 will take a screenshot
			else if (Driver->AsyncListener.isKeyPushed(KeyF12))
			{
				CBitmap btm;
				Driver->getBuffer(btm);
				COFile fs(CFile::findNewFile("screenshot.png"));
				btm.writePNG(fs,24);
			}
		}

  	// we are leaving the program

		// delete the scene
		Driver->deleteScene(Scene);

		// delete the light
		delete Light;

		// release all textures and others elements
		Driver->release();

		// delete the driver
		delete Driver;
	}
	catch(int a)
	{
		return a;
	}
	catch(...)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

