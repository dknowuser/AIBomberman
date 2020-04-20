#include "AIPlayer.h"

void AIPlayer::setMData(Level *m_level)
{
	std::vector< std::vector<TT::TileType> > im_data = m_level->GetMData();
	m_data.resize(m_level->GetHeight());
	for(unsigned int i = 0; i < m_level->GetHeight(); i++) {
		m_data[i].resize(m_level->GetWidth());
		for(unsigned int j = 0; j < m_level->GetWidth(); j++) {
			m_data[i][j].tileType = im_data[i][j];
			m_data[i][j].tileState = TT::TileState::NONE;
			m_data[i][j].parent = nullptr;
			m_data[i][j].y = i;
			m_data[i][j].x = j;
		};
	};
};

void AIPlayer::refreshMData(Level *m_level)
{
	int sizeX, sizeY;
	sizeY = m_data.size();
	sizeX =  m_data[0].size();
	std::vector< std::vector<TT::TileType> > im_data = m_level->GetMData();
	for(unsigned int i = 0; i < sizeY; i++)
		for(unsigned int j = 0; j < sizeX; j++)
			m_data[i][j].tileType = im_data[i][j];
};

void AIPlayer::setPlayer(AIPlayer *player)
{
	m_player = player;
};

void AIPlayer::setBombs(std::map<std::pair<int*, int*>, Bomb*>* bombs)
{
	m_bombs = bombs;
};

void AIPlayer::run(std::pair<int, int> &input)
{
	switch(state) {
	case AIPlayerStates::ANALYSE:
		getData();
		break;
	
	case AIPlayerStates::MOVE:
		move(input);
		break;
	
	case AIPlayerStates::PLACE_AND_ANALYSE:
		placeAndAnalyse();
		break;

	case AIPlayerStates::RUNAWAY:
		move(input);
		break;

	case AIPlayerStates::WAIT:
		wait();
		break;
	
	default:
		break;
	};
};

void AIPlayer::debug(std::pair<int, int> &input)
{
	if(!m_path.empty())
		input.first = 1;
	else
		input.first = -1;
};

void AIPlayer::getData(void)
{
	clearLevelState();
	getMyPosition();
	getEnemyPosition();
	getBombsAndDangerZones();
	getBombPlaces();
	buildPathToBomb();
	state = AIPlayerStates::MOVE;
};

void AIPlayer::placeAndAnalyse(void)
{
	placeBomb();
	clearLevelState();
	getMyPosition();
	getBombsAndDangerZones();
	buildPathToSafe();
	state = AIPlayerStates::RUNAWAY;
};

void AIPlayer::clearLevelState(void)
{
	int sizeX, sizeY;
	sizeY = m_data.size();
	sizeX =  m_data[0].size();
	for(unsigned int i = 0; i < sizeY; i++)
		for(unsigned int j = 0; j < sizeX; j++)
			m_data[i][j].tileState = TT::TileState::NONE;
};

void AIPlayer::getMyPosition(void)
{
	int x, y;
	x = this->GetPositionInTilesCoordsX();
	y = this->GetPositionInTilesCoordsY();
	m_data[y][x].tileState = (TT::TileState)((int)TT::TileState::AIPLAYER
		| (int)m_data[y][x].tileState);
};

void AIPlayer::getEnemyPosition(void)
{
	int x, y;
	x = m_player->GetPositionInTilesCoordsX();
	y = m_player->GetPositionInTilesCoordsY();
	m_data[y][x].tileState = (TT::TileState)((int)TT::TileState::PLAYER
		| (int)m_data[y][x].tileState);
};

