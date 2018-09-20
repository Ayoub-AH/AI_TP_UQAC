#ifndef DRUNKMAN_OWNED_STATES_H
#define DRUNKMAN_OWNED_STATES_H
//------------------------------------------------------------------------
//
//  Name:   DrunkmanOwnedStates.h
//
//  Desc:   All the states that can be assigned to the Drunkman class
//
//  Author: 
//
//------------------------------------------------------------------------
#include "fsm/State.h"

class Drunkman;
struct Telegram;



//------------------------------------------------------------------------
//	
//	The drunkman waits drinking alcoholic drinks, looking for his next victim
//------------------------------------------------------------------------
class Drinking : public State<Drunkman>
{
private:

	Drinking() {}

	//copy ctor and assignment should be private
	Drinking(const Drinking&);
	Drinking& operator=(const Drinking&);

public:

	//this is a singleton
	static Drinking* Instance();

	virtual void Enter(Drunkman* drunkman);

	virtual void Execute(Drunkman* drunkman);

	virtual void Exit(Drunkman* drunkman);

	virtual bool OnMessage(Drunkman* agent, const Telegram& msg);
};

//------------------------------------------------------------------------
//	
//	If he is fully drunk and meets somebody (let's say Bob), he starts a fight.
//------------------------------------------------------------------------
class DrukmanFighting : public State<Drunkman>
{
private:

	DrukmanFighting() {}

	//copy ctor and assignment should be private
	DrukmanFighting(const DrukmanFighting&);
	DrukmanFighting& operator=(const DrukmanFighting&);

public:

	//this is a singleton
	static DrukmanFighting* Instance();

	virtual void Enter(Drunkman* drunkman);

	virtual void Execute(Drunkman* drunkman);

	virtual void Exit(Drunkman* drunkman);

	virtual bool OnMessage(Drunkman* agent, const Telegram& msg);
};

//------------------------------------------------------------------------
//	
//	Beaten by his enemy, the drunk man is resting until his health is fully restore
//------------------------------------------------------------------------
class KO : public State<Drunkman>
{
private:

	KO() {}

	//copy ctor and assignment should be private
	KO(const KO&);
	KO& operator=(const KO&);

public:

	//this is a singleton
	static KO* Instance();

	virtual void Enter(Drunkman* drunkman);

	virtual void Execute(Drunkman* drunkman);

	virtual void Exit(Drunkman* drunkman);

	virtual bool OnMessage(Drunkman* agent, const Telegram& msg);
};


#endif
