#include <fstream>
#include <time.h>
#include <thread>
#include <SDL.h>

#include "Locations.h"
#include "Miner.h"
#include "MinersWife.h"
#include "Drunkman.h"
#include "EntityManager.h"
#include "MessageDispatcher.h"
#include "misc/ConsoleUtils.h"
#include "EntityNames.h"
#include "GUI_Base.h"
#include "constantes.h"



std::ofstream os;

void EntityUpdate(BaseGameEntity* entity) {
	entity->Update();
}


int main(int argc, char ** argv)
{
	//define this to send output to a text file (see locations.h)
#ifdef TEXTOUTPUT
	os.open("output.txt");
#endif

	//seed random number generator
	srand((unsigned)time(NULL));

	//create a miner
	Miner* Bob = new Miner(ent_Miner_Bob, GoHomeAndSleepTilRested::Instance());

	//create his wife
	MinersWife* Elsa = new MinersWife(ent_Elsa, DoHouseWork::Instance());

	//create the drunk man
	Drunkman* Drunker = new Drunkman(ent_Drunkman, Drinking::Instance());

	//create a ghost, useful for some GUI function
	Miner* Ghost = new Miner(ent_ghost, GoHomeAndSleepTilRested::Instance());


	//register them with the entity manager
	EntityMgr->RegisterEntity(Bob);
	EntityMgr->RegisterEntity(Elsa);
	EntityMgr->RegisterEntity(Drunker);
	EntityMgr->RegisterEntity(Ghost);

	//LOAD GUI
	myGUI* GUI = GUI->Instance();

	while (!GetAsyncKeyState('Q')) {

		std::thread t_Bob(EntityUpdate, Bob);
		std::thread t_Elsa(EntityUpdate, Elsa);
		std::thread t_Drunker(EntityUpdate, Drunker);

		t_Bob.join();
		t_Drunker.join();
		t_Elsa.join();

		//dispatch any delayed messages
		Dispatch->DispatchDelayedMessages();

		Sleep(800);
	}

	//tidy up
	delete Bob;
	delete Elsa;
	delete Drunker;

	delete Ghost;

	//wait for a keypress before exiting
	PressAnyKeyToContinue();

	//Quit SDL subsystems
	myGUI::Instance()->quitAllSDLSystems();

	return 0;
}