void AIPlayer::getBombsAndDangerZones(void)
{
	int sizeX, sizeY;
	sizeY = m_data.size();
	sizeX = m_data[0].size();
	for(int y = 0; y < sizeY; y++)
		for(int x = 0; x < sizeX; x++) 
			if((int)m_data[y][x].tileType == (int)TT::TileType::BOMB) {
				m_data[y][x].tileState = (TT::TileState)((int)TT::TileState::BOMB
					| (int)m_data[y][x].tileState);
		
				for(int i = 0; i < 5; i++) {
					// Up
					if((y - i) >= 0)
						m_data[y - i][x].tileState = (TT::TileState)((int)TT::TileState::DANGER
							| (int)m_data[y][x].tileState);

					// Down
					if((y + i) < m_data.size())
						m_data[y + i][x].tileState = (TT::TileState)((int)TT::TileState::DANGER
							| (int)m_data[y][x].tileState);

					// Left
					if((x - i) >= 0)
						m_data[y][x - i].tileState = (TT::TileState)((int)TT::TileState::DANGER
							| (int)m_data[y][x].tileState);

					// Right
					if((x + i) < m_data[0].size())
						m_data[y][x + i].tileState = (TT::TileState)((int)TT::TileState::DANGER
							| (int)m_data[y][x].tileState);
				};
			};
};

void AIPlayer::getBombPlaces(void)
{
	int sizeX, sizeY;
	sizeY = m_data.size();
	sizeX =  m_data[0].size();
	for(int i = 0; i < sizeY; i++)
		for(int j = 0; j < sizeX; j++) {
			if(!((int)m_data[i][j].tileState & (int)TT::TileState::BOMB)
					&& ((m_data[i][j].tileType == TT::TileType::NONE)
					|| (m_data[i][j].tileType == TT::TileType::NONE_WITH_SHADOW))) {
				// Up
				bool placeBombUp = true;
				bool playerUp = false;
				for(short g = 0; g < 5; g++) {
					if((i - g) > 0) {
						if((int)m_data[i - g][j].tileState & (int)TT::TileState::PLAYER) {
							playerUp = true;
							break;
						};

						placeBombUp *= (m_data[i - g][j].tileType == TT::TileType::NONE)
							|| (m_data[i - g][j].tileType == TT::TileType::NONE_WITH_SHADOW);
					};
				};
				placeBombUp *= playerUp;
				if(placeBombUp) {
					m_data[i][j].tileState = (TT::TileState)((int)TT::TileState::PLACE_FOR_BOMB
						| (int)m_data[i][j].tileState);
				};

				// Down
				bool placeBombDown = true;
				bool playerDown = false;
				for(short g = 0; g < 5; g++) {
					if((i + g) < m_data.size()) {
						if((int)m_data[i + g][j].tileState & (int)TT::TileState::PLAYER) {
							playerDown = true;
							break;
						};

						placeBombDown *= (m_data[i + g][j].tileType == TT::TileType::NONE)
							|| (m_data[i + g][j].tileType == TT::TileType::NONE_WITH_SHADOW);
					};
				};
				placeBombDown *= playerDown;
				if(placeBombDown) {
					m_data[i][j].tileState = (TT::TileState)((int)TT::TileState::PLACE_FOR_BOMB
						| (int)m_data[i][j].tileState);
				};

				// Left
				bool placeBombLeft = true;
				bool playerLeft = false;
				for(short g = 0; g < 5; g++) {
					if((j - g) > 0) {
						if((int)m_data[i][j - g].tileState & (int)TT::TileState::PLAYER) {
							playerLeft = true;
							break;
						};

						placeBombLeft *= (m_data[i][j - g].tileType == TT::TileType::NONE)
							|| (m_data[i][j - g].tileType == TT::TileType::NONE_WITH_SHADOW);
					};
				};
				placeBombLeft *= playerLeft;
				if(placeBombLeft) {
					m_data[i][j].tileState = (TT::TileState)((int)TT::TileState::PLACE_FOR_BOMB
						| (int)m_data[i][j].tileState);
				};

				// Right
				bool placeBombRight = true;
				bool playerRight = false;
				for(short g = 0; g < 5; g++) {
					if((j + g) < m_data[0].size()) {
						if((int)m_data[i][j + g].tileState & (int)TT::TileState::PLAYER) {
							playerRight = true;
							break;
						};

						placeBombRight *= (m_data[i][j + g].tileType == TT::TileType::NONE)
							|| (m_data[i][j + g].tileType == TT::TileType::NONE_WITH_SHADOW);
					};
				};
				placeBombRight *= playerRight;
				if(placeBombRight) {
					m_data[i][j].tileState = (TT::TileState)((int)TT::TileState::PLACE_FOR_BOMB
						| (int)m_data[i][j].tileState);
				};
			};
		};

};

