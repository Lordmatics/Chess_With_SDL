#include "Player.h"
#include "Board.h"

Player::Player() :
	ChessUser()
{
	
}

Player::~Player()
{

}

void Player::TryGenerateMoves(SDL_Renderer* pRenderer, SDL_Point& mousePos)
{
	Board* pBoard = m_pBoard;
	if (!pBoard)
	{
		return;
	}

	const bool playerIsWhite = IsWhite();
	// Get Tile At Cursor
	if (Tile* pTile = pBoard->GetTileAtPoint(&mousePos))
	{
		if (SDL_Rect* pawnTransform = &pTile->GetTransform())
		{
			if (SDL_PointInRect(&mousePos, pawnTransform))
			{
				// See if Tile contains a PIECE
				if (Piece* pOccupant = pTile->GetPiece())
				{					
					const bool isWhite = pOccupant->GetFlags() & (uint32_t)Piece::PieceFlag::White;
					if (isWhite != playerIsWhite && !m_disableAI)
					{
						// If it contains a piece of opposing colour - cancel the input
						// - DISABLE AI - Allow the player to move the AI Pieces
						ClearSelection(true);
					}
					else
					{
						m_pSelectedPiecesStartingTile = pTile;
						m_pSelectedPiece = pOccupant;
						OnPieceSelected(*pOccupant);
					}
				}
			}
		}
	}
}

void Player::OnPieceSelected(Piece& selectedPiece)
{
	// Get Position of Piece BEFORE we move it
	// So we can snap it back when we cancel the movement
	m_restRect = selectedPiece.GetTransform();
	selectedPiece.SetSelected(true);

	if (IsMyTurn())
	{
		Board* pBoard = m_pBoard;
		if (!pBoard)
		{
			return;
		}

		// This function will set a bunch of data on the selected piece
		// So it can be aware of:
		// Vision
		// Threats
		// Available Squares
		// When we Release - we COMMIT to the move, if it's on a valid square WITHOUT introducing a check on YOUR OWN King
		pBoard->GenerateLegalMoves(&selectedPiece);
	}
}


void Player::ClearSelection(bool snapToStart /*= true*/)
{
	// Undo Move
	if (m_pSelectedPiece)
	{
		m_pSelectedPiece->SetSelected(false);
		if (snapToStart)
		{
			SDL_Rect& transform = m_pSelectedPiece->GetTransform();
			transform.x = m_restRect.x;
			transform.y = m_restRect.y;		
		}
	}
	// Clear Settings
	m_pSelectedPiece = nullptr;
	m_pSelectedPiecesStartingTile = nullptr;
	m_restRect.x = 0;
	m_restRect.y = 0;

	if (m_pBoard)
	{
		m_pBoard->ClearLegalMoves();
	}
}

Piece* Player::MakeMove(SDL_Renderer* pRenderer, Tile& tileOnRelease)
{
	Board* pBoard = m_pBoard;
	if (!pBoard)
	{
		return nullptr;
	}

	if (IsMyTurn())
	{
		
		Piece* pSelectedPiece = m_pSelectedPiece;
		if (!pSelectedPiece)
		{
			// If we don't have a selected piece, there is no move to be made
			return nullptr;
		}
	
		// If The Release Tile is one of the Pieces VALID Tiles
		// Begin to allow the move
		auto tileMatch = [&tileOnRelease](Tile* pOtherTile)
		{
			return &tileOnRelease == pOtherTile;
		};
		if (pBoard->TileMatch(tileMatch, 1))
		{
			const bool isAPawn = pSelectedPiece->GetFlags() & (uint32_t)Piece::PieceFlag::Pawn ? true : false;
			const bool isAPromotionSquare = tileOnRelease.IsPromotionSquare();
			if (Piece* pDefender = tileOnRelease.GetPiece())
			{
				// If there is a piece on the tile
				// Initiate CAPTURE
				pDefender->SetCaptured(true);
				if (isAPawn && isAPromotionSquare)
				{
					// If we captured onto the back rank,
					// Promote
					// TODO: Give player Promotion Choice
					pSelectedPiece->Promote(pRenderer);
				}
			}
			else
			{
				// If there was no defender, we just moving
				// TODO: Ensure we not enabling some discovered Checks
				pSelectedPiece->CheckEnpassant(tileOnRelease, *pBoard);
				if (isAPawn)
				{
					if (isAPromotionSquare)
					{
						pSelectedPiece->Promote(pRenderer);
					}
				}
				else
				{
					pSelectedPiece->CheckCastling(tileOnRelease, *pBoard);
				}
			}

			// Do the Move (or the rest of it, in Enpassant/Castling Scenarios)
			// Piece Is Moving, Clear it from its Tile
			if (Tile* pPrevTile = m_pSelectedPiecesStartingTile)
			{
				pPrevTile->SetPiece(nullptr);
				m_pSelectedPiecesStartingTile = nullptr;
			}
			const Coordinate& releaseCoord = tileOnRelease.GetCoordinate();
			pSelectedPiece->OnPieceMoved(releaseCoord);
			tileOnRelease.SetPiece(pSelectedPiece);
			pBoard->SetPreviouslyMoved(pSelectedPiece);
			ClearSelection(false);

			// TODO: Checks
			// TODO: Trigger AI
			bool ismyTurn = IsMyTurn();
			pBoard->RunAI(pRenderer, ismyTurn);

		}
		else
		{
			ClearSelection(true);
		}
	}
	else
	{
		ClearSelection(true);
	}

	return nullptr;
}