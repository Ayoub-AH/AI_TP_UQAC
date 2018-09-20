#ifndef NAMES_H
#define NAMES_H

#include <string>
#include <mutex>
#include "BaseGameEntity.h"
#include "GUI_Base.h"

// mutex shared between all entities
static std::mutex mutex;

enum 
{
  ent_Miner_Bob,
  ent_Elsa,
  ent_Drunkman,
  ent_ghost
};

inline std::string GetNameOfEntity(int n)
{
  switch(n)
  {
  case ent_Miner_Bob:
    return "Miner Bob";

  case ent_Elsa:
    return "Elsa"; 

  case ent_Drunkman:
	  return "Joe";

  default:
    return "UNKNOWN!";
  }
}

 inline void printMsg(BaseGameEntity* entity ,std::string msg, int color) {
	 mutex.lock();

	 SDL_Color fontColor;

	 // choose a color for the text 
	 switch (color) {
		 // When color ==0 -> Miners' color which is red
		case 0:
			SetTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
			fontColor = { 209,26,26 };
			break;

		// When color ==1 -> Miners' Wife color which is green
		case 1:
			SetTextColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			fontColor = { 26,209,44 };
			break;

		// When color ==2 -> Drunkmans' color which is blue
		case 2:
			SetTextColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			fontColor = { 50,26,209 };
			break;

		// When color ==3 -> Messages color 
		case 3:
			SetTextColor(BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			break;

	 }

	 // To display the msgs in the GUI
	 if (color >= 0 && color < 3) {
		 char *cstr = new char[msg.length() + 1];
		 strcpy(cstr, msg.c_str());
		 myGUI::Instance()->speak(entity, cstr,fontColor);
		 delete[] cstr;
	 }

	 std::cout << "\n" << msg;
	 mutex.unlock();
}


 

#endif