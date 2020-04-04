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
