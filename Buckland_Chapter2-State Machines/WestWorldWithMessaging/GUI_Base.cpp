#include "GUI_Base.h"

#include <Windows.h>

#include "EntityManager.h"
#include "EntityNames.h"

char* FONT_PATH= "fonts/pixeled.ttf";
char* BACKGROUND_PATH= "sprites/background.bmp";
char* BOB_PATH="sprites/bob.bmp";
char* ELSA_PATH="sprites/elsa.bmp";
char* DRUNKMAN_PATH = "sprites/drunkman.bmp";
char* STATES_PATH = "sprites/states/";
const int FONT_SIZE = 10;

myGUI *myGUI::instance = nullptr;



// ------------------------------------------------------------------------------------------------
//	instanciation of the GUI
// ------------------------------------------------------------------------------------------------

// return 0 if everything is okay, -1 if a component failed to init
int myGUI::initGUI() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	if (TTF_Init() == -1)
	{
		printf("SDL_TTF could not initialize! SDL_TTF_Error: %s\n", TTF_GetError());
		return -1;
	}

	return 0;
}

void myGUI::loadGUI() {

	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::string path =std::string(buffer);
	
	
	// Change path if the program is launched from the visual studio
	if (path.substr(path.length() - 33, 7) == "\\..\\..\\") {

		

		FONT_PATH = "../../fonts/pixeled.ttf";
		BACKGROUND_PATH = "../../sprites/background.bmp";
		BOB_PATH = "../../sprites/bob.bmp";
		ELSA_PATH = "../../sprites/elsa.bmp";
		DRUNKMAN_PATH = "../../sprites/drunkman.bmp";
		STATES_PATH = "../../sprites/states/";
		
	}

	SDL_Window *window = NULL;
	font = NULL;

	if (initGUI() != -1) //if no error while initializing sdl compoments
	{
		window = SDL_CreateWindow(
			"Westworld",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN
		);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			this->window = window;

			// display blank background
			SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 255, 255, 255)); //background-color

			// display real background
			spriteBackground = SDL_LoadBMP(BACKGROUND_PATH);
			SDL_Rect backgroundPosition;
			backgroundPosition.x = 0;
			backgroundPosition.y = 0;
			SDL_BlitSurface(spriteBackground, NULL, screenSurface, &backgroundPosition);

			// display Bob in its initial position
			//SDL_Surface* spriteBob;
			spriteBob = SDL_LoadBMP(BOB_PATH);
			SDL_Rect minerPosition;
			minerPosition.x = POSITION_BOB_INIT_X;
			minerPosition.y = POSITION_BOB_INIT_Y;
			SDL_BlitSurface(spriteBob, NULL, screenSurface, &minerPosition);

			// display Elsa in its initial position
			spriteElsa = SDL_LoadBMP(ELSA_PATH);
			SDL_Rect minerWifePosition;
			minerWifePosition.x = POSITION_ELSA_INIT_X;
			minerWifePosition.y = POSITION_ELSA_INIT_Y;
			SDL_BlitSurface(spriteElsa, NULL, screenSurface, &minerWifePosition);

			// display the Drunkman in its initial position
			spriteDrunkman = SDL_LoadBMP(DRUNKMAN_PATH);
			SDL_Rect drunkmanPosition;
			drunkmanPosition.x = POSITION_DRUNKMAN_INIT_X;
			drunkmanPosition.y = POSITION_DRUNKMAN_INIT_Y;
			SDL_BlitSurface(spriteDrunkman, NULL, screenSurface, &drunkmanPosition);

			// load font
			font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
			fontColor = { 255,255,255 };

			// load states
			blitAllStates();

			// update window with all bmp sprites
			SDL_UpdateWindowSurface(window);

			// sleep for 2 seconds, time needed for the player to understand what is happening
			Sleep(2000); 

		}
	}
}

void myGUI::quitAllSDLSystems()
{
	TTF_CloseFont(font);
	freeAllSurfaces();
	TTF_Quit();
	SDL_Quit();
}


// ------------------------------------------------------------------------------------------------
//	surfaces management
// ------------------------------------------------------------------------------------------------

