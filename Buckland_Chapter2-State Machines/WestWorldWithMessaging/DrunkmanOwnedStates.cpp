#include "DrunkmanOwnedStates.h"
#include "fsm/State.h"
#include "Drunkman.h"
#include "Locations.h"
#include "messaging/Telegram.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"
#include "Time/CrudeTimer.h"
#include "EntityNames.h"
#include "GUI_Base.h"

#include <iostream>
using std::cout;

#ifdef TEXTOUTPUT
#include <fstream>
extern std::ofstream os;
#define cout os
#endif

// above, the drunk man fell in the coma
const int ETHYLIC_COMA_LEVEL = 10;

//------------------------------------------------------------------------- Drinking

Drinking* Drinking::Instance()
{
	static Drinking instance;

	return &instance;
}

void Drinking::Enter(Drunkman* drunkman)
{
	printMsg(drunkman,GetNameOfEntity(drunkman->ID()) + ": Time to drink a bit!", 2);
	//myGUI::Instance()->speak(drunkman, "Time to drink a bit!");
}


void Drinking::Execute(Drunkman* drunkman)
{
	switch (RandInt(0, 2))
	{
	case 0:
		printMsg(drunkman,GetNameOfEntity(drunkman->ID()) + ": Let's have a whisky!", 2);
		//myGUI::Instance()->speak(drunkman, "Let's have a whisky!");
		break;

	case 1:

		printMsg(drunkman,GetNameOfEntity(drunkman->ID()) + ": Imma fucking pirate! Wanta some rhum yoho!", 2);
		//myGUI::Instance()->speak(drunkman, "Imma fucking pirate! Wanta some rhum yoho!");
		break;

	case 2:

		printMsg(drunkman,GetNameOfEntity(drunkman->ID()) + ": How about a nice fresh beer?", 2);
		//myGUI::Instance()->speak(drunkman, "How about a nice fresh beer?");
		break;
	}
	drunkman->InscreaseDrunkenness();

	//if too drunk, the drunkman fell in the coma til his life is full again
	if (drunkman->getDrunkenness() > ETHYLIC_COMA_LEVEL) {
		printMsg(drunkman,GetNameOfEntity(drunkman->ID()) + ": Maybe dat was too much -hips", 2);
		//myGUI::Instance()->speak(drunkman, "Maybe dat was too much -hips");
		drunkman->GetFSM()->ChangeState(KO::Instance());
	}
}

void Drinking::Exit(Drunkman* drunkman) {}

bool Drinking::OnMessage(Drunkman* drunkman, const Telegram& msg)
{
	switch (msg.Msg) {
	case Msg_EnteringTheSaloon:
		std::ostringstream time;
		time << Clock->GetCurrentTime();
		printMsg(drunkman,"Message handled by " + GetNameOfEntity(drunkman->ID()) + " at time : " + time.str(), 3);

		//taunt Bob
		printMsg(drunkman,GetNameOfEntity(drunkman->ID()) + ": Wataya want stranger? Wanna fight?", 2);
		//myGUI::Instance()->speak(drunkman, "Wataya want stranger? Wanna fight?");

		Dispatch->DispatchMessage(
			SEND_MSG_IMMEDIATELY,	//time delay
			drunkman->ID(),			//ID of sender
			ent_Miner_Bob,			//ID of recipient
			Msg_TauntingBob,		//the message
			NO_ADDITIONAL_INFO);


		// Start a fight with Bob
		drunkman->GetFSM()->ChangeState(DrukmanFighting::Instance());

		return true;
	}
	return false;
}

//------------------------------------------------------------------------- Fighting

DrukmanFighting* DrukmanFighting::Instance()
{
	static DrukmanFighting instance;

	return &instance;
}

void DrukmanFighting::Enter(Drunkman* drunkman)
{
	printMsg(drunkman,GetNameOfEntity(drunkman->ID()) + ": Got a problem with me?!", 2);
	//myGUI::Instance()->speak(drunkman, "Got a problem with me?!");
}


void DrukmanFighting::Execute(Drunkman* drunkman)
{
	printMsg(drunkman,GetNameOfEntity(drunkman->ID()) + ": Take this! And that!", 2);
	//myGUI::Instance()->speak(drunkman, "Take this! And that!");

	int prob = 8 * (ETHYLIC_COMA_LEVEL - drunkman->getDrunkenness()) + 20;

	if (RandInt(0, 100) > prob) {

		//lose the fight
		printMsg(drunkman,GetNameOfEntity(drunkman->ID()) + ": Where is my bloody tooth? Not feelin' dat well...", 2);
		//myGUI::Instance()->speak(drunkman, "Where is my bloody tooth? Not feelin' dat well...");


		//global message "Entering the saloon"
		Dispatch->DispatchMessage(
			SEND_MSG_IMMEDIATELY,	 //time delay
			drunkman->ID(),			 //ID of sender
			ent_Miner_Bob,           //ID of recipient
			Msg_BobWins,   //the message
			NO_ADDITIONAL_INFO
		);

		drunkman->GetFSM()->ChangeState(KO::Instance());
	}
	else {
		//win the fight
		printMsg(drunkman,GetNameOfEntity(drunkman->ID()) + ": Yeah that's what happen' when someone messes with me!", 2);
		//myGUI::Instance()->speak(drunkman, "Yeah that's what happen' when someone messes with me!");



		//global message "Entering the saloon"
		Dispatch->DispatchMessage(
			SEND_MSG_IMMEDIATELY,	 //time delay
			drunkman->ID(),			 //ID of sender
			ent_Miner_Bob,           //ID of recipient
			Msg_BobLoses,			 //the message
			NO_ADDITIONAL_INFO
		);

		drunkman->GetFSM()->ChangeState(Drinking::Instance());
	}
}

void DrukmanFighting::Exit(Drunkman* drunkman) {}

bool DrukmanFighting::OnMessage(Drunkman* drunkman, const Telegram& msg)
{
	return false;
}

//------------------------------------------------------------------------- KO

KO* KO::Instance()
{
	static KO instance;

	return &instance;
}

void KO::Enter(Drunkman* drunkman)
{
	printMsg(drunkman,GetNameOfEntity(drunkman->ID()) + " - falls loudly on the floor -", 2);
	//myGUI::Instance()->speak(drunkman, "*falls loudly on the floor*");
	drunkman->knockDown();
}


void KO::Execute(Drunkman* drunkman)
{
	drunkman->rest();
	//rest until his health is back to max
	if (drunkman->getHealth() < HEALTH_MAX) {
		printMsg(drunkman,GetNameOfEntity(drunkman->ID()) + " - Still KO, resting -", 2);
		//myGUI::Instance()->speak(drunkman, "*Still KO, resting*");
	}
	else {
		drunkman->GetFSM()->ChangeState(Drinking::Instance());
	}
}

void KO::Exit(Drunkman* drunkman)
{
	printMsg(drunkman,GetNameOfEntity(drunkman->ID()) + " - singing - : I got a hangover oooooooooh I've been drinking too much for suuuuuure!", 2);
	//myGUI::Instance()->speak(drunkman, "*singing* I got a hangover oooooooooh I've been drinking too much for suuuuuure!");
}

bool KO::OnMessage(Drunkman* drunkman, const Telegram& msg)
{
	std::ostringstream time;
	time << Clock->GetCurrentTime();
	printMsg(drunkman,"Message handled by " + GetNameOfEntity(drunkman->ID()) + " at time : " + time.str() + "\n/ There is not point talking to a man lying drunk dead on the floor. /", 3);
	return true;
}
