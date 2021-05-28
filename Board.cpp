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

	//for (int i = 0; i < m_iRows; i++)
	//{
	//	for (int j = 0; j < m_iColumns; j++)
	//	{
	//		Tile& object = m_backgroundTiles[i][j];
	//		SDL_Rect& rect = object.GetTransform();
	//		if (SDL_PointInRect(point, &rect))
	//		{
	//			return &object;
	//		}
	//	}
	//}
	return nullptr;
}

Piece* Board::GetPieceAtPoint(SDL_Point* point, int startIndex)
{
	//if (Tile* pTile = GetTileAtPoint(point, startIndex))
	//{
	//	return pTile->GetPiece();
	//}
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
	if (!m_queryingTiles.empty())
	{
		return;
	}

	if (!pSelectedObject)
	{
		return;
	}

	int x = 0;
	int y = 0;
	int indexFound = 0;
	// Find Tile in Board
	//bool matchFoundOnBoard = false;
	
	const int tileId = pSelectedObject->GetTileIDFromCoord();

	uint32_t flags = pSelectedObject->GetFlags();
	//const bool isWhite = pSelectedObject->IsColour((uint32_t)Piece::PieceFlag::White);
	const bool isWhite = pSelectedObject->GetFlags() & (uint32_t)Piece::PieceFlag::White ? true : false;
	const bool isSouth = pSelectedObject->IsSouthPlaying();
//	x = 8 - pSelectedObject->GetTransform().x;
//	y = pSelectedObject->GetTransform().y;
	
	Coordinate coord = pSelectedObject->GetCoordinate();
	x = coord.m_x;
	y = coord.m_y;
	if (flags & (uint32_t)Piece::PieceFlag::Pawn)
	{
		GenerateLegalPawnMoves(x, y, isSouth, pSelectedObject);		
	}
	else if (flags & (uint32_t)Piece::PieceFlag::King)
	{
		GenerateLegalKingMoves(x, y, isSouth, pSelectedObject);
	}
	else if (flags & (uint32_t)Piece::PieceFlag::Horse)
	{
		GenerateLegalHorseMoves(x, y, isSouth, pSelectedObject);
	}
	else if (flags & (uint32_t)Piece::PieceFlag::Bishop)
	{
		GenerateLegalBishopMoves(x, y, isSouth, pSelectedObject);
	}
	else if (flags & (uint32_t)Piece::PieceFlag::Rook)
	{
		GenerateLegalRookMoves(x, y, isSouth, pSelectedObject);
	}
	else if (flags & (uint32_t)Piece::PieceFlag::Queen)
	{
		GenerateLegalQueenMoves(x, y, isSouth, pSelectedObject);

		if (indexFound >= 8)
		{
			//m_queryingTiles.push_back(&m_board[indexFound - 8]);
		}


	}
}

void Board::RenderLegalMoves(SDL_Renderer* pRenderer)
{
	// TODO: Filter Query Tiles for Actual Tiles
	// Query Tiles encompass ALL Possible moves without restriction

	const bool actual = true;
	std::vector<Tile*>& tiles = actual ? m_validTiles : m_queryingTiles;
	for (Tile* tile : m_validTiles)
	{
		if (tile)
		{
			tile->RenderLegalHighlight(pRenderer);
		}
	}
}

