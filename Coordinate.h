#pragma once
struct Coordinate
{
	Coordinate();
	Coordinate(int x, int y);
	Coordinate(const Coordinate& other);

	int m_x;
	int m_y;
};

