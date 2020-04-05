#pragma once
#include <map>
#include <SFML/Graphics.hpp>
#include "Bomb.h"
#include "AIPlayer.h"

class BombManager
{
public:
	BombManager();
	~BombManager();
	void Init(Level* level, std::vector<AIPlayer*>* players);
	void Update(float dt);
	std::map<std::pair<int*, int*>, Bomb*>* getBombs(void);
private:
	void gatherBombInformation();
	void handleBombKicking(float dt);
	void checkBombsAfterExplosionsCollisions();
	std::map<std::pair<int*, int*>, Bomb*> bombs;
	std::map<std::pair<int*, int*>, Bomb*>::iterator it;
	std::vector<AIPlayer*>* m_players;
	std::vector<Ray*>rays;
	Level* m_level;
};

