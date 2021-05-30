#include "Tile.h"
#include "SDL_render.h"
#include "Board.h"
#include <iostream>

Tile::Tile() :
	apiObject(),
	m_iBoardIndex(-1),
	m_occupiedPiece(nullptr)
{

}

Tile::~Tile()
{

}

bool Tile::IsColour(uint32_t flag)
{
	if (m_occupiedPiece)
	{
		if (m_occupiedPiece->GetFlags() & flag)
		{
			return true;
		}
	}
	
	return false;
}

uint32_t Tile::GetFlags() const
{
	if (m_occupiedPiece)
	{
		return m_occupiedPiece->GetFlags();
	}
	return 0;
}

bool Tile::SetFlags(uint32_t flags)
{
	if (m_occupiedPiece)
	{
		m_occupiedPiece->SetFlags(flags);
		return true;
	}
	return false;
}

void Tile::RenderLegalHighlight(SDL_Renderer* pRenderer)
{
	if (!pRenderer)
	{
		return;
	}

	SDL_Rect& transform = GetTransform();
	NMSprite& sprite = GetSprite();
	if (SDL_Texture* pTexture = sprite.GetTexture())
	{
		SDL_SetTextureColorMod(pTexture, 255, 0, 0);
	}
}

void Tile::Init(SDL_Renderer* pRenderer, int boardID)
{
	if (!pRenderer)
		return;

	m_iBoardIndex = boardID;
	// E O E O E O E O
	//if (boardID % 8 == 0)
	//{
	//	boardID = boardID + 1;
	//	// O E O E O E O E
	//}


	// Tile 5
	// Remainder = 5
	// Divisible = 0

				//int temp = i;// +1;
			//x = (temp ) % Board::m_iColumns;
			//y = (int)(temp / Board::m_iColumns);

	int x = boardID % 8;
	int y = (int)boardID / 8;
	m_boardCoordinate = { x, y };
	const int tileSize = 128;
	const int xOffset = 896 / 2; // Quarter X Reso
	const int yOffset = 28; // Each tile is 128, so 128 * 8 = 1024. reso = 1920:1080, so 1080 - 1024 = 56, then half top/bot, so 28 each side

	NMSprite& sprite = GetSprite();
	if (y % 2 == 0)
	{
		if (x % 2 != 0)
		{
			// While J is even, and I is Odd
			// Add Light Tile
			sprite.AssignSprite(pRenderer, Board::m_tileMap[Board::TileType::TILEL]);
		}
		else
		{
			// While J is even, and I is even
			sprite.AssignSprite(pRenderer, Board::m_tileMap[Board::TileType::TILED]);

		}
	}
	else
	{
		if (x % 2 == 0)
		{
			// While J is even, and I is Odd
			// Add Light Tile
			sprite.AssignSprite(pRenderer, Board::m_tileMap[Board::TileType::TILEL]);
		}
		else
		{
			sprite.AssignSprite(pRenderer, Board::m_tileMap[Board::TileType::TILED]);
		}
	}

	//if (boardID % 2 == 0)
	//{
	//	sprite.AssignSprite(pRenderer, Board::m_tileMap[Board::TileType::TILEL]);
	//}
	//else
	//{
	//	sprite.AssignSprite(pRenderer, Board::m_tileMap[Board::TileType::TILED]);
	//}
	SDL_Rect& rect = GetTransform();
	rect.h = s_iTileHeight;
	rect.w = s_iTileWidth;
	rect.x = xOffset + (x * s_iTileWidth);
	rect.y = yOffset + (y * s_iTileHeight);

	Debug();
}

void Tile::AddPiece(Piece* piece)
{
	m_occupiedPiece = piece;
}

void Tile::RemovePiece()
{
	m_occupiedPiece = nullptr;
}

void Tile::SetPiece(Piece* piece)
{
	if (!piece)
	{
		// Piece Has Moved Off of this tile
		volatile int i = 5;
	}
	else
	{
		// Piece has moved onto this tile
		volatile int i = 5;
	}
	m_occupiedPiece = piece;
}

void Tile::Debug()
{
	// Top Left -> Top Right
	// Decrement Row
	SDL_Rect& rect = GetTransform();
	std::cout << "Tile: " << m_iBoardIndex << " Initializing" << "\t";
	apiObject::Debug();

	if (Piece* pPiece = m_occupiedPiece)
	{
		pPiece->Debug();
	}
}

bool Tile::IsDefended()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void Tile::ResetLegalHighlight()
{
	SDL_Rect& transform = GetTransform();
	NMSprite& sprite = GetSprite();
	if (SDL_Texture* pTexture = sprite.GetTexture())
	{
		SDL_SetTextureColorMod(pTexture, 255, 255, 255);
	}
}

bool Tile::HasMoved() const
{
	if (Piece* pPiece = m_occupiedPiece)
	{
		return pPiece->HasMoved();
	}
	return true;
}

void Tile::Render(SDL_Renderer* pRenderer)
{
	RenderPiece(pRenderer);
}

void Tile::RenderBG(SDL_Renderer* pRenderer)
{
	SDL_Rect& transform = GetTransform();
	NMSprite& sprite = GetSprite();
	if (SDL_Texture* pTexture = sprite.GetTexture())
	{
		SDL_RenderCopy(pRenderer, pTexture, nullptr, &transform);
	}
}

void Tile::RenderPiece(SDL_Renderer* pRenderer)
{
	if (Piece* pPiece = m_occupiedPiece)
	{
		//if(pPiece->GetFlags() & (uint32_t)Piece::PieceFlag::Pawn)
		pPiece->Render(pRenderer);
	}
}

float Tile::Score(Piece& piece)
{
	const bool isWhite = piece.GetFlags() & (uint32_t)Piece::PieceFlag::White;
	const bool isSouthPlaying = piece.IsSouthPlaying();
	const Coordinate& coord = piece.GetCoordinate();
	const Coordinate& tileCoord = GetCoordinate();
	// Encourage Centre Play
	// TODO: Needs depth
	// TODO: Needs concept of attacking squares
	// TODO: Need to implement Checks / Fossils / Pins / Discovered Checks
	float centreScore = isSouthPlaying ? 1.0f / (tileCoord.m_y + 1) : 1.0f / (8 - tileCoord.m_y);
	if (Piece* pOccupant = GetPiece())
	{
		// Would be a capture

		return centreScore + 1.0f * pOccupant->GetValue();
	}
	else
	{
		return 1.0f + centreScore;
	}
}

const bool Tile::IsPromotionSquare()
{
	const Coordinate& coord = GetCoordinate();
	{
		if (coord.m_y == 7 || coord.m_y == 0)
		{
			return true;
		}
	}
	return false;
}
