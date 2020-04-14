#pragma once
#include "Player.h"
#include "Level.h"
#include "Bomb.h"

class AIPlayer: public Player
{
	enum class AIPlayerStates {
		ANALYSE,
		MOVE,
		PLACE_AND_ANALYSE,
		WAIT,
		RUNAWAY
	};

public:
	void run(std::pair<int, int> &input);
	void setMData(Level *m_level);
	void refreshMData(Level *m_level);
	void setPlayer(AIPlayer *player);
	void setBombs(std::map<std::pair<int*, int*>, Bomb*>* bombs);
private:
	void getData(void);
	void placeAndAnalyse(void);
	void clearLevelState(void);
	void getMyPosition(void);
	void getEnemyPosition(void);
	void getBombsAndDangerZones(void);
	void getBombPlaces(void);
	void buildPathToBomb(void);
	void move(std::pair<int, int> &input);
	void placeBomb(void);
	void buildPathToSafe(void);
	void wait(void);
	void debug(std::pair<int, int> &input);

	AIPlayerStates state;
	std::vector<std::vector<TT::AITileType>> m_data;
	AIPlayer *m_player;
	std::map<std::pair<int*, int*>, Bomb*>* m_bombs;
	std::vector<std::pair<int, int>> m_path;
	bool isTileReached;
	std::pair<int, int> nextPosition;
	std::pair<int, int> myBomb;

	TT::AITileType** nodes;

	int debugVar;
};

