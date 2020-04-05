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
		};
	};
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
	//input.first = 1;

	switch(state) {
	case AIPlayerStates::ANALYSE:
		getData();
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

void AIPlayer::getData(void)
{
	clearLevelState();
	getMyPosition();
	getEnemyPosition();
	getBombsAndDangerZones();
	getBombPlaces();
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
		+ (int)m_data[y][x].tileState);
};

void AIPlayer::getEnemyPosition(void)
{
	int x, y;
	x = m_player->GetPositionInTilesCoordsX();
	y = m_player->GetPositionInTilesCoordsY();
	m_data[y][x].tileState = (TT::TileState)((int)TT::TileState::PLAYER
		+ (int)m_data[y][x].tileState);
};

void AIPlayer::getBombsAndDangerZones(void)
{
	int x, y;
	std::map<std::pair<int*, int*>, Bomb*>::iterator it;
	for(it = m_bombs->begin(); it != m_bombs->end(); it++) {
		Bomb* tmpBomb = it->second;
		x = tmpBomb->GetPositionInTileCoordinatesX();
		y = tmpBomb->GetPositionInTileCoordinatesY();
		m_data[y][x].tileState = (TT::TileState)((int)TT::TileState::BOMB
			+ (int)m_data[y][x].tileState);

		for(unsigned short i = 0; i < 5; i++) {
			// Up
			if((y - i) >= 0)
				m_data[y - i][x].tileState = (TT::TileState)((int)TT::TileState::DANGER
					+ (int)m_data[y][x].tileState);

			// Down
			if((y + i) < m_data.size())
				m_data[y + i][x].tileState = (TT::TileState)((int)TT::TileState::DANGER
					+ (int)m_data[y][x].tileState);

			// Left
			if((x - i) >= 0)
				m_data[y][x - i].tileState = (TT::TileState)((int)TT::TileState::DANGER
					+ (int)m_data[y][x].tileState);

			// Right
			if((x + i) < m_data[0].size())
				m_data[y][x + i].tileState = (TT::TileState)((int)TT::TileState::DANGER
					+ (int)m_data[y][x].tileState);
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
				for(unsigned short g = 0; g < 5; g++) {
					if((i - g) > 0) {
						if(m_data[i - g][j].tileState == TT::TileState::PLAYER) {
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
						+ (int)m_data[i][j].tileState);
					continue;
				};

				// Down
				bool placeBombDown = true;
				bool playerDown = false;
				for(unsigned short g = 0; g < 5; g++) {
					if((i + g) < (m_data.size() - 1)) {
						if(m_data[i + g][j].tileState == TT::TileState::PLAYER) {
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
						+ (int)m_data[i][j].tileState);
					continue;
				};

				// Left
				bool placeBombLeft = true;
				bool playerLeft = false;
				for(unsigned short g = 0; g < 5; g++) {
					if((j - g) > 0) {
						if(m_data[i][j - g].tileState == TT::TileState::PLAYER) {
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
						+ (int)m_data[i][j].tileState);
					continue;
				};

				// Right
				bool placeBombRight = true;
				bool playerRight = false;
				for(unsigned short g = 0; g < 5; g++) {
					if((j + g) < (m_data[0].size() - 1)) {
						if(m_data[i][j + g].tileState == TT::TileState::PLAYER) {
							playerLeft = true;
							break;
						};

						placeBombLeft *= (m_data[i][j + g].tileType == TT::TileType::NONE)
							|| (m_data[i][j + g].tileType == TT::TileType::NONE_WITH_SHADOW);
					};
				};
				placeBombRight *= playerRight;
				if(placeBombRight) {
					m_data[i][j].tileState = (TT::TileState)((int)TT::TileState::PLACE_FOR_BOMB
						+ (int)m_data[i][j].tileState);
					continue;
				};
			};
		};

};

