#include "BasicAI.h"
#include "Board.h"

BasicAI::BasicAI() :
	ChessUser()
{

}

BasicAI::~BasicAI()
{

}

Piece* BasicAI::MakeMove(SDL_Renderer* pRenderer)
{
	int frameDelay = 0;
	while (true)
	{		
		//if (frameDelay < 200)
		//{
		//	frameDelay++;
		//	continue;
		//}
		if (Board* pBoard = m_pBoard)
		{
			Piece* pPieceToPlay = nullptr;
			Tile* pTileToCapture = nullptr;
			float score = 0.0f;
			bool noMoves = true;
			for (Piece& piece : m_material)
			{
				if(piece.IsCaptured())
					continue;

				noMoves = false;
				pBoard->ClearLegalMoves();
				pBoard->GenerateLegalMoves(&piece);

				const std::vector<Tile*> validTiles = pBoard->GetValidTiles();
				for (Tile* pTile : validTiles)
				{
					float tempScore = 0.0f;
					tempScore = pTile->Score(piece);
					if (tempScore > score)
					{
						score = tempScore;
						pPieceToPlay = &piece;
						pTileToCapture = pTile;
					}
				}
			}

			if (noMoves)
				break;

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
				pBoard->ClearLegalMoves();
				return pPieceToPlay;				
			}
			
		}
	}
	return nullptr;
}
