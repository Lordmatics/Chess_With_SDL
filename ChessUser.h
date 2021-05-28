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
protected:

	Side m_side;
	// TODO: Need to change this, to reference Tiles on teh Board instead
	std::vector<Tile*> m_materials;
	//Tile m_material[2][8];

	Piece m_material[16];

	Board* m_pBoard;
	bool m_bIsWhite;

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

