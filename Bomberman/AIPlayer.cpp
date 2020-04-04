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

void AIPlayer::run(std::pair<int, int> &input)
{
	//input.first = 1;

	switch(state) {
	case AIPlayerStates::ANALYSE:
		getData();
		getMyPosition();
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
