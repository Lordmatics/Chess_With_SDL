#pragma once
#include <vector>
#include "Coordinate.h"


struct PieceMoveData
{

	PieceMoveData() {}
	PieceMoveData(bool wasCapture, const Coordinate& start, const Coordinate& end);
	
	int m_turn;
	bool m_resultedInCapture;
	Coordinate m_targetTile;
	Coordinate m_previousTile;
};

class PieceMoveHistory
{
public:
	PieceMoveHistory() {}

	void AddMoveData(PieceMoveData& data);

	std::vector<PieceMoveData> m_history;
};

