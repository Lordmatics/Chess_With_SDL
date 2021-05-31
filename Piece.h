#pragma once
#include "apiObject.h"
#include <map>
#include "PieceMoveHistory.h"

class ChessUser;
class Tile;
class Board;
class Piece :
	public apiObject
{
public:
	Piece();
	virtual ~Piece();

	enum class PieceFlag : uint32_t
	{
		None = (uint32_t)0 << 0,
		Pawn = (uint32_t)1 << 1,
		King = (uint32_t)1 << 2,
		Horse = (uint32_t)1 << 3,
		Bishop = (uint32_t)1 << 4,
		Rook = (uint32_t)1 << 5,
		Queen = (uint32_t)1 << 6,

		White = (uint32_t)1 << 7,
		Black = (uint32_t)1 << 8

	};

	void SetFlags(uint32_t flags) { m_pieceflags = flags; }
	uint32_t GetFlags() const { return m_pieceflags; }

	const char* GetPieceName() const;

	//virtual void SetPos(int x, int y) override;
	void UpdatePosFromCoord();
	virtual void Debug() override;

	static std::map<Piece::PieceFlag, int> m_valueMap;

	void Render(SDL_Renderer* pRenderer) override;
	bool RenderAsSelected(SDL_Renderer* pRenderer, bool internal = false);

	void Init(SDL_Renderer* pRenderer, int i, int j, ChessUser* owner);
	void SetSelected(bool val);
	bool IsSelected() const { return m_bSelected; }
	bool CanCapture(Piece* pTargetPiece);

	bool IsSouthPlaying() const;

	bool IsCaptured() const { return m_bCaptured; }
	void SetCaptured(bool val) { m_bCaptured = val; }

	void SetMoved(bool val) { m_bHasMoved = val; }
	const bool HasMoved() const { return m_bHasMoved; }
	void CheckEnpassant(const Tile& tile, Board& board);
	void CheckCastling(const Tile& pTile, Board& m_board);
	bool CanAtackCoord(const Coordinate& param1) const;
	void OnPieceMoved(const Coordinate& newCoord);
private:
	ChessUser* m_pOwner;
	// Might even change this to store each move, as it's history
	// Need to know where we came from, in the event we want to undo the moves
	Tile* m_pPrevTile;
	PieceMoveHistory m_history;
	uint32_t m_pieceflags;	
	bool m_bSelected;
	bool m_bCaptured;
	bool m_bHasMoved;

	// So Tile will contain the attacked list + anything beyond those obstructed tiles
	// Idea being, we can quickly construct 'safe squares' to handle check better

	// Essentially a list of every tile this piece can currently see
	std::vector<Tile*> m_visibleTiles;
	// Essentially a list of every tile this piece can currently ATTACK
	std::vector<Tile*> m_attackedTiles;
public:
	int GetValue() const;
	void Promote(SDL_Renderer* pRenderer);
};

