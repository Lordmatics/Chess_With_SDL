#pragma once
#include "ChessUser.h"
class Player :
	public ChessUser
{
public:

	Player();
	virtual ~Player();

	virtual Piece* MakeMove(SDL_Renderer* pRenderer) override;
};

