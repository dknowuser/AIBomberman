#pragma once
#include "Player.h"
#include "Bomb.h"
#include "LevelView.h"

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
	void Spawn();
	void Respawn();
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
	std::vector<std::pair<int, int>> m_path;
	bool isTileReached;
	bool foundPlaceForBomb;
	std::pair<int, int> nextPosition;

	TT::AITileType** nodes;

	int debugVar;
	unsigned int resetCounter;
	const unsigned int resetCount = 50;
};

