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
	const Coordinate& GetCoordinate() const { return m_boardCoordinate; }
	const Coordinate& GetPreviousCoordinate() const { return m_prevCoordinate; }
	virtual void Render(SDL_Renderer* pRenderer) = 0;
	virtual void Debug() override;

	const int GetTileIDFromCoord() const;
protected:

	NMSprite m_graphic;
	SDL_Rect m_transform;
	Coordinate m_boardCoordinate;
	Coordinate m_prevCoordinate;
};