void AIPlayer::buildPathToBomb(void)
{
	int count = 0; 
	int newCount = 1;
	int x = this->GetPositionInTilesCoordsX();
	int y = this->GetPositionInTilesCoordsY();
	m_path.clear();
	for(unsigned int i = 0; i < m_data.size(); i++) {
		for(unsigned int j = 0; j < m_data[0].size(); j++) {
			m_data[i][j].parent = nullptr;
		};
	};
	nodes = new TT::AITileType*[m_data.size() *  m_data[0].size() * sizeof(TT::AITileType*)];
	if(nodes) {
		nodes[count] = &m_data[y][x];
		while((count < newCount) && (newCount < (m_data.size() * m_data[0].size()))
				&& !((int)nodes[count]->tileState & (int)TT::TileState::PLACE_FOR_BOMB)) {
			// Up
			if((nodes[count]->y - 1 >= 0) && !m_data[nodes[count]->y - 1][nodes[count]->x].parent
					&& ((m_data[nodes[count]->y - 1][nodes[count]->x].tileType == TT::TileType::NONE)
					       || (m_data[nodes[count]->y - 1][nodes[count]->x].tileType 
						       == TT::TileType::NONE_WITH_SHADOW)
					       || (m_data[nodes[count]->y - 1][nodes[count]->x].tileType 
						       == TT::TileType::BOMB))) {
				nodes[newCount] = &m_data[nodes[count]->y - 1][nodes[count]->x];
				nodes[newCount]->parent = nodes[count];
				newCount++;
			};

			// Right
			if((nodes[count]->x + 1 >= 0) && !m_data[nodes[count]->y][nodes[count]->x + 1].parent
					&& ((m_data[nodes[count]->y][nodes[count]->x + 1].tileType == TT::TileType::NONE)
					       || (m_data[nodes[count]->y][nodes[count]->x + 1].tileType 
						       == TT::TileType::NONE_WITH_SHADOW)
					       || (m_data[nodes[count]->y][nodes[count]->x + 1].tileType 
						       == TT::TileType::BOMB))) {
				nodes[newCount] = &m_data[nodes[count]->y][nodes[count]->x + 1];
				nodes[newCount]->parent = nodes[count];
				newCount++;
			};
			
			// Down
			if((nodes[count]->y + 1 < m_data.size()) && !m_data[nodes[count]->y + 1][nodes[count]->x].parent
					&& ((m_data[nodes[count]->y + 1][nodes[count]->x].tileType == TT::TileType::NONE)
					       || (m_data[nodes[count]->y + 1][nodes[count]->x].tileType 
						       == TT::TileType::NONE_WITH_SHADOW)
					       || (m_data[nodes[count]->y + 1][nodes[count]->x].tileType 
						       == TT::TileType::BOMB))) {
				nodes[newCount] = &m_data[nodes[count]->y + 1][nodes[count]->x];
				nodes[newCount]->parent = nodes[count];
				newCount++;
			};

			// Left
			if((nodes[count]->x - 1 >= 0) && !m_data[nodes[count]->y][nodes[count]->x - 1].parent
					&& ((m_data[nodes[count]->y][nodes[count]->x - 1].tileType == TT::TileType::NONE)
					       || (m_data[nodes[count]->y][nodes[count]->x - 1].tileType 
						       == TT::TileType::NONE_WITH_SHADOW)
					       || (m_data[nodes[count]->y][nodes[count]->x - 1].tileType 
						       == TT::TileType::BOMB))) {
				nodes[newCount] = &m_data[nodes[count]->y][nodes[count]->x - 1];
				nodes[newCount]->parent = nodes[count];
				newCount++;
			};

			count++;
		};

		if((count < newCount) 
				&& ((int)nodes[count]->tileState & (int)TT::TileState::PLACE_FOR_BOMB)) {
			TT::AITileType* currentTile = nodes[count];
			std::pair<int, int> pair; // first - x, second - y
			while(currentTile != &m_data[y][x]) {
				pair.first = currentTile->x - currentTile->parent->x;
				pair.second = currentTile->y - currentTile->parent->y;
				m_path.push_back(pair);
				currentTile = currentTile->parent;
			};
			std::reverse(m_path.begin(), m_path.end());
		}

		delete[] nodes;
	};
	isTileReached = true;
};

