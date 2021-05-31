#pragma once
#include "ChessUser.h"

struct SDL_Point;
class Player :
	public ChessUser
{
public:

	Player();
	virtual ~Player();

	void TryGenerateMoves(SDL_Renderer* pRenderer, SDL_Point& mousePos);
	virtual Piece* MakeMove(SDL_Renderer* pRenderer, Tile& tileOnRelease) override;
private:
	void OnPieceSelected(Piece& selectedPiece);

	void ClearSelection(bool snapToStart = true);
};

