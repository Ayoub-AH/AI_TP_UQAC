#ifndef GUI_BASE
#define GUI_BASE

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "misc/ConsoleUtils.h"
#include "misc/Utils.h"
#include "constantes.h"
#include "BaseGameEntity.h"


class myGUI
{
private:

	TTF_Font* font;
	SDL_Color fontColor;

	static myGUI* instance;

	myGUI() { loadGUI(); };

	~myGUI() {};

	myGUI (const myGUI&){}
	myGUI& operator= (const myGUI&){}

	

	int initGUI();
	void loadGUI();

public:
	void quitAllSDLSystems();

	SDL_Window* window;
	SDL_Surface *spriteBackground, *spriteBob, *spriteElsa, *spriteDrunkman;

	BaseGameEntity *bob, *elsa, *drunkman;
	

	static myGUI* Instance() {
		if (!instance) {
			instance = new myGUI;
		}
		return instance;
	};

	// surfaces management
	void freeAllSurfaces();

	void blitBackground();

	void blitMiner();
	void blitElsa();
	void blitDrunkman();

	void blitAllCharactersBut(BaseGameEntity*);

	SDL_Surface* getSprite(BaseGameEntity*);

	// basic movements functions
	void goLeft(BaseGameEntity*);
	void goRight(BaseGameEntity*);
	void downToSomewhere(BaseGameEntity*, int);
	void upToSomewhere(BaseGameEntity*, int);
	void HToSomewhere(BaseGameEntity*, int);
	void goToSomewhere(BaseGameEntity*, int, int);

	// functions in charge for message display	
	void speak(BaseGameEntity*, char*, SDL_Color);

	//functions to display states icons
	void myGUI::blitState(BaseGameEntity*);
	void myGUI::blitAllStates();
};

#endif
#pragma once
