#pragma once
#include "Player.h"
#include "Level.h"
#include "Bomb.h"

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
	void setPlayer(AIPlayer *player);
	void setBombs(std::map<std::pair<int*, int*>, Bomb*>* bombs);
private:
	void getData(void);
	void clearLevelState(void);
	void getMyPosition(void);
	void getEnemyPosition(void);
	void getBombsAndDangerZones(void);

	AIPlayerStates state;
	std::vector< std::vector<TT::AITileType> > m_data;
	AIPlayer *m_player;
	std::map<std::pair<int*, int*>, Bomb*>* m_bombs;
};
