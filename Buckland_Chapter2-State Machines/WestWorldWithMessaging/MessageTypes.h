#ifndef MESSAGE_TYPES
#define MESSAGE_TYPES

#include <string>

enum message_type
{
  Msg_HiHoneyImHome,
  Msg_StewReady,
  Msg_EnteringTheSaloon,
  Msg_TauntingBob,
  Msg_BobWins,
  Msg_BobLoses
};


inline std::string MsgToStr(int msg)
{
  switch (msg)
  {
	  case Msg_HiHoneyImHome:
		return "HiHoneyImHome"; 

	  case Msg_StewReady:
		return "StewReady";

	  case Msg_EnteringTheSaloon:
		  return "EnteringTheSaloon";

	  case Msg_TauntingBob:
		  return "TauntingBob";
	
	  case Msg_BobWins:
		  return "BobWins";

	  case Msg_BobLoses:
		  return "BobLoses";

	  default:
		return "Not recognized!";
  }
}

#endif