void Board::ClearLegalMoves()
{
	for (Tile* tile : m_queryingTiles)
	{
		if (tile)
		{
			tile->ResetLegalHighlight();
		}
	}
	m_queryingTiles.clear();

	for (Tile* tile : m_validTiles)
	{
		if (tile)
		{
			tile->ResetLegalHighlight();
		}
	}
	m_validTiles.clear();
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

	if (random == 1)
	{	
		m_players[0]->SetWhite(true);
		m_players[0]->SetSide(ChessUser::Side::BOTTOM);
		m_players[1]->SetWhite(false);
		m_players[1]->SetSide(ChessUser::Side::TOP);
	}				
	else			
	{				
		m_players[0]->SetWhite(false);
		m_players[0]->SetSide(ChessUser::Side::BOTTOM);
		m_players[1]->SetWhite(true);
		m_players[1]->SetSide(ChessUser::Side::TOP);
	}

	for (int i = 0; i < MAX_NUM_PLAYERS; i++)
	{
		if (m_players[i])
		{
			m_players[i]->Init(pRenderer, this);
		}
	}	

	//for (int i = 0; i < m_iRows; i++)
	//{
	//	for (int j = 0; j < m_iColumns; j++)
	//	{
	//		Tile& pieceTileRef = m_board[i + j * 8];
	//		pieceTileRef.SetSize(96, 96);
	//		pieceTileRef.SetBoardID(i, j);

	//		Tile& object = m_backgroundTiles[i][j];
	//		object.SetSize(tileSize, tileSize);
	//		object.SetBoardID(i, j);
	//		NMSprite& sprite = object.GetSprite();
	//		SDL_Rect& transform = object.GetTransform();
	//		if (j % 2 == 0)
	//		{
	//			if (i % 2 != 0)
	//			{
	//				// While J is even, and I is Odd
	//				// Add Light Tile
	//				sprite.AssignSprite(pRenderer, m_tileMap[Board::TileType::TILEL]);					
	//			}
	//			else
	//			{
	//				// While J is even, and I is even
	//				sprite.AssignSprite(pRenderer, m_tileMap[Board::TileType::TILED]);

	//			}
	//		}
	//		else
	//		{
	//			if (i % 2 == 0)
	//			{
	//				// While J is even, and I is Odd
	//				// Add Light Tile
	//				sprite.AssignSprite(pRenderer, m_tileMap[Board::TileType::TILEL]);
	//			}
	//			else
	//			{
	//				sprite.AssignSprite(pRenderer, m_tileMap[Board::TileType::TILED]);
	//			}
	//		}
	//		int xPos = xOffset + (i * tileSize);
	//		int yPos = yOffset + (j * tileSize);
	//		object.SetPos(xPos, yPos);
	//		pieceTileRef.SetPos(xPos, yPos);
	//		object.SetFlags((uint32_t)Piece::PieceFlag::None);
	//		pieceTileRef.SetFlags((uint32_t)Piece::PieceFlag::None);
	//	}
	//}


	//// Build Pieces
	//const int pieceSize = 96;
	//const int pawnSize = 76;
	//const int innerTilePieceOffset = 16;
	//const int innerTilePawnOffset = 26;

	//uint32_t colour = (uint32_t)Piece::PieceFlag::Black;
	//int pathID = 1;
	//for (int i = 0; i < 8 ; i++)
	//{
	//	//m_board[i].SetBoardID(i, 0);
	//	Tile& tile = m_board[i];
	//	NMSprite& sprite = tile.GetSprite();
	//	tile.SetSize(pieceSize, pieceSize);
	//	const int buffer = innerTilePieceOffset;
	//	int xPos = xOffset + buffer + (i * tileSize);
	//	//if (south)
	//	//{
	//	//	int yPos = yOffset + buffer + ((i + 6) * tileSize);
	//	//	object.SetPos(xPos, yPos);
	//	//}
	//	//else
	//	{
	//		//int yPos = 0;
	//		//if (i == 0)
	//		//{
	//		//int yPos = yOffset + buffer + (i + 1) * tileSize;
	//		//}
	//		//else
	//		//{
	//		//	yPos = yOffset + buffer + (i - 1) * tileSize;
	//		//}
	//		tile.SetPos(xPos, tile.GetTransform().y + yOffset / 2);
	//	}

	//	if (i == 0 || i == Board::m_iColumns - 1)
	//	{
	//		sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Rook].m_paths[pathID]);
	//		tile.SetFlags((uint32_t)Piece::PieceFlag::Rook | colour);
	//	}
	//	else if (i == 1 || i == Board::m_iColumns - 2)
	//	{
	//		sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Horse].m_paths[pathID]);
	//		tile.SetFlags((uint32_t)Piece::PieceFlag::Horse | colour);
	//	}
	//	else if (i == 2 || i == Board::m_iColumns - 3)
	//	{
	//		sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Bishop].m_paths[pathID]);
	//		tile.SetFlags((uint32_t)Piece::PieceFlag::Bishop | colour);
	//	}
	//	else if (i == 3)
	//	{
	//		sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Queen].m_paths[pathID]);
	//		tile.SetFlags((uint32_t)Piece::PieceFlag::Queen | colour);
	//	}
	//	else if (i == 4)
	//	{
	//		sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::King].m_paths[pathID]);
	//		tile.SetFlags((uint32_t)Piece::PieceFlag::King | colour);
	//	}
	//}

	//// ^^^ Correct sort of
	////return;
	//for (int i = 8; i < 16 ; i++)
	//{
	//	//m_board[i].SetBoardID(i - 8, 1);
	//	Tile& tile = m_board[i];
	//	NMSprite& sprite = tile.GetSprite();
	//	tile.SetSize(pawnSize, pawnSize);
	//	const int buffer = innerTilePawnOffset;
	//	int j = 15 - i;
	//	int xPos = xOffset + buffer + (j * tileSize);
	//	//if (south)
	//	//{
	//	//	int yPos = yOffset + buffer + ((i + 6) * tileSize);
	//	//	object.SetPos(xPos, yPos);
	//	//}
	//	//else
	//	{
	//		//int yPos = 0;
	//		//if (i == 0)
	//		//{
	//		//int yPos = yOffset + buffer + (i + 1) * tileSize;
	//		//}
	//		//else
	//		//{
	//		//	yPos = yOffset + buffer + (i - 1) * tileSize;
	//		//}
	//		tile.SetPos(xPos, tile.GetTransform().y + yOffset);
	//	}

	//	sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Pawn].m_paths[pathID]);
	//	tile.SetFlags((uint32_t)Piece::PieceFlag::Pawn | colour);
	//}

	////return;
	//pathID = 0;

	//for (int i = 48; i < 56; i++)
	//{
	//	//m_board[i].SetBoardID(i - 8, 1);
	//	Tile& tile = m_board[i];
	//	NMSprite& sprite = tile.GetSprite();
	//	tile.SetSize(pawnSize, pawnSize);
	//	const int buffer = innerTilePawnOffset;
	//	int j = 55 - i;
	//	int xPos = xOffset + buffer + (j * tileSize);
	//	//if (south)
	//	//{
	//	//	int yPos = yOffset + buffer + ((i + 6) * tileSize);
	//	//	object.SetPos(xPos, yPos);
	//	//}
	//	//else
	//	{
	//		//int yPos = 0;
	//		//if (i == 0)
	//		//{
	//		//int yPos = yOffset + buffer + (i + 1) * tileSize;
	//		//}
	//		//else
	//		//{
	//		//	yPos = yOffset + buffer + (i - 1) * tileSize;
	//		//}
	//		tile.SetPos(xPos, tile.GetTransform().y + yOffset);
	//	}
	//	sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Pawn].m_paths[pathID]);
	//	tile.SetFlags((uint32_t)Piece::PieceFlag::Pawn | colour);
	//}

	//for (int i = 56; i < 64; i++)
	//{
	//	//m_board[i].SetBoardID(i, 0);
	//	Tile& tile = m_board[i];
	//	NMSprite& sprite = tile.GetSprite();
	//	tile.SetSize(pieceSize, pieceSize);
	//	const int buffer = innerTilePieceOffset;
	//	int j = 63 - i;

	//	int xPos = xOffset + buffer + (j * tileSize);
	//	//if (south)
	//	//{
	//	//	int yPos = yOffset + buffer + ((i + 6) * tileSize);
	//	//	object.SetPos(xPos, yPos);
	//	//}
	//	//else
	//	{
	//		//int yPos = 0;
	//		//if (i == 0)
	//		//{
	//		//int yPos = yOffset + buffer + (i + 1) * tileSize;
	//		//}
	//		//else
	//		//{
	//		//	yPos = yOffset + buffer + (i - 1) * tileSize;
	//		//}
	//		tile.SetPos(xPos, tile.GetTransform().y + yOffset / 2);
	//	}

	//	if (j == 0 || j == Board::m_iColumns - 1)
	//	{
	//		sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Rook].m_paths[pathID]);
	//		tile.SetFlags((uint32_t)Piece::PieceFlag::Rook | colour);
	//	}
	//	else if (j == 1 || j == Board::m_iColumns - 2)
	//	{
	//		sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Horse].m_paths[pathID]);
	//		tile.SetFlags((uint32_t)Piece::PieceFlag::Horse | colour);
	//	}
	//	else if (j == 2 || j == Board::m_iColumns - 3)
	//	{
	//		sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Bishop].m_paths[pathID]);
	//		tile.SetFlags((uint32_t)Piece::PieceFlag::Bishop | colour);
	//	}
	//	else if (j == 3)
	//	{
	//		sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::Queen].m_paths[pathID]);
	//		tile.SetFlags((uint32_t)Piece::PieceFlag::Queen | colour);
	//	}
	//	else if (j == 4)
	//	{
	//		sprite.AssignSprite(pRenderer, Board::m_pieceMap[Piece::PieceFlag::King].m_paths[pathID]);
	//		tile.SetFlags((uint32_t)Piece::PieceFlag::King | colour);
	//	}
	//}
}

