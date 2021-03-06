#pragma once
#include "apiObject.h"
#include "Piece.h"

class Tile :
	public apiObject
{
public:

	Tile();
	virtual ~Tile();

	bool IsColour(uint32_t flag);

	uint32_t GetFlags() const;
	bool SetFlags(uint32_t flags);

	void RenderLegalHighlight(SDL_Renderer* pRenderer);
	void ResetLegalHighlight();

	bool HasMoved() const;

	void SetBoardIndex(int i) { m_iBoardIndex = i; }
	int GetBoardIndex() const { return m_iBoardIndex; }

	virtual void Render(SDL_Renderer* pRenderer) override;
	void RenderBG(SDL_Renderer* pRenderer);

	void Init(SDL_Renderer* pRenderer, int boardID);
	void AddPiece(Piece* piece);
	void RemovePiece();

	void SetPiece(Piece* piece);
	Piece* GetPiece() const { return m_occupiedPiece; }

	virtual void Debug() override;

	//static const int s_iTileWidth = 128;
	//static const int s_iTileHeight = 128;
	bool IsDefended();
private:

	void RenderPiece(SDL_Renderer* pRenderer);

	Piece* m_occupiedPiece;
	int m_iBoardIndex;
public:
	float Score(Piece& piece);
	const bool IsPromotionSquare();
};

