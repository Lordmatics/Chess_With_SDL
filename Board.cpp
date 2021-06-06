#include "Board.h"
#include "SDL_render.h"
#include <iostream>
#include <time.h>
#include "GameLoop.h"

std::map<Board::TileType, const char*> Board::m_tileMap =
{
	{ Board::TileType::TILEL, "Assets/Images/square brown light.png" },
	{ Board::TileType::TILED, "Assets/Images/square brown dark.png" },
};

int Board::GetTileIDFromCoord(const Coordinate& coord) const
{
	int x = coord.m_x;
	int y = coord.m_y;
	return y * 8 + x;
}

void Board::AddPiece(int boardID, Piece* object)
{
	if (boardID < 0 || boardID >= 64 || !object)
		return;

	m_board[boardID].AddPiece(object);
	std::cout << "Adding Piece To Tile: " << boardID << " ";
	object->Debug();
}

bool Board::CheckPiece(const Piece& selectedPiece, const Coordinate& coord)
{
	Coordinate pieceCoord = selectedPiece.GetCoordinate();

	bool isWhite = selectedPiece.GetFlags() & (uint32_t)Piece::PieceFlag::White;
	if (coord.m_x < 0 || coord.m_x >= 8 ||
		coord.m_y < 0 || coord.m_y >= 8)
	{
		return false;
	}

	const int leftTile = GetTileIDFromCoord(coord);
	if (Tile* pTile = GetTile(leftTile))
	{
		if (Piece* pPiece = pTile->GetPiece())
		{
			bool targetIsWhite = pPiece->GetFlags() & (uint32_t)Piece::PieceFlag::White;
			if (targetIsWhite == isWhite)
			{
				return false;
			}
			else
			{
				m_queryingTiles.push_back(pTile);
				m_validTiles.push_back(pTile);
				return false;
			}
		}
		m_queryingTiles.push_back(pTile);
		m_validTiles.push_back(pTile);
		return true;
	}
	return false;
}

std::map<Piece::PieceFlag, Board::PiecePaths> Board::m_pieceMap =
{
	{ Piece::PieceFlag::Pawn, { "Assets/Images/w_pawn.png", "Assets/Images/b_pawn.png"} },
	{ Piece::PieceFlag::Rook, { "Assets/Images/w_rook.png", "Assets/Images/b_rook.png"} },
	{ Piece::PieceFlag::Horse, { "Assets/Images/w_knight.png", "Assets/Images/b_knight.png"} },
	{ Piece::PieceFlag::Bishop, { "Assets/Images/w_bishop.png", "Assets/Images/b_bishop.png"} },
	{ Piece::PieceFlag::King, { "Assets/Images/w_king.png", "Assets/Images/b_king.png"} },
	{ Piece::PieceFlag::Queen, { "Assets/Images/w_queen.png", "Assets/Images/b_queen.png"} },
};

Tile* Board::GetTileAtPoint(SDL_Point* point, int startIndex)
{
	const int total = m_iRows * m_iColumns;
	for (int i = startIndex; i < total ; i++)
	{
		Tile& object = m_board[i];
		SDL_Rect& rect = object.GetTransform();
		if (SDL_PointInRect(point, &rect))
		{
			return &object;
		}
	}

	return nullptr;
}

Piece* Board::GetPieceAtPoint(SDL_Point* point, int startIndex)
{
	const int total = m_iRows * m_iColumns;
	for (int i = startIndex; i < total; i++)
	{	
		Tile& object = m_board[i];
		if (Piece* pOccupant = object.GetPiece())
		{
			SDL_Rect& rect = pOccupant->GetTransform();			
			if (SDL_PointInRect(point, &rect))
			{
				return pOccupant;
			}
		}	
	}
	return nullptr;
}

