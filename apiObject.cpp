#include "apiObject.h"
#include <iostream>

apiObject::apiObject() :
	m_graphic(),
	m_transform(),
	m_boardCoordinate(),
	m_prevCoordinate()
{

}

apiObject::~apiObject()
{

}

NMSprite& apiObject::GetSprite()
{
	return m_graphic;
}

SDL_Rect& apiObject::GetTransform()
{
	return m_transform;
}

void apiObject::SetPos(int x, int y)
{
	m_transform.x = x;
	m_transform.y = y;
}

void apiObject::SetCoord(Coordinate coord)
{
	m_prevCoordinate = m_boardCoordinate;
	m_boardCoordinate = coord;
}

void apiObject::SetCoord(int x, int y)
{
	m_prevCoordinate = m_boardCoordinate;
	m_boardCoordinate.m_x = x;
	m_boardCoordinate.m_y = y;
}

void apiObject::SetSize(int w, int h)
{
	m_transform.w = w;
	m_transform.h = h;
}

void apiObject::Debug()
{
	SDL_Rect& rect = GetTransform();
	std::cout << "X: " << rect.x << " (" << m_boardCoordinate.m_x << ") " << "\t";
	std::cout << "Y: " << rect.y << " (" << m_boardCoordinate.m_y << ") " << std::endl;
}

const int apiObject::GetTileIDFromCoord() const
{
	return m_boardCoordinate.m_y * 8 + m_boardCoordinate.m_x;
}
