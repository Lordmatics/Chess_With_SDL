#pragma once
#include "ChessUser.h"
class BasicAI :
	public ChessUser
{
public:

	BasicAI();
	virtual ~BasicAI();
	
	Piece* MakeMove(SDL_Renderer* pRenderer);
};

