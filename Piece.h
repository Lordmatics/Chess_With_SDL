#pragma once
#include "apiObject.h"
#include <map>

class ChessUser;
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

	virtual void Debug() override;

	static std::map<Piece::PieceFlag, int> m_valueMap;

	void Render(SDL_Renderer* pRenderer) override;
	void RenderAsSelected(SDL_Renderer* pRenderer);

	void Init(SDL_Renderer* pRenderer, int i, int j, ChessUser* owner);
	void SetSelected(bool val);
	bool IsSelected() const { return m_bSelected; }
private:
	ChessUser* m_pOwner;
	uint32_t m_pieceflags;	
	bool m_bSelected;
};

