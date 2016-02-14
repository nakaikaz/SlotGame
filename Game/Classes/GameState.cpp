#include "GameState.h"


//const unsigned short GameState::READY			= 0x0001;
const unsigned short GameState::PLAYING			= 0x0002;
const unsigned short GameState::STOPPING			= 0x0004;
const unsigned short GameState::STOPPED			= 0x0008;
const unsigned short GameState::OVER				= 0x0010;
const unsigned short GameState::ALREADY_PLAYED	= 0x0020;
const unsigned short GameState::ANIMATION		= 0x0040;
const unsigned short GameState::GAME_MODE		= 0x0100;