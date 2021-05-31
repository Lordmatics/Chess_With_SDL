#pragma once
#include "ChessUser.h"

const bool m_disableRandomness = true;

class BasicAI :
	public ChessUser
{
public:

	BasicAI();
	virtual ~BasicAI();
	
	enum AIFlags
	{
		Novice = 0 << 0,
		Beginner = 1 << 0,
		Intermediate = 1 << 1,
		Advanced = 1 << 2,
		Expert = 1 << 3,
		SuperGM = 1 << 4
	};

	Piece* MakeMove(SDL_Renderer* pRenderer, Tile& tileOnRelease) override;

	AIFlags GetAILevel() const { return m_aiSetting; }


private:
	AIFlags m_aiSetting;
};

