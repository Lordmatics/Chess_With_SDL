#include "GameLoop.h"
#include "SDL.h"
#include <iostream>
#include "ChessUser.h"
#include <utility>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

GameLoop::GameLoop() :
	m_pGameWindow(nullptr),
	m_pRenderer(nullptr),		
	m_bIsRunning(1),
	m_bPadding(0),
	m_iFrameCount(0),
	m_board()
{
	ConstructSDL();
}

GameLoop::GameLoop(int width, int height)
{
	ConstructSDL(width, height);
}

GameLoop::~GameLoop()
{
}

bool GameLoop::IsRunning() const
{
	return m_bIsRunning;
}

void GameLoop::ConstructSDL(int w, int h, bool fullscreen)
{
	int flags = 0;
	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0 )
	{
		std::cout << "SDL Initialized!" << std::endl;
		m_pGameWindow = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
		if (!m_pGameWindow)
			return;
		std::cout << "SDL Window Created" << std::endl;

		m_pRenderer = SDL_CreateRenderer(m_pGameWindow, -1, 0);
		if (!m_pRenderer)
			return;
		
		SDL_SetRenderDrawColor(m_pRenderer, 255, 233, 197, 255);
		std::cout << "SDL Renderer Created" << std::endl;
		m_bIsRunning = true;
	}
	else
	{
		m_bIsRunning = false;
		return;
	}

	InitGame();
}

void GameLoop::CleanUp()
{
	SDL_DestroyWindow(m_pGameWindow);
	std::cout << "Window Destroyed! " << std::endl;
	SDL_DestroyRenderer(m_pRenderer);
	std::cout << "Renderer Destroyed! " << std::endl;
	SDL_Quit();
	std::cout << "Quitting! " << std::endl;

	std::cout << "Game Cleaned!" << std::endl;
}

