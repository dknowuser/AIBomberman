#include "AIPlayer.h"

void AIPlayer::run(std::pair<int, int> &input)
{
	input.first = 1;

	switch(state) {
	case AIPlayerStates::ANALYSE:
		break;
	
	case AIPlayerStates::MOVE:
		break;
	
	case AIPlayerStates::PLACE:
		break;
	
	case AIPlayerStates::RUNAWAY:
		break;
	
	default:
		break;
	};
};
