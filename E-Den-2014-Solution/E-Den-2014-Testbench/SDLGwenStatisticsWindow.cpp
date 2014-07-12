#include "SDLGwenStatisticsWindow.h"

#define STATISTICS_WINDOW_DIM_X 500
#define STATISTICS_WINDOW_DIM_Y 400
#define STATISTICS_DATA_MAX_ENTIRES_STANDARD 500

namespace EDen {
	SDLGwenStatisticsWindow::SDLGwenStatisticsWindow(RuntimeManager* p_runtime)
	{
		runtime = p_runtime;

		window = 0;
		pRenderer = 0;
		pCanvas = 0;
		GwenInput = 0;

		dimx = STATISTICS_WINDOW_DIM_X;
		dimy = STATISTICS_WINDOW_DIM_Y;
		posx = SDL_WINDOWPOS_UNDEFINED;
		posy = SDL_WINDOWPOS_UNDEFINED;

		Gwen::Color* col;

		//col = &data["AvgBodypartCount"].col;
		//col->a = 255;
		//col->r = 255;
		//col->g = 200;
		//col->b = 0;

		col = &data["BodypartCount"].col;
		col->a = 255;
		col->r = 120;
		col->g = 120;
		col->b = 120;

		//col = &data["Random"].col;
		//col->a = 255;
		//col->r = 120;
		//col->g = 120;
		//col->b = 0;
	};


	SDLGwenStatisticsWindow::~SDLGwenStatisticsWindow()
	{
		if (isShown())
		{
			hide();
		}
	};

	int SDLGwenStatisticsWindow::getSDLWindowID() {
		return SDL_GetWindowID(window);
	};

	void SDLGwenStatisticsWindow::processEvent(SDL_Event* evt) {
		if(window != 0) {
			if(evt->window.windowID == SDL_GetWindowID(window)) {
				GwenInput->ProcessEvent(evt);
				if ((evt->type == SDL_WINDOWEVENT)) {
					if(evt->window.event == SDL_WINDOWEVENT_RESIZED) {
						resizeWindow(evt->window.data1, evt->window.data2);
					};
				};
			};
		};
	};

    void SDLGwenStatisticsWindow::resizeWindow(int pDimx, int pDimy) {
		dimx = pDimx;
		dimy = pDimy;

		pCanvas->SetSize(dimx,dimy);
	};

