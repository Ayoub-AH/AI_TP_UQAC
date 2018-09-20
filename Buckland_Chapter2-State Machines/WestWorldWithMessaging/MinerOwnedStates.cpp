#include "MinerOwnedStates.h"
#include "fsm/State.h"
#include "Miner.h"
#include "Locations.h"
#include "messaging/Telegram.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"
#include "Time/CrudeTimer.h"
#include "EntityNames.h"
#include "GUI_Base.h"

#include <sstream>
#include <iostream>

using std::cout;


#ifdef TEXTOUTPUT
#include <fstream>
extern std::ofstream os;
#define cout os
#endif



//------------------------------------------------------------------------methods for EnterMineAndDigForNugget
EnterMineAndDigForNugget* EnterMineAndDigForNugget::Instance()
{
	static EnterMineAndDigForNugget instance;

	return &instance;
}


void EnterMineAndDigForNugget::Enter(Miner* pMiner)
{
	//if the miner is not already located at the goldmine, he must
	//change location to the gold mine
	if (pMiner->Location() != goldmine)
	{
		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Walkin' to the goldmine", 0);
		//myGUI::Instance()->speak(pMiner, "Walkin' to the goldmine");

		myGUI::Instance()->goToSomewhere(pMiner, MINE_X, MINE_Y);

		pMiner->ChangeLocation(goldmine);
	}
}


void EnterMineAndDigForNugget::Execute(Miner* pMiner)
{
	//Now the miner is at the goldmine he digs for gold until he
	//is carrying in excess of MaxNuggets. If he gets thirsty during
	//his digging he packs up work for a while and changes state to
	//gp to the saloon for a whiskey.
	pMiner->AddToGoldCarried(1);

	pMiner->IncreaseFatigue();

	printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Pickin' up a nugget", 0);
	//myGUI::Instance()->speak(pMiner, "Pickin' up a nugget");

	//if enough gold mined, go and put it in the bank
	if (pMiner->PocketsFull())
	{
		pMiner->GetFSM()->ChangeState(VisitBankAndDepositGold::Instance());
	}

	if (pMiner->Thirsty())
	{
		pMiner->GetFSM()->ChangeState(GoesToABar::Instance());
	}
}


void EnterMineAndDigForNugget::Exit(Miner* pMiner)
{
	if (pMiner->Thirsty()) {
		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Ah'm leavin' the goldmine, I'm so thusty !", 0);
		//myGUI::Instance()->speak(pMiner, "Ah'm leavin' the goldmine, I'm so thusty !");
	}
	else {
		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Ah'm leavin' the goldmine with mah pockets full o' sweet gold", 0);
		//myGUI::Instance()->speak(pMiner, "Ah'm leavin' the goldmine with mah pockets full o' sweet gold");
	}
}


bool EnterMineAndDigForNugget::OnMessage(Miner* pMiner, const Telegram& msg)
{
	//send msg to global message handler
	return false;
}

//------------------------------------------------------------------------methods for VisitBankAndDepositGold

VisitBankAndDepositGold* VisitBankAndDepositGold::Instance()
{
	static VisitBankAndDepositGold instance;

	return &instance;
}

void VisitBankAndDepositGold::Enter(Miner* pMiner)
{
	//on entry the miner makes sure he is located at the bank
	if (pMiner->Location() != bank)
	{
		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Goin' to the bank. Yes siree", 0);
		//myGUI::Instance()->speak(pMiner, "Goin' to the bank. Yes siree");

		myGUI::Instance()->goToSomewhere(pMiner, BANK_X, BANK_Y);

		pMiner->ChangeLocation(bank);
	}
}


void VisitBankAndDepositGold::Execute(Miner* pMiner)
{
	//deposit the gold
	pMiner->AddToWealth(pMiner->GoldCarried());

	pMiner->SetGoldCarried(0);
	printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Depositing gold. Total savings now: " + std::to_string(pMiner->Wealth()), 0);
	//myGUI::Instance()->speak(pMiner, "Depositing gold.");

	//wealthy enough to have a well earned rest?
	if (pMiner->Wealth() >= ComfortLevel)
	{
		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": WooHoo! Rich enough for now. Back home to mah li'lle lady", 0);
		//myGUI::Instance()->speak(pMiner, "WooHoo! Rich enough for now. Back home to mah li'lle lady");

		pMiner->GetFSM()->ChangeState(GoHomeAndSleepTilRested::Instance());
	}

	//otherwise get more gold
	else
	{
		pMiner->GetFSM()->ChangeState(EnterMineAndDigForNugget::Instance());
	}
}


void VisitBankAndDepositGold::Exit(Miner* pMiner)
{
	printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Leavin' the bank", 0);
	//myGUI::Instance()->speak(pMiner, "Leavin' the bank");
}


