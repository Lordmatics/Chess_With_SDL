#include "Piece.h"
#include <iostream>
#include "ChessUser.h"
#include "Board.h"
#include "SDL_render.h"

std::map<Piece::PieceFlag, int> Piece::m_valueMap =
{
	{ Piece::PieceFlag::None, 0 },
	{ Piece::PieceFlag::Pawn, 1 },
	{ Piece::PieceFlag::Rook, 5 },
	{ Piece::PieceFlag::Horse, 3 },
	{ Piece::PieceFlag::Bishop, 3 },
	{ Piece::PieceFlag::King, 10 },
	{ Piece::PieceFlag::Queen, 9 },
};

Piece::Piece() :
	apiObject(),
	m_pieceflags((uint32_t)PieceFlag::None),
	m_bSelected(false)
{

}

Piece::~Piece()
{

}

const char* Piece::GetPieceName() const
{
	if (m_pieceflags & (uint32_t)PieceFlag::None)
	{
		return "None";
	}
	else if(m_pieceflags & (uint32_t)PieceFlag::Pawn)
	{
		return "Pawn";
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::King)
	{
		return "King";
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Horse)
	{
		return "Horse";
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Bishop)
	{
		return "Bishop";
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Rook)
	{
		return "Rook";
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Queen)
	{
		return "Queen";
	}

	return "Unknown";
}

void Piece::Debug()
{
	const bool playerControlled = dynamic_cast<Player*>(m_pOwner) ? true : false;
	const bool isWhite = m_pieceflags & (uint32_t)PieceFlag::White ? true : false;
	const char* colourTxt = isWhite ? "White" : "Black";
	const char* pieceName = GetPieceName();
	const char* pcText = playerControlled ? "(PC)" : "(AI)";
	std::cout << "Piece Info: Colour: " << colourTxt << " " << pcText << "\t Name: " << pieceName << "\t";
	apiObject::Debug();
}

void Piece::Render(SDL_Renderer* pRenderer)
{
	if (IsSelected())
		return;

	RenderAsSelected(pRenderer);
}

void Piece::RenderAsSelected(SDL_Renderer* pRenderer)
{
	SDL_Rect& transform = GetTransform();
	NMSprite& sprite = GetSprite();
	if (SDL_Texture* pTexture = sprite.GetTexture())
	{
		SDL_RenderCopy(pRenderer, pTexture, nullptr, &transform);
	}
}

void Piece::Init(SDL_Renderer* pRenderer, int i, int j, ChessUser* pOwner)
{
	m_pOwner = pOwner;
	if (!pOwner)
		return;

	//Tile** map = m_pBoard->GetTiles();
	const uint32_t colour = pOwner->IsWhite() ? (uint32_t)Piece::PieceFlag::White : (uint32_t)Piece::PieceFlag::Black;
	int pathID = 0;
	if (colour & (uint32_t)Piece::PieceFlag::Black)
	{
		pathID = 1;
	}
	const bool south = pOwner->GetSide() == ChessUser::Side::BOTTOM ? true : false;
	const int tileSize = 128;
	const int xOffset = 896 / 2; // Quarter X Reso
	const int yOffset = 28; // Each tile is 128, so 128 * 8 = 1024. reso = 1920:1080, so 1080 - 1024 = 56, then half top/bot, so 28 each side
	const int pieceSize = 96;
	const int pawnSize = 76;
	const int innerTilePieceOffset = 16;
	const int innerTilePawnOffset = 26;
	NMSprite& sprite = GetSprite();
	const bool pawnConfig = (i == 0 && south) || (i == 1 && !south);
	if (pawnConfig)
	{
		sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Pawn].m_paths[pathID]);
		SetFlags((uint32_t)Piece::PieceFlag::Pawn | colour);
	}
	else
	{
		if (j == 0 || j == Board::m_iColumns - 1)
		{
			sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Rook].m_paths[pathID]);
			SetFlags((uint32_t)Piece::PieceFlag::Rook | colour);
		}
		else if (j == 1 || j == Board::m_iColumns - 2)
		{
			sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Horse].m_paths[pathID]);
			SetFlags((uint32_t)Piece::PieceFlag::Horse | colour);
		}
		else if (j == 2 || j == Board::m_iColumns - 3)
		{
			sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Bishop].m_paths[pathID]);
			SetFlags((uint32_t)Piece::PieceFlag::Bishop | colour);
		}
		else if (j == 3)
		{
			sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Queen].m_paths[pathID]);
			SetFlags((uint32_t)Piece::PieceFlag::Queen | colour);
		}
		else if (j == 4)
		{
			sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::King].m_paths[pathID]);
			SetFlags((uint32_t)Piece::PieceFlag::King | colour);
		}
	}
	const int size = pawnConfig ? pawnSize : pieceSize;
	SetSize(size, size);
	const int buffer = pawnConfig ? innerTilePawnOffset : innerTilePieceOffset;
	int xPos = xOffset + buffer + (j * tileSize);
	if (south)
	{
		int yPos = yOffset + buffer + ((i + 6) * tileSize);
		SetPos(xPos, yPos);
	}
	else
	{
		int yPos = yOffset + buffer + (i * tileSize);
		SetPos(xPos, yPos);
	}

	const int index = j * (i + 1);
	int xCoord = index % 8;
	int yCoord = (int)index/ 8;

	if (south)
	{
		xCoord = j;
		yCoord = 8 + (i - 2);
	}
	else
	{
		// 0 then 1
		// we want 1 then 0
		xCoord = j;
		yCoord = i;//1 - i;
	}

	SetCoord({ xCoord, yCoord });

	//Debug();
}

void Piece::SetSelected(bool val)
{
	m_bSelected = val;
}