	void SDLGwenStatisticsWindow::update() {
		//addDataPoint("AvgBodypartCount",runtime->getBodypartCount() / (double)runtime->getOrganisms().size());
		addDataPoint("BodypartCount",runtime->getBodypartCount());
		//addDataPoint("Random",Randomizer::value());
		addDataPoint("AvgLifetime", runtime->getAvgLifetime());
		//for(std::list<Groundpart*>::iterator itGp = runtime->getGroundparts().begin(); itGp != runtime->getGroundparts().end(); itGp++) {

		//}

		if(window != 0) {
			pRenderer->BeginContext(NULL);
			pCanvas->RenderCanvas();

			int numVisibleValues = 0;
			
			for(std::map<std::string,DataStorageEntry>::iterator it = data.begin(); it != data.end(); it++) {
				if(it->second.visible) {
					numVisibleValues += 1;
				};
			};

			if(numVisibleValues > 0) {
				int currentViewport = 1;

				for(std::map<std::string,DataStorageEntry>::iterator it = data.begin(); it != data.end(); it++) {
					if(it->second.visible) {
						double minValue,maxValue;
						bool firstDataset = true;

						for(std::list<double>::iterator itData = it->second.data.begin(); itData != it->second.data.end(); itData++) {
							if(firstDataset) {
								minValue = *itData;
								maxValue = *itData;

								firstDataset = false;
							}
							else {
								if(minValue > *itData) minValue = *itData;
								if(maxValue < *itData) maxValue = *itData;
							}
						}

						double dataOffset = minValue;
						double dataScale;
						if(minValue != maxValue) {
							dataScale = (dimy - 20) /(maxValue - minValue);
						}
						else {
							dataScale = 0;
						}

						int cursor = 0;
						int maxDataValues = dimx / numVisibleValues;
						int startX = currentViewport * maxDataValues;
						
						if(it->second.maxValues != maxDataValues) it->second.maxValues = maxDataValues;

						double lastValue;
						for(std::list<double>::iterator itData = it->second.data.begin(); itData != it->second.data.end(); itData++) {
							if(cursor > maxDataValues) break;

							if(dataScale >= 0) {
								pRenderer->SetDrawColor(it->second.col);
								int y = (dimy - 10) - (*itData - dataOffset) * dataScale;
								pRenderer->DrawPixel(startX - cursor, y);

								if(cursor == 0) {
									lastValue = *itData;
								}
								else {
									if(lastValue != *itData) {
										int lastY = (dimy - 10) - (lastValue - dataOffset) * dataScale;
										pRenderer->DrawFilledRect(Gwen::Rect(startX - cursor, lastY, 1, y - lastY + 1));
										lastValue = *itData;
									}
								}
							}
							cursor++;
						}


						Gwen::Font* font = pCanvas->GetSkin()->GetDefaultFont();
						if(!font->data) {
							pRenderer->LoadFont(font);
						}

						if(font->data) {
						  pRenderer->RenderText(font, Gwen::Point(startX - 30,0),Gwen::Utility::StringToUnicode(Gwen::Utility::ToString(maxValue)));
						  pRenderer->RenderText(font, Gwen::Point(startX - 30,dimy - 10),Gwen::Utility::StringToUnicode(Gwen::Utility::ToString(minValue)));
						  pRenderer->RenderText(font, Gwen::Point(startX - ((maxDataValues*2)/3),dimy - 10),Gwen::Utility::StringToUnicode(it->first));
						}

						currentViewport++;
					};
				};
			}

			pRenderer->PresentContext(NULL);
			pRenderer->EndContext(NULL);
		}
	};

	void SDLGwenStatisticsWindow::show() {
		if(window == 0) {
			window = SDL_CreateWindow("Stats", posx, posy, dimx, dimy, SDL_WINDOW_RESIZABLE);
	
			if (!window) {
				return;
			}

			pRenderer = new Gwen::Renderer::SDL2(window);

			Gwen::Skin::TexturedBase* skin = new Gwen::Skin::TexturedBase(pRenderer);
			skin->SetRender(pRenderer);
			skin->Init(L"DefaultSkin.png");
			skin->SetDefaultFont(L"OpenSans.ttf", 8);

			pCanvas = new Gwen::Controls::Canvas(skin);
			pCanvas->SetSize(dimx, dimy);
			pCanvas->SetDrawBackground(true);
			pCanvas->SetBackgroundColor(Gwen::Color(0, 0, 0, 255));

			GwenInput = new Gwen::Input::SDL2();
			GwenInput->Initialize(pCanvas);
		}
	};

	void SDLGwenStatisticsWindow::hide() {
		if(window != 0) {
			SDL_GetWindowPosition(window,&posx,&posy);
			SDL_DestroyWindow(window);
			window = 0;
			pRenderer = 0;
			pCanvas = 0;
			GwenInput = 0;
		}
	};

	bool SDLGwenStatisticsWindow::isShown() {
		return window != 0;
	};

	void SDLGwenStatisticsWindow::addDataPoint(std::string identifier, double value) {
		DataStorageEntry* entrie = &data[identifier];
		entrie->data.push_front(value);

		if(entrie->maxValues > 0) {
			while(entrie->maxValues < entrie->data.size()) {
				entrie->data.pop_back();
			}
		}
		else {
			while(STATISTICS_DATA_MAX_ENTIRES_STANDARD < entrie->data.size()) {
				entrie->data.pop_back();
			}
		}
	};


}