void AIPlayer::move(std::pair<int, int> &input)
{
	if(m_path.empty()) {
		input.first = 0;
		input.second = 0;
		if(state == AIPlayerStates::MOVE)
			state = AIPlayerStates::PLACE_AND_ANALYSE;
		else
			state = AIPlayerStates::WAIT;
		return;
	};

	if(isTileReached) {
		isTileReached = false;

		// Correct AIPlayer position
		this->SetPositionX(static_cast<int>(this->GetPositionX() / TILE_SIZE) * TILE_SIZE + TILE_SIZE/2);
		this->SetPositionY(static_cast<int>(this->GetPositionY() / TILE_SIZE) * TILE_SIZE + TILE_SIZE/2);

		// Right
		if(m_path[0].first == 1) {
			nextPosition.first = this->GetPositionX() + TILE_SIZE;
			nextPosition.second = this->GetPositionY();
			return;
		};

		// Left
		if(m_path[0].first == -1) {
			nextPosition.first = this->GetPositionX() - TILE_SIZE;
			nextPosition.second = this->GetPositionY();
			return;
		};

		// Down
		if(m_path[0].second == 1) {
			nextPosition.first = this->GetPositionX();
			nextPosition.second = this->GetPositionY() + TILE_SIZE;
			return;
		};

		// Up
		if(m_path[0].second == -1) {
			nextPosition.first = this->GetPositionX();
			nextPosition.second = this->GetPositionY() - TILE_SIZE;
			return;
		};
	}
	else {
		if((m_path[0].first == 1) && (this->GetPositionX() < nextPosition.first))
			input.first = 1;

		if((m_path[0].first == -1) && (this->GetPositionX() > nextPosition.first))
			input.first = -1;

		if((m_path[0].second == 1) && (this->GetPositionY() < nextPosition.second))
			input.second = 1;

		if((m_path[0].second == -1) && (this->GetPositionY() > nextPosition.second))
			input.second = -1;


		if((m_path[0].first == 1) && (this->GetPositionX() >= nextPosition.first)) {
			m_path.erase(m_path.begin());
			isTileReached = true;
			return;
		};

		if((m_path[0].first == -1) && (this->GetPositionX() <= nextPosition.first)) {
			m_path.erase(m_path.begin());
			isTileReached = true;
			return;
		};

		if((m_path[0].second == 1) && (this->GetPositionY() >= nextPosition.second)) {
			m_path.erase(m_path.begin());
			isTileReached = true;
			return;
		};

		if((m_path[0].second == -1) && (this->GetPositionY() <= nextPosition.second)) {
			m_path.erase(m_path.begin());
			isTileReached = true;
			return;
		};
	};
};

void AIPlayer::placeBomb(void)
{
	int x = this->GetPositionInTilesCoordsX();
	int y = this->GetPositionInTilesCoordsY();
	m_data[y][x].tileType = TT::TileType::BOMB;
	myBomb.first = x;
	myBomb.second = y;
	this->OnActionKeyPressed();
	debugVar = 0;
};

void AIPlayer::wait(void)
{
	if(m_data[myBomb.second][myBomb.first].tileType != TT::TileType::BOMB) {
		if(debugVar < 50)
			debugVar++;
		else
			state = AIPlayerStates::ANALYSE;
	};
};

