#include "BasicAI.h"
#include "Board.h"

BasicAI::BasicAI() :
	ChessUser()
{

}

BasicAI::~BasicAI()
{

}

void BasicAI::MakeMove()
{
	int frameDelay = 0;
	while (true)
	{		
		if (frameDelay < 200)
		{
			frameDelay++;
			continue;
		}
		if (Board* pBoard = m_pBoard)
		{
			Piece* pPieceToPlay = nullptr;
			Tile* pTileToCapture = nullptr;
			float score = 0.0f;
			for (Piece& piece : m_material)
			{
				if(piece.IsCaptured())
					continue;

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

			if (pPieceToPlay && pTileToCapture)
			{
				if (Piece* pTarget = pTileToCapture->GetPiece())
				{
					pTarget->SetCaptured(true);
					pTileToCapture->SetPiece(nullptr);
				}

				if (Piece* pPrevTile = pPieceToPlay)
				{
					int id = pPrevTile->GetTileIDFromCoord();
					if (Tile* pPrevTile = m_pBoard->GetTile(id))
					{
						// Clear piece from tile that we're moving from
						pPrevTile->SetPiece(nullptr);
					}					
				}

				pPieceToPlay->SetCoord(pTileToCapture->GetCoordinate());
				pPieceToPlay->UpdatePosFromCoord();

				if (Tile* pDestinationTile = pTileToCapture)
				{
					pDestinationTile->SetPiece(pPieceToPlay);
				}

				//pBoard->
				pBoard->ClearLegalMoves();
				break;
			}
			
		}
	}
}
