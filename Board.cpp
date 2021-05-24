#include "Board.h"
#include "SDL_render.h"
#include <iostream>

std::map<Board::Tile, const char*> Board::m_tileMap =
{
	{ Board::Tile::TILEL, "Assets/Images/square brown light.png" },
	{ Board::Tile::TILED, "Assets/Images/square brown dark.png" },
};

std::map<Board::Piece, Board::PiecePaths> Board::m_pieceMap =
{
	{ Board::Piece::Pawn, { "Assets/Images/w_pawn.png", "Assets/Images/b_pawn.png"} },
	{ Board::Piece::Rook, { "Assets/Images/w_rook.png", "Assets/Images/b_rook.png"} },
	{ Board::Piece::Horse, { "Assets/Images/w_knight.png", "Assets/Images/b_knight.png"} },
	{ Board::Piece::Bishop, { "Assets/Images/w_bishop.png", "Assets/Images/b_bishop.png"} },
	{ Board::Piece::King, { "Assets/Images/w_king.png", "Assets/Images/b_king.png"} },
	{ Board::Piece::Queen, { "Assets/Images/w_queen.png", "Assets/Images/b_queen.png"} },
};

apiObject* Board::GetTileAtPoint(SDL_Point* point)
{

	for (int i = 0; i < m_iRows; i++)
	{
		for (int j = 0; j < m_iColumns; j++)
		{
			apiObject& object = m_board[i][j];
			SDL_Rect& rect = object.GetTransform();
			if (SDL_PointInRect(point, &rect))
			{
				return &object;
			}
		}
	}
	return nullptr;
}

Board::Board()
{

}

Board::~Board()
{

}

void Board::Init(SDL_Renderer* pRenderer)
{
	if (!pRenderer)
	{
		std::cout << "Board failed to initialize - Missing SDL_Renderer!" << std::endl;
		return;
	}

	// TODO: Make these more consistent dependant on resolution
	const int tileSize = 128;
	const int xOffset = 896 / 2; // Quarter X Reso
	const int yOffset = 28; // Each tile is 128, so 128 * 8 = 1024. reso = 1920:1080, so 1080 - 1024 = 56, then half top/bot, so 28 each side
	
	for (int i = 0; i < m_iRows; i++)
	{
		for (int j = 0; j < m_iColumns; j++)
		{
			apiObject& object = m_board[i][j];
			object.SetSize(tileSize, tileSize);
			NMSprite& sprite = object.GetSprite();
			SDL_Rect& transform = object.GetTransform();
			if (j % 2 == 0)
			{
				if (i % 2 != 0)
				{
					// While J is even, and I is Odd
					// Add Light Tile
					sprite.AssignSprite(pRenderer, m_tileMap[Board::Tile::TILEL]);
				}
				else
				{
					// While J is even, and I is even
					sprite.AssignSprite(pRenderer, m_tileMap[Board::Tile::TILED]);

				}
			}
			else
			{
				if (i % 2 == 0)
				{
					// While J is even, and I is Odd
					// Add Light Tile
					sprite.AssignSprite(pRenderer, m_tileMap[Board::Tile::TILEL]);
				}
				else
				{
					sprite.AssignSprite(pRenderer, m_tileMap[Board::Tile::TILED]);
				}
			}
			int xPos = xOffset + (i * tileSize);
			int yPos = yOffset + (j * tileSize);
			object.SetPos(xPos, yPos);
		}
	}
}

void Board::Render(SDL_Renderer* pRenderer)
{
	if (!pRenderer)
	{
		std::cout << "Board failed to render - Missing SDL_Renderer!" << std::endl;
		return;
	}

	for (int i = 0; i < m_iRows; i++)
	{
		for (int j = 0; j < m_iColumns; j++)
		{
			SDL_Rect& transform = m_board[i][j].GetTransform();
			NMSprite& sprite = m_board[i][j].GetSprite();
			if (SDL_Texture* pTexture = sprite.GetTexture())
			{
				SDL_RenderCopy(pRenderer, pTexture, nullptr, &transform);
			}
		}
	}
}

Board::PiecePaths::PiecePaths(const char* a, const char* b)
{
	m_paths[0] = a;
	m_paths[1] = b;
}

Board::PiecePaths::PiecePaths()
{

}
