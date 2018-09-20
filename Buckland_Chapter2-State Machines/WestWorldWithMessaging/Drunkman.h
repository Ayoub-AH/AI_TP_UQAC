#ifndef DRUNKMAN_H
#define DRUNKMAN_H
//------------------------------------------------------------------------
//
//  Name: Drunkman.h
//
//  Desc: class to implement the Drunk Man in the saloon.
//
//  Author:
//
//------------------------------------------------------------------------


#include "fsm/State.h"
#include "BaseGameEntity.h"
#include "Locations.h"
#include "DrunkmanOwnedStates.h"
#include "misc/ConsoleUtils.h"
#include "Miner.h"
#include "fsm/StateMachine.h"
#include "misc/Utils.h"
#include "constantes.h"

// above this value, the drunk man is ... well, drunk.
const int ALCOHOL_LEVEL = 5;

//full life
const int HEALTH_MAX = 5;

//initial drunkness
const int INIT_DRUNKENNESS = 0;

class Drunkman : public BaseGameEntity
{
private:

	//an instance of the state machine class
	StateMachine<Drunkman>* d_pStateMachine;

	location_type   d_Location;

	//alcohol use level
	int			d_Drunkenness;

	//Health points
	int			d_Health;

	bool isAThreat = false;


public:

	Drunkman(int id, State<Drunkman>* initState) :
		d_Location(saloon),
		d_Drunkenness(INIT_DRUNKENNESS),
		d_Health(HEALTH_MAX),
		BaseGameEntity(id)

	{
		//set up the state machine
		d_pStateMachine = new StateMachine<Drunkman>(this);

		d_pStateMachine->SetCurrentState(initState);
		//d_pStateMachine->SetCurrentState(Drinking::Instance());

		position.x = POSITION_DRUNKMAN_INIT_X;
		position.y = POSITION_DRUNKMAN_INIT_Y;

		/* NOTE, A GLOBAL STATE HAS NOT BEEN IMPLEMENTED FOR THE DRUNKMAN */
	}

	~Drunkman() { delete d_pStateMachine; }


	//this must be implemented
	void          Update();

	//so must this
	virtual bool  HandleMessage(const Telegram& msg);

	StateMachine<Drunkman>* GetFSM()const { return d_pStateMachine; }

	const std::string getCurrentStateName() { return GetFSM()->GetNameOfCurrentState(); }

	//----------------------------------------------------accessors
	location_type Location()const { return d_Location; }
	void          ChangeLocation(location_type loc) { d_Location = loc; }

	int			  getDrunkenness() { return d_Drunkenness; }
	void          SetDrunkenness(int val) { d_Drunkenness = val; }
	void		  InscreaseDrunkenness(int i=1) { d_Drunkenness += i; }
	void		  DecreaseDrunkenness(int i=1) { d_Drunkenness -= i; if (d_Drunkenness < 0) d_Drunkenness = 0; }
	bool		  isDrunk() { if (getDrunkenness() > ALCOHOL_LEVEL) return true; else return false; }

	void		  rest() { DecreaseDrunkenness(2); d_Health += 1; }
	int			  getHealth() { return d_Health; }

	void		  knockDown() { d_Health = 0; }
};

#endif
