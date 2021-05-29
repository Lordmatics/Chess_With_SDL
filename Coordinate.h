#pragma once


struct Coordinate
{
	Coordinate();
	Coordinate(int x, int y);
	Coordinate(const Coordinate& other);

	Coordinate& operator=(const Coordinate& other);
	Coordinate& operator+(const Coordinate& other);
	Coordinate& operator-(const Coordinate& other);
	bool operator ==(const Coordinate& other);
	void operator +=(const Coordinate& other);
	void operator -=(const Coordinate& other);
	int m_x;
	int m_y;
};

struct CoordSet
{
	Coordinate m_coord;
	bool m_bSet;
};