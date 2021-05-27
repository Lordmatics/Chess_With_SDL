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

void ChessUser::Init(SDL_Renderer* pRenderer, Board* board)
{
	if (!pRenderer)
	{
		return;
	}

	m_pBoard = board;
	if (!m_pBoard)
	{
		return;
	}
	

	const bool south = GetSide() == ChessUser::Side::BOTTOM ? true : false;

	for (int i = 0; i < 2; i++)
	{
		// 8 Objects
		for (int j = 0; j < Board::m_iColumns; j++)
		{





			/*
				0 + 1 * 0 + 1 = 1 - 1 = 0
				1 + 1 * 0 + 1 = 2 - 1 = 1
				2 + 1 * 0 + 1 = 3 - 1 = 2
				7 + 1 * 0 + 1 = 8 - 1 = 7

				0 + 1 * 1 + 1 = 2 - 1 = 1
				
				
				
			*/


			int index = (i * 8) + j;
			if(index < 0 || index >= 16)
				continue;

			//int index = i == 0 ? ( (j + 1) * (i + 1)) - 1 : (j + 1) * (i+1);// +1);
			Piece& object = m_material[index];
			object.Init(pRenderer, i, j, this);
			// Update Board with Players Piece
			if (south)
			{
				index = 48 + index;
				m_pBoard->AddPiece(index, &object);
			}
			else
			{
				// Need to invert tile id here
				m_pBoard->AddPiece(index, &object);
			}
		}
	}	
}

void ChessUser::Render(SDL_Renderer* pRenderer)
{
	if (!pRenderer)
	{
		std::cout << "Chess User: Failed To Render - Missing Renderer!" << std::endl;
		return;
	}

	//for (int i = 0; i < 2; i++)
	//{
	//	for (int j = 0; j < Board::m_iColumns; j++)
	//	{
	//		SDL_Rect& transform = m_material[i][j].GetTransform();
	//		NMSprite& sprite = m_material[i][j].GetSprite();
	//		if (SDL_Texture* pTexture = sprite.GetTexture())
	//		{
	//			SDL_RenderCopy(pRenderer, pTexture, nullptr, &transform);
	//		}
	//	}
	//}
}