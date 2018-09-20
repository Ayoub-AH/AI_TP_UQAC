#include "Drunkman.h"

bool Drunkman::HandleMessage(const Telegram& msg)
{
	return d_pStateMachine->HandleMessage(msg);
}


void Drunkman::Update()
{
	//set text color to green
	//SetTextColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);

	d_pStateMachine->Update();
}