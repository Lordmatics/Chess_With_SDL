#pragma once
#include "SDL_rect.h"
#include "NMSprite.h"
#include "Coordinate.h"

class IDebuggable
{
	virtual void Debug() = 0;
};

class apiObject : IDebuggable
{
public:

	apiObject();
	virtual ~apiObject();

	NMSprite& GetSprite();
	SDL_Rect& GetTransform();

	void SetPos(int x, int y);
	void SetCoord(Coordinate coord);
	void SetCoord(int x, int y);
	void SetSize(int w, int h);

	virtual void Render(SDL_Renderer* pRenderer) = 0;
	virtual void Debug() override;
protected:

	NMSprite m_graphic;
	SDL_Rect m_transform;
	Coordinate m_boardCoordinate;
};