void GameLoop::HandleEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type)
	{
		/* Drag and drop events */
		//SDL_DROPFILE = 0x1000, /**< The system requests a file open */
		//SDL_DROPTEXT,                 /**< text/plain drag-and-drop event */
		//SDL_DROPBEGIN,                /**< A new set of drops is beginning (NULL filename) */
		//SDL_DROPCOMPLETE,             /**< Current set of drops is now complete (NULL filename) */
		//	/* Keyboard events */
		//SDL_KEYDOWN = 0x300, /**< Key pressed */
		//SDL_KEYUP,                  /**< Key released */
		//SDL_TEXTEDITING,            /**< Keyboard text editing (composition) */
		//SDL_TEXTINPUT,              /**< Keyboard text input */
		//SDL_KEYMAPCHANGED,          /**< Keymap changed due to a system event such as an
		//								 input language or keyboard layout change.
		//							*/
		//
		//							/* Mouse events */
		//SDL_MOUSEMOTION = 0x400, /**< Mouse moved */
		//SDL_MOUSEBUTTONDOWN,        /**< Mouse button pressed */
		//SDL_MOUSEBUTTONUP,          /**< Mouse button released */
		//SDL_MOUSEWHEEL,             /**< Mouse wheel motion */

		//https://www.youtube.com/watch?v=jzasDqPmtPI&list=PLhfAbcv9cehhkG7ZQK0nfIGJC_C-wSLrx&index=5
		//https://gigi.nullneuron.net/gigilabs/sdl2-drag-and-drop/
		case SDL_MOUSEBUTTONDOWN:
		{

			if (!m_LMBD && event.button.button == SDL_BUTTON_LEFT)
			{
				m_LMBD = true;
				for (int i = 0; i < Board::m_iColumns ; i++)
				{
					if (Tile* pTile = m_board.GetTileAtPoint(&m_mousePosition))
					{
						if (SDL_Rect* pawnTransform = &pTile->GetTransform())
						{
							if (SDL_PointInRect(&m_mousePosition, pawnTransform))
							{
								if (Piece* pOccupant = pTile->GetPiece())
								{
									m_resetPos = pOccupant->GetTransform();
									m_pSelectedRect = &pOccupant->GetTransform();
									m_pPiecesTile = pTile;
									m_pSelectedPiece = pOccupant;
									m_pSelectedPiece->SetSelected(true);
								}
	
								break;
							}
						}
					}
				}
				m_board.GenerateLegalMoves(m_pSelectedPiece);
			}

			volatile int i = 5;
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			if (m_LMBD && event.button.button == SDL_BUTTON_LEFT)
			{
				m_LMBD = false;
				

				bool allowMove = false;
				bool isACapture = false;
				//Coordinate allowMoveLoc;
				Coordinate newCoord;
				Tile* pNewTile = nullptr;
				// If release position is in the valid tiles list - drop it off there
				for (int i = 0; i < Board::m_iColumns; i++)
				{
					if (Tile* pTile = m_board.GetTileAtPoint(&m_mousePosition))
					{
						if (SDL_Rect* pawnTransform = &pTile->GetTransform())
						{
							auto Predicate = [&pTile](Tile* pOtherTile)
							{
								return pTile == pOtherTile;
							};
							if (m_board.TileMatch(Predicate, 1))
							{
								if (Piece* pDefender = pTile->GetPiece())
								{
									isACapture = true;

									m_board.Test();

									pDefender->SetCaptured(true);
								}
								else
								{								
									// Need to consider EnPassant
									// If the piece could move to this square was a pawn
									// And the Tile BELOW it HAS a Pawn as well.
									// Need to terminate that tiles piece
									if (Piece* pSelectedPiece = m_pSelectedPiece)
									{
										if (pSelectedPiece->GetFlags() & (uint32_t)Piece::PieceFlag::Pawn)
										{
											Coordinate temp = pTile->GetCoordinate();
											Coordinate temp2 = pSelectedPiece->GetCoordinate();
											bool side = pSelectedPiece->IsSouthPlaying();
											int extra = side ? -1 : 1;
											if (temp.m_y == (temp2.m_y + extra) && temp.m_x != temp2.m_x)
											{
												if (side)
												{
													temp.m_y += 1;
												}
												else
												{
													temp.m_y -= 1;
												}
												int tileID = m_board.GetTileIDFromCoord(temp);
												if (Tile* pEnpassantTile = m_board.GetTile(tileID))
												{
													if (Piece* pEnpassantPawn = pEnpassantTile->GetPiece())
													{
														if (pEnpassantPawn->GetFlags() & (uint32_t)Piece::PieceFlag::Pawn)
														{
															// Terminate
															isACapture = true;
															pEnpassantTile->SetPiece(nullptr);
															pEnpassantPawn->SetCaptured(true);
														}
													}
												}
											}
										}										
									}
								}

								allowMove = true;
								// Need to apply piece offsets
								newCoord = pTile->GetCoordinate();
								pNewTile = pTile;
								//allowMoveLoc.m_x = pawnTransform->x;
								//allowMoveLoc.m_y = pawnTransform->y;
								break;
							}
						}
					}
				}

				if (allowMove)
				{
					if (Piece* pSelectedPiece = m_pSelectedPiece)
					{
						// Need to tell previous tile that we're gone
						// And New tile that we have arrived
						if (Tile* pPrevTile = m_pPiecesTile)
						{
							pPrevTile->SetPiece(nullptr);
						}
						pSelectedPiece->SetCoord(newCoord);
						pSelectedPiece->UpdatePosFromCoord();
						if (Tile* pDestinationTile = pNewTile)
						{
							pDestinationTile->SetPiece(pSelectedPiece);
						}
						//pSelectedPiece->SetPos(0, 0);// allowMoveLoc.m_x, allowMoveLoc.m_y);
					}
					//if (m_pSelectedRect)
					//{
					//	m_pSelectedRect->x = allowMoveLoc.m_x;
					//	m_pSelectedRect->y = allowMoveLoc.m_y;
					//
					//}
				}
				else
				{
					if (m_pSelectedRect)
					{
						m_pSelectedRect->x = m_resetPos.x;
						m_pSelectedRect->y = m_resetPos.y;
					}
				}

				
				m_board.ClearLegalMoves();

				m_pSelectedRect = nullptr;
				
				if (Piece* pSelected = m_pSelectedPiece)
				{
					pSelected->SetSelected(false);
					pSelected = nullptr;
				}
				if (Tile* pPiecesTile = m_pPiecesTile)
				{
					pPiecesTile = nullptr;
				}

			}
			break;
		}
		case SDL_MOUSEMOTION:
		{
			m_mousePosition = { event.motion.x, event.motion.y };

			// TODO: Be nice to change this to an actual texture highlight - white boarder or so.
			if (Tile* pTile = m_board.GetTileAtPoint(&m_mousePosition))
			{
				auto Predicate = [&pTile](Tile* pOtherTile)
				{
					return pTile == pOtherTile;
				};
				if (m_board.TileMatch(Predicate, 0))
				{
					return;
				}
				NMSprite& sprite = pTile->GetSprite();
				if (SDL_Texture* pTexture = sprite.GetTexture())
				{
					if (m_pHighlightedTex && m_pHighlightedTex != pTexture)
					{
						SDL_SetTextureColorMod(m_pHighlightedTex, 255, 255, 255);					
					}
					m_pHighlightedTex = pTexture;				
				}
			}
			else
			{

			}

			if (m_pHighlightedTex)
			{
				SDL_SetTextureColorMod(m_pHighlightedTex, 178, 178, 178);
			}

			break;
		}
		case SDL_QUIT:
		{
			m_bIsRunning = false;
			break;
		}
		default:
			break;
	}
}

void GameLoop::Process(float dt)
{
	m_iFrameCount++;

	if (SDL_Rect* pRect = m_pSelectedRect)
	{
		pRect->x = m_mousePosition.x - (pRect->w / 2);
		pRect->y = m_mousePosition.y - (pRect->h / 2);
	}
}

void GameLoop::InitGame()
{
	m_board.Init(m_pRenderer);
}

// https://opengameart.org/content/chess-pieces-and-board-squares
void GameLoop::Render()
{
	SDL_RenderClear(m_pRenderer);

	m_board.Render(m_pRenderer);

	if (Piece* pSelected = m_pSelectedPiece)
	{
		pSelected->RenderAsSelected(m_pRenderer);
	}

	// Show Legal Moves
	//m_board.RenderLegalMoves(m_pRenderer);

	//for (int i = 0; i < MAX_NUM_PLAYERS; i++)
	//{
	//	if (m_players[i])
	//	{
	//		m_players[i]->Render(m_pRenderer);
	//	}
	//}

	SDL_RenderPresent(m_pRenderer);
}
