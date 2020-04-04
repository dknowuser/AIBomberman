#pragma once
#include "Player.h"
#include "Level.h"

class AIPlayer: public Player
{
	enum class AIPlayerStates {
		ANALYSE,
		MOVE,
		PLACE,
		RUNAWAY
	};

public:
	void run(std::pair<int, int> &input);
	void setMData(Level *m_level);
private:
	void getData(void);
	void clearLevelState(void);
	void getMyPosition(void);

	AIPlayerStates state;
	std::vector< std::vector<TT::AITileType> > m_data;
};