void Board::Render(SDL_Renderer* pRenderer)
{
	if (!pRenderer)
	{
		std::cout << "Board failed to render - Missing SDL_Renderer!" << std::endl;
		return;
	}

	//for (int i = 0; i < m_iRows; i++)
	//{
	//	for (int j = 0; j < m_iColumns; j++)
	//	{
	//		SDL_Rect& transform = m_backgroundTiles[i][j].GetTransform();
	//		NMSprite& sprite = m_backgroundTiles[i][j].GetSprite();
	//		if (SDL_Texture* pTexture = sprite.GetTexture())
	//		{
	//			SDL_RenderCopy(pRenderer, pTexture, nullptr, &transform);
	//		}
	//	}
	//}

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

	RenderLegalMoves(pRenderer);
}

Board::PiecePaths::PiecePaths(const char* a, const char* b)
{
	m_paths[0] = a;
	m_paths[1] = b;
}

Board::PiecePaths::PiecePaths()
{

}

void Board::GenerateLegalPawnMoves(int x, int y, bool isSouth, Piece* pSelectedPiece)
{
	const int totalCoords = 6;
	CoordSet valiCoords[totalCoords] = { };
	int counter = 0;
	int targetY = isSouth ? y - 1 : y + 1;
	int exceptions = 0;
	bool bCheckEnpassant = false;
	Coordinate pieceCoord = pSelectedPiece->GetCoordinate();
	const int pieceY = pieceCoord.m_y;
	const bool side = pSelectedPiece->IsSouthPlaying();
	const bool hasMoved = pSelectedPiece->HasMoved();
	if (side)
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
				// TODO: Check For Obstructed
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
			bool capturable = pSelectedPiece->CanCapture(pTargetPiece);
			if (!capturable)
			{
				continue;
			}
			else
			{
				// Determine if it were to be enpassant
				Coordinate b = pTargetPiece->GetCoordinate();
				Coordinate a = pSelectedPiece->GetCoordinate();
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
							m_validTiles.push_back(pQuery);
							added = true;
							break;
						}
					}
					if(added)
						continue;
				}
			}
		}
		else
		{
			// No Piece
			// Check Coords with same X Position
			Coordinate pawnPos = pSelectedPiece->GetCoordinate();
			Coordinate tilePos = pQuery->GetCoordinate();
			if (pawnPos.m_x != tilePos.m_x)
			{
				continue;
			}
		}		
		m_validTiles.push_back(pQuery);
	}
}

