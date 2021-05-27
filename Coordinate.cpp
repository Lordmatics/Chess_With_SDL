#include "Coordinate.h"

Coordinate::Coordinate() :
	m_x(0),
	m_y(0)
{

}

Coordinate::Coordinate(int x, int y) :
	m_x(x),
	m_y(y)
{

}

Coordinate::Coordinate(const Coordinate& other) :
	m_x(other.m_x),
	m_y(other.m_y)
{

}
