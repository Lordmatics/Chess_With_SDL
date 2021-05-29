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

Coordinate& Coordinate::operator=(const Coordinate& other)
{	
	m_x = other.m_x;
	m_y = other.m_y;
	return *this;
}

Coordinate Coordinate::operator-(const Coordinate& other)
{
	Coordinate coord;
	coord.m_x = m_x - other.m_x;
	coord.m_y = m_y - other.m_y;
	return coord;
}

Coordinate Coordinate::operator+(const Coordinate& other)
{
	Coordinate coord;
	coord.m_x = m_x + other.m_x;
	coord.m_y = m_y + other.m_y;
	return coord;
}

void Coordinate::operator-=(const Coordinate& other)
{
	m_x -= other.m_x;
	m_y -= other.m_y;
}

void Coordinate::operator+=(const Coordinate& other)
{
	m_x += other.m_x;
	m_y += other.m_y;
}

bool Coordinate::operator==(const Coordinate& other)
{
	return m_x == other.m_x && m_y == other.m_y;
}
