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
	void findEnemy(void);
	void findEnemyDangerZone(void);
	void findBombs(void);
	void findDangerZone(void);
	void buildPath(void);
	void move(void);
	void placeBomb(void);	

	AIPlayerStates state;
	std::vector< std::vector<TT::AITileType> > m_data;
};
