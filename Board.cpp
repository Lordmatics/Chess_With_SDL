#include "Board.h"
#include "SDL_render.h"
#include <iostream>
#include <time.h>

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
				m_validTiles.push_back(pTile);
				return false;
			}
		}
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
	//if (!m_queryingTiles.empty())
	//{
	//	return;
	//}

	m_queryingTiles.clear();
	m_validTiles.clear();

	if (!pSelectedObject)
	{
		return;
	}

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

	// Validate Move
	// If as a result of this move, you would unleash a discovered Check...
	// Remove it from the valid tiles
	// So essentially, determine if we are pinned - then we cannot move anywhere, EXCEPT (maybe) a capture on the pinner
	pSelectedObject->UpdateVisibileTiles(m_queryingTiles);
	// NOTE: Can perhaps do this before evaluating this pieces moves altogether
	if (Piece* pPinner = IsPiecePinned(*pSelectedObject))
	{
		// For Now, disallow movement from this piece		
		m_validTiles.clear(); // NOTE: This clear may be unnecessary
		pSelectedObject->ClearAttackedTiles();
	}
	else
	{
		pSelectedObject->UpdateAttackedTiles(m_validTiles);
	}


	const bool playerIsWhite = m_player.IsWhite();
	const bool aiIsWhite = m_opponent.IsWhite();
	// Filter Valid Moves to Legal Moves
	// Essentially any move that doesn't result in us being put in check
	// So simulate each valid move, then evaluate the position
	// If the position results in the king being capturable
	// Exclude that move

	// Needs to include the attacker
	const std::vector<Tile*>& checkedTiles = GetCheckedTilesConst();
	if (checkedTiles.size() <= 0)
	{
		return;
	}

	int eraseIndex = 0;
	std::vector<Tile*> finalTiles;
	// Logic needs flipping for king
	bool isKing = flags & (uint32_t)Piece::PieceFlag::King;

	if (isKing)
	{
		for (Tile* pPseudoTile : m_validTiles)
		{
			bool dontAdd = false;
			for (Tile* pCheckedTile : checkedTiles)
			{
				if (pPseudoTile == pCheckedTile)
				{
					// Don't let the king go to a square that is under check vision
					// Unless... The move itself would allow us to capture the attacker
					Piece* pAttackingPiece = pCheckedTile->GetPiece();
					if(!pAttackingPiece)
					{
						dontAdd = true;
						break;
					}	
					else
					{
						finalTiles.push_back(pPseudoTile);
					}
					break;
				}	
			}
			if (!dontAdd)
			{
				finalTiles.push_back(pPseudoTile);	
			}
		}
	}
	else
	{
		for (Tile* pPseudoTile : m_validTiles)
		{
			for (Tile* pCheckedTile : checkedTiles)
			{
				if (pPseudoTile == pCheckedTile)
				{
					finalTiles.push_back(pPseudoTile);
					break;
				}
			}
		}
	}


	m_validTiles = finalTiles;

	if (isWhite)
	{
		if (!playerIsWhite)
		{
			// AI Playing as White vs Black Pieces [Player]
			
		}
		else if(!aiIsWhite)
		{
			// AI Pla
		}
		
	}
	else
	{

	}
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
		pTile->RenderLegalHighlight(pRenderer);
	}

	for (int i = 0; i < 2 ; i++)
	{
		m_players[i]->Render(pRenderer);
	}

	//if (Piece* pSelected = m_player. m_pSelectedPiece)
	//{
	//	if (!pSelected->RenderAsSelected(pRenderer))
	//	{
	//		m_pSelectedRect = nullptr;
	//
	//		pSelected->SetSelected(false);
	//		m_pSelectedPiece = nullptr;
	//
	//		if (Tile* pPiecesTile = m_pPiecesTile)
	//		{
	//			m_pPiecesTile = nullptr;
	//		}
	//	}
	//}
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
					m_validTiles.push_back(pTile);
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
	//const bool playerIsWhite = m_player.IsWhite();	
	m_player.TryGenerateMoves(pRenderer, m_mousePosition);
	//if (Tile* pTile = GetTileAtPoint(&m_mousePosition))
	//{
	//	if (SDL_Rect* pawnTransform = &pTile->GetTransform())
	//	{
	//		if (SDL_PointInRect(&m_mousePosition, pawnTransform))
	//		{
	//			if (Piece* pOccupant = pTile->GetPiece())
	//			{
	//				const bool isWhite = pOccupant->GetFlags() & (uint32_t)Piece::PieceFlag::White;
	//				if (isWhite != playerIsWhite && !m_disableAI)
	//				{
	//					ClearInput();
	//				}
	//				else
	//				{
	//					m_resetPos = pOccupant->GetTransform();
	//					m_pSelectedRect = &pOccupant->GetTransform();
	//					m_pPiecesTile = pTile;
	//					m_pSelectedPiece = pOccupant;
	//					m_pSelectedPiece->SetSelected(true);
	//				}
	//			}
	//		}
	//	}
	//}

	//if (IsPlayersTurn())
	//{
	//	GenerateLegalMoves(m_pSelectedPiece);
	//}
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


	//if (IsPlayersTurn())
	//{
	//	//for (int i = 0; i < Board::m_iColumns; i++)
	//	{
	//		if (Tile* pTile = GetTileAtPoint(&m_mousePosition))
	//		{
	//			if (SDL_Rect* pawnTransform = &pTile->GetTransform())
	//			{
	//				const bool isAPawn = m_pSelectedPiece ? m_pSelectedPiece->GetFlags() & (uint32_t)Piece::PieceFlag::Pawn : false;
	//				auto Predicate = [&pTile](Tile* pOtherTile)
	//				{
	//					return pTile == pOtherTile;
	//				};
	//				if (TileMatch(Predicate, 1))
	//				{
	//					const bool isTileAPromotionSquare = pTile->IsPromotionSquare();
	//					if (Piece* pDefender = pTile->GetPiece())
	//					{
	//						isACapture = true;

	//						pDefender->SetCaptured(true);

	//						if (isAPawn && isTileAPromotionSquare && m_pSelectedPiece)
	//						{
	//							if (Piece* pSelectedPiece = m_pSelectedPiece)
	//							{
	//								pSelectedPiece->Promote(pRenderer);
	//							}
	//						}
	//					}
	//					else
	//					{
	//						// Need to consider EnPassant
	//						// If the piece could move to this square was a pawn
	//						// And the Tile BELOW it HAS a Pawn as well.
	//						// Need to terminate that tiles piece
	//						if (Piece* pSelectedPiece = m_pSelectedPiece)
	//						{
	//							pSelectedPiece->CheckEnpassant(*pTile, *this);

	//							if (isAPawn)
	//							{
	//								if (isTileAPromotionSquare)
	//								{
	//									pSelectedPiece->Promote(pRenderer);
	//								}
	//							}
	//							else
	//							{
	//								pSelectedPiece->CheckCastling(*pTile, *this);
	//							}
	//						}
	//					}

	//					allowMove = true;
	//					// Need to apply piece offsets
	//					newCoord = pTile->GetCoordinate();
	//					pNewTile = pTile;
	//					//break;
	//				}
	//			}
	//		}
	//	}
	//}

	//if (allowMove)
	//{
	//	if (Piece* pSelectedPiece = m_pSelectedPiece)
	//	{
	//		// Need to tell previous tile that we're gone
	//		// And New tile that we have arrived
	//		if (Tile* pPrevTile = m_pPiecesTile)
	//		{
	//			pPrevTile->SetPiece(nullptr);
	//			m_pPiecesTile = nullptr;
	//		}
	//		pSelectedPiece->SetCoord(newCoord);
	//		pSelectedPiece->UpdatePosFromCoord();
	//		if (Tile* pDestinationTile = pNewTile)
	//		{
	//			pDestinationTile->SetPiece(pSelectedPiece);
	//		}
	//		m_playersTurn = false;

	//		m_pSelectedRect = nullptr;
	//		
	//		SetPreviouslyMoved(pSelectedPiece);
	//		pSelectedPiece->SetSelected(false);
	//		m_pSelectedPiece = nullptr;
	//		
	//		//if (Tile* pPiecesTile = m_pPiecesTile)
	//		//{
	//		//	m_pPiecesTile = nullptr;
	//		//}

	//		bool resultedInCheck = m_player.DetectChecks();
	//		if (resultedInCheck)
	//		{
	//			volatile int i = 5;
	//		}
	//		RunAI(pRenderer, m_playersTurn);
	//	}
	//}
	//else
	//{
	//	if (m_pSelectedRect)
	//	{
	//		m_pSelectedRect->x = m_resetPos.x;
	//		m_pSelectedRect->y = m_resetPos.y;
	//	}
	//}

	//ClearInput();
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
