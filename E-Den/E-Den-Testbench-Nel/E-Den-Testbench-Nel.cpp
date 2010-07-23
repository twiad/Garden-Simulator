// E-Den-Testbench-Nel.cpp
// test the 3d-binding of e-den
// Partly copied from nel shapeviewer example which is under "GNU Affero General Public License"

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

using namespace NLMISC;
using namespace NL3D;
using namespace EDen;

int _tmain(int argc, _TCHAR* argv[])
{
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

    NELOrganismPrinter* printer = new NELOrganismPrinter(Scene);

    std::vector<UInstance> Entities;

		// initial angle
		float angle = 0.f;

    printer->print();

		// main loop
		while (Driver->isActive())
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

