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

//void Piece::SetPos(int x, int y)
//{
//	if (ChessUser* pOwner = m_pOwner)
//	{
//		//if (south)
//		//{
//		//	xCoord = j;
//		//	yCoord = 8 + (i - 2);
//		//}
//		//else
//		//{
//		//	// 0 then 1
//		//	// we want 1 then 0
//		//	xCoord = j;
//		//	yCoord = i;//1 - i;
//		//}
//
//		const bool south = pOwner->GetSide() == ChessUser::Side::BOTTOM ? true : false;
//		int i = south ? m_boardCoordinate.m_y + 2 - 8 : m_boardCoordinate.m_y;
//		int j = m_boardCoordinate.m_x;
//		const bool pawnConfig = (i == 0 && south) || (i == 1 && !south);
//		const int innerTilePieceOffset = 16;
//		const int innerTilePawnOffset = 26;
//		const int buffer = pawnConfig ? innerTilePawnOffset : innerTilePieceOffset;
//		const int xOffset = 896 / 2; // Quarter X Reso
//		const int yOffset = 28; // Each tile is 128, so 128 * 8 = 1024. reso = 1920:1080, so 1080 - 1024 = 56, then half top/bot, so 28 each side
//		const int tileSize = 128;
//		int xPos = xOffset + buffer + (j * tileSize);
//		if (south)
//		{
//			int yPos = yOffset + buffer + ((i + 6) * tileSize);
//			x = xPos;
//			y = yPos;
//		}
//		else
//		{
//			int yPos = yOffset + buffer + (i * tileSize);
//			x = xPos;
//			y = yPos;
//		}
//	}
//
//	apiObject::SetPos(x, y);
//}

void Piece::UpdatePosFromCoord()
{
	int x = 0;
	int y = 0;
	if (ChessUser* pOwner = m_pOwner)
	{

		const bool south = pOwner->GetSide() == ChessUser::Side::BOTTOM ? true : false;
		int i = south ? m_boardCoordinate.m_y + 2 - 8 : m_boardCoordinate.m_y;
		int j = m_boardCoordinate.m_x;
		const bool pawnConfig = GetFlags() & (uint32_t)PieceFlag::Pawn;// (i == 0 && south) || (i == 1 && !south);
		const int innerTilePieceOffset = 16;
		const int innerTilePawnOffset = 26;
		const int buffer = pawnConfig ? innerTilePawnOffset : innerTilePieceOffset;
		const int xOffset = 896 / 2; // Quarter X Reso
		const int yOffset = 28; // Each tile is 128, so 128 * 8 = 1024. reso = 1920:1080, so 1080 - 1024 = 56, then half top/bot, so 28 each side
		const int tileSize = 128;
		int xPos = xOffset + buffer + (j * tileSize);
		if (south)
		{
			int yPos = yOffset + buffer + ((i + 6) * tileSize);
			x = xPos;
			y = yPos;
		}
		else
		{
			int yPos = yOffset + buffer + (i * tileSize);
			x = xPos;
			y = yPos;
		}
		SetPos(x, y);
	}
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
	if (IsSelected() || IsCaptured())
		return;

	RenderAsSelected(pRenderer, true);
}