void Board::GenerateLegalKingMoves(int x, int y, bool isSouth, Piece* pSelectedPiece)
{

}

void Board::GenerateLegalHorseMoves(int x, int y, bool isSouth, Piece* pSelectedPiece)
{

}

void Board::GenerateLegalBishopMoves(int x, int y, bool isSouth, Piece* pSelectedPiece)
{
	const int totalCoords = 6;
	CoordSet valiCoords[totalCoords] = { };
	int counter = 0;
	int targetY = isSouth ? y - 1 : y + 1;
	int exceptions = 0;
	bool bCheckEnpassant = false;
	Coordinate pieceCoord = pSelectedPiece->GetCoordinate();
	const int pieceY = pieceCoord.m_y;
	const bool side = pSelectedPiece->IsSouthPlaying();
	const bool hasMoved = pSelectedPiece->HasMoved();
	if (side)
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

	auto CheckBishop = [&](bool& bObstructed, Coordinate diagonalCoord, Coordinate rule)
	{
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
					bool amWhite = pSelectedPiece->GetFlags() & (uint32_t)Piece::PieceFlag::White;
					bool opponentWhite = pPiece->GetFlags()  & (uint32_t)Piece::PieceFlag::White;
					if (amWhite == opponentWhite)
					{
						bObstructed = true;
					}
					else
					{
						m_validTiles.push_back(pTile);
						break;
					}
						
					bObstructed = true;
					break;
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

void Board::GenerateLegalRookMoves(int x, int y, bool isSouth, Piece* pSelectedPiece)
{

}

void Board::GenerateLegalQueenMoves(int x, int y, bool isSouth, Piece* pSelectedPiece)
{

}

void Board::Test()
{
	volatile int i = 5;
	auto a = m_queryingTiles;

	auto b = m_validTiles;

	volatile int j = 6;

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
