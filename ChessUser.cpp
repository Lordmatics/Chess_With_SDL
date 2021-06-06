#include "ChessUser.h"
#include <iostream>
#include "SDL_render.h"
#include "Board.h"

ChessUser::ChessUser() :
	m_bIsWhite(false),
	m_bIsInCheck(false),
	m_bCheckMated(false),
	m_bStaleMated(false),
	m_bMyTurn(false)
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
			int index = (i * 8) + j;
			if(index < 0 || index >= 16)
				continue;

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

	if (IsMyTurn())
	{		
		if (Piece* pSelectedPiece = m_pSelectedPiece)
		{			
			const std::vector<Tile*>& attacked = pSelectedPiece->GetAttackedTiles();

			m_pBoard->SetHighlightTiles(attacked);
			//for (Tile* pTile : attacked)
			//{
			//	pTile->RenderLegalHighlight(pRenderer);
			//}
			//const int tileID = pSelectedPiece->GetTileIDFromCoord();
			//if (Tile* pTile = m_pBoard->GetTile(tileID))
			//{
			//	pTile->RenderLegalHighlight(pRenderer);
			//}
			pSelectedPiece->RenderAsSelected(pRenderer);			
		}
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

void ChessUser::GetMaterialScore(int& inPlayMaterialScore, int& capturedMaterialScore)
{
	for (int i = 0; i < 16 ; i++)
	{
		const Piece& piece = m_material[i];
		if (piece.IsCaptured())
		{
			capturedMaterialScore += piece.GetValue();
		}
		else
		{
			inPlayMaterialScore += piece.GetValue();
		}
	}
}

bool ChessUser::CanAttackTile(Tile& pTile)
{
	// Iterate over each piece
	// See what moves it can make
	// if one of those moves lands on this tile... (Without putting yourself in check)
	// return true
	for (const Piece& piece : m_material)
	{
		if(piece.IsCaptured())
			continue;

		if (piece.CanAtackCoord(pTile.GetCoordinate()))
		{
			return true;
		}
	}
	return false;
}

bool ChessUser::DetectChecks()
{
	Board* pBoard = m_pBoard;
	if (!pBoard)
		return false;

	// TODO: Need t opopulate list with ALL potential tiles under check

	// IMPLEMENT - ATTACKING REF COUNT FLAG FOR TILE
	// Explore UNDO

	std::vector<Tile*>& checkedTiles = m_pBoard->GetCheckedTiles();
	checkedTiles.clear();

	for (int i = 0; i < 16 ; i++)
	{
		// NOTE: Potential point for optimisation
		// Can probably query coordinates for atleast pawn
		// And rule them out early

		Piece& piece = m_material[i];
		if(piece.IsCaptured())
			continue;

		Coordinate pieceCoord = piece.GetCoordinate();
		//pBoard->ClearLegalMoves();
		pBoard->GenerateLegalMoves(&piece);

		// IF CHECKED BY HORSE
		// ONLY GOT 2 OPTIONS
		// CAPTURE THE HORSE
		// MOVE THE KING
		auto ApplyRule = [&](int loopCount, Coordinate startCoord, Coordinate addedCoord)
		{
			for (int i = 0; i < loopCount; i++)
			{
				const Coordinate& newCoord = startCoord + (addedCoord * (i + 1));
				const int tileID = pBoard->GetTileIDFromCoord(newCoord);
				if (Tile* pInbetweenTile = pBoard->GetTile(tileID))
				{
					checkedTiles.push_back(pInbetweenTile);
				}
			}
		};
		for (Tile* pMove : pBoard->GetValidTiles())
		{
			if(!pMove)
				continue;

			if (Piece* pTarget = pMove->GetPiece())
			{
				Coordinate kingCoord = pTarget->GetCoordinate();
				if (pTarget->GetFlags() & (uint32_t)Piece::PieceFlag::King)
				{
					// !!!
					// NOTE: Probably need to continue calculations for double checks...
					piece.GetCoordinate();
					const int pieceID = pBoard->GetTileIDFromCoord(pieceCoord);
					if (Tile* pAttacker = pBoard->GetTile(pieceID))
					{
						checkedTiles.push_back(pAttacker);
					}

					if (piece.GetFlags() & (uint32_t)Piece::PieceFlag::Horse)
					{
						// Should be able to optimise result here
					}
					else if (piece.GetFlags() & (uint32_t)Piece::PieceFlag::Bishop)
					{
						// Checked by bishop means, get all the tiles diagonally from his coord
						// And the target coord, since blocking any of those will prevent check

						// This will help determine how many iterations we have to check to get all the tiles 
						// Inbetween the king and the bishop
						int yDiff = kingCoord.m_y - pieceCoord.m_y;
						int unmodifiedYDiff = yDiff;
						if (yDiff < 0)
						{
							yDiff *= -1;
						}

						const int loopCount = yDiff - 1;
						Coordinate rules[4] =
						{
							{ 1, -1 },
							{ 1,  1 },
							{-1, -1 },
							{-1,  1 }
						};

						// First Add the bishop itself to the tile list
						// Then add the inbetweens
						//checkedTiles.push_back(pMove);
						if (kingCoord.m_x > pieceCoord.m_x)
						{
							// King is on our right side
							if (kingCoord.m_y < pieceCoord.m_y)
							{
								// King is above us
								// +1 x, -1 Y
								ApplyRule(loopCount, pieceCoord, rules[0]);
							}
							else
							{
								// King is below us
								// +1x, +1y
								ApplyRule(loopCount, pieceCoord, rules[1]);
							}
						}
						else
						{
							// King is on our left side
							if (kingCoord.m_y < pieceCoord.m_y)
							{
								// King is above us
								// -1x, -1y
								ApplyRule(loopCount, pieceCoord, rules[2]);
							}
							else
							{
								// King is below us
								// -1x, +1y
								ApplyRule(loopCount, pieceCoord, rules[3]);
							}
						}
					}
					else if (piece.GetFlags() & (uint32_t)Piece::PieceFlag::Rook)
					{
						// Checked by rook means, get all the tiles cardinally from his coord
						// And the target coord, since blocking any of those will prevent check
											
						// First Add the rook itself to the tile list
						// Then add the inbetweens
						//checkedTiles.push_back(pMove);
						
						//const int loopCount = yDiff - 1;

						if (kingCoord.m_y == pieceCoord.m_y)
						{
							// Same row - Check Horizontal Tiles Between
							int xDiff = kingCoord.m_x - pieceCoord.m_x;
							int unModifiedXDiff = xDiff;
							if (xDiff < 0)
							{
								xDiff *= 1;
							}
							if (unModifiedXDiff > 0)
							{
								// Rook is on our left								
								ApplyRule(xDiff - 1, pieceCoord, { 1,0 });
							}
							else
							{
								// Rook is on our right
								ApplyRule(xDiff - 1, pieceCoord, { -1,0 });
							}
						}
						else if (kingCoord.m_x == pieceCoord.m_x)
						{
							// Same Column - Check Vertical Tiles
							int yDiff = kingCoord.m_y - pieceCoord.m_y;
							int unModifiedYDiff = yDiff;
							if (yDiff < 0)
							{
								yDiff *= 1;
							}
							if (unModifiedYDiff > 0)
							{
								// Rook is above us
								ApplyRule(yDiff - 1, pieceCoord, { 1,0 });
							}
							else
							{
								// Rook is below us
								ApplyRule(yDiff - 1, pieceCoord, { 1,0 });
							}
						}
					}
					return true;
				}
			}
		}
	}
	return false;
}

void ChessUser::Process(float dt, const SDL_Point& mousePos)
{
	if (Piece* pSelectedPiece = m_pSelectedPiece)
	{
		SDL_Rect& transform = pSelectedPiece->GetTransform();
		transform.x = mousePos.x - (transform.w / 2);
		transform.y = mousePos.y - (transform.h / 2);
	}
}

Piece* ChessUser::GetPieces()
{
	return m_material;
}
