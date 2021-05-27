#include "Tile.h"
#include "SDL_render.h"
#include "Board.h"
#include <iostream>

Tile::Tile() :
	apiObject(),
	m_iBoardIndex(-1),
	m_occupiedPiece(nullptr),
	m_bHasMoved(false)
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

void Tile::ResetLegalHighlight()
{
	SDL_Rect& transform = GetTransform();
	NMSprite& sprite = GetSprite();
	if (SDL_Texture* pTexture = sprite.GetTexture())
	{
		SDL_SetTextureColorMod(pTexture, 255, 255, 255);
	}
}

void Tile::Render(SDL_Renderer* pRenderer)
{
	SDL_Rect& transform = GetTransform();
	NMSprite& sprite = GetSprite();
	if (SDL_Texture* pTexture = sprite.GetTexture())
	{
		SDL_RenderCopy(pRenderer, pTexture, nullptr, &transform);
	}

	if (Piece* pPiece = m_occupiedPiece)
	{
		//if(pPiece->GetFlags() & (uint32_t)Piece::PieceFlag::Pawn)
		pPiece->Render(pRenderer);
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
