#include "ChessUser.h"
#include <iostream>
#include "SDL_render.h"
#include "Board.h"

ChessUser::ChessUser()
{

}

ChessUser::~ChessUser()
{

}

void ChessUser::Init(SDL_Renderer* pRenderer)
{
	if (!pRenderer)
	{
		return;
	}
	
	const Board::Colour colour = IsWhite() ? Board::Colour::WHITE : Board::Colour::BLACK;
	const bool south = GetSide() == Side::BOTTOM ? true : false;

	const int tileSize = 128;
	const int xOffset = 896 / 2; // Quarter X Reso
	const int yOffset = 28; // Each tile is 128, so 128 * 8 = 1024. reso = 1920:1080, so 1080 - 1024 = 56, then half top/bot, so 28 each side
	const int pieceSize = 96;
	const int pawnSize = 76;
	const int innerTilePieceOffset = 16;
	const int innerTilePawnOffset = 26;
	for (int i = 0; i < 2; i++)
	{
		// 8 Objects
		for (int j = 0; j < Board::m_iColumns; j++)
		{
			apiObject& object = m_material[i][j];
			NMSprite& sprite = object.GetSprite();
			if (i == 0)
			{
				sprite.AssignSprite(pRenderer, Board::m_pieceMap[Board::Piece::Pawn].m_paths[colour]);
			}
			else
			{
				if (j == 0 || j == Board::m_iColumns - 1)
				{
					sprite.AssignSprite(pRenderer, Board::m_pieceMap[Board::Piece::Rook].m_paths[colour]);
				}
				else if (j == 1 || j == Board::m_iColumns - 2)
				{
					sprite.AssignSprite(pRenderer, Board::m_pieceMap[Board::Piece::Horse].m_paths[colour]);
				}
				else if (j == 2 || j == Board::m_iColumns - 3)
				{
					sprite.AssignSprite(pRenderer, Board::m_pieceMap[Board::Piece::Bishop].m_paths[colour]);
				}
				else if (j == 3)
				{
					sprite.AssignSprite(pRenderer, Board::m_pieceMap[Board::Piece::Queen].m_paths[colour]);
				}
				else if (j == 4)
				{
					sprite.AssignSprite(pRenderer, Board::m_pieceMap[Board::Piece::King].m_paths[colour]);
				}
			}
			const int size = i == 0 ? pawnSize : pieceSize;
			object.SetSize(size, size);
			const int buffer = i == 0 ? innerTilePawnOffset : innerTilePieceOffset;
			int xPos = xOffset + buffer + (j * tileSize);
			if (south)
			{
				int yPos = yOffset + buffer + ((i + 6) * tileSize);
				object.SetPos(xPos, yPos);
			}
			else
			{
				int yPos = 0;
				if (i == 0)
				{
					yPos = yOffset + buffer + (i + 1) * tileSize;
				}
				else
				{
					yPos = yOffset + buffer + (i - 1) * tileSize;
				}
				object.SetPos(xPos, yPos);
			}
		}
	}	
}

void ChessUser::Render(SDL_Renderer* pRenderer)
{
	if (!pRenderer)
	{
		std::cout << "Chess User: Failed To Render - Missing Renderer!" << std::endl;
	}

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < Board::m_iColumns; j++)
		{
			SDL_Rect& transform = m_material[i][j].GetTransform();
			NMSprite& sprite = m_material[i][j].GetSprite();
			if (SDL_Texture* pTexture = sprite.GetTexture())
			{
				SDL_RenderCopy(pRenderer, pTexture, nullptr, &transform);
			}
		}
	}
}

apiObject* ChessUser::GetPawns()
{
	return m_material[0];
}

apiObject* ChessUser::GetPieces()
{
	return m_material[1];
}

void ChessUser::VisitMaterial()
{

}