bool Piece::RenderAsSelected(SDL_Renderer* pRenderer, bool internal)
{
	if (IsCaptured())
	{
		return false;
	}
	SDL_Rect& transform = GetTransform();
	NMSprite& sprite = GetSprite();
	if (SDL_Texture* pTexture = sprite.GetTexture())
	{
		SDL_RenderCopy(pRenderer, pTexture, nullptr, &transform);
	}

	if (!internal)
	{

	}
	return true;
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


	const int index = j * (i + 1);
	int xCoord = index % 8;
	int yCoord = (int)index / 8;

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

	const int pieceSize = 96;
	const int pawnSize = 76;
	const int size = pawnConfig ? pawnSize : pieceSize;
	SetSize(size, size);
	//const int innerTilePieceOffset = 16;
	//const int innerTilePawnOffset = 26;
	//const int buffer = pawnConfig ? innerTilePawnOffset : innerTilePieceOffset;
	//const int xOffset = 896 / 2; // Quarter X Reso
	//const int yOffset = 28; // Each tile is 128, so 128 * 8 = 1024. reso = 1920:1080, so 1080 - 1024 = 56, then half top/bot, so 28 each side
	//const int tileSize = 128;
	//int xPos = xOffset + buffer + (j * tileSize);
	//if (south)
	//{
	//	int yPos = yOffset + buffer + ((i + 6) * tileSize);
	//	SetPos(xPos, yPos);
	//}
	//else
	//{
	//	int yPos = yOffset + buffer + (i * tileSize);
	//	SetPos(xPos, yPos);
	//}
	//SetPos(0, 0);
	UpdatePosFromCoord();
	//Debug();
}

void Piece::SetSelected(bool val)
{
	m_bSelected = val;
}

bool Piece::CanCapture(Piece* pTargetPiece)
{
	if (pTargetPiece)
	{
		uint32_t myFlags = GetFlags();
		const bool targetIsWhite = pTargetPiece->GetFlags() & (uint32_t)Piece::PieceFlag::White;
		const bool ownerIsWhite = myFlags & (uint32_t)Piece::PieceFlag::White;
		if (ownerIsWhite == targetIsWhite)
		{
			// Friendly piece, ignore
			return false;
		}

		// Need to apply PieceRules
		if (myFlags & (uint32_t)Piece::PieceFlag::Pawn)
		{
			// Diagonal Coordinates or Enpassant
			Coordinate myCoord = GetCoordinate();
			Coordinate targetCoord = pTargetPiece->GetCoordinate();
			Coordinate targetPrevCoord = pTargetPiece->GetPreviousCoordinate();
			if (myCoord.m_x == targetCoord.m_x)
			{
				// In the same column, ignore
				return false;
			} // Check Enpassant
			else if (myCoord.m_y == targetCoord.m_y &&
				((myCoord.m_x == targetCoord.m_x - 1) || (myCoord.m_x == targetCoord.m_x + 1)) )
			{
				// If we're adjacent - Determine if target had just moved 2 squares
				int diff = targetCoord.m_y - targetPrevCoord.m_y;
				if (diff < 0)
				{
					diff *= -1;
				}

				if (diff == 2)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool Piece::IsSouthPlaying() const
{
	if (ChessUser* pOwner = m_pOwner)
	{
		return pOwner->GetSide() == ChessUser::BOTTOM;
	}
	return true;
}

const bool Piece::HasMoved() const
{
	const Coordinate& coord = GetCoordinate();
	const bool side = IsSouthPlaying();
	const bool isPawn = GetFlags() & (uint32_t)Piece::PieceFlag::Pawn;
	if (side)
	{
		// If we're playing on the bottom
		// We haven't moved if we're on the 6th or 7th row depending on piece
		const int rowToCompare = isPawn ? 6 : 7;
		return coord.m_y != rowToCompare;
	}
	else
	{
		const int rowToCompare = isPawn ? 1 : 0;
		return coord.m_y != rowToCompare;
	}
	return false;
}

int Piece::GetValue() const
{
	if (m_pieceflags & (uint32_t)PieceFlag::None)
	{
		return m_valueMap[Piece::PieceFlag::None];
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Pawn)
	{
		return m_valueMap[Piece::PieceFlag::Pawn];
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::King)
	{
		return m_valueMap[Piece::PieceFlag::King];
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Horse)
	{
		return m_valueMap[Piece::PieceFlag::Horse];
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Bishop)
	{
		return m_valueMap[Piece::PieceFlag::Bishop];
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Rook)
	{
		return m_valueMap[Piece::PieceFlag::Rook];
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Queen)
	{
		return m_valueMap[Piece::PieceFlag::Queen];
	}

	return m_valueMap[Piece::PieceFlag::None];
}
