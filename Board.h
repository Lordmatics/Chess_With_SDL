#pragma once

#include <map>
#include <vector>
#include "Tile.h"
#include "Piece.h"
#include "ChessUser.h"
#include "Player.h"
#include "BasicAI.h"

struct SDL_Renderer;

class Board
{
public:

	Board();
	~Board();
	
	enum Colour
	{
		WHITE = 8,
		BLACK = 16,
	};

	enum TileType
	{
		TILEL,
		TILED
	};

	void Init(SDL_Renderer* pRenderer);

	void Render(SDL_Renderer* pRenderer);
	
	static const int m_iRows = 8;
	static const int m_iColumns = 8;
	const static int MAX_NUM_PLAYERS = 2;

	struct PiecePaths
	{	
		PiecePaths();
		PiecePaths(const char* a, const char* b);
		const char* m_paths[2];				
	};

	static std::map<Piece::PieceFlag, PiecePaths> m_pieceMap;

	Tile* GetTileAtPoint(SDL_Point* point, int startIndex = 0);
	Piece* GetPieceAtPoint(SDL_Point* point, int startIndex = 0);
	void GenerateLegalMoves(Tile* pSelectedObject);
	void RenderLegalMoves(SDL_Renderer* pRenderer);
	void ClearLegalMoves();

	static std::map<Board::TileType, const char*> m_tileMap;

	void AddPiece(int boardID, Piece* object);
private:
	//Tile m_backgroundTiles[8][8];
	Tile m_board[64]; // Generate the 32 pieces in here, and leave the rest of the tiles intiialised as NONE
	std::vector<Tile*> m_queryingTiles;
	Player m_player;
	BasicAI m_opponent;
	ChessUser* m_players[MAX_NUM_PLAYERS]; 
};

