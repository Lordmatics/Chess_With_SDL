#include "apiObject.h"

apiObject::apiObject()
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

void apiObject::SetSize(int w, int h)
{
	m_transform.w = w;
	m_transform.h = h;
}
