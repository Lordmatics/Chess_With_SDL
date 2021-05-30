#pragma once

#include <map>
#include <vector>
#include "Tile.h"
#include "Piece.h"
#include "ChessUser.h"
#include "Player.h"
#include "BasicAI.h"

struct SDL_Renderer;

const bool m_disableAI = false;

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
	void GenerateLegalMoves(Piece* pSelectedObject);
	void RenderLegalMoves(SDL_Renderer* pRenderer);
	void ClearLegalMoves();

	static std::map<Board::TileType, const char*> m_tileMap;
	int GetTileIDFromCoord(const Coordinate& coord) const;
	void AddPiece(int boardID, Piece* object);

	bool CheckPiece(const Piece& selectedPiece, const Coordinate& coord);
	template<class Function>
	bool TileMatch(const Function& Predicate, int validList = 1);

	void GenerateLegalPawnMoves(Piece& selectedPiece);
	void GenerateLegalKingMoves(Piece& selectedPiece);
	void GenerateLegalHorseMoves(Piece& selectedPiece);
	void GenerateLegalBishopMoves(Piece& selectedPiece);
	void GenerateLegalRookMoves(Piece& selectedPiece);
	void GenerateLegalQueenMoves(Piece& selectedPiece);

	int EvaluatePosition();

	const std::vector<Tile*>& GetValidTiles() const;
	void Test();

	Tile* GetTile(int id);
	void SetPreviouslyMoved(Piece* pSelectedPiece);
	void OnLeftClickDown(SDL_Renderer* pRenderer);
	void OnLeftClickRelease(SDL_Renderer* pRenderer);
	void Process(float dt);
	void SetMousePosition(const SDL_Point& mousePos) { m_mousePosition = mousePos; }

	bool IsPlayersTurn() const;

	const std::vector<Tile*>& GetCheckedTilesConst() { return m_checkedTiles; }
	std::vector<Tile*>& GetCheckedTiles() { return m_checkedTiles; }
	bool IsTileDefended(Tile& pTile, bool attackerIsWhite);
private:
	//Tile m_backgroundTiles[8][8];
	Tile m_board[64]; // Generate the 32 pieces in here, and leave the rest of the tiles intiialised as NONE
	std::vector<Tile*> m_queryingTiles;
	std::vector<Tile*> m_validTiles;
	std::vector<Tile*> m_checkedTiles;
	Player m_player;
	BasicAI m_opponent;
	ChessUser* m_players[MAX_NUM_PLAYERS]; 
	Piece* m_pPreviousMovedPiece;


	bool m_bIgnorePress;
	bool m_playersTurn;
	SDL_Point m_mousePosition;
	SDL_Rect* m_pSelectedRect;
	SDL_Point m_clickOffset;

	SDL_Rect m_resetPos;
	Tile* m_pPiecesTile;
	Piece* m_pSelectedPiece;
	void ClearInput();
public:
	void RunAI(SDL_Renderer* pRenderer, bool& m_playersTurn);
};

template<class Function>
bool Board::TileMatch(const Function& Predicate, int validList)
{
	std::vector<Tile*>& listToUse = validList == 1 ? m_validTiles : m_queryingTiles;
	for (Tile* pTile : listToUse)
	{
		if (Predicate(pTile))
		{
			return true;
		}
	}
	return false;
}

