#include "MinersWifeOwnedStates.h"
#include "MinerOwnedStates.h"
#include "MinersWife.h"
#include "Locations.h"
#include "Time/CrudeTimer.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"
#include "EntityNames.h"
#include "GUI_Base.h"

#include <iostream>
using std::cout;

#ifdef TEXTOUTPUT
#include <fstream>
extern std::ofstream os;
#define cout os
#endif

//-----------------------------------------------------------------------Global state

WifesGlobalState* WifesGlobalState::Instance()
{
	static WifesGlobalState instance;

	return &instance;
}


void WifesGlobalState::Execute(MinersWife* wife)
{
	//1 in 10 chance of needing the bathroom (provided she is not already
	//in the bathroom)
	if ((RandFloat() < 0.1) &&
		!wife->GetFSM()->isInState(*VisitBathroom::Instance()))
	{
		wife->GetFSM()->ChangeState(VisitBathroom::Instance());
	}
}

bool WifesGlobalState::OnMessage(MinersWife* wife, const Telegram& msg)
{
	switch (msg.Msg)
	{
	case Msg_HiHoneyImHome:
	{
		std::ostringstream time;
		time << Clock->GetCurrentTime();
		printMsg(wife,"Message handled by " + GetNameOfEntity(wife->ID()) + " at time : " + time.str(), 3);

		printMsg(wife,GetNameOfEntity(wife->ID()) + ": Hi honey. Let me make you some of mah fine country stew", 1);
		//myGUI::Instance()->speak(wife, "Hi honey. Let me make you some of mah fine country stew");

		wife->GetFSM()->ChangeState(CookStew::Instance());
	}
	return true;

	}//end switch

	return false;
}

//-------------------------------------------------------------------------DoHouseWork

DoHouseWork* DoHouseWork::Instance()
{
	static DoHouseWork instance;

	return &instance;
}


void DoHouseWork::Enter(MinersWife* wife)
{
	printMsg(wife,GetNameOfEntity(wife->ID()) + ": Time to do some more housework!", 1);
	//myGUI::Instance()->speak(wife, "Hi honey. Let me make you some of mah fine country stew");
}


void DoHouseWork::Execute(MinersWife* wife)
{
	switch (RandInt(0, 2))
	{
	case 0:
		printMsg(wife,GetNameOfEntity(wife->ID()) + ": Moppin' the floor", 1);
		//myGUI::Instance()->speak(wife, "Moppin' the floor");

		break;

	case 1:
		printMsg(wife,GetNameOfEntity(wife->ID()) + ": Washin' the dishes", 1);
		//myGUI::Instance()->speak(wife, "Washin' the dishes");
		break;
	case 2:
		printMsg(wife,GetNameOfEntity(wife->ID()) + ": Makin' the bed", 1);
		//myGUI::Instance()->speak(wife, "Makin' the bed");

		break;
	}
}

void DoHouseWork::Exit(MinersWife* wife) {}

bool DoHouseWork::OnMessage(MinersWife* wife, const Telegram& msg)
{
	return false;
}

//------------------------------------------------------------------------VisitBathroom

VisitBathroom* VisitBathroom::Instance()
{
	static VisitBathroom instance;

	return &instance;
}


void VisitBathroom::Enter(MinersWife* wife)
{
	printMsg(wife,GetNameOfEntity(wife->ID()) + ": Walkin' to the can. Need to powda mah pretty li'lle nose", 1);
	//myGUI::Instance()->speak(wife, "Walkin' to the can. Need to powda mah pretty li'lle nose");
}


void VisitBathroom::Execute(MinersWife* wife)
{
	printMsg(wife,GetNameOfEntity(wife->ID()) + ": Ahhhhhh! Sweet relief!", 1);
	//myGUI::Instance()->speak(wife, "Ahhhhhh! Sweet relief!");

	wife->GetFSM()->RevertToPreviousState();
}

void VisitBathroom::Exit(MinersWife* wife)
{
	printMsg(wife,GetNameOfEntity(wife->ID()) + ": Leavin' the Jon", 1);
	//myGUI::Instance()->speak(wife, "Leavin' the Jon");
}


bool VisitBathroom::OnMessage(MinersWife* wife, const Telegram& msg)
{
	return false;
}


//------------------------------------------------------------------------CookStew

CookStew* CookStew::Instance()
{
	static CookStew instance;

	return &instance;
}


void CookStew::Enter(MinersWife* wife)
{
	//if not already cooking put the stew in the oven
	if (!wife->Cooking())
	{
		printMsg(wife,GetNameOfEntity(wife->ID()) + ": Putting the stew in the oven", 1);
		//myGUI::Instance()->speak(wife, "Putting the stew in the oven");

		//send a delayed message myself so that I know when to take the stew
		//out of the oven

		Dispatch->DispatchMessage(1.5,                  //time delay
			wife->ID(),           //sender ID
			wife->ID(),           //receiver ID
			Msg_StewReady,        //msg
			NO_ADDITIONAL_INFO);


		wife->SetCooking(true);
	}
}


void CookStew::Execute(MinersWife* wife)
{
	printMsg(wife,GetNameOfEntity(wife->ID()) + ": Fussin' over food", 1);
	//myGUI::Instance()->speak(wife, "Fussin' over food");
}

void CookStew::Exit(MinersWife* wife)
{
	printMsg(wife,GetNameOfEntity(wife->ID()) + ": Puttin' the stew on the table", 1);
	//myGUI::Instance()->speak(wife, "Puttin' the stew on the table");
}


bool CookStew::OnMessage(MinersWife* wife, const Telegram& msg)
{
	switch (msg.Msg)
	{
	case Msg_StewReady:
	{
		std::ostringstream time;
		time << Clock->GetCurrentTime();
		printMsg(wife,"Message handled by " + GetNameOfEntity(wife->ID()) + " at time : " + time.str(), 3);

		printMsg(wife,GetNameOfEntity(wife->ID()) + ": StewReady! Lets eat", 1);
		//myGUI::Instance()->speak(wife, "StewReady! Lets eat");


		//let hubby know the stew is ready
		Dispatch->DispatchMessage(SEND_MSG_IMMEDIATELY,
			wife->ID(),
			ent_Miner_Bob,
			Msg_StewReady,
			NO_ADDITIONAL_INFO);


		wife->SetCooking(false);

		wife->GetFSM()->ChangeState(DoHouseWork::Instance());
	}

	return true;

	}//end switch

	return false;
}