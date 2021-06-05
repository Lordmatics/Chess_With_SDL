#include "BasicAI.h"
#include "Board.h"
#include <time.h>
#include <iostream>

BasicAI::BasicAI() :
	ChessUser(),
	m_aiSetting(AIFlags::Novice)
{

}

BasicAI::~BasicAI()
{

}

Piece* BasicAI::MakeMove(SDL_Renderer* pRenderer, Tile& tileOnRelease)
{
	const bool playCompletelyRandomly = true;
	//int frameDelay = 0;

	std::srand((unsigned int)time(NULL));

	//while (true)
	{	


		// TODO: - QUERY THE CURRENT MOVES THE PLAYER CAN MAKE
		// IF ANY OF THEM WOULD INVOLVE A CAPTURE OF OUR KING
		// WE ARE OFFICIALLY IN CHECK
		// USE THIS TO FILTER FOR LEGAL MOVES TO EITHER COMPLETELY BLOCK THE ATTACK
		// OR ESCAPE THE CHECK

		// TODO: NEED CONCEPT OF MAKE MOVE - QUERY POSITION - UNDO MOVE

		//if (frameDelay < 200)
		//{
		//	frameDelay++;
		//	continue;
		//}
		if (Board* pBoard = m_pBoard)
		{
			const std::vector<Tile*>& checkedTiles = pBoard->GetCheckedTilesConst();
			const bool inCheck = (int)checkedTiles.size() > 0;
			Piece* pPieceToPlay = nullptr;
			Tile* pTileToCapture = nullptr;
			float score = 0.0f;
			bool noMoves = true;
			float randomChance = 100.0f;
			for (Piece& piece : m_material)
			{
				if(piece.IsCaptured())
					continue;

				//pBoard->ClearLegalMoves();
				pBoard->GenerateLegalMoves(&piece);

				const std::vector<Tile*> validTiles = pBoard->GetValidTiles();
				if (validTiles.size() <= 0)
				{
					// STaleMate
					//SetStaleMate(true);
					//break;
					continue;
				}
				int queryThisPiece = std::rand() % 100 + 1;
				if (queryThisPiece < 100 - randomChance && !m_disableRandomness)
				{
					continue;
				}

				const bool pieceIsKing = piece.GetFlags() & (uint32_t)Piece::PieceFlag::King;
				const bool attackerIsWhite = piece.GetFlags() & (uint32_t)Piece::PieceFlag::White;
				noMoves = false;

				auto QueryMove = [&](Tile* pTile)
				{
					float tempScore = 0.0f;
					if (playCompletelyRandomly && !m_disableRandomness)
					{
						int randomChanceToAcceptThisMove = std::rand() % 100 + 1;
						if (randomChanceToAcceptThisMove >= 100.0f - randomChance)
						{

							// TODO: NEED TO ADD CHECK TO SEE IF TILE TO CAPTURE IS DEFENDED
							// IF THE ATTACKER IS THE KING AND SAY STOP RIGHT THERE CRIMINAL SCUM
							if (pieceIsKing)
							{
								if(pBoard->IsTileDefended(*pTile, attackerIsWhite))
								{
									return false;
								}
							}
							pPieceToPlay = &piece;
							pTileToCapture = pTile;
							randomChance *= 0.9f;
							return true;
						}
						else
						{
							randomChance *= 0.95f;
						}
					}
					else
					{
						tempScore = pTile->Score(piece);
						if (tempScore > score)
						{
							// TODO: NEED TO ADD CHECK TO SEE IF TILE TO CAPTURE IS DEFENDED
							// IF THE ATTACKER IS THE KING AND SAY STOP RIGHT THERE CRIMINAL SCUM
							if (pieceIsKing)
							{
								if (pBoard->IsTileDefended(*pTile, attackerIsWhite))
								{
									return false;
								}
							}
							score = tempScore;
							pPieceToPlay = &piece;
							pTileToCapture = pTile;
							return true;
						}
					}
					return false;
				};

				bool breakCompletely = false;
				for (Tile* pTile : validTiles)
				{
					if (inCheck)
					{
						// Move HAS to result in a change to one of the checked tiles
						// In order to block / capture the attacker
						for (Tile* pCheckedTile : checkedTiles)
						{
							if (pTile == pCheckedTile)
							{
								if (QueryMove(pTile))
								{
									breakCompletely = true;
									break;
								}
							}
						}
						if (breakCompletely)
							break;
					}
					else
					{
						if (QueryMove(pTile))
						{
							//breakCompletely = true;
							break;
						}
					}
				}
				if (breakCompletely)
					break;
			}

			if (noMoves)
			{
				std::cout << "AI Has No Valid Moves!" << std::endl;
				return nullptr;
			}
			if (pPieceToPlay && pTileToCapture)
			{
				if (Piece* pTarget = pTileToCapture->GetPiece())
				{
					pTarget->SetCaptured(true);
					pTileToCapture->SetPiece(nullptr);
				}
				
				int id = pPieceToPlay->GetTileIDFromCoord();
				if (Tile* pPrevTile = m_pBoard->GetTile(id))
				{
					// Clear piece from tile that we're moving from
					pPrevTile->SetPiece(nullptr);
				}					
				
				const bool isAPawn = pPieceToPlay->GetFlags() & (uint32_t)Piece::PieceFlag::Pawn;
				const bool isTileAPromotionSquare = pTileToCapture->IsPromotionSquare();

				if (isAPawn)
				{	
					if (isTileAPromotionSquare)
					{
						pPieceToPlay->Promote(pRenderer);
					}
					else
					{
						pPieceToPlay->CheckEnpassant(*pTileToCapture, *m_pBoard);
					}
				}
				else
				{
					pPieceToPlay->CheckCastling(*pTileToCapture, *m_pBoard);
				}

				pPieceToPlay->SetCoord(pTileToCapture->GetCoordinate());
				pPieceToPlay->UpdatePosFromCoord();

				//pPieceToPlay->SetMoved(true);

				if (Tile* pDestinationTile = pTileToCapture)
				{
					pDestinationTile->SetPiece(pPieceToPlay);
				}

				//pBoard->
				//pBoard->ClearLegalMoves();
				return pPieceToPlay;				
			}
			else
			{
				SetCheckMated(true);
				std::cout << "CheckMated!" << std::endl;
			}
		}
	}
	return nullptr;
}
