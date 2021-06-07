#include "Piece.h"
#include <iostream>
#include "ChessUser.h"
#include "Board.h"
#include "SDL_render.h"
#include "Utils.h"
#include "PieceMoveHistory.h"
#include "GameLoop.h"

std::map<Piece::PieceFlag, int> Piece::m_valueMap =
{
	{ Piece::PieceFlag::None, 0 },
	{ Piece::PieceFlag::Pawn, 1 },
	{ Piece::PieceFlag::Rook, 5 },
	{ Piece::PieceFlag::Horse, 3 },
	{ Piece::PieceFlag::Bishop, 3 },
	{ Piece::PieceFlag::King, 10 },
	{ Piece::PieceFlag::Queen, 9 },
};

Piece::Piece() :
	apiObject(),
	m_pieceflags((uint32_t)PieceFlag::None),
	m_bSelected(false)
{

}

Piece::~Piece()
{

}

const char* Piece::GetPieceName() const
{
	if (m_pieceflags & (uint32_t)PieceFlag::None)
	{
		return "None";
	}
	else if(m_pieceflags & (uint32_t)PieceFlag::Pawn)
	{
		return "Pawn";
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::King)
	{
		return "King";
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Horse)
	{
		return "Horse";
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Bishop)
	{
		return "Bishop";
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Rook)
	{
		return "Rook";
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Queen)
	{
		return "Queen";
	}

	return "Unknown";
}

void Piece::UpdatePosFromCoord()
{
	int x = 0;
	int y = 0;
	if (ChessUser* pOwner = m_pOwner)
	{
		const bool south = pOwner->GetSide() == ChessUser::Side::BOTTOM ? true : false;
		int i = south ? m_boardCoordinate.m_y + 2 - 8 : m_boardCoordinate.m_y;
		int j = m_boardCoordinate.m_x;
		const bool pawnConfig = GetFlags() & (uint32_t)PieceFlag::Pawn;// (i == 0 && south) || (i == 1 && !south);
		const int innerTilePieceOffset = GameLoop::s_pieceOffset;// 16;
		const int innerTilePawnOffset = GameLoop::s_pawnOffset;// 26;
		const int buffer = pawnConfig ? innerTilePawnOffset : innerTilePieceOffset;
		//const int xOffset = 896 / 2; // Quarter X Reso
		//const int yOffset = 28; // Each tile is 128, so 128 * 8 = 1024. reso = 1920:1080, so 1080 - 1024 = 56, then half top/bot, so 28 each side
		//const int tileSize = 128;
		//const int tileSize = GameLoop::s_tileSize; //(int)(GameLoop::s_width / 16);// (int)(GameLoop::s_height / 8.4375f);// 128;
		//const int xOffset = GameLoop::s_width / 4;//  896 / 2; // Quarter X Reso
		//const int yOffset = (GameLoop::s_height - (tileSize * 9)) / 2;// 28; // Each tile is 128, so 128 * 8 = 1024. reso = 1920:1080, so 1080 - 1024 = 56, then half top/bot, so 28 each side
		const int tileSize = GameLoop::s_tileSize;// (int)(GameLoop::s_width / 16);// (int)(GameLoop::s_height / 8.4375f);// 128;
		const int xOffset = GameLoop::s_xOffset;// GameLoop::s_width / 4;//  896 / 2; // Quarter X Reso
		const int yOffset = GameLoop::s_yOffset;// (GameLoop::s_height - (tileSize * 9)) / 2;// 28; // Each tile is 128, so 128 * 8 = 1024. reso = 1920:1080, so 1080 - 1024 = 56, then half top/bot, so 28 each side


		int xPos = xOffset + buffer + (j * tileSize);
		if (south)
		{
			int yPos = yOffset + buffer + ((i + 6) * tileSize);
			x = xPos;
			y = yPos;
		}
		else
		{
			int yPos = yOffset + buffer + (i * tileSize);
			x = xPos;
			y = yPos;
		}
		SetPos(x, y);
	}
}