void Board::GenerateLegalMoves(Piece* pSelectedObject)
{
	m_queryingTiles.clear();
	m_validTiles.clear();

	if (!pSelectedObject)
	{
		return;
	}

	Piece* pPinner = IsPiecePinned(*pSelectedObject);

	int x = 0;
	int y = 0;
	int indexFound = 0;
	const int tileId = pSelectedObject->GetTileIDFromCoord();

	uint32_t flags = pSelectedObject->GetFlags();
	const bool isWhite = pSelectedObject->GetFlags() & (uint32_t)Piece::PieceFlag::White ? true : false;
	const bool isSouth = pSelectedObject->IsSouthPlaying();
	
	Coordinate coord = pSelectedObject->GetCoordinate();
	x = coord.m_x;
	y = coord.m_y;
	if (flags & (uint32_t)Piece::PieceFlag::Pawn)
	{
		GenerateLegalPawnMoves(*pSelectedObject);		
	}
	else if (flags & (uint32_t)Piece::PieceFlag::King)
	{
		GenerateLegalKingMoves(*pSelectedObject);
	}
	else if (flags & (uint32_t)Piece::PieceFlag::Horse)
	{
		GenerateLegalHorseMoves(*pSelectedObject);
	}
	else if (flags & (uint32_t)Piece::PieceFlag::Bishop)
	{
		GenerateLegalBishopMoves(*pSelectedObject);
	}
	else if (flags & (uint32_t)Piece::PieceFlag::Rook)
	{
		GenerateLegalRookMoves(*pSelectedObject);
	}
	else if (flags & (uint32_t)Piece::PieceFlag::Queen)
	{
		GenerateLegalQueenMoves(*pSelectedObject);
	}

	if (m_validTiles.size() <= 0)
	{
		// Small optimisation
		// If Piece is already boxed in and cannot move, don't calculate any further
		//pSelectedObject->ClearAttackedTiles()
		pSelectedObject->ClearAttackedTiles();
		return;
	}

	std::vector<Tile*> checkers;
	if (IsInCheck(*pSelectedObject, checkers))
	{
		GenerateCheckList(*pSelectedObject, checkers);
	}
	else
	{
		m_checkedMapInterception.clear();

		if (pPinner)
		{
			Tile* pPinnerTile = GetTile(GetTileIDFromCoord(pPinner->GetCoordinate()));
			// Determine new valid moves
			// Basically only Capture is acceptable, unless we're double pinned in which case no moves are acceptable			
			bool allowCapture = false;
			for (Tile* pTile : m_validTiles)
			{
				if (pPinnerTile == pTile)
				{
					allowCapture = true;
					break;
				}
			}
			//pSelectedObject->ClearAttackedTiles();
			if (allowCapture)
			{
				m_validTiles.clear();
				m_validTiles.push_back(pPinnerTile);
			}
		}

		if (flags & (uint32_t)Piece::PieceFlag::King)
		{
			std::vector<Tile*> newValidMoves;
			// Check we're not moving INTO check...
			for (Tile* pValidMove : m_validTiles)
			{
				for (int i = 0; i < 2; i++)
				{
					if (ChessUser* pUser = m_players[i])
					{
						if (pUser != pSelectedObject->GetOwner())
						{
							Piece* pieces = pUser->GetPieces();
							bool dontAdd = false;
							for (int j = 0; j < 16; j++)
							{
								Piece& piece = pieces[j];
								if (piece.IsCaptured())
									continue;

								const std::vector<Tile*>& attackingList = piece.GetAttackedTiles();
								for (Tile* pTile : attackingList)
								{
									if (!pTile)
										continue;

									if (pTile == pValidMove)
									{
										dontAdd = true;
										break;
									}
								}				
								if (dontAdd)
									break;
							}
							if (!dontAdd)
							{
								newValidMoves.push_back(pValidMove);
							}
						}
					}
				}
			}
			m_validTiles = newValidMoves;
		}
		else
		{
			// Remove King from Movable Tile
			// This does work, but breaks 'InCheck' logic, as need king to be in the attacking list lol...
			// Now handling it differently
			//std::vector<Tile*> newValidMoves;
			//// Check we're not moving INTO check...			
			//// Scan for enemy king - and remove from valid list if its in there
			//for (int i = 0; i < 2; i++)
			//{
			//	if (ChessUser* pUser = m_players[i])
			//	{
			//		if (pUser != pSelectedObject->GetOwner())
			//		{
			//			Piece* pieces = pUser->GetPieces();
			//			bool dontAdd = false;
			//			static bool useDangerousLookup = true; // Faster but prone to error if piece arrangement changes
			//			if (useDangerousLookup)
			//			{
			//				// B 4
			//				// W 12
			//				const int pieceID = pUser->IsWhite() ? 12 : 4;
			//				Piece& piece = pieces[pieceID];
			//				const Coordinate& kingPos = piece.GetCoordinate();
			//				const int kingTileID = GetTileIDFromCoord(kingPos);
			//				if (Tile* pKingTile = GetTile(kingTileID))
			//				{
			//					for (Tile* pValidMove : m_validTiles)
			//					{
			//						if (pValidMove != pKingTile)
			//						{
			//							newValidMoves.push_back(pValidMove);
			//						}
			//					}
			//				}
			//			}
			//			else
			//			{
			//				for (int j = 0; j < 16; j++)
			//				{
			//					Piece& piece = pieces[j];
			//					if (piece.IsCaptured())
			//						continue;

			//					if (piece.GetFlags() & (uint32_t)Piece::PieceFlag::King)
			//					{
			//						const Coordinate& kingPos = piece.GetCoordinate();
			//						const int kingTileID = GetTileIDFromCoord(kingPos);
			//						if (Tile* pKingTile = GetTile(kingTileID))
			//						{
			//							for (Tile* pValidMove : m_validTiles)
			//							{
			//								if (pValidMove != pKingTile)
			//								{
			//									newValidMoves.push_back(pValidMove);
			//								}
			//							}
			//						}
			//					}
			//				}
			//			}
			//		}
			//	}
			//}	
			//m_validTiles = newValidMoves;
		}
	}

	pSelectedObject->UpdateVisibileTiles(m_queryingTiles);
	pSelectedObject->UpdateAttackedTiles(m_validTiles);
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
	const int tileSize = GameLoop::s_tileSize;// (int)(GameLoop::s_width / 16);// (int)(GameLoop::s_height / 8.4375f);// 128;
	const int xOffset = GameLoop::s_xOffset;// GameLoop::s_width / 4;//  896 / 2; // Quarter X Reso
	const int yOffset = GameLoop::s_yOffset;// (GameLoop::s_height - (tileSize * 9)) / 2;// 28; // Each tile is 128, so 128 * 8 = 1024. reso = 1920:1080, so 1080 - 1024 = 56, then half top/bot, so 28 each side
	
	int file = 0;
	int rank = 7;
	// Build Background Tiles
	const int total = m_iRows * m_iColumns;
	for (int i = 0; i < total ; i++)
	{
		Tile& object = m_board[i];
		object.Init(pRenderer, i);
	}

	// Add Players - Later give choice but for now Random!
	std::srand((unsigned int)time(NULL));
	int random = std::rand() % 1;
	
	m_players[0] = &m_player;
	m_players[1] = &m_opponent;

	random = 1;
	if (random == 1)
	{	
		m_players[0]->SetWhite(true);
		m_players[0]->SetMyTurn(true);
		m_players[0]->SetSide(ChessUser::Side::BOTTOM);
		m_players[1]->SetWhite(false);
		m_players[1]->SetMyTurn(false);
		m_players[1]->SetSide(ChessUser::Side::TOP);
		m_playersTurn = true;
	}				
	else			
	{				
		m_players[0]->SetWhite(false);
		m_players[0]->SetMyTurn(false);
		m_players[0]->SetSide(ChessUser::Side::BOTTOM);
		m_players[1]->SetWhite(true);
		m_players[1]->SetMyTurn(true);
		m_players[1]->SetSide(ChessUser::Side::TOP);
	}

	for (int i = 0; i < MAX_NUM_PLAYERS; i++)
	{
		if (m_players[i])
		{
			m_players[i]->Init(pRenderer, this);
		}		
	}	

	if (!m_playersTurn)
	{
		RunAI(pRenderer, m_playersTurn);
	}
}

