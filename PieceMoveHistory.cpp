#include "PieceMoveHistory.h"

PieceMoveData::PieceMoveData(bool wasCapture, const Coordinate& start, const Coordinate& end) :	
	m_resultedInCapture(wasCapture),
	m_previousTile(start),
	m_targetTile(end)
{
}

void PieceMoveHistory::AddMoveData(PieceMoveData& data)
{
	int i = (int)m_history.size() + 1;
	data.m_turn = i;
	m_history.push_back(data);
}
