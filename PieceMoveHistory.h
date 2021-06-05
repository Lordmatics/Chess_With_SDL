#pragma once
#include <vector>
#include "Coordinate.h"
#include "Singleton.h"


struct PieceMoveData
{

	PieceMoveData() {}
	PieceMoveData(bool wasCapture, const Coordinate& start, const Coordinate& end);
	
	int m_turn;
	bool m_resultedInCapture;
	const char* m_pieceName;
	const char* m_pieceColour;
	Coordinate m_targetTile;
	Coordinate m_previousTile;
};

class PieceMoveHistory : public Singleton<PieceMoveHistory>
{
	friend class Singleton<PieceMoveHistory>;
public:

	PieceMoveHistory() {}

	void AddMoveData(PieceMoveData& data);

	std::vector<PieceMoveData> m_history;
};

