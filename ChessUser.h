#pragma once
#include "apiObject.h"

struct SDL_Renderer;
class ChessUser
{
public:
	ChessUser();
	virtual ~ChessUser();

	enum Side
	{
		BOTTOM,
		TOP
	};

	bool IsWhite() const { return m_bIsWhite; }
	void SetWhite(bool val) { m_bIsWhite = val; }

	void SetSide(Side targetSide) { m_side = targetSide; }
	Side GetSide() const { return m_side; }

	void Init(SDL_Renderer* pRenderer);

	void Render(SDL_Renderer* pRenderer);

protected:

	Side m_side;
	apiObject m_material[2][8];
	bool m_bIsWhite;

};