void Board::Render(SDL_Renderer* pRenderer)
{
	if (!pRenderer)
	{
		std::cout << "Board failed to render - Missing SDL_Renderer!" << std::endl;
		return;
	}

	for (int i = 0; i < 64; i++)
	{
		Tile& tile = m_board[i];
		tile.RenderBG(pRenderer);
	}

	for (int i = 0; i < 64 ; i++)
	{
		Tile& tile = m_board[i];
		tile.Render(pRenderer);
	}

	//RenderLegalMoves(pRenderer);

	for (Tile* pTile : m_highlightTiles)
	{
		if (pTile->GetFlags() & (uint32_t)Piece::PieceFlag::King)
		{
			// Ignore
			continue;
		}
		pTile->RenderLegalHighlight(pRenderer);
	}

	for (int i = 0; i < 2 ; i++)
	{
		m_players[i]->Render(pRenderer);
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

void Board::GenerateLegalPawnMoves(Piece& pSelectedPiece)
{
	Coordinate pieceCoord = pSelectedPiece.GetCoordinate();
	int x = pieceCoord.m_x;
	int y = pieceCoord.m_y;
	const int totalCoords = 6;
	CoordSet valiCoords[totalCoords] = { };
	int counter = 0;
	const bool isSouth = pSelectedPiece.IsSouthPlaying();
	int targetY = isSouth ? y - 1 : y + 1;
	int exceptions = 0;
	bool bCheckEnpassant = false;
	const int pieceY = pieceCoord.m_y;
	const bool hasMoved = pSelectedPiece.HasMoved();
	if (isSouth)
	{
		if (pieceY == 3)
		{
			bCheckEnpassant = true;
		}
	}
	else
	{
		if (pieceY == 4)
		{
			bCheckEnpassant = true;
		}
	}
	bool allExceptions = true;
	for (int i = x - 1; i < x + 2; i++)
	{
		if (i < 0 || i > 7 || targetY < 0)
		{
			exceptions++;
			continue;
		}

		valiCoords[counter++] = { {i, targetY}, true };
		allExceptions = false;
		if (i == x)
		{
			if (!hasMoved)
			{
				// If we're stationary check for double move
				int id = GetTileIDFromCoord({ i, targetY });
				if (Tile* pTile = GetTile(id))
				{
					if (Piece* pPiece = pTile->GetPiece())
					{
						// Obstructed
						exceptions++;
						continue;
					}
				}

				if (isSouth)
				{
					const int localY = targetY - 1;
					if (localY >= 0 && localY < 8)
					{
						valiCoords[counter++] = { {i, targetY - 1}, true };
					}
				}
				else
				{
					const int localY = targetY + 1;
					if (localY >= 0 && localY < 8)
					{
						valiCoords[counter++] = { {i, targetY + 1}, true };
					}
				}
			}
		}
		else if (i == x - 1 && bCheckEnpassant)
		{
			// EnPassant Support
			if (isSouth)
			{
				const int localY = targetY + 1;
				if (localY >= 0 && localY < 8)
				{
					valiCoords[counter++] = { {i, targetY + 1}, true };
				}
			}
			else
			{
				const int localY = targetY - 1;
				if (localY >= 0 && localY < 8)
				{
					valiCoords[counter++] = { {i, targetY - 1}, true };
				}
			}
		}
		else if (i == x + 1 && bCheckEnpassant)
		{
			// EnPassant Support
			if (isSouth)
			{
				const int localY = targetY + 1;
				if (localY >= 0 && localY < 8)
				{
					valiCoords[counter++] = { {i, targetY + 1}, true };
				}
			}
			else
			{
				const int localY = targetY - 1;
				if (localY >= 0 && localY < 8)
				{
					valiCoords[counter++] = { {i, targetY - 1}, true };
				}
			}
		}
		else
		{
			exceptions++;
		}
	}

	if (allExceptions)
	{
		return;
	}

	for (int i = 0; i < totalCoords - exceptions; i++)
	{
		const bool isSet = valiCoords[i].m_bSet;
		if (!isSet)
		{
			continue;
		}
		int id = GetTileIDFromCoord(valiCoords[i].m_coord);
		m_queryingTiles.push_back(&m_board[id]);
	}

	for (Tile* pQuery : m_queryingTiles)
	{
		// Determine if we can ACTUALLY move there
		Piece* pTargetPiece = pQuery->GetPiece();
		if (pTargetPiece)
		{
			// If it has a piece, AND we can capture it, its valid
			// Else ignore
			bool capturable = pSelectedPiece.CanCapture(pTargetPiece);
			if (!capturable)
			{
				continue;
			}
			else
			{
				// Determine if it were to be enpassant
				Coordinate b = pTargetPiece->GetCoordinate();
				Coordinate a = pSelectedPiece.GetCoordinate();
				if (a.m_y == b.m_y)
				{
					// Adjacent
					Coordinate targetCoord = b;
					if (pTargetPiece->IsSouthPlaying())
					{
						targetCoord.m_y += 1;
					}
					else
					{
						targetCoord.m_y -= 1;
					}
					bool added = false;
					for (Tile* pQuery : m_queryingTiles)
					{
						// Look for the query tile above this one
						Coordinate coord = pQuery->GetCoordinate();
						if (coord == targetCoord)
						{
							for (Tile* pValid : m_validTiles)
							{
								if (pQuery == pValid)
								{
									return;
								}
							}

							if (Piece* pPrevPiece = m_pPreviousMovedPiece)
							{
								if (pTargetPiece == pPrevPiece)
								{
									// Only allow Enpassant, IF they just moved it
									// else it becomes an invalid move									 
									m_validTiles.push_back(pQuery);
									added = true;
									break;
								}
							}
						}
					}
					continue;

					if(added)
						continue;
				}
			}
		}
		else
		{
			// No Piece
			// Check Coords with same X Position
			Coordinate pawnPos = pSelectedPiece.GetCoordinate();
			Coordinate tilePos = pQuery->GetCoordinate();
			if (pawnPos.m_x != tilePos.m_x)
			{
				continue;
			}
		}		
		m_validTiles.push_back(pQuery);
	}
}

void Board::GenerateLegalKingMoves(Piece& selectedPiece)
{
	Coordinate pieceCoord = selectedPiece.GetCoordinate();
	Coordinate kingPositions[8] =
	{
		{ -1, -1},
		{ -1,  1},
		{ 1,  1},
		{ 1, -1},
		{ 0,  -1},
		{ 0,   1},
		{ 1,   0},
		{ -1,  0}
	};	


	const bool hasMoved = selectedPiece.HasMoved();
	const bool isSouthPlaying = selectedPiece.IsSouthPlaying();
	const int tileIDOfKing = GetTileIDFromCoord(pieceCoord);
	Tile* pKingTile = GetTile(tileIDOfKing);
	if (!pKingTile)
		return;

	// If the king hasn't moved
	bool checkCastle = !hasMoved;
	if (checkCastle)
	{
		if (isSouthPlaying)
		{
			Tile& rookTileC = m_board[63];
			Tile& rookTileD = m_board[56];
			// Determine if rooks are present AND unmoved

			if (rookTileC.HasMoved() && rookTileD.HasMoved())
			{
				checkCastle = false;
			}

			if (checkCastle)
			{
				if (!rookTileC.HasMoved())
				{
					bool hasLOS = true;
					for (int i = tileIDOfKing + 1; i < 63; i++)
					{
						if (Piece* pObstruction = m_board[i].GetPiece())
						{
							// Cannot castle this way
							hasLOS = false;
							break;
						}
					}

					if (hasLOS)
					{
						m_validTiles.push_back(&m_board[62]);
					}

				}
				if (!rookTileD.HasMoved())
				{
					bool hasLos = true;
					for (int i = 57; i < tileIDOfKing; i++)
					{
						if (Piece* pObstruction = m_board[i].GetPiece())
						{
							// Cannot castle this way
							hasLos = false;
							break;
						}
					}
					if (hasLos)
					{
						m_validTiles.push_back(&m_board[58]);
					}
				}
			}

			// Determine if we have line of sight to the rooks
		}
		else
		{
			Tile& rookTileA = m_board[7];
			Tile& rookTileB = m_board[0];
			if (rookTileA.HasMoved() && rookTileB.HasMoved())
			{
				checkCastle = false;
			}

			if (checkCastle)
			{
				if (!rookTileA.HasMoved())
				{
					bool hasLOS = true;
					for (int i = tileIDOfKing + 1; i < 7; i++)
					{
						if (Piece* pObstruction = m_board[i].GetPiece())
						{
							// Cannot castle this way
							hasLOS = false;
							break;
						}
					}

					if (hasLOS)
					{						
						m_validTiles.push_back(&m_board[6]);
					}
				}
				if (!rookTileB.HasMoved())
				{
					bool hasLos = true;
					for (int i = 1; i < tileIDOfKing; i++)
					{
						if (Piece* pObstruction = m_board[i].GetPiece())
						{
							// Cannot castle this way
							hasLos = false;
							break;
						}
					}
					if (hasLos)
					{
						m_validTiles.push_back(&m_board[2]);
					}
				}
			}
		}
	}
	
	// Check if rook has moved if visible
	Coordinate temp = pieceCoord;
	for (int i = 0; i < 8; i++)
	{
		Coordinate val = temp + kingPositions[i];
		if (CheckPiece(selectedPiece, val))
		{

		}
	}
}

void Board::GenerateLegalHorseMoves(Piece& selectedPiece)
{
	Coordinate pieceCoord = selectedPiece.GetCoordinate();
	Coordinate horsePositions[8] =
	{
		{ -2, -1},
		{ -2,  1},
		{ -1,  2},
		{ -1, -2},
		{ 2,  -1},
		{ 2,   1},
		{ 1,   2},
		{ 1,  -2}
	};

	Coordinate temp = pieceCoord;
	for (int i = 0; i < 8 ; i++)
	{
		temp = pieceCoord;
		Coordinate val = temp + horsePositions[i];				
		if (CheckPiece(selectedPiece, val))
		{

		}
	}
}

void Board::GenerateLegalBishopMoves(Piece& selectedPiece)
{
	Coordinate pieceCoord = selectedPiece.GetCoordinate();
	auto CheckBishop = [&](bool& bObstructed, Coordinate diagonalCoord, Coordinate rule)
	{
		bool hitFirstPiece = false;
		while (!bObstructed)
		{
			int TileID = GetTileIDFromCoord(diagonalCoord);
			if (Tile* pTile = GetTile(TileID))
			{
				Coordinate tileCoord = pTile->GetCoordinate();
				if (rule.m_x > 0)
				{
					// East - If we have overflown to the other side of the board. Stop Right There Criminal Scum.
					if (tileCoord.m_x < diagonalCoord.m_x)
					{
						bObstructed = true;
						break;
					}					
				}
				else if (rule.m_y > 0)
				{
					if (tileCoord.m_y < diagonalCoord.m_y)
					{
						bObstructed = true;
						break;
					}
				}
				if (diagonalCoord.m_x < 0 || diagonalCoord.m_y < 0 ||
					diagonalCoord.m_x >= 8 || diagonalCoord.m_y >= 8)
				{
					bObstructed = true;
					break;
				}
				if (Piece* pPiece = pTile->GetPiece())
				{
					bool amWhite = selectedPiece.GetFlags() & (uint32_t)Piece::PieceFlag::White;
					bool opponentWhite = pPiece->GetFlags()  & (uint32_t)Piece::PieceFlag::White;
					if (amWhite == opponentWhite)
					{
						bObstructed = true;
					}
					else
					{
						if (!hitFirstPiece)
						{
							m_queryingTiles.push_back(pTile);
							m_validTiles.push_back(pTile);
							hitFirstPiece = true;
						}
						else
						{
							m_queryingTiles.push_back(pTile);
						}
						diagonalCoord += rule;
						//break;
					}
						
					//bObstructed = true;
					//break;
				}
				else
				{
					if (!hitFirstPiece)
					{
						m_validTiles.push_back(pTile);
					}
					// Keep scanning in this direction for visibility
					// But don't add targets BEYOND obstructions
					diagonalCoord += rule;
				}
			}
			else
			{
				bObstructed = true;
				break;
			}
		}
	};

	for (int i = 0; i < 4 ; i++)
	{
		// Check 4 diagonals
		bool bObstructed = false;
		Coordinate diagonalCoord = pieceCoord;
		Coordinate rule;
		if (i == 0)
		{
			// Scan North West
			rule = { -1, -1 };
			diagonalCoord.m_x -= 1;
			diagonalCoord.m_y -= 1;
			if (diagonalCoord.m_x < 0 || diagonalCoord.m_y < 0)
			{
				bObstructed = true;
				continue;
			}			
		}
		else if (i == 1)
		{
			// Scan North East
			rule = { 1, -1 };
			diagonalCoord.m_x += 1;
			diagonalCoord.m_y -= 1;
			if (diagonalCoord.m_x >= 8 || diagonalCoord.m_y < 0)
			{
				bObstructed = true;
				continue;
			}
		}
		else if (i == 2)
		{
			// Scan South West
			rule = { -1, 1 };
			diagonalCoord.m_x -= 1;
			diagonalCoord.m_y += 1;
			if (diagonalCoord.m_x < 0 || diagonalCoord.m_y >= 8)
			{
				bObstructed = true;
				continue;
			}
		}
		else if (i == 3)
		{
			// Scan South East
			rule = { 1, 1 };
			diagonalCoord.m_x += 1;
			diagonalCoord.m_y += 1;
			if (diagonalCoord.m_x >= 8 || diagonalCoord.m_y >= 8)
			{
				bObstructed = true;
				continue;
			}
		}
		CheckBishop(bObstructed, diagonalCoord, rule);
	}	
}

void Board::GenerateLegalRookMoves(Piece& selectedPiece)
{
	Coordinate pieceCoord = selectedPiece.GetCoordinate();
	int x = pieceCoord.m_x;
	int y = pieceCoord.m_y;
	Coordinate coord;

	//bool isWhite = selectedPiece.GetFlags() & (uint32_t)Piece::PieceFlag::White;
	// Check Horizontals
	int left = x - 1;
	while (true)
	{
		if (left < 0)
			break;

		coord.m_x = left;
		coord.m_y = y;
		if (!CheckPiece(selectedPiece, coord))
		{
			break;
		}
		left--;
	}

	int right = x + 1;
	while (true)
	{
		if (right >= 8)
			break;

		coord.m_x = right;
		coord.m_y = y;
		if (!CheckPiece(selectedPiece, coord))
		{
			break;
		}
		right++;
	}

	int up = y - 1;
	while (true)
	{
		if (up < 0)
			break;

		coord.m_x = x;
		coord.m_y = up;
		if (!CheckPiece(selectedPiece, coord))
		{
			break;
		}
		up--;
	}

	int down = y + 1;
	while (true)
	{
		if (down >= 8)
			break;

		coord.m_x = x;
		coord.m_y = down;
		if (!CheckPiece(selectedPiece, coord))
		{
			break;
		}
		down++;
	}
}

void Board::GenerateLegalQueenMoves(Piece& selectedPiece)
{
	GenerateLegalBishopMoves(selectedPiece);
	GenerateLegalRookMoves(selectedPiece);
}

// If Positive - Position Favours the player
// If Negative - Position favours the AI
// The Numerical Value represents by HOW much it favours who
int Board::EvaluatePosition()
{
	// Get Material for both sides and return

	int playerActivePieces;
	int playerCapturedPieces;
	m_player.GetMaterialScore(playerActivePieces, playerCapturedPieces);

	int aiActivePieces;
	int aiCapturedPieces;
	m_opponent.GetMaterialScore(aiActivePieces, aiCapturedPieces);

	return playerActivePieces - aiActivePieces;
}

const std::vector<Tile*>& Board::GetValidTiles() const
{
	return m_validTiles;
}

void Board::Test()
{
	//volatile int i = 5;
	//auto a = m_queryingTiles;

	//auto b = m_validTiles;

	//volatile int j = 6;

}

Tile* Board::GetTile(int id)
{
	if (id >= 0 && id < 64)
	{
		return &m_board[id];
	}
	else
	{
		return nullptr;
	}
}

void Board::SetPreviouslyMoved(Piece* pSelectedPiece)
{
	pSelectedPiece->SetMoved(true);
	m_pPreviousMovedPiece = pSelectedPiece;
}

void Board::OnLeftClickDown(SDL_Renderer* pRenderer)
{
	m_player.TryGenerateMoves(pRenderer, m_mousePosition);
}

void Board::OnLeftClickRelease(SDL_Renderer* pRenderer)
{
	bool allowMove = false;
	bool isACapture = false;
	Coordinate newCoord;
	Tile* pNewTile = nullptr;
	// If release position is in the valid tiles list - drop it off there

	if (Tile* pTile = GetTileAtPoint(&m_mousePosition))
	{
		m_player.MakeMove(pRenderer, *pTile);
	}
}

void Board::Process(float dt)
{
	for (int i = 0; i < 2 ; i++)
	{
		m_players[i]->Process(dt, m_mousePosition);
	}
}

bool Board::IsPlayersTurn() const
{
	return m_playersTurn || (!m_playersTurn && m_disableAI);
}

bool Board::IsTileDefended(Tile& pTile, bool attackerIsWhite)
{
	const bool playerIsWhite = m_player.IsWhite();
	// If attacker is white, iterate over blacks pieces, to see what has vision on pTile
	if (attackerIsWhite)
	{
		if (playerIsWhite)
		{
			return m_opponent.CanAttackTile(pTile);
		}
		else
		{
			return m_player.CanAttackTile(pTile);
		}
	}
	else
	{
		if (!playerIsWhite)
		{
			return m_opponent.CanAttackTile(pTile);
		}
		else
		{
			return m_player.CanAttackTile(pTile);
		}
	}
}

void Board::UpdatePieces()
{
	
	std::cout << "Board::UpdatePieces: Turn: " << m_iTurn << std::endl;

	for (ChessUser* pUser : m_players)
	{
		Piece* pPieces = pUser->GetPieces();
		for (int i = 0; i < 16 ; i++)
		{
			Piece& currentPiece = pPieces[i];
			const bool isCaptured = currentPiece.IsCaptured();
			if (isCaptured)
			{
				continue;
			}
			GenerateLegalMoves(&currentPiece);

			// Output to console some useful information
			currentPiece.OnPieceUpdated();
		}		 
	}
	m_iTurn++;
	std::cout << std::endl;
}

void Board::SetHighlightTiles(const std::vector<Tile *>& attacked)
{
	if (m_highlightTiles.empty())
	{
		m_highlightTiles = attacked;
	}
}

void Board::UnhighlightTiles()
{
	for (Tile* pTile : m_highlightTiles)
	{
		pTile->ResetLegalHighlight();
	}
	m_highlightTiles.clear();
}

int Board::GetTurn() const
{
	return m_iTurn;
}

Piece* Board::IsPiecePinned(const Piece& selectedObject)
{
	if (ChessUser* pOwner = selectedObject.GetOwner())
	{
		Tile* pMyTile = GetTile(selectedObject.GetTileIDFromCoord());
		if (!pMyTile)
			return nullptr;

		for (int i = 0; i < MAX_NUM_PLAYERS ; i++)
		{
			ChessUser* pUser = m_players[i];
			if (pOwner != pUser)
			{
				// Check each piece pUser has, to see if it is currently pinning our selected object
				Piece* pUserMaterial = pUser->GetPieces();
				for (int j = 0; j < 16 ; j++)
				{
					Piece& piece = pUserMaterial[j];
					if(piece.IsCaptured())
						continue;

					// Optimisation - Only Bishop/Rook/Queen can pin					
					const bool isPinnerType = (piece.GetFlags() & (uint32_t)Piece::PieceFlag::Pinner);// == (uint32_t)Piece::PieceFlag::Pinner;
					if(!isPinnerType)
						continue;

					// We're checking the enemies, Bishops / Rooks / Queens, determining if they are pinning us 'selectedObject'

					const std::vector<Tile*>& attackedTiles = piece.GetAttackedTiles();
					for (Tile* pAttackedTile : attackedTiles)
					{
						if(!pAttackedTile)
							continue;

						if (pAttackedTile == pMyTile)
						{
							// This attacker, can see ME!
							// Determine if he's pinning me though							

							const std::vector<Tile*>& visibleTiles = piece.GetVisibleTiles();
							int numUntilKing = 0;
							for (Tile* pVisibleTile : visibleTiles)
							{
								if (!pVisibleTile)
									continue;

								// Means more than 1 object is in between attacker and king
								if (numUntilKing >= 2)
									break;

								if (Piece* pPiece = pVisibleTile->GetPiece())
								{									
									const bool isPinning = piece.IsPinning(*pPiece, numUntilKing);
									if (isPinning)
									{
										// return attacker
										return &piece;
									}
								}
							}
							// If we got here, it means this piece may be pinning in the near future, but not currently
							// check the next piece.
							break;
						}
					}				
				}

				// We only want to iterate over the enemies pieces
				break;
			}			
		}
	}
	return nullptr;
}

bool Board::IsInCheck(const Piece& selectedObject, std::vector<Tile*>& checkers)
{
	// If we are in check...
	// Discard any move that doesn't either
	// 1) block the check
	// 2) Capture the checking piece (this case is only valid if we are only in check by one piece)
	Coordinate attackerCoord;
	Coordinate kingCoord;
	for (int i = 0; i < 2; i++)
	{
		if (ChessUser* pUser = m_players[i])
		{
			if (pUser != selectedObject.GetOwner())
			{
				Piece* pieces = pUser->GetPieces();
				for (int j = 0; j < 16; j++)
				{
					Piece& piece = pieces[j];
					if (piece.IsCaptured())
						continue;

					const std::vector<Tile*>& attackingList = piece.GetAttackedTiles();
					const bool amIWhite = piece.GetFlags() & (uint32_t)Piece::PieceFlag::White;
					for (Tile* pTile : attackingList)
					{
						if (!pTile)
							continue;

						// If Pieces are same colour ignore
						const bool isWhite = pTile->IsColour((uint32_t)Piece::PieceFlag::White);
						if(isWhite == amIWhite)
							continue;

						if (pTile->GetFlags() & (uint32_t)Piece::PieceFlag::King)
						{
							// An enemy piece, can currently capture our king, this is bad..
							const int tileID = piece.GetTileIDFromCoord();							
							if (Tile* pAttackersTile = GetTile(tileID))
							{
								attackerCoord = pAttackersTile->GetCoordinate();
								kingCoord = pTile->GetCoordinate();
								// Generate Inbetween tiles

								// TODO: INBETWEEN TILES NOT ACCURATE IF QUEEN IS CHECKING YOU
								// NEED TO CONSIDER DIAGONALS AS WELL AS DIRECT LINE OF SIGHT
								// so compare all Visibile Tiles Queen has vs All Kings current movement
								// Then eliminate results from that
								// This will fix King being able to move diagonally next to queen

								std::vector<Tile*> inbetweenTiles;
								int xDiff = attackerCoord.m_x - kingCoord.m_x;
								int absXDiff = xDiff;
								if (xDiff < 0)
								{
									absXDiff *= -1;
								}
								int yDiff = attackerCoord.m_y - kingCoord.m_y;
								int absYDiff = yDiff;
								if (yDiff < 0)
								{
									absYDiff *= -1;
								}
								inbetweenTiles.push_back(pAttackersTile);

								const bool isHorseAttack = pAttackersTile->GetFlags() & (uint32_t)Piece::PieceFlag::Horse;
								if (!isHorseAttack)
								{
									// Deduce COORD Rule Based on positions
									Coordinate rules[8] =
									{
										{1,0},
										{1, 1},
										{1, -1},
										{0, 1},
										{0, -1},
										{-1, 0},
										{-1, 1},
										{ -1, -1}
									};

									int ruleID = -1;
									if (xDiff < 0)
									{

										// ADD X

										// King is on our right
										if (yDiff < 0)
										{
											// King is below us
											ruleID = 1; // SE
										}
										else if (yDiff == 0)
										{
											// King is adjacent to us
											ruleID = 0; // E
										}
										else
										{
											// King is above us
											ruleID = 2; // NE
										}
									}
									else if (xDiff == 0)
									{
										// King is vertical with us
										if (yDiff < 0)
										{
											// King is below us
											ruleID = 3;

										}
										else
										{
											// King is above us
											ruleID = 4;
										}
									}
									else
									{
										// King is on our left
										if (yDiff < 0)
										{
											// King is below us
											ruleID = 6;

										}
										else if (yDiff == 0)
										{
											// King is adjacent to us+
											ruleID = 5;
										}
										else
										{
											// King is above us
											ruleID = 7;
										}
									}

									bool kingNotFound = true;
									int counter = 1;
									while (kingNotFound)
									{
										Coordinate inBetweenCoord = attackerCoord + (rules[ruleID] * counter++);
										const int inbetweenTileID = GetTileIDFromCoord(inBetweenCoord);
										if (Tile* pInbetweenTile = GetTile(inbetweenTileID))
										{
											if (inBetweenCoord == kingCoord)
											{
												kingNotFound = false;
												break;
											}
											inbetweenTiles.push_back(pInbetweenTile);
										}
									};
								}
								
								const bool isQueenAttack = pAttackersTile->GetFlags() & (uint32_t)Piece::PieceFlag::Queen;

								if (isQueenAttack)
								{
									if (Piece* pKing = pTile->GetPiece())
									{
										const std::vector<Tile*>& kingSquares = pKing->GetAttackedTiles();
										for (Tile* pTmpKingSquare : kingSquares)
										{
											// If this king square is visible in the queens attacking list
											// Incorporate it as an inbetween tile
											for (Tile* pAttackingList : attackingList)
											{
												if (pAttackingList == pTmpKingSquare)
												{
													inbetweenTiles.push_back(pTmpKingSquare);
												}
											}
										}
									}
								}

								m_checkedMapInterception.emplace(pAttackersTile, inbetweenTiles);								
								checkers.push_back(pAttackersTile);
							}
						}
					}
				}
				return checkers.size() > 0;
			}
		}
	}
	return false;
}

void Board::GenerateCheckList(Piece& selectedObject, const std::vector<Tile*>& checkers)
{
	
	// Generate list of tiles from the attacking list, that contribute to causing a check
	m_checkedTiles.clear();

	std::vector<Tile*> newValidTiles;
	std::vector<Tile*> potentiallyValidTiles;
	uint32_t flags = selectedObject.GetFlags();
	const bool isKing = flags & (uint32_t)Piece::PieceFlag::King;
	const int numberOfCheckers = (const int)checkers.size();
	for (Tile* pChecker : checkers)
	{
		const std::vector<Tile*>& validMovement = m_validTiles;// selectedObject.GetAttackedTiles();
		const std::vector<Tile*>& checkedTilesForAttacker = m_checkedMapInterception[pChecker];
			
		for (Tile* pValidMove : validMovement)
		{
			bool addForKing = true;
			for (Tile* pCheckedTile : checkedTilesForAttacker)
			{
				// So this list represents all the in between tiles between the attacker and the king
			
				// Checked Tile List, includes Attacker
				// Valid Move List includes Captures	
				
				if (pValidMove == pCheckedTile)
				{
					if (isKing)
					{
						// Don't Allow King to block check - Not Legal
						addForKing = false;
						break;
					}
					else
					{
						// Move to this tile - Intercepts check - Allowed
						if (numberOfCheckers == 1)
						{
							newValidTiles.push_back(pValidMove);
						}
						else
						{
							// If we have more than one attacker creating check
							// Store this move for later - it might be valid - needs to pass test against all checkers, at blocking their line of sight on the king
							// Ensure no duplicates
							bool add = true;
							for (Tile* pPotentials : potentiallyValidTiles)
							{
								if (pPotentials == pValidMove)
								{
									add = false;
									break;
								}
							}
							if (add)
								potentiallyValidTiles.push_back(pValidMove);
						}
					}
				}
			}

			// If we scanned all tiles under threat, and our pValidMove wasn't in there
			if (isKing && addForKing)
			{
				if (numberOfCheckers == 1)
				{
					newValidTiles.push_back(pValidMove);
				}
				else
				{
					// Ensure Unique - Check aginst ALL Checkers after
					bool add = true;
					for (Tile* pPotentials : potentiallyValidTiles)
					{
						if (pPotentials == pValidMove)
						{
							add = false;
							break;
						}
					}
					if (add)
						potentiallyValidTiles.push_back(pValidMove);
				}
			}
		}
	}
	if (numberOfCheckers > 1)
	{
		int matchFound = 0;
		for (Tile* pPotentials : potentiallyValidTiles)
		{
			bool dontAddForKing = false;
			// Check if the move is in both attackers attacking lists
			for (Tile* pTile : checkers)
			{
				const std::vector<Tile*>& checkedTilesForAttacker = m_checkedMapInterception[pTile];

				for (Tile* pAttackedTile : checkedTilesForAttacker)
				{
					if (isKing)
					{
						// For King its the opposite, ensure move is not in ANY attacking list
						if (pPotentials == pAttackedTile)
						{
							dontAddForKing = true;
							break;
						}
					}
					else
					{
						if (pPotentials == pAttackedTile)
						{
							matchFound++;
							break;
						}
					}
				}
				// If it was in one of the lists, don't bother checking anymore
				if(dontAddForKing)
					break;
			}
			if (isKing)
			{
				if (!dontAddForKing)
				{
					// If False at this point, we escaped their LOS
					newValidTiles.push_back(pPotentials);
				}
			}
			else
			{
				// If it was a spot in both their attacking line of sights
				// It's valid, as it blocks both checks
				if (matchFound == numberOfCheckers)
				{
					newValidTiles.push_back(pPotentials);
				}
			}
		}
	}
	// Overwrite valid moves - now that CHECK rules has been applied
	m_validTiles = newValidTiles;
}

//void Board::ClearInput()
//{
//	ClearLegalMoves();
//}

void Board::RunAI(SDL_Renderer* pRenderer, bool& m_playersTurn)
{
	if (m_disableAI)
	{
		//m_playersTurn = true;
		return;
	}

	// Let'see if i'm in check	

	// Refactor AI
	//if (Piece* pPieceToMove = m_opponent.MakeMove(pRenderer, nullptr))
	//{
	//	SetPreviouslyMoved(pPieceToMove);
	//}

	m_playersTurn = true;
	bool resultedInCheck = m_opponent.DetectChecks();
	if (resultedInCheck)
	{
		volatile int i = 5;
	}
}
