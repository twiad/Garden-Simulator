#include "SDLGwenOrgnismInspector.h"

namespace EDen {
	SDLGwenOrgnismInspector::SDLGwenOrgnismInspector(Gwen::Controls::Canvas* parent) {
		parentCanvas = parent;
		listBox = 0;
		org = 0;
	};

	SDLGwenOrgnismInspector::~SDLGwenOrgnismInspector() {
		hide();
	};

	void SDLGwenOrgnismInspector::setBounds(int pPosX, int pPosY, int pDimX, int pDimY) {
		posX = pPosX;
		posY = pPosY;
		dimX = pDimX;
		dimY = pDimY;

		if(isShown()) {
			listBox->SetBounds(posX,posY,dimX,dimY);
		};
	};

	void SDLGwenOrgnismInspector::show(Organism* pOrg) {
		if(isShown()) {
			hide();
		}

		org = pOrg;

		listBox = new Gwen::Controls::ListBox(parentCanvas);
		listBox->SetBounds(posX, posY, dimX, dimY);
		listBox->AddItem("No Items Yet");
		listBox->SetShouldDrawBackground(false);

		update();
	};

	void SDLGwenOrgnismInspector::hide() {
		if(listBox != 0) {
			Gwen::Controls::ListBox* oldListBox = listBox;
			listBox = 0;
			delete oldListBox;
			org = 0;
		}
	};

	bool SDLGwenOrgnismInspector::isShown() {
		return listBox != 0;
	};

	void SDLGwenOrgnismInspector::update() {
		if(isShown()) { 
			if(org == 0) {
				hide();
			}
			else {
				listBox->Clear();

				listBox->AddItem(org->getName().append(" (").append(Gwen::Utility::ToString(org->getRootBodypart()->getGeneticCode()->getSpeciesIdentifier())).append(")"))->SetTextColor(Gwen::Color(250,200,0,255));
				listBox->AddItem(Gwen::Utility::ToString("gen: ").append(Gwen::Utility::ToString(org->getRootBodypart()->getGeneticCode()->getGeneration())))->SetTextColor(Gwen::Color(250,200,0,255));
				listBox->AddItem(Gwen::Utility::ToString("fitness: ").append(Gwen::Utility::ToString(org->getFitnessValue())))->SetTextColor(Gwen::Color(250,200,0,255));
				listBox->AddItem(Gwen::Utility::ToString("bp count: ").append(Gwen::Utility::ToString(org->getBodypartCount())))->SetTextColor(Gwen::Color(250,200,0,255));
				listBox->AddItem(Gwen::Utility::ToString("lifetime: ").append(Gwen::Utility::ToString(org->getLifetime())))->SetTextColor(Gwen::Color(250,200,0,255));
			}
		}
	};

	int SDLGwenOrgnismInspector::getPosX() {
		return posX;
	};

	int SDLGwenOrgnismInspector::getPosY() {
		return posY;
	};

	int SDLGwenOrgnismInspector::getWidth() {
		return dimX;
	};

	int SDLGwenOrgnismInspector::getHeight() {
		return dimY;
	};

	void SDLGwenOrgnismInspector::setOrganism(Organism* pOrg) {
		if(!isShown()) {
			show(pOrg);
		}
		else {
			org = pOrg;
		};
	};

	Organism* SDLGwenOrgnismInspector::getOrganism() {
		return org;
	};
}