void Piece::Debug()
{
	const bool playerControlled = dynamic_cast<Player*>(m_pOwner) ? true : false;
	const bool isWhite = m_pieceflags & (uint32_t)PieceFlag::White ? true : false;
	const char* colourTxt = isWhite ? "White" : "Black";
	const char* pieceName = GetPieceName();
	const char* pcText = playerControlled ? "(PC)" : "(AI)";
	std::cout << "Piece Info: Colour: " << colourTxt << " " << pcText << "\t Name: " << pieceName << "\t";
	apiObject::Debug();
}

void Piece::Render(SDL_Renderer* pRenderer)
{
	if (IsSelected() || IsCaptured())
		return;

	RenderAsSelected(pRenderer, true);
}

bool Piece::RenderAsSelected(SDL_Renderer* pRenderer, bool internal)
{
	if (IsCaptured())
	{
		return false;
	}
	SDL_Rect& transform = GetTransform();
	NMSprite& sprite = GetSprite();
	if (SDL_Texture* pTexture = sprite.GetTexture())
	{
		SDL_RenderCopy(pRenderer, pTexture, nullptr, &transform);
	}

	if (!internal)
	{

	}
	return true;
}

void Piece::Init(SDL_Renderer* pRenderer, int i, int j, ChessUser* pOwner)
{
	m_pOwner = pOwner;
	if (!pOwner)
		return;

	//Tile** map = m_pBoard->GetTiles();
	const uint32_t colour = pOwner->IsWhite() ? (uint32_t)Piece::PieceFlag::White : (uint32_t)Piece::PieceFlag::Black;
	int pathID = 0;
	if (colour & (uint32_t)Piece::PieceFlag::Black)
	{
		pathID = 1;
	}
	const bool south = pOwner->GetSide() == ChessUser::Side::BOTTOM ? true : false;
	NMSprite& sprite = GetSprite();
	const bool pawnConfig = (i == 0 && south) || (i == 1 && !south);
	if (pawnConfig)
	{
		sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Pawn].m_paths[pathID]);
		SetFlags((uint32_t)Piece::PieceFlag::Pawn | colour);
	}
	else
	{
		if (j == 0 || j == Board::m_iColumns - 1)
		{
			sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Rook].m_paths[pathID]);
			SetFlags((uint32_t)Piece::PieceFlag::Rook | colour);
		}
		else if (j == 1 || j == Board::m_iColumns - 2)
		{
			sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Horse].m_paths[pathID]);
			SetFlags((uint32_t)Piece::PieceFlag::Horse | colour);
		}
		else if (j == 2 || j == Board::m_iColumns - 3)
		{
			sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Bishop].m_paths[pathID]);
			SetFlags((uint32_t)Piece::PieceFlag::Bishop | colour);
		}
		else if (j == 3)
		{
			sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Queen].m_paths[pathID]);
			SetFlags((uint32_t)Piece::PieceFlag::Queen | colour);
		}
		else if (j == 4)
		{
			sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::King].m_paths[pathID]);
			SetFlags((uint32_t)Piece::PieceFlag::King | colour);
		}
	}


	const int index = j * (i + 1);
	int xCoord = index % 8;
	int yCoord = (int)index / 8;

	if (south)
	{
		xCoord = j;
		yCoord = 8 + (i - 2);
	}
	else
	{
		// 0 then 1
		// we want 1 then 0
		xCoord = j;
		yCoord = i;//1 - i;
	}

	SetCoord({ xCoord, yCoord });

	const int pieceSize = GameLoop::s_pieceSize;// 96;
	const int pawnSize = GameLoop::s_pawnSize;// 76;
	const int size = pawnConfig ? pawnSize : pieceSize;
	SetSize(size, size);
	UpdatePosFromCoord();
}

