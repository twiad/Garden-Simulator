// E-Den-Testbench-Nel.cpp
// test the 3d-binding of e-den
// Partly copied from nel shapeviewer example which is under "GNU Affero General Public License"

#define MAX_WATER 2.0e9
#define STARTING_WATER 1.7e9
#define MAX_GOO 2.0e9
#define STARTING_GOO 1.7e9


#include "stdafx.h"

#include <nel/misc/path.h>
#include <nel/misc/file.h>
#include <nel/misc/common.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_light.h>
#include <nel/3d/u_camera.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_instance.h>

#include "nelOrganismPrinter.h"

#include "shlobj.h"
#include "Shlwapi.h"

using namespace NLMISC;
using namespace NL3D;
using namespace EDen;

TCHAR appSettingsPath[MAX_PATH];
char appSettingsPathP[MAX_PATH];

int _tmain(int argc, _TCHAR* argv[])
{
  SHGetSpecialFolderPath(0,appSettingsPath,CSIDL_APPDATA,1);
  PathAppend(appSettingsPath,TEXT("EDen"));
  wcstombs(appSettingsPathP,appSettingsPath,MAX_PATH);

  try
	{
		// create OpenGL driver
		NL3D::UDriver *Driver = UDriver::createDriver();
		if (!Driver) throw 2;
		Driver->setDisplay(UDriver::CMode(800, 600, 32, true));
		Driver->setWindowTitle(ucstring("E-Den-Testbench-NEL"));

		// can use both dds and tga textures for shapes
		CPath::remapExtension ("dds", "tga", true);

		// create the light
		ULight *Light = ULight::createLight();
		if (!Light) throw 3;
		Light->setMode(ULight::DirectionalLight);
		Light->setPosition(CVector(-20.f, 30.f, 10.f));
  	Light->setAmbiant(CRGBA(255, 255, 255));

    Driver->setLight(0, *Light);
		Driver->enableLight(0);

		// Create a scene
		NL3D::UScene *Scene = Driver->createScene(true);
		if (!Scene) throw 4;

		// create the camera
		UCamera Camera = Scene->getCam();
		if (Camera.empty()) throw 5;
		Camera.setTransformMode (UTransformable::DirectMatrix);
		Camera.setPerspective ((float)Pi/2.f, 1.33f, 0.1f, 1000);

		Camera.lookAt (CVector(-10.f, 10.f, 0.f), CVector(0.f, 0.f, 0.f));

    /////////////////////
    RuntimeManager* runtime = new RuntimeManager();
    Groundpart* gp = new Groundpart(MAX_WATER,MAX_GOO);
    runtime->add(gp);

    NELOrganismPrinter* printer = new NELOrganismPrinter(Scene,runtime);
    runtime->add(printer);

    Bodypart* bp,* bp2,* bp3;
    bp = new Bodypart(BPT_Stick,"TESTPART3");
    Organism* organism = new Organism("TestOrganism2", bp, runtime);
    organism->connectToGoundpart(gp);
    runtime->add(organism);
    runtime->registerBodypart(bp);
    bp->setScaleModifier(1.0f);
    bp2 = new Bodypart(BPT_Stick,"TESTPART3",organism);
    bp->occupieSpawnpoint(bp2);
    runtime->registerBodypart(bp2);
    //bp3 = new Bodypart(BPT_Leaf,"TESTPART3",organism);
    bp->getChemicalStorage()->add("Energie",100.0f);
    bp2->getChemicalStorage()->add("Energie",100.0f);
    //bp3->getChemicalStorage()->add("Energie",10.0f);
    //bp3->getChemicalStorage()->add("Sonne",200.0f);

    gp->getChemicalStorage()->add("Wasser",STARTING_WATER);
    gp->getChemicalStorage()->add("Goo",STARTING_GOO);

    //runtime->initDatabase(appSettingsPathP);
    //runtime->loadDatabase("autosave.xml");
    ////////////////////

    std::vector<UInstance> Entities;

		// initial angle
		float angle = 0.f;

    runtime->update();
    printer->print();

		// main loop
		while (Driver->isActive() && runtime->orgsAlive())
		{
			Driver->EventServer.pump();
			Driver->clearBuffers(CRGBA(0, 0, 0));
      
      printer->print();

      // animate the scene
			Scene->animate(NLMISC::CTime::getLocalTime() / 1000.0);
			Scene->render();
			// show the scene
			Driver->swapBuffers();

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

		// delete all entities
		for(size_t i = 0; i < Entities.size(); ++i)
		{
			Scene->deleteInstance(Entities[i]);
		}

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