void AIPlayer::buildPathToSafe(void)
{
	unsigned int count = 0; 
	unsigned int newCount = 1;
	int x = this->GetPositionInTilesCoordsX();
	int y = this->GetPositionInTilesCoordsY();
	m_path.clear();
	for(unsigned int i = 0; i < m_data.size(); i++) {
		for(unsigned int j = 0; j < m_data[0].size(); j++) {
			m_data[i][j].parent = nullptr;
		};
	};

	nodes = new TT::AITileType*[m_data.size() * m_data[0].size() * sizeof(TT::AITileType*)];
	nodes[count] = &m_data[y][x];

	while(((int)nodes[count]->tileState & (int)TT::TileState::DANGER)
			&& (count < newCount) && (newCount < (m_data.size() *  m_data[0].size()))) {
		// Up
		if((nodes[count]->y - 1 >= 0) && !m_data[nodes[count]->y - 1][nodes[count]->x].parent
				&& ((m_data[nodes[count]->y - 1][nodes[count]->x].tileType == TT::TileType::NONE)
				       || (m_data[nodes[count]->y - 1][nodes[count]->x].tileType 
					       == TT::TileType::NONE_WITH_SHADOW)
				       || (m_data[nodes[count]->y - 1][nodes[count]->x].tileType 
					       == TT::TileType::BOMB))) {
			nodes[newCount] = &m_data[nodes[count]->y - 1][nodes[count]->x];
			nodes[newCount]->parent = nodes[count];
			newCount++;
		};

		// Right
		if((nodes[count]->x + 1 >= 0) && !m_data[nodes[count]->y][nodes[count]->x + 1].parent
				&& ((m_data[nodes[count]->y][nodes[count]->x + 1].tileType == TT::TileType::NONE)
				       || (m_data[nodes[count]->y][nodes[count]->x + 1].tileType 
					       == TT::TileType::NONE_WITH_SHADOW)
				       || (m_data[nodes[count]->y][nodes[count]->x + 1].tileType 
					       == TT::TileType::BOMB))) {
			nodes[newCount] = &m_data[nodes[count]->y][nodes[count]->x + 1];
			nodes[newCount]->parent = nodes[count];
			newCount++;
		};
		
		// Down
		if((nodes[count]->y + 1 < m_data.size()) && !m_data[nodes[count]->y + 1][nodes[count]->x].parent
				&& ((m_data[nodes[count]->y + 1][nodes[count]->x].tileType == TT::TileType::NONE)
				       || (m_data[nodes[count]->y + 1][nodes[count]->x].tileType 
					       == TT::TileType::NONE_WITH_SHADOW)
				       || (m_data[nodes[count]->y + 1][nodes[count]->x].tileType 
					       == TT::TileType::BOMB))) {
			nodes[newCount] = &m_data[nodes[count]->y + 1][nodes[count]->x];
			nodes[newCount]->parent = nodes[count];
			newCount++;
		};

		// Left
		if((nodes[count]->x - 1 >= 0) && !m_data[nodes[count]->y][nodes[count]->x - 1].parent
				&& ((m_data[nodes[count]->y][nodes[count]->x - 1].tileType == TT::TileType::NONE)
				       || (m_data[nodes[count]->y][nodes[count]->x - 1].tileType 
					       == TT::TileType::NONE_WITH_SHADOW)
				       || (m_data[nodes[count]->y][nodes[count]->x - 1].tileType 
					       == TT::TileType::BOMB))) {
			nodes[newCount] = &m_data[nodes[count]->y][nodes[count]->x - 1];
			nodes[newCount]->parent = nodes[count];
			newCount++;
		};

		count++;
	};
	
	if(!((int)nodes[count]->tileState & (int)TT::TileState::DANGER)) {
		TT::AITileType* currentTile = nodes[count];
		std::pair<int, int> pair; // first - x, second - y
		while(currentTile != &m_data[y][x]) {
			pair.first = currentTile->x - currentTile->parent->x;
			pair.second = currentTile->y - currentTile->parent->y;
			m_path.push_back(pair);
			currentTile = currentTile->parent;
		};
		std::reverse(m_path.begin(), m_path.end());
	};

	delete[] nodes;
	isTileReached = true;
};