void Piece::SetSelected(bool val, Board& board)
{
	m_bSelected = val;
	if (val)
	{
		OnPieceSelected();
	}
	else
	{
		// clear selection highlight
		board.UnhighlightTiles();
		//const std::vector<Tile*>& attacked = GetAttackedTiles();
		//for (Tile* pTile : attacked)
		//{
		//	pTile->ResetLegalHighlight();
		//}
	}
}

bool Piece::CanCapture(const Piece* pTargetPiece) const
{
	if (pTargetPiece)
	{
		uint32_t myFlags = GetFlags();
		const bool targetIsWhite = pTargetPiece->GetFlags() & (uint32_t)Piece::PieceFlag::White;
		const bool ownerIsWhite = myFlags & (uint32_t)Piece::PieceFlag::White;
		if (ownerIsWhite == targetIsWhite)
		{
			// Friendly piece, ignore
			return false;
		}

		// Need to apply PieceRules
		if (myFlags & (uint32_t)Piece::PieceFlag::Pawn)
		{
			// Diagonal Coordinates or Enpassant
			Coordinate myCoord = GetCoordinate();
			Coordinate targetCoord = pTargetPiece->GetCoordinate();
			Coordinate targetPrevCoord = pTargetPiece->GetPreviousCoordinate();
			if (myCoord.m_x == targetCoord.m_x)
			{
				// In the same column, ignore
				return false;
			} // Check Enpassant
			else if (myCoord.m_y == targetCoord.m_y &&
				((myCoord.m_x == targetCoord.m_x - 1) || (myCoord.m_x == targetCoord.m_x + 1)) )
			{
				// If we're adjacent - Determine if target had just moved 2 squares
				int diff = targetCoord.m_y - targetPrevCoord.m_y;
				if (diff < 0)
				{
					diff *= -1;
				}

				if (diff == 2)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool Piece::IsSouthPlaying() const
{
	if (ChessUser* pOwner = m_pOwner)
	{
		return pOwner->GetSide() == ChessUser::BOTTOM;
	}
	return true;
}

void Piece::SetCaptured(bool val)
{
	m_bCaptured = val;
	const char* chessUserName = Utils::GetChessUser(m_pOwner);	
	const Coordinate& coord = GetCoordinate();
	std::cout << chessUserName << "[Piece::SetCaptured] @ (" << coord.m_x << " , " << coord.m_y << ")" << std::endl;
}

void Piece::CheckEnpassant(const Tile& tileToCapture, Board& board)
{
	const bool isAPawn = GetFlags() & (uint32_t)Piece::PieceFlag::Pawn;
	if(!isAPawn)
	{
		return;
	}

	// Don't do it, if we have confirmed a normal capture
	if (Piece* pTargetPiece = tileToCapture.GetPiece())
	{
		return;
	}

	Coordinate targetTile = tileToCapture.GetCoordinate();
	Coordinate myCoord = GetCoordinate();
	const bool side = IsSouthPlaying();
	const int extra = side ? -1 : 1;

	const bool isBlack = GetFlags() & (uint32_t)Piece::PieceFlag::Black;
	if (side)
	{
		// South side
		if (myCoord.m_y != 3)
		{
			return;
		}
	}
	else
	{
		// North Side
		if (myCoord.m_y != 4)
		{
			return;
		}
	}


	if (targetTile.m_y == (myCoord.m_y + extra) && targetTile.m_x != myCoord.m_x)
	{
		if (side)
		{
			targetTile.m_y += 1;
		}
		else
		{
			targetTile.m_y -= 1;
		}
		const int tileID = board.GetTileIDFromCoord(targetTile);
		if (Tile* pEnpassantTile = board.GetTile(tileID))
		{
			if (Piece* pEnpassantPiece = pEnpassantTile->GetPiece())
			{
				const bool enpassantIsBlack = pEnpassantPiece->GetFlags() & (uint32_t)Piece::PieceFlag::Black;
				if(isBlack == enpassantIsBlack)
				{
					// Don't consume yourself
					return;
				}
				if (pEnpassantPiece->GetFlags() & (uint32_t)Piece::PieceFlag::Pawn)
				{
					// Destroy
					pEnpassantTile->SetPiece(nullptr);
					std::cout << "Enpassant Capture!!! - ";
					pEnpassantPiece->SetCaptured(true);
				}
			}
		}
	}
}

void Piece::CheckCastling(const Tile& pTile, Board& m_board)
{
	const bool isAKing = GetFlags() & (uint32_t)Piece::PieceFlag::King;
	if (!isAKing)
	{
		return;
	}

	// Consider Castling
	Coordinate temp = pTile.GetCoordinate();
	Coordinate temp2 = GetCoordinate();
	int xDiff = temp.m_x - temp2.m_x;
	int unModifiedXDiff = xDiff;
	int currentRrookX = 0;
	int targetRookX = 0;
	if (xDiff < 0)
	{
		currentRrookX = temp2.m_x - 4;
		targetRookX = temp2.m_x - 1;
		xDiff *= -1;
	}
	else
	{
		currentRrookX = temp2.m_x + 3;
		targetRookX = temp2.m_x + 1;
	}
	if (xDiff == 2)
	{
		// Castling in motion
		// Determine which rook needs to move.
		Coordinate rookCoord(currentRrookX, temp2.m_y);
		int rookTileID = m_board.GetTileIDFromCoord(rookCoord);
		if (Tile* pRookTile = m_board.GetTile(rookTileID))
		{
			if (Piece* pRook = pRookTile->GetPiece())
			{
				if (pRook->GetFlags() & (uint32_t)Piece::PieceFlag::Rook)
				{
					// Need to tell previous tile that we're gone
					// And New tile that we have arrived			

					pRookTile->SetPiece(nullptr);

					// Need to calculate where the rook is ending up
					Coordinate targetRookCoord(targetRookX, temp2.m_y);
					pRook->SetCoord({ targetRookX, temp2.m_y });
					pRook->UpdatePosFromCoord();
					if (Tile* pDestinationTile = m_board.GetTile(m_board.GetTileIDFromCoord(targetRookCoord)))
					{
						pDestinationTile->SetPiece(pRook);
					}
					std::cout << "Castled - Rook From (" << rookCoord.m_x << " , " << rookCoord.m_y << ") to (" << targetRookX << " , " << temp2.m_y << ")" << std::endl;
				}
			}
		}
	}
}

bool Piece::CanAtackCoord(const Coordinate& target) const
{
	const Coordinate& myCoord = GetCoordinate();
	const int myX = myCoord.m_x;
	const int myY = myCoord.m_y;

	const int targetX = target.m_x;
	const int targetY = target.m_y;	

	const bool isSouthPlaying = IsSouthPlaying();
	if (m_pieceflags & (uint32_t)PieceFlag::Pawn)
	{
		if (isSouthPlaying)
		{
			// Pawns move north
			// Can Defend if target is diagonally by 1 square
			if (targetX == myX - 1 || targetX == myX + 1)
			{
				if (targetY == myY - 1)
				{
					return true;
				}
			}
		}
		else
		{
			// Pawns move south
			// Can Defend if target is diagonally by 1 square
			if (targetX == myX - 1 || targetX == myX + 1)
			{
				if (targetY == myY + 1)
				{
					return true;
				}
			}
		}
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::King)
	{
		// King is hard to tell if they can defend, because they won't be able to defend
		// If it puts themselves in check... for now, let's not let kings defend
		return false;
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Horse)
	{
		// horse can defend in an L Shape
		if (myX == targetX + 1)
		{
			// If it's to my right
			if (myY == targetY + 2 ||
				myY == targetY - 2)
			{
				return true;
			}
		}
		else if (myX == targetX + 2)
		{
			// Further to my right
			if (myY == targetY + 1 ||
				myY == targetY - 1)
			{
				return true;
			}
		}
		else if (myX == targetX - 1)
		{
			// If its to my left
			if (myY == targetY + 2 ||
				myY == targetY - 2)
			{
				return true;
			}
		}
		else if (myX == targetX - 2)
		{
			// further to my left
			if (myY == targetY + 1 ||
				myY == targetY - 1)
			{
				return true;
			}
		}
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Bishop)
	{
		// Bishop can defend the diagonals
		// so as long as the gradient is uniform
		// We can defend
		// Will need to see if any tile along that gradient is blocking though

	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Rook)
	{
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Queen)
	{
	}
	return false;
}

void Piece::OnPieceSelected()
{

}

void Piece::OnPieceUpdated()
{
	int numUntilKing = 0;
	for (Tile* pTile : m_visibleTiles)
	{
		if (!pTile)
			continue;

		if (Piece* pPiece = pTile->GetPiece())
		{
			const bool isPinning = IsPinning(*pPiece, numUntilKing);
			if (isPinning)
			{
				std::cout << "King IS Pinned" << std::endl;
			}
			//if (pPiece->IsEnemy(GetFlags()))
			//{
			//	if (pPiece->GetFlags() & (uint32_t)Piece::PieceFlag::King)
			//	{
			//		if (numUntilKing == 1 && CanCapture(pPiece))
			//		{
			//			std::cout << "King IS Pinned" << std::endl;
			//		}
			//	}
			//	
			//	if(logVisibility)
			//		std::cout << GetInfo() << " - I can See: " << pPiece->GetInfo() << std::endl;

			//	numUntilKing++;
			//}
		}
	}

	for (Tile* pTile : m_attackedTiles)
	{
		if(!pTile)
			continue;

		if (Piece* pPiece = pTile->GetPiece())
		{
			if (pPiece->IsEnemy(GetFlags()))
			{
				std::cout << GetInfo() << " - I can Attack: " << pPiece->GetInfo() << std::endl;

				if (pPiece->GetFlags() & (uint32_t)Piece::PieceFlag::King)
				{					
					std::cout << "Enemy King IS in check!" << std::endl;					
				}
			}
		}
	}
}

void Piece::OnPieceMoved(const Coordinate& newCoord, bool resultedInCapture, int turn)
{
	const Coordinate prevCoord = GetCoordinate();
	const char* pieceColour = GetFlags() & (uint32_t)Piece::PieceFlag::White ? "W" : "B";
	if (ChessUser* pOwner = m_pOwner)
	{
		if (Player* pPlayer = dynamic_cast<Player*>(pOwner))
		{
			std::cout << "[Player] OnPieceMoved: " << GetPieceName() << " " << pieceColour << " (" << prevCoord.m_x << " , " << prevCoord.m_y << ") --> (" << newCoord.m_x << " , " << newCoord.m_y << ")" << std::endl;
		}
		else if (BasicAI* pPlayer = dynamic_cast<BasicAI*>(pOwner))
		{
			std::cout << "[AI] OnPieceMoved: " << GetPieceName() << " " << pieceColour << " (" << prevCoord.m_x << " , " << prevCoord.m_y << ") --> (" << newCoord.m_x << " , " << newCoord.m_y << ")" << std::endl;
		}
	}
	SetCoord(newCoord);
	UpdatePosFromCoord();

	// Regenerate Lists	

	//TODO: Improve Capture Data
	PieceMoveHistory& history = PieceMoveHistory::GetInstance();
	PieceMoveData data;
	data.m_previousTile = prevCoord;
	data.m_targetTile = newCoord;
	data.m_pieceName = GetPieceName();
	data.m_pieceColour = pieceColour;
	data.m_resultedInCapture = resultedInCapture;
	data.m_turn = turn;
	history.AddMoveData(data);
}

void Piece::UpdateVisibileTiles(const std::vector<Tile*>& queryTiles)
{
	m_visibleTiles = queryTiles;
}

void Piece::UpdateAttackedTiles(const std::vector<Tile*>& attackedTiles)
{
	m_attackedTiles = attackedTiles;
}

void Piece::UpdateCheckedTiles(const std::vector<Tile*>& checkedTiles)
{
	m_checkedTiles = checkedTiles;
}

void Piece::UpdateFriendlyTiles(const std::vector<Tile*>& friendlyTiles)
{
	m_friendlies = friendlyTiles;
}

void Piece::ClearAttackedTiles()
{
	m_attackedTiles.clear();
}

bool Piece::IsEnemy(uint32_t param1) const
{
	const bool attackerColour = param1 & (uint32_t)Piece::PieceFlag::White;
	const bool victimColour = GetFlags() & (uint32_t)Piece::PieceFlag::White;
	if (attackerColour != victimColour)
	{
		return true;
	}
	return false;
}

const std::string& Piece::GetInfo()
{
	const char* pieceName = GetPieceName();
	const char* pieceColour = GetFlags() & (uint32_t)Piece::PieceFlag::White ? "W" : "B";
	const Coordinate& position = GetCoordinate();
	std::string info;
	info.append(pieceName);
	const char* unformatted = " [%s] @ (%d , %d)";
	const std::string& formatted = Utils::FormatString(unformatted,pieceColour, position.m_x, position.m_y);
	info.append(formatted.c_str());
	//info.append(std::format("@ ({} , {})", position.m_x, position.m_y)); // C++ 20 only
	m_pieceInfo = info;
	return m_pieceInfo;
}

bool Piece::IsPinning(const Piece& target, int& numUntilKing)
{
	if (target.IsEnemy(GetFlags()))
	{
		if (target.GetFlags() & (uint32_t)Piece::PieceFlag::King)
		{
			if (numUntilKing == 1 && CanCapture(&target))
			{					
				return true;
			}
		}
	}
	numUntilKing++;
	return false;
}

//const bool Piece::HasMoved() const
//{
//	const Coordinate& coord = GetCoordinate();
//	const bool side = IsSouthPlaying();
//	const bool isPawn = GetFlags() & (uint32_t)Piece::PieceFlag::Pawn;
//	if (side)
//	{
//		// If we're playing on the bottom
//		// We haven't moved if we're on the 6th or 7th row depending on piece
//		const int rowToCompare = isPawn ? 6 : 7;
//		return coord.m_y != rowToCompare;
//	}
//	else
//	{
//		const int rowToCompare = isPawn ? 1 : 0;
//		return coord.m_y != rowToCompare;
//	}
//	return false;
//}

int Piece::GetValue() const
{
	if (m_pieceflags & (uint32_t)PieceFlag::None)
	{
		return m_valueMap[Piece::PieceFlag::None];
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Pawn)
	{
		return m_valueMap[Piece::PieceFlag::Pawn];
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::King)
	{
		return m_valueMap[Piece::PieceFlag::King];
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Horse)
	{
		return m_valueMap[Piece::PieceFlag::Horse];
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Bishop)
	{
		return m_valueMap[Piece::PieceFlag::Bishop];
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Rook)
	{
		return m_valueMap[Piece::PieceFlag::Rook];
	}
	else if (m_pieceflags & (uint32_t)PieceFlag::Queen)
	{
		return m_valueMap[Piece::PieceFlag::Queen];
	}

	return m_valueMap[Piece::PieceFlag::None];
}

void Piece::Promote(SDL_Renderer* pRenderer)
{
	if (!pRenderer)
		return;

	if (ChessUser* pOwner = m_pOwner)
	{
		const uint32_t colour = pOwner->IsWhite() ? (uint32_t)Piece::PieceFlag::White : (uint32_t)Piece::PieceFlag::Black;
		int pathID = 0;
		if (colour & (uint32_t)Piece::PieceFlag::Black)
		{
			pathID = 1;
		}

		m_pieceflags &= ~(uint32_t)Piece::PieceFlag::Pawn;
		m_pieceflags |= (uint32_t)Piece::PieceFlag::Queen;
		NMSprite& sprite = GetSprite();
		sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Queen].m_paths[pathID]);
		const int pieceSize = 96;		
		const int size = pieceSize;
		SetSize(size, size);
		UpdatePosFromCoord();

	}
}
