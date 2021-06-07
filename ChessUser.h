#pragma once
#include "apiObject.h"
#include "Tile.h"
#include <vector>
#include "Piece.h"

struct SDL_Renderer;
class Board;
class ChessUser
{
public:
	ChessUser();
	virtual ~ChessUser();

	enum Side
	{
		BOTTOM,
		TOP,
		UNKNOWN
	};

	bool IsWhite() const { return m_bIsWhite; }
	void SetWhite(bool val) { m_bIsWhite = val; }

	void SetSide(Side targetSide) { m_side = targetSide; }
	Side GetSide() const { return m_side; }

	void Init(SDL_Renderer* pRenderer, Board* board);

	void Render(SDL_Renderer* pRenderer);	

	template<class Function>
	bool VisitMaterial(const Function& func);

	void GetMaterialScore(int& inPlayMaterialScore, int& capturedMaterialScore);

	bool IsInCheck() const { return m_bIsInCheck; }
	void SetInCheck(bool val) { m_bIsInCheck = val; }

	bool IsCheckMated() const { return m_bCheckMated; }
	void SetCheckMated(bool val) { m_bCheckMated = val; }

	bool IsStaleMated() const { return m_bStaleMated; }
	void SetStaleMate(bool val) { m_bStaleMated = val; }

	bool CanAttackTile(Tile& pTile);

	virtual bool DetectChecks();

	virtual Piece* MakeMove(SDL_Renderer* pRenderer, Tile* tileOnRelease) = 0;

	const bool IsMyTurn() const { return m_bMyTurn; }
	void SetMyTurn(bool val) { m_bMyTurn = val; }
	void Process(float dt, const SDL_Point& mousePos);
protected:

	Side m_side;
	Piece m_material[16];
	Board* m_pBoard;

	bool m_bIsWhite;
	bool m_bIsInCheck;
	bool m_bCheckMated;
	bool m_bStaleMated;
	bool m_bMyTurn;

	Piece* m_pSelectedPiece;
	Tile* m_pSelectedPiecesStartingTile;
	SDL_Rect m_restRect;
public:
	Piece* GetPieces();
};

template<class Function>
bool ChessUser::VisitMaterial(const Function& func)
{
	for (int i = 0; i < 16 ; i++)
	{
		Piece& piece = &m_material[i];
		bool success = func(piece);
		if (success)
		{
			return true;
		}
	}
	return false;
}