void myGUI::freeAllSurfaces()
{
	SDL_FreeSurface(spriteBackground);
	SDL_FreeSurface(spriteBob);
	SDL_FreeSurface(spriteElsa);
	SDL_FreeSurface(spriteDrunkman);
}

// blit the surface again to refresh characters' positions
void myGUI::blitBackground() {
	SDL_Rect position;
	position.x = 0;
	position.y = 0;
	SDL_BlitSurface(spriteBackground, NULL, SDL_GetWindowSurface(this->window), &position);
}

void myGUI::blitMiner()
{
	SDL_Rect position = EntityManager::Instance()->GetEntityFromID(ent_Miner_Bob)->getPosition();
	SDL_BlitSurface(SDL_LoadBMP(BOB_PATH), NULL, SDL_GetWindowSurface(this->window), &position);
}

void myGUI::blitElsa()
{
	SDL_Rect position = EntityManager::Instance()->GetEntityFromID(ent_Elsa)->getPosition();
	SDL_BlitSurface(SDL_LoadBMP(ELSA_PATH), NULL, SDL_GetWindowSurface(this->window), &position);
}

void myGUI::blitDrunkman()
{
	SDL_Rect position = EntityManager::Instance()->GetEntityFromID(ent_Drunkman)->getPosition();
	SDL_BlitSurface(SDL_LoadBMP(DRUNKMAN_PATH), NULL, SDL_GetWindowSurface(this->window), &position);
}

void myGUI::blitAllCharactersBut(BaseGameEntity* npc) {
	blitBackground();
	if (npc->ID() != ent_Miner_Bob) blitMiner();
	if (npc->ID() != ent_Elsa) blitElsa();
	if (npc->ID() != ent_Drunkman) blitDrunkman();
}

SDL_Surface* myGUI::getSprite(BaseGameEntity* npc) {
	if (npc->ID() == ent_Miner_Bob) return spriteBob;
	if (npc->ID() == ent_Elsa) return spriteElsa;
	if (npc->ID() == ent_Drunkman) return spriteDrunkman;
	return nullptr;
};


// ------------------------------------------------------------------------------------------------
//	Movements management
// ------------------------------------------------------------------------------------------------

// basic function to have the NPC going down the road and then up to a building
void myGUI::downToSomewhere(BaseGameEntity* npc, int somewhereY)
{
	SDL_Surface* screenSurface = SDL_GetWindowSurface(this->window);
	SDL_Rect currentPosition = npc->getPosition();

	for (currentPosition.y; currentPosition.y < somewhereY; currentPosition.y += STEP_Y) {
		blitAllCharactersBut(npc);
		SDL_Surface* sprite = getSprite(npc);
		SDL_BlitSurface(sprite, NULL, screenSurface, &currentPosition);
		npc->setPosition(currentPosition);
		blitAllStates();
		SDL_UpdateWindowSurface(window);
		Sleep(100);
	}

	/*npc->setPosition(currentPosition);*/
}

void myGUI::upToSomewhere(BaseGameEntity* npc, int somewhereY)
{
	SDL_Surface* screenSurface = SDL_GetWindowSurface(this->window);
	SDL_Rect currentPosition = npc->getPosition();

	for (currentPosition.y; currentPosition.y > somewhereY; currentPosition.y -= STEP_Y) {
		blitAllCharactersBut(npc);
		SDL_Surface* sprite = getSprite(npc);
		SDL_BlitSurface(sprite, NULL, screenSurface, &currentPosition);
		npc->setPosition(currentPosition);
		blitAllStates();
		SDL_UpdateWindowSurface(window);
		Sleep(100);
	}

	/*npc->setPosition(currentPosition);*/
}

// basic function to move a character on the road to a specified location
void myGUI::goLeft(BaseGameEntity* npc) {
	SDL_Surface* screenSurface = SDL_GetWindowSurface(this->window);
	SDL_Rect currentPosition = npc->getPosition();

	currentPosition.x -= STEP_X;
	blitAllCharactersBut(npc);
	SDL_Surface* sprite = getSprite(npc);
	SDL_BlitSurface(sprite, NULL, screenSurface, &currentPosition);
	npc->setPosition(currentPosition);
	blitAllStates();
	SDL_UpdateWindowSurface(window);
	Sleep(PACE);

	/*npc->setPosition(currentPosition);*/
};

