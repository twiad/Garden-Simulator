#include "SDLGwenStatusWindow.h"

#define STATUS_WINDOW_DIM_X 200
#define STATUS_WINDOW_DIM_Y 400

namespace EDen {
	SDLGwenStatusWindow::SDLGwenStatusWindow(RuntimeManager* p_runtime)
	{
		runtime = p_runtime;

		window = SDL_CreateWindow("Species Overview", 100, 100, STATUS_WINDOW_DIM_X, STATUS_WINDOW_DIM_Y, SDL_WINDOW_SHOWN);
	
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

		listBox = new Gwen::Controls::ListBox(pCanvas);
		listBox->SetBounds(3, 3, STATUS_WINDOW_DIM_X - 6, STATUS_WINDOW_DIM_Y - 6);
		listBox->AddItem("No Items Yet");
	}

	SDLGwenStatusWindow::~SDLGwenStatusWindow()
	{
		SDL_DestroyWindow(window);
	}

	void SDLGwenStatusWindow::processEvent(SDL_Event* evt) {
		GwenInput->ProcessEvent(evt);
	}

	void SDLGwenStatusWindow::update() {
		listBox->Clear();

		//listBox->AddItem(runtime->getDebugOut(true));

		std::list<Groundpart*> groundparts = runtime->getGroundparts();
		for(std::list<Groundpart* >::iterator it = groundparts.begin(); it != groundparts.end(); it++) {
			//listBox->AddItem((*it)->getName());

			std::list<std::string> outputs;
			(*it)->getSpeciesDatabase()->getDebugOut(&outputs);

			for(std::list<std::string>::iterator itOutputs = outputs.begin(); itOutputs != outputs.end(); itOutputs++) {
				listBox->AddItem(*itOutputs);
			}
		};

		pRenderer->BeginContext(NULL);
		pCanvas->RenderCanvas();
		pRenderer->PresentContext(NULL);
		pRenderer->EndContext(NULL);
	}
}