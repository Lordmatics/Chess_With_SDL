#pragma once

#include "apiObject.h"
#include <map>

struct SDL_Renderer;

class Board
{
public:

	Board();
	~Board();

	enum Colour
	{
		WHITE = 0,
		BLACK = 1,
	};

	enum Piece
	{
		Pawn,
		Rook,
		Horse,
		Bishop,
		Queen,
		King
	};

	enum Tile
	{
		TILEL,
		TILED
	};

	void Init(SDL_Renderer* pRenderer);

	void Render(SDL_Renderer* pRenderer);
	
	static const int m_iRows = 8;
	static const int m_iColumns = 8;

	struct PiecePaths
	{	
		PiecePaths();
		PiecePaths(const char* a, const char* b);
		const char* m_paths[2];				
	};

	static std::map<Piece, PiecePaths> m_pieceMap;

	apiObject* GetTileAtPoint(SDL_Point* point);
private:
	static std::map<Board::Tile, const char*> m_tileMap;
	apiObject m_board[8][8];
};