void myGUI::goRight(BaseGameEntity* npc) {
	SDL_Surface* screenSurface = SDL_GetWindowSurface(this->window);
	SDL_Rect currentPosition = npc->getPosition();

	currentPosition.x += STEP_X;
	blitAllCharactersBut(npc);
	SDL_Surface* sprite = getSprite(npc);
	SDL_BlitSurface(sprite, NULL, screenSurface, &currentPosition);
	npc->setPosition(currentPosition);
	blitAllStates();
	SDL_UpdateWindowSurface(window);
	Sleep(PACE);

	//npc->setPosition(currentPosition);
}

void myGUI::HToSomewhere(BaseGameEntity* npc, int somewhereX)
{
	SDL_Surface* screenSurface = SDL_GetWindowSurface(this->window);
	SDL_Rect currentPosition = npc->getPosition();

	if (npc->getPosition().x < somewhereX)
		while (npc->getPosition().x < somewhereX) goRight(npc);
	else
		while (npc->getPosition().x > somewhereX) goLeft(npc);
}

// General function, using basic functions describe above
void myGUI::goToSomewhere(BaseGameEntity* npc, int somewhereX, int somewhereY) {
	downToSomewhere(npc, ROAD_Y);
	HToSomewhere(npc, somewhereX);
	upToSomewhere(npc, somewhereY);
}


// ------------------------------------------------------------------------------------------------
// Messages management
// ------------------------------------------------------------------------------------------------

void myGUI::speak(BaseGameEntity* npc, char* text, SDL_Color fontColor)
{
	//put the text to upper case
	char* upperText = new char[strlen(text)+1];
	strcpy(upperText, text);

	//modify char one by one to have them capitalized
	char c;
	int i=0;
	while (c = upperText[i])
	{
		upperText[i] = toupper(c);
		i++;
	}

	// create a surface containing only the text
	int h, w;
	SDL_Surface* textSurface = TTF_RenderText_Blended(this->font, upperText, fontColor);

	//create the black background around the text
	h = textSurface->h + 10;
	w = textSurface->w + 10;
	SDL_Surface* border = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
	SDL_FillRect(border, NULL, SDL_MapRGB(SDL_GetWindowSurface(this->window)->format, 0, 0, 0));
	// calculations will not be detailed but align the border rectangle so it is centered with the character sprite
	SDL_Rect borderPos;
	borderPos.x = npc->getPosition().x - (((textSurface->w + 10) / 2) - 15);
	borderPos.y = npc->getPosition().y - (textSurface->h + 12);

	SDL_BlitSurface(border, NULL, SDL_GetWindowSurface(this->window), &borderPos);


	// create position for the text and blit the surface
	SDL_Rect textPos;
	textPos.x = npc->getPosition().x - ((textSurface->w / 2) - 15);
	textPos.y = npc->getPosition().y - (textSurface->h + 7);
	SDL_BlitSurface(textSurface, NULL, SDL_GetWindowSurface(this->window), &textPos);
	//blitAllStates();

	//update surface
	SDL_UpdateWindowSurface(window);
	Sleep(MESSAGE_DURATION);

	// erase message after some sort of timeout
	blitAllCharactersBut(EntityManager::Instance()->GetEntityFromID(ent_ghost));
	blitAllStates();
	SDL_UpdateWindowSurface(window);
}


// ------------------------------------------------------------------------------------------------
// States management
// ------------------------------------------------------------------------------------------------

void myGUI::blitState(BaseGameEntity* npc)
{
	std::string path = STATES_PATH + npc->getCurrentStateName() + ".bmp";

	const char* statePath = path.c_str();

	SDL_Rect position = npc->getPosition();
	position.x += 4; //so the icon is centered above the character's head

	SDL_BlitSurface(SDL_LoadBMP(statePath), NULL, SDL_GetWindowSurface(this->window), &position);
}

void myGUI::blitAllStates()
{
	blitState(EntityManager::Instance()->GetEntityFromID(ent_Miner_Bob));
	blitState(EntityManager::Instance()->GetEntityFromID(ent_Elsa));
	blitState(EntityManager::Instance()->GetEntityFromID(ent_Drunkman));
}