bool VisitBankAndDepositGold::OnMessage(Miner* pMiner, const Telegram& msg)
{
	//send msg to global message handler
	return false;
}
//------------------------------------------------------------------------methods for GoHomeAndSleepTilRested

GoHomeAndSleepTilRested* GoHomeAndSleepTilRested::Instance()
{
	static GoHomeAndSleepTilRested instance;

	return &instance;
}

void GoHomeAndSleepTilRested::Enter(Miner* pMiner)
{
	if (pMiner->Location() != shack)
	{
		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Walkin' home", 0);
		//myGUI::Instance()->speak(pMiner, "Walkin' home");

		myGUI::Instance()->goToSomewhere(pMiner, SHACK_X, SHACK_Y);

		pMiner->ChangeLocation(shack);


		//let the wife know I'm home
		Dispatch->DispatchMessage(SEND_MSG_IMMEDIATELY, //time delay
			pMiner->ID(),        //ID of sender
			ent_Elsa,            //ID of recipient
			Msg_HiHoneyImHome,   //the message
			NO_ADDITIONAL_INFO);

	}
}

void GoHomeAndSleepTilRested::Execute(Miner* pMiner)
{
	//if miner is not fatigued start to dig for nuggets again.
	if (!pMiner->Fatigued() && pMiner->healthy())
	{
		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": All mah fatigue has drained away. Time to find more gold!", 0);
		//myGUI::Instance()->speak(pMiner, "All mah fatigue has drained away. Time to find more gold!");

		pMiner->GetFSM()->ChangeState(EnterMineAndDigForNugget::Instance());
	}

	else
	{
		//sleep
		if (pMiner->Fatigued())
		{
			pMiner->DecreaseFatigue();
			printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": ZZZZ... ", 0);
			//myGUI::Instance()->speak(pMiner, "ZZZZ... ");
		}

		//sleep
		if (!pMiner->healthy())
		{
			pMiner->rest();
			printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + " - Recoverin' at home... -", 0);
			//myGUI::Instance()->speak(pMiner, "Recoverin' at home...*");
		}
	}
}

void GoHomeAndSleepTilRested::Exit(Miner* pMiner)
{
	if (pMiner->healthy()) {
		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": It's time to earn a living ! Let's make some golds. Cya bae.", 0);
		//myGUI::Instance()->speak(pMiner, "It's time to earn a living ! Let's make some golds. Cya bae.");
	}

}


bool GoHomeAndSleepTilRested::OnMessage(Miner* pMiner, const Telegram& msg)
{

	switch (msg.Msg)
	{
	case Msg_StewReady:

		std::ostringstream time;
		time << Clock->GetCurrentTime();
		printMsg(pMiner,"Message handled by " + GetNameOfEntity(pMiner->ID()) + " at time : " + time.str(), 3);

		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Okay Hun, ahm a comin'!", 0);
		//myGUI::Instance()->speak(pMiner, "Okay Hun, ahm a comin'!");

		pMiner->GetFSM()->ChangeState(EatStew::Instance());

		return true;

	}//end switch

	return false; //send message to global message handler
}


//------------------------------------------------------------------------methods for GoesToABar
GoesToABar* GoesToABar::Instance()
{
	static GoesToABar instance;

	return &instance;
}

void GoesToABar::Enter(Miner* pMiner)
{
	if (pMiner->Location() != saloon)
	{
		
		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Walking to the saloon", 0);
		myGUI::Instance()->goToSomewhere(pMiner, SALOON_X, SALOON_Y);
		pMiner->ChangeLocation(saloon);


		//global message "Entering the saloon"
		Dispatch->DispatchMessage(
			SEND_MSG_IMMEDIATELY,	 //time delay
			pMiner->ID(),			 //ID of sender
			ent_Drunkman,            //ID of recipient
			Msg_EnteringTheSaloon,   //the message
			NO_ADDITIONAL_INFO
		);

	}
}

void GoesToABar::Execute(Miner* pMiner)
{
	pMiner->GetFSM()->ChangeState(QuenchThirst::Instance());
}


void GoesToABar::Exit(Miner* pMiner) {}


bool GoesToABar::OnMessage(Miner* pMiner, const Telegram& msg)
{
	switch (msg.Msg) {
	case Msg_TauntingBob:
		std::ostringstream time;
		time << Clock->GetCurrentTime();
		printMsg(pMiner,"Message handled by " + GetNameOfEntity(pMiner->ID()) + " at time : " + time.str(), 3);

		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Calm down. I just wanna hav' a li'lle drink 'key?", 0);
		

		pMiner->GetFSM()->ChangeState(MinerFighting::Instance());

		return true;

	}
	//send msg to global message handler
	return false;
}


//------------------------------------------------------------------------QuenchThirst

QuenchThirst* QuenchThirst::Instance()
{
	static QuenchThirst instance;

	return &instance;
}

