#pragma once
#include "SDL_rect.h"
#include "NMSprite.h"
class apiObject
{
public:

	apiObject();
	virtual ~apiObject();

	NMSprite& GetSprite();
	SDL_Rect& GetTransform();

	void SetPos(int x, int y);
	void SetSize(int w, int h);
private:
	NMSprite m_graphic;
	SDL_Rect m_transform;
};