void QuenchThirst::Enter(Miner* pMiner)
{
	printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Give me a drink man, ahm very thusty I've been workin' all day !", 0);
	
}

void QuenchThirst::Execute(Miner* pMiner)
{
	pMiner->BuyAndDrinkAWhiskey();
	printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": That's mighty fine sippin' liquer", 0);
	

	pMiner->GetFSM()->ChangeState(EnterMineAndDigForNugget::Instance());
}


void QuenchThirst::Exit(Miner* pMiner)
{
	printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Leaving the saloon, feelin' good", 0);
	
}


bool QuenchThirst::OnMessage(Miner* pMiner, const Telegram& msg)
{
	return false;
}


//------------------------------------------------------------------------ Fighting

MinerFighting* MinerFighting::Instance()
{
	static MinerFighting instance;

	return &instance;
}

void MinerFighting::Enter(Miner* pMiner)
{
	printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": I'll teach ya a lesson.", 0);
	
}

void MinerFighting::Execute(Miner* pMiner)
{
	printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Take this! And that!", 0);
	
}

void MinerFighting::Exit(Miner* pMiner) {}


bool MinerFighting::OnMessage(Miner* pMiner, const Telegram& msg)
{
	std::ostringstream time;
	time << Clock->GetCurrentTime();
	switch (msg.Msg) {
	case Msg_BobWins:
		printMsg(pMiner,"Message handled by " + GetNameOfEntity(pMiner->ID()) + " at time : " + time.str(), 3);

		if (pMiner->getHealth() >= (M_HEALTH_MAX * 20) / 100) {
			printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": That will teach not to play with me again you idiot !", 0);
			

			pMiner->GetFSM()->ChangeState(QuenchThirst::Instance());


		}
		else {
			printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": That will teach not to play with me again you idiot ! I must go take a rest...", 0);
			

			pMiner->GetFSM()->ChangeState(GoHomeAndSleepTilRested::Instance());

		}


		return true;

	case Msg_BobLoses:

		printMsg(pMiner,"Message handled by " + GetNameOfEntity(pMiner->ID()) + " at time : " + time.str(), 3);

		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Sh't ahm bleedin' ! I must go to the hospital...", 0);
		
		myGUI::Instance()->goToSomewhere(pMiner, HOSPITAL_X, HOSPITAL_Y);
		pMiner->GetFSM()->ChangeState(GoToHospital::Instance());

		return true;
	}

	return false;
}

//------------------------------------------------------------------------EatStew

EatStew* EatStew::Instance()
{
	static EatStew instance;

	return &instance;
}


void EatStew::Enter(Miner* pMiner)
{
	printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Smells Reaaal goood Elsa!", 0);
	
}

void EatStew::Execute(Miner* pMiner)
{
	printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Tastes real good too !", 0);
	

	pMiner->GetFSM()->RevertToPreviousState();
}

void EatStew::Exit(Miner* pMiner)
{
	printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": Thankya li'lle lady. Ah better get back to whatever ah wuz doin'", 0);
	
}


bool EatStew::OnMessage(Miner* pMiner, const Telegram& msg)
{
	//send msg to global message handler
	return false;
}


//------------------------------------------------------------------------- GoToHospital

GoToHospital* GoToHospital::Instance()
{
	static GoToHospital instance;

	return &instance;
}

void GoToHospital::Enter(Miner* pMiner)
{
	if (pMiner->Location() != hospital)
	{
		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + " - falls loudly on the floor -", 0);
		

		pMiner->ChangeLocation(hospital);
		pMiner->knockDown();
	}
}


void GoToHospital::Execute(Miner* pMiner)
{
	pMiner->rest();

	// Bob stays in the hospital till his health is equal or bigger than 60% of the max health then and only then goes to his home to rest
	if (pMiner->getHealth() < (M_HEALTH_MAX * 60) / 100) {
		std::string toPrint = "- Still in the hospital, resting(" + std::to_string(pMiner->getHealth()) + " / " + std::to_string((int)M_HEALTH_MAX) + ") -";
		printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + toPrint, 0);
		

	}
	else {
		pMiner->GetFSM()->ChangeState(GoHomeAndSleepTilRested::Instance());
	}
}

void GoToHospital::Exit(Miner* pMiner)
{
	printMsg(pMiner,GetNameOfEntity(pMiner->ID()) + ": I'm feeling better. Aah finaly get out from this hospital, I should go home I still haven't recovered totally!", 0);
	

}

bool GoToHospital::OnMessage(Miner* pMiner, const Telegram& msg)
{
	std::ostringstream time;
	time << Clock->GetCurrentTime();
	printMsg(pMiner,"Message handled by " + GetNameOfEntity(pMiner->ID()) + " at time : " + time.str() + "\n / Bob is in the hospital, he can't take any messages. /", 3);

	return true;
}


